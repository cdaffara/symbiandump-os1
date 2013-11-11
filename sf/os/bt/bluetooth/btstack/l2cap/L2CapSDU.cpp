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

#include "btsockettimer.h"

#include "L2CapSDU.h"
#include "L2CapPDU.h"
#include "l2util.h"
#include "L2CapDebugControlInterface.h"

#ifdef _DEBUG
#include "L2CapDebugControlInterface.h"
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP_SDU);
#endif

CL2CapSDU* CL2CapSDU::NewLC(RMBufChain& aSDUData,
                            ML2CapSDUHandler& aParent,
			                TUint16 aPDUSize,
                            TBool aIsBasicDataVersion,
			                TUint16 aTimerDuration)
	{
	LOG_STATIC_FUNC
	CL2CapSDU* self = new(ELeave) CL2CapSDU(aParent);
	CleanupStack::PushL(self);
	self->ConstructL(aSDUData, aIsBasicDataVersion, aTimerDuration, aPDUSize);
	return self;
	}


CL2CapSDU* CL2CapSDU::NewL(RMBufChain& aSDUData,
		                   ML2CapSDUHandler& aParent,
			               TUint16 aPDUSize,
		                   TBool aIsBasicDataVersion, 
			               TUint16 aTimerDuration)
	{
	LOG_STATIC_FUNC
	CL2CapSDU* self = NewLC(aSDUData, aParent, aPDUSize, aIsBasicDataVersion, aTimerDuration);
	CleanupStack::Pop();
	return self;
	}

CL2CapSDU::CL2CapSDU(ML2CapSDUHandler& aParent)
 : iParent(aParent),
   iPDUs(_FOFF(HL2CapPDU, iLink)),
   iCurrentPDU(iPDUs)
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ESDU,
	                             L2capDebugInfo::EAllocated));
	}

void CL2CapSDU::ConstructL(RMBufChain& aSDUData, TBool aIsBasicDataVersion, TUint16 aTimerDuration, TUint16 aPDUSize)
	{
	LOG_FUNC
	LEAVEIFERRORL(SegmentSDUIntoPDUs(aSDUData, aIsBasicDataVersion, aPDUSize));
	
	// Start the flush timer if required.
	StartFlushTimer(aTimerDuration);
	}

CL2CapSDU::~CL2CapSDU()
	{
	LOG_FUNC
	iLink.Deque();
	iCurrentPDU.SetToFirst();
	HL2CapPDU* pduPtr;
	while(iCurrentPDU)
		{
		pduPtr = iCurrentPDU++;
		pduPtr->iLink.Deque();
		delete pduPtr;
		}

	if(iFlushTimerRunning)
		{
		BTSocketTimer::Remove(iFlushTimerEntry);
		}	

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ESDU,
	                             L2capDebugInfo::EDeleted));
	}

/*
 * Function to return any additional SDU L2CAP overhead. This is in addition to any PDU
 * overhead as defined by HL2CapPDU::GetPDUOverhead.
 */
/*static*/ TInt CL2CapSDU::GetSDUOverhead(TBool aBasicMode)
	{
	LOG_STATIC_FUNC
	if (aBasicMode)
		{
		return HL2CapPDU::KPDUHeaderLength;
		}

	// Non-basic mode overhead
	return HL2CapPDU::KSDULengthFieldLength;
	}

TInt CL2CapSDU::SegmentSDUIntoPDUs(RMBufChain& aSDUData, TBool aIsBasicDataVersion, TUint16 aPDUPayloadSize)
	{
	LOG_FUNC
	TInt sduLength = aSDUData.Length();
	TInt rerr = KErrNone;
		
	if(aIsBasicDataVersion)
		{
		// This is basic mode.  The entire SDU should be placed into
		// a B-Frame PDU.
		HBFramePDU* pdu = HBFramePDU::New(aSDUData, aPDUPayloadSize);
		if(pdu)
			{
			iPDUs.AddLast(*pdu);
			L2CAP_DEBUG_PDU(PDUTimer(L2capDebugInfo::EBFrameCreated, pdu));
			}
		else
			{
			rerr = KErrNoMemory;
			}
		}
	else
		{
		// Check if the SDU needs to be segmented.
		if(aPDUPayloadSize >= sduLength)
			{
			HIFramePDU* pdu = HIFramePDU::New(aSDUData, EUnsegmentedL2CapSDU);
			if(pdu)
				{
				iPDUs.AddLast(*pdu);
				L2CAP_DEBUG_PDU(PDUTimer(L2capDebugInfo::EUnsegmentedFrameCreated, pdu));
				}
			else
				{
				rerr = KErrNoMemory;
				}				
			}
		else
			{
			// Work backwards through the SDU segmenting it into PDU's. The first segment will always
			// contain an extra SDU length field so this is taken into account when working out the 
			// size of the last segment below.
			TInt sduPosition = sduLength - ((sduLength + HL2CapPDU::KSDULengthFieldLength) % aPDUPayloadSize);

			// If the PDU size is a factor of the SDU length then start from the
			// SDU length - PDU size.
			if(sduPosition == sduLength)
				{
				sduPosition -= aPDUPayloadSize;
				}
												
			HIFramePDU* pdu = NULL;
			RMBufChain pduData;
			
			TRAP(rerr, aSDUData.SplitL(sduPosition, pduData));
			if(rerr == KErrNone)
				{
				pdu = HIFramePDU::New(pduData, EEndOfL2CapSDU);
				if(pdu)
					{
					sduPosition -= aPDUPayloadSize;
					iPDUs.AddFirst(*pdu);
					L2CAP_DEBUG_PDU(PDUTimer(L2capDebugInfo::EIFrameCreated, pdu));
					
					while(sduPosition > 0)
						{
						__ASSERT_DEBUG(sduPosition >= 0, Panic(EL2CAPSDUPositionNegativeDuringSegmentation));

						TRAP(rerr, aSDUData.SplitL(sduPosition, pduData));
						if(rerr == KErrNone)
							{
							pdu = HIFramePDU::New(pduData, EContinuationOfL2CapSDU);
							if(pdu)
								{
								sduPosition -= aPDUPayloadSize;
								iPDUs.AddFirst(*pdu);
								}
							else
								{
								rerr = KErrNoMemory;
								break;
								}
							}
						else
							{
							break;
							}							
						}
					
					if(rerr == KErrNone)
						{
						// Create and add the first PDU.
						pdu = HIFramePDU::New(aSDUData, EStartOfL2CapSDU);
						if(pdu)
							{
							pdu->SetSDUSize(static_cast<TUint16>(sduLength));
							iPDUs.AddFirst(*pdu);
							}
						else
							{
							rerr = KErrNoMemory;
							}
						}
					}
				else
					{
					rerr = KErrNoMemory;
					}				
				}
			}
		}
		
	if(rerr == KErrNone)
		{
		// Set the current PDU pointer to the first PDU.
		iCurrentPDU.SetToFirst();	
		}
	return rerr;
	}

TBool CL2CapSDU::GetPDU(HL2CapPDU*& aReturnedPDU)
	{
	LOG_FUNC
	TBool isLastPDU = EFalse;
	if(!iPDUs.IsEmpty())
		{
		aReturnedPDU = iCurrentPDU++;

		// TODO: this was put in as a fix and is in needed in general, but it also
		// actually hoses up some logic, i.e. CurrentPDUIsFirstPDU will always return
		// true. We either need a separate link for the list of PDUs inside the SDU,
		// or if possible just get rid of iCurrentPDU and have a iFirstPDU flag
		// set to one during initial segmentation.
		// Note - we the iCurrentPDU/iFirstPDU business is actually only used in two cases:
		// 1. to repack current SDU when PDU size has changed on a live connection - currently
		//    unused and unnecessary.
		// 2. on flush timer expiry, to mark already sent packets as flushed and purge the
		//    unsent ones - currently unused, but may get implemented in the future. However
		//    the marking of the packets may be done their current owner, which is a data
		//    controller or the muxer, so again this is unnecessary.
		// Conclusion: refactor to simplify.
		aReturnedPDU->iLink.Deque();

		if(iCurrentPDU == NULL)
			{
			// At the end of the PDU list.
			L2CAP_DEBUG_PDU(PDUTimer(L2capDebugInfo::EGetPDUCalled, aReturnedPDU));
			isLastPDU = ETrue;
			}
		}

	return isLastPDU;
	}
	

/*static*/ TInt CL2CapSDU::FlushTimerExpired(TAny* aL2CapSDU)
	{
	LOG_STATIC_FUNC
	CL2CapSDU* sdu = reinterpret_cast<CL2CapSDU*>(aL2CapSDU);
	sdu->HandleFlushTimerExpired();
	return EFalse;
	}
	
void CL2CapSDU::HandleFlushTimerExpired()
	{
	LOG_FUNC
	// Note that the timer is no longer running.
	iFlushTimerRunning = EFalse;

	// Any PDU's that have already been sent need to be
	// informed of the flush.
	TDblQueIter<HL2CapPDU> pduIter(iPDUs);
	TBool done = EFalse;

	HL2CapPDU* pduPtr;
	while((pduPtr = pduIter++) != NULL  && !done)
		{
		if(pduPtr == iCurrentPDU)
			{
			done = ETrue;
			}
		else
			{
			pduPtr->SetPDUFlushed();
			}
		}

	// The remaining PDU's (if any) that have not yet
	// been sent can be deleted. 
	while((pduPtr = iCurrentPDU++) != NULL)
		{
		pduPtr->iLink.Deque();
		delete pduPtr;
		}

	// Inform the SDU queue.
	iParent.ProcessFlushTimerExpiry(*this);
	}
	
TBool CL2CapSDU::IsSDUEmpty() const
	{
	LOG_FUNC
	return iPDUs.IsEmpty();
	}

	
TBool CL2CapSDU::CurrentPDUIsFirstPDU()
	{
	LOG_FUNC
	return iPDUs.IsFirst(iCurrentPDU);
	}

void CL2CapSDU::StartFlushTimer(TUint16 aTimerDuration)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iFlushTimerRunning, Panic(EL2CAPAttemptToRestartFlushTimer));
	
	// Only start the timer if the duration is valid.
	if(aTimerDuration >= TL2CapConfig::EMinDataObsolescenceTimeout && 
	   aTimerDuration != KInfiniteFlush)
		{
		TCallBack cb(FlushTimerExpired, this);
		iFlushTimerEntry.Set(cb);
		// Timer period is a factor of the duration parameter and 0.625ms
		BTSocketTimer::Queue(aTimerDuration*625, iFlushTimerEntry);
		iFlushTimerRunning = ETrue;
		}
	}
	
TInt CL2CapSDU::ChangeSDUSegmentation(TBool aIsBasicDataVersion, TUint16 aPDUSize)
	{
	LOG_FUNC
	__ASSERT_DEBUG(CurrentPDUIsFirstPDU(), Panic(EL2CAPAttemptToChangeSegmentationForPartiallySentSDU));

	TInt rerr = KErrNone;
	RMBufChain sduData;

	// Get the current SDU data, and delete all current
	// PDU's
	TDblQueIter<HL2CapPDU> iter(iPDUs);
	HL2CapPDU* pduPtr;
	while((pduPtr = iter++) != NULL)
		{
		pduPtr->AppendPayloadToBuffer(sduData);
		pduPtr->iLink.Deque();
		delete pduPtr;
		}
		
	rerr = SegmentSDUIntoPDUs(sduData, aIsBasicDataVersion, aPDUSize);
	
	return rerr;
	}


#ifdef _DEBUG
TInt CL2CapSDU::DebugManualFlush()
	{
	LOG_FUNC
	TInt rcode = -1;
	
	if(!IsSDUEmpty())
		{
		RMBuf* buf = iPDUs.First()->PDUBuffer().First();
		if(buf->Length() > 8)
			{
			rcode = buf->Get(8);
			}
		else
			{
			buf = buf->Next();
			if(buf)
				{
				rcode = buf->Get(0);
				}
			}
		}
		
	if(iFlushTimerRunning)
		{
		BTSocketTimer::Remove(iFlushTimerEntry);
		}
	HandleFlushTimerExpired();
	return rcode;
	}
#endif
