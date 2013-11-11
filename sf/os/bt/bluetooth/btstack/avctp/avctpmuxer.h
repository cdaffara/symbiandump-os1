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
 @internalComponent
*/

#ifndef AVCTPMUXER_H
#define AVCTPMUXER_H

#include <es_prot.h>
#include <e32hashtab.h>
#include "avctpconstants.h"
#include "avctpcommon.h"

class CAvctpPacketMgr;
class CAvctpProtocol;
class CBluetoothSAP;
class TAvctpMuxerState;
class TAvctpStateClosed;
class TAvctpStateLinkPending;
class TAvctpStateOpen;

class MSecondChannelPseudoSocketNotify;
class TClientItem;

typedef THashMapIter<TInt, const TClientItem*> TClientItemIter;

class TAvctpSecondChannelPseudoSocket : public MSocketNotify
	{
public:
	TAvctpSecondChannelPseudoSocket(MSecondChannelPseudoSocketNotify& aNotify);
private:
	// Pure virtuals From MSocketNotify
	// Made private so that only people accessing this class through the M- base class 
	// can get at them since these methods are public in the M- class.
	void NewData(TUint aCount);
	void CanSend();
	void ConnectComplete();
	void ConnectComplete(const TDesC8& /*aConnectData*/);
	void ConnectComplete(CServProviderBase& aSSP);
	void ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/);
	void CanClose(TDelete aDelete);
	void CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/);	
	void Error(TInt aError, TUint aOperationMask);
	void Disconnect();
	void Disconnect(TDesC8& /*aDisconnectData*/) {/*drop*/};
	void IoctlComplete(TDesC8* /*aBuf*/);
	void NoBearer(const TDesC8& /*aConnectionInfo*/) {/*drop*/};
	void Bearer(const TDesC8& /*aConnectionInfo*/) {/*drop*/};

private:
	MSecondChannelPseudoSocketNotify&	iNotify;
	};
	

/*
 This class doesn't derive from MSocketNotify, so it has only the methods we need to be implemented.
 */

class MSecondChannelPseudoSocketNotify
	{
public:
	virtual void MbpsnNewData(TUint aCount) =0;
	virtual void MbpsnCanSend() =0;
	virtual void MbpsnConnectComplete() =0;
	virtual void MbpsnConnectComplete(CServProviderBase& aSAP) =0;
	virtual void MbpsnCanClose(MSocketNotify::TDelete aDelete=MSocketNotify::EDelete) =0;
	virtual void MbpsnError(TInt aError, TUint aOperationMask=MSocketNotify::EErrorAllOperations) =0;
	virtual void MbpsnDisconnect() =0;
	};

/**
Acts as the "channel management entity".
   
There is one of these per remote device, as only one L2CAP connection 
can exist between two devices for AVCTP.  This class acts as an internal
socket binding to a lower level SAP.  Thus we derive from MSocketNotify
so that SAP can directly notify us of events.

Even though in the current usage of AVCTP via RAvctp, this class will not 
be managed by a CSubConnection class in ESOCK, it is a CFactoryObject and 
hence will be the responsibility of the CSubConnectionProviderFactoryBase
to delete.

This class leaves the data handling completely up to CAvctpSap. All it does 
is provide a path for the data. The only slightly clever thing it does is
get any data that the lower protocol sap indicates has arrived (NewData upcall)
and gives this to the protocol for distribution to the relevant CAvctpSap.
So data to send is pushed from the Sap to the Muxer and data received is 
pushed from the Muxer to the Sap.

Note that this class doesn't currently support ioctls on the lower protocol
sap below. 

@internalComponent
@see MSocketNotify
*/
NONSHARABLE_CLASS(CAvctpTransport) : public CBase, public MSocketNotify, public MSecondChannelPseudoSocketNotify
	{
	friend class CAvctpProtocol; // needed for iQueLink access

	// States can all be friends if they want to be ;-)
	friend class TAvctpMuxerState;
	friend class TAvctpStateClosed;
	friend class TAvctpStateLinkPending;
	friend class TAvctpStateOpen;
	
	friend class TAvctpStateSecondChannelPending;
	friend class TAvctpStateFullyOpen;

public:
	// Create/destroy
	static CAvctpTransport* NewL(CAvctpProtocol& aProt);

	// Start functions
	TInt Start(const TBTDevAddr& aAddr, TUint16 aClientId); // outgoing
	TInt StartIncoming(const TBTDevAddr& aAddr, CServProviderBase* aL2CAPConSAP);	

	// Commands from the Avctp Sap
	TInt Send(const RMBufChain& aData, TInt aChannel); 

	inline const TBTDevAddr& DevAddr() const;
	inline TBool IsClearToSend(TInt aChannel) const;
	inline CAvctpPacketMgr& PacketMgr();	
	
	TBool HasDataToSend();
	void Shutdown(TInt aError);
	TInt GetCurrentInboundMtu(TInt aChannel);
	TInt GetCurrentOutboundMtu(TInt aChannel);
	TInt DoWrite(RMBufChain& aPDU, TInt aChannel);
	
	void CheckForIdle();
	
	void NotifyAttachConfirm(TInt aError, TBool aIsSecondChannel);
	void NotifyAttachConfirm(TUint16 aClientId, TInt aError, TBool aIsSecondChannel);
	void NotifyDetachConfirm(TUint16 aClientId, TInt aError, TBool aOnlySecondChannel = EFalse);
	void NotifyLinkError(TInt aError, TBool aIsSecondChannel);
	
	TInt AddSecondChannel(CServProviderBase& aSAP);
	TBool HasSecondChannel() const;
	void RemoveSecondChannel();
	
	void NotifyLinkUp(const TBTDevAddr& aAddr, TBool aIsSecondChannel = EFalse);
	void NotifyLinkDown(const TBTDevAddr& aAddr, TBool aIsSecondChannel = EFalse, TInt aError = KErrNone);
	TInt GetChannelMtu(TInt aChannel, TInt& aMtu) const;
	
	void SetSecondChannelCtrlNotify(TUint16 aClientId, MSocketNotify& aSecondChannelControlSocket);
	TInt AddPrimaryChannelRef(const TClientItem* aItem);
	void RemovePrimaryChannelRef(TUint16 aClientId);
	void AddSecondaryChannelRef();
	void RemoveSecondaryChannelRef(TUint16 aClientId);
	TInt ClientCount();
	TInt ClientSecondaryChannelCount();
	TBool HasClient(TUint16 aClientId);
	void BindSecondaryChannelSap(CServProviderBase& aSAP);
private:
	~CAvctpTransport();
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
	void NoBearer(const TDesC8& /*aConnectionInfo*/) {};
	void Bearer(const TDesC8& /*aConnectionInfo*/) {};

private:
	virtual void MbpsnNewData(TUint aCount);
	virtual void MbpsnCanSend();
	virtual void MbpsnConnectComplete();
	virtual void MbpsnConnectComplete(CServProviderBase& aSAP);
	virtual void MbpsnCanClose(MSocketNotify::TDelete aDelete=MSocketNotify::EDelete);
	virtual void MbpsnError(TInt aError, TUint aOperationMask=MSocketNotify::EErrorAllOperations);
	virtual void MbpsnDisconnect();

private:
	CAvctpTransport(CAvctpProtocol& aProtocol);
	void ConstructL();

	// Utility Functions
	void AssignToDevice(const TBTDevAddr& aRemoteAddr);
	// Management functions for iIsClearToSend 
	inline void SetSendBlocked(TInt aChannel);
	inline void SetClearToSend(TInt aChannel);

	// NewData Callback functions
	static TInt SecondChannelNewDataAsyncCallBack(TAny* aMuxer);
	inline void StartSecondChannelNewDataAsyncCallBack();
	inline void CancelSecondChannelNewDataAsyncCallBack();
	
	// Idle Functions
	TBool IsIdle();
	void QueIdleTimer();
	void DequeIdleTimer();
	static TInt IdleTimerExpired(TAny* aMux);	
	void NotifyLinkState(const TBTDevAddr& aAddr, SymbianAvctp::TControlIoctls aIoctl, TInt aChannel, TInt aError);
	
private:
	static const TUint 				KMaxChannels = 2;
	
	CAvctpProtocol& 				iProtocol;	/** The avctp protocol object */
	
	/** Endpoints in the lower protocol over which we are communicating (normally L2CAP) */
	TFixedArray<CServProviderBase*, KMaxChannels>	iChannelSAPs;
	
	TAvctpSecondChannelPseudoSocket iSecondChannelSocket;
	
	TAvctpMuxerState* 				iState;		/** The current state */	
	TBTDevAddr iRemoteAddr;			/** The Bluetooth address of the remote device */
	TDeltaTimerEntry 				iIdleTimerEntry;	/** Disconnection idle timer */
	TBool 							iIdleTimerQueued;			/** Whether we have an idle timer queued */

	TDblQueLink 					iQueLink;	/** A link so that the protocol can queue multiplexors */

	TFixedArray<TBool,2> 			iIsClearToSend;
	
	// Incoming data asyncs for second channel
	CAsyncCallBack*					iNewDataAsyncCallBack; 	/** Async Callback to get New Data from L2CAP */
	TUint 							iSecondChannelPacketsWaiting; /** The number of packets waiting for us to pick up from L2CAP */
	
	CAvctpPacketMgr* 				iPacketMgr;
	RHashMap<TInt, const TClientItem*>	iClientItems;
	TBTDevAddr						iAddress;
	TInt 							iSecondaryChannelClientRefCount;
	};


	
#include "avctpmuxer.inl"

#endif 
// AVCTPMUXER_H

