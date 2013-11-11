// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file CTLbsFeaturesServer.cpp
// 
//

// System includes
#include <e32std.h>

// User includes
#include "ctlbsfeaturesserver.h"
#include "te_lbsfeaturespsystep.h"
#include "ctlbsfeaturesbtpsystep.h"

/**
  MainL()
  Description :		This is the main function which installs the 
					active scheduler and creates a server
  @internalTechnology
  @param  none
  @return none
  @pre    None
  @post   None
*/
LOCAL_C void MainL()
	{
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	__UHEAP_MARK;
	// this registers the server with the active scheduler and calls SetActive
	CT_LbsFeaturesServer* server = CT_LbsFeaturesServer::NewL();

	// signal to the client that we are ready by
	// rendevousing process
	RProcess::Rendezvous(KErrNone);

	// run the active scheduler
	sched->Start();

	// clean up
	delete server;
	__UHEAP_MARKEND;
	CleanupStack::PopAndDestroy(sched);
	}




/**
  E32Main()
  Description :	It is the entry point 
  @internalTechnology
  @param  none
  @return Returns the error code
  @pre    None
  @post   None
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, MainL());

	delete cleanup;
	__UHEAP_MARKEND;

	return err;
	}
    


/**
  NewL()
  Constructs a CT_LbsFeaturesServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsClientServer
  @pre    None
  @post   None
*/
CT_LbsFeaturesServer* CT_LbsFeaturesServer::NewL()
	{
	CT_LbsFeaturesServer* server = new(ELeave) CT_LbsFeaturesServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsFeaturesTestServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsFeaturesServer::~CT_LbsFeaturesServer()
	{
	}
	
	
/**
  Function : CT_LbsFeaturesServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsFeaturesServer::CT_LbsFeaturesServer()
	{
	}

/**
  Function : ConstructL
  Description : 
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
void CT_LbsFeaturesServer::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	}

	
/**
  Function : CreateTestStep
  Description : Creates a test step based on the step name read from the script file
  @internalTechnology
  @param : aStepName   The step name which needs to be created
  @return : Created object of type CTestStep
  @precondition : none
  @postcondition : none
*/
CTestStep* CT_LbsFeaturesServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Test case test steps, each test step supports one or more test cases.
	if(aStepName == KLbsFeaturesPsy)
		{
		return CT_LbsFeaturesPsyStep::New();
		}
	if (aStepName == KLbsFeaturesBTPSY)	
		{
		return CT_LbsFeaturesBTPSYStep::New();
		}
		
	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return NULL;
	}

