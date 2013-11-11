// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file Te_LbsApiSuiteStepBase.cpp
 @internalTechnology
*/

#include "te_lbsapisuitestepbase.h"
#include "te_lbsapisuitedefs.h"

#include "lcfsbucommondefinitions.h"

_LIT(KPanicThreadName, "TestPanicThread");
const TInt KTestHeapMinSize = 0x4000;
const TInt KTestHeapMaxSize = 0x8000;

// Device driver constants

TVerdict CTe_LbsApiSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsApiSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_LbsApiSuiteStepBase::~CTe_LbsApiSuiteStepBase()
	{
	}

CTe_LbsApiSuiteStepBase::CTe_LbsApiSuiteStepBase()
	{
	}




//
//
// TOOL FUNCTIONS
//
//
void CTe_LbsApiSuiteStepBase::StandardPrepareL()
{
    START_ERROR_LOGGING;
}

void CTe_LbsApiSuiteStepBase::StandardCleanup()
{
}

//
// PANIC TEST SUPPORT
//

void TurnJITBackOn(TAny* /*aPtr*/)
	{
	User::SetJustInTime (ETrue);
	}

TInt PanicThreadFunction(TAny* aPtr)
	{
	CTrapCleanup* cs = CTrapCleanup::New ();
	if ( cs == NULL)
		return KErrNoMemory;

	// we may need an ActiveScheduler
	CActiveScheduler scheduler;
	CActiveScheduler::Install (&scheduler);

	SPanicThreadFunctionData* data =
			reinterpret_cast<SPanicThreadFunctionData*>(aPtr);
	TInt err;
	TRAP(err, data->iPanicFunction(data->iPtr)); // should panic

	delete cs;
	User::Exit (err);
	return err;
	}

TInt CTe_LbsApiSuiteStepBase::DoPanicTestL (
		TThreadFunction aThreadFunction, TExitCategoryName aExpectedExitCat,
		TInt aExpectedExitReason, TTimeIntervalMicroSeconds32 aTimeoutValue,
		TAny* aPtr)

	{
	DECLARE_ERROR_LOGGING
	;

#ifdef __WINS__
	User::SetJustInTime (EFalse);
	CleanupStack::PushL (TCleanupItem (TurnJITBackOn));
#endif
	RThread panickingThread;

	SPanicThreadFunctionData threadData;
	threadData.iPanicFunction = aThreadFunction;
	threadData.iPtr = aPtr;

	// check that thread does not already exist
	TInt result = KErrNone;
	TBuf<24> threadName(KPanicThreadName);

	result = panickingThread.Create (threadName, PanicThreadFunction,
			KDefaultStackSize, KTestHeapMinSize, KTestHeapMaxSize, &threadData,
			EOwnerProcess);

	CHECK_EQUAL(result, KErrNone, "Unexpected error while creating panic thread")
	;
	User::LeaveIfError (result);

	CleanupClosePushL (panickingThread);

	RTimer timeoutTimer;
	User::LeaveIfError (timeoutTimer.CreateLocal ());
	CleanupClosePushL (timeoutTimer);

	TRequestStatus timerStatus;
	timeoutTimer.After (timerStatus, aTimeoutValue);

	TRequestStatus threadStatus;

	panickingThread.Rendezvous (threadStatus);
	if ( threadStatus != KRequestPending)
		{
		// logon failed - thread is not yet running, so cannot have terminated
		User::WaitForRequest (threadStatus); // eat signal
		panickingThread.Kill (threadStatus.Int ()); // abort startup
		}
	else
		{
		panickingThread.Resume ();
		User::WaitForRequest (threadStatus, timerStatus);
		}

	TInt exitReason = KErrNone;
	if ( threadStatus == KRequestPending)
		{
		ERR("Thread didn't die. Killing it...");
		panickingThread.Kill (KErrTimedOut);
		User::WaitForRequest (threadStatus);
		}
	else // (timerStatus == KRequestPending)
		{
		// stop timer
		timeoutTimer.Cancel ();
		User::WaitForRequest (timerStatus);
		}

	exitReason = panickingThread.ExitReason ();

	TExitCategoryName exitCat;
	
	switch (panickingThread.ExitType ())
		{
	case EExitKill:
		message.Format (_L("Thread was Killed with Reason %d"), exitReason);
		LOG_DES(message);
		break;
	case EExitTerminate:
		message.Format (_L("Thread was Terminated with Reason %d"), exitReason);
		LOG_DES(message);
		break;
	case EExitPanic:
		// check exit reasons
		exitCat = panickingThread.ExitCategory ();
		if ( exitCat.Compare (aExpectedExitCat) != 0)
			{
			message.Format (_L("Unexpected panic category: Actual '%S', Expected '%S'"),
			&exitCat, &aExpectedExitCat);
			LOG_DES(message);
			}
		CHECK_EQUAL(exitReason, aExpectedExitReason, "Unexpected panic reason");
		break;
	default:
		CHECK_EQUAL((TInt)EExitPanic, threadStatus.Int(), "Unexpected thread exit type!");
		break;
		}

	CleanupStack::PopAndDestroy (&timeoutTimer);
	CleanupStack::PopAndDestroy (&panickingThread);
#ifdef __WINS__
	CleanupStack::PopAndDestroy (); //TurnJITBackOn
#endif
	// wait a bit to make sure this thread is over
	const TTimeIntervalMicroSeconds32 KThreadDelay = 1000 * 1000; // 1 s
	User::After (KThreadDelay);

	return exitReason;
	}

void CheckPanic(const char* aFileName, TInt aLine)
	{
    TBuf<64> file;
    TBuf<80> panic;

    file.Copy(TPtrC8(_S8(aFileName)).Right(file.MaxLength()));
    file = file.Mid(file.LocateReverse('\\')+1);

    panic.Format(_L("%d-%S"), aLine, &file);
    User::Panic(panic, KErrGeneral);
	}
