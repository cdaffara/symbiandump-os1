// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// te_lbsbtgpsconfigstepbase.h
// lbsbtgpsconfig unit test: Base test step
//

#include "te_lbsbtgpsconfigstepbase.h"

const TTimeIntervalMicroSeconds32 timeout = 1; //100000;

CLbsBtGpsConfigStepBase::CLbsBtGpsConfigStepBase()
	: CActive(CActive::EPriorityIdle)
	{
	}


CLbsBtGpsConfigStepBase::~CLbsBtGpsConfigStepBase()
	{
	iKeys.Close();
	iTimer.Close();
	}


void CLbsBtGpsConfigStepBase::SetTestServer(CLbsBtGpsConfigTestServer* aServer)
	{
	iServer = aServer;
	}


TVerdict CLbsBtGpsConfigStepBase::doTestStepPreambleL(void)
	{
	SetTestStepResult(EFail);
	
	INFO_PRINTF2(_L("Test %S starting"), &TestStepName());

	// Install active scheduler if there isn't one already
	if(CActiveScheduler::Current() == NULL)
		{
		CActiveScheduler::Install(iServer->ActiveScheduler());
		}

	CActiveScheduler::Add(this);
	iTimer.CreateLocal();
	
	SetTestStepResult(EPass);
	
	return TestStepResult();
	}


TVerdict CLbsBtGpsConfigStepBase::doTestStepPostambleL(void)
	{
	INFO_PRINTF2(_L("Test %S ended"), &TestStepName());

	return TestStepResult();
	}


TVerdict CLbsBtGpsConfigStepBase::doTestStepL(void)
	{
	iStatus = KRequestPending;
	iTimer.After(iStatus, timeout);
	SetActive();
	
	// Do the thing
	CActiveScheduler::Start();
	
	return TestStepResult();
	}


void CLbsBtGpsConfigStepBase::OnDeviceListUpdate()
	{
	++iUpdatesRecieved;
	INFO_PRINTF2(_L("--- OnDeviceListUpdate(%d) ---"),iUpdatesRecieved);
	}


void CLbsBtGpsConfigStepBase::DoCancel()
	{
	iTimer.Cancel();
	}


void CLbsBtGpsConfigStepBase::RunL()
	{
	++iStage;
	INFO_PRINTF2(_L("Running test stage %d"),iStage);

	if (RunTestStage(iStage))
		{
		if (TestStepResult() != EPass) 
			{
			INFO_PRINTF2(_L("Test stage %d failed, exiting"),iStage);
			CActiveScheduler::Stop();
			}
		else
			{
			INFO_PRINTF2(_L("Test stage %d passed"),iStage);
			iStatus = KRequestPending;
			iTimer.After(iStatus, timeout);
			SetActive();
			}
		}
	else
		{
		INFO_PRINTF1(_L("Final stage reached, test finished"));
		CActiveScheduler::Stop();
		}
	}
