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


#include <amastart.h>

#include "appfwk_test_utils.h"
#include "tamastarter_constants.h"
#include "tamastarter_database_utils.h"
#include "tamastarter_step_startlasync.h"



//----------------------


void CAppfwkAmaStarterTestStepStartLAsync::CompareResultsL(const TDesC& aResultFileName, const TDesC& aAmcName, TInt aRetryCntr, TInt aLaunchCntr, TInt aSuccessCntr, TInt aPanicCntr, TBool aMonitored)
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

/**
Old Test CaseID 		APPFWK-AMASTARTER-0012
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0012
 */

void CAppfwkAmaStarterTestStepStartLAsync::TestCase1L()
	{
	// Launch the appropriate DSC via CAmaStart::Start()
	
	TRequestStatus myRequestStatus;

	CAmaStart* amaStart = CAmaStart::NewL();
	CleanupStack::PushL(amaStart);
	
	amaStart->Start(TUid::Uid(0x10000011), myRequestStatus);
	User::WaitForRequest(myRequestStatus);
	TEST(myRequestStatus == KErrNone);
	INFO_PRINTF3(_L("CAmaStart->Start(TUid::Uid(0x10000011), &myRequestStatus) finished with request status '%d' (expected '%d')"), myRequestStatus.Int(), KErrNone);
	
	//Give procs a chance to close, in case of lots of activity
	User::After(1000000); //1 Second
	
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app1"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app2"), 1, 2, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);

	// clean-up
	CleanupStack::PopAndDestroy(amaStart);
	}
	
/**
Old Test CaseID 		APPFWK-AMASTARTER-0013
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0012
 */

void CAppfwkAmaStarterTestStepStartLAsync::TestCase2L()
	{
	TRequestStatus myRequestStatus;

	CAmaStart* amaStart = CAmaStart::NewL();
	CleanupStack::PushL(amaStart);
	
	amaStart->Start(TUid::Uid(0x100000FF), myRequestStatus);
	User::WaitForRequest(myRequestStatus);
	TEST(myRequestStatus == KErrArgument);
	INFO_PRINTF3(_L("CAmaStart->StartL(TUid::Uid(0x100000FF), &myRequestStatus) finished with request status '%d' (expected '%d')"), myRequestStatus.Int(), KErrArgument);

	// clean-up
	CleanupStack::PopAndDestroy(amaStart);
	}




//---------

CAppfwkAmaStarterTestStepStartLAsync::CAppfwkAmaStarterTestStepStartLAsync(const TDesC& aStepName)
	{	
	SetTestStepName(aStepName);
	}


CAppfwkAmaStarterTestStepStartLAsync::~CAppfwkAmaStarterTestStepStartLAsync()
	{	
	}


TVerdict CAppfwkAmaStarterTestStepStartLAsync::doTestStepL()
	{	
	INFO_PRINTF1(_L("AMAStarter StartL asynchronous tests started...."));

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
	
	TRAP(ret, TestCase2L());
	TESTE(ret == KErrNone, ret);
	INFO_PRINTF2(_L("TestCase2L() finished with code '%d'\n"), ret);

	INFO_PRINTF1(_L("Restore DBMS / SQL data store backups available prior to running the test"));
	TRAP(ret, DscDatabaseRestoreL());
	TESTE(ret == KErrNone, ret);

	INFO_PRINTF1(_L("Delete log files created to store temporary results"));
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(fs.DeleteFileUsingWildcardL(KTestLogFileWild));
	CleanupStack::PopAndDestroy(&fs);

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("...AMAStarter StartL asynchronous tests finished!!"));

	return TestStepResult();
	}

