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
// Old Test CaseID 		APPFWK-SSS-0001
// New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0001
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/



#include "tss_appstart.h"


const TInt KFireAndForgetTimeout = 5000000;



TVerdict CSsTestStepAppStart::doTestStepL()
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

	TRAP(err, DoTestWaitForStartSuccessiveSyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForStartSuccessiveSyncL completed with err = %d"), err);

	TRAP(err, DoTestAppStartWithRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestAppStartWithRetriesL completed with err = %d"), err);

	__UHEAP_MARKEND;
	return TestStepResult();	
	}


/**
 Start a test application asynchronously using fire-and-forget
*/
void CSsTestStepAppStart::DoTestFireAndForgetAsyncL()
	{	
	INFO_PRINTF1( _L("\n\nPerforming App-Start Fire-and-forget Async test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmFireAndForget );
	
	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;

	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	ss->Start(*startupProperties, process, trs, id);
	//Application is started using fire-and-forget execution behaviour, 
	//Start method always sets TRequestStatus to KErrNone.
	if( KErrNone == trs.Int() )
		{
		CActiveScheduler::Start();
		INFO_PRINTF3( _L("TRS completed with %d. anticipated value %d"), stopper->CompletionCode(), KErrNone );
		}
	
	CleanupStack::PopAndDestroy( 4, startupProperties );

	TEST( 1 == FindAndKill(KTestAppGood) );
	}



const TInt KSsTestAppStartTimeout = 3000; // ms
/**
 Start a test application asynchronously using wait-for-start.
 A timeout is specified in order to instantiate the CTimeoutWaiter class
*/
void CSsTestStepAppStart::DoTestWaitForStartAsyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming App-Start Wait-for-start Async test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );

	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process;
	TInt id = 0;
	
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	ss->Start(*startupProperties, process, trs, id);
	CActiveScheduler::Start();
	process.Close();
	
	INFO_PRINTF3( _L("TRS completed with %d. anticipated value %d"),  stopper->CompletionCode(), KErrNone );

	CleanupStack::PopAndDestroy( 3, startupProperties );
	
	TEST( 1 == FindAndKill(KTestAppGood) );
	}



/**
 Start a test application synchronously using fire-and-forget
*/
void CSsTestStepAppStart::DoTestFireAndForgetSyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming App-start Fire-and-forget sync test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmFireAndForget );
	
	RProcess process;
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *ss, *startupProperties, process, KFireAndForgetTimeout, this );
	CleanupStack::PushL( testAndStopper );
	CActiveScheduler::Start();
	
	process.Close();
		
	CleanupStack::PopAndDestroy( 3, startupProperties );

	TEST( 1 == FindAndKill(KTestAppGood) );
	}



/**
 Start a test application asynchronously using wait-for-start.
 A timeout is specified in order to instantiate the CTimeoutWaiter class
*/
void CSsTestStepAppStart::DoTestWaitForStartSyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming App-start Wait-for-start sync test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );

	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process;
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *ss, *startupProperties, process, KFireAndForgetTimeout, this );
	CleanupStack::PushL( testAndStopper );
	CActiveScheduler::Start();

	process.Close();

	CleanupStack::PopAndDestroy( 3, startupProperties );
	
	TEST( 1 == FindAndKill(KTestAppGood) );
	}


/**
 This test makes two synchronous calls to StartL and checks for two instances of the test-app.
 A timeout is specified in order to instantiate the CTimeoutWaiter class.
 some retries are specified, although they will not be used
*/
void CSsTestStepAppStart::DoTestWaitForStartSuccessiveSyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming App-start Wait-for-start sync test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );

	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process_0;
	RProcess process_1;
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	CTestTwoAndStopper* testTwoAndStopper_0 = new(ELeave) CTestTwoAndStopper( *ss, *startupProperties, process_0, process_1, KFireAndForgetTimeout, this );
	CleanupStack::PushL( testTwoAndStopper_0 );
	
	CActiveScheduler::Start();

	process_0.Close();
	process_1.Close();
	CleanupStack::PopAndDestroy( 3, startupProperties );
	
	TEST( 2 == FindAndKill(KTestAppGood) );
	}



/**
 ssmtestapprvafterretry.exe is eventually invoked successfully. SsmStartSafe is configured to 
 make 1 + retries attempts to start the recalcitrant app. The app is configured to start
 after retries attempts
*/
void CSsTestStepAppStart::DoTestAppStartWithRetriesL()
	{	
	INFO_PRINTF1( _L("\nStart recalcitrant app with retries") );
	
	//
	// Write the retry number to a file to be read, updated 
	// and eventually deleted by the test-app.
	RFs fs;	
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	CleanupClosePushL( writeStream );
	
	if( KErrNone != fs.Connect()
	 || KErrNone != writeStream.Replace(fs, KDontRvCountFile, EFileWrite) )
		{
		User::Leave( KErrCouldNotConnect );	
		}	
	const TInt KSsTestDontRvCount = 4;				
	writeStream.WriteInt8L( KSsTestDontRvCount ); 
	writeStream.CommitL();		
	
	CleanupStack::PopAndDestroy( 2, &fs );
	//	
	
	
	const TInt KRetries = KSsTestDontRvCount + 1;
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	startupProperties_0->SetFileParamsL( KTestAppRvAfterRetry, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_0->SetRetries( KRetries );
	// For each retry KTestAppRvAfterRetry takes 2secs
	startupProperties_0->SetTimeout( KSsTestAppStartTimeout*1.5 ); 

	RProcess process;
	TInt id_0 = 0;
	
	CStartAcknowledge* ack_0 = new(ELeave) CStartAcknowledge( this );
	CleanupStack::PushL( ack_0 );
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	ss->Start(*startupProperties_0, process, ack_0->iStatus, id_0);

	
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 10 );
	CleanupStack::PushL( pauseStopper );
	CActiveScheduler::Start();
	process.Close();

	TEST( ack_0->iStatus == KErrNone );
		
	CleanupStack::PopAndDestroy( 4, startupProperties_0 );

	TEST( 1 == FindAndKill(KTestAppRvAfterRetry) );
	}



TVerdict CSsTestStepAppStart::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}



/** */
TVerdict CSsTestStepAppStart::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


/**
 From MSsTestAsyncNotifier
*/
void CSsTestStepAppStart::SsTestNotify( TInt aNotify )
	{
	TEST( KErrNone == aNotify );
	
	INFO_PRINTF3( _L("StartL supplied leave code %d. Anticipated value %d"), aNotify, KErrNone );
	}



CSsTestStepAppStart::CSsTestStepAppStart()
	{
	SetTestStepName( KCTestCaseAppStart );
	}



CSsTestStepAppStart::~CSsTestStepAppStart()
	{
	delete iActiveScheduler;
	}

