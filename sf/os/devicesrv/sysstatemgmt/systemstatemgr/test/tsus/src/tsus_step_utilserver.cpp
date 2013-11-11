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

#include "susutilserver.h"
#include "tsus_step_utilserver.h"

#include "tsus_ssmsuscli.h"
#include "tsus_startserver.h"

#include "tsus_goodsup.h"


_LIT(KFileNameMissing,	"missing.bin");
_LIT(KFileNameBadSup,	"tsus_badsup.dll");
_LIT(KFileNameGoodSup,	"tsus_goodsup.dll");

const TUid KIdentityMissing = {123};
const TUid KIdentityBadSup = {0x1028359A};
const TUid KIdentityGoodSup = {0x10283598};


CSusUtilServerTest::~CSusUtilServerTest()
	{
	}

CSusUtilServerTest::CSusUtilServerTest()
	{
	SetTestStepName(KTSusUtilServerStep);
	}

/** */
TVerdict CSusUtilServerTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/** */
TVerdict CSusUtilServerTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSusUtilServerTest tests started...."));

	__UHEAP_MARK;		
	doTestForMemoryLeaksL(); 					//UT server
	doTestForMemoryLeaksDuringHeapFailureL();	//UT server
	doTestProcessCriticalL();					//UT server
	doTestUnconnected();	 		//UT RSession
	doTestPositiveConnectCloseL();		//IT RSession+CSession
	doTestNegativeConnectCloseL();		//IT RSession+CSession
	doTestLoadSupL();					//IT all classes used by utility server
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CSusUtilServerTest tests completed!"));
	return TestStepResult();	
	}

/** */
TVerdict CSusUtilServerTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/** 
 Old Test CaseID 		APPFWK-SUS-0002
 New Test CaseID 		DEVSRVS-SSMA-SUS-0002
 */

void CSusUtilServerTest::doTestForMemoryLeaksL()
	{
	INFO_PRINTF1(_L("Checking for memoryleaks in SusUtilServer destructor"));
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	
	__UHEAP_MARK;
	RProcess process;
	CSusUtilServer* server = CSusUtilServer::NewLC(KTestServerName, process.SecureId());
	CleanupStack::PopAndDestroy(server);
	__UHEAP_MARKEND;
	
	CleanupStack::PopAndDestroy(sched);
	}

void CSusUtilServerTest::doTestForMemoryLeaksDuringHeapFailureL( )
	{
	INFO_PRINTF1(_L("Checking for memoryleaks during heap failure"));

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );

	// CSusUtilServer::NewLC
	INFO_PRINTF1(_L("  CSusUtilServer construction under OOM"));
	RProcess process;
	CSusUtilServer* server=NULL;
	TInt failRate = 1;
	for (;;failRate++)
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
	    __UHEAP_MARK;

	    TRAPD(ret, 
	    	{
	    	server = CSusUtilServer::NewLC(KTestServerName, process.SecureId());
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

void CSusUtilServerTest::doTestProcessCriticalL()
	{
	INFO_PRINTF1(_L("Checking that UtilServer set its thread to critical"));
	
	__UHEAP_MARK;
	User::TCritical before = User::Critical();
	TESTL(before == User::ENotCritical);
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL (sched );
	CActiveScheduler::Install (sched );
	RProcess process;
	CSusUtilServer* server = CSusUtilServer::NewLC(KTestServerName, process.SecureId());
	
	User::TCritical critical = User::Critical();
	TEST(critical == User::ESystemCritical);

	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(sched);
	
	User::TCritical after = User::Critical();
	TESTL(after == User::ENotCritical);
	__UHEAP_MARKEND;
	}

/** 
 Old Test CaseID 		APPFWK-SUS-0003
 New Test CaseID 		DEVSRVS-SSMA-SUS-0002
 */

void CSusUtilServerTest::doTestUnconnected()
	{
	INFO_PRINTF1(_L("Testing errormessages returned from an unconnected session"));
	
	RSsmSusCliTest client;

	const TInt connect = client.Connect(KTestServerName);
	TESTE(KErrNotFound == connect, connect);
	
	client.Close(); //should not cause a Panic or any other problem
		
	TSsmSupInfo supinfo;
	const TInt syncReq = client.RequestLoadSup(supinfo);
	TESTE(KErrDisconnected == syncReq, syncReq);
	
	TRequestStatus status;
	TPckgC<TSsmSupInfo> ptr(supinfo);
	client.RequestLoadSup(ptr, status);
	User::WaitForRequest(status);
	const TInt asyncReq = status.Int();
	TESTE(KErrDisconnected == asyncReq, asyncReq);
	
	client.RequestLoadSupCancel();  //should not cause a Panic or any other problem
	
	const TInt unload = client.RequestUnLoadSup(supinfo);
	TESTE(KErrDisconnected == unload, unload);
	}

/** 
 Old Test CaseID 		APPFWK-SUS-0006
 New Test CaseID 		DEVSRVS-SSMA-SUS-0002
 */


void CSusUtilServerTest::doTestPositiveConnectCloseL()
	{
	INFO_PRINTF1(_L("Testing a valid connection attempt of RSsmSusCli"));
	
	__UHEAP_MARK;	
	RProcess process;
	const TUint32 sid = process.SecureId();
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread, sid));
	
	//Test the RSession
	RSsmSusCliTest session;
	TInt err = session.Connect(KTestServerName);
	TESTE(KErrNone == err, err);
	
	//Test a second connect
	err = session.Connect(KTestServerName);
	TESTE(KErrAlreadyExists == err, err);
	
	session.Close();
	
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	__UHEAP_MARKEND;
	}


/** 
 Old Test CaseID 		APPFWK-SUS-0007
 New Test CaseID 		DEVSRVS-SSMA-SUS-0002
 */

void CSusUtilServerTest::doTestNegativeConnectCloseL()
	{
	INFO_PRINTF1(_L("Testing the SID-check in CSusUtilSession"));
	
	__UHEAP_MARK;	
	const TUint32 wrongSecureId = 1;
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread, wrongSecureId));
	
	//Test the RSession
	RSsmSusCliTest session;

	// As the server is started with a wrong secure id, no requests would be serviced by the server.
	// Hence no need to set the heap mark on server.
	TInt err = session.Connect(KTestServerName, EFalse);
	TESTE(KErrNone == err, err);
	err = session.SendAnyrequest();
	TESTE(KErrPermissionDenied == err, err);

	// As the server cannot service any request, no need to check the memory heap on server.
	session.Close(EFalse);
	
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	__UHEAP_MARKEND;
	}

/** 
 Old Test CaseID 		APPFWK-SUS-0005
 New Test CaseID 		DEVSRVS-SSMA-SUS-0002
 */

void CSusUtilServerTest::doTestLoadSupL()
	{
	__UHEAP_MARK;
	
	RProcess process;
	const TUint32 sid = process.SecureId();
	RThread thread;
	CleanupClosePushL(thread);
	TESTL(KErrNone == StartServer(thread, sid));
	
	//Connect the RSession
	RSsmSusCliTest session;
	CleanupClosePushL(session);
	TInt err = session.Connect(KTestServerName);
	TESTE(KErrNone == err, err);

	TestLoadMissing(session);
	TestLoadWrongType(session);
	TestLoadWrongIdentity(session);
	TestLoadOkL(session);
	TestLoadDuplicate(session);
	TestUnloadSup(session);
	
	//Cleanup
	CleanupStack::PopAndDestroy(&session);
	//Kill test-server
	thread.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&thread);
	
	__UHEAP_MARKEND;
	}

void CSusUtilServerTest::TestLoadMissing(RSsmSusCli& aSession)
	{
	INFO_PRINTF1(_L("Try Load missing SUP"));
	
	TSsmSupInfo info(KFileNameMissing, 1, KIdentityMissing);
	TInt result = aSession.RequestLoadSup(info);
	TESTE(KErrNotFound==result, result);
	}

void CSusUtilServerTest::TestLoadWrongType(RSsmSusCli& aSession)
	{
	INFO_PRINTF1(_L("Try Load a SUP with incorrect type UID2"));
	
	TSsmSupInfo info(KFileNameBadSup);
	TInt result = aSession.RequestLoadSup(info);
	TESTE(KErrNotSupported==result, result);
	}

void CSusUtilServerTest::TestLoadWrongIdentity(RSsmSusCli& aSession)
	{
	INFO_PRINTF1(_L("Try Load a SUP with wrong Identity UID3"));
	
	TSsmSupInfo info(KFileNameGoodSup, 1, KIdentityBadSup);
	TInt result = aSession.RequestLoadSup(info);
	TESTE(KErrBadName==result, result);
	}

void CSusUtilServerTest::TestLoadOkL(RSsmSusCli& aSession)
	{
	INFO_PRINTF1(_L("Load a valid and working SUP. Ensure that its RunL gets called."));

	RSemaphore sem;
	sem.CreateGlobal(KTestGoodSupSemaphore, 0);
	CleanupClosePushL(sem);
	
	//Issue a LoadSup request
	TRequestStatus status;
	TSsmSupInfo info(KFileNameGoodSup, 1, KIdentityGoodSup);
	TPckgC<TSsmSupInfo>* infoBuf = new (ELeave) TPckgC<TSsmSupInfo>(info);
	CleanupStack::PushL(infoBuf);
	aSession.RequestLoadSup(*infoBuf, status);

	//Wait until GoodSup signal our semaphore from its RunL (or give up after 5 seconds)
	const TInt timeout = 5000000;
	const TInt result = sem.Wait(timeout);
	TESTE(KErrNone == result, result); // a timeout would cause the result to be KErrTimedOut
	
	//The request to load the sup should already have been completed before the semaphore was signaled
	TESTE(status==KErrNone, status.Int());
	CleanupStack::PopAndDestroy(infoBuf);
	CleanupStack::PopAndDestroy(&sem);
	} //lint !e429 Suppress  Custodial pointer 'infoBuf' has not been freed or returned

void CSusUtilServerTest::TestLoadDuplicate(RSsmSusCli& aSession)
	{
	INFO_PRINTF1(_L("Test that a second request to load the same Sup will fail with KErrAlreadyExists."));
	TSsmSupInfo info(KFileNameGoodSup, 1, KIdentityGoodSup);
	TInt result = aSession.RequestLoadSup(info);
	TESTE(KErrAlreadyExists==result, result);
	}

void CSusUtilServerTest::TestUnloadSup(RSsmSusCli& aSession)
	{
	INFO_PRINTF1(_L("Unload Sup"));
	TSsmSupInfo info(KFileNameGoodSup, 1, KIdentityGoodSup);
	TInt result = aSession.RequestUnLoadSup(info);
	TESTE(KErrNone==result, result);
	
	INFO_PRINTF1(_L("Test error-message for a subsequent unload-request"));
	result = aSession.RequestUnLoadSup(info);
	TESTE(KErrNotFound==result, result);
	}

