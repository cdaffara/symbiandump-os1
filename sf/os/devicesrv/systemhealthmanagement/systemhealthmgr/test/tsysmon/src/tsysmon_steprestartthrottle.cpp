// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "tsysmon_steprestartthrottle.h"


CStepRestartThrottle::CStepRestartThrottle()
	{
	SetTestStepName(KCTestCaseRestartThrottle);
	}
/**
Old Test CaseID 		APPFWK-SYSMON-0004
New Test CaseID 		DEVSRVS-SHMA-SYSMON-0004
 */

TVerdict CStepRestartThrottle::doTestStepL()
	{
	TInt err = iProcStartSignalSem.CreateGlobal(KStartShmaTestProcGood,0);
	INFO_PRINTF2(_L("Semaphore created with err %d"), err);
	TEST(KErrNone == err);
		
	INFO_PRINTF1(_L("TEST APPFWK-SYSMON-0004"));
	
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
	DoTestMonitor();
	
	INFO_PRINTF1(_L("Test System Monitor functionality using ssmstartupproperties"));
	CleanupClosePushL(process);
	StartAndMonitorUsingSsmStartupPropL(process, EWaitForStart);

	iProcStartSignalSem.Wait();
	//Killing the process will cause sysmon to restart after KWaitTime (declared in sysmon_patch.h)
	process.Kill(KErrNone);
	CleanupStack::PopAndDestroy(&process);
	INFO_PRINTF1(_L("Killed monitored process."));
	DoTestMonitor();
	
	INFO_PRINTF1(_L("Test complete"));	
	iProcStartSignalSem.Close();
	return TestStepResult();	
	}

void CStepRestartThrottle::DoTestMonitor()
	{
    TInt millisecs1 =0;
    TDateTime datetime;
    TTime time;
    time.HomeTime();
    datetime = time.DateTime();
    millisecs1 = (datetime.MicroSecond() / 1000) + 
                (datetime.Second() * 1000) + 
                (datetime.Minute() * 60 * 1000) +
                (datetime.Hour() * 60 * 60 * 1000);
                
	//Sleep 14 seconds and then assert that the server is not yet restarted
	INFO_PRINTF1(_L("Going to sleep for 14 seconds."));		
	User::After(KThrottleTime - 1000000);
	RTestProcGoodSession server;
	TInt err = server.Connect();
	TESTE(KErrNotFound == err, err);
	server.Close();
	if(KErrNotFound == err)
		{
		INFO_PRINTF1(_L("Asserted that server is not yet restarted."));		
		}
	else
		{
		ERR_PRINTF1(_L("Error - the restart took place too soon."));		
		}
		
	//Allow the process to be restarted
	INFO_PRINTF1(_L("Waiting till the process is restarted using semaphore."));		
	//Wait for the process to be restarted 
	iProcStartSignalSem.Wait();
	
    time.HomeTime();
    datetime = time.DateTime();
    TInt millisecs2 = 0;
    millisecs2 = (datetime.MicroSecond() / 1000) + 
                (datetime.Second() * 1000) + 
                (datetime.Minute() * 60 * 1000) +
                (datetime.Hour() * 60 * 60 * 1000);
		
	//Assert that the server is running
	INFO_PRINTF1(_L("Woke up, now the server should be restarted (sysmon KWaitTime is 15s)."));		
	err = server.Connect();
	TESTE(KErrNone == err, err);
	INFO_PRINTF2(_L("Server restarted after %d ms"), (millisecs2 - millisecs1));
	//Testing whether the server restarted after the throttle time, which is 15s
	TEST((millisecs2 - millisecs1) >= 15000);
	if(KErrNone == err)
		{
		INFO_PRINTF1(_L("Yes, asserted that server is running, going to cancel monitoring shutdown server"));		
		}
	server.CancelMonitor();
	server.ShutDown();
	server.Close();
	}
