// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_ut_ecamservertestsuiteserver.h"
#include "te_ut_ecamserverteststep.h"

_LIT(KServerName,"te_ut_ecam_serversuite");

CTe_ut_ecam_serverSuite* CTe_ut_ecam_serverSuite::NewL()
	{
	CTe_ut_ecam_serverSuite * server = new (ELeave) CTe_ut_ecam_serverSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_ut_ecam_serverSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_ut_ecam_serverSuite::NewL());
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

CTestStep* CTe_ut_ecam_serverSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if (aStepName == KCameraCreationOOMTestStep)
		{
		testStep = new CCameraCreationOOMTestStep();
		}
	else if (aStepName == KCameraReserveOOMTestStep)
		{
		testStep = new CCameraReserveOOMTestStep();
		}
	else if (aStepName == KCameraPowerOnOOMTestStep)
		{
		testStep = new CCameraPowerOnOOMTestStep();
		}
	else if (aStepName == KCameraRepeatedReserveTestStep)
		{
		testStep = new CCameraRepeatedReserveTestStep();
		}
	else if (aStepName == KCameraRepeatedPowerOnTestStep)
		{
		testStep = new CCameraRepeatedPowerOnTestStep();
		}
	else if (aStepName == KCameraStartViewFinderTestStep)
		{
		testStep = new CCameraStartViewFinderTestStep();
		}
	else if (aStepName == KCameraStartStopViewFinderTestStep)
		{
		testStep = new CCameraStartStopViewFinderTestStep();
		}
	else if (aStepName == KCameraReserveThenDestructTestStep)
		{
		testStep = new CCameraReserveThenDestructTestStep();
		}
	else if (aStepName == KCameraPowerOnThenDestructTestStep)
		{
		testStep = new CCameraPowerOnThenDestructTestStep();
		}
	else if (aStepName == KCameraStartViewFinderThenDestructTestStep)
		{
		testStep = new CCameraStartViewFinderThenDestructTestStep();
		}
	else if (aStepName == KCameraReserveTwiceTestStep)
		{
		testStep = new CCameraReserveTwiceTestStep();
		}
	else if (aStepName == KCameraPowerOnTwiceTestStep)
		{
		testStep = new CCameraPowerOnTwiceTestStep();
		}
	else if (aStepName == KCameraPowerOnWithoutReserveTestStep)
		{
		testStep = new CCameraPowerOnWithoutReserveTestStep();
		}
	else if (aStepName == KCameraPowerOnAfterReleaseTestStep)
		{
		testStep = new CCameraPowerOnAfterReleaseTestStep();
		}
	else if (aStepName == KCameraStartViewFinderWithoutReserveTestStep)
		{
		testStep = new CCameraStartViewFinderWithoutReserveTestStep();
		}
	else if (aStepName == KCameraStartViewFinderWithoutPowerOnTestStep)
		{
		testStep = new CCameraStartViewFinderWithoutPowerOnTestStep();
		}
	else if (aStepName == KCameraStartViewFinderAfterPowerOffTestStep)
		{
		testStep = new CCameraStartViewFinderAfterPowerOffTestStep();
		}
	else if (aStepName == KCameraMultiClientReservePriorityTestStep)
		{
		testStep = new CCameraMultiClientReservePriorityTestStep();
		}
	else if (aStepName == KCameraConfigurationTestStep)
		{
		testStep = new CCameraConfigurationTestStep();
		}
	else if (aStepName == KCameraCreationStressTestStep)
		{
		testStep = new CCameraCreationStressTestStep();
		}
	else if (aStepName == KCameraMultiClientPowerOnPriorityTestStep)
		{
		testStep = new CCameraMultiClientPowerOnPriorityTestStep();
		}

	return testStep;
	}
