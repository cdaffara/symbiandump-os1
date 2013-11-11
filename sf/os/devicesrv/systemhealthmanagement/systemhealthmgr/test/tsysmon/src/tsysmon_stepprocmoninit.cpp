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
#include "tsysmon_stepprocmoninit.h"


CStepProcMonInit::CStepProcMonInit()
	{
	SetTestStepName(KCTestCaseProcMonInit);
	}

TVerdict CStepProcMonInit::doTestStepL()
	{
	DoTestProcL();
	DoTestAppL();
	DoTestErrArgumentL();
	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0001
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0001
*/

void CStepProcMonInit::DoTestProcL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0001A"));
	INFO_PRINTF1(_L("Going to set a process running"));

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);
	
	INFO_PRINTF1(_L("Going to request process monitoring using startupproperties"));
	DoTestMonitorL(process, KTestProcGood, 0); // The test using startupproperties
	
	process.Kill(KErrNone);
	CleanupStack::Pop(&process);

	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);

	INFO_PRINTF1(_L("Going to request process monitoring using ssmstartupproperties"));
	DoTestMonitorL(process, KTestProcGood, 1); // The test using ssmstartupproperties
	
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	}

/**
Bonus-test APPFWK-SYSMON-0001b using an application instead of a process
*/	
void CStepProcMonInit::DoTestAppL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0001B"));
		
	// Launch native application using an unconnected RApaLsSession
	INFO_PRINTF1(_L("Going to set a native application running"));
	TThreadId threadId;
	TRequestStatus requestStatus;
	StartViewlessBgApplicationL(KTestAppGood, threadId, requestStatus);
	
	// Test that the application was created, then wait for it to resume	
	RThread thread;
	RProcess process;
	CleanupClosePushL(thread);
	CleanupClosePushL(process);
	TEST(KErrNone == thread.Open(threadId));
	TEST(KErrNone == thread.Process(process)); 
	User::WaitForRequest(requestStatus);
	
	INFO_PRINTF1(_L("Going to request process monitoring using startupproperties"));
	// Do the actual testing
	DoTestMonitorL(process, KTestAppGood, 0); // test using startupproperties
	
	// Cleanup
	process.Kill(KErrNone);

	StartViewlessBgApplicationL(KTestAppGood, threadId, requestStatus);
	CleanupStack::Pop(&process);
	CleanupStack::Pop(&thread);
	
	CleanupClosePushL(thread);
	CleanupClosePushL(process);
	TEST(KErrNone == thread.Open(threadId));
	TEST(KErrNone == thread.Process(process)); 
	User::WaitForRequest(requestStatus);
	
	INFO_PRINTF1(_L("Going to request process monitoring using ssmstartupproperties"));
	// Do the actual testing
	DoTestMonitorL(process, KTestAppGood, 1); // test using ssmstartupproperties
	
	// Cleanup
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	CleanupStack::PopAndDestroy(&thread);
	}	

/**
 * Calls RSysMonSession::MonitorL twice. the first call is expected to succeed and the second call
 * returns KErrNone in release builds, whereas panics in debug builds
 */	
void CStepProcMonInit::DoTestMonitorL(const RProcess& aProcess, const TDesC& aFilename, const TInt aStartUpPropType)
	{
	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	
	if (aStartUpPropType)
		{
		CSsmStartupProperties* prop = CSsmStartupProperties::NewLC(aFilename, KNullDesC);
		// Testing consistency assertion of aStartupProperties in RSysMonSession::MonitorL
		TRAPD(err, sess.MonitorL(*prop, aProcess)); 
		TESTE(err == KErrArgument, err);
		if(err == KErrArgument)
			{
			INFO_PRINTF1(_L("Monitor request with invalid args was refused properly by RSysMonSession::MonitorL using ssmstartupproperties"));
			}
		
		prop->SetExecutionBehaviour(ESsmWaitForSignal);
		TSsmMonitorInfo monitorInfo;
		monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
		monitorInfo.iRestartMode = 0;
		monitorInfo.iTimeout = 0;
		monitorInfo.iRetries = 0;
		prop->SetMonitorInfoL(monitorInfo); // SysMon will not do any restart attempts
		
		// monitoring first time should succeed
		TRAP(err, sess.MonitorL(*prop, aProcess)); 
		TESTEL(err == KErrNone, err);
		INFO_PRINTF1(_L("First monitor request succeeded"));
		
		TRAP(err, sess.MonitorL(*prop, aProcess)); 
		TESTE(err == KErrAlreadyExists, err);
		if(err == KErrAlreadyExists)
			{
			INFO_PRINTF1(_L("Second monitor request returned KErrAlreadyExists as expected"));
			}
		
		CleanupStack::PopAndDestroy(prop);
		}
	else
		{
		CStartupProperties* prop = CStartupProperties::NewLC(aFilename, KNullDesC);
		// Testing consistency assertion of aStartupProperties in RSysMonSession::MonitorL
		TRAPD(err, sess.MonitorL(*prop, aProcess)); 
		TESTE(err == KErrArgument, err);
		if(err == KErrArgument)
			{
			INFO_PRINTF1(_L("Monitor request with invalid args was refused properly by RSysMonSession::MonitorL using startupproperties"));
			}
		
		prop->SetMonitored(ETrue);
		prop->SetNoOfRetries(0); // SysMon will not do any restart attempts
		
		// monitoring first time should succeed
		TRAP(err, sess.MonitorL(*prop, aProcess)); 
		TESTEL(err == KErrNone, err);
		INFO_PRINTF1(_L("First monitor request succeeded"));
		
		TRAP(err, sess.MonitorL(*prop, aProcess)); 
		TESTE(err == KErrAlreadyExists, err);
		if(err == KErrAlreadyExists)
			{
			INFO_PRINTF1(_L("Second monitor request returned KErrAlreadyExists as expected"));
			}

		CleanupStack::PopAndDestroy(prop);
		}
	CleanupStack::PopAndDestroy(&sess);
	}


/*
Bonus-test APPFWK-SYSMON-0001C 
Testing consistency of assertions in RSysMonSession::MonitorL
*/
void CStepProcMonInit::DoTestErrArgumentL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0001C -  validade that sysmon refuse invalid requests"));

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);
	
	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	CSsmStartupProperties* ssmProp = CSsmStartupProperties::NewLC(KTestProcGood, KNullDesC);
	
	INFO_PRINTF1(_L("Going to check if RSysMonSession::MonitorL correctly refuse monitor with CStartupProperties::Monitor()==EFalse using startupproperties"));
	TRAPD(err, sess.MonitorL(*prop, process)); 
	TESTE(err == KErrArgument, err);

	INFO_PRINTF1(_L("Going to check if RSysMonSession::MonitorL correctly refuse monitor with CStartupProperties::Monitor()==EFalse using ssmstartupproperties"));
	TRAP(err, sess.MonitorL(*ssmProp, process)); 
	TESTE(err == KErrArgument, err);
	
	INFO_PRINTF1(_L("Going to check if RSysMonSession::MonitorL correctly refuse monitor with CStartupProperties::StartMethod==EDeferredWaitForStart using startupproperties"));
	prop->SetStartMethod(EDeferredWaitForStart);
	prop->SetNoOfRetries(0);
	TRAP(err, sess.MonitorL(*prop, process)); 
	TESTE(err == KErrArgument, err);

	INFO_PRINTF1(_L("Going to check if RSysMonSession::MonitorL correctly refuse monitor with CStartupProperties::StartMethod==ESsmDeferredWaitForSignal using ssmstartupproperties"));
	ssmProp->SetExecutionBehaviour(ESsmDeferredWaitForSignal);	
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 0;
	ssmProp->SetMonitorInfoL(monitorInfo);
	TRAP(err, sess.MonitorL(*ssmProp, process)); 
	TESTE(err == KErrArgument, err);
	
	INFO_PRINTF1(_L("Going to check if RSysMonSession::MonitorL correctly refuse to restart another exe image than the one it monitors using startupproperties."));
	prop->SetStartMethod(EWaitForStart);
	prop->SetNoOfRetries(1);
	prop->SetFileParamsL(_L("sdkjfhsdk"), KNullDesC);
	TRAP(err, sess.MonitorL(*prop, process)); 
	TESTE(err == KErrArgument, err);

	INFO_PRINTF1(_L("Going to check if RSysMonSession::MonitorL correctly refuse to restart another exe image than the one it monitors using ssmsstartupproperties."));
	ssmProp->SetExecutionBehaviour(ESsmWaitForSignal);	
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 1;
	ssmProp->SetMonitorInfoL(monitorInfo);
	ssmProp->SetFileParamsL(_L("sdkjfhsdk"), KNullDesC);
	TRAP(err, sess.MonitorL(*ssmProp, process)); 
	TESTE(err == KErrArgument, err);

	INFO_PRINTF1(_L("Going to check if RSysMonSession::MonitorL correctly refuse to monitor a dead process using startupproperties."));
	prop->SetFileParamsL(KTestProcGood, KNullDesC);
	prop->SetStartMethod(EWaitForStart);
	prop->SetMonitored(ETrue);
	prop->SetNoOfRetries(1);
	process.Kill(KErrNone);
	TRAP(err, sess.MonitorL(*prop, process)); 
	TESTE(err == KErrDied, err);

	INFO_PRINTF1(_L("Going to check if RSysMonSession::MonitorL correctly refuse to monitor a dead process using ssmstartupproperties."));
	ssmProp->SetFileParamsL(KTestProcGood, KNullDesC);
	TRAP(err, sess.MonitorL(*ssmProp, process)); 
	TESTE(err == KErrDied, err);

	CleanupStack::PopAndDestroy(ssmProp);
	CleanupStack::PopAndDestroy(prop);
	CleanupStack::PopAndDestroy(&sess);
	CleanupStack::PopAndDestroy(&process);
	}
