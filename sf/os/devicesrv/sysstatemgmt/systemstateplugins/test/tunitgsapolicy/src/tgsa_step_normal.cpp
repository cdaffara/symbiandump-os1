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

#include "gsastatepolicynormal.h"
#include <ssm/ssmstatetransition.h>
#include "ssmdebug.h"
#include <e32uid.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmcommand.h>

#include "tgsa_step_normal.h"

TSsmCommandType ArrNormal[] = { ESsmCmdCustomCommand, ESsmCmdPublishSystemState };


CGsaNormalTest::~CGsaNormalTest()
	{
	}

CGsaNormalTest::CGsaNormalTest() : CGsaTestBase(KTGsaNormalStep)
	{
	}

TVerdict CGsaNormalTest::doTestStepL()
	{
	INFO_PRINTF1(_L("> CGsaNormalTest::doTestStepL"));
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
Old Test CaseID 		AFSS-GSA-0015
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0015
 */

void CGsaNormalTest::doTestNewL()
	{
	INFO_PRINTF1(_L("> CGsaNormalTest::doTestNewL"));
	
	TInt expectedErr = KErrNone;
	MSsmStatePolicy* policy = NULL;
	//test should pass
	TRAPD(err, policy = CGsaStatePolicyNormal::NewL());
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	TEST(policy != NULL);
	policy->Release();
	}

/**
Old Test CaseID 		AFSS-GSA-0016
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0015
 */
void CGsaNormalTest::doTestInitializeL()
	{
	INFO_PRINTF1(_L("> CGsaNormalTest::doTestInitializeL"));
	CGsaStatePolicyNormal* policy = CreateAndInitializeNormalPolicyLC();
	CleanupStack::PopAndDestroy(policy);
	}

/**
Old Test CaseID 		AFSS-GSA-0017
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0015
 */


 void CGsaNormalTest::doTestPrepareCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaNormalTest::doTestPrepareCommandListL"));
	TestPrepareCommandListL(ESsmNormal ,ESsmNormalSubState, KErrNone);
	TestPrepareCommandListL(ESsmNormal ,KSsmAnySubState, KErrNone);
	TestPrepareCommandListL(ESsmNormal ,100, KErrNotFound);
	}

/**
Old Test CaseID 		AFSS-GSA-0018
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0015
 */

void CGsaNormalTest::doTestCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaNormalTest::doTestCommandListL"));

	TestCommandListL(ESsmNormal, ESsmNormalSubState, (sizeof(ArrNormal) / sizeof(ArrNormal[0])));
	TestCommandListL(ESsmNormal, KSsmAnySubState, (sizeof(ArrNormal) / sizeof(ArrNormal[0])));
	}

/**
Old Test CaseID 		AFSS-GSA-0019
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0015
 */

void CGsaNormalTest::doTestGetNextStateL()
	{
	CGsaStatePolicyNormal* policy = CreateAndInitializeNormalPolicyLC();

	TSsmState state(ESsmNormal, KSsmAnySubState);
	TSsmState result;		
#ifdef TEST_SSM_GRACEFUL_OFFLINE
    //Further substates(RF ON/OFF) transitions are valid in Normal state when patchable constant
    //KSsmGracefulOffline is enabled
	result.Set(ESsmNormal,ESsmNormalRfOnSubState);

#else
	result.Set(0,0);
#endif	
	TestGetNextState(policy, state, KErrNone, result);
	
	//Set the expected result
	result.Set(0,0);
	TSsmState firstState(ESsmNormal, ESsmNormalSubState);
	TestGetNextState(policy, firstState, KErrNone, result);

	TSsmState unknownState(ESsmNormal, 100);
	TestGetNextState(policy, unknownState, KErrNone, result);

	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to create and initialize normal state policy.
*/
CGsaStatePolicyNormal* CGsaNormalTest::CreateAndInitializeNormalPolicyLC()
	{
	TRequestStatus trs;
	TRequestStatus expectedTrs = KErrNone;
	CGsaStatePolicyNormal* self = static_cast<CGsaStatePolicyNormal*>(CGsaStatePolicyNormal::NewL());
	CleanupStack::PushL(self);

	self->Initialize(trs);
	TEST(trs == KRequestPending);

	// Test for cancelling the initialize request
	self->InitializeCancel();
	expectedTrs = KErrCancel;
	TEST(trs.Int() == expectedTrs.Int());
	INFO_PRINTF3(_L("Normal Policy Initialization cancelled with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());

	// Call Initialize again to complete the intialization of the command list
	trs = KErrNone;
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	StartScheduler();
	User::WaitForRequest(trs);
	expectedTrs = KErrNone;
	INFO_PRINTF3(_L("Normal Policy Initialized with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());
	TEST( trs.Int() == expectedTrs.Int());

	return self;
	}

/**
 Helper function to test preparation of command lists for normal state policy.
*/
void CGsaNormalTest::TestPrepareCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aExpectedResult)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyNormal* policy = static_cast<CGsaStatePolicyNormal*>(CGsaStatePolicyNormal::NewL());
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
	INFO_PRINTF2(_L("Normal Policy PrepareCommandList cancelled with status : %d. expected status : -3"), trs.Int());

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
 Helper function to test the command type of each command present in command lists for normal state policy.
*/
void CGsaNormalTest::TestCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aNumSubStates)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyNormal* policy = static_cast<CGsaStatePolicyNormal*>(CGsaStatePolicyNormal::NewL());
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
		
		TEST (ArrNormal[i] == cmdType);
		}
	delete cmdList;
	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to test the GetNextState for each substate within normal state.
*/
void CGsaNormalTest::TestGetNextState(CGsaStatePolicyNormal* aPolicy, TSsmState aCurrentTransition, TInt aError, TSsmState aResult)
	{
	TRequestStatus trs;
	TSsmState state(ESsmNormal, KSsmAnySubState);
	//aPolicy->PrepareCommandList(state, KErrNone, trs);
	aPolicy->PrepareCommandList(aCurrentTransition, aError, trs);
	
	StartScheduler();
	User::WaitForRequest(trs);
//	TEST( trs.Int() == KErrNone);

	TSsmState nextState;
	TInt ret = aPolicy->GetNextState(aCurrentTransition, KErrNone, aError, ECmdCriticalSeverity, nextState);

	//Further substates(RF ON/OFF) transitions are valid in Normal state when patchable constant
	//KSsmGracefulOffline is enabled. 
#ifdef TEST_SSM_GRACEFUL_OFFLINE
	if (nextState.SubState() == ESsmNormalRfOnSubState || nextState.SubState() == ESsmNormalRfOffSubState )
	    {	    
	    TEST( ret == 1);
	    }
	else
	    {
	    TEST( ret == KErrNone);
	    }
#else
	TEST( ret == KErrNone);
#endif //TEST_SSM_GRACEFUL_OFFLINE
	INFO_PRINTF7(_L("Current State : 0x%x.0x%x Next State 0x%x.0x%x Expected Next State 0x%x.0x%x"), aCurrentTransition.MainState(), aCurrentTransition.SubState(), nextState.MainState(), nextState.SubState(), aResult.MainState(), aResult.SubState());

	TEST(nextState == aResult);
	}

