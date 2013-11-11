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
// Define the AVCTP Service Access Point (SAP) class
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef AVCTPSAP_H
#define AVCTPSAP_H

#include <es_prot.h>
#include <bt_sock.h>
#include "avctpOutboundQ.h"
#include "avctpconstants.h"
#include "avctppacket.h"
#include "BtSap.h"
#include "secman.h"
#include "avctpcommon.h"
/**
 This class is used to queue an indication (in some circustances) so to serialize more indications
 to be served. It is also used in the particular case of an oustanding ioctl in order to be able
 to complete it with the right indication.
 The logic is that when an Ioctl with EPrimaryChannelAttachToTransport or ESecondaryChannelAttachToTransport name has been submitted it
 means that a connection request to a remote device has been submitted. So we queue an "indication" with
 the remote device address. If an Ioctl completed arrives we want to discriminate if it is the request's 
 reply or an incoming connection from another device. If it isn't our request completion it will be queued
 So, we will serve first the request's completion and then the incoming connections. 
 **/
NONSHARABLE_CLASS(HQueuedIndication)
	{
public:
	static HQueuedIndication* New(const TControlIoctlMessage& aMessage);
	inline TControlIoctlMessage& Indication() const;
	~HQueuedIndication();
public:
	TSglQueLink	iLink;
private:
    void ConstructL(const TControlIoctlMessage& aMessage);
private:
	TControlIoctlMessage* iMessage;
	};

inline TControlIoctlMessage& HQueuedIndication::Indication() const
    {
    return *iMessage;
    }

// Forward declarations
class CAvctpProtocol;
class CAvctpTransport;

/**
The SAP for an AVCTP socket.

Note that this class doesn't currently support any ioctls or options,
not even to forward them on to a lower protocol

  @internalAll
  @see CBluetoothSAP
*/
NONSHARABLE_CLASS(CAvctpSap) : public CBluetoothSAP, public MSocketNotify
	{
friend class CAvctpProtocol; // needed for iQueLink access
friend class CAvctpTransport;

public:
	// Creation
	static CAvctpSap* NewL(CAvctpProtocol& aProtocol);
	~CAvctpSap();
	
	// Interface between the muxers and the saps via protocol
	TBool HasDataFor(const TBTDevAddr& aRemoteAddr);
	TBool NewData(HAvctpIncomingSdu* aSdu, TInt aChannel);
	
	inline SymbianAvctp::TPid Pid() const;
	inline TInt Channel() const;
	inline TBool IsInList() const;
private:
	CAvctpSap(CAvctpProtocol& aProtocol);
	void ConstructL();

	// virtuals from CServProviderBase
	// Made private so that only people accessing this class through the base base class 
	// can get at them since these methods are public in there.
	void Start();
	void LocalName(TSockAddr& aAddr) const;
	TInt SetLocalName(TSockAddr& aAddr);
	void RemName(TSockAddr& aAddr) const;
	TInt SetRemName(TSockAddr& aAddr);
	TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption) const;
	void Ioctl(TUint aLevel,TUint aName,TDes8* aOption);
	void CancelIoctl(TUint aLevel,TUint aName);
	void ActiveOpen();
	void ActiveOpen(const TDesC8& aConnectionData);
	TInt PassiveOpen(TUint aQueueSize);
	TInt PassiveOpen(TUint aQueueSize,const TDesC8& aConnectionData);
	void Shutdown(CServProviderBase::TCloseType aOption);
	void Shutdown(CServProviderBase::TCloseType aOption, const TDesC8& aDisconnectionData);
	void AutoBind();
	TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr=NULL);
	TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* aAddr);
	TInt SecurityCheck(MProvdSecurityChecker* aSecurityChecker);

	// virtual from CBluetoothSAP
	// Made private so that only people accessing this class through the base base class 
	// can get at them since these methods are public in there.
	TInt SAPSetOption(TUint aLevel,TUint aName,const TDesC8& aOption);

	// Callbacks
	void StartSendAsyncCallBack();
	void CancelSendAsyncCallBack();
	static TInt SendAsyncCallBack(TAny* aSap);

    // Utility functions
	TInt SetPid(SymbianAvctp::TPid aPid);
	MSocketNotify& Socket() const;
	void CheckForCanSend();
	void CheckForCanClose();	
	TInt CheckPidAllowed(SymbianAvctp::TPid aPid);
	
	void MuxerDown(const TBTDevAddr& aRemoteAddr);
	
private:
	// Pure virtuals From MSocketNotify
	// Made private so that only people accessing this class through the M- base class 
	// can get at them since these methods are public in the M- class.
	void NewData(TUint aCount);	// it's not used in favour of the public overloaded NewData()
	void CanSend();
	void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
	void ConnectComplete(CServProviderBase& aSSP);
	void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	void CanClose(TDelete aDelete=EDelete);
	void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	void Error(TInt aError, TUint aOperationMask=EErrorAllOperations);
	void Disconnect(void);
	void Disconnect(TDesC8& aDisconnectData);
	void IoctlComplete(TDesC8* aBuf);
	void NoBearer(const TDesC8& /*aConnectionInfo*/);
	void Bearer(const TDesC8& /*aConnectionInfo*/);

private: // unowned

	/** The protocol object */
	CAvctpProtocol& iProtocol;

private: // owned

	/** The packets that the Sap has taken responsibility of sending */
	TAvctpOutboundQ	iOutgoingSdus;
	/** The packets that the Sap has taken responsibility of, and has notified to esock to pick up 
		Packets should be added on the end as they come in and taken off the front as they go up to
		esock.*/
	TDblQue<HAvctpIncomingSdu>	iIncomingSdus;
	
	CAsyncCallBack* iSendAsyncCallBack;	/** Asynchronous callback for processing */
	SymbianAvctp::TPid	iPid;	/** The PID for this socket*/
	/** A class provided by esock that will allow us to perform the PID security check*/
	MProvdSecurityChecker* iSecurityChecker;
	
	TBool iSendBlocked;
	
	/** We need to store the address of any pending data we've been asked to send in case we need to error it
	    if the associated muxer goes down for some reason. Note this member variable will only contain a
	    valid address when iSendBlocked is ETrue */
	TBTDevAddr iPendingSendAddress;	
	TBool iIsClosing;
	TBool iClosed;
	
	TDblQueLink iQueLink;	/** A link so that the protocol can queue Saps */

	TInt				iChannel;
	TBool				iIsInList;
	};


// the SAPs are "sockets" so that the avctp transport can be very thin and "transient" so effectively forwards calls from L2CAP
// perhaps with a bit of roundrobinning in the CanSend
class CAvctpControlSAP : public CBluetoothSAP, public MSocketNotify
	{
public:
	static CAvctpControlSAP* NewL(CAvctpProtocol& aProtocol);
private:
	CAvctpControlSAP(CAvctpProtocol& aProtocol);
private:
	void Start();
	void LocalName(TSockAddr& aAddr) const;
	TInt SetLocalName(TSockAddr& aAddr);
	void RemName(TSockAddr& aAddr) const;
	TInt SetRemName(TSockAddr& aAddr);	
	TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption) const;
	TBool IsActiveRequest(TUint aName);
	void Ioctl(TUint aLevel,TUint aName,TDes8* aOption);
	void CancelIoctl(TUint aLevel,TUint aName);
	void ActiveOpen();
	void ActiveOpen(const TDesC8& aConnectionData);
	TInt PassiveOpen(TUint aQueueSize);
	TInt PassiveOpen(TUint aQueueSize,const TDesC8& aConnectionData);
	void Shutdown(CServProviderBase::TCloseType aOption);
	void Shutdown(CServProviderBase::TCloseType aOption, const TDesC8& aDisconnectionData);
	void AutoBind();
	TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* aAddr);
	TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* aAddr);
	TInt SecurityCheck(MProvdSecurityChecker* aSecurityChecker);

	// virtual from CBluetoothSAP
	// Made private so that only people accessing this class through the base base class 
	// can get at them since these methods are public in there.
	TInt SAPSetOption(TUint aLevel,TUint aName,const TDesC8& aOption);
	void SetOutstandingIndication(const TControlIoctlMessage& aMessage);
	void QueueIncomingIndication(const TControlIoctlMessage& aMessage);
	void CompleteIoctlWithError(const TControlIoctlMessage& aMessage);
	void QueueErrorIoctlComplete(const TControlIoctlMessage& aMessage);
	void CompleteQueuedIoctlWithError(TInt aError);
private:
// Pure virtuals From MSocketNotify
// Made private so that only people accessing this class through the M- base class 
// can get at them since these methods are public in the M- class.
	void NewData(TUint aCount);
	void CanSend();
	void ConnectComplete();
	void ConnectComplete(const TDesC8& aConnectData);
	void ConnectComplete(CServProviderBase& aSSP);
	void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	void CanClose(TDelete aDelete=EDelete);
	void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	void Error(TInt aError, TUint aOperationMask=EErrorAllOperations);
	void Disconnect(void);
	void Disconnect(TDesC8& aDisconnectData);
	void IoctlComplete(TDesC8* aBuf);
	void NoBearer(const TDesC8& /*aConnectionInfo*/);
	void Bearer(const TDesC8& /*aConnectionInfo*/);

	void ProcessIoctlQueue();
	inline void ClearIoctl();
	
private:
	CAvctpProtocol&				iProtocol;
	TDblQueLink					iQueLink;
	TSglQue<HQueuedIndication>	iIndicationQueue;
	};

	
#include "avctpsap.inl"

#endif // AVCTPSAP_H

