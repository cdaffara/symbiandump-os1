// btserialsession.cpp
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

#include "btserialsession.h"
#include "btserialserver.h"
#include "btserialclient.h"
#include <fshell/btincomingserial.h>
#include "..\connection\btdebug.h"


void PanicClient(const RMessagePtr2& aMessage,TBtSerialServerPanic aPanic)
//
// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
//
	{
	_LIT(KPanic,"BtSerialSrv");
	aMessage.Panic(KPanic,aPanic);
	}

//______________________________________________________________________________
//						CBtSerialSession
void CBtSerialSession::CreateL()
	{
	TRACE1("CBtSerialSession::CreateL");
	iConnection = CBluetoothIncomingSerialConnection::NewL(*this);
	}

CBtSerialSession::CBtSerialSession()
	{
	}

CBtSerialSession::~CBtSerialSession()
	{
	TRACE1("CBtSerialSession::~CBtSerialSession");
	delete iConnection;
	iReadBuf.Close();
	iWriteBuf.Close();	
	Server().DropSession();
	}

void CBtSerialSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case EBtSerialWaitForConnection:
		WaitForConnectionL(aMessage);
		break;
	case EBtSerialCancelWaitForConnection:
		CancelWaitForConnection(aMessage);
		break;
	case EBtSerialIsConnected:
		IsConnectedL(aMessage);
		break;
	case EBtSerialWrite:
		WriteL(aMessage);
		break;
	case EBtSerialCancelWrite:
		CancelWrite(aMessage);
		break;
	case EBtSerialRead:
		ReadL(aMessage);
		break;
	case EBtSerialCancelRead:
		CancelRead(aMessage);
		break;
	case EBtSerialGetConnectedDeviceAddr:
		GetConnectedDeviceAddrL(aMessage);
		break;
	case EBtSerialGetConnectedDeviceName:
		GetConnectedDeviceNameL(aMessage);
		break;
	default:
		PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}
	
void CBtSerialSession::Disconnect(const RMessage2& aMessage)
	{
	if (iConnection->Connected())
		{
		TRACE1("CBtSerialSession::Disconnect shutting down connection & storing message");
		iDisconnectMessage = aMessage;
		iConnection->ShutDown();
		}
	else
		{
		CSession2::Disconnect(aMessage);
		}
	}

void CBtSerialSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	if (aError==KErrBadDescriptor)
		PanicClient(aMessage,EPanicBadDescriptor);
	CSession2::ServiceError(aMessage,aError);
	}

inline CBtSerialServer& CBtSerialSession::Server()
	{
	return *static_cast<CBtSerialServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

void CBtSerialSession::WaitForConnectionL(const RMessage2& aMessage)
	{
	TRACE1("CBtSerialSession::WaitForConnectionL");
	if (iConnection->Connected())
		{
		aMessage.Complete(KErrNone);
		return;
		}
	if (!iConnectedMessage.IsNull())
		{
		User::Leave(KErrAlreadyExists);
		}
	iConnectedMessage = aMessage;
	if (!iWaitingForConnection)
		{
		iConnection->WaitForConnection();
		}
	}
	
void CBtSerialSession::CancelWaitForConnection(const RMessage2& aMessage)
	{
	TRACE1("CBtSerialSession::CancelWaitForConnection");
	if (!iConnectedMessage.IsNull())
		{
		iConnectedMessage.Complete(KErrCancel);
		}
	aMessage.Complete(KErrNone);
	}
	
void CBtSerialSession::IsConnectedL(const RMessage2& aMessage)
	{
	TRACE1("CBtSerialSession::IsConnectedL");
	aMessage.Complete(iConnection->Connected());
	}
	
void CBtSerialSession::WriteL(const RMessage2& aMessage)
	{
	TRACE1("CBtSerialSession::WriteL");
	if (!iWriteMessage.IsNull())
		{
		User::Leave(KErrInUse);
		}
	TInt len = aMessage.GetDesLengthL(0);
	if (iWriteBuf.MaxLength() < len)
		{
		RBuf8 buf;
		buf.CreateL(len);
		buf.Swap(iWriteBuf);
		buf.Close();
		}
	iWriteBuf.Zero();
	aMessage.ReadL(0, iWriteBuf);
	iWriteMessage = aMessage;
	if (iConnection->Connected())
		{
		iConnection->Write(iWriteBuf);
		}
	// else the write will be performed when we are connected
	}
	
void CBtSerialSession::CancelWrite(const RMessage2& aMessage)
	{
	TRACE1("CBtSerialSession::CancelWrite");
	iConnection->CancelWrite();
	iWriteBuf.Zero();
	if (!iWriteMessage.IsNull())
		{
		iWriteMessage.Complete(KErrCancel);
		}
	aMessage.Complete(KErrNone);
	}
	
void CBtSerialSession::ReadL(const RMessage2& aMessage)
	{
	TRACE1("CBtSerialSession::ReadL");
	if (!iReadMessage.IsNull())
		{
		User::Leave(KErrInUse);
		}
	TInt len = aMessage.GetDesMaxLengthL(0);
	if (iReadBuf.MaxLength() != len) // better to add length param to CBluetoothIncomingSerialConnection::Read
		{
		RBuf8 buf;
		buf.CreateL(len);
		buf.Swap(iReadBuf);
		buf.Close();
		}
	iReadBuf.Zero();
	iReadMessage = aMessage;
	if (iConnection->Connected())
		{
		iConnection->Read(iReadBuf);
		}
	}
	
void CBtSerialSession::CancelRead(const RMessage2& aMessage)
	{
	TRACE1("CBtSerialSession::CancelRead");
	iConnection->CancelRead();
	if (!iReadMessage.IsNull())
		{
		iReadMessage.Complete(KErrCancel);
		}
	aMessage.Complete(KErrNone);
	}

void CBtSerialSession::GetConnectedDeviceNameL(const RMessage2& aMessage)
	{
	if (!iGetNameMessage.IsNull())
		{
		User::Leave(KErrAlreadyExists);
		}
	iGetNameMessage = aMessage;
	iConnection->GetConnectedDeviceNameL();
	}

void CBtSerialSession::GetConnectedDeviceAddrL(const RMessage2& aMessage)
	{
	TBTDevAddr addr = iConnection->ConnectedDeviceAddrL();
	TPckg<TBTDevAddr> pckg(addr);
	aMessage.WriteL(0, pckg);
	aMessage.Complete(KErrNone);
	}

void CBtSerialSession::BtSerialConnected()
	{
	TRACE1("CBtSerialSession::BtSerialConnected");
	if (!iConnectedMessage.IsNull())
		{
		iConnectedMessage.Complete(KErrNone);
		}
	if (!iWriteMessage.IsNull())
		{
		iConnection->Write(iWriteBuf);
		}
	if (!iReadMessage.IsNull())
		{
		iConnection->Read(iReadBuf);
		}
	}
	
void CBtSerialSession::BtWriteComplete(TInt aError)
	{
	TRACE1("CBtSerialSession::BtWriteComplete");
	iWriteMessage.Complete(aError);
	}
	
void CBtSerialSession::BtReadComplete(TInt aError)
	{
	TRACE1("CBtSerialSession::BtReadComplete");
	if (aError == KErrNone)
		{
		aError = iReadMessage.Write(0, iReadBuf);			
		}
	iReadMessage.Complete(aError);
	}
	
void CBtSerialSession::BtSerialError(TInt aError)
	{
	TRACE1("CBtSerialSession::BtSerialError");
	if (!iConnectedMessage.IsNull())
		{
		iConnectedMessage.Complete(aError);
		}
	}
	
void CBtSerialSession::BtShutdownComplete(TInt /*aError*/)
	{
	TRACE1("CBtSerialSession::BtShutdownComplete");
	CSession2::Disconnect(iDisconnectMessage);
	}
	
void CBtSerialSession::BtNameReceived(TInt aError, const TDesC& aName)
	{
	TInt len = 0;
	if (aError == KErrNone)
		{
		aError = iGetNameMessage.GetDesMaxLength(0);
		if (aError >= 0)
			{
			len = aError;
			aError = KErrNone;
			}
		}
	if (aError == KErrNone)
		{
		aError = iGetNameMessage.Write(0, aName.Left(len));
		}
	if (aError == KErrNone)
		{
		if (aName.Length() > len)
			{
			aError = KErrOverflow;
			}
		}
	iGetNameMessage.Complete(aError);
	}

