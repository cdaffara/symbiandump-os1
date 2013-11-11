// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file TE_DialogServer.cpp
*/
//Test Harness Header
#include "TE_DialogServer.h"

//Test Step Headers
#include "TestDialogIniCopy.h"
#include "TestDialogPostDelete.h"
#include "TestIAPDialog.h"
#include "TestModemLocationDialog.h"
#include "TestIAPWArningDialog.h"
#include "TestLoginDialog.h"
#include "TestAuthenticationDialog.h"
#include "TestReconnectionDialog.h"
#include "TestPCTDialog.h"
#include "TestQoSWarningDialog.h"

_LIT(KServerName,"TE_DialogServer");

CDialogServer* CDialogServer::NewL()
	{
	CDialogServer * server = new (ELeave) CDialogServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}




LOCAL_C void MainL()
	{
#if(defined_DATA_CAGING_)	
	RProcess().DataCaging(RProcess::EDataCagingOn);
//	RProcess().DataCaging(RProcess::ESecureApiOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CDialogServer* server = NULL;
	// Create the CDialogServer derived server
	TRAPD(err,server = CDialogServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}







//#if (!defined __DATA_CAGING__)  IDO


GLDEF_C TInt E32Main()
	{
//	_UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
//	_UHEAP_MARKEND;
	return KErrNone;
    }


CTestStep* CDialogServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	       
	if(aStepName == KPreCopyDialog)
		testStep = new CTestPreCopyIni();
		
	else if(aStepName == KTestIAPDialog)
		testStep = new CTestIAPDialog();

	else if(aStepName == KTestAccessPointDialog)
		testStep = new CTestAccessPointDialog();

	else if(aStepName == KTestModemLocationDialog)
		testStep = new CTestModemLocationDialog();

	else if(aStepName == KTestIAPWArningDialog)
		testStep = new CTestIAPWArningDialog();

	else if(aStepName == KTestLoginDialog)
		testStep = new CTestLoginDialog();

	else if(aStepName == KTestAuthenticationDialog)
		testStep = new CTestAuthenticationDialog();

	else if(aStepName == KTestPCTDialog)
		testStep = new CTestPCTDialog();

	else if(aStepName == KTestReconnectionDialog)
		testStep = new CTestReconnectionDialog();

	else if(aStepName == KTestQoSWarningDialog)
		testStep = new CTestQoSWarningDialog();

	else if(aStepName == KPostDeleteDialog)
		testStep = new CTestPostDeleteIni();

  return testStep;
	}

