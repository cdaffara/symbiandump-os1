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
// This is the class implementation for the LBS Hybrid MOLR Test Server
// 
//

// System includes
#include <e32std.h>
#include <rsshared.h>

// User includes
#include "ctlbshybridmolrserver.h"
#include "ctlbsstepsetuproamselflocate.h"
#include "ctlbsstepconfighybridmodule.h"

#include "ctlbsuebasedmolrself.h"
#include "ctlbsueassistedmolrselfgpslate.h"
#include "ctlbsueassistedmolrselfgpsinaccurate.h"
#include "ctlbsstepsetupprotocolstub.h"
#include "ctlbshybridueassistedmolrgpsok.h"
#include "ctlbsueassistedmolrselfgpsinaccuratetimeout.h"
#include "ctlbsueassistedmolrselfgpslatefacilitytimeout.h"
#include "ctlbsautonomousmolrfutile.h"
#include "ctlbshybridmolrfutile.h"
#include "ctlbshybriduebasedmolrpartial.h"
#include "ctlbshybridueassistedmolrpartial.h"
#include "ctlbshybridueassistedmolrclienttimeout.h"
#include "ctlbshybridueassistedmolrtimeout.h"
#include "ctlbsmolrgpsoptions.h"
#include "ctlbshybridueassistedmolrnogpsupdate.h"
#include "ctlbsptamolrpartialearlycomplete.h"
#include "ctlbsmolrtracking.h"
#include "ctlbsmolrresetassistance.h"
#include "ctlbsuebasedmolrselfinternalvaluestest.h"
#include "ctlbssuplmolrtbfutilesp.h"
#include "ctlbsuebasedmolronesl.h"
#include "ctlbsmolrerrorassistance.h"
#include "ctlbshybridueassistednpudposstatus.h"

/**
  NewL()
  Constructs a CT_LbsHybridMOLRServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsClientServer
  @pre    None
  @post   None
*/
CT_LbsHybridMOLRServer* CT_LbsHybridMOLRServer::NewL()
	{
	CT_LbsHybridMOLRServer*server = new(ELeave) CT_LbsHybridMOLRServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsHybridMOLRTestServer);
	CleanupStack::Pop(server);
	return server;
	}


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
	CT_LbsHybridMOLRServer* server = CT_LbsHybridMOLRServer::NewL();

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


CT_LbsHybridMOLRServer::~CT_LbsHybridMOLRServer()
	{
	}
	
	
/**
  Function : CT_LbsHybridMOLRServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsHybridMOLRServer::CT_LbsHybridMOLRServer()
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
void CT_LbsHybridMOLRServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
	
	iSharedData = CT_LbsHybridMOLRSharedData::NewL();
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
CTestStep* CT_LbsHybridMOLRServer::CreateTestStep(const TDesC& aStepName)
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

	else if(aStepName == KLbsUEBasedMOLRSelf)	
		{
		return CT_LbsUEBasedMOLRSelf::New(*this);
		}
	else if (aStepName == KLbsUEAssistedMOLRSelfGPSLate)
		{
		return CT_LbsUEAssistedMOLRSelfGPSLate::New(*this);
		}
	else if (aStepName == KLbsUEAssistedMOLRSelfGPSInaccurate)
		{
		return CT_LbsUEAssistedMOLRSelfGPSInaccurate::New(*this);
		}
	else if(aStepName == KLbsUEAssistedMOLRSelfGPSInaccurateT)	
		{
		return CT_LbsUEAssistedMOLRSelfGPSInaccurateT::New(*this);
		}

	else if(aStepName == KLbsHybridUEAssistedMOLRGPSOk)	
		{
		return CT_LbsHybridUEAssistedMOLRGPSOk::New(*this);
		}

	else if (aStepName == KLbsAutonomousMOLRFutile)
		{
		return CT_LbsAutonomousMOLRFutile::New(*this);
		}

	else if (aStepName == KLbsHybridUEBasedMOLRPartial)
		{
		return CT_LbsHybridUEBasedMOLRPartial::New(*this);
		}
	else if (aStepName == KLbsHybridUEAssistedMOLRPartial)
		{
		return CT_LbsHybridUEAssistedMOLRPartial::New(*this);
		}
	else if (aStepName == KLbsUEAssistedMOLRSelfGPSLateFacTimeout)
	    {
		return CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout::New(*this);
	    }
	else if (aStepName == KLbsHybridMOLRFutile)	
		{
		return CT_LbsHybridMOLRFutile::New(*this);		
		}	
	else if (aStepName == KLbsUEAssistedMOLRClientTimeout)
		{
		return CT_LbsUEAssistedMOLRClientTimeout::New(*this);		
		}	
	else if (aStepName == KLbsHybridUEAssistedMOLRTimeOut)
		{
		return CT_LbsHybridUEAssistedMOLRTimeOut::New(*this);
		}
	else if (aStepName == KLbsMolrGpsOptions)
		{
		return CT_LbsMolrGpsOptions::New(*this);
		}
	else if (aStepName == KLbsHybridUEAssistedMOLRNoGPSUpdate)
		{
		return CT_LbsHybridUEAssistedMOLRNoGPSUpdate::New(*this);
		}
	else if (aStepName == KLbsPTAMOLRPartialEarlyComplete)
		{
		return CT_LbsPTAMOLRPartialEarlyComplete::New(*this);
		}
	else if (aStepName == KLbsMolrTracking)
		{
		return CT_LbsMolrTracking::New(*this);
		}
	else if (aStepName == KLbsMolrResetAssistance)
		{
		return CT_LbsMolrResetAssistance::New(*this);
		}
	else if (aStepName == KLbsUEBasedMOLRSelfInternalValueTest)
		{
		return CT_LbsUEBasedMOLRSelfInternalValueTest::New(*this);
		}
	else if (aStepName == KLbsSuplMolrTbFutileSp)
		{
		return CT_LbsSuplMolrTbFutileSp::New(*this);
		}
	else if (aStepName == KLbsUEBasedMOLROneSL)
		{
		return CT_LbsUEBasedMOLROneSL::New(*this);
		}
	else if (aStepName == KLbsMOLRErrorAssistance)
	    {
	    return CT_LbsMolrErrorAssistance::New(*this);
	    }
	else if (aStepName == KLbsHybridUEAssistedNpudPosStatus)
	    {
	    return CT_LbsHybridUEAssistedNpudPosStatus::New(*this);
	    }
		
	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}

