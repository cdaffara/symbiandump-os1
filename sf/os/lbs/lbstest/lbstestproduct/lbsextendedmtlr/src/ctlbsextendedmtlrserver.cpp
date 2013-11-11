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
// @file CTLbsExtendedMTLRServer.cpp
// This is the class implementation for the LBS Extended MTLR Test Server
// 
//

// System includes
#include <e32std.h>
#include <rsshared.h>
#include <lbs/test/lbsnetextendedprotocolproxy.h>
// User includes
#include "ctlbsextendedmtlrserver.h"
#include "ctlbsstepsetupprotocolstub.h"
#include "ctlbsstepconfighybridmodule.h"
#include "ctlbsextendedmtlrcancel.h"
#include "ctlbsextptbmtlrcancel.h"
#include "ctlbscancelbeforereq.h"
#include "ctlbsemergencycancel.h"
#include "ctlbsexthybridmtlrcancel.h"
#include "ctlbshybridemergencycancel.h"
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
	CT_LbsExtendedMTLRServer* server = CT_LbsExtendedMTLRServer::NewL();

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
  Constructs a CT_LbsExtendedMTLRServer object.
  Uses two phase construction and leaves nothing on the CleanupStack.
  @internalTechnology
  @param  none
  @return Created object of type CT_LbsClientServer
  @pre    None
  @post   None
*/
CT_LbsExtendedMTLRServer* CT_LbsExtendedMTLRServer::NewL()
	{
	CT_LbsExtendedMTLRServer*server = new(ELeave) CT_LbsExtendedMTLRServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsExtendedMTLRTestServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsExtendedMTLRServer::~CT_LbsExtendedMTLRServer()
	{
	}
	
	
/**
  Function : CT_LbsExtendedMTLRServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsExtendedMTLRServer::CT_LbsExtendedMTLRServer()
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
void CT_LbsExtendedMTLRServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
	
//	iSharedData = CT_LbsExtendedMTLRSharedData::NewL();
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
CTestStep* CT_LbsExtendedMTLRServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Test case test steps, each test step supports one or more test cases
	if(aStepName == KLbsStep_SetupProtocolStub)	
		{ 
		return CT_LbsStep_SetupProtocolStub::New(*this);
		}	
	else if(aStepName == KLbsStep_ConfigHybridModule)	
		{
		return CT_LbsStep_ConfigHybridModule::New(*this);
		}	
	
	else if(aStepName == KLbsExtendedMtlrCancel)
		{
		return CT_LbsExtendedMTLRCancel::New(*this);
		}
	else if(aStepName == KLbsExtPtbMtlrCancel)
		{
		return CT_LbsExtPtbMTLRCancel::New(*this);
		}
	else if(aStepName == KLbsCancelBeforeReq)
		{
		return CT_LbsCancelBeforeReq::New(*this);
		}
	else if(aStepName == KLbsEmergencyCancel)
		{
		return CT_LbsEmergencyCancel::New(*this);
		}
	else if(aStepName == KLbsExtHybridMtlrCancel)
		{
		return CT_LbsExtHybridMTLRCancel::New(*this);
		}
	else if(aStepName == KLbsHybridEmergencyCancel)
		{
		return CT_LbsHybridEmergencyCancel::New(*this);
		}
	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}
