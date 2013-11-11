// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testprocgoodsession.h"
#include "tsysmon_steposrestarttest.h"
#include <ssm/ssmstate.h>
#include <ssm/ssmdomaindefs.h>
#include <ssm/ssmstateawaresession.h>
#include <sysmonclisess.h>
#include <startupproperties.h>
#include <e32cmn.h>
#include <f32file.h>

_LIT(KSysMonDontRendezvousRunCountFile, "C:\\testdata\\runcounts\\tsysmon_app_dontrendezvous.exe");

CStepOSRestart::CStepOSRestart(const TDesC& aTestName): iTestName(aTestName)
	{
    if (aTestName == KCTestCaseCriticalNoRetries)
        SetTestStepName(KCTestCaseCriticalNoRetriesStep);
    else if (aTestName == KCTestCaseRestartOS)
        SetTestStepName(KCTestCaseRestartOSStep);
    else
        {
        // If the test script is none of the above then we would fail the test
        TEST(KErrArgument);
        }
	}

CStepOSRestart::~CStepOSRestart()
    {
    iSsmStateAwareSess.Close();
    if (iAsyncCallbackForStartTest)
        delete iAsyncCallbackForStartTest;
    if(iAsyncCallbackForStopScheduler)
        delete iAsyncCallbackForStopScheduler;
    }

void CStepOSRestart::StateChanged(TSsmState aSsmState)
    {
  
    }
void CStepOSRestart::CallStopSchedulerL()
	{
	TSsmState currenState;
	currenState = iSsmStateAwareSess.State();

	TUint16 currentMainState = currenState.MainState();
	// Active scheduler is stopped once the system goes into shutdown state, else it issues a request for callback again
	// Once the system gets into normal state start the state transition test
	if (currentMainState == ESsmFail || currentMainState == ESsmShutdown)
		{
 		RDebug::Print(_L("\nStateTran Test: Stop Active Scheduler\n"));
 		TInt runCount = CSysMonTestHelper::ReadRunCountL(KSysMonDontRendezvousRunCountFile);
 		if((iTestName == KCTestCaseCriticalNoRetries) && (1 != runCount))
 		    SetTestStepResult(EFail);
 		else if((iTestName == KCTestCaseRestartOS) && (2 != runCount))
 		    SetTestStepResult(EFail);
		CActiveScheduler::Stop();
		}
	else
		{
		iAsyncCallbackForStopScheduler->CallBack();
		}
	}

static TInt StopScheduler(TAny* aStateTranFromNormal)
 	{
 	CStepOSRestart* stateTran = reinterpret_cast<CStepOSRestart*>(aStateTranFromNormal);
 	TRAPD(err,stateTran->CallStopSchedulerL());
 	return err;
 	}


TVerdict CStepOSRestart::doTestStepL()
	{
    TInt err =0;
    if (iTestName == KCTestCaseCriticalNoRetries)
        {
        TRAP(err, TestCriticalNoRetriesL());
        }
    else if (iTestName == KCTestCaseRestartOS)
        {
        TRAP(err, TestRestartOSL());
        }
    TEST(KErrNone == err);
    return TestStepResult();    
	}

void CStepOSRestart::TestCriticalNoRetriesL()
    {
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	RSysMonSession sysmon;
	sysmon.OpenL();
	CleanupClosePushL(sysmon);

	CStartupProperties* props = CStartupProperties::NewLC(_L("tsysmon_app_dontrendezvous.exe"), KNullDesC);
	props->SetMonitored(ETrue);
	props->SetStartupType(EStartProcess);
	props->SetStartMethod(EWaitForStart);
	props->SetNoOfRetries(1);
	props->SetTimeout(0);
	props->SetRecoveryParams(ECriticalNoRetries, 0);

	RProcess slave1;
	slave1.Create(_L("tsysmon_app_dontrendezvous.exe"), _L("5000"));
	CleanupClosePushL(slave1);

	TSsmState currenState;
	// Connects to RSsmStateAwareSession in order to enquire about the current system state
	TInt err = iSsmStateAwareSess.Connect(KSM2UiServicesDomain3);
	TEST(err == KErrNone);
	if (err == KErrNone)
		RDebug::Print(_L("\nStateTran Test: RSsmStateAwareSession sess Connect %d\n"), err);
	else
		{
		RDebug::Print(_L("\nStateTran Test: RSsmStateAwareSession sess Connect Failed\n"));	
		User::Leave(err);
		}

	currenState = iSsmStateAwareSess.State();

	RDebug::Print(_L("\nStateTran Test: Current System State is %d\n"), currenState.MainState());
	TEST(currenState.MainState() == ESsmNormal);

	iAsyncCallbackForStopScheduler =  new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
	TCallBack stopSchedulerCallback(StopScheduler, this);
	iAsyncCallbackForStopScheduler->Set(stopSchedulerCallback);

	// call the callback function which keeps looping until the system state gets into fail state.
	iAsyncCallbackForStopScheduler->CallBack();

	// Notifier used to get the notification when system gets to fail state
	CSsmStateAwareSession* notif_for_failstate = CSsmStateAwareSession::NewLC(KSM2UiServicesDomain3);
	notif_for_failstate->AddSubscriberL(*this);

    TRequestStatus status;
    slave1.Rendezvous(status);
    slave1.Resume();
    User::WaitForRequest(status);
	// Register with SysMon
	sysmon.MonitorL(*props, slave1);
	slave1.Terminate(KErrNone);

	sched->Start();

	CleanupStack::PopAndDestroy(5);
	}

void CStepOSRestart::TestRestartOSL()
    {
    CActiveScheduler* sched=new(ELeave) CActiveScheduler;
    CleanupStack::PushL(sched);
    CActiveScheduler::Install(sched);

    RSysMonSession sysmon;
    sysmon.OpenL();
    CleanupClosePushL(sysmon);

    CStartupProperties* props = CStartupProperties::NewLC(_L("tsysmon_app_dontrendezvous.exe"), KNullDesC);
    props->SetMonitored(ETrue);
    props->SetStartupType(EStartProcess);
    props->SetStartMethod(EWaitForStart);
    props->SetNoOfRetries(1);
    props->SetTimeout(0);
    props->SetRecoveryParams(ERestartOS, 0);

    RProcess slave1;
    slave1.Create(_L("tsysmon_app_dontrendezvous.exe"), _L("5000"));
    CleanupClosePushL(slave1);

    TSsmState currenState;
    // Connects to RSsmStateAwareSession in order to enquire about the current system state
    TInt err = iSsmStateAwareSess.Connect(KSM2UiServicesDomain3);
    TEST(err == KErrNone);
    if (err == KErrNone)
        RDebug::Print(_L("\nStateTran Test: RSsmStateAwareSession sess Connect %d\n"), err);
    else
        {
        RDebug::Print(_L("\nStateTran Test: RSsmStateAwareSession sess Connect Failed\n")); 
        User::Leave(err);
        }

    currenState = iSsmStateAwareSess.State();

    RDebug::Print(_L("\nStateTran Test: Current System State is %d\n"), currenState.MainState());
    TEST(currenState.MainState() == ESsmNormal);

    iAsyncCallbackForStopScheduler =  new(ELeave) CAsyncCallBack(CActive::EPriorityIdle);
    TCallBack stopSchedulerCallback(StopScheduler, this);
    iAsyncCallbackForStopScheduler->Set(stopSchedulerCallback);

    // call the callback function which keeps looping until the system state gets into fail state.
    iAsyncCallbackForStopScheduler->CallBack();

    // Notifier used to get the notification when system gets to fail state
    CSsmStateAwareSession* notif_for_failstate = CSsmStateAwareSession::NewLC(KSM2UiServicesDomain3);
    notif_for_failstate->AddSubscriberL(*this);

    TRequestStatus status;
    slave1.Rendezvous(status);
    slave1.Resume();
    User::WaitForRequest(status);
    // Register with SysMon
    sysmon.MonitorL(*props, slave1);
    slave1.Terminate(KErrNone);

    sched->Start();

    CleanupStack::PopAndDestroy(5);
    }

