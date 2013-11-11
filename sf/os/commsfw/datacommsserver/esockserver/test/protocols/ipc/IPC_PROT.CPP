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

#include <es_ver.h>
#include "IPC_MAIN.H"
#include "ES_IPC.H"

CIpcProtocolHolder::CIpcProtocolHolder()
	:iSAPs(_FOFF(CIpcProvdBase,iLink))
	{
	}

void CIpcProtocolHolder::SocketRemoved(TInt aPort)
//
// Called from a Service providers destructor so we can keep a count of the sockets.
//
	{
	iNumSockets--;
	if (aPort)
		iPortNumbers->Free(aPort);
	}

TInt CIpcProtocolHolder::GetNextFreePort()
//
// The guts of AutoBind
//
	{
	return iPortNumbers->Alloc();
	}

CIpcProtocolHolder* CIpcProtocolHolder::NewL()
	{
	CIpcProtocolHolder* p=new(ELeave)CIpcProtocolHolder;
	CleanupStack::PushL(p);
	p->iPortNumbers=CBitMapAllocator::NewL(KIPSNumberSockets+1);
	p->iPortNumbers->Alloc();	// we reserve port 0 to indicate unset.
	CleanupStack::Pop();
	return p;
	}

CIpcProtocolHolder::~CIpcProtocolHolder()
	{
	if (iPortNumbers)
		{
		// Check that there is only one socket allocated (we always allocate 0 in CIpcProtocolHolder::NewL above)
		__ASSERT_DEBUG(iPortNumbers->Avail()==iPortNumbers->Size()-1,Panic(ECloseWithoutDeleteingAllSockets));
		delete iPortNumbers;
		}
	}

TInt CIpcProtocolHolder::CheckAndAllocatePortNumber(TInt aPort)
	{

	if(aPort<1 || aPort>KIPSNumberSockets)
		return KErrTooBig;

	if (iPortNumbers->IsFree(aPort))
		{
		iPortNumbers->AllocAt(aPort);
		return KErrNone;
		}
	else
		return KErrInUse;
	}

void CIpcProtocolHolder::Add(CIpcProvdBase* aSAP)
	{
	iNumSockets++;
	iSAPs.AddFirst(*aSAP);
	}

CIpcProvdBase* CIpcProtocolHolder::FindPeerForConnection(TInt aPort)
	{
	TDblQueIter<CIpcProvdBase> i(iSAPs);
	CIpcProvdBase* p;
	while (p=i++,p!=NULL)
		{
		if (p->iLocalAddr==aPort)
			return p;
		}
	return NULL;
	}


CIpcProtocol::~CIpcProtocol()
	{
	delete iStreamProtocolSAPs;
	}

CIpcProtocol::CIpcProtocol(TInt aProtocol)
	{
	__DECLARE_NAME(_S("CIpcProtocol"));
	
	iProtocol=aProtocol;
	}

CIpcProtocol* CIpcProtocol::NewL(TInt aProtocol)
	{
	CIpcProtocol* p=new(ELeave)CIpcProtocol(aProtocol);
	CleanupStack::PushL(p);
	p->iStreamProtocolSAPs=CIpcProtocolHolder::NewL();
	CleanupStack::Pop(p);
	return p;
	}

CServProviderBase *CIpcProtocol::NewSAPL(TUint /*aProtocol*/)
//
// Socket server asking for a host resolver
//
	{

	CIpcProvdBase* s=NULL;
	switch(iProtocol)
		{
	case KSockStream:
		if (iStreamProtocolSAPs->iNumSockets==KIPSNumberSockets)
			User::Leave(KErrTooBig);
		s=CIpcStreamProvd::NewL(iStreamProtocolSAPs);
		iStreamProtocolSAPs->Add(s);
		break;
	case KSockDatagram:
		User::Leave(KErrNotSupported);
		break;
		}
	
	return s;
	}

TBool CIpcProtocol::CanCreateSockets()
//
// Very rude question from the socket server. Doesn't even say Excuse me or please.
//
	{
	return ETrue;
	}

void CIpcProtocol::InitL(TDesC& /*aTag*/)
//
// InitL call from socket server. Do nothing
//
	{
	}

void CIpcProtocol::BindL(CProtocolBase* /*aProtocol*/, TUint /*anId*/)
//
// BindL call from peer protocol
//
	{
	Panic(ECantBind);
	}

void CIpcProtocol::StartL(void)
//
// StartL call from socket server. Do nothing
//
	{
	
	}

void CIpcProtocol::Identify(TServerProtocolDesc *aDesc)const
//
// Identify request from SOCKET server
//
	{
	_LIT(ipcStream,"IPC Stream");
	_LIT(ipcDatagramm,"IPC Datagramm");
	switch(iProtocol)
		{
	case KSockStream:

		aDesc->iName=ipcStream;
		aDesc->iAddrFamily=KIPCAddrFamily;
		aDesc->iSockType=KSockStream;
		aDesc->iProtocol=KIPCStreamProtocol;

		aDesc->iVersion=TVersion(KES32MajorVersionNumber,KES32MinorVersionNumber,KES32BuildVersionNumber);
		aDesc->iByteOrder=ELittleEndian;
		aDesc->iServiceInfo=0;
		aDesc->iNamingServices=0;
		aDesc->iSecurity=KSocketNoSecurity;
		aDesc->iMessageSize=KSocketMessageSizeIsStream;
		aDesc->iServiceTypeInfo=0;
		aDesc->iNumSockets=KIPSNumberSockets;
		return;
	case KSockDatagram:
		aDesc->iName=ipcDatagramm;
		aDesc->iAddrFamily=KIPCAddrFamily;
		aDesc->iSockType=KSockDatagram;
		aDesc->iProtocol=KIPCDatagramProtocol;

		aDesc->iVersion=TVersion(KES32MajorVersionNumber,KES32MinorVersionNumber,KES32BuildVersionNumber);
		aDesc->iByteOrder=ELittleEndian;
		aDesc->iServiceInfo=KIPCDatagramServiceInfo;
		aDesc->iNamingServices=0;
		aDesc->iSecurity=KSocketNoSecurity;
		aDesc->iMessageSize=KSocketMessageSizeNoLimit;
		aDesc->iServiceTypeInfo=0;
		aDesc->iNumSockets=KIPSNumberSockets;
		return;
		}

	}

void CIpcProtocol::BindToL(CProtocolBase* /*protocol*/)
//
// BindL call from socket server
//
	{
	Panic(ECantBindTo);
	}

TInt CIpcProtocol::Send(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
//
// Send Down call from bindee
//
	{
	Panic(ESendCallCantBind);
	return 0;
	}

void CIpcProtocol::Process(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
//
// Process up call from bindee
//
	{
	Panic(EProcessCallCantBind);
	}

TInt CIpcProtocol::Send(TDes8 &, TSockAddr* /*to*/,TSockAddr* /*from*/,CProtocolBase* /*aSourceProtocol*/)
//
// Send down call from bindee
//
	{
	Panic(ESendCallCantBind);
	return KErrNone;
	}

void CIpcProtocol::Process(TDes8 & ,TSockAddr* /*from*/,TSockAddr* /*to*/,CProtocolBase* /*aSourceProtocol*/)
//
// Process up call from bindee
//
	{
	Panic(EProcessCallCantBind);
	}


TInt CIpcProtocol::GetOption(TUint /*level*/,TUint /*name*/,TDes8 & /*anOption*/,CProtocolBase* /*aSourceProtocol*/)
//
// GetOption Down call from bindee
//
	{
	Panic(EGetOptionCallCantBind);
	return KErrNone;
	}

TInt CIpcProtocol::SetOption(TUint /*level*/,TUint /*name*/,const TDesC8& /*option*/,CProtocolBase* /*aSourceProtocol*/)
//
// SetOption Down call from bindee
//
	{
	Panic(ESetOptionCallCantBind);
	return KErrNone;
	}

void CIpcProtocol::Error(TInt /*anError*/,CProtocolBase* /*aSourceProtocol*/)
//
// Error up call from bindee
//
	{
	Panic(EErrorUpCallCantBind);
	}

CHostResolvProvdBase *CIpcProtocol::NewHostResolverL()
//
// Socket server asking for a host resolver
//
	{
	Panic(ECantCreateHostResolver);
	return NULL;
	}

CServiceResolvProvdBase *CIpcProtocol::NewServiceResolverL()
//
// Socket server asking for a service resolver
//
	{
	Panic(ECantCreateServiceResolver);
	return NULL;
	}

CNetDBProvdBase* CIpcProtocol::NewNetDatabaseL()
//
// Socket server asking for a net data base
//
	{
	Panic(ECantCreateNetDatabase);
	return NULL;
	}
