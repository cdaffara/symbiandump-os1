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
//CLayer test server(tclayer_serverwithssm.exe)

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <ssm/ssmswp.h>

#include "tclayer_serverwithssm.h"
#include "tclayer_step_startersessionwithssm.h"

_LIT(KCLayerTestServerName, "CLayerTestServerWithSsm");
_LIT(KCLayerTestWrapper, "CLayerTestWrapper");

CCLayerTestServerWithSsm* CCLayerTestServerWithSsm::NewL( )
	{
	CCLayerTestServerWithSsm * server = new (ELeave) CCLayerTestServerWithSsm();
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop(server);
	return server;
	}

CCLayerTestServerWithSsm::CCLayerTestServerWithSsm()
    {
    
    }

CCLayerTestServerWithSsm::~CCLayerTestServerWithSsm()
    {
    
    }

static void MainL()
	{
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install (sched);

	CCLayerTestServerWithSsm* server = NULL;
	server = CCLayerTestServerWithSsm::NewL();
	if(server)
	    {
        RProcess::Rendezvous (KErrNone);
        sched->Start();
	    }
	delete server;
	CleanupStack::PopAndDestroy();// sched;
	}

/**
 Process entry point. Called by client using RProcess API
 @return - Standard Epoc error code on process exit
 */

TInt E32Main(void)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	if (err)
		{
		_LIT(KMainErrorStatement, "CCLayerTestServerWithSsm::MainL - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
        User::Panic(KCLayerTestServerName, err);
		}
	delete cleanup;
	return err;
	} //lint -esym(765, E32Main) Suppress 'E32Main' could be made static
	  //lint -esym(714, E32Main) Suppress 'E32Main' not referenced

CTestBlockController* CCLayerTestServerWithSsm::CreateTestBlock()
	{
	CTestBlockController* controller = NULL;
    TRAPD(err, (controller = new (ELeave) CCLayerTestBlock()));
    if(KErrNone != err)
        {
        User::Panic(KCLayerTestServerName, err);
        }
    return controller;
	}

CDataWrapper* CCLayerTestBlock::CreateDataL(const TDesC& aData)
    {
    // Print out the parameters for debugging
    CDataWrapper* wrapper = NULL;
    if (KCLayerTestWrapper() == aData)
        {
        wrapper = CCLayerTestWrapperWithSsm::NewL();
        }
    return wrapper;
    }
