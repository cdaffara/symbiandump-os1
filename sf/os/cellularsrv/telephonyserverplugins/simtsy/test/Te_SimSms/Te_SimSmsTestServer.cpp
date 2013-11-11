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

#include "Te_SimSmsTestServer.h"
#include "Te_SimSms.h"

_LIT(KServerName,"Te_SimSms");

CSimSmsTestServer* CSimSmsTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSimSmsTestServer * testServer = new (ELeave) CSimSmsTestServer();
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

	// Create the CTestServer derived server
	CSimSmsTestServer* testServer = NULL;
	TRAPD(err,testServer = CSimSmsTestServer::NewL());
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

CTestStep* CSimSmsTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	if (aStepName == _L("TestUtilClass"))
		{
		testStep = new CTestUtilClass();
		}
	else if (aStepName == _L("GetCaps"))
		{
		testStep = new CGetCaps();
		}
	else if (aStepName == _L("TestSmspList"))
		{
		testStep = new CTestSmspList();
		}
	else if (aStepName == _L("MessageStoreInfo"))
		{
		testStep = new CMessageStoreInfo();
		}
	else if (aStepName == _L("MessageStoreOps"))
		{
		testStep = new CMessageStoreOps();
		}
	else if (aStepName == _L("SendSms"))
		{
		testStep = new CSendSms();
		}
	else if (aStepName == _L("ReceiveUnstoredSms"))
		{
		testStep = new CReceiveUnstoredSms();
		}
	else if (aStepName == _L("ReceiveStoredSms"))
		{
		testStep = new CReceiveStoredSms();
		}
	else if (aStepName == _L("ReceiveStoredSmsAndCancel"))
		{
		testStep = new CReceiveStoredSmsAndCancel();
		}
	else if (aStepName == _L("SendSmsAndCancel"))
		{
		testStep = new CSendSmsAndCancel();
		}
	else if (aStepName == _L("AckSmsStored"))
		{
		testStep = new CAckSmsStored();
		}
	else if (aStepName == _L("AckSmsStoredCancel"))
		{
		testStep = new CAckSmsStoredCancel();
		}
	else if (aStepName == _L("NackSmsStored"))
		{
		testStep = new CNackSmsStored();
		}
	else if (aStepName == _L("NackSmsStoredCancel"))
		{
		testStep = new CNackSmsStoredCancel();
		}
	else if (aStepName == _L("AckSmsStoredWithResponse"))
		{
		testStep = new CAckSmsStoredWithResponse();
		}
	else if (aStepName == _L("AckSmsStoredWithResponseCancel"))
		{
		testStep = new CAckSmsStoredWithResponseCancel();
		}
	else if (aStepName == _L("NackSmsStoredWithResponse"))
		{
		testStep = new CNackSmsStoredWithResponse();
		}
	else if (aStepName == _L("NackSmsStoredWithResponseCancel"))
		{
		testStep = new CNackSmsStoredWithResponseCancel();
		}
	else if (aStepName == _L("ResumeSmsReception"))
		{
		testStep = new CResumeSmsReception();
		}
	else if (aStepName == _L("NotifySmsEvent"))
		{
		testStep = new CNotifySmsEvent();
		}
	else if (aStepName == _L("NotifySmsEventCancel"))
		{
		testStep = new CNotifySmsEventCancel();
		}

	return testStep;
	}




