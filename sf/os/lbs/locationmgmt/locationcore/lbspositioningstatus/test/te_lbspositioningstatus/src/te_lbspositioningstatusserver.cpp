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
//

/**
 @file te_lbspositioningstatusserver.cpp
 @internalComponent
*/

#include "te_lbspositioningstatusserver.h"
#include "te_lbspositioningstatusstep.h"

_LIT(KServerName,"te_lbspositioningstatusserver");


CTe_LbsPositioningStatusServer* CTe_LbsPositioningStatusServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
    {
    CTe_LbsPositioningStatusServer * server = new (ELeave) CTe_LbsPositioningStatusServer();
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
    CTe_LbsPositioningStatusServer* server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CTe_LbsPositioningStatusServer::NewL());
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
    if(cleanup == NULL)
        {
        return KErrNoMemory;
        }
    TRAPD(err,MainL());
    delete cleanup;
    __UHEAP_MARKEND;
    return err;
    }


CTestStep* CTe_LbsPositioningStatusServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
    {
    CTestStep* testStep = NULL;
    if(aStepName == KStatusStep)
        {
        testStep = new CTe_LbsPositioningStatusStep();
        }

    return testStep;
    }
//end of file

