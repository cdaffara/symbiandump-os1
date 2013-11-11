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

#include "tss_server.h"
#include "tss_errors.h"
#include "tss_appstart.h"
#include "tss_procstart.h"
#include "tss_apptimeout.h"
#include "tss_procstartmon.h"
#include "tss_startandcancel.h"
#include "tss_appstartrverror.h"
#include "tss_appstartmultiple.h"
#include "tss_appstartapparcinit.h"
#include "tss_procstartwithfandf.h"

_LIT(KServerName, "tss_server");


CSsTestServer* CSsTestServer::NewL()
	{
	CSsTestServer * server = new (ELeave) CSsTestServer();
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
	CSsTestServer*	server = CSsTestServer::NewL();
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
    }



/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CSsTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KCTestCaseProcStart)
		{
		testStep = new CSsTestStepProcStart();
		}
	else if (aStepName == KCTestCaseAppStart)
		{
		testStep = new CSsTestStepAppStart();
		}
	else if (aStepName == KCTestCaseProcStartMon)
		{
		testStep = new CSsTestStepProcStartMon();
		}
	else if (aStepName == KCTestCaseAppTimeout)
		{
		testStep = new CSsTestStepAppTimeout();
		}
	else if (aStepName == KCTestCaseStartAndCancel)
		{
		testStep = new CSsTestStepStartAndCancel();
		}
	else if (aStepName == KCTestCaseAppStartMultiple)
		{
		testStep = new CSsTestStepAppStartMultiple();
		}
	else if (aStepName == KCTestCaseAppStartAppArcInit)
		{
		testStep = new CSsTestStepAppStartAppArcInit();
		}	
	else if(  aStepName == KCTestCaseAppStartRvError )
		{
		testStep = new CSsTestStepAppStartRvError();
		}
	else if( aStepName == KStartSafeErrors )
		{
		testStep = new CSsTestStepErrors();
		}
	else if (aStepName == KCTestCaseProcStartWithFAndF)
		{
		testStep = new CSsTestStepProcStartWithFAndF();
		}	
	else
		{
		RDebug::Printf("Unknown SsmStartSafe teststep %S", &aStepName);
		}
		 
		 
	return testStep;
	}
	
