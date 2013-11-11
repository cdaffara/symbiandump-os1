/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  TCUtil class implementation.
*
*/


#include "tcutil.h"
#include "tcstate.h"
#include "common.h"

// LOCAL DEFINITIONS

namespace TiltSSY
    {
    // CONSTANTS
    
    /** State types as descriptor */
    _LIT( KStateTypeTilt, "TILT" );

    /** States as descriptor */
    _LIT( KStateIdle, "IDLE" );
    _LIT( KStateOpen, "OPEN" );
    _LIT( KStateListenData, "LISTEN DATA" );
    _LIT( KStateBufferFull, "BUFFER FULL" );
    _LIT( KStateClosing, "CLOSING" );
    _LIT( KStateInvalid, "INVALID" );
    _LIT( KStateNull, "NULL" );

    /** Event as descriptor */
    _LIT( KEventIdHandleOpenChannel, "EEventIdHandleOpenChannel" );
    _LIT( KEventIdHandleCloseChannel, "EEventIdHandleCloseChannel" );
    _LIT( KEventIdHandleStartChannelData, "EEventIdHandleStartChannelData" );
    _LIT( KEventIdHandleStopChannelData, "EEventIdHandleStopChannelData" );
    _LIT( KEventIdHandleForceBufferFull, "EEventIdHandleForceBufferFull" );
    _LIT( KEventIdHandleAccelerometer, "EEventIdHandleForceBufferFull" );
    _LIT( KEventIdHandleMagnetometer, "EEventIdHandleMagnetometer" );
    _LIT( KEventIdHandlePropertyChanged, "EEventIdHandlePropertyChanged" );
    _LIT( KEventIdHandleTimerExpired, "EEventIdHandleTimerExpired" );
    _LIT( KEventIdInvalid, "INVALID EVENT ID!" );

    /** Trasaction as descriptor */
    _LIT( KTCTransactionIdOpenChannel, "ETCTransactionIdOpenChannel" );
    _LIT( KTCTransactionIdCloseChannel, "ETCTransactionIdCloseChannel" );
    _LIT( KTCTransactionIdStartChannelData, "ETCTransactionIdStartChannelData" );
    _LIT( KTCTransactionIdStopChannelData, "ETCTransactionIdStopChannelData" );
    _LIT( KTCTransactionIdFlushBuffer, "ETCTransactionIdFlushBuffer" );
    _LIT( KTCTransactionIdInvalid, "INVALID TRANSACTION ID!" );
    }

// Converts state type as descriptor
const TDesC& TCUtil::StateTypeAsDesC( CTCState* aState )
    {
    if( aState )
        {
        switch( aState->Type() )
            {
            case CTCState::ETCStateTypeTilt:
                {
                return TiltSSY::KStateTypeTilt; 
                }
            default:
                {
                return TiltSSY::KStateInvalid;
                }
            }
        }
    return TiltSSY::KStateNull;
    }

// Converts state ID to descriptor
const TDesC& TCUtil::StateIdAsDesC( CTCState* aState )
    {
    if( aState )
        {
        switch( aState->Id() )
            {
            case CTCState::ETCStateIdle:
                {
                return TiltSSY::KStateIdle; 
                }
            case CTCState::ETCStateOpen:
                {
                return TiltSSY::KStateOpen;
                }
            case CTCState::ETCStateListenData:
                {
                return TiltSSY::KStateListenData;
                }
            case CTCState::ETCStateBufferFull:
                {
                return TiltSSY::KStateBufferFull;
                }
            case CTCState::ETCStateClosing:
                {
                return TiltSSY::KStateClosing;
                }
            default:
                {
                return TiltSSY::KStateInvalid;
                }
            }
        }
        
    return TiltSSY::KStateNull;
    }
    
// Converts event ID to descriptor
const TDesC& TCUtil::EventIdAsDesC( TTCEventId aId )
    {
    switch( aId )
        {
        case EEventIdHandleOpenChannel:
            {
            return TiltSSY::KEventIdHandleOpenChannel;
            }
        case EEventIdHandleCloseChannel:
            {
            return TiltSSY::KEventIdHandleCloseChannel;
            }
        case EEventIdHandleStartChannelData:
            {
            return TiltSSY::KEventIdHandleStartChannelData;
            }
        case EEventIdHandleStopChannelData:
            {
            return TiltSSY::KEventIdHandleStopChannelData;
            }
        case EEventIdHandleForceBufferFull:
            {
            return TiltSSY::KEventIdHandleForceBufferFull;
            }
        case EEventIdHandleAccelerometer:
            {
            return TiltSSY::KEventIdHandleAccelerometer;
            }
        case EEventIdHandleMagnetometer:
            {
            return TiltSSY::KEventIdHandleMagnetometer;
            }
        case EEventIdHandlePropertyChanged:
            {
            return TiltSSY::KEventIdHandlePropertyChanged;
            }
        case EEventIdHandleTimerExpriredSend:
            {
            return TiltSSY::KEventIdHandleTimerExpired;
            }
        case EEventIdNull:
        // Fall through
        default:
            {
            return TiltSSY::KEventIdInvalid;
            }
        }
        
    }
    
// Converts transaction ID to descriptor
const TDesC& TCUtil::TransactionIdAsDesC( TTCTransactionId aId )
    {
    switch( aId )
        {
        case ETCTransactionIdOpenChannel:
            {
            return TiltSSY::KTCTransactionIdOpenChannel;
            }
        case ETCTransactionIdCloseChannel:
            {
            return TiltSSY::KTCTransactionIdCloseChannel;
            }
        case ETCTransactionIdStartChannelData:
            {
            return TiltSSY::KTCTransactionIdStartChannelData;
            }
        case ETCTransactionIdStopChannelData:
            {
            return TiltSSY::KTCTransactionIdStopChannelData;
            }
        case ETCTransactionIdFlushBuffer:
            {
            return TiltSSY::KTCTransactionIdFlushBuffer;
            }
        case ETCTransactionIdNull:
        // Fall through
        default:
            {
            return TiltSSY::KTCTransactionIdInvalid;
            }
        }
    }
    
// End of File
