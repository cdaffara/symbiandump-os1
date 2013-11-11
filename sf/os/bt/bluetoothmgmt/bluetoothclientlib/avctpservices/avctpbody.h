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

#ifndef AVCTPBODY_H
#define AVCTPBODY_H

#include <es_sock.h>
#include <avctpservices.h>

#define __ASSERT_BTADDR(d)  __ASSERT_DEBUG((d) != TBTDevAddr(0), Panic(EAvctpNullBTDevAddr));
								  
// Forward declarations
class CAvctpSender;
class CAvctpReceiver;
class CAvctpRemoteDevices;
class CAvctpCloser;


NONSHARABLE_CLASS(TPrimaryChannelProxy)
	: public MAvctpChannel
	{
public:
	TPrimaryChannelProxy(CAvctpBody& aBody);
	
private: // from MAvctpChannel
	TInt MacAttachRequest(const TBTDevAddr& aBTDevice);
	TBool MacIsAttached(const TBTDevAddr& aBTDevice);
	void MacCancelAttach(const TBTDevAddr& aBTDevice);
	TInt MacDetachRequest(const TBTDevAddr& aBTDevice);
	TInt MacSendMessage(const TBTDevAddr& aBTDevice, 
						SymbianAvctp::TTransactionLabel aTransactionLabel,
						SymbianAvctp::TMessageType aType,
						const TDesC8& aMessageInformation);
	void MacCancelSend();
	
private:
	CAvctpBody& iBody;
	};

NONSHARABLE_CLASS(TSecondaryChannelProxy)
	: public MAvctpChannel
	{
public:
	TSecondaryChannelProxy(CAvctpBody& aBody);
	
private: // from MAvctpChannel
	TInt MacAttachRequest(const TBTDevAddr& aBTDevice);
	TBool MacIsAttached(const TBTDevAddr& aBTDevice);
	void MacCancelAttach(const TBTDevAddr& aBTDevice);
	TInt MacDetachRequest(const TBTDevAddr& aBTDevice);
	TInt MacSendMessage(const TBTDevAddr& aBTDevice, 
						SymbianAvctp::TTransactionLabel aTransactionLabel,
						SymbianAvctp::TMessageType aType,
						const TDesC8& aMessageInformation);
	void MacCancelSend();
	
private:
	CAvctpBody& iBody;
	};


/**
This class is the handle of the RAvctp / CAvctpBody pair.
It implements the AVCTP interface presented by RAvctp.
It owns a couple of Sender/Receiver for the primary channel and a couple for the secondary channel.
CAvctpSender and CAvctpReceiver are active objects.
@internalComponent
*/
NONSHARABLE_CLASS(CAvctpBody) : public CBase
	{
public:
	static CAvctpBody* NewL(MAvctpEventNotify& aNotify, SymbianAvctp::TPid aPid, MAvctpChannel*& aPrimaryChannel);
	static CAvctpBody* NewLC(MAvctpEventNotify& aNotify, SymbianAvctp::TPid aPid, MAvctpChannel*& aPrimaryChannel);
	~CAvctpBody();

	// Methods to support RAvctp
	void Close(RAvctp::TCloseType aImmediacy);
	void CloseGracefully();
	TInt  PrimaryChannelAttachRequest(const TBTDevAddr& aBTDevice);
	TBool IsAttached(const TBTDevAddr& aBTDevice);
	TInt PrimaryChannelDetachRequest(const TBTDevAddr& aBTDevice);
	TInt  SendMessage(const TBTDevAddr& aBTDevice, 
					 SymbianAvctp::TTransactionLabel aTransactionLabel,
					 SymbianAvctp::TMessageType aType,
					 const TDesC8& aMessageInformation,
					 TBool aIsSecondChannel);
	void PrimaryChannelCancelAttach(const TBTDevAddr& aBTDevice);
	void PrimaryChannelCancelSend();
	
	TInt InstallSecondaryChannel(MAvctpEventNotify& aSecondChannelObserver, MAvctpChannel*& aSecondaryChannel);
	TInt SecondaryChannelAttachRequest(const TBTDevAddr& aBTDevice);
	TInt SecondaryChannelDetachRequest(const TBTDevAddr& aBTDevice);
	void SecondaryChannelCancelAttach(const TBTDevAddr& aBTDevice);
	void SecondaryChannelCancelSend();
	void UninstallSecondaryChannel(RAvctp::TCloseType aImmediacy);
	
private:
	CAvctpBody(MAvctpEventNotify& aNotify, SymbianAvctp::TPid aPid);
	void ConstructL(MAvctpChannel*& aPrimaryChannel);
	void DoInstallSecondaryChannelL(MAvctpEventNotify& aSecondChannelObserver, MAvctpChannel*& aSecondaryChannel);
	
private:

	MAvctpEventNotify& iNotify;
	SymbianAvctp::TPid iPid;

	RSocketServ iSocketServ;
	RSocket iAvctpPrimaryDataSocket;
	RSocket iAvctpSecondaryDataSocket;
	
	// Proxy classes for handling requests for different channels.
	TPrimaryChannelProxy* iPrimaryChannelProxy;
	TSecondaryChannelProxy* iSecondaryChannelProxy;

	// Active objects to deal with the primary channel data socket
	CAvctpSender* iPrimaryChannelSender;
	CAvctpReceiver* iPrimaryChannelReceiver;

	// Active objects to deal with the secondary channel data socket
	CAvctpSender* iSecondaryChannelSender;
	CAvctpReceiver* iSecondaryChannelReceiver;
	MAvctpEventNotify* iSecondaryChannelNotify; //non-owned
	
	// Active Object to manage the TransportControl socket.
	CAvctpRemoteDevices* iRemoteDevices;
	};
	
#endif // AVCTPBODY_H
