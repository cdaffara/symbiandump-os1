// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// StateMachineImpl.cpp
// This is the header file for StateMachine test implementation
// 
//

#ifndef STATEMACHINEIMPL_H_
#define STATEMACHINEIMPL_H_

#include "ElementServer.h"
#include <comms-infras/asynchevent.h>
#include <comms-infras/statemachine.h>

/*
 * @file StateMachineImpl.h
 */

const TInt KElementStateMachineError = -12056;

/*
 * Possible states the state machine can be in.
 */
enum TAsyncEventTest
    {
    EGreenAsyncEvent,
    EYellowAsyncEvent,
    ERedAsyncEvent,
    EErroringAsyncEvent,
    EErrorAsyncEvent,
    EForcedErrorAsyncEvent,
    EPanicAsyncEvent,
    ECancelAsyncEvent,
    ESuspendAsyncEvent
    };

/*
 * Interface to allow an AsyncEvent to pass information back to the test step.
 */
class MStateMachineStepLogger
    {
public:
    /*
     * Checks if the correct state has been taken by the state machine.
     * 
     * @param TAsyncEventTest The state that the machine is currently in.
     */
    virtual void LogStateChange(TAsyncEventTest aAsyncEvent) = 0;
    };

/*
 * Class that implements the notification interface.
 */
class CStateMachineNotifyImpl : public MStateMachineNotify
    {
public:
    /*
     * Runs completion code and determines if the machine should be deleted.
     * 
     * @param CStateMachine* The state machine that has completed.
     * @return Whether the machine should delete itself.
     */
    TBool OnCompletion( CStateMachine* aStateMachine )
        {
        if (aStateMachine->SuspendRequest())
            aStateMachine->Start(aStateMachine->ClientStatus(), NULL, this);
        return EFalse;
        }
    };

/*
 * Extended state machine object that provides getter and setter methods that are used by the
 * test steps.
 */
class CStateMachineImpl : public CStateMachine
    {
public:
    static CStateMachineImpl* NewL();
    MStateMachineNotify* StateMachineNotifier();
    void SetFragment(HBufC8* aFragment);
    void OnCompletion();
    static CActiveScheduler* StartActiveSchedulerL(); 
    };

/*
 * Implementation of a async event that determines the state of the machine.
 */
class CAsyncEventImpl : public CAsynchEvent
    {
public:
    /*
     * Constructor.
     */
    CAsyncEventImpl(TAsyncEventTest aAsyncEvent, MStateMachineStepLogger* aTestStep, CStateMachineImpl& aStateMachine, TInt aError) :
            CAsynchEvent(&aStateMachine) 
            {
            iRuns = 0;
            iError = aError;
            iAsyncEvent = aAsyncEvent;
            iTestStep = aTestStep;
            }
    CAsynchEvent* ProcessL(TRequestStatus& aStatus);
public:
    MStateMachineStepLogger* iTestStep;
    TAsyncEventTest iAsyncEvent;
    TInt iRuns;
    TInt iError;
    };

#endif /* STATEMACHINEIMPL_H_ */
