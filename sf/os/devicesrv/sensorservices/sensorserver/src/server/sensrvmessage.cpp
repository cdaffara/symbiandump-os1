/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor server message wrapper
*
*/


#include "sensrvdefines.h"
#include "sensrvtrace.h"
#include "sensrvmessage.h"
#include "sensrvsession.h"
#include "sensrvclientserver.h"

// -----------------------------------------------------------------------------
// Two-phased constructor for external message.
// -----------------------------------------------------------------------------
//
CSensrvMessage* CSensrvMessage::NewL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::NewL()" ) ) );

    CSensrvMessage* self = new( ELeave ) CSensrvMessage();
    
    // No ConstructL as nothing to do there

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CSensrvMessage::CSensrvMessage
// C++ constructor for external message
// -----------------------------------------------------------------------------
//
CSensrvMessage::CSensrvMessage()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::CSensrvMessage()" ) ) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::CSensrvMessage - return" )) );
    }


// ---------------------------------------------------------
// Destructor, completes message with error if not yet done so
// ---------------------------------------------------------
//
CSensrvMessage::~CSensrvMessage()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::~CSensrvMessage()" ) ) );

    // if message is not yet completed, complete with error. 
    // This should never happen if everything works properly
    if ( iMessage.Handle() )
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvMessage::~CSensrvMessage() - ERROR: External message not completed before destruction."  ) ) );
        iMessage.Complete(KErrGeneral);
        }

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::~CSensrvMessage - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Initializes the message
// -----------------------------------------------------------------------------
//
void CSensrvMessage::Initialize(const RMessage2& aMessage)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::Initialize()" ) ) );

    if (iMessage.Handle())
        {
        ERROR_TRACE( ( _L( "Sensor Server - CSensrvMessage::Initialize - ERROR: Previous message not completed, canceling it" ) ) );
        iMessage.Complete(KErrCancel);
        }
        
    iMessage = aMessage;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::Initialize - return") ) );
    }


// -----------------------------------------------------------------------------
// Completes the message
// -----------------------------------------------------------------------------
//
void CSensrvMessage::Complete(TInt aReason)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::Complete(%d)" ), aReason ) );

    iMessage.Complete(aReason);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::Complete - return" )) );
    }

// -----------------------------------------------------------------------------
// Returns the session handling this message
// -----------------------------------------------------------------------------
//
CSensrvSession* CSensrvMessage::Session()
    {
    CSensrvSession* session = NULL;

    session = static_cast<CSensrvSession*>(iMessage.Session());

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::Session - return 0x%x" ), session ) );
    
    return session;
    }

// -----------------------------------------------------------------------------
// Writes data to message
// -----------------------------------------------------------------------------
//
TInt CSensrvMessage::Write(TInt aParam, const TDesC8& aDes, TInt aOffset)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::WriteL(aParam: %d, <descriptor>, aOffset: %d)" ), aParam, aOffset ) );

#ifdef COMPONENT_TRACE_DEBUG
    TInt len = aDes.Length();
       const TUint8* currentByte(aDes.Ptr());
    HBufC* str = HBufC::New(len*6 + 20);
    if (str)
        {
        str->Des().Append(_L("### <descriptor>: 0x"));
        for (TInt j=0;j<len;j++)
            {
            str->Des().AppendNumUC(*currentByte, EHex);
            if ( j < (len-1) )
                {
                if ( (j%8) == 7 )
                    {
                    COMPONENT_TRACE((*str));
                    str->Des().Delete(0, 1000);
                    str->Des().Append(_L("###             : 0x"));
                    }
                else
                    {
                    str->Des().Append(_L(", 0x"));
                    }
                }        
                
            currentByte++;    
            }
        COMPONENT_TRACE((*str));
        delete str;
        str = NULL;
        }
#endif    

    TInt err(KErrNone);

    err = iMessage.Write(aParam, aDes, aOffset);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::WriteL - return %d" ), err ) );
    
    return err;
    }
    
// -----------------------------------------------------------------------------
// Gets maximum lenght of descriptor in specified index
// -----------------------------------------------------------------------------
//
TInt CSensrvMessage::GetDesMaxLength(TInt aParam) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::GetDesMaxLengthL(%d)" ), aParam ) );

    TInt length(0);

    length = iMessage.GetDesMaxLength(aParam);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::GetDesMaxLengthL - return: %d" ), length) );
    
    return length;
    }    
    
// -----------------------------------------------------------------------------
// Gets lenght of descriptor in specified index
// -----------------------------------------------------------------------------
//
TInt CSensrvMessage::GetDesLength(TInt aParam) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::GetDesLength(%d)" ), aParam ) );

    TInt lenght(0);

    lenght = iMessage.GetDesLength(aParam);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::GetDesLength - return: %d" ), lenght) );
    
    return lenght;
    }    

// -----------------------------------------------------------------------------
// Gets maximum lenght of descriptor in specified index
// -----------------------------------------------------------------------------
//
TSecureId CSensrvMessage::SecureId() const    
    {
    TUint32 sid(0);
    
    sid = iMessage.SecureId();
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::SecureId - return 0x%x" ), sid ) );    
    
    return TSecureId(sid);    
    }

// -----------------------------------------------------------------------------
// Read data from wrapped message. 
// -----------------------------------------------------------------------------
//     
TInt CSensrvMessage::Read( const TInt aParam, TDes8 &aDes ) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::ReadL(aParam: %d, <descriptor>)" ), aParam ) );
    
    TInt err(KErrNone);
    
    err = iMessage.Read( aParam, aDes );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::ReadL - return %d" ), err) );    
    
    return err;
    }

// -----------------------------------------------------------------------------
// Read integer data from wrapped message. 
// -----------------------------------------------------------------------------
//     
void CSensrvMessage::ReadInt( const TInt aParam, TInt &aInt ) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::ReadInt(aParam: %d, <target>)" ), aParam ) );
    
    switch(aParam)
        {
        case 0:
            aInt = iMessage.Int0();
            break;
        case 1:
            aInt = iMessage.Int1();
            break;
        case 2:
            aInt = iMessage.Int2();
            break;
        case 3:
            aInt = iMessage.Int3();
            break;
        default:
            User::Panic(KSensrvPanicCategory, ESensrvPanicOutOfBounds);
            break;               
        }
        
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::ReadInt - return, aInt: %d" ), aInt) );    
    }

// ---------------------------------------------------------------------------
// Gets channel id from message. 
// ---------------------------------------------------------------------------
//
TInt CSensrvMessage::GetChannelId(TSensrvChannelId& aChannelId) const    
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::GetChannelId(<retval>)" ) ) );
    
    TInt err(KErrNone);
    
    // Make sure message type supports channel id.
    TInt function = Function();
    
    if (   function == ESensrvSrvReqOpenChannel 
        || function == ESensrvSrvReqCloseChannel 
        || function == ESensrvSrvReqStartListening 
        || function == ESensrvSrvReqStopListening 
        || function == ESensrvSrvReqAsyncChannelData
        || function == ESensrvSrvReqGetProperty
        || function == ESensrvSrvReqSetProperty
        || function == ESensrvSrvReqAsyncPropertyData
        || function == ESensrvSrvReqStopPropertyListening
        || function == ESensrvSrvReqGetAllProperties 
        || function == ESensrvSrvReqAddConditionSet
        || function == ESensrvSrvReqRemoveConditionSet
        || function == ESensrvSrvReqStartConditionListening
        || function == ESensrvSrvReqStopConditionListening
        || function == ESensrvSrvReqAsyncConditionMet
       )
        {
        // Read required channel id from message
        // Channel id is always on message parameter KSensrvChannelIdSlot, i.e. slot zero.
        aChannelId = iMessage.Int0();
        }
    else
        {
        err = KErrNotSupported;
        }
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::GetChannelId - return %d, channel id: %d" ), err, aChannelId) );
    
    return err;
    }

// ---------------------------------------------------------------------------
// Gets client security info from message. 
// ---------------------------------------------------------------------------
//
void CSensrvMessage::GetSecurityInfo(TSecurityInfo& aSecurityInfo) const    
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::GetSecurityInfo(<retval>)" ) ) );
    
    // Read required channel id from message
    aSecurityInfo.Set(iMessage);
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvMessage::GetSecurityInfo - return" ) ) );
    }
    
