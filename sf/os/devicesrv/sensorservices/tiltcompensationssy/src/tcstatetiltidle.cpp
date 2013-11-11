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
* Description:  CTCStateTiltIdle class implementation.
*
*/


#include "tcstatetiltidle.h"
#include "tctransactionhandler.h"
#include "tcstatepool.h"
#include "tcutil.h"
#include "trace.h"

CTCStateTiltIdle* CTCStateTiltIdle::NewL( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltIdle* self = CTCStateTiltIdle::NewLC(
        aStatePool,
        aTransactionHandler );
    CleanupStack::Pop( self );
    
    return self;
    }

CTCStateTiltIdle* CTCStateTiltIdle::NewLC( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltIdle* self = new( ELeave ) CTCStateTiltIdle(
        aStatePool,
        aTransactionHandler );
    CleanupStack::PushL( self );
    
    return self;
    }

CTCStateTiltIdle::CTCStateTiltIdle( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler ):
    CTCStateTilt( CTCState::ETCStateIdle, aStatePool, aTransactionHandler )
    {
    FUNC_LOG;
    }
    
// METHODS

// ----------------------------------------------------------------------------------
// CTCStateTiltIdle::HandleTransactionL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltIdle::HandleEventL( TTCEventId aId, TTCEvent* /*aEvent */)
    {
    FUNC_LOG;
    
    INFO_2( "Handling event [%S] in state [%S]",
        &TCUtil::EventIdAsDesC( aId ),
        &TCUtil::StateIdAsDesC( this ) );
    
    // Idle state only handles EEventIdHandleOpenChannel event.
    switch( aId )
        {
        case EEventIdHandleOpenChannel:
            {
            iTransactionHandler.ProcessTransactionL( ETCTransactionIdOpenChannel );
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
// CTCStateTiltIdle::HandleTransactionCompletedL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltIdle::HandleTransactionCompletedL( TTCTransactionId aTransactionId,
    TInt /*aError */)
    {
    FUNC_LOG;
    
    INFO_2( "Transaction [%S] complete message recieved in [%S] state",
        &TCUtil::TransactionIdAsDesC( aTransactionId ),
        &TCUtil::StateIdAsDesC( this ) );
    
    // When ETCTransactionIdOpenChannel is completed, change top OPEN state
    switch( aTransactionId )
        {
        case ETCTransactionIdOpenChannel:
            {
            iStatePool.ChangeStateL( CTCState::ETCStateOpen );
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
// CTCStateTiltIdle::HandleStateEntryL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltIdle::HandleStateEntryL()
    {
    FUNC_LOG;
    
    // Nothing to do
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltIdle::HandleStateExitL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltIdle::HandleStateExitL()
    {
    FUNC_LOG;

    // Nothing to do
    }

// End of File
