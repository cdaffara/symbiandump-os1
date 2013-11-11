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
// Old Test CaseID 		APPFWK-SSS-0003
// New Test CaseID 		DEVSRVS-SSMA-STARTSAFE-0003
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "tss_appstartapparcinit.h"



TVerdict CSsTestStepAppStartAppArcInit::doTestStepL( void )
	{

	CSsmStartSafe* ss = CSsmStartSafe::NewL();	
	CleanupStack::PushL( ss );
	
	CSsmStartupProperties* properties = CSsmStartupProperties::NewL();
	CleanupStack::PushL( properties );
	
	RProcess process;
	CleanupClosePushL( process );
	
	properties->SetFileParamsL( KTestAppGood, KNullDesC );
	properties->SetCommandTypeL( ESsmCmdStartApp );
	properties->SetExecutionBehaviour( ESsmDeferredWaitForSignal );
	
	INFO_PRINTF1(_L("Starting the Test-App process asynchronously"));

	TInt index = 0;
	CAsStopper* stopper = new(ELeave) CAsStopper( KSsAsStopperNominalTimeout );
	CleanupStack::PushL( stopper );
	
	TRequestStatus& trs = stopper->Trs();
	
	ss->Start(*properties, process, trs, index);
	
	//Application is started using Deferred Wait for start execution behaviour, 
	//Start method always sets TRequestStatus to KRequestPending.
	if( KRequestPending == trs.Int() )
		{
		CActiveScheduler::Start();
		INFO_PRINTF3( _L("Rendezvous completed with %d. Anticipated value %d"), trs.Int(), KErrNone );
		}
	
	TInt appArcErr = ss->InitAppArcServer(); // try to initiate apparc, to see if that causes any problems

	TEST( KErrNone == appArcErr );
	TEST( 1 == FindAndKill(KTestAppGood) );
	TEST( trs == KErrNone );
		
	CleanupStack::PopAndDestroy( 4, ss );
	return TestStepResult();	
	}



CSsTestStepAppStartAppArcInit::CSsTestStepAppStartAppArcInit()
	{
	SetTestStepName(KCTestCaseAppStartAppArcInit);
	}



CSsTestStepAppStartAppArcInit::~CSsTestStepAppStartAppArcInit()
	{
	delete iActiveScheduler;
	}


TVerdict CSsTestStepAppStartAppArcInit::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();	
	}



TVerdict CSsTestStepAppStartAppArcInit::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}





