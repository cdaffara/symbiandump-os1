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

// activate verbose debugging
#define SSM_VERBOSE_DEBUG

#include <e32def.h>
#include "tadaptation_step_base.h"

//
// Callback to stop the scheduler
//
static TInt StopSchedulerCallBack(TAny* aCAdaptationTestBase)
	{
	DEBUGVERBOSE1A(">Stopping scheduler");
	CAdaptationTestBase* test = reinterpret_cast<CAdaptationTestBase*>(aCAdaptationTestBase);
	test->StopScheduler();
	DEBUGVERBOSE1A("Stopping scheduler>");
	return KErrNone;
	}

CAdaptationTestBase::CAdaptationTestBase(const TDesC& aStepName)
	: iActiveScheduler(NULL), iAsyncStopScheduler(NULL), iAsyncCancelCommands(NULL), iActivateCancel(EFalse)
	{
	SetTestStepName(aStepName);	
	}

CAdaptationTestBase::~CAdaptationTestBase()
	{
	delete iAsyncCancelCommands;
	delete iAsyncStopScheduler;
	delete iActiveScheduler;
	}

	//from CTestStep
TVerdict CAdaptationTestBase::doTestStepPreambleL()
	{
	DEBUGVERBOSE1A(">CAdaptationTestBase::doTestStepPreambleL");
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);
	
	// create AO to stop the scheduler
	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopSchedulerCallBack, this);
	iAsyncStopScheduler->Set(stop);

	// create AO to cancel the commands
	iAsyncCancelCommands = new(ELeave) CAsyncCallBack(CActive::EPriorityStandard + 1);
	
	DEBUGVERBOSE1A("CAdaptationTestBase::doTestStepPreambleL>");
	return CTestStep::doTestStepPreambleL();
	}

//TVerdict doTestStepL()
TVerdict CAdaptationTestBase::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

void CAdaptationTestBase::StartScheduler()
	{
	DEBUGVERBOSE1A("Adding stop scheduler callback");
	iAsyncStopScheduler->CallBack();

	if (iActivateCancel)
		{
		DEBUGVERBOSE1A("Adding cancel command callback");
		iAsyncCancelCommands->CallBack();
		}

	DEBUGVERBOSE1A("Starting scheduler...");
	iActiveScheduler->Start();
	}

void CAdaptationTestBase::StopScheduler()
	{
	iActiveScheduler->Halt(KErrNone);
	iActivateCancel = EFalse;
	}

