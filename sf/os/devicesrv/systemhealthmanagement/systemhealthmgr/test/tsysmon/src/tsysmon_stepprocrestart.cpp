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
	TInt err = iProcStartSignalSem.CreateGlobal(KStartShmaTestProcGood,0);
	INFO_PRINTF2(_L("Semaphore created with err %d"), err);
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0003"));
	TRAP(err, DoTestProcRestartL());
	TEST(err == KErrNone);
	
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0003b"));	
	TRAP(err, DoTestExtendedProcRestartL());
	TEST(err == KErrNone);

	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0055"));
	TRAP(err, DoTestProcRestartAferRateOfFailureL());
	TEST(err == KErrNone);

	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0056"));
	TRAP(err, DoTestProcRestartForRateOfFailureL());
	TEST(err == KErrNone);
	
	iProcStartSignalSem.Close();

	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0003
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0003
*/

void CStepProcRestart::DoTestProcRestartL()
	{
	INFO_PRINTF1(_L("New Test step"));
	INFO_PRINTF1(_L("Test System Monitor functionality using startupproperties"));
	RProcess process;
	
	CleanupClosePushL(process);
	StartAndMonitorUsingStartupPropL(process, EWaitForStart);

	iProcStartSignalSem.Wait();
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::Pop(&process);
	INFO_PRINTF1(_L("Killed monitored process."));		
	TESTL(AssertRestart()); // this is the main test

	Cleanup();
	
	INFO_PRINTF1(_L("Test System Monitor functionality using ssmstartupproperties"));
	CleanupClosePushL(process);
	StartAndMonitorUsingSsmStartupPropL(process, EWaitForStart);
	
	iProcStartSignalSem.Wait();
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
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0003
*/


void CStepProcRestart::DoTestExtendedProcRestartL()
	{
	INFO_PRINTF1(_L("New Test step"));
	INFO_PRINTF1(_L("Test System Monitor functionality using startupproperties"));
	RProcess process;
	CleanupClosePushL(process);
	StartAndMonitorUsingStartupPropL(process, EWaitForStart);
	
	iProcStartSignalSem.Wait();
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::Pop(&process);
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
	
	INFO_PRINTF1(_L("Test System Monitor functionality using ssmstartupproperties"));
	CleanupClosePushL(process);
	StartAndMonitorUsingSsmStartupPropL(process, EWaitForStart);
	
	iProcStartSignalSem.Wait();
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	INFO_PRINTF1(_L("Killed monitored process."));		
	TESTL(AssertRestart()); 
	
	//Now do the same test for the restarted process, can't use the old processhandle
	TEST(KErrNone == server.Connect());
	server.ShutDown();
	server.Close();
	INFO_PRINTF1(_L("Killed the restarted process."));		
	TESTL(AssertRestart()); // this is the main test
	
	Cleanup();	

	INFO_PRINTF1(_L("Test step complete"));		
	INFO_PRINTF1(_L(""));
	}

/**
Old Test CaseID 		APPFWK-SYSMON-0055
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0003
*/

void CStepProcRestart::DoTestProcRestartAferRateOfFailureL()
	{
	TRAPD(err, DoTestProcRestartAferRateOfFailureL(EWaitForStart));
	TEST(err == KErrNone);
	
	TRAP(err, DoTestProcRestartAferRateOfFailureL(EFireAndForget));
	TEST(err == KErrNone);
	}

void CStepProcRestart::DoTestProcRestartAferRateOfFailureL(TStartMethod aStartMethod)
	{
	INFO_PRINTF1(_L("New Test step"));
	
	// The KIntervalForReLaunchRateOfFailure time interval is 30mins by default, its patched to 25secs on emulator in epoc.ini file
	// and for hardware in iby file.
	INFO_PRINTF1(_L("For test to pass in Winscw - (patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure 25) should be added to epoc32\\data\\epoc.ini file"));
	RProcess process;
	CleanupClosePushL(process);
	StartAndMonitorUsingStartupPropL(process, aStartMethod);
	
	iProcStartSignalSem.Wait();
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

	//Now do the same test for the restarted process, can't use the old processhandle
	TEST(KErrNone == server.Connect());
	server.ShutDown();
	server.Close();
	INFO_PRINTF1(_L("Killed the restarted process. - 2"));
	TESTL(AssertRestart()); // this is the main test

	//Now do the same test for the restarted process, can't use the old processhandle
	TEST(KErrNone == server.Connect());
	server.ShutDown();
	server.Close();
	INFO_PRINTF1(_L("Killed the restarted process. - 3"));

	//Allow the process to be restarted
	INFO_PRINTF1(_L("Going to sleep for KWaitTime + 1 seconds."));		
	User::After(KThrottleTime + 1000000);
	
	//Assert that the server is not running
	const TInt err = server.Connect();
	TEST(KErrNotFound == err);
	if(err)
		{
		INFO_PRINTF1(_L("Asserted that server is not running - Monitor's Rate of Failure policy would be applied"));
		INFO_PRINTF1(_L("System Moniter is cancelled as the failure recovery policy is Ignore on failure"));
		}
	}



/**
Old Test CaseID 		APPFWK-SYSMON-0056
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0003
*/


void CStepProcRestart::DoTestProcRestartForRateOfFailureL()
	{
	TRAPD(err, DoTestProcRestartForRateOfFailureL(EWaitForStart));
	TEST(err == KErrNone);
	
	TRAP(err, DoTestProcRestartForRateOfFailureL(EFireAndForget));
	TEST(err == KErrNone);
	}
	
void CStepProcRestart::DoTestProcRestartForRateOfFailureL(TStartMethod aStartMethod)
	{
	INFO_PRINTF1(_L("New Test step"));

	// The KIntervalForReLaunchRateOfFailure time interval is 30mins by default, its patched to 25secs on emulator in epoc.ini file
	// and for hardware in iby file.
	INFO_PRINTF1(_L("For test to pass in Winscw - (patchdata_sysmon_exe_KIntervalForReLaunchRateOfFailure 25) should be added to epoc32\\data\\epoc.ini file"));
	RProcess process;
	CleanupClosePushL(process);
	StartAndMonitorUsingStartupPropL(process, aStartMethod);
	
	iProcStartSignalSem.Wait();
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	INFO_PRINTF1(_L("Killed monitored process - 1"));		
	TESTL(AssertRestart()); 
	
	//Now do the same test for the restarted process, can't use the old processhandle
	RTestProcGoodSession server;	
	TEST(KErrNone == server.Connect());
	server.ShutDown();
	server.Close();
	INFO_PRINTF1(_L("Killed the restarted process. - 2"));
	TESTL(AssertRestart()); // this is the main test

	//Now do the same test for the restarted process, can't use the old processhandle
	TEST(KErrNone == server.Connect());
	INFO_PRINTF1(_L("Going to sleep for KReLaunchRateofFailure - 25secs."));
	User::After(25000000);

	//Now do the same test for the restarted process, can't use the old processhandle
	server.ShutDown();
	server.Close();

	//Assert that the server is not running
	TInt err = server.Connect();
	TEST(KErrNotFound == err);
	INFO_PRINTF2(_L("Connect to server with %d"),err);

	INFO_PRINTF1(_L("Killed the restarted process after KIntervalForReLaunchRateOfFailure - 3"));
	
	//Assert that the server is running
	TESTL(AssertRestart());

	Cleanup();
	INFO_PRINTF1(_L("Test step complete"));	
	}

TBool CStepProcRestart::AssertRestart()
	{	
	//Allow the process to be restarted
	INFO_PRINTF1(_L("Going to sleep using Semaphore."));		

	//Wait for the process to restart
	iProcStartSignalSem.Wait();
	
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
