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
#include "testprocgoodsession.h"
#include "tsysmon_stepselfmoncancel2.h"

CStepSelfMonCancel2::CStepSelfMonCancel2()
	{
	SetTestStepName(KCTestCaseSelfMonCancel2);
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0009
New Test CaseID 		DEVSRVS-SYSSTART-SYSMON-0009
 */

TVerdict CStepSelfMonCancel2::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0009"));
	
	INFO_PRINTF1(_L("Going to start a process"));
	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(process);

	INFO_PRINTF1(_L("Going to request process monitoring"));
	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KLaunchServerCommandLineOption);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetMonitored(ETrue);
	prop->SetNoOfRetries(1); 
	
	RSysMonSession sysmon;
	CleanupClosePushL(sysmon);
	sysmon.OpenL();
	TRAPD(err, sysmon.MonitorL(*prop, process)); 
	TESTEL(err == KErrNone, err);
	
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
	process.Kill(KErrNone);
	TEST(EFalse == Exists(KTestProcGood));
	INFO_PRINTF1(_L("Killed monitored process."));
	
	CleanupStack::PopAndDestroy(&sysmon);
	CleanupStack::PopAndDestroy(prop);
	CleanupStack::PopAndDestroy(&process);

	INFO_PRINTF1(_L("Going to sleep for 16 seconds."));		
	User::After(KThrottleTime + 1000000);

	INFO_PRINTF1(_L("Check that the process wasn't restarted"));		
	TEST(EFalse == Exists(KTestProcGood));
	
	INFO_PRINTF1(_L("Test complete"));		
	return TestStepResult();	
	}
