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
// tsysmon_stepselfmoncancel.cpp
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <sysmonclisess.h>
#include <startupproperties.h>
#include <ssm/ssmstartupproperties.h>
#include "testprocgoodsession.h"
#include "tsysmon_stepselfmoncancel2.h"

CStepSelfMonCancel2::CStepSelfMonCancel2()
	{
	SetTestStepName(KCTestCaseSelfMonCancel2);
	}
/**
Old Test CaseID 		APPFWK-SYSMON-0009
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0009
 */

TVerdict CStepSelfMonCancel2::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0009"));
	
	INFO_PRINTF1(_L("Going to start a process"));
	RSysMonSession sysmon;
	CleanupClosePushL(sysmon);
	sysmon.OpenL();

	INFO_PRINTF1(_L("Going to request process monitoring using startupproperties"));
	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(process);

	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KLaunchServerCommandLineOption);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetMonitored(ETrue);
	prop->SetNoOfRetries(1); 
	
	TRAPD(err, sysmon.MonitorL(*prop, process)); 
	TESTEL(err == KErrNone, err);

	DoTestMonitorL(process);
	
	CleanupStack::PopAndDestroy(prop);
	CleanupStack::Pop(&process);

	INFO_PRINTF1(_L("Going to request process monitoring using ssmstartupproperties"));
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(process);

	CSsmStartupProperties* ssmProp = CSsmStartupProperties::NewLC(KTestProcGood, KLaunchServerCommandLineOption);
	ssmProp->SetExecutionBehaviour(ESsmWaitForSignal);
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 1;
	ssmProp->SetMonitorInfoL(monitorInfo);
	
	TRAP(err, sysmon.MonitorL(*ssmProp, process)); 
	TESTEL(err == KErrNone, err);

	DoTestMonitorL(process);
	
	CleanupStack::PopAndDestroy(ssmProp);
	CleanupStack::PopAndDestroy(&process);
	CleanupStack::PopAndDestroy(&sysmon);

	INFO_PRINTF1(_L("Test complete"));		
	return TestStepResult();	
	}

void CStepSelfMonCancel2::DoTestMonitorL(RProcess& aProc)
	{
	INFO_PRINTF1(_L("Monitoring is now setup, we will now make a request to cancel monitoring."));
	RTestProcGoodSession server;
	User::LeaveIfError(server.Connect());
	TEST(KErrNone == server.CancelMonitor());
	server.Close();

	INFO_PRINTF1(_L("A second cancel-request should make no difference."));
	User::LeaveIfError(server.Connect());
	TEST(KErrNone == server.CancelMonitor());
	server.Close();

	//Now, killing the process will not cause sysmon to restart after KWaitTime
	TEST(Exists(KTestProcGood));
	aProc.Kill(KErrNone);
	TEST(EFalse == Exists(KTestProcGood));
	INFO_PRINTF1(_L("Killed monitored process."));

	INFO_PRINTF1(_L("Going to sleep for 16 seconds."));		
	User::After(KThrottleTime + 1000000);

	INFO_PRINTF1(_L("Check that the process wasn't restarted"));		
	TEST(EFalse == Exists(KTestProcGood));
	}
