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


#include "tcle_step_fandf_good.h"
#include "ssmtestapps.h"


//
//


CCleFandFGoodStep::~CCleFandFGoodStep()
	{
	}

CCleFandFGoodStep::CCleFandFGoodStep()
	{
	SetTestStepName(KTCleFandFGoodStep);
	}

/**
Old Test CaseID 		APPFWK-CLE-0004
New Test CaseID 		DEVSRVS-SSMA-CLE-0004
 */

TVerdict CCleFandFGoodStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleFandFGoodStep started....."));
	
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

	//Creating the semaphore
	RSemaphore sem_proc;
	err = sem_proc.CreateGlobal(KStartProcSignalSemaphore,0);
	TEST (KErrNone == err);
	CleanupClosePushL(sem_proc);
		
	TInt numberOfCommands = 3;

	INFO_PRINTF1(_L("Running test with command list containing processes"));
	//Create Proc command list
	CSsmCommandList* cmdList = CreateGoodCommandListL(EProcess, ESsmFireAndForget, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);

	//Waiting using Semaphore
	sem_proc.Wait();
	
	//Find any instances of running processes started by this test and kill them
	INFO_PRINTF1(_L("Find any instances of running processes started by this test and kill them"));
	TInt instances = FindAndKill(KSsmCleTestProc);
	INFO_PRINTF2(_L("Instances of KSsmCleTestProcess found was '%d'."), instances);
	TEST(instances==3);
	
	//Clean up
	DeleteLogFiles(fs, numberOfCommands);
	CleanupStack::PopAndDestroy(2,&sem_proc);
	
	//Creating the semaphore
	RSemaphore sem_app;
	err = sem_app.CreateGlobal(KStartAppSignalSemaphore,0);
	TEST (KErrNone == err);
	CleanupClosePushL(sem_app);
		
	
	INFO_PRINTF1(_L("Running test with command list containing applications"));
	//Create App command list
	cmdList = CreateGoodCommandListL(EApp, ESsmFireAndForget, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//Waiting using Semaphore
	sem_app.Wait();
	
	// Clean-up & finish
	DeleteLogFiles(fs, numberOfCommands);
	CleanupStack::PopAndDestroy(2,&sem_app);
	
	//Find any instances of running application started by this test and kill them
	instances = FindAndKill(KSsmCleTestApplication);
	INFO_PRINTF2(_L("Instances of KSsmCleTestApplication found was '%d'."), instances);
	TEST(instances==3);
	
	TEST(iCleCli.HeapMarkEnd() == KErrNone);
	iCleCli.Close();
	CleanupStack::PopAndDestroy(2, sched); //fs, sched

	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("....CCleFandFGoodStep finished!!!"));
	
	return TestStepResult();
	}
