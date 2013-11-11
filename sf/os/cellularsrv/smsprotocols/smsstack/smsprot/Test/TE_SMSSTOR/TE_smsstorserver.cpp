// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_smsstorserver.h"
#include "TE_smsstor.h"



_LIT(KServerName,"TE_SMSSTOR");


CSmsStorTestServer* CSmsStorTestServer::NewL()
/**
 *  @return - Instance of the test server
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
	{
	CSmsStorTestServer * server = new (ELeave) CSmsStorTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 *  Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSmsStorTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSmsStorTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
 *  @return - Standard Epoc error code on exit
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


CTestStep* CSmsStorTestServer::CreateTestStep(const TDesC& aStepName)
/**
 *  @return - A CTestStep derived instance
 *  Implementation of CTestServer pure virtual
 */
	{
	CTestStep*  testStep = NULL;

	//
	// Here the test step is created when it is needed. Note that this
	// function is non-leaving so we cannot use new(ELeave). Therefore
	// the new could return NULL, but that is not a problem as it implies
	// the test step is missing and this will be marked in the log file.
	//
	if (aStepName == _L("SmsSegmentionStorePurge"))
		{
		testStep = new CSmsSegmentionStorePurge ;
		}
	else if (aStepName == _L("SmsSegmentionStorePurge1"))
		{
		testStep = new CSmsSegmentionStorePurge1 ;
		}
	else if (aStepName == _L("SmsSegmentionStorePurge2"))
		{
		testStep = new CSmsSegmentionStorePurge2 ;
		}
   else if (aStepName == _L("SmsSegmentionStorePurge3"))
        {
        testStep = new CSmsSegmentionStorePurge3 ;
        }
   else if (aStepName == _L("SmsSegmentionStorePurge4"))
        {
        testStep = new CSmsSegmentionStorePurge4 ;
        }

	else if (aStepName == _L("Reassemblystore"))
		{
		testStep = new CReassemblystore3 ;
		}
	else if (aStepName == _L("Reassemblystore2"))
		{
		testStep = new CReassemblystore2 ;
		}
	else if (aStepName == _L("EncodePDUs"))
		{
		testStep = new CEncodePDUs ;
		}
	else if (aStepName == _L("InternalizeExternalizeSMS"))
		{
		testStep = new CInternalizeExternalizeSMS ;
		}
	else if (aStepName == _L("PublicMethodTest"))
		{
		testStep = new CPublicMethodTest ;
		}
	else if (aStepName == _L("LowMemoryTest"))
		{
		testStep = new CLowMemoryTest ;
		}
	else if (aStepName == _L("CorruptionTest"))
		{
		testStep = new CCorruptionTest ;
		}
	else if (aStepName == _L("StatusReportTest"))
		{
		testStep = new CStatusReportTest ;
		}
	else if (aStepName == _L("PermanentFileStoreTest"))
		{
		testStep = new CSmsPermanentFileStoreTest ;
		}
	else if (aStepName == _L("PreallocatedFileTest"))
		{
		testStep = new CPreallocatedFileTest;
		}
	else if (aStepName == _L("Class0ReassemblyStoreTest"))
		{
		testStep = new CClass0ReassemblyStoreTest2;
		}
	else if (aStepName == _L("ReassemblyStorePerformanceTest"))
		{
		testStep = new CSmsReassemblyStorePerformanceTest;
		}

	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}
	
	return testStep;
	}
