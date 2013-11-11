/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   DRM Play Server
*
*/


#include <e32std.h>
#include <e32svr.h>
#include "DRMPlayClientServer.h"
#include "DRMPlayServer.h"



CDRMPlayServer::CDRMPlayServer(TInt aPriority)
    : CServer2(aPriority)
    {
    }


// Create and start a new DRM play server.
CDRMPlayServer* CDRMPlayServer::New()
    {
    CDRMPlayServer *pS=new CDRMPlayServer(EPriority);
    __ASSERT_ALWAYS(pS!=NULL,PanicServer(ESvrCreateServer));
    __ASSERT_ALWAYS(pS->Start(KDRMPlayServerName)==KErrNone, PanicServer(ESvrCreateServer));
    return pS;
    }


// Create a new server session.
CSession2 *CDRMPlayServer::NewSessionL(const TVersion& aVersion,const RMessage2& aMessage ) const
    {
    // check we're the right version
    TVersion v(KDRMPlayServMajorVersionNumber,KDRMPlayServMinorVersionNumber,KDRMPlayServBuildVersionNumber);
    if (!User::QueryVersionSupported(v,aVersion))
        User::Leave(KErrNotSupported);
    // make new session
    return CDRMPlayServerSession::NewL(const_cast<CDRMPlayServer*>(this), aMessage);
    }

CDRMPlayServer::~CDRMPlayServer()
    {

    }


// Panic the server
GLDEF_C void PanicServer(TDRMPlayServPanic aPanic)
    {
    _LIT(KTxtServerPanic,"DRM Play server panic");
    User::Panic(KTxtServerPanic,aPanic);
    }


static void StartDRMPlayServerL()
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
//
    {



    // Naming the server thread after the server helps to debug panics

    User::LeaveIfError(User::RenameThread(KDRMPlayServerName));

    // Create and install the active scheduler we need
    CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
    CActiveScheduler::Install(scheduler);

    // create the server
    CDRMPlayServer* server = CDRMPlayServer::New();

    // Initialisation complete, now signal the client
    RProcess::Rendezvous(KErrNone);

    // Start the scheduler and wait for client requests
    CActiveScheduler::Start();

    // Cleanup the server and scheduler
    CActiveScheduler::Install(NULL);
    delete scheduler;
    delete server;
    }

TInt E32Main()
//
// Server process entry-point
// Recover the startup parameters and run the server
//
    {
    __UHEAP_MARK;
    //
    CTrapCleanup* cleanup=CTrapCleanup::New();
    TInt r=KErrNoMemory;
    if (cleanup)
        {
        TRAP(r,StartDRMPlayServerL());
        delete cleanup;
        }
    //
    __UHEAP_MARKEND;
    return r;
    }

// End of file
