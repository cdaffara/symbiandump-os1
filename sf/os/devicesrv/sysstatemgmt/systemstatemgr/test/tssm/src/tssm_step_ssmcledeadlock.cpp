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
#include <ssm/ssmsystemwideproperty.h>
#include "ssmserver.h"
#include "ssmclisrv.h"

#include "tssm_startserver.h"

#include "tssm_step_ssmcledeadlock.h"

#include "tssm_statepolicy_generic.h"
#include "tssm_statepolicy_generic.hrh"

#include "tssm_swppolicy_generic.h"
#include "tssm_swppolicy_generic.hrh"


//---------------- contants  ---------------------------------------

const TUid KMySID={0x2000D75B};				// tssm_server SID = KSsmServerName SID (changed in tssm_server.mmp file)

const TUint KSwpTestKey1=0xE5325221;       // Test Key for testing SWP

const TUint KSwpTestKey2=0xE5325222;       // Test Key for testing SWP

const TUint KSwpTestKey3=0xE5325223;       // Test Key for testing SWP



//----- Utility methods -----

void CSsmSsmCleDeadlockTest::RequestAndWaitForStateTransitionL(TSsmStateTransition& aState)
	{
	TEST(iClient.Handle() != KNullHandle);
	
	// Wait until we have reached ESsmTestConditionalCommandsState
	RSsmStateAwareSession sas;
	TInt err = sas.Connect(KSM2UiServicesDomain3);
	
	INFO_PRINTF2(_L("  --sas.Connect(KSM2UiServicesDomain3) completion codes:  err (%d)"), err);
	TRequestStatus status1;
	sas.RequestStateNotification(status1);

	INFO_PRINTF2(_L("  --sas.RequestStateNotification(status1) status1:  err (%d)"), status1.Int());
	err = iClient.RequestStateTransition(aState);
	
	INFO_PRINTF2(_L("  --completion codes:  err (%d)"), err);
	TEST(err == KErrNone);
	
	TSsmState current = sas.State();
	TEST(err == KErrNone);
	while(err == KErrNone && current.MainState() != aState.State().MainState() && current.SubState() != aState.State().SubState())
		{
		// Wait for next notification of state change
		User::WaitForRequest(status1);
		TEST(status1.Int() == KErrNone);
		sas.AcknowledgeAndRequestStateNotification(KErrNone, status1);
		current = sas.State();
		}
	// Clear the outstanding notification
	sas.RequestStateNotificationCancel();
	User::WaitForRequest(status1);
	TEST(status1.Int() == KErrNone || status1.Int() == KErrCancel);
	sas.Close();
	}

void CSsmSsmCleDeadlockTest::DefinePropertyL(TUint aKey, TBool aValue)
	{
	TInt err = RProperty::Define(RProcess().SecureId(), aKey, RProperty::EInt, EFalse);
	if(err != KErrNone && err != KErrAlreadyExists)
		{	
		INFO_PRINTF3(_L("Error when defining property 0x%x: %d"), aKey, err);
		User::Leave(err);
		}
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), aKey, aValue ? 1 : 0));
	}

void CSsmSsmCleDeadlockTest::TestPropertyL(TUint aKey, TBool aValue)
	{
	TInt value = 0;
	TInt err = RProperty::Get(KMySID, aKey, value);
	if (err == KErrNotFound)
		{
		INFO_PRINTF2(_L("Testing property 0x%x did not exist, assuming false"), aKey);
		value = 0;
		err = KErrNone;
		}
	User::LeaveIfError(err);
	// Test value is the same truth value as aValue
	INFO_PRINTF4(_L("Testing Property 0x%x has value: %d, expected: %d"), aKey, value, aValue);
	TEST(aValue == (value == 1));
	}

void CSsmSsmCleDeadlockTest::WaitForPropertyL(TUint aKey, TBool aValue)
	{
	INFO_PRINTF3(_L("WaitForProperty() called with key 0x%x and value %d"), aKey, aValue);
	RProperty prop;
	TInt err = prop.Attach(KMySID, aKey);
	if(err != KErrNone)
		{
		INFO_PRINTF3(_L("Attaching to property 0x%x failed with error: %d"), aKey, err);
		User::Leave(err);
		}
	TRequestStatus status;
	prop.Subscribe(status);
	TInt currentValue;
	err = prop.Get(currentValue);
	TEST(KErrNone == err);
	if(err != KErrNone)
		{
		INFO_PRINTF3(_L("Getting value for property 0x%x failed with error: %d"), aKey, err);
		User::Leave(err);
		}
	INFO_PRINTF4(_L("Got property value for 0x%x, waiting for %d, got %d"), aKey, aValue, currentValue);
	while(currentValue != aValue)
		{
		User::WaitForRequest(status);
		prop.Subscribe(status);
		err = prop.Get(currentValue);
		TEST(KErrNone == err);
		if(err != KErrNone)
			{
			INFO_PRINTF3(_L("Getting value for propert 0x%x failed with error: %d"), aKey, err);
			User::Leave(err);
			}
		INFO_PRINTF4(_L("Got property value for 0x%x, waiting for %d, got %d"), aKey, aValue, currentValue);
		}
	prop.Cancel();
	User::WaitForRequest(status);
	INFO_PRINTF4(_L("Finished wait loop for property value for 0x%x, waiting for %d, got %d"), aKey, aValue, currentValue);
	TEST(status == KErrNone || status == KErrCancel);	
	}


void CSsmSsmCleDeadlockTest::WaitForSwpL(TUint aKey, TInt aValue)
	{
	RSsmSystemWideProperty property;
	TInt err = property.Connect(aKey);
	TEST(KErrNone == err);
	if(err != KErrNone)
		{
		INFO_PRINTF3(_L("Connecting to SwP 0x%x failed with error: %d"), aKey, err);
		User::Leave(err);
		}
	TRequestStatus status;
	property.Subscribe(status);
	TInt currentValue;
	err = property.GetValue(currentValue);
	TEST(KErrNone == err);
	if(err != KErrNone)
		{
		INFO_PRINTF3(_L("Getting value for SwP 0x%x failed with error: %d"), aKey, err);
		User::Leave(err);
		}
	while(currentValue != aValue)
		{
		User::WaitForRequest(status);
		property.Subscribe(status);
		err = property.GetValue(currentValue);
		TEST(KErrNone == err);
		if(err != KErrNone)
			{
			INFO_PRINTF3(_L("Getting value for SwP 0x%x failed with error: %d"), aKey, err);
			User::Leave(err);
			}
		}
	property.SubscribeCancel();
	User::WaitForRequest(status);
	TEST(status == KErrNone || status == KErrCancel);	
	}


//---------------- Test Steps ---------------------------------------
/**
Old Test CaseID 		APPFWK-SSM-0021
New Test CaseID 		DEVSRVS-SSMA-SSM-0021
 */

void CSsmSsmCleDeadlockTest::DoDeadlockTestL()
	{
	INFO_PRINTF1(_L("Doing a single sync RequestStateTransition then cancelling it with another..."));

	const TInt connect = iClient.Connect();
	TEST(KErrNone == connect);	
	iClient.HeapMark();
	
	// Request state change
	INFO_PRINTF1(_L("Request state transition to state EStateGenericSsmCleDeadlock...")); 
	TSsmStateTransition state1(KSsmTestGenericState, EStateGenericSsmCleDeadlock, 0);
	RequestAndWaitForStateTransitionL(state1);
	
	User::After(1000000); // Wait for 1 sec to allow transitions to fully complete

	// Now cancel the current transition by requesting a second transition
	INFO_PRINTF1(_L("Request state transition to state EStateDoNothing...")); 
	TSsmStateTransition state2(KSsmTestGenericState, EStateDoNothing, 0);
	RequestAndWaitForStateTransitionL(state2);
		
	User::After(1000000); // Wait for 1 sec to allow transitions to fully complete
	
	iClient.HeapMarkEnd(EDoCleanup);		//Pass EDoCleanup for cleaning up Transition Engine after the test case 	
	// clean-up
	iClient.Close();
	INFO_PRINTF1(_L("case completed\n"));
	}

/**
Old Test CaseID 		APPFWK-SSM-0023
New Test CaseID 		DEVSRVS-SSMA-SSM-0021
 */

void CSsmSsmCleDeadlockTest::DoSequentialSwpTestL()
	{
	INFO_PRINTF1(_L("Starting DoSequentialSwpTestL()"));
	
	INFO_PRINTF1(_L("Defining properties"));
	DefinePropertyL(EPropertySequentialSwpTestOutput1, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestOutput2, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestOutput3, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestInput1, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestInput2, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestInput3, EFalse);
	
	
	INFO_PRINTF1(_L("Connecting three SSM sessions"));
	RSsmStateManagerTestClient session1;
	TInt err = session1.Connect();
	TEST(err == KErrNone);
	RSsmStateManagerTestClient session2;
	err = session2.Connect();
	TEST(err == KErrNone);
	RSsmStateManagerTestClient session3;
	err = session3.Connect();
	TEST(err == KErrNone);
	
	INFO_PRINTF1(_L("Requesting initial SwP transition"));
	TSsmSwp swpInitial(KSwpTestKey1, ESwpGenericDoNothing);
	TEST(KErrNone == session1.RequestSwpChange(swpInitial));
	
	INFO_PRINTF1(_L("Requesting first SwP transition"));
	TRequestStatus status1;
	TSsmSwp swp1(KSwpTestKey1, ESwpGenericSequential1);
	session1.RequestSwpChange(swp1, status1);
		
	INFO_PRINTF1(_L("Requesting second SwP transition"));
	TRequestStatus status2;
	TSsmSwp swp2(KSwpTestKey1, ESwpGenericSequential2);
	session2.RequestSwpChange(swp2, status2);
	
	INFO_PRINTF1(_L("Requesting third SwP transition"));
	TRequestStatus status3;
	TSsmSwp swp3(KSwpTestKey1, ESwpGenericSequential3);
	session3.RequestSwpChange(swp3, status3);
	
	INFO_PRINTF1(_L("Checking that EPropertySequentialSwpTestOutput1 is set"));
	WaitForPropertyL(EPropertySequentialSwpTestOutput1, ETrue);
	TestPropertyL(EPropertySequentialSwpTestOutput2, EFalse);
	TestPropertyL(EPropertySequentialSwpTestOutput3, EFalse);
	INFO_PRINTF1(_L("Setting EPropertySequentialSwpTestInput1 to true"));
	DefinePropertyL(EPropertySequentialSwpTestInput1, ETrue);
	
	INFO_PRINTF1(_L("Waiting for ESwpGenericSequential1"));
	WaitForSwpL(KSwpTestKey1, ESwpGenericSequential1);
	User::WaitForRequest(status1);
	TEST(status1 == KErrNone);
	
	INFO_PRINTF1(_L("Checking that EPropertySequentialSwpTestOutput1 is set"));
	TestPropertyL(EPropertySequentialSwpTestOutput1, ETrue);
	WaitForPropertyL(EPropertySequentialSwpTestOutput2, ETrue);
	TestPropertyL(EPropertySequentialSwpTestOutput3, EFalse);
	INFO_PRINTF1(_L("Setting EPropertySequentialSwpTestInput2 to true"));
	DefinePropertyL(EPropertySequentialSwpTestInput2, ETrue);
	
	INFO_PRINTF1(_L("Waiting for ESwpGenericSequential2"));
	WaitForSwpL(KSwpTestKey1, ESwpGenericSequential2);
	User::WaitForRequest(status2);
	TEST(status2 == KErrNone);
	
	INFO_PRINTF1(_L("Checking that EPropertySequentialSwpTestOutput1 is set"));
	TestPropertyL(EPropertySequentialSwpTestOutput1, ETrue);
	TestPropertyL(EPropertySequentialSwpTestOutput2, ETrue);
	WaitForPropertyL(EPropertySequentialSwpTestOutput3, ETrue);
	INFO_PRINTF1(_L("Setting EPropertySequentialSwpTestInput3 to true"));
	DefinePropertyL(EPropertySequentialSwpTestInput3, ETrue);
	
	INFO_PRINTF1(_L("Waiting for ESwpGenericSequential3"));
	WaitForSwpL(KSwpTestKey1, ESwpGenericSequential3);
	User::WaitForRequest(status3);
	TEST(status3 == KErrNone);	
		
	session1.Close();
	session2.Close();
	session3.Close();
	
	INFO_PRINTF1(_L("End of DoSequentialSwpTestL()\n"));
	}

/**
Old Test CaseID 		APPFWK-SSM-0024
New Test CaseID 		DEVSRVS-SSMA-SSM-0021
 */
void CSsmSsmCleDeadlockTest::DoConcurrentSwpTestL()
	{
	INFO_PRINTF1(_L("Starting DoConcurrentSwpTestL()"));
		
	INFO_PRINTF1(_L("Defining properties"));
	DefinePropertyL(EPropertySequentialSwpTestOutput1, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestOutput2, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestOutput3, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestInput1, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestInput2, EFalse);
	DefinePropertyL(EPropertySequentialSwpTestInput3, EFalse);
	
	
	INFO_PRINTF1(_L("Connecting three SSM sessions"));
	RSsmStateManagerTestClient session1;
	TInt err = session1.Connect();
	TEST(err == KErrNone);
	RSsmStateManagerTestClient session2;
	err = session2.Connect();
	TEST(err == KErrNone);
	RSsmStateManagerTestClient session3;
	err = session3.Connect();
	TEST(err == KErrNone);
	
	INFO_PRINTF1(_L("Requesting initial SwP transition"));
	TSsmSwp swpInitial(KSwpTestKey1, ESwpGenericDoNothing);
	TEST(KErrNone == session1.RequestSwpChange(swpInitial));
	
	INFO_PRINTF1(_L("Requesting first SwP transition"));
	TRequestStatus status1;
	TSsmSwp swp1(KSwpTestKey1, ESwpGenericSequential1);
	session1.RequestSwpChange(swp1, status1);
		
	INFO_PRINTF1(_L("Requesting second SwP transition"));
	TRequestStatus status2;
	TSsmSwp swp2(KSwpTestKey2, ESwpGenericSequential2);
	session2.RequestSwpChange(swp2, status2);
	
	INFO_PRINTF1(_L("Requesting third SwP transition"));
	TRequestStatus status3;
	TSsmSwp swp3(KSwpTestKey3, ESwpGenericSequential3);
	session3.RequestSwpChange(swp3, status3);
	
	INFO_PRINTF1(_L("Checking that EPropertySequentialSwpTestOutput1, 2 and 3 are set"));
	WaitForPropertyL(EPropertySequentialSwpTestOutput1, ETrue);
	WaitForPropertyL(EPropertySequentialSwpTestOutput2, ETrue);
	WaitForPropertyL(EPropertySequentialSwpTestOutput3, ETrue);
	
	INFO_PRINTF1(_L("Setting EPropertySequentialSwpTestInput1 to true"));
	DefinePropertyL(EPropertySequentialSwpTestInput1, ETrue);
	INFO_PRINTF1(_L("Setting EPropertySequentialSwpTestInput2 to true"));
	DefinePropertyL(EPropertySequentialSwpTestInput2, ETrue);
	INFO_PRINTF1(_L("Setting EPropertySequentialSwpTestInput3 to true"));
	DefinePropertyL(EPropertySequentialSwpTestInput3, ETrue);
	
	INFO_PRINTF1(_L("Waiting for ESwpGenericSequential1"));
	WaitForSwpL(KSwpTestKey1, ESwpGenericSequential1);
	User::WaitForRequest(status1);
	TEST(status1 == KErrNone);
	
	INFO_PRINTF1(_L("Waiting for ESwpGenericSequential2"));
	WaitForSwpL(KSwpTestKey2, ESwpGenericSequential2);
	User::WaitForRequest(status2);
	TEST(status2 == KErrNone);
	
	INFO_PRINTF1(_L("Waiting for ESwpGenericSequential3"));
	WaitForSwpL(KSwpTestKey3, ESwpGenericSequential3);
	User::WaitForRequest(status3);
	TEST(status3 == KErrNone);	
		
	session1.Close();
	session2.Close();
	session3.Close();
	
	INFO_PRINTF1(_L("End of DoConcurrentSwpTestL()\n"));
	}

//---------------- CSsmSsmCleDeadlockTest step ---------------------------------------


CSsmSsmCleDeadlockTest::~CSsmSsmCleDeadlockTest()
	{
	}

CSsmSsmCleDeadlockTest::CSsmSsmCleDeadlockTest()
	{
	SetTestStepName(KTSsmSsmCleDeadlockStep);
	}

TVerdict CSsmSsmCleDeadlockTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsmSsmCleDeadlockTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}



TVerdict CSsmSsmCleDeadlockTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmSsmCleDeadlockTest started...."));
	
	__UHEAP_MARK;
	INFO_PRINTF1(_L("Creating a test clone for SsmServer...\n"));
	RThread thread;
	TEST(KErrNone == StartServer(thread));	
	
	INFO_PRINTF1(_L("Connecting to test SSM and mapping test SWP 1"));

	TInt err = RProperty::Define(KMySID, KSwpTestKey1, RProperty::EInt);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	const TInt connect = iClient.Connect();
	TEST(KErrNone == connect);
	err = iClient.RegisterSwpMapping(KSwpTestKey1, KTestSwpPolicyGenericFile);
	TEST(err == KErrNone || err == KErrAlreadyExists );

	INFO_PRINTF1(_L("Connecting to test SSM and mapping test SWP 2"));

	err = RProperty::Define(KMySID, KSwpTestKey2, RProperty::EInt);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	err = iClient.RegisterSwpMapping(KSwpTestKey2, KTestSwpPolicyGenericFile);
	TEST(err == KErrNone || err == KErrAlreadyExists );

	INFO_PRINTF1(_L("Connecting to test SSM and mapping test SWP 3"));

	err = RProperty::Define(KMySID, KSwpTestKey3, RProperty::EInt);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	
	err = iClient.RegisterSwpMapping(KSwpTestKey3, KTestSwpPolicyGenericFile);
	TEST(err == KErrNone || err == KErrAlreadyExists );
	iClient.Close();
	
	// Run the tests
	TRAP(err, DoDeadlockTestL());
	TEST(err == KErrNone);
	
	TRAP(err, DoSequentialSwpTestL());
	TEST(err == KErrNone);

	TRAP(err, DoConcurrentSwpTestL());
	TEST(err == KErrNone);
	
	// clean-up
	INFO_PRINTF1(_L("cleaning up"));
	thread.Kill(KErrNone);
	thread.Close();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmSsmCleDeadlockTest completed!!"));
	return TestStepResult();
	}


