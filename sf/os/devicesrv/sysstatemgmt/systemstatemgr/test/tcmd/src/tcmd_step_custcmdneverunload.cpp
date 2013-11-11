// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The test doTestNeverUnloadCustCmdL will run only on debug builds. 
// On release builds, the test does not do anything as the server feature this test requires are not available on release builds.
// If more tests are added into it which are available for both debug/release builds, please remove this comment.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/


#include <ssm/ssmcustomcommandinfo.h>
#include <ssm/ssmcustomcommand.h>

#include "cmdcustomcommand.h"
#include "tcle_cleclient.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "ssmdebug.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
#include "tcmd_step_custcmdneverunload.h"


#include <ssm/ssmcommandlist.h>

_LIT(KDllName, "tcustom_cmd.dll");

/**
 Constructor
*/
CTestCustCmd::CTestCustCmd()
	{
	SetTestStepName(KTCmdTestCustCmd);
	}

/**
 Destructor
*/
CTestCustCmd::~CTestCustCmd()
	{
	}

/**
 Run the tests
*/
TVerdict CTestCustCmd::doTestStepL()
	{

#ifdef _DEBUG
	__UHEAP_MARK;
	TRAPD(err, doTestNeverUnloadCustCmdL());
	TEST(err == KErrNone);
	__UHEAP_MARKEND;
#endif

	return TestStepResult();
	}

//
// Prepare test environment
//
TVerdict CTestCustCmd::doTestStepPreambleL()
	{
	DEBUGVERBOSE1A("CCmdTestBase::doTestStepPreambleL");
	// This active scheduler is never started from this test code.
	// It is provided since the product code has several active objects.
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	return CTestStep::doTestStepPreambleL();
	}


//
// Clean up
//
TVerdict CTestCustCmd::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}


/**
Old Test CaseID 		AFSS-CMDCUS-0016
New Test CaseID 		DEVSRVS-SSMA-CMD-0005
 */

void CTestCustCmd::doTestNeverUnloadCustCmdL()
	{
	TInt expectedResult = KErrNone;
	TCmdErrorSeverity severity = ECmdHighSeverity;
	TPtrC filename = KDllName();
	TInt32 ordinal = 1;
	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = ENeverUnload;
	TInt retries = 5;
	CSsmCustomCommandInfo* info = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(info);
	info->SetL(filename, ordinal, unloading, retries, params);

	INFO_PRINTF1(_L("Test normal execution with ENeverUnload unloading behaviour, executionBehaviour = ESsmFireAndForget"));

	INFO_PRINTF1(_L("Make a backup of the NeverUnload libraries handles' file."));

	_LIT(KUnloadLibHandlesFileName, "C:\\private\\2000d75b\\temp\\unloadlibhandles.bin");
	_LIT(KUnloadLibHandlesBackUpFileName, "C:\\private\\2000D75B\\temp\\unloadlibhandles_backup.bin");

	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);
	fs.Replace(KUnloadLibHandlesFileName, KUnloadLibHandlesBackUpFileName);

	MSsmCommand* cmd = CCmdCustomCommand::NewL(severity, ESsmFireAndForget, *info);
	TInt count = 0;
	TRAPD(err, count = RunTestCleWithNeverUnloadL(cmd));
	TEST(err == expectedResult);
	INFO_PRINTF2(_L("Command execution finished with count : %d, expected count: 1. "), count);
	TEST(count == 1);

	INFO_PRINTF1(_L("Restore the backup of the NeverUnload libraries handles' file to its original file."));
	fs.Replace(KUnloadLibHandlesBackUpFileName, KUnloadLibHandlesFileName);
	CleanupStack::PopAndDestroy(2, info);
	}

//
// Run test using the CleSvr with the ENeverUnload option. It returns the number of libraries unloaded by the server.
//
TInt CTestCustCmd::RunTestCleWithNeverUnloadL(MSsmCommand* aCommand)
	{
	iCleSession.ConnectL();
	CSsmCommandList* cmdList = CSsmCommandList::NewL();
	CleanupStack::PushL(cmdList);
	cmdList->AppendL(aCommand);

	TRequestStatus trs;
	TCmdErrorSeverity severity;
	iCleSession.ExecuteCommandList(*cmdList, trs, severity);
	DEBUGPRINT1A("Waiting for syscle command to finish...");
	User::WaitForRequest(trs);
	User::After(800000);

	CleanupStack::PopAndDestroy(cmdList);
	TInt ret = iCleSession.ReleaseNeverUnloadLibs();
	iCleSession.Close();
	return ret;
	}
