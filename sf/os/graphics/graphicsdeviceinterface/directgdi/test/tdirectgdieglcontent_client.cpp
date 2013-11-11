// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tdirectgdieglcontent_client.h"
#include "tdirectgdieglcontent_clientserver.h"
#include <graphics/sgimage.h>

/**
Starts the server process. Simultaneous launching
of two such processes should be detected when the second one attempts to
create the server object, failing with KErrAlreadyExists.
@return KErrNone if successful, error code if server startup failed.
*/
TInt REglContentSession::StartServer()
	{
	const TUidType serverUid(KNullUid, KNullUid, KServerUid3);
	TInt r = iServer.Create(KEglContentServerName, KNullDesC, serverUid);
	if(r != KErrNone)
		return r;
	TRequestStatus stat;
	iServer.Rendezvous(stat);
	if(stat != KRequestPending)
		iServer.Kill(0);     // abort startup
	else
		iServer.Resume();    // logon OK - start the server
	User::WaitForRequest(stat);     // wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r = (iServer.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	return r;
	}

/**
Connect to the server, attempting to start it if necessary
@return KErrNone if successful, error code if connection to server failed.
*/
TInt REglContentSession::Connect()
	{
	TInt retry = 2;
	for(;;)
		{
		TInt r = CreateSession(KEglContentServerName, TVersion(0, 0, 0), 1);
		if((r != KErrNotFound) && (r != KErrServerTerminated))
			return r;
		if((--retry) == 0)
			return r;
		r = StartServer();
		if((r != KErrNone) && (r != KErrAlreadyExists))
			return r;
		}
	}

/**
Force server termination and close server session.
*/
void REglContentSession::Close()
	{
	SendReceive(ETerminateServer, TIpcArgs());

	// wait until process terminate
	TRequestStatus status;
	iServer.Logon(status);
	User::WaitForRequest(status);

	iServer.Close();
	RSessionBase::Close();
	}

/**
Send a request to the server for an image in sync mode. Function returns when the image is available.
@param aImageId reference to drawable id for image id storing.
@return KErrNone if successful, error code if request sending failed.
*/
TInt REglContentSession::GetSyncImage(TSgDrawableId& aImageId)
	{
	TPckg<TSgDrawableId> idPckg(aImageId);
	return SendReceive(EGetSyncImage, TIpcArgs(&idPckg));
	}

/**
Send a request to the server for an image in async mode. Function returns when the image is available.
Command to render the next images is issued.
@param aImageId reference to drawable id for image id storing.
@param aFrameNumber reference to variable to store frame number
@return KErrNone if successful, error code if request sending failed.
*/
TInt REglContentSession::GetAsyncImage(TSgDrawableId& aImageId, TInt& aFrameNumber)
	{
	TPckg<TSgDrawableId> idPckg(aImageId);
	TPckg<TInt> fnumPckg(aFrameNumber);
	return SendReceive(EGetAsyncImage, TIpcArgs(&idPckg, &fnumPckg));
	}

/**
Send a request to the server for an image in async debug mode. Function returns when the image is available.
Command to render the next image (only one) is issued.
@param aImageId reference to drawable id for image id storing.
@param aFrameNumber reference to variable to store frame number
@return KErrNone if successful, error code if request sending failed.
*/
TInt REglContentSession::GetAsyncImageDebug(TSgDrawableId& aImageId, TInt& aFrameNumber)
	{
	TPckg<TSgDrawableId> idPckg(aImageId);
	TPckg<TInt> fnumPckg(aFrameNumber);
	return SendReceive(EGetAsyncImageDebug, TIpcArgs(&idPckg, &fnumPckg));
	}
