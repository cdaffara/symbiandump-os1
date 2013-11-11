// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// NOTE: It is implied in these tests that SWP are RProperties, as we make use of the RProperty to 
// set and check tha its value before and after a Swp command list is executed.
// In case this equivalence is not true, these tests may need updating.
// 
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
#include "tssm_swppolicy_common.h"
#include "tssm_step_swpcommonplugin.h"
#include "ssmatest_utils.h"

//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)
const TUid KPropertyCategory=KMySID;

// property uids should be real uids (use these temporarily for test purposes)
const TUint KTestSwp1={0x0011AA};
const TInt KTestSwp1_Value = 99;

const TUint KTestSwp2={0x0022BB};
const TUint KTestSwp3={0x0033CC};


//---------------- Test Steps ---------------------------------------


void CSsmSwpCommonPluginTest::RequestSingleSwpChange()
	{
	INFO_PRINTF1(_L("Starting RequestSingleSwpChange test case."));

	INFO_PRINTF1(_L("Connecting to the SsmServer..."));
	RSsmStateManagerTestClient client;
	TEST(KErrNone == client.Connect());
	client.HeapMark();
	
	// Create a swp
	INFO_PRINTF1(_L("Creating a swp..."));
	TSsmSwp swp(KTestSwp1, -35);
	RProperty property;
	TEST(KErrNone == property.Define(KPropertyCategory, swp.Key(), RProperty::EInt));
	TEST(KErrNone == property.Set(KPropertyCategory, swp.Key(), swp.Value()));

	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	TESTE(-35 == value, value);

	//Let client request a swp change
	INFO_PRINTF1(_L("Requesting a swp change..."));
	swp.Set(KTestSwp1, 53);
	TRequestStatus status1;
	client.RequestSwpChange(swp, status1);
	TEST(status1 == KRequestPending);

	User::WaitForRequest(status1);
	INFO_PRINTF2(_L("  --completion codes:  req1 (%d)"), status1.Int());
	TEST(status1.Int() == KErrNone);

	RSsmSystemWideProperty swpPropNotif;
	TInt err = swpPropNotif.Connect(swp.Key());
	INFO_PRINTF2(_L("Connect to RSsmSystemWideProperty completed with %d"), err);
	TEST(err == KErrNone);
	
	value = 0;
	//Ensure that the swp is not change already.
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	if(-35 == value)
		{
		TRequestStatus status_Notif;
		swpPropNotif.Subscribe(status_Notif);
		
		//Wait till the swp value has been changed.
		User::WaitForRequest(status_Notif);
		TEST(KErrNone == status_Notif.Int());
		}
	
	swpPropNotif.Close();
	//Ensure the property was properly updated. The current value is the last change requested.
	value = 0;
	TEST(KErrNone == property.Get(KPropertyCategory, swp.Key(), value));
	TESTE(53 == value, value);
	
	// Even after the SWP is published is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(0.2*1000*1000);

	// clean-up
	TEST(KErrNone == property.Delete(KPropertyCategory, swp.Key()));
	property.Close();
	client.HeapMarkEnd(ETrue);
	client.Close();

	INFO_PRINTF1(_L("RequestSingleSwpChange test case completed.\n"));
	}


void CSsmSwpCommonPluginTest::RequestMultipleSwpChangeSamePlugin()
	{
	INFO_PRINTF1(_L("Starting RequestMultipleSwpChangeSamePlugin test case."));

	INFO_PRINTF1(_L("Connecting to the SsmServer..."));
	RSsmStateManagerTestClient client;
	TEST(KErrNone == client.Connect());
	client.HeapMark();

	// Create a swp
	INFO_PRINTF1(_L("Creating several swp..."));
	TSsmSwp swp1(KTestSwp1, -11);
	RProperty property1;
	TEST(KErrNone == property1.Define(KPropertyCategory, swp1.Key(), RProperty::EInt));
	TEST(KErrNone == property1.Set(KPropertyCategory, swp1.Key(), swp1.Value()));

	TSsmSwp swp2(KTestSwp2, 22);
	RProperty property2;
	TEST(KErrNone == property2.Define(KPropertyCategory, swp2.Key(), RProperty::EInt));
	TEST(KErrNone == property2.Set(KPropertyCategory, swp2.Key(), swp2.Value()));

	TSsmSwp swp3(KTestSwp3, -33);
	RProperty property3;
	TEST(KErrNone == property3.Define(KPropertyCategory, swp3.Key(), RProperty::EInt));
	TEST(KErrNone == property3.Set(KPropertyCategory, swp3.Key(), swp3.Value()));

	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(-11 == value, value);

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(22 == value, value);

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(-33 == value, value);
	
	//Let client request swp change
	INFO_PRINTF1(_L("Requesting swp changes for all different keys..."));
	swp1.Set(KTestSwp1, 99);
	TRequestStatus status1;
	client.RequestSwpChange(swp1, status1);
	TEST(status1 == KRequestPending);

	swp2.Set(KTestSwp2, -88);
	TRequestStatus status2;
	client.RequestSwpChange(swp2, status2);
	TEST(status2 == KRequestPending);

	swp3.Set(KTestSwp3, 77);
	TRequestStatus status3;
	client.RequestSwpChange(swp3, status3);
	TEST(status3 == KRequestPending);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	INFO_PRINTF4(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d)"), status1.Int(), status2.Int(), status3.Int());
	TEST(status1.Int() == KErrNone);
	TEST(status2.Int() == KErrNone);
	TEST(status3.Int() == KErrNone);

	RSsmSystemWideProperty swpPropNotif;
	//Connect to the last swp.
	TInt err = swpPropNotif.Connect(swp3.Key());
	INFO_PRINTF2(_L("Connect to RSsmSystemWideProperty completed with %d"), err);
	TEST(err == KErrNone);
	
	value = 0;
	//Ensure that the swp is not change already.
	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	if(-33 == value)
		{
		TRequestStatus status_Notif;
		swpPropNotif.Subscribe(status_Notif);
		
		//Wait till the swp value has been changed.
		User::WaitForRequest(status_Notif);
		TEST(KErrNone == status_Notif.Int());
		}
	
	swpPropNotif.Close();

	value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(99 == value, value);

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(-88 == value, value);

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(77 == value, value);

	// clean-up
	TEST(KErrNone == property1.Delete(KPropertyCategory, swp1.Key()));
	TEST(KErrNone == property2.Delete(KPropertyCategory, swp2.Key()));
	TEST(KErrNone == property3.Delete(KPropertyCategory, swp3.Key()));
	property1.Close();
	property2.Close();
	property3.Close();

	// Even after the SWP is published is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(1000*1000);

	client.HeapMarkEnd(ECleanupSwpRequestHandlerOnly);
	client.Close();

	INFO_PRINTF1(_L("RequestMultipleSwpChangeSamePlugin test case completed.\n"));
	}


void CSsmSwpCommonPluginTest::RequestMultipleSwpChange()
	{
	INFO_PRINTF1(_L("Starting RequestMultipleSwpChange test case."));

	INFO_PRINTF1(_L("Connecting 2 clients to the SsmServer..."));
	RSsmStateManagerTestClient client1;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();
	RSsmStateManagerTestClient client2;
	TEST(KErrNone == client2.Connect());

	// Create a swp
	INFO_PRINTF1(_L("Creating 2 swp..."));
	TSsmSwp swp1(KTestSwp1, -11);
	RProperty property1;
	TEST(KErrNone == property1.Define(KPropertyCategory, swp1.Key(), RProperty::EInt));
	TEST(KErrNone == property1.Set(KPropertyCategory, swp1.Key(), swp1.Value()));

	TSsmSwp swp2(KTestSwp2, 22);
	RProperty property2;
	TEST(KErrNone == property2.Define(KPropertyCategory, swp2.Key(), RProperty::EInt));
	TEST(KErrNone == property2.Set(KPropertyCategory, swp2.Key(), swp2.Value()));

	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(-11 == value, value);

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(22 == value, value);

	//Let client request swp change
	INFO_PRINTF1(_L("Requesting swp changes for all different keys and clients..."));
	swp1.Set(KTestSwp1, 99);
	TRequestStatus status1;
	client1.RequestSwpChange(swp1, status1);		// client1 changes key1
	TEST(status1 == KRequestPending);

	swp1.Set(KTestSwp1, -33);
	TRequestStatus status2;
	client2.RequestSwpChange(swp1, status2);		// client2 changes key1
	TEST(status2 == KRequestPending);

	swp2.Set(KTestSwp2, -88);
	TRequestStatus status3;
	client2.RequestSwpChange(swp2, status3);		// client2 changes key2
	TEST(status3 == KRequestPending);

	swp2.Set(KTestSwp2, 77);
	TRequestStatus status4;
	client1.RequestSwpChange(swp2, status4);		// client1 changes key2
	TEST(status4 == KRequestPending);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	User::WaitForRequest(status4);
	INFO_PRINTF5(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d), req4 (%d)"), status1.Int(), status3.Int(), status3.Int(), status4.Int());
	TEST(status1.Int() == KErrNone);
	TEST(status2.Int() == KErrNone);
	TEST(status3.Int() == KErrNone);
	TEST(status4.Int() == KErrNone);

	RSsmSystemWideProperty swpPropNotif;
	//Connect to the last swp.
	TInt err = swpPropNotif.Connect(swp2.Key());
	INFO_PRINTF2(_L("Connect to RSsmSystemWideProperty completed with %d"), err);
	TEST(err == KErrNone);
	
	value = 0;
	//Ensure that the swp is not change already.
	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	if(77 != value)
		{
		TRequestStatus status_Notif;
		swpPropNotif.Subscribe(status_Notif);
		
		//Wait till the swp value has been changed.
		User::WaitForRequest(status_Notif);
		TEST(KErrNone == status_Notif.Int());
		
		value = 0;
		err = swpPropNotif.GetValue(value);
		TEST(KErrNone == err);
		// Since we are requesting swp change twice we have to wait
		// until the swp is changed to second value.
		if (value == -88)
			{
			swpPropNotif.Subscribe(status_Notif);
			
			//Wait till the swp value has been changed.
			User::WaitForRequest(status_Notif);
			TEST(KErrNone == status_Notif.Int());
			}
		}
	
	swpPropNotif.Close();

	//Ensure all properties were properly updated
	value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(-33 == value, value);		// last value changed by client2

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(77 == value, value);		// last value changed by client1

	// clean-up
	TEST(KErrNone == property1.Delete(KPropertyCategory, swp1.Key()));
	TEST(KErrNone == property2.Delete(KPropertyCategory, swp2.Key()));
	property1.Close();
	property2.Close();

	// Even after the SWP is published is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(0.5*1000*1000);

	client2.Close();
	client1.HeapMarkEnd(ECleanupSwpRequestHandlerOnly);
	client1.Close();

	INFO_PRINTF1(_L("RequestMultipleSwpChange test case completed.\n"));
	}

void CSsmSwpCommonPluginTest::RequestMultipleSwpChangeWithPanicL()
	{
	INFO_PRINTF1(_L("Starting RequestMultipleSwpChangeWithPanic test case."));

	INFO_PRINTF1(_L("Connecting client to the SsmServer..."));
	RSsmStateManagerTestClient client1;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();

	// Create a swp
	INFO_PRINTF1(_L("Creating a swp..."));
	TSsmSwp swp1(KTestSwp1, -11);
	RProperty property1;
	TEST(KErrNone == property1.Define(KPropertyCategory, swp1.Key(), RProperty::EInt));
	TEST(KErrNone == property1.Set(KPropertyCategory, swp1.Key(), swp1.Value()));

	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(-11 == value, value);

	//Let client request swp change
	INFO_PRINTF1(_L("Requesting swp changes multiple times..."));
	swp1.Set(KTestSwp1, KTestSwp1_Value);
	TRequestStatus status1;
	client1.RequestSwpChange(swp1, status1);
	TEST(status1 == KRequestPending);

	// Just adding random no.
	swp1.Set(KTestSwp1, KTestSwp1_Value+1);
	TRequestStatus status2;
	client1.RequestSwpChange(swp1, status2);
	TEST(status2 == KRequestPending);

	// Just adding random no.
	swp1.Set(KTestSwp1, KTestSwp1_Value+51);
	TRequestStatus status3;
	client1.RequestSwpChange(swp1, status3);
	TEST(status3 == KRequestPending);

	// Just adding random no.
	swp1.Set(KTestSwp1, KTestSwp1_Value+101);
	TRequestStatus status4;
	client1.RequestSwpChange(swp1, status4);
	TEST(status4 == KRequestPending);

	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	User::WaitForRequest(status4);
	INFO_PRINTF5(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d), req4 (%d)"), status1.Int(), status3.Int(), status3.Int(), status4.Int());
	TEST(status1.Int() == KErrNone);
	TEST(status2.Int() == KErrNone);
	TEST(status3.Int() == KErrServerTerminated);
	TEST(status4.Int() == KErrServerTerminated);

	RSsmSystemWideProperty swpPropNotif;
	//Connect to the last swp.
	TInt err = swpPropNotif.Connect(swp1.Key());
	INFO_PRINTF2(_L("Connect to RSsmSystemWideProperty completed with %d"), err);
	TEST(err == KErrNone);
	
	value = 0;
	//Ensure that the swp is not change already.
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	if(-11 == value)
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
	// Only one swp change will happen as the ssmswppolicyserver would panic after that
	//  and remaining request would be completed with KErrServerTerminated
	TESTE(99 == value, value);

	// clean-up
	TEST(KErrNone == property1.Delete(KPropertyCategory, swp1.Key()));
	property1.Close();
	client1.HeapMarkEnd(ECleanupSwpRequestHandlerOnly);
	client1.Close();
	CloseAllPanicWindowsL();

	INFO_PRINTF1(_L("RequestMultipleSwpChangeWithPanic test case completed.\n"));
	}

void CSsmSwpCommonPluginTest::RequestCancelQueuedSwpChange()
	{
	INFO_PRINTF1(_L("Starting RequestCancelQueuedSwpChange test case."));

	INFO_PRINTF1(_L("Connecting 2 clients to the SsmServer..."));
	RSsmStateManagerTestClient client1;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();
	RSsmStateManagerTestClient client2;
	TEST(KErrNone == client2.Connect());

	// Create a swp
	INFO_PRINTF1(_L("Creating 2 swp..."));
	TSsmSwp swp1(KTestSwp1, -11);
	RProperty property1;
	TEST(KErrNone == property1.Define(KPropertyCategory, swp1.Key(), RProperty::EInt));
	TEST(KErrNone == property1.Set(KPropertyCategory, swp1.Key(), swp1.Value()));

	TSsmSwp swp2(KTestSwp2, 22);
	RProperty property2;
	TEST(KErrNone == property2.Define(KPropertyCategory, swp2.Key(), RProperty::EInt));
	TEST(KErrNone == property2.Set(KPropertyCategory, swp2.Key(), swp2.Value()));

	TSsmSwp swp3(KTestSwp3, -33);
	RProperty property3;
	TEST(KErrNone == property3.Define(KPropertyCategory, swp3.Key(), RProperty::EInt));
	TEST(KErrNone == property3.Set(KPropertyCategory, swp3.Key(), swp3.Value()));

	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(-11 == value, value);

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(22 == value, value);

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(-33 == value, value);

	//Let client request swp change
	INFO_PRINTF1(_L("Requesting swp changes for all different keys and clients..."));
	swp1.Set(KTestSwp1, 99);
	TRequestStatus status1;
	client1.RequestSwpChange(swp1, status1);		// client1 changes key1
	TEST(status1 == KRequestPending);

	swp1.Set(KTestSwp1, -33);
	TRequestStatus status2;
	client2.RequestSwpChange(swp1, status2);		// client2 changes key1
	TEST(status2 == KRequestPending);

	swp2.Set(KTestSwp2, -88);
	TRequestStatus status3;
	client2.RequestSwpChange(swp2, status3);		// client2 changes key2
	TEST(status3 == KRequestPending);

	swp3.Set(KTestSwp3, 44);
	TRequestStatus status4;
	client1.RequestSwpChange(swp3, status4);		// client1 changes key3
	TEST(status4 == KRequestPending);

	INFO_PRINTF1(_L("Cancel requests on second client (both requests are in queue)..."));
	client2.RequestSwpChangeCancel(); 

	//Because the Cancel is synchronous, we don't need to do User::WaitForRequest(status1) etc.
	User::WaitForRequest(status4);

	INFO_PRINTF5(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d), req4 (%d)"), status1.Int(), status2.Int(), status3.Int(), status4.Int());
	TEST(status1.Int() == KErrNone);
	TEST(status2.Int() == KErrCancel);
	TEST(status3.Int() == KErrCancel);
	TEST(status4.Int() == KErrNone);

	RSsmSystemWideProperty swpPropNotif;
	//Connect to the last swp.
	TInt err = swpPropNotif.Connect(swp3.Key());
	INFO_PRINTF2(_L("Connect to RSsmSystemWideProperty completed with %d"), err);
	TEST(err == KErrNone);
	
	value = 0;
	//Ensure that the swp is not change already.
	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	if(-33 == value)
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
	TESTE(99 == value, value);		// last value changed by client1

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(22 == value, value);		// value not changed by any client (queued one was cancelled)

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(44 == value, value);		// last value changed by client1

	// clean-up
	TEST(KErrNone == property1.Delete(KPropertyCategory, swp1.Key()));
	TEST(KErrNone == property2.Delete(KPropertyCategory, swp2.Key()));
	TEST(KErrNone == property3.Delete(KPropertyCategory, swp3.Key()));
	property1.Close();
	property2.Close();
	property3.Close();
	client2.Close();

	// Even after the SWP is published is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(0.5*1000*1000);

	client1.HeapMarkEnd(ECleanupSwpRequestHandlerOnly);
	client1.Close();

	INFO_PRINTF1(_L("RequestCancelQueuedSwpChange test case completed.\n"));
	}


void CSsmSwpCommonPluginTest::RequestCancelRunningSwpChange()
	{
	INFO_PRINTF1(_L("Starting RequestCancelRunningSwpChange test case."));

	INFO_PRINTF1(_L("Connecting 2 clients to the SsmServer..."));
	RSsmStateManagerTestClient client1;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();
	RSsmStateManagerTestClient client2;
	TEST(KErrNone == client2.Connect());

	// Create a swp
	INFO_PRINTF1(_L("Creating 2 swp..."));
	TSsmSwp swp1(KTestSwp1, -11);
	RProperty property1;
	TEST(KErrNone == property1.Define(KPropertyCategory, swp1.Key(), RProperty::EInt));
	TEST(KErrNone == property1.Set(KPropertyCategory, swp1.Key(), swp1.Value()));

	TSsmSwp swp2(KTestSwp2, 22);
	RProperty property2;
	TEST(KErrNone == property2.Define(KPropertyCategory, swp2.Key(), RProperty::EInt));
	TEST(KErrNone == property2.Set(KPropertyCategory, swp2.Key(), swp2.Value()));

	TSsmSwp swp3(KTestSwp3, -33);
	RProperty property3;
	TEST(KErrNone == property3.Define(KPropertyCategory, swp3.Key(), RProperty::EInt));
	TEST(KErrNone == property3.Set(KPropertyCategory, swp3.Key(), swp3.Value()));

	//Ensure the environment is clean
	TInt value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(-11 == value, value);

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(22 == value, value);

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(-33 == value, value);

	//Let client request swp change
	INFO_PRINTF1(_L("Requesting swp changes for all different keys and clients..."));
	swp1.Set(KTestSwp1, 99);
	TRequestStatus status1;
	client1.RequestSwpChange(swp1, status1);		// client1 changes key1
	TEST(status1 == KRequestPending);

	swp2.Set(KTestSwp2, -33);
	TRequestStatus status2;
	client2.RequestSwpChange(swp2, status2);		// client2 changes key2
	TEST(status2 == KRequestPending);

	swp3.Set(KTestSwp3, 44);
	TRequestStatus status3;
	client2.RequestSwpChange(swp3, status3);		// client2 changes key3
	TEST(status3 == KRequestPending);
	
	swp3.Set(KTestSwp3, -55);
	TRequestStatus status4;
	client1.RequestSwpChange(swp3, status4);		// client1 changes key3
	TEST(status4 == KRequestPending);

	INFO_PRINTF1(_L("Cancel requests on first client (first request running!)"));
	TEST(status1.Int() == KRequestPending);				// client 1 still pending --> we can  cancel it!
	client1.RequestSwpChangeCancel(); 

	//Because the Cancel is synchronous, we don't need to do User::WaitForRequest(status1) etc.
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	
	INFO_PRINTF5(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d), req4 (%d)"), status1.Int(), status3.Int(), status3.Int(), status4.Int());
	TEST(status1.Int() == KErrCancel);
	TEST(status2.Int() == KErrNone);
	TEST(status3.Int() == KErrNone);
	TEST(status4.Int() == KErrCancel);

	RSsmSystemWideProperty swpPropNotif;
	//Connect to the last swp.
	TInt err = swpPropNotif.Connect(swp3.Key());
	INFO_PRINTF2(_L("Connect to RSsmSystemWideProperty completed with %d"), err);
	TEST(err == KErrNone);
	
	value = 0;
	//Ensure that the swp is not change already.
	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	if(-33 == value)
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
	TESTE(-11 == value, value);		// value not changed by any client (running one was cancelled)

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(-33 == value, value);		// last value changed by client2

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(44 == value, value);		// last value changed by client2

	// clean-up
	TEST(KErrNone == property1.Delete(KPropertyCategory, swp1.Key()));
	TEST(KErrNone == property2.Delete(KPropertyCategory, swp2.Key()));
	TEST(KErrNone == property3.Delete(KPropertyCategory, swp3.Key()));
	property1.Close();
	property2.Close();
	property3.Close();
	client2.Close();
	
	// Even after the SWP is published is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(0.5*1000*1000);

	client1.HeapMarkEnd(ECleanupSwpRequestHandlerOnly);
	client1.Close();

	INFO_PRINTF1(_L("RequestCancelRunningSwpChange test case completed.\n"));
	}


void CSsmSwpCommonPluginTest::StressSwpChange()
	{
	INFO_PRINTF1(_L("Starting StressSwpChange test case."));

	INFO_PRINTF1(_L("Connecting 2 clients to the SsmServer..."));
	RSsmStateManagerTestClient client1;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();
	RSsmStateManagerTestClient client2;
	TEST(KErrNone == client2.Connect());
	RSsmStateManagerTestClient client3;
	TEST(KErrNone == client3.Connect());
	
	// Create a swp
	INFO_PRINTF1(_L("Creating 2 swp..."));
	TSsmSwp swp1(KTestSwp1, 0);
	RProperty property1;
	TEST(KErrNone == property1.Define(KPropertyCategory, swp1.Key(), RProperty::EInt));
	TEST(KErrNone == property1.Set(KPropertyCategory, swp1.Key(), swp1.Value()));

	TSsmSwp swp2(KTestSwp2, 1000);
	RProperty property2;
	TEST(KErrNone == property2.Define(KPropertyCategory, swp2.Key(), RProperty::EInt));
	TEST(KErrNone == property2.Set(KPropertyCategory, swp2.Key(), swp2.Value()));

	TSsmSwp swp3(KTestSwp3, -1);
	RProperty property3;
	TEST(KErrNone == property3.Define(KPropertyCategory, swp3.Key(), RProperty::EInt));
	TEST(KErrNone == property3.Set(KPropertyCategory, swp3.Key(), swp3.Value()));

	//Ensure the environment is clean
	TInt value = -1;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	TESTE(0 == value, value);

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	TESTE(1000 == value, value);

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	TESTE(-1 == value, value);

	//Let client request swp change
	INFO_PRINTF1(_L("Requesting swp changes for all different keys and clients..."));
	TInt index = 0;
	TRequestStatus status;			// reusing same trequeststatus is a bad idea, however, we can
	for (; index < 100; index++)
		{
		swp1.Set(KTestSwp1, index);
		client1.RequestSwpChange(swp1, status);		// client1 changes key1
		
		swp2.Set(KTestSwp2, 1000-index);
		client2.RequestSwpChange(swp2, status);		// client2 changes key2
		}

	swp3.Set(KTestSwp3, 500);
	TRequestStatus status3;
	client1.RequestSwpChange(swp3, status3);		// client1 changes key3
	TEST(status3 == KRequestPending);

	User::WaitForRequest(status3);
	INFO_PRINTF2(_L("  --completion codes:  req3 (%d)"), status3.Int());
	TEST(status3.Int() == KErrNone);

	
	RSsmSystemWideProperty ssmSwpNotif;
	TEST(KErrNone == ssmSwpNotif.Connect(KTestSwp2));

	value = 0;
	ssmSwpNotif.GetValue(value);

	TRequestStatus status_notif;
	
	// We are waiting till we get the notification for the final SWP value.(i.e., the second
	// swp which has the max. no. of swp transitions)
	while(value > 901)
		{
		ssmSwpNotif.Subscribe(status_notif);
		User::WaitForRequest(status_notif);
		ssmSwpNotif.GetValue(value);
		}

	ssmSwpNotif.Close();
	//Ensure all properties were properly updated
	value = 0;
	TEST(KErrNone == property1.Get(KPropertyCategory, swp1.Key(), value));
	INFO_PRINTF2(_L("property1 value %d"), value);
	TESTE(99 == value, value);

	TEST(KErrNone == property2.Get(KPropertyCategory, swp2.Key(), value));
	INFO_PRINTF2(_L("property2 value %d"), value);
	TESTE(901 == value, value);

	TEST(KErrNone == property3.Get(KPropertyCategory, swp3.Key(), value));
	INFO_PRINTF2(_L("property3 value %d"), value);
	TESTE(500 == value, value);

	// clean-up
	TEST(KErrNone == property1.Delete(KPropertyCategory, swp1.Key()));
	TEST(KErrNone == property2.Delete(KPropertyCategory, swp2.Key()));
	TEST(KErrNone == property3.Delete(KPropertyCategory, swp3.Key()));
	property1.Close();
	property2.Close();
	property3.Close();
	client3.Close();
	client2.Close();

	// Even after the SWP is published is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(0.5*1000*1000);

	client1.HeapMarkEnd(ECleanupSwpRequestHandlerOnly);
	client1.Close();

	INFO_PRINTF1(_L("StressSwpChange test case completed.\n"));
	}



//---------------- CSsmSwpCommonPluginTest step ---------------------------------------


CSsmSwpCommonPluginTest::~CSsmSwpCommonPluginTest()
	{
	}

CSsmSwpCommonPluginTest::CSsmSwpCommonPluginTest()
	{
	SetTestStepName(KTSsmSwpCommonPluginStep);
	}

TVerdict CSsmSwpCommonPluginTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmSwpCommonPluginTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0015
New Test CaseID 		DEVSRVS-SSMA-SSM-0015
 */

TVerdict CSsmSwpCommonPluginTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSwpCommonPluginTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	TEST(KErrNone == StartServer(thread));	

	// Register mapping between keys and common swp policy DLL (done once per ssmserver)
	INFO_PRINTF1(_L("Registering mapping between keys and common swp policy DLL..."));
	RSsmStateManagerTestClient client;
	TEST(KErrNone == client.Connect());
	TEST(KErrNone == client.RegisterSwpMapping(KTestSwp1, KTestSwpPolicyCommonFile));
	TEST(KErrNone == client.RegisterSwpMapping(KTestSwp2, KTestSwpPolicyCommonFile));
	TEST(KErrNone == client.RegisterSwpMapping(KTestSwp3, KTestSwpPolicyCommonFile));
	client.Close();		// we don't perfrom Server Heap check here because the SwpMapping array has grown
	
	//perform tests
	RequestSingleSwpChange();
	RequestMultipleSwpChangeSamePlugin();
	RequestMultipleSwpChange();
	RequestCancelQueuedSwpChange();
	RequestCancelRunningSwpChange();
	StressSwpChange();

	// This test case should be run at the last as it panics the SysStateMgr
	RequestMultipleSwpChangeWithPanicL();
	// clean-up
	INFO_PRINTF1(_L("cleaning up"));
	thread.Kill(KErrNone);
	thread.Close();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSwpCommonPluginTest completed!!"));
	return TestStepResult();
	}


