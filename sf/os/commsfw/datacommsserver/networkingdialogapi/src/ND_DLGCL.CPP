// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dial Up Networking Dialog Server - Client Side Source
// 
//

/**
 @file ND_DLGCL.CPP
*/

#include "ND_DLGSV.H"


/** 
PctNotifier & PctRead concurrent requests 

@internalComponent
*/
const TUint	KNdDlgSvMessageSlots=2;		

#ifdef _DEBUG
#define SETIFDEBUG(R,FUNCTION) TInt R = FUNCTION
#else
#define SETIFDEBUG(R,FUNCTION) FUNCTION
#endif

EXPORT_C RGenConAgentDialogServer::RGenConAgentDialogServer() 
	: RSessionBase(),
	iNotifier(NULL),
	iIAP(0),
	iModemId(0),
	iLocationId(0),
	iConNames(TIspConnectionNames()),
	iPrefs(TConnectionPrefs()),
	iBool(EFalse),
	iPctBuffer(0),
	iPctResponse(TPctResponse()),
	iAuthenticationPair(TAuthenticationPair()),
	iNewIapPrefsBuffer(TNewIapConnectionPrefs()),
	iNotUsed(0),
	iStatus(0),
	iAccessPoint(0)
/**
RGenConAgentDialogServer Constructor.
*/
	{
//	__FLOG_OPEN(KDlgSvrLogSubsys, KDlgCliLogComponent);
	}

EXPORT_C RGenConAgentDialogServer::~RGenConAgentDialogServer()
/**
Destructor.
*/
	{}

EXPORT_C TInt RGenConAgentDialogServer::Connect()
/**
Connect to the  server

@return KErrNone if connection succeeded and a standard error code otherwise.
*/
	{
	TInt ret = CreateSession(KCommsDialogServerName,Version(),KNdDlgSvMessageSlots);

	if (ret!=KErrNone)
		{
		ret=StartDialogThread();
//		__FLOG_1(_L("RGenConAgentDialogServer::Connect: StartDialogThread returned %d"),ret);
		if (ret!=KErrNone)
			{
			return(ret);
			}

		ret = CreateSession(KCommsDialogServerName,Version(),KNdDlgSvMessageSlots);
//		__FLOG_1(_L("RGenConAgentDialogServer::Connect: CreateSession returned %d"),ret);
		}
	
	return ret;
	}

EXPORT_C void RGenConAgentDialogServer::Close()
/**
Disconnect from the notifier server. 
*/
	{
	RHandleBase::Close();
	}

EXPORT_C TVersion RGenConAgentDialogServer::Version(void) const
/**
Return the client side version number.

@return The API version.
*/
	{
	return(TVersion(KDialogServMajorVersionNumber,KDialogServMinorVersionNumber,KDialogServBuildVersionNumber));
	}

EXPORT_C void RGenConAgentDialogServer::ModemAndLocationSelection(TUint32 &aModemId,TUint32 &aLocationId, TRequestStatus &aStatus)
/**
Shows a dialog to enable the user to choose which modem and location to use for data transactions

@param aModemId The id of of the record in the modem table to be used for data transactions
@param aLocationId The id of the record in the location table to be used for the modem for data transactions
@param aStatus Any error code, or KErrNone if no error
*/
	{
	::new(&iModemId) TPckg<TUint32>(aModemId);
	::new(&iLocationId) TPckg<TUint32>(aLocationId);
 	SendReceive(EGetModemAndLocation, TIpcArgs( &iModemId, &iLocationId), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::IapConnection(TUint32& aIAP, const TConnectionPrefs& aPrefs, TRequestStatus& aStatus)
/**
Shows a connection dialog when CommDb database is of IAP type.

@param aIAP The id of the IAP service.
@param aPrefs Specifies the rank and desired direction of the connection and bearer.
@param aStatus Any error code, or KErrNone if no error.
*/
	{
	IapConnection(aIAP, aPrefs, KErrNone, aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::IapConnection(TUint32& aIAP, const TConnectionPrefs& aPrefs, TInt aLastError, TRequestStatus& aStatus)
/**
Shows a connection dialog when CommDb database is of IAP type.

@param aIAP The id of the IAP service.
@param aPrefs Specifies the rank and desired direction of the connection and bearer.
@param aLastError The error with which previous connection failed.
@param aStatus Any error code, or KErrNone if no error.
*/
	{
	::new(&iIAP) TPckg<TUint32>(aIAP);
	iPrefs() = aPrefs;

 	SendReceive(EGetIAP, TIpcArgs(&iIAP, &iPrefs, (TAny*)aLastError), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::WarnNewIapConnection(const TConnectionPrefs& aPrefs, TInt aLastError, const TDesC& aNewIapName, TBool& aResponse, TRequestStatus& aStatus)
/**
Shows a dialog warning when the CommDb database is of IAP type that the previous attempt to connect
failed and that a new connection is to be attempted.

@param aPrefs Specifies the rank and desired direction of the connection and bearer.
@param aLastError The error with which previous connection failed.
@param aNewIapName The name of the IAP to be used for next connection.
@param aResponse Specifies whether to proceed with the connection or stop the connection attempt.
@param aStatus Any error code, or KErrNone if no error.
*/
	{
	iPrefs() = aPrefs;
	iConNames().iServiceName = aNewIapName;
	::new(&iBool) TPckg<TBool>(aResponse);
	SendReceive(EWarnNewIAP, TIpcArgs( &iPrefs, aLastError, &iConNames().iServiceName, &iBool ), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::Login(TDes& aUsername,TDes& aPassword,TBool aIsReconnect,TRequestStatus& aStatus)
/**
Shows a login dialog when login information for use with a script is required any the NetDial agent.

@param aUsername Username.
@param aPassword Password.
@param aIsReconnect Whether this is a reconnect attempt or not
@param aStatus Any error code, or KErrNone if no error.
*/
	{
	SendReceive(EGetLogin, TIpcArgs( &aUsername, &aPassword, (TAny*)aIsReconnect), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::Authenticate(TDes& aUsername,TDes& aPassword,TBool aIsReconnect,TRequestStatus& aStatus)
/**
Shows an authentication dialog when the NetDial agent requests PPP authentication information.

@param aUsername Username.
@param aPassword Password.
@param aIsReconnect Whether this is a reconnect attempt or not
@param aStatus Any error code, or KErrNone if no error.
*/
	{
 	SendReceive(EGetAuthentication, TIpcArgs(&aUsername, &aPassword, (TAny*)aIsReconnect), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::Reconnect(TBool& aBool, TRequestStatus& aStatus)
/**
Shows a reconnect dialog when connection has been broken during data transfer.

@param aResponse Specifies whether to reconnect or cancel the reconnection attempt.
@param aStatus Any error code, or KErrNone if no error.
*/
	{
	::new(&iBool) TPckg<TBool>(aBool);

 	SendReceive(EGetReconnectReq, TIpcArgs( &iBool), aStatus);
	}

EXPORT_C TInt RGenConAgentDialogServer::OpenPct()
/**
Opens a Post Connect Terminal dialog.

@return Any error code, or KErrNone if no error.
*/
	{
	return SendReceive(EOpenPct, TIpcArgs(TIpcArgs::ENothing));
	}

EXPORT_C TInt RGenConAgentDialogServer::WritePct(const TDesC& aData)
/**
Called by NetDial agent to write incoming data into the PCT window.

@param aData Incoming data.
@return Any error code, or KErrNone if no error.
*/
	{
	return SendReceive(EWritePct, TIpcArgs(&aData));
	}

EXPORT_C void RGenConAgentDialogServer::ReadPct(TDes& aData, TRequestStatus& aStatus)
/**
Called by NetDial agent when the script indicates that user needs to enter some information.

@param aData Data entered by user.
@param aStatus Any error code, or KErrNone if no error.
*/
	{
	SendReceive(EReadPct, TIpcArgs( &aData), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::DestroyPctNotification(TRequestStatus& aStatus)
/**
Used by the NetDial agent to request that if the user cancels the dialog, then the dialog gives
notification of this.

@param aStatus KErrNone if a cancel occurs, or error code otherwise.
*/
	{
	SendReceive(EDestroyPctNotification, TIpcArgs(TIpcArgs::ENothing), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::ClosePct()
/**
Closes the Post Connect Terminal dialog.

@panic ESvrClosePctReturnedError 
*/
	{
	SETIFDEBUG(ret, SendReceive(EClosePct, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrClosePctReturnedError));
	}

EXPORT_C void RGenConAgentDialogServer::QoSWarning(TBool& aResponse, TRequestStatus& aStatus)
/**
Shows a dialog warning that the QoS of the connection has fallen below the minimum
values specified in commdb. Expects a response from the dialog indicating if the
connection is to be terminated.

@param aResponse Specifies whether to terminate the connection or not i.e. ETrue means terminate connection
@param aStatus Any error code, or KErrNone if no error.
*/
	{
	::new(&iBool) TPckg<TBool>(aResponse);
	SendReceive(EWarnQoS, TIpcArgs( &iBool), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::CancelModemAndLocationSelection()
/**
Cancels the ModemAndLocation connection dialog.

@panic ESvrCancelReturnedError.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelGetModemAndLocation, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}

EXPORT_C void RGenConAgentDialogServer::CancelIapConnection()
/**
Cancels the IAP type connection dialog.

@panic ESvrCancelReturnedError.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelGetIAP, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}	

EXPORT_C void RGenConAgentDialogServer::CancelWarnNewIapConnection()
/**
Cancels the IAP type new connection warning dialog.

@panic ESvrCancelReturnedError Cancel WarnNewIapConnection.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelWarnIAP, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}	

EXPORT_C void RGenConAgentDialogServer::CancelLogin()
/**
Cancels the login dialog.

@panic ESvrCancelReturnedError Request Cancel for Login.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelLogin, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}

EXPORT_C void RGenConAgentDialogServer::CancelAuthenticate()
/**
Cancels the authentication.

@panic ESvrCancelReturnedError.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelAuthenticate, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}

EXPORT_C void RGenConAgentDialogServer::CancelReconnect()
/**
Cancels the reconnect dialog.

@panic ESvrCancelReturnedError Cancel Reconnect Request.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelReconnect, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}

EXPORT_C void RGenConAgentDialogServer::CancelReadPct()
/**
Cancels the Read Post Connect Terminal request.

@panic ESvrCancelReturnedError Cancel Read Pct.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelReadPct, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}

EXPORT_C void RGenConAgentDialogServer::CancelDestroyPctNotification()
/**
Cancels the Destroy Post Connect Terminal Notification request.

@panic ESvrCancelReturnedError cancel Pct Notification.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelDestroyPctNotification, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}

EXPORT_C void RGenConAgentDialogServer::CancelQoSWarning()
/**
Cancels the QoS warning dialog

@panic ESvrCancelReturnedError Request Cancel from Server.
*/
	{
	SETIFDEBUG(ret, SendReceive(ECancelWarnQoS, TIpcArgs(TIpcArgs::ENothing)));
	__ASSERT_DEBUG(ret==KErrNone, PanicServer(ESvrCancelReturnedError));
	}

EXPORT_C void RGenConAgentDialogServer::AccessPointConnection(TUint32& aAccessPoint, TInt aAccessPointGroup, TRequestStatus& aStatus)
/**
Shows a connection dialog when connection preference is of an AP type.

@param aAP The id of the access point.
@param aStatus Any error code, or KErrNone if no error.
*/
	{
    ::new(&iAccessPoint) TPckg<TUint32>(aAccessPoint);
 	SendReceive(EGetAccessPoint, TIpcArgs(&iAccessPoint,aAccessPointGroup), aStatus);
	}

EXPORT_C void RGenConAgentDialogServer::CancelAccessPointConnection()
/**
Cancels the access point selection dialog.
*/
	{
 	SendReceive(ECancelGetAccessPoint);
	}

EXPORT_C void RGenConAgentDialogServer::AccessPointConnection(TUint32& aAP, TUint32 aAPType, TUint32& aBearerAPInd, TUint32 aBearerAPType, TRequestStatus& aStatus)
/**
Shows a connection dialog when connection preference is of an AP type filtered on bearer-related info.

@param aAP The id of the access point.
@param aStatus Any error code, or KErrNone if no error.
@param aBearerAPType an optional filter parameter to limit the data displayed to the client.
*/
	{
    ::new(&iAccessPoint) TPckg<TUint32>(aAP);
    ::new(&iIAP) TPckg<TUint32>(aBearerAPInd);
 	SendReceive(EGetAccessPointAP, TIpcArgs(&iAccessPoint,aAPType, &iIAP, aBearerAPType), aStatus);
	}
