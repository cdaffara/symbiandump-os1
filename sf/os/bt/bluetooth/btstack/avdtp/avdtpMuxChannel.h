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
//

#ifndef AVDTPMUXCHANNEL_H
#define AVDTPMUXCHANNEL_H

#include "avdtpTransportChannel.h"
#include "avdtpTransportSession.h"
#include "avdtpMessage.h"

class CTransportSession;
class CMuxChannel;


#ifdef _OOM_TEST
static const TInt KMuxSendInitial = 500;
static const TInt KMuxSendPeriod  = 500;
#else
static const TInt KMuxSendInitial = 2000000;
static const TInt KMuxSendPeriod  = 2000000;
#endif
// this constant already allows for AL header
static const TInt KMaxMuxedPacketLength = 335;// 339 for DH5 - 4 byte L2CAP
static const TInt KMuxedPacketLengthReadyToSend = 100; // for 3 slot use
static const TInt KALHeaderLength = 1; // we only support one byte (no AVDTP jumbograms)
static const TInt KALHeaderTSIDOffset = 5;
static const TInt KALHeaderFOffset = 2;
static const TInt KALHeaderLCODE_MSBOffset = 1;
static const TInt KALHeaderLCODE_LSBOffset = 0;


NONSHARABLE_CLASS(CAvdtpOutboundMuxedMessage) : public CAvdtpMessage
	{
	public:
	enum TMuxedMessageOperation
		{
		ENoMemory,
		EReadyToSendPacketAdded,
		EReadyToSendPacketNotConsumed,
		EPacketAdded,
		};
			
	static CAvdtpOutboundMuxedMessage* NewL();
	virtual TInt NewData(TUint aCount);
	TMuxedMessageOperation TryToAddPacket(TTSID aTSID, RMBufChain& aChain);
	TInt Reset();

private:
	TInt AddHeader(TTSID aTSID, TInt aPacketLen);
	/*
	*/	
	
/*
	class starts with blank muxpacket
	sessions write into channel
	muxchannel can put into send pool for that session
	muxchannelCB fetches stuff off pools into packet
	
OR!
	class starts with blank muxpacket
	sessions request to send (with a packet length?)
	channel invites send
	MuxTC can base sends on length (and above rules)
		
*/	
	
	// easier if send pools are in channels as then muxchannel can
	// "see" at any time what size packets it has and can squeeze into an ALPDU	

private:
	};


NONSHARABLE_CLASS(CAvdtpInboundMuxedMessage) : public CAvdtpMessage
	{
public:
	static CAvdtpInboundMuxedMessage* NewL(CMuxChannel& aMuxChannel);
	virtual TInt NewData(TUint aCount);	// should cause self-parsing
	
	void Reset();

private:
	CAvdtpInboundMuxedMessage(CMuxChannel& aMuxChannel);
private:
	CMuxChannel&	iMuxChannel;	// to callback when mux message parsed
	};


/*
helper for cmux channel to iterate over *all* of its sessions
it keeps its sessions in separate arrays cos of rom/ram/runtime savings
(e.g. it has to knwo the session type, but that would incur virtual function calls)

We prefer arrays so that inbound data can more quickly be put onto array rather than iter to find TSID

*/
// templates version (armv5 urel is only ~160 bytes bigger than a non-template version

template <class T>
class TSessionIterator
	{
public:
	TSessionIterator(const TArray< const TArray<T> >& aArrayOfArrays);

	const T& operator++(TInt aPostIncrementDummy);
	
	operator TBool();	// conversion operator to make iterator just like a normal one
	void Reset();
private:
	void DoIncrement();
	const TArray< const TArray<T> > iArrays;
	TInt	iArray;		// which array we're at
	TInt	iArrayIndex;// the index we're at in the array
	TBool	iFinished;
	};

	
NONSHARABLE_CLASS(CMuxChannel) : public CTransportChannel
	{
public:
	static CMuxChannel* NewL(CAvdtpProtocol& aProtocol, const TBTDevAddr& aRemoteDevice,
								TTCID aRemotelyAssignedTCID);

// management path
	virtual TInt AttachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType aType, TL2CapConfig::TChannelPriority aPriority = TL2CapConfig::ELow);
	virtual void DetachTransportSession(CUserPlaneTransportSession& aSession, TAvdtpTransportSessionType aType);
	virtual TBool CouldAttachSession(const TAvdtpSockAddr& aAddr);
	virtual TTCID TCID() const;
	CServProviderBase* ObtainSAP();
	void ActiveMultiplexer();
// send path
	virtual TUint SendPacket(TTSID aTSID, RMBufChain& aPacket);
// receive path	
	TInt GetData(RMBufChain& aRxBuffer);
	virtual void TransportSessionBlocked(TAvdtpTransportSessionType aSession, TBool aBlocked);

//... interface from inbound packet
//	TInt SessionDataReceived(TTSID aTSID, RMBufChain& aChain);
	
	~CMuxChannel();

private:	
	CMuxChannel(CAvdtpProtocol& aProtocol, const TBTDevAddr& aRemoteDevice);
	
	void UpdateChannelPriority();
	
	static TInt MuxSendIntervalCb(TAny* aCMuxChannel);
	void CheckForClose();
	TUint DoSend();
						
private:
// interesting stuff from l2cap
	virtual void NewData(TUint aCount); // forwards to inbound mux message
	virtual void Error(TInt aError,TUint aOperationMask=EErrorAllOperations);
	virtual void Disconnect();
	virtual void CanSend();
	
	void ConstructL(TTCID aRemotelyAssignedTCID);

private:
	// receive path	
	struct TUserPlaneTransportSessionState
		{
		inline TUserPlaneTransportSessionState(CUserPlaneTransportSession& aSession, TL2CapConfig::TChannelPriority aChannelPriority);
		
		CUserPlaneTransportSession& iSession;
		TBool iIsBlocked;
		TL2CapConfig::TChannelPriority iChannelPriority;
		};
		
	static void MaxChannelPriority(TL2CapConfig::TChannelPriority& aMaxPriority, const RArray<TUserPlaneTransportSessionState>& aSessions);
	
	RArray<TUserPlaneTransportSessionState>	iMediaSessions;
	RArray<TUserPlaneTransportSessionState>	iReportingSessions;
	RArray<TUserPlaneTransportSessionState>	iRecoverySessions; // recall not from same streams as other sessions

	RArray<const TArray<TUserPlaneTransportSessionState> > iSessions;

// for now shared by all the same-typed sessions
//	RPacketPool			iMediaReceivePool;
//	RPacketPool			iReportingReceivePool;
//	RPacketPool			iRecoveryReceivePool;
	
	CPeriodic*							iMuxSendTimer;	
	
	CAvdtpInboundMuxedMessage*			iInboundMessage;
	CAvdtpOutboundMuxedMessage*			iOutboundMessage;
#ifdef SESSION_ITERATOR_CONCRETE
	TSessionIterator iIter;
#else
	TSessionIterator<TUserPlaneTransportSessionState> iIter;
#endif
	// could have send iterator?
	};

inline CMuxChannel::TUserPlaneTransportSessionState::TUserPlaneTransportSessionState(
	CUserPlaneTransportSession& aSession,
	TL2CapConfig::TChannelPriority aChannelPriority)
	: iSession(aSession)
	, iChannelPriority(aChannelPriority)
	{
	}

		
#endif //AVDTPMUXCHANNEL_H
