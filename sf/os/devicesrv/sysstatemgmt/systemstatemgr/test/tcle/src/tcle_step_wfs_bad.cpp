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


#include <ssm/ssmcommandlist.h>
#include <s32file.h> 
#include "ssmdebug.h"
#include "ssmpanic.h"
#include <ssm/ssmcommand.h>
#include "ssmcommandbase.h"
#include <ssm/ssmcommandfactory.h>
#include "ssmcommandlistinterface.h"
#include <ssm/ssmstartupproperties.h>
#include "tcle_step_wfs_bad.h"
#include "ssmatest_utils.h"
#include "ssmtestapps.h"
#include "ssmcletestapp.h"

//
//


CCleWfsBadStep::~CCleWfsBadStep()
	{
	}

CCleWfsBadStep::CCleWfsBadStep()
	{
	SetTestStepName(KTCleWfsBadStep);
	}
/**
Old Test CaseID 		APPFWK-CLE-0006
New Test CaseID 		DEVSRVS-SSMA-CLE-0006
 */

TVerdict CCleWfsBadStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleWfsBadStep started....."));
	
	__UHEAP_MARK;
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	TRAPD(err, iCleCli.ConnectL(KCleSrver));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("Connecting client to server completed with code '%d'."), err);
	TEST(iCleCli.HeapMark() == KErrNone);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TInt numberOfCommands = 1;
	INFO_PRINTF1(_L("Test Process exit reason"));
	TBuf<10> arg;
	arg.Format(_L("%d"), numberOfCommands);
	MSsmCommand* command = NULL;
	command = CreateBadCommandLC(EProcess, ESsmWaitForSignal, arg, 0, 500, 0, 0, EBadRendezvous);
	CSsmCommandList* cmdList = CSsmCommandList::NewLC();
	cmdList->AppendL(command);
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList, KErrNone));
	CleanupStack::PopAndDestroy(cmdList);
	CleanupStack::Pop(command);
	
	//Getting the Exit Reason
	TBuf<KTestCommandLineMaxLength> prefix = KTestAppLogFileLoc();
	TBuf<KTestCommandLineMaxLength> logFileName;
	logFileName.Format(_L("%Sreason"), &prefix);
	RFileReadStream fileReadStream;
	err = fileReadStream.Open(fs, logFileName, EFileRead);
	CleanupClosePushL(fileReadStream);
	User::LeaveIfError(err);
	TInt exitReason = fileReadStream.ReadInt32L();
	CleanupStack::PopAndDestroy(); //fileReadStream
	fs.Delete(logFileName);
	
	TEST(exitReason==KErrBadRendezvousTest);	
	DeleteLogFiles(fs, numberOfCommands+1);
	iTestResults.Reset();

	INFO_PRINTF1(_L("Running test with command list containing processes"));
	numberOfCommands = 5;
	//Create Proc command list
	cmdList = CreateBadCommandListL(EProcess, ESsmWaitForSignal, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList, KErrNotFound));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands);
	CheckResultsL(numberOfCommands);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	INFO_PRINTF1(_L("Running test with command list containing applications"));
	//Create Proc command list
	cmdList = CreateBadCommandListL(EApp, ESsmWaitForSignal, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList, KErrNotFound));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands);
	CheckResultsL(numberOfCommands);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	//Find any instances of running process started by this test and kill them
	TInt instances = FindAndKill(KSsmCleTestProc);
	INFO_PRINTF2(_L("Instances of KSsmCleTestProc found was '%d'."), instances);
	TEST(instances==0);
	
	//Find any instances of running application started by this test and kill them
	instances = FindAndKill(KSsmCleTestApplication);
	INFO_PRINTF2(_L("Instances of KSsmCleTestApplication found was '%d'."), instances);
	TEST(instances==0);
	
	// Clean-up & finish	
	TEST(iCleCli.HeapMarkEnd() == KErrNone);
	iCleCli.Close();
	CleanupStack::PopAndDestroy(2, sched); //fs, sched

	__UHEAP_MARKEND;
	CloseAllPanicWindowsL();
	INFO_PRINTF1(_L("....CCleWfsBadStep finished!!!"));
	
	return TestStepResult();
	}
	
void CCleWfsBadStep::CheckResultsL(TInt aCount)
	{
	// Note: To see the expected behavior refer to CCleTestStepBase::CreateBadCommandListL()
	//			In this particular configuration of 5 cmds in the cmd list, we have:
	//			** 0,1,2 executed but failed, and failure is ignored
	//			** 3 executed and failed, failure is returned
	//			** 4 is never executed as 4's error was not ignored.	
	for(TInt i = 0; i < aCount; ++i)
		{
		if (i >= 4)
			{
			INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (Previous not ignored) (actual: '%d')"), i, 0, iTestResults[i].iStartCount);
			TEST(iTestResults[i].iStartCount == 0);
			}
		else
			{
			switch(i%5)
				{
				case 0:
				case 1:
				case 2:
					{
					INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (actual: '%d')"), i, 1, iTestResults[i].iStartCount);
					TEST(iTestResults[i].iStartCount == 1);
					break;
					}
				case 3:
					{
					INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (Doesn't exist) (actual: '%d')"), i, 0, iTestResults[i].iStartCount);
					TEST(iTestResults[i].iStartCount == 0);
					break;
					}
				}
			}
		}
	}
