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
// mecunittestserver.cpp
//


/**
@file 
@internalTechnology

Meta Extension Container Unit Test Server
*/

#include <e32base.h>

#include "mecunittestserver.h"

#include "mectest001step.h"
#include "mectest002step.h"
#include "mectest003step.h"
#include "mectest004step.h"
#include "mectest005step.h"
#include "mectest006step.h"
#include "mectest007step.h"
#include "mectest008step.h"
#include "mectest009step.h"
#include "mectest010step.h"
#include "mectest011step.h"
#include "mectest012step.h"
#include "mectest013step.h"
#include "mectest014step.h"

#include "mectest030step.h"
#include "mectest031step.h"
#include "mectest032step.h"
#include "mectest033step.h"

#include "mectest050step.h"
#include "mectest051step.h"
#include "mectest052step.h"
#include "mectest053step.h"
#include "mectest054step.h"
#include "mectest055step.h"


//#include "mectestpanic1step.h"
#include "mectestpanic2step.h"
#include "mectestpanic3step.h"
#include "mectestpanic4step.h"
#include "mectestpanic7step.h"
#include "mectestpanic8step.h"
#include "mectestpanic9step.h"
#include "mectestpanic10step.h"
#include "mectestpanic11step.h"
#include "mectestpanic12step.h"
#include "mectestpanic13step.h"
#include "mectestpanic14step.h"
#include "mectestpanic15step.h"
#include "mectestpanic16step.h"
#include "mectestpanic17step.h"


/**
*/
LOCAL_C void MainL()
	{
	CActiveScheduler* sched = NULL;
	sched = new(ELeave)CActiveScheduler();
	CActiveScheduler::Install(sched);
	
	CMecUnitTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CMecUnitTestServer::NewL());
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
// CMecUnitTestServer
// ------------------------
CMecUnitTestServer* CMecUnitTestServer::NewL()
	{
	CMecUnitTestServer* self = new(ELeave)CMecUnitTestServer();
	CleanupStack::PushL(self);
	self->StartL(self->ServerName());
	CleanupStack::Pop(self);
	return self;
	}


CMecUnitTestServer::CMecUnitTestServer()
	: CTestServer()
	{
	}


CMecUnitTestServer::~CMecUnitTestServer()
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
CTestStep* CMecUnitTestServer::CreateTestStep(const TDesC& aStepName)
	{
    BEGIN_TEST_STEP_LIST()
        TEST_STEP(MecTest001)
        TEST_STEP(MecTest002)        
        TEST_STEP(MecTest003)        
        TEST_STEP(MecTest004)        
        TEST_STEP(MecTest005)
        TEST_STEP(MecTest006)
		TEST_STEP(MecTest007)
		TEST_STEP(MecTest008)        
        TEST_STEP(MecTest009)
        TEST_STEP(MecTest010)
		TEST_STEP(MecTest011)
		TEST_STEP(MecTest012)
		TEST_STEP(MecTest013)
		TEST_STEP(MecTest014)
        
		TEST_STEP(MecTest030)
		TEST_STEP(MecTest031)
		TEST_STEP(MecTest032)
		TEST_STEP(MecTest033)

		TEST_STEP(MecTest050)
		TEST_STEP(MecTest051)
		TEST_STEP(MecTest052)
		TEST_STEP(MecTest053)
		TEST_STEP(MecTest054)
		TEST_STEP(MecTest055)

//		TEST_STEP(MecTestPanic1) 
        TEST_STEP(MecTestPanic2) 
        TEST_STEP(MecTestPanic3) 
        TEST_STEP(MecTestPanic4)
        // MecTestPanic5 and MecTestPanic6 removed as FindExtension()/FindExtensionL()
        // no longer panic if container hasn't been opened.
		TEST_STEP(MecTestPanic7) 
		TEST_STEP(MecTestPanic8) 
		TEST_STEP(MecTestPanic9) 
		TEST_STEP(MecTestPanic10) 
		TEST_STEP(MecTestPanic11)
		TEST_STEP(MecTestPanic12) 
		TEST_STEP(MecTestPanic13) 
		TEST_STEP(MecTestPanic14) 
		TEST_STEP(MecTestPanic15)
		TEST_STEP(MecTestPanic16)
		TEST_STEP(MecTestPanic17) 

	END_TEST_STEP_LIST()
	}



const TPtrC CMecUnitTestServer::ServerName() const
	{

	// On EKA2, test server runs in its own process.
	// So we arrive at the server name using the exe from which it is loaded.
	TParsePtrC serverName(RProcess().FileName());
	return serverName.Name();
	}

