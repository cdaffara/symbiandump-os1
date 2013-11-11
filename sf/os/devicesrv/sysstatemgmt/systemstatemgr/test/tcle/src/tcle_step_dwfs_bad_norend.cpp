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


#include "tcle_step_dwfs_bad_norend.h"
#include "ssmtestapps.h"

//
//


CCleDwfsBadNoRendStep::~CCleDwfsBadNoRendStep()
	{
	}

CCleDwfsBadNoRendStep::CCleDwfsBadNoRendStep()
	{
	SetTestStepName(KTCleDwfsBadNoRendStep);
	}

/**
Old Test CaseID 		APPFWK-CLE-0009
New Test CaseID 		DEVSRVS-SSMA-CLE-0009
 */

TVerdict CCleDwfsBadNoRendStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleDwfsBadNoRendStep started....."));
	
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
	CSsmCommandList* cmdList = CreateBadCommandListL(EProcess, ESsmDeferredWaitForSignal, numberOfCommands, EDontRendezvous);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList, KErrTimedOut));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands, ESsmDeferredWaitForSignal);
	CheckResultsL(numberOfCommands);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	INFO_PRINTF1(_L("Running test with command list containing applications"));
	//Create Proc command list
	cmdList = CreateBadCommandListL(EApp, ESsmDeferredWaitForSignal, numberOfCommands, EDontRendezvous);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList, KErrTimedOut));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands, ESsmDeferredWaitForSignal);
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
	INFO_PRINTF1(_L("....CCleDwfsBadNoRendStep finished!!!"));
	
	return TestStepResult();
	}
	
void CCleDwfsBadNoRendStep::CheckResultsL(TInt aCount)
	{
	for(TInt i = 0; i < aCount; ++i)
		{
		//Check exec observers
		INFO_PRINTF4(_L("Check to see if Item %d ran '%d' times : (Actual : '%d')"),i,1,iTestResults[i].iStartCount);
		TEST(iTestResults[i].iStartCount == 1);
		INFO_PRINTF3(_L("The result of execution was: '%d' ; Expected: '%d'(ENoRendezvous)"),iTestResults[i].iResult,2);
		TEST(iTestResults[i].iResult == 2);
		}
	//check to see item 'final' dint run successfully	
	INFO_PRINTF1(_L("Since the Dwfslist Type is EDontRendezvous, commands after multiple wait DO NOT execute till the ones before have finished"));
	INFO_PRINTF3(_L("Check to see if item 'final' ran %d times:(actual: '%d')"),0,iTestResults[aCount].iStartCount);
	TEST(iTestResults[aCount].iStartCount == 0);
	}
