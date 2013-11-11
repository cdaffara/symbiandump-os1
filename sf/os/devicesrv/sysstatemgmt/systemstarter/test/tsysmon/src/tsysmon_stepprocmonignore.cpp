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
#include "testprocgoodsession.h"
#include "tsysmon_stepprocmonignore.h"
#include "tpowerdownclient.h"

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
New Test CaseID 		DEVSRVS-SYSSTART-SYSMON-0005
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
	INFO_PRINTF1(_L("Going to request process monitoring"));	
	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KTestProcGoodNoRendevouz);
	prop->SetMonitored(ETrue);
	prop->SetStartupType(EStartProcess);
	prop->SetStartMethod(EWaitForStart);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(100);
	prop->SetRecoveryParams(EIgnoreOnFailure, 0); //this is what we are testing
	MonitorL(process, *prop);	
	CleanupStack::PopAndDestroy(prop);
	
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

	//Kill process and wait throttletime + time for recovery policy to execute
	TEST(Exists(KTestProcGood));
	process.Kill(KErrNone);
	TEST(EFalse == Exists(KTestProcGood));
	TTime time;
	time.HomeTime();
	INFO_PRINTF3(_L("Killed process at time %d:%d, going to sleep for KThrottleTime + 5 seconds."), time.DateTime().Minute(), time.DateTime().Second());	
		
	CActiveScheduler::Start(); // now wait until one of the callbacks occur
	
	timer->Cancel();
	CleanupStack::PopAndDestroy(timer);
	CleanupStack::PopAndDestroy(sched);
	CleanupStack::PopAndDestroy(&process);
	
	//Assert the process isn't there i.e. that the failure to restart KTestProcGood was ignored.	
	INFO_PRINTF1(_L("Going to assert that the process failed to restart."));
	const TBool running = Exists(KTestProcGood);
	TEST(EFalse == running);
	if(!running)
		{
		INFO_PRINTF1(_L("Process not running."));
		}
	
	INFO_PRINTF1(_L("Test complete."));
	return TestStepResult();	
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
	
	// Now call CPowerdownClient::QueryServerPowerState() which will in turn call the method 'ServerPowerState()'
	// to stop the shutdown. This function call will cause the shutdown server in syslibs/pwrcli to cancel power off.
	INFO_PRINTF1(_L("Aborting shutdown sequence"));
	TBool powerOff = EFalse;
	CPowerdownClient* powerdownclient = CPowerdownClient::NewL();
	CleanupStack::PushL(powerdownclient);
	TEST(KErrNone == powerdownclient->QueryServerPowerState(iNotifier,powerOff));
	TEST(powerOff);
	CleanupStack::PopAndDestroy(powerdownclient);
	CActiveScheduler::Stop();
	}
