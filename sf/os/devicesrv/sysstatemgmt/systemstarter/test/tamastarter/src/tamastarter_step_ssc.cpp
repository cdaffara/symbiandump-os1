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
// NOTE: When testing using sysstart, a test SSC resource file will be required that will contain the necessary AMAs for the test. 
// In addition, the SSC resource fill will contain an entry for an extra application that will create the necessary 
// DSC database store (tamastarter_amaadder.exe).
// The tests will certify that the applications specified in the SSC file via START_AMA_STARTER are launched appropriately.
// As a pre-requisite, tamastarter_amaadder will create these DSC items in the database, so when the START_AMC_STARTER
// structure is found the appropriate action is taken. Please note that this means that tamastarter_amaadder.exe 
// is required to be in the SSC startup resource file itself.
// It will be the SSC file, together with the database created by tamastarter_amaadder that will execute all the actions
// under these test cases, ie, launch applications. These tests cases here will just verify that all applications were
// executed according to the specifications by checking the log files produced.
//

// #ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
// This test will be run twice, 
// a) using the fallback mechanism to boot techview using sysstart. Resource file follows SysStart format.
// b) using new SSM architecture to boot techview. Resource file follows SSM format.		
// #endif
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "appfwk_test_utils.h"
#include "appfwk_test_appui.h"

#include "tamastarter_constants.h"
#include "tamastarter_database_utils.h"
#include "tamastarter_step_ssc.h"



//----------------------


void CAppfwkAmaStarterTestStepSsc::CompareResultsL(const TDesC& aResultFileName, const TDesC& aAmcName, TInt aRetryCntr, TInt aLaunchCntr, TInt aSuccessCntr, TInt aPanicCntr, TBool aMonitored)
	{	
	INFO_PRINTF3(_L("Comparing results for filename '%S', section '%S'"), &aResultFileName, &aAmcName);
	
	TBuf<KMaxFileName>	fileName(KTestAppResultsPath);
	fileName.Append(aResultFileName);
	fileName.Append(KTestLogFileExtension);
	
	CIniData* resultFile = CIniData::NewL(fileName);	
	CleanupStack::PushL(resultFile);
	
	// Get the actual counter values from the result log file
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
Old Test CaseID 		APPFWK-AMASTARTER-0001
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0001
 */

void CAppfwkAmaStarterTestStepSsc::TestCase1L()
	{
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app1"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app2"), 1, 2, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);
	}

/**
Old Test CaseID 		APPFWK-AMASTARTER-0002
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0001
 */

void CAppfwkAmaStarterTestStepSsc::TestCase2L()
	{
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_2_1"), _L("tamastarter_app1"), 1, 1, 0, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_2_2"), _L("tamastarter_app2"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_2_3"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_2_4"), _L("tamastarter_app4"), 0, 1, 1, 0, EFalse);
	}

/**
Old Test CaseID 		APPFWK-AMASTARTER-0003
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0001
 */

void CAppfwkAmaStarterTestStepSsc::TestCase3L()
	{
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_3_1"), _L("tamastarter_app1"), 0, 2, 0, 2, EFalse);
	CompareResultsL(_L("tamastarter_result_3_2"), _L("tamastarter_app2"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_3_3"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_3_4"), _L("tamastarter_app4"), 0, 1, 1, 0, EFalse);
	}

/**
Old Test CaseID 		APPFWK-AMASTARTER-0004
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0001
 */

void CAppfwkAmaStarterTestStepSsc::TestCase4L()
	{
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_4"), _L("tamastarter_app1"), 0, 3, 3, 0, EFalse);
	}
	
/**
Old Test CaseID 		APPFWK-AMASTARTER-0005
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0001
 */

void CAppfwkAmaStarterTestStepSsc::TestCase5L()
	{
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_5_1"), _L("tamastarter_app1"), 0, 1, 1, 0, ETrue);
	CompareResultsL(_L("tamastarter_result_5_2"), _L("tamastarter_app2"), 0, 1, 1, 0, ETrue);
	CompareResultsL(_L("tamastarter_result_5_3"), _L("tamastarter_app3"), 0, 1, 1, 0, ETrue);
	CompareResultsL(_L("tamastarter_result_5_4"), _L("tamastarter_app4"), 0, 1, 1, 0, ETrue);

	CompareDscItemsL(TUid::Uid(0x10000015), KAMCFileNameApp1, _L("-Log tamastarter_result_5_1 -Re 0"), EStartProcess, EWaitForStart, ERetry0, 5000, ETrue, EFalse, EFalse);
	CompareDscItemsL(TUid::Uid(0x10000015), KAMCFileNameApp2, _L("-Log tamastarter_result_5_2 -Re 0"), EStartApp, EWaitForStart, ERetry0, 5000, ETrue, ETrue, ETrue);
	CompareDscItemsL(TUid::Uid(0x10000015), KAMCFileNameApp3, _L("-Log tamastarter_result_5_3"), EStartProcess, EFireAndForget, ERetry0, 5000, ETrue, ETrue, EFalse);
	CompareDscItemsL(TUid::Uid(0x10000015), KAMCFileNameApp4, _L("-Log tamastarter_result_5_4"), EStartApp, EFireAndForget, ERetry0, 5000, ETrue, EFalse, EFalse);
	}

/**
Old Test CaseID 		APPFWK-AMASTARTER-0006
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0001
 */

void CAppfwkAmaStarterTestStepSsc::TestCase6L()
	{
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_6"), _L("tamastarter_app1"), 0, 3, 3, 0, ETrue);
	}



//---------

CAppfwkAmaStarterTestStepSsc::CAppfwkAmaStarterTestStepSsc(const TDesC& aStepName)
	{	
	SetTestStepName(aStepName);
	}


CAppfwkAmaStarterTestStepSsc::~CAppfwkAmaStarterTestStepSsc()
	{	
	}


TVerdict CAppfwkAmaStarterTestStepSsc::doTestStepL()
	{	
	INFO_PRINTF1(_L("AMAStarter SSC tests started...."));

 	__UHEAP_MARK;
 	
	// Run the test cases
	TRAPD(ret, TestCase1L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase1L() finished with code '%d'\n"), ret);

	TRAP(ret, TestCase2L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase2L() finished with code '%d'\n"), ret);

	TRAP(ret, TestCase3L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase3L() finished with code '%d'\n"), ret);

	TRAP(ret, TestCase4L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase4L() finished with code '%d'\n"), ret);

	TRAP(ret, TestCase5L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase5L() finished with code '%d'\n"), ret);

	TRAP(ret, TestCase6L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase6L() finished with code '%d'\n"), ret);

	// Restore DBMS / SQL data store backups available prior to running the test
	TRAP(ret, DscDatabaseRestoreL());
	TESTE(ret == KErrNone, ret);
	
	// Delete log files created to store temporary results
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(fs.DeleteFileUsingWildcardL(KTestLogFileWild));
	CleanupStack::PopAndDestroy(&fs);

	// Close All panic windows that appear from negative testing conditions in this test
	CloseAllPanicWindowsL();

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("...AMAStarter SSC tests finished!!"));

	return TestStepResult();
	}

