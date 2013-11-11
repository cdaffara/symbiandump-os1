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
// @file CTLbsHybridMTLRServer.cpp
// This is the class implementation for the LBS Hybrid Combined Test Server
// 
//

// System includes
#include <e32std.h>
#include <rsshared.h>

// User includes
#include "ctlbshybridcombinedserver.h"
#include "ctlbsstepsetuproamselflocate.h"
#include "ctlbsstepconfighybridmodule.h"

// i don't know why this isn't in the base class....
#include "ctlbsstepsetupprotocolstub.h"

#include "ctlbshybridcombinedstep_sequential01.h"
#include "ctlbshybridcombinedstep_sequential02.h"
#include "ctlbshybridcombinedstep_concurrent01.h"
#include "ctlbshybridcombinedstep_concurrent02.h"
#include "ctlbshybridcombinedstep_tracking01.h"
#include "ctlbshybridcombinedstep_tracking02.h"

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
	CT_LbsHybridCombinedServer* server = CT_LbsHybridCombinedServer::NewL();

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
  Constructs a CT_LbsHybridCombinedServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsHybridCombinedServer
  @pre    None
  @post   None
*/
CT_LbsHybridCombinedServer* CT_LbsHybridCombinedServer::NewL()
	{
	CT_LbsHybridCombinedServer*server = new(ELeave) CT_LbsHybridCombinedServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsHybridCombinedTestServer);
	CleanupStack::Pop(server);
	return server;
	}

	
	
CT_LbsHybridCombinedServer::~CT_LbsHybridCombinedServer()
	{
	}
	
	
/**
  Function : CT_LbsHybridCombinedServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsHybridCombinedServer::CT_LbsHybridCombinedServer()
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
void CT_LbsHybridCombinedServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
	
	iSharedData = CT_LbsHybridCombinedSharedData::NewL();
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
CTestStep* CT_LbsHybridCombinedServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Test case test steps, each test step supports one or more test cases.
	if (aStepName == KLbsStep_SetupRoamSelfLocate)	
		{
		return CT_LbsStep_SetupRoamSelfLocate::New(*this);
		}	
	else if(aStepName == KLbsStep_SetupProtocolStub)	
		{
		return CT_LbsStep_SetupProtocolStub::New(*this);
		}	
	else if(aStepName == KLbsStep_ConfigHybridModule)	
		{
		return CT_LbsStep_ConfigHybridModule::New(*this);
		}	

	else if(aStepName == KLbsHybridCombinedStep_Sequential01)	
		{
		return CT_LbsHybridCombinedStep_Sequential01::New(*this);
		}
   else if(aStepName == KLbsHybridCombinedStep_Sequential02) 
        {
        return CT_LbsHybridCombinedStep_Sequential02::New(*this);
        }
   else if(aStepName == KLbsHybridCombinedStep_Concurrent01) 
        {
        return CT_LbsHybridCombinedStep_Concurrent01::New(*this);
        }
   else if(aStepName == KLbsHybridCombinedStep_Concurrent02) 
        {
        return CT_LbsHybridCombinedStep_Concurrent02::New(*this);
        }
   else if(aStepName == KLbsHybridCombinedStep_Tracking01) 
        {
        return CT_LbsHybridCombinedStep_Tracking01::New(*this);
        }
   else if(aStepName == KLbsHybridCombinedStep_Tracking02) 
        {
        return CT_LbsHybridCombinedStep_Tracking02::New(*this);
        }

	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}

