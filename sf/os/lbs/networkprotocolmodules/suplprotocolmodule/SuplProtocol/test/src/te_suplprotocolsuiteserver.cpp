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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file Te_suplprotocolSuiteServer.cpp
 @internalTechnology
*/

#include <lbs/lbsadmin.h>

#include "te_suplprotocolsuiteserver.h"
#include "plugin1Step.h"
#include "plugin2Step.h"
#include "molr1Step.h"
#include "molr2Step.h"
#include "molr3Step.h"
#include "molr4Step.h"
#include "molr5Step.h"
#include "molr6Step.h"
#include "molr7Step.h"
#include "molr8Step.h"
#include "molr9Step.h"
#include "molr10Step.h"
#include "molr11Step.h"
#include "molr12Step.h"
#include "molr13Step.h"
#include "molr14Step.h"
#include "molr15Step.h"
#include "molr16Step.h"
#include "molr17Step.h"
#include "molr18Step.h"
#include "molr19Step.h"
#include "molr20Step.h"
#include "molr21Step.h"
#include "molr22Step.h"
#include "molr23Step.h"
#include "molr24Step.h"
#include "molr25Step.h"
#include "molr26Step.h"
#include "molr27Step.h"
#include "molr28Step.h"
#include "molr29Step.h"
#include "molr30Step.h"
#include "molr31Step.h"
#include "molr32Step.h"
#include "molr33Step.h"
#include "mtlr1Step.h"
#include "mtlr2Step.h"
#include "mtlr3Step.h"
#include "mtlr4Step.h"
#include "mtlr5Step.h"
#include "mtlr6Step.h"
#include "mtlr7Step.h"
#include "mtlr8Step.h"
#include "mtlr9Step.h"
#include "mtlr10Step.h"
#include "mtlr11Step.h"
#include "mtlr12Step.h"
#include "mtlr13Step.h"
#include "mtlr14Step.h"
#include "mtlr15Step.h"
#include "mtlr16Step.h"
#include "mtlr17Step.h"
#include "mtlr18Step.h"
#include "mtlr19Step.h"
#include "mtlr20Step.h"
#include "mtlr21Step.h"
#include "mtlr22Step.h"
#include "mtlr23Step.h"
#include "mtlr24Step.h"
#include "mtlr25Step.h"
#include "mtlr26Step.h"
#include "etel0Step.h"
#include "etel1Step.h"

_LIT(KServerName,"Te_suplprotocolSuite");
CTe_suplprotocolSuite* CTe_suplprotocolSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CLbsAdmin* adminApi = CLbsAdmin::NewL();
	CleanupStack::PushL(adminApi);
	User::LeaveIfError(adminApi->Set(KLbsSettingLogger, CLbsAdmin::ELoggerOff));
	CleanupStack::PopAndDestroy();
	
	
	CTe_suplprotocolSuite * server = new (ELeave) CTe_suplprotocolSuite();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
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
	CTe_suplprotocolSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_suplprotocolSuite::NewL());
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
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_suplprotocolSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	
	CTestStep* testStep = NULL;
	
     if(aStepName == Kplugin1Step)
                testStep = new Cplugin1Step();
     else if(aStepName == Kplugin2Step)
                testStep = new Cplugin2Step();     
     else if(aStepName == Kmolr1Step)
                testStep = new Cmolr1Step();
     else if(aStepName == Kmolr2Step)
                testStep = new Cmolr2Step();
	 else if(aStepName == Kmolr3Step)
    			testStep = new Cmolr3Step();
     else if(aStepName == Kmolr4Step)
                testStep = new Cmolr4Step();
     else if(aStepName == Kmolr5Step)
                testStep = new Cmolr5Step();
     else if(aStepName == Kmolr6Step)
                testStep = new Cmolr6Step();
     else if(aStepName == Kmolr7Step)
                testStep = new Cmolr7Step();
     else if(aStepName == Kmolr8Step)
                testStep = new Cmolr8Step();
     else if(aStepName == Kmolr9Step)
                testStep = new Cmolr9Step();
     else if(aStepName == Kmolr10Step)
                testStep = new Cmolr10Step();
     else if(aStepName == Kmolr11Step)
                testStep = new Cmolr11Step();
     else if(aStepName == Kmolr12Step)
                testStep = new Cmolr12Step();
     else if(aStepName == Kmolr13Step)
                testStep = new Cmolr13Step();
     else if(aStepName == Kmolr14Step)
                testStep = new Cmolr14Step();
     else if(aStepName == Kmolr15Step)
                testStep = new Cmolr15Step();
     else if(aStepName == Kmolr16Step)
                testStep = new Cmolr16Step();
     else if(aStepName == Kmolr17Step)
                testStep = new Cmolr17Step();
     else if(aStepName == Kmolr18Step)
                testStep = new Cmolr18Step();
     else if(aStepName == Kmolr19Step)
                testStep = new Cmolr19Step();
     else if(aStepName == Kmolr20Step)
                testStep = new Cmolr20Step();
     else if(aStepName == Kmolr21Step)
                testStep = new Cmolr21Step();
     else if(aStepName == Kmolr22Step)
                testStep = new Cmolr22Step();
     else if(aStepName == Kmolr23Step)
                testStep = new Cmolr23Step();
     else if(aStepName == Kmolr24Step)
                testStep = new Cmolr24Step();
     else if(aStepName == Kmolr25Step)
                testStep = new Cmolr25Step();
     else if (aStepName == Kmolr26Step)
                testStep = new Cmolr26Step();
     else if (aStepName == Kmolr27Step)
                testStep = new Cmolr27Step();     
     else if (aStepName == Kmolr28Step)
                testStep = new Cmolr28Step();
     else if (aStepName == Kmolr29Step)
                testStep = new Cmolr29Step();  
     else if (aStepName == Kmolr30Step)
                testStep = new Cmolr30Step();
     else if (aStepName == Kmolr31Step)
                testStep = new Cmolr31Step();
     else if (aStepName == Kmolr32Step)
                testStep = new Cmolr32Step();
     else if (aStepName == Kmolr33Step)
                testStep = new Cmolr33Step();     
     // MTLR
     else if (aStepName == Kmtlr1Step)
                testStep = new Cmtlr1Step();
     else if (aStepName == Kmtlr2Step)
                testStep = new Cmtlr2Step();
     else if (aStepName == Kmtlr3Step)
                testStep = new Cmtlr3Step();
     else if (aStepName == Kmtlr4Step)
                testStep = new Cmtlr4Step();
     else if (aStepName == Kmtlr5Step)
                testStep = new Cmtlr5Step();
     else if (aStepName == Kmtlr6Step)
                testStep = new Cmtlr6Step();
     else if (aStepName == Kmtlr7Step)
                testStep = new Cmtlr7Step();
     else if (aStepName == Kmtlr8Step)
                testStep = new Cmtlr8Step();
     else if (aStepName == Kmtlr9Step)
                testStep = new Cmtlr9Step();
     else if (aStepName == Kmtlr10Step)
                testStep = new Cmtlr10Step();
     else if (aStepName == Kmtlr11Step)
                testStep = new Cmtlr11Step();
     else if (aStepName == Kmtlr12Step)
                testStep = new Cmtlr12Step();
     else if (aStepName == Kmtlr13Step)
                testStep = new Cmtlr13Step();
     else if (aStepName == Kmtlr14Step)
                testStep = new Cmtlr14Step();
     else if (aStepName == Kmtlr15Step)
                testStep = new Cmtlr15Step();
     else if (aStepName == Kmtlr16Step)
                testStep = new Cmtlr16Step();
     else if (aStepName == Kmtlr17Step)
                testStep = new Cmtlr17Step();
     else if (aStepName == Kmtlr18Step)
                testStep = new Cmtlr18Step();
     else if (aStepName == Kmtlr19Step)
                testStep = new Cmtlr19Step();
     else if (aStepName == Kmtlr20Step)
                testStep = new Cmtlr20Step();
     else if (aStepName == Kmtlr21Step)
                testStep = new Cmtlr21Step();
     else if (aStepName == Kmtlr22Step)
                testStep = new Cmtlr22Step();
     else if (aStepName == Kmtlr23Step)
                testStep = new Cmtlr23Step();
     else if (aStepName == Kmtlr24Step)
                testStep = new Cmtlr24Step();   
     else if (aStepName == Kmtlr25Step)
                testStep = new Cmtlr25Step();   
      else if (aStepName == Kmtlr26Step)
                testStep = new Cmtlr26Step();     
     // ETel
     else if (aStepName == KEtel0Step)
                testStep = new CEtel0Step();    
     else if (aStepName == KEtel1Step)
                 testStep = new CEtel1Step();
     return testStep;
	}
