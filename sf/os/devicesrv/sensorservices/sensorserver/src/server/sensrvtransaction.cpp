/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Sensor server transaction base class
*
*/


#include "sensrvdefines.h"
#include "sensrvclientserver.h"
#include "sensrvtransaction.h"
#include "sensrvpluginproxy.h"
#include "sensrvproxymanager.h"
#include "sensrvsession.h"
#include "senserverchannel.h"

// ---------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------
//
CSensrvTransaction* CSensrvTransaction::NewL(CSensrvMessage* aMessage, 
                                             CSensrvPluginProxy* aProxy, 
                                             CSensrvChannel* aChannel,
                                             TSensrvTransactionType aType)
    {
    CSensrvTransaction* self = NewLC(aMessage,
                                     aProxy,
                                     aChannel,
                                     aType);
    
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------
//
CSensrvTransaction* CSensrvTransaction::NewLC(CSensrvMessage* aMessage, 
                                             CSensrvPluginProxy* aProxy, 
                                             CSensrvChannel* aChannel,
                                             TSensrvTransactionType aType)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::NewLC(aMessage: 0x%x, <proxymanager>, aProxy: 0x%x, aChannel.Id(): %d, aType: %d )" ), 
                       aMessage, 
                       aProxy ? aProxy->ImplementationUid().iUid : 0, 
                       aChannel ? aChannel->Id() : 0, 
                       aType ) );

    CSensrvTransaction* self = new CSensrvTransaction(aMessage,
                                                      aProxy,
                                                      aChannel,
                                                      aType);

    // Message needs to be completed if construction failed.    
    if (!self)
        {
        if (aMessage)
            {
            aMessage->Complete(KErrNoMemory);
            }
        User::Leave(KErrNoMemory);
        }

    CleanupStack::PushL( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::NewLC - return 0x%x" ), self ) );
    
    return self;
    }

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CSensrvTransaction::CSensrvTransaction(CSensrvMessage* aMessage, 
                                       CSensrvPluginProxy* aProxy, 
                                       CSensrvChannel* aChannel,
                                       TSensrvTransactionType aType)
        : iMessage(aMessage), 
          iProxy(aProxy),
          iChannel(aChannel),
          iType(aType),
          iState(ETransStateUninitialized),
          iErrorCode(KErrNone)
    {
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CSensrvTransaction::~CSensrvTransaction()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::~CSensrvTransaction()" ) ) );

    Complete();
    
    // Not owned components: iProxy, iProxyManager, iChannel not deleted

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::~CSensrvTransaction - return" ) ) );
    }


// ---------------------------------------------------------
// Completes the related message thus ending transaction.
// Message is completed with the iErrorCode value.
// ---------------------------------------------------------
//
void CSensrvTransaction::Complete()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::Complete()" ) ) );

    iState = ETransStateCompleted;

    if (iMessage)
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::Complete - Completing with code: %d" ), iErrorCode ) );
        iMessage->Complete(iErrorCode);
        iMessage = NULL;
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::Complete - return" ) ) );
    }

// ---------------------------------------------------------
// Sets transaction state. 
// ---------------------------------------------------------
//
void CSensrvTransaction::SetState(TSensrvTransactionState aState)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetState(aState: %d)" ), aState ) );

    // In debug mode, panic on invalid transitions to detect problems    
    __ASSERT_DEBUG(
        IsMediatorType() 
        || ((iState == ETransStateUninitialized && aState == ETransStateQueued )
                || (iState == ETransStateQueued && (aState == ETransStateExecuting || aState == ETransStateCompleted) )
                || (iState == ETransStateExecuting && (aState == ETransStateHandledAtSsy || aState == ETransStateCompleted || aState == ETransStateNotifyFailed) )
                || (iState == ETransStateHandledAtSsy && aState == ETransStateCompleted || aState == ETransStateNotifyFailed)
                || (iState == ETransStateNotifyFailed && (aState == ETransStateHandledAtSsy || aState == ETransStateCompleted)) 
                || (iState == ETransStateCompleted)),
        User::Panic(KSensrvPanicCategory, ESensrvPanicInvalidTransactionState));
                
    iState = aState;
    }

// ---------------------------------------------------------
// Sets data to be returned to client. 
// Transaction type determines how aData is interpreted.
// ---------------------------------------------------------
//
TInt CSensrvTransaction::SetMessageData(TAny* aData)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData " ) ) );
    TInt err(KErrNone);

    switch(iType)
        {
        case ETransTypeClientQueryChannels:
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ETransTypeClientQueryChannels" ) ) );

            // Cast list from parameter
            RSensrvChannelInfoList* list = static_cast<RSensrvChannelInfoList*>(aData);
            TInt count(0);
            if( list )
                {
                count = list->Count();
                COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - (aData: 0x%x)" ), aData ) );
                }
            // Dump list channel list and count to message if there is space
            if (iMessage)
                {
                TSensrvTIntPckgBuf pckg(count);
                
                TInt space = iMessage->GetDesMaxLength( KSensrvQueryChannelsCountSlot );
                
                if ( space >= pckg.Length() )
                    {
                    err = iMessage->Write( KSensrvQueryChannelsCountSlot, pckg );
                    
                    if (err != KErrNone)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: Write to KSensrvQueryChannelsCountSlot failed: %d" ), err ) );
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: Not enough space to write to KSensrvQueryChannelsCountSlot" ) ) );
                    // Do not use KErrOverflow here, as it is very unlikely
                    // and overflow already has specific meaning in this command.
                    err = KErrBadDescriptor; 
                    }
                
                if (err == KErrNone)
                    {
                    if (count > 0)
                        {
                        space = iMessage->GetDesMaxLength( KSensrvQueryChannelsChannelInfoArraySlot );
                        TInt arraySize = ( sizeof(TSensrvChannelInfo) ) * count;
                        
                        if ( space >= arraySize )
                            {
                            TPtr8 arrayPtr( reinterpret_cast<TUint8*>(&(*list)[0]), arraySize, arraySize );
                            err = iMessage->Write( KSensrvQueryChannelsChannelInfoArraySlot, arrayPtr );

                            if (err != KErrNone)
                                {
                                ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: Write to KSensrvQueryChannelsChannelInfoArraySlot failed: %d" ), err ) );
                                }
                            }
                        else    
                            {
                            // Not enough space to write. Note that this is not really an error. 
                            // Overflow will trigger requery with enough space allocated.
                            ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - Not enough space to write channel array. Expecting requery with more space. Completing with KErrOverflow (-9)" ) ) );
                            err = KErrOverflow;
                            }
                        }
                    }
                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: No message defined" ) ) );
                err = KErrBadHandle;
                }
            }
            break;

        case ETransTypeOpenChannel:
            {
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ETransTypeOpenChannel" ) ) );
            
            if ( aData && iMessage )
                {
                TChannelOpenMessageData* data = static_cast<TChannelOpenMessageData*>(aData);
                TSensrvTIntPckgBuf pckg(data->iMaxBufferingCount);
                
                TInt space = iMessage->GetDesMaxLength( KSensrvOpenChannelBufferSizeSlot );
                
                if ( space >= pckg.Length() )
                    {
                    err = iMessage->Write( KSensrvOpenChannelBufferSizeSlot, pckg );
                    
                    if (err != KErrNone)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: Write to KSensrvOpenChannelBufferSizeSlot failed: %d" ), err ) );
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: Not enough space to write to KSensrvOpenChannelBufferSizeSlot" ) ) );
                    err = KErrBadDescriptor; 
                    }
                
                TSensrvTIntPckgBuf pckg2(data->iDataItemSize);
                
                space = iMessage->GetDesMaxLength( KSensrvOpenChannelDataItemSizeSizeSlot );
                if ( space >= pckg2.Length() )
                    {
                    err = iMessage->Write( KSensrvOpenChannelDataItemSizeSizeSlot, pckg2 );
                    
                    if (err != KErrNone)
                        {
                        ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: Write to KSensrvOpenChannelDataItemSizeSizeSlot failed: %d" ), err ) );
                        }
                    }
                else
                    {
                    ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: Not enough space to write to KSensrvOpenChannelDataItemSizeSizeSlot" ) ) );
                    err = KErrBadDescriptor; 
                    }

                }
            else
                {
                ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: No message defined" ) ) );
                err = KErrBadHandle;
                }
            }
            break;

        // The other types of transactions have no return data, so use default:
        default:
            ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - ERROR: Invalid iType: %d, no data set" ), iType ) );
            break;                
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetMessageData - return %d" ), err ) );
    
    return err;
    }

// ---------------------------------------------------------
// Set error code of transaction.
// ---------------------------------------------------------
//
void CSensrvTransaction::SetErrorCode(TInt aErrorCode)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetErrorCode(aErrorCode: %d)" ), aErrorCode ) );

    iErrorCode = aErrorCode;    
    
#ifdef ERROR_TRACE_DEBUG
    if ( iErrorCode != KErrNone )
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetErrorCode - Message error code set: %d" ), aErrorCode ) );
        }
#endif 

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::SetErrorCode - return" ) ) );
    }

// ---------------------------------------------------------
// Extract message from transaction
// ---------------------------------------------------------
//
CSensrvMessage* CSensrvTransaction::ExtractMessage()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::ExtractMessage()" ) ) );

    CSensrvMessage* message = iMessage;
    
    iMessage = NULL;
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTransaction::ExtractMessage - return 0x%x" ), message ) );
   
    return message;
    }
