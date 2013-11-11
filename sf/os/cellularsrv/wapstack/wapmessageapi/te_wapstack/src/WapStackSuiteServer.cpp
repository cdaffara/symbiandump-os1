// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// WAP stack test suite server
// 
//

/**
 @file
 @internalComponent
*/

// Include your own server header file and step header file(s) here
#include "WapStackSuiteServer.h"

#include "WapBoundWDPSteps.h"
#include "WapBoundCLPushSteps.h"
#include "WapBoundWSPSteps.h"
#include "WapFullySpecCLPushSteps.h"
#include "WapFullySpecWDPSteps.h"
#include "WapFullySpecWSPSteps.h"
#include <ecom/ecom.h>

_LIT(KServerName,"WapStackSuite");
CWapStackSuite* CWapStackSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CWapStackSuite * server = new (ELeave) CWapStackSuite();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}



LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CWapStackSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CWapStackSuite::NewL());
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

CTestStep* CWapStackSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// Creating your own test steps here
	//*************************************
	// BOUND connection tests
	//*************************************

	// Bound WDP steps [1 - 9]
	if(aStepName == KBoundWDPStep_1) 
		testStep = new CBoundWDPStep_1();
	else if(aStepName == KBoundWDPStep_2) 
		testStep = new CBoundWDPStep_2();
	else if(aStepName == KBoundWDPStep_3) 
		testStep = new CBoundWDPStep_3();
	else if(aStepName == KBoundWDPStep_4) 
		testStep = new CBoundWDPStep_4();
	else if(aStepName == KBoundWDPStep_5) 
		testStep = new CBoundWDPStep_5();
	else if(aStepName == KBoundWDPStep_6) 
		testStep = new CBoundWDPStep_6();
	else if(aStepName == KBoundWDPStep_7) 
		testStep = new CBoundWDPStep_7();
	else if(aStepName == KBoundWDPStep_8) 
		testStep = new CBoundWDPStep_8();
	else if(aStepName == KBoundWDPStep_9) 
		testStep = new CBoundWDPStep_9();
	else if(aStepName == KBoundWDPStep_OutOfMemory) 
		testStep = new CBoundWDPStep_OutOfMemory();

	// Bound Push tests [1 - 9]
	else if(aStepName == KBoundCLPushStep_1) 
		testStep = new CBoundCLPushStep_1();
	else if(aStepName == KBoundCLPushStep_2) 
		testStep = new CBoundCLPushStep_2();
	else if(aStepName == KBoundCLPushStep_3) 
		testStep = new CBoundCLPushStep_3();
	else if(aStepName == KBoundCLPushStep_4) 
		testStep = new CBoundCLPushStep_4();
	else if(aStepName == KBoundCLPushStep_5) 
		testStep = new CBoundCLPushStep_5();
	else if(aStepName == KBoundCLPushStep_6) 
		testStep = new CBoundCLPushStep_6();
	else if(aStepName == KBoundCLPushStep_7) 
		testStep = new CBoundCLPushStep_7();
	else if(aStepName == KBoundCLPushStep_8) 
		testStep = new CBoundCLPushStep_8();
	else if(aStepName == KBoundCLPushStep_9) 
		testStep = new CBoundCLPushStep_9();
	
	// Bound WSP tests [1 - 9]
	else if(aStepName == KBoundWSPStep_1) 
		testStep = new CBoundWSPStep_1();
	else if(aStepName == KBoundWSPStep_2) 
		testStep = new CBoundWSPStep_2();
	else if(aStepName == KBoundWSPStep_3) 
		testStep = new CBoundWSPStep_3();
	else if(aStepName == KBoundWSPStep_4) 
		testStep = new CBoundWSPStep_4();
	else if(aStepName == KBoundWSPStep_5) 
		testStep = new CBoundWSPStep_5();
	else if(aStepName == KBoundWSPStep_6) 
		testStep = new CBoundWSPStep_6();
	else if(aStepName == KBoundWSPStep_7) 
		testStep = new CBoundWSPStep_7();
	else if(aStepName == KBoundWSPStep_8) 
		testStep = new CBoundWSPStep_8();
	else if(aStepName == KBoundWSPStep_9) 
		testStep = new CBoundWSPStep_9();

	//*************************************
	// FULLY SPECIFIED connection tests
	//*************************************

	// Fully spec. WDP [1 - 9
	else if(aStepName == KFullySpecWDPStep_1) 
		testStep = new CFullySpecWDPStep_1();
	else if(aStepName == KFullySpecWDPStep_2) 
		testStep = new CFullySpecWDPStep_2();
	else if(aStepName == KFullySpecWDPStep_3) 
		testStep = new CFullySpecWDPStep_3();
	else if(aStepName == KFullySpecWDPStep_4) 
		testStep = new CFullySpecWDPStep_4();
	else if(aStepName == KFullySpecWDPStep_5) 
		testStep = new CFullySpecWDPStep_5();
	else if(aStepName == KFullySpecWDPStep_6) 
		testStep = new CFullySpecWDPStep_6();
	else if(aStepName == KFullySpecWDPStep_7) 
		testStep = new CFullySpecWDPStep_7();
	else if(aStepName == KFullySpecWDPStep_8) 
		testStep = new CFullySpecWDPStep_8();
	else if(aStepName == KFullySpecWDPStep_9) 
		testStep = new CFullySpecWDPStep_9();
	
	// Fully spec. Push [1 - 9]
	else if(aStepName == KFullySpecCLPushStep_1) 
		testStep = new CFullySpecCLPushStep_1();
	else if(aStepName == KFullySpecCLPushStep_2) 
		testStep = new CFullySpecCLPushStep_2();
	else if(aStepName == KFullySpecCLPushStep_3) 
		testStep = new CFullySpecCLPushStep_3();
	else if(aStepName == KFullySpecCLPushStep_4) 
		testStep = new CFullySpecCLPushStep_4();
	else if(aStepName == KFullySpecCLPushStep_5) 
		testStep = new CFullySpecCLPushStep_5();
	else if(aStepName == KFullySpecCLPushStep_6) 
		testStep = new CFullySpecCLPushStep_6();
	else if(aStepName == KFullySpecCLPushStep_7) 
		testStep = new CFullySpecCLPushStep_7();
	else if(aStepName == KFullySpecCLPushStep_8) 
		testStep = new CFullySpecCLPushStep_8();
	else if(aStepName == KFullySpecCLPushStep_9) 
		testStep = new CFullySpecCLPushStep_9();

	// Fully spec. WSP [1 - 9]
	else if(aStepName == KFullySpecWSPStep_1) 
		testStep = new CFullySpecWSPStep_1();
	else if(aStepName == KFullySpecWSPStep_2) 
		testStep = new CFullySpecWSPStep_2();
	else if(aStepName == KFullySpecWSPStep_3) 
		testStep = new CFullySpecWSPStep_3();
	else if(aStepName == KFullySpecWSPStep_4) 
		testStep = new CFullySpecWSPStep_4();
	else if(aStepName == KFullySpecWSPStep_5) 
		testStep = new CFullySpecWSPStep_5();
	else if(aStepName == KFullySpecWSPStep_6) 
		testStep = new CFullySpecWSPStep_6();
	else if(aStepName == KFullySpecWSPStep_7)
		testStep = new CFullySpecWSPStep_7();
	else if(aStepName == KFullySpecWSPStep_8) 
		testStep = new CFullySpecWSPStep_8();
	else if(aStepName == KFullySpecWSPStep_9) 
		testStep = new CFullySpecWSPStep_9();

	return testStep;
	}
