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
* Description:  CTCStateTiltOpen class implementation.
*
*/


#include "tcstatetiltopen.h"
#include "tctransactionhandler.h"
#include "tcstatepool.h"
#include "tctransactionflushbuffer.h"
#include "tcutil.h"
#include "trace.h"

CTCStateTiltOpen* CTCStateTiltOpen::NewL( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltOpen* self = CTCStateTiltOpen::NewLC(
        aStatePool,
        aTransactionHandler );
    CleanupStack::Pop( self );
    
    return self;
    }

CTCStateTiltOpen* CTCStateTiltOpen::NewLC( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltOpen* self = new( ELeave ) CTCStateTiltOpen(
        aStatePool,
        aTransactionHandler );
    CleanupStack::PushL( self );
    
    return self;
    }

CTCStateTiltOpen::CTCStateTiltOpen( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler ):
    CTCStateTilt( CTCState::ETCStateOpen, aStatePool, aTransactionHandler )
    {
    FUNC_LOG;
    }

// METHODS

// ----------------------------------------------------------------------------------
// CTCStateTiltOpen::HandleTransactionL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltOpen::HandleEventL( TTCEventId aId, TTCEvent* /*aEvent */)
    {
    FUNC_LOG;
    
    INFO_2( "Handling event [%S] in state [%S]",
        &TCUtil::EventIdAsDesC( aId ),
        &TCUtil::StateIdAsDesC( this ) );
    
    switch( aId )
        {
        case EEventIdHandleCloseChannel:
            {
            // Close the tilt channel
            iTransactionHandler.ProcessTransactionL( ETCTransactionIdCloseChannel );
            break;
            }
        case EEventIdHandleStartChannelData:
            {
            // Start accelerometer and magnetometer channels
            iTransactionHandler.ProcessTransactionL(
                ETCTransactionIdStartChannelData );
            break;
            }
        default:
            {
            INFO_2( "Event [%S] not handled in [%S] state",
                &TCUtil::EventIdAsDesC( aId ),
                &TCUtil::StateIdAsDesC( this ) );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltOpen::HandleTransactionCompletedL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltOpen::HandleTransactionCompletedL( TTCTransactionId aTransactionId,
    TInt /*aError */)
    {
    FUNC_LOG;
    
    INFO_2( "Transaction [%S] complete message recieved in [%S] state",
        &TCUtil::TransactionIdAsDesC( aTransactionId ),
        &TCUtil::StateIdAsDesC( this ) );
    
    switch( aTransactionId )
        {
        case ETCTransactionIdCloseChannel:
            {
            // Switch back to idle state
            iStatePool.ChangeStateL( CTCState::ETCStateIdle );
            break;
            }
        case ETCTransactionIdStartChannelData:
            {
            // Switch to listen data state
            iStatePool.ChangeStateL( CTCState::ETCStateListenData );
            break;
            }
        default:
            {
            INFO_2( "Transaction [%S] complete message not handled in [%S] state",
                &TCUtil::TransactionIdAsDesC( aTransactionId ),
                &TCUtil::StateIdAsDesC( this ) );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltOpen::HandleStateEntryL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltOpen::HandleStateEntryL()
    {
    FUNC_LOG;
    
    // Nothing to do
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltOpen::HandleStateExitL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltOpen::HandleStateExitL()
    {
    FUNC_LOG;
    
    // Nothing to do
    }

// End of File
