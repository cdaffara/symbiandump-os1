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
// @file CT_LbsPrivFwServer.cpp
// This is the class implementation for the LBS Client Test Server
// 
//

// User includes
#include "ctlbsprivfwserver.h"

// System includes
#include <e32std.h>
#include <rsshared.h>

#include "ctlbsprivfwstepbase.h"
#include "ctlbsprivfwstepnotify.h"
#include "ctlbsprivfwstepverifynotify.h"
#include "ctlbsprivfwstepverify.h"
#include "ctlbsprivfwstepparam.h"
#include "ctlbsprivfwstepsettings.h"
#include "ctlbsprivfwsteptimeout.h"
#include "ctlbsprivfwstepramusage.h"
#include "ctlbsprivfwstepperftimings.h"
#include "ctlbsprivfwstepcancel.h"
#include "ctlbsprivfwstepverifybadstrings.h"
#include "ctlbsprivfwstepverifyreturnerrors.h"
#include "ctlbsprivfwstepctlrclosedown.h"
#include "ctlbsprivfwstepprivtimeout.h"
#include "ctlbsprivfwstepnoinstall.h"
#include "ctlbsstepinitializetestchannel.h"
#include "ctlbsstepclosetestchannel.h"

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
	CT_LbsPrivFwServer* server = CT_LbsPrivFwServer::NewL();

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
CT_LbsPrivFwServer* CT_LbsPrivFwServer::NewL()
	{
	CT_LbsPrivFwServer*server = new(ELeave) CT_LbsPrivFwServer();
	CleanupStack::PushL(server);
	server->ConstructL(KLbsPrivFwTestServer);
	CleanupStack::Pop(server);
	return server;
	}


CT_LbsPrivFwServer::~CT_LbsPrivFwServer()
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
CT_LbsPrivFwServer::CT_LbsPrivFwServer()
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
void CT_LbsPrivFwServer::ConstructL(const TDesC& aName)
	{
	CT_LbsServer::ConstructL(aName);
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
CTestStep* CT_LbsPrivFwServer::CreateTestStep(const TDesC& aStepName)
	{
	// NULL if insufficient memory. This suits the API.

	// Test case test steps, each test step supports one or more test cases.
	if(aStepName == KLbsPrivFwStep_Notify)	
		{
		return CT_LbsPrivFwStepNotify::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_VerifyNotify)	
		{
		return CT_LbsPrivFwStepVerifyNotify::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_Verify)	
		{
		return CT_LbsPrivFwStepVerify::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_Param)	
		{
		return CT_LbsPrivFwStepParam::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_Settings)	
		{
		return CT_LbsPrivFwStepSettings::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_VerifyBadStrings)	
		{
		return CT_LbsPrivFwStepVerifyBadStrings::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_VerifyReturnErrors)	
		{
		return CT_LbsPrivFwStepVerifyReturnErrors::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_RamUsage)	
		{
		return CT_LbsPrivFwStepRamUsage::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_PerfTimings)	
		{
		return CT_LbsPrivFwStepPerfTimings::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_Cancel)
		{
		return CT_LbsPrivFwStepCancel::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_Timeout)	
		{
		return CT_LbsPrivFwStepTimeout::New(*this);
		}
	else if(aStepName == KLbsPrivFwStep_PrivTimeout)	
		{
		return CT_LbsPrivFwStepPrivTimeout::New(*this);
		}
    else if(aStepName == KLbsPrivFwStep_CtlrCloseDown)	
		{
		return CT_LbsPrivFwStepCtlrCloseDown::New(*this);
		}
    else if(aStepName == KLbsPrivFwStep_NoInstall)	
		{
		return CT_LbsPrivFwStepNoInstall::New(*this);
		}
	else if (aStepName == KLbsStep_InitializeTestChannel)
    	{
    	return CT_LbsStep_InitializeTestChannel::New(*this);
    	}
    else if (aStepName == KLbsStep_CloseTestChannel)
    	{
    	return CT_LbsStep_CloseTestChannel::New(*this);
    	}

	// Let base class handle any common test steps - will return NULL if test step is not supported.
	return CT_LbsServer::CreateTestStep(aStepName);
	}

