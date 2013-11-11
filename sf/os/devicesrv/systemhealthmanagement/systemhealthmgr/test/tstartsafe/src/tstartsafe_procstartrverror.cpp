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
#include "tstartsafe_procstartrverror.h"
#include "testapps.h"
#include "testprocgoodsession.h"
#include "testandstopper.h"

const TInt KStartSafeProcStartRvErrorNominalTimeout = 1200; // milliseconds
const TInt KStartSafeProcStartRvErrorRetries = 3;


/**
New Test Case ID - DEVSRVS-SHMA-STARTSAFE-0017
Old Test Case ID - APPFWK-STARTSAFE-0017
*/
void CAppFwkStartSafeTestStepProcStartRvError::DoTestBadRendezvousL()
	{
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL(startSafe);
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL(prop);
	
	prop->SetFileParamsL(KTestAppRvError, KNullDesC);
	prop->SetStartupType(EStartProcess);
	prop->SetStartMethod(EWaitForStart);
	prop->SetTimeout(KStartSafeProcStartRvErrorNominalTimeout);
	prop->SetNoOfRetries(KStartSafeProcStartRvErrorRetries);
	
	// Number of retries should be reset at the start of every test case 
	iTried = 0;
	
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *startSafe, *prop, iProc, KStartSafeProcStartRvErrorNominalTimeout, iTried, this );
	CleanupStack::PushL( testAndStopper );
			
	CActiveScheduler::Start();

	// 1.Rv error code, 2.checking that process-name of the test-app is not extant etc reported via, and tested in SsTestNotify()

	//Closing the process member variable and cleaning up
	iProc.Close();
	CleanupStack::PopAndDestroy(3, startSafe);
	}


/**
New Test Case ID - DEVSRVS-SHMA-STARTSAFE-0017
Old Test Case ID - APPFWK-STARTSAFE-0018
*/
void CAppFwkStartSafeTestStepProcStartRvError::DoTestProcPanicL(const TDesC& aTestApp, const TDesC& aPanicType, TStartupType aStartType)
	{
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL(startSafe);
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL(prop);
	
	prop->SetFileParamsL(aTestApp, aPanicType);
	prop->SetStartupType(aStartType);
	prop->SetStartMethod(EWaitForStart);

	TInt err = KErrNone;

	if (aStartType == EStartApp)
		{
		// Create an 8-bit variant of aArgs
		RBuf writableArgs;
		writableArgs.CreateL(aPanicType);
		CleanupClosePushL(writableArgs);
		TPtr8 args8 = writableArgs.Collapse();
	
		RFs fs;
		err = fs.Connect();
		TEST(err == KErrNone);
		CleanupClosePushL(fs);
		
		RFile file;
		err = file.Replace(fs, KPanicFile, EFileWrite);
		CleanupClosePushL(file);
		TEST(err == KErrNone);
		
		err = file.Write(args8);
		TEST(err == KErrNone);
		
		CleanupStack::PopAndDestroy(3, &writableArgs);
		}

	// Number of retries should be reset at the start of every test case 
	iTried = 0;
	
	CTestAndStopper* testAndStopper = new(ELeave) CTestAndStopper( *startSafe, *prop, iProc, KStartSafeProcStartRvErrorNominalTimeout, iTried, this );
	CleanupStack::PushL( testAndStopper );
			
	CActiveScheduler::Start();

	// 1.Rv error code, 2.checking that process-name of the test-app is not extant etc reported via, and tested in SsTestNotify()

	//Closing the process member variable and cleaning up
	iProc.Close();
	CleanupStack::PopAndDestroy(3, startSafe);
	}

/**
New Test Case ID - DEVSRVS-SHMA-STARTSAFE-0017
Old Test Case ID - APPFWK-STARTSAFE-0019
*/
void CAppFwkStartSafeTestStepProcStartRvError::DoTestOOML(const TDesC& aTestApp, TStartupType aStartType)
	{
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL(startSafe);
	
	CStartupProperties* prop = CStartupProperties::NewL();
	CleanupStack::PushL(prop);
	
	prop->SetFileParamsL(aTestApp, KNullDesC);
	prop->SetStartupType(aStartType);
	prop->SetStartMethod(EWaitForStart);

	// Number of retries should be reset at the start of every test case 
	iTried = 0;
	INFO_PRINTF1(_L("Start the Test-App process"));

	TInt fail = 0;
	//OOM test for startSafe::StartL() method.
	for (fail = 1;; fail++)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, fail);
		__UHEAP_MARK;
		TRAPD(err, startSafe->StartL(*prop, iProc, iTried));
		
		TEST((err == KErrNone) || (err == KErrNoMemory));
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("Successfully started the process."));
			TEST(err == KErrNone);
			__UHEAP_MARKEND;
			break;
			}
		__UHEAP_MARKEND;
		}
	__UHEAP_RESET;
	
	const TExitType exitType = iProc.ExitType();
	
	// Ensure that the process-name of the test-app is not extant.
	TPtrC procNamePtr = iProc.FileName().Right(aTestApp.Length());
	TInt nameComparison = procNamePtr.Compare(aTestApp);
	
	if((0 == nameComparison) && (EExitPending == exitType))
		{
		INFO_PRINTF2(_L("Application whose process-name is\"%S\" was found"), &procNamePtr);
		// Kill the test application
		iProc.Kill(KErrNone);
		}
	else
		{
		INFO_PRINTF2(_L("%S Process was not found"), &procNamePtr);	
		TEST(EFalse);
		}
	
	//Closing the process member variable and cleaning up
	iProc.Close();
	CleanupStack::PopAndDestroy(2, startSafe);
	}

void CAppFwkStartSafeTestStepProcStartRvError::TestProcPanic()
	{
	//For process
	TRAPD(err, DoTestProcPanicL(KTestProcGood, KPanicCommandLineOption1, EStartProcess));
	TEST(err == KErrNone);
	
	TRAP(err, DoTestProcPanicL(KTestProcGood, KPanicCommandLineOption2, EStartProcess));
	TEST(err == KErrNone);

	//For application
	TRAP(err, DoTestProcPanicL(KTestAppGood, KPanicCommandLineOption1, EStartApp));
	TEST(err == KErrNone);

	TRAP(err, DoTestProcPanicL(KTestAppGood, KPanicCommandLineOption2, EStartApp));
	TEST(err == KErrNone);
	
	// OOM Tests for startprocess and startapplication
	TRAP(err, DoTestOOML(KTestProcGood, EStartProcess));
	TEST(err == KErrNone);
	
	TRAP(err, DoTestOOML(KTestProcGood, EStartApp));
	TEST(err == KErrNone);
	}

TVerdict CAppFwkStartSafeTestStepProcStartRvError::doTestStepL()
	{
	TInt threadHandles_Before;
	TInt processHandles_Before;
	TInt threadHandles_After;
	TInt processHandles_After;
	RThread().HandleCount(processHandles_Before, threadHandles_Before);
		
	TRAPD(err, DoTestBadRendezvousL());
	TEST(err == KErrNone);

	TestProcPanic();
	
	RThread().HandleCount(processHandles_After, threadHandles_After);
	TEST(processHandles_After == processHandles_Before);
	TEST(threadHandles_After == threadHandles_Before);
				
	return TestStepResult();	
	}

TVerdict CAppFwkStartSafeTestStepProcStartRvError::doTestStepPreambleL()
	{
	//Installing the active scheduler for the thread 
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

	return CTestStep::doTestStepPreambleL();
	}


TVerdict CAppFwkStartSafeTestStepProcStartRvError::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

CAppFwkStartSafeTestStepProcStartRvError::CAppFwkStartSafeTestStepProcStartRvError()
	{
	SetTestStepName(KCTestCaseProcStartRvError);
	}

CAppFwkStartSafeTestStepProcStartRvError::~CAppFwkStartSafeTestStepProcStartRvError()
	{
	delete iActiveScheduler;
	}

// from MSsTestAsyncNotifier
void CAppFwkStartSafeTestStepProcStartRvError::SsTestNotify( TInt aNotify )
	{
	const TInt exitType = iProc.ExitType();
		
	// Ensure that the process-name of the test-app is not extant.
	TPtrC procNamePtr = iProc.FileName().Right(KTestAppRvError().Length());
	TInt nameComparison = procNamePtr.Compare(KTestAppRvError());
		

	if((0 == nameComparison) && (EExitPending == exitType))
		{
		ERR_PRINTF2(_L("Application whose process-name is\"%S\" was found, but should not have been"), &procNamePtr);
			
		SetTestStepResult(EFail);
		// Dispose of the erroneously extant test-app.
		iProc.Kill(KErrNone);
			
		ERR_PRINTF1(_L("Test Failed"));
		}
	else if(((KStartSafeProcStartRvErrorRetries +1) != iTried) && (1 != iTried))
		{
		ERR_PRINTF1(_L("Process-name (correctly) not found, but..."));
		ERR_PRINTF3(_L("Total tries = %d  but should have been %d"), iTried, (KStartSafeProcStartRvErrorRetries +1));	
			
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Process was (correctly) not found"));	
		INFO_PRINTF2(_L("Error code %d"), aNotify);
		
		// The 'erroneous' rendezvous code
		TESTE(KErrGeneral, aNotify);
		}

	INFO_PRINTF4( _L("StartL supplied error code %d. anticipated value %d/%d"), aNotify, KErrGeneral, KProcPanic );

	TEST( KProcPanic == aNotify || KErrGeneral == aNotify );

	}

