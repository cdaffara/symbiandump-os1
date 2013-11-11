// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// netmetaunittestserver.cpp
//


/**
@file 
@internalTechnology

Net Meta Unit Test Server
*/

#include <e32base.h>

#include "netmetaunittestserver.h"

#include "netmetatest001step.h"


// TODO: Needs more unit test steps adding, not done now since this is created purely to test a defect fix
//#include "netmetatest002step.h"
//#include "netmetatest003step.h"
//#include "netmetatest004step.h"
//#include "netmetatest005step.h"
//#include "netmetatest006step.h"
//#include "netmetatest007step.h"
//#include "netmetatest008step.h"


/**
*/
LOCAL_C void MainL()
	{
	CActiveScheduler* sched = NULL;
	sched = new(ELeave)CActiveScheduler();
	CActiveScheduler::Install(sched);
	
	CNetMetaUnitTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CNetMetaUnitTestServer::NewL());
	if (!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}


/**
Process entry point. Called by client using RProcess API
@return - Standard Epoc error code on process exit
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



// ------------------------
// CNetMetaUnitTestServer
// ------------------------
CNetMetaUnitTestServer* CNetMetaUnitTestServer::NewL()
	{
	CNetMetaUnitTestServer* self = new(ELeave)CNetMetaUnitTestServer();
	CleanupStack::PushL(self);
	self->StartL(self->ServerName());
	CleanupStack::Pop(self);
	return self;
	}


CNetMetaUnitTestServer::CNetMetaUnitTestServer()
	: CTestServer()
	{
	}


CNetMetaUnitTestServer::~CNetMetaUnitTestServer()
	{
	}



#define BEGIN_TEST_STEP_LIST()  \
	CTestStep* testStep = NULL; \
	if (EFalse) { }

#define TEST_STEP(testStepTag)  \
    else if (aStepName.CompareF(K##testStepTag##Step) == 0) \
        { testStep = new C##testStepTag##Step(); }

#define END_TEST_STEP_LIST()    \
    return testStep;
    

/**
Implementation of CTestServer pure virtual
@return     A CTestStep derived instance
*/
CTestStep* CNetMetaUnitTestServer::CreateTestStep(const TDesC& aStepName)
	{
    BEGIN_TEST_STEP_LIST()
        TEST_STEP(NetMetaTest001)
		
//		TEST_STEP(NetMetaTest002)        
//		TEST_STEP(NetMetaTest003)        
//		TEST_STEP(NetMetaTest004)        
//		TEST_STEP(NetMetaTest005)
//		TEST_STEP(NetMetaTest006)
//		TEST_STEP(NetMetaTest007)
//		TEST_STEP(NetMetaTest008)        
        
	END_TEST_STEP_LIST()
	}

	
const TPtrC CNetMetaUnitTestServer::ServerName() const
	{
	TParsePtrC serverName(RProcess().FileName());
	return serverName.Name();
	}
