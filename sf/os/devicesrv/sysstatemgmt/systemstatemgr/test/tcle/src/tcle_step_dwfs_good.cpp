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


#include "tcle_step_dwfs_good.h"
#include "ssmtestapps.h"

//
//


CCleDwfsGoodStep::~CCleDwfsGoodStep()
	{
	}

CCleDwfsGoodStep::CCleDwfsGoodStep()
	{
	SetTestStepName(KTCleDwfsGoodStep);
	}
/**
Old Test CaseID 		APPFWK-CLE-0005
New Test CaseID 		DEVSRVS-SSMA-CLE-0005
 */
TVerdict CCleDwfsGoodStep::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleDwfsGoodStep started....."));
	
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
	CSsmCommandList* cmdList = CreateGoodCommandListL(EProcess, ESsmDeferredWaitForSignal, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	//giving a small delay for command exceution to finish
	User::After(0.2*1000*1000);
	//Find any instances of running process started by this test and kill them
	TInt instances = FindAndKill(KSsmCleTestProc);
	INFO_PRINTF2(_L("Instances of KSsmCleTestProc found was '%d'."), instances);
	TEST(instances==4);
	
	//Clean up
	DeleteLogFiles(fs, numberOfCommands);
	CleanupStack::PopAndDestroy(cmdList);
	
	INFO_PRINTF1(_L("Running test with command list containing applications"));
	//Create Proc command list
	cmdList = CreateGoodCommandListL(EApp, ESsmDeferredWaitForSignal, numberOfCommands);
	CleanupStack::PushL(cmdList);
	
	//Execute command list
	TRAP(err, ExecuteCommandListL(cmdList));
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);
	
	// Clean-up & finish
	DeleteLogFiles(fs, numberOfCommands);
	CleanupStack::PopAndDestroy(cmdList);
	
	instances = FindAndKill(KSsmCleTestProc);
	INFO_PRINTF2(_L("Instances of KSsmCleTestProc found was '%d'."), instances);
	TEST(instances==1);
	
	//Find any instances of running application started by this test and kill them
	instances = FindAndKill(KSsmCleTestApplication);
	INFO_PRINTF2(_L("Instances of KSsmCleTestApplication found was '%d'."), instances);
	TEST(instances==3);
		
	TEST(iCleCli.HeapMarkEnd() == KErrNone);
	iCleCli.Close();
	CleanupStack::PopAndDestroy(2, sched); //fs, sched

	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("....CCleDwfsGoodStep finished!!!"));
	
	return TestStepResult();
	}
