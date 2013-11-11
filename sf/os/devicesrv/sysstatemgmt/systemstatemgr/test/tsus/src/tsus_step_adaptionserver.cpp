// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "susadaptionserver.h"
#include <ssm/ssmadaptationcli.h>
#include "tsus_step_adaptionserver.h"
#include "tsus_startadaptationserver.h"

#ifdef TEST_SSM_SERVER
/**
 * Overloaded function to Connect to the Test Adaptation server
 */
TInt RTestSusAdaptationCli::Connect(const TDesC& aServerName)
    {
    return RSsmEmergencyCallRfAdaptation::Connect(aServerName);
    }
#endif //TEST_SSM_SERVER
/**
 * Client interface to simulate Heap failure at server side
 */
TInt RTestSusAdaptationCli::SetHeapFailure(TInt aFailureRate)
    {
    if(Handle())
        {
        return SendReceive(EDebugSetHeapFailure, TIpcArgs(aFailureRate));
        }
    return KErrDisconnected;
    }

/**
 * Client interface to restore Heap failure at server side
 */
TInt RTestSusAdaptationCli::UnSetHeapFailure()
    {
    if(Handle())
        {
        return SendReceive(EDebugUnSetHeapFailure);
        }
    return KErrDisconnected;
    }

TInt RTestSusAdaptationCli::SetHeapMark()
    {
    if(Handle())
        {
        return SendReceive(EDebugMarkHeap);
        }
    return KErrDisconnected;
    }

/**
 * Client interface to restore Heap Mark
 */
TInt RTestSusAdaptationCli::UnSetHeapMark()
    {
    if(Handle())
        {
        return SendReceive(EDebugMarkHeapEnd);
        }
    return KErrDisconnected;
    }
 
CSusAdaptionServerTest::~CSusAdaptionServerTest()
	{
	}

CSusAdaptionServerTest::CSusAdaptionServerTest()
	{
	SetTestStepName(KTSusAdaptionServerStep);
	}

/** */
TVerdict CSusAdaptionServerTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/** 
 Old Test CaseID 		APPFWK-SUS-0011
 New Test CaseID 		DEVSRVS-SSMA-SUS-0011
 */

TVerdict CSusAdaptionServerTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSusAdaptionServerTest tests started...."));

	__UHEAP_MARK;		
	TRAPD(err, doTestForMemoryLeaksL());
	TEST(err == KErrNone);

	TRAP(err, doTestProcessCriticalL());
	TEST(KErrNone == err || KErrAlreadyExists == err);
	TRAP(err, doTestOOML());
	TEST(err == KErrNone);	
#ifdef TEST_SSM_SERVER	
	doTestForNormalEmergencyCallinOOM();
	doTestForEmergencyCallOOM();
	doTestForSettingPriorityClient();
	doTestForMultipleClientinOOMcondition();
	doTestForEmergencyCallOOMIterative();
#endif //TEST_SSM_SERVER
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("....CSusAdaptionServerTest tests completed!"));
	return TestStepResult();	
	}

/** */
TVerdict CSusAdaptionServerTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


void CSusAdaptionServerTest::doTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("doTestForMemoryLeaksL tests started...."));
	
	INFO_PRINTF1(_L("Checking for memoryleaks in SusAdaptionServer destructor"));

	INFO_PRINTF1(_L("Create active scheduler."));
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	
	__UHEAP_MARK;
	StartAndDestroyServerL();
	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("Destroy active scheduler."));
	CleanupStack::PopAndDestroy(sched);

	INFO_PRINTF1(_L("doTestForMemoryLeaksL tests completed."));
	}


	
void CSusAdaptionServerTest::doTestProcessCriticalL()
	{
	INFO_PRINTF1(_L("doTestProcessCriticalL tests started...."));

	INFO_PRINTF1(_L("Checking that SusAdaptionServer sets it thread to critical"));
	
	INFO_PRINTF1(_L("Get the critical state of the current thread"));
	User::TCritical before = User::Critical();
	INFO_PRINTF3(_L("The critical state of the current thread: expected %d, actual %d"), User::ENotCritical, before);
	TESTL(before == User::ENotCritical);
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	INFO_PRINTF1(_L("Create SSM adaptation server."));
	CSsmAdaptationServer* server = CSsmAdaptationServer::NewLC();
	
	INFO_PRINTF1(_L("Get the critical state of the current thread"));
	User::TCritical critical = User::Critical();
	INFO_PRINTF3(_L("The critical state of the current thread: expected %d, actual %d"), User::ESystemCritical, critical);
	TEST(critical == User::ESystemCritical);

	INFO_PRINTF1(_L("Destroy SSM adaptation server."));
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(sched);
	
	User::TCritical after = User::Critical();
	INFO_PRINTF3(_L("The critical state of the current thread: expected %d, actual %d"), User::ENotCritical, after);
	TESTL(after == User::ENotCritical);

	INFO_PRINTF1(_L("doTestProcessCriticalL tests completed."));
	}
	
void CSusAdaptionServerTest::doTestOOML()
	{
	INFO_PRINTF1(_L("doTestOOML tests started...."));

	INFO_PRINTF1(_L("Create active scheduler."));
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	
	TInt ret = KErrNoMemory;
	for(TInt fail=0; ret == KErrNoMemory; fail++)
		{
		__UHEAP_FAILNEXT(fail);
		__UHEAP_MARK;
		INFO_PRINTF2(_L("Iteration #: %d"), fail);
		TRAP(ret, StartAndDestroyServerL());
		INFO_PRINTF2(_L("StartAndDestroyServerL() returned: %d"), ret);
		__UHEAP_SETFAIL(RHeap::ENone,0);// turn failure off
		__UHEAP_MARKEND;
		TEST((ret==KErrNoMemory || ret==KErrNone));
		}

	INFO_PRINTF1(_L("Destroy active scheduler."));
	CleanupStack::PopAndDestroy(sched);
	INFO_PRINTF1(_L("doTestOOML tests completed."));
	}

void CSusAdaptionServerTest::StartAndDestroyServerL()
	{
	INFO_PRINTF1(_L("Create SSM adaptation server."));
	CSsmAdaptationServer* server = NULL;
	TRAPD(err, server = CSsmAdaptationServer::NewLC(); CleanupStack::Pop(server));
	TEST(err == KErrAlreadyExists || err == KErrNone);
	if(err != KErrNone && err != KErrAlreadyExists)
		User::LeaveIfError(err);
	INFO_PRINTF1(_L("Destroy SSM adaptation server."));
	delete server;
	}
#ifdef TEST_SSM_SERVER
/**
 * Tests for client calling Activate/Deactivate Rf without setting as priorityclient.
 */
void CSusAdaptionServerTest::doTestForNormalEmergencyCallinOOM()
    {
    INFO_PRINTF1(_L("doTestForNormalEmergencyCallinOOM started."));
    __UHEAP_MARK;
    RThread thread;
    CleanupClosePushL(thread);
    TESTL(KErrNone == StartAdaptationServer(thread));
      
    RTestSusAdaptationCli adaptationclitest;    
    TInt err = adaptationclitest.Connect(KTestAdaptationServerName);
    INFO_PRINTF2(_L("Connect() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    TRequestStatus status;
    // Simulate OOM condition
    User::__DbgSetAllocFail(ETrue, RAllocator::EDeterministic, 1);
    //Simulate OOM at Serverside
    adaptationclitest.SetHeapFailure(1);
    adaptationclitest.SetHeapMark();
    //Normal client calling ActivateRfForEmergencyCall Rf in OOM condition. This call must fail with KErrNoMemory 
    //as there is no memory reserved.
    adaptationclitest.ActivateRfForEmergencyCall(status);
    // Wait for completion of requests
    User::WaitForRequest(status);    
    //Restore OOM condition
    adaptationclitest.UnSetHeapFailure();
    adaptationclitest.UnSetHeapMark();
    adaptationclitest.Close();    
    User::__DbgSetAllocFail(ETrue, RAllocator::ENone, 1);
    TEST(status.Int() == KErrNoMemory);
    thread.Kill(KErrNone);
    CleanupStack::PopAndDestroy(&thread);
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("doTestForNormalEmergencyCallinOOM completed.")); 
    }


/**
 * Tests for performing ActivateRfForEmergencyCall + DeactivateRfForEmergencyCall 
 *  during OOM condition by priority clients.
 */
void CSusAdaptionServerTest::doTestForEmergencyCallOOM()
    {
    INFO_PRINTF1(_L("doTestForEmergencyCallOOM started."));
    __UHEAP_MARK;
    RThread thread;
    CleanupClosePushL(thread); 
    TESTL(KErrNone == StartAdaptationServer(thread));
    RTestSusAdaptationCli adaptationclitest;    
    TInt err = adaptationclitest.Connect(KTestAdaptationServerName);
    INFO_PRINTF2(_L("Connect() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);    
    err = adaptationclitest.SetAsPriorityClient();
    adaptationclitest.SetHeapMark();
    INFO_PRINTF2(_L("SetAsPriorityClient() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    TRequestStatus status1,status2,status3,status4,status5,status6,status7; 
    adaptationclitest.ActivateRfForEmergencyCall(status1); 
    adaptationclitest.ActivateRfForEmergencyCall(status2);
    adaptationclitest.ActivateRfForEmergencyCall(status3);
    //Wait for request to complete
    User::WaitForRequest(status1);
    User::WaitForRequest(status2);
    User::WaitForRequest(status3);
    TEST(status1.Int() == KErrNone);
    TEST(status2.Int() == KErrNone);
    TEST(status3.Int() == KErrNone);
    // Simulate OOM condition
    User::__DbgSetAllocFail(ETrue, RAllocator::EDeterministic, 1);
    //Simulate OOM at Serverside
    adaptationclitest.SetHeapFailure(1);
 
    //Two simulataneous calls for Active/deactivate Rf are allowed. The 3rd simultaneous request fail with KErrNoMemory
    adaptationclitest.ActivateRfForEmergencyCall(status1); 
    adaptationclitest.DeactivateRfForEmergencyCall(status2);    
    adaptationclitest.ActivateRfForEmergencyCall(status3);
    // Wait for completion of requests
    User::WaitForRequest(status1);
    User::WaitForRequest(status2);
    User::WaitForRequest(status3);
  
    //If the previous request for Activate/Deactivate is already finished. User can perform 2 more calls.
    adaptationclitest.ActivateRfForEmergencyCall(status4); 
    adaptationclitest.DeactivateRfForEmergencyCall(status5);
    
    User::WaitForRequest(status4);
    User::WaitForRequest(status5);
    
    adaptationclitest.ActivateRfForEmergencyCall(status6); 
    adaptationclitest.DeactivateRfForEmergencyCall(status7);  
    adaptationclitest.Cancel();
    // Wait for completion of requests
    User::WaitForRequest(status6);
    User::WaitForRequest(status7);
    
    //Restore OOM condition
    adaptationclitest.UnSetHeapFailure();
    User::__DbgSetAllocFail(ETrue, RAllocator::ENone, 1);
    TEST(status1.Int() == KErrNone);
    TEST(status2.Int() == KErrNone);
    TEST(status3.Int() == KErrNoMemory);
    TEST(status4.Int() == KErrNone);
    TEST(status5.Int() == KErrNone);
    TEST(status6.Int() == KErrNone);
    TEST(status7.Int() == KErrCancel);    
    adaptationclitest.UnSetHeapMark();
    adaptationclitest.Close();
    thread.Kill(KErrNone);
    CleanupStack::PopAndDestroy(&thread);
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("doTestForEmergencyCallOOM completed."));        
    }

/**
 * Tests to Set PriorityClient from two different clients.
 */
void CSusAdaptionServerTest::doTestForSettingPriorityClient()
    {
    INFO_PRINTF1(_L("doTestForSettingPriorityClient started."));
    __UHEAP_MARK;
    RThread thread;
    CleanupClosePushL(thread);
    TESTL(KErrNone == StartAdaptationServer(thread));
    RTestSusAdaptationCli adaptationclitest1;
    TInt err = adaptationclitest1.Connect(KTestAdaptationServerName);
    INFO_PRINTF2(_L("adaptationclitest1.Connect() returned Error %d : Expected Error is KErrNone."),err);   
    TEST(err == KErrNone);
    
    RTestSusAdaptationCli adaptationclitest2;
    err = adaptationclitest2.Connect(KTestAdaptationServerName);
    INFO_PRINTF2(_L("adaptationclitest2.Connect() returned Error %d : Expected Error is KErrNone."),err);   
    TEST(err == KErrNone);
       
    err = adaptationclitest1.SetAsPriorityClient();
    INFO_PRINTF2(_L("adaptationclitest1.SetAsPriorityClient() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    
    err = adaptationclitest2.SetAsPriorityClient();
    INFO_PRINTF2(_L("adaptationclitest2.SetAsPriorityClient() returned Error %d : Expected Error is KErrAlreadyExists."),err);
    TEST(err == KErrAlreadyExists);
    
    // Restore OOM condition
    adaptationclitest1.Close();
    adaptationclitest2.Close();
    
    thread.Kill(KErrNone);
    CleanupStack::PopAndDestroy(&thread);
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("doTestForSettingPriorityClient completed."));
    }

/**
 * Multiple clients performing Emergency call during OOM situation
 */
void CSusAdaptionServerTest::doTestForMultipleClientinOOMcondition()
    {
    INFO_PRINTF1(_L("doTestForMultipleClientinOOMcondition started."));
    __UHEAP_MARK;
    RThread thread;
    CleanupClosePushL(thread); 
    
    TESTL(KErrNone == StartAdaptationServer(thread));
    RTestSusAdaptationCli adaptationclitest1;
    TInt err = adaptationclitest1.Connect(KTestAdaptationServerName);
    INFO_PRINTF2(_L("adaptationclitest1.Connect() returned Error %d : Expected Error is KErrNone."),err);   
    TEST(err == KErrNone);
    
    RTestSusAdaptationCli adaptationclitest2;
    err = adaptationclitest2.Connect(KTestAdaptationServerName);
    INFO_PRINTF2(_L("adaptationclitest2.Connect() returned Error %d : Expected Error is KErrNone."),err);   
    TEST(err == KErrNone);
    
    RTestSusAdaptationCli adaptationclitest3;
    err = adaptationclitest3.Connect(KTestAdaptationServerName);
    INFO_PRINTF2(_L("adaptationclitest3.Connect() returned Error %d : Expected Error is KErrNone."),err);   
    TEST(err == KErrNone);
       
    err = adaptationclitest1.SetAsPriorityClient();
    INFO_PRINTF2(_L("adaptationclitest1.SetAsPriorityClient() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    
    err = adaptationclitest2.SetAsPriorityClient();
    INFO_PRINTF2(_L("adaptationclitest2.SetAsPriorityClient() returned Error %d : Expected Error is KErrAlreadyExists."),err);
    TEST(err == KErrAlreadyExists);
    
    err = adaptationclitest3.SetAsPriorityClient();
    INFO_PRINTF2(_L("adaptationclitest3.SetAsPriorityClient() returned Error %d : Expected Error is KErrAlreadyExists."),err);
    TEST(err == KErrAlreadyExists);
    
    TRequestStatus status1,status2,status3,status4,status5,status6;
    // Simulate OOM condition
    User::__DbgSetAllocFail(ETrue, RAllocator::EDeterministic, 1);
    //Simulate OOM Condition on server side
    adaptationclitest1.SetHeapFailure(1);
    adaptationclitest1.SetHeapMark();
    adaptationclitest1.ActivateRfForEmergencyCall(status1);
    adaptationclitest2.ActivateRfForEmergencyCall(status2);
    adaptationclitest3.ActivateRfForEmergencyCall(status3);
    adaptationclitest1.DeactivateRfForEmergencyCall(status4);
    adaptationclitest2.DeactivateRfForEmergencyCall(status5);
    adaptationclitest3.DeactivateRfForEmergencyCall(status6);
    
    User::WaitForRequest(status1);
    User::WaitForRequest(status2);
    User::WaitForRequest(status3);
    User::WaitForRequest(status4);
    User::WaitForRequest(status5);
    User::WaitForRequest(status6);    
    // Restore OOM condition
    adaptationclitest1.Close();
    adaptationclitest2.Close();
    adaptationclitest3.Close();
    //Restore OOM condition
    adaptationclitest1.UnSetHeapMark();
    adaptationclitest1.UnSetHeapFailure();
    User::__DbgSetAllocFail(ETrue, RAllocator::ENone, 1);
    //Priority client Performing Activate Rf returns KErrNone
    TEST(status1.Int() == KErrNone);
    //Normal clients performing Activate Rf fail with KErrNoMemory in OOM codition
    TEST(status2.Int() == KErrNoMemory);
    TEST(status3.Int() == KErrNoMemory);
    //Priority client Performing Deactivate Rf returns KErrNone
    TEST(status4.Int() == KErrNone);
    //Normal clients peforming Deactivate Rf fail with KErrNoMemory in OOM codition
    TEST(status5.Int() == KErrNoMemory);
    TEST(status6.Int() == KErrNoMemory);
    thread.Kill(KErrNone);
    CleanupStack::PopAndDestroy(&thread);
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("doTestForMultipleClientinOOMcondition completed."));
    }

/**
 * Tests for iterating heapfailure in ActivateRfForEmergencyCall and DeactivateRfForEmergencyCall.
 */
void CSusAdaptionServerTest::doTestForEmergencyCallOOMIterative()
    {
    INFO_PRINTF1(_L("doTestForEmergencyCallOOMIterative started."));
    __UHEAP_MARK;
    RThread thread;
    CleanupClosePushL(thread);
    TESTL(KErrNone == StartAdaptationServer(thread));
    RTestSusAdaptationCli adaptationclitest;    
    TInt err = adaptationclitest.Connect(KTestAdaptationServerName);
    INFO_PRINTF2(_L("Connect() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);    
    err = adaptationclitest.SetAsPriorityClient();
    INFO_PRINTF2(_L("SetAsPriorityClient() returned Error %d : Expected Error is KErrNone."),err);
    TEST(err == KErrNone);
    TRequestStatus status1,status2;
    //simulate OOM at kernel
    User::__DbgSetAllocFail(ETrue, RAllocator::EDeterministic, 1);
    TInt maximumAllocation = 3;
    //Iterate through the ActivateRfForEmergencyCall.
    adaptationclitest.SetHeapMark();
    for(TInt allocFailRate=1; allocFailRate <= 2*maximumAllocation; ++allocFailRate)
        {
        INFO_PRINTF2(_L("allocFailRateL %d:."), allocFailRate);
        adaptationclitest.SetHeapFailure(allocFailRate);
        adaptationclitest.ActivateRfForEmergencyCall(status1);
        adaptationclitest.ActivateRfForEmergencyCall(status2);
        User::WaitForRequest(status1);
        User::WaitForRequest(status2);
        TEST(status1.Int() == KErrNone);
        TEST(status2.Int() == KErrNone);
        }
    
    //Iterate through the DeactivateRfForEmergencyCall.    
    for(TInt allocFailRate=1; allocFailRate < 2*maximumAllocation; allocFailRate++)
        {
        INFO_PRINTF2(_L("allocFailRateL %d:."), allocFailRate);
        adaptationclitest.SetHeapFailure(allocFailRate);
        adaptationclitest.DeactivateRfForEmergencyCall(status1);
        adaptationclitest.DeactivateRfForEmergencyCall(status2);
        User::WaitForRequest(status1);
        User::WaitForRequest(status2);
        TEST(status1.Int() == KErrNone);
        TEST(status2.Int() == KErrNone);
        }
    adaptationclitest.UnSetHeapMark();
    //Restore OOM condition
    adaptationclitest.UnSetHeapFailure(); 
    User::__DbgSetAllocFail(ETrue, RAllocator::ENone, 1);    
    adaptationclitest.Close();
    thread.Kill(KErrNone);
    CleanupStack::PopAndDestroy(&thread);
    __UHEAP_MARKEND;
    INFO_PRINTF1(_L("doTestForEmergencyCallOOMIterative completed."));        
    }

#endif //TEST_SSM_SERVER


