// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32base.h>
#include <e32math.h>
#include <lbs.h>
#include <lbsipc.h>

//#include "StartPosServer.h"

#ifdef _DEBUG
#include <e32debug.h>
#endif

// The third UID of the server DLL
const TUid KPositionServerUid3 = { 0x101f97b2 };

/*
Provides server startup parameters.
 
@internalComponent
*/
NONSHARABLE_CLASS(TPositionServerStart)
        {
public:
        TPositionServerStart(TRequestStatus& aStatus);
        TPositionServerStart();

        void SignalL();

private:
        /** The thread ID of the thread trying to start the server */
        TThreadId iId;
        /** The status used to denote the success / failure of server startup */
        TRequestStatus* iStatus;
        };

/**
@internalComponent
@param aStatus TRequestStatus to indicate whether the server has successfully started
*/
inline TPositionServerStart::TPositionServerStart(TRequestStatus& aStatus)
    :       
    iId(RThread().Id()),
    iStatus(&aStatus)
    {
    aStatus=KRequestPending;
    }

/**
Start the server process/thread which lives in an EPOCEXE object.
 
@internalComponent
@return KErrNone if successful, any other error code otherwise
*/
TInt RPositionServer::StartServer()
    {
    TRequestStatus started;
    TPositionServerStart start(started);

#ifdef _DEBUG
    RDebug::Print(_L("EPos: LbsApi: Starting Location Server..."));
#endif
    const TUidType serverUid(KNullUid, KNullUid, KPositionServerUid3);

    // Simultaneous launching of two such processes should be detected 
    // when the second one attempts to create the server object, 
    // failing with KErrAlreadyExists.
    //
    RProcess server;
    TInt ret = server.Create(KPositionServerImg, KNullDesC, serverUid);

    if (ret != KErrNone)
        {
#ifdef _DEBUG
        RDebug::Print(_L("EPos: LbsApi: server start failed %d"), ret);
#endif
        return ret;
        }

    TRequestStatus died;
    server.Rendezvous(died);

    if (died != KRequestPending)
        {
        // logon failed - server is not yet running, so cannot have terminated
        User::WaitForRequest(died);             // eat signal
        server.Kill(0);                         // abort startup
        }
    else
        {
        server.Resume();
        User::WaitForRequest(died);             // wait for start or death
        }

    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    ret = (server.ExitType() == EExitPanic) ? KErrGeneral : died.Int();
    server.Close();
    return ret;
    }

//
// End of file
