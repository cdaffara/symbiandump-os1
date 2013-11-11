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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tss_appstartrverror.h"
#include "ssmtestapps.h"
#include "ssmtestapprverror.h"

const TInt KStartSafeAppStartErrorNominalTimeout = 300; // milliseconds


/**
Old Test CaseID 		APPFWK-SSS-0008
New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0008
 */

TVerdict CSsTestStepAppStartRvError::doTestStepL()
	{
	__UHEAP_MARK;
	
	TRAPD(err, DoTestRvErrorProcAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestRvErrorProcAsyncL completed with err = %d"), err);

	TRAP(err, DoTestRvErrorProcSyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestRvErrorProcSyncL completed with err = %d"), err);
	
	TRAP(err, DoTestRvErrorAppAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestRvErrorAppAsyncL completed with err = %d"), err);

	TRAP(err, DoTestRvErrorAppSyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestRvErrorAppSyncL completed with err = %d"), err);

	__UHEAP_MARKEND;

	return TestStepResult();	
	}



/**
 Use SsmStartSafe to start the test app as a process asynchronously. The test app rv's with a unique error code.
*/
void CSsTestStepAppStartRvError::DoTestRvErrorProcAsyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Process-Start Rendezvous error Async test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppRvError, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	
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
	
	INFO_PRINTF3( _L("TRS completed with %d. anticipated value %d"), stopper->CompletionCode(), KSsmTestAppRvError );
	const TInt stopperCompletionCode = stopper->CompletionCode();
	
	CleanupStack::PopAndDestroy( 4, startupProperties );
	
	TEST( KSsmTestAppRvError == stopperCompletionCode );
	TEST( 0 == FindAndKill(KTestAppRvError) );	// SsmStartSafe should dispose of the poorly behaved app.
	}



/**
 Use SsmStartSafe to start the test app as a process synchronously. The test app rv's with a unique error code
 which should be reflected in the synchronous StartL's leave code, reported via SsTestNotify().
*/
void CSsTestStepAppStartRvError::DoTestRvErrorProcSyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Process-Start Rendezvous error Sync test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppRvError, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	
	RProcess process;
	CleanupClosePushL( process );
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *ss, *startupProperties, process, KStartSafeAppStartErrorNominalTimeout, this );
	CleanupStack::PushL( testAndStopper );
	
	CActiveScheduler::Start();

	// Rv error code reported via, and tested in SsTestNotify()
	
	CleanupStack::PopAndDestroy( 4, startupProperties );

	TEST( 0 == FindAndKill(KTestAppRvError) );	// SsmStartSafe should dispose of the poorly behaved app.	
	}



void CSsTestStepAppStartRvError::DoTestRvErrorAppAsyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming App-Start Rendezvous error Async test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppRvError, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	
	RProcess process;
	CleanupClosePushL( process );
	TInt id = 0;

	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TInt stopperCompletionCode = KErrNone;
	TRequestStatus& trs = stopper->Trs();
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	ss->Start(*startupProperties, process, trs, id);
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.
	if( KRequestPending == trs.Int() )
		{
		CActiveScheduler::Start();
		
		stopperCompletionCode = stopper->CompletionCode();
		INFO_PRINTF3( _L("TRS completed with %d. anticipated value %d"), stopperCompletionCode, KSsmTestAppRvError );
		}
	
	CleanupStack::PopAndDestroy( 4, startupProperties );
	
	TEST( KSsmTestAppRvError == stopperCompletionCode );
	TEST( 0 == FindAndKill(KTestAppRvError) );	// SsmStartSafe should dispose of the poorly behaved app.	
	}



void CSsTestStepAppStartRvError::DoTestRvErrorAppSyncL()
	{
	INFO_PRINTF1( _L("\n\nPerforming App-Start Rendezvous error Sync test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	startupProperties->SetFileParamsL( KTestAppRvError, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	
	RProcess process;
	CleanupClosePushL( process );
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *ss, *startupProperties, process, KStartSafeAppStartErrorNominalTimeout, this );
	CleanupStack::PushL( testAndStopper );
	
	CActiveScheduler::Start();

	// Rv error code reported via, and tested in SsTestNotify()
	
	CleanupStack::PopAndDestroy( 4, startupProperties );

	TEST( 0 == FindAndKill(KTestAppRvError) );	// SsmStartSafe should dispose of the poorly behaved app.	
	}



// from MSsTestAsyncNotifier
void CSsTestStepAppStartRvError::SsTestNotify( TInt aNotify )
	{
	INFO_PRINTF3( _L("StartL supplied eror code %d. anticipated value %d"), aNotify, KSsmTestAppRvError );

	TEST( KSsmTestAppRvError == aNotify );
	}



TVerdict CSsTestStepAppStartRvError::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}



TVerdict CSsTestStepAppStartRvError::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}



CSsTestStepAppStartRvError::CSsTestStepAppStartRvError()
	{
	SetTestStepName( KCTestCaseAppStartRvError );
	}


CSsTestStepAppStartRvError::~CSsTestStepAppStartRvError()
	{
	delete iActiveScheduler;
	}


