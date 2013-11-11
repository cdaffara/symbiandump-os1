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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file Te_lbsnetgatewaySuiteServer.cpp
*/

#include "lbsdevloggermacros.h"
#include "Te_lbsnetgatewaySuiteServer.h"
#include "lbsnetgatewaystartstep.h"
#include "lbsnetgatewaystopstep.h"
#include "lbsnetgateway1Step.h"
#include "lbsnetgateway2Step.h"
#include "lbsnetgateway3Step.h"
#include "lbsnetgateway4Step.h"
#include "lbsnetgateway5Step.h"
#include "lbsnetgateway6Step.h"
#include "lbsnetgatewaynetworkpsystep.h"
#include "lbsnetgatewaymultiprivacystep.h"
#include "lbsnetgatewaynetregstatusstep.h"
#include "lbsnetgateway7multipleprotocolmodulestep.h"
#include "lbsnetgatewaystartmultiplepmstep.h"
#include "lbsnetgatewaysettohomenetwork.h"
#include "lbsnetgatewaysettoroamingnetwork.h"

_LIT(KServerName,"Te_lbsnetgatewaySuite");
CTe_lbsnetgatewaySuite* CTe_lbsnetgatewaySuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	//LBSLOG(ELogP1, "CTe_lbsnetgatewaySuite::NewL()");
	CTe_lbsnetgatewaySuite * server = new (ELeave) CTe_lbsnetgatewaySuite();
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
	//LBSLOG(ELogP3, "te_lbsnetgatewaySuite MainL");
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_lbsnetgatewaySuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_lbsnetgatewaySuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		//LBSLOG(ELogP3, "te_lbsnetgatewaySuite Randezvous and starting scheduler");
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
	//LBSLOG_BEGIN();
	//LBSLOG(ELogP3, "te_lbsnetgatewaySuite start");
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	//LBSLOG(ELogP3, "te_lbsnetgatewaySuite stop");
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_lbsnetgatewaySuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
              if(aStepName == Klbsnetgatewaystartstep)
                            testStep = new Clbsnetgatewaystartstep();
              else if(aStepName == Klbsnetgateway1Step)
                            testStep = new Clbsnetgateway1Step();
              else if(aStepName == Klbsnetgateway2Step)
                            testStep = new Clbsnetgateway2Step();
              else if(aStepName == Klbsnetgateway3Step)
                            testStep = new Clbsnetgateway3Step();
              else if(aStepName == Klbsnetgateway4Step)
                            testStep = new Clbsnetgateway4Step();
              else if(aStepName == Klbsnetgateway5Step)
                            testStep = new Clbsnetgateway5Step();
              else if(aStepName == Klbsnetgateway6Step)
                            testStep = new Clbsnetgateway6Step();
              else if(aStepName == Klbsnetgatewaystopstep)
                            testStep = new Clbsnetgatewaystopstep();
              else if(aStepName == Klbsnetgatewaymultiprivacystep)
              				testStep = new Clbsnetgatewaymultiprivacystep();
              else if(aStepName == KLbsNetGatewayNetRegStatusStep)
              				testStep = new CLbsNetGatewayNetRegStatusStep();
              else if(aStepName == Klbsnetgateway7MultiplePMStep)
              				testStep = new Clbsnetgateway7MultiplePMStep();
              else if(aStepName == Klbsnetgatewaystartstep2)
              				testStep = new Clbsnetgatewaystartmultiplepmstep();

              else if(aStepName == KLbsNetGatewaySetToHomeNetwork)
              				testStep = new CLbsNetGatewaySetToHomeNetwork();
              else if(aStepName == KLbsNetGatewaySetToRoamingNetwork)
              				testStep = new CLbsNetGatewaySetToRoamingNetwork();
              
              else if(aStepName == KLbsNetgatewayNetworkPsyStep)
                            testStep = new CLbsNetGatewayNetworkPsyStep();
              
              else
            	  {
            	  TBuf<100> stepnamethatcannotbefound(aStepName);
            	  INFO_PRINTF2(_L("StepName that cannot be found is %S"), &stepnamethatcannotbefound);
            	  }

	return testStep;
	}
