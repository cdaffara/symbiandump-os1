// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This implements the CL2CAPProtocol object that is the contact point from
// BT PRT to the L2cap stack
// 
//

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include <bluetooth/aclsockaddr.h>

#include "l2cap.h"

#include "l2sapstates.h"
#include "l2util.h"
#include "linkmgr.h"
#include "l2capSAPSignalHandler.h"
#include "l2sap.h"


#ifdef _DEBUG
#include "L2CapDebugControlInterface.h"
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

// Construction & Initialisation

CL2CAPProtocol::CL2CAPProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
 : CBluetoothProtocolBase(aSecMan, aControlPlane, aCodMan)
	{
	LOG_FUNC
	TCallBack cb(TryToClose, this);
	iIdleTimerEntry.Set(cb);
	}

CL2CAPProtocol::~CL2CAPProtocol()
	{
	LOG_FUNC
	RemoveIdleTimerEntry();

	delete iStateFactory;
	delete iSigStateFactory;
	
	if(LowerProtocol())
		{
		LowerProtocol()->Close();
		}

	// Delete the Mux controller.
	delete iMuxController;

#ifdef _DEBUG
	SBtTls* tls = static_cast<SBtTls*>(Dll::Tls());
	CDebugControlInterface* ptr = tls->iDebugCtrlIF;
	delete ptr;
	tls->iDebugCtrlIF = NULL;
#endif

#ifdef __FLOG_ACTIVE
	CLOSE_LOGGER
#endif
	}

CL2CAPProtocol* CL2CAPProtocol::NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
	{
#ifdef __FLOG_ACTIVE
	CONNECT_LOGGER
#endif
	LOG_STATIC_FUNC
	CL2CAPProtocol* prot = new (ELeave) CL2CAPProtocol(aSecMan, aControlPlane, aCodMan);
	CleanupStack::PushL(prot);
	prot->ConstructL();
	CleanupStack::Pop();
	return prot;
	}

void CL2CAPProtocol::ConstructL()
	{
	LOG_FUNC
	iMuxController = new (ELeave) CL2CAPMuxController(*this);	
	}

void CL2CAPProtocol::InitL(TDesC& /*aTag*/)
/**
	Pre-binding initialise.
	Allocate any objects we need here.
	The socket ensures that this function will only be called once per
	object of this class, regardless of how long it hangs around.
**/
	{
	LOG_FUNC
	// Init 
	__ASSERT_DEBUG(!iClosePending, Panic(EL2CAPProtocolOpenedAfterClose));
	__ASSERT_DEBUG(!iIdleEntryQueued, Panic(EL2CAPProtocolOpenedAfterClose));
	__ASSERT_DEBUG(!iStateFactory, Panic(EL2CAPProtocolOpenedAfterClose));
	__ASSERT_DEBUG(!iSigStateFactory, Panic(EL2CAPProtocolOpenedAfterClose));

	// Create all the things we need now
	LOG(_L("L2CAP : Initialising states"));
	iStateFactory=CL2CAPSAPStateFactory::NewL();
	iSigStateFactory=CL2CAPSignalStateFactory::NewL();
	LOG(_L("L2CAP : Initialising complete"));

#ifdef _DEBUG
	SBtTls* tls = static_cast<SBtTls*>(Dll::Tls());
	tls->iDebugCtrlIF = CDebugControlInterface::NewL();
#endif
	}


void CL2CAPProtocol::StartL()
/**
	Binding complete.
	Startup call from socket.  Do nothing
**/
	{
	LOG_FUNC
	}

// From higher protocol
void CL2CAPProtocol::BindL(CProtocolBase* /*aProtocol*/, TUint /*aId*/)
	{
	// Could register the protocol that's bound to us
	LOG_FUNC
	}

void CL2CAPProtocol::BindToL(CProtocolBase* aProtocol)
	{
	LOG_FUNC
#ifdef _DEBUG
	TServerProtocolDesc prtDesc;
	aProtocol->Identify(&prtDesc);

	if(prtDesc.iAddrFamily!=KBTAddrFamily ||
	   prtDesc.iProtocol!=KBTLinkManager)
		{
		LEAVEL(KErrBtEskError);
		}
#endif
	aProtocol->BindL(this, 0);
	aProtocol->Open();
	iLowerProtocol = static_cast<CBluetoothProtocolBase*>(aProtocol);
	}

// Factory functions
CServProviderBase* CL2CAPProtocol::NewSAPL(TUint aSockType)
/** 
	Create a new SAP.
	The SAP returned is owned by the caller -- this protocol will not clean it up.
	The socket uses this function to create a new SAP, and the socket will delete when it
	is finished with it.
**/
	{
	LOG_FUNC
	
	CL2CAPConnectionSAP* lsap = NULL;
		
	switch(aSockType)
		{
	case KSockSeqPacket:  // packet interface
		lsap=CL2CAPConnectionSAP::NewL(*this);
		break;
	default:
		LEAVEL(KErrNotSupported);
		break;
		}

	return lsap;
	}

// Query functions

void CL2CAPProtocol::Identify(TServerProtocolDesc *aDesc)const
//
// Identify request from SOCKET server
//
	{
	LOG_FUNC
	CL2CAPProtocol::ProtocolIdentity(aDesc);
	}

void CL2CAPProtocol::ProtocolIdentity(TServerProtocolDesc* aDesc)
	{
	LOG_STATIC_FUNC
	_LIT(name,"L2CAP");
	aDesc->iProtocol=KL2CAP;
	aDesc->iName=name;
	aDesc->iAddrFamily=KBTAddrFamily;
	aDesc->iSockType=KSockSeqPacket;
	
	aDesc->iVersion=TVersion(KBTMajor,KBTMinor,KBTBuild);
	aDesc->iByteOrder=ELittleEndian;
	aDesc->iServiceInfo=KL2CAPSeqPacketServiceInfo;
	aDesc->iNamingServices=0;
	aDesc->iSecurity=KSocketNoSecurity;
	aDesc->iMessageSize=KSocketMessageSizeNoLimit; //This is the MAX packet size.
	aDesc->iServiceTypeInfo=ESocketSupport|ETransport|EPreferMBufChains|ENeedMBufs|EUseCanSend;
	aDesc->iNumSockets=KL2CAPSockets;
	}

void CL2CAPProtocol::CloseNow()
/**
	Close command from base class.
	Called when ref count reaches 0.
	We don't actually have to close now, but when we finally
	do, we must call CanClose (done within TryToClose). That will
	actually delete us. In the mean time, if a new client tries
	to connect to L2CAP, the socket will use this existing one and just
	call Open on it.
	What we do here, is queue an idle timer entry. It's expiry will
	call TryToClose, which will actually do the close.
**/
	{
	LOG_FUNC
	iClosePending = ETrue;
	QueIdleTimerEntry();
	}

void CL2CAPProtocol::Open()
/**
	Open L2CAP protocol.
	Called every time a new client (of L2CAP) wants to use it.
**/
	{
	LOG_FUNC
	iClosePending = EFalse;
	RemoveIdleTimerEntry();
	CProtocolBase::Open();
	}

void CL2CAPProtocol::Close()
	{
	LOG_FUNC
	CProtocolBase::Close();
	}

void CL2CAPProtocol::QueIdleTimerEntry()
/**
	Queue idle timer entry.
	When this timer expires, it'll call TryToClose, which actually
	causes the thing to finally close down.
**/
	{
	LOG_FUNC
	RemoveIdleTimerEntry();
	iIdleEntryQueued = ETrue;
	BTSocketTimer::Queue(KL2ProtocolIdleTimeout*KL2ProtocolSecondTimerMultiplier, iIdleTimerEntry);
	}

void CL2CAPProtocol::RemoveIdleTimerEntry()
/**
	Called whenever we're opened.
	Checks there are no idle timer entries queued.
**/
	{
	LOG_FUNC
	if (!iIdleEntryQueued)
		return;
	BTSocketTimer::Remove(iIdleTimerEntry);
	iIdleEntryQueued = EFalse;
	}

void CL2CAPProtocol::TryToClose()
	{
	LOG_FUNC
	if (!iIdleEntryQueued && iClosePending && MuxController().CanProtocolClose())
		{
		CanClose();
		}
	}

TInt CL2CAPProtocol::TryToClose(TAny* aProtocol)
/*
	Actually try to close the protocol.
	Called after the idle timeout period by the BTSocketTimer. If
	we're all set to close down, the thing is closed.
*/
	{
	LOG_STATIC_FUNC
	CL2CAPProtocol* p=static_cast<CL2CAPProtocol*>(aProtocol);
	p->iIdleEntryQueued = EFalse;
	p->TryToClose();
	return EFalse;
	}

CL2CAPSAPStateFactory& CL2CAPProtocol::StateFactory() const
	{
	LOG_FUNC
	return *iStateFactory;
	}

CL2CAPSignalStateFactory& CL2CAPProtocol::SigStateFactory() const
	{
	LOG_FUNC
	return *iSigStateFactory;
	}

TInt CL2CAPProtocol::BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSAP)
	{
	LOG_FUNC
	return iMuxController->BearerConnectComplete(aAddr, aSAP);
	}

const TBTDevAddr& CL2CAPProtocol::LocalBTAddr() const
	{
	LOG_FUNC
	return static_cast<CLinkMgrProtocol*>(iLowerProtocol)->LocalBTAddress();
	}

TInt CL2CAPProtocol::StartProtocolListening()
// forward L2CAPs listening requirements to listener
	{
	LOG_FUNC
	return CBluetoothProtocolBase::StartListening(EACLPortL2CAP,
												  KSockBluetoothTypeACL,
												  KL2CAPIncomingConnQueueSize,
												  KUidServiceL2CAP);
	}


