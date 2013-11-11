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
* Description:  CTCStateTiltClosing class implementation.
*
*/


#include "tcstatetiltclosing.h"
#include "tctransaction.h"
#include "tcstatepool.h"
#include "tctransactionhandler.h"
#include "tcutil.h"
#include "trace.h"

CTCStateTiltClosing* CTCStateTiltClosing::NewL( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltClosing* self = CTCStateTiltClosing::NewLC( aStatePool,
        aTransactionHandler );
    CleanupStack::Pop( self );
    
    return self;
    }

CTCStateTiltClosing* CTCStateTiltClosing::NewLC( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltClosing* self = new( ELeave ) CTCStateTiltClosing( aStatePool,
        aTransactionHandler );
    CleanupStack::PushL( self );
    
    return self;
    }

CTCStateTiltClosing::CTCStateTiltClosing( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler ):
    CTCStateTilt( CTCState::ETCStateClosing, aStatePool, aTransactionHandler )
    {
    FUNC_LOG;
    }

// METHODS

// ----------------------------------------------------------------------------------
// CTCStateTiltClosing::HandleTransactionL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltClosing::HandleEventL( TTCEventId aId, TTCEvent* /*aEvent */)
    {
    FUNC_LOG;
    
    INFO_2( "Handling event [%S] in state [%S]",
        &TCUtil::EventIdAsDesC( aId ),
        &TCUtil::StateIdAsDesC( this ) );
    
    switch( aId )
        {
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
// CTCStateTiltClosing::HandleTransactionCompletedL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltClosing::HandleTransactionCompletedL( TTCTransactionId aTransactionId,
    TInt /*aError */)
    {
    FUNC_LOG;

    INFO_2( "Transaction [%S] complete message recieved in [%S] state",
        &TCUtil::TransactionIdAsDesC( aTransactionId ),
        &TCUtil::StateIdAsDesC( this ) );

    // Switch back to data listening state when ETCTransactionIdCloseChannel
    // transaction has been completed
    switch( aTransactionId )
        {
        case ETCTransactionIdCloseChannel:
            {
            // Channel closed, switch to idle state
            iStatePool.ChangeStateL( CTCState::ETCStateIdle );
            break;
            }
        case ETCTransactionIdStopChannelData:
            {
            // Channel data stopped, close the channel
            iTransactionHandler.ProcessTransactionL( ETCTransactionIdCloseChannel );
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
// CTCStateTiltClosing::HandleStateEntryL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltClosing::HandleStateEntryL()
    {
    FUNC_LOG;
    
    // Nothing to do
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltClosing::HandleStateExitL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltClosing::HandleStateExitL()
    {
    FUNC_LOG;
    
    // Nothing to do
    }

// End of File
