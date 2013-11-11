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
* Description:   DRM PlayUtility Session
*
*/


#include "mmfdrmSession.h"
#include "DRMPlayClientServer.h"

#ifdef _DEBUG
#define DEP_PRN0(str)   RDebug::Print(str)
#define DEP_PRN1(str, v1)   RDebug::Print(str, v1)
#define DEP_PRN2(str, v1, v2)   RDebug::Print(str, v1, v2)
#else
#define DEP_PRN0(str)
#define DEP_PRN1(str, v1)
#define DEP_PRN2(str, v1, v2)
#endif // _DEBUG



// Standard server startup code
//
static TInt StartServer()
    {
    DEP_PRN0(_L("DRM Client: Starting server..."));

    // EPOC is easy, we just create a new server process. Simultaneous
    // launching of two such processes should be detected when the second one
    // attempts to create the server object, failing with KErrAlreadyExists.
    RProcess server;
    //TInt r=server.Create(KHelloWorldServerImg,KNullDesC,serverUid);
    TInt r=server.Create(KDRMPlayServerImg,KNullDesC);

    if (r!=KErrNone)
        {
        DEP_PRN1(_L(" DRM Client: server start failed %d"),r);

        return r;
        }
    TRequestStatus stat;
    server.Rendezvous(stat);
    if (stat!=KRequestPending)
        server.Kill(0);     // abort startup
    else
        server.Resume();    // logon OK - start the server

    DEP_PRN0(_L("DRM Client: Started"));

    User::WaitForRequest(stat);     // wait for start or death
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }




RDrmSession::RDrmSession()
    {
    }

RDrmSession::~RDrmSession()
    {
    if ( Handle() )
        {
        Disconnect();
        }
    }


TInt RDrmSession::Connect()
    {
    // Try 2 times
    TInt retry(2);
    for (;;)
        {
        TInt r = CreateSession( KDRMPlayServerName, TVersion(0,0,0), -1);
        if ((r != KErrNotFound) && (r != KErrServerTerminated) )
            return r;
        if (--retry == 0)
            return r;
        r = StartServer();
        if (( r!= KErrNone) && (r !=KErrAlreadyExists))
            return r;
        }
    }

void RDrmSession::Disconnect()
    {
    RSessionBase::Close();
    }

TVersion RDrmSession::Version() const
    {
    return(TVersion(KDRMPlayServMajorVersionNumber,
                    KDRMPlayServMinorVersionNumber,
                    KDRMPlayServBuildVersionNumber));
    }

//Sync send no data
TInt RDrmSession::Send(TInt aFunction)
    {
    TInt status(KErrSessionClosed);
    if (Handle())
        {
        status = RSessionBase::SendReceive(aFunction);
        }
    return status;
    }

//Sync send with data
TInt RDrmSession::Send(TInt aFunction,const TIpcArgs& aArgs)
    {
    TInt status(KErrSessionClosed);
    if (Handle())
        {
        status = RSessionBase::SendReceive(aFunction, aArgs);
        }
    return status;
    }

//Async send no data
void RDrmSession::Send(TInt aFunction,TRequestStatus& aStatus)
    {
    if (Handle())
        {
        RSessionBase::SendReceive(aFunction, aStatus);
        }
    else
        {
        TRequestStatus* s = &aStatus;
        User::RequestComplete( s , KErrSessionClosed );
        }
    }

//Async send with data
void RDrmSession::Send(TInt aFunction,const TIpcArgs& aArgs,TRequestStatus& aStatus)
    {
    if (Handle())
        {
        RSessionBase::SendReceive(aFunction, aArgs, aStatus);
        }
    else
        {
        TRequestStatus* s = &aStatus;
        User::RequestComplete( s , KErrSessionClosed );
        }
    }

// End of file
