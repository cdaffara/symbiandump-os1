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
// @file CT_LbsX3PServer.cpp
// This is the class implementation for the LBS Client Test Server
// 
//

// User includes
#include "ctlbsx3pserver.h"

// System includes
#include <e32std.h>
#include <rsshared.h>

// Header files of LBS Client API Test Steps
#include "ctlbsx3pstepopenclose.h"
#include "ctlbsx3pstepconflict.h"
#include "ctlbsx3pstepcancel.h"
#include "ctlbsx3psteptransmit.h"
#include "ctlbsx3pstepnegative.h"
#include "ctlbsx3psteptransmitconcurrent.h"

/**
  MainL()
  Description :		This is the main function which installs the 
					active scheduler and creates an object of the Email server
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
	CT_LbsX3PServer* server = CT_LbsX3PServer::NewL();

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
  Constructs a CT_LbsClientServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsClientServer
  @pre    None
  @post   None
*/
CT_LbsX3PServer* CT_LbsX3PServer::NewL()
	{
	CT_LbsX3PServer*server = new(ELeave) CT_LbsX3PServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsX3PTestServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsX3PServer::~CT_LbsX3PServer()
	{
	}
	
	
/**
  Function : CT_LbsClientServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsX3PServer::CT_LbsX3PServer()
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
void CT_LbsX3PServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
	
	iSharedData = CT_LbsX3PSharedData::NewL();
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
CTestStep* CT_LbsX3PServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Test case test steps, each test step supports one or more test cases.
	if(aStepName == KLbsX3PStep_OpenClose)	
		{
		return CT_LbsX3PStep_OpenClose::New(*this);
		}
	if(aStepName == KLbsX3PStep_Cancel)	
		{
		return CT_LbsX3PStep_Cancel::New(*this);
		}
	if(aStepName == KLbsX3PStep_Conflict)	
		{
		return CT_LbsX3PStep_Conflict::New(*this);
		}
	if(aStepName == KLbsX3PStep_Transmit)	
		{
		return CT_LbsX3PStep_Transmit::New(*this);
		}
	if(aStepName == KLbsX3P_Step_Negative)	
		{
		return CT_LbsX3PStep_Negative::New(*this);
		}
	if (aStepName == KLbsX3PStep_Transmit_Concurrent)
		{
		return CT_LbsX3PStep_Transmit_Concurrent::New();
		}	
    
	
	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}

