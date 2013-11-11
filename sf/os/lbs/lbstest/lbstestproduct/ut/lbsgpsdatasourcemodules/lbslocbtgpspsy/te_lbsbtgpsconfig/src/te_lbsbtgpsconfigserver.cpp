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
// te_lbsbtgpsconfigserver.h
// lbsbtgpsconfig unit test: Test server
//

#include "te_lbsbtgpsconfigserver.h"
#include "te_lbsbtgpsconfigstepbase.h"

// Test step headers:
#include "te_lbsbtgpsconfigstepadd.h"
#include "te_lbsbtgpsconfigstepremove.h"
#include "te_lbsbtgpsconfigstepreorder.h"
#include "te_lbsbtgpsconfigstepinternalapi.h"
#include "te_lbsbtgpsconfigstepmultipleclient.h"
#include "te_lbsbtgpsconfigstepreadonly.h"
#include "te_lbsbtgpsconfigstepnoaccess.h"


CLbsBtGpsConfigTestServer* CLbsBtGpsConfigTestServer::NewL()
	{
	CLbsBtGpsConfigTestServer* self = new (ELeave) CLbsBtGpsConfigTestServer();
	CleanupStack::PushL(self);
	// CServer base class call
	self->ConstructL(_L("te_lbsbtgpsconfig_sc"));
	CleanupStack::Pop(self);
	return self;
	}


CActiveScheduler* CLbsBtGpsConfigTestServer::ActiveScheduler()
	{
	return iScheduler;
	}


TBTDevAddr CLbsBtGpsConfigTestServer::GenerateUniqueBTDevAddr()
	{
	return TBTDevAddr(MAKE_TINT64(++iUniqueAddr, 0x01234567));
	}


CTestStep* CLbsBtGpsConfigTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CLbsBtGpsConfigStepBase* testStep = NULL;
	
	if (aStepName == KLbsBtGpsConfigStepAdd)
		{
		testStep =  new(ELeave) CLbsBtGpsConfigStepAdd();
		}
	else if (aStepName == KLbsBtGpsConfigStepRemove)
		{
		testStep =  new(ELeave) CLbsBtGpsConfigStepRemove();
		}
	else if (aStepName == KLbsBtGpsConfigStepReorder)
		{
		testStep =  new(ELeave) CLbsBtGpsConfigStepReorder();
		}
	else if (aStepName == KLbsBtGpsConfigStepInternalApi)
		{
		testStep =  new(ELeave) CLbsBtGpsConfigStepInternalApi();
		}
	else if (aStepName == KLbsBtGpsConfigStepMultipleClient)
		{
		testStep =  new(ELeave) CLbsBtGpsConfigStepMultipleClient();
		}
	else if (aStepName == KLbsBtGpsConfigStepReadOnly)
		{
		testStep =  new(ELeave) CLbsBtGpsConfigStepReadOnly();
		}
	else if (aStepName == KLbsBtGpsConfigStepNoAccess)
		{
		testStep =  new(ELeave) CLbsBtGpsConfigStepNoAccess();
		}
	
	// ^^^ Add new test steps above ^^^	
	
	// Give the test step a pointer to the test server
	if (testStep != NULL)
		{
		testStep->SetTestServer(this);
		}
	
	return testStep;
	}


CLbsBtGpsConfigTestServer::~CLbsBtGpsConfigTestServer()
	{
	delete iScheduler;
	}


void CLbsBtGpsConfigTestServer::ConstructL(const TDesC& aName)
	{
	iScheduler = new (ELeave) CActiveScheduler();
	CTestServer::ConstructL(aName);
	}

	
LOCAL_C void MainL()
	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CLbsBtGpsConfigTestServer* server = NULL;
	TRAPD(err, server = CLbsBtGpsConfigTestServer::NewL());
	if (!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}	
	delete server;
	delete sched;
	}


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
