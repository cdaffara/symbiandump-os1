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
// This contains ESock Test cases from section 15
// 
//

#include "StateMachineStep.h"
#include <test/testexecutelog.h>

void CElementStateMachineTestBase::Setup()
    {
    iActiveScheduler = CStateMachineImpl::StartActiveSchedulerL();
    CleanupStack::PushL(iActiveScheduler);
    iStateMachine = CStateMachineImpl::NewL();
    }

void CElementStateMachineTestBase::Destroy()
    {
    CleanupStack::PopAndDestroy(iStateMachine);
    CActiveScheduler::Install(NULL);
    CleanupStack::PopAndDestroy(iActiveScheduler);
    }

CElementStateMachineStep1_1::CElementStateMachineStep1_1()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep1_1);
    }

TVerdict CElementStateMachineStep1_1::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    GetIntFromConfig(KElementStateMachineStep1_1, KElementStateMachineHistory, iHistory);
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_1::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the history member of a state machine can be set and retreived correctly
 */
    {
    INFO_PRINTF2(_L("Check history can be set and retrieved as %d."), iHistory);
    iStateMachine->UpdateHistory(iHistory);
    TInt returnHistory = iStateMachine->History();
    INFO_PRINTF3(_L("Expected history: %d, returned history: %d"), iHistory, returnHistory);
    if (returnHistory != iHistory)
        iResult = EFail;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_1::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep1_2::CElementStateMachineStep1_2()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep1_2);
    }

TVerdict CElementStateMachineStep1_2::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    GetIntFromConfig(KElementStateMachineStep1_2, KElementStateMachineLastError, iLastError);
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_2::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the last error member of a state machine can be set and retreived correctly
 */
    {
    INFO_PRINTF2(_L("Check last error can be set and retrieved as %d."), iLastError);
    iStateMachine->SetLastError(iLastError);
    TInt returnLastError = iStateMachine->LastError();
    INFO_PRINTF3(_L("Expected last error: %d, returned : %d"), iLastError, returnLastError);
    if (returnLastError != iLastError)
        iResult = EFail;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_2::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep1_3::CElementStateMachineStep1_3()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep1_3);
    }

TVerdict CElementStateMachineStep1_3::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_3::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the suspend reques member of a state machine can be set and retreived correctly
 */
    {
    INFO_PRINTF1(_L("Check suspend request can be set and retrieved."));
    iStateMachine->SetSuspendRequest(EFalse);
    TBool returnBool = iStateMachine->SuspendRequest();
    if (returnBool)
        iResult = EFail;
    iStateMachine->SetSuspendRequest(ETrue);
    returnBool = iStateMachine->SuspendRequest(); 
    if (!returnBool)
        iResult = EFail;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_3::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep1_4::CElementStateMachineStep1_4()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep1_4);
    }

TVerdict CElementStateMachineStep1_4::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    GetIntFromConfig(KElementStateMachineStep1_4, KElementStateMachNullRAllcLngth, iNullReAllocLength);
    GetIntFromConfig(KElementStateMachineStep1_4, KElementStateMachSetRAllocLngth, iSetReAllocLength);
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_4::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the fragment member of a state machine can be allocated and returned
 */
    {
    INFO_PRINTF1(_L("Check fragmentation reallocation when fragment is null."));
    iStateMachine->SetFragment(NULL);
    HBufC8* fragment = iStateMachine->ReAllocL(iNullReAllocLength);
    TInt fragmentLength = fragment->Des().MaxSize();
    INFO_PRINTF3(_L("Expected length at least: %d, returned length: %d"), iNullReAllocLength, fragmentLength);
    //Length returned is slightly bigger initially
    if (fragmentLength < iNullReAllocLength)
        iResult = EFail;
    INFO_PRINTF1(_L("Check fragmentation reallocation when fragment is already set."));
    fragment = iStateMachine->ReAllocL(iSetReAllocLength);
    fragmentLength = fragment->Des().MaxSize();
    INFO_PRINTF3(_L("Expected length at least: %d, returned length: %d"), iSetReAllocLength, fragmentLength);
    if (fragmentLength < iSetReAllocLength)
        iResult = EFail;
    INFO_PRINTF1(_L("Check fragmentation reallocation isn't done when new length is greater than Des().MaxLength()"));
    fragment = iStateMachine->ReAllocL((fragment->Des().MaxLength() - iSetReAllocLength) + 1);
    fragmentLength = fragment->Des().MaxSize();
    INFO_PRINTF3(_L("Expected length at least: %d, returned length: %d"), iSetReAllocLength, fragmentLength);
    if (fragmentLength < iSetReAllocLength)
        iResult = EFail;
    INFO_PRINTF1(_L("Check fragment return equals expected object"));
    if (fragment != iStateMachine->Fragment())
        iResult = EFail;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_4::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep1_5::CElementStateMachineStep1_5()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep1_5);
    }

TVerdict CElementStateMachineStep1_5::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_5::doTestStepL()
/**
 * @return - TVerdict code
 * Tests state machine notifier member can be registered and deregistered
 */
    {
    CStateMachineNotifyImpl stateMachineNotify = CStateMachineNotifyImpl();
    INFO_PRINTF1(_L("Check that initial state machine notifier is NULL"));
    MStateMachineNotify* stateMachineNotifyReturn = iStateMachine->StateMachineNotifier();
    if (stateMachineNotifyReturn != NULL)
        iResult = EFail;
    INFO_PRINTF1(_L("Check that the notifier can be set"));
    iStateMachine->RegisterNotify(&stateMachineNotify);
    stateMachineNotifyReturn = iStateMachine->StateMachineNotifier();
    if (stateMachineNotifyReturn != &stateMachineNotify)
        iResult = EFail;
    INFO_PRINTF1(_L("Check that the notifier can be cleared"));
    iStateMachine->DeRegisterNotify(&stateMachineNotify);
    stateMachineNotifyReturn = iStateMachine->StateMachineNotifier();
    if (stateMachineNotifyReturn != NULL)
        iResult = EFail;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep1_5::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep2_1::CElementStateMachineStep2_1()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep2_1);
    }

TVerdict CElementStateMachineStep2_1::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iExpectedAsyncEvent = EGreenAsyncEvent;
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep2_1::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the state machine can be started correctly and moves through a set of states
 */
    {
    INFO_PRINTF1(_L("Check that state machine correctly moves through states GREEN, YELLOW, RED before stopping"));
    INFO_PRINTF1(_L("Create asynchronous events (states)"));
    CAsyncEventImpl greenAsync = CAsyncEventImpl(EGreenAsyncEvent, this, *iStateMachine, KErrNone);
    CAsyncEventImpl yellowAsync = CAsyncEventImpl(EYellowAsyncEvent, this, *iStateMachine, KErrNone);
    CAsyncEventImpl redAsync = CAsyncEventImpl(ERedAsyncEvent, this, *iStateMachine, KErrNone);
    greenAsync.SetNext(&yellowAsync);
    yellowAsync.SetNext(&redAsync);
    redAsync.SetNext(NULL);
    TRequestStatus startStatus;
    INFO_PRINTF1(_L("Set state machine first active event to GREEN"));
    iStateMachine->SetActiveEvent(&greenAsync);
    INFO_PRINTF1(_L("Start state machine"));
    iStateMachine->Start(&startStatus, NULL, NULL);
    iActiveScheduler->Start();
    User::WaitForRequest(startStatus);
    return TestStepResult();
    }

void CElementStateMachineStep2_1::LogStateChange(TAsyncEventTest aAsyncEvent)
    {
    if (iExpectedAsyncEvent == EGreenAsyncEvent)
        {
        if (aAsyncEvent == EGreenAsyncEvent)
            INFO_PRINTF1(_L("Received expected GREEN state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected GREEN state"));
            iResult = EFail;
            }
        iExpectedAsyncEvent = EYellowAsyncEvent;
        }
    else if (iExpectedAsyncEvent == EYellowAsyncEvent)
        {
        if (aAsyncEvent == EYellowAsyncEvent)
            INFO_PRINTF1(_L("Received expected YELLOW state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected YELLOW state"));
            iResult = EFail;
            }
        iExpectedAsyncEvent = ERedAsyncEvent;
        }
    else if (iExpectedAsyncEvent == ERedAsyncEvent)
        {
        if (aAsyncEvent == ERedAsyncEvent)
            INFO_PRINTF1(_L("Received expected RED state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected RED state"));
            iResult = EFail;
            }
        }
    }

TVerdict CElementStateMachineStep2_1::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep2_2::CElementStateMachineStep2_2()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep2_2);
    }

TVerdict CElementStateMachineStep2_2::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iExpectedAsyncEvent = ERedAsyncEvent;
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep2_2::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the state machine does nothing when started without a client status,
 * and runs when one is attached with KRequestPending
 */
    {
    INFO_PRINTF1(_L("Create asynchronous RED event (state)"));
    CAsyncEventImpl redAsync = CAsyncEventImpl(ERedAsyncEvent, this, *iStateMachine, KErrNone);
    redAsync.SetNext(NULL);
    INFO_PRINTF1(_L("Set state machine first active event to RED"));
    iStateMachine->SetActiveEvent(&redAsync);
    INFO_PRINTF1(_L("Start state machine with NULL client status"));
    iStateMachine->Start(NULL, NULL, NULL);
    TRequestStatus* nullClientStatus = iStateMachine->ClientStatus();
    INFO_PRINTF1(_L("Check returned client status from state machine is NULL"));
    if (nullClientStatus != NULL)
        {
        INFO_PRINTF1(_L("NULL TRequestStatus not recieved from state machine!"));
        iResult = EFail;
        }
    TRequestStatus clientStatus;
    clientStatus = KRequestPending;
    INFO_PRINTF1(_L("Set client status of state machine to KRequestPending"));
    iStateMachine->SetClientStatus(&clientStatus);
    INFO_PRINTF1(_L("Start state machine"));
    iActiveScheduler->Start();
    User::WaitForRequest(clientStatus);
    return TestStepResult();
    }

void CElementStateMachineStep2_2::LogStateChange(TAsyncEventTest aAsyncEvent)
    {
    if (aAsyncEvent == ERedAsyncEvent)
        INFO_PRINTF1(_L("Received expected RED state"));
    else
        {
        INFO_PRINTF1(_L("Did not receive expected RED state"));
        iResult = EFail;
        }
    }

TVerdict CElementStateMachineStep2_2::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep2_3::CElementStateMachineStep2_3()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep2_3);
    }

TVerdict CElementStateMachineStep2_3::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iExpectedAsyncEvent = EPanicAsyncEvent;
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep2_3::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the state machine handles error upon an active event leaving in ProcessL
 */
    {
    INFO_PRINTF1(_L("Create asynchronous panicing event (state)"));
    CAsyncEventImpl panicAsync = CAsyncEventImpl(EPanicAsyncEvent, this, *iStateMachine, KErrNone);
    panicAsync.SetNext(NULL);
    INFO_PRINTF1(_L("Set state machine first active event to PANIC"));
    iStateMachine->SetActiveEvent(&panicAsync);
    INFO_PRINTF1(_L("Start state machine"));
    TRequestStatus clientStatus;
    iStateMachine->Start(&clientStatus, NULL, NULL);
    iActiveScheduler->Start();
    User::WaitForRequest(clientStatus);
    TInt errorCode = iStateMachine->LastError();
    INFO_PRINTF1(_L("Check returned error is expected error (from iLastError member)"));
    INFO_PRINTF3(_L("Expected error: %d, received error: %d"), KElementStateMachineError, errorCode);
    if (errorCode != KElementStateMachineError)
        iResult = EFail;
    return TestStepResult();
    }

void CElementStateMachineStep2_3::LogStateChange(TAsyncEventTest aAsyncEvent)
    {
    if (iExpectedAsyncEvent == EPanicAsyncEvent)
        {
        if (aAsyncEvent == EPanicAsyncEvent)
            INFO_PRINTF1(_L("Received expected PANIC state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected PANIC state"));
            iResult = EFail;
            }
        }
    }

TVerdict CElementStateMachineStep2_3::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep2_4::CElementStateMachineStep2_4()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep2_4);
    }

TVerdict CElementStateMachineStep2_4::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iExpectedAsyncEvent = EErroringAsyncEvent;
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep2_4::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the state machine sets and runs an error event if an active event returns an error
 */
    {
    INFO_PRINTF1(_L("Create asynchronous erroring and error events (states)"));
    CAsyncEventImpl erroringAsync = CAsyncEventImpl(EErroringAsyncEvent, this, *iStateMachine, KElementStateMachineError);
    CAsyncEventImpl errorAsync = CAsyncEventImpl(EErrorAsyncEvent, this, *iStateMachine, KErrNone);
    erroringAsync.SetNext(NULL);
    errorAsync.SetNext(NULL);
    INFO_PRINTF1(_L("Set state machine first active event to ERRORING"));
    iStateMachine->SetActiveEvent(&erroringAsync);
    INFO_PRINTF1(_L("Start state machine"));
    TRequestStatus clientStatus;
    iStateMachine->Start(&clientStatus, NULL, NULL);
    iStateMachine->SetErrorEvent(&errorAsync);
    iActiveScheduler->Start();
    User::WaitForRequest(clientStatus);
    TInt error = iStateMachine->LastError();
    INFO_PRINTF1(_L("Check returned error is expected error (from iLastError member)"));
    INFO_PRINTF3(_L("Expected error: %d, received error: %d"), KElementStateMachineError, error);
    if (error != KElementStateMachineError)
        iResult = EFail;
    return TestStepResult();
    }

void CElementStateMachineStep2_4::LogStateChange(TAsyncEventTest aAsyncEvent)
    {
    if (iExpectedAsyncEvent == EErroringAsyncEvent)
        {
        if (aAsyncEvent == EErroringAsyncEvent)
            INFO_PRINTF1(_L("Received expected ERRORING state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected ERRORING state"));
            iResult = EFail;
            }
        iExpectedAsyncEvent = EErrorAsyncEvent;
        }
    else if (iExpectedAsyncEvent == EErrorAsyncEvent)
        {
        if (aAsyncEvent == EErrorAsyncEvent)
            INFO_PRINTF1(_L("Received expected ERROR state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected ERROR state"));
            iResult = EFail;
            }
        }
    }

TVerdict CElementStateMachineStep2_4::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep2_5::CElementStateMachineStep2_5()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep2_5);
    }

TVerdict CElementStateMachineStep2_5::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iExpectedAsyncEvent = EForcedErrorAsyncEvent;
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep2_5::doTestStepL()
/**
 * @return - TVerdict code
 * Tests that the state machine responds to retrospective errors being set in the async event
 */
    {
    INFO_PRINTF1(_L("Create asynchronous erroring and error events (states)"));
    CAsyncEventImpl forcedErroringAsync = CAsyncEventImpl(EForcedErrorAsyncEvent, this, *iStateMachine, KElementStateMachineError);
    forcedErroringAsync.SetNext(&forcedErroringAsync);
    INFO_PRINTF1(_L("Set state machine first active event to FORCEDERRORING"));
    iStateMachine->SetActiveEvent(&forcedErroringAsync);
    INFO_PRINTF1(_L("Start state machine"));
    TRequestStatus clientStatus;
    iStateMachine->Start(&clientStatus, &forcedErroringAsync, NULL);
    iActiveScheduler->Start();
    User::WaitForRequest(clientStatus);
    TInt error = iStateMachine->LastError();
    INFO_PRINTF1(_L("Check returned error is expected error (from iLastError member)"));
    INFO_PRINTF3(_L("Expected error: %d, received error: %d"), KElementStateMachineError, error);
    if (error != KElementStateMachineError)
        iResult = EFail;
    INFO_PRINTF1(_L("Check number of runs of async ProcessL"));
    TInt runs = forcedErroringAsync.iRuns;
    INFO_PRINTF2(_L("Expected runs: 2, actual runs: %d"), runs);
    if (runs != 2)
        iResult = EFail;
    return TestStepResult();
    }

void CElementStateMachineStep2_5::LogStateChange(TAsyncEventTest aAsyncEvent)
    {
    if (iExpectedAsyncEvent == EForcedErrorAsyncEvent)
        {
        if (aAsyncEvent == EForcedErrorAsyncEvent)
            INFO_PRINTF1(_L("Received expected FORCEDERRORING state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected FORCEDERRORING state"));
            iResult = EFail;
            }
        }
    }

TVerdict CElementStateMachineStep2_5::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep2_6::CElementStateMachineStep2_6()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep2_6);
    }

TVerdict CElementStateMachineStep2_6::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iExpectedAsyncEvent = ECancelAsyncEvent;
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep2_6::doTestStepL()
/**
 * @return - TVerdict code
 * Tests cancelling the state machine with no error
 */
    {
    INFO_PRINTF1(_L("Create asynchronous cancelling event (state)"));
    CAsyncEventImpl cancelAsync = CAsyncEventImpl(ECancelAsyncEvent, this, *iStateMachine, KErrNone);
    cancelAsync.SetNext(NULL);
    INFO_PRINTF1(_L("Set state machine first active event to CANCEL"));
    iStateMachine->SetActiveEvent(&cancelAsync);
    INFO_PRINTF1(_L("Start state machine"));
    TRequestStatus clientStatus;
    iStateMachine->Start(&clientStatus, NULL, NULL);
    iActiveScheduler->Start();
    User::WaitForRequest(clientStatus);
    TInt error = clientStatus.Int();
    INFO_PRINTF1(_L("Check returned error is expected error"));
    INFO_PRINTF3(_L("Expected error: %d, received error: %d"), KErrNone, error);
    if (error != KErrNone)
        iResult = EFail;
    return TestStepResult();
    }

void CElementStateMachineStep2_6::LogStateChange(TAsyncEventTest aAsyncEvent)
    {
    if (iExpectedAsyncEvent == ECancelAsyncEvent)
        {
        if (aAsyncEvent == ECancelAsyncEvent)
            INFO_PRINTF1(_L("Received expected CANCEL state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected CANCEL state"));
            iResult = EFail;
            }
        }
    }

TVerdict CElementStateMachineStep2_6::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep2_7::CElementStateMachineStep2_7()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep2_7);
    }

TVerdict CElementStateMachineStep2_7::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iExpectedAsyncEvent = ECancelAsyncEvent;
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep2_7::doTestStepL()
/**
 * @return - TVerdict code
 * Tests cancelling the state machine with an error
 */
    {
    INFO_PRINTF1(_L("Create asynchronous cancelling and error events (states)"));
    CAsyncEventImpl cancelAsync = CAsyncEventImpl(ECancelAsyncEvent, this, *iStateMachine, KElementStateMachineError);
    CAsyncEventImpl errorAsync = CAsyncEventImpl(EErrorAsyncEvent, this, *iStateMachine, KErrNone);
    cancelAsync.SetNext(NULL);
    errorAsync.SetNext(NULL);
    INFO_PRINTF1(_L("Set state machine first active event to CANCEL"));
    iStateMachine->SetActiveEvent(&cancelAsync);
    INFO_PRINTF1(_L("Start state machine"));
    TRequestStatus clientStatus;
    iStateMachine->Start(&clientStatus, &errorAsync, NULL);
    iActiveScheduler->Start();
    User::WaitForRequest(clientStatus);
    TInt error = clientStatus.Int();
    INFO_PRINTF1(_L("Check returned error is expected error"));
    INFO_PRINTF3(_L("Expected error: %d, received error: %d"), KErrCancel, error);
    if (error != KErrCancel)
        iResult = EFail;
    return TestStepResult();
    }

void CElementStateMachineStep2_7::LogStateChange(TAsyncEventTest aAsyncEvent)
    {
    if (iExpectedAsyncEvent == ECancelAsyncEvent)
        {
        if (aAsyncEvent == ECancelAsyncEvent)
            INFO_PRINTF1(_L("Received expected CANCEL state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected CANCEL state"));
            iResult = EFail;
            }
        iExpectedAsyncEvent = EErrorAsyncEvent;
        }
    else if (iExpectedAsyncEvent == EErrorAsyncEvent)
        {
        if (aAsyncEvent == EErrorAsyncEvent)
            INFO_PRINTF1(_L("Received expected ERROR state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected ERROR state"));
            iResult = EFail;
            }
        }
    }

TVerdict CElementStateMachineStep2_7::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }

CElementStateMachineStep2_8::CElementStateMachineStep2_8()
/**
 * Constructor
 */
    {
    // Call base class method to set up the human readable name for logging
    SetTestStepName(KElementStateMachineStep2_8);
    }

TVerdict CElementStateMachineStep2_8::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    SetTestStepResult(EFail);
    iResult = EFail;
    Setup();
    iExpectedAsyncEvent = EGreenAsyncEvent;
    iResult = EPass;
    return TestStepResult();
    }

TVerdict CElementStateMachineStep2_8::doTestStepL()
/**
 * @return - TVerdict code
 * Tests suspending and restarting the state machine
 */
    {
    INFO_PRINTF1(_L("Create asynchronous green, red and suspend events (states)"));
    CAsyncEventImpl greenAsync = CAsyncEventImpl(EGreenAsyncEvent, this, *iStateMachine, KErrNone);
    CAsyncEventImpl suspendAsync = CAsyncEventImpl(ESuspendAsyncEvent, this, *iStateMachine, KErrNone);
    CAsyncEventImpl redAsync = CAsyncEventImpl(ERedAsyncEvent, this, *iStateMachine, KErrNone);
    greenAsync.SetNext(&suspendAsync);
    suspendAsync.SetNext(&redAsync);
    redAsync.SetNext(NULL);
    INFO_PRINTF1(_L("Set state machine first active event to GREEN"));
    iStateMachine->SetActiveEvent(&greenAsync);
    CStateMachineNotifyImpl stateMachineNotify = CStateMachineNotifyImpl();
    INFO_PRINTF1(_L("Start state machine"));
    TRequestStatus clientStatus;
    iStateMachine->Start(&clientStatus, NULL, &stateMachineNotify);
    iActiveScheduler->Start();
    User::WaitForRequest(clientStatus);
    TInt greenRuns = greenAsync.iRuns;
    INFO_PRINTF2(_L("Check that GREEN has been run once, runs: %d"), greenRuns);
    if (greenRuns != 1)
        iResult = EFail;
    TInt suspendRuns = suspendAsync.iRuns;
    INFO_PRINTF2(_L("Check that SUSPEND has been run once, runs: %d"), suspendRuns);
    if (suspendRuns != 1)
        iResult = EFail;
    TInt redRuns = redAsync.iRuns;
    INFO_PRINTF2(_L("Check that RED has been run once, runs: %d"), redRuns);
    if (redRuns != 1)
        iResult = EFail;
    return TestStepResult();
    }

void CElementStateMachineStep2_8::LogStateChange(TAsyncEventTest aAsyncEvent)
    {
    if (iExpectedAsyncEvent == EGreenAsyncEvent)
        {
        if (aAsyncEvent == EGreenAsyncEvent)
            INFO_PRINTF1(_L("Received expected GREEN state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected GREEN state"));
            iResult = EFail;
            }
        iExpectedAsyncEvent = ESuspendAsyncEvent;
        }
    else if (iExpectedAsyncEvent == ESuspendAsyncEvent)
        {
        if (aAsyncEvent == ESuspendAsyncEvent)
            INFO_PRINTF1(_L("Received expected SUSPEND state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected SUSPEND state"));
            iResult = EFail;
            }
        iExpectedAsyncEvent = ERedAsyncEvent;
        }
    else if (iExpectedAsyncEvent == ERedAsyncEvent)
        {
        if (aAsyncEvent == ERedAsyncEvent)
            INFO_PRINTF1(_L("Received expected RED state"));
        else
            {
            INFO_PRINTF1(_L("Did not receive expected RED state"));
            iResult = EFail;
            }
        }
    }

TVerdict CElementStateMachineStep2_8::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
    {
    Destroy();
    SetTestStepResult(iResult);
    return TestStepResult();
    }
