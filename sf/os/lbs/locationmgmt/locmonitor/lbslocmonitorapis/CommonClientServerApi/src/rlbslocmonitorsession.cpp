/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include "rlbslocmonitorsession.h"
#include "lbsdevloggermacros.h"
#include "tserverstartparams.h"
#include "cserverlaunch.h"
#include "lbslocmonitorserverdata.h"
#include "lbsprocessuiddefs.h"
#include "lbsipc.h"


//-----------------------------------------------------------------------------
// RLbsLocMonitorSession
//-----------------------------------------------------------------------------
EXPORT_C RLbsLocMonitorSession::RLbsLocMonitorSession()
	{
	LBSLOG(ELogP9, "->S RLbsLocMonitorSession::RLbsLocMonitorSession() lbslocmonitor.exe\n");
	}

/**
Creates a session with the location monitor server.
This method starts the server if not yet started.

@return Standard Symbian OS error code.
*/
EXPORT_C TInt RLbsLocMonitorSession::Connect()
	{
	LBSLOG(ELogP9, "->S RLbsLocMonitorSession::Connect()\n");
	TInt ret(KErrNone);
    TServerStartParams params;
	params.SetServerFileName(KLocMonitorExeName);
	params.SetServerName(KLocMonitorServerName);
	params.SetNumberOfServerSlots(KLbsDefaultMessageSlots);
	params.SetServerUids(KNullUid, KNullUid, KLbsLocMonitorUid);
	
	// Base class creates the session (and the server if required)	
	ret = RSecureSessionBase::Connect(params);
	
	LBSLOG2(ELogP9, "  Return  = %d\n", ret);
	return ret;
	}

EXPORT_C void RLbsLocMonitorSession::Close()
	{
	LBSLOG(ELogP9, "->S RLbsLocMonitorSession::Close() lbslocationmonitor.exe\n");
	RSecureSessionBase::Close();
	}

/**
Attempts to Cancel a previously issued asynchronous request to delete the
database of area-locations.

The TRequestStatus of the original request will be set to KErrCancel if
the cancellation request was successful, or any other Symbian OS error code
if CancelRequest was called too late.

@return Standard Symbian OS error code.
 */
EXPORT_C TInt RLbsLocMonitorSession::CancelDatabaseWipeOut() const
	{
	LBSLOG(ELogP9, "->S RLbsLocMonitorSession::CancelDatabaseWipeOut() lbslocationmonitor.exe\n");	
	__ASSERT_ALWAYS(Handle(), User::Invariant());
	return SendReceive(ECancelDbWipeOut);
	}

EXPORT_C void RLbsLocMonitorSession::WipeOutDatabase(TRequestStatus& aStatus) const
	{
	LBSLOG(ELogP9, "->S RLbsLocMonitorSession::WipeOutDatabase() lbslocationmonitor.exe\n");	
	__ASSERT_ALWAYS(Handle(), User::Invariant());
	return SendReceive(EWipeOutDb, aStatus);
	}

EXPORT_C TVersion RLbsLocMonitorSession::Version()
/**
Obtains the current version number of the location monitor.

@return the version of the client API.
 */
	{
	LBSLOG(ELogP9, "->S RLbsLocMonitorSession::Version() lbslocationmonitor.exe\n");	
	LBSLOG4(ELogP9, "  Return TVersion  = (%d, %d, %d)\n", KLbsLocMonitorMajorVersionNumber,
														   KLbsLocMonitorMinorVersionNumber,
														   KLbsLocMonitorBuildVersionNumber);
	return TVersion(
		KLbsLocMonitorMajorVersionNumber,
		KLbsLocMonitorMinorVersionNumber,
		KLbsLocMonitorBuildVersionNumber);
	}

TInt RLbsLocMonitorSession::StartServer(TServerStartParams& aParams)
	{
	return RSecureSessionBase::StartServer(aParams);
	}
