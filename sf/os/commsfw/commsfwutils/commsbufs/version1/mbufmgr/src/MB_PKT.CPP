// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MBuf Packet Info Headers 
// 
//

/**
 @file
 @internalComponent
*/

#include <nifmbuf.h>


//
// MBuf Manager Extensions
//

__IMPLEMENT_CLEANUP(RMBufPacketBase, Free)

EXPORT_C RMBufPacketBase::RMBufPacketBase()
/**
Constructor
*/
    {
	iInfo=NULL;
	}

EXPORT_C void RMBufPacketBase::CopyInfoL(RMBufPacketBase& aNewPkt)
/**
Copy the information header
@param aNewPkt the new packet where the header is copied to
*/
	{
	aNewPkt.iInfo = (RMBufPktInfo*)iInfo->CopyL();
	}


EXPORT_C RMBufPktInfo* RMBufPacketBase::PeekInfo()
/**
Allows the packet info header to be accessed without unpacking
@return the packet info header
*/
	{
	__ASSERT_DEBUG(iInfo==NULL, MBufExtPanic(EMbExtPanic_PacketHeader));
	__ASSERT_ALWAYS(!IsEmpty(), MBufExtPanic(EMbExtPanic_PacketHeaderEmpty));
	__ASSERT_ALWAYS(First()->Type()==EMBufHeader, MBufExtPanic(EMbExtPanic_PacketHeader));
	return (RMBufPktInfo*)(First()->Ptr());
	}


EXPORT_C RMBufPktInfo* RMBufPacketBase::Unpack()
/**
Splits off the info header
@return the packet info header
*/
	{
	__ASSERT_DEBUG(iInfo==NULL, MBufExtPanic(EMbExtPanic_PacketHeader));
	__ASSERT_ALWAYS(!IsEmpty(), MBufExtPanic(EMbExtPanic_PacketHeaderEmpty));
	__ASSERT_ALWAYS(First()->Type()==EMBufHeader, MBufExtPanic(EMbExtPanic_PacketHeader));
	iInfo = (RMBufPktInfo*)(Remove()->Ptr());
	return Info();
	}


EXPORT_C void RMBufPacketBase::Pack()
/**
Combine info header with data ready for queueing
*/
	{
	__ASSERT_ALWAYS(iInfo!=NULL, MBufExtPanic(EMbExtPanic_PacketHeader));
	Prepend(iInfo->MBuf());
	iInfo = NULL;
	}


EXPORT_C void RMBufPacketBase::SetInfo(RMBufPktInfo* aInfo)
/**
Associate an info header with packet
(Also used to disassociate if aInfo==NULL)
@param aInfo the info header
*/
	{
	iInfo = aInfo;
	}


EXPORT_C void RMBufPacketBase::FreeInfo()
/**
Free info header only 
*/
	{
	if (iInfo)
		{
		iInfo->MBuf()->Free();
		iInfo = NULL;
		}
	}

EXPORT_C void RMBufPacketBase::Free()
/**
Delete info and data
*/
	{
	FreeInfo();
	RMBufChain::Free();
	}


EXPORT_C RMBufPktInfo* RMBufPacketBase::Info()
/**
Returns pointer to info header
@return the info header
*/
	{
	__ASSERT_ALWAYS(iInfo!=NULL, MBufExtPanic(EMbExtPanic_PacketHeader));
	return (RMBufPktInfo*)iInfo;
	}

EXPORT_C RMBufPktInfo* RMBufPacketBase::CreateL(RMBufQ& aPacket, TInt aLength, TUint aMBufHdrSize)
/**
Create an MBuf chain from an received MBuf Queue
 - used by Interfaces
 - queue in emptied.
@param aPacket the MBuf Queue
@param aLength the chain length
@param aMBufHdrSize the MBuf header size
@return the info header
*/
	{
	iInfo = new (ELeave, aMBufHdrSize-sizeof(RMBufPktInfo)) RMBufPktInfo;
	Assign(aPacket);
	iInfo->iLength = aLength==0 ? Length() : aLength;
	return (RMBufPktInfo*)iInfo;
	}

EXPORT_C RMBufPktInfo* RMBufPacketBase::CreateL(TInt aLength, TUint aMBufHdrSize)
/**
Create an empty MBuf chain of required length and header size
 - used by Interfaces
 - refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aLength the chain length
@param aMBufHdrSize the header size
@return the info header
*/
	{
	RMBufChain::AllocL(aLength);	
	CleanupStack::PushL(*this);
	iInfo = new (ELeave, aMBufHdrSize-sizeof(RMBufPktInfo)) RMBufPktInfo;
	CleanupStack::Pop();
	iInfo->iLength = aLength;
	return (RMBufPktInfo*)iInfo;
	}

EXPORT_C RMBufPktInfo* RMBufPacketBase::CreateL(const TDesC8 &aDesc, TInt aHdrSize, TUint aMBufHdrSize)
/**
Create a MBuf chain and header for receiving from a descriptor
 - used by SAPs
@param aDesc the buffer which contains the header info
@param aHdrSize the header size 
@param aMBufHdrSize the MBuf header size
@return the info header
*/
	{
	RMBufChain::CreateL(aDesc, aHdrSize);
	CleanupStack::PushL(*this);
	iInfo = new (ELeave, aMBufHdrSize-sizeof(RMBufPktInfo)) RMBufPktInfo;
	CleanupStack::Pop();
	iInfo->iLength = aDesc.Length()+aHdrSize;
	return (RMBufPktInfo*)iInfo;
	}

EXPORT_C RMBufPktInfo* RMBufPacketBase::CreateCopyL(const RMBufQ& aPacket, TInt aOffset, TInt aLength, TInt aHdrSize, TUint aMBufHdrSize)
/**
Create a MBuf chain from an received MBuf Queue
 - used by Interfaces
@param aPacket the MBuf Queue
@param aOffset the offset 
@param aLength the length of the data to be copied
@param aHdrSize the header size
@param aMBufHdrSize the MBuf Header Size
@return the info header
*/
	{
	iInfo = new (ELeave, aMBufHdrSize-sizeof(RMBufPktInfo)) RMBufPktInfo;
	iInfo->iLength = (aLength==0 ? Length() : aLength) + aHdrSize;
	((RMBufChain&)aPacket).CopyL(*this, aOffset, aLength);
	PrependL(aHdrSize);
	return (RMBufPktInfo*)iInfo;
	}

EXPORT_C RMBufPktInfo* RMBufPacketBase::CreateCopyL(RMBufStreamQ& aStream, TInt aOffset, TInt aLength, TInt aHdrSize, TUint aMBufHdrSize)
/**
Create a MBuf chain by copying data from a stream queue (basically an RMBufQ)
aOffset and aLength specify location in stream queue.
@param aStream the stream queue
@param aOffset the offset
@param aLength the data length
@param aHdrSize the header size
@param aMBufHdrSize the MBuf Header size
@return the info header
*/
	{
	iInfo = new (ELeave, aMBufHdrSize-sizeof(RMBufPktInfo)) RMBufPktInfo;
	iInfo->iLength = (aLength==0 ? Length() : aLength) + aHdrSize;
	aStream.CopySegmentL(*this, aOffset, aLength);
	PrependL(aHdrSize);
	return (RMBufPktInfo*)iInfo;
	}


EXPORT_C RMBufPktInfo* RMBufPacketBase::CreateCopyL(const RMBufPktQ& aList, TInt aHdrSize, TUint aMBufHdrSize)
/**
Build a MBuf chain by copying a packet queue list, leaving space on the front
for a header. (Very heavily used in PPP option processing).
This is specially coded rather than using combination of RMBufChain::CopyL
and RMBufChain::AppendL to ensure that the resulting packet is stored efficiently.
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aList the packet queue
@param aHdrSize the header size
@param aMBufHdrSize the MBuf Header size
@return the info header
*/
	{
	// min K_MBufSmallSize sized mbuf is a valid assumption, because all headers are assumed to derive from a cell, which in turn is size K_MBufSmallSize
	__ASSERT_ALWAYS(aHdrSize<KMBufSmallSize, MBufExtPanic(EMbExtPanic_HeaderTooBig));
	
	iInfo = new (ELeave, aMBufHdrSize-sizeof(RMBufPktInfo)) RMBufPktInfo;

	RMBufChain i;

	// Get total length of chain needed
	TInt len = 0;
	i = aList.First();
	while (!i.IsEmpty())
		{
		len += i.Length();
		i = i.Next();
		}
	
	// Allocate the chain
	iInfo->iLength = len+aHdrSize;
	AllocL(iInfo->iLength);
	
	// Do the copy
	TInt n, n1, n2;
	TUint8* p1, * p2;
	RMBuf* m1, * m2;

	i = aList.First();
	m1 = i.First();
	p1 = m1->Ptr();
	n1 = m1->Length();

	m2 = First();
	p2 = m2->Ptr();
	n2 = m2->Length();

	if (aHdrSize>0)
		{
		if (aHdrSize==KMBufSmallSize) // min K_MBufSmallSize sized mbuf is a valid assumption, because all headers are assumed to derive from a cell, which in turn is size K_MBufSmallSize
			{
			m2 = m2->Next();
			p2 = m2->Ptr();
			n2 = m2->Length();
			}
		else
			{
			p2 += aHdrSize;		
			n2 -= aHdrSize;
			}
		}

	while (len>0)
		{
		n = n1<n2 ? n1 : n2;
		Mem::Copy(p2, p1, n);
		if (n1 -= n, n1==0)
			{
			if (m1 = m1->Next(), m1==NULL)
				{
				i = i.Next();
				m1 = i.First();
				}
			p1 = m1->Ptr();
			n1 = m1->Length();
			}
		else
			p1 += n;
		if (n2 -= n, n2==0)
			{
			m2 = m2->Next();
			p2 = m2->Ptr();
			n2 = m2->Length();
			}
		else
			p2 += n;
		len -= n;
		}
	
	return (RMBufPktInfo*)iInfo;
	}


EXPORT_C void RMBufPacketBase::TrimStart(TInt aOffset)
/**
As RMBufChain::TrimStart, but updates length in info header
@param aOffset the offset 
*/
	{
	RMBufChain::TrimStart(aOffset);
	iInfo->iLength -= aOffset;
	}


EXPORT_C void RMBufPacketBase::TrimEnd(TInt aOffset)
/**
As RMBufChain::TrimEnd, but updates length in info header
@param aOffset the offset
*/
	{
// !BUG! Work around
	if (aOffset==0)
		RMBufChain::Free();
	else
// End work-around
	RMBufChain::TrimEnd(aOffset);
	iInfo->iLength = aOffset;
	}


EXPORT_C RMBufPktInfo* RMBufPacketBase::NewInfoL(TUint aMBufHdrSize)
/**
Create an info header 
@param aMBufHdrSize the MBuf header size
@return the info header
*/
	{
	iInfo = new (ELeave, aMBufHdrSize-sizeof(RMBufPktInfo)) RMBufPktInfo;
	return (RMBufPktInfo*)iInfo;
	}

EXPORT_C RMBufPktInfo* RMBufPacketBase::NewInfo(TUint aMBufHdrSize)
/**
Create an info header 
@param aMBufHdrSize the MBuf header size
@return the info header
*/
	{
	iInfo = new (aMBufHdrSize-sizeof(RMBufPktInfo)) RMBufPktInfo;
	return (RMBufPktInfo*)iInfo;
	}

EXPORT_C void RMBufPacketBase::CopyPackedL(RMBufChain& aPacket)
/**
Copy an entire packed packet, complete with info header
@param aPacket the packet
*/
	{
	__ASSERT_DEBUG(iInfo==NULL, MBufExtPanic(EMbExtPanic_PacketHeader));
	__ASSERT_ALWAYS(!aPacket.IsEmpty(), MBufExtPanic(EMbExtPanic_PacketHeaderEmpty));
	__ASSERT_ALWAYS(aPacket.First()->Type()==EMBufHeader, MBufExtPanic(EMbExtPanic_PacketHeader));
	RMBuf* m = aPacket.Remove();
	//
	// The following assumes that aPacket.CopyL would actually leave before it makes any change 
	// to aPacket at all. Inspection of CopyL reveals that this is the case.
	//		
	TRAPD(err, aPacket.CopyL(*this));
	//
	// In case CopyL failed, we'd need to restore aPacket, so we'd call Prepend.
	// In case it succedded, we'd still need to call Prepend so that the info block remains the same.
	// Hence we call aPacket.Prepend irrespective of aPacket.CopyL failure.
	//
	aPacket.Prepend(m);
	User::LeaveIfError(err);
	
	RMBufPktInfo* info = (RMBufPktInfo*)((RMBufPktInfo*)(m->Ptr()))->CopyL();
	Prepend(info->MBuf());
	}

EXPORT_C RMBufPktInfo* RMBufPacketBase::PeekInfoInChain(RMBufChain& aChain)
/**
Allows the packet info header to be accessed while still on a packet queue
@param aChain the chain to be accessed
@return the info header
*/
	{
	__ASSERT_ALWAYS(!aChain.IsEmpty(), MBufExtPanic(EMbExtPanic_PacketHeaderEmpty));
	__ASSERT_ALWAYS(aChain.First()->Type()==EMBufHeader, MBufExtPanic(EMbExtPanic_PacketHeader));
	return (RMBufPktInfo*)(aChain.First()->Ptr());
	}

void MBufExtPanic(TMBufExtPanic aPanic)
/**
extra panic function that causes panic
@param panic code
*/
 	{
	_LIT(mBufExtString,"MBufExt");
	User::Panic(mBufExtString, aPanic);
	}

