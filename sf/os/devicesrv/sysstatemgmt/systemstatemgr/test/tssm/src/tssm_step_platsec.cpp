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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <ssm/ssmstates.hrh>
#include <ssm/ssmstatemanager.h>
#include "ssmserver.h"
#include "ssmclisrv.h"
#include "tssm_step_platsec.h"

#include "tssm_ssmclient.h"
#include "tssm_startserver.h"

#include "tssm_statepolicy_capability.h"


CSsmPlatsecTest::~CSsmPlatsecTest()
	{
	}

CSsmPlatsecTest::CSsmPlatsecTest()
	{
	SetTestStepName(KTSsmPlatsecStep);
	}



TVerdict CSsmPlatsecTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0002
New Test CaseID 		DEVSRVS-SSMA-SSM-0002
 */

TVerdict CSsmPlatsecTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmPlatsecTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));
	
	INFO_PRINTF1(_L("Test to make sure SsmServer allows connection from clients without PowerMgmt"));
	RSsmStateManagerTestClient client;
	TInt connect = client.Connect();
	TEST(KErrNone == connect);	
	client.HeapMark();

	INFO_PRINTF1(_L("Test to make sure a state request transition is accepted from clients without PowerMgmt"));
	TSsmStateTransition state1(ESsmTestCapabilityState, KSsmAnySubState, ESsmTestCapabilityReasonCustomCmd);
	TRequestStatus status1;
	client.RequestStateTransition(state1, status1);						// (simple) policy returns EDefinitelyAllowed
	TEST(status1 == KRequestPending);
	User::WaitForRequest(status1);
	INFO_PRINTF2(_L("  --completion codes:  req1 (%d)"), status1.Int());
	TEST(status1.Int() == KErrNone);
	
	// The test (capability) policy will panic if it doesn't get its KErrPermissionDenied, hence next request would fail.
	INFO_PRINTF1(_L("Repeat to verify that now the new policy dll is properly loaded"));
	TSsmStateTransition state2(ESsmTestCapabilityState, KSsmAnySubState, ESsmTestCapabilityReasonCustomCmd);
	TRequestStatus status2;
	client.RequestStateTransition(state2, status2);						// (capability) policy returns ENotAllowed
	TEST(status2 == KRequestPending);
	User::WaitForRequest(status2);
	INFO_PRINTF2(_L("  --completion codes:  req2 (%d)"), status2.Int());
	TEST(status2.Int() == KErrNotSupported);

	INFO_PRINTF1(_L("Test to make sure a policy can do security checks on originator with missing capabilities"));
	TSsmStateTransition state3(ESsmTestCapabilityState, KSsmAnySubState, ESsmTestCapabilityReasonPowerMgmt);
	TRequestStatus status3;
	client.RequestStateTransition(state3, status3);						// (capability) policy returns EDefinitelyAllowed
	TEST(status3 == KRequestPending);
	User::WaitForRequest(status3);
	INFO_PRINTF2(_L("  --completion codes:  req3 (%d)"), status3.Int());
	TEST(status3.Int() == KErrNone);
	
	INFO_PRINTF1(_L("Repeat to verify that same policy dll is still loaded"));
	TSsmStateTransition state4(ESsmTestCapabilityState, KSsmAnySubState, 0);
	TRequestStatus status4;
	client.RequestStateTransition(state4, status4);						// (capability) policy returns ENotAllowed
	TEST(status4 == KRequestPending);
	User::WaitForRequest(status4);
	INFO_PRINTF2(_L("  --completion codes:  req4 (%d)"), status4.Int());
	TEST(status4.Int() == KErrNotSupported);

	// This test is (currently) only valid for state transitions, as the verification for a transition allowed is not done
	//	in the engine, but in the server before submiting it to the state transition engine!
	INFO_PRINTF1(_L("Test to make sure SsmServer handles a leaving TransitionAllowed in state policy"));
	TSsmStateTransition state5(ESsmTestCapabilityState, KSsmAnySubState, ESsmTestCapabilityReasonLeave);
	TRequestStatus status5;
	client.RequestStateTransition(state5, status5);						// (capability) policy leaves with KErrPermissionDenied
	TEST(status5 == KRequestPending);
	User::WaitForRequest(status5);
	INFO_PRINTF2(_L("  --completion codes:  req5 (%d)"), status5.Int());
	TEST(status5.Int() == KErrPermissionDenied);

	INFO_PRINTF1(_L("Repeat to verify that same policy dll is still loaded"));
	TSsmStateTransition state6(ESsmTestCapabilityState, KSsmAnySubState, 0);
	TRequestStatus status6;
	client.RequestStateTransition(state6, status6);						// (capability) policy returns ENotAllowed
	TEST(status6 == KRequestPending);
	User::WaitForRequest(status6);
	INFO_PRINTF2(_L("  --completion codes:  req6 (%d)"), status6.Int());
	TEST(status6.Int() == KErrNotSupported);

	// Clean-up
	INFO_PRINTF1(_L("cleaning up"));
	client.HeapMarkEnd(EDoCleanup);	//Pass EDoCleanup for cleaning up Transition Engine after the test case 
	client.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("....CSsmPlatsecTest completed!"));
	return TestStepResult();
	}

/** */
TVerdict CSsmPlatsecTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

