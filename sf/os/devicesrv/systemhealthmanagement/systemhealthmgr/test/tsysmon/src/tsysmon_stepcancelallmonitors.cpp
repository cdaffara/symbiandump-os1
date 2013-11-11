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
// Description: Test for testing sysmon's CancelAllMonitor functionality
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <sysmonclisess.h>
#include <ssm/ssmstartupproperties.h>
#include "testprocgoodsession.h"
#include "tsysmon_stepcancelallmonitors.h"

CStepCancelAllMonitors::CStepCancelAllMonitors()
	{
	SetTestStepName(KCTestCaseCancelAllMonitors);
	}

CStepCancelAllMonitors::~CStepCancelAllMonitors()
    {
    iProcStartSignalSem.Close();
    iAppStartSignalSem.Close();
    }

TVerdict CStepCancelAllMonitors::doTestStepL()
	{
	INFO_PRINTF1(_L("Starting doTestStepL"));
	
    TInt err = iProcStartSignalSem.CreateGlobal(KStartShmaTestProcGood,0);
    INFO_PRINTF2(_L("Semaphore for process created with err %d"), err);
    TEST(KErrNone == err);
    
    err = iAppStartSignalSem.CreateGlobal(KStartShmaTestAppGood,0);
    INFO_PRINTF2(_L("Semaphore for application created with err %d"), err);
    TEST(KErrNone == err);

    _LIT_SECURITY_POLICY_PASS(KTestReadPolicy);
    _LIT_SECURITY_POLICY_PASS(KTestWritePolicy);  
    
    err = RProperty::Define(KCancelMonitorProperty, KCancelMonitorKey, RProperty::EInt, KTestReadPolicy, KTestWritePolicy);
    INFO_PRINTF2(_L("RProperty Define completed with %d"), err);
    TEST(KErrNone == err || KErrAlreadyExists == err);
    err = RProperty::Set(KCancelMonitorProperty, KCancelMonitorKey, 0);
    INFO_PRINTF2(_L("RProperty initial set completed with %d"), err);
    TEST(KErrNone == err);

    TRAP(err, DoTestCancelAllMonitorsL());
    INFO_PRINTF2(_L("Test completed with %d"), err);
    
	return TestStepResult();	
	}

void CStepCancelAllMonitors::DoTestCancelAllMonitorsL()
	{
    INFO_PRINTF1(_L("Open session with Sysmon Server"));
	RSysMonSession sysmonSess;
	CleanupClosePushL(sysmonSess);
	sysmonSess.OpenL();
 
	RProcess process;
    CleanupClosePushL(process);
    INFO_PRINTF1(_L("Going to start first process"));
    StartAndMonitorUsingSsmStartupPropL(process, EWaitForStart);

    // Launch native application using an unconnected RApaLsSession
    INFO_PRINTF1(_L("Going to set a native application running"));
    TThreadId threadId;
    TRequestStatus requestStatus;
    StartViewlessBgApplicationL(KTestAppGood, threadId, requestStatus);
    
    // Test that the application was created, then wait for it to resume    
    RThread thread;
    RProcess process1;
    CleanupClosePushL(thread);
    CleanupClosePushL(process1);
    TEST(KErrNone == thread.Open(threadId));
    TEST(KErrNone == thread.Process(process1)); 
    User::WaitForRequest(requestStatus);
    TEST(KErrNone == requestStatus.Int());
    
    INFO_PRINTF1(_L("Going to request app monitoring using ssmstartupproperties"));
    CSsmStartupProperties* ssmProp = CSsmStartupProperties::NewLC(KTestAppGood, KNullDesC);
    ssmProp->SetExecutionBehaviour(ESsmWaitForSignal);
    TSsmMonitorInfo monitorInfo;
    monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
    monitorInfo.iRestartMode = 0;
    monitorInfo.iTimeout = 0;
    monitorInfo.iRetries = 1;
    ssmProp->SetMonitorInfoL(monitorInfo);

    TRAPD(err, sysmonSess.MonitorL(*ssmProp, process1)); 
    TESTEL(err == KErrNone, err);
    
	INFO_PRINTF1(_L("Monitoring is now setup, we will now make a request to cancel all monitors"));
    err = sysmonSess.CancelAllMonitors();
    INFO_PRINTF2(_L("CancelAllMonitors completed with %d"), err);
    
    //Since this test is run once with SSM SID and again without SSM SID, testing for both the conditions
    TEST(err == KErrNone || err == KErrPermissionDenied);

    INFO_PRINTF1(_L("Waiting on semphore just to make sure the both the processes are started"));
    //Wait for the process to restart
    iProcStartSignalSem.Wait();
    //Wait for the application to be restarted
    iAppStartSignalSem.Wait();

    TEST(Exists(KTestProcGood) && Exists(KTestAppGood));
    process.Kill(KErrNone);
    process1.Kill(KErrNone);
    TEST(EFalse == Exists(KTestProcGood) && EFalse == Exists(KTestAppGood));

    INFO_PRINTF1(_L("Killed monitored process."));

    if (err == KErrNone)
        {
        // If the process is running with SSM SID then the monitoring is cancelled and processes will not be
        // restarted
        INFO_PRINTF1(_L("Going to sleep for 16 seconds."));     
        User::After(KThrottleTime + 1000000);

        INFO_PRINTF1(_L("Check that the process wasn't restarted"));
        TEST(EFalse == Exists(KTestProcGood) && EFalse == Exists(KTestAppGood));
        }
    else if (err == KErrPermissionDenied)
        {
        // If the process is running without SSM SID then the monitoring is not cancelled and monitoring have to 
        // cancelled by processes itself and then they should be killed

        //Allow the process to be restarted
        INFO_PRINTF1(_L("Going to sleep using Semaphore."));
        //Wait for the process to restart
        iProcStartSignalSem.Wait();
        //Wait for the application to be restarted
        iAppStartSignalSem.Wait();
        INFO_PRINTF1(_L("Check that the processes are restarted"));
        TEST(Exists(KTestProcGood) && Exists(KTestAppGood));
        
        // For process
        //Assert that the server is running
        INFO_PRINTF1(_L("Woke up, now the server should be restarted (sysmon KWaitTime is 15s).")); 
        RTestProcGoodSession server;    
        err = server.Connect();
        const TBool assert = (KErrNone == err);
        INFO_PRINTF2(_L("Connect to RTestProcGoodSession server completed with %d"), err);
        TESTE(assert, err);
        if(assert)
            {
            INFO_PRINTF1(_L("Yes, asserted that server is running."));      
            }
        err = server.CancelMonitor();
        INFO_PRINTF2(_L("Cancel Monitor completed with %d"), err);
        err = server.ShutDown();
        INFO_PRINTF2(_L("Process killed with %d"), err);
        TEST(EFalse == Exists(KTestProcGood));
        server.Close();
        
        //For Application
        err = RProperty::Set(KCancelMonitorProperty, KCancelMonitorKey, 0);
        TEST(KErrNone == err);
        RDebug::Print(_L("RProperty set completed with %d"), err);
        
        //Wait for the process to cancel the monitoring and exit
        iAppStartSignalSem.Wait();

        INFO_PRINTF2(_L("Process killed with %d"), err);
        User::After(2000000);
        TEST(EFalse == Exists(KTestAppGood));
        }
    
    CleanupStack::PopAndDestroy(5, &sysmonSess);
	}


