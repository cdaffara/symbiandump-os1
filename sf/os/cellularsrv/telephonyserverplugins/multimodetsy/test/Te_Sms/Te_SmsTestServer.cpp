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

#include "Te_SmsTestServer.h"
#include "Te_Sms.h"

_LIT(KServerName,"Te_Sms");

CSmsTestServer* CSmsTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSmsTestServer * testServer = new (ELeave) CSmsTestServer();
	CleanupStack::PushL(testServer);

	testServer->ConstructL(KServerName);
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
	CSmsTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CSmsTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	User::After(10000000);
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
	User::After(10000000);
	return err;
    }

CTestStep* CSmsTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
    
	if(aStepName == _L("TestGetCaps"))
		{
		testStep = new CTestGetCaps;
		}
	if(aStepName == _L("TestSmspList"))
		{
		testStep = new CTestSmspList;
		}
	if(aStepName == _L("TestMessageStoreInfo"))
		{
		testStep = new CTestMessageStoreInfo;
		}
	if(aStepName == _L("TestMessageStoreOps"))
		{
		testStep = new CTestMessageStoreOps;
		}
	if(aStepName == _L("TestSendSms"))
		{
		testStep = new CTestSendSms;
		}
	if(aStepName == _L("TestReceiveUnstoredSms"))
		{
		testStep = new CTestReceiveUnstoredSms;
		}
	if(aStepName == _L("TestReceiveStoredSms"))
		{
		testStep = new CTestReceiveStoredSms;
		}
	if(aStepName == _L("TestReceiveStoredSmsAndCancel"))
		{
		testStep = new CTestReceiveStoredSmsAndCancel;
		}
	if(aStepName == _L("TestSendSmsAndCancel"))
		{
		testStep = new CTestSendSmsAndCancel;
		}
	if(aStepName == _L("TestAckSmsStored"))
		{
		testStep = new CTestAckSmsStored;
		}
	if(aStepName == _L("TestAckSmsStoredCancel"))
		{
		testStep = new CTestAckSmsStoredCancel;
		}
	if(aStepName == _L("TestNackSmsStored"))
		{
		testStep = new CTestNackSmsStored;
		}
	if(aStepName == _L("TestNackSmsStoredCancel"))
		{
		testStep = new CTestNackSmsStoredCancel;
		}
	if(aStepName == _L("TestAckSmsStoredWithResponseCancel"))
		{
		testStep = new CTestAckSmsStoredWithResponseCancel;
		}
	if(aStepName == _L("TestNackSmsStoredWithResponse"))
		{
		testStep = new CTestNackSmsStoredWithResponse;
		}
	if(aStepName == _L("TestNackSmsStoredWithResponseCancel"))
		{
		testStep = new CTestNackSmsStoredWithResponseCancel;
		}
	if(aStepName == _L("TestResumeSmsReception"))
		{
		testStep = new CTestResumeSmsReception;
		}
	if(aStepName == _L("TestNotifySmsEvent"))
		{
		testStep = new CTestNotifySmsEvent;
		}
	if(aStepName == _L("TestNotifySmsEventCancel"))
		{
		testStep = new CTestNotifySmsEventCancel;
		}

	return testStep;
	}
