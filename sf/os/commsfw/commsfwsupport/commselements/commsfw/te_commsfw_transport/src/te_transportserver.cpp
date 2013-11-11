// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_TransportServer.cpp
*/

#include "te_TransportServer.h"
#include "te_TransportStep.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemCommsFwtrnsp, "ElemCommsFwtrnsp");
#endif


_LIT(KServerName,"Te_Commsfw_Transport");
CCommsTransportTestServer* CCommsTransportTestServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CCommsTransportTestServer* server = new (ELeave) CCommsTransportTestServer();
	CleanupStack::PushL(server);
	server->StartL(KServerName);
	CleanupStack::Pop();
	return server;
	}
	
	
CCommsTransportTestServer::~CCommsTransportTestServer()
	{
	
	}
	
CTestStep* CCommsTransportTestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName.Compare(_L("CTransportTestStep1_1")) == 0)
		testStep=new CTransportTestStep1_1();
	else if(aStepName.Compare(_L("CTransportTestStep1_2")) == 0)
		testStep=new CTransportTestStep1_2();
	else if(aStepName.Compare(_L("CTransportTestStep1_3")) == 0)
		testStep=new CTransportTestStep1_3();
	else if(aStepName.Compare(_L("CTransportTestStep1_4")) == 0)
		testStep=new CTransportTestStep1_4();
	else if(aStepName.Compare(_L("CTransportTestStep1_5")) == 0)
		testStep=new CTransportTestStep1_5();
	else if(aStepName.Compare(_L("CTransportTestStep1_6")) == 0)
		testStep=new CTransportTestStep1_6();
	else if(aStepName.Compare(_L("CTransportTestStep1_7")) == 0)
		testStep=new CTransportTestStep1_7();
	else if(aStepName.Compare(_L("CTransportTestStep2_1")) == 0)
		testStep=new CTransportTestStep2_1();
	else if(aStepName.Compare(_L("CTransportTestStep3_5")) == 0)
		testStep=new CTransportTestStep3_5();
	else if(aStepName.Compare(_L("CTransportTestStep3_6")) == 0)
		testStep=new CTransportTestStep3_6();
	else if(aStepName.Compare(_L("CTransportTestStep4_1")) == 0)
		testStep=new CTransportTestStep4_1();
	else if(aStepName.Compare(_L("CTransportTestStep5_1")) == 0)
		testStep=new CTransportTestStep5_1();
	else if(aStepName.Compare(_L("CTransportTestStep5_3")) == 0)
		testStep=new CTransportTestStep5_3();
	else if(aStepName.Compare(_L("CTransportTestStep7_1")) == 0)
		testStep=new CTransportTestStep7_1();
	else if(aStepName.Compare(_L("CTransportTestStep7_2")) == 0)
		testStep=new CTransportTestStep7_2();
	else
		__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ElemCommsFwtrnsp, 1)); // your script is broken
	return testStep;
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
	CCommsTransportTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CCommsTransportTestServer::NewL());
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


