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

#include <bluetooth/logger.h>
#include <bluetoothav.h>
#include <es_mbuf.h>

#include "avctpPacketMgr.h" 
#include "Avctp.h"
#include "avctpmuxer.h"
#include "avctpconstants.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
#endif

using namespace SymbianAvctp;

/**
Static factory function for an AVCTP Packet Manager

  @internalComponent
  @leave KErrNoMemory if the new packet could not be allocated
  @return A pointer to an AVCTP Packet Manager
*/
CAvctpPacketMgr* CAvctpPacketMgr::NewL(CAvctpTransport& aMuxer,
										CAvctpProtocol& aProtocol)
	{
	LOG_STATIC_FUNC
	CAvctpPacketMgr* mgr = new(ELeave) CAvctpPacketMgr(aMuxer, aProtocol);
	CleanupStack::PushL(mgr);
	mgr->ConstructL();
	CleanupStack::Pop(mgr);
	return mgr;	
	}

void CAvctpPacketMgr::ConstructL()
	{
	LOG_FUNC
	iIncomingAssemblers[KAvctpPrimaryChannel] = CIncomingSduAssembler::NewL(*this,0);
	iOutgoingFragmenters[KAvctpPrimaryChannel] = COutgoingSduFragmenter::NewL(*this,0);	
	iIncomingAssemblers[KAvctpSecondaryChannel] = CIncomingSduAssembler::NewL(*this,1);
	iOutgoingFragmenters[KAvctpSecondaryChannel] = COutgoingSduFragmenter::NewL(*this,1);	
	}

/**
Default constructor for an AVCTP Packet Manager

  @internalComponent
*/
CAvctpPacketMgr::CAvctpPacketMgr(CAvctpTransport& aTransport, CAvctpProtocol& aProtocol)
	: iTransport(aTransport),
	  iProtocol(aProtocol)
	{
	LOG_FUNC
	}

/**
Default destructor for an AVCTP Packet Manager

  @internalComponent
*/
CAvctpPacketMgr::~CAvctpPacketMgr()
	{
	LOG_FUNC
	
	delete iIncomingAssemblers[0];
	delete iOutgoingFragmenters[0];
	delete iIncomingAssemblers[1];
	delete iOutgoingFragmenters[1];
	}

/**
Transfers ownership of the aOutgoingSdu to CAvctpPacketMgr if the write succeeded
i.e. didn't return 0

Note this function should not be used by other classes to send IPID responses. 
Use WriteIpid for this instead.
*/
void CAvctpPacketMgr::Write(HAvctpOutgoingSdu*& aOutgoingSdu, TInt aChannel)
	{
	LOG_FUNC
	
	iOutgoingFragmenters[aChannel]->Write(aOutgoingSdu);
	}
	
/**
This functions

Transfers ownership of the aOutgoingSdu to CAvctpPacketMgr if the write succeeded
i.e. didn't return 0

Note this function should only be used to send IPID responses. Use Write for all other
Sdus instead.
*/
TInt CAvctpPacketMgr::WriteIpid(HAvctpOutgoingSdu*& aOutgoingSdu)
	{
	LOG_FUNC
	
	Write(aOutgoingSdu, aOutgoingSdu->Channel());
	
	TInt ret = KErrNone;
	if (!aOutgoingSdu)
		{
		++iIpidSdusSent;
		LOG1(_L("Sent %d IPID responses"), iIpidSdusSent);
		if (iIpidSdusSent >= KMaximumIpidResponsesAllowed)
			{
			ret = KErrRemoteSentTooManyIpidSdus;
			}
		}
	else
		{
		ret = KErrMuxerBlocked;	// if the sdu is not null means it hasn't been sent.
		}
	return ret;
	}
	
/** 
This is the lower protocol notifying us it can now send more data
We check to see if we were in the middle of sending fragments and
if so we write all those we can.
@return EFalse if we've blocked the muxer o/w EFalse
*/
void CAvctpPacketMgr::CanSend(TInt aChannel)
	{
	LOG_FUNC

	iOutgoingFragmenters[aChannel]->CanSend();
	}

/**
We don't pass on the send / recv errors to the saps
because these errors only concern the packets we've
tried to send over L2CAP.

We don't do anything on fatal errors cause the muxer 
goes down in that situation and takes the packet mgr
with it.
*/
void CAvctpPacketMgr::SignalMuxerError(TInt /*aError*/,TUint aOperationMask)
	{
	LOG_FUNC

	if (aOperationMask & (MSocketNotify::EErrorSend))
		{
		iIncomingAssemblers[KAvctpPrimaryChannel]->Reset();
		iIncomingAssemblers[KAvctpSecondaryChannel]->Reset();
		}
	
	if (aOperationMask & (MSocketNotify::EErrorRecv))
		{
		iOutgoingFragmenters[KAvctpPrimaryChannel]->Reset();
		iOutgoingFragmenters[KAvctpSecondaryChannel]->Reset();
		}
	}
	
/**
This function transfers the ownership of aIncomingPdu from the
muxer to CAvctpPacketMgr
*/ 
TInt CAvctpPacketMgr::NewData(RMBufChain& aIncomingPdu, TInt aChannel)
	{
	LOG_FUNC

	TAvctpStartHeaderInfo headerInfo;
	TInt err = CAvctpPacket::ParseHeader(aIncomingPdu, headerInfo);
	if (err == KErrNone)
		{
		TRAP(err, iIncomingAssemblers[aChannel]->AddDataL(headerInfo, aIncomingPdu)); // transfers ownership of aIncomingPdu on success 
		if (err != KErrNone)
			{
			// We've OOM'd so drop packet
			aIncomingPdu.Free();
			
			// Don't reset iIncomingAssembler since it'll reset itself on 
			// the next start or normal packet if we missed a vital fragment here
			}
			
		err=KErrNone; //Want to read remaining packets, so don't pass an error back up.
		}
	else
		{
		// There was something wrong with the PDU so punish the remote
		Transport().Shutdown(err);
		aIncomingPdu.Free();
		err = KErrMuxerShutDown;
		}
		
	return err;
	}

TBool CAvctpPacketMgr::WouldLikeToSend()
	{
	return iOutgoingFragmenters[KAvctpPrimaryChannel]->HasData() || iOutgoingFragmenters[KAvctpSecondaryChannel]->HasData();  
	}
	
//
//
// implementation of COutgoingSduFragmenter
//
//

/**
Default constructor for an outgoing partial AVCTP Sdu
  @internalComponent
*/
COutgoingSduFragmenter::COutgoingSduFragmenter(CAvctpPacketMgr& aMgr, TInt aChannel)
	: iMgr(aMgr), iChannel(aChannel)
	{
	LOG_FUNC

	}

/**
Static factory function

  @internalComponent
  @leave KErrNoMemory if the new packet could not be allocated
  @return A pointer to an outgoing partial AVCTP Sdu
*/
COutgoingSduFragmenter* COutgoingSduFragmenter::NewL(CAvctpPacketMgr& aMgr, TInt aChannel)
	{
	LOG_STATIC_FUNC

	COutgoingSduFragmenter* packet = new(ELeave) COutgoingSduFragmenter(aMgr, aChannel);
	CleanupStack::PushL(packet);
	packet->ConstructL();
	CleanupStack::Pop(packet);
	return packet;
	}	

void COutgoingSduFragmenter::ConstructL()
	{
	LOG_FUNC
	
	TCallBack cb(SendAsyncCallBack, this);
	iSendAsyncCallBack = new(ELeave) CAsyncCallBack(cb, EActiveHighPriority);
	}

COutgoingSduFragmenter::~COutgoingSduFragmenter()
	{
	LOG_FUNC

	delete iSendAsyncCallBack;
	}	
	
void COutgoingSduFragmenter::Reset()
	{
	LOG_FUNC

	iSduData.Free();
	iCurrentWriteState = ENormal;
	// we don't have anymore data to send, so check for idle so that if the transport is idle, the 
	// timer will be set and when expires it will be destroyed.
	iMgr.Transport().CheckForIdle();
	}

TBool COutgoingSduFragmenter::HasData()
	{
	return iSduData.Length() > 0;
	}

/**
If we can take ownership of this we do, and kick off async send.  Otherwise
we do nothing and the SDU remains on the caller's queue.
*/
void COutgoingSduFragmenter::Write(HAvctpOutgoingSdu*& aOutgoingSdu)
	{
	LOG_FUNC

	if (iSduData.Length() == 0)
		{
		iSduData.Assign(aOutgoingSdu->Data());
		iAddr = aOutgoingSdu->BTAddr();
		iHeaderInfo = aOutgoingSdu->HeaderInfo();
		
		delete aOutgoingSdu;
		aOutgoingSdu = NULL;

		StartSendAsyncCallBack();
		}
	}

TInt COutgoingSduFragmenter::CountFragments(const RMBufChain& aSdu, TInt iMtuUsedToFragment) const
	{
	TUint totalFragmentsNeeded = 1;
	if (aSdu.Length() + ENormalHeaderLength > iMtuUsedToFragment)
		{
		TInt dataRemaining = aSdu.Length() + EStartFragHeaderLength
									 - iMtuUsedToFragment;
		while (dataRemaining > 0)
			{
			totalFragmentsNeeded++;
			dataRemaining -= (iMtuUsedToFragment - EOtherFragHeaderLength);
			}
		}
	return totalFragmentsNeeded;
	}

void COutgoingSduFragmenter::DoSendCurrentSDU()
	{
	// we should have one fragmenter allowed to write to a given remote at any time
	// once the fragmenter has done its job then we can ask the SAPs to drain an SDU into the fragger
	TInt mtu = iMgr.Transport().GetCurrentOutboundMtu(iChannel);
	
	// If we couldn't retrieve the MTU we aren't in a state where we can send.
	// Once we are send should be kicked off by mux calling CanSend().
	if(mtu > 0)
		{
		switch(iCurrentWriteState)
			{
			case ENormal:
				{
				BeginSendingSdu(mtu);
				break;
				}
			case EFragmenting:
				{
				ContinueSendingSdu(mtu);
				break;
				}
			default:
				__ASSERT_DEBUG(EFalse, Panic(EAvctpInvalidFragmenterState));
			};
		}
	}

void COutgoingSduFragmenter::BeginSendingSdu(TInt aMtu)
	{
	LOG_FUNC
	
	__ASSERT_DEBUG(iCurrentWriteState == COutgoingSduFragmenter::ENormal, Panic(EAvctpInvalidFragmenterState));

	RMBufChain pdu;
	TInt numberFragments = 1;
	
	if (aMtu >= iSduData.Length() + ENormalHeaderLength)
		{
		pdu.Assign(iSduData);
		}
	else
		{
		numberFragments = CountFragments(iSduData, aMtu);
		pdu.Assign(iSduData);
		
		__DEBUG_ONLY(TInt err =) pdu.Split(aMtu - EStartFragHeaderLength, iSduData);
		__ASSERT_DEBUG(err==KErrNone, Panic(EAvctpRMBufChainSplitError));
		}

	iHeaderInfo.iPktType = (iSduData.Length()) ? EStartFrag : ENormalPkt;

	if (AddHeader(pdu, numberFragments) == KErrNone)
		{
		if (iMgr.Transport().DoWrite(pdu,iChannel))
			{
			if(iSduData.Length() == 0)
				{
				Reset();
				}
			else
				{
				iCurrentWriteState = COutgoingSduFragmenter::EFragmenting;
				}
			
			CheckForCanSend();
			}
		else
			{
			pdu.Remove();	// remove the first RMBuf (that is the header)
			iSduData.Prepend(pdu); // put back the pdu in the iSduData
			}
		}
	else
		{
		LOG(_L("Error creating the header because KErrNoMemory"));
		__ASSERT_DEBUG(!pdu.IsEmpty(), Panic(EAvctpFragmenterEmptyPdu));
		pdu.Free();
		Reset();
		}
	__ASSERT_DEBUG(pdu.IsEmpty(), Panic(EAvctpFragmenterNonEmptyPdu));
	}

void COutgoingSduFragmenter::ContinueSendingSdu(TInt aMtu)
	{
	RMBufChain pdu;
	// if the data payload plus the header fit in the mtu then we can send all the iSduData content 
	if (aMtu >= iSduData.Length() + EOtherFragHeaderLength)
		{
		pdu.Assign(iSduData);
		}
	// we need to split iSduData as it is bigger than the mtu
	else
		{
		pdu.Assign(iSduData);
		
		__DEBUG_ONLY(TInt err =) pdu.Split(aMtu - EOtherFragHeaderLength, iSduData);
		__ASSERT_DEBUG(err==KErrNone, Panic(EAvctpRMBufChainSplitError));
		}

	// pdu now containts either the whole remainend sdu data (first if branch) or a bit of it (second branch)
	// and iSduData contains either nothing or the remained sdu payload. Based on its length we know the packet type
	// we are going to send.
	iHeaderInfo.iPktType = (iSduData.Length()) ? EContinueFrag : EEndFrag;

	if (AddHeader(pdu, 1) == KErrNone)
		{
		if (iMgr.Transport().DoWrite(pdu,iChannel))
			{
			if(iSduData.Length() == 0)
				{
				Reset();
				}
			
			CheckForCanSend();
			}
		else
			{
			pdu.Remove();	// remove the first RMBuf (that is the header)
			iSduData.Prepend(pdu); // put back the pdu in the iSduData
			}
		}
	else
		{
		LOG(_L("Error creating the header because KErrNoMemory"));
		__ASSERT_DEBUG(!pdu.IsEmpty(), Panic(EAvctpFragmenterEmptyPdu));
		pdu.Free();
		Reset();
		}
	__ASSERT_DEBUG(pdu.IsEmpty(), Panic(EAvctpFragmenterNonEmptyPdu));
	}
	
TInt COutgoingSduFragmenter::AddHeader(RMBufChain& aPdu, TInt aNumFragments)
	{
	TInt headerLength = 0;
	switch(iHeaderInfo.iPktType)
		{
		case ENormalPkt:
			headerLength = ENormalHeaderLength;
			break;
		case EStartFrag:
			headerLength = EStartFragHeaderLength;
			break;
		case EContinueFrag:
		case EEndFrag:
			headerLength = EOtherFragHeaderLength;
			break;
		default:
			__ASSERT_DEBUG(EFalse, Panic(EAvctpFragmenterInvalidHeaderType));
		}
	
	RMBuf* header = RMBuf::Alloc(headerLength);
	if (header)
		{
		TUint8& avctpHeaderByte = *(header->Ptr());
		avctpHeaderByte = 0;
	
		// Check the transaction label is valid
		AssertValidTransactionLabel(iHeaderInfo.iTransactionLabel);
	
		avctpHeaderByte |= iHeaderInfo.iTransactionLabel << KTransactionLabelShift;
		avctpHeaderByte |= iHeaderInfo.iPktType << KPacketTypeShift;
			
		if (iHeaderInfo.iMsgType == SymbianAvctp::EResponse)
			{
			avctpHeaderByte |= KResponseMsgBit;
			}
		
		if(iHeaderInfo.iPktType == ENormalPkt)
			{
			BigEndian::Put16(header->Ptr()+KNormalHeaderPidOffset,iHeaderInfo.iPid);
			}
		else if(iHeaderInfo.iPktType == EStartFrag)
			{
			*(header->Ptr() + KNumFragmentsOffset) = aNumFragments;
			BigEndian::Put16(header->Ptr()+KStartHeaderPidOffset,iHeaderInfo.iPid);
			}
		
		if (!iHeaderInfo.iHasValidPid)
			{
			avctpHeaderByte |= KIsValidPidMask;
			}
		
		aPdu.Prepend(header);
		}
	
	return header ? KErrNone : KErrNoMemory;
	}

void COutgoingSduFragmenter::CanSend()
	{
	LOG_FUNC

	// If we have Sdu left to send this will kick off an async send, else
	// it will signal to SAPs that we are ready to receive more data
	CheckForCanSend();
	}

/*static*/ TInt COutgoingSduFragmenter::SendAsyncCallBack(TAny* aFragmenter)
	{
	static_cast<COutgoingSduFragmenter*>(aFragmenter)->DoSendCurrentSDU();
	return KErrNone;
	}


void COutgoingSduFragmenter::StartSendAsyncCallBack()
	{
	iSendAsyncCallBack->CallBack();
	}

void COutgoingSduFragmenter::CancelSendAsyncCallBack()
	{
	iSendAsyncCallBack->Cancel();
	}

void COutgoingSduFragmenter::CheckForCanSend()
	{
	LOG_FUNC

	if (iSduData.Length())
		{
		StartSendAsyncCallBack();
		}
	else
		{	
		// Only signal CanSend to the saps if we haven't got any
		// fragments left to send
		iMgr.Protocol().SignalCanSendToSaps(iMgr);		
		}
	}

//
//
// implementation of CIncomingSduAssembler
//
//

/**
Default constructor for an incoming partial AVCTP Sdu

  @internalComponent
*/
CIncomingSduAssembler::CIncomingSduAssembler(CAvctpPacketMgr& aMgr, TInt aChannel)
	: iMgr(aMgr), iChannel(aChannel)
	{
	LOG_FUNC
	}

/**
Static factory function

  @internalComponent
  @leave KErrNoMemory if the new packet could not be allocated
  @return A pointer to an incoming partial AVCTP Sdu
 */
CIncomingSduAssembler* CIncomingSduAssembler::NewL(CAvctpPacketMgr& aMgr, TInt aChannel)
	{
	LOG_STATIC_FUNC

	CIncomingSduAssembler* assembler = new(ELeave) CIncomingSduAssembler(aMgr, aChannel);
	CleanupStack::PushL(assembler);
	assembler->ConstructL();
	CleanupStack::Pop(assembler);
	return assembler;
	}	

void CIncomingSduAssembler::ConstructL()
	{
	LOG_FUNC

	// chain member just takes ownership of stuff from l2cap
	}

CIncomingSduAssembler::~CIncomingSduAssembler()
	{
	LOG_FUNC

	iAccretingSdu.Free();
	}

void CIncomingSduAssembler::Reset()
	{
	LOG_FUNC

	iStartHeaderInfo = *new (&iStartHeaderInfo) TAvctpStartHeaderInfo;
	iFragmentsReceived = 0;
	iContinueFragmentSize = 0;
	iAccretingSdu.Free();
	}
	
/**
This function transfers the ownership of aIncomingPdu to CIncomingSduAssembler.
aIncomingPdu will represent a valid PDU as described by TAvctpStartHeaderInfo
*/
void CIncomingSduAssembler::AddDataL(TAvctpStartHeaderInfo& aHeaderInfo, 
									RMBufChain& aIncomingPdu)
	{
	LOG_FUNC

	switch (aHeaderInfo.iPktType)
		{
		case ENormalPkt:
			ProcessNormalPduL(aIncomingPdu);
			break;
		case EStartFrag:
			ProcessStartPdu(aHeaderInfo, aIncomingPdu);
			break;
			
		case EContinueFrag:
			ProcessContinuePdu(aHeaderInfo, aIncomingPdu);
			break;
			
		case EEndFrag:
			ProcessEndPduL(aHeaderInfo, aIncomingPdu);
			break;
			
		default:
			Panic(EUnknownPacketType);
		}	
	}
/**
Because fragments in the same SDU can't be interleaved, this functions throws away any existing 
partial SDU
*/	
void CIncomingSduAssembler::ProcessNormalPduL(RMBufChain& aIncomingPdu)
	{
	LOG_FUNC

	HAvctpIncomingSdu* sdu = new (ELeave) HAvctpIncomingSdu(iMgr.DevAddr(), aIncomingPdu);

	iMgr.Protocol().SignalNewDataToSaps(sdu, iChannel); // transfers ownership of iAccretingSdu data via the HAvctpIncomingSdu
	Reset(); 
	}
	
/**
Because fragments in the same SDU can't be interleaved, this functions throws away any existing 
partial SDU
*/
void CIncomingSduAssembler::ProcessStartPdu(TAvctpStartHeaderInfo& aHeaderInfo, RMBufChain& aIncomingPdu)
	{
	LOG_FUNC
	
	iStartHeaderInfo.iPktType = EStartFrag;	 //Not sure we want to bother updating this during aggregation
	iStartHeaderInfo.iFragmentsInSdu = aHeaderInfo.iFragmentsInSdu;
	iStartHeaderInfo.iTransactionLabel = aHeaderInfo.iTransactionLabel;
	iStartHeaderInfo.iMsgType = aHeaderInfo.iMsgType;
	iFragmentsReceived = 1;
	iContinueFragmentSize = aIncomingPdu.Length();
	
	// All packets we pass up to AVCTPServices have the same header format.
	// Because fragmentation happens transparently to the client side we 
	// present all packets with a normal header.
	TUint8 headerByte = *(aIncomingPdu.First()->Ptr());
	aIncomingPdu.TrimStart(1);
	*(aIncomingPdu.First()->Ptr()) = headerByte & KAvctpNormalHeaderMask;
	
	iAccretingSdu.Free();
	iAccretingSdu.Assign(aIncomingPdu);
	}
	
void CIncomingSduAssembler::ProcessContinuePdu(const TAvctpStartHeaderInfo& aHeaderInfo,
												RMBufChain& aIncomingPdu)
	{
	LOG_FUNC
	if (iAccretingSdu.Length() &&
			iStartHeaderInfo.iTransactionLabel == aHeaderInfo.iTransactionLabel)
		{
		// the end packet matches by label - good!
		if (iFragmentsReceived < iStartHeaderInfo.iFragmentsInSdu -1 && // not <= since we expect a End Fragment at least after a Continue
			iStartHeaderInfo.iMsgType == aHeaderInfo.iMsgType &&		
			aIncomingPdu.Length() <= iContinueFragmentSize)			
			{
			iFragmentsReceived++;
			aIncomingPdu.TrimStart(EOtherFragHeaderLength);
			iAccretingSdu.Append(aIncomingPdu);
			}
		else if (iFragmentsReceived    >= iStartHeaderInfo.iFragmentsInSdu - 1 || // - 1 cause we've not counted this packet yet
				 aIncomingPdu.Length() != iContinueFragmentSize ||
				 iStartHeaderInfo.iMsgType != aHeaderInfo.iMsgType)
				  
			{
			iMgr.Transport().Shutdown(KErrMalformedPacketFromRemote);
			Reset();
			}
		else
			{
			// else the PDU is out of place but not actually incorrect so just allow it to drop
			aIncomingPdu.Free();
			}
		}
	else
		{
		// else the PDU is not on our current transaction label so just allow it to drop
		aIncomingPdu.Free();
		}
	}
	
void CIncomingSduAssembler::ProcessEndPduL(const TAvctpStartHeaderInfo& aHeaderInfo,
											RMBufChain& aIncomingPdu)
	{
	LOG_FUNC

	if (iAccretingSdu.Length() &&
		iStartHeaderInfo.iTransactionLabel == aHeaderInfo.iTransactionLabel)
		{		
		// the end packet matches by label - good!
		if (iFragmentsReceived == iStartHeaderInfo.iFragmentsInSdu - 1 && // - 1 cause we've not counted this packet yet
			iStartHeaderInfo.iMsgType == aHeaderInfo.iMsgType &&		
			aIncomingPdu.Length() <= iContinueFragmentSize)

			{
			iFragmentsReceived++;
			aIncomingPdu.TrimStart(EOtherFragHeaderLength);
			iAccretingSdu.Append(aIncomingPdu);
			
			// This is only new'd on the heap so we can safely pass it along.
			HAvctpIncomingSdu* completeSdu = new (ELeave) HAvctpIncomingSdu(iMgr.DevAddr(), iAccretingSdu);

			iMgr.Protocol().SignalNewDataToSaps(completeSdu, iChannel); // transfers ownership of iAccretingSdu data via the HAvctpIncomingSdu				
			Reset(); 
			}
		else if (aIncomingPdu.Length() > iContinueFragmentSize ||
				iStartHeaderInfo.iMsgType != aHeaderInfo.iMsgType)
			{
			iMgr.Transport().Shutdown(KErrMalformedPacketFromRemote);
			Reset();
			}
		else
			{
			// else the PDU is out of place but not actually incorrect so just allow it to drop
			iAccretingSdu.Free();
			}
		}
	else
		{
		// else the PDU is not on our current transaction label so just allow it to drop
		iAccretingSdu.Free();
		}
	}
