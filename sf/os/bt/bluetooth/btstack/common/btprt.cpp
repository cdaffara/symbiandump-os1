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
// This file implements the generic interface to a ESOCK PRT
// for the Bluetooth PRT module
// 
//

// The Bluetooth protocol family also provides Control-plane facilities
// thereby relieving the user-plane SAPs and muxers of these duties
#include <bluetooth/logger.h>
#include "Avctp.h"
#include "bt.h"
#include "linkmgr.h"
#include "l2cap.h"
#include "rfcomm.h"
#include "sdp.h"
#include "avdtp.h"

#include "IncomingConnListener.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_PRT);
#endif

#ifdef _DEBUG
PANICCATEGORY("btprt");
#endif

_LIT(KBTPrtPanic, "BluetoothPRT Panic");


void Panic(TBluetoothFamilyPanic aPanic)
	{
	LOG_STATIC_FUNC
	User::Panic(KBTPrtPanic, aPanic);
	}

extern "C"
	{
	IMPORT_C CProtocolFamilyBase* InstallBTProtocolFamily();	// Force export 
	}

EXPORT_C CProtocolFamilyBase* InstallBTProtocolFamily()
// Create a new protocol family
	{
	LOG_STATIC_FUNC
	//Return NULL if CBTProtocolFamily::NewL() leaves
	CProtocolFamilyBase* result=NULL;
	TRAP_IGNORE(result=CBTProtocolFamily::NewL());
	return result;
	}

CBTProtocolFamily* CBTProtocolFamily::NewL()
	{
#ifdef __FLOG_ACTIVE
	CONNECT_LOGGER
#endif
	LOG_STATIC_FUNC
    // Flogging can start after the installation of the Blogger
	// and the instantiation of the HCI
	//	LOG(_L("+CBTProtocolFamily::NewL"));
	CBTProtocolFamily* pf= new (ELeave) CBTProtocolFamily();
	CleanupStack::PushL(pf);
	pf->ConstructL();
	CleanupStack::Pop();
	return pf;
	}

CBTProtocolFamily::CBTProtocolFamily() 
 : CProtocolFamilyBase(),
   iBTRefCount(0)
	{
	LOG_FUNC
	}

void CBTProtocolFamily::ConstructL()
	{
	LOG_FUNC
	// install TLS
	Dll::SetTls(&iBTTls);

	LOG(_L("+CBTProtocolFamily::ConstructL"));
	iCallback = new (ELeave) CAsyncCallBack(EPriorityHigh);
	}

CBTProtocolFamily::~CBTProtocolFamily()
	{
	LOG_FUNC
	LOG(_L("+CBTProtocolFamily::~CBTProtocolFamily"));
	BTSocketTimer::Close();
	delete iSecurityMgr;
	delete iCallback;
	delete iCodServiceMan;
	iControlPlane.Close();
	Dll::FreeTls();
#ifdef __FLOG_ACTIVE
	CLOSE_LOGGER
#endif
	}
	

TInt CBTProtocolFamily::Install()
	{
	LOG_FUNC

	TRAPD(err, BTSocketTimer::InitL());
	return err;
	}

TInt CBTProtocolFamily::Remove()
	{
	LOG_FUNC
	// set the asynccallback to delete us cleanly
	return KErrNone;
	}

TUint CBTProtocolFamily::ProtocolList(TServerProtocolDesc*& aProtocolPointer)
	{
	LOG_FUNC
	// This is cleaned up by ESOCK
	aProtocolPointer = new TServerProtocolDesc[6];

	if (!aProtocolPointer)
		return 0;
	
	// Note it helps if the protocols are defined here in the same order 
	// as in the .esk file or Esock's tags will get mis-assigned.

	CLinkMgrProtocol::ProtocolIdentity(&aProtocolPointer[0]);
	CL2CAPProtocol::ProtocolIdentity  (&aProtocolPointer[1]);
#ifdef TCI
	return 2;
#else // !TCI
	CRfcommProtocol::ProtocolIdentity (&aProtocolPointer[2]);
	CSdpProtocol::ProtocolIdentity    (&aProtocolPointer[3]);
#ifndef BLUETOOTH_NO_AV
 	CAvctpProtocol::ProtocolIdentity  (&aProtocolPointer[4]); 	
	CAvdtpProtocol::ProtocolIdentity  (&aProtocolPointer[5]);
	return 6;
#else
	#pragma message("Note: Bluetooth stack will not contain AVCTP or AVDTP");
	return 4;
#endif // BLUETOOTH_NO_AV
#endif // TCI
	};

CProtocolBase* CBTProtocolFamily::NewProtocolL(TUint /*aSockType*/,TUint aProtocol)
/**
	Called by the socket server to create an instance of a new 
	Protocol (layer) of the specified name - must be a subclass of 
	CProtocolBase.
**/
	{
	LOG_FUNC

	// Create the Security mgr
	if (!iSecurityMgr)
		{
		iSecurityMgr = CBTSecMan::NewL();
		}

	// We create our one instance of our protocols
	CBluetoothProtocolBase* p = NULL;

	// Create the Cod Service manager with reference to linkmgr
	if (!iCodServiceMan)
		{
		iCodServiceMan = CBTCodServiceMan::NewL(/*reinterpret_cast<CLinkMgrProtocol&>(p)*/);
		}
		

	switch (aProtocol)
		{
	case KBTLinkManager:
		LOG(_L("CBTProtocolFamily::NewProtocolL - BTLinkMgr"));
		p = CLinkMgrProtocol::NewL(*iSecurityMgr, iControlPlane, *iCodServiceMan);
		
		iCodServiceMan->iLinkMgr = reinterpret_cast<CLinkMgrProtocol*>(p);
		
		break;

#ifdef TCI
	case KTCIL2CAP:
#endif
	case KL2CAP:
		LOG(_L("CBTProtocolFamily::NewProtocolL - L2CAP"));
		p = CL2CAPProtocol::NewL(*iSecurityMgr, iControlPlane, *iCodServiceMan);
		break;

#ifndef TCI
	case KRFCOMM:
		LOG(_L("CBTProtocolFamily::NewProtocolL - RFCOMM"));
		p = CRfcommProtocol::NewL(*iSecurityMgr, iControlPlane, *iCodServiceMan );
		break;

	case KSDP:
		LOG(_L("CBTProtocolFamily::NewProtocolL - SDP"));
		p = CSdpProtocol::NewL(*iSecurityMgr, iControlPlane, *iCodServiceMan);
		break;
	case KAVCTP:
#ifndef BLUETOOTH_NO_AV
		LOG(_L("CBTProtocolFamily::NewProtocolL - AVCTP"));
		p = CAvctpProtocol::NewL(*iSecurityMgr, iControlPlane, *iCodServiceMan);
#else
		LOG(_L("CBTProtocolFamily::NewProtocolL - AVCTP: NOT SUPPORTED, Leaving"));
		User::Leave(KErrNotSupported);
#endif //BLUETOOTH_NO_AV
		break;

	case KAVDTP:
#ifndef BLUETOOTH_NO_AV
		LOG(_L("CBTProtocolFamily::NewProtocolL - AVDTP"));
		p = CAvdtpProtocol::NewL(*iSecurityMgr, iControlPlane, *iCodServiceMan);
#else
		LOG(_L("CBTProtocolFamily::NewProtocolL - AVDTP: NOT SUPPORTED, Leaving"));
		User::Leave(KErrNotSupported);
#endif //BLUETOOTH_NO_AV
		break;

#endif //TCI

	default:
		User::Leave(KErrArgument);
		}
	CleanupStack::PushL(p);
	iControlPlane.AttachProtocolL(aProtocol, *p);
	CleanupStack::Pop(p);
	return p;
	}
	
void CBTProtocolFamily::Open()
	{
	LOG_FUNC
	// we don't use the reference-counting of the base-class. We prefer to async close
	// so we override Open amd implement our own ref counting.
	iBTRefCount++;
	}

void CBTProtocolFamily::Close()
	{
	LOG_FUNC
	if(--iBTRefCount <= 0)
		{
		iBTRefCount = 0;
		iCallback->Cancel();
		// defer calling d'tor
		TCallBack cb(Destruct, this);
		iCallback->Set(cb);
		iCallback->CallBack();
		}
	}

/*static*/ TInt CBTProtocolFamily::Destruct(TAny* aProtocolFamily)
	{
	LOG_STATIC_FUNC
	// self-destruct
	CProtocolFamilyBase* p = reinterpret_cast<CProtocolFamilyBase*>(aProtocolFamily);
	delete p;

	return EFalse;
	}

CBluetoothProtocolBase::CBluetoothProtocolBase(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan)
: iSecMan(aSecMan), iControlPlane(aControlPlane), iCodMan(aCodMan)
	{
	LOG_FUNC
	}

CBluetoothProtocolBase::~CBluetoothProtocolBase()
	{
	LOG_FUNC
	// detach from control plane
	ControlPlane().DetachProtocol(*this);
	delete iListener;
	}


TInt CBluetoothProtocolBase::IncrementListeners()
/**
	A protocol object (typically a SAP) has declared that it is listening

	This may mean that the protocol has to provide a bearer-listener
**/
	{
	LOG_FUNC
	TInt retVal = KErrNone;

	if (!iListeningEntities++)
		{
		// need our own listener to bring up lower layers if required for this protocol-level listening SAPs
		retVal = StartProtocolListening();
		}
	return retVal;
	}

void CBluetoothProtocolBase::DecrementListeners()
/**
	A protocol object (typically a SAP) has declared that it is not listening

	This may mean that the protocol can cleanup the bearer-listener
**/
	{
	LOG_FUNC
	--iListeningEntities;
	ASSERT_DEBUG(iListeningEntities>=0);
	if (!iListeningEntities)
		{
		// stop protocol listening
		StopProtocolListening();
		}
	}


TInt CBluetoothProtocolBase::StartListening(TUint aPort, TUint aSockType, TUint aQueSize, TUid aUid)
	{
	LOG_FUNC
	TRAPD(err, DoStartListeningL(aPort, aSockType, aQueSize, aUid));
	return err;
	}

TInt CBluetoothProtocolBase::StartProtocolListening()
	{
	LOG_FUNC
	// protocol doesnt support listening
	Panic(EBTPrtProtocolCannotListen);
	return KErrNotSupported;
	}

// default implementation for stopping a protocol listening
void CBluetoothProtocolBase::StopProtocolListening()
	{
	LOG_FUNC
	delete iListener;
	iListener = NULL;
	}


void CBluetoothProtocolBase::DoStartListeningL(TUint aPort, TUint aSockType, TUint aQueSize, TUid aUid)
	{
	LOG_FUNC
	CServProviderBase* sap =iLowerProtocol->NewSAPL(aSockType);
	iListener=CIncomingConnectionListener::NewL(*this, sap, aPort, aQueSize, aUid);
	}

TInt CBluetoothProtocolBase::ControlPlaneMessage(TBTControlPlaneMessage /*aMessage*/, TAny* /*aParam*/)
	{
	LOG_FUNC
	__DEBUGGER(); // a C-plane message was targetted for a protocol that didnt implement C-plane stuff
	return KErrNotSupported;
	}
void CBluetoothProtocolBase::Error(TInt /*aError*/,CProtocolBase* /*aSourceProtocol=NULL*/)
	{
	LOG_FUNC
	//Don't do anything, overrides CProtocolBase::Error which causes a Panic
	}
void RBTControlPlane::AttachProtocolL(TInt aProtocolNum, CBluetoothProtocolBase& aProtocol)
/**
	Called when a protocol is instantiated - allows the Control plane to
	be aware of it
**/
	{
	LOG_FUNC
	if (Protocol(aProtocolNum))
		{
		User::Leave(KErrAlreadyExists);
		}
		
	TBTProtocol protocol;
	protocol.iProtocolNum = aProtocolNum;
	protocol.iProtocol = &aProtocol;

	User::LeaveIfError(iProtocols.Append(protocol));
	}
	
void RBTControlPlane::DetachProtocol(CBluetoothProtocolBase& aProtocol)
	{
	LOG_FUNC
	for (TInt i=0; i<iProtocols.Count();i++)
		{
		if (iProtocols[i].iProtocol == &aProtocol)
			{
			iProtocols.Remove(i);
			break;
			}
		}
	}
	
void RBTControlPlane::Close()
	{
	LOG_FUNC
	iProtocols.Close();
	}


CBluetoothProtocolBase* RBTControlPlane::Protocol(TInt aProtocolNum) const
	{
	LOG_FUNC
	CBluetoothProtocolBase* p= NULL;
	for (TInt i=0; i<iProtocols.Count(); i++)
		{
		if (iProtocols[i].iProtocolNum == aProtocolNum)
			p = iProtocols[i].iProtocol;
		}
	return p;	// does not pass ownership
	}

/*
a C-plane message to send a signal to the (possible) SAP handling the physical link
to the address aAddr
*/
TInt RBTControlPlane::ModifyPhysicalLink(TBTControlPlaneMessage aMessage, const TBTDevAddr& aAddr) const
	{
	LOG_FUNC
	// link mgr must be there!
	CBluetoothProtocolBase* linkmgr = Protocol(KBTLinkManager);
	ASSERT_DEBUG(linkmgr);
	return linkmgr->ControlPlaneMessage(aMessage, const_cast<TAny*>(reinterpret_cast<const TAny*>(&aAddr)));
	}
	
void RBTControlPlane::TryToAndThenPreventHostEncryptionKeyRefresh(const TBTDevAddr& aAddr, MBluetoothControlPlaneToken*& aOutToken)
	{
	LOG_FUNC
	CBluetoothProtocolBase* linkmgr = Protocol(KBTLinkManager);
	ASSERT_DEBUG(linkmgr);
	const TBTDevAddr* addrPtr = &aAddr;
	TAny* ptr = reinterpret_cast<TAny*>(&addrPtr);
	TInt err = linkmgr->ControlPlaneMessage(ETryToAndThenPreventHostEncryptionKeyRefresh, ptr);
	if (err == KErrNone)
		{
		aOutToken = *reinterpret_cast<MBluetoothControlPlaneToken**>(ptr);
		}
	else
		{
		aOutToken = NULL;
		}
	}

/*
a C-plane message to send a message to a target protocol telling them not to authorise a device
used for multiple protocols taking part in a single use-case that requires only
one authorisation - eg AVCTP and AVDTP doing A2DP + AVRCP stereo headset
*/
TInt RBTControlPlane::Preauthorise(TInt aTargetProtocolNum,
									const TOverrideAuthorise& aSetPreauthorisation)
	{
	LOG_FUNC
	// either protocol may not be running
	CBluetoothProtocolBase* target = Protocol(aTargetProtocolNum);
	if (target)
		{
		return target->ControlPlaneMessage(EPreauthoriseDevice, const_cast<TAny*>(reinterpret_cast<const TAny*>(&aSetPreauthorisation)));
		}
	else
		{
		return KErrNotFound;
		}
	}

TPhysicalLinkPresent RBTControlPlane::PhysicalLinkPresent(const TBTDevAddr& aAddr) const
	{
	LOG_FUNC
	// need to stick down a control plane message to ascertain whether a physical channel is in place
	// linkmgr must be there
	CBluetoothProtocolBase* linkmgr = Protocol(KBTLinkManager);
	ASSERT_DEBUG(linkmgr);
	
	return static_cast<TPhysicalLinkPresent>(
		linkmgr->ControlPlaneMessage(EPresent,
		const_cast<TAny*>(reinterpret_cast<const TAny*>(&aAddr))));
	}

/*
a C-plane message to send a signal to the (possible) SAP handling the Physical Link(aAddr)
to add aObserver to its list of subscribers.
*/
TInt RBTControlPlane::SubscribePhysicalLink(MPhysicalLinkObserver& aObserver, TBTDevAddr aAddr) const
	{
	LOG_FUNC
	// link mgr must be there!
	CBluetoothProtocolBase* linkmgr = Protocol(KBTLinkManager);
	ASSERT_DEBUG(linkmgr);
	
	TPhysicalLinkSubscriptionInfo subscriptionInfo(aObserver,aAddr);
	
	return linkmgr->ControlPlaneMessage(ESubscribePhysicalLink, const_cast<TAny*>(reinterpret_cast<const TAny*>(&subscriptionInfo)));
	}
	
/*
a C-plane message to send a signal to the (possible) SAP handling the Physical Link(aAddr)
to remove aObserver its list of subscribers.
*/
TInt RBTControlPlane::UnsubscribePhysicalLink(MPhysicalLinkObserver& aObserver, TBTDevAddr aAddr) const
	{
	LOG_FUNC
	// link mgr must be there!
	CBluetoothProtocolBase* linkmgr = Protocol(KBTLinkManager);
	ASSERT_DEBUG(linkmgr);
	
	TPhysicalLinkSubscriptionInfo subscriptionInfo(aObserver,aAddr);
	
	return linkmgr->ControlPlaneMessage(EUnsubscribePhysicalLink, const_cast<TAny*>(reinterpret_cast<const TAny*>(&subscriptionInfo)));
	}
	
void MBluetoothControlPlaneToken::Release(MBluetoothControlPlaneToken*& aToken)
	{
	if(aToken)
		{
		aToken->Release();
		aToken = NULL;	
		}
	}	


