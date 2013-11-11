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
// Defines the avdtp transport channel base class
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPTRANSPORTCHANNEL_H
#define AVDTPTRANSPORTCHANNEL_H

#include <bt_sock.h>
#include "avdtpAllocators.h"
#include "avdtpLogicalChannelFactory.h"
#include "avdtpTransportSession.h"

#ifdef _DEBUG
_LIT(KAVDTPTransportChannel, "AVDTP_TransportChannel");
#define DEBUG_UNEXPECTED_L2CAP_EVENT User::Panic(KAVDTPTransportChannel, 0);
#else
#define DEBUG_UNEXPECTED_L2CAP_EVENT
#endif

class CAvdtpProtocol;
class CTransportSession;
class TAvdtpSockAddr;

typedef TUint32 TAvdtpAdaptationServices;
const TUint32 KFragmentationService = 0x01;

/**
Base class for Direct and Mux channels	
Derivers must implement certain send-related methods
*/
NONSHARABLE_CLASS(CTransportChannel) : public CBase, public MSocketNotify
	{
friend class CAvdtpProtocol;
public:
	static CTransportChannel* NewDirectChannelL(CAvdtpProtocol& aParent,
								   const TBTDevAddr& aAddr);
	static CTransportChannel* NewMuxChannelL(CAvdtpProtocol& aProtocol,
							  const TBTDevAddr& aRemoteDevice,
							  TTCID aRemotelyAssignedTCID/*=KInvalidTCID*/);
	virtual ~CTransportChannel();

// management path
	virtual void DetachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType aType)=0;
	virtual TInt AttachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType aType, TL2CapConfig::TChannelPriority aPriority)=0;
	virtual TBool CouldAttachSession(const TAvdtpSockAddr& aAddr)=0;
	virtual TTCID TCID() const=0;
	inline const TBTDevAddr& RemoteAddress() const;
	virtual CServProviderBase* ObtainSAP()=0;

// send path
	virtual TUint SendPacket(TTSID aTSID, RMBufChain& aPacket)=0;
	virtual TInt MaxPacketSize(TInt& aMTU); // default is just whatever l2cap is
// receive path
	virtual void TransportSessionBlocked(TAvdtpTransportSessionType aSession, TBool aBlocked)=0;
	
// interface for stream
	virtual void LogicalChannelComplete(TLogicalChannelRecord aLogicalChannelRecord, TInt aErr);
	void AllowServices(TAvdtpAdaptationServices aServices);
	inline TBool IsConnected() const;
	
	// MSocketNotify stuff that is never used
	// (still virtual though - so implementors *can* implement!)
	void Bearer(const TDesC8&) {DEBUG_UNEXPECTED_L2CAP_EVENT};
	void NoBearer(const TDesC8&) {DEBUG_UNEXPECTED_L2CAP_EVENT};
	void Disconnect(TDesC8&) {DEBUG_UNEXPECTED_L2CAP_EVENT};
	void CanClose(const TDesC8&, TDelete) {DEBUG_UNEXPECTED_L2CAP_EVENT};
	void ConnectComplete() {DEBUG_UNEXPECTED_L2CAP_EVENT};
	void ConnectComplete(const TDesC8&) {DEBUG_UNEXPECTED_L2CAP_EVENT};
    void ConnectComplete(CServProviderBase&) {DEBUG_UNEXPECTED_L2CAP_EVENT};
	void ConnectComplete(CServProviderBase&,const TDesC8&) {DEBUG_UNEXPECTED_L2CAP_EVENT};
	TInt SetOption(TUint aLevel, TUint aName, const TDesC8 &aOption);
	// MSocketNotify stuff that the base class can sort out
	// no ioctl-forwarding supported yet
	// if implemented this base class can handle the ioctlcomplete
	void IoctlComplete(TDesC8*) {DEBUG_UNEXPECTED_L2CAP_EVENT};
	void CanClose(TDelete aDelete=EDelete);

	virtual TInt MaxReceivePacketSize(TInt& aMTU); // default is just whatever l2cap is		
	
protected:
	CTransportChannel(CAvdtpProtocol& aParent, const TBTDevAddr& aAddr);
	void ConstructL();
	void CloseLogicalChannel();
	inline CAvdtpProtocol& Protocol() const;

private:
	virtual void UpdateChannelPriority() = 0;

protected:
	RTCID				iTCID;
	CServProviderBase*	iLogicalChannel;
	TDblQueLink			iProtocolQLink;
	TBool				iLogicalChannelBlocked;
	TAvdtpAdaptationServices iAllowedServices;
private:
	CAvdtpProtocol&		iProtocol;
	TBTDevAddr			iRemoteDevice;
	};
	
inline CAvdtpProtocol& CTransportChannel::Protocol() const
	{
	return iProtocol;
	}
	
inline const TBTDevAddr& CTransportChannel::RemoteAddress() const
	{
	return iRemoteDevice;
	}
	
inline TBool CTransportChannel::IsConnected() const
	{
	return (iLogicalChannel!=NULL);
	}

#endif //AVDTPTRANSPORTCHANNEL_H
