// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains main DLL entry point for dll and definition of test suite class.
// 
//

// EPOC includes
#include <e32base.h>
#include <es_sock.h>

#include "TestSuiteIPC.h"
#include "Test01IPCStreamConnection.h"
#include "Test02IPCSimpleDataTransfer.h"
#include "Test03IPCLongDataTransfer.h"
#include "Test04IPCMultithreadedDataTransfer.h"
#include "Test05IPCIllegalOperations.h"
#include "Test06IPCTestSelect.h"
#include "Test07IPCDataTransferMemoryLeak.h"
#include "Test08IPCOpenSocketMemoryLeak.h"


_LIT(KServerName,"TE_IPCTest");
// __EDIT_ME__ - Use your own server class name
CTestServerIPC* CTestServerIPC::NewL()
/**
* @return - Instance of the test server
* Called inside the MainL() function to create and start the
* CTestServer derived server.
*/
	{
	CTestServerIPC * server = new (ELeave) CTestServerIPC();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.
	
	//server->StartL(KServerName); 
	server-> ConstructL(KServerName);
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
	
	CTestServerIPC* server = NULL;
	// Create the CTestServer derived server
	
	TRAPD(err,server = CTestServerIPC::NewL());
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
	TRAPD(err,MainL());
	delete cleanup;
	return err;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestServerIPC::CTestServerIPC()
	{
	}

CTestServerIPC::~CTestServerIPC()
	{
	}


CTestStep* CTestServerIPC::CreateTestStep(const TDesC& aStepName)
/**
* @return - A CTestStep derived instance
* Implementation of CTestServer pure virtual
*/
	{
	START_TEST_STEP_LIST
		ADD_TEST_STEP(CTest01IPCStreamConnection)
		ADD_TEST_STEP(CTest02IPCSimpleDataTransfer)
		ADD_TEST_STEP(CTest03IPCLongDataTransfer)
		ADD_TEST_STEP(CTest04IPCMultithreadedDataTransfer)
		ADD_TEST_STEP(CTest05IPCIllegalOperations)
		ADD_TEST_STEP(CTest06IPCTestSelect)
		ADD_TEST_STEP(CTest07IPCDataTransferMemoryLeak)
		ADD_TEST_STEP(CTest08IPCOpenSocketMemoryLeak)
		END_TEST_STEP_LIST
	}


