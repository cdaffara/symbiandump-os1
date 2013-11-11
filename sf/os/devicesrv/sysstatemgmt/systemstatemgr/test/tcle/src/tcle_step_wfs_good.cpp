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

#include "tcle_step_wfs_good.h"
#include "ssmtestapps.h"

//
//


CCleWfsGoodStep::~CCleWfsGoodStep()
	{
	}

CCleWfsGoodStep::CCleWfsGoodStep()
	{
	SetTestStepName(KTCleWfsGoodStep);
	}

/**
Old Test CaseID 		APPFWK-CLE-0003
New Test CaseID 		DEVSRVS-SSMA-CLE-0003
 */

TVerdict CCleWfsGoodStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleWfsGoodStep started....."));
	
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

	TInt numberOfCommands = 3;

	INFO_PRINTF1(_L("Running test with command list containing processes"));
	//Create Proc command list
	CSsmCommandList* cmdList = CreateGoodCommandListL(EProcess, ESsmWaitForSignal, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands);
	CheckResultsL(numberOfCommands);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	INFO_PRINTF1(_L("Running test with command list containing applications"));
	//Create App command list
	cmdList = CreateGoodCommandListL(EApp, ESsmWaitForSignal, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
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
	TEST(instances==3);
	
	//Find any instances of running application started by this test and kill them
	instances = FindAndKill(KSsmCleTestApplication);
	INFO_PRINTF2(_L("Instances of KSsmCleTestApplication found was '%d'."), instances);
	TEST(instances==3);
	
	// Clean-up & finish
	TEST(iCleCli.HeapMarkEnd() == KErrNone);
	iCleCli.Close();
	CleanupStack::PopAndDestroy(2, sched); //fs, sched

	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CCleWfsGoodStep finished!!!"));
	
	return TestStepResult();
	}
	
void CCleWfsGoodStep::CheckResultsL(TInt aCount)
	{
	// Note: To see the expected behavior refer to CCleTestStepBase::CreateGoodCommandListL()
	//			In this particular configuration of 3 cmds in the cmd list, we have:
	//			** 0 executed with no error
	//			** 1 executed with no error
	//			** 2 executed, fails first time and then runs successfully
	for(TInt i = 0; i < aCount; ++i)
		{
		INFO_PRINTF2(_L("Check to see if item %d started successfully"), i);
		switch(i%3)
			{
			case 0:
				{
				INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (actual: '%d')"), i, 1, iTestResults[i].iStartCount);
				TEST(iTestResults[i].iStartCount == 1);
				if (i > 0)
					{
					// We can't compare first cmd (cmd0) with the previous one, so skip it here
					INFO_PRINTF4(_L("Check to see if item %d waited until the previous item started (time between expected greater than: '%d' actual: '%Ld')"), i, 100000, (iTestResults[i].iStartTime.Int64() - iTestResults[i-1].iStartTime.Int64()));
					TEST((iTestResults[i].iStartTime.Int64() - iTestResults[i-1].iStartTime.Int64()) > 100000); // 100ms is delay of previous item (set in CCleTestStepBase::CreateGoodCommandListL)
					}
				break;
				}
			case 1:
				{
				INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (actual: '%d')"), i, 1, iTestResults[i].iStartCount);
				TEST(iTestResults[i].iStartCount == 1);
				INFO_PRINTF4(_L("Check to see if item %d waited until the previous item started (time between expected greater than: '%d' actual: '%Ld')"), i, 100000, (iTestResults[i].iStartTime.Int64() - iTestResults[i-1].iStartTime.Int64()));
				TEST((iTestResults[i].iStartTime.Int64() - iTestResults[i-1].iStartTime.Int64()) > 100000); // 100ms is delay of previous item (set in CCleTestStepBase::CreateGoodCommandListL)
				break;
				}
			case 2:
				{
				INFO_PRINTF4(_L("Check to see if item %d ran '%d' times (actual: '%d')"), i, 2, iTestResults[i].iStartCount);
				TEST(iTestResults[i].iStartCount == 2);
				INFO_PRINTF4(_L("Check to see if item %d waited until the previous item started (time between expected greater than: '%d' actual: '%Ld')"), i, 100000, (iTestResults[i].iStartTime.Int64() - iTestResults[i-1].iStartTime.Int64()));
				TEST((iTestResults[i].iStartTime.Int64() - iTestResults[i-1].iStartTime.Int64()) > 100000); // 100ms is delay of previous item (set in CCleTestStepBase::CreateGoodCommandListL)
				break;
				}
			}
		}
	}
