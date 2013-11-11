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

#include <sysmonclisess.h>
#include <startupproperties.h>
#include <ssm/ssmstartupproperties.h>
#include "testapps.h"
#include "tsysmon_stepnoprocmon.h"


CStepNoProcMon::CStepNoProcMon()
	{
	SetTestStepName(KCTestCaseNoProcMon);
	}

TVerdict CStepNoProcMon::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0007"));
	
	TRAPD(err, DoTestNullHandleL());
	TEST(err == KErrNone);

	TRAP(err, DoTestKilledHandleL());
	TEST(err == KErrNone);
	
	TRAP(err, DoTestWrongHandleL());
	TEST(err == KErrNone);

	TRAP(err, DoTestProcMonErrorsL());
	TEST(err == KErrNone);
	
	return TestStepResult();
	}
/**
Old Test CaseID 		APPFWK-SYSMON-0007
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0007
 */

void CStepNoProcMon::DoTestNullHandleL()
	{
	INFO_PRINTF1(_L("Requesting SysMon to monitor a process NULL handle using startupproperties"));
	
	RProcess process;
	process.Close();

	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	
	CStartupProperties *prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetRecoveryParams(EIgnoreOnFailure, 0);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(0);
			
	TRAPD(err, sess.MonitorL(*prop, process)); // check that passing in a null handle would return KErrArugment
	
	// check that the initiation of monitoring failed 
	TESTE(KErrArgument == err, err);
	if(KErrArgument == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrArgument"));
		}
	CleanupStack::PopAndDestroy(prop);

	INFO_PRINTF1(_L("Requesting SysMon to monitor a process NULL handle using ssmstartupproperties"));
	CSsmStartupProperties *ssmProp = CSsmStartupProperties::NewLC(KTestProcGood, KNullDesC);
	ssmProp->SetExecutionBehaviour(ESsmWaitForSignal);	
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 1;

	ssmProp->SetMonitorInfoL(monitorInfo);// SysMon will not do any restart attempts

	TRAP(err, sess.MonitorL(*ssmProp, process)); // check that passing in a null handle would return KErrArugment
	
	// check that the initiation of monitoring failed 
	TESTE(KErrArgument == err, err);
	if(KErrArgument == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrArgument"));
		}

	CleanupStack::PopAndDestroy(ssmProp);
	CleanupStack::PopAndDestroy(&sess);
	}

/**
Check that passing in a handle to a killed process will return KErrArugment
*/
void CStepNoProcMon::DoTestKilledHandleL()
	{
	INFO_PRINTF1(_L("Requesting SysMon to monitor a handle to a killed process using startupproperties"));

	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);
	
	process.Kill(KErrNone);
	INFO_PRINTF1(_L("Process killed."));
	
	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetRecoveryParams(EIgnoreOnFailure, 0);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(0);
				
	TRAPD(err, sess.MonitorL(*prop, process));
	
	// check that the initiation of monitoring failed 
	TESTE(KErrDied == err, err);
	if(KErrDied == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrDied"));
		}
		
	CleanupStack::PopAndDestroy(prop);
	CleanupStack::Pop(&process);

	INFO_PRINTF1(_L("Requesting SysMon to monitor a handle to a killed process using ssmstartupproperties"));
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);
	
	process.Kill(KErrNone);
	INFO_PRINTF1(_L("Process killed."));

	CSsmStartupProperties *ssmProp = CSsmStartupProperties::NewLC(KTestProcGood, KNullDesC);
	ssmProp->SetExecutionBehaviour(ESsmWaitForSignal);	
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 1;

	ssmProp->SetMonitorInfoL(monitorInfo); // SysMon will not do any restart attempts

	TRAP(err, sess.MonitorL(*ssmProp, process));
	
	// check that the initiation of monitoring failed 
	TESTE(KErrDied == err, err);
	if(KErrDied == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrDied"));
		}

	CleanupStack::PopAndDestroy(ssmProp);
	CleanupStack::PopAndDestroy(&process);
	CleanupStack::PopAndDestroy(&sess);	
	}

void CStepNoProcMon::DoTestWrongHandleL()
	{
	INFO_PRINTF1(_L("Requesting SysMon to monitor wrong process handle using startupproperties"));
	
	RProcess process; // initiated with handle to current process

	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	
	CStartupProperties *prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop->SetMonitored(ETrue);
	prop->SetStartMethod(EWaitForStart);	
	prop->SetRecoveryParams(EIgnoreOnFailure, 0);
	prop->SetNoOfRetries(1);
	prop->SetTimeout(0);
			
	TRAPD(err, sess.MonitorL(*prop, process)); // check that passing in a null handle would return KErrArugment
	
	// check that the initiation of monitoring failed 
	TESTE(KErrArgument == err, err);
	if(KErrArgument == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrArgument"));
		}
		
	CleanupStack::PopAndDestroy(prop);

	INFO_PRINTF1(_L("Requesting SysMon to monitor wrong process handle using ssmstartupproperties"));
	CSsmStartupProperties *ssmProp = CSsmStartupProperties::NewLC(KTestProcGood, KNullDesC);
	ssmProp->SetExecutionBehaviour(ESsmWaitForSignal);	
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 1;

	ssmProp->SetMonitorInfoL(monitorInfo);// SysMon will not do any restart attempts
			
	TRAP(err, sess.MonitorL(*ssmProp, process)); // check that passing in a null handle would return KErrArugment
	
	// check that the initiation of monitoring failed 
	TESTE(KErrArgument == err, err);
	if(KErrArgument == err)
		{
		INFO_PRINTF1(_L("Request refused with KErrArgument"));
		}
		
	CleanupStack::PopAndDestroy(ssmProp);
	CleanupStack::PopAndDestroy(&sess);
	}

void CStepNoProcMon::DoTestProcMonErrorsL()
	{
	INFO_PRINTF1(_L("Testin SysMon in different error conditions"));
	RProcess process;
	CleanupClosePushL(process);
	
	INFO_PRINTF1(_L("Going start a process"));
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);
	
	//Setup monitoring	
	INFO_PRINTF1(_L("Going to request process monitoring"));
	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);

	CSsmStartupProperties* prop = CSsmStartupProperties::NewLC(KTestProcGood, KNullDesC);
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 1;
	prop->SetMonitorInfoL(monitorInfo);
	prop->SetFileParamsL(KNullDesC,KNullDesC);
	prop->SetCommandTypeL(ESsmCmdStartApp);

	INFO_PRINTF1(_L("Going to request process monitoring with NULL filename type"));
	TRAPD(err, sess.MonitorL(*prop, process));
	TEST(err == KErrArgument);
	INFO_PRINTF3(_L("Actual return value for NULL filename %d, Expected return value %d"), err, KErrArgument);

	prop->Reset();
	prop->SetFileParamsL(KNullDesC,KNullDesC);
	prop->SetCommandTypeL(ESsmCmdStartApp);
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = -12;
	prop->SetMonitorInfoL(monitorInfo);

	CleanupStack::Pop(prop);
	CleanupStack::PushL(prop);
	
	INFO_PRINTF1(_L("Going to request process monitoring with -ve retries for WaitForStart execution behaviour"));
	TRAP(err, sess.MonitorL(*prop, process));
	TEST(err == KErrArgument);
	INFO_PRINTF3(_L("Actual return value for -ve retries for WaitForStart execution behaviour %d, Expected return value %d"), err, KErrArgument);

	prop->Reset();
	prop->SetFileParamsL(KNullDesC,KNullDesC);
	prop->SetCommandTypeL(ESsmCmdStartProcess);
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = -100;
	monitorInfo.iRetries = 1;
	prop->SetMonitorInfoL(monitorInfo);

	CleanupStack::Pop(prop);
	CleanupStack::PushL(prop);

	INFO_PRINTF1(_L("Going to request process monitoring with -ve timeout for WaitForStart execution behaviour"));
	TRAP(err, sess.MonitorL(*prop, process));
	TEST(err == KErrArgument);
	INFO_PRINTF3(_L("Actual return value for -ve timeout for WaitForStart execution behaviour %d, Expected return value %d"), err, KErrArgument);

	prop->Reset();
	prop->SetFileParamsL(KNullDesC,KNullDesC);
	prop->SetCommandTypeL(ESsmCmdStartProcess);
	monitorInfo.iRestartPolicy = (static_cast <TSsmRecoveryMethod>(100));
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = -100;
	monitorInfo.iRetries = 1;
	prop->SetMonitorInfoL(monitorInfo);

	CleanupStack::Pop(prop);
	CleanupStack::PushL(prop);

	INFO_PRINTF1(_L("Going to request process monitoring with invalid recoverymethod for WaitForStart execution behaviour"));
	TRAP(err, sess.MonitorL(*prop, process));
	TEST(err == KErrArgument);
	INFO_PRINTF3(_L("Actual return value for invalid recoverymethod for WaitForStart execution behaviour %d, Expected return value %d"), err, KErrArgument);

	CStartupProperties* prop1 = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	prop1->SetStartupType(static_cast<TStartupType>(10));
	prop1->SetMonitored(ETrue);
	prop1->SetStartMethod(EWaitForStart);	
	prop1->SetRecoveryParams(EIgnoreOnFailure, 0);
	prop1->SetNoOfRetries(1);
	prop1->SetTimeout(0);
				
	INFO_PRINTF1(_L("Going to request process monitoring with invalid startuptype"));
	TRAP(err, sess.MonitorL(*prop, process));
	TEST(err == KErrArgument);
	INFO_PRINTF3(_L("Actual return value for invalid startuptype %d, Expected return value %d"), err, KErrArgument);

	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(4, &process);
	INFO_PRINTF1(_L("Test step complete"));		
	}


