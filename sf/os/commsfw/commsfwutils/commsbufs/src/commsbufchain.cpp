// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "es_commsbuf_internal.h"
#include <comms-infras/mbufpanic.h>
#include <comms-infras/commsbufpondop.h>
#include "commsbufpondintf.h"
#include "commsbufpool.h"

RCommsBuf* RCommsBufChain::Last()
/**
Returns the pointer to the last RCommsBuf in the chain
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
	
	TCommsBufIter it(iNext);
	
	RCommsBuf* buf = it.Current();
	while(!(++it).AtEnd())
		buf = it.Current();		
	return buf;
	}

EXPORT_C RCommsBuf* RCommsBufChain::Last() const
/**
Returns the const pointer to the last RCommsBuf in the chain
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
		
	TCommsBufIter it(iNext);
	RCommsBuf* buf = it.Current();
	while(!(++it).AtEnd())
		buf = it.Current();		
	return buf;
	}

EXPORT_C TInt RCommsBufChain::Length() const
/**
Returns the actual number of bytes present in the chain
*/
	{
	TInt len = 0;
	TCommsBufIter it(iNext);
	while(!it.AtEnd())
		{
		len += it.Current()->Length();
		++it;
		}

	return len;
	}

EXPORT_C void RCommsBufChain::Assign(RCommsBufChain& aChain)	
/**
Assign the supplied RCommsBufChain to "this" chain. After assign the supplied 
RCommsBufChain will be reset and becomes empty

@param aChain The chain to be assigned

*/
	{
	__ASSERT_ALWAYS(iNext==NULL, CommsBuf::Panic(EMBuf_NotEmptyChain));
	
	iNext = aChain.First();
	aChain.iNext = NULL;			
	}

EXPORT_C void RCommsBufChain::Append(RCommsBufChain& aChain)
/**
Append the supplied RCommsBufChain to "this" chain. After append the supplied
RCommsBufChain will be reset and becomes empty

@param aChain The chain to be appended
*/
	{
	 __ASSERT_ALWAYS(iNext!=aChain.iNext, CommsBuf::Panic(EMBuf_CircularRef));
	
	if(iNext != NULL)
		{
		RCommsBuf* last = Last();
		last->SetNext(aChain.iNext);			
		}
	else
		iNext = aChain.First();
	aChain.iNext = NULL;
	}
	
EXPORT_C void RCommsBufChain::Append(RCommsBuf* aBuf)
/**
Append the supplied RCommsBuf to "this" chain. 

@param aBuf The RCommsBuf to be appended
*/
	{
	 __ASSERT_ALWAYS(iNext!=aBuf, CommsBuf::Panic(EMBuf_CircularRef));
	
	if(iNext == NULL)
		{
		iNext = aBuf;
		return;			
		}	
	Last()->SetNext(aBuf);	
	}
	
EXPORT_C TInt RCommsBufChain::Append(TInt aLen)
/**
Append space to the end of "this" chain. More RCommsBuf will be allocated if
available append length is exceeding the supplied length

@aLen Length to appended
@return TInt KErrNoMemory on failure otherwise KErrNone
*/
	{
	__ASSERT_ALWAYS(iNext, CommsBuf::Panic(EMBuf_EmptyChain));
	TInt lenToAllocate = aLen;
	RCommsBuf* last = NULL;
	if (iNext != NULL)
		{
		last = Last();
		// Amount of space available on end of last Mbuf
		TInt n = Min(last->RawSize()-(last->Offset()+last->Length()), aLen);
		// Amount of space that needs to be Allocated in new buffers
		lenToAllocate = aLen-n;
		last->AdjustDataEnd(n);
		}

	if (lenToAllocate > 0)
		{
		RCommsBuf* buf = last->Pool()->Pond().Alloc(lenToAllocate, last->RawSize(), KMaxTInt);;

		if (buf == NULL)
			{
			return KErrNoMBufs;
			}
		else
			{
			Append(buf);
			}
		}
	return KErrNone;
	}

EXPORT_C void RCommsBufChain::Prepend(RCommsBufChain& aChain)
/**
Prepend the supplied RCommsBufChain to "this" chain. After prepend the supplied
RCommsBufChain will be reset and becomes empty

@param aChain The chain to be prepended
*/
	{
	if (iNext !=NULL)
		{
		RCommsBuf* last = aChain.Last();
		last->SetNext(iNext);
		}
	iNext = aChain.iNext;
	aChain.iNext = NULL;		
	}
	
EXPORT_C void RCommsBufChain::Prepend(RCommsBuf* aBuf)
/**
Prepend the supplied RCommsBuf to "this" chain. 

@param aBuf The RCommsBuf to be prepended
*/
	{
	aBuf->SetNext(iNext);
	iNext = aBuf;
	}

EXPORT_C TInt RCommsBufChain::Prepend(TInt aLen)
/**
Prepend space to the end of "this" chain. More RCommsBuf will be allocated if
available prepend length is exceeding the supplied length

@aLen Length to prepended
@return TInt KErrNoMBufs on failure otherwise KErrNone
*/
	{
	__ASSERT_ALWAYS(iNext, CommsBuf::Panic(EMBuf_EmptyChain));
	// See if the length can be fit in the first comms buf
	if(iNext->Offset() >= aLen)
		{
		iNext->AdjustDataStart(-aLen);			
		return KErrNone;
		}
	
	RCommsBuf* buf = iNext->Pool()->Pond().Alloc(aLen, 0, KMaxTInt);	 
		
	if (!buf)
		{
		return KErrNoMBufs;			
		}
	
	RCommsBufChain chain(buf);
	
	aLen -= chain.Length();	
   	// Performance enhancement - most of these prepends are 20 byes or so
   	// This allows the next one to use this MBuf
   	if (aLen<0)
   		{
  		buf->SetDataRange(-aLen, buf->Length()+aLen);
   		}
	
	Prepend(chain);
	return KErrNone;	
	}
		
EXPORT_C TInt RCommsBufChain::Alloc(TInt aSize, TCommsBufAllocator& aAllocator)
/**
Allocate a chain of RCommsBuf

@param 	aSize 		The requested total size
@param	aAllocator	A handle to the allocator

@return KErrNoMBufs if allocation is failed otherwise KErrNone
*/
	{	
	return Alloc(aSize, 0, KMaxTInt, aAllocator);	
	}
	
EXPORT_C TInt RCommsBufChain::Alloc(TInt aSize, TInt aMinBufSize, TCommsBufAllocator& aAllocator)
/**
Allocate a chain of RCommsBuf

@param 	aSize 		The requested total size
@param 	aMinBufSize The size that the allocated buffer "must" atleast have

@param	aAllocator	A handle to the allocator

@return KErrNoMBufs if allocation is failed otherwise KErrNone
*/
	{
	return Alloc(aSize, aMinBufSize, KMaxTInt, aAllocator);	
	}
	
EXPORT_C TInt RCommsBufChain::Alloc(TInt aSize, TInt aMinBufSize, TInt aMaxBufSize, TCommsBufAllocator& aAllocator)
/**
Allocate a chain of RCommsBuf

@param 	aSize 		The requested total size
@param 	aMinBufSize The size that the allocated buffer must atleast have
@param 	aMinBufSize The size that the allocated buffer can have

@param	aAllocator	A handle to the allocator

@return KErrNoMBufs if allocation is failed otherwise KErrNone
*/
	{
	 __ASSERT_ALWAYS(iNext==NULL, CommsBuf::Panic(EMBuf_NotEmptyChain));
	
	iNext = aAllocator.iPond.Alloc(aSize, aMinBufSize, aMaxBufSize);
	return iNext ? KErrNone : KErrNoMBufs;
	}
	
EXPORT_C TInt RCommsBufChain::Split(TInt aOffset, RCommsBufChain& aNewChain)
	{
  	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
    __ASSERT_ALWAYS(aOffset>=0, CommsBuf::Panic(EMBuf_NegativeOffset));

	
	if (aOffset == 0)
		{
		aNewChain.iNext = NULL;
		return KErrNone;	
		}

	TInt o, n;
	RCommsBuf* m, *p;
	
	if ((m = Goto(aOffset, o, n, p)) == NULL)
		return KErrNone;

	if (o!=m->Offset()) // Not on an mbuf boundary
		{
		// get another mbuf and copy extra data
		RCommsBuf* m2;
		m2 = m->Pool()->Pond().Alloc(n, n, KMaxTInt);
		if(!m2)
			{
			return KErrNoMBufs;
			}

		__ASSERT_DEBUG(m2->RawSize() >= n, CommsBuf::Panic(EMBuf_TooSmall));
		Mem::Copy((TUint8*)m2->RawBase() + m2->Offset(), m->RawBase()+o, n);		// trs; possible future enhancement to optionally not split the mbuf unless requested, thus avoiding the copy, but kept as is to avoid a functional break
		m2->SetDataRange(m2->Offset(), n);
		m2->SetNext(m->Next());
		m->AdjustDataEnd(-n);
		m->SetNext(NULL);
		aNewChain = m2;
		}
	else
		{
		p->SetNext(NULL);
		aNewChain = m;
		}
	return KErrNone;
	}

EXPORT_C void RCommsBufChain::Write(const TDesC8& aDes, TInt aOffset /* =0 */)
/**
Writes the supplied descriptor at the specified offset within the chain

@param aDes		Descriptor to write
@param aOffset 	Offset within the chain
*/
	{
 	 __ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
     __ASSERT_ALWAYS(aOffset>=0, CommsBuf::Panic(EMBuf_NegativeOffset));
 
	
	TUint8* ptr = (TUint8*)aDes.Ptr();
	TInt len = aDes.Length();

	TInt n;
	RCommsBuf* m;

	if (aOffset>0)
		{
		TInt o;

		if ((m = Goto(aOffset, o, n)) == NULL)
			return;

		if (n>len)
			n = len;

		Mem::Copy((TUint8*)m->RawBase()+o, ptr, n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
	else
		m = iNext;

	while (len>0 && m!=NULL)
		{
		n = len > m->Length() ? m->Length() : len;
		Mem::Copy((TUint8*)m->RawBase() + m->Offset(), ptr, n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
		
	}

EXPORT_C void RCommsBufChain::Read(TDes8& aDes, TInt aOffset /* =0 */) const
/**
Read to the supplied descriptor at the specified offset within the chain

@param aDes		Descriptor to read
@param aOffset 	Offset within the chain
*/
	{
//removed after internal discussion; generally unhelpful as empty == zero length is ok concept
 //	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(aOffset>=0, CommsBuf::Panic(EMBuf_NegativeOffset));

	TUint8* ptr = (TUint8*)aDes.Ptr();
	TInt len = aDes.Length();

	TInt n;
	RCommsBuf* m;

	if (aOffset>0)
		{
		TInt o;

		if ((m = RCommsBufChain::Goto(aOffset, o, n) ) == NULL)
			{
			aDes.SetLength(0);
			return;
			}

		if (n>len)
			n = len;

		Mem::Copy(ptr, m->RawBase()+o, n);
		ptr += n;
		len -= n;
		m = m->Next();
		}
	else
		m = static_cast<RCommsBuf*>(iNext);

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

EXPORT_C RCommsBuf* RCommsBufChain::Goto(TInt aDataOffset, TInt& aBufDataOffset, TInt& aBufDataLen, RCommsBuf* &resPrevBuf) const
/**
Goto specified byte offset into an CommsBuf chain. Used as part of Read/Write, split etc to position
CommsBuf pointer and offset from start of iBuffer.

@param anOffset The offset
@param resBuf result buffer
@param resOffset result offset
@param resLength result length
@param resPrevBuf result previous Buf in the chain
@return ETrue if successful
*/

	{
   	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
    __ASSERT_ALWAYS(aDataOffset>=0, CommsBuf::Panic(EMBuf_NegativeOffset));

	
	TInt o = 0;
	RCommsBuf* p = NULL, *m = iNext;

	while (m!=NULL)
		{
		if (o + m->Length() > aDataOffset)
			{
			aBufDataOffset = (aDataOffset - o) + m->Offset();
			aBufDataLen = m->Length() - (aDataOffset - o);
			resPrevBuf = p;
			return m;
			}
		o += m->Length();
		p = m;
		m = m->Next();
		}

	// Attempt to goto beyond end of chain
	__ASSERT_ALWAYS(o==aDataOffset, CommsBuf::Panic(EMBuf_BadOffset));
	return NULL;	
	}
	
EXPORT_C void RCommsBufChain::Free()
/**
Frees the RCommsBuf's in the chain. Upon completion the chain will become empty
*/
	{
	if (iNext)
		{
		iNext->Pool()->Pond().Free(iNext);			
		}
	iNext = NULL;
	}
	
EXPORT_C void RCommsBufChain::TrimStart(TInt aBytes)
/**
Trim the leftmost part of the data in the chain with the no. of bytes that is supplied

@param aBytes The no. of bytes to be trimmed
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(aBytes>=0, CommsBuf::Panic(EMBuf_NegativeOffset));

	if (aBytes==0)
		return;

	RCommsBuf* m, *p;
	TInt o, n;
	if ((m = Goto(aBytes, o, n, p)) == NULL)
		{
		Free();
		return;
		}

	if (p!=NULL) // m not first mbuf?
		{
		p->SetNext(NULL);
		iNext->Free();

		iNext = m;
		}

	if (o!=m->Offset()) // not at mbuf boundary?
		m->SetDataRange(o, n);		
	}
	
EXPORT_C void RCommsBufChain::TrimEnd(TInt aBytes)
/**
Trim the rightmost part of the data in the chain with the no. of bytes that is supplied

@param aBytes The no. of bytes to be trimmed
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(aBytes>=0, CommsBuf::Panic(EMBuf_NegativeOffset));

	if(aBytes==0)
		{
		Free();
		return;
		}

	RCommsBuf* m, *p;
	TInt o, n;
	if ((m = Goto(aBytes, o, n, p)) == NULL )
		return;

	if (o!=m->Offset()) // not at mbuf boundary?
		{
		m->AdjustDataEnd(-n);
		if (p = m->Next(), p!=NULL)
			{
			m->SetNext(NULL);
			p->Free();
			}
		}
	else
		{
		if (p!=NULL)
			p->SetNext(NULL);
		m->Free();
		}		
	}

EXPORT_C TInt RCommsBufChain::Align(TInt aSize)
 /**
Ensure that the first aSize bytes can be safely cast to a structure of size aSize.

@param aSize A size
@return the number of bytes actually aligned. This will be the min of aSize and chain length.
*/
	{
	__ASSERT_ALWAYS(iNext!=NULL, CommsBuf::Panic(EMBuf_EmptyChain));
	__ASSERT_ALWAYS(aSize>=0, CommsBuf::Panic(EMBuf_NegativeLength));

	// update length to the largest sized mbuf possible
	if (aSize == KMaxTInt)	// trs; does the KMBufAll concept (ie. 'largest sized' mbuf) really make sense? code kept as is to avoid a functional break
		{
		aSize = iNext->Pool()->Pond().LargestBufSize();
		}

	// All data required is already in the first MBuf
	if (aSize <= iNext->Length())
		{
		// Case 1 - allready aligned
		// Case 2 - it needs to be aligned
		if (!IS_ALIGNED(iNext->Offset()))
			{
			Mem::Copy(iNext->RawBase(), iNext->Ptr(), iNext->Length());
			iNext->SetOffset(0);
			}
		return aSize; // already as required
		}

	// Get existing data at start
	if (iNext->Offset() != 0)
		{
		Mem::Copy(iNext->RawBase(), iNext->Ptr(), iNext->Length());
		iNext->SetOffset(0);
		}

	RCommsBuf* m = iNext->Next();
	TInt len = iNext->Length();
	while (len<aSize && m!=NULL)
		{
		TInt n = aSize-len;
		if (n>m->Length())
			n = m->Length();
		Mem::Copy((TUint8*)iNext->RawBase() + (iNext->Offset() + iNext->Length()), (TUint8*)m->RawBase() + m->Offset(), n);
		iNext->AdjustDataEnd(n);
		m->AdjustDataStart(n);
		len += n;

		// MBuf might now be empty so free it
		if (m->Length()==0)
			{
			iNext->SetNext(m->Next());
			m->SetNext(NULL);
			m->Free();
			m = iNext->Next();
			}
		}
	return len;		
	}
