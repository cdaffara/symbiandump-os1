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



/**
 @file te_settingsmanagerserver.cpp
 @internalTechnology
*/

#include "te_settingsmanagerserver.h"
#include "settingsmanagercommoncrteststep.h"
#include "settingsmanagerfunctionteststep.h"
#include "settingsmanagerspecificcrteststep.h"
#include "settingsmanagerinitialisationstep.h"

_LIT(KServerName,"te_settingsmanager");

CTe_settingsmanager* CTe_settingsmanager::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_settingsmanager * server = new (ELeave) CTe_settingsmanager();
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
	CTe_settingsmanager* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_settingsmanager::NewL());
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


CTestStep* CTe_settingsmanager::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	TRAPD(err,
	if(aStepName == KSettingsManagerFunctionTestStep)
		testStep = CSettingsManagerFunctionTestStep::NewL();
	else if(aStepName == KSettingsManagerInitialisationStep)
		testStep = CSettingsManagerInitialisationStep::NewL();
	else if(aStepName == KSettingsManagerCommonCRTestStep)
		testStep = CSettingsManagerCommonCRTestStep::NewL();
	else if(aStepName == KSettingsManagerSpecificCRTestStep)
		testStep = CSettingsManagerSpecificCRTestStep::NewL();
	);
	
	if(KErrNone != err)
		{
		ERR_PRINTF3(KStepCreation, &aStepName, err);
		}
	
	return testStep;
	}

void CTe_settingsmanager::AddHeader()
	{
	//_LIT(KInformationHeader, "More logs are available in the following directory: <B>%S</B>");
	//INFO_PRINTF2(KInformationHeader, &KLoggingFullName);
	}

