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
// Old Test CaseID 		APPFWK-SSS-0007
// New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0007
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "tss_startandcancel.h"


const TInt KSsDeletionTimeout = 300000; // us
const TInt KStartAndCancelNominalTimeout = 3000; // ms...Generous



TVerdict CSsTestStepStartAndCancel::doTestStepL()
	{
	__UHEAP_MARK;
	
	TRAPD(err, DoTestStartProcessAndCancelL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartProcessAndCancelL completed with err = %d"), err);

	TRAP(err, DoTestStartAppAndCancelL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestStartAppAndCancelL completed with err = %d"), err);

	TRAP(err, DoTestMultiStartAndDeleteL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiStartAndDeleteL completed with err = %d"), err);

	__UHEAP_MARKEND;
	return TestStepResult();	
	}


/**
 Start an instance of ssmtestappslow.exe asynchronously, as a process, specifying wait-for-start,
 but setting the timeout to zero which means that startsafe would wait indefinitely if need be.
 After a short period, StartCancel() is called on SsmStartSafe and the value of the TRS reported and tested
 via the mixin CSsTestStepStartAndCancel::SsTestNotify( TInt aNotify )
*/
void CSsTestStepStartAndCancel::DoTestStartProcessAndCancelL()
	{	
	INFO_PRINTF1( _L("\n\nPerforming async process start-and-cancel test") );
	
	CSsmStartupProperties* startupProperties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties );
	
	// Zero timeout specified, therefore RvObserver will wait indefinitely
	startupProperties->SetFileParamsL( KTestAppSlow, KNullDesC );
	startupProperties->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties->SetTimeout( KSsmStartSafeNoTimeout );
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	RProcess process;
	CleanupClosePushL( process );
	
	CTestAndCanceler* canceler = new(ELeave) CTestAndCanceler( *ss, *startupProperties, process, KSsTestTimeToCancel, this );
	CleanupStack::PushL( canceler );
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy( 4, startupProperties );

	FindAndKill( KTestAppSlow );
	}



/**
 Start an instance of ssmtestappslow.exe asynchronously, as an app, specifying wait-for-start,
 but setting the timeout to zero which means that startsafe would wait indefinitely if need be.
 After a short period, StartCancel() is called on SsmStartSafe and the value of the TRS reported and tested
 via the mixin CSsTestStepStartAndCancel::SsTestNotify( TInt aNotify )
*/
void CSsTestStepStartAndCancel::DoTestStartAppAndCancelL()
	{
	INFO_PRINTF1( _L("\n\nPerforming async app. start-and-cancel test") );
	
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
	
	CTestAndCanceler* canceler = new(ELeave) CTestAndCanceler( *ss, *startupProperties, process, KSsTestTimeToCancel, this );
	CleanupStack::PushL( canceler );
	
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy( 4, startupProperties );

	FindAndKill( KTestAppSlow );
	}



/**
 Start four instances of ssmtestappnorv.exe (which never rendezvous), although supplying a timeout
 so that the timeoutwaiter is instantiated for each of them. Then delete CSsmStartSafe and check the completion code of their TRSs.
 Start two as apps and two as processes.
*/
void CSsTestStepStartAndCancel::DoTestMultiStartAndDeleteL()
	{
	INFO_PRINTF1( _L("\n\nPerforming Multi-Start and delete SS test") );
	
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );

	CSsmStartupProperties* startupProperties_1 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_1 );
	
	// Define a startup properties as EStartProcess
	startupProperties_0->SetFileParamsL( KTestAppNoRv, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_0->SetTimeout( KStartAndCancelNominalTimeout );
	// Define a startup properties as EStartApp
	startupProperties_1->SetFileParamsL( KTestAppNoRv, KNullDesC );
	startupProperties_1->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties_1->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_1->SetTimeout( KStartAndCancelNominalTimeout );
	
	RProcess process_0;
	CleanupClosePushL( process_0 );
	RProcess process_1;
	CleanupClosePushL( process_1 );
	RProcess process_2;
	CleanupClosePushL( process_2 );
	RProcess process_3;
	CleanupClosePushL( process_3 );
	
	TInt id_0 = 0;
	TInt id_1 = 0;
	TInt id_2 = 0;
	TInt id_3 = 0;
	
	CStartAcknowledge* ack_0 = new(ELeave) CStartAcknowledge( this );
	CleanupStack::PushL( ack_0 );
	CStartAcknowledge* ack_1 = new(ELeave) CStartAcknowledge( this );
	CleanupStack::PushL( ack_1 );
	CStartAcknowledge* ack_2 = new(ELeave) CStartAcknowledge( this );
	CleanupStack::PushL( ack_2 );
	CStartAcknowledge* ack_3 = new(ELeave) CStartAcknowledge( this );
	CleanupStack::PushL( ack_3 );	
	
	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_1, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_1, process_3, ack_3->iStatus, id_3);
	CleanupStack::Pop( ss );
	
	// Ao to timeout and delete the SS
	CSsPauseDeleter* pauseDeleter = new(ELeave) CSsPauseDeleter( KSsDeletionTimeout, ss );
	CleanupStack::PushL( pauseDeleter );
	
	// Times-out and stops the sched.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 5 ); // 10s
	CleanupStack::PushL( pauseStopper );
	
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{		
		CActiveScheduler::Start();
		
		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Expected values                 %d %d %d %d "), KErrCancel, KErrCancel, KErrCancel, KErrCancel );
		
		TEST( ack_0->iStatus == KErrCancel );
		TEST( ack_1->iStatus == KErrCancel );
		TEST( ack_2->iStatus == KErrCancel );
		TEST( ack_3->iStatus == KErrCancel );
		}
	

	CleanupStack::PopAndDestroy( 12, startupProperties_0 );

	TEST( 4 == FindAndKill(KTestAppNoRv) );
	}



TVerdict CSsTestStepStartAndCancel::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}



/** */
TVerdict CSsTestStepStartAndCancel::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


/**
 From MSsTestAsyncNotifier
*/
void CSsTestStepStartAndCancel::SsTestNotify( TInt aNotify )
	{
	TEST( KErrCancel == aNotify );

	INFO_PRINTF3( _L("Expected completion code %d. Supplied completion code %d"), KErrCancel, aNotify );
	}



CSsTestStepStartAndCancel::CSsTestStepStartAndCancel()
	{
	SetTestStepName( KCTestCaseStartAndCancel );
	}



CSsTestStepStartAndCancel::~CSsTestStepStartAndCancel()
	{
	delete iActiveScheduler;
	}

