// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test server for calculation tests
// 
//

// User includes
#include "ctlbscalculationserver.h"

// System includes
#include <e32std.h>
#include <rsshared.h>

// Header files of LBS Client API Test Steps
#include "ctlbscalculationtp233.h"
#include "ctlbscalculationtp234.h"
#include "ctlbscalculationtp235.h"
#include "ctlbscalculationtp236.h"

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
	CT_LbsCalculationServer* server = CT_LbsCalculationServer::NewL();

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
  Constructs a CT_LbsCalculationServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsCalculationServer
  @pre    None
  @post   None
*/
CT_LbsCalculationServer* CT_LbsCalculationServer::NewL()
	{
	CT_LbsCalculationServer *server = new(ELeave) CT_LbsCalculationServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsCalculationTestServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsCalculationServer::~CT_LbsCalculationServer()
	{
	}
	
	
/**
  Function : CT_LbsCalculationServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsCalculationServer::CT_LbsCalculationServer()
	{
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
CTestStep* CT_LbsCalculationServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.
    if(aStepName == KLbsCalculationCalculateDistance)   
        {
        return CT_LbsCalculationTP233::New(*this);
        }
    else if(aStepName == KLbsCalculationCalculateBearing)   
        {
        return CT_LbsCalculationTP234::New(*this);
        }
    else if(aStepName == KLbsCalculationCalculateSpeed)   
        {
        return CT_LbsCalculationTP235::New(*this);
        }
    else if(aStepName == KLbsCalculationCoordinateMove)   
        {
        return CT_LbsCalculationTP236::New(*this);
        }

	// Let base class handle any common test steps - will return NULL if test step is not supported.
    return CT_LbsServer::CreateTestStep(aStepName);
	}

