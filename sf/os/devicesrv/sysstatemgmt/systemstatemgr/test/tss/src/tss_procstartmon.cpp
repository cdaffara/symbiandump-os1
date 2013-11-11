// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Old Test CaseID 		APPFWK-SSS-0006
// New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0006
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include <ssm/ssmstartsafe.h>
#include "tss_procstartmon.h"
#include "ssmtestapps.h"


const TInt KThrottleTime = 15000000; //15s
const TInt KStartMonStopperTimeout = 5000000; // Could (probably) be shaved.

     
TVerdict CSsTestStepProcStartMon::doTestStepL( void )
	{
	//Creating the semaphore
	TInt err = iSem.CreateGlobal(KStartProcSignalSemaphore, 0);
	INFO_PRINTF2(_L("Created semaphore with err %d"),err);
	TEST(err == KErrNone);
		
	__UHEAP_MARK;
	
	TRAP(err, DoTestMonForAlreadyStartedProcL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMonForAlreadyStartedProcL completed with err = %d"), err);
	
	TRAP(err, DoTestMonSyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMonSyncL completed with err = %d"), err);
	
	__UHEAP_MARKEND;
	iSem.Close();
	return TestStepResult();	
	}



/**
 Starts an instance of ssmtestprocgood.exe synchronously, in a process, as a server, specifying 1 retry.
 Kill it and ensure that it is restarted. Tell it to stop being monitored and kill it again.
 Ensure that it stays killed.
*/
void CSsTestStepProcStartMon::DoTestMonSyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Process-monitor sync") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	// Need to start testprocess as a server so that we can tell it to stop being monitored.
	startupProperties->SetFileParamsL( KTestProcGood, KLaunchServerCommandLineOption );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal ); // Rem: EssmFireAndForget is disallowed
	
	const TInt KMonitorTimeout = 3000; // milliseconds I presume
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = KMonitorTimeout;
	monitorInfo.iRetries = 1;
	startupProperties->SetMonitorInfoL(monitorInfo);

	RProcess process;
	CleanupClosePushL( process );
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *ss, *startupProperties, process, KStartMonStopperTimeout, this );
	CleanupStack::PushL( testAndStopper );
	CActiveScheduler::Start();
	
	//Waiting to decrement the count of the semaphore which is being signalled as the application is started first time 
	iSem.Wait();
		
	// Check that the killed process is restarted by SysMon
	TEST( 1 == FindAndKill(KTestProcGood) );
	INFO_PRINTF1( _L("Await Restart before killing the process again") );
	//Waiting using semaphore
	iSem.Wait();
	
	TEST( 1 == FindAndKill(KTestProcGood) );
	INFO_PRINTF1( _L("Await Restart -> connect -> cancel monitor -> kill") );
	//Waiting using semaphore
	iSem.Wait();
		
	// Stop monitoring to stop restarts.
	RTestProcGoodSession testProcServerSession;
	
	if( KErrNone == testProcServerSession.Connect() )
		{
		testProcServerSession.CancelMonitor();
		testProcServerSession.Close();
		}
	
	TEST( 1 == FindAndKill(KTestProcGood) );
	INFO_PRINTF1( _L("Ensure the test-process is dead") );
	User::After( KThrottleTime );
	// Ensure it's not hanging around
	TEST( 0 == FindAndKill(KTestProcGood) );
	
	CleanupStack::PopAndDestroy( 4, startupProperties );
	}

void CSsTestStepProcStartMon::DoTestMonForAlreadyStartedProcL()
	{
	INFO_PRINTF1( _L("Performing Process-monitor test for already started process") );
	
	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KLaunchServerCommandLineOption));
	
	TRequestStatus status;
	process.Rendezvous(status);
	if (status == KRequestPending)
		{
		process.Resume();
		}
	else
		{
		process.Kill(KErrGeneral);
		TESTEL(EFalse, status.Int());
		}
	
	//Waiting to decrement the count of the semaphore which is being signalled as the application is started first time 
	iSem.Wait();
	User::WaitForRequest(status);
	INFO_PRINTF1(_L("Process started"));
	TEST(process.ExitType() == EExitPending);
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	// Need to start testprocess as a server so that we can tell it to stop being monitored.
	startupProperties->SetFileParamsL(KTestProcGood, KLaunchServerCommandLineOption);
	startupProperties->SetCommandTypeL(ESsmCmdStartProcess);
	startupProperties->SetExecutionBehaviour(ESsmWaitForSignal);
	
	const TInt KMonitorTimeout = 3000; // milliseconds I presume
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = KMonitorTimeout;
	monitorInfo.iRetries = 1;
	startupProperties->SetMonitorInfoL(monitorInfo);

	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();

	// Try to start and monitor the process which already started using RProcess::Create().
	TRAPD(err, ss->StartAndMonitorL(*startupProperties, process));

	// Trying to start and monitor an already running process is treated as successful. Startsafe would monitor the
	// already running process.
	TEST(err == KErrNone);
	INFO_PRINTF3( _L("StartAndMonitorL() completed with %d. anticipated value %d"),err, KErrNone );
	
	RTestProcGoodSession testProcServerSession;
	err = testProcServerSession.Connect();
	TEST(KErrNone == err);
	INFO_PRINTF1( _L("Asserted that server is running"));
	process.Kill(KErrNone);
	//In multiprocessor environment, potentially objects can be deleted on a different CPU
	//from the code which requested the deletion. Hence the delay of 5 seconds has been added.
	User::After(5000000); //5sec delay.

	err = testProcServerSession.Connect();
	TEST(KErrNotFound == err);
	INFO_PRINTF3( _L("Connect to test proc server session failed with %d. anticipated value %d"),err, KErrNotFound);

	TEST(process.ExitType() == EExitKill);
	//Waiting using semaphore
	iSem.Wait();
		
	// Check that the killed process is restarted by SysMon
	err = testProcServerSession.Connect();
	TEST(KErrNone == err);
	INFO_PRINTF1( _L("Asserted that server is re-started by sysmon"));
	
	// Stop monitoring to stop restarts.
	if(KErrNone == testProcServerSession.Connect())
		{
		testProcServerSession.CancelMonitor();
		testProcServerSession.Close();
		}
	TEST(1 == FindAndKill(KTestProcGood));
	CleanupStack::PopAndDestroy(3, &process);
	}

void CSsTestStepProcStartMon::SsTestNotify( TInt aNotify )
	{
	INFO_PRINTF2( _L("StartAndMonitorL( left with %d"), aNotify );
	
	TEST( 0 );
	}



TVerdict CSsTestStepProcStartMon::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install ( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}



/** */
TVerdict CSsTestStepProcStartMon::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}



CSsTestStepProcStartMon::CSsTestStepProcStartMon()
	{
	SetTestStepName(KCTestCaseProcStartMon);
	}



CSsTestStepProcStartMon::~CSsTestStepProcStartMon()
	{
	delete iActiveScheduler;
	}


