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
// Old Test CaseID 		APPFWK-SSS-0010
// New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0010
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "tss_procstartwithfandf.h"
#include "ssmtestappslow.h"
#include "ssmtestapps.h"

const TInt KProcStartStopperTimeout = 5000000;
const TInt KWaitTimeForTestAppForFAndF = 500;	// 500ms is the delay given for the KTestAppForFAndF test app for each retry.

TVerdict CSsTestStepProcStartWithFAndF::doTestStepL()
	{
	__UHEAP_MARK;
	
	TInt threadHandles_Before;
	TInt processHandles_Before;
	TInt threadHandles_After;
	TInt processHandles_After;
		
	RThread().HandleCount(processHandles_Before, threadHandles_Before);
	
	TRAPD(err, DoTestFAndFWithLessTimeOutAndWithRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFAndFWithLessTimeOutAndWithRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestFAndFWithLessTimeOutAndNoRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFAndFWithLessTimeOutAndNoRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestFAndFWithMoreTimeOutAndNoRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFAndFWithMoreTimeOutAndNoRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestFAndFWithMoreTimeOutAndWithRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFAndFWithMoreTimeOutAndWithRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestFAndFWithRetriesForFailureAppL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFAndFWithRetriesForFailureAppL completed with err = %d"), err);
	
	TRAP(err, DoTestFAndFWithLessTimeOutAndMoreRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFAndFWithLessTimeOutAndMoreRetriesL completed with err = %d"), err);
	
	TRAP(err, DoTestFAndFWithRetriesForSuccessfullAppL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFAndFWithRetriesForSuccessfullAppL completed with err = %d"), err);
	
	RThread().HandleCount(processHandles_After, threadHandles_After);
	TEST(processHandles_After == processHandles_Before);
	TEST(threadHandles_After == threadHandles_Before);
	
	__UHEAP_MARKEND;
	return TestStepResult();	
	}

void CSsTestStepProcStartWithFAndF::DoTestAndStopperL(TInt aRetries, TInt aTimeOut, const TDesC& aFileName)
	{
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL(startupProperties);
	
	startupProperties->SetFileParamsL(aFileName, KNullDesC);
	startupProperties->SetCommandTypeL(ESsmCmdStartProcess);
	startupProperties->SetExecutionBehaviour(ESsmFireAndForget);
	startupProperties->SetTimeout(aTimeOut);
	startupProperties->SetRetries(aRetries);
	
	RProcess process;
	CleanupClosePushL(process);
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper(*ss, *startupProperties, process, KProcStartStopperTimeout, this);
	CleanupStack::PushL(testAndStopper);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(4, startupProperties);
	}

/**
 Starts an instance of ssmtestappslow.exe synchronously as a process using fire-and-forget.
 No retries are specified. Timeout is specified, therefore CTimeoutWaiter is not instantiated.
*/
void CSsTestStepProcStartWithFAndF::DoTestFAndFWithLessTimeOutAndWithRetriesL()
	{	
	INFO_PRINTF1(_L("Performing Fire-and-forget Timeout test with retries"));
	
	const TInt KTestLessTimeOut = KTestAppSlowTimeoutInMilliSeconds-100;
	DoTestAndStopperL(1, KTestLessTimeOut, KTestAppSlow);

	// ssmtestappslow.exe would terminated due to timeout. 
	// Since ssmtestappslow application takes more time to start than the timeout specified in the ssmsstartupproperties,
	// it doesn't start anytime irrespective of no. of retries specified.
	TEST(0 == FindAndKill(KTestAppSlow));
	INFO_PRINTF1( _L("ssmtestappslow.exe would be terminated due to timeout"));
	}

/**
 Start a ssmtestappslow test application synchronously using fire-and-forget with timeout
 less than the time required for the application to start and without retries.
*/
void CSsTestStepProcStartWithFAndF::DoTestFAndFWithLessTimeOutAndNoRetriesL()
	{	
	INFO_PRINTF1(_L("Performing Fire-and-forget Timeout test with no retries"));
	
	const TInt KTestLessTimeOut = KTestAppSlowTimeoutInMilliSeconds-100;
	DoTestAndStopperL(0, KTestLessTimeOut, KTestAppSlow);

	// ssmtestappslow.exe would terminated due to timeout. 
	// Since ssmtestappslow application takes more time to start than the timeout specified in the ssmsstartupproperties
	TEST(0 == FindAndKill(KTestAppSlow));
	INFO_PRINTF1( _L("ssmtestappslow.exe would be terminated due to timeout"));
	}

/**
 Start a ssmtestappslow test application synchronously using fire-and-forget with timeout
 more than the time required for the application to start and without retries.
*/
void CSsTestStepProcStartWithFAndF::DoTestFAndFWithMoreTimeOutAndNoRetriesL()
	{	
	INFO_PRINTF1(_L("Performing Fire-and-forget Timeout test with no retries and.."));
	INFO_PRINTF1(_L("Timeout specified in ssmstartupproperties is more than timeout required for application startup"));
	
	const TInt KTestMoreTimeOut = KTestAppSlowTimeoutInMilliSeconds * 3;
	DoTestAndStopperL(0, KTestMoreTimeOut, KTestAppSlow);

	// ssmtestappslow.exe would be started as ssmtestappslow application takes less time to start than 
	// the timeout specified in the ssmsstartupproperties
	TEST(1 == FindAndKill(KTestAppSlow));
	}

/**
 Start a test application synchronously using fire-and-forget with timeout
 more than the time required for the application to start and which starts after the no. of retries given.
*/
void CSsTestStepProcStartWithFAndF::DoTestFAndFWithMoreTimeOutAndWithRetriesL()
	{	
	INFO_PRINTF1(_L("Performing Fire-and-forget test with timeout and retries"));

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
	DoTestAndStopperL(KRetries, KWaitTimeForTestAppForFAndF * 4, KTestAppForFAndF);

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
 Start a test application which fails to start synchronously using fire-and-forget 
 without timeout and 1 retry.
*/
void CSsTestStepProcStartWithFAndF::DoTestFAndFWithRetriesForFailureAppL()
	{	
	INFO_PRINTF1(_L("Performing Fire-and-forget test for failure app with retries and no timeout"));

	DoTestAndStopperL(1, 0, KTestAppRvError);

	// Since ssmtestapprverror application is terminated with KErrGeneral each it is started, so
	// it doesn't start anytime irrespective of no. of retries specified.
	INFO_PRINTF1( _L("ssmtestapprverror.exe would not be started even after 1 retry as the app Rendezvous with error"));
	TEST( 0 == FindAndKill(KTestAppRvError) );
	}

/**
 Start a test application which fails to start even with 2 retries remaining due to timeout
 synchronously using fire-and-forget.
*/
void CSsTestStepProcStartWithFAndF::DoTestFAndFWithLessTimeOutAndMoreRetriesL()
	{
	INFO_PRINTF1(_L("Performing Fire-and-forget test with Less Timeout and more retries"));

	// Write the retry number to a file to be read, updated 
	// and eventually deleted by the test-app.
	RFs fs;	
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	CleanupClosePushL( writeStream );

	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(writeStream.Replace(fs, KFileForFAndF, EFileWrite));

	const TInt KSsTestFailCount = 3;
	const TInt KSsTestRetryCount = 0;
	const TInt KSsTestRequiredDelay = 1;
	writeStream.WriteInt8L(KSsTestRetryCount);
	writeStream.WriteInt8L(KSsTestFailCount);
	writeStream.WriteInt8L(KSsTestRequiredDelay);
	writeStream.CommitL();
	writeStream.Close();
	
	const TInt KRetries = KSsTestFailCount + 1;
	DoTestAndStopperL(KRetries, KWaitTimeForTestAppForFAndF * 2.5, KTestAppForFAndF);

	RFileReadStream readStream;
	CleanupClosePushL(readStream);

	User::LeaveIfError(readStream.Open(fs, KFileForFAndF, EFileRead));
	TInt retryCount = readStream.ReadInt8L();
	readStream.Close();	
	fs.Delete(KFileForFAndF);
	
	CleanupStack::PopAndDestroy(3, &fs);
	// ssmtestappsucessfulafterretry.exe would terminated due to timeout. 
	TEST(0 == FindAndKill(KTestAppForFAndF));
	TInt retriesRemaining = KRetries-retryCount;
	// remaining retries would differ in case of emulator and h4 due to timeout required is different in each case.
	// so we are testing if there are any remaining retries rather than the exact number.
	TEST(retryCount >= 1);
	TEST(retriesRemaining >= 1);
	INFO_PRINTF2(_L("ssmtestappsucessfulafterretry.exe is not started even after %d retries. anticipated retries more than or equal to 1"),retryCount);
	INFO_PRINTF2(_L("Actual remaining retries %d. anticipated remaining retries more than or equal to 1"),retriesRemaining);
	}

/**
 Start a test application which fails to start twice and starts on the 3rd retry 
 synchronously using fire-and-forget without timeout and 3 retries.
*/
void CSsTestStepProcStartWithFAndF::DoTestFAndFWithRetriesForSuccessfullAppL()
	{	
	INFO_PRINTF1(_L("Performing Fire-and-forget test without timeout and with retries"));

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
	DoTestAndStopperL(KRetries, 0, KTestAppForFAndF);

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

TVerdict CSsTestStepProcStartWithFAndF::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	return CTestStep::doTestStepPreambleL();
	}

TVerdict CSsTestStepProcStartWithFAndF::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

CSsTestStepProcStartWithFAndF::CSsTestStepProcStartWithFAndF()
	{
	SetTestStepName(KCTestCaseProcStartWithFAndF);
	}

CSsTestStepProcStartWithFAndF::~CSsTestStepProcStartWithFAndF()
	{
	delete iActiveScheduler;
	}


