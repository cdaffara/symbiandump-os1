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

#include <bluetooth/logger.h>

#include <e32base.h>

#include "L2CapDataController.h"
#include "L2CapEnhancedDataController.h"

#include "L2CapSDUQueue.h"
#include "l2signalmgr.h"

#include "btsockettimer.h"
 
#include "l2util.h"
#include "L2CapDebugControlInterface.h"

#include "l2constants.h"	

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP_DATA_CONTROLLER);
#endif

using namespace L2CapDataUtils;

// ***** CL2CapBasicDataController Implementation
/*static*/ CL2CapBasicDataController* CL2CapBasicDataController::NewL(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig)
	{
	LOG_STATIC_FUNC
	// Create the required data controller. 
	CL2CapBasicDataController* controller = NULL;

	switch(aConfig->LinkMode())
		{
		case EL2CAPBasicMode:
			LOG(_L("FEC L2CapBasicDataController Basic Mode"))
			controller = new(ELeave) CL2CapBasicDataController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig, ETrue);
			break;
			
		case EL2CAPRetransmissionMode:
			LOG(_L("FEC L2CapBasicDataController Retransmission Mode"))
			controller = new(ELeave) CL2CapDataReTxController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig);		
			break;
			
		case EL2CAPFlowControlMode:
			LOG(_L("FEC L2CapBasicDataController Flow Control Mode"))
			controller = new(ELeave) CL2CapDataFlowController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig);		
			break;

		case EL2CAPStreamingMode:
			LOG(_L("FEC L2CapBasicDataController Streaming Mode"))
			controller = new(ELeave) CL2CapStreamingController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig);		
			break;

		case EL2CAPEnhancedRetransmissionMode:
			LOG(_L("FEC L2CapBasicDataController Enhnaced Retransmission Mode"))
			controller = CL2CapEnhancedReTxController::NewL(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig);		
			break;
		
		default:
			Panic(EL2CAPInvalidLinkMode);
			break;
		};
	
	return controller;
	}

CL2CapBasicDataController::CL2CapBasicDataController(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig, TBool aIsBasicDataVersion)
 : iSDUQueue(aSDUQueue),
   iMuxer(aMuxer),
   iLocalCID(aLocalCID),
   iRemoteCID(aRemoteCID),
   iConfig(aConfig),
   iIsBasicDataVersion(aIsBasicDataVersion)
	{
	LOG_FUNC
	// Register with the Mux.
	iMuxer.RegisterDataPDUHandler(*this);

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EDataPlanes,
	                             L2capDebugInfo::EAllocated));	
	}

CL2CapBasicDataController::~CL2CapBasicDataController()
	{
	LOG_FUNC
	delete iConfig;

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EDataPlanes,
	                             L2capDebugInfo::EDeleted));	
	}	

HL2CapPDU* CL2CapBasicDataController::GetPdu()
	{
	LOG_FUNC
	HL2CapPDU* pdu = NULL;
	TRAPD(err, pdu = GetPduL());
	if (err != KErrNone)
		{
		// The protocol here is that all leaves propagated up to here cause
		// us to ErrorD() ourselves, which will destruct data plane objects
		// (the data controller, SDU queue and the whole shebang) and error
		// the socket with the leave error code.
		ErrorD(err);
		}
	return pdu;
	}

HL2CapPDU* CL2CapBasicDataController::GetPduL()
	{
	LOG_FUNC
	HL2CapPDU* pduToSend = NULL;
	if((pduToSend = iSDUQueue.GetPDU()) != NULL)
		{
		pduToSend->SetPDUCID(iRemoteCID);
		}
	return pduToSend;
	}

void CL2CapBasicDataController::ProcessFlushTimerExpiry()
	{
	LOG_FUNC
	iMuxer.ProcessFlushTimerExpiry();
	}

void CL2CapBasicDataController::OutgoingPduAvailableOnSduQ()
	{
	LOG_FUNC
	iMuxer.PDUAvailable();
	}

void CL2CapBasicDataController::UpdateConfig(TL2CapDataControllerConfig* aConfig)
	{
	LOG_FUNC
	TBool channelPriorityUpdated = (iConfig->ChannelPriority() != aConfig->ChannelPriority());
	// Delete the existing config, and replace it with the new one.
	delete iConfig;
	iConfig = aConfig;

	if(channelPriorityUpdated)
		{
		iMuxer.ChannelPriorityUpdated(*this);
		}
	}

void CL2CapBasicDataController::UpdateChannelPriority(TUint8 aNewPriority)
	{
	LOG_FUNC
	if(iConfig->ChannelPriority() != aNewPriority)
		{
		iConfig->SetChannelPriority(aNewPriority);
		iMuxer.ChannelPriorityUpdated(*this);
		}
	}
	
	
void CL2CapBasicDataController::ErrorD(TInt aError)
	{
	LOG_FUNC
	LOG1(_L("FEC CL2CapBasicDataController::Error %d"), aError)
	iDataPlaneErrored = ETrue;
	iSDUQueue.ErrorD(aError);
	}

void CL2CapBasicDataController::SetIncomingSduQFull(TBool /*aIncomingSduQFull*/)
	{
	LOG_FUNC
	// No action can be taken in basic mode.
	}

TBool CL2CapBasicDataController::DeliverOutgoingDataAndSignalToSduQWhenDone()
	{
	LOG_FUNC
	// We can be deleted right away.
	return ETrue;
	}

void CL2CapBasicDataController::DeregisterFromMuxer()
	{
	LOG_FUNC
	iMuxer.DataChannelRemoved(this);
	}

#ifdef _DEBUG
TInt CL2CapBasicDataController::GetDataPlaneConfig(TL2DataPlaneConfig& conf) const
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	// Populate the debug structure.
	if(iConfig)
		{
		conf.iPriority = iConfig->ChannelPriority();
		conf.iLinkMode = iConfig->LinkMode();
		conf.iTxWindowSize = iConfig->TXWindowSize();
		conf.iMaxTransmit = iConfig->MaxTransmit();
		conf.iRetransmissionTimeout = iConfig->RetransmissionTimeout();
		conf.iMonitorTimeout = iConfig->MonitorTimeout();
		}
	else
		{
		rerr = KErrNotReady;
		}
	return rerr;
	}
#endif

TBool CL2CapBasicDataController::HandleIncomingDataFrame(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	TBool packetDelivered = EFalse;
	TRAPD(err, packetDelivered = HandleIncomingDataFrameL(aDataFrame));
	if (err != KErrNone)
		{
		// The protocol here is that all leaves propagated up to here cause
		// us to ErrorD() ourselves, which will destruct data plane objects
		// (the data controller, SDU queue and the whole shebang) and error
		// the socket with the leave error code.
		ErrorD(err);

		if (aDataFrame.IsEmpty())
			{
			packetDelivered = ETrue;
			}
		// else frame will be offered to other data controllers
		}
	return packetDelivered;
	}

void CL2CapBasicDataController::HandleIncomingIFrameL(RMBufChain& /*aDataFrame*/)
	{
	LOG_FUNC
	Panic(EL2CAPIFrameOrSFrameHandledByBasicController);
	}

void CL2CapBasicDataController::HandleIncomingBFrameL(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	iSDUQueue.PutBFramePDU(aDataFrame);
	}

void CL2CapBasicDataController::HandleIncomingSFrameL(RMBufChain& /*aDataFrame*/)
	{
	LOG_FUNC
	Panic(EL2CAPIFrameOrSFrameHandledByBasicController);
	}

TBool CL2CapBasicDataController::HandleIncomingDataFrameL(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	TBool PDUProcessed = EFalse;
	TInt err = KErrNone;
	
	if(HL2CapPDU::PDUCID(aDataFrame) == iLocalCID)
		{
		if(iIsBasicDataVersion)
			{
			// This can only be a B-Frame.
			if(HBFramePDU::CheckDecode(aDataFrame) == KErrNone)
			   	{
			   	HandleIncomingBFrameL(aDataFrame);
			   	}
			else
				{
				// Invalid frame.  Drop it.
				aDataFrame.Free();
				LOG(_L("FEC L2CapBasicDataController B-Frame Dropped"))
				}
			}
		else
			{
			if(HIFramePDU::IFrameIdentifier(aDataFrame))
				{
				// This is an I-Frame

				// Check stuff that causes us to drop frames first:
				// - FCS (2.1 Core Spec Addendum 1: 3.3.7 Invalid Frame Detection Algorithm pt. 2)
				// - length field matching the actual payload length (funnily 3.3.7 doesn't say
				//   what to do with it, pt. 7.2.2 Recombination Of L2CAP PDUs mentions it in
				//   passing).
				// Note: the frame is dropped when length is incorrect (instead of disconnecting)
				// because we don't know where FCS is (it's at the end of the packet and we don't
				// know where that is), so have to assume that the length field problem is due to
				// transmission corruption and not the remote misbehaving.
				if (HIFramePDU::CheckPayloadDecode(aDataFrame) != KErrNone ||
					HL2CapPDU::CheckDecode(aDataFrame) != KErrNone)
					{
					aDataFrame.Free();
					LOG(_L("FEC L2CapBasicDataController I-Frame Dropped"))
					}
				else
					{
					// Now check stuff that causes us to disconnect (per CSA1 3.3.7).
					err = HIFramePDU::CheckLengthWithinLimits(aDataFrame, iConfig->IncomingMps());
					if (err != KErrNone)
						{
						LOG(_L("FEC L2CapBasicDataController I-Frame invalid"))

						aDataFrame.Free();
						// This will cause the channel to be closed.
						LEAVEL(err);
						}
					else
						{
						HandleIncomingIFrameL(aDataFrame);
						}
					}
				}
			else
				{
				// S-Frame

				// Check stuff that causes the frame to be dropped first.
				if (HSFramePDU::CheckPayloadDecode(aDataFrame) != KErrNone ||
					HL2CapPDU::CheckDecode(aDataFrame) != KErrNone)
					{
					LOG(_L("FEC L2CapBasicDataController S-Frame Dropped"))
					}
				else
					{
					// Now check stuff that causes us to disconnect (per CSA1 3.3.7).
					err = HSFramePDU::CheckLengthField(aDataFrame);
					if (err != KErrNone)
						{
						LOG(_L("FEC L2CapBasicDataController S-Frame Invalid"))

						aDataFrame.Free();
						// This will cause the channel to be closed.
						LEAVEL(err);
						}
					else
						{
						HandleIncomingSFrameL(aDataFrame);
					  	}
					}
				// Finished with the S-Frame.
				aDataFrame.Free();
				}
			}
		PDUProcessed = ETrue;
		}
	return PDUProcessed;
	}

// Outgoing PDU handler.
TInt CL2CapBasicDataController::HandleOutgoingIFrame(HIFramePDU* /*aIFrame*/)
	{
	LOG_FUNC
	return KErrNone;
	}

TInt CL2CapBasicDataController::HandleOutgoingBFrame(HBFramePDU* /*aBFrame*/)
	{
	LOG_FUNC
	return KErrNone;
	}

TInt CL2CapBasicDataController::HandleOutgoingGFrame(HGFramePDU* /*aGFrame*/)
	{
	LOG_FUNC
	return KErrNone;
	}

TInt CL2CapBasicDataController::HandleOutgoingCFrame(HCFramePDU* /*aCFrame*/)
	{
	LOG_FUNC
	return KErrNone;
	}

TInt CL2CapBasicDataController::HandleOutgoingSFrame(HSFramePDU* /*aSFrame*/)
	{
	LOG_FUNC
	return KErrNone;
	}

// ***** CL2CapDataFlowController Implementation
CL2CapDataFlowController::CL2CapDataFlowController(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig)
 : CL2CapBasicDataController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig, EFalse),
   iPendingSentPDUs(_FOFF(HIFramePDU, iLink)),
   iSentPDUs(_FOFF(HIFramePDU, iLink)),
   iTimerMan(*this),
   iIncomingSduQFull(EFalse)
	{
	LOG_FUNC
	}

CL2CapDataFlowController::~CL2CapDataFlowController()
	{
	LOG_FUNC
	// Some PDUs may hold a reference to this data controller.
	TDblQueIter<HIFramePDU> pendingIter(iPendingSentPDUs);
	TDblQueIter<HIFramePDU> sentIter(iSentPDUs);
	HIFramePDU* pduPtr;
	
	while((pduPtr = pendingIter++) != NULL)
		{
		LOG1(_L("Deregistering TxSeq = %d"), pduPtr->TxSeqNumber());
		pduPtr->DeregisterPduOwner();
		}

	// Delete any PDUs that have already been sent.
	while((pduPtr = sentIter++) != NULL)
		{
		delete pduPtr;
		}

	// Cancel any outstanding timers.
	iTimerMan.Close();
	}


void CL2CapDataFlowController::ProcessFlushTimerExpiry()
	{
	LOG_FUNC
	iMuxer.ProcessFlushTimerExpiry();
	PDUAvailable();

	if(!iSentPDUs.IsEmpty())
		{
		iNextTxSeq = Mod64(iSentPDUs.Last()->TxSeqNumber() + 1);
		}
	else
		{
		iNextTxSeq = iExpectedAckSeq;
		}
	}

void CL2CapDataFlowController::HandleIncomingIFrameL(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	// Check if flow control is active for incoming data.
	if(iIncomingSduQFull)
		{
		// Check that the PDU sequence number is within the expected range.
		if(!iSenderTxWindowClosed && Mod64(HIFramePDU::TxSeqNumber(aDataFrame) - iLastAckSentRxSeqNum) >= iConfig->PeerTXWindowSize())
			{
			// The number is not in range.  No more I-Frames will be processed
			// until flow control is deactivated.
			iSenderTxWindowClosed = ETrue;
			}
		}

	LOG(_L("FEC CL2CapDataFlowController: HandleIncomingIFrame"))
	LOG1(_L("\tTxSeqNumber = %d"), HIFramePDU::TxSeqNumber(aDataFrame))
	LOG1(_L("\tLastAckSentRxSeqNum = %d"), iLastAckSentRxSeqNum)
	
	if(!iSenderTxWindowClosed)
		{
		ProcessIFrameL(aDataFrame);
		}
	else
		{
		// Drop the frame.
		aDataFrame.Free();
		LOG(_L("FEC CL2CapDataFlowController: Incoming PDU seq number of I-Frame is not in range!"))
		}
	}

void CL2CapDataFlowController::ProcessIFrameL(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	iExpectedTxSeq = Mod64(iExpectedTxSeq + 1); 
	iExpectedAckSeq = HIFramePDU::ReqSeqNumber(aDataFrame);
	
	LEAVEIFERRORL(iSDUQueue.PutIFramePDU(aDataFrame));

	// Check if the acknowledged frame(s) has created
	// space in the window.	
	RemoveAckedPDUsFromSentQueue();

	// Only send an ack if incoming data is not currently flow controlled off.
	if(!iIncomingSduQFull)
		{
		// Check if the peer frames need to be acknowledged now.
		// Check the peer TxWindow.  This check is
		// (Number of I-Frames recv since last Ack was sent > (TxWindow - Constant)
		if(Mod64(iExpectedTxSeq - iLastAckSentRxSeqNum) > (iConfig->PeerTXWindowSize() - KTxWinAckThresholdOffset))
			{
			// Send an ack.
			iSendAckToPeer = ETrue;
			}
		else
			{
			// If the timer is not running, start it.
			if(!iTimerMan.IsSendPeerAckTimerRunning())
				{
				if(!iTimerMan.StartSendPeerAckTimer())
					{
					// A timer could not be started.  Send an ack immediately.
					iSendAckToPeer = ETrue;
					}
				}
			}
		}
	// Check if any outbound data can be sent.
	PDUAvailable();
	}

void CL2CapDataFlowController::HandleIncomingSFrameL(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	// Check that the function is RR.  If not this S-Frame is not valid for
	// flow control mode.
	if(HSFramePDU::SupervisoryFunction(aDataFrame) == EReceiverReady)
		{
		iExpectedAckSeq = HSFramePDU::ReqSeqNumber(aDataFrame);
		// Check if the acknowledged frame(s) has created
		// space in the window.	
		RemoveAckedPDUsFromSentQueue();
		PDUAvailable();				
		LOG(_L("FEC CL2CapDataFlowController: Incoming S-Frame: RR"))				
		LOG1(_L("\tiLastAckedFrameNum = %d"),  iExpectedAckSeq)
		}
	}

TInt CL2CapDataFlowController::HandleOutgoingIFrame(HIFramePDU* aIFrame)
	{
	LOG_FUNC
	aIFrame->SetPDUCID(iRemoteCID);
	aIFrame->SetTxSeqNumber(iNextTxSeq);
	LOG(_L("FEC CL2CapDataFlowController: HandleOutgoingIFrame"))
	LOG1(_L("\tTxSeqNumber = %d"), iNextTxSeq)
	iNextTxSeq = Mod64(iNextTxSeq + 1);

	// If flow controlled off - don't acknowledge any new I-Frames
	aIFrame->SetReqSeqNumber(iIncomingSduQFull ? iLastAckSentRxSeqNum : iExpectedTxSeq);

	// Set the retransmission disable bit if flow control is on.
	aIFrame->SetRetransmitDisable(iIncomingSduQFull);
	aIFrame->CalculateAndSetFCS();

	aIFrame->iLink.Deque(); // just in case, it shouldn't be on any list at this point
	aIFrame->SetPduOwner(this);
	iPendingSentPDUs.AddLast(*aIFrame);
	
	// Cancel the peer ack timer.
	iTimerMan.StopSendPeerAckTimer();
	iSendAckToPeer = EFalse;

	// Store the last acknowledged frame index.
	if(!iIncomingSduQFull)
		{
		iLastAckSentRxSeqNum = iExpectedTxSeq;
		}
	LOG1(_L("\tLastAckSentRxSeqNum = %d"), iLastAckSentRxSeqNum)
	
	return KErrNone;
	}

TInt CL2CapDataFlowController::HandleOutgoingSFrame(HSFramePDU* aSFrame)
	{
	LOG_FUNC
	aSFrame->SetPDUCID(iRemoteCID);
	// If flow controlled off - don't acknowledge any new I-Frames
	aSFrame->SetReqSeqNumber(iIncomingSduQFull ? iLastAckSentRxSeqNum : iExpectedTxSeq);
	LOG(_L("FEC CL2CapDataFlowController: HandleOutgoingSFrame"))		
	LOG1(_L("\tReqSeqNUmber = %d"), iIncomingSduQFull ? iLastAckSentRxSeqNum : iExpectedTxSeq)
	// Set the retransmission disable bit if flow control is on.
	aSFrame->SetRetransmitDisable(iIncomingSduQFull);
	aSFrame->CalculateAndSetFCS();

	// If the monitor timer is currently running.  Re-start it.
	if(!iTimerMan.IsAckTimerRunning())
		{
		iTimerMan.StartMonitorTimer();
		}

	// Store the last acknowledged frame index.
	if(!iIncomingSduQFull)
		{
		iLastAckSentRxSeqNum = iExpectedTxSeq;
		}
		
	// Cancel the peer ack timer.
	iTimerMan.StopSendPeerAckTimer();
	iSendAckToPeer = EFalse;
	LOG1(_L("\tLastAckSentRxSeqNum = %d"), iLastAckSentRxSeqNum)
	return KErrNone;
	}

TBool CL2CapDataFlowController::CanSendPDU()
	{
	LOG_FUNC
	TBool canSend = ETrue;

	// This condition is (V(S) - V(A))mod64 >= TxWindow
	if(Mod64(iNextTxSeq - iExpectedAckSeq) >= iConfig->TXWindowSize())
		{
		canSend = EFalse;
		}	
	return canSend;
	}

					
HL2CapPDU* CL2CapDataFlowController::GetPduL()
	{
	LOG_FUNC
	// This is called from the signal manager.
	HL2CapPDU* pduToSend = NULL;

	if(CanSendPDU())
		{	
		pduToSend = iSDUQueue.GetPDU();
		}

	if(pduToSend)
		{
		pduToSend->DeliverOutgoingPDU(*this);
		}

	// Note:  If pduToSend has a valid pointer then the call to 
	// pduToSend->DeliverOutgoingPDU(*this) (above) will clear the 
	// iSendAckToPeer flag.
	if(iSendAckToPeer)
		{
		pduToSend = HSFramePDU::New(EReceiverReady);
		if(pduToSend)
			{
			pduToSend->DeliverOutgoingPDU(*this);
			}
		else
			{
			ErrorD(KErrNoMemory);
			// We've been deleted!
			}
		}
	return pduToSend;
	}


void CL2CapDataFlowController::PDUAvailable()
	{
	LOG_FUNC

	// Note that CanSend only checks whether there's available outgoing window space,
	// not whether there really is some outstanding data on SDU Q. GetPduL will check
	// that and do nothing if there isn't.
	if(CanSendPDU() || iSendAckToPeer)
		{
		iMuxer.PDUAvailable();
		}
	}

void CL2CapDataFlowController::RemoveAckedPDUsFromSentQueue()
	{
	LOG_FUNC
	TDblQueIter<HIFramePDU> iter(iSentPDUs);
	HIFramePDU* pduPtr;
	TInt ackedPDUs = 0;
	TInt count = 0;

	TUint8 frameSeqNumber = Mod64(iExpectedAckSeq - 1);

	// Find out how many PDU's have been ack'd
	while((pduPtr = iter++) != NULL)
		{
		if(pduPtr->TxSeqNumber() == frameSeqNumber)
			{
			// Increment the count by one and exit the loop.
			ackedPDUs = count + 1;
			break;
			}
		count++;
		}

	// Remove all ack'd PDUs
	iter.SetToFirst();
	for(TInt i=0;i<ackedPDUs;i++)
		{
		pduPtr = iter++;
		delete pduPtr;
		}

	// Some PDUs have been ack'd.
	if(ackedPDUs)
		{
		if(iSentPDUs.IsEmpty())
			{
			iTimerMan.StartMonitorTimer();
			}
		else
			{
			iTimerMan.StopMonitorTimer();
			iTimerMan.StartAckTimer();
			}
		}
	}

void CL2CapDataFlowController::MonitorTimerExpired()
	{
	LOG_FUNC
	// The monitor timer has expired.  Send a S-Frame to the peer by setting
	// the ack outstanding flag and calling PDUAvailable.  This method will
	// call this back at which point the S-Frame will be created.
	LOG(_L("FEC CL2CapDataFlowController::MonitorTimerExpired"))
	iSendAckToPeer = ETrue;
	PDUAvailable();
	}
	
	
void CL2CapDataFlowController::AckTimerExpired()
	{
	LOG_FUNC
	// Remove the PDU that the timer is supervising.  This can
	// only be the first Sent PDU.
	LOG(_L("FEC CL2CapDataFlowController::AckTimerExpired"))
	__ASSERT_DEBUG(!iSentPDUs.IsEmpty(), Panic(EL2CAPAckTimerExpiryWithoutPDUToSupervise));
	
	HL2CapPDU* pdu = iSentPDUs.First();
	delete pdu;

	iExpectedAckSeq = Mod64(iExpectedAckSeq + 1);

	if(iSentPDUs.IsEmpty())
		{
		iTimerMan.StartMonitorTimer();
		}
	else
		{
		iTimerMan.StopMonitorTimer();
		iTimerMan.StartAckTimer();
		}
	PDUAvailable();	
	}

void CL2CapDataFlowController::SendPeerAckTimerExpired()
	{
	LOG_FUNC

	// Indicate that an ack should be sent to the peer to
	// acknowledge receipt of I-Frames.
	iSendAckToPeer = ETrue;
	PDUAvailable();	
	}

TUint16 CL2CapDataFlowController::MonitorTimeout()
	{
	return iConfig->MonitorTimeout();
	}

TUint16 CL2CapDataFlowController::RetransmissionTimeout()
	{
	return iConfig->RetransmissionTimeout();
	}

TUint16 CL2CapDataFlowController::PeerRetransmissionTimeout()
	{
	return iConfig->PeerRetransmissionTimeout();
	}


void CL2CapDataFlowController::HandlePduSendComplete(HL2CapPDU& aPdu)
	{
	LOG_FUNC

	// We only claim ownership of I-Frames.
	HIFramePDU& IFrame = static_cast<HIFramePDU&>(aPdu);

	// Remove the PDU from the pending send queue.
	IFrame.iLink.Deque();
	
	// If there are outstanding (i.e., un-ack'ed) PDU's, the
	// Ack timer will already be running.  Otherwise it should
	// be started.
	if(iSentPDUs.IsEmpty())
		{
		iTimerMan.StartAckTimer();
		}

	// It's waiting for acknowledgement now.
	iSentPDUs.AddLast(IFrame);
	PDUAvailable();
	}

void CL2CapDataFlowController::HandlePduSendError(HL2CapPDU& /*aPdu*/)
	{
	LOG_FUNC
	// In FC mode the frame will get deleted when we get an acknowledgement.
	// In RTM, protocol-level retransmissions will be run when peer rejects or
	// our ack timer expires.
	}


void CL2CapDataFlowController::SetIncomingSduQFull(TBool aIncomingSduQFull)
	{
	LOG_FUNC
	// Check that the FC status has changed
	if(iIncomingSduQFull != aIncomingSduQFull)
		{
		iIncomingSduQFull = aIncomingSduQFull;
		iSenderTxWindowClosed = EFalse;

		// Send an acknowledgement to the peer indicate the new
		// flow control status.
		iSendAckToPeer = ETrue;
		PDUAvailable();
		}
	}


// ***** CL2CapDataReTxController Implementation
CL2CapDataReTxController::CL2CapDataReTxController(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, CL2CapSDUQueue& aSDUQueue, TL2CapDataControllerConfig* aConfig)
 : CL2CapDataFlowController(aLocalCID, aRemoteCID, aMuxer, aSDUQueue, aConfig),
   iRejectPDU(0),
   iRetransmitSentPDUs(EFalse),
   iRejectSent(EFalse),
   iRetransmissionDisabled(EFalse),
   iRestartAckTimer(EFalse),
   iRetransTxVal(iNextTxSeq)
	{
	LOG_FUNC
	}
	
void CL2CapDataReTxController::HandleIncomingIFrameL(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	TBool newRtxDisable = HIFramePDU::RetransmitDisable(aDataFrame);
	if(iExpectedTxSeq == HIFramePDU::TxSeqNumber(aDataFrame))
		{
		LOG(_L("FEC CL2CapDataReTxController: HandleIncomingIFrame"))
		LOG1(_L("\tTxSeqNumber = %d"), HIFramePDU::TxSeqNumber(aDataFrame))	
		ProcessIFrameL(aDataFrame);
		iRejectSent = EFalse;
		}
	else
		{
		// This I-Frame has an invalid sequence number.
		// Check that a Reject Exception is not already being
		// handled.
		if(!iRejectSent)
			{
			iRejectPDU = HSFramePDU::NewL(EReject);
			iRejectPDU->DeliverOutgoingPDU(*this);
			// Send the Reject PDU.
			iMuxer.PDUAvailable();
			iRejectSent = ETrue;
			}
		aDataFrame.Free();
		LOG(_L("FEC CL2CapDataReTxController: Incoming PDU seq number of I-Frame is not in range!"))
		}
	
	// Check if the re-transmission disable bit has transitioned.
	if(iRetransmissionDisabled != newRtxDisable)
		{
		if(newRtxDisable)
			{
			// Re-transmission has been disabled.
			iTimerMan.StartMonitorTimer();
			}
		else
			{
			// Re-transmission has been enabled.
			if(!iSentPDUs.IsEmpty())
				{
				iTimerMan.StopMonitorTimer();
				iTimerMan.StartAckTimer();
				}
			}
		iRetransmissionDisabled = newRtxDisable;
		}
	}

void CL2CapDataReTxController::HandleIncomingSFrameL(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	if(HSFramePDU::SupervisoryFunction(aDataFrame) == EReject)
		{
		iRetransmitSentPDUs = ETrue;
		iRetransTxVal = HSFramePDU::ReqSeqNumber(aDataFrame);
		LOG(_L("FEC CL2CapDataReTxController: Incoming S-Frame: Reject"))		
		}
	iExpectedAckSeq = HSFramePDU::ReqSeqNumber(aDataFrame);
	LOG(_L("FEC CL2CapDataReTxController: Incoming S-Frame"))
	LOG1(_L("\tLastAckedFrameNum = %d"), iExpectedAckSeq)

	TBool newRtxDisable = HSFramePDU::RetransmitDisable(aDataFrame);

	// Check if the acknowledged frame(s) has created
	// space in the window.	
	RemoveAckedPDUsFromSentQueue();
	PDUAvailable();				
	
	// Check if the re-transmission disable bit has transitioned.
	if(iRetransmissionDisabled != newRtxDisable)
		{
		if(newRtxDisable)
			{
			// Re-transmission has been disabled.
			iTimerMan.StartMonitorTimer();
			}
		else
			{
			// Re-transmission has been enabled.
			if(!iSentPDUs.IsEmpty())
				{
				iTimerMan.StopMonitorTimer();
				iTimerMan.StartAckTimer();
				}
			}
		iRetransmissionDisabled = newRtxDisable;
		}
	}

HL2CapPDU* CL2CapDataReTxController::GetPduL()
	{
	LOG_FUNC
	// This is called from the signal manager.
	HL2CapPDU* pduToSend = NULL;

	// Check the re-transmission disabled flag.  If its set
	// then only S-Frames can be sent.
	if(!iRetransmissionDisabled && !iDataPlaneErrored)
		{
		if(iRejectPDU)
			{
			pduToSend = iRejectPDU;
			iRejectPDU = NULL;
			}
		else
			{
			if(iRetransmitSentPDUs)
				{
				pduToSend = RetransmitSentPDU();
				}	

			if(!pduToSend && CanSendPDU())
				{	
				pduToSend = iSDUQueue.GetPDU();
				}
			}
		}

	if(pduToSend)
		{
		pduToSend->DeliverOutgoingPDU(*this);
		}

	// Note:  If pduToSend has a valid pointer then the call to 
	// pduToSend->DeliverOutgoingPDU(*this) (above) will clear the 
	// iSendAckToPeer flag.
	if(iSendAckToPeer)
		{
		pduToSend = HSFramePDU::New(EReceiverReady);
		if(pduToSend)
			{
			pduToSend->DeliverOutgoingPDU(*this);
			}
		else
			{
			ErrorD(KErrNoMemory);
			// We've been deleted!
			}
		}
	return pduToSend;
	}

void CL2CapDataReTxController::PDUAvailable()
	{
	LOG_FUNC

	// Note that CanSend only checks whether there's available outgoing window space,
	// not whether there really is some outstanding data on SDU Q. GetPduL will check
	// that and do nothing if there isn't.
	if(CanSendPDU() || iSendAckToPeer || iRetransmitSentPDUs)
		{
		iMuxer.PDUAvailable();
		}
	}

void CL2CapDataReTxController::AckTimerExpired()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iSentPDUs.IsEmpty(), Panic(EL2CAPAckTimerExpiryWithoutPDUToSupervise));

	// Check if the PDU can be re-sent (i.e., the Max.
	// transmit has not been exceeded.)
	
    HIFramePDU* pdu = iSentPDUs.First();
        
	if(pdu->CanTransmit(iConfig->MaxTransmit()))
		{
		// Set the next frame to be transmitted to the PDU 
		// whose timer expired.
		iRetransTxVal = pdu->TxSeqNumber();
		iRetransmitSentPDUs = ETrue;
		iRestartAckTimer = ETrue;

		// The frame will be re-transmitted when GetPDU is called.
		// This will also re-start the Retransmission timer.
		PDUAvailable();	
		}
	else
		{
		// The channel can not support the requested quality.
		// i.e., the number of retransmissions before the channel is
		// disconnected has been exceeded.
		// Error the SAP to disconnect the channel.
		ErrorD(KErrL2CAPMaxTransmitExceeded);
		LOG(_L("FEC CL2CapDataReTxController::AckTimerExpired - KErrL2CAPMaxTransmitExceeded"))	
		}
	}

HL2CapPDU* CL2CapDataReTxController::RetransmitSentPDU()
	{
	LOG_FUNC
	HIFramePDU* pduToSend = NULL;
	TDblQueIter<HIFramePDU> iter(iSentPDUs);
	HIFramePDU* pduPtr;
	
	while((pduPtr = iter++) != NULL)
		{
		if(pduPtr->TxSeqNumber() == iRetransTxVal)
			{
			pduToSend = pduPtr;
			break;
			}
		}

	if(!pduToSend)
		{
		iRetransmitSentPDUs = EFalse;
		}
	return pduToSend;
	}

void CL2CapDataReTxController::HandlePduSendComplete(HL2CapPDU& aPdu)
	{
	LOG_FUNC

	// We only claim ownership of I-Frames.
	HIFramePDU& IFrame = static_cast<HIFramePDU&>(aPdu);

	// Remove the PDU from the pending send queue.
	IFrame.iLink.Deque();
	
	// If there are outstanding (i.e., un-ack'ed) PDU's, the
	// Ack timer will already be running.  Otherwise it should
	// be started.
	if(iSentPDUs.IsEmpty() || iRestartAckTimer)
		{
		iTimerMan.StopMonitorTimer();
		iRestartAckTimer = EFalse;
		iTimerMan.StartAckTimer();
		}

	iSentPDUs.AddLast(IFrame);
	
	// When operating in lossy conditions such as when running 
	// concurrently with a name inquiry it is possible for an 
	// acknowledgement to arrive before the sent completion code is called
	// for the relevant PDU.  As a result aIFrame may have already been 
	// acknowledged.  Now that we have ownership of the PDU we can 
	// remove it and cancel the Ack timer if necessary.
	
	// N.B. Here we have assumed that the delay in the notification 
	// that a PDU has been sent (thereby moving it from the pending sent
	// queue to the sent queue) is not so large that in the mean time we
	// have sent so many new PDUs that the transmission window has wrapped
	// so far around that an old I-Frame from the last cycle is 
	// considered an unacknowledged one because the Tx Seq value is once again 
	// in the valid transmission window.  This would require BT to be much
	// faster than it currently is (11/04/2007). 
	RemoveAckedPDUsFromSentQueue();
	PDUAvailable();
	}
	
void CL2CapDataReTxController::RemoveAckedPDUsFromSentQueue()
	{
	LOG_FUNC
	TDblQueIter<HIFramePDU> iter(iSentPDUs);
	HIFramePDU* pduPtr;
	TBool ackedPDUs = EFalse;

	// Remove PDUs which have been previously acknowledged.
	// The queue may be out of order due to retransmissions 
	// being appended at the end of the sent queue.
	while((pduPtr = iter++) != NULL)
		{
		// If the PDU's Tx Seq number is not in the current valid transmission window it is considered a previously acknowledged I-Frame and is removed.
		if (!(((pduPtr->TxSeqNumber()>= iExpectedAckSeq) && (pduPtr->TxSeqNumber()< (iExpectedAckSeq + iConfig->TXWindowSize()))) || (((iExpectedAckSeq + iConfig->TXWindowSize()) > KL2CapTxSeqValues) && (pduPtr->TxSeqNumber()< Mod64((iExpectedAckSeq + iConfig->TXWindowSize()))))))
			{
			ackedPDUs = ETrue;
			delete pduPtr;
			}
		}

	// Some PDUs have been ack'd.
	if(ackedPDUs)
		{
		if(iSentPDUs.IsEmpty())
			{
			if (iDeliverOutgoingDataAndSignalToSduQWhenDone && iPendingSentPDUs.IsEmpty())
				{
				iSDUQueue.DataControllerDeliveredOutgoingData();
				}
			else
				{
				iTimerMan.StartMonitorTimer();
				}
			}
		else
			{
			iTimerMan.StopMonitorTimer();
			iTimerMan.StartAckTimer();
			}
		}
	}

TInt CL2CapDataReTxController::HandleOutgoingIFrame(HIFramePDU* aIFrame)
	{
	LOG_FUNC
	aIFrame->SetPDUCID(iRemoteCID);
	
	// When sending a retransmission iRetransmitSentPDUs = ETrue.
	// When doing a retransmission the I-Frame has already had its Tx Seq
	// value set and iRetransTxVal is incremented so that we will continue
	// iterating through the sent PDUs for retransmission.
	// When sending a new I-Frame iNextTxSeq is used to get the next
	// brand new Tx Seq value (given the limitations of wrapping round the number
	// space).
	if (!iRetransmitSentPDUs)
		{
		aIFrame->SetTxSeqNumber(iNextTxSeq);
		iNextTxSeq = Mod64(iNextTxSeq + 1);
		}
	else
		{
		++iRetransTxVal;
		}
	
	LOG(_L("FEC CL2CapDataReTxController::HandleOutgoingIFrame"))		
	LOG1(_L("\tTxSeqNumber = %d"), (aIFrame->TxSeqNumber()))

	// If flow controlled off - don't acknowledge any new I-Frames
	aIFrame->SetReqSeqNumber(iIncomingSduQFull ? iLastAckSentRxSeqNum : iExpectedTxSeq);

	// Set the retransmission disable bit if flow control is on.
	aIFrame->SetRetransmitDisable(iIncomingSduQFull);
	aIFrame->CalculateAndSetFCS();

	aIFrame->iLink.Deque(); // may be on Sent PDU list if it's a retransmission
	aIFrame->SetPduOwner(this);
	iPendingSentPDUs.AddLast(*aIFrame);
	if ((iSentPDUs.IsEmpty()) && iTimerMan.IsAckTimerRunning())
		{
		iRestartAckTimer = EFalse;
		iTimerMan.StartMonitorTimer();
		}

	// Cancel the peer ack timer.
	iTimerMan.StopSendPeerAckTimer();
	iSendAckToPeer = EFalse;

	// Store the last acknowledged frame index.
	if(!iIncomingSduQFull)
		{
		iLastAckSentRxSeqNum = iExpectedTxSeq;
		}
	LOG1(_L("\tLastAckSentRxSeqNum = %d"), iLastAckSentRxSeqNum)

	return KErrNone;
	}

TBool CL2CapDataReTxController::DeliverOutgoingDataAndSignalToSduQWhenDone()
	{
	LOG_FUNC
	iDeliverOutgoingDataAndSignalToSduQWhenDone = ETrue;
	// Returning true means we don't have any outstanding data to deliver and
	// hence can be deleted immediately.
	return iPendingSentPDUs.IsEmpty() && iSentPDUs.IsEmpty();
	}


RL2CapRetransmissionModeTimerManager::RL2CapRetransmissionModeTimerManager(MRetransmissionModeTimerClient& aClient)
 :	iClient(aClient) 
	{
	LOG_FUNC
	}

void RL2CapRetransmissionModeTimerManager::Close()
	{
	LOG_FUNC
	CancelFECTimer();
	StopSendPeerAckTimer();
	}

void RL2CapRetransmissionModeTimerManager::StartMonitorTimer()
	{
	LOG_FUNC
	// Monitor timer preempts the Ack timer, so we can stop whichever we have running.
	CancelFECTimer();
	
	TCallBack cb(FECTimerExpired, this);
	iFECTimerEntry.Set(cb);
	
	// Set the timeout.  The value is in Milliseconds.
	BTSocketTimer::Queue(iClient.MonitorTimeout()*1000, iFECTimerEntry);
	iFECTimerState = EMonitorTimerRunning;
	}

void RL2CapRetransmissionModeTimerManager::StartAckTimer()
	{
	LOG_FUNC

	if (iFECTimerState != EMonitorTimerRunning)
		// Ack timer can't be run when the Monitor timer is running.
		{
		CancelFECTimer();
	
		TCallBack cb(FECTimerExpired, this);
		iFECTimerEntry.Set(cb);
	
		// Set the timeout.  The value is in Milliseconds.
		BTSocketTimer::Queue(iClient.RetransmissionTimeout()*1000, iFECTimerEntry);
		iFECTimerState = EAckTimerRunning;
		}
	}

void RL2CapRetransmissionModeTimerManager::StopMonitorTimer()
	{
	LOG_FUNC
	if (iFECTimerState == EMonitorTimerRunning)
		{
		CancelFECTimer();
		}
	}

void RL2CapRetransmissionModeTimerManager::StopAckTimer()
	{
	LOG_FUNC
	if (iFECTimerState == EAckTimerRunning)
		{
		CancelFECTimer();
		}
	}

void RL2CapRetransmissionModeTimerManager::CancelFECTimer()
	{
	LOG_FUNC
	// Cancel either the monitor or ack timer.
	if(iFECTimerState != EFECTimerIdle)
		{
		// Cancel the current outgoing timer.
		BTSocketTimer::Remove(iFECTimerEntry);
		iFECTimerState = EFECTimerIdle;
		}
	}

void RL2CapRetransmissionModeTimerManager::HandleFECTimerExpired()
	{
	LOG_FUNC
	// Call the appropriate method to handle this expiry.
	switch(iFECTimerState)
		{
		case EFECTimerIdle:
			Panic(EL2CAPInvalidDataControllerTimerState);
			break;
			
		case EMonitorTimerRunning:
			iFECTimerState = EFECTimerIdle;
			iClient.MonitorTimerExpired();
			break;
			
		case EAckTimerRunning:
			iFECTimerState = EFECTimerIdle;
			iClient.AckTimerExpired();
			break;

		default:
			Panic(EL2CAPInvalidDataControllerTimerState);
			break;
		}
	}

void RL2CapRetransmissionModeTimerManager::HandleSendPeerAckTimerExpired()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iSendPeerAckTimerRunning, Panic(EL2CAPInvalidDataControllerTimerState));
	iSendPeerAckTimerRunning = EFalse;
	iClient.SendPeerAckTimerExpired();
	}

TBool RL2CapRetransmissionModeTimerManager::StartSendPeerAckTimer()
	{
	LOG_FUNC
	// This timer should only be started if the peer is using a long re-transmission timer.
	__ASSERT_ALWAYS(!iSendPeerAckTimerRunning, Panic(EL2CAPInvalidDataControllerTimerState));
	
	TInt peerAckTimer = iClient.PeerRetransmissionTimeout() - KAveTimeToTransmitAckToPeer;
	if(peerAckTimer > KMinimumPeerAckTimeout)
		{
		TCallBack cb(SendPeerAckTimerExpired, this);
		iSendPeerAckTimerEntry.Set(cb);

		// Set the timeout.  The value os in Milliseconds.
		BTSocketTimer::Queue(peerAckTimer*1000, iSendPeerAckTimerEntry);
		iSendPeerAckTimerRunning = ETrue;
		}
	return iSendPeerAckTimerRunning;
	}

void RL2CapRetransmissionModeTimerManager::StopSendPeerAckTimer()
	{
	LOG_FUNC
	if (iSendPeerAckTimerRunning)
		{
		BTSocketTimer::Remove(iSendPeerAckTimerEntry);
		iSendPeerAckTimerRunning = EFalse;
		}
	}

/*static*/ TInt RL2CapRetransmissionModeTimerManager::SendPeerAckTimerExpired(TAny* aTimerMan)
	{
	LOG_STATIC_FUNC
	RL2CapRetransmissionModeTimerManager* timerMan = reinterpret_cast<RL2CapRetransmissionModeTimerManager*>(aTimerMan);
	timerMan->HandleSendPeerAckTimerExpired();
	return EFalse;
	}

/*static*/ TInt RL2CapRetransmissionModeTimerManager::FECTimerExpired(TAny* aTimeMan)
	{
	LOG_STATIC_FUNC
	RL2CapRetransmissionModeTimerManager* timeMan = reinterpret_cast<RL2CapRetransmissionModeTimerManager*>(aTimeMan);
	timeMan->HandleFECTimerExpired();
	return EFalse;
	}
