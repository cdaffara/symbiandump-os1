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
//CmdCleTestServer created to test the commands reconstructed by the CLE

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tcmd_runcletestserver.h"
#include "tcmd_runclecmdtest.h"

#include "ssmdebug.h"

_LIT(KCmdCleTestServer, "CmdCleTestServer");

CCmdCleTestServer* CCmdCleTestServer::NewLC( )
    {
    CCmdCleTestServer * server = new (ELeave) CCmdCleTestServer();
    CleanupStack::PushL(server);
    TParsePtrC serverName(RProcess().FileName());   
    server->StartL(serverName.Name());
    return server;
    }

static void MainL()
    {
    CActiveScheduler* sched = new(ELeave) CActiveScheduler;
    CleanupStack::PushL (sched);
    CActiveScheduler::Install (sched);

    CCmdCleTestServer* server = CCmdCleTestServer::NewLC( );
    RProcess::Rendezvous (KErrNone );
    sched->Start( );

    CleanupStack::PopAndDestroy (server );
    CleanupStack::PopAndDestroy (sched );
    }

/**
 Process entry point. Called by client using RProcess API
 @return - Standard Epoc error code on process exit
 */
TInt E32Main(void)
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
        {
        return KErrNoMemory;
        }
    TRAPD(err,MainL()); 
    if (err)
        {
        _LIT(KMainErrorStatement, "CCmdCleTestServer::MainL - Error: %d");
        RDebug::Print(KMainErrorStatement, err);
        User::Panic(KCmdCleTestServer, err);
        }       
    delete cleanup;
        
    __UHEAP_MARKEND;
    return KErrNone;
    }

/**
 @return - A CTestStep derived instance
 */
CTestStep* CCmdCleTestServer::CreateTestStep(const TDesC& aStepName)
    {
    CTestStep* testStep = NULL;
    if (aStepName == KTCCmdRunCleTest)
        {
        testStep = new CCmdRunCleTest();
        }
    else
        {
        RDebug::Printf ("Unknown CustCmdTest step %S", &aStepName );
        }

    return testStep;
    }

