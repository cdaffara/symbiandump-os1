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

#include <startupproperties.h>
#include <ssm/ssmstartupproperties.h>
#include "testprocgoodsession.h"
#include "tsysmon_stepprocmonignore.h"


CStepProcMonIgnore::~CStepProcMonIgnore()
	{
	delete iNotifier;
	}

CStepProcMonIgnore::CStepProcMonIgnore()
	{
	SetTestStepName(KCTestCaseProcMonIgnore);
	}
/**
Old Test CaseID 		APPFWK-SYSMON-0005
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0005
 */

TVerdict CStepProcMonIgnore::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0005"));
		
	INFO_PRINTF1(_L("Going to start a process"));
	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);
	
	//Setup monitoring and instruct KTestProcGood to not rendevouz when restarted
	INFO_PRINTF1(_L("Going to request process monitoring using startupproperties"));
	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KTestProcGoodNoRendevouz);
	prop->SetMonitored(ETrue);
	prop->SetStartupType(EStartProcess);
	prop->SetStartMethod(EWaitForStart);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(100);
	prop->SetRecoveryParams(EIgnoreOnFailure, 0); //this is what we are testing
	MonitorL(process, *prop);	
	CleanupStack::PopAndDestroy(prop);
	ProcMonIgnoreL(process);
	CleanupStack::PopAndDestroy(&process);

	// Test System Monitor using SsmStartupProperties
	INFO_PRINTF1(_L("Going to request process monitoring using ssmstartupproperties"));
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);

	CSsmStartupProperties* ssmProp = CSsmStartupProperties::NewLC(KTestProcGood, KTestProcGoodNoRendevouz);
	ssmProp->SetExecutionBehaviour(ESsmWaitForSignal);	
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 100;
	monitorInfo.iRetries = 1;

	ssmProp->SetMonitorInfoL(monitorInfo); //this is what we are testing
	MonitorL(process, *ssmProp);	
	CleanupStack::PopAndDestroy(ssmProp);
	ProcMonIgnoreL(process);
	CleanupStack::PopAndDestroy(&process);
	
	INFO_PRINTF1(_L("Test complete."));
	return TestStepResult();	
	}

void CStepProcMonIgnore::ProcMonIgnoreL(RProcess& aProcess)
	{
	//Create a scheduler for callbacks
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	//Timed callback to stop the nested scheduler when we have allowed enough time for the recovery policy to execute
	CPeriodic* timer = CPeriodic::NewL(CActive::EPriorityStandard);
	CleanupStack::PushL(timer);
	const TInt delay = KThrottleTime + 5000000; //make sure delay allow all retries enough time to complete their timeouts
	timer->Start(delay, delay, TCallBack(CancelWait, this));
		
	//Observer for OS restart 
	iNotifier = CSaveNotifier::NewL(*this);

	TRequestStatus status;
	aProcess.Logon(status);
	TEST(status == KRequestPending);
		
	//Kill process and wait throttletime + time for recovery policy to execute
	TEST(Exists(KTestProcGood));
	aProcess.Kill(KErrNone);
	
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	TEST(KErrNone == aProcess.ExitReason());

	TEST(EFalse == Exists(KTestProcGood));
	TTime time;
	time.HomeTime();
	INFO_PRINTF3(_L("Killed process at time %d:%d, going to sleep for KThrottleTime + 5 seconds."), time.DateTime().Minute(), time.DateTime().Second());	
		
	CActiveScheduler::Start(); // now wait until one of the callbacks occur
	
	timer->Cancel();
	CleanupStack::PopAndDestroy(timer);
	CleanupStack::PopAndDestroy(sched);
	
	//Assert the process isn't there i.e. that the failure to restart KTestProcGood was ignored.	
	INFO_PRINTF1(_L("Going to assert that the process failed to restart."));
	const TBool running = Exists(KTestProcGood);
	TEST(EFalse == running);
	if(!running)
		{
		INFO_PRINTF1(_L("Process not running."));
		}
	}

TInt CStepProcMonIgnore::CancelWait(TAny* aSelf)
	{
	static_cast<CStepProcMonIgnore*>(aSelf)->ReportTestSuccessful();
	CActiveScheduler::Stop();
	return KErrNone;
	}

void CStepProcMonIgnore::ReportTestSuccessful()
	{
	TTime time;
	time.HomeTime();
	INFO_PRINTF3(_L("No OS restart is taking place, aborting wait at time %d:%d"), time.DateTime().Minute(), time.DateTime().Second());
	}

/**
A call to this function means the test failed.
*/
void CStepProcMonIgnore::SaveL(TSaveType /*aSaveType*/)
	{  // Leavescan, virtual function is allowed to leave althought current implementation doesn't
	
	INFO_PRINTF1(_L("OS restart detected - test failed"));		
	TEST(EFalse); // if a OS restart is initiatedm the test if failed
	iNotifier->DelayRequeue();
	// Now call iNotifier->HandleError() to stop the shutdown.
	// This function call RSaveSession::HandleSaveError(), which will cause 
	// the shutdown server in syslib/pwrcli to cancel power off
	iNotifier->HandleError(KErrNone);
	CActiveScheduler::Stop();
	}
