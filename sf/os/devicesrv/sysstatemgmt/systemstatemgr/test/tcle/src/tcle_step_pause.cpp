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


#include "tcle_step_pause.h"

#include "../../testapps/inc/ssmtestapps.h"


//
//

CSsmCommandList* CClePauseTest::CreateCommandListL(TInt aCount)
	{
	INFO_PRINTF2(_L("Constructing a test Command list consisting of '%d' StartProcess commands..."), aCount);
	CSsmCommandList* cmdList = CSsmCommandList::NewLC();
	
	for(TInt appNum = 0; appNum < aCount; appNum++)
		{
		TBuf<4> arg;
		arg.Format(_L("%d"), appNum);
		INFO_PRINTF3(_L("Creating start process command with process name: %S, argument string: %S"), &KSsmCleTestProc, &arg);
		CSsmStartupProperties* props = CSsmStartupProperties::NewLC(KSsmCleTestProc, arg);
		MSsmCommand* cmd = SsmCommandFactory::ConstructStartProcessCommandLC(ECmdLowSeverity, props);
		cmdList->AppendL(cmd);
		CleanupStack::Pop(cmd);
		CleanupStack::PopAndDestroy(props);
		}
	CleanupStack::Pop(cmdList);
	return cmdList;
	}



void CClePauseTest::CheckResultsAndCleanupL(RFs aFs, TInt aCount, TInt aPause)
	{
	TInt pauseInMicroseconds = 1000000*aPause;
	/* Allow for the recorded time between executions to be slightly less, 
	 * because the time is recorded just after launch, by the process itself.
	 * The difference allowed is 0.1s */
	TInt minTimeBetweenLaunches = pauseInMicroseconds - 1000000;
	/* Allow for the recorded time between executions to be somewhat more,
	 * because, strictly, the pause is a minimum time between launches - the
	 * specification does not state a maximum. We test for one here, because
	 * there is no reason for it to be hugely longer, so a very long pause
	 * could be indicative of another, wider problem.
	 * We allow the difference to be a whole second longer. */
	TInt maxTimeBetweenLaunches = pauseInMicroseconds + 1000000;
	
	INFO_PRINTF1(_L("Reading start times of apps from files"));
	INFO_PRINTF2(_L("Log file location: %S"), &KTestAppLogFileLoc);
	
	GetTestResultsL(aFs, aCount);

	for (TInt appNum = 1; appNum < aCount; ++appNum) // Start counting from 1, as we compare n and n-1
		{
		INFO_PRINTF3(_L("Testing time between start of apps '%d' and '%d'"), appNum, appNum-1);
		TTimeIntervalMicroSeconds timeBetweenLaunches = iTestResults[appNum].iStartTime.MicroSecondsFrom(iTestResults[appNum-1].iStartTime);
		INFO_PRINTF3(_L("~~~ Expected difference more than '%d', actual '%Ld'"), minTimeBetweenLaunches, timeBetweenLaunches.Int64());
		TEST(timeBetweenLaunches >= minTimeBetweenLaunches);
		INFO_PRINTF3(_L("~~~ Expected difference less than '%d', actual '%Ld'"), maxTimeBetweenLaunches, timeBetweenLaunches.Int64());
		TEST(timeBetweenLaunches < maxTimeBetweenLaunches);
		}
	
	DeleteLogFiles(aFs, aCount);
	iTestResults.Reset();
	}
	
void CClePauseTest::RunDelayTest(RFs aFs, CSsmCommandList* aCmdList, TInt aCount, TInt aPause)
	{
	INFO_PRINTF2(_L("Executing command list with delay of '%d' seconds."), aPause);
		
	//Set pause. If the pause value passed in is -1, leave it alone
	if (aPause != -1)
		{
		aCmdList->SetDelayBetweenCommands(aPause*1000); //Seconds to Milliseconds
		}
	else
		{
		//If we aren't changing the pause, we want to know what it really is
		aPause = aCmdList->DelayBetweenCommands() / 1000; //Milliseconds to Seconds
		}

	//Execute command list
	TRAPD(err, ExecuteCommandListL(aCmdList));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);

	//giving a small delay for command exceution to finish
	User::After(0.2*1000*1000);
	//Check results and clean up
	TRAP(err, CheckResultsAndCleanupL(aFs, aCount, aPause));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("CheckResultsAndCleanupL() completed with code '%d'."), err);
	}


//
//


CClePauseTest::~CClePauseTest()
	{
	}

CClePauseTest::CClePauseTest()
	{
	SetTestStepName(KTClePauseStep);
	}

/**
Old Test CaseID 		APPFWK-CLE-0002
New Test CaseID 		DEVSRVS-SSMA-CLE-0002
 */

TVerdict CClePauseTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CClePauseTest started....."));
	
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

	//Create command list
	CSsmCommandList* cmdList = CreateCommandListL(numberOfCommands);
	
	//Run the test
	RunDelayTest(fs, cmdList, numberOfCommands, -1); // Called with a delay of -1, doesn't change the delay
	RunDelayTest(fs, cmdList, numberOfCommands, 1);
	RunDelayTest(fs, cmdList, numberOfCommands, 5);
	
	//Find any instances of running process started by this test and kill them
	TInt instances = FindAndKill(KSsmCleTestProc);
	INFO_PRINTF2(_L("Instances of KSsmCleTestProc found was '%d'."), instances);
	TEST(instances==9);
	
	// Clean-up & finish	
	TEST(iCleCli.HeapMarkEnd() == KErrNone);
	iCleCli.Close();
	delete cmdList;
	CleanupStack::PopAndDestroy(2, sched);

	__UHEAP_MARKEND;
	
	INFO_PRINTF1(_L("....CClePauseTest finished!!!"));
	
	return TestStepResult();
	}
	
