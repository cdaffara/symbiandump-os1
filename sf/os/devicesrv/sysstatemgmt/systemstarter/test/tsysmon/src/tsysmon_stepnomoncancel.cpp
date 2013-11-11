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

#include "testprocgoodsession.h"
#include "tsysmon_stepnomoncancel.h"


CStepNoMonCancel::CStepNoMonCancel()
	{
	SetTestStepName(KCTestCaseNoMonCancel);
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0010
New Test CaseID 		DEVSRVS-SYSSTART-SYSMON-0010
 */

TVerdict CStepNoMonCancel::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0008"));
	
	INFO_PRINTF1(_L("Going to start a process"));
	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(process);

	INFO_PRINTF1(_L("Without monitoring the process, we will now make a request to cancel monitoring."));
	RTestProcGoodSession server;
	User::LeaveIfError(server.Connect());
	TEST(KErrNone == server.CancelMonitor());
	server.Close();

	//Killing the process will not cause sysmon to restart after KWaitTime
	TEST(Exists(KTestProcGood));
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	TEST(EFalse == Exists(KTestProcGood));
	INFO_PRINTF1(_L("Killed monitored process."));

	INFO_PRINTF1(_L("Going to sleep for 16 seconds."));		
	User::After(KThrottleTime + 1000000);

	INFO_PRINTF1(_L("Check that the process wasn't restarted"));		
	TEST(EFalse == Exists(KTestProcGood));
	
	INFO_PRINTF1(_L("Test complete"));		
	return TestStepResult();	
	}
