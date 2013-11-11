// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimPacketTestServer.h"
#include "Te_SimPacket.h"

CSimPacketTestServer* CSimPacketTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSimPacketTestServer * testServer = new (ELeave) CSimPacketTestServer();
	CleanupStack::PushL(testServer);

	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	testServer->ConstructL(serverName.Name());
	CleanupStack::Pop(testServer);
	return testServer;
	}

LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
   	TInt result = StartC32();
	if (result != KErrNone  &&  result != KErrAlreadyExists)
		{
		User::Leave(result);
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	// Create the CTestServer derived server
	CSimPacketTestServer* testServer = NULL;
	TRAPD(err,testServer = CSimPacketTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete testServer;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }

CTestStep* CSimPacketTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
    //
	// Create the required test step...
	//
	CTestStep*  testStep = NULL;

	if(aStepName == _L("GprsDefaultContextParamsTest"))
		{
		testStep = new CGprsDefaultContextParamsTest();
		}
	else if(aStepName == _L("QoSTest"))
		{
		testStep = new CQoSTest();
		}
	else if(aStepName == _L("GprsUnSupportedTest"))
		{
		testStep = new CGprsUnSupportedTest();
		}
	else if(aStepName == _L("GprsContextAttachTest"))
		{
		testStep = new CGprsContextAttachTest();
		}
	else if(aStepName == _L("GprsContextDeleteTest"))
		{
		testStep = new CGprsContextDeleteTest();
		}
	else if(aStepName == _L("GprsContextLoanComport"))
		{
		testStep = new CGprsContextLoanComport();
		}
	else if(aStepName == _L("GprsContextUnSupportedTest"))
		{
		testStep = new CGprsContextUnSupportedTest();
		}
	else if(aStepName == _L("GprsCancelTest"))
		{
		testStep = new CGprsCancelTest();
		}
	else if(aStepName == _L("GprsAttachTest"))
		{
		testStep = new CGprsAttachTest();
		}
	else if(aStepName == _L("GprsNotificationTest"))
		{
		testStep = new CGprsNotificationTest();
		}
	else if(aStepName == _L("GprsMsClassTest"))
		{
		testStep = new CGprsMsClassTest();
		}
	else if(aStepName == _L("GprsContextConfigTest"))
		{
		testStep = new CGprsContextConfigTest();
		}
	else if(aStepName == _L("GprsContextConfigNotificationsTest"))
		{
		testStep = new CGprsContextConfigNotificationsTest();
		}
	else if(aStepName == _L("OpenGprsMaxContextsTest"))
		{
		testStep = new COpenGprsMaxContextsTest();
		}
	else if(aStepName == _L("GprsContextConfigArbitraryAddressTest"))
		{
		testStep = new CGprsContextConfigArbitraryAddressTest();
		}
	else if(aStepName == _L("EtelPcktMbmsCap1"))
		{
		testStep = new CMbmsOpenNewContextTest();
		}
	else if(aStepName == _L("EtelPcktMbmsCap2"))
		{
		testStep = new CMbmsUpdateSessionTest();
		}
	else if(aStepName == _L("EtelPcktMbmsCap3"))
		{
		testStep = new CGetMbmsNetworkServiceStatus();
		}
	else if(aStepName == _L("EtelPcktMbmsCap4"))
		{
		testStep = new CNotifyMbmsNetworkServiceStatusChange();
		}
	else if(aStepName == _L("EtelPcktMbmsCap5"))
		{
		testStep = new CNotifyMbmsServiceAvailabilityChange();
		}
	else if(aStepName == _L("EtelPcktMbmsCap6"))
		{
		testStep = new CUpdateMbmsMonitorServiceList();
		}
	else if(aStepName == _L("EtelPcktMbmsCap7"))
		{
		testStep = new CEnumerateMbmsActiveServiceList();
		}
	else if(aStepName == _L("EtelPcktMbmsCap8"))
		{
		testStep = new CEnumerateMbmsMonitorServiceList();
		}
	else if(aStepName == _L("TestMultipleCompletionWithClosingContext"))
		{
		testStep = new CTestMultipleCompletionWithClosingContext();
		}
	else if (aStepName == _L("TestDynamicCaps"))
		{
		testStep = new CTestDynamicCaps();
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




