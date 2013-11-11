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


/*
 the incoming assembler owns a chain to try to preserve the idea to not allocate
 the outgoing fragmenter owns the HOutboundSDU
 
 the inbound one could change, but it may be ok to only transfer the MBufChain into the HInboundSDU when we are happy that
 we have something worth holding onto on a queue
 */

#ifndef AVCTPPACKETMGR_H
#define AVCTPPACKETMGR_H

#include "avctppacket.h"

// Forward declarations
class CAvctpProtocol;
class CAvctpTransport;
class CPartialIncomingSdu;
class CPartialOutgoingSdu;
class HAvctpOutgoingSdu;
class RMBufChain;
class CAvctpPacketMgr;

NONSHARABLE_CLASS(MAvctpSDUSender)
	{
public:
	virtual void Write(HAvctpOutgoingSdu*& aOutgoingSdu, TInt aChannel) =0; 
	virtual TInt WriteIpid(HAvctpOutgoingSdu*& aOutgoingSdu) =0; 
	virtual TBool IsClearToSend(TInt aChannel) const =0;
	};

	
/**
This class helps the Mgr collect together the incoming packet fragments from a muxer
*/
NONSHARABLE_CLASS(CIncomingSduAssembler) : public CBase
	{
public:
	static CIncomingSduAssembler* NewL(CAvctpPacketMgr& aMgr, TInt aChannel);
	~CIncomingSduAssembler();

	void Reset();	
	void AddDataL(TAvctpStartHeaderInfo& aHeaderInfo, RMBufChain& aIncomingPdu);
	
private:
	CIncomingSduAssembler(CAvctpPacketMgr& aMgr, TInt aChannel);
	void ConstructL();
	
	void ProcessNormalPduL(RMBufChain& aIncomingPdu);
	void ProcessStartPdu(TAvctpStartHeaderInfo& aHeaderInfo, RMBufChain& aIncomingPdu);
	void ProcessContinuePdu(const TAvctpStartHeaderInfo& aHeaderInfo, RMBufChain& aIncomingPdu);
	void ProcessEndPduL(const TAvctpStartHeaderInfo& aHeaderInfo, RMBufChain& aIncomingPdu);
	
private:
	static const TInt	KAvctpNormalHeaderMask = 0xf3;
	
	CAvctpPacketMgr&	iMgr;
	RMBufChain			iAccretingSdu;
	TUint				iFragmentsReceived;
	TAvctpStartHeaderInfo	iStartHeaderInfo;
	
	/** The spec mandates that Start and Continue fragments have the same length &
	that End fragments have a equal or smaller size. NB it doesn't say the size
	has to equal the current MTU
	*/
	TUint				iContinueFragmentSize;
	TInt				iChannel;
	};
	

/**
This class helps the Mgr fragment the outgoing Sdus from a sap
*/
NONSHARABLE_CLASS(COutgoingSduFragmenter) : public CBase
	{
public:
	enum TWriteState
		{
		ENormal,
		EFragmenting,
		};

public:
	static COutgoingSduFragmenter* NewL(CAvctpPacketMgr& aMgr, TInt aChannel);
	~COutgoingSduFragmenter();

	TBool HasData();
	void Write(HAvctpOutgoingSdu*& aOutgoingSdu);
	void CanSend();
	void Reset();	
	
private:
	COutgoingSduFragmenter(CAvctpPacketMgr& aMgr, TInt aChannel);
	void ConstructL();
	
	TInt CountFragments(const RMBufChain& aSdu, TInt iMtuUsedToFragment) const;
	void DoSendCurrentSDU();
	void BeginSendingSdu(TInt aMtu);
	void ContinueSendingSdu(TInt aMtu);
	
	TInt AddHeader(RMBufChain& aPdu, TInt aNumFragments);
	
	// Callbacks
	void StartSendAsyncCallBack();
	void CancelSendAsyncCallBack();
	static TInt SendAsyncCallBack(TAny* aFragmenter);
	
	void CheckForCanSend();

private:
	CAvctpPacketMgr&		iMgr;	
	
	RMBufChain				iSduData;
	TAvctpNormalHeaderInfo	iHeaderInfo;  // FIXME rename
	TBTDevAddr				iAddr;
	
	TInt					iChannel;
	TWriteState				iCurrentWriteState;
	CAsyncCallBack*			iSendAsyncCallBack;	
	};
	

	
NONSHARABLE_CLASS(CAvctpPacketMgr) : public CBase,
									 public MAvctpSDUSender
	{
public:
	~CAvctpPacketMgr();
	static CAvctpPacketMgr* NewL(CAvctpTransport& aMuxer, CAvctpProtocol& aProtocol);
	
	// From above
	virtual void Write(HAvctpOutgoingSdu*& aOutgoingSdu, TInt aChannel); 
	virtual TInt WriteIpid(HAvctpOutgoingSdu*& aOutgoingSdu); 
	
	// From the transport
	void CanSend(TInt aChannel);
	void SignalMuxerError(TInt aError,TUint aOperationMask);
	TInt NewData(RMBufChain& aIncomingPdu, TInt aChannel);
	TBool WouldLikeToSend();
	
	inline CAvctpTransport& Transport() const;
	inline CAvctpProtocol& Protocol() const;
	inline TBool IsClearToSend(TInt aChannel) const;
	inline const TBTDevAddr& DevAddr() const;
	
private:
	CAvctpPacketMgr(CAvctpTransport& aMuxer, CAvctpProtocol& aProtocol);
	void ConstructL();
	
private:
	CAvctpTransport&		iTransport;
	CAvctpProtocol&			iProtocol;

	TFixedArray<CIncomingSduAssembler*,2>	iIncomingAssemblers;
	TFixedArray<COutgoingSduFragmenter*,2>	iOutgoingFragmenters;	

	TUint					iIpidSdusSent;
	};

#include "avctppacketmgr.inl"

#endif // AVCTPPACKETMGR_H 

