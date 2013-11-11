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
#include <es_prot.h>
#include <bluetooth/hci/aclpacketconsts.h>

#include "L2CapPDU.h"
#include "l2capCommand.h"
#include "l2signalmgr.h"
#include "L2CapDataController.h"
#include "L2CapSDU.h"
#include "L2CapDebugControlInterface.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP_PDU);
#endif

TDataPlaneElementHandle::TDataPlaneElementHandle(MDataPlaneElement* aDataElement, TUint16 aElementID)
	{
	LOG_FUNC
	SetUserLen(sizeof(SDataPlaneElementHandle));
	SDataPlaneElementHandle* handlePtr = reinterpret_cast<SDataPlaneElementHandle*>(UserPtr());
	handlePtr->iDataElement = aDataElement;
	handlePtr->iElementID = aElementID;
	}

MDataPlaneElement& TDataPlaneElementHandle::DataPlaneElement() const
	{
	LOG_FUNC
	SDataPlaneElementHandle* handlePtr = reinterpret_cast<SDataPlaneElementHandle*>(UserPtr());
	return *(handlePtr->iDataElement);
	}
	
TUint16 TDataPlaneElementHandle::ElementID() const
	{
	LOG_FUNC
	SDataPlaneElementHandle* handlePtr = reinterpret_cast<SDataPlaneElementHandle*>(UserPtr());
	return (handlePtr->iElementID);
	}


//
// Base class for all PDU types.
//
// Disable warning WINS 4355: 'this' : used in base member initializer list
// This will not cause any problems in this usage and is preferable to the use of a
// non-owned pointer.
#pragma warning (disable: 4355)
HL2CapPDU::HL2CapPDU(RMBufChain& aPDUData, TInt aOptimalFragmentSize)
 : iIsFlushed(EFalse),
   iSendingError(EFalse),
   iQueuedForSend(EFalse),
   iAwaitingHciCompletion(EFalse), // first transmission never waits for a previous one to finish
   iElementHandle(this),
   iPduOwner(NULL),
   iOptimalFragmentSize(aOptimalFragmentSize),
   iTotalNumberOfFragments(KNumberOfFragmentsUnknown),
   iFragmentAcksReceived(0)
	{
	LOG_FUNC
	iPDUData.Assign(aPDUData);
	iPDUData.Align(Min(iPDUData.Length(), KMaxPDUHeaderLength));

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EBasePDU,
	                             L2capDebugInfo::EAllocated));
	}

HL2CapPDU::HL2CapPDU(TInt aOptimalFragmentSize)
 : iIsFlushed(EFalse),
   iSendingError(EFalse),
   iQueuedForSend(EFalse),
   iAwaitingHciCompletion(EFalse), // first transmission never waits for a previous one to finish
   iElementHandle(this),
   iPduOwner(NULL),
   iOptimalFragmentSize(aOptimalFragmentSize),
   iTotalNumberOfFragments(KNumberOfFragmentsUnknown),
   iFragmentAcksReceived(0)
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EBasePDU,
	                             L2capDebugInfo::EAllocated));
	}
#pragma warning (default: 4355)


HL2CapPDU::~HL2CapPDU()
	{
	LOG_FUNC
	iPDUData.Free();

	iLink.Deque();
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EBasePDU,
	                             L2capDebugInfo::EDeleted));
	}
	
	
// Static Message Accessors.
/*static*/ TBool HL2CapPDU::AddFragment(RMBufChain& aPDU, RMBufChain& aPDUFragment)
	{
	LOG_STATIC_FUNC
	if(aPDUFragment.First()!= NULL)
		{
		aPDU.Append(aPDUFragment);
		return ((aPDU.Length() - KPDUHeaderLength) >= PDUPayloadLength(aPDU));
		}
	else
		{
		return(EFalse);
		}
	}

/*static*/ TUint16 HL2CapPDU::PDUPayloadLength(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KPDUHeaderLength,
		           Panic(EL2CapPDUInvalidLength));
	return LittleEndian::Get16((aPDU.First())->Ptr()+KLengthByteOffset);
	}

/*static*/ TUint16 HL2CapPDU::PDUCID(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KPDUHeaderLength,
		           Panic(EL2CapPDUInvalidLength));
	return LittleEndian::Get16((aPDU.First())->Ptr()+KCIDByteOffset);
	}

void HL2CapPDU::SetPDUPayloadLength(TUint16 aLength)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iPDUData.First() && (iPDUData.First())->Length() >= KPDUHeaderLength,
		           Panic(EL2CapPDUInvalidLength));
	LittleEndian::Put16((iPDUData.First())->Ptr()+KLengthByteOffset, aLength);
	}

void HL2CapPDU::SetPDUCID(TUint16 aCID)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iPDUData.First() && (iPDUData.First())->Length() >= KPDUHeaderLength,
		           Panic(EL2CapPDUInvalidLength));
	LittleEndian::Put16((iPDUData.First())->Ptr()+KCIDByteOffset, aCID);
	}

void HL2CapPDU::WritePDUPayloadLength()
	{
	LOG_FUNC
	SetPDUPayloadLength(static_cast<TUint16>(iPDUData.Length() - KPDUHeaderLength));
	}
	
/*
 * Function to return the PDU L2CAP overhead.
 */
/*static*/ TInt HL2CapPDU::GetPDUOrFragmentOverhead(TBool aBasicMode)
	{
	LOG_STATIC_FUNC
	if (aBasicMode)
		{
		// Basic mode does not segment into PDUs and therefore has no PDU overhead
		return 0;
		}
	
	// Non-basic mode overhead
	return KPDUHeaderLength + KControlFieldLength + KFCSFieldLength;
	}

/*
 * Return the payload size of the optimal pdu based on the baseband packet
 * sizes, the negotiated MTU (aMTU) and the size of the buffers on the 
 * controller (aBufSize).
 * SDU length field is taken into account when it get segmented by CL2CapSDU::SegmentSDUIntoPDUs
 */
/*static*/ TInt HL2CapPDU::GetPDUOrFragmentSize(TInt aMTU, TInt aMaxMps, TInt aBufSize, TBool aBasicMode)
	{
	LOG_STATIC_FUNC
	// aBufSize == 0 means that getting the buffer size failed so assume the best
	// case scenario for performance
	TInt bufSize = aBufSize != 0 ? aBufSize : KBBPacketSize[KBaseBandPacketCount - 1];

	// get the overhead size for every l2cap pdu	
	TInt packetOverhead = GetPDUOrFragmentOverhead(aBasicMode);

	// it's possible that the buffer size is so small that when including the packet overhead there is no
	// room for a payload, this is of course no good.
	__ASSERT_ALWAYS((bufSize - packetOverhead) > 0, Panic(EL2CAPACLBufferTooSmall));

	// store the current baseband packet size as we go through the below for loop
	TInt currentPacketSize;

	// iterate backwards through the baseband packet sizes array
	for (TInt index = KBaseBandPacketCount - 1; index >= 0; index--)
		{
		currentPacketSize = KBBPacketSize[index];

		if (aMTU >= (index > 0 ? ((currentPacketSize + KBBPacketSize[index - 1]) / 2) : currentPacketSize))
			{
			if (bufSize >= currentPacketSize)
				{
				// if the current baseband packet size is less than the MTU size and the baseband
				// packet will fit into the controllers buffer then use the baseband packet
				return (aBasicMode ? (currentPacketSize - packetOverhead) : Min(aMaxMps, (currentPacketSize - packetOverhead)));
				}
			else if ((index > 0) && (bufSize >= ((currentPacketSize + KBBPacketSize[index - 1]) / 2)))
				{
				// if the current baseband packet is less than the MTU but the baseband packet
				// won't fit into the controllers buffer see if the buffer size is closer to the
				// current baseband packet than the next size down and if so use the buffer size
				// of the controller
				return (aBasicMode ? (bufSize - packetOverhead) : Min(aMaxMps, (bufSize - packetOverhead)));
				}
			}
		}

	// to get here means that the buffer size of the controller or the MTU is less than the minimum
	// baseband packet size. A small ACL buffer reduces performance as there is a big overhead to data
	// ratio, but we handle that situation anyway.
	return (aBasicMode ? (bufSize - packetOverhead) : Min(aMaxMps, (bufSize - packetOverhead)));
	}

/*static*/ TInt HL2CapPDU::CheckDecode(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	TInt rerr = KErrNone;
	if(aPDU.Length() != (PDUPayloadLength(aPDU) + KPDUHeaderLength))
		{
		rerr = KErrCorrupt;
		}
	return rerr;
	} 

void HL2CapPDU::AppendPayloadToBuffer(RMBufChain& /*aSDUData*/)
	{
	LOG_FUNC
	Panic(EL2CAPInvalidCallToAppendPayloadToBuffer);
	}

void HL2CapPDU::DeliverOutgoingPDU(MOutgoingPDUHandler& aPDUHandler)
	{
	iAwaitingHciCompletion = ETrue;
	DeliverOutgoingPDUDoubleDispatch(aPDUHandler);
	}


void HL2CapPDU::PDUSendPending(TUint16 aTotalNumberOfFragments)
	{
	LOG_FUNC
	// The fragment sender has completed sending the PDU to the
	// ACL.
	__ASSERT_DEBUG(iTotalNumberOfFragments == KNumberOfFragmentsUnknown, Panic(EL2CAPPDUSendPendingCalledTwice));

	iTotalNumberOfFragments = aTotalNumberOfFragments;
	
	// In some error conditions the expected number of outstanding fragments might
	// have been reached before this method is called.
	if(iFragmentAcksReceived == iTotalNumberOfFragments)
		{
		PDUSendComplete();
		}
	}

void HL2CapPDU::PDUSendComplete()
	{
	LOG_FUNC
	iAwaitingHciCompletion = EFalse;

	if(!iIsFlushed && iPduOwner)
		{
		iFragmentAcksReceived = 0;
		iTotalNumberOfFragments = KNumberOfFragmentsUnknown;

		if(!iSendingError)
			{
			// Do frame-type specific stuff.
			SendComplete();

			iPduOwner->HandlePduSendComplete(*this);
			}
		else
			{
			iSendingError = EFalse; // in case the owner decides to resend us
			iPduOwner->HandlePduSendError(*this);
			}
		}
	else
		{
		// No references are held to this PDU.  Delete it.
		delete this;
		}
	}
	

// MDataPlaneElement Interface
void HL2CapPDU::DataElementSent(TUint16 /*aElementID*/)
	{
	LOG_FUNC
	// Check if the PDU has been fully sent to the peer.
	if(++iFragmentAcksReceived == iTotalNumberOfFragments)
		{
		PDUSendComplete();
		}
	}
	
void HL2CapPDU::DataElementFlushed(TUint16 aElementID)
	{
	LOG_FUNC
	// Check if this PDU has been flushed.
	if(!iIsFlushed)
		{
		iSendingError = ETrue;
		}
	DataElementSent(aElementID);
	}
	
TUint16 HL2CapPDU::CalcCRC(const RMBufChain& aPDU)
 	{
 	LOG_STATIC_FUNC
  	TUint16 crcValue = 0;
  	TUint8 dataByte;
	TInt i, bufLength;
	TUint16 carryBit, bitCount;
	
	const RMBuf* bufPtr = aPDU.First();
	TInt fcsLength = aPDU.Length() - KFCSFieldLength;
	TInt byteCount = 0;
	while(bufPtr)
		{
		// Check if this is the last buffer.  If so don't
		// process the FCS bytes.
		bufLength = bufPtr->Length();
	 	for(i=0;i<bufLength;i++)
	  		{
	  		if(byteCount < fcsLength)
	  			{
		  		dataByte = bufPtr->Get(i);
		  		byteCount++;
				for(bitCount=0;bitCount<8;bitCount++)
					{
					carryBit = static_cast<TUint16>(crcValue & 0x8000 ? 1 : 0);
					crcValue <<= 1;
					if ((dataByte & 1) ^ carryBit)
						{
						crcValue ^= 0x8005;
						}
					dataByte >>= 1;
					}
				}
  			}
  		
  		bufPtr = bufPtr->Next();
  		}

	carryBit = 0;
	for(bitCount=0;bitCount<16;bitCount++)
		{
		carryBit = static_cast<TUint16>((carryBit >> 1) | (crcValue & 0x8000));
		crcValue <<= 1;
		}
  		
  	return carryBit;
  	}

void HL2CapPDU::SendComplete()
	{
	LOG_FUNC
	// Nothing to do for most PDUs.
	}


//
// Class used to fragment and send any type of PDU.
// An instance of this class is currently owned by the Mux
//
HFragmentedPDUSender::HFragmentedPDUSender(CL2CAPMux& aMuxer)
 : iPDU(NULL),
   iPDUBuffer(NULL),
   iMuxer(aMuxer),
   iCurrentWriteIndex(0),
   iCurrentFragmentID(0),
   iPDULength(0)
	{
	LOG_FUNC
	}

HFragmentedPDUSender::~HFragmentedPDUSender()
	{
	LOG_FUNC
	delete iPDUBuffer;
	__ASSERT_DEBUG(iPDU == NULL, Panic(EL2CAPFragmentSenderDeletedWhileSending));
	}

TInt HFragmentedPDUSender::FragmentPDU(HL2CapPDU& aPDU)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	__ASSERT_DEBUG(!iCurrentWriteIndex && !iCurrentFragmentID, Panic(EL2CAPRequestToFragmentWhileBufferContainsData));

	RMBufChain& pduData = aPDU.PDUBuffer();
	if(!iPDUBuffer || iPDUBuffer->Size() < pduData.Length())
		{
		// Increase the size of the buffer.
		delete iPDUBuffer; 
		iPDUBuffer = HBufC8::New(pduData.Length());
		}

	if(iPDUBuffer)
		{
		iPDU = &aPDU;
		iPDULength = pduData.Length();
		
		// Copy the Buffer into the descriptor
  		TPtr8 ptr = iPDUBuffer->Des();
  		ptr.SetLength(iPDULength);
		pduData.CopyOut(ptr);

		// store the optimal fragment size
		iPDUFragmentSize = aPDU.OptimalFragmentSize();
		}
	else
		{
		rerr = KErrNoMemory;
		}
	return rerr;
	}

HFragmentedPDUSender::TFragmentSenderStatus HFragmentedPDUSender::WriteNextFragment(CServProviderBase& aSender, TInt aACLMTU)
	{
	LOG_FUNC
	// If optimal fragment size not set then this PDU should only have been segmented, not fragmented
	__ASSERT_DEBUG((iPDULength <= aACLMTU) || (iPDUFragmentSize != 0), Panic(EL2CAPUnexpectedFragmentation));
	if (iPDUFragmentSize == 0)
		{
		// For udeb builds we know that this PDU can fit into the ACL buffer so just set the fragment size
		// to be the ACL buffer size to allow logic below to work. For urel builds this can also recover the
		// situation if the debug assert fails above, just not using optimal performance.
		iPDUFragmentSize = aACLMTU;
		}

	// Ensure that the optimal fragment size will fit into the ACL buffer.
	__ASSERT_DEBUG(aACLMTU >= iPDUFragmentSize, Panic(EL2CAPOptimalFragmentSizeTooBigForACLBuffer));

	TFragmentSenderStatus rStatus = EFragmentOK;

	TUint8 flags = iCurrentWriteIndex ? KContinuingHLFragment : KFirstHLFragment;

	TInt fragLength = Min(iPDULength - iCurrentWriteIndex, Min(iPDUFragmentSize, aACLMTU));
	
	// The 'Write' returns the number of fragments (NOT bytes) 
	// that will be sent. This is typically either 0 or 1.
	TUint fragmentsSent = aSender.Write(iPDUBuffer->Mid(iCurrentWriteIndex, fragLength), flags, &iPDU->ElementHandle()); 
	if(!fragmentsSent)
		{
		rStatus = EFlowControlledOff;
		}
	else
		{
		iCurrentWriteIndex += fragLength;
		iCurrentFragmentID++;
		}
	
	// Check if the PDU has been fully sent.
	if(iCurrentWriteIndex >= iPDULength)
		{
		rStatus = EFragmentationComplete;
		
		// Inform the PDU that it has been sent.
		iPDU->PDUSendPending(iCurrentFragmentID);
		
		// Reset the internal state of this sender.
		Reset();
		}
	return rStatus; 
	}

void HFragmentedPDUSender::Reset()
	{
	LOG_FUNC
	iPDU = NULL;
	iPDULength = iCurrentWriteIndex = 0;
	iCurrentFragmentID = 0;
	iPDUFragmentSize = 0;
	}

void HFragmentedPDUSender::CheckForFlushed()
	{
	LOG_FUNC
	// Check if the PDU currently being sent(if there is one)
	// needs to be flushed.
	if(iPDU)
		{
		if(iPDU->IsPDUFlushed())
			{
			// Inform the PDU that it has been sent.
			iPDU->PDUSendPending(iCurrentFragmentID);
			
			// Reset the internal state of this sender.
			Reset();
			}
		}
	}

void HFragmentedPDUSender::PDUSenderFailed()
	{
	LOG_FUNC
	if(iPDU)
		{
		// Inform the PDU that it has been sent.
		iPDU->PDUSendPending(iCurrentFragmentID);
		
		// Reset the internal state of this sender.
		Reset();
		}
	}



//
// Basic frame PDU class.
//
HBFramePDU::HBFramePDU(RMBufChain& aPDUData, TInt aOptimalFragmentSize)
 : HL2CapPDU(aPDUData, aOptimalFragmentSize)
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EBFrame,
	                             L2capDebugInfo::EAllocated));
	}

HBFramePDU::~HBFramePDU()
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EBFrame,
	                             L2capDebugInfo::EDeleted));
	}

HBFramePDU* HBFramePDU::New(RMBufChain& aPayloadData, TInt aOptimalFragmentSize)
	{
	LOG_STATIC_FUNC
	HBFramePDU* self = NULL;
	TRAPD(rerr, aPayloadData.PrependL(KPDUHeaderLength));
	if(rerr == KErrNone)
		{
		self = new HBFramePDU(aPayloadData, aOptimalFragmentSize);
		}
	
	if(self)
		{	
		self->WritePDUPayloadLength();
		}
	return self;
	}

void HBFramePDU::DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler)
	{
	LOG_FUNC
	aPDUHandler.HandleOutgoingBFrame(this);
	}

void HBFramePDU::AppendPayloadToBuffer(RMBufChain& aSDUData)
	{
	LOG_FUNC
	// Remove header and trailing bytes.
	iPDUData.TrimStart(KPDUHeaderLength);
	aSDUData.Append(iPDUData);
	}

/*static*/ void HBFramePDU::RemoveHeaderBytes(RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	// Remove header bytes.
	aPDU.TrimStart(KPDUHeaderLength);
	}

void HBFramePDU::SendComplete()
	{
	LOG_FUNC
	L2CAP_DEBUG_PDU(PDUTimer(L2capDebugInfo::EPDUSent, this, PDUCID()));
	}


//
// Information frame PDU class.
//
HIFramePDU::HIFramePDU(RMBufChain& aPDUData)
 : HL2CapPDU(aPDUData, 0),
   iTransmissionCount(0),
   iAcked(EFalse)
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EIFrame,
	                             L2capDebugInfo::EAllocated));
	}

HIFramePDU::~HIFramePDU()
	{
	LOG_FUNC
	LOG1(_L("Deleting TxSeq = %d"), TxSeqNumber());

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EIFrame,
	                             L2capDebugInfo::EDeleted));
	}


HIFramePDU* HIFramePDU::New(RMBufChain& aPayloadData, TL2CapSAR aPduSAR)
	{
	LOG_STATIC_FUNC
	TInt rerr = KErrNone;
	HIFramePDU* self = NULL;

	switch(aPduSAR)
		{
		case EStartOfL2CapSDU:
			TRAP(rerr, aPayloadData.PrependL(KPDUHeaderLength + KControlFieldLength + KSDULengthFieldLength));
			break;
		case EUnsegmentedL2CapSDU:
		case EEndOfL2CapSDU:
		case EContinuationOfL2CapSDU:
			TRAP(rerr, aPayloadData.PrependL(KPDUHeaderLength + KControlFieldLength));
			break;
			
		default:
			Panic(EL2CAPInvalidPDUSAR);
			break;
		};		

	if(rerr == KErrNone)
		{
		TRAP(rerr, aPayloadData.AppendL(KFCSFieldLength));
		}

	if(rerr == KErrNone)
		{
		self = new HIFramePDU(aPayloadData);
		if(self)
			{
			self->WritePDUPayloadLength();
			self->SetIFrameControlDefault();
			self->SetSAR(aPduSAR);
			}
		}

	return self;
	}

void HIFramePDU::DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler)
	{
	LOG_FUNC
	aPDUHandler.HandleOutgoingIFrame(this);
	}	

// Message Accessors.
/*static*/ TBool HIFramePDU::IFrameIdentifier(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset);
	return (!(ctrl & KCtrlFrameTypeMask));
	}

/*static*/ TUint8 HIFramePDU::TxSeqNumber(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset);
	return static_cast<TUint8>((ctrl & KCtrlTxSeqMask) >> KCtrlTxSeqShift);
	}

/*static*/ TBool HIFramePDU::FinalBit(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
	           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset);
	return (ctrl & KCtrlFinalBitMask);
	}

void HIFramePDU::SetFinalBit(TBool aFinalBit)
	{
	LOG_FUNC
	SetRetransmitDisable(aFinalBit);
	}

void HIFramePDU::SetTxSeqNumber(TUint8 aTxSeqNum)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iPDUData.First() && (iPDUData.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlTxSeqMask;
	ctrl |= (aTxSeqNum << KCtrlTxSeqShift);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);
	}

/*static*/ TBool HIFramePDU::RetransmitDisable(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	return (LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset) & KCtrlReTxDisableMask);
	}
	
void HIFramePDU::SetRetransmitDisable(TBool aReTxDisable)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iPDUData.First() && (iPDUData.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlReTxDisableMask;
	ctrl |= (aReTxDisable << KCtrlReTxDisableShift);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);	
	}

/*static*/ TUint8 HIFramePDU::ReqSeqNumber(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset);
	return static_cast<TUint8>((ctrl & KCtrlReqSeqMask) >> KCtrlReqSeqShift);
	}

void HIFramePDU::SetReqSeqNumber(TUint8 aReqSeqNum)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(iPDUData.First() && (iPDUData.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlReqSeqMask;
	ctrl |= (aReqSeqNum << KCtrlReqSeqShift);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);
	}

/*static*/ TBool HIFramePDU::IsStartOfSDU(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	return (SAR(aPDU) == EUnsegmentedL2CapSDU || SAR(aPDU) == EStartOfL2CapSDU);
	}

	
/*static*/ TL2CapSAR HIFramePDU::SAR(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset);
	return TL2CapSAR((ctrl & KCtrlSARMask) >> KCtrlSARShift);
	}
	
void HIFramePDU::SetSAR(TL2CapSAR aSARValue)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iPDUData.First() && (iPDUData.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlSARMask;
	TUint8 sar = static_cast<TUint8>(aSARValue);
	ctrl |= (sar << KCtrlSARShift);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);	
	}
	
/*static*/ TUint16 HIFramePDU::SDUSize(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KSDULengthFieldLength + KSDULengthFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));
	
	return LittleEndian::Get16((aPDU.First())->Ptr()+KSDULengthFieldByteOffset);	
	}
	
void HIFramePDU::SetSDUSize(TUint16 aSDUSize)
	{
	LOG_FUNC
	LittleEndian::Put16((iPDUData.First())->Ptr()+KSDULengthFieldByteOffset, aSDUSize);
	}
	
void HIFramePDU::CalculateAndSetFCS()
	{
	LOG_FUNC

	TUint16 fcs = HL2CapPDU::CalcCRC();
	
	RMBuf* buf = iPDUData.Last();	
	if(buf->Length() < KFCSFieldLength)
		{
		// This can only mean that the last buffer contains one byte.
		// Get a pointer to the penultimate buffer.
		buf = iPDUData.First();
		for(TInt i=1;i<(iPDUData.NumBufs() - 1);i++)
			{
			buf = buf->Next();
			}
		buf->Put(static_cast<TUint8>(fcs & 0x00ff), buf->Length() - 1);
		fcs = static_cast<TUint16>(fcs >> 8);
		buf = buf->Next();
		buf->Put(static_cast<TUint8>(fcs & 0x00ff), 0);
		}
	else
		{
		LittleEndian::Put16(buf->Ptr()+buf->Length()-KFCSFieldLength, fcs); 
		}
	}
	
/*static*/ TBool HIFramePDU::CheckFCS(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KPDUHeaderLength + KFCSFieldLength,
		           Panic(EL2CapPDUInvalidLength));

	TUint16 msgFCS;
	RMBuf* buf = aPDU.Last();
	if(buf->Length() < KFCSFieldLength)
		{
		// This can only mean that the last buffer contains one byte.
		msgFCS = static_cast<TUint16>((buf->Get(0)) << 8);
		
		// Get a pointer to the penultimate buffer.
		const RMBuf* cBuf = aPDU.First();
		for(TInt i=1;i<(aPDU.NumBufs() - 1);i++)
			{
			cBuf = cBuf->Next();
			}
		msgFCS |= cBuf->Get(cBuf->Length() - 1);
		}
	else
		{
		msgFCS = LittleEndian::Get16(buf->Ptr()+buf->Length()-KFCSFieldLength); 
		}
		
	return (msgFCS == HL2CapPDU::CalcCRC(aPDU));
	}

TBool HIFramePDU::CheckFCS()
	{
	LOG_FUNC
	return CheckFCS(iPDUData);
	}
	
void HIFramePDU::SetIFrameControlDefault()
	{
	LOG_FUNC
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, KIFrameControlDefault);
	}


void HIFramePDU::AppendPayloadToBuffer(RMBufChain& aSDUData)
	{
	LOG_FUNC
	// Remove header and trailing bytes.
	if(SAR() == EStartOfL2CapSDU)
		{
		iPDUData.TrimStart(KPDUHeaderLength + KControlFieldLength + KSDULengthFieldLength);
		}
	else
		{
		iPDUData.TrimStart(KPDUHeaderLength + KControlFieldLength);
		}
	iPDUData.TrimEnd(iPDUData.Length()-KFCSFieldLength);
	aSDUData.Append(iPDUData);
	}

/*static*/ void HIFramePDU::RemoveHeaderAndFCSBytes(RMBufChain& aPDU)
	{
	// Remove header and trailing bytes.
	if(SAR(aPDU) == EStartOfL2CapSDU)
		{
		aPDU.TrimStart(KPDUHeaderLength + KControlFieldLength + KSDULengthFieldLength);
		}
	else
		{
		aPDU.TrimStart(KPDUHeaderLength + KControlFieldLength);
		}
	aPDU.TrimEnd(aPDU.Length()-KFCSFieldLength);
	}

/*static*/ TInt HIFramePDU::CheckPayloadDecode(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	TInt rerr = KErrNone;
	
	if(PDUCID(aPDU) < KL2CapDynamicCIDStart)
		{
		rerr = KErrCorrupt;
		}
	else if(!CheckFCS(aPDU))
		{
		rerr = KErrCorrupt;
		}
	return rerr;
	}

/*static*/ TInt HIFramePDU::CheckLengthWithinLimits(const RMBufChain& aPDU, TUint16 aMps)
	{
	LOG_STATIC_FUNC
	// Check that the PDU size is >= than minimal and that the information payload size is <= MPS. 

	TInt err = KErrNone;
	TBool hasSduLength = (SAR(aPDU) == EStartOfL2CapSDU);

	TInt infoPayloadSize = PDUPayloadLength(aPDU) - KControlFieldLength - KFCSFieldLength;
	if (hasSduLength)
		{
		infoPayloadSize -= KSDULengthFieldLength;
		}

	if (infoPayloadSize > aMps)
		{
		err = KErrL2CAPIncomingIFrameTooBig;
		}
	else
		{
		if (aPDU.Length() < (KPDUHeaderLength + KControlFieldLength + KFCSFieldLength))
			{
			err = KErrL2CAPIncomingIFrameTooSmall;
			}
		}

	return err;
	}

/*static*/ TInt HIFramePDU::CheckStartSduLength(const RMBufChain& aPDU, TUint16 aMtu)
	{
	LOG_STATIC_FUNC
	TInt err = KErrNone;

	if (aPDU.Length() < (KPDUHeaderLength + KControlFieldLength + KSDULengthFieldLength + KFCSFieldLength))
		{
		err = KErrL2CAPIncomingIFrameTooSmall;
		}
	else if (SDUSize(aPDU) > aMtu)
		{
		err = KErrL2CAPIncomingSduTooBig;
		}
	return err;
	}

void HIFramePDU::SendComplete()
	{
	LOG_FUNC
	iTransmissionCount++;
		
#ifdef _DEBUG	
	if(SAR() == EEndOfL2CapSDU || SAR() == EUnsegmentedL2CapSDU)
		{
		L2CAP_DEBUG_PDU(PDUTimer(L2capDebugInfo::EPDUSent, this, PDUCID()));
		}
#endif
	}


//
// Supervisory frame PDU class.
//
HSFramePDU* HSFramePDU::New(TSupervisoryFunction aFunction)
	{
	LOG_STATIC_FUNC
	TInt rerr = KErrNone;
	HSFramePDU* self = NULL;
	 
	RMBufChain frameBuffer;	
	TRAP(rerr, frameBuffer.AllocL(KSFrameLength));

	if(rerr == KErrNone)
		{
		self = new HSFramePDU(frameBuffer);
		if(self)
			{
			self->WritePDUPayloadLength();
			self->SetSFrameControlDefault();
			self->SetSupervisoryFunction(aFunction);
			}
		else
			{
			frameBuffer.Free();
			}
		}
	return self;
	}

HSFramePDU* HSFramePDU::NewL(TSupervisoryFunction aFunction)
	{
	LOG_STATIC_FUNC
	HSFramePDU* self = New(aFunction);
	if (self == NULL)
		{
		LEAVEL(KErrNoMemory);
		}
	return self;
	}

HSFramePDU::HSFramePDU(RMBufChain& aPDUData)
 : HL2CapPDU(aPDUData, 0)
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ESFrame,
	                             L2capDebugInfo::EAllocated));
	}

HSFramePDU::~HSFramePDU()	
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ESFrame,
	                             L2capDebugInfo::EDeleted));
	}

void HSFramePDU::DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler)
	{
	LOG_FUNC
	aPDUHandler.HandleOutgoingSFrame(this);
	}	

/*static*/ TInt HSFramePDU::CheckLengthField(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	TInt err = KErrNone;

	// The length field doesn't include the L2CAP Basic header.
	if (PDUPayloadLength(aPDU) != KControlFieldLength + KFCSFieldLength)
		{
		err = KErrL2CAPInvalidIncomingSFrameSize;
		}

	return err;
	}

/*static*/ TInt HSFramePDU::CheckPayloadDecode(RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	TInt rerr = KErrNone;

	if(!CheckFCS(aPDU))
		{
		rerr = KErrCorrupt;
		}

	return rerr;
	}

/*static*/ TSupervisoryFunction HSFramePDU::SupervisoryFunction(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));

	TUint16 ctrl = LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset);
	return TSupervisoryFunction((ctrl & KCtrlSupervisoryMask) >> 2);
	}
	
/*static*/ TUint8 HSFramePDU::ReqSeqNumber(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));

	TUint16 ctrl = LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset);
	return static_cast<TUint8>((ctrl & KCtrlReqSeqMask) >> 8);
	}

void HSFramePDU::SetSupervisoryFunction(TSupervisoryFunction aSupervisoryFunction)
	{
	LOG_FUNC
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlSupervisoryMask;
	ctrl |= (aSupervisoryFunction << 2);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);
	}
	
/*static*/ TBool HSFramePDU::RetransmitDisable(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));

	return (LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset) & KCtrlReTxDisableMask);
	}

/*static*/ TBool HSFramePDU::FinalBit(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));

	return (LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset) & KCtrlFinalBitMask);
	}

/*static*/ TBool HSFramePDU::PollBit(const RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KControlFieldLength + KControlFieldByteOffset,
		           Panic(EL2CapPDUInvalidLength));

	return (LittleEndian::Get16((aPDU.First())->Ptr()+KControlFieldByteOffset) & KCtrlPollBitMask);
	}

void HSFramePDU::SetFinalBit(TBool aFinalBit)
	{
	LOG_FUNC
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlFinalBitMask;
	ctrl |= (aFinalBit << 7);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);	
	}

void HSFramePDU::SetPollBit(TBool aPollBit)
	{
	LOG_FUNC
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlPollBitMask;
	ctrl |= (aPollBit << 4);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);	
	}

void HSFramePDU::SetRetransmitDisable(TBool aReTxDisable)
	{
	LOG_FUNC
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlReTxDisableMask;
	ctrl |= (aReTxDisable << 7);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);	
	}

void HSFramePDU::SetReqSeqNumber(TUint8 aReqSeqNum)
	{
	LOG_FUNC
	TUint16 ctrl = LittleEndian::Get16((iPDUData.First())->Ptr()+KControlFieldByteOffset);
	ctrl &= ~KCtrlReqSeqMask;
	ctrl |= (aReqSeqNum << 8);
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, ctrl);
	}

void HSFramePDU::CalculateAndSetFCS()
	{
	LOG_FUNC

	TUint16 fcs = HL2CapPDU::CalcCRC();

	RMBuf* lastBuf = iPDUData.Last();
	if(lastBuf->Length() < KFCSFieldLength)
		{
		// This can only be 8 bytes long.  Align the buffer.
		iPDUData.Align(iPDUData.Length());
		lastBuf = iPDUData.Last();
		}
	LittleEndian::Put16(lastBuf->Ptr()+lastBuf->Length()-KFCSFieldLength, fcs);
	}
	
/*static*/ TBool HSFramePDU::CheckFCS(RMBufChain& aPDU)
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aPDU.First() && (aPDU.First())->Length() >= KPDUHeaderLength + KFCSFieldLength,
		           Panic(EL2CapPDUInvalidLength));

	TUint16 msgFCS;
	RMBuf* buf = aPDU.Last();
	if(buf->Length() < KFCSFieldLength)
		{
		// This can only be 8 bytes long.  Align the buffer.
		aPDU.Align(aPDU.Length());
		buf = aPDU.Last();
		}

	msgFCS = LittleEndian::Get16(buf->Ptr()+buf->Length()-KFCSFieldLength); 
		
	return (msgFCS == HL2CapPDU::CalcCRC(aPDU));
	}

TBool HSFramePDU::CheckFCS()
	{
	LOG_FUNC
	return CheckFCS(iPDUData);
	}

void HSFramePDU::SetSFrameControlDefault()
	{
	LOG_FUNC
	LittleEndian::Put16((iPDUData.First())->Ptr()+KControlFieldByteOffset, KSFrameControlDefault);
	}


//
// Group frame PDU class.
//
HGFramePDU::HGFramePDU(RMBufChain& aPDUData)
 : HL2CapPDU(aPDUData, 0)
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EGFrame,
	                             L2capDebugInfo::EAllocated));
	}

HGFramePDU::~HGFramePDU()	
	{
	LOG_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EGFrame,
	                             L2capDebugInfo::EDeleted));
	}

void HGFramePDU::DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler)
	{
	LOG_FUNC
	aPDUHandler.HandleOutgoingGFrame(this);
	}	

void HGFramePDU::AppendPayloadToBuffer(RMBufChain& aSDUData)
	{
	LOG_FUNC
	// Remove header and trailing bytes.
	iPDUData.TrimStart(KPDUHeaderLength + KGFramePSMLength);
	aSDUData.Append(iPDUData);
	}


//
// Control frame PDU class.
//
/*static*/ HCFramePDU* HCFramePDU::New(TInt aOptimalFragmentSize)
	{
	LOG_STATIC_FUNC
	RMBufChain buf;
	HCFramePDU* cFrame = NULL;
	
	TRAPD(err, buf.AllocL(KCFrameHeaderLength));
	if(err == KErrNone)
		{
		cFrame = new HCFramePDU(buf, aOptimalFragmentSize);
		if(cFrame)
			{
			cFrame->SetPDUCID(KL2CapSignallingCID);
			cFrame->WritePDUPayloadLength();
			}
		else
			{
			buf.Free();
			}
		}
	return cFrame;
	}

HCFramePDU::HCFramePDU(RMBufChain& aPDUData, TInt aOptimalFragmentSize)
 : HL2CapPDU(aPDUData, aOptimalFragmentSize),
   iCommands(_FOFF(HL2CapCommand, iLink)),
   iCommandIter(iCommands)   
	{
	LOG_STATIC_FUNC
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ECFrame,
	                             L2capDebugInfo::EAllocated));
	}

HCFramePDU::~HCFramePDU()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iCommands.IsEmpty(), Panic(EL2CAPDeleteCFrameWhileContainingCommands));

	while(!iCommands.IsEmpty())
		{
		HL2CapCommand *cmd = iCommands.First();
		cmd->iLink.Deque();
		delete cmd;
		}
	
	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ECFrame,
	                             L2capDebugInfo::EDeleted));
	}

void HCFramePDU::DeliverOutgoingPDUDoubleDispatch(MOutgoingPDUHandler& aPDUHandler)
	{
	LOG_FUNC
	aPDUHandler.HandleOutgoingCFrame(this);
	}

TInt HCFramePDU::CheckDecode()
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(iPDUData.Length() != (PDUPayloadLength() + KPDUHeaderLength))
		{
		rerr = KErrCorrupt;
		}
	else
		{
		rerr = CheckPayloadDecode();
		}
	return rerr;
	}

TInt HCFramePDU::CheckPayloadDecode()
	{
	LOG_FUNC
	return CreateCommands();
	}
   

// Takes this original PDU and extracts out a list of the contained Command Frames
// Upon completion the original PDU will be freed, but this class will hold a list 
// CFrames.
TInt HCFramePDU::CreateCommands()
	{
	LOG_FUNC
	// Trim off the standard header.
	iPDUData.TrimStart(KCFrameHeaderLength);
	
	HL2CapCommand* cmd = NULL;
	
	// If a command is decoded it will be removed from the 
	// start of the buffer.
	TInt err = KErrNone;
	while(err == KErrNone)
		{
		err = HL2CapCommand::DecodeCommand(iPDUData, cmd);
		LOG1(_L("HCFramePDU::CreateCommands, DecodeCommand returned %d"), err);

		if(err == KErrNone && cmd)
			{
			iCommands.AddLast(*cmd);
			}
		}

	// A C-Frame must contain at least one command.
	if(iCommands.IsEmpty())
		{
		err = KErrCorrupt;
		}
	return (err == KErrCompletion ? KErrNone : err);
	}


HL2CapCommand* HCFramePDU::FirstCommand()
	{
	LOG_FUNC
	iCommandIter.SetToFirst();
	return iCommandIter++;
	}

HL2CapCommand* HCFramePDU::NextCommand()
	{
	LOG_FUNC
	return iCommandIter++;
	}
	
	
TInt HCFramePDU::AddCommand(HL2CapCommand& aCommand, CL2CAPMux& aMuxer)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	// Check if there is room in the PDU.
	if((aCommand.CommandLength() + PDUPayloadLength()) <= aMuxer.SigMTU())
		{
		// Add this command onto end of PDU buff
		RMBufChain buf;
		rerr = aCommand.GetCommand(aMuxer, buf);
		if(rerr == KErrNone)
			{
			iPDUData.Append(buf);

			WritePDUPayloadLength();
			}
		}
	else
		{
		rerr = KErrCompletion;
		}
	return rerr;
	}
