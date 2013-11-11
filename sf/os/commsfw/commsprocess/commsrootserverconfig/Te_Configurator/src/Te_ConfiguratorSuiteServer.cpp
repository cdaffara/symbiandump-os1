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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file Te_ConfiguratorSuiteServer.cpp
*/

#include "Te_ConfiguratorSuiteServer.h"

_LIT(KServerName,"Te_ConfiguratorSuite");
CTe_ConfiguratorSuite* CTe_ConfiguratorSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_ConfiguratorSuite * server = new (ELeave) CTe_ConfiguratorSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	//server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_ConfiguratorSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_ConfiguratorSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	__ASSERT_ALWAYS(!err, User::Panic(_L("Panic From MainL"), err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }


CTestStep* CTe_ConfiguratorSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below         

	// remove ELeave as harness will test ptr. This is more efficient
	// than using TRAP_IGNORE

	if(aStepName == _L("LoadCpm"))
		{
		testStep = new CLoadCpm;
		}
	else
	if(aStepName == _L("LoadCpmMorethanOne"))
		{
		testStep = new CLoadCpmMorethanOne;
		}
	else
	if(aStepName == _L("LoadCpmBootTime"))
		{
		testStep = new CLoadCpmBootTime;
		}
	else
	if(aStepName == _L("LoadCpmWithoutCMI"))
		{
		testStep = new CLoadCpmWithoutCMI;
		}
	else
	if(aStepName == _L("LoadCpmWithDiffHeapOption"))
		{
		testStep = new CLoadCpmWithDiffHeapOption;
		}
	else
	if(aStepName == _L("LoadOnDemandCpmBootTime"))
		{
		testStep = new CLoadOnDemandCpmBootTime;
		}
	else
	if(aStepName == _L("LoadCpmDiffSession"))
		{
		testStep = new CLoadCpmDiffSession;
		}
	else
	if(aStepName == _L("LoadCpmGrpDiffSession"))
		{
		testStep = new CLoadCpmGrpDiffSession;
		}
	else
	if(aStepName == _L("LoadallDemandCpm"))
		{
		testStep = new CLoadallDemandCpm;
		}
	else
	if(aStepName == _L("LoadCpmCorruptCMI"))
		{
		testStep = new CLoadCpmCorruptCMI;
		}
	else
	if(aStepName == _L("LoadCpmBrokenCmi"))
		{
		testStep = new CLoadCpmBrokenCmi;
		}
	else
	if(aStepName == _L("LoadCpmCorruptOnDemand"))
		{
		testStep = new CLoadCpmCorruptOnDemand;
		}
	else
	if(aStepName == _L("UnloadCpm"))
		{
		testStep = new CUnloadCpm;
		}
	else
	if(aStepName == _L("UnloadCpmPreNotLoaded"))
		{
		testStep = new CUnloadCpmPreNotLoaded;
		}
	else
	if(aStepName == _L("LoadUnloadDiffCombination"))
		{
		testStep = new CLoadUnloadDiffCombination;
		}
	else
	if(aStepName == _L("CancelLoadCpm"))
		{
		testStep = new CCancelLoadCpm;
		}
	else
	if(aStepName == _L("CancelUnLoadCpm"))
		{
		testStep = new CCancelUnLoadCpm;
		}
	else
	if(aStepName == _L("GetInfoCpm"))
		{
		testStep = new CGetInfoCpm;
		}
	else
	if(aStepName == _L("GetLongIniInfoCpm"))
		{
		testStep = new CGetLongIniInfoCpm;
		}
	else
	if(aStepName == _L("GetMultiLineIniInfoCpm"))
		{
		testStep = new CGetMultiLineIniInfoCpm;
		}
	else
	if(aStepName == _L("GetInfoCpmWithoutCMI"))
		{
		testStep = new CGetInfoCpmWithoutCMI;
		}
	else
	if(aStepName == _L("GetInfoCpmWithoutINI"))
		{
		testStep = new CGetInfoCpmWithoutINI;
		}
	else
	if(aStepName == _L("GetInfoCpmIniOverflow"))
		{
		testStep = new CGetInfoCpmIniOverflow;
		}
	else
	if(aStepName == _L("ListModules"))
		{
		testStep = new CListModules;
		}
	else
	if(aStepName == _L("EnumerateGroup"))
		{
		testStep = new CEnumerateGroup;
		}
	else
	if(aStepName == _L("EnumeratCpmwithTwoCmi"))
   		{
		testStep = new CEnumeratCpmwithTwoCmi;
		}
	else
	if(aStepName == _L("EnumeratCpmwithOneCmi"))
   		{
		testStep = new CEnumeratCpmwithOneCmi;
		}	
	else
	if(aStepName == _L("ListModulesGroupUnknown"))
		{
		testStep = new CListModulesGroupUnknown;
		}
	else
	if(aStepName == _L("HeapAllocationFailure"))
		{
		testStep = new CHeapAllocationFailure;
		}
	else
	if(aStepName == _L("HeapFailurecancelLoad"))
		{
		testStep = new CHeapFailurecancelLoad;
		}
	else
	if(aStepName == _L("HeapFailurecancelUnLoad"))
		{
		testStep = new CHeapFailurecancelUnLoad;
		}
	else
	if(aStepName == _L("HeapFailurecancelUnLoad"))
		{
		testStep = new CHeapFailurecancelUnLoad;
		}
	else
	if(aStepName == _L("HeapFailurecancelGetIni"))
		{
		testStep = new CHeapFailurecancelGetIni;
		}
	else
	if(aStepName == _L("HeapFailureEnumerateGrp"))
		{
		testStep = new CHeapFailureEnumerateGrp;
		}
	else
	if(aStepName == _L("HeapFailureconfigApi"))
		{
		testStep = new CHeapFailureconfigApi;
		}
	else
	if(aStepName == _L("LoadCpmSameWorkerID"))
		{
		testStep = new CLoadCpmSameWorkerID;
		}
	else if(aStepName == _L("LoadCpmBindTimeOut"))
		{
		testStep = new CLoadCpmBindTimeOut;
		}
	else if(aStepName == _L("LoadOnDemandBootCPMFailToLoad"))
		{
		testStep = new CLoadOnDemandBootCPMFailToLoad;
		}
	else if(aStepName == _L("OnDemandCPMFailToLoad"))
		{
		testStep = new COnDemandCPMFailToLoad;
		}
	else if(aStepName == _L("OnDemandCPMFailToLoadBindingOrderChanged"))
		{
		testStep = new COnDemandCPMFailToLoadBindingOrderChanged;
		}
	else if(aStepName == _L("LoadOnDemandCPMOneofTwoBindingsFail"))
		{
		testStep = new CLoadOnDemandCPMOneofTwoBindingsFail;
		}
			
	return testStep;
	}

