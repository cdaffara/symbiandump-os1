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
* Description:  CTCStateTiltBufferFull class implementation.
*
*/


#include "tcstatetiltbufferfull.h"
#include "tctransaction.h"
#include "tcstatepool.h"
#include "tctransactionhandler.h"
#include "tcutil.h"
#include "trace.h"

CTCStateTiltBufferFull* CTCStateTiltBufferFull::NewL( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltBufferFull* self = CTCStateTiltBufferFull::NewLC(
        aStatePool,
        aTransactionHandler );
    CleanupStack::Pop( self );
    
    return self;
    }

CTCStateTiltBufferFull* CTCStateTiltBufferFull::NewLC( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler )
    {
    FUNC_LOG;
    
    CTCStateTiltBufferFull* self = new( ELeave ) CTCStateTiltBufferFull(
        aStatePool,
        aTransactionHandler );
    CleanupStack::PushL( self );
    
    return self;
    }

CTCStateTiltBufferFull::CTCStateTiltBufferFull( MTCStatePool& aStatePool,
    MTCTransactionHandler& aTransactionHandler ):
    CTCStateTilt( CTCState::ETCStateBufferFull, aStatePool, aTransactionHandler )
    {
    FUNC_LOG;
    }

// METHODS

// ----------------------------------------------------------------------------------
// CTCStateTiltBufferFull::HandleEventL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltBufferFull::HandleEventL( TTCEventId aId, TTCEvent* /*aEvent */)
    {
    FUNC_LOG;
    
    INFO_2( "Handling event [%S] in state [%S]",
        &TCUtil::EventIdAsDesC( aId ),
        &TCUtil::StateIdAsDesC( this ) );
    
    switch( aId )
        {
        case EEventIdHandleCloseChannel:
            {
            // Stop channel data and switch to closing state
            iTransactionHandler.ProcessTransactionL( ETCTransactionIdStopChannelData );
            iStatePool.ChangeStateL( CTCState::ETCStateClosing );
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
// CTCStateTiltBufferFull::HandleTransactionCompletedL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltBufferFull::HandleTransactionCompletedL( TTCTransactionId aTransactionId,
    TInt /*aError */)
    {
    FUNC_LOG;

    INFO_2( "Transaction [%S] complete message recieved in [%S] state",
        &TCUtil::TransactionIdAsDesC( aTransactionId ),
        &TCUtil::StateIdAsDesC( this ) );

    // Switch back to data listening state when ETCTransactionIdFlushBuffer
    // transaction has been completed
    switch( aTransactionId )
        {
        case ETCTransactionIdFlushBuffer:
            {
            INFO( "ETCTransactionIdFlushBuffer handled in BUFFER FULL state" );
            
            // Switch back to listen data state
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
// CTCStateTiltBufferFull::HandleStateEntryL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltBufferFull::HandleStateEntryL()
    {
    FUNC_LOG;
    
    // Nothing to do
    }

// ----------------------------------------------------------------------------------
// CTCStateTiltBufferFull::HandleStateExitL
// ----------------------------------------------------------------------------------
//
void CTCStateTiltBufferFull::HandleStateExitL()
    {
    FUNC_LOG;
    
    // Nothing to do
    }

// End of File
