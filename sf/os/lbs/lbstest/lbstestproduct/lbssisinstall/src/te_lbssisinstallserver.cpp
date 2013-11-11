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
// testexecute server implementation
// 
//

/**
 @file te_lbssisinstallserver.cpp
 @internalTechnology
*/

#include "te_lbssisinstallserver.h"
#include "tdbtsisinstallstep.h"
#include "tdbtsisuninstallstep.h"
#include "checkbtpsyfromserverstep.h"
#include "checkbtpsyinromstep.h"
#include "startlbsstep.h"
#include "stoplbsstep.h"

_LIT(KServerName,"te_lbssisinstall");

CTe_LbsSisInstall* CTe_LbsSisInstall::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_LbsSisInstall * server = new (ELeave) CTe_LbsSisInstall();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_LbsSisInstall* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_LbsSisInstall::NewL());
	server->AddHeader();
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_LbsSisInstall::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	TRAPD(err,
	if(aStepName == KTDBTSisInstallStep)
				testStep = new CTDBTSisInstallStep();
	else if(aStepName == KTDBTSisUninstallStep)
				testStep = new CTDBTSisUninstallStep();
	else if(aStepName == KCheckBTPSYFromServerStep)
				testStep = new CCheckBTPSYFromServerStep();
	else if(aStepName == KCheckBTPSYinRomStep)
				testStep = new CCheckBTPSYinRomStep();
	else if (aStepName == KStartLbsStep)
  		{
  		testStep = new CT_StartLbsStep();
  		}
	else if (aStepName == KStopLbsStep)
  		{
  		testStep = new CT_StopLbsStep();
  		}	
	);
	
	if(KErrNone != err)
		{
		ERR_PRINTF3(KStepCreation, &aStepName, err);
		}
	
	return testStep;
	}

void CTe_LbsSisInstall::AddHeader()
	{
	//_LIT(KInformationHeader, "More logs are available in the following directory: <B>%S</B>");
	//INFO_PRINTF2(KInformationHeader, &KLoggingFullName);
	}

