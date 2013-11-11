// btserialclient.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

// Transient server example - client interface implementation

#include <fshell/btserialclient.h>
#include "btserialclientserver.h"
#include "..\connection\btdebug.h"

_LIT(KSubThreadName, "btconsolethread");
static const TInt KMaxHeapSize = 0x100000; // 1M

TInt RBtSerialSession::StartServer()
//
// Start the server process. Simultaneous launching
// of two such processes should be detected when the second one attempts to
// create the server object, failing with KErrAlreadyExists.
//
	{
	TInt err = iSubThread.Create(KSubThreadName, &ServerThread, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, &iServer);
	
	if (err != KErrNone)
		{
		return err;
		}
	
	TRequestStatus rendezvous;
	iSubThread.Rendezvous(rendezvous);
	
	iSubThread.Resume();
	User::WaitForRequest(rendezvous);
	
	return rendezvous.Int();
	}

//______________________________________________________________________________
//						RBtSerialSession
EXPORT_C TInt RBtSerialSession::Connect()
	{
	TInt err = StartServer();
	if (err == KErrNone)
		{
		TVersion ver;
		err = CreateSession(iServer, ver);
		}
	return err;
	}

EXPORT_C void RBtSerialSession::Close()
	{
	TRACE1("RBtSerialSession::Close closing IPC session");
	RSessionBase::Close();
	// when we close the session (above) the server thread will exit.
	// logon and wait for it
	TRequestStatus stat;
	iSubThread.Logon(stat);
	TRACE1("RBtSerialSession::Close waiting for thread to exit");
	User::WaitForRequest(stat);	
	iSubThread.Close();
	TRACE2("RBtSerialSession::Close done (%d)", stat.Int());
	}
	
EXPORT_C void RBtSerialSession::WaitForConnection(TRequestStatus& aStatus)
	{
	SendReceive(EBtSerialWaitForConnection, aStatus);
	}
	
EXPORT_C void RBtSerialSession::CancelWaitForConnection()
	{
	SendReceive(EBtSerialCancelWaitForConnection);
	}
	
EXPORT_C TInt RBtSerialSession::IsConnected() const
	{
	return SendReceive(EBtSerialIsConnected);
	}
	
EXPORT_C void RBtSerialSession::Write(const TDesC8& aData, TRequestStatus& aStatus)
	{
	SendReceive(EBtSerialWrite, TIpcArgs(&aData), aStatus);
	}
	
EXPORT_C void RBtSerialSession::CancelWrite()
	{
	SendReceive(EBtSerialCancelWrite);
	}
	
EXPORT_C void RBtSerialSession::Read(TDes8& aData, TRequestStatus& aStatus) const
	{
	SendReceive(EBtSerialRead, TIpcArgs(&aData), aStatus);
	}
	
EXPORT_C void RBtSerialSession::CancelRead() const
	{
	SendReceive(EBtSerialCancelRead);
	}

EXPORT_C TInt RBtSerialSession::GetConnectedDeviceName(TDes& aName)
	{
	return SendReceive(EBtSerialGetConnectedDeviceName, TIpcArgs(&aName));
	}
	
EXPORT_C TInt RBtSerialSession::GetConnectedDeviceAddr(TBTDevAddr& aAddr)
	{
	TPckg<TBTDevAddr> pckg(aAddr);
	return SendReceive(EBtSerialGetConnectedDeviceAddr, TIpcArgs(&pckg));
	}
