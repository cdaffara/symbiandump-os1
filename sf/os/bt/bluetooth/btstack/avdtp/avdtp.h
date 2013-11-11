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
// Defines the avdtp protocol class
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTP_H
#define AVDTP_H

#include <es_prot.h>
#include <bt_sock.h>
#include <bluetoothav.h>
#include "secman.h"
#include "notification.h"
#include "bt.h"
#include "avdtpTransportSession.h"
#include "avdtpSEPCache.h"
#include "avdtpStream.h"
#include "avdtpLocalSEP.h"
#include "avdtpAllocators.h"
#include <es_prot_internal.h>

// Stream and Datagram I/f's are muturally exclusive. You can't have some sockets one
// time and some the other -- esock will always give presidence to stream.
// don't support graceful close
const TUint KAvdtpServiceInfo = KSIInOrder|
								KSIDatagram|
								KSICanReconnect;
#ifdef _OOM_TEST
const TInt KAvdtpIdleTimeout = 1000;
#undef __ASSERT_DEBUG
#define __ASSERT_DEBUG(c,p) 
#else
const TInt KAvdtpIdleTimeout = 5000000;
#endif

const TUid KAvdtpUID = {0x100095FE};

class CBTProtocolFamily;
class CLinkMgrProtocol;
class CBTInquiryMgr;
class CTransportChannel;
class CSignallingChannel;

class CAvdtpSAP;
class CLogicalChannelFactory;
class CAVStream;
class CRemoteSEPCache;
class CAVStreamStateFactory;

NONSHARABLE_CLASS(CAvdtpProtocol) : public CBluetoothProtocolBase
	{
friend class CAvdtpSAP;

public:
	// export for protocol
	
	static CAvdtpProtocol* NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	static CAvdtpProtocol* NewLC(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	// From CProtocolBase
	// Factories
	CServProviderBase* NewSAPL(TUint aSockType);
	
	void Close();
	void Open();
	void CloseNow();

    // From ProtocolManager before all binding.
	void InitL(TDesC &aTag);
	// From Protocol Manager - after all binding
	void StartL();	
	// From higher protocol
	void BindL(CProtocolBase* aProtocol, TUint aId);
	void BindToL(CProtocolBase* aProtocol);	// From Protocol Manager

	// Query functions
	void Identify(TServerProtocolDesc* aDesc) const;	
	static void ProtocolIdentity(TServerProtocolDesc* aDesc);	

	// bluetooth protocol overridden functions
	TInt StartProtocolListening();

	// Get a factory for the logical channels

	inline CLogicalChannelFactory& LogicalChannelFactory() const;
	TInt SetPreauthorisation(const TBTDevAddr& aPreauthoriseAddress, TBool aSetPreauthorisation);

	void StreamCreated(CAVStream& aStream);
	CAVStream* FindStream(const TAvdtpSockAddr& aAddr);
	void RemoveStream(CAVStream& aStream);
	inline const CAVStreamStateFactory& StreamStateFactory() const;

	CTransportChannel* GetTransportChannel(const TAvdtpSockAddr& aSockAddr, TBool aUseMux, 
											TTCID aRemotelyAssignedTTCID=KInvalidTCID);
	CTransportChannel* FindMuxChannel(TTCID aTCID);
	CSignallingChannel* GetSignallingChannel(const TBTDevAddr& aDevAddr);
	CSignallingChannel* FindSignallingChannel(const TBTDevAddr& aDevAddr);
	CSignallingChannel* FindListeningSignallingChannel();
	CSignallingChannel* CreateSignallingChannelForListening();
	void ConnectSignallingListeners(CSignallingChannel& aConnectedSignallingChannel);
	CUserPlaneTransportSession* CreateUserPlaneSessionL(CAvdtpSAP& aSAP,
														CAVStream& aStream,
														TAvdtpTransportSessionType aType);
	
	virtual TInt BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSAP);

	void TransportChannelDown(CTransportChannel& aTransportChannel);
	void TransportChannelClosing(CTransportChannel& aTransportChannel);

	void SignallingChannelDown(CSignallingChannel& aSignallingChannel);

	inline CRemoteSEPCache& RemoteSEPCache() const;
	inline TSEIDManager& SEIDManager();
	
	// secondary SAP management

	void AddSecondarySAP(CAvdtpSAP& aSecondarySAP);
	CAvdtpSAP* GetSecondarySAP();
	
	virtual TInt ControlPlaneMessage(TBTControlPlaneMessage aMessage, TAny* aParam);

private:
	CAvdtpProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	~CAvdtpProtocol();
	void ConstructL();

	CTransportChannel* CreateTransportChannelL(const TBTDevAddr& aAddr, TBool aMuxed);

	void QueIdleTimerEntry();
	void RemoveIdleTimerEntry();
	TBool ShouldClose();
	static TInt TryToClose(TAny* aProtocol);
	void DestroySecondarySAPs();
	void DoStartAvdtpListeningL();

private:	
	// Singleton objects
	CLogicalChannelFactory*	iLogicalChannelFactory;
	CAVStreamStateFactory*	iStreamStateFactory;

	CRemoteSEPCache*		iRemoteSEPCache;
	TDeltaTimerEntry		iIdleTimerEntry;
	TBool					iIdleEntryQueued;
	
	// List of TransportChannels (Direct and Mux)
	TDblQue<CTransportChannel>	iTransportChannels;
	TDblQue<CTransportChannel>	iClosingTransportChannels;

	TDblQue<CSignallingChannel>	iSignallingChannels;
	// no closing signalling channel tracking as they perform immediate shutdowns

	TDblQue<CAVStream>			iStreams;	// to allow for U-plane sessions to find the stream they belong to
		
	TSEIDManager				iSEIDManager;
	// Q of SAPs waiting for a transport channel to attach to
	TSglQue<CAvdtpSAP>  		iSecondarySAPs;
	
	TBool						iClosePending;
	};


inline CLogicalChannelFactory& CAvdtpProtocol::LogicalChannelFactory() const
	{
	return *iLogicalChannelFactory;
	}

inline CRemoteSEPCache& CAvdtpProtocol::RemoteSEPCache() const
	{
	return *iRemoteSEPCache;
	}
	
inline TSEIDManager& CAvdtpProtocol::SEIDManager()
	{
	return iSEIDManager;
	}

inline const CAVStreamStateFactory& CAvdtpProtocol::StreamStateFactory() const
	{
	return *iStreamStateFactory;
	}


#endif
