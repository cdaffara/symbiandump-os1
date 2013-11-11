// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "mmcameraclientinterface.h"
#include <e32uid.h>


/**
 * Starts the server process. 
 * 
 * Simultaneous launching of two such processes should be detected when the second
 * one attempts to create the server object, failing with KErrAlreadyExists.
 */
static TInt StartServer()
    {
    const TUidType serverUid(KNullUid, KExecutableImageUid, KECamServerUid3);
    RProcess server;
    TInt err = server.Create(KMMCameraServerExe, KNullDesC, serverUid);
    if (err != KErrNone)
		{
        return err;
		}
    TRequestStatus stat;
    server.Rendezvous(stat);
    if (stat != KRequestPending)
		{
        server.Kill(0);     // abort startup
		}
    else
		{
        server.Resume();    // logon OK - start the server
		}

    User::WaitForRequest(stat);     // wait for start or death
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    err = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
    server.Close();

    return err;
    }


/**
 * Connect to the server.
 * 
 * Will attempt to start the server if it has not yet been started or has been shut down.
 */
TInt RMMCameraSession::Connect()
    {
    TInt serverStartRetries = 2;
    for (;;)
        {
        // Attempt to create a session to the server
        TInt err = CreateSession(KMMCameraServerName, TVersion(0, 0, 0), KECamNumAsynchMsg);
        if (err != KErrNotFound && err != KErrServerTerminated)
			{
            return err;
			}

		if (--serverStartRetries == 0)
			{
            return err;
			}

		// Server not found or shut down so (re)start the server
        err = StartServer();
        if (err != KErrNone && err != KErrAlreadyExists)
            {
			return err;
			}
        }
    }


//
// Synchronous interface APIs
//

TInt RMMCameraSession::SendMessage(TECamMsgId aMsgId, TCameraParameterId aParameterId, TDes8& aMessage)
	{
	if (aMessage.Length() > KECamMaxMessage)
    	{
    	return KErrArgument;
    	}

	return SendReceive(aMsgId, TIpcArgs(aParameterId, &aMessage));
	}

TInt RMMCameraSession::SendMessage(TECamMsgId aMsgId, TCameraParameterId aParameterId, TInt aMessage)
	{
	return SendReceive(aMsgId, TIpcArgs(aParameterId, aMessage));
	}

TInt RMMCameraSession::SendMessage(TECamMsgId aMsgId, TDes8& aMessage)
    {
    if (aMessage.Length() > KECamMaxMessage)
    	{
    	return KErrArgument;
    	}

    return SendReceive(aMsgId, TIpcArgs(&aMessage));
    }

TInt RMMCameraSession::SendMessage(TECamMsgId aMsgId, TInt aMessage)
	{
	return SendReceive(aMsgId, TIpcArgs(aMessage));
	}

TInt RMMCameraSession::SendMessage(TECamMsgId aMsgId)
	{
	return SendReceive(aMsgId);
	}

TInt RMMCameraSession::SendRxMessage(TECamMsgId aMsgId, TCameraParameterId aParameterId, TDes8& aMessage) const
	{
	if (aMessage.Length() > KECamMaxMessage)
    	{
    	return KErrArgument;
    	}

	return SendReceive(aMsgId, TIpcArgs(aParameterId, &aMessage));
	}

TInt RMMCameraSession::SendRxMessage(TECamMsgId aMsgId, TCameraParameterId aParameterId, TInt aMessage) const
	{
	return SendReceive(aMsgId, TIpcArgs(aParameterId, aMessage));
	}

TInt RMMCameraSession::SendRxMessage(TECamMsgId aMsgId, TDes8& aMessage) const
	{
	if (aMessage.Length() > KECamMaxMessage)
    	{
    	return KErrArgument;
    	}

    return SendReceive(aMsgId, TIpcArgs(&aMessage));
	}


//
// Asynchronous interface APIs
//

TInt RMMCameraSession::ReceiveMessage(TECamMsgId aMsgId, TDes8& aMessage, TRequestStatus& aStatus)
    {
    if (aMessage.Length() > KECamMaxMessage)
    	{
    	return KErrArgument;
    	}

    SendReceive(aMsgId, TIpcArgs(&aMessage), aStatus);

	return KErrNone;
    }

void RMMCameraSession::ReceiveMessage(TECamMsgId aMsgId, TInt aMessage, TRequestStatus& aStatus)
    {
    SendReceive(aMsgId, TIpcArgs(aMessage), aStatus);
    }
