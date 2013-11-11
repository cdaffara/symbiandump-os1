// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the avdtp "direct" (!muxed) transport channel object, by derivation
// from CTransportChannel
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPDIRECTCHANNEL_H
#define AVDTPDIRECTCHANNEL_H

#include "avdtpTransportChannel.h"

class CUserPlaneTransportSession;

NONSHARABLE_CLASS(CDirectChannel) : public CTransportChannel
	{
public:
	static CDirectChannel* NewL(CAvdtpProtocol& aProtocol,
								const TBTDevAddr& aRemoteDevice);
	//from base
// can just forward down to L2CAP...

	virtual void DetachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType aType);
	virtual TInt AttachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType aType, TL2CapConfig::TChannelPriority aPriority = TL2CapConfig::ELow);
	virtual TBool CouldAttachSession(const TAvdtpSockAddr& aAddr);
	virtual TTCID TCID() const;
	CServProviderBase* ObtainSAP();

	virtual TUint SendPacket(TTSID aTSID, RMBufChain& aPacket);

	virtual void TransportSessionBlocked(TAvdtpTransportSessionType aSession, TBool aBlocked);
	
private:
	CDirectChannel(CAvdtpProtocol& aProtocol, const TBTDevAddr& aRemoteDevice);
	void UpdateChannelPriority();
	
private:
// interesting stuff from l2cap
	virtual void NewData(TUint aCount);
	virtual void CanSend();
	virtual void Error(TInt aError,TUint aOperationMask=EErrorAllOperations);
	virtual void Disconnect();

private:
	CUserPlaneTransportSession*	iTransportSession;			// non-owned
	TBool						iTransportSessionBlocked;
	TL2CapConfig::TChannelPriority iChannelPriority;
	};
	
#endif //AVDTPDIRECTCHANNEL_H
