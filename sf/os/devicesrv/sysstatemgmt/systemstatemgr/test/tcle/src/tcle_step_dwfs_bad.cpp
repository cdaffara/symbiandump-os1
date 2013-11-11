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


#include "tcle_step_dwfs_bad.h"
#include "ssmatest_utils.h"

#include "ssmtestapps.h"

//
//


CCleDwfsBadStep::~CCleDwfsBadStep()
	{
	}

CCleDwfsBadStep::CCleDwfsBadStep()
	{
	SetTestStepName(KTCleDwfsBadStep);
	}
/**
Old Test CaseID 		APPFWK-CLE-0008
New Test CaseID 		DEVSRVS-SSMA-CLE-0008
 */

TVerdict CCleDwfsBadStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleDwfsBadStep started....."));
	
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
	CSsmCommandList* cmdList = CreateBadCommandListL(EProcess, ESsmDeferredWaitForSignal, numberOfCommands, EIgnoreAll);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//giving a small delay for command exceution to finish
	User::After(0.2*1000*1000);
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands, ESsmDeferredWaitForSignal);
	CheckResultsL(numberOfCommands, EIgnoreAll);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	INFO_PRINTF1(_L("Running test with command list containing applications"));
	//Create Proc command list
	cmdList = CreateBadCommandListL(EApp, ESsmDeferredWaitForSignal, numberOfCommands, EIgnoreAll);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//giving a small delay for command exceution to finish
	User::After(0.2*1000*1000);
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands, ESsmDeferredWaitForSignal);
	CheckResultsL(numberOfCommands, EIgnoreAll);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	INFO_PRINTF1(_L("Running test with command list containing processes, one with high severity"));
	//Create Proc command list
	cmdList = CreateBadCommandListL(EProcess, ESsmDeferredWaitForSignal, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList, KErrBadRendezvousTest));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands, ESsmDeferredWaitForSignal);
	CheckResultsL(numberOfCommands, ENormal);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	INFO_PRINTF1(_L("Running test with command list containing applications, one with high severity"));
	//Create App command list
	cmdList = CreateBadCommandListL(EApp, ESsmDeferredWaitForSignal, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList, KErrGeneral));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//Check results and clean up
	GetTestResultsL(fs, numberOfCommands, ESsmDeferredWaitForSignal);
	CheckResultsL(numberOfCommands, ENormal);
	DeleteLogFiles(fs, numberOfCommands);
	iTestResults.Reset();
	CleanupStack::PopAndDestroy(cmdList);
	
	//Find any instances of running process started by this test and kill them
	TInt instances = FindAndKill(KSsmCleTestProc);
	INFO_PRINTF2(_L("Instances of KSsmCleTestProc found was '%d'."), instances);
	TEST(instances==2);
	
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
	INFO_PRINTF1(_L("....CCleDwfsBadStep finished!!!"));
	
	return TestStepResult();
	}
	
void CCleDwfsBadStep::CheckResultsL(TInt aCount, TDwfsListType aListType)
	{
	for(TInt i = 0; i < aCount; ++i)
		{
		//Check exec observers
		switch(aListType)
			{
			case ENormal:
				{
				switch(i%3)
					{
					case 0:
						{
						INFO_PRINTF4(_L("Check to see if Item %d ran '%d' times : (Actual : '%d')"),i,1,iTestResults[i].iStartCount);
						TEST(iTestResults[i].iStartCount == 1);
						INFO_PRINTF4(_L("The time at which it ran was %d:%d:%d"),iTestResults[i].iStartHour,iTestResults[i].iStartMinute,iTestResults[i].iStartSecond);
						INFO_PRINTF3(_L("The result of execution was: '%d' ; Expected: '%d'(EPanic)"),iTestResults[i].iResult,1);
						TEST(iTestResults[i].iResult == 1);
						break;
						}
					case 1:
						{
						INFO_PRINTF4(_L("Check to see if Item %d ran '%d' times : (Actual : '%d')"),i,1,iTestResults[i].iStartCount);
						TEST(iTestResults[i].iStartCount == 1);
						INFO_PRINTF4(_L("The time at which it ran was %d:%d:%d"),iTestResults[i].iStartHour,iTestResults[i].iStartMinute,iTestResults[i].iStartSecond);
						INFO_PRINTF3(_L("The result of execution was: '%d' ; Expected: '%d'(EBadRendezvous)"),iTestResults[i].iResult,3);
						TEST(iTestResults[i].iResult == 3);
						break;
						}
					case 2:
						{
						INFO_PRINTF4(_L("Check to see if Item %d ran '%d' times(Doesn't Exist) : (Actual : '%d')"),i,0,iTestResults[i].iStartCount);
						TEST(iTestResults[i].iStartCount == 0);
						INFO_PRINTF3(_L("The result of execution was: '%d' ; Expected: '%d'(Result not found)"),iTestResults[i].iResult,-1);
						TEST(iTestResults[i].iResult == -1);
						break;
						}
					}
				break;	
				}
			case EIgnoreAll:
				{
				switch(i%3)
					{
					case 0:
						{
						INFO_PRINTF4(_L("Check to see if Item %d ran '%d' times : (Actual : '%d')"),i,1,iTestResults[i].iStartCount);
						TEST(iTestResults[i].iStartCount == 1);
						INFO_PRINTF4(_L("The time at which it ran was %d:%d:%d"),iTestResults[i].iStartHour,iTestResults[i].iStartMinute,iTestResults[i].iStartSecond);
						INFO_PRINTF3(_L("The result of execution was: '%d' ; Expected: '%d'(EPanic)"),iTestResults[i].iResult,1);
						TEST(iTestResults[i].iResult == 1);
						break;
						}
					case 1:
						{
						INFO_PRINTF2(_L("Since the Retries is 1 for this command it should have run %d times"),2);
						INFO_PRINTF4(_L("Check to see if Item %d ran '%d' times : (Actual : '%d')"),i,2,iTestResults[i].iStartCount);
						TEST(iTestResults[i].iStartCount == 2);
						INFO_PRINTF4(_L("The time at which it ran was %d:%d:%d"),iTestResults[i].iStartHour,iTestResults[i].iStartMinute,iTestResults[i].iStartSecond);
						INFO_PRINTF3(_L("The result of execution was: '%d' ; Expected: '%d'(EBadRendezvous)"),iTestResults[i].iResult,3);
						TEST(iTestResults[i].iResult == 3);
						break;
						}
					case 2:
						{
						INFO_PRINTF4(_L("Check to see if Item %d ran '%d' times(Doesn't Exist) : (Actual : '%d')"),i,0,iTestResults[i].iStartCount);
						TEST(iTestResults[i].iStartCount == 0);
						INFO_PRINTF3(_L("The result of execution was: '%d' ; Expected: '%d'(Result not found)"),iTestResults[i].iResult,-1);
						TEST(iTestResults[i].iResult == -1);
						break;
						}
					}
					break;
				}
			}
		}
	//If the command list type is Normal, it will have a commnad with critical severity that fails
	//Then the last "good" Command will not be executed 
	if (ENormal == aListType)
		{
		INFO_PRINTF1(_L("Since the Dwfslist Type is ENormal, commands after multiple wait DO NOT execute till the ones before have finished"));
		INFO_PRINTF3(_L("Check to see if item 'final' ran %d times:(actual: '%d')"),0,iTestResults[aCount].iStartCount);
		TEST(iTestResults[aCount].iStartCount == 0);
		
		}
	//If the list type is IgnoreAll, the 
	else if (EIgnoreAll == aListType)
		{
		INFO_PRINTF1(_L("Since the Dwfslist Type is EIgnoreAll, commands after multiple wait DO execute regardless of previous result"));
		INFO_PRINTF3(_L("Check to see if item 'final' ran %d times:(actual: '%d')"),1,iTestResults[aCount].iStartCount);
		TEST(iTestResults[aCount].iStartCount == 1);
		}
	}
