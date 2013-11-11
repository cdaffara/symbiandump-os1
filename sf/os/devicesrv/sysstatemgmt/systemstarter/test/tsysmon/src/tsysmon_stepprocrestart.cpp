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

#include "testprocgoodsession.h"
#include "tsysmon_stepprocrestart.h"


CStepProcRestart::CStepProcRestart()
	{
	SetTestStepName(KCTestCaseProcRestart);
	}

TVerdict CStepProcRestart::doTestStepL()
	{
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0003"));
	DoTestProcRestartL();
	
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0003b"));	
	DoTestExtendedProcRestartL();

//	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0003c"));	
//	DoTestAppRestartL();
	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0003
New Test CaseID 		DEVSRVS-SYSSTART-SYSMON-0003
 */

void CStepProcRestart::DoTestProcRestartL()
	{
	INFO_PRINTF1(_L("New Test step"));
	RProcess process;
	CleanupClosePushL(process);
	StartAndMonitorL(process, EWaitForStart);
	
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	INFO_PRINTF1(_L("Killed monitored process."));		
	TESTL(AssertRestart()); // this is the main test
	
	Cleanup();	
	
	INFO_PRINTF1(_L("Test step complete"));		
	INFO_PRINTF1(_L(""));
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0003b
New Test CaseID 		DEVSRVS-SYSSTART-SYSMON-0003
 */	
void CStepProcRestart::DoTestExtendedProcRestartL()
	{
	INFO_PRINTF1(_L("New Test step"));
	RProcess process;
	CleanupClosePushL(process);
	StartAndMonitorL(process, EWaitForStart);
	
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	INFO_PRINTF1(_L("Killed monitored process."));		
	TESTL(AssertRestart()); 
	
	//Now do the same test for the restarted process, can't use the old processhandle
	RTestProcGoodSession server;	
	TEST(KErrNone == server.Connect());
	server.ShutDown();
	server.Close();
	INFO_PRINTF1(_L("Killed the restarted process."));		
	TESTL(AssertRestart()); // this is the main test
	
	Cleanup();	
	
	INFO_PRINTF1(_L("Test step complete"));		
	INFO_PRINTF1(_L(""));
	}
	
TBool CStepProcRestart::AssertRestart()
	{	
	//Allow the process to be restarted
	INFO_PRINTF1(_L("Going to sleep for KWaitTime + 1 seconds."));		
	User::After(KThrottleTime + 1000000);
	
	//Assert that the server is running
	INFO_PRINTF1(_L("Woke up, now the server should be restarted (sysmon KWaitTime is 15s)."));	
	RTestProcGoodSession server;	
	const TInt err = server.Connect();
	const TBool assert = (KErrNone == err);
	TESTE(assert, err);
	if(assert)
		{
		INFO_PRINTF1(_L("Yes, asserted that server is running."));		
		}
	server.Close();
	return assert;
	}
	
void CStepProcRestart::Cleanup()
	{
	RTestProcGoodSession server;	
	const TInt err = server.Connect();
	TESTE(KErrNone == err, err);
	if(KErrNone == err)
		{
		INFO_PRINTF1(_L("Going to cancel monitor and shutdown server."));		
		}
	server.CancelMonitor();
	server.ShutDown();
	server.Close();
	}
