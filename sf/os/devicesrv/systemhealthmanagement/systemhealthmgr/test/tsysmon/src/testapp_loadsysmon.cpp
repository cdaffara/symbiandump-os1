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

#include "testapp_loadsysmon.h"
#include "testapps.h"

_LIT(KTestAppLoadSysmon, "testapp_loadsysmon.exe");
_LIT(KLoadSysMonDLL, "loadsysmon.dll");
typedef MSsmLoadSysMon* (*TFuncNewL)(void);

void TestLoadSysMonDllL()
	{
	CTLoadSysMon* loadSysMon = CTLoadSysMon::NewLC();
	
	//Tests Sysmon functionlities using SsmStartupProperties
	CSsmStartupProperties *ssmPropForSelfMon = CSsmStartupProperties::NewLC(KTestAppLoadSysmon, KNullDesC);
	ssmPropForSelfMon->SetExecutionBehaviour(ESsmWaitForSignal);	
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 0;
	ssmPropForSelfMon->SetMonitorInfoL(monitorInfo);// SysMon will not do any restart attempts

	CSsmStartupProperties *ssmForOtherProcMon = CSsmStartupProperties::NewLC(KTestProcGood, KNullDesC);
	ssmForOtherProcMon->SetExecutionBehaviour(ESsmWaitForSignal);	
	ssmForOtherProcMon->SetMonitorInfoL(monitorInfo);// SysMon will not do any restart attempts

	loadSysMon->TestForSysMonFuncUsingSsmStartupPropL(*ssmPropForSelfMon, *ssmForOtherProcMon);
	
	CleanupStack::PopAndDestroy(ssmForOtherProcMon);
	CleanupStack::PopAndDestroy(ssmPropForSelfMon);

	//Tests Sysmon functionlities using StartupProperties
	CStartupProperties *propForSelfMon = CStartupProperties::NewLC(KTestAppLoadSysmon, KNullDesC);
	propForSelfMon->SetMonitored(ETrue);
	propForSelfMon->SetNoOfRetries(0); // SysMon will not do any restart attempts

	CStartupProperties *propForOtherProcMon = CStartupProperties::NewLC(KTestProcGood, KNullDesC);
	propForOtherProcMon->SetMonitored(ETrue);
	propForOtherProcMon->SetNoOfRetries(0); // SysMon will not do any restart attempts

	loadSysMon->TestForSysMonFuncUsingStartupPropL(*propForSelfMon, *propForOtherProcMon);
	
	CleanupStack::PopAndDestroy(propForOtherProcMon);
	CleanupStack::PopAndDestroy(propForSelfMon);
	CleanupStack::PopAndDestroy(loadSysMon);
	}

CTLoadSysMon* CTLoadSysMon::NewLC()
	{
	CTLoadSysMon* self = new (ELeave) CTLoadSysMon();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTLoadSysMon::CTLoadSysMon()
	{
	}

void CTLoadSysMon::ConstructL()
	{
	User::LeaveIfError(iMonitorProxyLib.Load(KLoadSysMonDLL)); 
		 
	TFuncNewL sysMonNewL = reinterpret_cast<TFuncNewL>(iMonitorProxyLib.Lookup(1));
	iSysMonCli = sysMonNewL();
	}

CTLoadSysMon::~CTLoadSysMon()
	{
	delete iSysMonCli;
	iMonitorProxyLib.Close();
	}

void CTLoadSysMon::TestForSysMonFuncUsingSsmStartupPropL(const CSsmStartupProperties& aSsmStartupPropForSelfMonitor, const CSsmStartupProperties& aSsmStartupPropForOtherProcMonitor)
	{
	// Opens session with sysmoncli through loadsysmon interface
	iSysMonCli->OpenL();

	// Sets up monitor for self
	iSysMonCli->MonitorSelfL(aSsmStartupPropForSelfMonitor);
	// Cancel the Monitor
	iSysMonCli->CancelMonitorSelfL();

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);

	// Sets up monitor for KTestProcGood
	iSysMonCli->MonitorL(aSsmStartupPropForOtherProcMonitor, process);
	// Cancel the Monitor
	iSysMonCli->CancelMonitorSelfL();

	//Killing the process
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);

	// Closes the session with sysmoncli
	iSysMonCli->Close();
	}

void CTLoadSysMon::TestForSysMonFuncUsingStartupPropL(const CStartupProperties& aStartupPropForSelfMonitor, const CStartupProperties& aStartupPropForOtherProcMonitor)
	{
	// Opens session with sysmoncli through loadsysmon interface
	iSysMonCli->OpenL();

	// Sets up monitor for self
	iSysMonCli->MonitorSelfL(aStartupPropForSelfMonitor);
	// Cancel the Monitor
	iSysMonCli->CancelMonitorSelfL();

	RProcess process;
	CleanupClosePushL(process);
	User::LeaveIfError(process.Create(KTestProcGood, KNullDesC));
	ResumeL(process);

	// Sets up monitor for KTestProcGood
	iSysMonCli->MonitorL(aStartupPropForOtherProcMonitor, process);
	// Cancel the Monitor
	iSysMonCli->CancelMonitorSelfL();

	//Killing the process
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);

	// Closes the session with sysmoncli
	iSysMonCli->Close();
	}

/** 
Waits for the process to rendevouz, then resumes it.
*/
void CTLoadSysMon::ResumeL(RProcess& aProcess)
	{
	TRequestStatus status;
	aProcess.Rendezvous(status);
	if (status == KRequestPending)
		{
		aProcess.Resume();
		}
	else
		{
		aProcess.Kill(KErrGeneral);
		User::Leave(status.Int());
		}
	
	User::WaitForRequest(status);
	}

//  Global Functions
TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	if (!cleanup)
		{
		RProcess().Terminate(KErrNoMemory);
		}

	// Run application code inside TRAP harness
	TRAPD(testError, TestLoadSysMonDllL());
	if (testError)
		{
		RProcess().Terminate(testError);
		}

	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	} //lint -e714 Suppress 'not referenced'
