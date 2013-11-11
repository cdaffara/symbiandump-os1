/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Subsession handles are stored here for later handling
*
*/



// INCLUDE FILES
#include "AccSrvNotificationQueue.h"
#include "acc_debug.h"
#include "AccSrvSubAudioControl.h" //for callbacks

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::CAccSrvNotificationQueue
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvNotificationQueue::CAccSrvNotificationQueue()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CAccSrvNotificationQueue()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::ConstructL()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvNotificationQueue* CAccSrvNotificationQueue::NewL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::NewL()" );
    CAccSrvNotificationQueue* self = new( ELeave ) CAccSrvNotificationQueue;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// Destructor
CAccSrvNotificationQueue::~CAccSrvNotificationQueue()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::~CAccSrvNotificationQueue()" );

    iConnectAccessory.Close();
    iAccessoryAudioLinkOpenArray.Close();
    iAccessoryAudioLinkOpenedArray.Close();
    iAccessoryAudioLinkClosedArray.Close();
    iAccessoryAudioLinkCloseArray.Close();
    iBluetoothAudioLinkOpenReqArray.Close();
    iBluetoothAudioLinkCloseReqArray.Close();
    iBluetoothAudioLinkOpenRespArray.Close();
    iBluetoothAudioLinkCloseRespArray.Close();
    iNewAccessoryConnectedArray.Close();
    iAccessoryConnectionStatusChangedArray.Close();
    iGetAccessoryConnectionStatusAsyncArray.Close();
    iAccessoryDisconnectedArray.Close();
    iAccessoryModeChangedArray.Close();
    iGetAccessoryModeAsyncArray.Close();
    iNotifyProcessCommandArray.Close();
    iNotifyAccessoryValueChangedTBoolArray.Close();
    iNotifyAccessoryValueChangedTIntArray.Close();
    iNotifyAccessoryValueChangedObjectArray.Close();
    iSetValueArray.Close();
    iGetValueArray.Close();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::~CAccSrvNotificationQueue - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::ControlMessageArray(
// -----------------------------------------------------------------------------
//
RArray<MQueueObserver*>* CAccSrvNotificationQueue::ControlMessageArray( TMsgID aMsgID )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvNotificationQueue::ControlMessageArray(0x%x)", aMsgID );

    switch ( aMsgID )
        {
        case EConnectAccessory:
            {
            return &iConnectAccessory;
            }
        case ENewAccessoryConnected:
            {
            return &iNewAccessoryConnectedArray;
            }
        case EAccessoryDisconnected:
            {
            return &iAccessoryDisconnectedArray;
            }
        case EAccessoryModeChanged:
            {
            return &iAccessoryModeChangedArray;
            }
        case EAccessoryConnectionStatusAsyncChanged:
            {
            return &iGetAccessoryConnectionStatusAsyncArray;
            }
        case EAccessoryModeAsyncChanged:
            {
            return &iGetAccessoryModeAsyncArray;
            }
        case EAccessoryConnectionStatusChanged:
            {
            return &iAccessoryConnectionStatusChangedArray;
            }
        case EAccessoryValueChangedTBool:
            {
            return &iNotifyAccessoryValueChangedTBoolArray;
            }
        case EAccessoryValueChangedTInt:
            {
            return &iNotifyAccessoryValueChangedTIntArray;
            }
        case EAccessoryValueChangedObject:
            {
            return &iNotifyAccessoryValueChangedObjectArray;
            }
        case EAccessoryAudioLinkOpen:
            {
            return &iAccessoryAudioLinkOpenArray;
            }
        case EAccessoryAudioLinkOpened:
            {
            return &iAccessoryAudioLinkOpenedArray;
            }
        case EAccessoryAudioLinkClosed:
            {
            return &iAccessoryAudioLinkClosedArray;
            }
        case EAccessoryAudioLinkClose:
            {
            return &iAccessoryAudioLinkCloseArray;
            }
        case EBluetoothAudioLinkOpenReq:
            {
            return &iBluetoothAudioLinkOpenReqArray;
            }
        case EBluetoothAudioLinkCloseReq:
            {
            return &iBluetoothAudioLinkCloseReqArray;
            }
        case EBluetoothAudioLinkOpenResp:
            {
            return &iBluetoothAudioLinkOpenRespArray;
            }
        case EBluetoothAudioLinkCloseResp:
            {
            return &iBluetoothAudioLinkCloseRespArray;
            }
        case ENotifyProcessCommand:
            {
            return &iNotifyProcessCommandArray;
            }
        case ESetValue:
            {
            return &iSetValueArray;
            }
        case EGetValue:
            {
            return &iGetValueArray;
            }
        default:
            {
            // Unknown notification type
            PANIC_ALWAYS;
            }
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::ControlMessageArray - return NULL" );

    return NULL;
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::RegisterControlMessage
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::RegisterControlMessage( TMsgID aMsgID,
                                                       MQueueObserver* aCallback )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvNotificationQueue::RegisterControlMessage(%d)", aMsgID );

    ControlMessageArray( aMsgID )->Append( aCallback );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::RegisterControlMessage - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::CancelControlMessage
//
// Find the correct array and remove corresponding element (aCallback) from it.
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::CancelControlMessage( TMsgID aMsgID,
                                                     MQueueObserver* aCallback )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvNotificationQueue::CancelNotificationRequest(%d)", aMsgID );

    TInt index( ControlMessageArray( aMsgID )->Find( aCallback ) );

    if ( KErrNotFound != index )
        {
        ControlMessageArray( aMsgID )->Remove( index );
        }
    
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvNotificationQueue::CancelNotificationRequest - %d return void", index );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::CompleteControlMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::CompleteControlMessageL( TMsgID aMsgID,
                                                        TInt aErrorCode,
                                                        TInt aUniqueID )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteControlMessageL(%d)", aMsgID );

    for ( TInt i( ControlMessageArray( aMsgID )->Count()-1 ); i >= 0; --i )
        {
        if ( ControlMessageArray( aMsgID )->operator[]( i )->CompleteMessageL( 
                                                                aMsgID, 
                                                                aErrorCode, 
                                                                aUniqueID ) )
            {
            ControlMessageArray( aMsgID )->Remove( i );
            }
        else
            {
            // Message was not consumed by subsession in question,
            // leave it to queue
            }
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteControlMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::CompleteASYCmdL
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::CompleteASYCmdL( TMsgID aMsgID,
                                                TProcessCmdId aCmdn,
                                                TInt aTrId,
                                                TASYCommandParamRecord& aASYCommandParamRecord,
                                                TPtr8* aPtrBuf )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteASYCmdL()" );

    for ( TInt i( ControlMessageArray( aMsgID )->Count()-1 ); i >= 0; --i )
        {
        if ( ControlMessageArray( aMsgID )->operator[]( i )->CompleteProcessCommandL( 
                                                                aCmdn, 
                                                                aTrId, 
                                                                aASYCommandParamRecord,
                                                                aPtrBuf ) )
            {
            ControlMessageArray( aMsgID )->Remove( i );
            }
        else
            {
            // Message was not consumed by subsession in question,
            // leave it to queue
            }
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteASYCmdL return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::CompleteValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::CompleteValueMessageL( TMsgID aMsgID,
                                                      TAccValueTypeTBool aCmdResponse,
                                                      TInt aErrorCode,
                                                      TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteValueMessageL() - TBool" );

    for ( TInt i( ControlMessageArray( aMsgID )->Count()-1 ); i >= 0; --i )
        {
        ControlMessageArray( aMsgID )->operator[]( i )->HandleValueMessageL( 
                                                            aMsgID, 
                                                            aCmdResponse, 
                                                            aErrorCode, 
                                                            aTrId );
        ControlMessageArray( aMsgID )->Remove( i );
        }


    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteValueMessageL - TBool - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::CompleteValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::CompleteValueMessageL( TMsgID aMsgID,
                                                      TAccValueTypeTInt aCmdResponse,
                                                      TInt aErrorCode,
                                                      TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteValueMessageL() - TInt" );

    for ( TInt i( ControlMessageArray( aMsgID )->Count()-1 ); i >= 0; --i )
        {
        ControlMessageArray( aMsgID )->operator[]( i )->HandleValueMessageL( 
                                                            aMsgID, 
                                                            aCmdResponse,
                                                            aErrorCode, 
                                                            aTrId );
        ControlMessageArray( aMsgID )->Remove( i );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteValueMessageL - TInt- return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::CompleteValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::CompleteValueMessageL( TMsgID aMsgID,
                                                      TDesC8& aCmdResponse,
                                                      TInt aErrorCode,
                                                      TInt aTrId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteValueMessageL() - TDesC8" );

    for ( TInt i( ControlMessageArray( aMsgID )->Count()-1 ); i >= 0; --i )
        {
        ControlMessageArray( aMsgID )->operator[]( i )->HandleValueMessageL( 
                                                            aMsgID, 
                                                            aCmdResponse, 
                                                            aErrorCode, 
                                                            aTrId );
        ControlMessageArray( aMsgID )->Remove( i );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteValueMessageL - TDesC8 - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::CompleteValueMessageL
// -----------------------------------------------------------------------------
//
void CAccSrvNotificationQueue::CompleteValueMessageL( TMsgID aMsgID,
                                                      TInt aTrId,
                                                      TPtr8* aPtrBuf,
                                                      TInt aErrorCode )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteValueMessageL() - Object" );    

    for ( TInt i( ControlMessageArray( aMsgID )->Count()-1 ); i >= 0; --i )
        {
        ControlMessageArray( aMsgID )->operator[]( i )->HandleValueMessageL( 
                                                            aMsgID, 
                                                            aTrId,
                                                            aPtrBuf, 
                                                            aErrorCode );
        ControlMessageArray( aMsgID )->Remove( i );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::CompleteValueMessageL - Object - return void" );
    }
    
// -----------------------------------------------------------------------------
// CAccSrvNotificationQueue::ControlMessagesWaiting
// -----------------------------------------------------------------------------
//
TInt CAccSrvNotificationQueue::ControlMessagesWaiting( TMsgID aMsgID )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvNotificationQueue::ControlMessagesWaiting()" );
    
    TInt ret( ControlMessageArray( aMsgID )->Count() );
    
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvNotificationQueue::ControlMessagesWaiting - return %d", ret );
    
    return ret;
    }
	
//  End of File

