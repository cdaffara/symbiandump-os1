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
// @file CTLbsHybridConflictSrv.cpp
// This is the class implementation for the LBS Hybrid Conflict Test Server
// 
//

// System includes
#include <e32std.h>
#include <rsshared.h>

// User includes
#include "ctlbshybridconflictsrv.h"
#include "ctlbsMoLrIntByX3P.h"
#include "ctlbsX3PIntByMoLr.h"

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
	CT_LbsHybridConflictServer* server = CT_LbsHybridConflictServer::NewL();

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
  Constructs a CT_LbsHybridConflictServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsClientServer
  @pre    None
  @post   None
*/
CT_LbsHybridConflictServer* CT_LbsHybridConflictServer::NewL()
	{
	CT_LbsHybridConflictServer*server = new(ELeave) CT_LbsHybridConflictServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsHybridConflictServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsHybridConflictServer::~CT_LbsHybridConflictServer()
	{
	}
	
	
/**
  Function : CT_LbsHybridConflictServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsHybridConflictServer::CT_LbsHybridConflictServer()
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
void CT_LbsHybridConflictServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
	
	iSharedData = CT_LbsHybridConflictSharedData::NewL();
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
CTestStep* CT_LbsHybridConflictServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Test case test steps, each test step supports one or more test cases.
	if(aStepName == KLbsMoLrIntByX3P)	
		{
		return CT_LbsMoLrIntByX3P::New(*this);
		}
	else if (aStepName == KLbsX3PIntByMoLr)
		{
		return CT_LbsX3PIntByMoLr::New(*this);
		}

	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}

