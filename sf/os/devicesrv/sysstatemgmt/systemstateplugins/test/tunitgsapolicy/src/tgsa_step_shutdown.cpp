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

#include "gsastatepolicyshutdown.h"
#include <ssm/ssmstatetransition.h>
#include "ssmdebug.h"
#include <e32uid.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmcommand.h>
#include <ssmsubstateext.hrh> 

#include "tgsa_step_shutdown.h"

TSsmCommandType ArrCriticalShutdown[] = {   ESsmCmdCustomCommand,       //r_cmd_sastate
                                            ESsmCmdPublishSystemState,  //r_cmd_publishstate
                                            ESsmCmdSetPAndSKey,         //r_cmd_psstate
                                            ESsmCmdCustomCommand,       //r_cmd_cancelmonitoring
                                            ESsmCmdCustomCommand        //r_cmd_shutdownapps
                                            };
TSsmCommandType ArrNonCriticalShutdown[] = {ESsmCmdPublishSystemState,  //r_cmd_publishstate 
                                            ESsmCmdSetPAndSKey,         //r_cmd_killsplash
                                            ESsmCmdCustomCommand,       //r_cmd_sashutdown
                                            ESsmCmdFinaliseDrives       //r_cmd_finalisedrives
                                            };
TSsmCommandType ArrPowerOffShutdown[] = {   ESsmCmdPublishSystemState,  //r_cmd_publishstate
                                            ESsmCmdPowerOff             //r_cmd_poweroff 
                                            };

const TUint16 KUnknownState = 100;

CGsaShutdownTest::~CGsaShutdownTest()
	{
	}

CGsaShutdownTest::CGsaShutdownTest() :  CGsaTestBase(KTGsaShutdownStep)
	{
	}

TVerdict CGsaShutdownTest::doTestStepL()
	{
	INFO_PRINTF1(_L("> CGsaShutdownTest::doTestStepL"));
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
Old Test CaseID 		AFSS-GSA-0020
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0020
 */

void CGsaShutdownTest::doTestNewL()
	{
	INFO_PRINTF1(_L("> CGsaShutdownTest::doTestNewL"));
	
	TInt expectedErr = KErrNone;
	MSsmStatePolicy* policy = NULL;
	//test should pass
	TRAPD(err, policy = CGsaStatePolicyShutdown::NewL());
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	TEST(policy != NULL);
	policy->Release();
	}

/**
Old Test CaseID 		AFSS-GSA-0021
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0020
 */

void CGsaShutdownTest::doTestInitializeL()
	{
	INFO_PRINTF1(_L("> CGsaShutdownTest::doTestInitializeL"));
	CGsaStatePolicyShutdown* policy = CreateAndInitializeShutdownPolicyLC();
	CleanupStack::PopAndDestroy(policy);
	}

/**
Old Test CaseID 		AFSS-GSA-0022
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0020
 */


 void CGsaShutdownTest::doTestPrepareCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaShutdownTest::doTestPrepareCommandListL"));
	TestPrepareCommandListL(ESsmShutdown ,ESsmShutdownSubStateCritical, KErrNone);
	TestPrepareCommandListL(ESsmShutdown ,KSsmAnySubState, KErrNone);
	TestPrepareCommandListL(ESsmShutdown ,ESsmShutdownSubStateNonCritical, KErrNone);
	TestPrepareCommandListL(ESsmShutdown ,ESsmShutdownSubStatePowerOff, KErrNone);
	TestPrepareCommandListL(ESsmShutdown ,100, KErrNotFound);
	}

/**
Old Test CaseID 		AFSS-GSA-0023
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0020
 */
void CGsaShutdownTest::doTestCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaShutdownTest::doTestCommandListL"));

	TestCommandListL(ESsmShutdown, ESsmShutdownSubStateCritical, (sizeof(ArrCriticalShutdown)/sizeof(ArrCriticalShutdown[0])));
	// KSsmAnySubState
	TestCommandListL(ESsmShutdown, KSsmAnySubState, (sizeof(ArrCriticalShutdown)/sizeof(ArrCriticalShutdown[0])));
	TestCommandListL(ESsmShutdown, ESsmShutdownSubStateNonCritical, (sizeof(ArrNonCriticalShutdown)/sizeof(ArrNonCriticalShutdown[0])));
	TestCommandListL(ESsmShutdown, ESsmShutdownSubStatePowerOff, (sizeof(ArrPowerOffShutdown)/sizeof(ArrPowerOffShutdown[0])));
	}

/**
Old Test CaseID 		AFSS-GSA-0024
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0020
 */

void CGsaShutdownTest::doTestGetNextStateL()
	{
	CGsaStatePolicyShutdown* policy = CreateAndInitializeShutdownPolicyLC();

	TSsmState state(ESsmShutdown, KSsmAnySubState);
	TSsmState result(ESsmShutdown, ESsmShutdownSubStateNonCritical);
	// KSsmAnySubState (0xffff) will return ESsmShutdownSubStateNonCritical only for the first time, 
	// next call will always return the next sub state for transition.
	TestGetNextState(policy, state, KErrNone, result);

	TSsmState firstState(ESsmShutdown, ESsmShutdownSubStateCritical);
	TSsmState firstResult(ESsmShutdown, ESsmShutdownSubStateNonCritical);
	TestGetNextState(policy, firstState, KErrNone, firstResult);

	TSsmState secondState(ESsmShutdown, ESsmShutdownSubStateNonCritical);
	TSsmState secondResult(ESsmShutdown, ESsmShutdownSubStatePowerOff);
	TestGetNextState(policy, secondState, KErrNone, secondResult);

	TSsmState thirdState(ESsmShutdown, ESsmShutdownSubStatePowerOff);
	TSsmState thirdResult(KErrNone, KErrNone);
	TestGetNextState(policy, thirdState, KErrNone, thirdResult);

	TSsmState unknownState(ESsmShutdown, KUnknownState);
	TSsmState nullResult(0, 0);
	TestGetNextState(policy, unknownState, KErrNone, nullResult);

	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to create and initialize shutdown state policy.
*/
CGsaStatePolicyShutdown* CGsaShutdownTest::CreateAndInitializeShutdownPolicyLC()
	{
	TRequestStatus trs;
	TRequestStatus expectedTrs = KErrNone;
	CGsaStatePolicyShutdown* self = static_cast<CGsaStatePolicyShutdown*>(CGsaStatePolicyShutdown::NewL());
	CleanupStack::PushL(self);
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	// Test for cancelling the initialize request
	self->InitializeCancel();
	expectedTrs = KErrCancel;
	TEST(trs.Int() == expectedTrs.Int());
	INFO_PRINTF3(_L("Shutdown Policy Initialization cancelled with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());

	// Call Initialize again to complete the intialization of the command list
	trs = KErrNone;
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	StartScheduler();
	User::WaitForRequest(trs);
	expectedTrs = KErrNone;
	INFO_PRINTF3(_L("Shutdown Policy Initialized with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());
	TEST( trs.Int() == expectedTrs.Int());

	return self;
	}

/**
 Helper function to test preparation of command lists for shutdown state policy.
*/
void CGsaShutdownTest::TestPrepareCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aExpectedResult)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyShutdown* policy = static_cast<CGsaStatePolicyShutdown*>(CGsaStatePolicyShutdown::NewL());
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
	INFO_PRINTF2(_L("Shutdown Policy PrepareCommandList cancelled with status : %d. expected status : -3"), trs.Int());

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
 Helper function to test the command type of each command present in command lists for shutdown state policy.
*/
void CGsaShutdownTest::TestCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aNumSubStates)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyShutdown* policy = static_cast<CGsaStatePolicyShutdown*>(CGsaStatePolicyShutdown::NewL());
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
		
		switch (aSubState)
			{
			case ESsmShutdownSubStateCritical:
				{
				TEST (ArrCriticalShutdown[i] == cmdType);
				break;
				}
			case KSsmAnySubState:
				{
				TEST (ArrCriticalShutdown[i] == cmdType);
				break;
				}
			case ESsmShutdownSubStateNonCritical:
				{
				TEST (ArrNonCriticalShutdown[i] == cmdType);
				break;
				}
			case ESsmShutdownSubStatePowerOff:
				{
				TEST (ArrPowerOffShutdown[i] == cmdType);
				break;
				}
			default:
				{
				break;
				}
			}
		}
	delete cmdList;
	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to test the GetNextState for each substate within shutdown state.
*/
void CGsaShutdownTest::TestGetNextState(CGsaStatePolicyShutdown* aPolicy, TSsmState aCurrentTransition, TInt aError, TSsmState aResult)
	{
	TRequestStatus trs;
	TSsmState state(ESsmShutdown, KSsmAnySubState);
	//aPolicy->PrepareCommandList(state, KErrNone, trs);
	aPolicy->PrepareCommandList(aCurrentTransition, aError, trs);
	
	StartScheduler();
	User::WaitForRequest(trs);
	//TEST( trs.Int() == KErrNone);

	TSsmState nextState;
	TBool res = aPolicy->GetNextState(aCurrentTransition, KErrNone, aError, ECmdCriticalSeverity, nextState);
	if((aCurrentTransition.SubState() == ESsmShutdownSubStatePowerOff) || (aCurrentTransition.SubState() == KUnknownState))
		{
		TEST(res == EFalse);
		}
	else
		{
		TEST(res);
		}
	
	INFO_PRINTF7(_L("Current State : 0x%x.0x%x Next State 0x%x.0x%x Expected Next State 0x%x.0x%x"), aCurrentTransition.MainState(), aCurrentTransition.SubState(), nextState.MainState(), nextState.SubState(), aResult.MainState(), aResult.SubState());

	TEST(nextState == aResult);
	}

