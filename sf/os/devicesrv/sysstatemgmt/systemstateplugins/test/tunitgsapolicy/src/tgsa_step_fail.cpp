// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "gsastatepolicyfail.h"
#include <ssm/ssmstatetransition.h>
#include "ssmdebug.h"
#include <e32uid.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmcommand.h>
#include "ssmsubstateext.hrh"

#include "tgsa_step_fail.h"

TSsmCommandType ArrFail[] = {   ESsmCmdCustomCommand,       //r_cmd_sastate
                                ESsmCmdPublishSystemState,  //r_cmd_publishstate
                                ESsmCmdSetPAndSKey,         //r_cmd_psstate
                                ESsmCmdCustomCommand,       //r_cmd_cancelmonitoring
                                ESsmCmdSetPAndSKey,         //r_cmd_killsplash
                                ESsmCmdCustomCommand,       //r_cmd_contactservice
                                ESsmCmdStartApp,            //r_cmd_sysap
                                ESsmCmdMultipleWait         //r_cmd_multiwaitforever
                                };
CGsaFailTest::~CGsaFailTest()
	{
	}

CGsaFailTest::CGsaFailTest() : CGsaTestBase(KTGsaFailStep)
	{
	}

TVerdict CGsaFailTest::doTestStepL()
	{
	INFO_PRINTF1(_L("> CGsaFailTest::doTestStepL"));
	TInt err = KErrNone;	

	__UHEAP_MARK;
	TRAP(err, doTestNewL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestInitializeL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestPrepareCommandListL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestCommandListL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TRAP(err, doTestGetNextStateL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

/**
Old Test CaseID 		AFSS-GSA-0025
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0025
 */


void CGsaFailTest::doTestNewL()
	{
	INFO_PRINTF1(_L("> CGsaFailTest::doTestNewL"));
	
	TInt expectedErr = KErrNone;
	MSsmStatePolicy* policy = NULL;
	//test should pass
	TRAPD(err, policy = CGsaStatePolicyFail::NewL());
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	TEST(policy != NULL);
	policy->Release();
	}

/**
Old Test CaseID 		AFSS-GSA-0026
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0025
 */
void CGsaFailTest::doTestInitializeL()
	{
	INFO_PRINTF1(_L("> CGsaFailTest::doTestInitializeL"));
	CGsaStatePolicyFail* policy = CreateAndInitializeFailPolicyLC();
	CleanupStack::PopAndDestroy(policy);
	}

/**
Old Test CaseID 		AFSS-GSA-0027
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0025
 */
 void CGsaFailTest::doTestPrepareCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaFailTest::doTestPrepareCommandListL"));
	//The default substate in the actual (fail policy) is mapped to 
	//ESsmStateFail = 0x30
	//Hence testing for the same.
	TestPrepareCommandListL(ESsmFail ,ESsmStateFail , KErrNone);
	TestPrepareCommandListL(ESsmFail ,100, KErrNotFound);
	}

/**
Old Test CaseID 		AFSS-GSA-0028
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0025
 */
void CGsaFailTest::doTestCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaFailTest::doTestCommandListL"));

	TestCommandListL(ESsmFail, ESsmStateFail, (sizeof(ArrFail)/sizeof(ArrFail[0])));
	}

/**
Old Test CaseID 		AFSS-GSA-0029
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0025
 */

void CGsaFailTest::doTestGetNextStateL()
	{
	CGsaStatePolicyFail* policy = CreateAndInitializeFailPolicyLC();

	TSsmState state(ESsmFail, ESsmStateFail);
	TSsmState result(0, 0);
	TestGetNextState(policy, state, KErrNone, result);

	TSsmState firstState(ESsmFail, ESsmStateFail);
	TestGetNextState(policy, firstState, KErrNone, result);

	TSsmState unknownState(ESsmFail, 100);
	TestGetNextState(policy, unknownState, KErrNone, result);

	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to create and initialize fail state policy.
*/
CGsaStatePolicyFail* CGsaFailTest::CreateAndInitializeFailPolicyLC()
	{
	TRequestStatus trs;
	TRequestStatus expectedTrs = KErrNone;
	CGsaStatePolicyFail* self = static_cast<CGsaStatePolicyFail*>(CGsaStatePolicyFail::NewL());
	CleanupStack::PushL(self);
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	// Test for cancelling the initialize request
	self->InitializeCancel();
	expectedTrs = KErrCancel;
	TEST(trs.Int() == expectedTrs.Int());
	INFO_PRINTF3(_L("Fail Policy Initialization cancelled with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());

	// Call Initialize again to complete the intialization of the command list
	trs = KErrNone;
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	StartScheduler();
	User::WaitForRequest(trs);
	expectedTrs = KErrNone;
	INFO_PRINTF3(_L("Fail Policy Initialized with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());
	TEST(trs.Int() == expectedTrs.Int());

	return self;
	}

/**
 Helper function to test preparation of command lists for fail state policy.
*/
void CGsaFailTest::TestPrepareCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aExpectedResult)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyFail* policy = static_cast<CGsaStatePolicyFail*>(CGsaStatePolicyFail::NewL());
	CleanupStack::PushL(policy);
	policy->Initialize(trs);
	StartScheduler();
	User::WaitForRequest(trs);
	TEST( trs.Int() == KErrNone);

	// PrepareCommandList //
	trs = -1;
	TSsmState state = TSsmState(aMainState, aSubState);
	policy->PrepareCommandList(state, 0, trs);
	TEST(trs == KRequestPending);

	// Test for cancelling the prepare command list request
	policy->PrepareCommandListCancel();
	TEST(trs.Int() == KErrCancel);
	INFO_PRINTF2(_L("Fail Policy PrepareCommandList cancelled with status : %d. expected status : -3"), trs.Int());

	// Call PrepareCommandList again to complete the preparation of the command list
	trs = KErrNone;
	policy->PrepareCommandList(state, 0, trs);
	TEST(trs == KRequestPending);
	StartScheduler();
	User::WaitForRequest(trs);
	TSsmStateName name = state.Name();
	INFO_PRINTF4(_L("PrepareCommandList() completed for State: %S with Status: %d. Expected Status: %d"), &name, trs.Int(), aExpectedResult);
	TEST( trs.Int() == aExpectedResult);
	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to test the command type of each command present in command lists for fail state policy.
*/
void CGsaFailTest::TestCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aNumSubStates)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyFail* policy = static_cast<CGsaStatePolicyFail*>(CGsaStatePolicyFail::NewL());
	CleanupStack::PushL(policy);
	policy->Initialize(trs);
	StartScheduler();
	User::WaitForRequest(trs);
	TEST( trs.Int() == KErrNone);

	// PrepareCommandList //
	trs = -1;
	TSsmState state = TSsmState(aMainState, aSubState);
	policy->PrepareCommandList(state, 0, trs);
	StartScheduler();
	User::WaitForRequest(trs);
	TEST( trs.Int() == KErrNone);

	// CommandList //
	CSsmCommandList* cmdList = policy->CommandList();
	TEST(cmdList != NULL);
	const TInt count = cmdList->Count();
	INFO_PRINTF2(_L("CommandList() has %d commands"), count);
	TEST( count == aNumSubStates);

	for (TInt i = 0; i < count ; i++)
		{
		const MSsmCommand* const command = (*cmdList)[i];
		const TSsmCommandType cmdType = static_cast<TSsmCommandType>(command->Type());
		INFO_PRINTF3(_L("Command num is : %d Command type is : %d"), i, cmdType);
		
		TEST (ArrFail[i] == cmdType);
		}
	delete cmdList;
	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to test the GetNextState for each substate within fail state.
*/
void CGsaFailTest::TestGetNextState(CGsaStatePolicyFail* aPolicy, TSsmState aCurrentTransition, TInt aError, TSsmState aResult)
	{
	TRequestStatus trs;
	TSsmState state(ESsmFail, ESsmStateFail);
	aPolicy->PrepareCommandList(state, KErrNone, trs);
	StartScheduler();
	User::WaitForRequest(trs);
	TEST( trs.Int() == KErrNone);

	TSsmState nextState;
	aPolicy->GetNextState(aCurrentTransition, KErrNone, aError, ECmdCriticalSeverity, nextState);
	INFO_PRINTF7(_L("Current State : 0x%x.0x%x Next State 0x%x.0x%x Expected Next State 0x%x.0x%x"), aCurrentTransition.MainState(), aCurrentTransition.SubState(), nextState.MainState(), nextState.SubState(), aResult.MainState(), aResult.SubState());

	TEST(nextState == aResult);
	}
