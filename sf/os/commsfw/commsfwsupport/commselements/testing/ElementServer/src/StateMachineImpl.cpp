// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This contains the implementation of a StateMachine for testing
// purpose.
// 
//

#include "StateMachineImpl.h"

/*
 * @file StateMachineImpl.cpp
 */

/*
 * Creates a new state machine object.
 * 
 * @return Pointer to a state machine implementation.
 */
CStateMachineImpl* CStateMachineImpl::NewL()
    {
    CStateMachineImpl* stateMachine = new (ELeave) CStateMachineImpl();
    CleanupStack::PushL(stateMachine);
    return stateMachine;
    }

/*
 * Sets the state machines fragment member to the given HBuf.
 * 
 * @param HBufC8* The fragment that the member should be set to.
 */
void CStateMachineImpl::SetFragment(HBufC8* aFragment)
    {
    iFragment = aFragment;
    }

/*
 * Returns the member variable to test the setting function.
 * 
 * @return The member variable value.
 */
MStateMachineNotify* CStateMachineImpl::StateMachineNotifier()
    {
    return iStateMachineNotify;
    }

/*
 * Overriden function so we can stop the active scheduler.
 */
void CStateMachineImpl::OnCompletion()
    {
    CActiveScheduler::Stop();
    CStateMachine::OnCompletion();
    }

/*
 * Creates an active scheduler that a newly created state machine will be added too.
 * 
 * @return An active scheduler used for testing.
 */
CActiveScheduler* CStateMachineImpl::StartActiveSchedulerL()
    {
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(activeScheduler);
    return activeScheduler;
    }

/*
 * The processing method called from within a state machine's RunL. Processes
 * state changes.
 * 
 * @param TRequestStatus The state machines internal status.
 * @return CAsyncEvent* A pointer to the next state, or NULL to complete.
 */
CAsynchEvent* CAsyncEventImpl::ProcessL(TRequestStatus& aStatus)
    {
    iRuns++;
    iTestStep->LogStateChange(iAsyncEvent);
    if (iAsyncEvent == EPanicAsyncEvent)
        User::Leave(KElementStateMachineError);
    if (iAsyncEvent == EForcedErrorAsyncEvent)
        {
            iStateMachine->SetLastError(iError);
        if (iRuns == 2)
            iStateMachine->SetErrorEvent(NULL);
        }
    if (iAsyncEvent == ESuspendAsyncEvent)
        {
        iStateMachine->SetSuspendRequest(ETrue);
        }
    TRequestStatus* p = &aStatus;
    User::RequestComplete(p, iError); //move to the next state
    if (iAsyncEvent == ECancelAsyncEvent)
        {
        iStateMachine->Cancel(iError);
        }
    return Next();
    }
