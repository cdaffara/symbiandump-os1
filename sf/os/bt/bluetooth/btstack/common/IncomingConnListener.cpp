// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the incoming connection listener that protocols may use
// to receive inbound connections from lower protocols
// Also acts as a conduit for Ioctls, as there may not always be a route
// present through the stack for them.
// In future though these Ioctls should aim to convey control messages
// along a dedicated Control-plane rather than using any of the User-plane
// SAPs
// 
//

#include <bluetooth/logger.h>
#include <es_prot.h>
#include <bt_sock.h>
#include "IncomingConnListener.h"
#include "BtSap.h"
#include "bt.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_IN_CONN_LISTENER);
#endif

//	Construction/destruction

CIncomingConnectionListener* CIncomingConnectionListener::NewL(CBluetoothProtocolBase& aProtocol, CServProviderBase* aNewSAP, const TBTSockAddr& aCompleteSockAddr, TUint aQueSize)
	{
	LOG_STATIC_FUNC
	CleanupStack::PushL(aNewSAP);	//	i.e. take immediate ownership
	CIncomingConnectionListener* self=new(ELeave) CIncomingConnectionListener(aProtocol, aNewSAP);
	CleanupStack::Pop(aNewSAP);		//	listener now has ownership, safe to remove from Cleanup
	CleanupStack::PushL(self);
	self->ConstructL(aCompleteSockAddr, aQueSize);
	CleanupStack::Pop(self);
	return self;
	}

CIncomingConnectionListener*
	CIncomingConnectionListener::NewL(CBluetoothProtocolBase& aProtocol, CServProviderBase* aNewSAP,
										TUint aPort, TUint aQueSize, TUid aUid)
	{
	LOG_STATIC_FUNC	
	// careful about sap - just in case anything were to leave take ownership
	CleanupStack::PushL(aNewSAP);	//	i.e. take immediate ownership	
	//set security
	TBTSockAddr localAddr;
	localAddr.SetPort(aPort);

	TBTServiceSecurity incomingSecurity;
	incomingSecurity.SetUid(aUid);

	// as a bearer listener we don't set security; that's left to the listening SAPs themselves
	incomingSecurity.SetAuthentication(EMitmNotRequired);
	incomingSecurity.SetAuthorisation(EFalse);
	incomingSecurity.SetEncryption(EFalse);
	incomingSecurity.SetDenied(EFalse);

	localAddr.SetSecurity(incomingSecurity);
	CleanupStack::Pop(aNewSAP);		//	calling other overload that equally takes ownership precautions
	
	return CIncomingConnectionListener::NewL(aProtocol, aNewSAP, localAddr, aQueSize);	
	}
	
CIncomingConnectionListener::CIncomingConnectionListener(CBluetoothProtocolBase& aProtocol, CServProviderBase* aNewSAP)
:	iProtocol(aProtocol),
	iListeningSAP(aNewSAP)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aNewSAP!=NULL, Panic(EListenerNullSAPForListening));
	iListeningSAP->SetNotify(this);
	}

// this overload allows non-default security to be set
void CIncomingConnectionListener::ConstructL(const TBTSockAddr& aCompletedSockAddr, TUint aQueSize)
	{
	LOG_FUNC
	User::LeaveIfError(iListeningSAP->SetLocalName(const_cast<TBTSockAddr&>(aCompletedSockAddr)));
	User::LeaveIfError(iListeningSAP->PassiveOpen(aQueSize));
	}

CIncomingConnectionListener::~CIncomingConnectionListener()
	{
	LOG_FUNC
	//it's polite to shutdown a sap...
	iListeningSAP->Shutdown(CServProviderBase::EImmediate);
	delete iListeningSAP;
	}

void CIncomingConnectionListener::SetPreauthorisation(TUint aProtocolLevel, const TBTDevAddr& aPreauthoriseAddress, TBool aSetPreauthorisation)
	{
	LOG_FUNC
	if (!iListeningSAP)
		{
		// somebody must be listening for this to work
		return;
		}
	
	// only allow one preauthorised device at moment, so don't keep record
	// of the overrides set into L2CAP - just punt this address down
	TBTDeviceSecurity devSecurity;
	devSecurity.SetNoAuthorise(aSetPreauthorisation);

	TBTServiceSecurityPerDevice	overriddenSecurity(aPreauthoriseAddress, devSecurity);
	const TPckgC<TBTServiceSecurityPerDevice> override(overriddenSecurity);
	
	// *could* assume the level to setopt as lower protocol
	// but reduces genericity for caller
	// who can preauthorise down at an arbitrary level with the api
	(void)iListeningSAP->SetOption(aProtocolLevel, KBTSecurityDeviceOverride, override);
	}

/**
Getter to see if the remote address is preauthorised
*/
TBool CIncomingConnectionListener::IsPreauthorised(TUint aProtocolLevel, const TBTDevAddr& aRemoteAddress)
	{
	LOG_FUNC
	TBTServiceSecurityPerDevice	overriddenSecurity;
	overriddenSecurity.SetAddress(aRemoteAddress);
	
	TPckg<TBTServiceSecurityPerDevice> override(overriddenSecurity);
	
	TBool res = EFalse;
	
	if (iListeningSAP)
		{
		if (iListeningSAP->GetOption(aProtocolLevel, KBTSecurityDeviceOverride, override) == KErrNone)
			{
			res = overriddenSecurity.DeviceSecurity().NoAuthorise();
			}
		}
	return res;
	}
	
TInt CIncomingConnectionListener::SetOption(const TUint aLevel, TUint aName, TDesC8& aOption)
	{
	LOG_FUNC
	if(!iListeningSAP)
		{
		return KErrNotReady;
		}
		
	return iListeningSAP->SetOption(aLevel, aName, aOption);
	}
	
void CIncomingConnectionListener::Ioctl(const CBluetoothSAP& aSAP, TUint aLevel, TUint aName, TDes8* aOption)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iRequestingSAP, Panic(EListenerSAPTwoIoctls));

	iRequestingSAP = const_cast<CBluetoothSAP*>(&aSAP);
	iIoctlLevel = aLevel;
	iIoctlName = aName;

	iListeningSAP->Ioctl(iIoctlLevel, iIoctlName, aOption);
	}

//	From MSocketNotify
void CIncomingConnectionListener::NewData(TUint /*aCount*/)
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent);
	}

void CIncomingConnectionListener::CanSend()
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent);
	}

void CIncomingConnectionListener::ConnectComplete()
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent); // should have been given a SAP
	}

void CIncomingConnectionListener::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent);
	}

void CIncomingConnectionListener::ConnectComplete(CServProviderBase& aSSP)
/**
	Called by the lower SAP when an incoming connection occurs.

	This checks with the protocol to check for an existing muxer/signaller - if there
	is one, an error is returned and the new lower SAP is shut down (the 
	remote device should be using the existing muxer). If not, the SAP is
	given to the muxer.
**/
	{
	LOG_FUNC
	TBTSockAddr remoteAddr;
	aSSP.RemName(remoteAddr);
	const TBTDevAddr& devAddr = remoteAddr.BTAddr();

	//	NB If no error occurs, ownership of aSSP passes on, otherwise we close SAP
	TInt error=iProtocol.BearerConnectComplete(devAddr, &aSSP);

	if(error != KErrNone)
		{
		//	Protocol was not happy with connection
		//	We need to disallow the connection, and get rid of it.
		LOG(_L("IncomingConnectionListener: ConnectComplete called for existing muxer/signaller - stopping bound SAP"));
		aSSP.Shutdown(CServProviderBase::EImmediate);
		delete(&aSSP);
		}
	}

void CIncomingConnectionListener::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent);
	}

void CIncomingConnectionListener::CanClose(TDelete /*aDelete*/)
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent);
	}

void CIncomingConnectionListener::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent);
	}

#ifdef _DEBUG
void CIncomingConnectionListener::Error(TInt aError,TUint aOperationMask)
#else
void CIncomingConnectionListener::Error(TInt aError,TUint /*aOperationMask*/)
#endif
	{
	LOG_FUNC
	// clear up our stuff
	if (iRequestingSAP)
		{	
		__ASSERT_DEBUG(aOperationMask & MSocketNotify::EErrorIoctl, Panic(EListenerUnexpectedEvent));

		iRequestingSAP->IoctlComplete(aError, iIoctlLevel, iIoctlName, NULL);
		ClearIoctl();
		}

	// now tell our protocol
	iProtocol.Error(aError);
	}
	
void CIncomingConnectionListener::Disconnect(void)
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent);
	}

void CIncomingConnectionListener::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	LOG_FUNC
	Panic(EListenerUnexpectedEvent);
	}

void CIncomingConnectionListener::IoctlComplete(TDesC8* aBuf)
	{
	LOG_FUNC
	if (!iRequestingSAP)
		return;		// the SAP may have gone home
	iRequestingSAP->IoctlComplete(KErrNone, iIoctlLevel, iIoctlName, aBuf);
	ClearIoctl();
	}

#ifdef _DEBUG
void CIncomingConnectionListener::CancelIoctl(const CBluetoothSAP& aSAP)
#else
void CIncomingConnectionListener::CancelIoctl(const CBluetoothSAP& /*aSAP*/)
#endif
	{
	LOG_FUNC
	if (!iRequestingSAP)
		return;		// the SAP may have gone home
	__ASSERT_DEBUG(iRequestingSAP == &aSAP, Panic(EListenerInvalidSAPIoctlCancel));
	ClearIoctl();
	}

void CIncomingConnectionListener::ClearIoctl()
	{
	LOG_FUNC
	iRequestingSAP=NULL;
	iIoctlName=0;
	iIoctlLevel=0;
	}


void Panic(TListenerPanic aPanic)
	{
	LOG_STATIC_FUNC
	User::Panic(_L("BT Incoming Listener Panic"),aPanic);
	}
