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
// NOTE:	The DSCs created for SSC test step will be used to launch the AMCs when AMAStarter is used standalone. 
// This should lead to same results when used within SSC using SysStart. 
// All the test conditions presented with SSC will lead to a DSC creation, which will be used to launch AMCs 
// within it, using AMAStarter EXE as standalone wherever possible.
// NOTE: In order to run these tests properly, we need to set the SID of the server to the correct one KSysStartSID, so we
// are able to launch AmaStarter.exe.
// To achieve this, the SETCAP test tool is used in teh script to update tamastarter_server with the following command:
// RUN_PROGRAM -1 setcap tamastarter_server.exe 0003FFFE -SID 10205C44 tamastarter_server_KSysStartSID.exe
// For further explanation, check the script file under ..\scripts\sysstarttest_amastarter_standalone.script
// 
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
#include "tamastarter_step_standalone.h"


#ifdef __WINS__
// We need to create separate EXEs as AmaStarter is EXEXP and more than 1 processes cannot be created for emulator.
// NOTE: The name given here has to match the one given when creating these exexp's with the SETCAP tool
_LIT(KAmaStarterTestDefault,"amastarter_default.exe");
_LIT(KAmaStarterTest1,"amastarter1.exe");
_LIT(KAmaStarterTest2,"amastarter2.exe");
_LIT(KAmaStarterTest3,"amastarter3.exe");
_LIT(KAmaStarterTest4_1,"amastarter4_1.exe");
_LIT(KAmaStarterTest4_2,"amastarter4_2.exe");
_LIT(KAmaStarterTest5,"amastarter5.exe");
_LIT(KAmaStarterTest6_1,"amastarter6_1.exe");
_LIT(KAmaStarterTest6_2,"amastarter6_2.exe");
_LIT(KAmaStarterTest6_3,"amastarter6_3.exe");
#else
// We don't need separate EXEs for AmaStarter in hardware, we can use the same. Also, SETCAP tools is not usable on 
// hardware with amastarter.exe (as the source executable is in ROM, then it must be a RAM executable image, not
// an execute-in-place (XIP) image ==> translates to must be "data=..." not "file=..." in the .iby file!)
_LIT(KAmaStarterTestDefault,"amastarter.exe");
_LIT(KAmaStarterTest1,"amastarter.exe");
_LIT(KAmaStarterTest2,"amastarter.exe");
_LIT(KAmaStarterTest3,"amastarter.exe");
_LIT(KAmaStarterTest4_1,"amastarter.exe");
_LIT(KAmaStarterTest4_2,"amastarter.exe");
_LIT(KAmaStarterTest5,"amastarter.exe");
_LIT(KAmaStarterTest6_1,"amastarter.exe");
_LIT(KAmaStarterTest6_2,"amastarter.exe");
_LIT(KAmaStarterTest6_3,"amastarter.exe");
#endif


//----------------------


void CAppfwkAmaStarterTestStepStandalone::CompareResultsL(const TDesC& aResultFileName, const TDesC& aAmcName, TInt aRetryCntr, TInt aLaunchCntr, TInt aSuccessCntr, TInt aPanicCntr, TBool aMonitored)
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



void CAppfwkAmaStarterTestStepStandalone::LaunchExe(const TDesC& aFilename, const TDesC& aArguments, TInt aExitReason)
	{
	RProcess process;
	TInt ret = process.Create(aFilename, aArguments, EOwnerThread);
	TEST(ret == KErrNone);
	
	TRequestStatus	status;
	process.Logon(status);
	User::SetJustInTime(EFalse);
	process.Resume();
	User::WaitForRequest(status);
	User::SetJustInTime(ETrue);
	TInt exitReason = process.ExitReason();
	process.Close();
	
	// Check the process ended with the expected exit reason
	INFO_PRINTF3(_L("Process finished with ExitReason '%d' (expected was '%d')"), exitReason, aExitReason);
	TEST(exitReason == aExitReason);
	}


//---------


void CAppfwkAmaStarterTestStepStandalone::TestCase1L()
	{
	// Launch the appropriate AMAStarter(s)
	LaunchExe(KAmaStarterTest1, _L("10000011"), KErrNone);
	
	//One of the three apps in 0x10000011 (KAMCFileNameApp3) is launched using EFireAndForget 
	//and is resuming right now. Because generic\tools\cinidata\src\CIniData.cpp is not 
	//thread-safe we must wait until KAMCFileNameApp3 is finished with the logfile before we can 
	//CompareResultsL. Tests on H4 hardware shows that KAMCFileNameApp3 needs about 400ms to run.
	User::After(600000);
		
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app1"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app2"), 1, 2, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_1"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);
	}


void CAppfwkAmaStarterTestStepStandalone::TestCase2L()
	{
	// Launch the appropriate AMAStarter(s)
	LaunchExe(KAmaStarterTest2, _L("10000012"), KErrNone);
	
	//Two of the apps in 0x10000012 are launched using EFireAndForget and are resuming right now. 
	//Because generic\tools\cinidata\src\CIniData.cpp is not thread-safe we must wait until 
	//these apps are finished with the logfile before we can CompareResultsL. 
	//Tests on H4 hardware shows that these testapps need about 400ms each to run until completion.
	User::After(1200000);
		
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_2_1"), _L("tamastarter_app1"), 1, 1, 0, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_2_2"), _L("tamastarter_app2"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_2_3"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_2_4"), _L("tamastarter_app4"), 0, 1, 1, 0, EFalse);
	}


void CAppfwkAmaStarterTestStepStandalone::TestCase3L()
	{
	// Launch the appropriate AMAStarter(s)
	LaunchExe(KAmaStarterTest3, _L("10000013"), KErrNone);
	
	//Two of the 'valid' apps in 0x10000013 are launched using EFireAndForget and are resuming right now. 
	//Because generic\tools\cinidata\src\CIniData.cpp is not thread-safe we must wait until 
	//these apps are finished with the logfile before we can CompareResultsL. 
	//Tests on H4 hardware shows that these testapps need about 400ms each to run until completion.
	User::After(1200000);
	
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_3_1"), _L("tamastarter_app1"), 0, 2, 0, 2, EFalse);
	CompareResultsL(_L("tamastarter_result_3_2"), _L("tamastarter_app2"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_3_3"), _L("tamastarter_app3"), 0, 1, 1, 0, EFalse);
	CompareResultsL(_L("tamastarter_result_3_4"), _L("tamastarter_app4"), 0, 1, 1, 0, EFalse);
	}


void CAppfwkAmaStarterTestStepStandalone::TestCase4L()
	{
	// Launch the appropriate AMAStarter(s)
	LaunchExe(KAmaStarterTest4_1, _L("10000014"), KErrNone);
	LaunchExe(KAmaStarterTest4_2, _L("10000024"), KErrNone);
	LaunchExe(KAmaStarterTestDefault, _L("2000836D"), KErrNone);	// KDefaultSymbianDsc
	
	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_4"), _L("tamastarter_app1"), 0, 3, 3, 0, EFalse);
	}
	

void CAppfwkAmaStarterTestStepStandalone::TestCase5L()
	{
	// Launch the appropriate AMAStarter(s)
	LaunchExe(KAmaStarterTest5, _L("10000015"), KErrNone);

	//Two of the apps in 0x10000015 are launched using EFireAndForget and are resuming right now. 
	//Because generic\tools\cinidata\src\CIniData.cpp is not thread-safe we must wait until 
	//these apps are finished with the logfile before we can CompareResultsL. 
	//Tests on H4 hardware shows that these testapps need about 400ms each to run until completion.
	User::After(1200000);
	
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


void CAppfwkAmaStarterTestStepStandalone::TestCase6L()
	{
	// Launch the appropriate AMAStarter(s)
	LaunchExe(KAmaStarterTest6_1, _L("10000016"), KErrNone);
	LaunchExe(KAmaStarterTest6_2, _L("10000026"), KErrNone);
	LaunchExe(KAmaStarterTest6_3, _L("10000036"), KErrNone);

	//The app in 0x10000036 is launched using EFireAndForget and is resuming right now. 
	//Because generic\tools\cinidata\src\CIniData.cpp is not thread-safe we must wait until 
	//this app is finished with the logfile before we can CompareResultsL. 
	//Tests on H4 hardware shows that these testapps need about 400ms to run.
	User::After(600000);

	// Check and compare results as they have been stored in the result log file
	CompareResultsL(_L("tamastarter_result_6"), _L("tamastarter_app1"), 0, 3, 3, 0, ETrue);
	}

/**
Old Test CaseID 		APPFWK-AMASTARTER-0007
New Test CaseID 		DEVSRVS-SYSSTART-AMASTARTER-0007
 */

	
void CAppfwkAmaStarterTestStepStandalone::RunStandaloneTests()
	{
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
	}

//---------

CAppfwkAmaStarterTestStepStandalone::CAppfwkAmaStarterTestStepStandalone(const TDesC& aStepName)
	{	
	SetTestStepName(aStepName);
	}


CAppfwkAmaStarterTestStepStandalone::~CAppfwkAmaStarterTestStepStandalone()
	{	
	}


TVerdict CAppfwkAmaStarterTestStepStandalone::doTestStepL()
	{	
	INFO_PRINTF1(_L("AMAStarter Standalone tests started...."));

 	__UHEAP_MARK;

#ifdef __WINS__
// Create separate EXEs as AmaStarter is EXEXP and more than 1 processes cannot be created for emulator.
// Capabilities of AmaStarter.exe PowerMgmt(00000004) + ReadDeviceData(00000010) + ProtServ(00000100)
// As SETCAP tool can't be used on hardware for AmaStarter.exe, we don't call the tool in the script, because
// this would mean different scripts for emulator and hardware.
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter_default.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter1.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter2.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter3.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter4_1.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter4_2.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter5.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter6_1.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter6_2.exe"), KErrNone);
	LaunchExe(_L("setcap.exe"), _L("amastarter.exe 00000114 amastarter6_3.exe"), KErrNone);
	INFO_PRINTF1(_L("Creation of separate EXEs for AmaStarter (for emulator) completed.\n"));
#endif

	INFO_PRINTF1(_L("Creating DBMS / SQL data store backups and delete original data stores"));
	TRAPD(ret, DscDatabaseBackupL());
	TESTE(ret == KErrNone, ret);

	INFO_PRINTF1(_L("Creating appropriate data store for the tests"));
	TRAP(ret, CreateTestDatabaseL());
	TESTE(ret == KErrNone, ret);

	// Run the standalone tests cases
	INFO_PRINTF1(_L("Now, run the tests"));
	RunStandaloneTests();
	
	INFO_PRINTF1(_L("Restoring DBMS / SQL data store backups available prior to running the test"));
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

	INFO_PRINTF1(_L("...AMAStarter Standalone tests finished!!"));

	return TestStepResult();
	}

