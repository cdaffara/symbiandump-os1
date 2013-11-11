// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// NOTE: These test cases are very similar to those in the standalone step, but with the difference that instead of launching
// Amastarter.exe, we use AmaStarter DLL, ie, the test now uses CAmaStart::StartL() to start the AMAs.
// Since this is intended as Unit testing for StartL() API only one instance of the test database is checked, unlike 
// standalone tests where different test cases are perfomed.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "amastart.h"

#include "appfwk_test_utils.h"
#include "tamastarter_constants.h"
#include "tamastarter_database_utils.h"
#include "tamastarter_step_startl.h"

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
#include "ssmloadamastart.h"

_LIT( KAmaStartDLL, "loadamastart.dll" );
typedef CSsmLoadAmaStart* (*TFuncCreateL)(void);
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT

//----------------------


void CAppfwkAmaStarterTestStepStart::CompareResultsL(const TDesC& aResultFileName, const TDesC& aAmcName, TInt aRetryCntr, TInt aLaunchCntr, TInt aSuccessCntr, TInt aPanicCntr, TBool aMonitored)
	{	
	INFO_PRINTF3(_L("Comparing results for filename '%S', section '%S'"), &aResultFileName, &aAmcName);
	
	TBuf<KMaxFileName>	fileName(KTestAppResultsPath);
	fileName.Append(aResultFileName);
	fileName.Append(KTestLogFileExtension);
	
	CIniData* resultFile = CIniData::NewL(fileName);	
	CleanupStack::PushL(resultFile);
	
	INFO_PRINTF1(_L("Check the actual counter values from the result log file"));
	TInt actualLaunchCntr = 0;
	TEST(resultFile->FindVar(aAmcName, KLaunchCntr, actualLaunchCntr));
	
	TInt actualSuccessCntr = 0;
	TEST(resultFile->FindVar(aAmcName, KSuccessCntr, actualSuccessCntr));
	
	TInt actualRetryCntr = 0;
	TEST(resultFile->FindVar(aAmcName, KRetryCntr, actualRetryCntr));
	
	TInt actualPanicCntr = 0;
	TEST(resultFile->FindVar(aAmcName, KPanicCntr, actualPanicCntr));
	
	
	// If the application was requested to be monitored by SysMon, the test application might be launched 
	//	more than the expected number of times, as the test application may die and restarted many times 
	//  during the entire test case execution.		
	if (!aMonitored)
		{
		TEST(actualLaunchCntr == aLaunchCntr);
		TEST(actualSuccessCntr == aSuccessCntr);
		TEST(actualRetryCntr == aRetryCntr);
		TEST(actualPanicCntr == aPanicCntr);
		}
	else
		{
		TEST(actualLaunchCntr >= aLaunchCntr);
		TEST(actualSuccessCntr >= aSuccessCntr);
		TEST(actualRetryCntr >= aRetryCntr);
		TEST(actualPanicCntr >= aPanicCntr);
		}

	CleanupStack::PopAndDestroy(resultFile);
	}


//---------

/**
Old Test CaseID 		APPFWK-AMASTARTER-0008
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0008
 */

void CAppfwkAmaStarterTestStepStart::TestCase1L()
	{
	// Launch the appropriate AMAStarter(s) via CAmaStart::StartL()

	CAmaStart* amaStart = CAmaStart::NewL();
	CleanupStack::PushL(amaStart);
	
	TRAPD(ret, amaStart->StartL(TUid::Uid(0x10000011)));
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("CAmaStart->StartL(TUid::Uid(0x10000011)) finished with return code '%d'"), ret);
	
	//One of the three apps in 0x10000011 (KAMCFileNameApp3) is launched using EFireAndForget 
	//and is resuming right now. Because generic\tools\cinidata\src\CIniData.cpp is not 
	//thread-safe we must wait until KAMCFileNameApp3 is finished with the logfile before we can 
	//CompareResultsL. Tests on H4 hardware shows that KAMCFileNameApp3 needs about 400ms to run.
	User::After(600000);
	
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app1"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app2"), 1, 2, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);

	// clean-up
	CleanupStack::PopAndDestroy(amaStart);
	}

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
/**
Old Test CaseID 		APPFWK-AMA-0006
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0008
 */

void CAppfwkAmaStarterTestStepStart::TestCase2L()
	{
	// Launch the appropriate AMAStarter(s) via CAmaStart::StartL()

	RLibrary myAmaStartLib;
	CSsmLoadAmaStart* myAmaStart;
	User::LeaveIfError(myAmaStartLib.Load(KAmaStartDLL));
	CleanupClosePushL(myAmaStartLib);
	TFuncCreateL amaStart = reinterpret_cast<TFuncCreateL>(myAmaStartLib.Lookup(1));
	TInt err = NULL==amaStart?KErrGeneral:KErrNone;
	User::LeaveIfError(err);
	myAmaStart = amaStart();
	CleanupStack::PushL(myAmaStart);
	
	
	TRAPD(ret, myAmaStart->StartL(TUid::Uid(0x10000011)));
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("CLoadAmaStart->StartL(TUid::Uid(0x10000011)) finished with return code '%d'"), ret);
	
	//One of the three apps in 0x10000011 (KAMCFileNameApp3) is launched using EFireAndForget 
	//and is resuming right now. Because generic\tools\cinidata\src\CIniData.cpp is not 
	//thread-safe we must wait until KAMCFileNameApp3 is finished with the logfile before we can 
	//CompareResultsL. Tests on H4 hardware shows that KAMCFileNameApp3 needs about 400ms to run.
	User::After(600000);
	
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app1"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app2"), 1, 2, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);

	// clean-up
	CleanupStack::PopAndDestroy(2); //myAmaStart, myAmaStartLib
	}
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT

/**
Old Test CaseID 		APPFWK-AMASTARTER-0013
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0008
 */


void CAppfwkAmaStarterTestStepStart::TestCase3L()
	{
	// Launch the appropriate AMAStarter(s) via CAmaStart::StartL()
  	TInt threadHandles_BeforeAmaStart;
	TInt processHandles_BeforeAmaStart;
	RThread().HandleCount(processHandles_BeforeAmaStart, threadHandles_BeforeAmaStart);
	INFO_PRINTF2(_L("No. of open handles in process which owns this process %d before calling CAmaStart::StartL"),processHandles_BeforeAmaStart);
	INFO_PRINTF2(_L("No. of open handles in this thread %d before calling CAmaStart::StartL"),threadHandles_BeforeAmaStart);

	CAmaStart* amaStart = CAmaStart::NewL();
	CleanupStack::PushL(amaStart);
	
	TRAPD(ret, amaStart->StartL(TUid::Uid(0x10000018)));
	TEST(ret == KErrNone);
	INFO_PRINTF2(_L("CAmaStart->StartL(TUid::Uid(0x10000018)) finished with return code '%d'"), ret);
	
	//One of the three apps in 0x10000018 (KAMCFileNameApp3) is launched using EFireAndForget 
	//and is resuming right now. Because generic\tools\cinidata\src\CIniData.cpp is not 
	//thread-safe we must wait until KAMCFileNameApp3 is finished with the logfile before we can 
	//CompareResultsL. Tests on H4 hardware shows that KAMCFileNameApp3 needs about 400ms to run.
	User::After(600000);
	
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_7"), _L("tamastarter_app1"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_7"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);

	TInt threadHandles_AfterAmaStart;
	TInt processHandles_AfterAmaStart;
	RThread().HandleCount(processHandles_AfterAmaStart, threadHandles_AfterAmaStart);
	TEST(processHandles_AfterAmaStart == processHandles_BeforeAmaStart);
	INFO_PRINTF2(_L("No. of open handles in process which owns this process %d after calling CAmaStart::StartL"),processHandles_AfterAmaStart);

	TEST(threadHandles_AfterAmaStart > threadHandles_BeforeAmaStart);
	INFO_PRINTF2(_L("No. of open handles in this thread %d after calling CAmaStart::StartL"),threadHandles_AfterAmaStart);

	// clean-up
	CleanupStack::PopAndDestroy(amaStart);

	TInt threadHandles_AfterAmaStartDeleted;
	TInt processHandles_AfterAmaStartDeleted;
	RThread().HandleCount(processHandles_AfterAmaStartDeleted, threadHandles_AfterAmaStartDeleted);

	TEST(processHandles_AfterAmaStartDeleted == processHandles_BeforeAmaStart);
	INFO_PRINTF2(_L("No. of open handles in process which owns this process %d after deleting CAmaStart object"),processHandles_AfterAmaStartDeleted);

	TEST(threadHandles_AfterAmaStartDeleted == threadHandles_BeforeAmaStart);
	INFO_PRINTF2(_L("No. of open handles in this thread %d after deleting CAmaStart object"),threadHandles_AfterAmaStartDeleted);
	}

//---------

CAppfwkAmaStarterTestStepStart::CAppfwkAmaStarterTestStepStart(const TDesC& aStepName)
	{	
	SetTestStepName(aStepName);
	}


CAppfwkAmaStarterTestStepStart::~CAppfwkAmaStarterTestStepStart()
	{	
	}


TVerdict CAppfwkAmaStarterTestStepStart::doTestStepL()
	{	
	INFO_PRINTF1(_L("AMAStarter StartL tests started...."));

 	__UHEAP_MARK;
 	
	// Create DBMS / SQL data store backups and delete original data stores
	TRAPD(ret, DscDatabaseBackupL());
	TESTE(ret == KErrNone, ret);

	INFO_PRINTF1(_L("Create appropriate data store for the tests"));
	TRAP(ret, CreateTestDatabaseL());
	TESTE(ret == KErrNone, ret);

	INFO_PRINTF1(_L("Run the test cases"));
	TRAP(ret, TestCase1L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase1L() finished with code '%d'\n"), ret);
	
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	INFO_PRINTF1(_L("Delete log files created to store temporary results"));
	User::LeaveIfError(fs.DeleteFileUsingWildcardL(KTestLogFileWild));
	
	TRAP(ret, TestCase2L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase2L() finished with code '%d'\n"), ret);
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT

	TRAP(ret, TestCase3L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase3L() finished with code '%d'\n"), ret);

	INFO_PRINTF1(_L("Restore DBMS / SQL data store backups available prior to running the test"));
	TRAP(ret, DscDatabaseRestoreL());
	TESTE(ret == KErrNone, ret);

	INFO_PRINTF1(_L("Delete log files created to store temporary results"));
	User::LeaveIfError(fs.DeleteFileUsingWildcardL(KTestLogFileWild));
	CleanupStack::PopAndDestroy(&fs);

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("...AMAStarter StartL tests finished!!"));

	return TestStepResult();
	}

