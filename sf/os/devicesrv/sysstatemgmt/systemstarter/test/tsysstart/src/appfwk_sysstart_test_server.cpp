// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "appfwk_sysstart_test_server.h"
#include <ecom/ecom.h>

// START_APP_INFO test cases
#include "appfwk_sysstart_test_step_AppFailRetry0Ignore.h"
#include "appfwk_sysstart_test_step_AppFailRetry1Ignore.h"
#include "appfwk_sysstart_test_step_AppFailRetry0Panic.h"
#include "appfwk_sysstart_test_step_AppFailRetry1Panic.h"
#include "appfwk_sysstart_test_step_AppStartCd.h"
#include "appfwk_sysstart_test_step_AppStartCs.h"
#include "appfwk_sysstart_test_step_AppStartFailPanic.h"
#include "appfwk_sysstart_test_step_AppStartNc.h"
#include "appfwk_sysstart_test_step_AppTimeoutRetry3Ignore.h"
#include "appfwk_sysstart_test_step_AppTimeoutRetry3Panic.h"

// EDeferredWaitForStart test cases
#include "appfwk_sysstart_test_step_DefInvalidAppRetry.h"
#include "appfwk_sysstart_test_step_DefInvalidAppTimeout.h"
#include "appfwk_sysstart_test_step_DefInvalidMultFail.h"
#include "appfwk_sysstart_test_step_DefInvalidMultWait.h"
#include "appfwk_sysstart_test_step_DefInvalidProcRetry.h"
#include "appfwk_sysstart_test_step_DefInvalidProcTimeout.h"
#include "appfwk_sysstart_test_step_DefMultGrp.h"
#include "appfwk_sysstart_test_step_DefNoMult1.h"
#include "appfwk_sysstart_test_step_DefNoMult2.h"
#include "appfwk_sysstart_test_step_DefNoMult3.h"
#include "appfwk_sysstart_test_step_DefRendFailAppPanic.h"
#include "appfwk_sysstart_test_step_DefRendFailIgnore.h"
#include "appfwk_sysstart_test_step_DefRendFailProcPanic.h"
#include "appfwk_sysstart_test_step_DefStartAllState.h"
#include "appfwk_sysstart_test_step_DefStartCd.h"
#include "appfwk_sysstart_test_step_DefStartCs.h"
#include "appfwk_sysstart_test_step_DefStartFailAppPanic.h"
#include "appfwk_sysstart_test_step_DefStartFailIgnore.h"
#include "appfwk_sysstart_test_step_DefStartFailProcPanic.h"
#include "appfwk_sysstart_test_step_DefStartNc.h"
#include "appfwk_sysstart_test_step_DefTimeoutAppIgnore.h"
#include "appfwk_sysstart_test_step_DefTimeoutAppPanic.h"
#include "appfwk_sysstart_test_step_DefTimeoutProcIgnore.h"
#include "appfwk_sysstart_test_step_DefTimeoutProcPanic.h"

// START_DLL_INFO test cases
#include "appfwk_sysstart_test_step_DllCdFailIgnore.h"
#include "appfwk_sysstart_test_step_DllCdFailPanic.h"
#include "appfwk_sysstart_test_step_DllCdNormal.h"
#include "appfwk_sysstart_test_step_DllCsFailIgnore.h"
#include "appfwk_sysstart_test_step_DllCsFailPanic.h"
#include "appfwk_sysstart_test_step_DllCsFnInvalidFail.h"
#include "appfwk_sysstart_test_step_DllCsFnMultipleRetry.h"
#include "appfwk_sysstart_test_step_DllCsNormal.h"
#include "appfwk_sysstart_test_step_DllFnSharedData.h"
#include "appfwk_sysstart_test_step_DllInvalidCustomData.h"
#include "appfwk_sysstart_test_step_DllInvalidOrdinal0.h"
#include "appfwk_sysstart_test_step_DllInvalidOrdinalRetry0Ignore.h"
#include "appfwk_sysstart_test_step_DllInvalidOrdinalRetry1Ignore.h"
#include "appfwk_sysstart_test_step_DllInvalidOrdinalRetry0Panic.h"
#include "appfwk_sysstart_test_step_DllInvalidOrdinalRetry1Panic.h"
#include "appfwk_sysstart_test_step_DllInvalidRetry0Ignore.h"
#include "appfwk_sysstart_test_step_DllInvalidRetry1Ignore.h"
#include "appfwk_sysstart_test_step_DllInvalidRetry0Panic.h"
#include "appfwk_sysstart_test_step_DllInvalidRetry1Panic.h"
#include "appfwk_sysstart_test_step_DllNcFailIgnore.h"
#include "appfwk_sysstart_test_step_DllNcFailPanic.h"
#include "appfwk_sysstart_test_step_DllNcNormal.h"

// INIT_APPARC test cases
#include "appfwk_sysstart_test_step_InitApparc.h"
#include "appfwk_sysstart_test_step_InitApparcBeforeServer.h"

// START_PROCESS_INFO test cases
#include "appfwk_sysstart_test_step_ProcFailRetry0Ignore.h"
#include "appfwk_sysstart_test_step_ProcFailRetry1Ignore.h"
#include "appfwk_sysstart_test_step_ProcFailRetry0Panic.h"
#include "appfwk_sysstart_test_step_ProcFailRetry1Panic.h"
#include "appfwk_sysstart_test_step_ProcStartCd.h"
#include "appfwk_sysstart_test_step_ProcStartCs.h"
#include "appfwk_sysstart_test_step_ProcStartFailPanic.h"
#include "appfwk_sysstart_test_step_ProcStartNc.h"
#include "appfwk_sysstart_test_step_ProcTimeoutRetry3Ignore.h"
#include "appfwk_sysstart_test_step_ProcTimeoutRetry3Panic.h"

// SysStart re-running protection test
#include "appfwk_sysstart_test_step_ReRunSysStart.h"

// STFP test cases
#include "appfwk_sysstart_test_step_StfpCdToNcDefault.h"
#include "appfwk_sysstart_test_step_StfpCdToNcRetry0Ignore.h"
#include "appfwk_sysstart_test_step_StfpCdToNcRetry0Panic.h"
#include "appfwk_sysstart_test_step_StfpCdToNcRetry1Ignore.h"
#include "appfwk_sysstart_test_step_StfpCdToNcRetry1Panic.h"
#include "appfwk_sysstart_test_step_StfpCsToCdDefault.h"
#include "appfwk_sysstart_test_step_StfpCsToCdRetry0Ignore.h"
#include "appfwk_sysstart_test_step_StfpCsToCdRetry0Panic.h"
#include "appfwk_sysstart_test_step_StfpCsToCdRetry1Ignore.h"
#include "appfwk_sysstart_test_step_StfpCsToCdRetry1Panic.h"

_LIT(KCAppfwkSysstartTestServerName,"appfwk_sysstart_test_server");

/**
  @return - Instance of the test server
  Called inside the MainL() function to create and start the
  CTestServer derived server.
 */
CAppfwkSysstartTestServer* CAppfwkSysstartTestServer::NewL()
	{
	CAppfwkSysstartTestServer * server = new (ELeave) CAppfwkSysstartTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KCAppfwkSysstartTestServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CAppfwkSysstartTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CAppfwkSysstartTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
  @return - Standard Epoc error code on process exit
  Process entry point. Called by client using RProcess API
  */
GLDEF_C TInt E32Main(void)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	
	if (err)
		{
		RDebug::Print(_L("CAppfwkSysstartTestServer::MainL - Error: %d"), err);
	   	User::Panic(KCAppfwkSysstartTestServerName,err);
		}
		
	delete cleanup;
	REComSession::FinalClose();	
	__UHEAP_MARKEND;
	return KErrNone;
    }

/**
  @return - A CTestStep derived instance
  @param - aStepName the test case name to be executed
  Implementation of CTestServer pure virtual
 */
CTestStep* CAppfwkSysstartTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KCAppfwkSysStartTestStepAppFailRetry0Ignore) 		// START_APP_INFO test cases
		{
		testStep = new CAppfwkSysStartTestStepAppFailRetry0Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppFailRetry0Panic)
		{
		testStep = new CAppfwkSysStartTestStepAppFailRetry0Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppFailRetry1Ignore)
		{
		testStep = new CAppfwkSysStartTestStepAppFailRetry1Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppFailRetry1Panic)
		{
		testStep = new CAppfwkSysStartTestStepAppFailRetry1Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppStartCs)
		{
		testStep = new CAppfwkSysStartTestStepAppStartCs();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppStartFailPanic)
		{
		testStep = new CAppfwkSysStartTestStepAppStartFailPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppStartCd)
		{
		testStep = new CAppfwkSysStartTestStepAppStartCd();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppStartNc)
		{
		testStep = new CAppfwkSysStartTestStepAppStartNc();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppTimeoutRetry3Ignore)
		{
		testStep = new CAppfwkSysStartTestStepAppTimeoutRetry3Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepAppTimeoutRetry3Panic)
		{
		testStep = new CAppfwkSysStartTestStepAppTimeoutRetry3Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefInvalidAppRetry) 	// EDeferredWaitForStart test cases
		{
		testStep = new CAppfwkSysStartTestStepDefInvalidAppRetry();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefInvalidAppTimeout)
		{
		testStep = new CAppfwkSysStartTestStepDefInvalidAppTimeout();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefInvalidMultFail)
		{
		testStep = new CAppfwkSysStartTestStepDefInvalidMultFail();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefInvalidMultWait)
		{
		testStep = new CAppfwkSysStartTestStepDefInvalidMultWait();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefInvalidProcRetry)
		{
		testStep = new CAppfwkSysStartTestStepDefInvalidProcRetry();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefInvalidProcTimeout)
		{
		testStep = new CAppfwkSysStartTestStepDefInvalidProcTimeout();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefMultGrp)
		{
		testStep = new CAppfwkSysStartTestStepDefMultGrp();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefNoMult1)
		{
		testStep = new CAppfwkSysStartTestStepDefNoMult1();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefNoMult2)
		{
		testStep = new CAppfwkSysStartTestStepDefNoMult2();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefNoMult3)
		{
		testStep = new CAppfwkSysStartTestStepDefNoMult3();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefRendFailAppPanic)
		{
		testStep = new CAppfwkSysStartTestStepDefRendFailAppPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefRendFailIgnore)
		{
		testStep = new CAppfwkSysStartTestStepDefRendFailIgnore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefRendFailProcPanic)
		{
		testStep = new CAppfwkSysStartTestStepDefRendFailProcPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefStartAllState)
		{
		testStep = new CAppfwkSysStartTestStepDefStartAllState();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefStartCd)
		{
		testStep = new CAppfwkSysStartTestStepDefStartCd();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefStartCs)
		{
		testStep = new CAppfwkSysStartTestStepDefStartCs();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefStartFailAppPanic)
		{
		testStep = new CAppfwkSysStartTestStepDefStartFailAppPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefStartFailIgnore)
		{
		testStep = new CAppfwkSysStartTestStepDefStartFailIgnore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefStartFailProcPanic)
		{
		testStep = new CAppfwkSysStartTestStepDefStartFailProcPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefStartNc)
		{
		testStep = new CAppfwkSysStartTestStepDefStartNc();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefTimeoutAppIgnore)
		{
		testStep = new CAppfwkSysStartTestStepDefTimeoutAppIgnore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefTimeoutAppPanic)
		{
		testStep = new CAppfwkSysStartTestStepDefTimeoutAppPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefTimeoutProcIgnore)
		{
		testStep = new CAppfwkSysStartTestStepDefTimeoutProcIgnore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDefTimeoutProcPanic)
		{
		testStep = new CAppfwkSysStartTestStepDefTimeoutProcPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllCdFailIgnore)		// START_DLL_INFO test cases
		{
		testStep = new CAppfwkSysStartTestStepDllCdFailIgnore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllCdFailPanic)
		{
		testStep = new CAppfwkSysStartTestStepDllCdFailPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllCdNormal)
		{
		testStep = new CAppfwkSysStartTestStepDllCdNormal();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllCsFailIgnore)
		{
		testStep = new CAppfwkSysStartTestStepDllCsFailIgnore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllCsFailPanic)
		{
		testStep = new CAppfwkSysStartTestStepDllCsFailPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllCsFnInvalidFail)
		{
		testStep = new CAppfwkSysStartTestStepDllCsFnInvalidFail();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllCsFnMultipleRetry)
		{
		testStep = new CAppfwkSysStartTestStepDllCsFnMultipleRetry();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllCsNormal)
		{
		testStep = new CAppfwkSysStartTestStepDllCsNormal();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllFnSharedData)
		{
		testStep = new CAppfwkSysStartTestStepDllFnSharedData();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidCustomData)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidCustomData();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidOrdinal0)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidOrdinal0();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidOrdinalRetry0Ignore)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidOrdinalRetry0Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidOrdinalRetry1Ignore)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidOrdinalRetry1Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidOrdinalRetry0Panic)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidOrdinalRetry0Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidOrdinalRetry1Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidRetry0Ignore)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidRetry0Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidRetry1Ignore)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidRetry1Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidRetry0Panic)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidRetry0Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllInvalidRetry1Panic)
		{
		testStep = new CAppfwkSysStartTestStepDllInvalidRetry1Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllNcFailIgnore)
		{
		testStep = new CAppfwkSysStartTestStepDllNcFailIgnore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllNcFailPanic)
		{
		testStep = new CAppfwkSysStartTestStepDllNcFailPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepDllNcNormal)
		{
		testStep = new CAppfwkSysStartTestStepDllNcNormal();
		}
	else if(aStepName == KCAppfwkSysStartTestStepInitApparc)				// INIT_APPARC test cases
		{
		testStep = new CAppfwkSysStartTestStepInitApparc();
		}
	else if(aStepName == KCAppfwkSysStartTestStepInitApparcBeforeServer)
		{
		testStep = new CAppfwkSysStartTestStepInitApparcBeforeServer();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcFailRetry0Ignore)		// START_PROCESS_INFO test cases
		{
		testStep = new CAppfwkSysStartTestStepProcFailRetry0Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcFailRetry0Panic)
		{
		testStep = new CAppfwkSysStartTestStepProcFailRetry0Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcFailRetry1Ignore)
		{
		testStep = new CAppfwkSysStartTestStepProcFailRetry1Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcFailRetry1Panic)
		{
		testStep = new CAppfwkSysStartTestStepProcFailRetry1Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcStartCs)
		{
		testStep = new CAppfwkSysStartTestStepProcStartCs();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcStartFailPanic)
		{
		testStep = new CAppfwkSysStartTestStepProcStartFailPanic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcStartCd)
		{
		testStep = new CAppfwkSysStartTestStepProcStartCd();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcStartNc)
		{
		testStep = new CAppfwkSysStartTestStepProcStartNc();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcTimeoutRetry3Ignore)
		{
		testStep = new CAppfwkSysStartTestStepProcTimeoutRetry3Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepProcTimeoutRetry3Panic)
		{
		testStep = new CAppfwkSysStartTestStepProcTimeoutRetry3Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepReRunSysStart)
		{
		testStep = new CAppfwkSysStartTestStepReRunSysStart();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCdToNcDefault)			// STFP Test cases
		{
		testStep = new CAppfwkSysStartTestStepStfpCdToNcDefault();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCdToNcRetry0Ignore)
		{
		testStep = new CAppfwkSysStartTestStepStfpCdToNcRetry0Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCdToNcRetry0Panic)
		{
		testStep = new CAppfwkSysStartTestStepStfpCdToNcRetry0Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCdToNcRetry1Ignore)
		{
		testStep = new CAppfwkSysStartTestStepStfpCdToNcRetry1Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCdToNcRetry1Panic)
		{
		testStep = new CAppfwkSysStartTestStepStfpCdToNcRetry1Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCsToCdDefault)
		{
		testStep = new CAppfwkSysStartTestStepStfpCsToCdDefault();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCsToCdRetry0Ignore)
		{
		testStep = new CAppfwkSysStartTestStepStfpCsToCdRetry0Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCsToCdRetry0Panic)
		{
		testStep = new CAppfwkSysStartTestStepStfpCsToCdRetry0Panic();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCsToCdRetry1Ignore)
		{
		testStep = new CAppfwkSysStartTestStepStfpCsToCdRetry1Ignore();
		}
	else if(aStepName == KCAppfwkSysStartTestStepStfpCsToCdRetry1Panic)
		{
		testStep = new CAppfwkSysStartTestStepStfpCsToCdRetry1Panic();
		}
	

	return testStep;
	}
