/**
 * Copyright (c) Symbian Ltd 2008
 *
 * @file TE_mbufmgrServer.cpp
 *
 * Example file/test code to demonstrate how to develop a TestExecute Server
 * Developers should take this project as a template and substitute their own
 * code at the __EDIT_ME__ tags
 *
 * for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
 * in the process of the client. The client initialises the server by calling the
 * one and only ordinal.
 */
#include "TE_mbufmgrServer.h"
#include "TestStepCTMbufmgr.h"

#include "Test01CreateDeleteMBufMgr.h"
#include "Test02AllocDealloc.h"
#include "Test03AllocLeave.h"
#include "Test04CopyInOut.h"
#include "Test05CopyInOutOffset.h"
#include "Test06SplitL.h"
#include "Test07TrimStart.h"
#include "Test08TrimEnd.h"
#include "Test09Align.h"
#include "Test10CopyL.h"
#include "Test11AsyncAlloc.h"
#include "Test12General.h"
#include "Test13Performance.h"
#include "Test14HeapFreeCheck.h"
#include "Test15Concurrency.h"
#include "test16memoryfull.h"
#include "test17requestsizelimits.h"
#include "test18exhaustmidsizepools.h"
#include "test19prepend.h"
#include "test20poolceilingfull.h"
#include "test21commsbufs.h"
#include "Test22Append.h"
#include "Test23RMBufQ.h"

EXPORT_C CTE_mbufmgrServer* NewMbufmgrServer( void ) 
	{ 
	return new (ELeave) CTE_mbufmgrServer;
	}
	
// __EDIT_ME__ - Substitute the name of your test server 
_LIT(KServerName,"te_ctmbufmgr");
// __EDIT_ME__ - Use your own server class name
CTE_mbufmgrServer* CTE_mbufmgrServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// __EDIT_ME__ new your server class here
	CTE_mbufmgrServer * server = new (ELeave) CTE_mbufmgrServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.

	//server->StartL(KServerName); 
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	// __EDIT_ME__ Your server name
	CTE_mbufmgrServer* server = NULL;
	// Create the CTestServer derived server
	// __EDIT_ME__ Your server name
	TRAPD(err,server = CTE_mbufmgrServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

// __EDIT_ME__ - Use your own server class name
CTestStep* CTE_mbufmgrServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// __EDIT_ME__ - Create your own test steps here
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created

	if(aStepName.Compare(_L("MBufMgrTest01")) == 0 )
		testStep = new(ELeave) CTest01CreateDeleteMBufMgr;
	else if(aStepName.Compare(_L("MBufMgrTest02")) == 0 )
		testStep = new(ELeave) CTest02AllocDealloc;
	else if(aStepName.Compare(_L("MBufMgrTest03")) == 0 )
		testStep = new(ELeave) CTest03AllocLeave;
	else if(aStepName.Compare(_L("MBufMgrTest04")) == 0 )
		testStep = new(ELeave) CTest04CopyInOut;
	else if(aStepName.Compare(_L("MBufMgrTest05")) == 0 )
		testStep = new(ELeave) CTest05CopyInOutOffset;
	else if(aStepName.Compare(_L("MBufMgrTest06")) == 0 )
		testStep = new(ELeave) CTest06Split;	
	else if(aStepName.Compare(_L("MBufMgrTest07")) == 0 )
		testStep = new(ELeave) CTest07TrimStart;
	else if(aStepName.Compare(_L("MBufMgrTest08")) == 0 )
		testStep = new(ELeave) CTest08TrimEnd;
	else if(aStepName.Compare(_L("MBufMgrTest09")) == 0 )
		testStep = new(ELeave) CTest09Align;
	else if(aStepName.Compare(_L("MBufMgrTest10")) == 0 )
		testStep = new(ELeave) CTest10Copy;
	else if(aStepName.Compare(_L("MBufMgrTest11")) == 0 )
		testStep = new(ELeave) CTest11AsyncAlloc;
	else if(aStepName.Compare(_L("MBufMgrTest12")) == 0 )
		testStep = new(ELeave) CTest12General;
	else if(aStepName.Compare(_L("MBufMgrTest13")) == 0 )
		testStep = new(ELeave) CTest13Performance;
	else if(aStepName.Compare(_L("MBufMgrTest14")) == 0 )
		testStep = new(ELeave) CTest14HeapFreeCheck;
	else if(aStepName.Compare(_L("MBufMgrTest15")) == 0 )
		testStep = new(ELeave) CTest15Concurrency;
	else if(aStepName.Compare(_L("MBufMgrTest16")) == 0 )
		testStep = new(ELeave) CTest16MemoryFull;
	else if(aStepName.Compare(_L("MBufMgrTest17")) == 0 )
		testStep = new(ELeave) CTest17RequestSizeLimits;
	else if(aStepName.Compare(_L("MBufMgrTest18")) == 0 )
		testStep = new(ELeave) CTest18ExhaustMidSizePools;
	else if(aStepName.Compare(_L("MBufMgrTest19")) == 0 )
		testStep = new(ELeave) CTest19Prepend;
	else if(aStepName.Compare(_L("MBufMgrTest20")) == 0 )
		testStep = new(ELeave) CTest20PoolCeilingFull;
	else if(aStepName.Compare(_L("MBufMgrTest21")) == 0 )
		testStep = new(ELeave) CTest21CommsBufs;
	else if(aStepName.Compare(_L("MBufMgrTest22")) == 0 )
		testStep = new(ELeave) CTest22Append;
	else if(aStepName.Compare(_L("MBufMgrTest23")) == 0 )
		testStep = new(ELeave) CTest23RMBufQ;

	return testStep;
	}

