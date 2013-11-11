// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test server for the ESOCK PlatSec tests
// 
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/
#include "TE_EsockPlatSecServer.h"
#include "TE_EsockPlatSecStep.h"

#include "SocketTransfer.h"
#include "ConnCloning.h"
#include "SubConnection.h"



_LIT(KServerName,"TE_Cap_Esock_Transfer");

/**
* @return - Instance of the test server
* Called inside the MainL() function to create and start the
* CTestServer derived server.
*/
CEsockPlatSecTestServer* CEsockPlatSecTestServer::NewL()
    {
    // __EDIT_ME__ new your server class here
    CEsockPlatSecTestServer * server = new (ELeave) CEsockPlatSecTestServer();
    CleanupStack::PushL(server);
    // CServer base class call
    server->StartL(KServerName);
    CleanupStack::Pop(server);
    return server;
    }

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
* Much simpler, uses the new Rendezvous() call to sync with the client
*/
    {
    // Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
    CActiveScheduler* sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CEsockPlatSecTestServer* server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CEsockPlatSecTestServer::NewL());
    if(!err)
        {
        // Sync with the client and enter the active scheduler
        RProcess::Rendezvous(KErrNone);
        sched->Start();
        }
    delete server;
    delete sched;
    }

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
* @return - Standard Epoc error code on exit
*/
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
        {
        return KErrNoMemory;
        }
    TRAP_IGNORE(MainL());
    delete cleanup;
    return KErrNone;
    }

// Create a thread in the calling process
// Emulator typhoon and earlier

/**
* @return - A CTestStep derived instance
* Implementation of CTestServer pure virtual
*/
CTestStep* CEsockPlatSecTestServer::CreateTestStep(const TDesC& aStepName)
    {
    CTestStep* testStep = NULL;

    if(aStepName == KTestStep_SocketTransfer)
        testStep = new CSocketTransferStep(this);

    if(aStepName == KTestStep_ConnCloning1)
        testStep = new CConnCloningStep1(this);

    if(aStepName == KTestStep_ConnCloning2)
        testStep = new CConnCloningStep2(this);

    if(aStepName == KTestStep_SubConnection1)
        testStep = new CSubConnectionStep1(this);

    if(aStepName == KTestStep_SubConnection2)
        testStep = new CSubConnectionStep2(this);

    return testStep;
    }






