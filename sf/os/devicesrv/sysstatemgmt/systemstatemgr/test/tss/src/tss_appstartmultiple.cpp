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
// Old Test CaseID 		APPFWK-SSS-0004
// New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0004
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "tss_appstartmultiple.h"


const TInt KSsTestAppStartTimeout = 3000; // ms
const TInt KInduceTimeoutFailWaitTime = 300; //ms
const TInt KTestMultiRetries = 5; // for a successful start this value must exceed KDontRvCount (declared in ssmtestapprvafterretry.h)



TVerdict CSsTestStepAppStartMultiple::doTestStepL()
	{
	__UHEAP_MARK;
	
	TRAPD(err, DoTestMultiAppAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiAppAsyncL completed with err = %d"), err);

	TRAP(err, DoTestMultiAppWithTimeoutAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiAppWithTimeoutAsyncL completed with err = %d"), err);
	
	TRAP(err, DoTestMultiAppAllTimeoutAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiAppAllTimeoutAsyncL completed with err = %d"), err);
	
	TRAP(err, DoTestMultiAppIncludingRetryAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiAppIncludingRetryAsyncL completed with err = %d"), err);

	TRAP(err, DoTestMultiProcAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiProcAsyncL completed with err = %d"), err);
	
	TRAP(err, DoTestMultiProcWithTimeoutAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiProcWithTimeoutAsyncL completed with err = %d"), err);
	
	TRAP(err, DoTestMultiProcAllTimeoutAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiProcAllTimeoutAsyncL completed with err = %d"), err);
	
	TRAP(err, DoTestMultiProcIncludingRetryAsyncL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("DoTestMultiProcIncludingRetryAsyncL completed with err = %d"), err);

	__UHEAP_MARKEND;
	return TestStepResult();	
	}



//
// START AS APPS
//
/**
  Concurrently starts four instances of ssmtestappgood.exe
  There is no timeout specified, therefore no instances of CTimeoutWaiter are constructed.
 */
void CSsTestStepAppStartMultiple::DoTestMultiAppAsyncL()
	{	

	INFO_PRINTF1( _L("\n\n Performing Multiple App-Start Async test---no timeout") );
	
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	// Slightly cheeky. Use the same startup properties for each call
	startupProperties_0->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	
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
	// Use this timeout AO. Therefore not making assumptions about the order of completions.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 2);
	CleanupStack::PushL( pauseStopper );

	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_0, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_0, process_3, ack_3->iStatus, id_3);
	
	INFO_PRINTF5( _L("id_0 to id_3  have the values  %d %d %d %d "), id_0, id_1, id_2, id_3 );
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{
		CActiveScheduler::Start();
		
		TBuf<KMaxFileName> buf( process_0.FileName() );
		INFO_PRINTF2( _L("process_0 filename %S"), &buf );
		
		buf.Copy( process_1.FileName() );
		INFO_PRINTF2( _L("process_1 filename %S"), &buf );
		
		buf.Copy( process_2.FileName() );	
		INFO_PRINTF2( _L("process_2 filename %S"), &buf );
		
		buf.Copy( process_3.FileName() );
		INFO_PRINTF2( _L("process_3 filename %S"), &buf );
	
		
		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Anticipated values were         %d %d %d %d "), KErrNone, KErrNone, KErrNone, KErrNone );
	
		TEST( ack_0->iStatus == KErrNone );
		TEST( ack_1->iStatus == KErrNone );
		TEST( ack_2->iStatus == KErrNone );
		TEST( ack_3->iStatus == KErrNone );
		}
	
	CleanupStack::PopAndDestroy( 11, startupProperties_0 );

	TEST( 4 == FindAndKill(KTestAppGood) );
	}



/**
  Concurrently starts four instances of ssmtestappgood.exe
  A timeout value is specified which ensures that intances of CTimeoutWaiter are created
 */
void CSsTestStepAppStartMultiple::DoTestMultiAppWithTimeoutAsyncL()
	{	

	INFO_PRINTF1( _L("\n\n Performing Multiple App-Start Async test---*with* timeout") );
	
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	// Slightly cheeky. Use the same startup properties for each call
	// Timeout is specified, therefore there are instances of CTimeoutWaiter constructed.
	startupProperties_0->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_0->SetTimeout( KSsTestAppStartTimeout );
	
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
	

	// Use this timeout AO. Therefore not making assumptions about the order of completions.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 5 );
	CleanupStack::PushL( pauseStopper );

	CSsmStartSafe* ss  = CSsmStartSafe::NewLC();
	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_0, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_0, process_3, ack_3->iStatus, id_3);
	
	INFO_PRINTF5( _L("id_0 to id_3  have the values  %d %d %d %d "), id_0, id_1, id_2, id_3 );
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{
		CActiveScheduler::Start();
		
		TBuf<KMaxFileName> buf( process_0.FileName() );
		INFO_PRINTF2( _L("process_0 filename %S"), &buf );
		
		buf.Copy( process_1.FileName() );
		INFO_PRINTF2( _L("process_1 filename %S"), &buf );
		
		buf.Copy( process_2.FileName() );	
		INFO_PRINTF2( _L("process_2 filename %S"), &buf );
		
		buf.Copy( process_3.FileName() );
		INFO_PRINTF2( _L("process_3 filename %S"), &buf );
	
		
		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Anticipated values were         %d %d %d %d "), KErrNone, KErrNone, KErrNone, KErrNone );
		
		TEST( ack_0->iStatus == KErrNone );
		TEST( ack_1->iStatus == KErrNone );
		TEST( ack_2->iStatus == KErrNone );
		TEST( ack_3->iStatus == KErrNone );
		}
	
	CleanupStack::PopAndDestroy( 11, startupProperties_0 );

	TEST( 4 == FindAndKill(KTestAppGood) );
	}



/**
  Concurrently starts four instances of ssmtestappnorv.exe
  All of these timeout (naturalment)
 */
void CSsTestStepAppStartMultiple::DoTestMultiAppAllTimeoutAsyncL()
	{	

	INFO_PRINTF1( _L("\n\n Performing Multiple App-Start Async test---ALL timeout") );
	
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	// Slightly cheeky. Use the same startup properties for each call
	// Timeout is specified, therefore there are instances of CTimeoutWaiter constructed.
	startupProperties_0->SetFileParamsL( KTestAppNoRv, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_0->SetTimeout( KInduceTimeoutFailWaitTime );
	
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
	
	// Use this timeout AO. Therefore not making assumptions about the order of completions.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 5 );
	CleanupStack::PushL( pauseStopper );
	
	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_0, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_0, process_3, ack_3->iStatus, id_3);
	
	INFO_PRINTF5( _L("id_0 to id_3  have the values  %d %d %d %d "), id_0, id_1, id_2, id_3 );
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{
		CActiveScheduler::Start();

		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Anticipated values were         %d %d %d %d "), KErrTimedOut, KErrTimedOut, KErrTimedOut, KErrTimedOut );
		
		TEST( ack_0->iStatus == KErrTimedOut );
		TEST( ack_1->iStatus == KErrTimedOut );
		TEST( ack_2->iStatus == KErrTimedOut );
		TEST( ack_3->iStatus == KErrTimedOut );
		}
	
	CleanupStack::PopAndDestroy( 11, startupProperties_0 );

	TEST( 0 == FindAndKill(KTestAppNoRv) );
	}


/**
  This test starts three instances of ssmtestappgood.exe and one of ssmtestapprvafterretry.exe.The latter 
  fails to rv 1 + the number of retries specified in the dat file (written here and deleted by the app)
  Rv is eventually successful
 */
void CSsTestStepAppStartMultiple::DoTestMultiAppIncludingRetryAsyncL()
	{	
	INFO_PRINTF1( _L("\n\n Performing Multiple App-Start Async test---Inc one Retry") );

	//
	// Write the retry number to a file to be read, updated 
	// and eventually deleted by the test-app.
	RFs fs;	
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	CleanupClosePushL( writeStream );
	
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(writeStream.Replace(fs, KDontRvCountFile, EFileWrite));
					
	writeStream.WriteInt8L( KDontRvCount ); // KDontRvCount is declared in ssmtestapprvafterretry.h
	writeStream.CommitL();		
	
	CleanupStack::PopAndDestroy( 2, &fs );
	//
	
	
		
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	// Timeout is specified, therefore there are instances of CTimeoutWaiter constructed.
	startupProperties_0->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_0->SetRetries( KTestMultiRetries );
	startupProperties_0->SetTimeout( KSsTestAppStartTimeout );
	
	CSsmStartupProperties* startupProperties_1 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_1 );
	// The app that rv's after some retries (specified in c:\dontrvcount.dat
	startupProperties_1->SetFileParamsL( KTestAppRvAfterRetry, KNullDesC );
	startupProperties_1->SetCommandTypeL( ESsmCmdStartApp );
	startupProperties_1->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_1->SetRetries( KTestMultiRetries );
	// Each retry in the test app takes 2ms, so delay for 5 retries is given as 18ms.
	startupProperties_1->SetTimeout( KSsTestAppStartTimeout * 6);	//18000ms because KTestAppRvAfterRetry takes 2ms delay for each retry
		
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
	// Use this timeout AO. Therefore not making assumptions about the order of completions.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 10 );
	CleanupStack::PushL( pauseStopper );
	
	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_0, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_1, process_3, ack_3->iStatus, id_3); // The problematic app. Starts after the retries-count file is depleted
	
	INFO_PRINTF5( _L("id_0 to id_3  have the values  %d %d %d %d "), id_0, id_1, id_2, id_3 );
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{
		CActiveScheduler::Start();
		
		TBuf<KMaxFileName> buf( process_0.FileName() );
		INFO_PRINTF2( _L("process_0 filename %S"), &buf );
		
		buf.Copy( process_1.FileName() );
		INFO_PRINTF2( _L("process_1 filename %S"), &buf );
		
		buf.Copy( process_2.FileName() );	
		INFO_PRINTF2( _L("process_2 filename %S"), &buf );
		
		buf.Copy( process_3.FileName() );
		INFO_PRINTF2( _L("process_3 filename %S"), &buf );
		
	
		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Anticipated values were         %d %d %d %d "), KErrNone, KErrNone, KErrNone, KErrNone );
		
		TEST( ack_0->iStatus == KErrNone );
		TEST( ack_1->iStatus == KErrNone );
		TEST( ack_2->iStatus == KErrNone );
		TEST( ack_3->iStatus == KErrNone );
		}
	
	CleanupStack::PopAndDestroy( 12, startupProperties_0 );

	TEST( 1 == FindAndKill(KTestAppRvAfterRetry) );
	TEST( 3 == FindAndKill(KTestAppGood) );
	}




//
// START AS PROCESSES
//

/**
  Concurrently starts four instances of ssmtestappgood.exe as processes
 */
void CSsTestStepAppStartMultiple::DoTestMultiProcAsyncL()
	{	

	INFO_PRINTF1( _L("\n\n Performing Multiple Process-Start Async test---no timeout") );
	
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	// Slightly cheeky. Use the same startup properties for each call
	// No timeout specified, therefore there is no instance of CTimeoutWaiter constructed.
	startupProperties_0->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	
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
	// Use this timeout AO. Therefore not making assumptions about the order of completions.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 2 );
	CleanupStack::PushL( pauseStopper );

	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_0, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_0, process_3, ack_3->iStatus, id_3);
	
	INFO_PRINTF5( _L("id_0 to id_3  have the values  %d %d %d %d "), id_0, id_1, id_2, id_3 );	
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{	
		CActiveScheduler::Start();
		
		TBuf<KMaxFileName> buf( process_0.FileName() );
		INFO_PRINTF2( _L("process_0 filename %S"), &buf );
		
		buf.Copy( process_1.FileName() );
		INFO_PRINTF2( _L("process_1 filename %S"), &buf );
		
		buf.Copy( process_2.FileName() );	
		INFO_PRINTF2( _L("process_2 filename %S"), &buf );
		
		buf.Copy( process_3.FileName() );
		INFO_PRINTF2( _L("process_3 filename %S"), &buf );
		
	
		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Anticipated values were         %d %d %d %d "), KErrNone, KErrNone, KErrNone, KErrNone );
		
		TEST( ack_0->iStatus == KErrNone );
		TEST( ack_1->iStatus == KErrNone );
		TEST( ack_2->iStatus == KErrNone );
		TEST( ack_3->iStatus == KErrNone );
		}
	
	CleanupStack::PopAndDestroy( 11, startupProperties_0 );

	TEST( 4 == FindAndKill(KTestAppGood) );
	}



/**
  Concurrently starts four instances of ssmtestappgood.exe as processes
  A timeout value is specified which ensures that intances of CTimeoutWaiter are created
 */
void CSsTestStepAppStartMultiple::DoTestMultiProcWithTimeoutAsyncL()
	{	

	INFO_PRINTF1( _L("\n\n Performing Multiple Process-Start Async test---*with* timeout") );
	
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	// Slightly cheeky. Use the same startup properties for each call
	// Timeout is specified, therefore there are instances of CTimeoutWaiter constructed.
	startupProperties_0->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_0->SetTimeout( KSsTestAppStartTimeout );
	
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
	// Use this timeout AO. Therefore not making assumptions about the order of completions.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 5 );
	CleanupStack::PushL( pauseStopper );

	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_0, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_0, process_3, ack_3->iStatus, id_3);
	
	INFO_PRINTF5( _L("id_0 to id_3  have the values  %d %d %d %d "), id_0, id_1, id_2, id_3 );	
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{		
		CActiveScheduler::Start();
		
		TBuf<KMaxFileName> buf( process_0.FileName() );
		INFO_PRINTF2( _L("process_0 filename %S"), &buf );
		
		buf.Copy( process_1.FileName() );
		INFO_PRINTF2( _L("process_1 filename %S"), &buf );
		
		buf.Copy( process_2.FileName() );	
		INFO_PRINTF2( _L("process_2 filename %S"), &buf );
		
		buf.Copy( process_3.FileName() );
		INFO_PRINTF2( _L("process_3 filename %S"), &buf );
		
	
		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Anticipated values were         %d %d %d %d "), KErrNone, KErrNone, KErrNone, KErrNone );
		
		TEST( ack_0->iStatus == KErrNone );
		TEST( ack_1->iStatus == KErrNone );
		TEST( ack_2->iStatus == KErrNone );
		TEST( ack_3->iStatus == KErrNone );
		}
	
	CleanupStack::PopAndDestroy( 11, startupProperties_0 );

	TEST( 4 == FindAndKill(KTestAppGood) );
	}



/**
  Concurrently starts four instances of ssmtestappnorv.exe as processes
  All of these timeout (naturalment)
 */
void CSsTestStepAppStartMultiple::DoTestMultiProcAllTimeoutAsyncL()
	{	

	INFO_PRINTF1( _L("\n\n Performing Multiple App-as-Process-Start Async test---ALL timeout") );
	
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	// Slightly cheeky. Use the same startup properties for each call
	startupProperties_0->SetFileParamsL( KTestAppNoRv, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_0->SetTimeout( KInduceTimeoutFailWaitTime );

	
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
	// Use this timeout AO. Therefore not making assumptions about the order of completions.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 5 );
	CleanupStack::PushL( pauseStopper );

	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_0, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_0, process_3, ack_3->iStatus, id_3);
	
	INFO_PRINTF5( _L("id_0 to id_3  have the values  %d %d %d %d "), id_0, id_1, id_2, id_3 );	
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{		
		CActiveScheduler::Start();
		
		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Anticipated values were         %d %d %d %d "), KErrTimedOut, KErrTimedOut, KErrTimedOut, KErrTimedOut );
		
		TEST( ack_0->iStatus == KErrTimedOut );
		TEST( ack_1->iStatus == KErrTimedOut );
		TEST( ack_2->iStatus == KErrTimedOut );
		TEST( ack_3->iStatus == KErrTimedOut );
		}
	
	CleanupStack::PopAndDestroy( 11, startupProperties_0 );

	TEST( 0 == FindAndKill(KTestAppNoRv) );
	}



/**
  This test starts three instances of ssmtestappgood.exe and one of ssmtestapprvafterretry.exe as processes.The latter 
  fails to rv 1 + the number of retries specified in the dat file (written here and deleted by the app)
  Rv is eventually successful
 */
void CSsTestStepAppStartMultiple::DoTestMultiProcIncludingRetryAsyncL()
	{	
	INFO_PRINTF1( _L("\n\n Performing Multiple Slow App-As-Process-Start Async test---ALL Retry") );

	//
	// Write the retry number to a file to be read, updated 
	// and eventually deleted by the test-app.
	RFs fs;	
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	CleanupClosePushL( writeStream );
	
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(writeStream.Replace(fs, KDontRvCountFile, EFileWrite));
					
	writeStream.WriteInt8L( KDontRvCount ); // KDontRvCount is declared in ssmtestapprvafterretry.h
	writeStream.CommitL();		
	
	CleanupStack::PopAndDestroy( 2, &fs );
	//
	
	
		
	CSsmStartupProperties* startupProperties_0 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_0 );
	
	// Timeout is specified, therefore there are instances of CTimeoutWaiter constructed.
	startupProperties_0->SetFileParamsL( KTestAppGood, KNullDesC );
	startupProperties_0->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties_0->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_0->SetRetries( KTestMultiRetries );
	startupProperties_0->SetTimeout( KSsTestAppStartTimeout );
	
	CSsmStartupProperties* startupProperties_1 = CSsmStartupProperties::NewL();
	CleanupStack::PushL( startupProperties_1 );
	// The app that rv's after some retries (specified in c:\dontrvcount.dat
	startupProperties_1->SetFileParamsL( KTestAppRvAfterRetry, KNullDesC );
	startupProperties_1->SetCommandTypeL( ESsmCmdStartProcess );
	startupProperties_1->SetExecutionBehaviour( ESsmWaitForSignal );
	startupProperties_1->SetRetries( KTestMultiRetries );
	startupProperties_1->SetTimeout( KSsTestAppStartTimeout * 6);	//18000ms because KTestAppRvAfterRetry takes 2ms delay for each retry
		
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
	// Use this timeout AO. Therefore not making assumptions about the order of completions.
	CAsPauseStopper* pauseStopper = new(ELeave) CAsPauseStopper( KSsAsStopperNominalTimeout * 10 );
	CleanupStack::PushL( pauseStopper );

	ss->Start(*startupProperties_0, process_0, ack_0->iStatus, id_0);
	ss->Start(*startupProperties_0, process_1, ack_1->iStatus, id_1);
	ss->Start(*startupProperties_0, process_2, ack_2->iStatus, id_2);
	ss->Start(*startupProperties_1, process_3, ack_3->iStatus, id_3); // The problematic app. Starts after the retries-count file is depleted
	
	INFO_PRINTF5( _L("id_0 to id_3  have the values  %d %d %d %d "), id_0, id_1, id_2, id_3 );	
	//Process are started using Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.	
	if( (KRequestPending == ack_0->iStatus.Int()) && (KRequestPending == ack_1->iStatus.Int()) && (KRequestPending == ack_2->iStatus.Int()) && (KRequestPending == ack_3->iStatus.Int()) )
		{		
		CActiveScheduler::Start();
	
		TBuf<KMaxFileName> buf( process_0.FileName() );
		INFO_PRINTF2( _L("process_0 filename %S"), &buf );
		
		buf.Copy( process_1.FileName() );
		INFO_PRINTF2( _L("process_1 filename %S"), &buf );
		
		buf.Copy( process_2.FileName() );	
		INFO_PRINTF2( _L("process_2 filename %S"), &buf );
		
		buf.Copy( process_3.FileName() );
		INFO_PRINTF2( _L("process_3 filename %S"), &buf );
		
	
		INFO_PRINTF5( _L("Test apps rv'd with the values  %d %d %d %d "), ack_0->iStatus.Int(), ack_1->iStatus.Int(), ack_2->iStatus.Int(), ack_3->iStatus.Int() );
		INFO_PRINTF5( _L("Anticipated values were         %d %d %d %d "), KErrNone, KErrNone, KErrNone, KErrNone );
		
		TEST( ack_0->iStatus == KErrNone );
		TEST( ack_1->iStatus == KErrNone );
		TEST( ack_2->iStatus == KErrNone );
		TEST( ack_3->iStatus == KErrNone );
		}
	
	CleanupStack::PopAndDestroy( 12, startupProperties_0 );

	TEST( 1 == FindAndKill(KTestAppRvAfterRetry) );
	TEST( 3 == FindAndKill(KTestAppGood) );
	}



TVerdict CSsTestStepAppStartMultiple::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}



/** */
TVerdict CSsTestStepAppStartMultiple::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


/**
 From MSsTestAsyncNotifier
*/
void CSsTestStepAppStartMultiple::SsTestNotify( TInt aNotify )
	{
	// This method is called if CSsmStartSafe's start method leaves in void CTestAndStopper::DoStartsL().
	INFO_PRINTF2( _L("*** App rv'd with %d"), aNotify );
	}



CSsTestStepAppStartMultiple::CSsTestStepAppStartMultiple()
	{
	SetTestStepName( KCTestCaseAppStartMultiple );
	}



CSsTestStepAppStartMultiple::~CSsTestStepAppStartMultiple()
	{
	delete iActiveScheduler;
	}

