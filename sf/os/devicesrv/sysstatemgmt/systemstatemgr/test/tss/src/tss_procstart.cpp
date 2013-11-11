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
// Old Test CaseID 		APPFWK-SSS-0002
// New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0002
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/



#include "tss_procstart.h"


const TInt KProcStartStopperTimeout = 5000000;
const TInt KWaitForActiveSchedStart = 500000; //0.5s

/**
 From MSsTestAsyncNotifier
*/
void CSsTestStepProcStart::SsTestNotify( TInt aNotify )
	{
	TEST( KErrNone == aNotify );
	
	INFO_PRINTF2( _L("*** Start supplied code %d"), aNotify );
	}



TVerdict CSsTestStepProcStart::doTestStepL()
	{
	__UHEAP_MARK;
	TRAPD(err, DoTestFireAndForgetAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFireAndForgetAsyncL completed with err = %d"), err);

	TRAP(err, DoTestWaitForStartAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForStartAsyncL completed with err = %d"), err);

	TRAP(err, DoTestFireAndForgetSyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFireAndForgetSyncL completed with err = %d"), err);

	TRAP(err, DoTestWaitForStartSyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForStartSyncL completed with err = %d"), err);

	TRAP(err, DoTestForMonitorAlreadyMonProcL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestForMonitorAlreadyMonProcL completed with err = %d"), err);

	TRAP(err, DoTestStartTwoServersL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartTwoServersL completed with err = %d"), err);
	
	__UHEAP_MARKEND;
	return TestStepResult();	
	}


/**
 Starts an instance of ssmtestappgood.exe asynchronously as a process using fire-and-forget.
 No timeout is specified, therefore CTimeoutWaiter is not instantiated.
*/
void CSsTestStepProcStart::DoTestFireAndForgetAsyncL()
	{	
	INFO_PRINTF1( _L("\n\nPerforming Fire-and-forget async test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmFireAndForget );
	
	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;

	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	TInt stopperCompletion = KErrGeneral;
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	ss->Start(*startupProperties, process, trs, id);
	//Process are started using fire-and-forget execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( KErrNone == trs.Int() )
		{
		CActiveScheduler::Start();
		
		stopperCompletion = stopper->CompletionCode();
		INFO_PRINTF3( _L("TRS completed with %d. Anticipated value %d"), stopperCompletion, KErrNone );
		}
	
	INFO_PRINTF3( _L("Start supplied %d. anticipated value %d"), trs.Int(), KErrNone );
	
	CleanupStack::PopAndDestroy( 4, startupProperties );
	
	TEST( KErrNone == stopperCompletion );
	
	TEST( 1 == FindAndKill(KTestAppGood) );
	}



const TInt KSsTestProcStartTimeout = 3000; // ms
/**
 Starts an instance of ssmtestappgood.exe asynchronously as a process using wait-for-start.
 A timeout is specified, therefore CTimeoutWaiter is instantiated.
*/
void CSsTestStepProcStart::DoTestWaitForStartAsyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Wait-for-start async test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );

	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	startupProperties->SetTimeout( KSsTestProcStartTimeout );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	TInt stopperCompletion = KErrGeneral;
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	ss->Start(*startupProperties, process, trs, id);
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.
	if( KRequestPending == trs.Int() )
		{
		CActiveScheduler::Start();
		
		stopperCompletion = stopper->CompletionCode();
		INFO_PRINTF3( _L("TRS completed with %d. Anticipated value %d"), stopperCompletion, KErrNone );
		}

	INFO_PRINTF3( _L("Start supplied %d. anticipated value %d"), trs.Int(), KErrNone );

	CleanupStack::PopAndDestroy( 4, startupProperties );
	
	TEST( KErrNone == stopperCompletion );
	
	TEST( 1 == FindAndKill(KTestAppGood) );
	}


/**
 Starts an instance of ssmtestappgood.exe synchronously as a process using fire-and-forget.
 No timeout is specified, therefore CTimeoutWaiter is not instantiated.
*/
void CSsTestStepProcStart::DoTestFireAndForgetSyncL()
	{	
	INFO_PRINTF1( _L("\n\nPerforming Fire-and-forget sync test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmFireAndForget );
	
	RProcess process;
	CleanupClosePushL( process );
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();

	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *ss, *startupProperties, process, KProcStartStopperTimeout, this );
	CleanupStack::PushL( testAndStopper );
	CActiveScheduler::Start();
		
	CleanupStack::PopAndDestroy( 4, startupProperties );

	TEST( 1 == FindAndKill(KTestAppGood) );
	}


/**
 Starts an instance of ssmtestappgood.exe synchronously as a process using wait-for-start.
 A timeout is specified, therefore CTimeoutWaiter is instantiated.
*/
void CSsTestStepProcStart::DoTestWaitForStartSyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Wait-for-start sync test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	startupProperties->SetTimeout( KSsTestProcStartTimeout );

	RProcess process;
	CleanupClosePushL( process );
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *ss, *startupProperties, process, KProcStartStopperTimeout, this );
	CleanupStack::PushL( testAndStopper );
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy( 4, startupProperties );
	
	TEST( 1 == FindAndKill(KTestAppGood) );
	}



/**
 Start ssmtestprocgood as a server. Attempt to start it a second time.
 Ensure that only one instance exists and SsmStartSafe has not reported any error.
*/
void CSsTestStepProcStart::DoTestStartTwoServersL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Process-start as server. Ensure only one") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	// Start testprocess as a server.
	startupProperties->SetFileParamsL( KTestProcGood, KLaunchServerCommandLineOption );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal ); 

	RProcess process_0;
	CleanupClosePushL( process_0 );
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	CTestAndStopper* testAndStopper_0 = new(ELeave) CTestAndStopper( *ss, *startupProperties, process_0, KProcStartStopperTimeout, this );
	CleanupStack::PushL( testAndStopper_0 );
	CActiveScheduler::Start();
	
	
	RProcess process_1;
	CleanupClosePushL( process_1 );
	
	// Attempt to start another
	CTestAndStopper* testAndStopper_1 = new(ELeave) CTestAndStopper( *ss, *startupProperties, process_1, KProcStartStopperTimeout, this );
	CleanupStack::PushL( testAndStopper_1 );
	CActiveScheduler::Start();

	
	INFO_PRINTF1( _L("Checking for one instance of the server process") );
	TEST( 1 == FindAndKill(KTestProcGood) );
	
	CleanupStack::PopAndDestroy( 6, startupProperties );
	}

void CSsTestStepProcStart::DoTestForMonitorAlreadyMonProcL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Process-monitor test for already started and monitored process") );
	
	RProcess process;
	CleanupClosePushL(process);
	
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

	// Try to start and monitor the process
	TRAPD(err, ss->StartAndMonitorL(*startupProperties, process));
	TEST(err == KErrNone);
	INFO_PRINTF3( _L("StartAndMonitorL() completed with %d. anticipated value %d"),err, KErrNone );
	
	RTestProcGoodSession testProcServerSession;
	err = testProcServerSession.Connect();
	TEST(KErrNone == err);
	INFO_PRINTF1( _L("Asserted that server is running"));

	// Try to start and monitor an already started and monitored process
	TRAP(err, ss->StartAndMonitorL(*startupProperties, process));

	// sysmon returns KErrAlreadyExists, if we try to monitor an already monitored process
	TEST(err == KErrAlreadyExists);
	INFO_PRINTF3( _L("StartAndMonitorL() completed with %d. anticipated value %d"),err, KErrAlreadyExists);
	
	process.Kill(KErrNone);
	
	err = testProcServerSession.Connect();
	//Connecting to the test proc server session should fail here with error value as either KErrNotFound(in case session object is cleaned up) or KErrServerTerminated(session object is there but session terminated) 
	TEST(KErrNotFound == err || KErrServerTerminated == err);
	INFO_PRINTF3( _L("Connect to test proc server session failed with %d. anticipated value %d"),err, KErrNotFound);

	RSemaphore sem;
	err = sem.CreateGlobal(KStartProcSignalSemaphore, 0);
	INFO_PRINTF2(_L("Created semaphore with err %d"),err);
	TEST(err == KErrNone);
	CleanupClosePushL(sem);
	
	INFO_PRINTF1(_L("Waiting for Start of the test application"));
	sem.Wait();
	//Waiting for 5 seconds as in the ssmtestprocgood application the active scheduler
	// will be started after signalling the semaphore.  And we can connect to the
	// application only after the active scheduler is started.
	User::After(KWaitForActiveSchedStart);
	
	// Check that the killed process is restarted by SysMon
	err = testProcServerSession.Connect();
	TEST(KErrNone == err);
	INFO_PRINTF3( _L("process re-started by sysmon with %d, anticipate value %d"),err,KErrNone);
	
	// Stop monitoring to stop restarts.
	if(KErrNone == testProcServerSession.Connect())
		{
		testProcServerSession.CancelMonitor();
		testProcServerSession.Close();
		}
	TEST(1 == FindAndKill(KTestProcGood));
	CleanupStack::PopAndDestroy(4, &process);
	}

TVerdict CSsTestStepProcStart::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install ( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}



/** */
TVerdict CSsTestStepProcStart::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}



CSsTestStepProcStart::CSsTestStepProcStart()
	{
	SetTestStepName( KCTestCaseProcStart );
	}



CSsTestStepProcStart::~CSsTestStepProcStart()
	{
	delete iActiveScheduler;	
	}





























