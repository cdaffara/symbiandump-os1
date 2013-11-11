// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSU_MMF_A2DPBLUETOOTH_Step.cpp
// 
//

// Test system includes
#include "TSU_MMF_A2DPBLUETOOTH_Async.h"
#include "TSU_MMF_A2DPBLUETOOTH_Step.h"

CAsyncTestStepNotifier* CAsyncTestStepNotifier::NewL(CTestStep_MMF_A2DPBLUETOOTH* aTestStep)
	{
	CAsyncTestStepNotifier* self = new(ELeave) CAsyncTestStepNotifier();
	CleanupStack::PushL(self);
	self->ConstructL(aTestStep);
	CleanupStack::Pop();
	return self;
	}
	
CAsyncTestStepNotifier::CAsyncTestStepNotifier() : CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}
	
void CAsyncTestStepNotifier::ConstructL(CTestStep_MMF_A2DPBLUETOOTH* aTestStep)
	{
	iTestStep = aTestStep;
	}


CAsyncTestStepNotifier::~CAsyncTestStepNotifier()
	{
	
	}

/**
 * Method to kick off the active scheduler.  The calling test
 * must have a pending request before calling this.
 * 
 */	
void CAsyncTestStepNotifier::HandleAsyncRequest()
	{
	if (!IsActive())
		{
		SetActive();
		}
	if (!(CActiveScheduler::Current()->StackDepth()))
		{//active scheduler has not been started so start it
		CActiveScheduler::Current()->Start();
		}
	}
	
void CAsyncTestStepNotifier::SetActiveIfNotActive()
	{
	if (!IsActive())
		{
		SetActive();
		}
	}
	
	//CActive
void CAsyncTestStepNotifier::RunL()
	{
	iTestStep->AsyncNotification(iStatus.Int());
	}
	
void CAsyncTestStepNotifier::DoCancel()
	{
	iTestStep->AsyncNotification(KErrCancel);	
	}

