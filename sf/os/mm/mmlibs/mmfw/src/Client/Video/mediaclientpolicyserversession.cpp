// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mediaclientpolicyserversession.h"
#include <e32std.h>
#include "mediaclientvideotrace.h"

// Policy Server EXE name
_LIT( KIvePolicyServerImg, "ivepolicyserver" );

// Policy Server thread name
_LIT( KIvePolicyServerName, "!ivepolicyserver0x10204C27" ); 

// Number of asynchronous message slots
const TInt KIveAsyncMessageSlots = 1;

static TInt StartServer()
    {
    DEBUG_PRINTF("medialclientsession StartServer +++");
    
    RProcess server;
    TInt error = server.Create(KIvePolicyServerImg, KNullDesC);
    if (error != KErrNone)
        {
        DEBUG_PRINTF2("medialclientsession StartServer server creation failed %d", error);
        return error;
        }
        
    TRequestStatus status;

    server.Rendezvous(status);
    if (status != KRequestPending)
        {
        server.Kill( 0 );
        // abort startup
        }
    else
        {
        server.Resume();
        // logon OK - start the server
        }

    User::WaitForRequest(status);
    // wait for start or death
    error = (server.ExitType() == EExitPanic) ? KErrGeneral : status.Int();
    server.Close();
   
    DEBUG_PRINTF(" medialclientsession StartServer ---");
    return error;
    }

RMediaClientPolicyServerSession::RMediaClientPolicyServerSession():
    RSessionBase()
    {
    DEBUG_PRINTF(" RMediaClientPolicyServerSession::RMediaClientPolicyServerSession +++");
    DEBUG_PRINTF(" RMediaClientPolicyServerSession::RMediaClientPolicyServerSession ---");
    }

TInt RMediaClientPolicyServerSession::Connect()
    {
    DEBUG_PRINTF(" RMediaClientPolicyServerSession::Connect +++");
    
    TInt error = KErrNone;
    for (TInt i = 0; i < 2; i++)
        {
        // Try to create session.
        error = CreateSession(KIvePolicyServerName, TVersion( 0, 0, 0 ), KIveAsyncMessageSlots);

        if (error != KErrNotFound && error != KErrServerTerminated) 
            {
            DEBUG_PRINTF("RMediaClientPolicyServerSession::Connect Server found and session created."); 
            DEBUG_PRINTF2("RMediaClientPolicyServerSession::Connect --- returns %d", error);
            return error;
            }
  
        // If session failed, try to start server once.
        if (i == 0)
            {            
            DEBUG_PRINTF("RMediaClientPolicyServerSession::Connect StartServer");
            // Start server
            error = StartServer();                     
            if (error != KErrNone && error != KErrAlreadyExists)
                {
                DEBUG_PRINTF("RMediaClientPolicyServerSession::Connect Server startup failed.");
                DEBUG_PRINTF2("RMediaClientPolicyServerSession::Connect --- returns %d", error);
                return error;
                }
            }
        else
            {
            DEBUG_PRINTF2("RMediaClientPolicyServerSession::Connect Session creation failed error %d", error);
            }
        }
    
    DEBUG_PRINTF2("RMediaClientPolicyServerSession::Connect --- returns %d", error);
    return error;
    }

TInt RMediaClientPolicyServerSession::SendMessage( TInt aFunction, const TIpcArgs &aArgs ) const 
    {
    DEBUG_PRINTF("RMediaClientPolicyServerSession::SendMessage +++");

    TInt error = SendReceive( aFunction, aArgs );    
    
    DEBUG_PRINTF2("RMediaClientPolicyServerSession::SendMessage --- returns %d", error);
    return error;
    }
