// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
*/

#include "Te_gsmu_emsSuiteServer.h"
#include "testaddingpredefsoundaftermelodyStep.h"
#include "testcorruptedinformationelementsStep.h"
#include "testcsmsmessageemsapiStep.h"
#include "testcsmsmessageemsgetapiStep.h"
#include "testemsanimationieStep.h"
#include "testemsformatieStep.h"
#include "testemsierelativestartpositionStep.h"
#include "testemsodiieStep.h"
#include "testemsoomStep.h"
#include "testemspictureieStep.h"
#include "testemspredefanimationieStep.h"
#include "testemspredefsoundieStep.h"
#include "testemssoundieStep.h"
#include "testemsuserpromptieStep.h"
#include "testformattingsegmentationStep.h"
#include "testinvalidodipositioningStep.h"
#include "testinvalidpromptpositioningStep.h"
#include "testnumberofmessagesStep.h"
#include "testoftextobjectpositioningStep.h"
#include "testremovalofmultipleodiandupiesStep.h"
#include "testremovalofodiandupiesStep.h"
#include "testremovalofodicontrollediesStep.h"
#include "testremovenonexistingobjectStep.h"
#include "testsamepositionStep.h"
#include "teststreamingStep.h"
#include "testtoobigconcatenatedemsmessageStep.h"

_LIT(KServerName,"Te_gsmu_ems");


CTe_gsmu_emsSuite* CTe_gsmu_emsSuite::NewL()
/**
 *  @return - Instance of the test server
 *  Same code for Secure and non-secure variants
 *  Called inside the MainL() function to create and start the
 *  CTestServer derived server.
 */
	{
	CTe_gsmu_emsSuite * server = new (ELeave) CTe_gsmu_emsSuite();
	CleanupStack::PushL(server);
	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(KServerName);
	//server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
/**
 *  Secure variant
 *  Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_gsmu_emsSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_gsmu_emsSuite::NewL());
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
 *  @return - Standard Epoc error code on process exit
 *  Secure variant only
 *  Process entry point. Called by client using RProcess API
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


CTestStep* CTe_gsmu_emsSuite::CreateTestStep(const TDesC& aStepName)
/**
 *  @return - A CTestStep derived instance
 *  Secure and non-secure variants
 *  Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	//
	// Here the test step is created when it is needed. Note that this
	// function is non-leaving so we cannot use new(ELeave). Therefore
	// the new could return NULL, but that is not a problem as it implies
	// the test step is missing and this will be marked in the log file.
	//
    if (aStepName == _L("testaddingpredefsoundaftermelodyStep"))
    	{
        testStep = new CtestaddingpredefsoundaftermelodyStep();
        }
    else if(aStepName == _L("testcorruptedinformationelementsStep"))
    	{
        testStep = new CtestcorruptedinformationelementsStep();
        }
    else if(aStepName == _L("testcsmsmessageemsapiStep"))
    	{
        testStep = new CtestcsmsmessageemsapiStep();
        }
    else if(aStepName == _L("testcsmsmessageemsgetapiStep"))
    	{
        testStep = new CtestcsmsmessageemsgetapiStep();
        }
    else if(aStepName == _L("testemsanimationieStep"))
    	{
        testStep = new CtestemsanimationieStep();
        }
    else if(aStepName == _L("testemsformatieStep"))
    	{
        testStep = new CtestemsformatieStep();
        }
    else if(aStepName == _L("testemsierelativestartpositionStep"))
    	{
        testStep = new CtestemsierelativestartpositionStep();
        }
    else if(aStepName == _L("testemsodiieStep"))
    	{
        testStep = new CtestemsodiieStep();
        }
    else if(aStepName == _L("testemsoomStep"))
    	{
        testStep = new CtestemsoomStep();
        }
    else if(aStepName == _L("testemspictureieStep"))
    	{
        testStep = new CtestemspictureieStep();
        }
    else if(aStepName == _L("testemspredefanimationieStep"))
    	{
        testStep = new CtestemspredefanimationieStep();
        }
    else if(aStepName == _L("testemspredefsoundieStep"))
    	{
        testStep = new CtestemspredefsoundieStep();
        }
    else if(aStepName == _L("testemssoundieStep"))
    	{
        testStep = new CtestemssoundieStep();
        }
    else if(aStepName == _L("testemsuserpromptieStep"))
    	{
        testStep = new CtestemsuserpromptieStep();
        }
    else if(aStepName == _L("testformattingsegmentationStep"))
    	{
        testStep = new CtestformattingsegmentationStep();
        }
    else if(aStepName == _L("testinvalidodipositioningStep"))
    	{
        testStep = new CtestinvalidodipositioningStep();
        }
    else if(aStepName == _L("testinvalidpromptpositioningStep"))
    	{
        testStep = new CtestinvalidpromptpositioningStep();
        }
    else if(aStepName == _L("testnumberofmessagesStep"))
    	{
        testStep = new CtestnumberofmessagesStep();
        }
    else if(aStepName == _L("testoftextobjectpositioningStep"))
    	{
        testStep = new CtestoftextobjectpositioningStep();
        }
    else if(aStepName == _L("testremovalofmultipleodiandupiesStep"))
    	{
        testStep = new CtestremovalofmultipleodiandupiesStep();
        }
    else if(aStepName == _L("testremovalofodiandupiesStep"))
    	{
        testStep = new CtestremovalofodiandupiesStep();
        }
    else if(aStepName == _L("testremovalofodicontrollediesStep"))
    	{
        testStep = new CtestremovalofodicontrollediesStep();
        }
    else if(aStepName == _L("testremovenonexistingobjectStep"))
    	{
        testStep = new CtestremovenonexistingobjectStep();
        }
    else if(aStepName == _L("testsamepositionStep"))
    	{
        testStep = new CtestsamepositionStep();
        }
    else if(aStepName == _L("teststreamingStep"))
    	{
        testStep = new CteststreamingStep();
        }
    else if(aStepName == _L("testtoobigconcatenatedemsmessageStep"))
    	{
        testStep = new CtesttoobigconcatenatedemsmessageStep();
        }
	
	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}
	
	return testStep;
	}
