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
 @file Te_LbsApiSuiteServer.cpp
 @internalTechnology
*/

#include "te_lbsapisuiteserver.h"
#include "calculationmethodsstep.h"
#include "criteriaclassesstep.h"
#include "dataclassesstep.h"
#include "genericinfostep.h"
#include "moduleclassesstep.h"
#include "nomemorystep.h"
#include "reminderstep.h"
#include "noserverstep.h"
#include "requestorclassesstep.h"

_LIT(KServerName,"Te_LbsApiSuite");
CTe_LbsApiSuite* CTe_LbsApiSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsApiSuite * server = new (ELeave) CTe_LbsApiSuite();
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
	CTe_LbsApiSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsApiSuite::NewL());
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


CTestStep* CTe_LbsApiSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	

	
	if(aStepName == KCalculationMethodsStep)
		testStep = new CCalculationMethodsStep();
	else if(aStepName == KCriteriaClassesStep)
		testStep = new CCriteriaClassesStep();
	else if(aStepName == KDataClassesStep)
		testStep = new CDataClassesStep();
	else if(aStepName == KGenericInfoStep)
		testStep = new CGenericInfoStep();
	else if(aStepName == KModuleClassesStep)
		testStep = new CModuleClassesStep();
	else if(aStepName == KNoMemoryStep)
#if defined(__WINS__) && defined(__DEBUG__)
		testStep = new CNoMemoryStep();
#else
		testStep = new CReminderStep();
#endif
	else if(aStepName == KNoServerStep)
		testStep = new CNoServerStep();
	else if(aStepName == KRequestorClassesStep)
		testStep = new CRequestorClassesStep();
	return testStep;
	}
