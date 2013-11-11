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
// @file CTLbsAGPSHAIAssDataServer.cpp
// This is the class implementation for the LBS AGPS HAI Validation Test Server
// 
//

// User includes
#include "ctlbsagpshaiassdataserver.h"
#include "ctlbsstartlbsstep.h"
#include "ctlbsstoplbsstep.h"
#include "ctlbsgetmodinfostep.h"
#include "ctlbsrootcenrepmodstep.h"
#include "ctlbsrootcenrepresetstep.h"

// System includes
#include <e32std.h>
#include <rsshared.h>

// Header files of LBS Client API Test Steps
#include "ctlbsrootcenrepmodstep.h"
#include "ctlbsgetmodinfostep.h"
#include "ctlbsrootcenrepresetstep.h"
#include "ctlbsstartlbsstep.h"
#include "ctlbsstoplbsstep.h"
#include "ctlbsagpsassdatastep.h"

//Literals Used
_LIT(KLbsAGPSHAIAssDataServer,"LbsAGPSHAIAssDataTestServer");
_LIT(KLbsAGPSHAIAssDataStep, "LbsAGPSHAIAssDataStep");


class CTestServerBase : public CBase
	{
public:
	CTestServerBase() 
		{
		};
	void ConstructL()
		{
		iServer = CreateServerL();
		}
	virtual ~CTestServerBase()
		{
		delete iServer;
		}
protected:
	virtual CTestServer* CreateServerL();
protected:
	CTestServer*	iServer;
	};


CTestServer* CTestServerBase::CreateServerL()
	{
	return CT_LbsAGPSHAIAssDataServer::NewL();
	}


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
/*	
	// this registers the server with the active scheduler and calls SetActive
	CT_LbsAGPSHAIAssDataServer* server = CT_LbsAGPSHAIAssDataServer::NewL();
*/

	CTestServerBase* s = new (ELeave) CTestServerBase();
	CleanupStack::PushL(s);
	s->ConstructL();
	// signal to the client that we are ready by
	// rendevousing process
	RProcess::Rendezvous(KErrNone);

	// run the active scheduler
	sched->Start();


	CleanupStack::PopAndDestroy(s);
/*
	// clean up
	delete server;
*/
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
  Constructs a CT_LbsAGPSHAIValidateServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsClientServer
  @pre    None
  @post   None
*/
CT_LbsAGPSHAIAssDataServer* CT_LbsAGPSHAIAssDataServer::NewL()
	{
	CT_LbsAGPSHAIAssDataServer*server = new(ELeave) CT_LbsAGPSHAIAssDataServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsAGPSHAIAssDataServer::~CT_LbsAGPSHAIAssDataServer()
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
CT_LbsAGPSHAIAssDataServer::CT_LbsAGPSHAIAssDataServer() : CT_LbsAGPSHAIServerRoot()
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
CTestStep* CT_LbsAGPSHAIAssDataServer::CreateTestStep(const TDesC& aStepName)
	{
	// Main test step:	
	if(aStepName == KLbsAGPSHAIAssDataStep)	
		{
		return CT_LbsAGPSAssDataStep::New(*this);	// NULL if insufficient memory. This suits the API.	
		}
		
	if(aStepName == KLbsGetModuleInfoStep)	
		{
		return CT_LbsAGPSHAIValidateStep_GetModuleInfo::New(*this);	
		}	
				
	if(aStepName == KLbsStartLbsStep)	
		{
		return CT_LbsAGPSHAIValidateStep_StartLbs::New(*this);	
		}	
		
	if(aStepName == KLbsStopLbsStep)	
		{
		return CT_LbsAGPSHAIValidateStep_StopLbs::New(*this);	
		}	
		
	if(aStepName == KLbsRootCenRepModStep)	
		{
		return CT_LbsAGPSHAIValidateStep_RootCenRepMod::New(*this);	
		}				
		
	if(aStepName == KLbsRootCenRepResetStep)	
		{
		return CT_LbsAGPSHAIValidateStep_RootCenRepReset::New(*this);	
		}
						
	return NULL;
	
	}


const TDesC& CT_LbsAGPSHAIAssDataServer::ServerName()
	{
	return KLbsAGPSHAIAssDataServer();
	}
