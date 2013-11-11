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
// appfwk_sysmon_teststep_selfmonignore.cpp
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "testprocgoodsession.h"
#include "tsysmon_stepselfmonignore.h"
#include <appfwk_test.h>


CStepSelfMonIgnore::CStepSelfMonIgnore()
	{
	SetTestStepName(KCTestCaseSelfMonIgnore);
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0006
New Test CaseID 		DEVSRVS-SYSSTART-SYSMON-0006
 */

TVerdict CStepSelfMonIgnore::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0006"));
	INFO_PRINTF1(_L("Going to set a process running"));

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(process);
	
	APPFWK_NEGATIVE_PLATSEC_START;
	TEST(EFalse == process.HasCapability(ECapabilityProtServ));
	APPFWK_NEGATIVE_PLATSEC_FINISH;
	APPFWK_NEGATIVE_PLATSEC_START;
	if (process.HasCapability(ECapabilityProtServ))
		{
		APPFWK_NEGATIVE_PLATSEC_START;
		ERR_PRINTF1(_L("The Test Server got wrong set of capability - test failed"));
		process.Kill(KErrNone);
		User::Leave(KErrGeneral);
		}
			
	INFO_PRINTF1(_L("Going to initiate self monitoring of the process with ERestartOS"));	
	RTestProcGoodSession server;
	CleanupClosePushL(server);	
	TEST(KErrNone == server.Connect());
	APPFWK_NEGATIVE_PLATSEC_START;
	TInt err = server.MonitorSelf(ERestartOS); // we are expecting error for ERestartOS due to platsec
	APPFWK_NEGATIVE_PLATSEC_FINISH;
	TESTE(KErrPermissionDenied == err, err);
	
	if (err == KErrPermissionDenied)
		{
		INFO_PRINTF1(_L("Sysmon responded with KErrPermissionDenied denied as expected."));
		}
	else
		{
		ERR_PRINTF1(_L("Did not get permission denied for self montioring of Restart OS - test failed"));
		User::Leave(KErrGeneral);
		}

	INFO_PRINTF1(_L("Going to initiate self monitoring of the process with EIgnoreOnFailure"));	
	err = server.MonitorSelf(EIgnoreOnFailure); // self monitor should not need PlatSec for EIgnoreOnFailure
	TESTE(KErrNone == err, err);
	if (err == KErrNone)
		{
		INFO_PRINTF1(_L("Monitoring initiated."));
		}

	INFO_PRINTF1(_L("Killing the process - monitor should restart"));	
	process.Kill(KErrNone);
	
	CleanupStack::PopAndDestroy(&server);	
	CleanupStack::PopAndDestroy(&process);
	
	User::After(KThrottleTime + 2000000); // allow time the process to restart
	
	err = server.Connect();
	TESTEL(KErrNone == err, err);
	INFO_PRINTF1(_L("Process restarted - Test completed"));	
	server.CancelMonitor();
	server.ShutDown();
	server.Close();

	return TestStepResult();	
	}
