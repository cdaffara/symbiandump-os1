// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#include <bluetooth/logger.h>
#include <es_sock.h>
#include <bluetoothav.h>

#include "avctpbody.h"
#include "avctpreceiver.h"
#include "avctpsender.h"
#include "avctpremotedevices.h"
#include "avctpserviceutils.h"
#include "avctpcommon.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP_SERVICES);
#endif

using namespace SymbianAvctp;

/***********************************************************************************/
// Construction / Destruction Methods

/**
Two phase constructor of CAvctpBody. 
@param aNotify the client to notify of AVCTP events
@param aPid the profile id that the client wishes to use
@internalComponent
*/
CAvctpBody* CAvctpBody::NewL(MAvctpEventNotify& aNotify, TPid aPid, MAvctpChannel*& aPrimaryChannel)
	{
	LOG_STATIC_FUNC
	CAvctpBody* self = CAvctpBody::NewLC(aNotify, aPid, aPrimaryChannel);
	CleanupStack::Pop(self);
	return self;	
	}

/**
Two phase constructor of CAvctpBody which leaves newly created object on cleanupstack
@param aNotify the client to notify of AVCTP events
@param aPid the profile id that the client wishes to use
@internalComponent
*/
CAvctpBody* CAvctpBody::NewLC(MAvctpEventNotify& aNotify, TPid aPid, MAvctpChannel*& aPrimaryChannel)
	{
	LOG_STATIC_FUNC
	CAvctpBody* self = new(ELeave) CAvctpBody(aNotify, aPid);
	CleanupStack::PushL(self);
	self->ConstructL(aPrimaryChannel);
	return self;
	}


void CAvctpBody::ConstructL(MAvctpChannel*& aPrimaryChannel)
	{
	CONNECT_LOGGER
	LOG_FUNC // must be after tls is created
	
	aPrimaryChannel = NULL;
	
	// Create proxy objects
	iPrimaryChannelProxy = new(ELeave) TPrimaryChannelProxy(*this);
	iSecondaryChannelProxy = new(ELeave) TSecondaryChannelProxy(*this);

	// Start establishing AVCTP services
	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iSocketServ.ShareAuto());

	User::LeaveIfError(iAvctpPrimaryDataSocket.Open(iSocketServ, KBTAddrFamily, KSockDatagram, KAVCTP));

	// bind that socket so that the protocol knows what channel it represents
	TAvctpSockAddr addr;
	addr.SetPid(iPid);
	addr.SetChannel(KAvctpPrimaryChannel);
	User::LeaveIfError(iAvctpPrimaryDataSocket.Bind(addr));

	iPrimaryChannelSender = CAvctpSender::NewL(iNotify, iAvctpPrimaryDataSocket, iPid);
	iPrimaryChannelReceiver = CAvctpReceiver::NewL(iNotify, iAvctpPrimaryDataSocket, iPid);

	// Needs to be called after the iAvctpControlSocket.Open call
	iRemoteDevices = CAvctpRemoteDevices::NewL(iNotify, iSocketServ, iPid);
	
	// Finally we are done so assign channel interface
	aPrimaryChannel = static_cast<MAvctpChannel*>(iPrimaryChannelProxy);
	}

/**
c'tor
@internalComponent
*/
CAvctpBody::CAvctpBody(MAvctpEventNotify& aNotify, TPid aPid):
 	iNotify(aNotify), 
	iPid(aPid)
	{
	LOG_FUNC // okay to be before tls created cause it's not created by the time this function returns
	LOG2(_L("aNotify: 0x%08x, aPid: %d"), &aNotify, aPid)
	}

/**
d'tor
@internalComponent
*/
CAvctpBody::~CAvctpBody()
	{
	LOG_FUNC

	delete iPrimaryChannelSender;
	delete iPrimaryChannelReceiver;
	delete iSecondaryChannelSender;
	delete iSecondaryChannelReceiver;
	delete iRemoteDevices;

	iAvctpSecondaryDataSocket.Close();
	iAvctpPrimaryDataSocket.Close();
	iSocketServ.Close();
	
	delete iSecondaryChannelProxy;
	delete iPrimaryChannelProxy;

	CLOSE_LOGGER
	}

/***********************************************************************************/
// Methods to support RAvctp
	
void CAvctpBody::Close(RAvctp::TCloseType aImmediacy)
	{
	LOG_FUNC
	
	TRequestStatus status;
	
	if(iAvctpSecondaryDataSocket.SubSessionHandle())
		{
		iAvctpSecondaryDataSocket.Shutdown(aImmediacy == RAvctp::ENormal ? RSocket::ENormal : RSocket::EImmediate, status);
		User::WaitForRequest(status);
		}

	iAvctpPrimaryDataSocket.Shutdown(aImmediacy == RAvctp::ENormal ? RSocket::ENormal : RSocket::EImmediate, status);
	User::WaitForRequest(status);
	
	delete this;
	}
		
void CAvctpBody::CloseGracefully()
	{
	LOG_FUNC
	
	iNotify.MaenErrorNotify(TBTDevAddr(0), KErrNotSupported);
	}
	
/**
Function from the AVCTP spec to request a channel connection to a remote
entity.
@return KErrArgument if a NULL TBTDevAddr is provided as an argument or
		 another system wide error code  
@internalComponent
*/	
TInt CAvctpBody::PrimaryChannelAttachRequest(const TBTDevAddr& aBTDevice)
 	{
	LOG_FUNC

#ifdef _DEBUG
	TBuf<12> addr;
	aBTDevice.GetReadable(addr);
	LOG1(_L("to BT Device %S"), &addr);
#endif
	
	return !IsNullAddress(aBTDevice) ? iRemoteDevices->PrimaryChannelAttachRequest(aBTDevice) : KErrArgument;
 	}

TBool CAvctpBody::IsAttached(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	return iRemoteDevices->IsAttached(aBTDevice);
	}

/**
Function from the AVCTP spec to explicitly request disconnection of an existing
channel.
@param aBTDevice The remote device to disconnect
@return KErrArgument if a NULL TBTDevAddr is provided as an argument or
		 another system wide error code
@internalComponent
*/
TInt CAvctpBody::PrimaryChannelDetachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
#ifdef _DEBUG
	TBuf<12> addr;
	aBTDevice.GetReadable(addr);
	LOG1(_L("to BT Device %S"), &addr);
#endif

	return !IsNullAddress(aBTDevice) ? iRemoteDevices->PrimaryChannelDetachRequest(aBTDevice) : KErrArgument;
	}

/**
Sends AVCTP message
The Pid to send the message to was given when the client opened the RAvctp object
used to send this message.

@pre must be connected to remote
@param aBTDevice device address to which to send message
@param aTransactionLabel transaction label
@param aType the message type, i.e. Command or Response
@param aMessageInformation the contents of the message to send
@return KErrInUse if the sender AO is active or
		 KErrInvalidTransactionLabel if the transaction label was invalid or
		 KErrArgument if a NULL TBTDevAddr is provided as an argument or
		 another system wide error code
@internalComponent
*/

TInt CAvctpBody::SendMessage(const TBTDevAddr& aBTDevice, 
							 TTransactionLabel aTransactionLabel,
							 TMessageType aType,
							 const TDesC8& aMessageInformation,
							 TBool aIsSecondChannel)
	{
	LOG_FUNC

#ifdef _DEBUG
	TBuf<12> addr;
	aBTDevice.GetReadable(addr);
	LOG1(_L("to BT Device %S"), &addr);
#endif

	LOG2(_L("aTransactionLabel: %d, aType: %d"), aTransactionLabel, aType)

	TInt ret = KErrNone;
	if (IsNullAddress(aBTDevice))
		{
		ret = KErrArgument;
		}
	else if(IsValidTransactionLabel(aTransactionLabel))
		{
		__ASSERT_ALWAYS(aType == ECommand || aType == EResponse, Panic(EAvctpInvalidMessageType));
		
		CAvctpSender& sender(aIsSecondChannel ? *iSecondaryChannelSender : *iPrimaryChannelSender);
		ret = sender.SendMessage(aBTDevice, aTransactionLabel, aType, aMessageInformation);
		}
	else
		{
		ret = KErrInvalidTransactionLabel;
		}
	return ret;
	}


void CAvctpBody::PrimaryChannelCancelAttach(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC

#ifdef _DEBUG
	TBuf<12> addr;
	aBTDevice.GetReadable(addr);
	LOG1(_L("to BT Device %S"), &addr);
#endif

	__ASSERT_DEBUG(!IsNullAddress(aBTDevice), Panic(EAvctpNullBTDevAddr));
	iRemoteDevices->PrimaryChannelCancelAttach(aBTDevice);
	}

void CAvctpBody::PrimaryChannelCancelSend()
	{
	LOG_FUNC

	iPrimaryChannelSender->Cancel();
	}

void CAvctpBody::SecondaryChannelCancelSend()
	{
	LOG_FUNC

	iSecondaryChannelSender->Cancel();
	}

TInt CAvctpBody::InstallSecondaryChannel(MAvctpEventNotify& aSecondChannelObserver, MAvctpChannel*& aSecondaryChannel)
	{
	LOG_FUNC
	TRAPD(err, DoInstallSecondaryChannelL(aSecondChannelObserver, aSecondaryChannel));
	return err;
	}

void CAvctpBody::DoInstallSecondaryChannelL(MAvctpEventNotify& aSecondChannelObserver, MAvctpChannel*& aSecondaryChannel)
	{
	LOG_FUNC
	LEAVEIFERRORL(iAvctpSecondaryDataSocket.Open(iSocketServ, KBTAddrFamily, KSockDatagram, KAVCTP));

	// bind that socket so that the protocol knows what channel it represents
	TAvctpSockAddr addr;
	addr.SetChannel(KAvctpSecondaryChannel);
	addr.SetPid(iPid);
	User::LeaveIfError(iAvctpSecondaryDataSocket.Bind(addr));

	iSecondaryChannelNotify = &aSecondChannelObserver;

	__ASSERT_DEBUG(!iSecondaryChannelSender, Panic(EAvctpSenderAlreadyAllocated));
	iSecondaryChannelSender = CAvctpSender::NewL(*iSecondaryChannelNotify, iAvctpSecondaryDataSocket, iPid);
	__ASSERT_DEBUG(!iSecondaryChannelReceiver, Panic(EAvctpReceiverAlreadyAllocated));
	iSecondaryChannelReceiver = CAvctpReceiver::NewL(*iSecondaryChannelNotify, iAvctpSecondaryDataSocket, iPid);
	iRemoteDevices->SetSecondaryChannelNotifyL(&aSecondChannelObserver);
	
	// finally set the interface
	aSecondaryChannel = static_cast<MAvctpChannel*>(iSecondaryChannelProxy);
	}

void CAvctpBody::UninstallSecondaryChannel(RAvctp::TCloseType aImmediacy)
	{
	LOG_FUNC
	
	TRequestStatus status;
	iAvctpSecondaryDataSocket.Shutdown(aImmediacy == RAvctp::ENormal ? RSocket::ENormal : RSocket::EImmediate, status);
	User::WaitForRequest(status);
	
	delete iSecondaryChannelSender;
	iSecondaryChannelSender = NULL;
	delete iSecondaryChannelReceiver;
	iSecondaryChannelReceiver = NULL;
	iSecondaryChannelNotify = NULL;
	iAvctpSecondaryDataSocket.Close();
	TRAPD(err, iRemoteDevices->SetSecondaryChannelNotifyL(NULL));
	__ASSERT_DEBUG(err == KErrNone, Panic(EAvctpUnexpectedLeave));
	(void)(err == KErrNone);	// to avoid "variable not used" warning in release mode
	}

TInt CAvctpBody::SecondaryChannelAttachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC

#ifdef _DEBUG
	TBuf<12> addr;
	aBTDevice.GetReadable(addr);
	LOG1(_L("to BT Device %S"), &addr);
#endif

	return !IsNullAddress(aBTDevice) ? iRemoteDevices->SecondaryChannelAttachRequest(aBTDevice) : KErrArgument;
	}

TInt CAvctpBody::SecondaryChannelDetachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC

#ifdef _DEBUG
	TBuf<12> addr;
	aBTDevice.GetReadable(addr);
	LOG1(_L("to BT Device %S"), &addr);
#endif

	return !IsNullAddress(aBTDevice) ? iRemoteDevices->SecondaryChannelDetachRequest(aBTDevice) : KErrArgument;
	}

void CAvctpBody::SecondaryChannelCancelAttach(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
#ifdef _DEBUG
	TBuf<12> addr;
	aBTDevice.GetReadable(addr);
	LOG1(_L("to BT Device %S"), &addr);
#endif

	__ASSERT_DEBUG(!IsNullAddress(aBTDevice), Panic(EAvctpNullBTDevAddr));
	iRemoteDevices->SecondaryChannelCancelAttach(aBTDevice);
	}

//
// Channel Proxies
//

TPrimaryChannelProxy::TPrimaryChannelProxy(CAvctpBody& aBody)
	: iBody(aBody)
	{
	}
	
TInt TPrimaryChannelProxy::MacAttachRequest(const TBTDevAddr& aBTDevice)
	{
	return iBody.PrimaryChannelAttachRequest(aBTDevice);
	}

TBool TPrimaryChannelProxy::MacIsAttached(const TBTDevAddr& aBTDevice)
	{
	return iBody.IsAttached(aBTDevice);
	}

void TPrimaryChannelProxy::MacCancelAttach(const TBTDevAddr& aBTDevice)
	{
	iBody.PrimaryChannelCancelAttach(aBTDevice);
	}

TInt TPrimaryChannelProxy::MacDetachRequest(const TBTDevAddr& aBTDevice)
	{
	return iBody.PrimaryChannelDetachRequest(aBTDevice);
	}

TInt TPrimaryChannelProxy::MacSendMessage(const TBTDevAddr& aBTDevice, 
					SymbianAvctp::TTransactionLabel aTransactionLabel,
					SymbianAvctp::TMessageType aType,
					const TDesC8& aMessageInformation)
	{
	return iBody.SendMessage(aBTDevice, aTransactionLabel, aType, aMessageInformation, EFalse);
	}

void TPrimaryChannelProxy::MacCancelSend()
	{
	iBody.PrimaryChannelCancelSend();
	}


TSecondaryChannelProxy::TSecondaryChannelProxy(CAvctpBody& aBody)
	: iBody(aBody)
	{
	}
	
TInt TSecondaryChannelProxy::MacAttachRequest(const TBTDevAddr& aBTDevice)
	{
	return iBody.SecondaryChannelAttachRequest(aBTDevice);
	}

TBool TSecondaryChannelProxy::MacIsAttached(const TBTDevAddr& aBTDevice)
	{
	return iBody.IsAttached(aBTDevice);
	}

void TSecondaryChannelProxy::MacCancelAttach(const TBTDevAddr& aBTDevice)
	{
	iBody.SecondaryChannelCancelAttach(aBTDevice);
	}

TInt TSecondaryChannelProxy::MacDetachRequest(const TBTDevAddr& aBTDevice)
	{
	return iBody.SecondaryChannelDetachRequest(aBTDevice);
	}

TInt TSecondaryChannelProxy::MacSendMessage(const TBTDevAddr& aBTDevice, 
					SymbianAvctp::TTransactionLabel aTransactionLabel,
					SymbianAvctp::TMessageType aType,
					const TDesC8& aMessageInformation)
	{
	return iBody.SendMessage(aBTDevice, aTransactionLabel, aType, aMessageInformation, ETrue);
	}

void TSecondaryChannelProxy::MacCancelSend()
	{
	iBody.SecondaryChannelCancelSend();
	}


