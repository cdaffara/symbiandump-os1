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

#include "L2CapSDUQueue.h"

#include "L2CapDataController.h"
#include "l2util.h"
#include "l2sap.h"

#include "L2CapDebugControlInterface.h"
#include "btsockettimer.h"
#include <es_prot_internal.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP_SDU_Q);
#endif

CL2CapSDUQueue* CL2CapSDUQueue::NewLC(CL2CAPConnectionSAP& aL2CapSap,
		                              TL2CAPPort aLocalCID,
		                              TL2CAPPort aRemoteCID,
                                      CL2CAPMux& aMuxer,
	                                  TUint16 aPDUSize,
									  TUint8 aOutboundQueueSize,
                                      TL2CapDataControllerConfig* aConfig,
	                                  TUint16 aFlushTimeout, 
		                              TUint16 aMaxOutgoingMTU,
                               		  TUint16 aMaxIncomingMTU,
                               		  TBool aCanDropSdus)
	{
	LOG_STATIC_FUNC
	CL2CapSDUQueue* self = new(ELeave) CL2CapSDUQueue(aL2CapSap, aOutboundQueueSize, aPDUSize, aFlushTimeout, aMaxOutgoingMTU, aMaxIncomingMTU, aCanDropSdus);
	CleanupStack::PushL(self);
	self->ConstructL(aLocalCID, aRemoteCID, aMuxer, aConfig);
	return self;
	}
		                                  
CL2CapSDUQueue* CL2CapSDUQueue::NewL(CL2CAPConnectionSAP& aL2CapSap,
		                             TL2CAPPort aLocalCID,
		                             TL2CAPPort aRemoteCID,
                                     CL2CAPMux& aMuxer,
	                                 TUint16 aPDUSize,
									 TUint8 aOutboundQueueSize,
                                     TL2CapDataControllerConfig* aConfig,
	                                 TUint16 aFlushTimeout, 
		                             TUint16 aMaxOutgoingMTU,
                               		 TUint16 aMaxIncomingMTU,
                               		 TBool aCanDropSdus)
	{
	LOG_STATIC_FUNC
	CL2CapSDUQueue* self = NewLC(aL2CapSap, aLocalCID, aRemoteCID, aMuxer, aPDUSize, aOutboundQueueSize, aConfig, aFlushTimeout, aMaxOutgoingMTU, aMaxIncomingMTU, aCanDropSdus);
	CleanupStack::Pop();
	return self;
	}	                                 
 
void CL2CapSDUQueue::ConstructL(TL2CAPPort aLocalCID, TL2CAPPort aRemoteCID, CL2CAPMux& aMuxer, TL2CapDataControllerConfig* aConfig)
	{
	LOG_FUNC
	iDataController = CL2CapBasicDataController::NewL(aLocalCID, aRemoteCID, aMuxer, *this, aConfig);

	TCallBack canSendCB(CanSendAsyncCallBack, this);
	iCanSendAsyncCallBack = new (ELeave)CAsyncCallBack(canSendCB, EActiveMedPriority);
	TCallBack sdusSentCB(SDUsSentAsyncCallBack, this);
	iSDUSentAsyncCallBack = new (ELeave)CAsyncCallBack(sdusSentCB, EActiveLowPriority);
	TCallBack qClosingCB(QueueClosingAsyncCallBack, this);
	iQueueClosingCallBack = new (ELeave)CAsyncCallBack(qClosingCB, EActiveHighPriority);
	}
	

CL2CapSDUQueue::CL2CapSDUQueue(CL2CAPConnectionSAP& aL2CapSap,
                               TUint8 aOutboundQueueSize,
                               TUint16 aPDUSize,
                               TUint16 aFlushTimeout, 
                               TUint16 aMaxOutgoingMTU,
                               TUint16 aMaxIncomingMTU,
                               TBool aCanDropSdus)
 : iL2CapSap(aL2CapSap),
   iOutboundQueueSize(aOutboundQueueSize),
   iOutgoingSDUs(_FOFF(CL2CapSDU, iLink)),
   iOutgoingPulledSDUs(_FOFF(CL2CapSDU, iLink)),
   iFlushTimeout(aFlushTimeout),
   iCurrentPDUSize(aPDUSize),
   iMaximumPDUSize(aPDUSize),
   iNegotiatedPDUSize(aPDUSize),
   iMaxOutgoingMTU(aMaxOutgoingMTU),
   iMaxIncomingMTU(aMaxIncomingMTU),
   iCanDropSdus(aCanDropSdus)
	{
	LOG_FUNC
	}
	
CL2CapSDUQueue::~CL2CapSDUQueue()
	{
	LOG_FUNC
	TDblQueIter<CL2CapSDU> ogSDUIter(iOutgoingSDUs);
	CL2CapSDU* sduPtr;
	while((sduPtr = ogSDUIter++) != NULL)
		{
		delete sduPtr;
		}

	TDblQueIter<CL2CapSDU> ogPulledSDUIter(iOutgoingPulledSDUs);
	while((sduPtr = ogPulledSDUIter++) != NULL)
		{
		delete sduPtr;
		}

	// Free any incoming SDU's in the list.
	iIncomingSDUs.Free();
	iCurrentIncomingSDU.Free();
	
	// If the outgoing queue has not been closed then the
	// data controller needs to be deregistered. iDataController
	// can be NULL if CL2CapSDUQueue is created and deleted immediately
	if(iDataController)
		{
		iDataController->DeregisterFromMuxer();
		}
		
	delete iDataController;
	delete iCanSendAsyncCallBack;
	delete iSDUSentAsyncCallBack;
	delete iQueueClosingCallBack;
	
	CancelOutgoingQueueClosingTimer();
	}

void CL2CapSDUQueue::ProcessFlushTimerExpiry(CL2CapSDU& aL2CapSDU)
	{
	LOG_FUNC
	CL2CapSDU* sduPtr;
	TBool done = EFalse;
	TBool dataPlaneFlushRequired = EFalse;

	if(!iOutgoingSDUs.IsEmpty())
		{
		// Check the SDU that is first in line to be sent.  It may
		// have already been partially sent in which case it must be
		// flushed.  If it has not been sent at all then just delete it. 
		sduPtr = iOutgoingSDUs.First();
		
		if(sduPtr == &aL2CapSDU)
			{
			// The flushed SDU is first in line to be sent.
			if(sduPtr->IsSDUEmpty())
				{
				// None of the SDU has been pulled yet.
				delete sduPtr;
				L2CAP_DEBUG(UpdateFlushCounters(L2capDebugInfo::ESDUFlushQueuedSDUFlushed));
				}
			else
				{
				// Some of the SDU has been sent.  Remove it from
				// the OG queue so no more is sent.
				sduPtr->iLink.Deque();
				iOutgoingPulledSDUs.AddLast(*sduPtr);
				dataPlaneFlushRequired = ETrue;
				L2CAP_DEBUG(UpdateFlushCounters(L2capDebugInfo::ESDUFlushPartialSentSDUFlushed));
				}
			done = ETrue;
			}

		// If its not the first SDU check the rest of the outbound queue.
		TDblQueIter<CL2CapSDU> ogSDUIter(iOutgoingSDUs);
		// Start from 2nd element in the queue.
		ogSDUIter++;
		
		while(!done && ((sduPtr = ogSDUIter++) != NULL))
			{
			if(sduPtr == &aL2CapSDU)
				{
				// Flushed SDU found.  None of this SDU has been
				// sent yet so it can just be deleted from the
				// outbound queue.
				delete sduPtr;
				done = ETrue;
				L2CAP_DEBUG(UpdateFlushCounters(L2capDebugInfo::ESDUFlushQueuedSDUFlushed));
				}
			}
		}
		
	if(done)
		{
		// The SDU has been found and removed from the outgoing
		// queue.			
		SDURemovedFromOutboundQueue();
		}
	else
		{
		// If the SDU is not found in the outgoing queue it must have already 
		// been sent
		dataPlaneFlushRequired = ETrue;
		L2CAP_DEBUG(UpdateFlushCounters(L2capDebugInfo::ESDUFlushSentSDUFlushed));
		}

	if(iDataController && dataPlaneFlushRequired)
		{
		iDataController->ProcessFlushTimerExpiry();
		}
	}


HL2CapPDU* CL2CapSDUQueue::GetPDU()
	{
	LOG_FUNC
	HL2CapPDU* pduPtr = NULL;
	if(!iOutgoingSDUs.IsEmpty())
		{
		CL2CapSDU* sduPtr = iOutgoingSDUs.First();
		// If the outbound queue is suspended, and this is the start
		// of a new SDU.  Then don't send a PDU.
		if(!(iOutboundQueueSuspended && sduPtr->CurrentPDUIsFirstPDU()))
			{
			TBool lastPDU = sduPtr->GetPDU(pduPtr);
			if(lastPDU)
				{
				sduPtr->iLink.Deque();
				iOutgoingPulledSDUs.AddLast(*sduPtr);
				SDURemovedFromOutboundQueue();
				}
			}
		}
	return pduPtr;
	}
	
void CL2CapSDUQueue::PutBFramePDU(RMBufChain& dataFrame)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iDataController->IsBasicDataVersion(), 
	                Panic(EL2CapPDUInvalidFrameType));

	// Trim off the PDU header and store the SDU data.
	HBFramePDU::RemoveHeaderBytes(dataFrame);
	iIncomingSDUs.Append(dataFrame);
	iCurrentInboundQueueLength++;
	iL2CapSap.NewData();
	}
	
TInt CL2CapSDUQueue::PutIFramePDU(RMBufChain& aDataFrame)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iDataController->IsBasicDataVersion() == EFalse, 
	                Panic(EL2CapPDUInvalidFrameType));

	TL2CapSAR sar = HIFramePDU::SAR(aDataFrame);

	if (sar == EStartOfL2CapSDU)
		{
		// SDU length and minimal StartOfSdu I-Frame length as per 2.1 Core Spec Addendum 1:
		// 3.3.7 Invalid Frame Detection Algorithm steps 4a&c.
		TInt err = HIFramePDU::CheckStartSduLength(aDataFrame, iMaxIncomingMTU);
		if (err != KErrNone)
			{
			aDataFrame.Free();
			return err;
			}
		}
	
	if(HIFramePDU::IsStartOfSDU(aDataFrame))
		{
		// TODO: Commented out until RTM and FC are fixed not to call this method
		// once they're notified we're full.
//		__ASSERT_DEBUG(iCurrentInboundQueueLength < KInboundQueueFlowCtrlThreshold,
//					   Panic(EL2CAPPutIFrameCalledWhenSDUQFull));

		if(!iCurrentIncomingSDU.IsEmpty())
			{
			aDataFrame.Free();
			// The previous SDU has not been completed.  Take the
			// required action, and remove the SDU from the queue.
			return HandleIncompleteSDU(iCurrentIncomingSDU);
			}

		// A new SDU needs to be started.
		if(sar == EStartOfL2CapSDU)
			{
			iCurrentIncomingSDULength = HIFramePDU::SDUSize(aDataFrame);
			}
		else
			{
			// Unsegmented PDU.
			iCurrentIncomingSDULength = static_cast<TUint16>(HIFramePDU::PDUPayloadLength(aDataFrame) - (HL2CapPDU::KControlFieldLength + HL2CapPDU::KFCSFieldLength));
			}
			
		HIFramePDU::RemoveHeaderAndFCSBytes(aDataFrame);
		iCurrentIncomingSDU.Assign(aDataFrame);
		}
	else
		{
		HIFramePDU::RemoveHeaderAndFCSBytes(aDataFrame);

		// The existing SDU should not be empty.
		if(iCurrentIncomingSDU.IsEmpty())
			{
			aDataFrame.Free();
			return HandleIncompleteSDU(aDataFrame);
			}
		else
			{
			// Add to the existing SDU.
			iCurrentIncomingSDU.Append(aDataFrame);

			if(sar == EEndOfL2CapSDU)
				{
				// Check the expected length matches the actual length.
				if(iCurrentIncomingSDU.Length() != iCurrentIncomingSDULength)
					{
					return HandleIncompleteSDU(iCurrentIncomingSDU);
					}
				}
			}
		}

	if(iCurrentIncomingSDU.Length() == iCurrentIncomingSDULength)
		{
		// Note: the two lines below should sit next to each other to be atomic wrt.
		// a call to Read(), in whose context we may be executing right now.
		// (Read() frees up space in the queue, calls iDataController->SetIncomingSduQFull,
		// which in turn calls PutIFramePDU for buffered frames).
		iIncomingSDUs.Append(iCurrentIncomingSDU);
		iCurrentInboundQueueLength++;
		
		if(iCurrentInboundQueueLength >= KInboundQueueFlowCtrlThreshold)
			{
			iDataController->SetIncomingSduQFull(ETrue);
			}
			
		// SDU is complete, signal new data.
		iL2CapSap.NewData();		
		}
	else
		{
		if(iCurrentIncomingSDU.Length() > iCurrentIncomingSDULength)	
			{
			// The incoming SDU has exceeded the length.
			return HandleIncompleteSDU(iCurrentIncomingSDU);
			}
		}
	return KErrNone;
	}

TInt CL2CapSDUQueue::Write(RMBufChain& aSDUData)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(aSDUData.Length() <= iMaxOutgoingMTU)
		{
		CL2CapSDU* sduPtr = NULL;

		if(!iOutboundSendingBlocked && 
		   (iCurrentOutboundQueueLength < iOutboundQueueSize))
			{
			TRAP(rerr, sduPtr = CL2CapSDU::NewL(aSDUData, *this, iCurrentPDUSize, iDataController->IsBasicDataVersion(), iFlushTimeout));   

			if(rerr == KErrNone)
				{
				iOutgoingSDUs.AddLast(*sduPtr);
				iCurrentOutboundQueueLength++;
				
				iDataController->OutgoingPduAvailableOnSduQ();
				}
			}
		else
			{
			iOutboundSendingBlocked = ETrue;
			rerr = KErrNotReady;
			}
		}
	else
		{
		// Attempt to send a SDU larger than the negotiated MTU size.
		rerr = KErrTooBig;
		}
		
	return rerr;
	}

void CL2CapSDUQueue::FlushCurrentlyAssembledSdu()
	{
	LOG_FUNC
	iCurrentIncomingSDU.Free();
	iCurrentIncomingSDULength = 0;
	}

void CL2CapSDUQueue::FlushOldestIncomingSdu()
	{
	LOG_FUNC
	if (!iIncomingSDUs.IsEmpty())
		{
		RMBufChain sdu;
		//Check on return value of iIncomingSDUs.Remove() is unnecessary, since we've checked iIncomingSDUs.IsEmpty()
		static_cast<void>(iIncomingSDUs.Remove(sdu));
		sdu.Free();
		iCurrentInboundQueueLength--;
		}
	}

void CL2CapSDUQueue::Read(RMBufChain& readData)
	{
	LOG_FUNC
	if(!iIncomingSDUs.IsEmpty())
		{
		//Check on return value of iIncomingSDUs.Remove() is unnecessary, since we've checked iIncomingSDUs.IsEmpty()
		static_cast<void>(iIncomingSDUs.Remove(readData));
		iCurrentInboundQueueLength--;

		if (iDataController && iCurrentInboundQueueLength < KInboundQueueFlowCtrlThreshold)
			{
			// Note: ERTM will call PutIFramePDU() for each buffered frame, in the context of the
			// call below.
			// Conclusion 1: flow control may be on again when returning from this call.
			// Conclusion 2: this method (Read) and PutIFramePDU need to be reentrant with respect
			// to each other: iIncomingSDUs.Append/Remove and corresponding iCurrentInboundQueueLength++/--
			// need to be "atomic".
			iDataController->SetIncomingSduQFull(EFalse);
			}

		// Check if this is the last SDU in the incoming queue.
		if(iSDUQueuesClosing && iIncomingSDUs.IsEmpty())
			// Note: see comment in CloseSDUQueue for why we don't need to check for incoming data
			// in the data controller.
			{
			iIncomingQueueClosed = ETrue;
			TryToCloseQueue();
			}
		}
	}
					
void CL2CapSDUQueue::CloseSDUQueue(TIncomingSDUQueueCloseAction aIncomingQueueAction, 
	                               TOutgoingSDUQueueCloseAction aOutgoingQueueAction)
	{
	LOG_FUNC
	iSDUQueuesClosing = ETrue;
	if (aIncomingQueueAction == EImmediatelyCloseIncomingQueue ||
		iIncomingSDUs.IsEmpty())
		// ^- note it's the remote's responsibility to only initiate disconnection when
		// all it's outgoing data has been delivered, which on receiveing side means
		// we don't have to check the data controller for pending incoming data -
		// everything that's been acknowledged is always in the SDU Q.
		{
		iIncomingQueueClosed = ETrue;
		}
		
	if (aOutgoingQueueAction == EImmediatelyCloseOutgoingQueue)
		{
		iOutgoingQueueClosed = ETrue;
		}
	else if (iOutgoingSDUs.IsEmpty() &&
			 !iOutgoingQueueClosed)
		//   ^- iDataController may be NULL if the outgoing Q has been closed.
		//   It'll happen if the remote initiates the disconnection, we close the outgoing
		//   Q and delete the data controller but still have unread SDUs in the incoming
		//   Q, and then our application calls Shutdown without reading the SDUs.
		{
		// Some of the data may still be waiting for transmission/acknowledgement
		// in the data controller's outgoing queues. If it's a reliable mode data controller
		// then we need to make sure all data has been delivered.
		iOutgoingQueueClosed = iDataController->DeliverOutgoingDataAndSignalToSduQWhenDone();
		}
	else // draining & have outgoing SDUs
		{
		// Start a guard timer to ensure the shutdown of the outgoing queue.
		StartOutgoingQueueClosingTimer();
		}
		
	TryToCloseQueue();
	}

void CL2CapSDUQueue::DataControllerDeliveredOutgoingData()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iSDUQueuesClosing && iOutgoingSDUs.IsEmpty(),
				   Panic(EL2CAPStrayCallToDataControllerDeliveredOutgoingData));

	// Use the async callback instead of deleting directly - spares us having to inform
	// the data controller that it's been deleted and thus having a special exit path
	// inside it.
	iSDUSentAsyncCallBack->CallBack();
	}

void CL2CapSDUQueue::TryToCloseQueue()
	{
	LOG_FUNC
	// If the outgoing queue is not closed nothing can
	// be done.
	if(iOutgoingQueueClosed)
		{
		// The outgoing queue is closed.  Remove the
		// data controller from the muxer list, it is no longer required.
		CancelOutgoingQueueClosingTimer();

		if(iDataController)
			{
			// If we initiated the Shutdown, then we don't care about incoming data.
			// If the remote initiated the shutdown, then it has delivered the data
			// it wants us to read and it's now in the incoming SDU Q (because everything that
			// we've acknowledged is). In both cases we can delete the data controller now.
			iDataController->DeregisterFromMuxer();
			delete iDataController;
			iDataController = NULL;
			}
			
		// If both queues are closed signal back to the SAP.
		if(iIncomingQueueClosed)
			{
			iQueueClosingCallBack->CallBack();
			}
		}
	}

/*static*/ TInt CL2CapSDUQueue::QueueClosingAsyncCallBack(TAny* aSDUQueue)
	{
	LOG_STATIC_FUNC
	CL2CapSDUQueue* sduQueue = static_cast<CL2CapSDUQueue*>(aSDUQueue);
	sduQueue->SDUQueueClosed();
	return EFalse;
	}
	
void CL2CapSDUQueue::SDUQueueClosed()
	{
	LOG_FUNC
	iL2CapSap.SDUQueueClosed();
	}	

void CL2CapSDUQueue::SDURemovedFromOutboundQueue()
	{
	LOG_FUNC
	iCurrentOutboundQueueLength--;
	
	if(iOutboundSendingBlocked &&
	   (iCurrentOutboundQueueLength < iOutboundQueueSize))
	   	{
		iOutboundSendingBlocked = EFalse;
		iCanSendAsyncCallBack->CallBack();
	   	}

	// Check if any sent SDUs need deleting and if we can close the outgoing queue
	// now (if we're in shutdown at all).
	iSDUSentAsyncCallBack->CallBack();
	}


/*static*/ TInt CL2CapSDUQueue::CanSendAsyncCallBack(TAny* aSDUQueue)
	{
	LOG_STATIC_FUNC
	CL2CapSDUQueue* sduQueue = static_cast<CL2CapSDUQueue*>(aSDUQueue);
	sduQueue->CanSend();
	return EFalse;
	}

void CL2CapSDUQueue::CanSend()
	{
	LOG_FUNC
	iL2CapSap.CanSend();
	}

/*static*/ TInt CL2CapSDUQueue::SDUsSentAsyncCallBack(TAny* aSDUQueue)
	{
	LOG_STATIC_FUNC
	CL2CapSDUQueue* sduQueue = static_cast<CL2CapSDUQueue*>(aSDUQueue);
	sduQueue->SDUsSent();
	return EFalse;
	}
	
void CL2CapSDUQueue::SDUsSent()
	{
	LOG_FUNC
	// Check the pulled SDU queue and remove all fully sent SDU's. 
	CL2CapSDU* sdu = NULL;
	while(!iOutgoingPulledSDUs.IsEmpty())
		{		
		sdu = iOutgoingPulledSDUs.First();
		if(sdu->IsSDUEmpty())
			{
			delete sdu;
			}
		else
			{
			break;
			}
		}

	// If the queue is waiting to close and nothing is left to be done close it.	
	if(iSDUQueuesClosing && iOutgoingSDUs.IsEmpty())
		{
		// Take the data controller's outgoing PDU queues into account.
		// In reliable modes all data needs to be acknowledged by the peer before we can close.
		iOutgoingQueueClosed = iDataController->DeliverOutgoingDataAndSignalToSduQWhenDone();
		TryToCloseQueue();
		}
	}

/*static*/ TInt CL2CapSDUQueue::OutgoingQueueClosingTimerExpired(TAny *aSDUQueue)
	{
	LOG_STATIC_FUNC
	CL2CapSDUQueue* sduQueue = reinterpret_cast<CL2CapSDUQueue*>(aSDUQueue);
	sduQueue->HandleOutgoingQueueClosingTimerExpired();
	return EFalse;
	}

void CL2CapSDUQueue::StartOutgoingQueueClosingTimer()
	{
	LOG_FUNC
	if(!iOutgoingQueueClosingTimerRunning)
		{
		TCallBack cb(OutgoingQueueClosingTimerExpired, this);
		iOutgoingQueueClosingTimer.Set(cb);
		BTSocketTimer::Queue(KOutgoingQueueClosingTimer * KL2ProtocolSecondTimerMultiplier, iOutgoingQueueClosingTimer);
		iOutgoingQueueClosingTimerRunning = ETrue;
		}
	}
	
void CL2CapSDUQueue::CancelOutgoingQueueClosingTimer()
	{
	LOG_FUNC
	if(iOutgoingQueueClosingTimerRunning)
		{
		BTSocketTimer::Remove(iOutgoingQueueClosingTimer);
		iOutgoingQueueClosingTimerRunning = EFalse;
		}	
	}
	
void CL2CapSDUQueue::HandleOutgoingQueueClosingTimerExpired()
	{
	LOG_FUNC
	iOutgoingQueueClosingTimerRunning = EFalse;
	
	// The outgoing queue closing timer has expired.  Force the outgoing
	// queue to close.  Pending data will NOT be sent.
	iOutgoingQueueClosed = ETrue;
	TryToCloseQueue();		
	}

TInt CL2CapSDUQueue::HandleIncompleteSDU(RMBufChain& aIncompleteSDU)
	{
	LOG_FUNC
	// This method is the starting point for reporting errored / incomplete SDU's to
	// the socket.  At present this functionality is not implemented and the buffer
	// is just freed.
	aIncompleteSDU.Free();
	iCurrentIncomingSDULength = 0;

	L2CAP_DEBUG(UpdateFlushCounters(L2capDebugInfo::EIncompleteErroredSDUReceived));

	// This function is always called when SAR bits in the received I-Frame are unexpected.
	// Need to close the connection as per 2.1 Core Spec Addendum 1: 3.3.7 Invalid Frame
	// Detection Algorithm point 4b.
	return iCanDropSdus ? KErrNone : KErrL2CAPIncomingSduSegmentationError;
	}

void CL2CapSDUQueue::ResumeSDUQueue(TL2CapDataControllerConfig* aConfig,
                    				TUint16 aFlushTimeout,
                    				TUint16 aPDUSize,
                    				TUint16 aMaxOutgoingMTU,
                    				TUint16 aMaxIncomingMTU)
	{
	LOG_FUNC
	// Update the data controller.
	iDataController->UpdateConfig(aConfig);
	
	// Update internal members.
	iFlushTimeout = aFlushTimeout;
	iMaxOutgoingMTU = aMaxOutgoingMTU;
	iMaxIncomingMTU = aMaxIncomingMTU;	
	iMaximumPDUSize = aPDUSize;
	iNegotiatedPDUSize = aPDUSize;

	TInt err = KErrNone;

	// If the max PDU size has been increased or remained the same
	// none of the existing SDU's need to be modified.
	if(iNegotiatedPDUSize < iCurrentPDUSize)
		{
		// For each completely unsent SDU.  Adjust the PDU size.
		// Check if there are any un-sent SDU's
		CL2CapSDU* sduPtr = NULL;
		if(!iOutgoingSDUs.IsEmpty())
			{
			// The first SDU on the outgoing queue might be partially sent.  If it is
			// it can't be re-segmented.
			TDblQueIter<CL2CapSDU> ogSDUIter(iOutgoingSDUs);
			while((sduPtr = ogSDUIter++) != NULL)
				{
				if(sduPtr->CurrentPDUIsFirstPDU())
					{
					err = sduPtr->ChangeSDUSegmentation(iDataController->IsBasicDataVersion(), iNegotiatedPDUSize);
					if(err != KErrNone)
						{
						ErrorD(err);
						delete sduPtr;
						break;
						}
					}
				}
			}
		}
	if (err == KErrNone)
		{
		iCurrentPDUSize = iNegotiatedPDUSize;
	
		// Allow the outbound queue to send again, and tell the data
		// plane if SDU's are available.
		iOutboundQueueSuspended = EFalse;
		if(iCurrentOutboundQueueLength)
			{
			iDataController->OutgoingPduAvailableOnSduQ();
			}
		} // ... else we're a zombie.
	}
	
void CL2CapSDUQueue::ErrorD(TInt aErrorCode)
	{
	LOG_FUNC
	// We'll get deleted down the call chain. 
	iL2CapSap.DataPlaneError(aErrorCode, MSocketNotify::TOperationBitmasks(MSocketNotify::EErrorSend | MSocketNotify::EErrorRecv));
	}

void CL2CapSDUQueue::UpdateChannelPriority(TUint8 aNewPriority)
	{
	LOG_FUNC
	if (iDataController)
		{
		iDataController->UpdateChannelPriority(aNewPriority);	
		}
	}

TBool CL2CapSDUQueue::IsBasicDataVersion() const
	{
	LOG_FUNC
	TBool rValue = EFalse;
	if (iDataController)
		{
		rValue = iDataController->IsBasicDataVersion();
		}
	return rValue;
	}

#ifdef _DEBUG
TInt CL2CapSDUQueue::GetDataPlaneConfig(TL2DataPlaneConfig& conf) const
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	// Populate the debug structure.
	if(iDataController)
		{
		rerr = iDataController->GetDataPlaneConfig(conf);
		if(rerr == KErrNone)
			{
			conf.iOutboundQueueSize = iOutboundQueueSize;
			conf.iFlushTimeout = iFlushTimeout;
			conf.iMaximumPDUSize = iCurrentPDUSize;
			conf.iMaxOutgoingMTU = iMaxOutgoingMTU;
			conf.iMaxIncomingMTU = iMaxIncomingMTU;
			}
		}
	else
		{
		rerr = KErrNotReady;
		}
	return rerr;
	}

void CL2CapSDUQueue::PretendIncomingSduQFull(TBool aIncomingSduQFull)
	{
	LOG_FUNC
	if (iDataController)
		{
		iDataController->SetIncomingSduQFull(aIncomingSduQFull);
		}
	}
#endif
