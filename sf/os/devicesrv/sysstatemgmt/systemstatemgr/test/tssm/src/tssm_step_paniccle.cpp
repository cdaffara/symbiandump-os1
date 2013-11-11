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

#include <e32property.h>
#include <ssm/ssmstates.hrh>
#include <ssm/ssmstatemanager.h>
#include "ssmserver.h"
#include "ssmclisrv.h"

#include "ssmatest_utils.h"
#include "tssm_ssmclient.h"
#include "tssm_startserver.h"
#include "tssm_statepolicy_simple.h"
#include "tssm_statepolicy_paniccle.h"
#include "tssm_swppolicy_common.h"
#include "tssm_swppolicy_paniccle.h"

#include "tssm_step_paniccle.h"


//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)
const TUid KPropertyCategory=KMySID;

// property uids should be real uids (use these temporarily for test purposes)
const TUint KTestSwp1={0x00B0BA};




//---------------- Test Steps ---------------------------------------


void CSsmPanicCleTest::DoSwpRequestPanicsCleL()
	{
	INFO_PRINTF1(_L("Doing multiple async RequestSwpTransition that will cause the Cle to panic"));

	RSsmStateManagerTestClient client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);
	client.HeapMark();
		
	// Create swps
	INFO_PRINTF1(_L("Creating swp..."));
	TSsmSwp swp1(KTestSwp1, 11);
	RProperty property1;
	TEST(KErrNone == property1.Define(KPropertyCategory, swp1.Key(), RProperty::EInt));
	TEST(KErrNone == property1.Set(KPropertyCategory, swp1.Key(), swp1.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(11 == value, value);

	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	INFO_PRINTF1(_L("Registering swp..."));
	TEST(KErrNone == client.RegisterSwpMapping(KTestSwp1, KTestSwpPolicyPanicCleFile));
	
	//Let client request swp changes
	INFO_PRINTF1(_L("Request several swp changes (some panic and some complete normally)..."));
	swp1.Set(KTestSwp1, -99);
	TRequestStatus status1;
	client.RequestSwpChange(swp1, status1);				// panics
	TEST(status1 == KRequestPending);

	swp1.Set(KTestSwp1, 44);
	TRequestStatus status2;
	client.RequestSwpChange(swp1, status2);				// panics
	TEST(status2 == KRequestPending);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	
	INFO_PRINTF3(_L("  --completion codes:  req1 (%d), req2 (%d)"), status1.Int(), status2.Int());
	TEST(status1.Int() == KErrNone);	// the panic is after allowing the swp, hence no error is ever returned at this point
	TEST(status2.Int() == KErrNone);
	
	User::After(500000); // Wait for 0.5sec to allow transitions to fully complete

	//Ensure all properties were properly updated
	value = 0;

	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(44 == value, value);

	// clean-up
	TEST(KErrNone == property1.Delete(KPropertyCategory, swp1.Key()));
	property1.Close();
	client.RequestDeRegisterSwpMappingL(KTestSwp1,KTestSwpPolicyPanicCleFile);
	client.HeapMarkEnd();
	client.Close();
	INFO_PRINTF1(_L("case completed\n"));
	}


void CSsmPanicCleTest::DoStateRequestPanicsCle()
	{
	INFO_PRINTF1(_L("Doing multiple async RequestStateTransition that will cause the Cle to panic"));

	RSsmStateManagerTestClient client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);
	client.HeapMark();
		
	// Request state transitions in the same session
	INFO_PRINTF1(_L("Request state transition to state that will panic Cle followed by valid transition...")); 
	TSsmStateTransition state1(ESsmTestPanicCleState,123,0);
	TRequestStatus status1;
	client.RequestStateTransition(state1, status1);						// (simple) policy returns EDefinitelyAllowed
	TEST(status1 == KRequestPending);
	
	TSsmStateTransition state2(ESsmTestSimpleState,321,0);
	TRequestStatus status2;
	client.RequestStateTransition(state2, status2);						// (simple) policy returns EDefinitelyAllowed
	TEST(status2 == KRequestPending);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	INFO_PRINTF3(_L("  --completion codes:  req1 (%d), req2 (%d)"), status1.Int(), status2.Int());
	TEST(status1.Int() == KErrNone);
	TEST(status2.Int() == KErrNone);

	User::After(500000); // Wait for 0.5sec to allow transitions to fully complete

	INFO_PRINTF1(_L("Request two state transitions to states that will panic Cle...")); 
	TSsmStateTransition state3(ESsmTestPanicCleState,456,0);
	TRequestStatus status3;
	client.RequestStateTransition(state3, status3);						// (simple) policy returns EDefinitelyAllowed
	TEST(status3 == KRequestPending);
	
	TSsmStateTransition state4(ESsmTestPanicCleState,654,0);
	TRequestStatus status4;
	client.RequestStateTransition(state4, status4);						// (simple) policy returns EDefinitelyAllowed
	TEST(status4 == KRequestPending);
	
	User::WaitForRequest(status3);
	User::WaitForRequest(status4);
	INFO_PRINTF3(_L("  --completion codes:  req3 (%d), req4 (%d)"), status3.Int(), status4.Int());
	TEST(status3.Int() == KErrNone);
	TEST(status4.Int() == KErrNone);

	User::After(500000); // Wait for 0.5sec to allow transitions to fully complete
	
	INFO_PRINTF1(_L("Request vaild state transition followed by a request to state that will panic Cle...")); 
	TSsmStateTransition state5(ESsmTestSimpleState,789,ESsmTestPanicCleGoodReason);
	TRequestStatus status5;
	client.RequestStateTransition(state5, status5);						// (paniccle) policy returns EDefinitelyAllowed
	TEST(status5 == KRequestPending);

	TSsmStateTransition state6(ESsmTestPanicCleState,987,ESsmTestPanicCleGoodReason);
	TRequestStatus status6;
	client.RequestStateTransition(state6, status6);						// (paniccle) policy returns EDefinitelyAllowed
	TEST(status6 == KRequestPending);

	User::WaitForRequest(status5);
	User::WaitForRequest(status6);
	INFO_PRINTF3(_L("  --completion codes:  req5 (%d), req6 (%d)"), status5.Int(), status6.Int());
	TEST(status5.Int() == KErrNone);
	TEST(status6.Int() == KErrNone);
	
	User::After(500000);	// Wait for 0.5sec to allow transitions to fully complete
	
	client.HeapMarkEnd(ETrue);		//Pass ETrue for cleaning up Transition Engine after the test case 
	// clean-up
	client.Close();
	INFO_PRINTF1(_L("case completed\n"));
	}


void CSsmPanicCleTest::DoCleSrvRecovery()
	{
	INFO_PRINTF1(_L("Doing hack to recover original CleSrv that we panic'ed in this test..."));

	// Note that we need to use the production SsmServer, not the test one
	RSsmStateManager client;
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);

	// Create swps
	TSsmSwp swp(KTestSwp1, 1);
	RProperty property;
	TEST(KErrNone == property.Define(KPropertyCategory, swp.Key(), RProperty::EInt));
	TEST(KErrNone == property.Set(KPropertyCategory, swp.Key(), swp.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	TESTE(1 == value, value);

	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	TEST(KErrNone == client.RegisterSwpMapping(KTestSwp1, KTestSwpPolicyCommonFile));
	
	//Let client request swp changes
	swp.Set(KTestSwp1, 2);
	TRequestStatus status;
	client.RequestSwpChange(swp, status);
	TEST(status == KRequestPending);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	User::After(500000); // Wait for 0.5sec to allow transitions to fully complete

	//Ensure all properties were properly updated
	value = 0;
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	TESTE(2 == value, value);

	// clean-up
	TEST(KErrNone == property.Delete(KPropertyCategory, swp.Key()));
	property.Close();
	client.Close();
	}



//---------------- CSsmPanicCleTest step ---------------------------------------


CSsmPanicCleTest::~CSsmPanicCleTest()
	{
	}

CSsmPanicCleTest::CSsmPanicCleTest()
	{
	SetTestStepName(KTSsmPanicCleStep);
	}

TVerdict CSsmPanicCleTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmPanicCleTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0017
New Test CaseID 		DEVSRVS-SSMA-SSM-0017
 */


TVerdict CSsmPanicCleTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmPanicCleTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	TEST(KErrNone == StartServer(thread));	

	//perform state tests
	DoStateRequestPanicsCle();

	//perform swp tests
	DoSwpRequestPanicsCleL();
	
	// clean-up
	INFO_PRINTF1(_L("cleaning up"));
	thread.Kill(KErrNone);
	thread.Close();
	__UHEAP_MARKEND;
	
	// this test raises panics due to negative testing - close them to clear the screen.
	CloseAllPanicWindowsL();

	// This is a hack to recover the original CleSrv that was panic'ed during this testing. 
	// For more details, see DEF119818 (PREQ1871: ssmatest_ssm_paniccle needs to run last (it panics original clesrv))
	// Notes:
	//	1.- This method is not a test, it doesn't really add any value to the test, it's just a utility to recover the original CleSrv
	//	2.- Simply by using RSsmStateManager instead of RSsmStateManagerTestClient it will create a new instance of CleSrv
	//		but using the SID of SsmServer (unlike RSsmStateManagerTestClient that uses the SID of the test server), which is what
	//		we need, otherwise, the server created by RSsmStateManagerTestClient is left running, and any cmds executed through it would
	//		fail the SID checks.
	//	3.- This method needs to run before completion (any tests added to this step must be added BEFORE DocleSrvRecovery())
	DoCleSrvRecovery();

	INFO_PRINTF1(_L("....CSsmPanicCleTest completed!!"));
	return TestStepResult();
	}


