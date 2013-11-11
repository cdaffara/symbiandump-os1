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


#include "tssm_step_server.h"
#include "tssm_ssmclient.h"
#include "tssm_startserver.h"
#include "tssm_statepolicy_simple.h"
#include "tssm_statepolicy_fail.h"
#include "tssm_swppolicy_simple.h"

_LIT(KTestWrongPolicyFile, "i_do_not_exist.dll");

CSsmServerTest::~CSsmServerTest()
	{
	iSemaphore.Close();
	}

CSsmServerTest::CSsmServerTest()
	{
	SetTestStepName(KTSsmServerStep);
	}

/** */
TVerdict CSsmServerTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/** */
TVerdict CSsmServerTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSsmServerTest started...."));
	
	__UHEAP_MARK;
	
	INFO_PRINTF1(_L("	------ Unit Tests ------	"));
	doSsmServerTestsL();
	doTestConnectedSessionL();

	
	INFO_PRINTF1(_L("	------ Integ Tests ------	"));
	doSwpHandlingTestsL();
	doStateHandlingTestsL();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSsmServerTest completed!!"));
	return TestStepResult();
	}


TVerdict CSsmServerTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-SSM-0001
New Test CaseID 		DEVSRVS-SSMA-SSM-0001
 */

void CSsmServerTest::doSsmServerTestsL()
	{
	doTestForMemoryLeaksL();
	doTestHeapFailureL();
	doTestProcessCriticalL();
	}

void CSsmServerTest::doTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Checking for memoryleaks in SsmServer destructor"));
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	
	__UHEAP_MARK;
	CSsmServer* server = CSsmServer::NewLC(KSsmServerNameTest, ESsmTestSimpleState);
	CleanupStack::PopAndDestroy(server);
	__UHEAP_MARKEND;
	
	CleanupStack::PopAndDestroy(sched);
	}
	
void CSsmServerTest::doTestHeapFailureL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );

	// CSsmServer
	INFO_PRINTF1(_L("CSsmServer construction under OOM"));
	CSsmServer* server=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, 
	    	{
	    	server = CSsmServer::NewLC(KSsmServerNameTest, ESsmTestSimpleState);
	    	CleanupStack::Pop(server);
	    	})
	    		   
	    
        TEST((ret==KErrNone || ret==KErrNoMemory));
	    if (ret!=KErrNone)
	        {
	        __UHEAP_MARKEND;
	        TEST(server==NULL);
	        }
	    else
	        {
	        TEST(server!=NULL);
	        delete server;
	        server = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2(_L("  #allocs for c'tion: %d."),failRate-1);

	CleanupStack::PopAndDestroy(sched);
	}
	
void CSsmServerTest::doTestProcessCriticalL()
	{
	INFO_PRINTF1(_L("Checking that SsmServer sets it thread to critical"));
	
	User::TCritical before = User::ProcessCritical();
	TESTL(before == User::ENotCritical);
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	CSsmServer* server = CSsmServer::NewLC(KSsmServerNameTest, ESsmTestSimpleState);
	
	User::TCritical critical = User::Critical();
	TEST(critical == User::ESystemCritical);

	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(sched);
	
	User::TCritical after = User::Critical();
	TESTL(after == User::ENotCritical);
	}

/**
Old Test CaseID 		APPFWK-SSM-0003
New Test CaseID 		DEVSRVS-SSMA-SSM-0001
 */

void CSsmServerTest::doTestConnectedSessionL()
	{
	INFO_PRINTF1(_L("Testing basic connection, wrong version, connect twice etc."));
	
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));
	
	RSsmStateManagerTestClient client1;
	RSsmStateManagerTestClient client2;

	//Call connect with invalid version
	TVersion future(KSsmServMajorVersionNumber+1, KSsmServMinorVersionNumber, KSsmServBuildVersionNumber);
	TInt connect = client1.Connect(KSsmServerNameTest, future);
	TEST(KErrNotSupported == connect);

	//Call connect with invalid server name
	_LIT(KSsmServerWrongNameTest, "wrongname");
	connect = client1.Connect(KSsmServerWrongNameTest, TVersion(KSsmServMajorVersionNumber, KSsmServMinorVersionNumber, KSsmServBuildVersionNumber));
	TEST(KErrNotFound == connect);
			
	//Call connect twice
	connect = client1.Connect();
	TEST(KErrNone == connect);
	connect = client1.Connect();
	TEST(KErrAlreadyExists == connect);
	
	//Call cancel methods without having requested anything
	client1.RequestStateTransitionCancel();
	client1.RequestSwpChangeCancel();
	
	//Call Close() twice, so second one is trying to close an already closed client -> should not panic.
	client1.Close();
	client1.Close();
	
	//Call RequestStateTransition on a non-connected client
	TSsmStateTransition state(ESsmStartup, KSsmAnySubState, 0);
	TRequestStatus status1;
	client1.RequestStateTransition(state, status1);
	TEST(status1 == KErrDisconnected);
	TEST(client1.RequestStateTransition(state) == KErrDisconnected);
	
	//Call RequestSwpTransition on a non-connected client
	const TUint key = {666999};
	TSsmSwp swp(key, 666999);
	TRequestStatus status2;
	client1.RequestSwpChange(swp, status2);
	TEST(status2 == KErrDisconnected);
	TEST(client1.RequestSwpChange(swp) == KErrDisconnected);

	//Call RegisterSwpMapping on a non-connected client
	TEST(KErrDisconnected == client1.RequestSwpChange(swp));
	
	// void methods should not cause a Panic or any other problem
	client1.RequestStateTransitionCancel();
	client1.RequestSwpChangeCancel();	

	//Connect 2 different sessions with same server
	TEST(KErrNone == client1.Connect());
	TEST(KErrNone == client2.Connect());
	
	//Calling RegisterSwpMapping with same key in different clients (even with different policy files)
	// Note that keys are kept in a common table, and it is independent of which client tries to register that key
	TEST(KErrNone == client1.RegisterSwpMapping(swp.Key(), KTestSwpPolicySimpleFile));
	TEST(KErrAlreadyExists == client2.RegisterSwpMapping(swp.Key(), KTestSwpPolicySimpleFile));
	TEST(KErrAlreadyExists == client2.RegisterSwpMapping(swp.Key(), KTestWrongPolicyFile));

	//Disconnect client 1 and try to register again same key now with client2 (table still contains key)
	client1.Close();
	TEST(KErrAlreadyExists == client2.RegisterSwpMapping(swp.Key(), KTestSwpPolicySimpleFile));
	
	//Disconnect client 2 and connect it again, trying to register again same key (table still contains key)
	client2.Close();
	TEST(KErrNone == client2.Connect());
	TEST(KErrAlreadyExists == client2.RegisterSwpMapping(swp.Key(), KTestSwpPolicySimpleFile));
	TEST(KErrDisconnected == client1.RegisterSwpMapping(swp.Key(), KTestSwpPolicySimpleFile));

	//Calling RegisterSwpMapping with a laaarge filename (must be > 256 characters long, in this case it's 259 characters long)
	_LIT(KTestLongPolicyFile, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdefXYZ");
	TEST(KErrArgument == client2.RegisterSwpMapping(swp.Key(), KTestLongPolicyFile));
	
	client2.Close(); //client1 already closed above.
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}

/**
Old Test CaseID 		APPFWK-SSM-0014
New Test CaseID 		DEVSRVS-SSMA-SSM-0001
 */

void CSsmServerTest::doSwpHandlingTestsL()
	{
	//	This is used to wait for the swp request completion.
	iSemaphore.CreateGlobal(KRequestCompleteSem, 1);
	doTestMultipleSwpRequestsL();
	doTestErrorenousSwpPolicyL();
	doTestCancelSwpRequestsQueuedL();
	doTestCancelSwpRequestsRunningL();
	iSemaphore.Close();
	}

void CSsmServerTest::doTestMultipleSwpRequestsL()
	{
	INFO_PRINTF1(_L("Doing 3 concurrent async RequestSwpChange"));
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));	
	RSsmStateManagerTestClient client;
	
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);
	client.HeapMark();
	
	const TUint key = {666999};
	TESTL(KErrNone == client.RegisterSwpMapping(key, KTestSwpPolicySimpleFile));
	
	//Request 3 swp transitions in the same session. 
	TSsmSwp swp(key,666999);
	TRequestStatus status1;
	client.RequestSwpChange(swp, status1);
	TEST(status1 == KRequestPending);
	
	swp.Set(key, swp.Value()+1);
	TRequestStatus status2;
	client.RequestSwpChange(swp, status2);
	TEST(status2 == KRequestPending);

	swp.Set(key, swp.Value()+1);
	TRequestStatus status3;
	client.RequestSwpChange(swp, status3);
	TEST(status2 == KRequestPending);
	
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	
	// Wait on the semaphore until the HandleCleReturnValue of the last
	// swp request is called.
	iSemaphore.Wait();
	//Even after the HandleCleReturnValue is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(5*100*1000);

	INFO_PRINTF4(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d)"), status1.Int(), status2.Int(), status3.Int());
	
	TEST(status1.Int() == KErrNone);
	TEST(status2.Int() == KErrNone);
	TEST(status3.Int() == KErrNone);
	
	client.RequestDeRegisterSwpMappingL(key, KTestSwpPolicySimpleFile);
	client.HeapMarkEnd(EDoCleanup);
	client.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}


void CSsmServerTest::doTestErrorenousSwpPolicyL()
	{
	INFO_PRINTF1(_L("Requesting async RequestSwpChange with a Swp registered with a missing Swp policy or an unregistered Swp"));
	
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));	
	RSsmStateManagerTestClient client;
	
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);
	client.HeapMark();
	
	// RegisterSwpMapping does not perform any check, so it should accept wrong dll files.
	const TUint valid = {666999};
	const TUint errorenous = {66669999};
	const TUint unregistered = {6699};
	TESTL(KErrNone == client.RegisterSwpMapping(valid, KTestSwpPolicySimpleFile));
	TESTL(KErrNone == client.RegisterSwpMapping(errorenous, KTestWrongPolicyFile));
	
	//Request 3 swp transitions in the same session. 
	TSsmSwp swp(valid,666999);
	TRequestStatus status1;
	client.RequestSwpChange(swp, status1);
	TEST(status1 == KRequestPending);

	swp.Set(errorenous, swp.Value()+1);
	TRequestStatus status2;
	client.RequestSwpChange(swp, status2);
	TEST(status2 == KRequestPending);

	swp.Set(unregistered, swp.Value()+1);
	TRequestStatus status3;
	client.RequestSwpChange(swp, status3);
	TEST(status3 == KRequestPending);
	
	swp.Set(valid, KTestNotAllowedValue);
	TRequestStatus status4;
	client.RequestSwpChange(swp, status4);
	TEST(status4 == KRequestPending);

	swp.Set(valid, swp.Value()+1);
	TRequestStatus status5;
	client.RequestSwpChange(swp, status5);
	TEST(status5 == KRequestPending);
	
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	User::WaitForRequest(status4);
	User::WaitForRequest(status5);
	
	INFO_PRINTF6(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d), req4 (%d), req5 (%d)"), status1.Int(), status2.Int(), status3.Int(), status4.Int(), status5.Int());
	
	TEST(status1.Int() == KErrNone);
	TEST(status2.Int() == KErrNotFound);
	TEST(status3.Int() == KErrNotFound);
	TEST(status4.Int() == KErrNotSupported);
	TEST(status5.Int() == KErrNone);

	// Wait on the semaphore until the HandleCleReturnValue of the last
	// swp request is called.
	iSemaphore.Wait();
	//Even after the HandleCleReturnValue is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(5*100*1000);

	client.RequestDeRegisterSwpMappingL(valid, KTestSwpPolicySimpleFile);
	client.RequestDeRegisterSwpMappingL(errorenous, KTestWrongPolicyFile);
	client.HeapMarkEnd(EDoCleanup);
	client.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}

void CSsmServerTest::doTestCancelSwpRequestsQueuedL()
	{
	INFO_PRINTF1(_L("Requesting async RequestSwpChange from 2 sessions, canceling the second."));
	
	//Start test-server
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));	
	
	//Connect client1
	RSsmStateManagerTestClient client1;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();
	
	//Connect client2
	RSsmStateManagerTestClient client2;
	TEST(KErrNone == client2.Connect());
	
	//Register mapping between key and policy DLL
	const TUint key = {666999};
	TESTL(KErrNone == client1.RegisterSwpMapping(key, KTestSwpPolicySimpleFile));
	
	//Let client1 request its first swp change
	TSsmSwp swp(key,1);
	TRequestStatus status1;
	client1.RequestSwpChange(swp, status1);
	TEST(status1 == KRequestPending);
	
	//Let client2 request 2 swp changes;
	swp.Set(key, swp.Value()+1);
	TRequestStatus status2;
	client2.RequestSwpChange(swp, status2);
	TEST(status2 == KRequestPending);
	
	swp.Set(key, swp.Value()+1);
	TRequestStatus status3;
	client2.RequestSwpChange(swp, status3);
	TEST(status3 == KRequestPending);
		
	//Let client1 request its second swp change
	swp.Set(key, swp.Value()+1);
	TRequestStatus status4;
	client1.RequestSwpChange(swp, status4);
	TEST(status4 == KRequestPending);

	//Let client 2 Cancel its 2 requests (client 2 has both requests on queue)
	client2.RequestSwpChangeCancel(); 

	//Because the Cancel-request is the last made and it is synchronous, we don't need to wait for client2 status'es.
	//Make sure client2's requestd were the only that were cancelled
	User::WaitForRequest(status4);
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	
	TEST(status2.Int() == KErrCancel);
	TEST(status3.Int() == KErrCancel);
	TEST(status1.Int() == KErrNone);
	TEST(status4.Int() == KErrNone);

	INFO_PRINTF5(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d), req4 (%d)"), status1.Int(), status2.Int(), status3.Int(), status4.Int());
	// Wait on the semaphore until the HandleCleReturnValue of the last
	// swp request is called.
	iSemaphore.Wait();
	//Even after the HandleCleReturnValue is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(5*100*1000);
	
	client1.RequestDeRegisterSwpMappingL(key, KTestSwpPolicySimpleFile);
	client2.Close();
	client1.HeapMarkEnd(EDoCleanup);
	client1.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}

void CSsmServerTest::doTestCancelSwpRequestsRunningL()
	{
	INFO_PRINTF1(_L("Requesting async RequestSwpChange from 2 sessions, canceling the first."));
	
	//Start test-server
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));	
	
	//Connect client1
	RSsmStateManagerTestClient client1;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();
	
	//Connect client2
	RSsmStateManagerTestClient client2;
	TEST(KErrNone == client2.Connect());
	
	//Register mapping between key and policy DLL
	const TUint key = {666999};
	TESTL(KErrNone == client1.RegisterSwpMapping(key, KTestSwpPolicySimpleFile));
	
	//Let client1 request its first swp change
	TSsmSwp swp(key,1);
	TRequestStatus status1;
	client1.RequestSwpChange(swp, status1);
	TEST(status1 == KRequestPending);
	
	//Let client2 request 2 swp changes;
	swp.Set(key, swp.Value()+1);
	TRequestStatus status2;
	client2.RequestSwpChange(swp, status2);
	TEST(status2 == KRequestPending);
	
	swp.Set(key, swp.Value()+1);
	TRequestStatus status3;
	client2.RequestSwpChange(swp, status3);
	TEST(status3 == KRequestPending);
		
	//Let client1 request its second swp change
	swp.Set(key, swp.Value()+1);
	TRequestStatus status4;
	client1.RequestSwpChange(swp, status4);
	TEST(status4 == KRequestPending);

	//Let client 1 Cancel its 2 requests (client 1 has a running request)
	client1.RequestSwpChangeCancel(); 

	//Because the Cancel-request is the last made and it is synchronous, we don't need to wait for client1 status'es.
	//Make sure client1's requestd were the only that were cancelled
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	User::WaitForRequest(status4);
	TEST(status1.Int() == KErrCancel);
	TEST(status4.Int() == KErrCancel);
	TEST(status2.Int() == KErrNone);
	TEST(status3.Int() == KErrNone);

	INFO_PRINTF5(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d), req4 (%d)"), status1.Int(), status2.Int(), status3.Int(), status4.Int());

	// Wait on the semaphore until the HandleCleReturnValue of the last
	// swp request is called.
	iSemaphore.Wait();
	//Even after the HandleCleReturnValue is completed the server requires some time to
	// to mark the individual scheduler as Idle, so this delay is required.  There is no other
	// synchronization mechansim to know the completion.
	User::After(5*100*1000);

	client1.RequestDeRegisterSwpMappingL(key, KTestSwpPolicySimpleFile);
	client2.Close();
	client1.HeapMarkEnd(EDoCleanup);
	client1.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}

/**
Old Test CaseID 		APPFWK-SSM-0013
New Test CaseID 		DEVSRVS-SSMA-SSM-0001
 */

void CSsmServerTest::doStateHandlingTestsL()
	{
	doTestMultipleStateRequestsL();
	doTestFailStateRequestsL();
	doTestValidStateRequestsCancelCurrentL();
	doTestValidStateRequestsCancelQueuedL();
	}

void CSsmServerTest::doTestMultipleStateRequestsL()
	{
	INFO_PRINTF1(_L("Doing multiple concurrent async RequestStateTransition"));
	
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));	
	RSsmStateManagerTestClient client;
	
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);
	client.HeapMark();
	
	//Request 3 state transitions in the same session. 
	TSsmStateTransition state1(ESsmTestSimpleState,123,0);
	TRequestStatus status1;
	client.RequestStateTransition(state1, status1);				// policy returns EDefinitelyAllowed
	TEST(status1 == KRequestPending);
	
	TSsmStateTransition state2(ESsmTestSimpleState,234,0);		// policy returns EDefinitelyAllowed
	TRequestStatus status2;
	client.RequestStateTransition(state2, status2);
	TEST(status2 == KRequestPending);

	TSsmStateTransition state3(ESsmTestSimpleState,456,0);		// policy returns ECurrentRemainReplaceQueued
	TRequestStatus status3;
	client.RequestStateTransition(state3, status3);
	TEST(status3 == KRequestPending);
	
	TSsmStateTransition state4(ESsmTestSimpleState,789,KTestNotAllowedReason);	// policy returns ENotAllowed
	TRequestStatus status4;
	client.RequestStateTransition(state4, status4);
	TEST(status4 == KRequestPending);
	
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	User::WaitForRequest(status4);
	
	INFO_PRINTF5(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d), req4 (%d)"), status1.Int(), status2.Int(), status3.Int(), status4.Int());
	
	TEST(status1.Int() == KErrNone || status1.Int() == KErrCancel); //If the initial transition while creating the server is not completed, this will be KErrCancel.
	TEST(status2.Int() == KErrCancel);		// it is replaced in the queue by the third request
	TEST(status3.Int() == KErrNone);
	TEST(status4.Int() == KErrNotSupported);
	
	client.HeapMarkEnd(EDoCleanup);
	client.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}

void CSsmServerTest::doTestFailStateRequestsL()
	{
	INFO_PRINTF1(_L("Doing 2 concurrent async RequestStateTransition, then request to fail"));
	
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));	
	RSsmStateManagerTestClient client;
	
	const TInt connect = client.Connect();
	TEST(KErrNone == connect);
	client.HeapMark();	
	
	//Request 2 state transitions in the same session. 
	TSsmStateTransition state1(ESsmTestSimpleState,123,0);
	TRequestStatus status1;
	client.RequestStateTransition(state1, status1);
	TEST(status1 == KRequestPending);
	
	TSsmStateTransition state2(ESsmTestSimpleState,234,0);
	TRequestStatus status2;
	client.RequestStateTransition(state2, status2);
	TEST(status2 == KRequestPending);

	//Request to fail
	TSsmStateTransition state3(ESsmTestFailState, KSsmAnySubState, 0);
	TRequestStatus status3;
	client.RequestStateTransition(state3, status3);
	TEST(status3 == KRequestPending);
	
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	
	INFO_PRINTF4(_L("  --completion codes:  req1 (%d), req2 (%d), req3 (%d)"), status1.Int(), status2.Int(), status3.Int());
	
	TEST(status1.Int() == KErrCancel);
	TEST(status2.Int() == KErrCancel);
	TEST(status3.Int() == KErrNone);
	
	client.HeapMarkEnd(EDoCleanup);		//Pass EDoCleanup for cleaning up Transition Engine after the test case 	
	client.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}

void CSsmServerTest::doTestValidStateRequestsCancelCurrentL()
	{
	INFO_PRINTF1(_L("Testing to cancel first scheduled state requests"));
	
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));	
	
	//Connect clients
	RSsmStateManagerTestClient client1;
	RSsmStateManagerTestClient client2;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();
	TEST(KErrNone == client2.Connect());
		
	//Request first state transitions from client1 
	TSsmStateTransition state1(ESsmTestSimpleState,123,0);
	TRequestStatus status1;
	client1.RequestStateTransition(state1, status1);
	TEST(status1 == KRequestPending);
	
	//Request second state transitions from client2
	TSsmStateTransition state2(ESsmTestSimpleState,234,0);
	TRequestStatus status2;
	client2.RequestStateTransition(state2, status2);
	TEST(status2 == KRequestPending);
	
	//Cancel client1's request
	client1.RequestStateTransitionCancel();
	
	INFO_PRINTF3(_L("  --completion codes:  req1 (%d), req2 (%d)"), status1.Int(), status2.Int());
	
	//Because the Cancel-request is the last made and it is synchronous, 
	//we don't need to do User::WaitForRequest(status1) etc.
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
		
	TEST(status1.Int() == KErrCancel);
	TEST(status2.Int() == KErrNone);

	client2.Close();
	client1.HeapMarkEnd(EDoCleanup);		//Pass EDoCleanup for cleaning up Transition Engine after the test case 	
	client1.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}

void CSsmServerTest::doTestValidStateRequestsCancelQueuedL()
	{
	INFO_PRINTF1(_L("Testing to cancel queued state requests"));
	
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread));	
	
	//Connect clients
	RSsmStateManagerTestClient client1;
	RSsmStateManagerTestClient client2;
	TEST(KErrNone == client1.Connect());
	client1.HeapMark();
	TEST(KErrNone == client2.Connect());
	
	//Request first state transitions from client1 
	TSsmStateTransition state1(ESsmTestSimpleState,123,0);
	TRequestStatus status1;
	client1.RequestStateTransition(state1, status1);
	TEST(status1 == KRequestPending);
	
	//Request second state transitions from client2
	TSsmStateTransition state2(ESsmTestSimpleState,234,0);
	TRequestStatus status2;
	client2.RequestStateTransition(state2, status2);
	TEST(status2 == KRequestPending);
	
	//Cancel client2's request
	client2.RequestStateTransitionCancel();
	
	INFO_PRINTF3(_L("  --completion codes:  req1 (%d), req2 (%d)"), status1.Int(), status2.Int());
	
	//Because the Cancel-request is the last made and it is synchronous, 
	//we don't need to do User::WaitForRequest(status1) etc.
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);	
	
	TEST(status1.Int() == KErrNone);
	TEST(status2.Int() == KErrCancel);
	
	client2.Close();
	client1.HeapMarkEnd(EDoCleanup);
	client2.Close();
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	}
