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

#include "tsysmon_server.h"

#include "tsysmon_stepmultiprocmon.h"
#include "tsysmon_stepnoprocmon.h"
#include "tsysmon_stepprocmonignore.h"
#include "tsysmon_stepselfmonignore.h"
#include "tsysmon_stepprocmoninit.h"
#include "tsysmon_stepprocrestart.h"
#include "tsysmon_steprestartthrottle.h"
#include "tsysmon_stepselfmoncancel.h"
#include "tsysmon_stepselfmoncancel2.h"
#include "tsysmon_stepnomoncancel.h"

#include "tsysmon_step_connect_disconnect.h"
#include "tsysmon_step_register.h"
#include "tsysmon_step_register_twice_different.h"
#include "tsysmon_step_register_twice_same.h"
#include "tsysmon_step_result_check.h"
#include "tsysmon_step_run_count_check.h"
#include "tsysmon_steploadsysmon.h"
#include "tsysmon_stepoomsysmon.h"
#include "tsysmon_stepcancelallmonitors.h"
#include "tsysmon_steposrestarttest.h"

CSysMonTestServer* CSysMonTestServer::NewLC()
	{
	CSysMonTestServer * server = new (ELeave) CSysMonTestServer();
	CleanupStack::PushL(server);
	TParsePtrC serverName(RProcess().FileName());
	server->StartL(serverName.Name());
	return server;
	}
	

static void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	// Create the CTestServer derived server
	CSysMonTestServer* server = CSysMonTestServer::NewLC();
	RProcess::Rendezvous(KErrNone);
	sched->Start();
	
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(sched);
	}


TInt E32Main()
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt err = KErrNoMemory;
	if(cleanup)
		{
		TRAP(err, MainL());
		delete cleanup;
		}
		
	__UHEAP_MARKEND; 
	return err;
    } //lint -e765 -e714 Suppress 'not referenced' and 'could be static'


/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CSysMonTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KCTestCaseMultiProcMon)
		{
		testStep = new CStepMultiProcMon();
		}
	else if (aStepName == KCTestCaseNoProcMon)
		{
		testStep = new CStepNoProcMon();
		}
	else if (aStepName == KCTestCaseProcMonIgnore)
		{
		testStep = new CStepProcMonIgnore();
		}
	else if (aStepName == KCTestCaseProcMonInit)
		{
		testStep = new CStepProcMonInit();
		}
	else if (aStepName == KCTestCaseProcRestart)
		{
		testStep = new CStepProcRestart();
		}
	else if (aStepName == KCTestCaseRestartThrottle)
		{
		testStep = new CStepRestartThrottle();
		}
	else if (aStepName == KCTestCaseSelfMonCancel)
		{
		testStep = new CStepSelfMonCancel();
		}
	else if (aStepName == KCTestCaseSelfMonIgnore)
		{
		testStep = new CStepSelfMonIgnore();
		}
	else if (aStepName == KCTestCaseSelfMonCancel2)
		{
		testStep = new CStepSelfMonCancel2();
		}
	else if (aStepName == KCTestCaseNoMonCancel)
		{
		testStep = new CStepNoMonCancel();
		}
	else if(aStepName == KConnectDisconnectStep)
		{
        testStep = new CConnectDisconnectStep();
		}
    else if(aStepName == KRegisterStep)
		{
        testStep = new CRegisterStep();
		}
    else if(aStepName == KRegisterTwiceDifferentStep)
		{
        testStep = new CRegisterTwiceDifferentStep();
		}
    else if(aStepName == KRegisterTwiceSameStep)
		{
        testStep = new CRegisterTwiceSameStep();
		}
    else if(aStepName == KResultCheckStep)
		{
        testStep = new CResultCheckStep();
		}
    else if(aStepName == KRunCountCheckStep)
		{
		testStep = new CRunCountCheckStep();
		}
    else if(aStepName == KCTestCaseLoadSysMon)
		{
		testStep = new CStepLoadSysMon();
		}
    else if(aStepName == KOOMTestCaseSysMon)
		{
		testStep = new CTOOMSysMonTestStep();
		}
    else if(aStepName == KCTestCaseCancelAllMonitors)
        {
        testStep = new CStepCancelAllMonitors();
        }
    else if(aStepName == KCTestCaseCriticalNoRetriesStep)
        {
        testStep = new CStepOSRestart(KCTestCaseCriticalNoRetries);
        }
    else if(aStepName == KCTestCaseRestartOSStep)
        {
        testStep = new CStepOSRestart(KCTestCaseRestartOS);
        }
	else 
		{
		RDebug::Printf("Unknown sysmon teststep %S", &aStepName);	
		}
		
	return testStep;
	}
	
