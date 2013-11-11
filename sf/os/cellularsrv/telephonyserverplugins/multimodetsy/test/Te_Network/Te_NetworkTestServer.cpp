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

#include "Te_NetworkTestServer.h"
#include "Te_Network.h"

_LIT(KServerName,"Te_Network");

CNetworkTestServer* CNetworkTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CNetworkTestServer * testServer = new (ELeave) CNetworkTestServer();
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
 	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CNetworkTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CNetworkTestServer::NewL());
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

CTestStep* CNetworkTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
    
	if(aStepName == _L("ServerInfo"))
		{
		testStep = new CServerInfo;
		}
	else if(aStepName == _L("GetNetworkCaps"))
		{
		testStep = new CGetNetworkCaps;
		}
	else if(aStepName == _L("DisconnectedNetworkTest"))
		{
		testStep = new CDisconnectedNetworkTest;
		}
		
 	//Network registration tests
	else if(aStepName == _L("GetNetworkRegistrationStatus"))
		{
		testStep = new CGetNetworkRegistrationStatus;
		}
	else if(aStepName == _L("GetNetworkRegistrationStatusCancel"))
		{
		testStep = new CGetNetworkRegistrationStatusCancel;
		}
	else if(aStepName == _L("NotifyNetworkRegistrationStatusChange"))
		{
		testStep = new CNotifyNetworkRegistrationStatusChange;
		}
	else if(aStepName == _L("NotifyNetworkRegistrationStatusChangeCancel"))
		{
		testStep = new CNotifyNetworkRegistrationStatusChangeCancel;
		}
 	
	//Network mode tests
	else if(aStepName == _L("GetCurrentMode"))
		{
		testStep = new CGetCurrentMode;
		}
	else if(aStepName == _L("NotifyModeChange"))
		{
		testStep = new CNotifyModeChange;
		}
	else if(aStepName == _L("NotifyModeChangeCancel"))
		{
		testStep = new CNotifyModeChangeCancel;
		}
 
	//Current network information tests
	else if(aStepName == _L("GetCurrentNetwork"))
		{
		testStep = new CGetCurrentNetwork;
		}
	else if(aStepName == _L("GetCurrentNetworkCancel"))
		{
		testStep = new CGetCurrentNetworkCancel;
		}
	else if(aStepName == _L("NotifyCurrentNetworkChange"))
		{
		testStep = new CNotifyCurrentNetworkChange;
		}
	else if(aStepName == _L("NotifyCurrentNetworkChangeCancel"))
		{
		testStep = new CNotifyCurrentNetworkChangeCancel;
		}

	//Home Network tests
	else if(aStepName == _L("GetHomeNetwork"))
		{
		testStep = new CGetHomeNetwork;
		}
	else if(aStepName == _L("GetHomeNetworkCancel"))
		{
		testStep = new CGetHomeNetworkCancel;
		}
 	
 	//Detected networks test
	else if(aStepName == _L("DetectedNetworkTest"))
		{
		testStep = new CDetectedNetworkTest;
		}
 	
	//Network selection settings tests
	else if(aStepName == _L("GetNetworkSelectionSetting"))
		{
		testStep = new CGetNetworkSelectionSetting;
		}
	else if(aStepName == _L("SetNetworkSelectionSetting"))
		{
		testStep = new CSetNetworkSelectionSetting;
		}
	else if(aStepName == _L("SetNetworkSelectionSettingCancel"))
		{
		testStep = new CSetNetworkSelectionSettingCancel;
		}
	else if(aStepName == _L("NotifyNetworkSelectionSettingChange"))
		{
		testStep = new CNotifyNetworkSelectionSettingChange;
		}
	else if(aStepName == _L("NotifyNetworkSelectionSettingChangeCancel"))
		{
		testStep = new CNotifyNetworkSelectionSettingChangeCancel;
		}

 	//Network Date/Time tests
	else if(aStepName == _L("GetNITZInfo"))
		{
		testStep = new CGetNITZInfo;
		}
	else if(aStepName == _L("NotifyNITZInfoChangeCancel"))
		{
		testStep = new CNotifyNITZInfoChangeCancel;
		}
	else if(aStepName == _L("NotifyNITZInfoChange"))
		{
		testStep = new CNotifyNITZInfoChange;
		}

	return testStep;
	}
