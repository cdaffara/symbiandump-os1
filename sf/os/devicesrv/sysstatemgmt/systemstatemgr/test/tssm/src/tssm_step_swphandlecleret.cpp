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
#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmdomaindefs.h>
#include "ssmserver.h"
#include "ssmclisrv.h"

#include "tssm_ssmclient.h"
#include "tssm_startserver.h"
#include "tssm_statepolicy_simple.h"
#include "tssm_swppolicy_common.h"
#include "tssm_swppolicy_ssmincleret.h"

#include "tssm_step_swphandlecleret.h"
#include "tssm_statepolicy_generic.h"

//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)
const TUid KPropertyCategory=KMySID;



//---------------- Test Steps ---------------------------------------



void CSsmSwpHandleCleRetTest::DoStateTranTestInSwpPolicyL(TInt aTestType)
	{
	INFO_PRINTF1(_L("Doing a single async RequestSwpTransition that will cause dependent swp to be changed too."));

	RSsmStateManagerTestClient client;
	const TInt connect = client.Connect();
	client.HeapMark();
	TEST(KErrNone == connect);

	// Create swps
	INFO_PRINTF1(_L("Creating a test swp..."));
	TSsmSwp swp(KTestSwpHandleCleRetStateReq, -23);
	RProperty property;
	TEST(KErrNone == property.Define(KPropertyCategory, swp.Key(), RProperty::EInt));
	TEST(KErrNone == property.Set(KPropertyCategory, swp.Key(), swp.Value()));

	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	TESTE(-23 == value, value);
	
	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	INFO_PRINTF1(_L("Registering swp..."));
	TEST(KErrNone == client.RegisterSwpMapping(KTestSwpHandleCleRetStateReq, KTestSwpPolicySsmInCleRetFile));
	
	//Let client request swp change
	INFO_PRINTF1(_L("Requesting swp change..."));
	swp.Set(KTestSwpHandleCleRetStateReq, aTestType);
	TRequestStatus status;
	client.RequestSwpChange(swp, status);				
	TEST(status == KRequestPending);

	User::WaitForRequest(status);
	INFO_PRINTF2(_L("  --completion codes:  req1 (%d)"), status.Int());
	TEST(status.Int() == KErrNone);

	// Wait a sec to ensure all cmd lists have finished executing
	User::After(1*1000*1000);
	
	//Ensure all properties were properly updated
	value = 0;
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	// aTestType is the expected value of the swp
	TESTE(aTestType == value, value);		// As requested above

	if (aTestType == 10)
		// ERequestStateTranSync - We issue a StateTransition request in HandleCleReturnValue
		// and wait for the completion of the transition before completing HandleCleReturnValue
		{
		//Ensure we really had a successful transition in the HandleCleReturn of the swp policy
		//	NOTE: Before starting the swp request, we were in ESsmTestSimpleState, when completed, we should be in ESsmTestFailState
		INFO_PRINTF1(_L("Request state transition to state simple, but current fail state should not allow it")); 
		TSsmStateTransition state2(ESsmTestSimpleState, KSsmAnySubState, 00);
		TRequestStatus status2;
		client.RequestStateTransition(state2, status2);						// (fail) policy returns ENotAllowed
		TEST(status2 == KRequestPending);
	
		User::WaitForRequest(status2);
		INFO_PRINTF2(_L("  --completion codes:  req2 (%d)"), status2.Int());
		TEST(status2.Int() == KErrNotSupported);
		}
	else if (aTestType == 20)
		// ERequestStateTranAsync - We issue a StateTransition request in HandleCleReturnValue
		// but do not wait for the completion of the transition whereas complete the HandleCleReturnValue
		// Since the swp policy is not unloaded, there are no stray signals from swppolicy.
		{
		//Ensure we really had a successful transition in the HandleCleReturn of the swp policy
		//	NOTE: Before starting the swp request, we were in ESsmTestSimpleState, when completed, we should be in ESsmNormal
		INFO_PRINTF1(_L("Request state transition to state generic, and the system should be Normal state")); 

		// Connects to RSsmStateAwareSession in order to enquire about the current system state
		RSsmStateAwareSession ssmStateAwareSess;
		TInt err = ssmStateAwareSess.Connect(KSM2UiServicesDomain3);
		TSsmState currenState;
		currenState = ssmStateAwareSess.State();
		TEST(currenState.MainState() == ESsmNormal);
		ssmStateAwareSess.Close();
		}
	
	client.RequestDeRegisterSwpMappingL(KTestSwpHandleCleRetStateReq, KTestSwpPolicySsmInCleRetFile);
	// clean-up
	TEST(KErrNone == property.Delete(KPropertyCategory, swp.Key()));
	property.Close();
	client.HeapMarkEnd(EDoCleanup);
	client.Close();
	INFO_PRINTF1(_L("case completed\n"));
	}



//---------------- CSsmSwpHandleCleRetTest step ---------------------------------------


CSsmSwpHandleCleRetTest::~CSsmSwpHandleCleRetTest()
	{
	}

CSsmSwpHandleCleRetTest::CSsmSwpHandleCleRetTest()
	{
	SetTestStepName(KTSsmSwpHandleCleRetStep);
	}

TVerdict CSsmSwpHandleCleRetTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmSwpHandleCleRetTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0019
New Test CaseID 		DEVSRVS-SSMA-SSM-0019
 */


TVerdict CSsmSwpHandleCleRetTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSwpHandleCleRetTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	TEST(KErrNone == StartServer(thread));	

	//perform swp tests
	TRAPD(err, DoStateTranTestInSwpPolicyL(10));
	TEST(err == KErrNone);

	// clean-up
	thread.Kill(KErrNone);
	thread.Close();

	// Starting the server again, since the current system to be in FailState. In order to request for
	// further transitions the system should be in simple state
	TEST(KErrNone == StartServer(thread));	
	TRAP(err, DoStateTranTestInSwpPolicyL(20));
	TEST(err == KErrNone);

	// clean-up
	INFO_PRINTF1(_L("cleaning up"));
	thread.Kill(KErrNone);
	thread.Close();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSwpHandleCleRetTest completed!!"));
	return TestStepResult();
	}


