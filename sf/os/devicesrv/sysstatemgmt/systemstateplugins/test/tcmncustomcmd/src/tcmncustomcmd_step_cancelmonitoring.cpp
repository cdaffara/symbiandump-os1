// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tcmncustomcmd_step_cancelmonitoring.h"
#include "ssmcmncustomcmdfactory.h"
#include "cmdcancelmonitoring.h"
#include "gtestapps.h"
#include "gsatestprocgoodsession.h"
#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmstartupproperties.h>

CCustomCmdTestCancelMonitoring::~CCustomCmdTestCancelMonitoring()
	{
    iProcStartSignalSem.Close();
	}

CCustomCmdTestCancelMonitoring::CCustomCmdTestCancelMonitoring()
	{
	SetTestStepName(KTCCustomCmdTestCancelMonitoring);
	}

/**
Old Test CaseID 		APPFWK-CUSTCMD-0001
New Test CaseID 		DEVSRVS-SSPLUGINS-CUSTCMD-0001
 */

void CCustomCmdTestCancelMonitoring::TestCustomCmdCancelMonitoringL()
	{
	INFO_PRINTF1(_L("TestCustomCmdPersistRebootsL"));

	//Create Persist Reboots custom command
	MSsmCustomCommand* customCmdCancelMonitoring = SsmCmnCustomCmdFactory::CmdCancelMonitoringL();
	RFs aFs;
	aFs.Connect();
	CSsmCustomCommandEnv* cmdEnv = CSsmCustomCommandEnv::NewL(aFs);
	CleanupStack::PushL(cmdEnv);
	
	//Initialise the command.
	customCmdCancelMonitoring->Initialize(cmdEnv);

	// Creates two processes and sets up monitoring for them
    RProcess process;
    CleanupClosePushL(process);
    CreateProcessAndMonitorL(process, KTestProcGood, KLaunchServerCommandLineOption);

    RProcess process1;
    CleanupClosePushL(process1);
    CreateProcessAndMonitorL(process1, KTestAppGood, KLaunchProcessAndWait);
	
	TRequestStatus status;
	customCmdCancelMonitoring->Execute(KNullDesC8, status);
	User::WaitForRequest(status);
	TEST(KErrNone == status.Int());
	INFO_PRINTF2(_L("Execute completed with %d"), status.Int());
	
	customCmdCancelMonitoring->Execute(KNullDesC8, status);
    customCmdCancelMonitoring->ExecuteCancel();
    
    //Reference plugins doesnt have Cancel implementation and also by the time ExecuteCancel()
    //is called it might have completed executing Execute() function. Thats is the reason for 
    //checking the status to KErrNone instead of KErrCancel.
    User::WaitForRequest(status);
    TEST(KErrNone == status.Int());
	
    // Tests whether the monitoring the both the processes is cancelled
    DoTestCancelMonitoringL(process, process1);

	customCmdCancelMonitoring->Close();
	customCmdCancelMonitoring->Release();
	CleanupStack::PopAndDestroy(3, cmdEnv);
	INFO_PRINTF1(_L("Ending the CCustomCmdTestCancelMonitoring tests "));

	}

void CCustomCmdTestCancelMonitoring::CreateProcessAndMonitorL(RProcess& aProcess, const TDesC& aProcName, const TDesC& aArgs)
    {
    INFO_PRINTF2(_L("Going to start %S process"), &aProcName);
    User::LeaveIfError(aProcess.Create(aProcName, aArgs));
    TRequestStatus status;
    aProcess.Rendezvous(status);
    if (status == KRequestPending)
        {
        aProcess.Resume();
        
        User::WaitForRequest(status);
        TEST(KErrNone == status.Int());
        INFO_PRINTF3(_L("Process %S Rendezvoused with %d"), &aProcName, status.Int());
        
        INFO_PRINTF2(_L("Waiting for %S to be started"), &aProcName);
        iProcStartSignalSem.Wait();
        
        INFO_PRINTF1(_L("Open session with Sysmon Server"));
        RSysMonSession sysmonSess;
        CleanupClosePushL(sysmonSess);
        sysmonSess.OpenL();
     
        //Setup monitoring  
        INFO_PRINTF1(_L("Going to request process monitoring"));
        CSsmStartupProperties* prop = CSsmStartupProperties::NewLC(aProcName, aArgs);
        prop->SetExecutionBehaviour(static_cast <TSsmExecutionBehaviour> (ESsmWaitForSignal));
        TSsmMonitorInfo monitorInfo;
        monitorInfo.iRestartPolicy = ESsmIgnoreOnFailure;
        monitorInfo.iRestartMode = 0;
        monitorInfo.iTimeout = 0;
        monitorInfo.iRetries = 1;
        prop->SetMonitorInfoL(monitorInfo);
        TRAPD(err, sysmonSess.MonitorL(*prop, aProcess));
        INFO_PRINTF2(_L("Process monitoring set up with %d"), err);
        TEST(err == KErrNone); 
        CleanupStack::PopAndDestroy(2, &sysmonSess);
        }
    else
        {
        aProcess.Kill(KErrGeneral);
        TESTEL(EFalse, status.Int());
        }
    }

void CCustomCmdTestCancelMonitoring::DoTestCancelMonitoringL(RProcess& aProcess1, RProcess& aProcess2)
    {
    TEST(Exists(KTestProcGood) && Exists(KTestAppGood));
    aProcess1.Kill(KErrNone);
    aProcess2.Kill(KErrNone);
    TEST(EFalse == Exists(KTestProcGood) && EFalse == Exists(KTestAppGood));

    INFO_PRINTF1(_L("Killed monitored process."));

    // If the process is running with SSM SID then the monitoring is cancelled and processes will not be
    // restarted
    INFO_PRINTF1(_L("Going to sleep for 16 seconds."));     
    User::After(1600000);

    INFO_PRINTF1(_L("Check that the process wasn't restarted"));
    TEST(EFalse == Exists(KTestProcGood) && EFalse == Exists(KTestAppGood));
    }

/**
Asserts that at least one process starting with the name aProcessName is running.
*/
TBool CCustomCmdTestCancelMonitoring::Exists(const TDesC& aProcessName)
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

TVerdict CCustomCmdTestCancelMonitoring::doTestStepL()
	{
	__UHEAP_MARK;
	
    INFO_PRINTF1(_L("Starting doTestStepL"));
    
    TInt err = iProcStartSignalSem.CreateGlobal(KStartTestProcGoodSem,0);
    INFO_PRINTF2(_L("Semaphore for process created with err %d"), err);
    TEST(KErrNone == err);
    
 	TRAP(err, TestCustomCmdCancelMonitoringL());
	TEST(KErrNone == err);
	__UHEAP_MARKEND;
	return TestStepResult();
	}
