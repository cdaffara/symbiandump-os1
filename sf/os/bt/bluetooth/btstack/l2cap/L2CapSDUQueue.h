// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef L2CAPSDUQUEUE_H_
#define L2CAPSDUQUEUE_H_

#include <bt_sock.h>

#include "L2CapSDU.h"


class TL2CapDataControllerConfig;
class CL2CapBasicDataController;
class CL2CAPMux;
class CL2CAPConnectionSAP;

NONSHARABLE_CLASS(CL2CapSDUQueue) : public CBase,
                       public ML2CapSDUHandler
	{
public:
	// Can buffer this many outgoing SDUs.
	const static TUint8 KDefaultOutboundQueueSize = 2;

	// Can buffer this many incoming SDUs. That's after reassembly, doesn't
	// include I-Frames that a data controller can buffer on its own.
	// Note: leaving this at 5 until the problem with legacy data controllers putting
	// frames on the Q even when they've been told not to is fixed - then should probably
	// decrease to 2.
	const static TUint8 KInboundQueueFlowCtrlThreshold = 5;

	// During normal shutdown initiated by our upper layer we try to deliver
	// all outstanding data, but kill it brutally if this time is exceeded.
	const static TUint8 KOutgoingQueueClosingTimer = 60;

	enum TIncomingSDUQueueCloseAction
		{
		EDrainIncomingQueue,
		EImmediatelyCloseIncomingQueue,
		};

	enum TOutgoingSDUQueueCloseAction
		{
		EDrainOutgoingQueue,
		EImmediatelyCloseOutgoingQueue,
		};

	static CL2CapSDUQueue* NewLC(CL2CAPConnectionSAP& aL2CapSap,
	                             TL2CAPPort aLocalCID,
	                             TL2CAPPort aRemoteCID,
	                             CL2CAPMux& aMuxer,
	                             TUint16 aPDUSize,
  	                             TUint8 aOutboundQueueSize,
	                             TL2CapDataControllerConfig* aConfig,
	                             TUint16 aFlushTimeout, 
	                             TUint16 aMaxOutgoingMTU,
	                             TUint16 aMaxIncomingMTU,
	                             TBool aCanDropSdus);
	static CL2CapSDUQueue* NewL(CL2CAPConnectionSAP& aL2CapSap,
                                TL2CAPPort aLocalCID,
                                TL2CAPPort aRemoteCID,
	                            CL2CAPMux& aMuxer,
	                            TUint16 aPDUSize,
	                            TUint8 aOutboundQueueSize,
	                            TL2CapDataControllerConfig* aConfig,
	                            TUint16 aFlushTimeout, 
                                TUint16 aMaxOutgoingMTU,
                                TUint16 aMaxIncomingMTU,
                                TBool aCanDropSdus);
 
	~CL2CapSDUQueue();

	static TInt CanSendAsyncCallBack(TAny *aSDUQueue);
	static TInt SDUsSentAsyncCallBack(TAny *aSDUQueue);
	static TInt QueueClosingAsyncCallBack(TAny *aSDUQueue);
	
	static TInt OutgoingQueueClosingTimerExpired(TAny *aSDUQueue);
	
	void ProcessFlushTimerExpiry(CL2CapSDU& aL2CapSDU);

	HL2CapPDU* GetPDU();
	inline TBool HavePDUToSend() const;

	TInt PutIFramePDU(RMBufChain& dataFrame);		
	void PutBFramePDU(RMBufChain& dataFrame);

	void FlushCurrentlyAssembledSdu();
	void FlushOldestIncomingSdu();
	inline TBool IsIncomingQueueFull() const;
	

	TInt Write(RMBufChain& aSDUData);
	void Read(RMBufChain& readData);
		
	void CloseSDUQueue(TIncomingSDUQueueCloseAction aIncomingQueueAction = EImmediatelyCloseIncomingQueue, 
	                   TOutgoingSDUQueueCloseAction aOutgoingQueueAction = EImmediatelyCloseOutgoingQueue);

	void DataControllerDeliveredOutgoingData();

	inline void SuspendSDUQueue();
	void ResumeSDUQueue(TL2CapDataControllerConfig* aConfig,
	                    TUint16 aFlushTimeout, 
	                    TUint16 aPDUSize,
	                    TUint16 aMaxOutgoingMTU,
	                    TUint16 aMaxIncomingMTU);
	void UpdateChannelPriority(TUint8 aNewPriority);
	void ErrorD(TInt aErrorCode);

#ifdef _DEBUG
	TInt GetDataPlaneConfig(TL2DataPlaneConfig& conf) const;
	void PretendIncomingSduQFull(TBool aIncomingSduQFull);
#endif

	inline TUint16 MaxOutgoingMTU() const;
	inline TUint16 MaxIncomingMTU() const;

	inline TUint16 MaximumPDUSize() const;
	inline TUint16 OptimalPDUSize() const;
	inline void SetOptimalPDUSize(TUint16 aPDUSize);

	TBool IsBasicDataVersion() const;

private:
	void ConstructL(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, TL2CapDataControllerConfig* aConfig);
	
	CL2CapSDUQueue(CL2CAPConnectionSAP& aL2CapSap,
				   TUint8 aOutboundQueueSize,
	               TUint16 aPDUSize,
	               TUint16 aFlushTimeout, 
                   TUint16 aMaxOutgoingMTU,
                   TUint16 aMaxIncomingMTU,
                   TBool aCanDropSdus);


	void SDURemovedFromOutboundQueue();
	void CanSend();
	void SDUsSent();

	TInt HandleIncompleteSDU(RMBufChain& aIncompleteSDU);

	void TryToCloseQueue();
	void SDUQueueClosed();

	void StartOutgoingQueueClosingTimer();
	void CancelOutgoingQueueClosingTimer();
	void HandleOutgoingQueueClosingTimerExpired();

private:		
	CL2CAPConnectionSAP& iL2CapSap;
	TUint8 iOutboundQueueSize;
	
	CL2CapBasicDataController* iDataController;

	TDblQue<CL2CapSDU> iOutgoingSDUs;
	TDblQue<CL2CapSDU> iOutgoingPulledSDUs;

	RMBufPktQ iIncomingSDUs;
	RMBufChain iCurrentIncomingSDU;
	TUint16 iCurrentIncomingSDULength;
	
	TUint16 iFlushTimeout;
	TUint16 iCurrentPDUSize;
	TUint16 iMaximumPDUSize;
	TUint16 iNegotiatedPDUSize;
	
	TUint8 iCurrentOutboundQueueLength;
	TUint16 iCurrentInboundQueueLength;
	TBool iOutboundSendingBlocked;
	TBool iOutboundQueueSuspended;
	
	CAsyncCallBack* iCanSendAsyncCallBack;
	CAsyncCallBack* iSDUSentAsyncCallBack;
	CAsyncCallBack* iQueueClosingCallBack;		

	TBool iSDUQueuesClosing;
	TBool iIncomingQueueClosed;
	TBool iOutgoingQueueClosed;

	TDeltaTimerEntry iOutgoingQueueClosingTimer;
	TBool iOutgoingQueueClosingTimerRunning;

	TUint16 iMaxOutgoingMTU;
	TUint16 iMaxIncomingMTU;

	TBool	iCanDropSdus;
	};


inline TBool CL2CapSDUQueue::IsIncomingQueueFull() const
	{
	return iCurrentInboundQueueLength >= KInboundQueueFlowCtrlThreshold;
	}

inline TUint16 CL2CapSDUQueue::MaxOutgoingMTU() const
	{
	return iMaxOutgoingMTU;
	}
	
inline TUint16 CL2CapSDUQueue::MaxIncomingMTU() const
	{
	return iMaxIncomingMTU;
	}

inline TUint16 CL2CapSDUQueue::MaximumPDUSize() const
	{
	return iMaximumPDUSize;
	}

inline TUint16 CL2CapSDUQueue::OptimalPDUSize() const
	{
	return iCurrentPDUSize;
	}

inline void CL2CapSDUQueue::SetOptimalPDUSize(TUint16 aPDUSize)
	{
	iCurrentPDUSize = aPDUSize;
	}

inline void CL2CapSDUQueue::SuspendSDUQueue()
	{
	iOutboundQueueSuspended = ETrue;
	}

inline TBool CL2CapSDUQueue::HavePDUToSend() const
	{
	TBool havePDU = EFalse;
	if(!iOutgoingSDUs.IsEmpty())
		{
		CL2CapSDU* sdu = iOutgoingSDUs.First();
		// If the outbound queue is suspended, and this is the start
		// of a new SDU.  Then don't send a PDU.
		if(!(iOutboundQueueSuspended && sdu->CurrentPDUIsFirstPDU()))
			{
			if (!sdu->IsSDUEmpty())
				{
				havePDU = ETrue;
				}
			}
		}
	return havePDU;
	}

#endif
