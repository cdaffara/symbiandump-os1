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

#ifndef L2CAPENHANCEDDATACONTROLLER_INL
#define L2CAPENHANCEDDATACONTROLLER_INL


inline TBool RL2CapErtmTimerManager::IsLocalBusyDelayTimerRunning() const
	{
	return iLocalBusyDelayTimerRunning;
	}


inline void RL2CapErtmUnacknowledgedIFrames::Append(HIFramePDU& aFrame)
	{
	__ASSERT_DEBUG(iFrameIndex[aFrame.TxSeqNumber()] == NULL,
				   Panic(EL2CAPTryingToAppendSameTxSeqIFrameTwice));

	iFrameList.AddLast(aFrame);
	iFrameIndex[aFrame.TxSeqNumber()] = &aFrame;
	}

inline void RL2CapErtmUnacknowledgedIFrames::Remove(HIFramePDU& aFrame)
	{
	__ASSERT_DEBUG(iFrameIndex[aFrame.TxSeqNumber()] == &aFrame,
				   Panic(EL2CAPTryingToRemoveUnackedIFrameWithSameTxSeqButDifferentAddress));

	aFrame.iLink.Deque();
	iFrameIndex[aFrame.TxSeqNumber()] = NULL;
	}

inline HIFramePDU* RL2CapErtmUnacknowledgedIFrames::First() const
	{
	return iFrameList.First();
	}

inline HIFramePDU* RL2CapErtmUnacknowledgedIFrames::Last() const
	{
	return iFrameList.Last();
	}

inline HIFramePDU* RL2CapErtmUnacknowledgedIFrames::operator[](TUint8 aTxSeq) const
	{
	return iFrameIndex[aTxSeq];
	}

inline TBool RL2CapErtmUnacknowledgedIFrames::IsEmpty() const
	{
	return iFrameList.IsEmpty();
	}

inline TDblQueIter<HIFramePDU> RL2CapErtmUnacknowledgedIFrames::Iterator()
	{
	return TDblQueIter<HIFramePDU>(iFrameList);
	}


inline TBool CL2CapErtmDataTransmitter::RemoteBusy() const
	{
	return iRemoteBusy;
	}

inline TBool CL2CapErtmDataTransmitter::IsWaitAckStatePending() const
	{
	return iWaitAckStatePending;
	}

inline void CL2CapErtmDataTransmitter::EnterWaitAckState()
	{
	__ASSERT_DEBUG(!iController.IsPollOutstanding(),
				   Panic(EL2CAPAttemptToEnterWaitAckWhenAlreadyInAWaitState));

	iWaitAckStatePending = EFalse;
	iInWaitAckState = ETrue;
	}

inline TBool CL2CapErtmDataTransmitter::InWaitAckState() const
	{
	return iInWaitAckState;
	}

inline void CL2CapErtmDataTransmitter::SendOrPendL(HIFramePDU& aIFrame)
	{
	if (iInWaitAckState)
		{
		iController.OutgoingQ().PendRetransmitIFrameL(aIFrame);
		}
	else
		{
		iController.OutgoingQ().QueueIFrameL(aIFrame);
		}
	}

inline RL2CapRetransmissionModeTimerManager& CL2CapErtmDataTransmitter::TimerMan()
	{
	return iController.TimerMan();
	}

inline RL2CapErtmOutgoingQueue& CL2CapErtmDataTransmitter::OutgoingQ()
	{
	return iController.OutgoingQ();
	}

inline TBool CL2CapErtmDataTransmitter::HaveUnackedIFrames() const
	{
	return !iUnackedIFrames.IsEmpty();
	}

inline TBool CL2CapErtmDataTransmitter::IsRetransmittingUnackedIFrames() const
	{
	return iUnackedIFrames[iNextTxSeq] != NULL;
	}

inline TBool CL2CapErtmDataTransmitter::IsNextUnackedIFrameAwaitingHciCompletion() const
    {
    __ASSERT_DEBUG(IsRetransmittingUnackedIFrames(), Panic(EL2CapReferencingUnackedIFrameWhenNoRetransmissionGoing));
    return iUnackedIFrames[iNextTxSeq]->IsAwaitingHciCompletion();
    }

inline TBool CL2CapErtmDataTransmitter::IsReqSeqValid(TUint8 aReqSeq)
	{
	// Per the spec:
	// With-Valid-ReqSeq: The ReqSeq of the received frame is in the range
	// ExpectedAckSeq <= ReqSeq <= NextTxSeq.
	//
	// We can't just use NextTxSeq though because in our implementation we slip it back when we
	// retransmit all unacked I-Frames, which means it doesn't always point to the next TxSeq to
	// allocate, it's rather the next TxSeq to allocate OR retransmit. The real boundary of the
	// TxSeqs sent out so far in the current window is the youngest I-Frame on the unacknowledged
	// list, so we use that in this check.
	//
	// Note: ideally we should just be able to use NextTxSeq because the protocol is designed
	// so that you only retransmit all unacked I-Frames once you've made sure they haven't been
	// received by the peer, which would mean that we only slip NextTxSeq back having made
	// sure that the peer will not acknowledge anything beyond that value. There is however
	// one loophole in the protocol: when the peer exits LocalBusy and sends us an RR (we need
	// to restart the transmission from the ReqSeq given in that RR) while we keep on sending
	// I-Frames during the RR's travel time. These I-Frames will obviously have newer TxSeqs
	// than the ReqSeq from the RR acknowledges and will thus be retransmitted when we receive
	// the RR. So if the peer didn't ignore the original transmissions that came through
	// when it was in WAIT_F, it will detect the retransmissions as duplicates and (if it's a
	// correct implementation) close the connection. Some unwisely coded remotes however will
	// accept the originals and ignore the duplicates, and in that case we may receive a ReqSeq
	// for the originals while we're retransmitting them, which means it will be a younger
	// seqNum than NextTxSeq.

	const TUint8 lastValidReqSeq = iUnackedIFrames.IsEmpty() ? iNextTxSeq : L2CapDataUtils::Mod64(iUnackedIFrames.Last()->TxSeqNumber() + 1); 
	return L2CapDataUtils::InWindow(aReqSeq, iExpectedAckSeq, lastValidReqSeq);
	}

inline TUint8 CL2CapErtmDataTransmitter::ExpectedAckSeq() const
	{
	return iExpectedAckSeq;
	}

inline TUint8 CL2CapErtmDataTransmitter::NextTxSeq() const
	{
	return iNextTxSeq;
	}


inline void TL2CapErtmMissingTxSeqs::AppendTxSeq(TUint8 aTxSeq)
	{
	__ASSERT_DEBUG(iNumMissingTxSeqs < KMaxSRejsInFlight,
				   Panic(EL2CAPGivenMoreSRejedTxSeqsToTraceThanCanStore));

	iMissingTxSeqs[iNumMissingTxSeqs++] = aTxSeq;
	}

inline TBool TL2CapErtmMissingTxSeqs::GetNextTxSeqForResend(TUint8& aTxSeq)
	{
	__ASSERT_DEBUG(iResendIdx < iNumMissingTxSeqs,
				   Panic(EL2CAPStrayCallToGetTxSeqForResend));

	aTxSeq = iMissingTxSeqs[iResendIdx++];
	return iResendIdx < iNumMissingTxSeqs;
	}

inline TUint8 TL2CapErtmMissingTxSeqs::LastTxSeq() const
	{
	__ASSERT_DEBUG(!IsEmpty(),
				   Panic(EL2CAPLastTxSeqCalledWhenNoneAreExpected));

	return iMissingTxSeqs[iNumMissingTxSeqs - 1];
	}

inline TBool TL2CapErtmMissingTxSeqs::IsEmpty() const
	{
	return iNumMissingTxSeqs == 0;
	}

inline TBool TL2CapErtmMissingTxSeqs::AllRequestedFramesReceived() const
	{
	return iExpectedRecvIdx == iNumMissingTxSeqs;
	}

inline TUint8 TL2CapErtmMissingTxSeqs::ExpectedSRejTxSeq() const
	{
	__ASSERT_DEBUG(!IsEmpty() && !AllRequestedFramesReceived(),
				   Panic(EL2CAPNextExpectedTxSeqCalledWhenNoneAreExpected));
	return iMissingTxSeqs[iExpectedRecvIdx];
	}

inline TInt TL2CapErtmMissingTxSeqs::NumMissingTxSeqs() const
	{
	return iNumMissingTxSeqs;
	}

inline TBool TL2CapErtmMissingTxSeqs::HaveSpaceForNewTxSeqs(TInt aNumTxSeqs) const
	{
	return KMaxSRejsInFlight - iNumMissingTxSeqs >= aNumTxSeqs;
	}


inline void TL2CapErtmMissingTxSeqs::Reset()
	{
	iNumMissingTxSeqs = iExpectedRecvIdx = iResendIdx = 0;
	}


inline TBool RL2CapErtmIncomingIFrameQueue::IsEmpty() const
	{
	return iQueue.IsEmpty();
	}

inline void RL2CapErtmIncomingIFrameQueue::DeleteAllFrames()
	{
	iQueue.Free();
	}

inline TUint8 RL2CapErtmIncomingIFrameQueue::TxSeqExpectedBySduQ() const
	{
	return iTxSeqExpectedBySduQ;
	}

inline void CL2CapErtmDataReceiver::PassToIncomingQL(RMBufChain& aIFrame)
	{
	iIncomingIFrameQ.HandleIncomingIFrameL(aIFrame, *this);
	}

inline void CL2CapErtmDataReceiver::PassToSduQL(RMBufChain& aIFrame) const
	{
	iController.PassToSduQL(aIFrame);
	}

inline TBool CL2CapErtmDataReceiver::IsIncomingSduQFull() const
	{
	return iIncomingSduQFull;
	}

inline TBool CL2CapErtmDataReceiver::LocalBusy() const
	{
	return iLocalBusy;
	}

inline void CL2CapErtmDataReceiver::FlushBufferedIncomingIFrames()
	{
	__ASSERT_DEBUG(iReceiveState == &iStateRecv,
				   Panic(EL2CAPFlushingIncomingIFramesInStateOtherThanRecv));
	iIncomingIFrameQ.DeleteAllFrames();
	iExpectedTxSeq = iIncomingIFrameQ.TxSeqExpectedBySduQ();
	}

inline TBool CL2CapErtmDataReceiver::IsWaitFStatePending() const
	{
	return iWaitFStatePending;
	}

inline void CL2CapErtmDataReceiver::EnterWaitFState()
	{
	__ASSERT_DEBUG(!iController.IsPollOutstanding(),
				   Panic(EL2CAPAttemptToEnterWaitFWhenAlreadyInAWaitState));

	iWaitFStatePending = EFalse;
	iInWaitFState = ETrue;
	}

inline TBool CL2CapErtmDataReceiver::InWaitFState() const
	{
	return iInWaitFState;
	}

inline TBool CL2CapErtmDataReceiver::SendAck() const
	{
	return iSendAck;
	}

inline RL2CapErtmTimerManager& CL2CapErtmDataReceiver::TimerMan()
	{
	return iController.TimerMan();
	}

inline RL2CapErtmOutgoingQueue& CL2CapErtmDataReceiver::OutgoingQ()
	{
	return iController.OutgoingQ();
	}

inline TUint8 CL2CapErtmDataReceiver::ExpectedTxSeq() const
	{
	return iExpectedTxSeq;
	}

inline void CL2CapErtmDataReceiver::SetExpectedTxSeq(TUint8 aTxSeq)
	{
	iExpectedTxSeq = aTxSeq;
	}

inline void CL2CapErtmDataReceiver::IncExpectedTxSeq()
	{
	iExpectedTxSeq = L2CapDataUtils::Mod64(iExpectedTxSeq + 1);
	}

inline TUint8 CL2CapErtmDataReceiver::BufferSeq() const
	{
	return iBufferSeq;
	}

inline void CL2CapErtmDataReceiver::SetBufferSeq(TUint8 aTxSeq)
	{
	iBufferSeq = aTxSeq;
	}

inline TUint8 CL2CapErtmDataReceiver::LastAckReqSeq() const
	{
	return iLastAckReqSeq;
	}

inline TUint8 CL2CapErtmDataReceiver::TxSeqExpectedBySduQ() const
	{
	return iIncomingIFrameQ.TxSeqExpectedBySduQ();
	}

inline void CL2CapErtmDataReceiver::SetStateRecvL()
	{
	SetStateL(iStateRecv, NULL);
	}

inline void CL2CapErtmDataReceiver::SetStateRejSentL()
	{
	SetStateL(iStateRejSent, NULL);
	}

inline void CL2CapErtmDataReceiver::SetStateSRejSentL(RMBufChain& aUnexpectedIFrame)
	{
	SetStateL(iStateSRejSent, &aUnexpectedIFrame);
	}

inline CL2CapEnhancedReTxController& CL2CapErtmDataReceiver::Controller() const
	{
	return iController;
	}

inline TBool CL2CapErtmDataReceiver::IsTxSeqUnexpected(TUint8 aTxSeq)
	{
	// With-unexpected-TxSeq: The TxSeq of the received I-frame is within the
	// TxWindow of the L2CAP entity receiving the I-frame but has a TxSeq "greater"
	// than ExpectedTxSeq where "greater" means later in sequence than ExpectedTxSeq.
	return // in sender's window
		   L2CapDataUtils::InWindow(aTxSeq, iBufferSeq, L2CapDataUtils::Mod64(iBufferSeq + iController.Config().PeerTXWindowSize() - 1)) &&
		   // ... and "greater" than the expected one
		   !L2CapDataUtils::InWindow(aTxSeq, iBufferSeq, iExpectedTxSeq);
	}

inline TBool CL2CapErtmDataReceiver::IsTxSeqInvalid(TUint8 aTxSeq)
	{
	// With-Invalid-TxSeq: The TxSeq of the received I-frame is not within the
	// TxWindow of the L2CAP entity receiving the frame.
	return !L2CapDataUtils::InWindow(aTxSeq, iBufferSeq, L2CapDataUtils::Mod64(iBufferSeq + iController.Config().PeerTXWindowSize() - 1));
	}


inline TBool RL2CapErtmOutgoingQueue::HaveFramesToTransmit() const
	{
	return !iOutgoingQ.IsEmpty();
	}

inline TBool RL2CapErtmOutgoingQueue::HavePendingRetransmitIFrames() const
	{
	return !iPendRetransmitIFrameQ.IsEmpty();
	}


template<typename FrameType>
inline void CL2CapEnhancedReTxController::StampWithReqSeq(FrameType& aFrame)
	{
	iReceiver->StampWithReqSeq(aFrame);
	}

inline RL2CapErtmOutgoingQueue& CL2CapEnhancedReTxController::OutgoingQ()
	{
	return iOutgoingQ;
	}

inline RL2CapErtmTimerManager& CL2CapEnhancedReTxController::TimerMan()
	{
	return iTimerMan;
	}

inline TBool CL2CapEnhancedReTxController::IsOutgoingDataPathClosing()
	{
	return iDeliverOutgoingDataAndSignalToSduQWhenDone;
	}

inline void CL2CapEnhancedReTxController::SignalOutgoingDataDeliveredToSduQ()
	{
	iSDUQueue.DataControllerDeliveredOutgoingData();
	}

inline void CL2CapEnhancedReTxController::PassToSduQL(RMBufChain& aIFrame)
	{
	User::LeaveIfError(iSDUQueue.PutIFramePDU(aIFrame));
	}

inline TBool CL2CapEnhancedReTxController::IsPollOutstanding() const
	{
	return iTransmitter->InWaitAckState() || iReceiver->InWaitFState();
	}

inline TBool CL2CapEnhancedReTxController::IsFBitValid(TBool aFinal)
	{
	// With-Valid-F-bit: The F-bit of a received frame is valid if it is 0 or if it is 1 and
	// a frame sent with P=1 by the local L2CAP entity is unanswered (i.e. the local
	// L2CAP entity send a frame with P=1 and has not yet received a frame with F=1
	// until receiving this one). If the Transmitter state machine is in the WAIT_ACK or
	// WAIT_F states then a frame sent with P=1 is unanswered.
	return (aFinal && IsPollOutstanding()) || !aFinal; 
	}

#endif /*L2CAPENHANCEDDATACONTROLLER_INL*/
