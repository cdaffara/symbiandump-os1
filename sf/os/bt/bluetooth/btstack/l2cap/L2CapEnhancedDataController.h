// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef L2CAPENHANCEDDATACONTROLLER_H
#define L2CAPENHANCEDDATACONTROLLER_H

#include <e32std.h>
#include <bt_sock.h>

#include "L2CapPDU.h"
#include "L2CapSDUQueue.h"
#include "L2CapDataController.h"
#include "l2signalmgr.h"


NONSHARABLE_CLASS(CL2CapStreamingController) : public CL2CapBasicDataController, public MPduOwner
	{
public:
	CL2CapStreamingController(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig);
	virtual ~CL2CapStreamingController();

	virtual void ProcessFlushTimerExpiry();

	// MPduOwner
	virtual void HandlePduSendComplete(HL2CapPDU& aPdu);
	virtual void HandlePduSendError(HL2CapPDU& aPdu);

protected:
	virtual void HandleIncomingIFrameL(RMBufChain& aIFrame);
	virtual TInt HandleOutgoingIFrame(HIFramePDU* aIFrame);

	virtual HL2CapPDU* GetPduL();

protected:
	// Window related information, as defined in the spec.
	// Sequencer for outgoing I-Frame numbers.
	TUint8 iNextTxSeq;
	// Next expected incoming I-Frame number. 
	TUint8 iExpectedTxSeq;
	};


NONSHARABLE_CLASS(RL2CapErtmTimerManager) : public RL2CapRetransmissionModeTimerManager
	{
public:
	RL2CapErtmTimerManager(class CL2CapEnhancedReTxController& aClient);
	void Close();

	void StartLocalBusyDelayTimer();
	void StopLocalBusyDelayTimer();
	inline TBool IsLocalBusyDelayTimerRunning() const;
private:
	void HandleLocalBusyDelayTimerExpired();
	static TInt LocalBusyDelayTimerExpired(TAny* aTimerMan);
private:
	// Peer Retransmission Timer value is divided by this to obtain the LB delay timer
	// value to use.
	const static TInt KLocalBusyDelayTimerDenominator = 2;
private:
	TBool				iLocalBusyDelayTimerRunning;
	TDeltaTimerEntry	iLocalBusyDelayTimerEntry;
	};


class CL2CapEnhancedReTxController;
class RL2CapErtmOutgoingQueue;

// Encapsulates a list of unacknowledged I-Frames and an index for fast TxSeqs
// lookups for SREJ retransmissions.
NONSHARABLE_CLASS(RL2CapErtmUnacknowledgedIFrames)
	{
public:
	RL2CapErtmUnacknowledgedIFrames();
	void Close();

	inline void Append(HIFramePDU& aFrame);
	inline void Remove(HIFramePDU& aFrame);
	inline HIFramePDU* First() const;
	inline HIFramePDU* Last() const;
	inline HIFramePDU* operator[](TUint8 aTxSeq) const;
	inline TBool IsEmpty() const;
	inline TDblQueIter<HIFramePDU> Iterator();
private:
	// Ordered chronologically, oldest to youngest.
	TDblQue<HIFramePDU>		iFrameList;
	// Indexed by TxSeq.
	TFixedArray<HIFramePDU*, KL2CapTxSeqValues>	iFrameIndex;
	};

NONSHARABLE_CLASS(CL2CapErtmDataTransmitter)
	{
public:
	static CL2CapErtmDataTransmitter* NewL(CL2CapEnhancedReTxController& aController);
	virtual ~CL2CapErtmDataTransmitter();	

	void HandleIncomingIFrame(RMBufChain& aIFrame);
	void HandleIncomingSFrameL(RMBufChain& aSFrame);

	TBool HaveSpaceInOutgoingWindow() const;
	HIFramePDU* GetIFrameToSendL();
 
	void HciCompletedIFrame(HIFramePDU& aIFrame);

	void AckTimerExpired();	

	inline TBool RemoteBusy() const;

	inline TBool IsWaitAckStatePending() const;
	inline void EnterWaitAckState();
	inline TBool InWaitAckState() const;

	inline TUint8 NextTxSeq() const;
	inline TUint8 ExpectedAckSeq() const;

	inline TBool IsReqSeqValid(TUint8 aReqSeq);

	inline TBool HaveUnackedIFrames() const;
	inline TBool IsRetransmittingUnackedIFrames() const;
    inline TBool IsNextUnackedIFrameAwaitingHciCompletion() const;	

private:
	CL2CapErtmDataTransmitter(CL2CapEnhancedReTxController& aController);

	void HandleReqSeqAck(TUint8 aReqSeq);
	void HandleFinalAck();

	void HandleIncomingSRejL(RMBufChain& aSRejFrame);

	void RetransmitUnackedIFrames();

	inline void SendOrPendL(HIFramePDU& aIFrame);

	inline RL2CapRetransmissionModeTimerManager& TimerMan();
	inline RL2CapErtmOutgoingQueue& OutgoingQ();
private:
	CL2CapEnhancedReTxController& iController;

	// Outgoing window information, as defined by the spec.
	// Sequencer for outgoing I-Frame numbers. With the little exception that when
	// a decision to retransmit all unacked I-Frames is made, this gets set to the
	// first number of the bunch and frames are retransmitted until iUnackedIFrames[iNextTxSeq]
	// is NULL again.
	TUint8				iNextTxSeq;
	// I-Frames preceding this number have been acknowledged by the peer.
	TUint8				iExpectedAckSeq;

	RL2CapErtmUnacknowledgedIFrames	iUnackedIFrames;

	// If remote indicates busy condition we keep transmitting within send window limits,
	// but disable our retransmission timer.
	TBool				iRemoteBusy;

	// For protecting against duplicates. Explained in HandleIncomingSFrameL().
	// See state tables in the spec.
	TBool				iSRejActioned;
	TUint8				iSRejSaveReqSeq;

	// Signals to GetPdu that we need to enter WAIT_ACK.
	TBool				iWaitAckStatePending;
	// Xmit state WAIT_ACK (syncing ReqSeq to start retransmission from) - see spec.
	TBool				iInWaitAckState;
	};


class CL2CapErtmDataReceiver;

NONSHARABLE_CLASS(TL2CapErtmMissingTxSeqs)
	{
public:
	TL2CapErtmMissingTxSeqs();

	// Register SREJ-requested I-Frame TxSeq.
	inline void AppendTxSeq(TUint8 aTxSeq);

	// Received an SREJ requested I-Frame. Take note of this fact and indicate whether
	// it was out of sequence wrt to previous ones. If so, GetNextTxSeqForResend() needs
	// to be called in a loop to return all the TxSeqs to re-request.
	TBool ReceivedTxSeq(TUint8 aTxSeq);
	inline TBool GetNextTxSeqForResend(TUint8& aTxSeq);

	TBool IsTxSeqOnTheList(TUint8 aTxSeq) const;
	inline TUint8 LastTxSeq() const;
	inline TBool IsEmpty() const;

	inline TBool AllRequestedFramesReceived() const;
	inline TUint8 ExpectedSRejTxSeq() const;
	inline TInt NumMissingTxSeqs() const;
	inline TBool HaveSpaceForNewTxSeqs(TInt aNumTxSeqs) const;
	inline void Reset();
private:
#ifdef __FLOG_ACTIVE
	void Log();
#endif
public:
	const static TUint KMaxSRejsInFlight = 1;
private:
	TFixedArray<TUint8, KMaxSRejsInFlight>	iMissingTxSeqs;
	// Number of TxSeqs missing since entering SREJ_SENT.
	// The array is split in two parts - frames already received and the ones we're still
	// waiting for.
	TInt		iNumMissingTxSeqs;
	// Frames below this index have been received.
	TInt		iExpectedRecvIdx;
	// Frames starting from this index need to be re-requested.
	TInt		iResendIdx;
	};


NONSHARABLE_CLASS(TL2CapErtmReceiverStateBase)
	{
public:
	// Note: think about the frame parameter as "the frame that caused the transition",
	// though it's only really used for SREJ_SENT. Still, this is the cleanest form
	// of passing the required information.
	virtual void EnterL(RMBufChain* aIFrame);
	virtual void ExitL();
	virtual void HandleIncomingIFrameL(RMBufChain& aIFrame) = 0;
	virtual void HandlePollL() = 0;
	virtual TBool IsLocalBusySupported() const;

	// Called to notify the state that the incoming SDU Q has just consumed some
	// I-Frames and we could slide the receive window if we can (we can't in SREJ_SENT).
	virtual void TxSeqExpectedBySduQChanged(TUint8 aTxSeq) = 0;
protected:
	TL2CapErtmReceiverStateBase(CL2CapErtmDataReceiver& aReceiver);
protected:
	CL2CapErtmDataReceiver& iReceiver;
	};

NONSHARABLE_CLASS(TL2CapErtmReceiverStateRecv) : public TL2CapErtmReceiverStateBase
	{
public:
	TL2CapErtmReceiverStateRecv(CL2CapErtmDataReceiver& aReceiver);
	virtual void EnterL(RMBufChain* aIFrame);
	virtual void HandleIncomingIFrameL(RMBufChain& aIFrame);
	virtual void HandlePollL();
	virtual TBool IsLocalBusySupported() const;
	virtual void TxSeqExpectedBySduQChanged(TUint8 aTxSeq);
	};

NONSHARABLE_CLASS(TL2CapErtmReceiverStateRejSent) : public TL2CapErtmReceiverStateBase
	{
public:
	TL2CapErtmReceiverStateRejSent(CL2CapErtmDataReceiver& aReceiver);
	virtual void EnterL(RMBufChain* aIFrame);
	virtual void HandleIncomingIFrameL(RMBufChain& aIFrame);
	virtual void HandlePollL();
	virtual void TxSeqExpectedBySduQChanged(TUint8 aTxSeq);
	};

NONSHARABLE_CLASS(TL2CapErtmReceiverStateSRejSent) : public TL2CapErtmReceiverStateBase
	{
public:
	TL2CapErtmReceiverStateSRejSent(CL2CapErtmDataReceiver& aReceiver);
	virtual void EnterL(RMBufChain* aIFrame);
	virtual void ExitL();
	virtual void HandleIncomingIFrameL(RMBufChain& aIFrame);
	virtual void HandlePollL();
	virtual void TxSeqExpectedBySduQChanged(TUint8 aTxSeq);
private:
	void SendSRejsUpToReceivedIFrameL(RMBufChain& aIFrame);
private:
	TBool					iGoToRej;
	TL2CapErtmMissingTxSeqs	iMissingTxSeqs;
	};


// Incoming I-Frame queue. Used both when the SDU Q is full and we have to buffer incoming
// frames (a whole window is buffered) and when SREJ_SENT is entered and we wait for
// SREJ-requested retransmissions to come through.
NONSHARABLE_CLASS(RL2CapErtmIncomingIFrameQueue)
	{
public:
	RL2CapErtmIncomingIFrameQueue();
	void Close();

	// Enqueue the new I-Frame and try to consume as much of the existing ones as possible.
	void HandleIncomingIFrameL(RMBufChain& aIFrame, const CL2CapErtmDataReceiver& aReceiver);
	// Pass as many consequent I-Frames to SDU Q ("consume them") as possible.
	void ConsumeUpToFirstGapL(const CL2CapErtmDataReceiver& aReceiver);
	inline void DeleteAllFrames();

	inline TBool IsEmpty() const;
	inline TUint8 TxSeqExpectedBySduQ() const;
private:
	void Insert(RMBufChain& aIFrame);
#ifdef _DEBUG
	void LogQ();
#endif
private:
	RMBufPktQ		iQueue;
	// Next TxSeq in sequence for SDU Q.
	// If we're not in SREJ_SENT, it's = BufferSeq.
	// If we're in SREJ_SENT, it's >= BufferSeq.
	TUint8			iTxSeqExpectedBySduQ;
	};

NONSHARABLE_CLASS(CL2CapErtmDataReceiver)
	{
public:
	static CL2CapErtmDataReceiver* NewL(CL2CapEnhancedReTxController& aController);
	virtual ~CL2CapErtmDataReceiver();

	void HandleIncomingIFrameL(RMBufChain& aIFrame);
	void HandleIncomingSFrameL(RMBufChain& aSFrame);

	// Recv window management.
	void SendPeerAckTimerExpiredL();
	TBool IsEndOfReceiveWindowApproaching() const;
	template<typename FrameType>
	void StampWithReqSeq(FrameType& aFrame);

	// Send SREJ or REJ ?
	TBool IsSRejPreferredToRej(TUint8 aExpectedTxSeq, TUint8 aReceivedTxSeq);

	// Incoming SDU Q status/LocalBusy.
	void SetIncomingSduQFullL(TBool aIncomingSduQFull);
	void UpdateLocalBusyStatusL();
	inline TBool IsIncomingSduQFull() const;
	inline TBool LocalBusy() const;
	void LocalBusyDelayTimerExpired();

	inline void FlushBufferedIncomingIFrames();

	inline TBool IsWaitFStatePending() const;
	inline void EnterWaitFState();
	inline TBool InWaitFState() const;

	// Window state accessors, both for state objects and the controller.
	inline TUint8 ExpectedTxSeq() const;
	inline void SetExpectedTxSeq(TUint8 aTxSeq);
	inline void IncExpectedTxSeq();

	inline TUint8 BufferSeq() const;
	inline void SetBufferSeq(TUint8 aTxSeq);

	inline TUint8 LastAckReqSeq() const;

	inline TUint8 TxSeqExpectedBySduQ() const;

	inline TBool SendAck() const;

	// As defined in the spec.
	inline TBool IsTxSeqUnexpected(TUint8 aTxSeq);
	inline TBool IsTxSeqInvalid(TUint8 aTxSeq);

	// Handy helpers.
	HSFramePDU* GetAckFrameL(TBool aFinal = EFalse);
	inline void PassToIncomingQL(RMBufChain& aIFrame);
	inline void PassToSduQL(RMBufChain& aIFrame) const;

	inline void SetStateRecvL();
	inline void SetStateRejSentL();
	inline void SetStateSRejSentL(RMBufChain& aUnexpectedIFrame);

	inline RL2CapErtmTimerManager& TimerMan();
	inline RL2CapErtmOutgoingQueue& OutgoingQ();
	inline CL2CapEnhancedReTxController& Controller() const;

private:
	CL2CapErtmDataReceiver(CL2CapEnhancedReTxController& aController);
	void ConstructL();

	void SetStateL(TL2CapErtmReceiverStateBase& aState, RMBufChain* aIFrame);

	void HandleFinalAckL();

	void EnterLocalBusyL();
	void ExitLocalBusy();

private:
	// This value represents the number of frames the peer can still send within its current TxWin
	// when we decide to send an acknowledgement. E.g. if the peer TxWindow is 20 then we'll send
	// an acknowledgement after receiving the 18th frame, thus moving the peer's window and ready
	// to receive next 18 frames before sending the next ack. Sending the ack before the end
	// of the receive window is reached prevents transmission delays caused by the peer waiting
	// for an ack before it can send more.
	const static TUint8 KReceiveWinFreeSpaceLeftToTriggerAck = 2;
	// Used to decide whether to send an SREJ or a REJ. An SREJ will never be sent if the gap
	// between ExpectedTxSeq and the received frame's TxSeq is wider than this value.
	const static TUint8 KSRejMissingFrameThreshold	= 1;

	__ASSERT_COMPILE(KSRejMissingFrameThreshold <= TL2CapErtmMissingTxSeqs::KMaxSRejsInFlight);

private:
	CL2CapEnhancedReTxController& iController;

	// Window variables as defined in the spec.
	// Next expected I-Frame num.
	TUint8		iExpectedTxSeq;
	// Ack num (ReqSeq) that can be sent to the peer. Usually equal to ExpectedTxSeq,
	// unless SDU Queue is full or we're in SREJ_SENT. IOW the beginning of our receive window.
	TUint8		iBufferSeq;
	// Stores the acknowledgment number most recently sent to the peer.
	// Used to detect that we're reaching the end of peer's send window and hence should
	// send an ack.
	TUint8		iLastAckReqSeq;

	// This means SDU queue won't accept any more data, so we can't acknowledge anything new.
	// Whether LocalBusy can be entered depends on the receive state (RECV only) and other nasty
	// details.
	TBool		iIncomingSduQFull;
	// This means we're sending out RNRs instead of RRs. Proper LocalBusy.
	TBool		iLocalBusy;
	// Xmit state WAIT_F (exiting Local Busy) - see spec.
	TBool		iInWaitFState;
	// Signals to GetPdu that we need to enter WAIT_F.
	TBool		iWaitFStatePending;

	// This is set to True when we're approaching the end of receive window or the send peer ack
	// timer expires, or sending a generic ack is needed for some other reason.
	// While flags are bad bad bad in general because they don't store time-ordering information
	// of related events (that's why we have the outgoing Q instead), it is fine this case -
	// - it's legitimate to send a vanilla ack at any time. It's an optimization that prevents
	// multiple acks to be queued when the outgoing pipe gets clogged for a while but we keep
	// receiving incoming I-Frames.
	TBool		iSendAck;
	
	RL2CapErtmIncomingIFrameQueue	iIncomingIFrameQ;
	
	// "Heavyweight" State pattern.
	TL2CapErtmReceiverStateBase*	iReceiveState;

	TL2CapErtmReceiverStateRecv		iStateRecv;
	TL2CapErtmReceiverStateRejSent	iStateRejSent;
	TL2CapErtmReceiverStateSRejSent	iStateSRejSent;
	};

// This encapsulates the managament of the outgoing packet queue.
// Every packet should go through the queue before it's sent - certain actions need to be
// executed when a packet is enqueued/dequeued and to make sure that it's always done
// the queue is hidden in this class.
NONSHARABLE_CLASS(RL2CapErtmOutgoingQueue)
	{
public:
	RL2CapErtmOutgoingQueue(CL2CapEnhancedReTxController& aController);
	void Close();

	// These queue frames that can be sent immediately.
	void QueueIFrameL(HIFramePDU& aIFrame);
	void QueueAckingSFrame(HSFramePDU& aSFrame);
	void QueueNonAckingSFrame(HSFramePDU& aSFrame, TUint8 aReqSeq);

	// This queues the I-Frame onto a separate list pending clearing of WAIT_ACK.
	// It's used for I-Frames requested with SREJs coming through in WAIT_ACK.
	void PendRetransmitIFrameL(HIFramePDU& aIFrame);

	// This requeues the I-Frames from the pending list onto the normal send queue.
	void SendPendingRetransmitIFramesL();
	// Used when we decide to retransmit all I-Frames when we still haven't sent
	// out the SREJ-requested ones.
	void CancelPendingRetransmitIFrames();

	void DeleteAllFrames();

	// Called by controller's GetPdu when the link muxer asks for a frame to send.
	HL2CapPDU* DequeueNextToSend();

	inline TBool HaveFramesToTransmit() const;
	inline TBool HavePendingRetransmitIFrames() const;
private:
	CL2CapEnhancedReTxController& iController;
	TSglQue<HL2CapPDU>		iOutgoingQ;
	// I-Frames SREJ-requested by the remote when we're in WAIT_ACK and thus can't send data.
	TSglQue<HIFramePDU>		iPendRetransmitIFrameQ;
	};

NONSHARABLE_CLASS(CL2CapEnhancedReTxController)
	: public CL2CapBasicDataController, public MPduOwner, public MRetransmissionModeTimerClient
	{
public:
	static CL2CapEnhancedReTxController* NewL(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig);
	virtual ~CL2CapEnhancedReTxController();

	// MPduOwner
	// Called back by I-Frames on HCI completion.
	virtual void HandlePduSendComplete(HL2CapPDU& aPdu);
	virtual void HandlePduSendError(HL2CapPDU& aPdu);

	// Called back by PDUs going out through the HL2CapPDU::DeliverOutgoingPDU
	// double-dispatch trickery, in the context of GetPduL.
	virtual TInt HandleOutgoingIFrame(HIFramePDU* aIFrame);
	virtual TInt HandleOutgoingSFrame(HSFramePDU* aSFrame);

	// Called by the SDU Q when it gets full/space frees up.
	virtual void SetIncomingSduQFull(TBool aIncomingSduQFull);

	// Called by the SDU Q when the connection is being shutdown by the application
	// and data needs to be delivered reliably before we disconnect.
	virtual TBool DeliverOutgoingDataAndSignalToSduQWhenDone();
	inline TBool IsOutgoingDataPathClosing();
	// The data transmitter calls this when it wants to notify the SDU Q that it has
	// delivered all outgoing data and so we can now be destroyed.
	inline void SignalOutgoingDataDeliveredToSduQ();

	// Incoming Q calls this to pass an I-Frame to the SDU Q.
	inline void PassToSduQL(RMBufChain& aIFrame);

	// Called from SDU Q when it's got new PDUs to send.
	virtual void OutgoingPduAvailableOnSduQ();

    // Called to evaluate whether we want to send anything.
	inline void NotifyMuxerOfPdusToSendIfHaveSome();

	// Puts the current acknowledgement number into the frame being queued for send.
	// Templated to handle both I-Frames and S-Frames.
	template<typename FrameType>
	inline void StampWithReqSeq(FrameType& aFrame);

	// A helper for when a ReqSeq/Final ack needs to be sent - tries to piggy-back on I-Frames.
	void SendIOrRrOrRnrL(TBool aFinal);

	virtual void LocalBusyDelayTimerExpired();
 
	inline TBool IsPollOutstanding() const;
	inline TBool IsFBitValid(TBool aFinal);

	inline RL2CapErtmOutgoingQueue& OutgoingQ();
	inline RL2CapErtmTimerManager& TimerMan();

protected:
	CL2CapEnhancedReTxController(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig);
	void ConstructL();

	// These are the entry points for incoming PDUs, called and trapped by a dispatch routine
	// in CL2CapBasicDataController.
	virtual void HandleIncomingIFrameL(RMBufChain& aIFrame);
	virtual void HandleIncomingSFrameL(RMBufChain& aSFrame);

	// Provide a PDU for transmission. Called and trapped by GetPdu in CL2CapBasicDataController.
	virtual HL2CapPDU* GetPduL();

	void HandleFinalAck();

	// From MErtmModeTimerClient
	virtual void MonitorTimerExpired();
	virtual void AckTimerExpired();
	virtual void SendPeerAckTimerExpired();
	virtual TUint16 MonitorTimeout();
	virtual TUint16 RetransmissionTimeout();
	virtual TUint16 PeerRetransmissionTimeout();
private:
	TUint8						iPollSFrameTransmitCount;
	TBool						iDeliverOutgoingDataAndSignalToSduQWhenDone;

	RL2CapErtmOutgoingQueue		iOutgoingQ;
	RL2CapErtmTimerManager		iTimerMan;
	CL2CapErtmDataReceiver*		iReceiver;
	CL2CapErtmDataTransmitter*	iTransmitter;
	};

#include "L2CapEnhancedDataController.inl"

#endif /*L2CAPENHANCEDDATACONTROLLER_H*/
