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
//

#include <e32std.h>
#include "IPC_MAIN.H"
#include "ES_IPC.H"

CIpcProvdBase::CIpcProvdBase(CIpcProtocolHolder* aProtocol)
	{
	__DECLARE_NAME(_S("CIpcProvdBase"));
	iProtocol=aProtocol;
	}

CIpcProvdBase::~CIpcProvdBase()
	{
	// ensure that another CIpcProvdBase doesn't end up with a dangling pointer to this CIpcProvdBase
	if (iConnection && iConnection->iConnection == this)
		{
		iConnection->iConnection = NULL;
		}
	
	iProtocol->SocketRemoved(iLocalAddr);
	}

void CIpcProvdBase::RemName(TSockAddr &anAddr)const
//
// Get the remote name we would connect to
//
	{
	anAddr.SetPort(iRemoteAddr);
	}

TInt  CIpcProvdBase::SetRemName(TSockAddr& anAddr)
//
// Set the remote name prior to connecting (or as part of a send to)
//
	{
	iRemoteAddr=anAddr.Port();
	return KErrNone;
	}

void CIpcProvdBase::LocalName(TSockAddr &anAddr)const
//
//  Get hte local name
//
	{
	anAddr.SetPort(iLocalAddr);
	}

TInt CIpcProvdBase::SetLocalName(TSockAddr &anAddr)
//
// Set the local name prior to accepting
//
	{
	TInt ret=iProtocol->CheckAndAllocatePortNumber(anAddr.Port());
	if (ret==KErrNone)
		iLocalAddr=anAddr.Port();
	return ret;
	}

void CIpcProvdBase::AutoBind( void )
//
// Automatically set the local name
//
	{
	iLocalAddr=iProtocol->GetNextFreePort();
	}

TInt CIpcProvdBase::GetOption(TUint /*level*/,TUint /*name*/,TDes8& /*anOption*/)const
//
//
//
	{
	return KErrNotSupported;
	}

TInt CIpcProvdBase::SetOption(TUint /*level*/,TUint /*name*/,const TDesC8& /*anOption*/)
//
//
//
	{
	return KErrNotSupported;
	}



void CIpcProvdBase::Ioctl(TUint /*level*/,TUint /*name*/,TDes8 * /*anOption*/)
//
//
//
	{
	iSocket->Error(KErrNotSupported,MSocketNotify::EErrorIoctl);
	}

void CIpcProvdBase::CancelIoctl(TUint /*aLevel*/,TUint /*aName*/)
	{
	}

TInt CIpcProvdBase::PassiveOpen(TUint aQue,const TDesC8 &/*aConnectionData*/)
//
//
//
	{
	return PassiveOpen(aQue);
	}

void CIpcProvdBase::Shutdown(TCloseType option,const TDesC8 &/*aDisconnectData*/)
//
// Shutdown with disconnect data
//
	{
	Shutdown(option);
	}

void CIpcProvdBase::ActiveOpen(const TDesC8 &/*aConnectionData*/)
//
// Active open with connection data - pass it on
//
	{
	ActiveOpen();
	}

void CIpcProvdBase::Start()
//
// Start the provider - do nothing
//
	{
	}


void CIpcProvdBase::ActiveOpen(void)
//
// Actively connect to a peer socket
//
	{

	CIpcProvdBase* peer=iProtocol->FindPeerForConnection(iRemoteAddr);
	CIpcProvdBase* newSocket=NULL;
	if (peer)
		newSocket=peer->GetCloneSocket();
	if (newSocket)
		{
		iConnection=newSocket;
		newSocket->iConnection=this;
		newSocket->iRemoteAddr=iLocalAddr;
		iSocket->ConnectComplete();
		peer->Connected(*newSocket);
		}
	else
		iSocket->Error(KErrCouldNotConnect);
	}

void CIpcProvdBase::Shutdown(TCloseType aType)
	{
	if (iConnection)
		iConnection->Disconnect();
	iConnection=NULL;
	if (aType!=EImmediate)
		iSocket->CanClose();
	}

void CIpcProvdBase::Disconnect()
	{

	if (iConnection)
		{
		if (iSocket)
			{
			iSocket->Disconnect();
			}
		iConnection=NULL;
		}

	iState=EDisconnected;
	}

TInt CIpcProvdBase::PassiveOpen(TUint /*aQue*/)
	{
	iState=EWaitingConnect;
	return KErrNone;
	}

void CIpcProvdBase::Connected(CIpcProvdBase& aSocket)
	{
	iSocket->ConnectComplete(aSocket);
	}

//

CIpcStreamProvd* CIpcStreamProvd::NewL(CIpcProtocolHolder* aProtocol)
	{
	CIpcStreamProvd* p=new(ELeave)CIpcStreamProvd(aProtocol);
	CleanupStack::PushL(p);
	p->iBuffer.SetLengthL(KIPCStreamDefaultBufferSize);
	CleanupStack::Pop();
	return p;
	}

CIpcStreamProvd::CIpcStreamProvd(CIpcProtocolHolder* aProtocol)
	:CIpcProvdBase(aProtocol)
	{
	__DECLARE_NAME(_S("CIpcStreamProvd"));
	}

CIpcProvdBase* CIpcStreamProvd::GetCloneSocket()
	{
	CIpcStreamProvd* newSock=NULL;
	if (iState!=EWaitingConnect)
		return newSock;

	TRAPD(ret,newSock=CIpcStreamProvd::NewL(iProtocol));
	if (ret!=KErrNone)
		{
		iSocket->Error(ret,MSocketNotify::EErrorConnect);
		return NULL;
		}

	newSock->AutoBind();
	iProtocol->Add(newSock);
	return newSock;
	}


TUint CIpcStreamProvd::Write(const TDesC8& aDesc,TUint /*options*/, TSockAddr* /*anAddr*/)
	{
	__ASSERT_DEBUG(iSocket,Panic(EBadWriteCall));
	if (!iConnection)
		{
		iSocket->Error(KErrDisconnected);
		// error occured, the only possible error for this protocol is KErrNoMemory
		return 0;
		}
	TInt len=Min(aDesc.Size(),iBuffer.Length()-iBuffer.Count());
	if (len)
		{
		iBuffer.Add(aDesc.Ptr(),len);
		iConnection->NewData(len);
		}
	else
		{
		// error occured on attempt to send data, the only possible error for 
		// this protocol is KErrNoMemory
		return 0;
		}
		
	return len;
	}

void CIpcStreamProvd::GetData(TDes8 &aDesc,TUint /*options*/,TSockAddr* /*anAddr*/)
	{
	__ASSERT_DEBUG(iConnection,Panic(EBadWriteCall));
	__ASSERT_DEBUG(iSocket,Panic(EBadWriteCall));

	CCirBuffer& buf=((CIpcStreamProvd *)iConnection)->iBuffer;

	__ASSERT_DEBUG(aDesc.Length()<=buf.Count(),Panic(EReadGetTooMuch));

	buf.Remove((TUint8 *)aDesc.Ptr(),aDesc.Length());
	iConnection->CanSend();
	}

void CIpcStreamProvd::NewData(TInt aLen)
//
// New data has arrived at our oppo.
//
	{
	iSocket->NewData(aLen);
	}

void CIpcStreamProvd::CanSend()
//
// New space has been made (in our buffer) by at our oppo.
//
	{
	iSocket->CanSend();
	}
