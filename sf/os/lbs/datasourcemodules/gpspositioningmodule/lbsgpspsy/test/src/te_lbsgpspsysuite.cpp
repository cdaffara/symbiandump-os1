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
// This is the test server for Lbs GPS PSY tests.
// 
//

#include <centralrepository.h>
#include <lbs/test/lbsnetsimtest.h> 
#include <test/TestExecuteStepBase.h>

#include "lbssystemcontroller.h"
#include "lbsrootcenrepdefs.h"

#include "te_lbspsyutils.h"
#include "te_lbsgpspsysuite.h"
#include "te_lbspsyposupdatestep.h"
#include "te_lbspsyposupdatetimingstep.h"
#include "te_lbspsyposupdateerrstep.h"
#include "te_lbspsymodinfostep.h"
#include "te_lbspsypoweradvicestep.h"

/** The string name of the test suite */
_LIT(KServerName, "te_lbsgpspsysuite");

/** The UID of the unit test suite*/
//const TUid  KServerUid = {0x102869C5};

// TODO - put this somewhere that the gps and agps test suites can share it?
const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierLS2GpsLocManager = 
	{
		{KLbsGpsLocManagerUidValue},{KLbsLocServerUidValue}
	};


/**
Static factory method for creating an instance of the CTe_LbsGpsPsySuite class.

@return An instance of the class. The calling application becomes the
owner of the returned instance and is responsible its disposal.

@leave If a error happens, it leaves with one of the system error codes.
*/
CTe_LbsGpsPsySuite* CTe_LbsGpsPsySuite::NewL()
	{
	CTe_LbsGpsPsySuite* server = new (ELeave) CTe_LbsGpsPsySuite();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

/**
2nd phase constructor. Calls the base class method passing the name of the suite.

@leave If a error happens, it leaves with one of the system error codes.
*/	
void CTe_LbsGpsPsySuite::ConstructL()
	{
	//ConstructL of the base class
	CTestServer::ConstructL(KServerName);
	
	CTe_SystemStarter::DefineLbsPropertiesL();
	}

CTe_LbsGpsPsySuite::~CTe_LbsGpsPsySuite()
	{
    TRAP_IGNORE(CTe_SystemStarter::DeleteLbsPropertiesL());
	}

/**
Overrides the pure virtual CTestServer::CreateTestStep. Creates a test step by its name.

@return - A CTestStep derived instance or NULL if error or there is no a test step with the name
specified.

@see CTestServer::CreateTestStep
*/
CTestStep* CTe_LbsGpsPsySuite::CreateTestStep(const TDesC& aStepName)
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

@see CTe_LbsGpsPsySuite::CreateTestStep
*/
CTestStep* CTe_LbsGpsPsySuite::CreateTestStepL(const TDesC& aStepName)
	{
	if(aStepName==KLbsNotifyPositionUpdateStep)
	   	{
	   	return CTe_LbsNotifyPositionUpdateStep::NewL(this);
	  	} 
	else if(aStepName==KLbsNotifyPositionUpdateTimingStep)
	   	{
	   	return CTe_LbsNotifyPositionUpdateTimingStep::NewL(this);
	  	}
	else if(aStepName==KLbsNotifyPositionUpdateErrStep)
		{
		return CTe_LbsNotifyPositionUpdateErrStep::NewL(this);
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


TUid CTe_LbsGpsPsySuite::ModuleUid() const
	{
	return KLbsGpsLocManagerUid;
	}

const RLbsPositionUpdateRequests::TChannelIdentifer& CTe_LbsGpsPsySuite::UpdateReqChanId() const
	{
	return KChannelIdentifierLS2GpsLocManager;
	}

TUint CTe_LbsGpsPsySuite::CountPositioner()
	{
	return iPositionerCount++;
	}

CTestExecuteLogger& CTe_LbsGpsPsySuite::Logger() const
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
	
	CTe_LbsGpsPsySuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsGpsPsySuite::NewL());
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

