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
 @file te_defproxysuiteserver.cpp
 @internalTechnology
*/

#include "te_defproxysuiteserver.h"
#include "availabilityfallbackstep.h"
#include "basicusagestep.h"
#include "canceltrackingstep.h"
#include "cenrepteststep.h"
#include "classfallbackstep.h"
#include "errorfallbackstep.h"
#include "prioritychangesstep.h"
#include "trackingandprioritystep.h"
#include "trackingsessionstep.h"
#include "DynamicListStep.h"
#include "extgpspsystep.h"
#include "hgenericinfostep.h"
#include "multipleclientsstep.h"
#include "oneintgpspsystep.h"
#include "oomstep.h"
#include "partialupdateandnetworkstep.h"
#include "partialupdatestep.h"
#include "psydisabledstep.h"
#include "psyenabledstep.h"
#include "psyinvisiblestep.h"
#include "psyvisiblestep.h"
#include "threepsysstep.h"
#include "twointernalgpspsystep.h"
#include "twopsysactivestep.h"
#include "twopsysstep.h"

_LIT(KServerName,"te_defproxysuite");

CTe_defproxySuite* CTe_defproxySuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_defproxySuite * server = new (ELeave) CTe_defproxySuite();
	CleanupStack::PushL(server);

	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

/**
2nd phase constructor. Calls the base class method passing the name of the suite.
*/	
void CTe_defproxySuite::ConstructL()
	{
	//ConstructL of the base class
	CTestServer::ConstructL(KServerName);
	
	// Start lbsroot (so properties are defined), 
	iStarter.RestartLbs_RootOnlyL();	
	}

CTe_defproxySuite::~CTe_defproxySuite()
	{
	TRAP_IGNORE(iStarter.RestartLbs_NormalL());
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
	CTe_defproxySuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_defproxySuite::NewL());
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
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_defproxySuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
    if(aStepName == KAvailabilityFallbackStep)
                    testStep = new CAvailabilityFallbackStep();
    else if(aStepName == KBasicUsageStep)
                    testStep = new CBasicUsageStep();
    else if(aStepName == KCancelTrackingStep)
                    testStep = new CCancelTrackingStep();
    else if(aStepName == KCenrepTestStep)
                    testStep = new CCenrepTestStep();
    else if(aStepName == KClassFallbackStep)
                    testStep = new CClassFallbackStep();
    else if(aStepName == KErrorFallbackStep)
                    testStep = new CErrorFallbackStep();
    else if(aStepName == KPriorityChangesStep)
                    testStep = new CPriorityChangesStep();
    else if(aStepName == KTrackingAndPriorityStep)
                    testStep = new CTrackingAndPriorityStep();
    else if(aStepName == KTrackingSessionStep)
                    testStep = new CTrackingSessionStep();
    else if(aStepName == KDynamicListStep)
                    testStep = new CDynamicListStep();
    else if(aStepName == KExtGpsPsyStep)
                    testStep = new CExtGpsPsyStep();
    else if(aStepName == KHGenericInfoStep)
                    testStep = new CHGenericInfoStep();
    else if(aStepName == KMultipleClientsStep)
                    testStep = new CMultipleClientsStep();
    else if(aStepName == KOneIntGpsPsyStep)
                    testStep = new COneIntGpsPsyStep();
    else if(aStepName == KOOMStep)
                    testStep = new COOMStep();
    else if(aStepName == KPartialUpdateAndNetworkStep)
                    testStep = new CPartialUpdateAndNetworkStep();
    else if(aStepName == KPartialUpdateStep)
                    testStep = new CPartialUpdateStep();
    else if(aStepName == KPsyDisabledStep)
                    testStep = new CPsyDisabledStep();
    else if(aStepName == KPsyEnabledStep)
                    testStep = new CPsyEnabledStep();
    else if(aStepName == KPsyInvisibleStep)
                    testStep = new CPsyInvisibleStep();
    else if(aStepName == KPsyVisibleStep)
                    testStep = new CPsyVisibleStep();
    else if(aStepName == KThreePsysStep)
                    testStep = new CThreePsysStep();
    else if(aStepName == KTwoInternalGpsPsyStep)
                    testStep = new CTwoInternalGpsPsyStep();
    else if(aStepName == KTwoPsysActiveStep)
                    testStep = new CTwoPsysActiveStep();
    else if(aStepName == KTwoPsysStep)
                    testStep = new CTwoPsysStep();

	return testStep;
	}
