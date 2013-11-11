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

#ifndef L2CAPDATACONTROLLER_H_
#define L2CAPDATACONTROLLER_H_

#include <e32std.h>
#include <bt_sock.h>

#include "L2CapPDU.h"
#include "L2CapSDUQueue.h"

namespace L2CapDataUtils
	{
	// Helper function used for transmit and receive sequence numbers which are 6 bits long.
	inline TUint8 Mod64(TInt aValue);
	// Helper function for checking whether a sequence number is within a given window.
	// Handles both Start <= End and Start > End.
	inline TBool InWindow(TInt aTxSeq, TInt aStart, TInt aEnd);
	}

class CL2CapSDUQueue;
class CL2CAPMux;
class CL2CapSDU;

NONSHARABLE_CLASS(CL2CapBasicDataController) : public CBase, public MOutgoingPDUHandler
	{
public:
	static CL2CapBasicDataController* NewL(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig);

	CL2CapBasicDataController(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig, TBool aIsBasicDataVersion);
	virtual ~CL2CapBasicDataController();
	
	virtual void ProcessFlushTimerExpiry();

	void UpdateConfig(TL2CapDataControllerConfig* aConfig);
	void UpdateChannelPriority(TUint8 aNewPriority);
	
	void DeregisterFromMuxer();

	// These two are the main input/output interface to the data controller, lots of interesting
	// stuff happens in their context.
	// Input - called by the muxer when a data frame is received.
	TBool HandleIncomingDataFrame(RMBufChain& aDataFrame);
	// Output - called by the muxer when it wants some data to send out.
	HL2CapPDU* GetPdu();

	// Outgoing PDU handler.
	virtual TInt HandleOutgoingIFrame(HIFramePDU* aIFrame);
	virtual TInt HandleOutgoingBFrame(HBFramePDU* aBFrame);
	virtual TInt HandleOutgoingGFrame(HGFramePDU* aGFrame);
	virtual TInt HandleOutgoingCFrame(HCFramePDU* aCFrame);
	virtual TInt HandleOutgoingSFrame(HSFramePDU* aSFrame);

	virtual void OutgoingPduAvailableOnSduQ();

	virtual void ErrorD(TInt aError);

	virtual void SetIncomingSduQFull(TBool aIncomingSduQFull);

	// Signal from the SDU Q that we need to send remaining data and call the SDU Q back
	// when it's been sent & acknowledged by the peer. Returning True means it's already
	// been done and we can be synchronously deleted.
	virtual TBool DeliverOutgoingDataAndSignalToSduQWhenDone();

	inline CL2CapSDUQueue& SDUQueue() const;
	inline CL2CAPMux& Muxer() const;
	inline TL2CAPPort LocalCID() const;
	inline TL2CAPPort RemoteCID() const;
	inline TL2CapDataControllerConfig& Config() const;

	inline TBool IsBasicDataVersion() const;

protected:
	// Note: leaving functions need to be trapped within this class, all public
	// functions are non-leaving.

	// Dispatches incoming data frames to appropriate frame-type handlers below.
	// Leaves are trapped by HandleIncomingDataFrame, which will close the connection.
	TBool HandleIncomingDataFrameL(RMBufChain& aDataFrame);
	// These need to be overridden by deriving controllers.
	virtual void HandleIncomingIFrameL(RMBufChain& aDataFrame);
	virtual void HandleIncomingBFrameL(RMBufChain& aDataFrame);
	virtual void HandleIncomingSFrameL(RMBufChain& aDataFrame);

	// Needs to be overridden by deriving controllers.
	// Leaves are trapped by GetPdu, which will close the connection. 
	virtual HL2CapPDU* GetPduL();

#ifdef _DEBUG
public:
	TInt GetDataPlaneConfig(TL2DataPlaneConfig& conf) const;
#endif	

public:
	TDblQueLink iLink;	
				
protected:
	CL2CapSDUQueue& iSDUQueue;
	CL2CAPMux& iMuxer;
	TL2CAPPort iLocalCID; 
	TL2CAPPort iRemoteCID;
	TL2CapDataControllerConfig* iConfig;

	TBool iDataPlaneErrored;
	TBool iIsBasicDataVersion;
	};


NONSHARABLE_CLASS(MRetransmissionModeTimerClient)
	{
public:
	virtual void MonitorTimerExpired() = 0;
	virtual void AckTimerExpired() = 0;
	virtual void SendPeerAckTimerExpired() = 0;
	virtual TUint16 MonitorTimeout() = 0;
	virtual TUint16 RetransmissionTimeout() = 0;
	virtual TUint16 PeerRetransmissionTimeout() = 0;
	};

NONSHARABLE_CLASS(RL2CapRetransmissionModeTimerManager)
	{
public:
	RL2CapRetransmissionModeTimerManager(MRetransmissionModeTimerClient& aClient);
	void Close();

	void StartMonitorTimer();
	void StartAckTimer();
	void StopMonitorTimer();
	void StopAckTimer();

	TBool StartSendPeerAckTimer();
	void StopSendPeerAckTimer();

	void HandleFECTimerExpired();
	void HandleSendPeerAckTimerExpired();
	static TInt FECTimerExpired(TAny* aTimerMan);
	static TInt SendPeerAckTimerExpired(TAny* aTimerMan);

	inline TBool IsAckTimerRunning() const;
	inline TBool IsMonitorTimerRunning() const;
	inline TBool IsSendPeerAckTimerRunning() const;

protected:
	MRetransmissionModeTimerClient& iClient;

private:
	void CancelFECTimer();

	// Both of these values are in milliseconds.
	const static TUint16 KAveTimeToTransmitAckToPeer 		= 800;
	const static TUint16 KMinimumPeerAckTimeout				= 800;

	enum TFECTimerState
		{
		EFECTimerIdle,
		EMonitorTimerRunning,
		EAckTimerRunning,
		};

	// Timer for outgoing data.  Used for both Monitor and RTx timers.
	// Note that only one of them can be outstanding at any time (as per the spec).
	TFECTimerState		iFECTimerState;
	TDeltaTimerEntry	iFECTimerEntry;

	// Timer for incoming data.  Send ack. to peer timer.
	TBool				iSendPeerAckTimerRunning;
	TDeltaTimerEntry	iSendPeerAckTimerEntry;
	};


NONSHARABLE_CLASS(CL2CapDataFlowController)
	: public CL2CapBasicDataController, public MPduOwner, public MRetransmissionModeTimerClient
	{
public:
	CL2CapDataFlowController(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig);
	virtual ~CL2CapDataFlowController();
	
	virtual void ProcessFlushTimerExpiry();

	// MPduOwner
	virtual void HandlePduSendComplete(HL2CapPDU& aPdu);
	virtual void HandlePduSendError(HL2CapPDU& aPdu);

	virtual HL2CapPDU* GetPduL();

	virtual TInt HandleOutgoingIFrame(HIFramePDU* aIFrame);
	virtual TInt HandleOutgoingSFrame(HSFramePDU* aSFrame);

	virtual void SetIncomingSduQFull(TBool aIncomingSduQFull);

protected:
	virtual void HandleIncomingIFrameL(RMBufChain& aDataFrame);
	virtual void HandleIncomingSFrameL(RMBufChain& aDataFrame);

	virtual void PDUAvailable();
	TBool CanSendPDU();
	virtual void RemoveAckedPDUsFromSentQueue();
	void ProcessIFrameL(RMBufChain& aDataFrame);

	// MRetransmissionModeTimerClient
	virtual void MonitorTimerExpired();
	virtual void AckTimerExpired();
	virtual void SendPeerAckTimerExpired();
	virtual TUint16 MonitorTimeout();
	virtual TUint16 RetransmissionTimeout();
	virtual TUint16 PeerRetransmissionTimeout();

protected:
	// This value represents the number of frames the peer can still send prior 
	// to the local device sending an ack.  ie, if the peer TxWindow is 20 then
	// (20 - KTxWinAckThresholdOffset) frames can be received before an ack is 
	// sent.  NB.  This does not affect the operation of the SendPeerAckTimer.
	const static TUint8 KTxWinAckThresholdOffset 			= 3;

	// Window related information, as defined in the spec (chapter "Variables and sequence numbers").
	TUint8 iNextTxSeq;
	TUint8 iExpectedAckSeq;
	
	TUint8 iExpectedTxSeq;
	// Stores the acknowledgment number most recently sent to the peer.
	// Used to detect that we're reaching the end of peer's send window and hence should
	// send an ack.
	TUint8 iLastAckSentRxSeqNum;

	// PDUs being sent, not yet completed by HCI.
	TDblQue<HIFramePDU> iPendingSentPDUs;
	// Completed by HCI and awaiting acknowledgement.
	TDblQue<HIFramePDU> iSentPDUs;
	
	RL2CapRetransmissionModeTimerManager iTimerMan;

	TBool iIncomingSduQFull;
	
	TBool iSendAckToPeer;
	TBool iSenderTxWindowClosed;
	};

NONSHARABLE_CLASS(CL2CapDataReTxController) : public CL2CapDataFlowController
	{
public:
	CL2CapDataReTxController(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig);

	// MPduOwner
	virtual void HandlePduSendComplete(HL2CapPDU& aPdu);

	virtual TInt HandleOutgoingIFrame(HIFramePDU* aIFrame);

	virtual TBool DeliverOutgoingDataAndSignalToSduQWhenDone();

protected:
	virtual void HandleIncomingIFrameL(RMBufChain& aDataFrame);
	virtual void HandleIncomingSFrameL(RMBufChain& aDataFrame);

	virtual void PDUAvailable();

	virtual HL2CapPDU* GetPduL();

	virtual void RemoveAckedPDUsFromSentQueue();

	virtual void AckTimerExpired();
private:
	HL2CapPDU* RetransmitSentPDU();

private:
	HSFramePDU* iRejectPDU;
	TBool iRetransmitSentPDUs;
	TBool iRejectSent;
	TBool iRetransmissionDisabled;
	TBool iRestartAckTimer;
	TInt iRetransTxVal;		// This variable is used to store the Tx Seq number of the next PDU to be retransmitted.  This keeps this value separate from the one used for new I-Frames.
	TBool iDeliverOutgoingDataAndSignalToSduQWhenDone;
	};


inline TUint8 L2CapDataUtils::Mod64(TInt aValue)
	{
	return static_cast<TUint8>(aValue & 0x3f);
	}

inline TBool L2CapDataUtils::InWindow(TInt aTxSeq, TInt aStart, TInt aEnd)
	{
	if (aStart <= aEnd)
		{
		return aStart <= aTxSeq && aTxSeq <= aEnd;
		}
	else
		{
		return aStart <= aTxSeq || aTxSeq <= aEnd;
		}
	}


inline CL2CapSDUQueue& CL2CapBasicDataController::SDUQueue() const
	{
	return iSDUQueue;
	}
	
inline CL2CAPMux& CL2CapBasicDataController::Muxer() const
	{
	return iMuxer;
	}
	
inline TL2CAPPort CL2CapBasicDataController::LocalCID() const
	{
	return iLocalCID;
	}
	
inline TL2CAPPort CL2CapBasicDataController::RemoteCID() const
	{
	return iRemoteCID;
	}

inline TL2CapDataControllerConfig& CL2CapBasicDataController::Config() const
	{
	return *iConfig;
	}

inline TBool CL2CapBasicDataController::IsBasicDataVersion() const
	{
	return iIsBasicDataVersion;
	}


inline TBool RL2CapRetransmissionModeTimerManager::IsAckTimerRunning() const
	{
	return iFECTimerState == EAckTimerRunning;
	}

inline TBool RL2CapRetransmissionModeTimerManager::IsMonitorTimerRunning() const
	{
	return iFECTimerState == EMonitorTimerRunning;
	}

inline TBool RL2CapRetransmissionModeTimerManager::IsSendPeerAckTimerRunning() const
	{
	return iSendPeerAckTimerRunning;
	}

#endif
