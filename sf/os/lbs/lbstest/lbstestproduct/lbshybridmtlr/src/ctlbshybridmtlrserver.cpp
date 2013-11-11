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
// This is the class implementation for the LBS Hybrid MTLR Test Server
// 
//

// System includes
#include <e32std.h>
#include <rsshared.h>

// User includes
#include "ctlbshybridmtlrserver.h"
#include "ctlbsuebasedmtlr.h"
#include "ctlbshybridueassistedmtlrnw.h"
#include "ctlbshybridueassistedmtlrgpsok.h"
#include "ctlbsstepsetupprotocolstub.h"
#include "ctlbsstepconfighybridmodule.h"
#include "ctlbshybridueassistedmtlrtimeout.h"
#include "ctlbshybridueassistedmtlrnogps.h"
#include "ctlbshybridmtlrfutile.h"
#include "ctlbsmtlrAccRefPos.h"
#include "ctlbsmtlrgpsoptions.h"
#include "ctlbsmtlrresetassistance.h"
#include "ctlbsnetworkinducedmtlr.h"
#include "ctlbsoom.h"
#include "ctlbshybridueassistedmtlrposstatus.h"

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
	CT_LbsHybridMTLRServer* server = CT_LbsHybridMTLRServer::NewL();

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
  Constructs a CT_LbsHybridMTLRServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsClientServer
  @pre    None
  @post   None
*/
CT_LbsHybridMTLRServer* CT_LbsHybridMTLRServer::NewL()
	{
	CT_LbsHybridMTLRServer*server = new(ELeave) CT_LbsHybridMTLRServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsHybridMTLRTestServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsHybridMTLRServer::~CT_LbsHybridMTLRServer()
	{
	}
	
	
/**
  Function : CT_LbsHybridMTLRServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsHybridMTLRServer::CT_LbsHybridMTLRServer()
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
void CT_LbsHybridMTLRServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
	
	iSharedData = CT_LbsHybridMTLRSharedData::NewL();
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
CTestStep* CT_LbsHybridMTLRServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Test case test steps, each test step supports one or more test cases.
	if(aStepName == KLbsUEBasedMTLR)	
		{
		return CT_LbsUEBasedMTLR::New(*this);
		}
	else if(aStepName == KLbsHybridUEAssitedMTLRNW)	
		{
		return CT_LbsHybridUEAssistedMTLRNW::New(*this);
		}	
	else if(aStepName == KLbsHybridUEAssistedMTLRGPSOK)	
		{
		return CT_LbsHybridUEAssistedMTLRGPSOK::New(*this);
		}	
	else if(aStepName == KLbsStep_SetupProtocolStub)	
		{
		return CT_LbsStep_SetupProtocolStub::New(*this);
		}	
	else if(aStepName == KLbsStep_ConfigHybridModule)	
		{
		return CT_LbsStep_ConfigHybridModule::New(*this);
		}	
	else if(aStepName == KLbsHybridUEAssitedMTLRTimeout)
		{
		return CT_LbsHybridUEAssistedMTLRTimeout::New(*this);
		}
	else if(aStepName == KLbsHybridUEAssitedMTLRNoGPS)
		{
		return CT_LbsHybridUEAssistedMTLRNoGPS::New(*this);
		}
	else if(aStepName == KLbsHybridMTLRFutile)
		{
		return CT_LbsHybridMTLRFutile::New(*this);
		}
	else if(aStepName == KLbsMTLRAccurateReferencePosition)
		{
		return CT_LbsMtlrAccRefPos::New(*this);
		}
	else if(aStepName == KLbsMTLRGpsOptions)
		{
		return CT_LbsMTLRGpsOptions::New(*this);
		}	
	else if(aStepName == KLbsMTLRResetAssistance)
		{
		return CT_LbsMTLRResetAssistance::New(*this);
		}	
	else if (aStepName == KLbsNetworkInducedMTLR)
		{
		return CT_LbsNetworkInducedMTLR::New(*this);		
		}
	else if (aStepName == KLbsOom)
		{
		return CT_LbsOom::New(*this);		
		} 
	else if (aStepName == KLbsHybridUEAssistedMTLRPosStatus)
		{
		return CT_LbsHybridUEAssistedMTLRPosStatus::New(*this);		
		} 

	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}
