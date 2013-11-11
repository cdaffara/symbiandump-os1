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

#include "Te_MiscTestServer.h"
#include "Te_Misc.h"

_LIT(KServerName,"Te_Misc");

CMiscTestServer* CMiscTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CMiscTestServer * testServer = new (ELeave) CMiscTestServer();
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
	CMiscTestServer* testServer = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err,testServer = CMiscTestServer::NewL());
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

CTestStep* CMiscTestServer::CreateTestStep(const TDesC& aStepName)
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
	else if(aStepName == _L("SignalAndBatteryStrengthTest"))
		{
		testStep = new CSignalAndBatteryStrengthTest;
		}
	else if(aStepName == _L("ManufacturerInfoTest"))
		{
		testStep = new CManufacturerInfoTest;
		}
	else if(aStepName == _L("SubscriberIdTest"))
		{
		testStep = new CSubscriberIdTest;
		}
	else if(aStepName == _L("PhoneStoreTest"))
		{
		testStep = new CPhoneStoreTest;
		}
	else if(aStepName == _L("OwnNumberTest"))
		{
		testStep = new COwnNumberTest;
		}
	else if(aStepName == _L("GetCaps"))
		{
		testStep = new CGetCaps;
		}
	else if(aStepName == _L("GetLinesStatus"))
		{
		testStep = new CGetLinesStatus;
		}
	else if(aStepName == _L("GetMultimodeCaps"))
		{
		testStep = new CGetMultimodeCaps;
		}
	else if(aStepName == _L("GetNetworkCaps"))
		{
		testStep = new CGetNetworkCaps;
		}
	else if(aStepName == _L("GetCurrentMode"))
		{
		testStep = new CGetCurrentMode;
		}
	else if(aStepName == _L("GetAsyncNetList"))
		{
		testStep = new CGetAsyncNetList;
		}
	else if(aStepName == _L("IsSupportedByModuleTest"))
		{
		testStep = new CIsSupportedByModuleTest;
		}

	return testStep;
	}
