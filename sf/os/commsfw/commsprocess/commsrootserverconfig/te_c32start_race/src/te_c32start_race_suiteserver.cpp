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

/**
 @file te_c32start_race_suiteserver.cpp
*/

#include <test/testexecuteserverbase.h>
#include <rsshared.h>
#include <hal.h>

class CTe_ConfiguratorSuite : public CTestServer
	{
public:
	static CTe_ConfiguratorSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

_LIT(KStartC32RaceStep,"StartC32Race");
class CC32StartRaceStep : public CTestStep
	{
public:
	CC32StartRaceStep()
		{
		SetTestStepName(KStartC32RaceStep);
		}

	virtual TVerdict doTestStepL()
		{
		SetTestStepResult(EFail);
		_LIT(KEsockMainMask, "*ESock_Main*");
		TFindThread findThread(KEsockMainMask);
		TFullName name;
		if(findThread.Next(name) == KErrNone)
			{
			INFO_PRINTF1(_L("CC32StartRaceStep: 'ESock_Main' module already running; test environment improper\n"));
			}
		else
			{
			TRaceResults res1;
			User::LeaveIfError(res1.iStartLock.CreateLocal(0));
			CleanupClosePushL(res1.iStartLock);
			TRaceResults res2;
			res2.iStartLock = res1.iStartLock;
			RThread racer1;
			User::LeaveIfError(racer1.Create(KNullDesC, (TThreadFunction) RacingThread, 8192, NULL, &res1));
			CleanupClosePushL(racer1);
			TRequestStatus racer1Stat;
			racer1.Rendezvous(racer1Stat);
			racer1.Resume();
			INFO_PRINTF1(_L("CC32StartRaceStep: 'Waiting for racer1 thread to get ready\n"));
			User::WaitForRequest(racer1Stat);
			User::LeaveIfError(racer1Stat.Int());
			racer1.Logon(racer1Stat);
			RThread racer2;
			User::LeaveIfError(racer2.Create(KNullDesC, (TThreadFunction) RacingThread, 8192, NULL, &res2));
			CleanupClosePushL(racer2);
			TRequestStatus racer2Stat;
			racer2.Rendezvous(racer2Stat);
			racer2.Resume();
			INFO_PRINTF1(_L("CC32StartRaceStep: 'Waiting for racer2 thread to get ready\n"));
			User::WaitForRequest(racer2Stat);
			User::LeaveIfError(racer2Stat.Int());
			racer1.Logon(racer2Stat);
			// all ready to race
			INFO_PRINTF1(_L("CC32StartRaceStep: Releasing racing threads\n"));
			TUint32 startFC = User::FastCounter();
			res1.iStartLock.Signal(2);
			User::WaitForRequest(racer1Stat, racer2Stat);
			INFO_PRINTF2(_L("CC32StartRaceStep: 'Racer #%d finished\n"), racer1Stat.Int() != KRequestPending? 1: 2);
			User::WaitForRequest(racer1Stat, racer2Stat);
			INFO_PRINTF1(_L("CC32StartRaceStep: 'Other racer finished\n"));
			if(racer1Stat.Int() == KErrNone && racer2Stat.Int() == KErrNone)
				{
				// NB: ignoring absence of FC & wraparound problems here :-(
				TInt fcRate = 1;
				HAL::Get(HALData::EFastCounterFrequency, fcRate);
				TReal res1Delay = (TReal) (res1.iWhen - startFC) / fcRate;
				TReal res2Delay = (TReal) (res2.iWhen - startFC) / fcRate;
				INFO_PRINTF4(_L("CC32StartRaceStep: 'Racer1 call result %d, see_core_module=%d, time=%1.4f\n"), res1.iStartError, res1.iAfterCall, res1Delay);
				INFO_PRINTF4(_L("CC32StartRaceStep: 'Racer2 call result %d, see_core_module=%d, time=%1.4f\n"), res2.iStartError, res2.iAfterCall, res2Delay);
				if(res1.iStartError == KErrNone && res1.iAfterCall &&
					res2.iStartError == KErrNone && res2.iAfterCall)
					{
					SetTestStepResult(EPass);
					}
				}
			CleanupStack::PopAndDestroy(3, &res1.iStartLock);
			}
		return TestStepResult();
		}
private:
	struct TRaceResults
		{
		RSemaphore iStartLock;
		TInt iStartError;
		TBool iAfterCall;
		TUint32 iWhen;
		};
	static TInt RacingThread(TRaceResults* aResults)
		{
		RThread::Rendezvous(KErrNone);
		aResults->iStartLock.Wait();
		aResults->iStartError = StartC32();
		aResults->iWhen = User::FastCounter();
		_LIT(KEsockMainMask, "*ESock_Main*");
		TFindThread findThread(KEsockMainMask);
		TFullName name;
		aResults->iAfterCall = findThread.Next(name) == KErrNone;
		return KErrNone;
		};
	};


_LIT(KServerName,"te_c32start_race");
CTe_ConfiguratorSuite* CTe_ConfiguratorSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_ConfiguratorSuite * server = new (ELeave) CTe_ConfiguratorSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	//server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_ConfiguratorSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_ConfiguratorSuite::NewL());
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
	__ASSERT_ALWAYS(!err, User::Panic(_L("Panic From MainL"), err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


CTestStep* CTe_ConfiguratorSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below         

	// remove ELeave as harness will test ptr. This is more efficient
	// than using TRAP_IGNORE

	if(aStepName == KStartC32RaceStep())
		{
		testStep = new CC32StartRaceStep;
		}
			
	return testStep;
	}

