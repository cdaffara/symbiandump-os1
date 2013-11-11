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
//#define SSM_VERBOSE_DEBUG

#include <e32def.h>
#include "tgsa_step_base.h"

//
// Callback to stop the scheduler
//
static TInt StopSchedulerCallBack(TAny* aCGsaTestBase)
	{
	DEBUGPRINT1A(">Stopping scheduler");
	CGsaTestBase* test = reinterpret_cast<CGsaTestBase*>(aCGsaTestBase);
	test->StopScheduler();
	DEBUGPRINT1A("Stopping scheduler>");
	return KErrNone;
	}

CGsaTestBase::CGsaTestBase(const TDesC& aStepName)
	: iActiveScheduler(NULL), iAsyncStopScheduler(NULL), iAsyncCancelCommands(NULL), iActivateCancel(EFalse)
	{
	SetTestStepName(aStepName);	
	}

CGsaTestBase::~CGsaTestBase()
	{
	delete iAsyncCancelCommands;
	delete iAsyncStopScheduler;
	delete iActiveScheduler;
	}

	//from CTestStep
TVerdict CGsaTestBase::doTestStepPreambleL()
	{
	DEBUGPRINT1A(">CGsaTestBase::doTestStepPreambleL");
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);
	
	// create AO to stop the scheduler
	iAsyncStopScheduler = new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stop(StopSchedulerCallBack, this);
	iAsyncStopScheduler->Set(stop);

	// create AO to cancel the commands
	iAsyncCancelCommands = new(ELeave) CAsyncCallBack(CActive::EPriorityStandard + 1);
	
	DEBUGPRINT1A("CGsaTestBase::doTestStepPreambleL>");
	return CTestStep::doTestStepPreambleL();
	}

//TVerdict doTestStepL()
TVerdict CGsaTestBase::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

void CGsaTestBase::StartScheduler()
	{
	DEBUGPRINT1A("Adding stop scheduler callback");
	iAsyncStopScheduler->CallBack();

	if (iActivateCancel)
		{
		DEBUGPRINT1A("Adding cancel command callback");
		iAsyncCancelCommands->CallBack();
		}

	DEBUGPRINT1A("Starting scheduler...");
	iActiveScheduler->Start();
	}

void CGsaTestBase::StopScheduler()
	{
	iActiveScheduler->Halt(KErrNone);
	iActivateCancel = EFalse;
	}

