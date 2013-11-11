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
// @file ctlbsconflictserver.cpp
// This is the class implementation for the LBS Conflict Test Server
// 
//

// User includes
#include "ctlbsconflictserver.h"

// System includes
#include <e32std.h>
#include <rsshared.h>

// Header files of LBS Client API Test Steps
#include "ctlbsconflictstepexample.h"
#include "ctlbsconflictstepx3ppushselflocate.h"
#include "ctlbsconflictstepx3pmenupush.h"
#include "ctlbsconflictstepx3ptimerpush.h"
#include "ctlbsconflictstepx3ppushtimer.h"
#include "ctlbsconflictstepx3ppushmenu.h"
#include "ctlbsconflictstepx3ppushpush.h"
#include "ctlbsconflictstepx3pmenumenu.h"
#include "ctlbsconflictstepselflocatex3ppush.h"
#include "ctlbsconflictstepmtlrx3ppush.h"
#include "ctlbsconflictstepcanceltracking.h"
#include "ctlbsconflictstepcanceltrackingconflict.h"
#include "ctlbsconflictstepcanceltrackingconflictb.h"
#include "ctlbsconflictstepmtlrnetlocreq.h"

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
	CT_LbsConflictServer* server = CT_LbsConflictServer::NewL();

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
CT_LbsConflictServer* CT_LbsConflictServer::NewL()
	{
	CT_LbsConflictServer*server = new(ELeave) CT_LbsConflictServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsConflictTestServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsConflictServer::~CT_LbsConflictServer()
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
CT_LbsConflictServer::CT_LbsConflictServer()
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
void CT_LbsConflictServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
	
	iSharedData = CT_LbsConflictSharedData::NewL();
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
CTestStep* CT_LbsConflictServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Test case test steps, each test step supports one or more test cases.
	if(aStepName == KLbsConflictStep_Example)	
		{
		return CT_LbsConflictStep_Example::New(*this);
		}
	if(aStepName == KLbsConflictStepselflocatex3ppush)	
		{
		return CT_LbsConflictStep_selflocatex3ppush::New(*this);
		}
	if(aStepName == KLbsConflictStepx3ppushselflocate)	
		{
		return CT_LbsConflictStepx3ppushselflocate::New(*this);
        }
	if (aStepName == KLbsConflictStep_X3PMenuPush)
		{
		return CT_LbsConflictStep_X3PMenuPush::New(*this);
		}
	if (aStepName == KLbsConflictStep_X3PTimerPush)
		{
		return CT_LbsConflictStep_X3PTimerPush::New(*this);
		}
	if(aStepName == KLbsConflictStep_X3PPushTimer)	
		{
		return CT_LbsConflictStep_X3PPushTimer::New(*this);
		}
	if (aStepName == KLbsConflictStep_MTLRX3PPush)
		{
		return CT_LbsConflictStep_MTLRX3PPush::New(*this);
		}
	if (aStepName == KLbsConflictStep_X3PPushMenu)	
		{
		return CT_LbsConflictStep_X3PPushMenu::New(*this);
		}
	if (aStepName == KLbsConflictStep_X3PPushPush)	
		{
		return CT_LbsConflictStep_X3PPushPush::New(*this);
		}
	if (aStepName == KLbsConflictStep_X3PMenuMenu)	
		{
		return CT_LbsConflictStep_X3PMenuMenu::New(*this);
		}

	if(aStepName == KLbsConflictStep_canceltracking)	
		{
		return CT_LbsConflictStep_canceltracking::New(*this);
		}
	if(aStepName == KLbsConflictStep_canceltrackingconflict)	
		{
		return CT_LbsConflictStep_canceltrackingconflict::New(*this);
		}
	if(aStepName == KLbsConflictStep_canceltrackingconflictb)	
		{
		return CT_LbsConflictStep_canceltrackingconflictb::New(*this);
		}
		
 	if(aStepName == KLbsConflictStep_MTLRNETLOCREQ)	
 		{
 		return CT_LbsConflictStep_MTLRNETLOCREQ::New(*this);
 		}		
	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}

