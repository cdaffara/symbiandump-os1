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
// Buffer Manager for Protocols (MBuf Chains)
// 
//

/**
 @file
 @internalComponent
*/

#include <es_mbman.h>
#include "MBufPoolChain.h"
#include "MBufPoolManager.h"
#include <cflog.h>


//
// MBUF CHAIN
//

#ifdef __CFLOG_ACTIVE
__CFLOG_STMT(_LIT8(KSubsysMBufMgr, "MBufMgr");) // subsystem name
__CFLOG_STMT(_LIT8(KComponentPerformance, "performance");)	// component name - used for any sub-optimal performance behaviuor
#endif

__IMPLEMENT_CLEANUP(RMBufChain, Free)


EXPORT_C void RMBufChain::AllocL(TInt aLen)
/**
Allocate sufficient mbufs to hold specfied amount of data,
optionally zeroing the buffers.
- Overloaded min/max variants (eg. refer RMBufChain::Alloc) deliberately do not exist because;
  a. Intention is to ultimately deprecate (or at least discourage) the leaving variants since their usage is not typically (but not always
     the case) recommended because they the throw/catch mechanism can hinder performance if used in appopriately.
  b. Intention is to ultimately deprecate (or at least discourage) this class as it will likely be superceded by a generic system wide
     equivalent.
  c. If they shown to useful for this API, they can easily be added in the future... but the same can not be said for removing them in the future!
@param aLen A length of the cell
*/
	{
	RMBufAllocator allocator;
	return AllocL(aLen, allocator);
	}

EXPORT_C TInt RMBufChain::Alloc(TInt aLen)
/**
Allocate sufficient mbufs to hold specfied amount of data,
optionally zeroing the buffers.
@param aLen A length of the cell
*/
	{
	RMBufAllocator allocator;
	return Alloc(aLen, allocator);
	}
TInt RMBufChain::Alloc(TInt aLen, const RMBufChain &aMBufChain)
	{
	RMBufAllocator allocator;
	return Alloc(aLen, aMBufChain, allocator);
	}
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, TInt aMinMBufSize)
	{
	RMBufAllocator allocator;
	return Alloc(aLen, aMinMBufSize, allocator);
	}
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, TInt aMinMBufSize, TInt aMaxMBufSize)
	{
	RMBufAllocator allocator;
	return Alloc(aLen, aMinMBufSize, aMaxMBufSize, allocator);
	}


// overloading for TLS
EXPORT_C void RMBufChain::AllocL(TInt aLen, RMBufAllocator& aRMBufAllocator)
	{
	__ASSERT_ALWAYS(iNext==NULL, CMBufManager::Panic(EMBuf_NotEmptyChain));

	iNext = aRMBufAllocator.MBufManager().AllocL(aLen);
	}
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, RMBufAllocator& aRMBufAllocator)
	{
	__ASSERT_ALWAYS(iNext==NULL, CMBufManager::Panic(EMBuf_NotEmptyChain));

	iNext = aRMBufAllocator.MBufManager().Alloc(aLen);
	return iNext? KErrNone: KErrNoMBufs;
	}
TInt RMBufChain::Alloc(TInt aLen, const RMBufChain& aMBufChain, RMBufAllocator& aRMBufAllocator)
	{
	__ASSERT_ALWAYS(iNext==NULL, CMBufManager::Panic(EMBuf_NotEmptyChain));

	iNext = aRMBufAllocator.MBufManager().Alloc(aLen, aMBufChain);
	return iNext? KErrNone: KErrNoMBufs;
	}
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, TInt aMinMBufSize, RMBufAllocator& aRMBufAllocator)
	{
	__ASSERT_ALWAYS(iNext==NULL, CMBufManager::Panic(EMBuf_NotEmptyChain));

	iNext = aRMBufAllocator.MBufManager().Alloc(aLen, aMinMBufSize);
	return iNext? KErrNone: KErrNoMBufs;
	}
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, TInt aMinMBufSize, TInt aMaxMBufSize, RMBufAllocator& aRMBufAllocator)
	{
	__ASSERT_ALWAYS(iNext==NULL, CMBufManager::Panic(EMBuf_NotEmptyChain));

	iNext = aRMBufAllocator.MBufManager().Alloc(aLen, aMinMBufSize, aMaxMBufSize);
	return iNext? KErrNone: KErrNoMBufs;
	}

EXPORT_C TInt RMBufChain::ReAlloc(TInt aLen)
/**
Adjust the size of a chain, allocates a new memory for the chain
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aLen A new length
*/
	{
	__ASSERT_ALWAYS(aLen>=0, CMBufManager::Panic(EMBuf_NegativeLength));

	TInt currLen = Length();
	if(aLen < currLen)
		TrimEnd(aLen);
	else if(aLen > currLen)
		{
		TInt extraReq = aLen - currLen;
		if(currLen > 0)
			{
			// Extend the final buf to consume any idle space
			RMBuf* pLast = Last();
			TInt idleSpace = pLast->Size() - pLast->End();
			ASSERT(idleSpace >= 0);
			if(idleSpace)
				{
				TInt useSpace = Min(idleSpace, extraReq);
				pLast->AdjustEnd(useSpace);
				extraReq -= useSpace;
				// Did this yield enough?
				if(!extraReq)
					return KErrNone;
				}
			}
		// Need additional allocation
		RMBufChain extraChain;
		TInt err = extraChain.Alloc(extraReq, *this);
		if(err != KErrNone)
			{
			return err;
			}
		Append(extraChain);
		}
	return KErrNone;
	}

EXPORT_C void RMBufChain::ReAllocL(TInt aLen)
/**
Adjust the size of a chain, allocates a new memory for the chain
@param aLen A new length
*/
	{
	User::LeaveIfError(ReAlloc(aLen));
	}



EXPORT_C TInt RMBufChain::Create(const TDesC8& aDes, TInt aHdrLen)
/**
Create an Mbuf chain from a descriptor optionally allowing room at front for a protocol header.
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aDes
@param aHdrLen A header length
*/
	{
	TInt err = Alloc(aDes.Length() + aHdrLen);
	if(err == KErrNone)
		{
		CopyIn(aDes, aHdrLen);	// NB! old version prepended explicit buf. wonder if that matters?
		}
	return err;
	}


EXPORT_C void RMBufChain::CreateL(const TDesC8& aDes, TInt aHdrLen)
/**
Create an Mbuf chain from a descriptor optionally allowing room at front for a protocol header.
@param aDes
@param aHdrLen A header length
*/
	{
	User::LeaveIfError(Create(aDes, aHdrLen));
	}


EXPORT_C void RMBufChain::Free()
/**
Free an MBuf chain, returning it to the free Pool
A pointer to the first mbuf of the next packet is returned.
*/
	{
	if (iNext!=NULL)
		iNext->Free();
	Init();
	}


EXPORT_C void RMBufChain::FillZ(TInt aLen)
/**
Zero fill the first aLen bytes of an mbuf chain
@param aLen the length (how many byte to be appended to the end)
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));

	TInt n;
	RMBuf* m = iNext;

	while (aLen>0 && m!=NULL)
		{
			n = aLen < m->Length() ? aLen : m->Length();
			Mem::FillZ(m->Ptr(), n);
			aLen -= n;
			m = m->Next();
		}
	}


EXPORT_C void RMBufChain::CopyL(RMBufChain& newChain, TInt anOffset, TInt aLen) const
/**
Copy data into a new chain starting at a given offset
into this chain.
Allocate sufficient mbufs to hold specfied amount of data,
optionally zeroing the buffers.
@param aLen A length of the cell
*/
	{
	User::LeaveIfError(Copy(newChain, anOffset, aLen));
	}

EXPORT_C TInt RMBufChain::Copy(RMBufChain &newChain, TInt anOffset, TInt aLen) const
/**
Copy data into a new chain starting at a given offset
into this chain.
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param newChain A new chain, where the data is copied to
@param anOffset A offset,
@param aLen the length of the data to be copied
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(aLen>=0, CMBufManager::Panic(EMBuf_NegativeLength));
	__ASSERT_ALWAYS(anOffset>=0, CMBufManager::Panic(EMBuf_NegativeOffset));

	TInt n, n1, n2;
	TUint8* p1, *p2;
	RMBuf* m1, *m2;

	TInt len = Length();
	if (anOffset>0)
		{
		__ASSERT_ALWAYS(anOffset<len, CMBufManager::Panic(EMBuf_BadOffset));
		n = len - anOffset;
		len = Min(aLen, n);
		}
	else
		len = Min(aLen, len);

	TInt err = newChain.Alloc(len, *this);
	if(err != KErrNone)
		{
		return err;
		}

	if (anOffset>0)
		{
		if (!Goto(anOffset, m1, n, n1))
			return KErrNone;
		p1 = m1->Buffer()+n;
		}
	else
		{
		m1 = iNext;
		p1 = m1->Ptr();
		n1 = m1->Length();
		}

	m2 = newChain.iNext;
	p2 = m2->Ptr();
	n2 = m2->Length();

	while (len>0)
		{
		__ASSERT_DEBUG(n1>0 && n2>0, CMBufManager::Panic(EMBuf_NegativeLength));

		n = n1 < n2 ? n1 : n2;

		Mem::Copy(p2, p1, n);

		if (n1 -= n, n1 == 0)
			{
			if (m1 = m1->Next(), m1==NULL)
				break;
			p1 = m1->Ptr();
			n1 = m1->Length();
			}
		else
			p1 += n;

		if (n2 -= n, n2 == 0)
			{
			if (m2 = m2->Next(), m2==NULL)
				break;
			p2 = m2->Ptr();
			n2 = m2->Length();
			}
		else
			p2 += n;

		len -= n;
		}
	return KErrNone;
	}


EXPORT_C void RMBufChain::CopyIn(const TDesC8& aDes, TInt aOffset)
/**
Copy data into an mbuf chain from a linear buffer
starting at specified offset.
@param aDes The buffer from where data is copied to the chain
@param aOffset The offset
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(aOffset>=0, CMBufManager::Panic(EMBuf_NegativeOffset));

	TUint8* ptr = (TUint8*)aDes.Ptr();
	TInt len = aDes.Length();

	TInt n;
	RMBuf* m;

	if (aOffset>0)
		{
		TInt o;

		if (!Goto(aOffset, m, o, n))
			return;

		if (n>len)
			n = len;

		Mem::Copy(m->Buffer()+o, ptr, n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
	else
		m = iNext;

	while (len>0 && m!=NULL)
		{
		n = len > m->Length() ? m->Length() : len;
		Mem::Copy(m->Ptr(), ptr, n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
	}


EXPORT_C void RMBufChain::CopyOut(TDes8& aDes, TInt aOffset) const
/**
Copy data from an mbuf chain into linear buffer
starting at specified offset.
@param aDes the buffer to copy in to
@param aOffset the offset
*/
	{
//removed after internal discussion; generally unhelpful as empty == zero length is ok concept
//	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(aOffset>=0, CMBufManager::Panic(EMBuf_NegativeOffset));

	TUint8* ptr = (TUint8*)aDes.Ptr();
	TInt len = aDes.Length();

	TInt n;
	RMBuf* m;

	if (aOffset>0)
		{
		TInt o;

		if (!Goto(aOffset, m, o, n))
			{
			aDes.SetLength(0);
			return;
			}

		if (n>len)
			n = len;

		Mem::Copy(ptr, m->Buffer()+o, n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
	else
		m = iNext;

	while (len>0 && m!=NULL)
		{
		n = len > m->Length() ? m->Length() : len;
		Mem::Copy(ptr, m->Ptr(), n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
	aDes.SetLength(ptr-aDes.Ptr());
	}


EXPORT_C void RMBufChain::Assign(RMBufQ& aQueue)
/**
Take ownership of Mbuf from a queue
Previously allocated data (e.g. by a call to RMBufChain::AllocL) in the chain must be
emptied (e.g. by calling RMBufChain::Free) before the assignment
@param aQueue the queue
@see RMBufChain::IsEmpty()
@see RMBufChain::Free()
*/
	{
	__ASSERT_ALWAYS(iNext==NULL, CMBufManager::Panic(EMBuf_NotEmptyChain));
	iNext = aQueue.First();
	aQueue.Init();
	}


EXPORT_C void RMBufChain::Assign(RMBufChain& aChain)
/**
Take ownership of Mbuf from another chain
Previously allocated data (e.g. by a call to RMBufChain::AllocL) in the chain must be
emptied (e.g. by calling RMBufChain::Free) before the assignment
@param aChain the chain
@see RMBufChain::IsEmpty()
@see RMBufChain::Free()
*/
	{
	__ASSERT_ALWAYS(iNext==NULL, CMBufManager::Panic(EMBuf_NotEmptyChain));
	iNext = aChain.iNext;
	aChain.Init();
	}


EXPORT_C void RMBufChain::Append(RMBufChain& aChain)
/**
Append an MBuf chain, taking ownership of the MBufs
@param aChain the chain to be appended
*/
	{
	__ASSERT_ALWAYS(iNext!=aChain.iNext, CMBufManager::Panic(EMBuf_CircularRef));

	if (iNext!=NULL)
		{
		RMBuf* last	= Last();
		last->Link(aChain.iNext);
		}
	else
		iNext = aChain.iNext;

	aChain.Init();
	}


EXPORT_C void RMBufChain::AppendL(TInt aLen)
/**
Append space to the end of a MBuf chain
@param aLen the length (how many byte to be appended to the end)
*/
	{
	User::LeaveIfError(Append(aLen));
	}

EXPORT_C TInt RMBufChain::Append(TInt aLen)
/**
Append space to the end of a MBuf chain
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aLen the length (how many byte to be appended to the end)
*/
	{
	RMBufAllocator allocator;
	return Append(aLen, allocator);
	}

// overloading for TLS
EXPORT_C TInt RMBufChain::Append(TInt aLen, RMBufAllocator& aRMBufAllocator)
	{
	if (iNext==NULL)
		{
		return Alloc(aLen, *this, aRMBufAllocator);
		}

	RMBuf* last = Last();
	// Amount of space available on end of last Mbuf
	TInt n = Min(last->Size()-(last->Offset()+last->Length()), aLen);
	// Amount of space that needs to be Allocated in new buffers
	TInt m = aLen-n;

	if (m>0)
		{
		RMBufChain chn;
		TInt err = chn.Alloc(m, *this, aRMBufAllocator);
		if(err != KErrNone)
			{
			return err;
			}
		last->Link(chn.iNext);
		}
	// Left to last to avoid modifying the chain unless the extension suceeds
	last->AdjustEnd(n);
	return KErrNone;
	}


EXPORT_C RMBuf* RMBufChain::Remove()
/**
Remove 1st mbuf from chain
@param aChain the chain to be prepended
*/
	{
	if (IsEmpty())
		return NULL;
	else
		{
		RMBuf* m = iNext;
		iNext = iNext->Next();
		m->Unlink();
		return m;
		}
	}


EXPORT_C void RMBufChain::Prepend(RMBufChain& aChain)
/**
Prepend an MBuf chain, taking ownership of the MBufs
@param aChain the chain to be prepended
*/
	{
	if (iNext!=NULL)
		{
		RMBuf* last = aChain.Last();
		last->Link(iNext);
		}
	iNext = aChain.iNext;
	aChain.Init();
	}


EXPORT_C void RMBufChain::Prepend(RMBuf* aBuf)
/**
Prepend an MBuf to a chain, taking ownership of the MBuf
@param aBuf the buffer to be prepended
*/
	{
	aBuf->Link(iNext);
	iNext = aBuf;
	}


EXPORT_C void RMBufChain::PrependL(TInt aLen)
/**
Prepend space onto the front of a chain
@param aLen the length of the space
*/
	{
	User::LeaveIfError(Prepend(aLen));
	}

EXPORT_C TInt RMBufChain::Prepend(TInt aLen)
/**
Prepend space onto the front of a chain
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aLen the length of the space
*/
	{
	RMBufAllocator allocator;
	return Prepend(aLen, allocator);
	}

// overloading for TLS
EXPORT_C TInt RMBufChain::Prepend(TInt aLen, RMBufAllocator& aRMBufAllocator)
	{
	__ASSERT_ALWAYS(aLen>=0, CMBufManager::Panic(EMBuf_NegativeLength));

	// If can fit and the start would remain aligned
	// then prepend by moving offset and length
 	// if (iNext->Offset()>=aLen && IS_ALIGNED(iNext->Offset()-aLen))
	if (iNext && iNext->Offset()>=aLen)
		{
		iNext->AdjustStart(-aLen);
		return KErrNone;
		}

	// else allocation required
#ifdef __CFLOG_ACTIVE
	__CFLOG_VAR((KSubsysMBufMgr, KComponentPerformance, _L8("RMBufChain::Prepend() - Warning! sub-optimal performance; prepend required to alloc a new mbuf/chain, iOffset=%d reqLen=%d"),
		iNext?iNext->Offset():0, aLen));
#endif
	RMBuf* m=NULL;

	m = aRMBufAllocator.MBufManager().Alloc(aLen);
	if(!m)
		{
		return KErrNoMBufs;
		}

    // Iterate along the mbuf chain to determine the size and find the last in the chain.
    RMBuf* next = m;
	RMBuf* last;
	do
	{
		last = next;
		aLen -= last->Length();
	    next = last->Next();
	}
	while(next);

	// Performance enhancement - most of these prepends are 20 byes or so
	// This allows the next one to use this MBuf
	if (aLen<0)
		{
		m->SetData(-aLen, m->Length()+aLen);
		}

	last->Link(iNext);
	iNext = m;
	return KErrNone;
	}

EXPORT_C TInt RMBufChain::NumBufs() const
/**
Count the number of buffers in a chain
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));

	TInt len = 0;
	RMBuf* m;
	for (m = iNext; m!=NULL; m = m->Next())
		len++;

	return len;
	}


EXPORT_C TInt RMBufChain::Length() const
/**
Return the number of bytes of actual data contained
in an MBuf chain
*/
	{
//removed after internal & 3rd party discussion; generally unhelpful as empty == zero length is ok concept
//	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));

	TInt len = 0;
	RMBuf* m;
	for (m = iNext; m!=NULL; m = m->Next())
		len += m->Length();

	return len;
	}


EXPORT_C void RMBufChain::SplitL(TInt anOffset, RMBufChain& newChain)
/**
Split a chain into two new chains
Original chain gets the 1st half
newChain gets the other half.
@param anOffset The offset
@param newChain The result chain
*/
	{
	User::LeaveIfError(Split(anOffset, newChain));
	}

EXPORT_C TInt RMBufChain::Split(TInt anOffset, RMBufChain& newChain)
/**
Split a chain into two new chains
Original chain gets the 1st half
newChain gets the other half.
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param anOffset The offset
@param newChain The result chain
*/
	{
	RMBufAllocator allocator;
	return Split(anOffset, newChain, allocator);
	}

// overloading for TLS
EXPORT_C TInt RMBufChain::Split(TInt anOffset, RMBufChain& newChain, RMBufAllocator& aRMBufAllocator)
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(anOffset>=0, CMBufManager::Panic(EMBuf_NegativeOffset));

	if (anOffset==0)
		{
		newChain.iNext = NULL;
		return KErrNone;
		}

	TInt o, n;
	RMBuf* m, *p;

	if (!Goto(anOffset, m, o, n, p))
		return KErrNone;

	if (o!=m->Offset()) // Not on an mbuf boundary
		{
		// get another mbuf and copy extra data
		RMBuf* m2;
		m2 = aRMBufAllocator.MBufManager().Alloc(n, n);
		if(!m2)
			{
			return KErrNoMBufs;
			}

		__ASSERT_DEBUG(m2->Size() >= n, CMBufManager::Panic(EMBuf_TooSmall));
		Mem::Copy(m2->Ptr(), m->Buffer()+o, n);		// trs; possible future enhancement to optionally not split the mbuf unless requested, thus avoiding the copy, but kept as is to avoid a functional break
		m2->SetLength(n);
		m2->Link(m->Next());
		m->AdjustEnd(-n);
		m->Unlink();
		newChain = m2;
		}
	else
		{
		p->Unlink();
		newChain = m;
		}
	return KErrNone;
	}

EXPORT_C void RMBufChain::TrimStart(TInt anOffset)
/**
Trim chain upto offset
@param anOffset The offset
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(anOffset>=0, CMBufManager::Panic(EMBuf_NegativeOffset));

	if (anOffset==0)
		return;

	RMBuf* m, *p;
	TInt o, n;
	if (!Goto(anOffset, m, o, n, p))
		{
		Free();
		return;
		}

	if (p!=NULL) // m not first mbuf?
		{
		p->Unlink();
		iNext->Free();

		iNext = m;
		}

	if (o!=m->Offset()) // not at mbuf boundary?
		m->SetData(o, n);
	}

EXPORT_C void RMBufChain::TrimEnd(TInt anOffset)
/**
Trim chain after offset
@param anOffset The offset
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(anOffset>=0, CMBufManager::Panic(EMBuf_NegativeOffset));

	if(anOffset==0)
		{
		Free();
		return;
		}

	RMBuf* m, *p;
	TInt o, n;
	if (!Goto(anOffset, m, o, n, p))
		return;

	if (o!=m->Offset()) // not at mbuf boundary?
		{
		m->AdjustEnd(-n);
		if (p = m->Next(), p!=NULL)
			{
			m->Unlink();
			p->Free();
			}
		}
	else
		{
		if (p!=NULL)
			p->Unlink();
			m->Free();
		}
	}


EXPORT_C TBool RMBufChain::Goto(TInt anOffset, RMBuf* &resBuf, TInt& resOffset, TInt& resLength, RMBuf* &resPrevBuf) const
/**
Goto specified byte offset into an Mbuf chain
Used as part of copyin/out, split etc to position
MBuf pointer and offset from start of iBuffer.
@param anOffset The offset
@param resBuf result buffer
@param resOffset result offset
@param resLength result length
@param resPrevBuf result previous Buf in the chain
@return ETrue if successful
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(anOffset>=0, CMBufManager::Panic(EMBuf_NegativeOffset));

	TInt o = 0;
	RMBuf* p = NULL, *m = iNext;

	while (m!=NULL)
		{
		if (o + m->Length() > anOffset)
			{
			resBuf = m;
			resOffset = (anOffset - o) + m->Offset();
			resLength = m->Length() - (anOffset - o);
			resPrevBuf = p;
			return ETrue;
			}
		o += m->Length();
		p = m;
		m = m->Next();
		}

	// Attempt to goto beyond end of chain
	__ASSERT_ALWAYS(o==anOffset, CMBufManager::Panic(EMBuf_BadOffset));
	return EFalse;
	}

EXPORT_C RMBuf* RMBufChain::Last() const
/**
Find the last MBuf in a chain
@return the last MBuf in the chain
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));

	return iNext->Last();
	}


EXPORT_C TInt RMBufChain::Align(TInt aSize)
/**
Ensure that the first aSize bytes can be safely cast
to a structure of size aSize.
@param aSize A size
@return the number of bytes actually aligned. This will be the min of aSize and chain length.
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CMBufManager::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(aSize>=0, CMBufManager::Panic(EMBuf_NegativeLength));

	// update length to the largest sized mbuf possible
	if (aSize == KMBufAll)	// trs; does the KMBufAll concept (ie. 'largest sized' mbuf) really make sense? code kept as is to avoid a functional break
		{
		aSize = ((RMBufPoolChain*)iNext->MBufPoolChain())->MBufPoolManager().LargestMBufSize();
		}

	// All data required is already in the first MBuf
	if (aSize <= iNext->Length())
		{
		// Case 1 - allready aligned
		// Case 2 - it needs to be aligned
		if (!IS_ALIGNED(iNext->Offset()))
			{
			Mem::Copy(iNext->Buffer(), iNext->Ptr(), iNext->Length());
			iNext->SetOffset(0);
			}
		return aSize; // already as required
		}

	// Get existing data at start
	if (iNext->Offset()!=0)
		{
		Mem::Copy(iNext->Buffer(), iNext->Ptr(), iNext->Length());
		iNext->SetOffset(0);
		}

	RMBuf* m = iNext->Next();
	TInt len = iNext->Length();
	while (len<aSize && m!=NULL)
		{
		TInt n = aSize-len;
		if (n>m->Length())
			n = m->Length();
		Mem::Copy(iNext->EndPtr(), m->Ptr(), n);
		iNext->AdjustEnd(n);
		m->AdjustStart(n);
		len += n;

		// MBuf might now be empty so free it
		if (m->Length()==0)
			{
			iNext->Link(m->Next());
			m->Unlink();
			m->Free();
			m = iNext->Next();
			}
		}
	return len;
	}
