
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

#include "tstartsafe_test_server.h"

#include "tstartsafe_appasynchstart.h"
#include "tstartsafe_appretry.h"
#include "tstartsafe_appstart.h"
#include "tstartsafe_appstartapparcinit.h"
#include "tstartsafe_apptimeout.h"
#include "tstartsafe_apptimesucc.h"
#include "tstartsafe_procasynchstart.h"
#include "tstartsafe_procfailsysrestart.h"
#include "tstartsafe_procmonsysrestart.h"
#include "tstartsafe_procretry.h"
#include "tstartsafe_procstart.h"
#include "tstartsafe_procstartmon.h"
#include "tstartsafe_proctimeout.h"
#include "tstartsafe_proctimesucc.h"
#include "tstartsafe_errors.h"
#include "tstartsafe_test_server.h"
#include "tstartsafe_procstartrverror.h"
#include "tstartsafe_appstartrverror.h"


_LIT(KServerName, "tstartsafe_test_server");

CAppFwkStartSafeTestServer* CAppFwkStartSafeTestServer::NewL()
	{
	CAppFwkStartSafeTestServer * server = new (ELeave) CAppFwkStartSafeTestServer();
	CleanupStack::PushL(server);
	
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	

static void MainL()
	{
	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// Create the CTestServer derived server, sync with the client and enter the active scheduler
	CAppFwkStartSafeTestServer*	server = CAppFwkStartSafeTestServer::NewL();
	RProcess::Rendezvous(KErrNone);
	sched->Start();

	delete server;
	CleanupStack::PopAndDestroy(sched);
	}


TInt E32Main()
	{
	__UHEAP_MARK;
	
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
		
	TRAPD( err, MainL() );
	
	if( KErrNone != err )
		{
		RDebug::Print( _L("%S MainL - Error: %d"), &KServerName(),err );
	   	User::Panic( KServerName, err );
		}
			
	delete cleanup;
	
	__UHEAP_MARKEND;
	
	return KErrNone;
    }//lint -e765 -e714 Suppress 'not referenced' and 'could be static'



/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CAppFwkStartSafeTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KCTestCaseAppAsynchStart)
		{
		testStep = new CAppFwkStartSafeTestStepAppAsynchStart();
		}
	else if (aStepName == KCTestCaseAppRetry)
		{
		testStep = new CAppFwkStartSafeTestStepAppRetry();
		}
	else if (aStepName == KCTestCaseProcStart)
		{
		testStep = new CAppFwkStartSafeTestStepProcStart();
		}
	else if (aStepName == KCTestCaseAppStart)
		{
		testStep = new CAppFwkStartSafeTestStepAppStart();
		}
	else if (aStepName == KCTestCaseAppStartAppArcInit)
		{
		testStep = new CAppFwkStartSafeTestStepAppStartAppArcInit();
		}
	else if (aStepName == KCTestCaseAppTimeout)
		{
		testStep = new CAppFwkStartSafeTestStepAppTimeout();
		}
	else if (aStepName == KCTestCaseAppTimeSucc)
		{
		testStep = new CAppFwkStartSafeTestStepAppTimeSucc();
		}
	else if (aStepName == KCTestCaseProcAsynchStart)
		{
		testStep = new CAppFwkStartSafeTestStepProcAsynchStart();
		}
#ifndef __WINSCW__	// RestartSys is not supported on the emulator
	else if (aStepName == KCTestCaseProcFailSysRestart)
		{
		testStep = new CAppFwkStartSafeTestStepProcFailSysRestart();
		}
	else if (aStepName == KCTestCaseProcMonSysRestart)
		{
		testStep = new CAppFwkStartSafeTestStepProcMonSysRestart();
		}
#endif
	else if (aStepName == KCTestCaseProcRetry)
		{
		testStep = new CAppFwkStartSafeTestStepProcRetry();
		}
	else if (aStepName == KCTestCaseProcStart)
		{
		testStep = new CAppFwkStartSafeTestStepProcStart();
		}
	else if (aStepName == KCTestCaseProcStartMon)
		{
		testStep = new CAppFwkStartSafeTestStepProcStartMon();
		}
	else if (aStepName == KCTestCaseProcTimeout)
		{
		testStep = new CAppFwkStartSafeTestStepProcTimeout();
		}
	else if (aStepName == KCTestCaseProcTimeSucc)
		{
		testStep = new CAppFwkStartSafeTestStepProcTimeSucc();
		}	
	else if( aStepName == KStartSafeErrors )
		{
		testStep = new CStartSafeTestStepErrors();
		}
	else if(  aStepName == KCTestCaseAppStartRvError )
		{
		testStep = new CAppFwkStartSafeTestStepAppStartRvError();
		}
	else if( aStepName == KCTestCaseProcStartRvError )
		{
		testStep = new CAppFwkStartSafeTestStepProcStartRvError();
		}
	else
		{
		RDebug::Printf("Unknown startsafe teststep %S", &aStepName);
		}
		 
		 
	return testStep;
	}
	
