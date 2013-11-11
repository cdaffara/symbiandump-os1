// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Old Test CaseID 		APPFWK-SSS-0005
// New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0005
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/



#include "tss_apptimeout.h"
#include "ssmtestapps.h"
#include "ssmtestappslow.h"

const TInt KWaitTime = 300;
const TInt KWaitTimeForTestAppForFAndF = 500;	// 500ms is the delay given for the KTestAppForFAndF test app for each retry.
	 
TVerdict CSsTestStepAppTimeout::doTestStepL()
	{
	__UHEAP_MARK;
	
	TRAPD(err, DoTestAppTimeoutAsAppL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestAppTimeoutAsAppL completed with err = %d"), err);
	
	TRAP(err, DoTestAppTimeoutAsProcL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestAppTimeoutAsProcL completed with err = %d"), err);
	
	TRAP(err, DoTestWaitForSignalWithLessTimeOutAndWithRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForSignalWithLessTimeOutAndWithRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestWaitForSignalWithLessTimeOutAndNoRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForSignalWithLessTimeOutAndNoRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestWaitForSignalWithMoreTimeOutAndNoRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForSignalWithMoreTimeOutAndNoRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestWaitForSignalWithMoreTimeOutAndWithRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForSignalWithMoreTimeOutAndWithRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestWaitForSignalWithRetriesForFailureAppL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForSignalWithRetriesForFailureAppL completed with err = %d"), err);
	
	TRAP(err, DoTestWaitForSignalWithRetriesForSuccessfullAppL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestWaitForSignalWithRetriesForSuccessfullAppL completed with err = %d"), err);

	__UHEAP_MARKEND;
	return TestStepResult();	
	}

TInt CSsTestStepAppTimeout::DoTestAndStopperL(TInt aRetries, TInt aTimeOut, const TDesC& aFileName)
	{
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL(startupProperties);
	
	startupProperties->SetFileParamsL(aFileName, KNullDesC);
	startupProperties->SetCommandTypeL(ESsmCmdStartProcess);
	startupProperties->SetExecutionBehaviour(ESsmWaitForSignal);
	startupProperties->SetTimeout(aTimeOut);
	startupProperties->SetRetries(aRetries);
	
	RProcess process;
	CleanupClosePushL(process);
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();

	TInt id = 0;
	ss->Start(*startupProperties, process, trs, id);
	//Process are started using Wait for signal execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.		
	if( KRequestPending == trs.Int() )
		{
		CActiveScheduler::Start();
		}

	TInt completionCode = stopper->CompletionCode();	
	
	CleanupStack::PopAndDestroy(4, startupProperties);
	return completionCode;
	}

/**
 Starts an instance of ssmtestappslow.exe synchronously as a process using Wait-for-Signal.
 No retries are specified. Timeout is specified, therefore CTimeoutWaiter is not instantiated.
*/
void CSsTestStepAppTimeout::DoTestWaitForSignalWithLessTimeOutAndWithRetriesL()
	{	
	INFO_PRINTF1(_L("Performing Wait-for-Signal Timeout test with retries"));
	
	const TInt KTestLessTimeOut = KTestAppSlowTimeoutInMilliSeconds-100;
	TInt completionCode = DoTestAndStopperL(1, KTestLessTimeOut, KTestAppSlow);

	TEST(completionCode == -33);
	INFO_PRINTF2( _L("Anticipated completion code -33. Supplied completion code %d"),completionCode);
	// ssmtestappslow.exe would terminated due to timeout. 
	// Since ssmtestappslow application takes more time to start than the timeout specified in the ssmsstartupproperties,
	// it doesn't start anytime irrespective of no. of retries specified.
	TEST(0 == FindAndKill(KTestAppSlow));
	INFO_PRINTF1( _L("ssmtestappslow.exe would be terminated due to timeout"));
	}

/**
 Start a ssmtestappslow test application synchronously using Wait-for-Signal with timeout
 less than the time required for the application to start and without retries.
*/
void CSsTestStepAppTimeout::DoTestWaitForSignalWithLessTimeOutAndNoRetriesL()
	{	
	INFO_PRINTF1(_L("Performing Wait-for-Signal Timeout test with no retries"));
	
	const TInt KTestLessTimeOut = KTestAppSlowTimeoutInMilliSeconds-100;
	TInt completionCode = DoTestAndStopperL(0, KTestLessTimeOut, KTestAppSlow);

	TEST(completionCode == -33);
	INFO_PRINTF2( _L("Anticipated completion code -33. Supplied completion code %d"),completionCode);
	// ssmtestappslow.exe would terminated due to timeout. 
	// Since ssmtestappslow application takes more time to start than the timeout specified in the ssmsstartupproperties
	TEST(0 == FindAndKill(KTestAppSlow));
	INFO_PRINTF1( _L("ssmtestappslow.exe would be terminated due to timeout"));
	}

/**
 Start a ssmtestappslow test application synchronously using Wait-for-Signal with timeout
 more than the time required for the application to start and without retries.
*/
void CSsTestStepAppTimeout::DoTestWaitForSignalWithMoreTimeOutAndNoRetriesL()
	{	
	INFO_PRINTF1(_L("Performing Wait-for-Signal Timeout test with no retries and.."));
	INFO_PRINTF1(_L("Timeout specified in ssmstartupproperties is more than timeout required for application startup"));
	
	const TInt KTestMoreTimeOut = KTestAppSlowTimeoutInMilliSeconds * 2;
	TInt completionCode = DoTestAndStopperL(0, KTestMoreTimeOut, KTestAppSlow);

	TEST(completionCode == 0);
	INFO_PRINTF2( _L("Anticipated completion code 0. Supplied completion code %d"),completionCode);
	// ssmtestappslow.exe would be started as ssmtestappslow application takes less time to start than 
	// the timeout specified in the ssmsstartupproperties
	TEST(1 == FindAndKill(KTestAppSlow));
	}

/**
 Start a test application synchronously using Wait-for-Signal with timeout
 more than the time required for the application to start and which starts after the no. of retries given.
*/
void CSsTestStepAppTimeout::DoTestWaitForSignalWithMoreTimeOutAndWithRetriesL()
	{	
	INFO_PRINTF1(_L("Performing Wait-for-Signal test with timeout and retries"));

	// Write the retry number to a file to be read, updated 
	// and eventually deleted by the test-app.
	RFs fs;	
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	CleanupClosePushL( writeStream );

	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(writeStream.Replace(fs, KFileForFAndF, EFileWrite));

	const TInt KSsTestRequiredDelay = 0;
	const TInt KSsTestFailCount = 1;
	const TInt KSsTestRetryCount = 0;
	writeStream.WriteInt8L(KSsTestRetryCount);
	writeStream.WriteInt8L(KSsTestFailCount);
	writeStream.WriteInt8L(KSsTestRequiredDelay);
	writeStream.CommitL();
	writeStream.Close();
	
	const TInt KRetries = KSsTestFailCount + 1;
	TInt completionCode = DoTestAndStopperL(KRetries, KWaitTimeForTestAppForFAndF * 3, KTestAppForFAndF);
	TEST(completionCode == 0);
	
	INFO_PRINTF2( _L("Anticipated completion code 0. Supplied completion code %d"),completionCode);
	RFileReadStream readStream;
	CleanupClosePushL(readStream);

	User::LeaveIfError(readStream.Open(fs, KFileForFAndF, EFileRead));
	TInt retryCount = readStream.ReadInt8L();
	readStream.Close();	
	fs.Delete(KFileForFAndF);

	CleanupStack::PopAndDestroy(3, &fs);

	TEST(1 == FindAndKill(KTestAppForFAndF));
	TEST(retryCount == 1);

	INFO_PRINTF3( _L("ssmtestappsucessfulafterretry.exe is started after %d retries. anticipated retries value %d"),retryCount, 1);
	}

/**
 Start a test application which fails to start synchronously using Wait-for-Signal 
 without timeout and 1 retry.
*/
void CSsTestStepAppTimeout::DoTestWaitForSignalWithRetriesForFailureAppL()
	{	
	INFO_PRINTF1(_L("Performing Wait-for-Signal test for failure app with retries and no timeout"));

	TInt completionCode = DoTestAndStopperL(1, 0, KTestAppRvError);

	TEST(completionCode == -2012);
	INFO_PRINTF2( _L("Anticipated completion code -2012. Supplied completion code %d"),completionCode);
	// Since ssmtestapprverror application is terminated with KErrGeneral each it is started, so
	// it doesn't start anytime irrespective of no. of retries specified.
	INFO_PRINTF1( _L("ssmtestapprverror.exe would not be started even after 1 retry as the app Rendezvous with error"));
	TEST( 0 == FindAndKill(KTestAppRvError) );
	}

/**
 Start a test application which fails to start twice and starts on the 3rd retry 
 synchronously using Wait-for-Signal without timeout and 3 retries.
*/
void CSsTestStepAppTimeout::DoTestWaitForSignalWithRetriesForSuccessfullAppL()
	{	
	INFO_PRINTF1(_L("Performing Wait-for-Signal test without timeout and with retries"));

	// Write the retry number to a file to be read, updated 
	// and eventually deleted by the test-app.
	RFs fs;	
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	CleanupClosePushL( writeStream );

	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(writeStream.Replace(fs, KFileForFAndF, EFileWrite));

	const TInt KSsTestRequiredDelay = 0;
	const TInt KSsTestFailCount = 1;
	const TInt KSsTestRetryCount = 0;
	writeStream.WriteInt8L(KSsTestRetryCount);
	writeStream.WriteInt8L(KSsTestFailCount);
	writeStream.WriteInt8L(KSsTestRequiredDelay);
	writeStream.CommitL();
	writeStream.Close();

	const TInt KRetries = KSsTestFailCount + 1;
	TInt completionCode = DoTestAndStopperL(KRetries, 0, KTestAppForFAndF);
	TEST(completionCode == 0);
	INFO_PRINTF2( _L("Anticipated completion code 0. Supplied completion code %d"),completionCode);
	RFileReadStream readStream;
	CleanupClosePushL(readStream);

	User::LeaveIfError(readStream.Open(fs, KFileForFAndF, EFileRead));
	TInt retryCount = readStream.ReadInt8L();
	readStream.Close();	
	fs.Delete(KFileForFAndF);

	CleanupStack::PopAndDestroy(3, &fs);

	TEST(retryCount == 1);

	INFO_PRINTF3( _L("ssmtestappsucessfulafterretry.exe is started after %d retries. anticipated retries value %d"),retryCount, 1);
	TEST(1 == FindAndKill(KTestAppForFAndF));
	}

/**
 Starts an instance of ssmtestappslow.exe asynchronously, allowing an insufficient period for it to rv.
*/
void CSsTestStepAppTimeout::DoTestAppTimeoutAsAppL()
	{
	INFO_PRINTF1( _L("\n\nPerforming App-timeout as app test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );

	startupProperties->SetFileParamsL( KTestAppSlow, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetTimeout( KWaitTime );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	ss->Start(*startupProperties, process, trs, id);
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.		
	if( KRequestPending == trs.Int() )
		{
		CActiveScheduler::Start();
		}

	
	TInt completionCode = stopper->CompletionCode();
	INFO_PRINTF3( _L("Anticipated completion code %d. Supplied completion code %d"), KErrTimedOut, completionCode );
	
	TEST(  KErrTimedOut == completionCode );
	
	CleanupStack::PopAndDestroy( 4, startupProperties );
	TEST( 0 == FindAndKill(KTestAppSlow) );	
	}


/**
  Starts an instance of ssmtestappslow.exe as a process asynchronously, 
  allowing an insufficient period for it to rv.
*/
void CSsTestStepAppTimeout::DoTestAppTimeoutAsProcL()
	{
	INFO_PRINTF1( _L("\n\nPerforming App-timeout as process test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );

	startupProperties->SetFileParamsL( KTestAppSlow, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	startupProperties->SetTimeout( KWaitTime );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	ss->Start(*startupProperties, process, trs, id);
	//Process is started using Wait for signal execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.
	if( KRequestPending == trs.Int() )
		{
		CActiveScheduler::Start();
		}

	
	TInt completionCode = stopper->CompletionCode();
	INFO_PRINTF3( _L("Anticipated completion code %d. Supplied completion code %d"), KErrTimedOut, completionCode );
	
	TEST(  KErrTimedOut == completionCode );
	
	CleanupStack::PopAndDestroy( 4, startupProperties );
	TEST( 0 == FindAndKill(KTestAppSlow) );
	}



TVerdict CSsTestStepAppTimeout::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();	
	}



TVerdict CSsTestStepAppTimeout::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}



CSsTestStepAppTimeout::CSsTestStepAppTimeout()
	{
	SetTestStepName(KCTestCaseAppTimeout);
	}



CSsTestStepAppTimeout::~CSsTestStepAppTimeout()
	{
	delete iActiveScheduler;
	}

