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
// This is the test server for Lbs AGPS PSY tests.
// 
//

#include <centralrepository.h>
#include <lbs/test/lbsnetsimtest.h> 

#include "lbssystemcontroller.h"
#include "lbsrootcenrepdefs.h"

#include "te_lbspsyutils.h"
#include "te_lbsagpspsysuite.h"
#include "te_lbsagpspsyposupdatemodestep.h"
#include "te_lbsagpspsyposupdatestep.h"
#include "te_lbspsyposupdatetimingstep.h"
#include "te_lbsagpspsyposupdateerrstep.h"
#include "te_lbspsymodinfostep.h"
#include "te_lbspsypoweradvicestep.h"

/** The string name of the test suite */
_LIT(KServerName, "te_lbsagpspsysuite");

/** The UID of the unit test suite*/
//const TUid  KServerUid = {0x10285ACB};

const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierLS2GpsLocManager = 
	{
		{KLbsGpsLocManagerUidValue},{KLbsLocServerUidValue}
	};


/**
Static factory method for creating an instance of the CTe_LbsAgpsPsySuite class.

@return An instance of the class. The calling application becomes the
owner of the returned instance and is responsible its disposal.

@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsAgpsPsySuite* CTe_LbsAgpsPsySuite::NewL()
	{
	CTe_LbsAgpsPsySuite* server = new (ELeave) CTe_LbsAgpsPsySuite();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

/**
2nd phase constructor. Calls the base class method passing the name of the suite.

@leave If a error happens, it leaves with one of the system error codes.
*/	
void CTe_LbsAgpsPsySuite::ConstructL()
	{
	//ConstructL of the base class
	CTestServer::ConstructL(KServerName);
	
    CTe_SystemStarter::DefineLbsPropertiesL();
	}

CTe_LbsAgpsPsySuite::~CTe_LbsAgpsPsySuite()
	{
    TRAP_IGNORE(CTe_SystemStarter::DeleteLbsPropertiesL());
	}

/**
Overrides the pure virtual CTestServer::CreateTestStep. Creates a test step by its name.

@return - A CTestStep derived instance or NULL if error or there is no a test step with the name
specified.

@see CTestServer::CreateTestStep
*/
CTestStep* CTe_LbsAgpsPsySuite::CreateTestStep(const TDesC& aStepName)
	{
	iCurrentStep = NULL;
	TRAPD(err, iCurrentStep = CreateTestStepL(aStepName));
	if(err!=KErrNone)
		{
		return NULL;
		}
	return iCurrentStep;
	}

/**
The leaving version of the CTe_LbsSuplSmsTriggerSuiteCreateTestStep. The leaving new operator is used 
to initialize the test step object with zeroes.

@return - A CTestStep derived instance or NULL if error or there is no a test step with the name
specified.

@leave KErrNoMemory if not enough memory to create the test step.

@see CTe_LbsAgpsPsySuite::CreateTestStep
*/
CTestStep* CTe_LbsAgpsPsySuite::CreateTestStepL(const TDesC& aStepName)
	{
	if(aStepName==KLbsNotifyPositionUpdateModeStep)
	   	{
	   	return CTe_LbsAgpsPsyPosUpdateModeStep::NewL(this);
	  	}	
	if(aStepName==KLbsNotifyPositionUpdateStep)
	   	{
	   	return CTe_LbsAgpsPsyPosUpdateStep::NewL(this);
	  	} 
	else if(aStepName==KLbsNotifyPositionUpdateTimingStep)
	   	{
	   	return CTe_LbsNotifyPositionUpdateTimingStep::NewL(this);
	  	}
	else if(aStepName==KLbsNotifyPositionUpdateErrStep)
		{
		return CTe_LbsAgpsNotifyPositionUpdateErrStep::NewL(this);
		}
	else if(aStepName==KLbsModuleInfoStep)
		{
		return CTe_LbsModuleInfoStep::NewL(this);
		}
	else if(aStepName==KLbsPowerAdviceStep)
		{
		return CTe_LbsPsyPowerAdviceStep::NewL(this);
		}	
	
	return NULL;
	}


TUid CTe_LbsAgpsPsySuite::ModuleUid() const
	{
	return KLbsGpsLocManagerUid;
	}

const RLbsPositionUpdateRequests::TChannelIdentifer& CTe_LbsAgpsPsySuite::UpdateReqChanId() const
	{
	return KChannelIdentifierLS2GpsLocManager;
	}

TUint CTe_LbsAgpsPsySuite::CountPositioner()
	{
	return iPositionerCount++;
	}

CTestExecuteLogger& CTe_LbsAgpsPsySuite::Logger() const
	{
	__ASSERT_ALWAYS(iCurrentStep, User::Invariant());
	return iCurrentStep->Logger();
	}


/**
Secure variant. Much simpler, uses the new Rendezvous() call to sync with the client.

@leave If a error happens, it leaves with one of the system error codes.
*/
LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CTe_LbsAgpsPsySuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsAgpsPsySuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	
	delete server;
	delete sched;
	}


/**
Secure variant only. Process entry point. Called by client using RProcess API.
 
@return - Standard Epoc error code on process exit.
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }

