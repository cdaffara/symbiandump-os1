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

#ifndef AVCTP_H
#define AVCTP_H

#include <es_prot.h>
#include <bluetoothav.h>
#include <e32hashtab.h>
#include "btsockettimer.h"
#include "avctpcommon.h"
#include <es_prot_internal.h>

namespace SymbianAvctp
{

const static TUint KServiceInfo =
	KSIConnectionLess | KSIReliable | KSIInOrder | 
	KSIDatagram | KSIGracefulClose;

const static TInt KServiceTypeInfo = 
	ESocketSupport | ECantProcessMBufChains | EUseCanSend;

const static TInt KMaxNumSocketsInProtocol = 10;
} // end of namespace SymbianAvctp


// ok to not have in namespace
const static TL2CAPPort KAvctpSecondChannelPSM = 0x1b;
const TInt KAvctpSecondaryChannelOutboundMTU = 0xffff;
const TInt KAvctpSecondaryChannelInboundMTU = 0xffff;

// forward declarations
class CAvctpSap;
class CAvctpTransport;
class CAvctpMuxerStateFactory;
class CAvctpConnection;
class CAvctpPacketMgr;
class HAvctpIncomingSdu;
class HAvctpOutgoingSdu;
class CAvctpSubConDeviceErrorEvent;
class CAvctpSubConEventConnectCfm;
class CAvctpSubConEventConnectInd;
class CAvctpSubConEventDiscCfm;
class CAvctpSubConEventDiscInd;
class MAvctpSDUSender;

NONSHARABLE_CLASS(TClientItem)
	{
public:
	TClientItem(TUint16 aClientId);
	void AttachPrimaryChannel(MSocketNotify& aNotify);
	void AttachSecondaryChannel(MSocketNotify& aNotify);
	void DetachSecondaryChannel();
	TUint16 ClientId() const;
	MSocketNotify* PrimaryChannel() const;
	MSocketNotify* SecondaryChannel() const;
	TBool IsSecondaryChannelAttached() const;
private:
	TUint16					iClientId;
	MSocketNotify*			iPrimaryChannel;
	MSocketNotify*			iSecondaryChannel;
	TBool					iIsSecondaryChannelAttached;
	TInt					iRefCount;
	};

/**
The AVCTP Protocol object.  Used by BT_v2.prt to create a CProtocolBase object
for ESOCK, so ESOCK can create AVCTP sockets.

This is the object created by ESock to give access to AVCTP services

  @internalAll
  @see CProtocolBase
*/

NONSHARABLE_CLASS(CAvctpProtocol) : public CBluetoothProtocolBase
    {
public:
	// Factory functions
    static CAvctpProtocol* NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
    static CAvctpProtocol* NewLC(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	
	// Functions from CProtocolBase
	virtual void InitL(TDesC& aTag);				// need to get a state factory
	virtual void StartL();							// check we're bound
	virtual void BindToL(CProtocolBase* aProtocol);	// bind to lower protocol (L2CAP)
	virtual void CloseNow();
	virtual void Open();
	virtual void Close();
	virtual void Identify(TServerProtocolDesc* aProtocolDesc) const;
	virtual CServProviderBase* NewSAPL(TUint aSockType);
	
	// Functions  from CBluetoothProtocolBase
	virtual TInt BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSSP);
	virtual TInt ControlPlaneMessage(TBTControlPlaneMessage aMessage, TAny* aParam);

	// Other general protocol functions	
	TInt StartProtocolListening();
	void DoStartAvctpListeningL();
	static void ProtocolIdentity(TServerProtocolDesc* aDesc); // Static query function used by Identify
	TInt SetPreauthorisation(const TBTDevAddr& aPreauthoriseAddress, TBool aSetPreauthorisation);

	// AVCTP specific functions
	CAvctpMuxerStateFactory& MuxerStateFactory() const;

	// Interface to the muxer
	TInt ProvideTransport(const TBTDevAddr& aRemoteAddr, TUint16 aClientId, MSocketNotify& aControlSocket);
	TInt ActiveExtendTransport(const TBTDevAddr& aRemoteAddr, TUint16 aClientId, MSocketNotify& aSecondChannelControlSocket);
	void AwaitForExtendedTransport(TUint16 aClientId, MSocketNotify& aSecondChannelControlSocket);
	void PrimaryChannelRefuseAttach(const TBTDevAddr& aRemoteAddr, TUint16 aClientId);
	void SecondaryChannelRefuseAttach(const TBTDevAddr& aRemoteAddr, TUint16 aClientId);	
	TInt AttachInboundConnectionToTransport(const TBTDevAddr& aRemoteAddr, CServProviderBase* aL2CAPConSAP);
	TInt GetChannelMtu(TInt aChannel, const TBTDevAddr& aAddr, TInt& aMtu);
	TInt ReleaseTransport(const TBTDevAddr& aRemoteAddr, TUint16 aClientId);
	TInt ReleaseExtendedTransport(const TBTDevAddr& aRemoteAddr, TUint16 aClientId);
	void PrimaryChannelIncomingRemoteDisconnection(const TBTDevAddr& aRemoteAddr);
	MAvctpSDUSender* GetSDUSender(const TBTDevAddr& aRemoteAddr, TInt aChannel);
	CAvctpTransport* FindTransport(const TBTDevAddr& aDevAddr);
	void AwaitTransport(TUint16 aClientId, MSocketNotify& aControlSocket);
	TInt PrimaryChannelAgreeAttachment(TUint16 aClientId, const TBTDevAddr& aRemoteAddr);
	TInt SecondaryChannelAgreeAttachment(TUint16 aClientId, const TBTDevAddr& aRemoteAddr);
	// Interface to all Saps provided for the muxers
	TBool SapsHaveDataFor(const TBTDevAddr& aRemoteAddr);
	void  SignalCanSendToSaps(CAvctpPacketMgr& aPacketMgr);
	void  SignalNewDataToSaps(HAvctpIncomingSdu* aSdu, TInt aChannel);
	void  SignalMuxerDownToSaps(const TBTDevAddr& aRemoteAddr);
	
	// Que Management
	TInt AddSap(CAvctpSap& aSap);
	void RemoveSap(CAvctpSap& aSap);
	CAvctpSap* FindSap(TUint16 aClientId);
	TInt AddTransport(CAvctpTransport& aMuxer);
	void RemoveTransport(CAvctpTransport& aMuxer);
	CAvctpTransport* FindOrCreateTransport(const TBTDevAddr& aDevAddr, TInt aChannel, CServProviderBase* aL2CAPConSAP = NULL);
	void SetSecondChannelCtrlNotify(TUint16 aClientId, MSocketNotify& aSecondChannelControlSocket);
	void NotifyLinkUp(const TBTDevAddr& aAddr, TBool aIsSecondChannel);
private:
	CAvctpProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	void ConstructL();
	~CAvctpProtocol();

	// Close Functions
	void CheckForIdle();
	TBool IsIdle();
	void QueIdleTimer();
	void DequeIdleTimer();
	const TClientItem* AddClientItem(TUint16 aClientId, MSocketNotify& aNotify);
	const TClientItem* ClientItem(TUint16 aClientId) const;
	static TInt TryToClose(TAny* aProtocol);
	
	// Ipid Response Callback functions
	static TInt IpidAsyncCallBack(TAny* aProtocol);
	inline void StartSendIpidAsyncCallBack();
	inline void CancelIpidAsyncCallBack();
	
#ifdef __FLOG_ACTIVE
	void LogSaps();
	void LogMuxers();
#endif
	
private:
	/** The state factory for the lightweight AVCTP SAP states */
	CAvctpMuxerStateFactory*		iMuxerStateFactory;

	// Queues
	/** All AVCTP Service Access Providers are owned by esock. 
	    This queue is for Saps that have been assigned a PID 
	    to allow us to enforce the 1 SAP / PID rule */
	TDblQue<CAvctpSap> iSaps;
	/** All AVCTP transports are owned by the protocol.
	    This queue is for Muxers that have been assigned a remote address
	    to allow us to enforce the 1 Muxer / remote address rule */
	TDblQue<CAvctpTransport> iTransports;
	/** This is a Que of IPID responses to remote devices 
		These are owned by the protocol */
	TDblQue<HAvctpOutgoingSdu> iIpidResponses;

	// Stuff for close callbacks, etc.
	/** Whether we are about to die */
	TBool					iClosePending;

	/** An entry in the BT protocol family delta timer queue
		The idle timer is something that runs to prevent the protocol going down and 
		then being brought up again immediately because this is costly. Instead we 
		stay alive for a small amount of time to try and smooth out the Opening and
		closing */
	TDeltaTimerEntry		iIdleTimerEntry;
	TBool			iIdleTimerQueued;
	
	CAsyncCallBack*	iIpidResponseAsyncCallBack;
	CIncomingConnectionListener*	iSecondChannelListener;
	RHashMap<TInt, TClientItem>		iClientItems;
	};

#include "avctp.inl"

#endif // AVCTP_H
 
