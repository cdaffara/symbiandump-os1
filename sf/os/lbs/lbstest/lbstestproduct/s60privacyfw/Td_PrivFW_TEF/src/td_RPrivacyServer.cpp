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



// INCLUDE FILES
#include <e32std.h>
#include <s32mem.h>
#include <s32strm.h>
#include <epos_privacycommon.h>
#include <EPos_PrivacyInternal.h>

#include "Td_RPrivacyServer.h"

// CONSTANTS
const TUint KDefaultMessageSlots = 10;

const TInt KPosNumberOfTrials = 5;

_LIT(KPosPrivacyServerImg, "EposPrivSrv");
const TUid KPrivacyServerUid = { 0x101f7a85 };

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
 RTdPrivacyServer::RTdPrivacyServer()
    : RSessionBase()
    {
    }


// ---------------------------------------------------------
// RTdPrivacyServer::Connect
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
 TInt RTdPrivacyServer::Connect()
    {
    TBool retrying = ETrue;
    TInt counter = KPosNumberOfTrials;
    TInt err;

    while (retrying && counter > 0)
        {
        err = CreateSession(
            KPosPrivacyServerName,
            TVersion(
                KPosPrivacyServerMajorVersionNumber,
                KPosPrivacyServerMinorVersionNumber,
                KPosPrivacyServerBuildVersionNumber),
            KDefaultMessageSlots);

        if (err != KErrNotFound && err != KErrServerTerminated)
            {
            // Session is up.
            retrying = EFalse;
            }
        else
            {
            if (--counter > 0)
                {
                err = StartServer();

                if (err != KErrAlreadyExists && err != KErrNone)
                    {
                    // Server returned error code other than
                    // KErrAlreadyExists or KErrNone. The client should
                    // receive this code.
                    retrying = EFalse;
                    }
                }
            }
        }

    if (err != KErrNone)
        {
        Close();
        }

    return err;
    }


/*
    EPosPrivacyVerifyService = 0,
    EPosPrivacyCancel,
    EPosPrivacyNotifyNonVerified,
    EPosPrivacyGetRequestId,
    EPosPrivacyServerMaxEnumValue,
    EPosPrivacyServerLast
*/

// Just send out a null pointer with varying function id
TInt RTdPrivacyServer::SendCrap(TInt aFunction)
	{
    TPtr nullPtr((TUint16*)0,0);
    
	return SendReceive(aFunction, TIpcArgs(&nullPtr));

	}

// ---------------------------------------------------------
// RTdPrivacyServer::Close
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void RTdPrivacyServer::Close()
    {
    RHandleBase::Close();
    }

// -----------------------------------------------------------------------------
// RTdPrivacyServer::StartServer
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt RTdPrivacyServer::StartServer()
    {
    TRequestStatus started;

    const TUidType serverUid(KNullUid, KNullUid, KPrivacyServerUid);

    /**
    EPOC is easy, we just create a new server process. Simultaneous launching
    of two such processes should be detected when the second one attempts to
    create the server object, failing with KErrAlreadyExists.
    **/
    RProcess server;
    TInt ret = server.Create(KPosPrivacyServerImg, KNullDesC, serverUid);

    if (ret != KErrNone)
        {
        return ret;
        }

    server.Rendezvous(started);
    //
    // logon OK - start the server
    server.Resume();

    User::WaitForRequest(started);  // wait for start or death

    server.Close();

    return started.Int();
    }

//  End of File
