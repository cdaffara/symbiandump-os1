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

#include "Te_SimVoiceTestServer.h"
#include "Te_SimVoice.h"

_LIT(KServerName,"Te_SimVoice");

CSimVoiceTestServer* CSimVoiceTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CSimVoiceTestServer * testServer = new (ELeave) CSimVoiceTestServer();
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
	CSimVoiceTestServer* testServer = NULL;
	TRAPD(err,testServer = CSimVoiceTestServer::NewL());
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

CTestStep* CSimVoiceTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CSimVoiceTestStepBase* testStep = NULL;

	if(aStepName == _L("TestLineStatusIncommingCall"))
		{
		testStep = new CTestLineStatusIncommingCall() ;
		testStep->SetState(0);
 		}
	else if(aStepName == _L("TestLineNotificationsIncommingCall"))
		{
		testStep = new CTestLineNotificationsIncommingCall() ;
		testStep->SetState(10);
		}
	else if(aStepName == _L("TestCallNotificationsIncommingCall"))
		{
		testStep = new CTestCallNotificationsIncommingCall();
		testStep->SetState(20);
		}
	else if(aStepName == _L("TestCallNotificationsOutgoingCall"))
		{
		testStep = new CTestCallNotificationsOutgoingCall();
		testStep->SetState(30);
		}
	else if(aStepName == _L("TestLineNotificationsOutgoingCall"))
		{
		testStep = new CTestLineNotificationsOutgoingCall();
		testStep->SetState(40);
		}
	else if(aStepName == _L("TestLineStatusOutgoingCall"))
		{
		testStep = new CTestLineStatusOutgoingCall() ;
		testStep->SetState(50);
		}
	else if(aStepName == _L("TestCallStatusOutgoingCall"))
		{
		testStep = new CTestCallStatusOutgoingCall() ;
		testStep->SetState(60);
		}
	else if(aStepName == _L("TestCallStatusIncommingCall"))
		{
		testStep = new CTestCallStatusIncommingCall() ;
		testStep->SetState(70);
		}
	else if(aStepName == _L("TestLineInfo"))
		{
		testStep = new CTestLineInfo() ;
		testStep->SetState(80);
    		}
	else if(aStepName == _L("TestCallCancels"))
		{
		testStep = new CTestCallCancels() ;
		testStep->SetState(90);
		}
	else if(aStepName == _L("TestLineCancels"))
		{
		testStep = new CTestLineCancels() ;
		testStep->SetState(100);
		}
	else if(aStepName == _L("TestMoreCallInfo"))
		{
		testStep = new CTestMoreCallInfo() ;
		testStep->SetState(110);
		}
	else if(aStepName == _L("TestCallInfo"))
		{
		testStep = new CTestCallInfo() ;
		testStep->SetState(120);
		}
	else if(aStepName == _L("TestPhoneInfo"))
		{
		testStep = new CTestPhoneInfo() ;
		testStep->SetState(130);
		}
	else if(aStepName == _L("TestDtmf"))
		{
		testStep = new CTestDtmf() ;
		testStep->SetState(140);
		}
	else if(aStepName == _L("TestCallDuration"))
		{
		testStep = new CTestCallDuration() ;
		testStep->SetState(150);
		}
	else if(aStepName == _L("TestRemoteHangup"))
		{
		testStep = new CTestRemoteHangup() ;
		testStep->SetState(160);
		}

	return testStep;
	}




