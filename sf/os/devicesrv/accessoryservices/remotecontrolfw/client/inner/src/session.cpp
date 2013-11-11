// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <e32base.h>
#include "remconserver.h"
#include <remconaddress.h>
#include "remconclient.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_INNER);
#endif

/** 
Starts the server process.
*/
static TInt StartServer()
	{
	LOG_STATIC_FUNC
	const TUidType serverUid(KNullUid, KNullUid, KRemConSrvUid);

	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt err = server.Create(KRemConServerImg, KNullDesC, serverUid);
	
	if ( err != KErrNone )
		{
		return err;
		}

	TRequestStatus stat;
	server.Rendezvous(stat);
	
	if ( stat != KRequestPending )
		{
		server.Kill(0); 	// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}

	User::WaitForRequest(stat); 	// wait for start or death

	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	err = (server.ExitType() == EExitPanic) ? KErrServerTerminated : stat.Int();

	server.Close();

	return err;
	}

RRemCon::RRemCon(TRemConClientType aClientType) 
:	iClientType(aClientType),
	iNumRemotesPckg(0),
	iReceivePckg(TRemConClientReceivePackage())
	{
	LOG_FUNC
	}

EXPORT_C TInt RRemCon::Connect()
	{
	LOG_FUNC

	TInt err = DoConnect();

	return err;
	}

EXPORT_C TInt RRemCon::Connect(const TPlayerType& aPlayerType, const TPlayerSubType& aPlayerSubType, const TDesC8& aName)
	{
	LOG_FUNC

	TInt err = DoConnect();
	if ( err == KErrNone )	
		{
		err = SetPlayerType(aPlayerType,aPlayerSubType,aName);
		if ( err != KErrNone )
			{
			// For this API to be clean, we must clean up the session handle 
			// already successfully created.
			Close();
			}
		}
	return err;
	}
/**
Connects the session, starting the server if necessary. This involves two IPC 
calls, the first to connect the session to the server, the second to set the 
server-side session's type correctly (controller or target).
@return Error.
*/
TInt RRemCon::DoConnect()
	{
	LOG_FUNC
	TInt retry = 2;
	
	FOREVER
		{
		// 4 is the number of asynchronous APIs which may be outstanding 
		// simultaneously.
		TInt err = CreateSession(KRemConServerName, Version(), 4);

		if ((err != KErrNotFound) && (err != KErrServerTerminated))
			{
			return err;
			}

		if (--retry == 0)
			{
			return err;
			}

		err = StartServer();

		if ((err != KErrNone) && (err != KErrAlreadyExists))
			{
			return err;
			}
		}
	}

/**
Does IPC with the server to set the type of the session from our member (set 
at construction time).
@return Error.
*/
TInt RRemCon::SetPlayerType(const TPlayerType& aPlayerType, const TPlayerSubType& aPlayerSubType, const TDesC8& aName)
	{
	LOG_FUNC
	TIpcArgs args;
	// Message slot 0 is not currently used
	iPlayerTypePckg().iPlayerType = aPlayerType;
	iPlayerTypePckg().iPlayerSubType = aPlayerSubType;
	args.Set(1,&iPlayerTypePckg);
	args.Set(2, &aName);
	return SendReceive(ERemConSetPlayerType, args);
	}

EXPORT_C TInt RRemCon::SendUnreliable(TUid aInterfaceUid, 
		TUint aOperationId, 
		TRemConMessageSubType aSubType,
		const TDesC8& aData)
	{
	LOG_FUNC
	TIpcArgs args;
	iOpInfoPckg().iOperationId = aOperationId;
	iOpInfoPckg().iMessageSubType = aSubType;
	
	args.Set(0, aInterfaceUid.iUid);
	args.Set(1, &iOpInfoPckg);
	args.Set(3, &aData);
	TInt err = SendReceive(ERemConSendUnreliable, args); 
	return err;
	}


EXPORT_C void RRemCon::Send(TRequestStatus& aStatus, 
		TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint& aNumRemotes,
		TRemConMessageSubType aSubType,
		const TDesC8& aData)
	{
	LOG_FUNC

	TIpcArgs args;
	iOpInfoPckg().iOperationId = aOperationId;
	iOpInfoPckg().iMessageSubType = aSubType;
	args.Set(0, aInterfaceUid.iUid);
	args.Set(1, &iOpInfoPckg);
	iNumRemotesPckg.Set((TUint8*)&aNumRemotes, sizeof(TUint), sizeof(TUint));
	args.Set(2, &iNumRemotesPckg);
	args.Set(3, &aData);

	SendReceive(ERemConSend,
		args,
		aStatus
		);
	}

/**
Sends a notify command to the remote device.

@see RRemCon::Send()
*/
EXPORT_C void RRemCon::SendNotify(TRequestStatus& aStatus, 
		TUid aInterfaceUid, 
		TUint aOperationId, 
		TRemConMessageSubType aSubType,
		const TDesC8& aData)
	{
	LOG_FUNC

	TIpcArgs args;
	iOpInfoPckg().iOperationId = aOperationId;
	iOpInfoPckg().iMessageSubType = aSubType;
	args.Set(0, aInterfaceUid.iUid);
	args.Set(1, &iOpInfoPckg);
	args.Set(2, &aData);
		
	SendReceive(ERemConSendNotify,
		args,
		aStatus
		);
	}
	
EXPORT_C TInt RRemCon::SendCancel()
	{
	LOG_FUNC
	
	//Ignore Return code because
	// a) It'll mostly be other than KErrNone because the server has terminated, in which
	//    case the original async request will have completed with the error anyway!
	// b) It's meaningless to the client whatever the return code is.
	(void)SendReceive(ERemConSendCancel);
	
	return KErrNone; 
	}

EXPORT_C void RRemCon::Receive(TRequestStatus& aStatus, 
							   TRemConClientReceivePackage& aReceivePackage,
							   TDes8& aData)
	{
	LOG_FUNC

	TIpcArgs args;
	iReceivePckg.Set(reinterpret_cast<TUint8*>(&aReceivePackage), sizeof(TRemConClientReceivePackage), sizeof(TRemConClientReceivePackage));
	args.Set(0, &iReceivePckg);
	args.Set(1, &aData);

	SendReceive(ERemConReceive, 
		args,
		aStatus);
	}

EXPORT_C TInt RRemCon::ReceiveCancel()
	{
	LOG_FUNC

	// See RRemCon::SendCancel() for comment
	(void)SendReceive(ERemConReceiveCancel);
	
	return KErrNone;
	}

EXPORT_C TInt RRemCon::GetConnections(TSglQue<TRemConAddress>& aConnections)
	{
	LOG_FUNC

	// Plan: 
	// 1/ Empty the given queue (in case of any subsequent error).
	// 2/ First IPC call: get the current number of connections from the 
	// server and allocate a buffer to read their statuses into. 
	// 3/ Second IPC call: read the connections into that buffer, and translate 
	// them into the client's queue.
	// What if the set of connections changes between these 2 IPC calls? The 
	// server-side session needs to remember the set of connection statuses at 
	// the point in time of the 1st IPC call, to correctly answer the 2nd IPC 
	// call. This means the server-side session has to queue connection state 
	// changes that occur in the meantime, which can fail due to memory 
	// allocation. This is OK because (a) they can fail a new connection 
	// indication back to the bearer, and (b) they can make sure they can 
	// handle a disconnection indication by pre-allocating (at connection 
	// establishment time) enough memory to be able to remember its future 
	// disconnection.

	// 1/
	aConnections.Reset();

	// 2/
	TUint connCount = 0;
	TPckg<TUint> countBuf(connCount);
	TInt err = SendReceive(ERemConGetConnectionCount, TIpcArgs(&countBuf));
	// Only bother to get the statuses if there are some to get- otherwise 
	// just give the client back their empty queue and KErrNone.
	if ( err == KErrNone && connCount != 0 )
		{
		RBuf8 buf;
		err = buf.Create(connCount * sizeof(TRemConAddress));
		if ( err == KErrNone )
			{
			// 3/
			err = SendReceive(ERemConGetConnections, TIpcArgs(&buf));
			if ( err == KErrNone )
				{
				// Read the statuses into the client's array.
				for ( TUint ii = 0 ; ii < connCount ; ii++ )
					{
					// Divide the data in to TRemConAddress portions
					TRemConAddress* tempAddr = new TRemConAddress();
					if (tempAddr)
						{
						Mem::Copy((TAny*)(tempAddr),(TAny*)(buf.Ptr()+(ii*sizeof(TRemConAddress))) , sizeof(TRemConAddress));	
						aConnections.AddLast(*tempAddr);			
						}
					else
						{
						// Perform the cleanup in case of error
						TSglQueIter<TRemConAddress> iter(aConnections);
						iter.SetToFirst();
						TRemConAddress* addr;
						while (( addr = iter++ )  != NULL )
							{
							aConnections.Remove(*addr);
							delete addr;	
							};
							
						err = KErrNoMemory;
						break;				
						}
					}
				}
			// The data that was in buf is passed into small appBuf's 
			// and it is now owned by the client (via their TSglQue).
			}
		// Cleanup the allocated buffer for statuses info
		buf.Close();
		}
	return err;
	}

EXPORT_C void RRemCon::NotifyConnectionsChange(TRequestStatus& aStatus)
	{
	LOG_FUNC

	SendReceive(ERemConNotifyConnectionsChange, 
		aStatus);
	}

EXPORT_C TInt RRemCon::NotifyConnectionsChangeCancel()
	{
	LOG_FUNC

	// See RRemCon::SendCancel() for comment
	(void)SendReceive(ERemConNotifyConnectionsChangeCancel);

	return KErrNone;
	}

EXPORT_C TInt RRemCon::RegisterInterestedAPIs(const TDesC8& aAPIs)
	{
	LOG_FUNC
	return SendReceive(ERemConRegisterInterestedAPIs, TIpcArgs(&aAPIs));
	}

EXPORT_C TInt RRemCon::__DbgMarkHeap()
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ERemConDbgMarkHeap);
#else
	return KErrNone;
#endif
	}

EXPORT_C TInt RRemCon::__DbgCheckHeap(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ERemConDbgCheckHeap, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

EXPORT_C TInt RRemCon::__DbgMarkEnd(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ERemConDbgMarkEnd, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

EXPORT_C TInt RRemCon::__DbgFailNext(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ERemConDbgFailNext, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

EXPORT_C RRemConController::RRemConController()
:	RRemCon(ERemConClientTypeController)
	{
	LOG_FUNC
	}

EXPORT_C TInt RRemConController::GoConnectionOriented(const TRemConAddress& aConnection)
	{
	LOG_FUNC

	return SendReceive(ERemConGoConnectionOriented,
			TIpcArgs(aConnection.BearerUid().iUid, 
					&aConnection.Addr())
		);
	}

EXPORT_C TInt RRemConController::GoConnectionless()
	{
	LOG_FUNC

	return SendReceive(ERemConGoConnectionless);
	}

EXPORT_C void RRemConController::ConnectBearer(TRequestStatus& aStatus)
	{
	LOG_FUNC

	SendReceive(ERemConConnectBearer, 
		aStatus);
	}

EXPORT_C TInt RRemConController::ConnectBearerCancel()
	{
	LOG_FUNC

	// See RRemCon::SendCancel() for comment
	(void)SendReceive(ERemConConnectBearerCancel);
	
	return KErrNone;
	}

EXPORT_C void RRemConController::DisconnectBearer(TRequestStatus& aStatus)
	{
	LOG_FUNC

	SendReceive(ERemConDisconnectBearer, 
		aStatus);
	}

EXPORT_C TInt RRemConController::DisconnectBearerCancel()
	{
	LOG_FUNC

	// See RRemCon::SendCancel() for comment
	(void)SendReceive(ERemConDisconnectBearerCancel);

	return KErrNone;
	}

TVersion RRemConController::Version() const
	{
	LOG_FUNC
	return(TVersion(	KRemConSrvMajorVersionNumber,
						KRemConSrvMinorVersionNumber,
						KRemConSrvControllerSession
					)
			);
	}

EXPORT_C RRemConTarget::RRemConTarget()
:	RRemCon(ERemConClientTypeTarget)
	{
	LOG_FUNC
	}

TVersion RRemConTarget::Version() const
	{
	LOG_FUNC
	return(TVersion(	KRemConSrvMajorVersionNumber,
						KRemConSrvMinorVersionNumber,
						KRemConSrvTargetSession
					)
			);
	}

