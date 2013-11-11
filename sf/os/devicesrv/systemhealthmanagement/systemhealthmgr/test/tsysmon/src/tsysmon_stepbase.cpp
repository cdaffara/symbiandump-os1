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

#include <apacmdln.h>
#include <apgcli.h>
#include <sysmonclisess.h>
#include <startupproperties.h>
#include <ssm/ssmstartupproperties.h>
#include "testprocgoodsession.h"
#include "tsysmon_stepbase.h"

/** 
Waits for the process to rendevouz, then resumes it.
*/
void CTestStepBase::ResumeL(RProcess& aProcess)
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
		TESTEL(EFalse, status.Int());
		}
	
	User::WaitForRequest(status);
	INFO_PRINTF1(_L("Process started"));
	}

/**
Connects to sysmon and request monitoring of aProcess using aProp.
*/

void CTestStepBase::MonitorL(const RProcess& aProcess, const CSsmStartupProperties& aProp)
	{	
	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	TRAPD(err, sess.MonitorL(aProp, aProcess));
	TESTL(err == KErrNone);	
	CleanupStack::PopAndDestroy(&sess);
	INFO_PRINTF1(_L("Monitoring initiated"));
	}		

/**
Connects to sysmon and request monitoring of aProcess using aProp.
*/
void CTestStepBase::MonitorL(const RProcess& aProcess, const CStartupProperties& aProp)
	{	
	RSysMonSession sess;
	sess.OpenL();
	CleanupClosePushL(sess);
	TRAPD(err, sess.MonitorL(aProp, aProcess));
	TESTL(err == KErrNone);	
	CleanupStack::PopAndDestroy(&sess);
	INFO_PRINTF1(_L("Monitoring initiated"));
	}		

/**
Creates and resumes aProcess with file=KTestProcGood args=KLaunchServerCommandLineOption, 
then sets up monitoring with aStartMethod and 1 retry.
*/
void CTestStepBase::StartAndMonitorUsingStartupPropL(RProcess& aProcess, TStartMethod aStartMethod)
	{
	//Start a process that launch a CServer2
	INFO_PRINTF1(_L("Going start a process"));
	User::LeaveIfError(aProcess.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(aProcess);
	
	//Setup monitoring	
	INFO_PRINTF1(_L("Going to request process monitoring"));
	CStartupProperties* prop = CStartupProperties::NewLC(KTestProcGood, KLaunchServerCommandLineOption);
	prop->SetMonitored(ETrue);
	prop->SetStartupType(EStartProcess);
	prop->SetStartMethod(aStartMethod);
	prop->SetNoOfRetries(1);
	MonitorL(aProcess, *prop);	
	CleanupStack::PopAndDestroy(prop);

	AssertServerIsRunning();
	}

/**
Creates and resumes aProcess with file=KTestProcGood args=KLaunchServerCommandLineOption, 
then sets up monitoring with aStartMethod and 1 retry.
*/
void CTestStepBase::StartAndMonitorUsingSsmStartupPropL(RProcess& aProcess, TStartMethod aStartMethod)
	{
	//Start a process that launch a CServer2
	INFO_PRINTF1(_L("Going start a process"));
	User::LeaveIfError(aProcess.Create(KTestProcGood, KLaunchServerCommandLineOption));
	ResumeL(aProcess);
	
	//Setup monitoring	
	INFO_PRINTF1(_L("Going to request process monitoring"));
	CSsmStartupProperties* prop = CSsmStartupProperties::NewLC(KTestProcGood, KLaunchServerCommandLineOption);
	prop->SetExecutionBehaviour(static_cast <TSsmExecutionBehaviour> (aStartMethod));
	TSsmMonitorInfo monitorInfo;
	monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
	monitorInfo.iRestartMode = 0;
	monitorInfo.iTimeout = 0;
	monitorInfo.iRetries = 1;
	prop->SetMonitorInfoL(monitorInfo);
	MonitorL(aProcess, *prop);
	CleanupStack::PopAndDestroy(prop);
	
	AssertServerIsRunning();
	}

void CTestStepBase::AssertServerIsRunning()
	{
	//Assert that the server is running
	RTestProcGoodSession server;
	TInt err = server.Connect();
	TEST(KErrNone == err);
	if(KErrNone == err)
		{
		INFO_PRINTF1(_L("Asserted that server is running"));		
		}
	server.Close();
	}

/**
Launch native application aAppName using an unconnected RApaLsSession
*/
void CTestStepBase::StartViewlessBgApplicationL(const TDesC& aAppName, TThreadId& aTreadId, TRequestStatus& aRequestStatus)
	{
	RApaLsSession apa;
	TThreadId threadId;
	CApaCommandLine* const cmdLine = CApaCommandLine::NewLC();
	cmdLine->SetExecutableNameL(aAppName);
	cmdLine->SetCommandL(EApaCommandBackgroundAndWithoutViews);
	User::LeaveIfError(apa.StartApp(*cmdLine, aTreadId, &aRequestStatus));
	CleanupStack::PopAndDestroy(cmdLine);
	}

/**
Asserts that at least one process starting with the name aProcessName is running.
*/
TBool CTestStepBase::Exists(const TDesC& aProcessName)
	{	
	TPtrC ptr(aProcessName);
	ptr.Set( ptr.Ptr(), ptr.Find(_L(".")) );
	TFullName procSerchTerm( ptr );
	procSerchTerm += _L("*");

	TFindProcess find(procSerchTerm);
	TFullName name;
	TBool found = EFalse;
	while(find.Next(name)==KErrNone)
		{
		RProcess process;
		const TInt err = process.Open(find);
		if (err == KErrNone)
			{
			if (process.ExitType() == EExitPending)
				{
				found = ETrue;
				process.Close(); 
				break;
				}
			process.Close(); 
			}
		}
	return found;
	}
