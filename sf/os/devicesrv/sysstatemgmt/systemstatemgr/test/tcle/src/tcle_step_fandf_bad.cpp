
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

#include "tcle_step_fandf_bad.h"
#include "ssmatest_utils.h"
#include "ssmtestapps.h"


//
//


CCleFandFBadStep::~CCleFandFBadStep()
	{
	}

CCleFandFBadStep::CCleFandFBadStep()
	{
	SetTestStepName(KTCleFandFBadStep);
	}

/**
Old Test CaseID 		APPFWK-CLE-0007
New Test CaseID 		DEVSRVS-SSMA-CLE-0007
 */
TVerdict CCleFandFBadStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleFandFBadStep started....."));
	
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

	TInt numberOfCommands = 4;

	INFO_PRINTF1(_L("Running test with command list containing processes"));
	//Create Proc command list
	CSsmCommandList* cmdList = CreateBadCommandListL(EProcess, ESsmFireAndForget, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//We need to wait a short while for the processes launched to finish.
	//ExecuteCommandListL() didn't wait because they were FireAndForget.
	User::After(2000000); //2 Seconds
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands);
	CheckResultsL(numberOfCommands);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	INFO_PRINTF1(_L("Running test with command list containing applications"));
	//Create App command list
	cmdList = CreateBadCommandListL(EApp, ESsmFireAndForget, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//We need to wait a short while for the apps launched to finish.
	//ExecuteCommandListL() didn't wait because they were FireAndForget.
	User::After(2000000); //2 Seconds
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands);
	CheckResultsL(numberOfCommands);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	//Find any instances of running process started by this test and kill them
	TInt instances = FindAndKill(KSsmCleTestProc);
	INFO_PRINTF2(_L("Instances of KSsmCleTestProc found was '%d'."), instances);
	TEST(instances==1);
	
	//Find any instances of running application started by this test and kill them
	instances = FindAndKill(KSsmCleTestApplication);
	INFO_PRINTF2(_L("Instances of KSsmCleTestApplication found was '%d'."), instances);
	TEST(instances==1);
	
	// Clean-up & finish	
	TEST(iCleCli.HeapMarkEnd() == KErrNone);
	iCleCli.Close();
	CleanupStack::PopAndDestroy(2, sched); //fs, sched
	
	__UHEAP_MARKEND;
	CloseAllPanicWindowsL();
	INFO_PRINTF1(_L("....CCleFandFBadStep finished!!!"));
	
	return TestStepResult();
	}
	
void CCleFandFBadStep::CheckResultsL(TInt aCount)
	{
	for(TInt i = 0; i < aCount; ++i)
		{
		switch (i%4)
			{
			case 0:
				{
				INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (actual: '%d')"), i, 1, iTestResults[i].iStartCount);
				TEST(iTestResults[i].iStartCount == 1);
				INFO_PRINTF4(_L("The time at which it ran was %d:%d:%d"),iTestResults[i].iStartHour,iTestResults[i].iStartMinute,iTestResults[i].iStartSecond);
				INFO_PRINTF2(_L("The result of execution was '%d' : (expected : '1' - EPanic)"),iTestResults[i].iResult);
				TEST(iTestResults[i].iResult == 1);
				break;
				}
			case 1:
				{
				INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (actual: '%d')"), i, 1, iTestResults[i].iStartCount);
				TEST(iTestResults[i].iStartCount == 1);
				INFO_PRINTF4(_L("The time at which it ran was %d:%d:%d"),iTestResults[i].iStartHour,iTestResults[i].iStartMinute,iTestResults[i].iStartSecond);
				INFO_PRINTF2(_L("The result of execution was '%d' : (expected : '3' - EBadRendezvous)"),iTestResults[i].iResult);
				TEST(iTestResults[i].iResult == 3);
				break;
				}
			case 2:
				{
				INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (Doesn't exist) (actual: '%d')"), i, 0, iTestResults[i].iStartCount);
				TEST(iTestResults[i].iStartCount == 0);
				INFO_PRINTF4(_L("The time at which it ran was %d:%d:%d"),iTestResults[i].iStartHour,iTestResults[i].iStartMinute,iTestResults[i].iStartSecond);
				break;
				}
			case 3:
				{
				INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (actual: '%d')"), i, 1, iTestResults[i].iStartCount);
				TEST(iTestResults[i].iStartCount == 1);
				INFO_PRINTF2(_L("The result of execution was '%d' : (expected : '0' - EDontFail)"),iTestResults[i].iResult);
				TEST(iTestResults[i].iResult == 0);
				INFO_PRINTF2(_L("Item %d ran irrespective of the previous results; Thus Fire and Forget behavior is verfied"),i);
				break;
				}
			}
		}
	}
