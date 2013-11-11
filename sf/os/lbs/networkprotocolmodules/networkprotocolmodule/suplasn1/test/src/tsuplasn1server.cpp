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
 @file
 @internalComponent - Internal Symbian test code 
*/


#include "tsuplasn1server.h"
#include "tsuplasn1step.h"

CSuplAsn1Server* CSuplAsn1Server::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
	{
	CSuplAsn1Server * server = new (ELeave) CSuplAsn1Server();
	CleanupStack::PushL(server);
	// CServer base class call
	TParsePtrC serverName(RProcess().FileName());	
	server->StartL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

// __EDIT_ME__ - Use your own server class name
CTestStep* CSuplAsn1Server::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// User::After(TTimeIntervalMicroSeconds32(5000000));
	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created 
	
	if(aStepName == KSuplMessageStep)
		{
		testStep = new CSuplMessageStep();
		}
		
	if(aStepName == KRrcPayLoadStep)
		{
		testStep = new CRrcPayloadStep();
		}
			
	return testStep;
	}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
   Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSuplAsn1Server* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSuplAsn1Server::NewL());;
	
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

// Only a DLL on emulator for typhoon and earlier
GLDEF_C TInt E32Main()
/**
   @return - Standard Epoc error code on exit
 */
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	// This if statement is here just to shut up RVCT, which would otherwise warn
	// that err was set but never used
	if (err)
	    {
	    err = KErrNone;
	    }
	delete cleanup;
	return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier
