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

#include <startupproperties.h>

#include "startsafe.h"
#include "tstartsafe_appstartrverror.h"
#include "testapps.h"
#include "testandstopper.h"


const TInt KStartSafeAppStartErrorNominalTimeout = 1200; // milliseconds
const TInt KStartSafeAppStartRvErrorRetries = 3;

/**
Old Test CaseID 		APPFWK-STARTSAFE-0016
New Test CaseID 		DEVSRVS-SHMA-STARTSAFE-0016
 */


TVerdict CAppFwkStartSafeTestStepAppStartRvError::doTestStepL()
	{
	TInt threadHandles_Before;
	TInt processHandles_Before;
	TInt threadHandles_After;
	TInt processHandles_After;
		
	RThread().HandleCount(processHandles_Before, threadHandles_Before);
	
	CStartSafe *startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties *prop = CStartupProperties::NewL();
	CleanupStack::PushL( prop );
	
	prop->SetFileParamsL( KTestAppRvError, KNullDesC );
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EWaitForStart );
	prop->SetNoOfRetries( KStartSafeAppStartRvErrorRetries );
	prop->SetTimeout( KStartSafeAppStartErrorNominalTimeout ); 

	iTried = 0;

	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *startSafe, *prop, iProc, KStartSafeAppStartErrorNominalTimeout, iTried, this );
	CleanupStack::PushL( testAndStopper );
		
	CActiveScheduler::Start();

	// 1.Rv error code, 2.checking that process-name of the test-app is not extant etc reported via, and tested in SsTestNotify()
	
	iProc.Close();
	CleanupStack::PopAndDestroy( 3, startSafe ); 
	
	RThread().HandleCount(processHandles_After, threadHandles_After);
	TEST(processHandles_After == processHandles_Before);
	TEST(threadHandles_After == threadHandles_Before);
	
	return TestStepResult();	
	}

TVerdict CAppFwkStartSafeTestStepAppStartRvError::doTestStepPreambleL()
	{
	//Installing the active scheduler for the thread 
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}


TVerdict CAppFwkStartSafeTestStepAppStartRvError::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


CAppFwkStartSafeTestStepAppStartRvError::CAppFwkStartSafeTestStepAppStartRvError()
	{
	SetTestStepName( KCTestCaseAppStartRvError );
	}


CAppFwkStartSafeTestStepAppStartRvError::~CAppFwkStartSafeTestStepAppStartRvError()
	{
	delete iActiveScheduler;
	}


// from MSsTestAsyncNotifier
void CAppFwkStartSafeTestStepAppStartRvError::SsTestNotify( TInt aNotify )
	{
	const TExitType exitType = iProc.ExitType();

	// Ensure that the process-name of the test-app is not extant.
	TPtrC procNamePtr = iProc.FileName().Right( KTestAppRvError().Length() );
	TInt nameComparison = procNamePtr.Compare( KTestAppRvError() );
			
	if ( (0 == nameComparison) && (EExitPending == exitType) )
		{
		ERR_PRINTF2( _L("Application whose process-name is\"%S\" was found, but should not have been"), &procNamePtr );
		
		// Dispose of the erroneously extant test-app.
		iProc.Kill( KErrNone );
		SetTestStepResult( EFail );
			
		ERR_PRINTF1( _L("Test failed") );
		}
	else if( (KStartSafeAppStartRvErrorRetries +1) != iTried )
		{
		ERR_PRINTF1( _L("Process-name (correctly) not found, but...") );
		ERR_PRINTF3( _L("Total tries = %d  should have been %d"), iTried, (KStartSafeAppStartRvErrorRetries +1) );	
			
		SetTestStepResult( EFail );
		}
	else
		{
		INFO_PRINTF1( _L("Process was (correctly) not found") );
		INFO_PRINTF2( _L("Error code %d"), aNotify );
			
		// The 'erroneous' rendezvous code
		TESTE( KErrGeneral, aNotify );		
		}	

	INFO_PRINTF3( _L("StartL supplied error code %d. anticipated value %d"), aNotify, KErrGeneral );

	TEST( KErrGeneral == aNotify );
	
	}

