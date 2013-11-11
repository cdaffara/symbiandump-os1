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

CStepSelfMonIgnore::CStepSelfMonIgnore()
	{
	SetTestStepName(KCTestCaseSelfMonIgnore);
	}
/**
Old Test CaseID 		APPFWK-SYSMON-0006
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0006
 */

TVerdict CStepSelfMonIgnore::doTestStepL()
	{
	INFO_PRINTF1(_L("Going to create the semaphore"));

	TInt err = iProcStartSignalSem.CreateGlobal(KStartShmaTestProcGood,0);
	INFO_PRINTF2(_L("Semaphore created with err %d"), err);
	TEST(KErrNone == err);

	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0006"));

	TRAP(err, DoTestSelfMonIgnoreL());
	TEST(err == KErrNone);

	TRAP(err, DoTestSsmSelfMonIgnoreL());
	TEST(err == KErrNone);

	iProcStartSignalSem.Close();
	return TestStepResult();
	}

void CStepSelfMonIgnore::DoTestSelfMonIgnoreL()
	{
	INFO_PRINTF1(_L("Going to set a process running"));

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(process);
	
	//Waiting here to decrement the semaphore value
	iProcStartSignalSem.Wait();
	SHMA_NEGATIVE_PLATSEC_START;
	TEST(EFalse == process.HasCapability(ECapabilityProtServ));
	SHMA_NEGATIVE_PLATSEC_FINISH;
	SHMA_NEGATIVE_PLATSEC_START;
	if (process.HasCapability(ECapabilityProtServ))
		{
		SHMA_NEGATIVE_PLATSEC_START;
		ERR_PRINTF1(_L("The Test Server got wrong set of capability - test failed"));
		process.Kill(KErrNone);
		User::Leave(KErrGeneral);
		}
			
	INFO_PRINTF1(_L("Going to initiate self monitoring of the process with ERestartOS using startupproperties"));	
	RTestProcGoodSession server;
	CleanupClosePushL(server);	
	TEST(KErrNone == server.Connect());
	SHMA_NEGATIVE_PLATSEC_START;
	TInt err = server.MonitorSelfWithStartupProp(ERestartOS); // we are expecting error for ERestartOS due to platsec
	SHMA_NEGATIVE_PLATSEC_FINISH;
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

	SHMA_NEGATIVE_PLATSEC_START;
	err = server.MonitorSelfWithStartupProp(ECriticalNoRetries); // we are expecting error for ECriticalNoRetries due to platsec
	SHMA_NEGATIVE_PLATSEC_FINISH;
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

	INFO_PRINTF1(_L("Going to initiate self monitoring of the process with EIgnoreOnFailure using startupproperties"));	
	err = server.MonitorSelfWithStartupProp(EIgnoreOnFailure); // self monitor should not need PlatSec for EIgnoreOnFailure
	TESTE(KErrNone == err, err);
	if (err == KErrNone)
		{
		INFO_PRINTF1(_L("Monitoring initiated."));
		}

	INFO_PRINTF1(_L("Killing the process - monitor should restart"));	
	process.Kill(KErrNone);
	
	CleanupStack::PopAndDestroy(&server);	
	CleanupStack::PopAndDestroy(&process);
	
	//Waiting for the process to restart using semaphore
	iProcStartSignalSem.Wait();
	
	err = server.Connect();
	TESTEL(KErrNone == err, err);
	INFO_PRINTF1(_L("Process restarted - Test completed"));	
	server.CancelMonitor();
	server.ShutDown();
	server.Close();
	}

void CStepSelfMonIgnore::DoTestSsmSelfMonIgnoreL()
	{
	INFO_PRINTF1(_L("Going to set a process running"));

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(process);

	//Waiting here to decrement the semaphore value
	iProcStartSignalSem.Wait();
	INFO_PRINTF1(_L("Going to initiate self monitoring of the process with ESsmRestartOS using ssmstartupproperties"));	
	RTestProcGoodSession server;
	CleanupClosePushL(server);	
	TEST(KErrNone == server.Connect());
	SHMA_NEGATIVE_PLATSEC_START;
	TInt err = server.MonitorSelfWithSsmStartupProp(ERestartOS); // we are expecting error for ERestartOS due to platsec
	SHMA_NEGATIVE_PLATSEC_FINISH;
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

	SHMA_NEGATIVE_PLATSEC_START;
	err = server.MonitorSelfWithSsmStartupProp(ECriticalNoRetries); // we are expecting error for ECriticalNoRetries due to platsec
	SHMA_NEGATIVE_PLATSEC_FINISH;
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

	INFO_PRINTF1(_L("Going to initiate self monitoring of the process with ESsmIgnoreOnFailure using ssmstartupproperties"));	
	err = server.MonitorSelfWithSsmStartupProp(EIgnoreOnFailure); // self monitor should not need PlatSec for EIgnoreOnFailure
	TESTE(KErrNone == err, err);
	if (err == KErrNone)
		{
		INFO_PRINTF1(_L("Monitoring initiated."));
		}

	INFO_PRINTF1(_L("Killing the process - monitor should restart"));	
	process.Kill(KErrNone);
	
	CleanupStack::PopAndDestroy(&server);	
	CleanupStack::PopAndDestroy(&process);
	//Waiting for the process to restart using semaphore
	iProcStartSignalSem.Wait();
	
	err = server.Connect();
	TESTEL(KErrNone == err, err);
	INFO_PRINTF1(_L("Process restarted - Test completed"));	
	server.CancelMonitor();
	server.ShutDown();
	server.Close();
	}


