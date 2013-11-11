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

#include <es_mbuf.h>
#include <cflog.h>
#include "commsbufpondintf.h"
#include "commsbufpool.h"
//
// MBUF CHAIN
//

__IMPLEMENT_CLEANUP(RMBufChain, Free)


EXPORT_C void RMBufChain::AllocL(TInt aLen)
/**
Allocate sufficient mbufs to hold specfied amount of data

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
	User::LeaveIfError(RCommsBufChain::Alloc(aLen, allocator));
	}

EXPORT_C TInt RMBufChain::Alloc(TInt aLen)
/**
Allocate sufficient mbufs to hold specfied amount of data

@param aLen A length of the cell
*/
	{
	RMBufAllocator allocator;	
	return RCommsBufChain::Alloc(aLen, allocator);
	}

TInt RMBufChain::Alloc(TInt aLen, const RMBufChain &aMBufChain)
/**
Allocate sufficient mbufs to hold specfied amount of data

@param aLen A length of the cell

@return KErrNone if allocation is success 
@return KErrNoMBufs if allocation is failed
*/
	{	
	if(aMBufChain.First())
		{
		const RCommsBuf* commsBuf = static_cast<const RCommsBuf*>(aMBufChain.First());
		iNext = commsBuf->Pool()->Pond().Alloc(aLen, commsBuf->RawSize(), commsBuf->RawSize());
		return iNext ? KErrNone : KErrNoMBufs;		
		}
	RMBufAllocator allocator;
	return RCommsBufChain::Alloc(aLen, allocator);	
	}
	
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, TInt aMinMBufSize)
	{
	RMBufAllocator allocator;
	return RCommsBufChain::Alloc(aLen, aMinMBufSize, allocator);
	}
	
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, TInt aMinMBufSize, TInt aMaxMBufSize)
	{
	RMBufAllocator allocator;
	return RCommsBufChain::Alloc(aLen, aMinMBufSize, aMaxMBufSize, allocator);
	}


// overloading for TLS
EXPORT_C void RMBufChain::AllocL(TInt aLen, RMBufAllocator& aRMBufAllocator)
	{
	User::LeaveIfError(RCommsBufChain::Alloc(aLen, aRMBufAllocator));
	}
	
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, RMBufAllocator& aRMBufAllocator)
	{
	return RCommsBufChain::Alloc(aLen, aRMBufAllocator);
	}
	
TInt RMBufChain::Alloc(TInt aLen, const RMBufChain& aMBufChain, RMBufAllocator& aRMBufAllocator)
	{
	if(aMBufChain.First())
		{
		return RCommsBufChain::Alloc(aLen, aMBufChain.First ()->Size(), aMBufChain.First ()->Size(), aRMBufAllocator);			
		}
	else
		{
		return RCommsBufChain::Alloc(aLen, aLen, aRMBufAllocator);	
		}		
	}
	
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, TInt aMinMBufSize, RMBufAllocator& aRMBufAllocator)
	{
	return RCommsBufChain::Alloc(aLen, aMinMBufSize, aRMBufAllocator);
	}
	
EXPORT_C TInt RMBufChain::Alloc(TInt aLen, TInt aMinMBufSize, TInt aMaxMBufSize, RMBufAllocator& aRMBufAllocator)
	{
	return RCommsBufChain::Alloc(aLen, aMinMBufSize, aMaxMBufSize, aRMBufAllocator);
	}

EXPORT_C TInt RMBufChain::ReAlloc(TInt aLen)
/**
Adjust the size of a chain, allocates a new memory for the chain
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aLen A new length
*/
	{
	__ASSERT_ALWAYS(aLen>=0, CommsBuf::Panic(EMBuf_NegativeLength));

	TInt currLen = Length();
	if(aLen < currLen)
		{
		TrimEnd(aLen);			
		}
	else if(aLen > currLen)
		{
		TInt extraReq = aLen - currLen;
		if(currLen > 0)
			{
			// Extend the final buf to consume any idle space
			RMBuf* pLast = static_cast<RMBuf*>(Last());
			TInt idleSpace = pLast->Size() - pLast->End();
			ASSERT(idleSpace >= 0);
			if(idleSpace)
				{
				TInt useSpace = Min(idleSpace, extraReq);
				pLast->AdjustEnd(useSpace);
				extraReq -= useSpace;
				// Did this yield enough?
				if(!extraReq)
					{
					return KErrNone;						
					}
				}
			}
		// Need additional allocation
		RMBufChain extraChain;
		TInt err = extraChain.Alloc(extraReq, *this);
		if(err != KErrNone)
			{
			return err;
			}
		RCommsBufChain::Append(extraChain);
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



EXPORT_C void RMBufChain::FillZ(TInt aLen)
/**
Zero fill the first aLen bytes of an mbuf chain
@param aLen the length (how many byte to be appended to the end)
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));

	TInt n;
	RMBuf* m = static_cast<RMBuf*>(iNext);

	while(aLen>0 && m!=NULL)
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
	User::LeaveIfError(Copy(newChain, anOffset, aLen, 0));
	}

EXPORT_C TInt RMBufChain::Copy(RMBufChain &newChain, TInt aOffset, TInt aLen, TInt aHdrReserve) const
/**
Copy data into a new chain starting at a given offset
into this chain.
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param newChain A new chain, where the data is copied to
@param aOffset A offset,
@param aLen the length of the data to be copied
@param aHdrReserve Amount of space (bytes) to offset the start of data from the start of the first buffer, so allowing
subsequent prepending without further allocation (typically used for protocol headers)
*/
	{
 	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
 	__ASSERT_ALWAYS(aLen>=0, CommsBuf::Panic(EMBuf_NegativeLength));
 	__ASSERT_ALWAYS(aOffset>=0, CommsBuf::Panic(EMBuf_NegativeOffset));

	TInt n, n1, n2;
	TUint8* p1, *p2;
	RMBuf* m1, *m2;

	TInt len = Length();
	if (aOffset>0)
		{
 		__ASSERT_ALWAYS(aOffset<len, CommsBuf::Panic(EMBuf_BadOffset));
		n = len - aOffset;
		len = Min(aLen, n);
		}
	else
		{
		len = Min(aLen, len);	
		}		

// Suppress the "follow-the-leader" behaviour of preserving the buffer sizing of the
// existing chain. The goal of preserving buffer characteristics remains desirable but
// not at the cost of having TCP use unnecessarily small buffers - this area needs 
// rework once the Comms stack really adopts buffer pools and zero copy  	
//	TInt err = newChain.Alloc(len + aHdrReserve, *this);
	TInt err;
    if(First())
        {
        newChain.iNext = First()->Pool()->Pond().Alloc(len + aHdrReserve, 0, KMaxTInt);
        err = newChain.iNext ? KErrNone : KErrNoMBufs;      
        }
    else
        {
        RMBufAllocator allocator;
        err = newChain.Alloc(aLen + aHdrReserve, allocator);  
        }
	
	if(err != KErrNone)
		{
		return err;
		}

	if (aOffset>0)
		{
		if ((m1 = static_cast<RMBuf*>(RCommsBufChain::Goto(aOffset, n, n1))) == NULL)
			{
			return KErrNone;				
			}
		p1 = m1->Buffer()+n;
		}
	else
		{
		m1 = static_cast<RMBuf*>(iNext);
		p1 = m1->Ptr();
		n1 = m1->Length();
		}

    if (aHdrReserve > 0)
        {
        m2 = static_cast<RMBuf*>(newChain.First());
        if(aHdrReserve >= m2->Length())
            {
            // In principle we could have one or more buffers as header reserve, however it's not
            // obvious whether a chain can contain wholly empty buffers in it, ie whether code
            // walking the chain will have encountered such a situation. So we prevent this - in
            // practice this shouldn't pose a problem as the expected use is reserving capacity
            // within the copied buffer for subsequent pre-pending to avoid allocating another buf 
            newChain.Free();
            return KErrOverflow;
            }
        m2->SetData(aHdrReserve, m2->Length() - aHdrReserve);
        p2 = m2->Ptr();
        n2 = m2->Length();
        }
    else
        {
        m2 = static_cast<RMBuf*>(newChain.iNext);
        p2 = m2->Ptr();
        n2 = m2->Length();
        }
    
	while (len>0)
		{
 		__ASSERT_DEBUG(n1>0 && n2>0, CommsBuf::Panic(EMBuf_NegativeLength));

		n = n1 < n2 ? n1 : n2;

		Mem::Copy(p2, p1, n);

		if (n1 -= n, n1 == 0)
			{
			if (m1 = m1->Next(), m1==NULL)
				{
				break;					
				}

			p1 = m1->Ptr();
			n1 = m1->Length();
			}
		else
			{
			p1 += n;	
			}
			

		if (n2 -= n, n2 == 0)
			{
			if (m2 = m2->Next(), m2==NULL)
				{
				break;					
				}

			p2 = m2->Ptr();
			n2 = m2->Length();
			}
		else
			{
			p2 += n;	
			}
		len -= n;
		}
	return KErrNone;
	}

EXPORT_C void RMBufChain::CopyIn(const TDesC8& aDes, TInt aOffset/*=0*/)
	{
	RCommsBufChain::Write(aDes, aOffset);	
	}

EXPORT_C void RMBufChain::CopyOut(TDes8& aDes, TInt aOffset) const
/**
Copy data from an mbuf chain into linear buffer
starting at specified offset.
@param aDes the buffer to copy in to
@param aOffset the offset
*/
	{
	RCommsBufChain::Read(aDes, aOffset);
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
	__ASSERT_ALWAYS(iNext==NULL, CommsBuf::Panic(EMBuf_NotEmptyChain));
	iNext = aQueue.First();
	aQueue.Init();
	}

EXPORT_C void RMBufChain::Assign(RMBufChain& aChain)
	{
	RCommsBufChain::Assign(aChain);	
	}

EXPORT_C void RMBufChain::Append(RMBufChain& aChain)
	{
	RCommsBufChain::Append(aChain);	
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
	if(iNext == NULL)
		{		
		RMBufAllocator allocator;
		return Append(aLen, allocator);		
		}
	return RCommsBufChain::Append(aLen);		
	}

// overloading for TLS
EXPORT_C TInt RMBufChain::Append(TInt aLen, RMBufAllocator& aRMBufAllocator)
	{
	RMBuf* buf = static_cast<RMBuf*>(aRMBufAllocator.iPond.Alloc(aLen, 0, KMaxTInt));
	if(buf == NULL)
		{
		return KErrNoMBufs;			
		}

	RCommsBufChain::Append(buf);
	return KErrNone;
	}


EXPORT_C RMBuf* RMBufChain::Remove()
/**
Removes and returns the first RCommsBuf. The ownership of the returned RCommsBuf is with the caller
*/
	{
	RMBuf* buf = static_cast<RMBuf*>(iNext);
	if(buf)
		{
		iNext = buf->Next();
		buf->Unlink();
		}

	return buf;
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
	if(iNext)
		{
		return RCommsBufChain::Prepend(aLen);					
		}
	RMBufAllocator allocator;
	return Prepend(aLen, allocator);	
	}
	
// overloading for TLS
EXPORT_C TInt RMBufChain::Prepend(TInt aLen, RMBufAllocator& aRMBufAllocator)
	{
	RMBuf* buf = static_cast<RMBuf*>(aRMBufAllocator.iPond.Alloc(aLen, 0, KMaxTInt));
	if(buf == NULL)
		{
		return KErrNoMBufs;			
		}

	RMBufChain chain(buf);
	RCommsBufChain::Prepend(chain);
	return KErrNone;
	}

EXPORT_C void RMBufChain::Prepend(RMBuf* aBuf)
	{
	RCommsBufChain::Prepend(aBuf);	
	}

EXPORT_C void RMBufChain::Prepend(RMBufChain& aChain)
	{
	RCommsBufChain::Prepend(aChain);	
	}


EXPORT_C TInt RMBufChain::NumBufs() const
/**
Count the number of buffers in a chain
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));

	TInt len = 0;
	RMBuf* m = static_cast<RMBuf*>(iNext);
	for (; m!=NULL; m = m->Next())
		{
		len++;			
		}
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

EXPORT_C TInt RMBufChain::Split(TInt aOffset, RMBufChain& aNewChain)
/** 	  	 
Split a chain into two new chains Original chain gets the 1st half 	  	 
newChain gets the other half. 	  	 
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant 	  	 
@param anOffset The offset 	  	 
@param newChain The result chain 	  	 
*/
	{
    // RCommsBuf::Split() will not allocate a smaller buffer size than the current, as part of its approach of (trying to)
    // support zero-copy transfer by respecting the buffer pool in use. This work is incomplete (needs support throughout
    // the stack and probably a cleverer idea of what constitutes an acceptable buffer than simply size), so in the meantime
    // having MBufMgr reflect this behaviour by refusing to Split() a big buf into smaller bufs is unnecessarily purist.
    // Hence the functionality is implemented directly here
    
    __ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
    __ASSERT_ALWAYS(aOffset>=0, CommsBuf::Panic(EMBuf_NegativeOffset));
    
    // For testing post-conditions
#ifdef _DEBUG
    TInt origLen = Length();
#endif
    TInt splitBufOffset;
    TInt splitBufRemainder;
    RMBuf* splitBuf = NULL;
    RMBuf* splitBufPrev = NULL;
    
    if(!Goto(aOffset, splitBuf, splitBufOffset, splitBufRemainder, splitBufPrev))
        {
        aNewChain.Init();
        return KErrNone;
        }
    
    if(splitBufOffset != splitBuf->Offset()) // Not on an mbuf boundary
        {
        // Copy tail of splitBuf out to a new chain (hopefully a single buf, but needn't be)
        TInt splitDataOffset = splitBufOffset - splitBuf->Offset();
        TInt err = RMBufChain(splitBuf).Copy(aNewChain, splitDataOffset, splitBufRemainder);
        if(err != KErrNone)
            {
            return err;
            }
        splitBuf->AdjustDataEnd(-splitBufRemainder);
        RMBufChain splitTail(splitBuf->Next());
        aNewChain.Append(splitTail);
        splitBuf->SetNext(NULL);
        }
    else
        {
        // Split cleaves chain between bufs
        aNewChain = splitBuf;
        if(splitBufPrev)
            {
            splitBufPrev->Unlink();
            }
        }
    
    // Check post-conditions
#ifdef _DEBUG
    TInt frag1Len = Length(); 
    TInt frag2Len = aNewChain.Length();
    ASSERT(origLen == frag1Len + frag2Len);
    ASSERT(frag1Len == aOffset);
#endif
    return KErrNone;
	}           

// overloading intended to avoid TLS lookup of pool manager. Seemingly not really useful since most Split() 
// operations will have an existing buffer to base a Copy() from
EXPORT_C TInt RMBufChain::Split(TInt anOffset, RMBufChain& newChain, RMBufAllocator& /* aRMBufAllocator */)
	{
	return RMBufChain::Split ( anOffset, newChain );
	}


EXPORT_C void RMBufChain::Free()
/** 	  	 
Free an MBuf chain, returning it to the free Pool 	  	 
A pointer to the first mbuf of the next packet is returned. 	  	 
*/
	{
	RCommsBufChain::Free ();	
	}

EXPORT_C void RMBufChain::TrimStart(TInt nBytes)
/** 	  	 
Trim chain upto offset 	  	 
@param anOffset The offset 	  	 
*/
	{
	RCommsBufChain::TrimStart(nBytes);	
	}

EXPORT_C void RMBufChain::TrimEnd(TInt anOffset)
/** 	  	 
Trim chain after offset 	  	 
@param anOffset The offset 	  	 
*/
	{
	RCommsBufChain::TrimEnd(anOffset);	
	}

EXPORT_C TBool RMBufChain::Goto(TInt anOffset, RMBuf* &resBuf, TInt& resOffset, TInt& resLength, RMBuf* &resPrevBuf) const
/** 	  	 
Goto specified byte offset into an Mbuf chain. Used as part of copyin/out, split etc to position 	  	 
MBuf pointer and offset from start of iBuffer. 	  	 
@param anOffset The offset 	  	 
@param resBuf result buffer 	  	 
@param resOffset result offset 	  	 
@param resLength result length 	  	 
@param resPrevBuf result previous Buf in the chain 	  	 
@return ETrue if successful 	  	 
*/
	{
	RCommsBuf* commsResBuf;
	RCommsBuf* commsResPrevBuf;
	
	commsResBuf = RCommsBufChain::Goto(anOffset, resOffset, resLength, commsResPrevBuf); 	
	if(commsResBuf)
		{
		resBuf = static_cast<RMBuf*>(commsResBuf);
		resPrevBuf = static_cast<RMBuf*>(commsResPrevBuf);
		return ETrue;
		}
	return EFalse;	
	}

EXPORT_C TInt RMBufChain::Length() const
/** 	  	 
Return the number of bytes of actual data contained in an MBuf chain 	  	 
*/
	{
	return RCommsBufChain::Length ();	
	}

EXPORT_C RMBuf* RMBufChain::Last() const
/**
Find the last MBuf in a chain
@return the last MBuf in the chain
*/
	{
	return static_cast<RMBuf*>(RCommsBufChain::Last());
	}


EXPORT_C TInt RMBufChain::Align(TInt aSize)
/**
Ensure that the first aSize bytes can be safely cast
to a structure of size aSize.
@param aSize A size
@return the number of bytes actually aligned. This will be the min of aSize and chain length.
*/
	{
	return RCommsBufChain::Align ( aSize );
	}
