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
//

#include <ssm/ssmstartupproperties.h>

#include "tss_step_base.h"
#include "tss_errors.h"
#include <ssm/ssmstartsafe.h>
#include "ssmtestapps.h"
#include "testandstopper.h"
#include "testandcanceler.h"
#include "ssmtestprocgoodsession.h"
#include "ssmtestapprvafterretry.h"
//#include "ssmtestapppanic.h"
#include "ssmatest_utils.h"


const TInt KStartMonStopperTimeout = 5000000; // Could (probably) be shaved.
const TInt KSsTestAppStartTimeout = 2000;

/**
Old Test CaseID 		APPFWK-SSS-009
New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0015
 */

TVerdict CSsTestStepErrors::doTestStepL()
	{
	__UHEAP_MARK;

	TRAPD(err, DoTestNoMonitorFlagL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestNoMonitorFlagL completed with err = %d"), err);

	TRAP(err, DoTestStartAsProcessWithNullFileNameL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAsProcessWithNullFileNameL completed with err = %d"), err);

	TRAP(err, DoTestStartAsAppWithNullFileNameL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAsAppWithNullFileNameL completed with err = %d"), err);
	
	TRAP(err, DoTestStartAsProcessWithFictitiousFileNameL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAsProcessWithFictitiousFileNameL completed with err = %d"), err);

	TRAP(err, DoTestStartAsAppWithFictitiousFileNameL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAsAppWithFictitiousFileNameL completed with err = %d"), err);

	TRAP(err, DoTestCommandTypeOutOfRangeL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestCommandTypeOutOfRangeL completed with err = %d"), err);

	TRAP(err, DoTestStartMethodOutOfRangeL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartMethodOutOfRangeL completed with err = %d"), err);

	TRAP(err, DoTestNegativeTimeoutL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestNegativeTimeoutL completed with err = %d"), err);

	TRAP(err, DoTestNegativeRetriesL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestNegativeRetriesL completed with err = %d"), err);

	TRAP(err, DoTestNegativeRetriesStartupFailsL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestNegativeRetriesStartupFailsL completed with err = %d"), err);

	TRAP(err, DoTestFireAndForgetWithNullFilenameL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestFireAndForgetWithNullFilenameL completed with err = %d"), err);

	TRAP(err, DoTestStartandCancelWithBadIndexL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartandCancelWithBadIndexL completed with err = %d"), err);

	TRAP(err, DoTestStartAppWhichPanicsKErrNoneAsAppL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAppWhichPanicsKErrNoneAsAppL completed with err = %d"), err);

	TRAP(err, DoTestStartAppWhichPanicsKErrNoneAsProcessL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAppWhichPanicsKErrNoneAsProcessL completed with err = %d"), err);

	TRAP(err, DoTestStartAppWhichPanicsAsProcessL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAppWhichPanicsAsProcessL completed with err = %d"), err);

	TRAP(err, DoTestStartAppWhichPanicsAsAppL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAppWhichPanicsAsAppL completed with err = %d"), err);

	__UHEAP_MARKEND;
	
	TRAP(err, DoTestOOML());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestOOML completed with err = %d"), err);
	
	//Some of the tests test for negative conditions and leave a panic dialog on the screen. These panic dialogs need to be closed
	CloseAllPanicWindowsL();
	return TestStepResult();
	}
	
	

/**
 Atempt to start ssmtestprocgood via the StartAndMonitorL() method. Supply a correctly formed CSsmStartupProperties 
 with the exception of its monitor flag.
 The method should leave with KErrArgument.
*/
void CSsTestStepErrors::DoTestNoMonitorFlagL()	
	{
	INFO_PRINTF1( _L("\n\nAttempt StartandMonitor() without specifying in startup properties") );
	
	iScenario = ESsErrorScenarioNoMonitorFlag;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestProcGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal ); // Rem: EssmFireAndForget is disallowed
	// No monitoring has been specified


	RProcess process;
	CleanupClosePushL( process );
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	// This class calls StartAndMonitorL() even though monitoring may not have been specified.
	CDuffTestAndStopper* duffTestAndStopper = new(ELeave) CDuffTestAndStopper( *ss, *startupProperties, process, KStartMonStopperTimeout, this );
	CleanupStack::PushL( duffTestAndStopper );
	CActiveScheduler::Start();
	
	INFO_PRINTF1( _L("Ensure no instances were started") );
	TEST( 0 == FindAndKill(KTestProcGood) );
	
	CleanupStack::PopAndDestroy( 4, startupProperties );
	}
	
	

/**
 Attempt a start-as-process, supplying CSsmStartupProperties containing a NULL filename.
 
  Note: The error code supplied in this case comes from RProcess::Create.
*/	
void CSsTestStepErrors::DoTestStartAsProcessWithNullFileNameL()
	{
	INFO_PRINTF1( _L("\n\nAttempt start as process with NULL filename") );

	// Start as process. NULL filename.
	iScenario = ESsErrorScenarioNullFileNameProcess;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KNullDesC, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 0 );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	CleanupStack::PopAndDestroy( 4, startupProperties );
	}
	


/**
 Attempt a start-as-app, supplying CSsmStartupProperties containing a NULL filename.
 
 Note: The error code supplied in this case comes from AppArc.
*/
void CSsTestStepErrors::DoTestStartAsAppWithNullFileNameL()	
	{
	INFO_PRINTF1( _L("\n\nAttempt start as app with NULL filename") );

	// Start as app. NULL filename.
	iScenario = ESsErrorScenarioNullFileNameApp;

	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KNullDesC, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 0 );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}



/**
 Attempt a start-as-process, supplying CSsmStartupProperties containing a fictitious filename.
 
  Note: The error code supplied in this case comes from RProcess::Create.
*/	
void CSsTestStepErrors::DoTestStartAsProcessWithFictitiousFileNameL()
	{
	INFO_PRINTF1( _L("\n\nAttempt start as process with fictitious filename") );

	iScenario = ESsErrorScenarioFictitiousFileNameProcess;
	_LIT( KFictitiousFilename, "gruntfuttock.exe");
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KFictitiousFilename, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 0 );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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
	
	CleanupStack::PopAndDestroy( 4, startupProperties );
	}
	


/**
 Attempt a start-as-app, supplying CSsmStartupProperties containing a fictitious filename.
 
 Note: The error code supplied in this case comes from AppArc.
*/
void CSsTestStepErrors::DoTestStartAsAppWithFictitiousFileNameL()	
	{
	INFO_PRINTF1( _L("\n\nAttempt start as app with fictitious filename") );

	iScenario = ESsErrorScenarioFictitiousFileNameApp;
	_LIT( KFictitiousFilename, "gruntfuttock.exe");
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KFictitiousFilename, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 0 );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}



/**
 Attempt to start ssmtestappgood.exe with a CommandType parameter which is out of range.
*/
void CSsTestStepErrors::DoTestCommandTypeOutOfRangeL()
	{
	INFO_PRINTF1( _L("\n\nAttempt start. CommandType parameter out of range") );

	iScenario = ESsErrorScenarioCommandTypeOutOfRange;
	const TInt KStartupParamOutOfRange = 500;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	TRAPD( err, startupProperties->SetCommandTypeL( (TSsmCommandType)KStartupParamOutOfRange ));
	TEST(KErrArgument == err);
	INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), err, KErrArgument);
	    
	CleanupStack::PopAndDestroy( 1, startupProperties );	
	}



/**
 Attempt to start ssmtestappgood.exe with a StartMethod parameter which is out of range.
*/
void CSsTestStepErrors::DoTestStartMethodOutOfRangeL()
	{
	INFO_PRINTF1( _L("\n\nAttempt start. StartMethod parameter out of range") );

	iScenario = ESsErrorScenarioStartMethodOutOfRange;
	const TInt KStartMethodParamOutOfRange = 500;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( (TSsmExecutionBehaviour)KStartMethodParamOutOfRange );
	startupProperties->SetRetries( 0 );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	ss->Start(*startupProperties, process, trs, id);
	//Process are started using Invalid execution behaviour, 
	//Start method always sets TRequestStatus to KErrArgument.	
	if( KErrArgument == trs.Int() )
		{
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}
 


/**
 Attempt to start ssmtestappgood.exe with a -ve timeout parameter.
 This should start the test-app successfully, treating the -ve parameter
 as meaning zero, or 'wait indefinitely'.
*/
void CSsTestStepErrors::DoTestNegativeTimeoutL()
	{
	INFO_PRINTF1( _L("\n\nAttempt start. -ve timeout parameter") );

	iScenario = ESsErrorScenarioNegativeTimeout;
	const TInt KNegativeTimeout = -500;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 0 );
	startupProperties->SetTimeout( KNegativeTimeout );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	TEST( 1 == FindAndKill(KTestAppGood) );
	
	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}



/**
 Attempt to start ssmtestappgood.exe with a -ve retries parameter.
 This should start the test-app successfully, treating the -ve parameter
 as meaning 'zero retries'
*/
void CSsTestStepErrors::DoTestNegativeRetriesL()
	{
	INFO_PRINTF1( _L("\n\nAttempt start. -ve retries ") );

	iScenario = ESsErrorScenarioNegativeRetries;
	const TInt KNegativeRetries = -500;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( KNegativeRetries );
	startupProperties->SetTimeout( KSsTestAppStartTimeout );

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	TEST( 1 == FindAndKill(KTestAppGood) );
	
	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}



/**
 Attempt to start ssmtestapprvafterretry.exe with a -ve retries parameter.
 The app is configured (via the file-write) to start after one retry.
 Therefore this test should fail to start the app as the -ve retries param is
 interpreted to mean 'zero retries'.
 The test should therefore report KErrTimedOut

*/
void CSsTestStepErrors::DoTestNegativeRetriesStartupFailsL()
	{
	INFO_PRINTF1( _L("\n\nAttempt start the retry app. -ve retries param") );

	iScenario = ESsErrorScenarioNegativeRetriesFails;
	const TInt KNegativeRetries = -500;
	
	
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
	
	const TInt KDontRvCount = 1;
	writeStream.WriteInt8L( KDontRvCount ); // KDontRvCount is declared in ssmtestapprvafterretry.h
	writeStream.CommitL();		
	
	CleanupStack::PopAndDestroy( 2, &fs );
	//	
	
	
	
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppRvAfterRetry, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( KNegativeRetries );
	startupProperties->SetTimeout( KSsTestAppStartTimeout - 1); // 1ms delay, because the delay in the test application for each retry is 2ms.

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	TEST( 0 == FindAndKill(KTestAppRvAfterRetry) );
	
	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}
	
	

/**
 Attempt F&F supplying NULL filename
*/
void CSsTestStepErrors::DoTestFireAndForgetWithNullFilenameL()
	{
	INFO_PRINTF1( _L("\n\nAttempt fire and Forget with NULL filename") );

	iScenario = ESsErrorScenarioFireAndForgetWithNullFilename;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KNullDesC, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmFireAndForget ); 

	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	
	ss->Start(*startupProperties, process, trs, id);
	//Process are started using Fire-and-Forget execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.
	if( KErrNone == trs.Int() )
		{
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}



/**
 Check that if StartCancel is called with an index who's value is out of range, its value on return is KErrArgument
*/
void CSsTestStepErrors::DoTestStartandCancelWithBadIndexL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Start-and-cancel supplying duff index value") );
	
	iScenario = ESsErrorScenarioStartAndCancelWithDuffIndex;
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	// Zero timeout specified, therefore RvObserver will wait indefinitely
	startupProperties->SetFileParamsL( KTestAppSlow, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetTimeout( KSsmStartSafeNoTimeout );
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	RProcess process;
	CleanupClosePushL( process );
	const TInt KSsTestTimeToCancel = 10000; // 10 ms
	
	CTestAndCanceler* canceler = new(ELeave) CTestAndCanceler( *ss, *startupProperties, process, 
																KSsTestTimeToCancel, this, ESsTestDuffBehaviourBadIndex );
	CleanupStack::PushL( canceler );
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy( 4, startupProperties );

	TEST( 1 == FindAndKill(KTestAppSlow) );
	}



/**
 Start as an app, the test app which panics with KErrNone. Ensure SsmStartSafe translates this to KErrGeneral.
 Retries are specified.
*/
void CSsTestStepErrors::DoTestStartAppWhichPanicsKErrNoneAsAppL()
	{
	INFO_PRINTF1( _L("\n\nStart as app a test application which panics with KErrNone") );
	
	iScenario = ESsErrorScenarioStartAppWhichPanicsKErrNoneAsApp;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppPanicZero, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	// ssmstartsafe would timeout if app is not restarted after 3 retries and the time is out
	startupProperties->SetTimeout( KSsTestAppStartTimeout * 3);
	
	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}



/**
 Start as a process, the test app which panics with KErrNone. Ensure SsmStartSafe translates this to KErrGeneral.
 Retries are specified.
*/
void CSsTestStepErrors::DoTestStartAppWhichPanicsKErrNoneAsProcessL()
	{
	INFO_PRINTF1( _L("\n\nStart as process a test application which panics with KErrNone") );
	
	iScenario = ESsErrorScenarioStartAppWhichPanicsKErrNoneAsProcess;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppPanicZero, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	// ssmstartsafe would timeout if app is not restarted after 3 retries and the time is out
	startupProperties->SetTimeout( KSsTestAppStartTimeout * 3 );
	
	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	CleanupStack::PopAndDestroy( 4, startupProperties );
	}



/**
 Start as an app, the test app which panics.
 Retries are specified.
*/
void CSsTestStepErrors::DoTestStartAppWhichPanicsAsAppL()
	{
	INFO_PRINTF1( _L("\n\nStart as app a test application which panics") );
	
	iScenario = ESsErrorScenarioStartAppWhichPanicsAsApp;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppPanic, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	// ssmstartsafe would timeout if app is not restarted after 3 retries and the time is out
	startupProperties->SetTimeout( KSsTestAppStartTimeout * 3);
	
	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	CleanupStack::PopAndDestroy( 4, startupProperties );	
	}



/**
 Start as a process, the test app which panics.
 Retries are specified.
*/
void CSsTestStepErrors::DoTestStartAppWhichPanicsAsProcessL()
	{
	INFO_PRINTF1( _L("\n\nStart as process a test application which panics") );
	
	iScenario = ESsErrorScenarioStartAppWhichPanicsAsProcess;
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppPanic, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetRetries( 3 );
	// ssmstartsafe would timeout if app is not restarted after 3 retries and the time is out
	startupProperties->SetTimeout( KSsTestAppStartTimeout * 3);
	
	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout, this );
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

	CleanupStack::PopAndDestroy( 4, startupProperties );
	}



/**
 Heap allocation check under OOM.
*/
void CSsTestStepErrors::DoTestOOML()
	{
	INFO_PRINTF1(_L("\n\nCSsmStartSafe construction under OOM"));
	iScenario = ESsErrorScenarioOOM; // Not used at the time of writing
	CSsmStartSafe* ss = NULL;
	TInt failRate = 1;
	
	for ( ;;failRate++ )
	    {
	    __UHEAP_RESET;
	    __UHEAP_SETFAIL( RHeap::EDeterministic, failRate );
	    __UHEAP_MARK;

	    TRAPD(err, 
	    	{
	    	ss = CSsmStartSafe::NewLC();
	    	CleanupStack::Pop( ss );
	    	})
	    		   
	    
        TEST( (err == KErrNone || err == KErrNoMemory) );
	    if ( err != KErrNone )
	        {
	        __UHEAP_MARKEND;
	        TEST( ss == NULL );
	        }
	    else
	        {
	        TEST( ss != NULL );
	        delete ss;
	        ss = NULL;
	        __UHEAP_MARKEND;
	        break;
	        }
	    }
	__UHEAP_RESET;
	INFO_PRINTF2( _L("  #allocs for construction: %d."), failRate - 1 );	
	}

TVerdict CSsTestStepErrors::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}



TVerdict CSsTestStepErrors::doTestStepPostambleL()
	{
	// In case any of the tests were inadvertently successful in starting the test-app
	FindAndKill( KTestAppGood );
	FindAndKill( KTestProcGood );
	FindAndKill( KTestAppRvAfterRetry );
	FindAndKill( KTestAppPanicZero );
	FindAndKill( KTestAppSlow );
	
	return TestStepResult();	
	}
	


void CSsTestStepErrors::SsTestNotify( TInt aNotify )
	{

	TInt anticipated = KErrNone;
	
	switch( iScenario )
		{
		case ESsErrorScenarioNoMonitorFlag:	
			{
			anticipated = KErrArgument;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;
					
		case ESsErrorScenarioNullFileNameProcess:	
			{
			anticipated = KErrNotFound;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;
			
		case ESsErrorScenarioNullFileNameApp:	
			{
			anticipated = KErrNotFound;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;

		case ESsErrorScenarioFictitiousFileNameProcess:	
			{
			anticipated = KErrNotFound;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;

		case ESsErrorScenarioFictitiousFileNameApp:	
			{
			anticipated = KErrNotFound;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;

		case ESsErrorScenarioCommandTypeOutOfRange:	
			{
			anticipated = KErrArgument;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;

		case ESsErrorScenarioStartMethodOutOfRange:	
			{
			anticipated = KErrArgument;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;
			
		case ESsErrorScenarioNegativeTimeout:	
			{
			anticipated = KErrNone;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;		
			
		case ESsErrorScenarioNegativeRetries:	
			{
			anticipated = KErrNone;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;
			
		case ESsErrorScenarioNegativeRetriesFails:	
			{
			anticipated = KErrTimedOut;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;

		case ESsErrorScenarioFireAndForgetWithNullFilename:	
			{
			anticipated = KErrArgument;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;

		case ESsErrorScenarioStartAndCancelWithDuffIndex:	
			{
			anticipated = KErrArgument;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;

		case ESsErrorScenarioStartAppWhichPanicsKErrNoneAsApp:	
		case ESsErrorScenarioStartAppWhichPanicsKErrNoneAsProcess:
			{
			anticipated = KErrGeneral;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;

		case ESsErrorScenarioStartAppWhichPanicsAsApp:	
		case ESsErrorScenarioStartAppWhichPanicsAsProcess:
			{
			const TInt KSsErrorsTestPanicNumber = -130258;
			anticipated = KSsErrorsTestPanicNumber;
			
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;
			
		case ESsErrorScenarioOOM:	
			{
			anticipated = NULL;
			// Unused at the time of writing
			INFO_PRINTF3( _L("Supplied code %d Anticipated code %d"), aNotify, anticipated );
			TEST( anticipated == aNotify );
			}
			break;
			
		default:
			{
			INFO_PRINTF2( _L("Unanticipated scenario. Error code %d"), aNotify );
			TEST( KErrNotSupported );
			}
			break;
		}
	
	}



CSsTestStepErrors::CSsTestStepErrors()
	{
	SetTestStepName( KStartSafeErrors );	
	}



CSsTestStepErrors::~CSsTestStepErrors()
	{
	delete iActiveScheduler;
	}
	
	
	
