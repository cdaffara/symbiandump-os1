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
// @file ctlbshybridx3pserver.h.cpp
// System includes
// 
//

#include <e32std.h>
#include <rsshared.h>

// User includes
#include "ctlbshybridx3pserver.h"
#include "ctlbsuebasedx3p.h"
#include "ctlbshybridueassistedx3pnogps.h"
#include "ctlbshybridueassistedx3paccurategps.h"
#include "ctlbsstepsetupprotocolstub.h"
#include "ctlbsstepconfighybridmodule.h"
#include "ctlbshybridueassistedx3pnogpstimeout.h"
#include "ctlbshybridueassistedx3paccurategpstimeout.h"
#include "ctlbshybriduebasedx3pgpsfutile.h"
#include "ctlbshybridueassistedx3pgpsfutile.h"
#include "ctlbshybridueassistedx3pclienttimeout.h"
#include "ctlbsx3pAccRefPos.h"
#include "ctlbsx3pgpsoptions.h"
#include "ctlbsx3presetassistance.h"

// Header files of LBS Client API Test Steps

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
 	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	__UHEAP_MARK;
	// this registers the server with the active scheduler and calls SetActive
	CT_LbsHybridX3PServer* server = CT_LbsHybridX3PServer::NewL();

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
CT_LbsHybridX3PServer* CT_LbsHybridX3PServer::NewL()
	{
	CT_LbsHybridX3PServer* server = new(ELeave) CT_LbsHybridX3PServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsHybridX3PTestServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsHybridX3PServer::~CT_LbsHybridX3PServer()
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
CT_LbsHybridX3PServer::CT_LbsHybridX3PServer()
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
void CT_LbsHybridX3PServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
	iSharedData = CT_LbsSharedData::NewL();	
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
CTestStep* CT_LbsHybridX3PServer::CreateTestStep(const TDesC& aStepName)
	{
	if(aStepName == KLbsStep_SetupProtocolStub)	
		{
		return CT_LbsStep_SetupProtocolStub::New(*this);
		}	
	else if(aStepName == KLbsStep_ConfigHybridModule)	
		{
		return CT_LbsStep_ConfigHybridModule::New(*this);
		}
	else if (aStepName == KLbsUEBasedX3P)
		{
		return CT_LbsUEBasedX3P::New(*this);
		}
	else if(aStepName == KLbsHybridUEAssistedX3PNoGPS)
		{
		return CT_LbsHybridUEAssistedX3PNoGPS::New(*this);
		}	
	else if(aStepName == KLbsHybridUEAssistedX3PAccurateGPS)
		{
		return CT_LbsHybridUEAssistedX3PAccurateGPS::New(*this);
		}	
	else if(aStepName == KLbsHybridUEAssistedX3PNoGPSTimeout)	
		{
		return CT_LbsHybridUEAssistedX3PNoGPSTimeout::New(*this);
		}
	else if(aStepName == KLbsHybridUEAssistedX3PAccurateGPSTimeout)
		{
		return CT_LbsHybridUEAssistedX3PAccurateGPSTimeout::New(*this);
		}		
	else if (aStepName == KLbsHybridUEBasedX3PGPSFutile)
		{
		return CT_LbsHybridUEBasedX3PGPSFutile::New(*this);
		}
	else if (aStepName == KLbsHybridUEAssistedX3PGPSFutile)
		{
		return CT_LbsHybridUEAssistedX3PGPSFutile::New(*this);
		}
	else if(aStepName == KLbsHybridUEAssistedX3PClientTimeout)	
		{
		return CT_LbsHybridUEAssistedX3PClientTimeout::New(*this);
		}
	else if(aStepName == KLbsX3pAccurateReferencePosition)	
		{
		return CT_LbsX3pAccRefPos::New(*this);
		}
	else if (aStepName == KLbsX3PGpsOptions)
		{
		return CT_LbsX3PGpsOptions::New(*this);
		}	
	else if (aStepName == KLbsX3PResetAssistance)
		{
		return CT_LbsX3PResetAssistance::New(*this);
		}	
	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}

