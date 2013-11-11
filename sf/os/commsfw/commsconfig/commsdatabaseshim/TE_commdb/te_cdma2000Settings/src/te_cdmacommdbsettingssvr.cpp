//
//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
//

/**
 @file 
 @internalComponent
 
 Implementation of CTeCdma2000settingsServer class
 
 for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
 in the process of the client. The client initialises the server by calling the
 one and only ordinal.
 */



#include "Cdma2000settingsstep.h"
#include "te_cdmacommdbsettingssvr.h"

using namespace te_Cdma2000CommDbSettings;

_LIT(KServerName,"te_cdmacommdbsettingssvr");

CTe_Cdma2000CommdbSettingsSvr* CTe_Cdma2000CommdbSettingsSvr::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_Cdma2000CommdbSettingsSvr * server = new (ELeave)CTe_Cdma2000CommdbSettingsSvr;
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
	CTe_Cdma2000CommdbSettingsSvr* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_Cdma2000CommdbSettingsSvr::NewL());
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

CTestStep* CTe_Cdma2000CommdbSettingsSvr::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{	
	CTestStep* testStep = NULL;
	
	if(aStepName == KCdma2000SettingsStep)  
		{
		testStep = new CCdma2000SettingsTestStep;
		}

	
	return testStep;
	}

