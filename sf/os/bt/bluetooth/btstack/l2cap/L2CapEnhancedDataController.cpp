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

#include <bluetooth/logger.h>
#include "btsockettimer.h"
#include "L2CapEnhancedDataController.h"
#include "L2CapSDUQueue.h"
#include "l2signalmgr.h"
#include "l2util.h"
#include "L2CapDebugControlInterface.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP_DATA_CONTROLLER);
#endif

using namespace L2CapDataUtils;


CL2CapStreamingController::CL2CapStreamingController(TL2CAPPort aLocalCID,
													 TL2CAPPort aRemoteCID,
													 CL2CAPMux& aMuxer,
													 CL2CapSDUQueue& aSDUQueue,
													 TL2CapDataControllerConfig* aConfig)
 :	CL2CapBasicDataController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig, EFalse)
	{
	LOG_FUNC
	}

CL2CapStreamingController::~CL2CapStreamingController()
	{
	LOG_FUNC
	}

void CL2CapStreamingController::ProcessFlushTimerExpiry()
	{
	LOG_FUNC
	__ASSERT_DEBUG(EFalse, Panic(EL2CAPFlushingNotReallySupported));
	}

HL2CapPDU* CL2CapStreamingController::GetPduL()
	{
	LOG_FUNC
	// This is called from the signal manager.
	HL2CapPDU* pduToSend = NULL;

	pduToSend = iSDUQueue.GetPDU();

	if(pduToSend)
		{
		pduToSend->DeliverOutgoingPDU(*this);
		}
	
    if (iSDUQueue.HavePDUToSend())
        {
        iMuxer.PDUAvailable();
        }
	
	return pduToSend;
	}

void CL2CapStreamingController::HandleIncomingIFrameL(RMBufChain& aIFrame)
	{
	LOG_FUNC

	const TUint8 txSeq = HIFramePDU::TxSeqNumber(aIFrame);
	
	if (txSeq != iExpectedTxSeq)
		{
		// SDU currently being assembled is no good now. Need to tell the SDU Q to flush
		// it, otherwise if we lost the right amount of I-Frames, we would end up assembling
		// an FDU (Frankenstein Data Unit) - half SDU number n, half SDU number n+1.
		// Note that FDUs are still possible if 63 frames are lost. There'se no notion of
		// SDU checksumming in the protocol, so that can't be prevented.
		iSDUQueue.FlushCurrentlyAssembledSdu();
		}

	if (iSDUQueue.IsIncomingQueueFull())
		{
		// Make room for new data. From the spec:
		// "If there is no buffer space for the received I-frame an existing I-frame
		// (i.e. the oldest) shall be discarded (flushed) freeing up buffer space for
		// the new I-frame.	The discarded I-frame shall be marked as missing."
		iSDUQueue.FlushOldestIncomingSdu();
		}
	LEAVEIFERRORL(iSDUQueue.PutIFramePDU(aIFrame));

	iExpectedTxSeq = Mod64(txSeq + 1);

	// SDU Q sucks out the content, so we can free the frame whether it's been consumed or dropped.
	aIFrame.Free();
	}

TInt CL2CapStreamingController::HandleOutgoingIFrame(HIFramePDU* aIFrame)
	{
	LOG_FUNC
	aIFrame->SetPDUCID(iRemoteCID);
	aIFrame->SetTxSeqNumber(iNextTxSeq);
	iNextTxSeq = Mod64(iNextTxSeq + 1);

	// The Control field is zero-ed when the frame is created.

	aIFrame->CalculateAndSetFCS();

	return KErrNone;
	}

void CL2CapStreamingController::HandlePduSendComplete(HL2CapPDU& aPdu)
	{
	LOG_FUNC
	delete &aPdu;
	}

void CL2CapStreamingController::HandlePduSendError(HL2CapPDU& aPdu)
	{
	LOG_FUNC
	delete &aPdu;
	}


RL2CapErtmTimerManager::RL2CapErtmTimerManager(CL2CapEnhancedReTxController& aClient)
 :	RL2CapRetransmissionModeTimerManager(aClient),
 	iLocalBusyDelayTimerRunning(EFalse)
	{
	LOG_FUNC
	}

void RL2CapErtmTimerManager::Close()
	{
	LOG_FUNC
	StopLocalBusyDelayTimer();
	RL2CapRetransmissionModeTimerManager::Close();
	}

void RL2CapErtmTimerManager::StartLocalBusyDelayTimer()
	{
	LOG_FUNC
	// Shouldn't happen, but no big deal if it does.
	__ASSERT_DEBUG(!iLocalBusyDelayTimerRunning, Panic(EL2CAPInvalidDataControllerTimerState));

	if (!iLocalBusyDelayTimerRunning)
		{
		TCallBack cb(LocalBusyDelayTimerExpired, this);
		iLocalBusyDelayTimerEntry.Set(cb);
	
		// Set the timeout. The value is in Milliseconds.
		BTSocketTimer::Queue((iClient.PeerRetransmissionTimeout() * 1000) / KLocalBusyDelayTimerDenominator,
							 iLocalBusyDelayTimerEntry);
		iLocalBusyDelayTimerRunning = ETrue;
		}
	}

void RL2CapErtmTimerManager::StopLocalBusyDelayTimer()
	{
	LOG_FUNC
	if (iLocalBusyDelayTimerRunning)
		{
		BTSocketTimer::Remove(iLocalBusyDelayTimerEntry);
		iLocalBusyDelayTimerRunning = EFalse;
		}
	}

void RL2CapErtmTimerManager::HandleLocalBusyDelayTimerExpired()
	{
	LOG_FUNC
	iLocalBusyDelayTimerRunning = EFalse;
	static_cast<CL2CapEnhancedReTxController&>(iClient).LocalBusyDelayTimerExpired();
	}

/*static*/ TInt RL2CapErtmTimerManager::LocalBusyDelayTimerExpired(TAny* aTimerMan)
	{
	LOG_STATIC_FUNC
	RL2CapErtmTimerManager* timerMan = reinterpret_cast<RL2CapErtmTimerManager*>(aTimerMan);
	timerMan->HandleLocalBusyDelayTimerExpired();
	return EFalse;
	}



RL2CapErtmUnacknowledgedIFrames::RL2CapErtmUnacknowledgedIFrames()
 :	iFrameList(_FOFF(HIFramePDU, iLink))
	{
	LOG_FUNC
	iFrameIndex.Reset();
	}

void RL2CapErtmUnacknowledgedIFrames::Close()
	{
	LOG_FUNC

	// Some PDUs may hold a reference to this data controller.
	TDblQueIter<HIFramePDU> unackedIter(iFrameList);
	HIFramePDU* pdu;

	while((pdu = unackedIter++) != NULL)
		{
		Remove(*pdu);
		if (!pdu->IsAwaitingHciCompletion())
			{
			delete pdu;
			}
		else
			{
			// Will delete itself on HCI completion.
			pdu->DeregisterPduOwner();
			}
		}
	}


CL2CapErtmDataTransmitter* CL2CapErtmDataTransmitter::NewL(CL2CapEnhancedReTxController& aController)
	{
	LOG_STATIC_FUNC
	return new (ELeave) CL2CapErtmDataTransmitter(aController);
	}

CL2CapErtmDataTransmitter::~CL2CapErtmDataTransmitter()
	{
	LOG_FUNC
	iUnackedIFrames.Close();
	}

CL2CapErtmDataTransmitter::CL2CapErtmDataTransmitter(CL2CapEnhancedReTxController& aController)
 :	iController(aController),
	iNextTxSeq(0),
	iExpectedAckSeq(0),
	iRemoteBusy(EFalse),
	iSRejActioned(EFalse),
	iSRejSaveReqSeq(0),
	iWaitAckStatePending(EFalse),
	iInWaitAckState(EFalse)
	{
	LOG_FUNC
	}

void CL2CapErtmDataTransmitter::HandleIncomingIFrame(RMBufChain& aIFrame)
	{
	LOG_FUNC
	LOG2(_L("NextTxSeq=%d, ExpectedAckSeq = %d"), iNextTxSeq, iExpectedAckSeq)

	const TUint8 reqSeq = HIFramePDU::ReqSeqNumber(aIFrame);
	const TBool final = HIFramePDU::FinalBit(aIFrame);

	HandleReqSeqAck(reqSeq);
	if (final)
		{
		HandleFinalAck();
		}
	}

void CL2CapErtmDataTransmitter::HandleIncomingSFrameL(RMBufChain& aSFrame)
	{
	LOG_FUNC
	LOG2(_L("NextTxSeq=%d, ExpectedAckSeq = %d"), iNextTxSeq, iExpectedAckSeq)

	const TUint8 reqSeq = HSFramePDU::ReqSeqNumber(aSFrame);
	const TSupervisoryFunction function = HSFramePDU::SupervisoryFunction(aSFrame);
	const TBool final = HSFramePDU::FinalBit(aSFrame);
	const TBool poll = HSFramePDU::PollBit(aSFrame);

	// An S-Frame [SREJ] with the POLL=0 does NOT acknowledge any I-Frames.
	if (!(function == ESelectiveReject && !poll))
		{
		HandleReqSeqAck(reqSeq);
		}
	else // SREJ [P=0]
		{
		// The remote detected packet loss itself, so restart the retransmission timer -
		// - SREJ -> I-Frame -> RR roundtrip time is too long and would almost always
		// cause it to expire if we didn't (most probably unnecessarily).
		if (TimerMan().IsAckTimerRunning())
			{
			TimerMan().StartAckTimer();
			}
		}

	switch (function)
		{
		case EReceiverNotReady:
			iRemoteBusy = ETrue;
			TimerMan().StopAckTimer();
			if (final && iInWaitAckState)
				{
				iInWaitAckState = EFalse;
				// We'll start retransmitting when we get RR from the peer. 
				}
			break;
	
		case EReceiverReady:
			if (final)
				{
				HandleFinalAck();
				}
			// iRemoteBusy cleared at the bottom of the function.
			break;
	
		case EReject:
			// A REJ should be ignored if we're in WAIT_ACK and the Final bit in the
			// frame is 0. We'll receive a Final packet in a while anyway, and that
			// packet is guaranteed to contain the same ReqSeq, because the remote
			// can't acknowledge anything new until we retransmit, and we won't
			// retransmit until we receive the Final ack.
			if (!(iInWaitAckState && !final))
				{
				RetransmitUnackedIFrames();
	
				TimerMan().StopAckTimer();
				// The timer will be started again when the first retransmitted I-Frame
				// is completed.
				}
	
			if (final)
				{
				HandleFinalAck();
				}
			// iRemoteBusy cleared at the bottom of the function.
			break;
			
		case ESelectiveReject:
			HandleIncomingSRejL(aSFrame);
			// iRemoteBusy cleared at the bottom of the function.
			break;
		}

	// Anything other than RNR means remote isn't busy anymore.
	if (iRemoteBusy && function != EReceiverNotReady)
		{
		__ASSERT_DEBUG(!TimerMan().IsAckTimerRunning(), Panic(EL2CAPAckTimerRunningWhenRemoteBusy));
		iRemoteBusy = EFalse;
		if (!iUnackedIFrames.IsEmpty())
			{
			TimerMan().StartAckTimer();
			}
		LOG(_L("Exitted RemoteBusy condition"))
		}
	}

void CL2CapErtmDataTransmitter::HandleIncomingSRejL(RMBufChain& aSRejFrame)
	{
	LOG_FUNC

	const TUint8 reqSeq = HSFramePDU::ReqSeqNumber(aSRejFrame);
	const TSupervisoryFunction function = HSFramePDU::SupervisoryFunction(aSRejFrame);
	const TBool final = HSFramePDU::FinalBit(aSRejFrame);
	const TBool poll = HSFramePDU::PollBit(aSRejFrame);

	HIFramePDU* requestedIFrame = iUnackedIFrames[reqSeq];
	// If the ReqSeq is invalid then it should've been caught earlier.
	__ASSERT_ALWAYS(requestedIFrame != NULL, Panic(EL2CAPSRejReqSeqNotOnUnackedList));

	if (poll)
		{
		// Set the Final bit directly here to make sure it's in the right frame.
		requestedIFrame->SetFinalBit(ETrue);
		SendOrPendL(*requestedIFrame);

		if (iController.IsPollOutstanding())
			{
			iSRejActioned = ETrue;
			iSRejSaveReqSeq = reqSeq;
			// If Poll is set, then Final must be 0, so we don't have to check for it.
			}
		} // Poll = 1
	else
		{
		// If peer is in SREJ_SENT when it receives the Poll, then it needs to respond
		// with an SREJ[F=1]. This SREJ may contain a ReqSeq it has already sent in the
		// previous SREJ[F=0], and we need to guard against this on our side, so that we
		// don't retransmit a packet twice - it could cause the channel to be closed, as
		// the second copy would be outside of peer's receive window and thus invalid.
		TBool duplicateWithFinal = EFalse;

		if (iController.IsPollOutstanding())
			{
			if (final)
				{
				if (iSRejActioned && iSRejSaveReqSeq == reqSeq)
					{
					// Peer has sent us a SREJ F=0 for the same frame before, need to
					// ignore this SREJ.
					duplicateWithFinal = ETrue;
					LOG1(_L("Received a duplicate SREJ with ReqSeq=%d with Final=1, will drop it"),
						 reqSeq)
					}

				if (!iUnackedIFrames.IsEmpty() && !iRemoteBusy)
					{
					TimerMan().StartAckTimer();
					}

				iSRejActioned = EFalse;
				iInWaitAckState = EFalse;
				// Don't go into ERetransmitAllUnackedFrames, even in WAIT_ACK:
				// - an SREJ[P=0,F=1] frame doesn't carry an acking ReqSeq, so we
				//   wouldn't know at which frame we should start the retransmission;
				// - the peer has just SREJed some I-Frames so there's a chance
				//   we're already retransmitting the stuff we timed out on.
				//   If we're not (which is possible if acknowledgments from the
				//   peer have been lost), then we'll need to wait for the
				//   next Retransmission Timeout and then retransmit (unless we
				//   we hit remote being in SREJ_SENT again, rinse-and-repeat then).
				OutgoingQ().SendPendingRetransmitIFramesL();
				}
			else
				{
				iSRejActioned = ETrue;
				iSRejSaveReqSeq = reqSeq;
				}
			} // waiting for final ack

		if (!duplicateWithFinal)
			{
			// Clean the F-bit in case it was set during previous transmission.
			requestedIFrame->SetFinalBit(EFalse);
			SendOrPendL(*requestedIFrame);
			}
		} // Poll bit = 0
	}

void CL2CapErtmDataTransmitter::HciCompletedIFrame(HIFramePDU& aIFrame)
	{
	LOG_FUNC
	if (aIFrame.Acked())
		{
		__ASSERT_DEBUG(iUnackedIFrames[aIFrame.TxSeqNumber()] == NULL,
					   Panic(EL2CAPAckedTxSeqFoundOnUnackedList));

		// This condition will happen extremely rarely or none at all on real hardware.
		// It does happen on the emulator with USB HCTL.
		LOG1(_L("I-Frame %d has been acknowledged before HCI completion"), aIFrame.TxSeqNumber())
		// Frame has been already sent and acked - skip putting it on the Send queue, just delete.
		delete &aIFrame;
		// We'll start the retransmission timer the first time a frame that hasn't been acked is
		// completed (the else branch below). 
		}
	else
		{
		__ASSERT_DEBUG(iUnackedIFrames[aIFrame.TxSeqNumber()] != NULL,
					   Panic(EL2CAPUnackedTxSeqNotFoundOnUnackedList));

		if (!TimerMan().IsAckTimerRunning() && !iRemoteBusy)
			{
			// It's the first frame sent out after a period of silence.
			// ... or it's the first frame of a retransmission.
			// ... or we can has a bug (LOL that's impossible, I coded this myself).
			TimerMan().StartAckTimer();
			}
		}
	}

void CL2CapErtmDataTransmitter::AckTimerExpired()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iUnackedIFrames.IsEmpty(), Panic(EL2CAPAckTimerExpiryWithoutPDUToSupervise));
	// Signal to GetPdu that we need to enter WAIT_ACK.
	iWaitAckStatePending = ETrue;
	iController.NotifyMuxerOfPdusToSendIfHaveSome();
	}

HIFramePDU* CL2CapErtmDataTransmitter::GetIFrameToSendL()
	{
	LOG_FUNC
	HIFramePDU* pduToSend = NULL;

	// Can't send data in WAIT_ACK.
	// We send data in RemoteBusy until the window fills up, but with the ack timer stopped.
	if (!iInWaitAckState)
		{
		if (iUnackedIFrames[iNextTxSeq] != NULL)
			{
			// Rreceived a REJ or Ack timer expired.
			pduToSend = iUnackedIFrames[iNextTxSeq];
			if (pduToSend->IsAwaitingHciCompletion())
				{
				pduToSend = NULL;
				}
			else
				{
				// In case it was set for the previous transmission. 
				pduToSend->SetFinalBit(EFalse);
				}
			}
		else if (HaveSpaceInOutgoingWindow())
			{
			pduToSend = static_cast<HIFramePDU*>(iController.SDUQueue().GetPDU());
			if (pduToSend != NULL)
				{
				pduToSend->SetTxSeqNumber(iNextTxSeq);
				// It's a new frame that hasn't been sent before, so we need to append it to the
				// unacked list. Once put on the list, it should never get delinked from it until
				// acknowledged, even during retransmissions.
				__ASSERT_DEBUG(iUnackedIFrames[iNextTxSeq] == NULL, Panic(EL2CAPNewTxSeqFoundOnUnackedList));
				iUnackedIFrames.Append(*pduToSend);
				}
			}

		if (pduToSend != NULL)
			{
			iNextTxSeq = Mod64(iNextTxSeq + 1);
			}
		} // state != WAIT_ACK (can transmit data)
	return pduToSend;
	}

TBool CL2CapErtmDataTransmitter::HaveSpaceInOutgoingWindow() const
	{
	LOG_FUNC
	TBool canSend = ETrue;
	if (Mod64(iNextTxSeq - iExpectedAckSeq) >= iController.Config().TXWindowSize())
		{
		canSend = EFalse;
		}
	return canSend;
	}

void CL2CapErtmDataTransmitter::HandleFinalAck()
	{
	LOG_FUNC

	if (iInWaitAckState)
		{
		iInWaitAckState = EFalse;
		RetransmitUnackedIFrames();
		}

	// Whether it was WAIT_ACK or WAIT_F, Ack timer was stopped because Monitor timer
	// was running.
	if (!iUnackedIFrames.IsEmpty() && !iRemoteBusy)
		{
		TimerMan().StartAckTimer();
		}
	}

void CL2CapErtmDataTransmitter::HandleReqSeqAck(TUint8 aReqSeq)
	{
	LOG_FUNC

	__ASSERT_DEBUG((aReqSeq == iExpectedAckSeq) || !iUnackedIFrames.IsEmpty(),
				   Panic(EL2CAPUnacknowledgedPdusMissingFromList));

	// See if any I-Frames have been acknowledged by the peer, slide the send window,
	// send new data if window space is now available, restart ack timer.

	if (!iUnackedIFrames.IsEmpty() && aReqSeq != iExpectedAckSeq)
		{
		const TBool wasWindowFull = !HaveSpaceInOutgoingWindow();

		TDblQueIter<HIFramePDU> iter(iUnackedIFrames.Iterator());
		HIFramePDU* pduPtr;
		TBool ackedPDUs = EFalse;

		__ASSERT_DEBUG(iUnackedIFrames.First()->TxSeqNumber() == iExpectedAckSeq,
					   Panic(EL2CAPOldestUnackedPduTxSeqNotMatchingExpectedAck));

		while ((pduPtr = iter++) != NULL)
			{
			if (InWindow(pduPtr->TxSeqNumber(), iExpectedAckSeq, Mod64(aReqSeq - 1)))
				{
				iUnackedIFrames.Remove(*pduPtr);
				pduPtr->SetAcked(ETrue);
				ackedPDUs = ETrue;

				if (!pduPtr->IsAwaitingHciCompletion())
					{
					delete pduPtr;
					}
				else
					{
					LOG1(_L("I-Frame %d has been acknowledged before HCI completion!"),
						 pduPtr->TxSeqNumber())
					// Will delete itself on HCI completion.
					pduPtr->DeregisterPduOwner();
					}
				} // pdu in acked window
			else
				{
				// The queue is sorted chronologically - the head is always the oldest PDU.
				// First PDU outside of the acknowledged window means we can stop the loop.
				break;
				}
			}

		if (iUnackedIFrames.IsEmpty())
			{
			TimerMan().StopAckTimer();
			}
		else if (ackedPDUs && !iRemoteBusy)
			{
			// This will actually REstart the timer.
			TimerMan().StartAckTimer();
			}

		iExpectedAckSeq = aReqSeq;
		LOG1(_L("New ExpectedAckSeq=%d"), aReqSeq)

		if (iController.IsOutgoingDataPathClosing() && iUnackedIFrames.IsEmpty())
			{
			iController.SignalOutgoingDataDeliveredToSduQ();
			}
		}
	}

void CL2CapErtmDataTransmitter::RetransmitUnackedIFrames()
	{
	LOG_FUNC
	LOG1(_L("Setting NextTxSeq to ExpectedAckSeq = %d"), iExpectedAckSeq)
	iNextTxSeq = iExpectedAckSeq;
	// This is important when exiting WAIT_ACK: forget about all I-Frames SREJ-requested
	// by the peer. We'll retransmit them as part of the whole bunch. And we can't transmit
	// the same frame twice, which would happen had we left these hanging around.
	OutgoingQ().CancelPendingRetransmitIFrames();
	}


TL2CapErtmMissingTxSeqs::TL2CapErtmMissingTxSeqs()
 :	iNumMissingTxSeqs(0),
	iExpectedRecvIdx(0),
	iResendIdx(0)
	{}

TBool TL2CapErtmMissingTxSeqs::IsTxSeqOnTheList(TUint8 aTxSeq) const
	{
	TBool found = EFalse;
	for (TInt i = 0; i < iNumMissingTxSeqs && !found; i++)
		{
		if (iMissingTxSeqs[i] == aTxSeq)
			{
			found = ETrue;
			}
		}
	return found;
	}

TBool TL2CapErtmMissingTxSeqs::ReceivedTxSeq(TUint8 aTxSeq)
	{
	LOG_FUNC
	TBool resendNeeded = EFalse;
	if (iMissingTxSeqs[iExpectedRecvIdx] == aTxSeq)
		{
		// This is the only path possible with the default of max. 1 SREJ at a time.
		iExpectedRecvIdx++;
		}
	else
		{
		// Should only fall here with the experimental KMaxSRejsInFlight > 1
		__ASSERT_DEBUG(KMaxSRejsInFlight > 1, Panic(EL2CAPMultiSRejPathHitWhenNotSoConfigured));

		LOG1(_L("SREJ resend path hit, TxSeq=%d"), aTxSeq);
#ifdef __FLOG_ACTIVE
		Log();
#endif
		resendNeeded = ETrue;

		// General idea: reshuffle the array to maintain chronological ordering, in which
		// we expect the requested I-Frames to come through. All I-Frames between iExpectedRecvIdx
		// and the one we're processing now are considered to have been lost and will be
		// re-requested, so we need to move them to the very end of the list.
		//
		// E.g.:
		// 0. Current list is 6 elements (iNumMissingTxSeqs == 6):
		//     32 33 | 20 21 45 46
		//   32 and 33 have been received (iExpectedRecvIdx == 2), we're still waiting for the
		//   rest.
		// 1. 45 comes through, we fall here. We need to move it to right after 33 and increase
		//   iExpectedRecvIdx:
		//     32 33 45 | 20 21 46
		// 2. But the order in the array implied that 20 & 21 were requested before 45 and
		//   haven't come through, so they must have been lost (remote has to respond to SREJs
		//   in order they are sent). Ergo, we need to resend the SREJs for 20 and 21, which in
		//   turn means we're now expecting them after all other frames, so finally:
		//     32 33 45 | 46 20 21

		// Find the index of the received TxSeq.
		TInt receivedIdx = iExpectedRecvIdx;
		while (iMissingTxSeqs[receivedIdx] != aTxSeq && receivedIdx < iNumMissingTxSeqs)
			{
			receivedIdx++;
			}

		const TInt numToResend = receivedIdx - iExpectedRecvIdx;
		LOG2(_L("Last received = %d, num SREJs to resend = %d"),
			 (iExpectedRecvIdx > 0 ? iMissingTxSeqs[iExpectedRecvIdx] : -1), numToResend);

		// To maintain the partitioning of the array we need to move the received
		// TxSeq towards the < iExpectedRecvIdx half, over the to-be-SREJed again TxSeqs.
		// (step 1 in the example)
		while (receivedIdx > iExpectedRecvIdx)
			{
			TUint8 tmp = iMissingTxSeqs[receivedIdx - 1];
			iMissingTxSeqs[receivedIdx - 1] = iMissingTxSeqs[receivedIdx];
			iMissingTxSeqs[receivedIdx] = tmp;
			receivedIdx--;
			}
		iExpectedRecvIdx++;

		// Now move the TxSeqs to be SREJed again onto the end of the list to maintain correct
		// ordering (step 2 in the example).
		// Note: this could be optimized a bit at least theoretically down to O(1) by
		// copying the TxSeqs to re-SREJ out to a temp array, then moving the ones currently
		// at the end, and then copying the ones from the temp array back in at the end.
		// Note 2: this is the first time I've used bubble sort in my carreer. Honest.
		for (TInt i = 0; i < numToResend; i++)
			{
			for (TInt bubble = iExpectedRecvIdx; bubble < iNumMissingTxSeqs - 1; bubble++)
				{
				TUint8 tmp = iMissingTxSeqs[bubble];
				iMissingTxSeqs[bubble] = iMissingTxSeqs[bubble + 1];
				iMissingTxSeqs[bubble + 1] = tmp;
				}
			}

		// Store the index of the beginning of the TxSeqs to re-request, they will be pulled
		// in an outer loop until the index reaches iNumMissingTxSeqs.
		iResendIdx = iNumMissingTxSeqs - numToResend;
#ifdef __FLOG_ACTIVE
		Log();
#endif
		}
	return resendNeeded;
	}

#ifdef __FLOG_ACTIVE
void TL2CapErtmMissingTxSeqs::Log()
	{
	LOG3(_L("ExpectedRecvIdx=%d, ResendIdx=%d, NumMissingTxSeqs=%d"),
		 iExpectedRecvIdx, iResendIdx, iNumMissingTxSeqs);

	TBuf<256> buf;
	for (TInt i = 0; i < iNumMissingTxSeqs; i++)
		{
		buf.AppendFormat(_L("%d "), iMissingTxSeqs[i]);
		}
	LOG1(_L("%S"), &buf)
	}
#endif


TL2CapErtmReceiverStateBase::TL2CapErtmReceiverStateBase(CL2CapErtmDataReceiver& aReceiver)
 : iReceiver(aReceiver)
	{
	LOG_FUNC
	}

void TL2CapErtmReceiverStateBase::EnterL(RMBufChain* /*aIFrame*/)
	{
	LOG_FUNC
	}

void TL2CapErtmReceiverStateBase::ExitL()
	{
	LOG_FUNC
	}

TBool TL2CapErtmReceiverStateBase::IsLocalBusySupported() const
	{
	LOG_FUNC
	return EFalse;
	}


TL2CapErtmReceiverStateRecv::TL2CapErtmReceiverStateRecv(CL2CapErtmDataReceiver& aReceiver)
 : TL2CapErtmReceiverStateBase(aReceiver)
	{
	LOG_FUNC
	}

void TL2CapErtmReceiverStateRecv::EnterL(RMBufChain* /*aIFrame*/)
	{
	LOG_FUNC

	if (iReceiver.IsIncomingSduQFull())
		{
		// We may have transitioned from REJ_SENT or SREJ_SENT, which don't allow LocalBusy.
		// We can enter it now.
		iReceiver.TimerMan().StopLocalBusyDelayTimer();
		iReceiver.UpdateLocalBusyStatusL();
		}

	__ASSERT_DEBUG((iReceiver.IsIncomingSduQFull() && iReceiver.LocalBusy()) ||
				   (!iReceiver.IsIncomingSduQFull() && !iReceiver.LocalBusy()) ||
				   (iReceiver.IsIncomingSduQFull() && iReceiver.TimerMan().IsLocalBusyDelayTimerRunning()) ||
				   (!iReceiver.IsIncomingSduQFull() && !iReceiver.TimerMan().IsLocalBusyDelayTimerRunning()),
				   Panic(EL2CapSduQAndLocalBusyStateInconsistent));
	}

void TL2CapErtmReceiverStateRecv::HandleIncomingIFrameL(RMBufChain& aIFrame)
	{
	LOG_FUNC
	__ASSERT_DEBUG((iReceiver.IsIncomingSduQFull() && iReceiver.LocalBusy()) ||
				   (!iReceiver.IsIncomingSduQFull() && !iReceiver.LocalBusy()) ||
				   (iReceiver.IsIncomingSduQFull() && iReceiver.TimerMan().IsLocalBusyDelayTimerRunning()) ||
				   (!iReceiver.IsIncomingSduQFull() && !iReceiver.TimerMan().IsLocalBusyDelayTimerRunning()),
				   Panic(EL2CapSduQAndLocalBusyStateInconsistent));

	__ASSERT_DEBUG(!iReceiver.IsIncomingSduQFull() ||
				   InWindow(iReceiver.TxSeqExpectedBySduQ(), iReceiver.BufferSeq(), iReceiver.ExpectedTxSeq()),
				   Panic(EL2CAPTxSeqExpectedBySduQNotWithinBufferSeqAndExpectedTxSeq));

	__ASSERT_DEBUG(iReceiver.IsIncomingSduQFull() ||
				   (iReceiver.ExpectedTxSeq() == iReceiver.BufferSeq() &&
					iReceiver.BufferSeq() == iReceiver.TxSeqExpectedBySduQ()),
				   Panic(EL2CAPExpectedTxSeqAndBufferSeqAndTxSeqExpectedBySduQNotEqualWhenIncomingQEmptyAndInRecvState));

	const TUint8 txSeq = HIFramePDU::TxSeqNumber(aIFrame);

	TBool freeFrame = ETrue;

	if (iReceiver.ExpectedTxSeq() == txSeq)
		// With-Expected-TxSeq
		{
		iReceiver.IncExpectedTxSeq();
		if (!iReceiver.IsIncomingSduQFull())
			{
			iReceiver.SetBufferSeq(iReceiver.ExpectedTxSeq());
			}
		iReceiver.PassToIncomingQL(aIFrame);
		freeFrame = EFalse;
		}
	else if (!iReceiver.LocalBusy())
		{
		if (iReceiver.IsTxSeqUnexpected(txSeq))
			// With-Unexpected-TxSeq
			{
			if (iReceiver.IsSRejPreferredToRej(iReceiver.ExpectedTxSeq(), txSeq))
				{
				iReceiver.SetStateSRejSentL(aIFrame);
				freeFrame = EFalse;
				}
			else
				{
				if (iReceiver.BufferSeq() == iReceiver.ExpectedTxSeq())
					{
					iReceiver.SetStateRejSentL();
					}
				else
					{
					// BufferSeq != ExpectedTxSeq only when SDU Q is full or in SREJ_SENT.
					__ASSERT_DEBUG(iReceiver.IsIncomingSduQFull(),
								   Panic(EL2CAPBufferSeqNotEqualToExpectedTxSeqWhenInRecvAndSduQNotFull));

					__ASSERT_DEBUG(iReceiver.TimerMan().IsLocalBusyDelayTimerRunning(),
								   Panic(EL2CAPLocalBusyDelayTimerNotRunningWhenSduQFullButNotInLB));

					iReceiver.TimerMan().StopLocalBusyDelayTimer();

					iReceiver.UpdateLocalBusyStatusL();
					}
				}
			} // Unexpected TxSeq
			// else must be duplicate (With-Duplicate-TxSeq), in which case we just drop it
		} // !LocalBusy
	// LocalBusy and !ExpectedTxSeq - drop it - the remote will initiate a retransmission
	// when we exit LocalBusy anyway.

	if (freeFrame)
		{
		aIFrame.Free();
		}
	}

void TL2CapErtmReceiverStateRecv::HandlePollL()
	{
	LOG_FUNC
	if (iReceiver.IsIncomingSduQFull())
		{
		// If the Poll is a result of the peer's retransmission timer expiry, then the peer will
		// start retransmitting I-Frames from the sequence number given here by us (our current
		// BufferSeq). Basically, the point of a Poll in WAIT_ACK is the peer asking "what have
		// you received, need to know where to start retransmitting from", and we're supposed
		// to be honest.
		// Otherwise, if our upper layer Read()s and we process buffered I-Frames between now
		// and the moment the first retransmitted packet comes through, we will have moved
		// BufferSeq and render that packet invalid (a duplicate effectively). Therefore we
		// need to drop the buffered I-Frames to prevent duplicates and guarantee that BufferSeq
		// won't move.
		iReceiver.FlushBufferedIncomingIFrames();

		// Incoming I-Frame buffer empty and state = RECV:
		__ASSERT_DEBUG(// no buffered I-Frames
					   iReceiver.ExpectedTxSeq() == iReceiver.BufferSeq() &&
					   // most recent ReqSeq
					   iReceiver.BufferSeq() == iReceiver.TxSeqExpectedBySduQ(),
					   Panic(EL2CAPExpectedTxSeqAndBufferSeqAndTxSeqExpectedBySduQNotEqualWhenIncomingQEmptyAndInRecvState));
		}
	// Now respond with a data frame or an ack frame with the Final bit set.
	iReceiver.Controller().SendIOrRrOrRnrL(ETrue);
	}

TBool TL2CapErtmReceiverStateRecv::IsLocalBusySupported() const
	{
	LOG_FUNC
	return ETrue;
	}

void TL2CapErtmReceiverStateRecv::TxSeqExpectedBySduQChanged(TUint8 aTxSeq)
	{
	LOG_FUNC
	LOG1(_L("Slipping BufferSeq to %d"), aTxSeq)
	iReceiver.SetBufferSeq(aTxSeq);
	__ASSERT_DEBUG(iReceiver.IsIncomingSduQFull() || iReceiver.BufferSeq() == iReceiver.ExpectedTxSeq(),
				   Panic(EL2CAPBufferSeqNotEqualToExpectedTxSeqWhenInRecvAndSduQNotFull));
	}


TL2CapErtmReceiverStateRejSent::TL2CapErtmReceiverStateRejSent(CL2CapErtmDataReceiver& aReceiver)
 :	TL2CapErtmReceiverStateBase(aReceiver)
	{
	LOG_FUNC
	}

void TL2CapErtmReceiverStateRejSent::EnterL(RMBufChain* /*aIFrame*/)
	{
	LOG_FUNC

	// LocalBusy is only allowed in RECV.
	__ASSERT_DEBUG(!iReceiver.LocalBusy(), Panic(EL2CAPInRejSentAndLocalBusy));

	// Otherwise by sending the REJ we'd acknowledge frames between BufferSeq and ExpectedTxSeq
	// (and if BufferSeq != ExpectedTxSeq and we're not in SREJ_SENT then incoming SDU Q is full
	// and we can not acknowledge anything new).
	__ASSERT_DEBUG(iReceiver.BufferSeq() == iReceiver.ExpectedTxSeq(),
				   Panic(EL2CAPRejSentEnteredWithBufferedFrames));
	
	iReceiver.Controller().OutgoingQ().QueueNonAckingSFrame(*HSFramePDU::NewL(EReject), iReceiver.ExpectedTxSeq());
	}

void TL2CapErtmReceiverStateRejSent::HandleIncomingIFrameL(RMBufChain& aIFrame)
	{
	LOG_FUNC

	__ASSERT_DEBUG(iReceiver.TxSeqExpectedBySduQ() == iReceiver.BufferSeq(),
				   Panic(EL2CAPNextTxSeqExpectedBySduQNotEqualToBufferSeqWhenNotInSRejSent));

	__ASSERT_DEBUG(InWindow(iReceiver.TxSeqExpectedBySduQ(), iReceiver.BufferSeq(), iReceiver.ExpectedTxSeq()),
				   Panic(EL2CAPTxSeqExpectedBySduQNotWithinBufferSeqAndExpectedTxSeq));

	__ASSERT_DEBUG(!iReceiver.LocalBusy(), Panic(EL2CAPInRejSentAndLocalBusy));

	const TUint8 txSeq = HIFramePDU::TxSeqNumber(aIFrame);

	if (iReceiver.ExpectedTxSeq() == txSeq)
		// With-Expected-TxSeq
		{
		iReceiver.IncExpectedTxSeq();
		if (!iReceiver.IsIncomingSduQFull())
			{
			iReceiver.SetBufferSeq(iReceiver.ExpectedTxSeq());
			}

		// Received the missing frame, clear REJ exception condition.
		iReceiver.SetStateRecvL();

		iReceiver.PassToIncomingQL(aIFrame);
		}
	else
		{
		// else it's a duplicate or unexpected frame:
		// - drop the duplicate as usual,
		// - drop the unexpected frame as we're already in a REJ exception condition
		//   and aren't allowed to send a second REJ until the first one is cleared.
		aIFrame.Free();
		}
	}

void TL2CapErtmReceiverStateRejSent::HandlePollL()
	{
	LOG_FUNC
	// General-case Poll handling: simply respond with a data frame or an ack frame with
	// the Final bit set.
	iReceiver.Controller().SendIOrRrOrRnrL(ETrue);
	}

void TL2CapErtmReceiverStateRejSent::TxSeqExpectedBySduQChanged(TUint8 aTxSeq)
	{
	LOG_FUNC
	LOG1(_L("NextConsumedTxSeq moved: slipping BufferSeq to %d"), aTxSeq)
	iReceiver.SetBufferSeq(aTxSeq);
	__ASSERT_DEBUG(iReceiver.IsIncomingSduQFull() || iReceiver.BufferSeq() == iReceiver.ExpectedTxSeq(),
				   Panic(EL2CAPBufferSeqNotEqualToExpectedTxSeqWhenInRecvAndSduQNotFull));
	}


TL2CapErtmReceiverStateSRejSent::TL2CapErtmReceiverStateSRejSent(CL2CapErtmDataReceiver& aReceiver)
 :	TL2CapErtmReceiverStateBase(aReceiver),
 	iGoToRej(EFalse)
	{
	LOG_FUNC
	}

void TL2CapErtmReceiverStateSRejSent::EnterL(RMBufChain* aIFrame)
	{
	LOG_FUNC

	__ASSERT_DEBUG(!iReceiver.LocalBusy(), Panic(EL2CAPInSRejSentAndLocalBusy));
	__ASSERT_DEBUG(iMissingTxSeqs.IsEmpty(), Panic(EL2CAPNormalReceiveStateWhenMissingSRejFramesListIsNotEmpty));
	__ASSERT_DEBUG(aIFrame != NULL, Panic(EL2CAPDataFrameNotPassedOnEntryToSRejSent));

	iGoToRej = EFalse;
	SendSRejsUpToReceivedIFrameL(*aIFrame);
	}

void TL2CapErtmReceiverStateSRejSent::ExitL()
	{
	LOG_FUNC
	TL2CapErtmReceiverStateBase::ExitL();
	
	iMissingTxSeqs.Reset();

	// BufferSeq was frozen once SREJ_SENT was entered. Now we can move it forward.
	if (iReceiver.IsIncomingSduQFull())
		{
		// If incoming SDU Q filled up during SREJ_SENT, then we can only move BufferSeq up to
		// the point at which that happened, not ExpectedTxSeq.
		iReceiver.SetBufferSeq(iReceiver.TxSeqExpectedBySduQ());
		}
	else
		{
		// Bring BufferSeq back in sync with ExpectedTxSeq.
		iReceiver.SetBufferSeq(iReceiver.ExpectedTxSeq());
		}
	LOG2(_L("On exit from SREJ_SENT[IsIncomingSduQFull = %d]: slipped BufferSeq to %d"),
		 iReceiver.IsIncomingSduQFull(), iReceiver.BufferSeq())
	}

void TL2CapErtmReceiverStateSRejSent::HandleIncomingIFrameL(RMBufChain& aIFrame)
	{
	LOG_FUNC

	__ASSERT_DEBUG(InWindow(iReceiver.TxSeqExpectedBySduQ(), iReceiver.BufferSeq(), iReceiver.ExpectedTxSeq()),
				   Panic(EL2CAPTxSeqExpectedBySduQNotWithinBufferSeqAndExpectedTxSeq));

	__ASSERT_DEBUG(!iReceiver.LocalBusy(), Panic(EL2CAPInSRejSentAndLocalBusy));

	const TUint8 txSeq = HIFramePDU::TxSeqNumber(aIFrame);

	TBool freeFrame = ETrue;
	
	if (iMissingTxSeqs.ExpectedSRejTxSeq() == txSeq)
		// With-Expected-TxSeq-Srej
		{
		LOG1(_L("With-Expected-TxSeq-Srej: TxSeq=%d"), txSeq)
		iReceiver.PassToIncomingQL(aIFrame);
		freeFrame = EFalse;

		TBool resendPreviousSRej = iMissingTxSeqs.ReceivedTxSeq(txSeq);
		__ASSERT_ALWAYS(!resendPreviousSRej, Panic(EL2CAPHaveSRejsToResendEvenThoughReceivedFirstInOrder));

		if (iMissingTxSeqs.AllRequestedFramesReceived())
			{
			if (iGoToRej)
				{
				if (iReceiver.IsIncomingSduQFull())
					{
					// We couldn't have entered LocalBusy in SREJ_SENT, but now we can transition
					// to RECV with LocalBusy instead of REJ_SENT. The retransmission will be
					// started when we exit LocalBusy anyway.
					iReceiver.SetStateRecvL();
					// We rely on this...
					__ASSERT_DEBUG(iReceiver.LocalBusy(),
								   Panic(EL2CAPNotInLocalBusyAfterTransitionToRecvFromSrejSentWithSduQFull));
					}
				else
					{
					iReceiver.SetStateRejSentL();
					}
				}
			else // !iGoToRej
				{
				iReceiver.SetStateRecvL();
				}
			} // all SREJed frames received
		} // With-Expected-TxSeq-Srej
	else
		{
		if (iReceiver.ExpectedTxSeq() == txSeq)
			{
			// With-Expected-TxSeq
			if (!iGoToRej)
				{
				LOG1(_L("With-Expected-TxSeq: TxSeq=%d"), txSeq)
				iReceiver.IncExpectedTxSeq();
				iReceiver.PassToIncomingQL(aIFrame);
				freeFrame = EFalse;
				}
			}
		else if (InWindow(txSeq, iReceiver.BufferSeq(), iReceiver.ExpectedTxSeq()))
			{
			if (iMissingTxSeqs.IsTxSeqOnTheList(txSeq))
				// With-Unexpected-TxSeq-Srej
				{
				__ASSERT_DEBUG(TL2CapErtmMissingTxSeqs::KMaxSRejsInFlight > 1,
							   Panic(EL2CAPMultiSRejPathHitWhenNotSoConfigured));

				// Multiple SRej requests have been made and some of the retransmitted I-Frames
				// were lost again. Send new SREJs for those frames.

				__ASSERT_DEBUG(!iMissingTxSeqs.IsEmpty(), Panic(EL2CAPOldestSRejedFrameNotOnMissingList));
				LOG2(_L("With-Unexpected-TxSeq-Srej: TxSeq=%d. Expected was=%d"),
					 txSeq, iMissingTxSeqs.ExpectedSRejTxSeq())

				TBool resendSRej = iMissingTxSeqs.ReceivedTxSeq(txSeq);
				while (resendSRej)
					{
					TUint8 resendTxSeq;
					resendSRej = iMissingTxSeqs.GetNextTxSeqForResend(resendTxSeq);
					iReceiver.Controller().OutgoingQ().QueueNonAckingSFrame(*HSFramePDU::NewL(ESelectiveReject), resendTxSeq);
					LOG1(_L("Resending SREJ for TxSeq=%d"), resendTxSeq)
					}
				iReceiver.PassToIncomingQL(aIFrame);
				freeFrame = EFalse;
				}
			else // else must be a duplicate: With-duplicate-TxSeq-Srej
				{
				LOG1(_L("With-duplicate-TxSeq-Srej: TxSeq=%d"), txSeq)
				}
			}
		else if (!iGoToRej) // With-Unexpected-TxSeq
			{
			LOG1(_L("With-Unexpected-TxSeq: TxSeq=%d."), txSeq)
			if (iReceiver.IsSRejPreferredToRej(iReceiver.ExpectedTxSeq(), txSeq) &&
				iMissingTxSeqs.HaveSpaceForNewTxSeqs(Mod64(txSeq - iReceiver.ExpectedTxSeq())))
				{
				__ASSERT_DEBUG(TL2CapErtmMissingTxSeqs::KMaxSRejsInFlight > 1,
							   Panic(EL2CAPMultiSRejPathHitWhenNotSoConfigured));
				SendSRejsUpToReceivedIFrameL(aIFrame);
				freeFrame = EFalse;
				}
			else
				{
				// Don't send more SREJs. Wait for I-Frames requested so far to be
				// retransmitted and go to REJ mode.
				iGoToRej = ETrue;
				LOG1(_L("NumMissingTxSeqs=%d, GoToRej=1"), iMissingTxSeqs.NumMissingTxSeqs())
				}
			}
		} // !With-Expected-TxSeq-Srej

	if (freeFrame)
		{
		aIFrame.Free();
		}
	}

void TL2CapErtmReceiverStateSRejSent::HandlePollL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iMissingTxSeqs.IsEmpty(), Panic(EL2CAPCaughtInSRejSentWithNoMissingTxSeqs));

	HSFramePDU* frame = HSFramePDU::NewL(ESelectiveReject);
	frame->SetFinalBit(ETrue);
	iReceiver.OutgoingQ().QueueNonAckingSFrame(*frame, iMissingTxSeqs.LastTxSeq());
	}

void TL2CapErtmReceiverStateSRejSent::TxSeqExpectedBySduQChanged(TUint8 /*aTxSeq*/)
	{
	LOG_FUNC
	// Can't slip BufferSeq until SREJ_SENT is left.
	}

void TL2CapErtmReceiverStateSRejSent::SendSRejsUpToReceivedIFrameL(RMBufChain& aIFrame)
	{
	LOG_FUNC

	const TUint8 receivedTxSeq = HIFramePDU::TxSeqNumber(aIFrame);

	for (TUint8 sRejSeq = iReceiver.ExpectedTxSeq(); sRejSeq != receivedTxSeq; sRejSeq = Mod64(sRejSeq + 1))
		{
		iMissingTxSeqs.AppendTxSeq(sRejSeq);
		iReceiver.OutgoingQ().QueueNonAckingSFrame(*HSFramePDU::NewL(ESelectiveReject), sRejSeq);
		}

	iReceiver.SetExpectedTxSeq(Mod64(receivedTxSeq + 1));
	iReceiver.PassToIncomingQL(aIFrame);

	LOG3(_L("ExpectedTxSeq = %d, First Expected SREJed TxSeq = %d, Last Expected SREJed TxSeq = %d"),
		 iReceiver.ExpectedTxSeq(), iMissingTxSeqs.ExpectedSRejTxSeq(), iMissingTxSeqs.LastTxSeq())
	}

RL2CapErtmIncomingIFrameQueue::RL2CapErtmIncomingIFrameQueue()
 : iTxSeqExpectedBySduQ(0)
	{
	LOG_FUNC
	}

void RL2CapErtmIncomingIFrameQueue::Close()
	{
	LOG_FUNC
	if (!iQueue.IsEmpty())
		{
		LOG(_L("Incoming I-Frame queue not empty, freeing mbufs."))
		iQueue.Free();
		}
	}


void RL2CapErtmIncomingIFrameQueue::HandleIncomingIFrameL(RMBufChain& aIFrame, const CL2CapErtmDataReceiver& aReceiver)
	{
	LOG_FUNC
	Insert(aIFrame);
	if (!aReceiver.IsIncomingSduQFull())
		{
		ConsumeUpToFirstGapL(aReceiver);
		}
	}

void RL2CapErtmIncomingIFrameQueue::Insert(RMBufChain& aIFrame)
	{
	LOG_FUNC
	const TUint8 txSeq = HIFramePDU::TxSeqNumber(aIFrame);
	LOG2(_L("TxSeq=%d, NextConsumedTxSeq=%d"), txSeq, iTxSeqExpectedBySduQ)

	if (iQueue.IsEmpty() || !InWindow(txSeq, iTxSeqExpectedBySduQ, HIFramePDU::TxSeqNumber(iQueue.Last())))
		// Fast path for the most common cases.
		{
		__ASSERT_DEBUG(iQueue.IsEmpty() || txSeq != HIFramePDU::TxSeqNumber(iQueue.Last()),
					   Panic(EL2CapDuplicateIFramePassedToIncomingQ));
		iQueue.Append(aIFrame);
		LOG(_L("Appending on fast path"))
		}
	else
		// Generic insertion algorithm.
		// Depends on there not being duplicate TxSeqs on the queue, i.e. the queue mustn't contain
		// acknowledged frames. It can only contain frames within the current receive window.
		{
		TBool inserted = EFalse;
		for (TMBufPktQIter iter(iQueue); !inserted && iter.More(); iter++)
			{
			TUint8 currentTxSeq = HIFramePDU::TxSeqNumber(iter.Current());
			__ASSERT_DEBUG(txSeq != currentTxSeq, Panic(EL2CapDuplicateIFramePassedToIncomingQ));

			if (InWindow(txSeq, iTxSeqExpectedBySduQ, currentTxSeq))
				{
				LOG1(_L("Inserting before %d"), currentTxSeq)
				iter.Insert(aIFrame);
				inserted = ETrue;
				}
			}
		if (!inserted)
			{
			// It's actually impossible to get here with the fast path in the beginning
			// of the function, but it's here for testing of the loop. Might change to
			// __ASSERT_DEBUG when the testing dust settles.
			LOG(_L("Gone through the Q, appending"))
			iQueue.Append(aIFrame);
			}
		}
#ifdef _DEBUG
	LogQ();
#endif
	}

#ifdef _DEBUG
void RL2CapErtmIncomingIFrameQueue::LogQ()
	{
	LOG_FUNC
	TMBufPktQIter i(iQueue);
	TBuf<512> buf;
	while (i.More())
		{
		buf.AppendFormat(_L("%d "), HIFramePDU::TxSeqNumber(i++));
		}
	LOG1(_L("Q: %S"), &buf)
	}
#endif

void RL2CapErtmIncomingIFrameQueue::ConsumeUpToFirstGapL(const CL2CapErtmDataReceiver& aReceiver)
	{
	LOG_FUNC
	while (!iQueue.IsEmpty() && iTxSeqExpectedBySduQ == HIFramePDU::TxSeqNumber(iQueue.First())
		   && !aReceiver.IsIncomingSduQFull())
		{
		RMBufChain frame;
		//Check on return value of iQueue.Remove() is unnecessary, since we've checked iQueue.IsEmpty()
		static_cast<void>(iQueue.Remove(frame));
		iTxSeqExpectedBySduQ = Mod64(iTxSeqExpectedBySduQ + 1);
		// This feeds the frame to the SDU Queue. This is also the place where we get notified
		// if the queue gets full. If that happens, SetIncomingSduQFull is synchronously
		// called by SDU Q here in the same stack frame - hence the guard in the loop condition.
		aReceiver.PassToSduQL(frame);
		}
	LOG3(_L("On return from ConsumeUpToFirstGap: Queue empty=%d, NextConsumedTxSeq=%d, IsIncomingSduQFull=%d"),
		 iQueue.IsEmpty(), iTxSeqExpectedBySduQ, aReceiver.IsIncomingSduQFull())

#ifdef _DEBUG
	LogQ();
#endif
	}


CL2CapErtmDataReceiver* CL2CapErtmDataReceiver::NewL(CL2CapEnhancedReTxController& aController)
	{
	LOG_STATIC_FUNC
	CL2CapErtmDataReceiver* receiver = new (ELeave) CL2CapErtmDataReceiver(aController);
	CleanupStack::PushL(receiver);
	receiver->ConstructL();
	CleanupStack::Pop(receiver);
	return receiver;
	}

void CL2CapErtmDataReceiver::ConstructL()
	{
	LOG_FUNC
	iReceiveState = &iStateRecv;
	iReceiveState->EnterL(NULL);
	}

CL2CapErtmDataReceiver::CL2CapErtmDataReceiver(CL2CapEnhancedReTxController& aController)
 :	iController(aController),
	iExpectedTxSeq(0),
	iBufferSeq(0),
	iLastAckReqSeq(0),
	iIncomingSduQFull(EFalse),
	iLocalBusy(EFalse),
	iInWaitFState(EFalse),
	iWaitFStatePending(EFalse),
	iSendAck(EFalse),
	iStateRecv(*this),
	iStateRejSent(*this),
	iStateSRejSent(*this)
	{
	LOG_FUNC
	}

CL2CapErtmDataReceiver::~CL2CapErtmDataReceiver()
	{
	LOG_FUNC
	iIncomingIFrameQ.Close();
	}

void CL2CapErtmDataReceiver::HandleIncomingIFrameL(RMBufChain& aIFrame)
	{
	LOG_FUNC
	LOG3(_L("ExpectedTxSeq=%d, BufferSeq=%d, LastAckReqSeq=%d"),
		 iExpectedTxSeq, iBufferSeq, iLastAckReqSeq)

	if (HIFramePDU::FinalBit(aIFrame))
		{
		// It's important that this is executed first, we wouldn't like to drop an I-Frame that
		// carries the Final Ack if we're in WAIT_F. If the remote has set F=1 then it must have
		// received the Poll and is therefore aware of the ReqSeq we've sent in RR[P=1].
		// So if the remote is a correct implementation, then this I-Frame is the one we're
		// expecting and we shouldn't drop it.
		HandleFinalAckL();
		}

	if (// While we're in WAIT_F, peer may be still sending us I-Frames that are newer than those
		// acknowledged by the RR[P=1] frame. They will be retransmitted when it receives the RR,
		// so we can't accept the original transmissions, as that would move ExpectedAckSeq and
		// thus cause the retransmissions to be invalid - we'd close the connection.
		!iInWaitFState &&
		// ... also need to drop I-Frames when waiting to enter WAIT_F - if we're in WAIT_ACK,
		// then iWaitFStatePending persists for an extended period of time during which we may
		// have sent an RR due to some other unrelated reason. The peer started retransmitting
		// when it got that RR, which makes the situation identical to being in WAIT_F.
		// Ideally the spec would say that RR[P=1] makes the peer start retransmitting from
		// the given ReqSeq and RR[P=0] just makes it pick up where it left it, but it's not
		// the case - both restart the transmission, so both must be treated equally.
		!iWaitFStatePending)
		{
		const TUint8 bufferSeqBefore = iBufferSeq;

		iReceiveState->HandleIncomingIFrameL(aIFrame);

		if (iBufferSeq != bufferSeqBefore	// anything new to ack ?
			&& !iSendAck)					// an ack already pending ?
			{
			iSendAck = IsEndOfReceiveWindowApproaching();
			if (!iSendAck)
				{
				// Note that we'll never fall here if PeerTxWin <= KReceiveWinFreeSpaceLeftToTriggerAck
				if (!TimerMan().IsSendPeerAckTimerRunning())
					{
					if (!TimerMan().StartSendPeerAckTimer())
						{
						// The timer could not be started. Send an ack immediately.
						iSendAck = ETrue;
						}
					}
				}
			}
		} // !WAIT_F
	else
		{
		aIFrame.Free();
		}
	}

void CL2CapErtmDataReceiver::HandleIncomingSFrameL(RMBufChain& aSFrame)
	{
	LOG_FUNC
	LOG3(_L("ExpectedTxSeq=%d, BufferSeq=%d, LastAckReqSeq=%d"),
		 iExpectedTxSeq, iBufferSeq, iLastAckReqSeq)

	const TBool poll = HSFramePDU::PollBit(aSFrame);
	const TBool final = HSFramePDU::FinalBit(aSFrame);
	const TSupervisoryFunction function = HSFramePDU::SupervisoryFunction(aSFrame);

	if (poll && final)
		{
		LOG(_L("Incoming S-Frame has both P and F bits set!"))
		LEAVEL(KErrL2CAPIllegalRemoteBehavior);
		}

	if (final)
		{
		HandleFinalAckL();
		}
	
	if (poll && function != ESelectiveReject) // SREJ[P=1] is handled by the transmitter.
		{
		iReceiveState->HandlePollL();
		}
	}

void CL2CapErtmDataReceiver::HandleFinalAckL()
	{
	LOG_FUNC
	if (iInWaitFState)
		{
		iInWaitFState = EFalse;
		if (iIncomingSduQFull)
			{
			// Incoming SDU Q filled up again while we were exiting the previous LB condition.
			UpdateLocalBusyStatusL();
			}
		}
	}

void CL2CapErtmDataReceiver::SetIncomingSduQFullL(TBool aIncomingSduQFull)
	{
	LOG_FUNC
	LOG2(_L("Incoming SDU Q full %d -> %d"), iIncomingSduQFull, aIncomingSduQFull)

	if (!iIncomingSduQFull && aIncomingSduQFull)
		{
		// Don't enter LocalBusy even if we can, start the delay timer instead.
		// If the queue remains full until its expiry, we'll enter LocalBusy then.
		// LocalBusy means we inform the peer of our busy condition by sending RNRs
		// instead of RRs.
		// We refrain from entering LocalBusy right off because exiting it means
		// that the peer will have to restart the transmission from a seqnum given by us
		// in the RR exiting the LocalBusy condition, which in turn means some frames
		// which are already in transit (if there are any) will be lost. So if the full
		// SDU Q is just a momentary hiccup, we hide this from the peer and just buffer
		// the incoming frames and anchor BufferSeq at its current value to prevent the
		// receive window from moving.
		TimerMan().StartLocalBusyDelayTimer();
		}
	else if (iIncomingSduQFull && !aIncomingSduQFull)
		{
		TimerMan().StopLocalBusyDelayTimer();
		}

	iIncomingSduQFull = aIncomingSduQFull;
	UpdateLocalBusyStatusL();
	}

// This routine manages the LocalBusy status by evaluating the necessary conditions:
// - whether the incoming SDU queue is full at the moment,
// - whether LocalBusy delay timer is still running,
// - whether we're exiting a previous LocalBusy condition,
// - whether we're able to enter LB in current receive state (LB is only allowed in RECV).
void CL2CapErtmDataReceiver::UpdateLocalBusyStatusL()
	{
	LOG_FUNC
	LOG5(_L("iIncomingSduQFull=%d, iLocalBusy=%d, iWaitF=%d, iWaitFPending=%d, recv state = 0x%08x"),
		 iIncomingSduQFull, iLocalBusy, iInWaitFState, iWaitFStatePending, iReceiveState)

	if (iIncomingSduQFull)
		{
		if (!iLocalBusy && !TimerMan().IsLocalBusyDelayTimerRunning())
			{
			if (!iInWaitFState && !iWaitFStatePending // have to wait until previous LB is properly finished
				&& iReceiveState->IsLocalBusySupported()) // only RECV
				{
				EnterLocalBusyL();
				}
			}
		// Once we enter LB the receive state is locked onto RECV so don't have to check whether
		// it's changed.
		}
	else // !iIncomingSduQFull
		{
		// First we need to consume the frames on the incoming Q, which may cause the SDU Q to
		// fill up again.
		iIncomingIFrameQ.ConsumeUpToFirstGapL(*this);

		if (!iIncomingSduQFull)
			// Incoming SDU Q didn't fill up again in ConsumeUpToFirstGapL().
			{
			if (iLocalBusy)
				{
				// iLocalBusy is only possible when iInWaitFState && iWaitFStatePending are false
				// and the only receive state in LB is RECV, so we can exit it anytime.
				__ASSERT_DEBUG(iReceiveState == &iStateRecv && !iInWaitFState && !iWaitFStatePending,
							   Panic(EL2CAPLocalBusyUnderIllegalConditions));
				ExitLocalBusy();
				}
			}
		else // Incoming SDU Q filled up again in ConsumeUpToFirstGapL().
			{
			// Some frames have been consumed and we've restarted the LB delay timer,
			// so it's a good idea to ack now. 
			iSendAck = ETrue;
			}

		// Give the receive state a chance to sync BufferSeq with TxSeqExpectedBySduQ.
		// Must keep them in sync with if we can - there're reasons for that - see
		// TL2CapErtmReceiverStateRecv::HandlePollL. We don't do that in SREJ_SENT though
		// as BufferSeq can't change in that state but it's cool because our response to
		// a Poll in SREJ_SENT will be SREJ which doesn't trigger retransmission of all
		// unacked I-Frames and there's a separate mechanism for the peer to protect
		// itself against a duplicate SREJ[F=1].
		// This is asserted through the code.
		iReceiveState->TxSeqExpectedBySduQChanged(iIncomingIFrameQ.TxSeqExpectedBySduQ());

		__ASSERT_DEBUG(iIncomingSduQFull ||
					   ((iReceiveState != &iStateSRejSent &&
						 iIncomingIFrameQ.TxSeqExpectedBySduQ() == iBufferSeq &&
						 iBufferSeq == iExpectedTxSeq) ||
					    (iReceiveState == &iStateSRejSent)),
					   Panic(EL2CAPWindowInformationInconsistentWhenExitingSduQFull));

		__ASSERT_DEBUG(!iIncomingSduQFull ||
					   ((iReceiveState != &iStateSRejSent &&
						 iIncomingIFrameQ.TxSeqExpectedBySduQ() == iBufferSeq) ||
						(iReceiveState == &iStateSRejSent)),
					   Panic(EL2CAPWindowInformationInconsistentAfterMovingBufferSeqWhenSduQFull));
		} // !iIncomingSduQFull
	}

void CL2CapErtmDataReceiver::EnterLocalBusyL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!TimerMan().IsLocalBusyDelayTimerRunning(),
				   Panic(EL2CAPEnterLocalBusyCalledWhileDelayTimerStillRunning));

	iLocalBusy = ETrue;
	// Send an RNR immediately.
	iSendAck = ETrue;
	iController.NotifyMuxerOfPdusToSendIfHaveSome();
	}

void CL2CapErtmDataReceiver::ExitLocalBusy()
	{
	LOG_FUNC
	iLocalBusy = EFalse;
	// Signal to GetPdu that we need to enter WAIT_F.
	iWaitFStatePending = ETrue;
	iController.NotifyMuxerOfPdusToSendIfHaveSome();
	}

template<typename FrameType>
void CL2CapErtmDataReceiver::StampWithReqSeq(FrameType& aFrame)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!aFrame.IsQueuedForSend(), Panic(EL2CAPReqSeqSetOnFrameAlreadyQueuedForSend));

	aFrame.SetReqSeqNumber(iLastAckReqSeq = iBufferSeq);
	TimerMan().StopSendPeerAckTimer();
	iSendAck = EFalse;
	}

TBool CL2CapErtmDataReceiver::IsSRejPreferredToRej(TUint8 aExpectedTxSeq, TUint8 aReceivedTxSeq)
	{
	LOG_FUNC
	return InWindow(aReceivedTxSeq, aExpectedTxSeq, Mod64(aExpectedTxSeq + KSRejMissingFrameThreshold));
	}

void CL2CapErtmDataReceiver::LocalBusyDelayTimerExpired()
	{
	LOG_FUNC

	// This guards from the classic state-changed-between-timer-expiry-and-handler-entry
	// race condition.
	if (iIncomingSduQFull)
		{
		TRAPD(err, UpdateLocalBusyStatusL());
		if (err != KErrNone)
			{
			iController.ErrorD(err);
			// Just joined the majority.
			}
		}
	}

TBool CL2CapErtmDataReceiver::IsEndOfReceiveWindowApproaching() const
	{
	LOG_FUNC
	TBool ackNeeded = EFalse;

	const TUint8 numFramesReceivedSinceLastAck = Mod64(iExpectedTxSeq - iLastAckReqSeq);
	const TInt numFramesReceivedBeforeWantToSendAck = iController.Config().PeerTXWindowSize() - KReceiveWinFreeSpaceLeftToTriggerAck; 
	if (numFramesReceivedSinceLastAck >= numFramesReceivedBeforeWantToSendAck)
		{
		LOG2(_L("LastAckReqSeq = %d, ExpectedTxSeq = %d. Reaching end of window, will send an ack."),
			 iLastAckReqSeq, iExpectedTxSeq)
		ackNeeded = ETrue;
		}
	return ackNeeded;
	}

void CL2CapErtmDataReceiver::SendPeerAckTimerExpiredL()
	{
	LOG_FUNC
	if (iBufferSeq != iLastAckReqSeq)
		{
		iSendAck = ETrue;
		iController.NotifyMuxerOfPdusToSendIfHaveSome();
		}
	}

HSFramePDU* CL2CapErtmDataReceiver::GetAckFrameL(TBool aFinal)
	{
	HSFramePDU* frame = HSFramePDU::NewL(iLocalBusy ? EReceiverNotReady : EReceiverReady);
	frame->SetFinalBit(aFinal);
	return frame;
	}

void CL2CapErtmDataReceiver::SetStateL(TL2CapErtmReceiverStateBase& aState, RMBufChain* aIFrame)
	{
	LOG_FUNC
	iReceiveState->ExitL();
	iReceiveState = &aState;
	iReceiveState->EnterL(aIFrame);
	}


RL2CapErtmOutgoingQueue::RL2CapErtmOutgoingQueue(CL2CapEnhancedReTxController& aController)
 :	iController(aController),
	iOutgoingQ(_FOFF(HL2CapPDU, iDataControllerInternalQLink)),
	iPendRetransmitIFrameQ(_FOFF(HL2CapPDU, iDataControllerInternalQLink))
	{
	LOG_FUNC
	}

void RL2CapErtmOutgoingQueue::Close()
	{
	LOG_FUNC
	DeleteAllFrames();
	}

void RL2CapErtmOutgoingQueue::QueueIFrameL(HIFramePDU& aIFrame)
	{
	LOG_FUNC
	if (!TRetransmissionAndFlowControlOption::EnhancedMaxTransmitLessOrEqual(aIFrame.TransmissionCount() + 1,
																			 iController.Config().MaxTransmit()))
		{
		LOG2(_L("MaxTransmit=%d exceeded by I-Frame no %d"),
			 iController.Config().MaxTransmit(), aIFrame.TxSeqNumber())
		LEAVEL(KErrL2CAPMaxTransmitExceeded);
		}
	else if (aIFrame.IsQueuedForSend())
		{
		// This is either due to the I-Frame being SREJ-requested twice or a bug
		// in our code. We don't have enough information to tell between the two,
		// so have to assume it's the former.
		LOG1(_L("I-Frame no %d already queued for send"), aIFrame.TxSeqNumber())
		LEAVEL(KErrL2CAPIllegalRemoteBehavior);
		}

	iController.StampWithReqSeq(aIFrame);
	iOutgoingQ.AddLast(aIFrame);
	aIFrame.SetQueuedForSend(ETrue);
	LOG3(_L("Queued I-Frame for send, TxSeq=%d, ReqSeq=%d, F=%d"),
		 aIFrame.TxSeqNumber(), aIFrame.ReqSeqNumber(), aIFrame.FinalBit())
	}

void RL2CapErtmOutgoingQueue::QueueAckingSFrame(HSFramePDU& aSFrame)
	{
	LOG_FUNC
	// This method is meant only for those S-Frames which carry an acknowledgement number.
	__ASSERT_DEBUG(aSFrame.SupervisoryFunction() != ESelectiveReject || aSFrame.PollBit(),
				   Panic(EL2CAPSendSFrameWithAckCalledForSRejP0));
	// S-Frames are fire-n-forget, no healthy S-Frame gets queued twice!
	__ASSERT_DEBUG(!aSFrame.IsQueuedForSend(), Panic(EL2CAPSFrameQueuedForSendTwice));

	iController.StampWithReqSeq(aSFrame);
	iOutgoingQ.AddLast(aSFrame);
	aSFrame.SetQueuedForSend(ETrue);
	LOG4(_L("Queued S-Frame for send, S=%d, ReqSeq=%d, P=%d, F=%d"),
		 aSFrame.SupervisoryFunction(), aSFrame.ReqSeqNumber(), aSFrame.PollBit(), aSFrame.FinalBit())
	}

void RL2CapErtmOutgoingQueue::QueueNonAckingSFrame(HSFramePDU& aSFrame, TUint8 aReqSeq)
	{
	LOG_FUNC
	// S-Frames are fire-n-forget, no healthy S-Frame gets queued twice!
	__ASSERT_DEBUG(!aSFrame.IsQueuedForSend(), Panic(EL2CAPSFrameQueuedForSendTwice));

	aSFrame.SetReqSeqNumber(aReqSeq);
	iOutgoingQ.AddLast(aSFrame);
	aSFrame.SetQueuedForSend(ETrue);
	LOG4(_L("Queued S-Frame for send, S=%d, ReqSeq=%d, P=%d, F=%d"),
		 aSFrame.SupervisoryFunction(), aSFrame.ReqSeqNumber(), aSFrame.PollBit(), aSFrame.FinalBit())
	}

void RL2CapErtmOutgoingQueue::PendRetransmitIFrameL(HIFramePDU& aIFrame)
	{
	LOG_FUNC
	if (aIFrame.IsQueuedForSend())
		{
		// This is either due to the I-Frame being SREJ-requested twice or a bug
		// in our code. We don't have enough information to tell between the two,
		// so have to assume it's the former.
		LOG1(_L("I-Frame no %d already queued for send!"), aIFrame.TxSeqNumber())
		LEAVEL(KErrL2CAPIllegalRemoteBehavior);
		}
	iPendRetransmitIFrameQ.AddLast(aIFrame);
	aIFrame.SetQueuedForSend(ETrue);
	LOG3(_L("Queued I-Frame pending retransmission, TxSeq=%d, ReqSeq=%d, F=%d"),
		 aIFrame.TxSeqNumber(), aIFrame.ReqSeqNumber(), aIFrame.FinalBit())
	}

void RL2CapErtmOutgoingQueue::SendPendingRetransmitIFramesL()
	{
	LOG_FUNC
	if (!iPendRetransmitIFrameQ.IsEmpty())
		{
		TSglQueIter<HIFramePDU> iter(iPendRetransmitIFrameQ);
		while (HIFramePDU* eyeFrame = iter++)
			{
			eyeFrame->SetQueuedForSend(EFalse);	// QueueIFrameL would leave if this flag was set.
			QueueIFrameL(*eyeFrame);
			}
		iPendRetransmitIFrameQ.Reset();
		}
	}

void RL2CapErtmOutgoingQueue::CancelPendingRetransmitIFrames()
	{
	LOG_FUNC
	if (!iPendRetransmitIFrameQ.IsEmpty())
		{
		TSglQueIter<HIFramePDU> iter(iPendRetransmitIFrameQ);
		while (HIFramePDU* eyeFrame = iter++)
			{
			LOG1(_L("Cancelling SREJ-requested pending retransmission of I-Frame %d"),
				 eyeFrame->TxSeqNumber())
			eyeFrame->SetQueuedForSend(EFalse);
			}
		iPendRetransmitIFrameQ.Reset();
		}
	}

void RL2CapErtmOutgoingQueue::DeleteAllFrames()
	{
	LOG_FUNC
	if (!iPendRetransmitIFrameQ.IsEmpty())
		{
		TSglQueIter<HIFramePDU> iter(iPendRetransmitIFrameQ);
		while (HIFramePDU* eyeFrame = iter++)
			{
			delete eyeFrame;
			}
		iPendRetransmitIFrameQ.Reset();
		}

	if (!iOutgoingQ.IsEmpty())
		{
		TSglQueIter<HL2CapPDU> iter(iOutgoingQ);
		while (HL2CapPDU* pdu = iter++)
			{
			delete pdu;
			}
		iOutgoingQ.Reset();
		}	
	}

HL2CapPDU* RL2CapErtmOutgoingQueue::DequeueNextToSend()
	{
	LOG_FUNC
	HL2CapPDU* pdu = NULL;
	if (!iOutgoingQ.IsEmpty())
		{
		pdu = iOutgoingQ.First();
		if (!pdu->IsAwaitingHciCompletion())
			{
			iOutgoingQ.Remove(*pdu);
			pdu->SetQueuedForSend(EFalse);
			}
		else
			{
			LOG(_L("OutgoingQ stalled pending HCI Completion of the previous transmission of the next PDU"))
			pdu = NULL;
			}
		}
	return pdu;
	}


CL2CapEnhancedReTxController* CL2CapEnhancedReTxController::NewL(TL2CAPPort aLocalCID,
																 TL2CAPPort aRemoteCID,
																 CL2CAPMux& aMuxer,
																 CL2CapSDUQueue& aSDUQueue,
																		TL2CapDataControllerConfig* aConfig)
	{
	LOG_STATIC_FUNC
	CL2CapEnhancedReTxController* controller = new (ELeave) CL2CapEnhancedReTxController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig);
	CleanupStack::PushL(controller);
	controller->ConstructL();
	CleanupStack::Pop(controller);
	return controller;
	}

void CL2CapEnhancedReTxController::ConstructL()
	{
	LOG_FUNC
	iTransmitter = CL2CapErtmDataTransmitter::NewL(*this);
	iReceiver = CL2CapErtmDataReceiver::NewL(*this);
	}

// ***** CL2CapEnhancedReTxController Implementation
CL2CapEnhancedReTxController::CL2CapEnhancedReTxController(TL2CAPPort aLocalCID,
														   TL2CAPPort aRemoteCID,
														   CL2CAPMux& aMuxer,
														   CL2CapSDUQueue& aSDUQueue,
														   TL2CapDataControllerConfig* aConfig)
 :	CL2CapBasicDataController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig, EFalse),
	iDeliverOutgoingDataAndSignalToSduQWhenDone(EFalse),
	iOutgoingQ(*this),
 	iTimerMan(*this)
	{
	LOG_FUNC
	LOG2(_L("CL2CapEnhancedReTxController: local CID = %d, remote CID = %d"), aLocalCID, aRemoteCID)
	}

CL2CapEnhancedReTxController::~CL2CapEnhancedReTxController()
	{
	LOG_FUNC
	iTimerMan.Close();
	iOutgoingQ.Close();
	delete iTransmitter;
	delete iReceiver;
	}

void CL2CapEnhancedReTxController::HandleIncomingIFrameL(RMBufChain& aIFrame)
	{
	LOG_FUNC
	const TUint8 txSeq = HIFramePDU::TxSeqNumber(aIFrame);
	const TUint8 reqSeq = HIFramePDU::ReqSeqNumber(aIFrame);
	const TBool final = HIFramePDU::FinalBit(aIFrame);

	LOG3(_L("Incoming I-Frame: TxSeq = %d, F = %d, ReqSeq = %d"), 
		 txSeq, final, reqSeq)
	
	if (iReceiver->IsTxSeqInvalid(txSeq))
		{
		LOG3(_L("Received invalid TxSeq num %d, [BufferSeq = %d, Receive TxWin = %d]"),
			 txSeq, iReceiver->BufferSeq(), iConfig->PeerTXWindowSize())
		LEAVEL(KErrL2CAPInvalidPacketSequenceNumber);
		}
	else if (!iTransmitter->IsReqSeqValid(reqSeq))
		{
		LOG3(_L("Received invalid ReqSeq num %d, [ExpectedAckSeq = %d, NextTxSeq = %d]"),
			 reqSeq, iTransmitter->ExpectedAckSeq(), iTransmitter->NextTxSeq())
		LEAVEL(KErrL2CAPInvalidAcknowledgementNumber);
		}
	else if (IsFBitValid(final)) // ... and TxSeq and ReqSeq are valid
		{
		// ReqSeq & FBit valid: use this information whatever TxSeq value is, as long as it's
		// within window.

		if (final)
			{
			HandleFinalAck();
			}

		iTransmitter->HandleIncomingIFrame(aIFrame);
		iReceiver->HandleIncomingIFrameL(aIFrame);

		NotifyMuxerOfPdusToSendIfHaveSome();
		} // ReqSeq & Final Valid, TxSeq within window
	else
		{
		LOG(_L("F-bit invalid, dropping the I-Frame"))
		aIFrame.Free();
		}
	}

void CL2CapEnhancedReTxController::HandleIncomingSFrameL(RMBufChain& aSFrame)
	{
	LOG_FUNC

	const TUint8 reqSeq = HSFramePDU::ReqSeqNumber(aSFrame);
	const TBool poll = HSFramePDU::PollBit(aSFrame);
	const TBool final = HSFramePDU::FinalBit(aSFrame);

	LOG4(_L("Incoming S-Frame: S = %d, P = %d, F = %d, ReqSeq = %d"),
		 HSFramePDU::SupervisoryFunction(aSFrame), poll, final, reqSeq)

	if (!iTransmitter->IsReqSeqValid(reqSeq))
		{
		LOG3(_L("Received invalid ReqSeq num %d, [ExpectedAckSeq = %d, NextTxSeq = %d]"),
			 reqSeq, iTransmitter->ExpectedAckSeq(), iTransmitter->NextTxSeq())
		LEAVEL(KErrL2CAPInvalidAcknowledgementNumber);
		}
	else if (IsFBitValid(final)) // ... and TxSeq and ReqSeq are valid
		{
		// ReqSeq & FBit valid: use this information whatever TxSeq value is, as long as it's
		// within window.

		if (final)
			{
			HandleFinalAck();
			}

		iTransmitter->HandleIncomingSFrameL(aSFrame);
		iReceiver->HandleIncomingSFrameL(aSFrame);

		NotifyMuxerOfPdusToSendIfHaveSome();
		}
	else
		{
		LOG(_L("F-bit invalid, dropping the S-Frame"))
		aSFrame.Free();
		}
	}

void CL2CapEnhancedReTxController::HandleFinalAck()
	{
	LOG_FUNC
	// Just do housekeeping here. Transmitter/Receiver maintain their respective WAIT_* states
	// and know what to do with the final ack.
	iTimerMan.StopMonitorTimer();

	iPollSFrameTransmitCount = 0;
	}

void CL2CapEnhancedReTxController::SendIOrRrOrRnrL(TBool aFinal)
	{
	LOG_FUNC
	// Try piggybacking on an I-Frame first...
	HIFramePDU* eyeFrame = iTransmitter->GetIFrameToSendL();
	if (eyeFrame)
		{
		eyeFrame->SetFinalBit(aFinal);
		iOutgoingQ.QueueIFrameL(*eyeFrame);
		}
	else
		{
		// ... no I-Frame to send or can't send one.
		iOutgoingQ.QueueAckingSFrame(*iReceiver->GetAckFrameL(aFinal));
		}
	}

HL2CapPDU* CL2CapEnhancedReTxController::GetPduL()
	{
	LOG_FUNC
	// Note that:
	// - the queue contains S-Frames (naturally) and I-Frames (if they're SREJ-requested by
	// the peer or a Poll request was received and we had a data frame to piggyback the Final
	// response on);
	// - once a packet is put on the Q, it should be virtually considered to be on the air from
	// a logical POV.
	if (!iOutgoingQ.HaveFramesToTransmit())
		{
		// Check for async events to handle.
		// Note: polls are only sent when the OutgoingQ is empty.
		// Reason:
		// 1. The frames on the queue may be I-Frames queued in response to SREJs from the peer.
		// Sending the Poll before responding to the SREJs would lead to a race condition
		// (in WAIT_F) - the SREJs were received when XMIT state = Normal and so SRejActioned was
		// not set to True. So if they're not responded to before the Poll, we'll receive a
		// duplicate SREJ with the Final ack and respond to it with a duplicate I-Frame, which
		// causes disconnect.
		//
		// 2. Even though this may increase the latency of handling an Ack timer expiry if
		// there're packets on the queue, it actually bets on the assumption that these packets are
		// I-Frames SREJ-requested by the peer (same one(s) that caused us to time out) and
		// sending them before the Poll packet still gives the SREJ chance to work and hence
		// minimizes the amount of stuff we'll have to retransmit once we receive the final ack.]
		if ((iReceiver->IsWaitFStatePending() || iTransmitter->IsWaitAckStatePending()) &&
			!IsPollOutstanding())
			// ^- this serializes poll cycles - only one poll can be outstanding at a time
			// but the conditions for WAIT_F and WAIT_ACK are independent and can occur
			// simultaneously.
			{
			// Start a new Poll cycle.
			__ASSERT_DEBUG(iPollSFrameTransmitCount == 0, Panic(EL2CAPPollFrameNumberTransmitIsNotZero));

			HSFramePDU* sFrame = iReceiver->GetAckFrameL();
			sFrame->SetPollBit(ETrue);

			// Give WAIT_ACK preference in case both are outstanding.
			if (iTransmitter->IsWaitAckStatePending())
				{
				LOG(_L("Entering WaitAck"))
				iTransmitter->EnterWaitAckState();
				}
			else
				{
				LOG(_L("Entering WaitF"))
				iReceiver->EnterWaitFState();
				}

			iPollSFrameTransmitCount++;
			iTimerMan.StartMonitorTimer();

			iOutgoingQ.QueueAckingSFrame(*sFrame);
			}
		else // outgoing Q empty & no async events outstanding, can send data
			{
			HIFramePDU* eyeFrame = iTransmitter->GetIFrameToSendL();
			if (eyeFrame)
				{
				iOutgoingQ.QueueIFrameL(*eyeFrame);
				}
			}
		}

	// If an acking frame was queued in the previous step, this flag has been cleared.
	if (iReceiver->SendAck())
		{
		SendIOrRrOrRnrL(EFalse);
		}

	// If there's anything ready to send it's on the queue by now.
	HL2CapPDU* pduToSend = iOutgoingQ.DequeueNextToSend();
	if (pduToSend)
		{
		pduToSend->DeliverOutgoingPDU(*this);
		NotifyMuxerOfPdusToSendIfHaveSome();
		}

	return pduToSend;
	}

void CL2CapEnhancedReTxController::HandlePduSendComplete(HL2CapPDU& aPdu)
	{
	LOG_FUNC
	// We only claim ownership of I-Frames.
	iTransmitter->HciCompletedIFrame(static_cast<HIFramePDU&>(aPdu));
    // May be waiting for the current I-Frame to complete if it's already been
    // requested for retransmission.
    NotifyMuxerOfPdusToSendIfHaveSome();
	}

void CL2CapEnhancedReTxController::HandlePduSendError(HL2CapPDU& /*aPdu*/)
	{
	LOG_FUNC
	// We use protocol-level retransmissions for I-Frames.
	}

TInt CL2CapEnhancedReTxController::HandleOutgoingIFrame(HIFramePDU* aIFrame)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iTransmitter->InWaitAckState(), Panic(EL2CAPIFrameSentInWaitAck));

	aIFrame->SetPDUCID(iRemoteCID);
	aIFrame->CalculateAndSetFCS();

	aIFrame->SetPduOwner(this);

	LOG3(_L("Outgoing I-Frame: TxSeq = %d, F = %d, ReqSeq = %d"), 
		 aIFrame->TxSeqNumber(), aIFrame->FinalBit(), aIFrame->ReqSeqNumber())

	return KErrNone;
	}

TInt CL2CapEnhancedReTxController::HandleOutgoingSFrame(HSFramePDU* aSFrame)
	{
	LOG_FUNC
	aSFrame->SetPDUCID(iRemoteCID);
	aSFrame->CalculateAndSetFCS();

	LOG4(_L("Outgoing S-Frame: S = %d, P = %d, F = %d, ReqSeq = %d"),
		 aSFrame->SupervisoryFunction(), aSFrame->PollBit(), aSFrame->FinalBit(), aSFrame->ReqSeqNumber())

	return KErrNone;
	}

void CL2CapEnhancedReTxController::SendPeerAckTimerExpired()
	{
	LOG_FUNC
	TRAPD(err, iReceiver->SendPeerAckTimerExpiredL());
	if (err != KErrNone)
		{
		ErrorD(err);
		// RIP
		}
	}

void CL2CapEnhancedReTxController::AckTimerExpired()
	{
	LOG_FUNC
	// Reset the POLL transmit count (it will be incremented when the SFrame[POLL] is pulled for sending
	iPollSFrameTransmitCount = 0;
	iTransmitter->AckTimerExpired();
	}

void CL2CapEnhancedReTxController::LocalBusyDelayTimerExpired()
	{
	LOG_FUNC
	iReceiver->LocalBusyDelayTimerExpired();
	}

void CL2CapEnhancedReTxController::MonitorTimerExpired()
	{
	LOG_FUNC
	__ASSERT_DEBUG(IsPollOutstanding(), Panic(EL2CAPUnexpectedMonitorTimeout));
	__ASSERT_DEBUG(iPollSFrameTransmitCount > 0, Panic(EL2CAPPollFrameNumberTransmitIsZero));

	TInt err = KErrNone;

	if (!TRetransmissionAndFlowControlOption::EnhancedMaxTransmitLessOrEqual(iPollSFrameTransmitCount + 1,
																			 iConfig->MaxTransmit()))
		{
		LOG1(_L("MaxTransmit=%d exceeded by a Poll S-Frame"), iConfig->MaxTransmit())
		err = KErrL2CAPMaxTransmitExceeded;
		}
	else
		{
		HSFramePDU* frame = NULL;
		TRAP(err, frame = iReceiver->GetAckFrameL());
		if (err == KErrNone)
			{
			frame->SetPollBit(ETrue);
			iOutgoingQ.QueueAckingSFrame(*frame);

			iPollSFrameTransmitCount++;
			iTimerMan.StartMonitorTimer();

			NotifyMuxerOfPdusToSendIfHaveSome();
			}
		}
	if (err != KErrNone)
		{
		ErrorD(err);
		// RIP
		}
	}

TUint16 CL2CapEnhancedReTxController::MonitorTimeout()
	{
	return iConfig->MonitorTimeout();
	}

TUint16 CL2CapEnhancedReTxController::RetransmissionTimeout()
	{
	return iConfig->RetransmissionTimeout();
	}

TUint16 CL2CapEnhancedReTxController::PeerRetransmissionTimeout()
	{
	return iConfig->PeerRetransmissionTimeout();
	}

void CL2CapEnhancedReTxController::OutgoingPduAvailableOnSduQ()
	{
	LOG_FUNC
	NotifyMuxerOfPdusToSendIfHaveSome();
	}

void CL2CapEnhancedReTxController::SetIncomingSduQFull(TBool aIncomingSduQFull)
	{
	LOG_FUNC
	TRAPD(err, iReceiver->SetIncomingSduQFullL(aIncomingSduQFull));
	if (err != KErrNone)
		{
		ErrorD(err);
		// RIP
		}
	}

TBool CL2CapEnhancedReTxController::DeliverOutgoingDataAndSignalToSduQWhenDone()
	{
	LOG_FUNC
	iDeliverOutgoingDataAndSignalToSduQWhenDone = ETrue;
	// Returning true means we don't have any outstanding data to deliver and
	// hence can be deleted immediately.
	return !iTransmitter->HaveUnackedIFrames();
	}

inline void CL2CapEnhancedReTxController::NotifyMuxerOfPdusToSendIfHaveSome()
    {
    LOG_FUNC
    // This is intended to be your one-stop kick-the-muxer routine.
    // It should evaluate all conditions that may cause us to want to send
    // a PDU and should be called at the end of handling of every event which
    // may cause us to want to send stuff.
    //
    // The conditions are:
    // - if there's something on the OutgoingQ, then always transmit it ASAP,
    //   Outgoing Q is "the air" from the protocols point of view;
    // - if we're not in WAIT_ACK (now called WAIT_F in the spec), then send
    //   data:
    //      - I-Frames requested for retransmission (with REJ or ack timer expiry,
    //        SREJ-requested ones are put on the OutgoingQ),
    //      - new I-Frames from SDUs on the SDU Q.
    // - if there's some signalling to be send that hasn't been directly put
    //   on the OutgoingQ - send it:
    //      - need to go through WAIT_ACK and can enter it (WAIT_F not in progress),
    //      - need to go through WAIT_F and can enter it (WAIT_ACK not in progress);
    //      - time to send an acknowledgement to the peer.

    if (iOutgoingQ.HaveFramesToTransmit()  ||  // various stuff already on the transmit Q,
        // ... or data:
        (!iTransmitter->InWaitAckState() &&
            // ... in need of retransmission ...
            ((iTransmitter->IsRetransmittingUnackedIFrames() && !iTransmitter->IsNextUnackedIFrameAwaitingHciCompletion()) ||
            // ... or waiting to be pulled from SDU Q,
            (!iTransmitter->IsRetransmittingUnackedIFrames() && iSDUQueue.HavePDUToSend() && iTransmitter->HaveSpaceInOutgoingWindow()))
        ) ||
        // or some boring signalling:
        // ... want to enter WAIT_ACK and not already in WAIT_F ...
        (iTransmitter->IsWaitAckStatePending() && !iReceiver->InWaitFState()) ||
        // ... want to enter WAIT_F and not already in WAIT_ACK ...
        (iReceiver->IsWaitFStatePending()      && !iTransmitter->InWaitAckState()) ||
        // ... time to send an acknowledgement.
        iReceiver->SendAck())
        {
        iMuxer.PDUAvailable();
        }

    #ifdef __L2CAP_ERTM_GETPDU_DEBUG
    LOG1(_L("iOutgoingQ.HaveFramesToTransmit(): %d"), iOutgoingQ.HaveFramesToTransmit())
    LOG1(_L("iTransmitter->HaveSpaceInOutgoingWindow(): %d"), iTransmitter->HaveSpaceInOutgoingWindow())
    LOG1(_L("iTransmitter->InWaitAckState(): %d"), iTransmitter->InWaitAckState())
    LOG1(_L("iTransmitter->IsRetransmittingUnackedIFrames(): %d"), iTransmitter->IsRetransmittingUnackedIFrames())
    if (iTransmitter->IsRetransmittingUnackedIFrames())
        {
        LOG1(_L("iTransmitter->IsNextUnackedIFrameAwaitingHciCompletion(): %d"), iTransmitter->IsNextUnackedIFrameAwaitingHciCompletion())
        }
    LOG1(_L("iChannel.DataQueue().HavePDUToSend(): %d"), iSDUQueue.HavePDUToSend())
    LOG1(_L("iTransmitter->IsWaitAckStatePending(): %d"), iTransmitter->IsWaitAckStatePending())
    LOG1(_L("iReceiver->InWaitFState(): %d"), iReceiver->InWaitFState())
    LOG1(_L("iReceiver->IsWaitFStatePending(): %d"), iReceiver->IsWaitFStatePending())
    LOG1(_L("iTransmitter->InWaitAckState(): %d"), iTransmitter->InWaitAckState())
    LOG1(_L("iReceiver->SendAck(): %d"), iReceiver->SendAck())
    #endif
    }
