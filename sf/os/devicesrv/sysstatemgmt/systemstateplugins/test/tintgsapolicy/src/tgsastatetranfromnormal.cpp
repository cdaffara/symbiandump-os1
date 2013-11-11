// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "gsatestapps.h"

#include <ssm/ssmstates.hrh>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmdomaindefs.h>

#include "tgsastatetran_base.h"

class CStateTranFromNormal : public CTGsaStateTransition
	{
public:
	static CStateTranFromNormal* NewL();
	~CStateTranFromNormal();
	void StartTestL();

private:
	void ConstructL();
	CStateTranFromNormal();

public:
	void DoTestStateTranFromNormalL();
	};



static TInt StartTest(TAny* aStateTranFromNormal)
 	{
 	TInt testCompletionReason = KErrNone;
 	CStateTranFromNormal* stateTran = reinterpret_cast<CStateTranFromNormal*>(aStateTranFromNormal);
 	TBool stateTest = stateTran->DoStartStateTranTest();
 	if (stateTest)
		TRAP(testCompletionReason, stateTran->DoTestStateTranFromNormalL());

 	return testCompletionReason;
 	}

CStateTranFromNormal* CStateTranFromNormal::NewL()
	{
	CStateTranFromNormal* self = new(ELeave) CStateTranFromNormal();
	self->ConstructL();
	return self;
	}

void CStateTranFromNormal::ConstructL()
	{
	CTGsaStateTransition::ConstructL(KGsaTestStateTranResultFilePath);
	}

CStateTranFromNormal::~CStateTranFromNormal()
	{
	}

CStateTranFromNormal::CStateTranFromNormal()
	{
	}

// Tests the Fail state transitions under different scenarios
void CStateTranFromNormal::DoTestStateTranFromNormalL()
	{
	RSsmStateManager stateMan;
	TInt err = stateMan.Connect();
	CleanupClosePushL(stateMan);
	if (err == KErrNone)
		RDebug::Print(_L("\nStateTran Test: RSsmStateManager sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Test: Unable to connect to RSsmStateManager sess\n"));	
		User::Leave(KTestAppFailure);
		}

	// Creates and open the result file for streaming
	err = iFileWriteStream.Replace(iFs, KGsaTestStateTranFromNormalResult, EFileWrite);
	RDebug::Print(_L("\nStateTran Test: iFileWriteStream open with %d\n"), err);

	// Gets the Current system state
	CurrentSystemStateL();

	//Scenario 1 - Request for state change from Normal to invalid state
	RDebug::Print(_L("\nStateTran Test: Scenario 1 \n"));
	TSsmState state(10, KSsmAnySubState);
	TSsmStateTransition stateTrans_InvalidState(state, 1);

	TRequestStatus status;
	stateMan.RequestStateTransition(stateTrans_InvalidState, status);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Normal to Invalid state with %d\n "), status.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrNotSupported)
		{
		CurrentSystemStateL();
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to Invalid state completes with %d Expected -5\n "), status.Int());
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to Invalid state Failed"));
		User::Leave(KTestAppFailure);
		}

	//Scenario 2 - Request for state change from Normal to fail state with invalid substate
	RDebug::Print(_L("\nStateTran Test: Scenario 2 \n"));
	const TInt KInvalidFailSubState = 20;
	state.Set(ESsmFail, KInvalidFailSubState);
	TSsmStateTransition stateTrans_InvalidSubState(state, 1);

	stateMan.RequestStateTransition(stateTrans_InvalidSubState, status);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Normal to fail state with invalid substate %d\n "), status.Int());
	
	User::WaitForRequest(status);
	iFileWriteStream.WriteInt32L(status.Int());
	if (status.Int() == KErrNotSupported)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to fail state with invalid substate completes with %d Expected -5\n "), status.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to fail state with invalid substate Failed"));
		User::Leave(KTestAppFailure);		
		}

	TRequestStatus status1;
	TSsmState shutdownState_Critical(ESsmShutdown, ESsmShutdownSubStateCritical);
	TSsmStateTransition stateTrans_Shutdowncritical(shutdownState_Critical, 1);

	TRequestStatus status2;
	state.Set(ESsmFail, KSsmAnySubState);
	TSsmStateTransition stateTrans3(state, 1);

	//Scenario 3 - Request for two state changes(1. fail state 2. shutdown state) one after the other without waiting 
	// for the first one to complete. Cancels the state transition request. The first request to the fail state will be 
	// completed with KErrCancel and the second request to transit to the shutdown state is also completed with KErrCancel.

	RDebug::Print(_L("\nStateTran Test: Scenario 3 \n"));
	stateMan.RequestStateTransition(stateTrans3, status1);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Normal to fail state %d\n "), status1.Int());
	
	stateMan.RequestStateTransition(stateTrans_Shutdowncritical, status2);
	RDebug::Print(_L("\nStateTran Test: Start RequestStateTransition from Normal to shutdown state %d\n "), status2.Int());
	
	stateMan.RequestStateTransitionCancel();
	User::WaitForRequest(status1);
	iFileWriteStream.WriteInt32L(status1.Int());
	if (status1.Int() == KErrCancel)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to fail state completes with %d Expected -3\n "), status1.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to fail state Failed"));
		User::Leave(KTestAppFailure);		
		}

	User::WaitForRequest(status2);
	iFileWriteStream.WriteInt32L(status2.Int());
	if (status2.Int() == KErrCancel)
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to shutdown state completes %d Expected -3\n "), status2.Int());
		CurrentSystemStateL();
		}
	else
		{
		RDebug::Print(_L("\nStateTran Test: RequestStateTransition from Normal to shutdown state Failed"));
		User::Leave(KTestAppFailure);		
		}
	
	iFileWriteStream.Close();
	
	// Creates and open the result file for streaming
	    err = iFileWriteStream.Replace(iFs, KGsaTestStateTranFromShutdownResult, EFileWrite);
	    RDebug::Print(_L("\nStateTran Test: iFileWriteStream open with %d\n"), err);


	    // Gets the Current system state before starting the test
	    CurrentSystemStateL();

	    //Scenario 1 - Request for state change from Normal to shutdown state to critical substate
	    // Cancel the request immediately
	    RDebug::Print(_L("\nScenario 1 \n"));
	    TRequestStatus status_shutdowncritsubstate;
	    /*TSsmState shutdownState_Critical(ESsmShutdown, ESsmShutdownSubStateCritical);
	    TSsmStateTransition stateTrans_Shutdowncritical(shutdownState_Critical, 1);*/

	    stateMan.RequestStateTransition(stateTrans_Shutdowncritical, status_shutdowncritsubstate);
	    RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state critical substate %d\n "), status_shutdowncritsubstate.Int());

	    stateMan.RequestStateTransitionCancel();
	    
	    iFileWriteStream.WriteInt32L(status_shutdowncritsubstate.Int());
	    if (status_shutdowncritsubstate.Int() == KErrCancel)
	        {
	        CurrentSystemStateL();
	        RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state critical substate completed %d Expected -3\n "), status_shutdowncritsubstate.Int());
	        }
	    else
	        User::Leave(KTestAppFailure);

	    //Scenario 2 - Request for state change from Normal to shutdown state to any substate
	    // Cancel the request immediately, so that we can proceed with the next test scenario.
	    RDebug::Print(_L("\nScenario 2 \n"));
	    TRequestStatus status_shutdownanysubstate;
	    TSsmState shutdownState_Any(ESsmShutdown, KSsmAnySubState);
	    TSsmStateTransition stateTrans_shutdownAnySubState(shutdownState_Any, 1);

	    stateMan.RequestStateTransition(stateTrans_shutdownAnySubState, status_shutdownanysubstate);
	    RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state any substate %d\n "), status_shutdownanysubstate.Int());

	    stateMan.RequestStateTransitionCancel();
	    
	    iFileWriteStream.WriteInt32L(status_shutdownanysubstate.Int());
	    if (status_shutdownanysubstate.Int() == KErrCancel)
	        {
	        CurrentSystemStateL();
	        RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state any substate completed %d Expected -3\n "), status_shutdownanysubstate.Int());
	        }
	    else
	        User::Leave(KTestAppFailure);
	    
	    iFileWriteStream.Close();
	    
	    // Creates and open the result file for streaming
	        err = iFileWriteStream.Replace(iFs, KGsaTestStateTranToShutdownResult, EFileWrite);
	        RDebug::Print(_L("\nStateTran Test: iFileWriteStream open with %d\n"), err);


	        // Gets the Current system state before starting the test
	        CurrentSystemStateL();

	        //Scenario 1 - Request for state transition to shutdown state - invalid substate
	        // request to transit to the shutdown state will be completed with KErrNotSupported
	        RDebug::Print(_L("\nScenario 1 \n"));

	        TRequestStatus status_invalidsubstate;
	        TSsmState shutdownState_Invalid(ESsmShutdown, 150);
	        TSsmStateTransition stateTrans_Invalid(shutdownState_Invalid, 1);

	        stateMan.RequestStateTransition(stateTrans_Invalid, status_invalidsubstate);
	        RDebug::Print(_L("\n Start RequestStateTransition from Normal to shutdown state with invalid substate %d\n "), status_invalidsubstate.Int());
	        
	        User::WaitForRequest(status_invalidsubstate);
	        iFileWriteStream.WriteInt32L(status_invalidsubstate.Int());
	        if (status_invalidsubstate.Int() == KErrNotSupported)
	            {
	            CurrentSystemStateL();
	            RDebug::Print(_L("\n RequestStateTransition from Normal to shutdown state with invalid substate completes with %d Expected -5\n "), status_invalidsubstate.Int());
	            }
	        else
	            User::Leave(KTestAppFailure);
	        
	        iFileWriteStream.Close();
	    CleanupStack::PopAndDestroy(&stateMan);
	}

void CStateTranFromNormal::StartTestL()
	{
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// This async callback is used to start the state transition test once system enters to normal state.
	iAsyncCallbackForStartTest =  new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack startTestCallback(StartTest, this);
	iAsyncCallbackForStartTest->Set(startTestCallback);

	// call the callback function which keeps looping until the system state gets into normal state.
	iAsyncCallbackForStartTest->CallBack();
	
	RProcess::Rendezvous(KErrNone);
	sched->Start();

	CleanupStack::PopAndDestroy(1);
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	TInt testCompletionReason = KErrNone;
	if (cleanup)
		{
		__UHEAP_MARK;
		CStateTranFromNormal* stateTran = NULL;
		TRAP(r, stateTran = CStateTranFromNormal::NewL());
		if (r == KErrNone)
			{
			TRAP(testCompletionReason, stateTran->StartTestL());
			// testCompletionReason is the reason with which the test case completes.
			TRAP(r, stateTran->CommitTestResultsL(testCompletionReason));
			delete stateTran;
			}
		delete cleanup;
		__UHEAP_MARKEND;

		if (r != KErrNone)
			{
			User::Panic(_L("tgsastatetranfromnormalPanic"), r);
			}
		}

	__UHEAP_MARKEND;
	return r;
	}



