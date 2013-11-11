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

#include "tgsa_step_startup.h"
#include "gsastatepolicystartup.h"
#include <ssm/ssmstatetransition.h>
#include "ssmdebug.h"
#include <e32uid.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmcommand.h>
#include <ssmsubstateext.hrh>


#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
TSsmCommandType ArrCriticalStartUp[] = {ESsmCmdPublishSystemState,  //r_cmd_publishstate
                                        ESsmCmdStartProcess,        //r_cmd_sysagt
                                        ESsmCmdCustomCommand,       //r_cmd_initpskeys
                                        ESsmCmdCustomCommand,       //r_cmd_checkuserdrive
                                        ESsmCmdCustomCommand,       //r_cmd_deltempfiles
                                        ESsmCmdStartProcess,        //r_cmd_reservedisk
                                        ESsmCmdStartProcess,        //r_cmd_sysmon
                                        ESsmCmdStartProcess,        //r_cmd_ssmutilsrv
                                        ESsmCmdMultipleWait         //r_cmd_multiwaitforever
                                        };
#else
TSsmCommandType ArrCriticalStartUp[] = { ESsmCmdCustomCommand,ESsmCmdPublishSystemState,ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess,ESsmCmdStartProcess, ESsmCmdSetPAndSKey, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess };
#endif
TSsmCommandType ArrDynamicStartUp[] = {ESsmCmdPublishSystemState, ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartApp, ESsmCmdMultipleWait, ESsmCmdSetPAndSKey};
TSsmCommandType ArrNetworkingStartUp[] = { ESsmCmdPublishSystemState,ESsmCmdStartProcess, ESsmCmdStartProcess, ESsmCmdStartProcess };
#ifdef TEST_SSM_GRACEFUL_OFFLINE
TSsmCommandType ArrNonCriticalStartUp[] = { ESsmCmdSetPAndSKey, ESsmCmdSetPAndSKey,ESsmCmdSetPAndSKey,ESsmCmdPublishSystemState ,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdStartProcess,ESsmCmdAMAStarter};
#else
TSsmCommandType ArrNonCriticalStartUp[] = { ESsmCmdCustomCommand,       //r_cmd_sastate
                                            ESsmCmdPublishSystemState,  //r_cmd_publishstate
                                            ESsmCmdSetPAndSKey,         //r_cmd_psstate
                                            ESsmCmdCustomCommand,       //r_cmd_startupmode
                                            ESsmCmdLoadSup,             //r_cmd_loadpowersup
                                            ESsmCmdCustomCommand,       //r_cmd_rfsfirstboot
                                            ESsmCmdCustomCommand,       //r_cmd_rfsdeep
                                            ESsmCmdCustomCommand,       //r_cmd_rfsnormal
                                            ESsmCmdCustomCommand,       //r_cmd_initclkeys
                                            ESsmCmdCustomCommand,       //r_cmd_initramdrive
                                            ESsmCmdStartProcess,        //r_cmd_wserv
                                            ESsmCmdStartProcess,        //r_cmd_splash
                                            ESsmCmdStartProcess,        //r_cmd_tzsrv
                                            ESsmCmdStartProcess,        //r_cmd_mediator
                                            ESsmCmdStartProcess,        //r_cmd_hwrmsrv
                                            ESsmCmdCustomCommand,       //r_cmd_rtc
                                            ESsmCmdMultipleWait,        //r_cmd_multiwaitforever1
                                            ESsmCmdStartProcess,        //r_cmd_dbrecovery
                                            ESsmCmdStartProcess,        //r_cmd_accsrv
                                            ESsmCmdCustomCommand,       //r_cmd_selectlanguage
                                            ESsmCmdMultipleWait,        //r_cmd_multiwaitforever2
                                            ESsmCmdLoadSup,             //r_cmd_loadlocalesup
                                            ESsmCmdLoadSup,             //r_cmd_loadcollation
                                            ESsmCmdLoadSup,             //r_cmd_loadregion
                                            ESsmCmdLoadSup,             //r_cmd_loaduilanguage
                                            ESsmCmdCustomCommand,       //r_cmd_aknstart
                                            ESsmCmdStartProcess,        //r_cmd_devicedialog
                                            ESsmCmdStartProcess,         //r_cmd_hbthemeserver
                                            ESsmCmdWaitForApparcInit,   //r_cmd_apparc_init
                                            ESsmCmdCustomCommand        //r_cmd_rfspostui
                                            };
#endif
TSsmCommandType ArrCriticalAppsStartUp[] = {ESsmCmdCustomCommand,       //r_cmd_sastate
                                            ESsmCmdPublishSystemState,  //r_cmd_publishstate
                                            ESsmCmdSetPAndSKey,         //r_cmd_psstate
                                            ESsmCmdCustomCommand,       //r_cmd_selftest
                                            ESsmCmdStartProcess,        //r_cmd_cfserver
                                            ESsmCmdStartApp,            //r_cmd_sysap
                                            ESsmCmdStartProcess,        //r_cmd_profmon
                                            ESsmCmdMultipleWait         //r_cmd_multiwaitforever
                                            };

CGsaStartupTest::~CGsaStartupTest()
	{
	}

CGsaStartupTest::CGsaStartupTest() : CGsaTestBase(KTGsaStartupStep)
	{
	}

TVerdict CGsaStartupTest::doTestStepL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestStepL"));
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
Old Test CaseID 		AFSS-GSA-0010
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */

void CGsaStartupTest::doTestNewL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestNewL"));

	TInt expectedErr = KErrNone;
	MSsmStatePolicy* policy = NULL;
	//test should pass
	TRAPD(err, policy = CGsaStatePolicyStartup::NewL());
	INFO_PRINTF3(_L("Test completed with err : %d. expected err : %d"), err, expectedErr);
	TEST(err == expectedErr);
	TEST(policy != NULL);
	policy->Release();
	}

/**
Old Test CaseID 		AFSS-GSA-0011
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */

void CGsaStartupTest::doTestInitializeL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestInitializeL"));
	CGsaStatePolicyStartup* policy = CreateAndInitializeStartUpPolicyLC();
	CleanupStack::PopAndDestroy(policy);
	}

/**
Old Test CaseID 		AFSS-GSA-0012
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */
 void CGsaStartupTest::doTestPrepareCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestPrepareCommandListL"));
	TestPrepareCommandListL(ESsmStartup ,ESsmStartupSubStateCriticalStatic, KErrNone);
	TestPrepareCommandListL(ESsmStartup ,KSsmAnySubState, KErrNone);
	TestPrepareCommandListL(ESsmStartup ,ESsmStartupSubStateNonCritical, KErrNone);
	TestPrepareCommandListL(ESsmStartup ,ESsmStateStartingCriticalApps, KErrNone); //ESsmStateStartingCriticalApps = 0x31
	TestPrepareCommandListL(ESsmStartup ,100, KErrNotFound);
	}

/**
Old Test CaseID 		AFSS-GSA-0013
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */

void CGsaStartupTest::doTestCommandListL()
	{
	INFO_PRINTF1(_L("> CGsaStartupTest::doTestCommandListL"));

	TestCommandListL(ESsmStartup, ESsmStartupSubStateCriticalStatic, sizeof(ArrCriticalStartUp) / sizeof(ArrCriticalStartUp[0]));
	TestCommandListL(ESsmStartup, ESsmStartupSubStateNonCritical, sizeof(ArrNonCriticalStartUp) / sizeof(ArrNonCriticalStartUp[0]));
	TestCommandListL(ESsmStartup, ESsmStateStartingCriticalApps, sizeof(ArrCriticalAppsStartUp) / sizeof(ArrCriticalAppsStartUp[0]));	
	}

/**
Old Test CaseID 		AFSS-GSA-0014
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0010
 */


void CGsaStartupTest::doTestGetNextStateL()
	{
	CGsaStatePolicyStartup* policy = CreateAndInitializeStartUpPolicyLC();

	TSsmState state(ESsmStartup, KSsmAnySubState);
	TSsmState result(ESsmStartup, ESsmStartupSubStateNonCritical);
	// KSsmAnySubState (KSsmAnySubState) will return ESsmStartupSubStateNonCritical only for the first time, 
	// next call will always return the next sub state for transition.
	TestGetNextState(policy, state, KErrNone, result);

	TSsmState firstState(ESsmStartup, ESsmStartupSubStateCriticalStatic);
	TSsmState firstResult(ESsmStartup, ESsmStartupSubStateNonCritical);
	TestGetNextState(policy, firstState, KErrNone, firstResult);

	TSsmState secondState(ESsmStartup, ESsmStartupSubStateNonCritical);
	TSsmState secondResult(ESsmStartup, ESsmStateStartingCriticalApps);
	TestGetNextState(policy, secondState, KErrNone, secondResult);

	TSsmState thirdState(ESsmStartup, ESsmStateStartingCriticalApps);
	TSsmState thirdResult(ESsmStartup, ESsmStateSelfTestOK);
	TestGetNextState(policy, thirdState, KErrNone, thirdResult);

	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to create and initialize start-up state policy.
*/
CGsaStatePolicyStartup* CGsaStartupTest::CreateAndInitializeStartUpPolicyLC()
	{
	TRequestStatus trs;
	TRequestStatus expectedTrs = KErrNone;
	CGsaStatePolicyStartup* self = static_cast<CGsaStatePolicyStartup*>(CGsaStatePolicyStartup::NewL());
	CleanupStack::PushL(self);
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	// Test for cancelling the initialize request
	self->InitializeCancel();
	expectedTrs = KErrCancel;
	TEST(trs.Int() == expectedTrs.Int());
	INFO_PRINTF3(_L("Startup Policy Initialization cancelled with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());

	// Call Initialize again to complete the intialization of the command list
	trs = KErrNone;
	self->Initialize(trs);
	TEST(trs == KRequestPending);

	StartScheduler();
	User::WaitForRequest(trs);
	expectedTrs = KErrNone;
	INFO_PRINTF3(_L("StartUp Policy Initialized with status : %d. expected status : %d"), trs.Int(), expectedTrs.Int());
	TEST( trs.Int() == expectedTrs.Int());

	return self;
	}

/**
 Helper function to test preparation of command lists for start-up state policy.
*/
void CGsaStartupTest::TestPrepareCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aExpectedResult)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyStartup* policy = static_cast<CGsaStatePolicyStartup*>(CGsaStatePolicyStartup::NewL());
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
	INFO_PRINTF2(_L("Startup Policy PrepareCommandList cancelled with status : %d. expected status : -3"), trs.Int());

	// Call PrepareCommandList again to complete the preparation of the command list
	trs = KErrNone;
	policy->PrepareCommandList(state, 0, trs);
	TEST(trs == KRequestPending);

	StartScheduler();
	User::WaitForRequest(trs);

	INFO_PRINTF4(_L("PrepareCommandList() completed for Sub State: 0x%x with Status: %d. Expected Status: %d"), aSubState, trs.Int(), aExpectedResult);
	TEST( trs.Int() == aExpectedResult);
	CleanupStack::PopAndDestroy(policy);
	}

/**
 Helper function to test the command type of each command present in command lists for start-up state policy.
*/
void CGsaStartupTest::TestCommandListL(TUint16 aMainState, TUint16 aSubState, TInt aNumSubStates)
	{
	// Initialize //
	TRequestStatus trs;
	CGsaStatePolicyStartup* policy = static_cast<CGsaStatePolicyStartup*>(CGsaStatePolicyStartup::NewL());
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
		INFO_PRINTF3(_L("command number is : %d command type is : %d"), i, cmdType);
		
		switch (aSubState)
			{
			case ESsmStartupSubStateCriticalStatic:
				{
				TEST (ArrCriticalStartUp[i] == cmdType);
				break;
				}
			case KSsmAnySubState:
				{
				TEST (ArrCriticalStartUp[i] == cmdType);
				break;
				}
			case ESsmStartupSubStateCriticalDynamic:
				{
				TEST (ArrDynamicStartUp[i] == cmdType);
				break;
				}
			case ESsmStartupSubStateNetworkingCritical:
				{
				TEST (ArrNetworkingStartUp[i] == cmdType);
				break;
				}
			case ESsmStartupSubStateNonCritical:
				{
				TEST (ArrNonCriticalStartUp[i] == cmdType);
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
 Helper function to test the GetNextState for each substate within start-up state.
*/
void CGsaStartupTest::TestGetNextState(CGsaStatePolicyStartup* aPolicy, TSsmState aCurrentTransition, TInt aError, TSsmState aResult)
	{
	TRequestStatus trs;
	TSsmState state(ESsmStartup, KSsmAnySubState);
	aPolicy->PrepareCommandList(aCurrentTransition, KErrNone, trs);
	StartScheduler();
	User::WaitForRequest(trs);

	TSsmState nextState;
	TBool res = aPolicy->GetNextState(aCurrentTransition, KErrNone, aError, ECmdCriticalSeverity, nextState);
	
	TEST(res);	
	
	INFO_PRINTF7(_L("Current State : 0x%x.0x%x Next State 0x%x.0x%x Expected Next State 0x%x.0x%x"), aCurrentTransition.MainState(), aCurrentTransition.SubState(), nextState.MainState(), nextState.SubState(), aResult.MainState(), aResult.SubState());

	TEST(nextState == aResult);
	}

