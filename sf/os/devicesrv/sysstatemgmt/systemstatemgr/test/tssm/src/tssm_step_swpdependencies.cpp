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
#include <ssm/ssmsystemwideproperty.h>
#include "ssmserver.h"
#include "ssmclisrv.h"

#include "tssm_ssmclient.h"
#include "tssm_startserver.h"
#include "tssm_statepolicy_simple.h"
#include "tssm_statepolicy_dependency.h"
#include "tssm_swppolicy_common.h"
#include "tssm_swppolicy_dependency.h"

#include "tssm_step_swpdependencies.h"


//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)
const TUid KPropertyCategory=KMySID;

// property uids should be real uids (use these temporarily for test purposes)
const TUint KTestSwp1={0xC0C0A0A0};


//---------------- Test Steps ---------------------------------------


void CSsmSwpDependenciesTest::DoSwpRequestWithDependentSwp()
	{
	INFO_PRINTF1(_L("Doing a single async RequestSwpTransition that will cause dependent swp to be changed too."));

	RSsmStateManagerTestClient client;
	TInt connect = client.Connect();
	TEST(KErrNone == connect);
	
	// Create swps
	INFO_PRINTF1(_L("Creating test swp (plus the dependent ones)..."));
	TSsmSwp swp1(KTestSwp1, 10);
	RProperty property1;
	TEST(KErrNone == property1.Define(KPropertyCategory, swp1.Key(), RProperty::EInt));
	TEST(KErrNone == property1.Set(KPropertyCategory, swp1.Key(), swp1.Value()));

	TSsmSwp swp2(KTestDependentSwp1, 20);
	RProperty property2;
	TEST(KErrNone == property2.Define(KPropertyCategory, swp2.Key(), RProperty::EInt));
	TEST(KErrNone == property2.Set(KPropertyCategory, swp2.Key(), swp2.Value()));
	
	TSsmSwp swp3(KTestDependentSwp2, 30);
	RProperty property3;
	TEST(KErrNone == property3.Define(KPropertyCategory, swp3.Key(), RProperty::EInt));
	TEST(KErrNone == property3.Set(KPropertyCategory, swp3.Key(), swp3.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(10 == value, value);

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(20 == value, value);
	
	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(30 == value, value);
	
	// Register mapping between keys and swp policy DLL (done once per ssmserver_test)
	INFO_PRINTF1(_L("Registering swps..."));
	TEST(KErrNone == client.RegisterSwpMapping(KTestSwp1, KTestSwpPolicyDependencyFile));
	TEST(KErrNone == client.RegisterSwpMapping(KTestDependentSwp1, KTestSwpPolicyCommonFile));
	TEST(KErrNone == client.RegisterSwpMapping(KTestDependentSwp2, KTestSwpPolicyCommonFile));
	
	//Register the swp's even with the main servers as the dependent swp change commands which are
	//requested from inside the state/swp policy will go to the main server as they use RSsmStateManager 
	//instead of RSsmStateManagerTestClient which is the test client.
	RSsmStateManager client2;
	connect = client2.Connect();
	TEST(KErrNone == connect);

	TEST(KErrNone == client2.RegisterSwpMapping(KTestSwp1, KTestSwpPolicyDependencyFile));
	TEST(KErrNone == client2.RegisterSwpMapping(KTestDependentSwp1, KTestSwpPolicyCommonFile));
	TEST(KErrNone == client2.RegisterSwpMapping(KTestDependentSwp2, KTestSwpPolicyCommonFile));
	client2.Close();
	
	//Let client request swp change
	INFO_PRINTF1(_L("Requesting swp change..."));
	swp1.Set(KTestSwp1, 100);
	TRequestStatus status1;
	client.RequestSwpChange(swp1, status1);				
	TEST(status1 == KRequestPending);

	User::WaitForRequest(status1);
	INFO_PRINTF2(_L("  --completion codes:  req1 (%d)"), status1.Int());
	TEST(status1.Int() == KErrNone);

	RSsmSystemWideProperty swpPropNotif;
	//Connect to the last swp.
	TInt err = swpPropNotif.Connect(swp3.Key());
	INFO_PRINTF2(_L("Connect to RSsmSystemWideProperty completed with %d"), err);
	TEST(err == KErrNone);
	
	value = 0;
	//Ensure that the swp is not change already.
	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	if(30 == value)
		{
		TRequestStatus status_Notif;
		swpPropNotif.Subscribe(status_Notif);
		
		//Wait till the swp value has been changed.
		User::WaitForRequest(status_Notif);
		TEST(KErrNone == status_Notif.Int());
		}
	
	swpPropNotif.Close();

	//Ensure all properties were properly updated
	value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(100 == value, value);		// As requested above

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(101 == value, value);		// As per the swp dependency policy (adds 1)

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(102 == value, value);		// As per the swp dependency policy (adds 2)

	// clean-up
	TEST(KErrNone == property1.Delete(KPropertyCategory, swp1.Key()));
	TEST(KErrNone == property2.Delete(KPropertyCategory, swp2.Key()));
	TEST(KErrNone == property3.Delete(KPropertyCategory, swp3.Key()));
	property1.Close();
	property2.Close();
	property3.Close();
	client.Close();
	INFO_PRINTF1(_L("case completed\n"));
	}


void CSsmSwpDependenciesTest::DoStateRequestWithDependentSwp()
	{
	INFO_PRINTF1(_L("Doing a single async RequestStateTransition that will cause dependent swp to be changed too."));

	RSsmStateManagerTestClient client;
	TInt connect = client.Connect();
	TEST(KErrNone == connect);

	// Create dependent swps
	INFO_PRINTF1(_L("Creating dependent swp..."));
	TSsmSwp swp3(KTestDependentSwp3, 30);
	RProperty property3;
	TEST(KErrNone == property3.Define(KPropertyCategory, swp3.Key(), RProperty::EInt));
	TEST(KErrNone == property3.Set(KPropertyCategory, swp3.Key(), swp3.Value()));
	
	TSsmSwp swp4(KTestDependentSwp4, 40);
	RProperty property4;
	TEST(KErrNone == property4.Define(KPropertyCategory, swp4.Key(), RProperty::EInt));
	TEST(KErrNone == property4.Set(KPropertyCategory, swp4.Key(), swp4.Value()));
	
	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(30 == value, value);
	
	TEST(KErrNone == property4.Get(KPropertyCategory, swp4.Key(), value));
	TESTE(40 == value, value);
	
	// Register mapping between keys and swp policy DLL (done once per ssmserver)
	INFO_PRINTF1(_L("Registering swps..."));
	TEST(KErrNone == client.RegisterSwpMapping(KTestDependentSwp3, KTestSwpPolicyCommonFile));
	TEST(KErrNone == client.RegisterSwpMapping(KTestDependentSwp4, KTestSwpPolicyCommonFile));
	
	//Register the swp's even with the main servers as the dependent swp change commands which are
	//requested from inside the state/swp policy will go to the main server as they use RSsmStateManager 
	//instead of RSsmStateManagerTestClient which is the test client.
	RSsmStateManager client2;
	connect = client2.Connect();
	TEST(KErrNone == connect);

	TEST(KErrNone == client2.RegisterSwpMapping(KTestDependentSwp3, KTestSwpPolicyCommonFile));
	TEST(KErrNone == client2.RegisterSwpMapping(KTestDependentSwp4, KTestSwpPolicyCommonFile));
	client2.Close();

	// Request state transitions in the session
	INFO_PRINTF1(_L("Request state transition")); 
	TSsmStateTransition state1(ESsmTestDependencyState,123,456);
	TRequestStatus status1;
	client.RequestStateTransition(state1, status1);				// (simple) policy returns EDefinitelyAllowed
	TEST(status1 == KRequestPending);
	
	User::WaitForRequest(status1);
	INFO_PRINTF2(_L("  --completion codes:  req1 (%d)"), status1.Int());
	TEST(status1.Int() == KErrNone);

	RSsmSystemWideProperty swpPropNotif;
	//Connect to the last swp.
	TInt err = swpPropNotif.Connect(swp4.Key());
	INFO_PRINTF2(_L("Connect to RSsmSystemWideProperty completed with %d"), err);
	TEST(err == KErrNone);
	
	value = 0;
	//Ensure that the swp is not change already.
	TEST(KErrNone == property4.Get(KPropertyCategory, swp4.Key(), value));
	if(40 == value)
		{
		TRequestStatus status_Notif;
		swpPropNotif.Subscribe(status_Notif);
		
		//Wait till the swp value has been changed.
		User::WaitForRequest(status_Notif);
		TEST(KErrNone == status_Notif.Int());
		}
	
	swpPropNotif.Close();

	//Ensure all properties were properly updated
	value = 0;
	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(123+456 == value, value);		// As per the state dependency policy (reason + sub state)

	TEST(KErrNone == property4.Get(KPropertyCategory, swp4.Key(), value));
	TESTE(456-123 == value, value);		// As per the state dependency policy (reason - sub state)

	// clean-up
	TEST(KErrNone == property3.Delete(KPropertyCategory, swp3.Key()));
	TEST(KErrNone == property4.Delete(KPropertyCategory, swp4.Key()));
	property3.Close();
	property4.Close();
	client.Close();
	INFO_PRINTF1(_L("case completed\n"));
	}



//---------------- CSsmSwpDependenciesTest step ---------------------------------------


CSsmSwpDependenciesTest::~CSsmSwpDependenciesTest()
	{
	}

CSsmSwpDependenciesTest::CSsmSwpDependenciesTest()
	{
	SetTestStepName(KTSsmSwpDependenciesStep);
	}

TVerdict CSsmSwpDependenciesTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmSwpDependenciesTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0016
New Test CaseID 		DEVSRVS-SSMA-SSM-0016
 */


TVerdict CSsmSwpDependenciesTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSwpDependenciesTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	TEST(KErrNone == StartServer(thread));	

	//perform state tests
	DoStateRequestWithDependentSwp();

	//perform swp tests
	DoSwpRequestWithDependentSwp();

	// clean-up
	INFO_PRINTF1(_L("cleaning up"));
	thread.Kill(KErrNone);
	thread.Close();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSwpDependenciesTest completed!!"));
	return TestStepResult();
	}


