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
// file te_sysutilcapabilityserver.cpp
// 
//

#include "te_sysutilcapabilityserver.h"
#include "te_sysutiltestpsreadpolicy.h"
#include "te_sysutiltestpswritepolicy.h"

CTeSysUtilSuite* CTeSysUtilSuite::NewL()
	{
	CTeSysUtilSuite* server = new(ELeave) CTeSysUtilSuite();
	CleanupStack::PushL(server);
	
	RProcess handle = RProcess();
	TParsePtrC serverName(handle.FileName());
	
	server->ConstructL(serverName.Name());
	CleanupStack::Pop(server);
	return server;
	}

void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTeSysUtilSuite* server = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err, server = CTeSysUtilSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	
	delete server;
	delete sched;
	}

TInt E32Main()
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

CTestStep* CTeSysUtilSuite::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KSysUtilTestPSReadPolicyStep)
		{
		testStep = new CSysUtilTestPSReadPolicyStep();
		}
	else if(aStepName == KSysUtilTestPSWritePolicyStep)
		{
		testStep = new CSysUtilTestPSWritePolicyStep();
		}
	
	return testStep;
	}
