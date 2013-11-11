// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// NOTE:
//

// ### The idea is that the test will request the following transitions:
// InitialState 
// |	(request transition to state NextStateA)
// NextStateA (with sub states A1 and A2)
// |	(::GetNextState() returns NextStateB)
// NextStateB (with sub states B1 and B2)
// |	(::GetNextState() returns NextStateC)
// NextStateC ----> There are no further transitions in this test state, so the loop terminates!
// ### To verify that each state is activated, the cmd list will inclue a CreateSwp command,
// so the test needs to verify that the 4 unique swp keys exist.
// swp	UID = substate
// swp	Value = mainstate + substate
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32property.h>
#include <ssm/ssmstates.hrh>
#include <ssm/ssmstatemanager.h>
#include "ssmserver.h"
#include "ssmclisrv.h"

#include "tssm_ssmclient.h"
#include "tssm_startserver.h"
#include "tssm_statepolicy_simple.h"
#include "tssm_statepolicy_nextstatea.h"
#include "tssm_statepolicy_nextstateb.h"
#include "tssm_statepolicy_nextstatec.h"
#include "tssm_statepolicy_fail.h"
#include "ssmtestapps.h"

#include "tssm_step_statefurthertransition.h"
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmdomaindefs.h>
#include <ssm/ssmstateawaresession.h>

//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)
const TUid KPropertyCategory=KMySID;


/**
Old Test CaseID 		APPFWK-SSM-0018
New Test CaseID 		DEVSRVS-SSMA-SSM-0018
 */
//---------------- Test Steps ---------------------------------------

void CSsmStateFurtherTransitionTest::DoStateRequestTransition()
	{
	INFO_PRINTF1(_L("Doing a single async RequestStateTransition that will chain other transition requests..."));

	RSsmStateManagerTestClient client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);
	client.HeapMark();

	// Request state transitions in the same session
	INFO_PRINTF1(_L("Request state transition to state NextStateA...")); 
	TSsmStateTransition state1(ESsmTestNextStateAState, ESsmTestNextStateA1Substate, 00);
	TRequestStatus status1;
	client.RequestStateTransition(state1, status1);						// (simple) policy returns EDefinitelyAllowed
	TEST(status1 == KRequestPending);

	User::WaitForRequest(status1);
	INFO_PRINTF2(_L("  --completion codes:  req1 (%d)"), status1.Int());
	TEST(status1.Int() == KErrNone);
	
	// This semphore is used to signal the completion of the command list execution.
	RSemaphore sem;
	TInt err = sem.CreateGlobal(KStateTransitionComplete, 0);
	TEST(err == KErrNone);

	//Waiting for the completion of the command list execution.
	sem.Wait();
	sem.Close();
	
	// Check that all SWP were properly created while transitioning all the states
	RProperty prop;
	TInt value = 0;
	TEST(KErrNone == prop.Get(KPropertyCategory, ESsmTestNextStateA1Substate, value));
	TESTE(value==ESsmTestNextStateAState+ESsmTestNextStateA1Substate, value);

	TEST(KErrNone == prop.Get(KPropertyCategory, ESsmTestNextStateA2Substate, value));
	TESTE(value==ESsmTestNextStateAState+ESsmTestNextStateA2Substate, value);

	TEST(KErrNone == prop.Get(KPropertyCategory, ESsmTestNextStateB1Substate, value));
	TESTE(value==ESsmTestNextStateBState+ESsmTestNextStateB1Substate, value);

	TEST(KErrNone == prop.Get(KPropertyCategory, ESsmTestNextStateB2Substate, value));
	TESTE(value==ESsmTestNextStateBState+ESsmTestNextStateB2Substate, value);

	TEST(KErrNone == prop.Get(KPropertyCategory, ESsmTestNextStateC1Substate, value));
	TESTE(value==ESsmTestNextStateCState+ESsmTestNextStateC1Substate, value);

	// Request state transitions in the same session, to check that last state's the policy is properly loaded
	//	Use appropriate reason, otherwise NextStateC would panic
	INFO_PRINTF1(_L("Request state transition to state simple, but NextStateC should not allow it")); 
	TSsmStateTransition state2(ESsmTestSimpleState, KSsmAnySubState, ESsmTestNextStateCNoPanicReason);
	TRequestStatus status2;
	client.RequestStateTransition(state2, status2);						// (NextStateC) policy returns ENotAllowed
	TEST(status2 == KRequestPending);

	User::WaitForRequest(status2);
	INFO_PRINTF2(_L("  --completion codes:  req2 (%d)"), status2.Int());
	TEST(status2.Int() == KErrNotSupported);
	
	client.HeapMarkEnd(EDoCleanup);		//Pass ETrue for cleaning up Transition Engine after the test case 	
	// clean-up
	client.Close();
	INFO_PRINTF1(_L("case completed\n"));
	}

/**
Old Test CaseID 		APPFWK-SSM-0021
New Test CaseID 		DEVSRVS-SSMA-SSM-0018
 */


void CSsmStateFurtherTransitionTest::DoStateRequestTransitionFromNormalState()
	{
	INFO_PRINTF1(_L("DoStateRequestTransitionFromNormalState test started...."));

	TInt err = KErrNone;

	// The current state is tested only in winscw because in armv5 the current state would not 
	// be normal state as other test cases which have run before this test case would have changed 
	// the system state to other than normal state.
#ifdef __WINS__
	RSsmStateAwareSession sess;
	err = sess.Connect(KSM2UiServicesDomain3);
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("RSsmStateAwareSession sess Connect %d"), err);

	TSsmState currenState_before;
	currenState_before = sess.State();
	TEST(currenState_before.MainState() == ESsmNormal);
	INFO_PRINTF2(_L("Current System State before starting the test %d"), currenState_before.MainState());
#endif
	
	INFO_PRINTF1(_L("Create a test application without powermgt capability"));
	RProcess proc;
	err = proc.Create(KTestProcTranFromNormalWoCap, KNullDesC);
	INFO_PRINTF2(_L("Test application Created with err %d"), err);

	proc.Resume();
	TRequestStatus status;
	proc.Logon(status);
	INFO_PRINTF2(_L("Process resumed and logon status is %d"),status.Int());
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Process exit status %d"),status.Int());
	TEST(status != KTestAppFailure && status == KErrNone );
	proc.Close();

#ifdef __WINS__
	TSsmState currenState_After = sess.State();
	INFO_PRINTF2(_L("Current System State After completing the test %d"), currenState_After.MainState());
	TEST(currenState_After.MainState() == ESsmNormal);
	TEST(currenState_After.MainState() == currenState_before.MainState());
	sess.Close();
#endif
	INFO_PRINTF1(_L("DoStateRequestTransitionFromNormalState test completed....\n"));
	}


//---------------- CSsmStateFurtherTransitionTest step ---------------------------------------


CSsmStateFurtherTransitionTest::~CSsmStateFurtherTransitionTest()
	{
	}

CSsmStateFurtherTransitionTest::CSsmStateFurtherTransitionTest()
	{
	SetTestStepName(KTSsmStateFurtherTransitionStep);
	}

TVerdict CSsmStateFurtherTransitionTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmStateFurtherTransitionTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CSsmStateFurtherTransitionTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmStateFurtherTransitionTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	TEST(KErrNone == StartServer(thread));	

	// Run the tests
	DoStateRequestTransitionFromNormalState();
	DoStateRequestTransition();

	// clean-up
	INFO_PRINTF1(_L("cleaning up"));
	thread.Kill(KErrNone);
	thread.Close();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmStateFurtherTransitionTest completed!!"));
	return TestStepResult();
	}


