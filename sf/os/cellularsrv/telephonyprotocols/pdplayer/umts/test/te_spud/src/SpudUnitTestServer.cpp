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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to
// be started in the process of the client. The client initialises the server
// by calling the one and only ordinal.
// 
//

/**
 @file
 @internalComponent
*/

#include "SpudUnitTestServer.h"
#include "SpudUnitTestStep.h"

_LIT(KServerName,"TE_SpudUnit");

CSpudUnitTestServer* CSpudUnitTestServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	// Instantiate server class here
	CSpudUnitTestServer * server = new (ELeave) CSpudUnitTestServer();
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
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	TInt err = StartC32WithCMISuppressions(KPhbkSyncCMI);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);

	CSpudUnitTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAP(err,server = CSpudUnitTestServer::NewL());
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

/** export function */
REtelDriverInput* NewEtelDriverInputL()
	{	
	return new(ELeave) REtelDriverInput();
	}


// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CSpudUnitTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;

	TRAPD
		(err,
		
		// SPUD TEL unit tests
		if(aStepName == KSpudTelUnitNormalOperation)
			{
			testStep = new(ELeave) CSpudTelUnitNormalOperation();
			}
		if(aStepName == KSpudTelUnitNotifications)
			{
			testStep = new(ELeave) CSpudTelUnitNotifications();
			}
		if(aStepName == KSpudTelUnitCreatePrimaryError)
			{
			testStep = new(ELeave) CSpudTelUnitCreatePrimaryError();
			}
		if(aStepName == KSpudTelUnitSetQosError)
			{
			testStep = new(ELeave) CSpudTelUnitSetQosError();
			}
		if(aStepName == KSpudTelUnitSetTftError)
			{
			testStep = new(ELeave) CSpudTelUnitSetTftError();
			}
		if(aStepName == KSpudTelUnitActivateError1)
			{
			testStep = new(ELeave) CSpudTelUnitActivateError1();	
			}
		if(aStepName == KSpudTelUnitActivateError2)
			{
			testStep = new(ELeave) CSpudTelUnitActivateError2();	
			}
		if(aStepName == KSpudTelUnitDeleteError1)
			{
			testStep = new(ELeave) CSpudTelUnitDeleteError1();
			}
		if(aStepName == KSpudTelUnitDeleteError2)
			{
			testStep = new(ELeave) CSpudTelUnitDeleteError2();
			}
		if(aStepName == KSpudTelUnitMultipleContexts)
			{
			testStep = new(ELeave) CSpudTelUnitMultipleContexts();	
			}
		if(aStepName == KSpudTelUnitNotificationsR5OrR99R4)
			{
			testStep = new(ELeave) CSpudTelUnitNotificationsR5OrR99R4();
			}
		if(aStepName == KSpudTelUnitCancel)
			{
			testStep = new(ELeave) CSpudTelUnitCancel();
			}
		
		// SPUD FSM unit tests
		if(aStepName == KSpudFsmUnitNormalOperation)
			{
			testStep = new(ELeave) CSpudFsmUnitNormalOperation();
			}
		if(aStepName == KSpudFsmCreatePrimaryError1)
			{
			testStep = new(ELeave) CSpudFsmUnitCreatePrimaryError1();
			}
		if(aStepName == KSpudFsmCreatePrimaryError2)
			{
			testStep = new(ELeave) CSpudFsmUnitCreatePrimaryError2();
			}
		if(aStepName == KSpudFsmCreatePrimaryError3)
			{
			testStep = new(ELeave) CSpudFsmUnitCreatePrimaryError3();
			}
		if(aStepName == KSpudFsmSetQosAndTftError)
			{
			testStep = new(ELeave) CSpudFsmUnitSetQosAndTftError();
			}
		if(aStepName == KSpudFsmChangeQosAndTftError)
			{
			testStep = new(ELeave) CSpudFsmUnitChangeQosAndTftError();
			}
		if(aStepName == KSpudFsmTestNotifications)
			{
			testStep = new(ELeave) CSpudFsmUnitNotifications();
			}
		if(aStepName == KSpudFsmTestNotificationsR5OrR99R4)
			{
			testStep = new(ELeave) CSpudFsmUnitNotificationsR5OrR99R4();
			}
		if(aStepName == KSpudFsmNetworkDelete)
			{
			testStep = new(ELeave) CSpudFsmUnitNetworkDelete();
			}

		);
	
	if (err != KErrNone)
		{
		return NULL;
		}
		
	return testStep;
	}

