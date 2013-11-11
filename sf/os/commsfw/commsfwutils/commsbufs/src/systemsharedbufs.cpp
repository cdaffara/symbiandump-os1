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
#include <comms-infras/commsbufpondop.h>
#include "systemsharedbufs.h"
#include "commsbufasyncreqinternal.h"
#include "commsbufpanic.h"
#include "commsbufpond_internal.h"

CSystemSharedBufPool* CSystemSharedBufPool::New(MCommsBufPondIntf& aPondIntf, const TCommsBufPoolCreateInfo& aCreateInfo)
    {   
    CSystemSharedBufPool* self = NULL;
#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST
    // We take the freelist count as 50% of the ceiling size.
    const TInt KFreeListCount = aCreateInfo.iCeiling / 2; 
    self = new CSystemSharedBufPool(aPondIntf, aCreateInfo.iBufSize, KFreeListCount);
#elif
    self = new CSystemSharedBufPool(aPondIntf, aCreateInfo.iBufSize);
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST
    if(self)
        {
        if(self->Construct(aCreateInfo) != KErrNone)
            {
            delete self;
            self = NULL;
            }
        }
    return self;
    }

TInt CSystemSharedBufPool::Construct(const TCommsBufPoolCreateInfo& aCreateInfo)
    {           
    TShPoolCreateInfo poolCreateInfo(
            TShPoolCreateInfo::ENonPageAlignedBuffer,
            aCreateInfo.iBufSize + ALIGN_UP(sizeof(RCommsBuf)),
            aCreateInfo.iInitialBufs,
            0);
    // The ratios are represented as a 32-bit fixed-point number, where the binary point is defined 
    // to be between bits 7 and 8 (where the least-significant bit is defined as bit 0).
    // The format is also know as Q8.
    // See RShBuf documentation for more details.
    const TReal KQ8Number = 256.0; // Q8 value 2^8.
    // Normalise to the Q8 number format.
    TUint growTriggerRatio = (((TReal)aCreateInfo.iMinFreeBufs/aCreateInfo.iInitialBufs) * KQ8Number);
    TUint growByRatio = (((TReal)aCreateInfo.iGrowByBufs/aCreateInfo.iInitialBufs) * KQ8Number); 
    const TUint shrinkHysteresisRatio = 332; // Value of 1.3 normalised with 256 (Q8). 
                                             // Shrink hysteresis ration should be > 256 
    poolCreateInfo.SetSizingAttributes(aCreateInfo.iCeiling,growTriggerRatio,growByRatio,shrinkHysteresisRatio);
    
    TInt err = iPool.Create(poolCreateInfo, EShPoolAllocate | EShPoolWriteable);
#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST    
    if(err == KErrNone)
        {
        err = iFreeListLock.CreateLocal();
        }
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST    
    return err;       
    }


CSystemSharedBufPool::~CSystemSharedBufPool()
	{
#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST    
	iFreeListCount = iMaxFreeListCount;
	iFreeList.Free();
	iFreeListLock.Close();
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST	
	iPool.Close();
	}

#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST   

TInt CSystemSharedBufPool::AllocFromFreeList(RCommsBufQ& aBufQ, TInt aSize)
    {
    // Allocate from the free list and returns the total size that is allocated.
    // Note that the total size that is allocated may not be equal to the size
    // and depends on the pool size, sometimes would be little greater than than 
    // the requested size.  
    TInt transfered = 0;
    TInt bufCount = 0;
    iFreeListLock.Wait();
    transfered = iFreeList.Transfer(aBufQ, aSize, BufSize(), bufCount);
    iFreeListCount -= bufCount;
    iFreeListLock.Signal();
    return transfered;
    }

TBool CSystemSharedBufPool::ReleaseToFreeList(RCommsBuf* aBuf)
    {
    TBool released = EFalse;
     aBuf->SetDataRange(0, aBuf->RawSize()); // Reset the offset to 0 and length to the size.
     iFreeListLock.Wait();
     if(iFreeListCount < iMaxFreeListCount)
         {
          ++iFreeListCount;
          iFreeList.Append(aBuf);    
          released = ETrue;
          }
      iFreeListLock.Signal();
    return released;
    }

TInt CSystemSharedBufPool::FreeListCount()
    {
    iFreeListLock.Wait();
    TInt freeCount = iFreeListCount;
    iFreeListLock.Signal();
    return freeCount;
    }
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST 

TInt CSystemSharedBufPool::AllocOverflow(RCommsBufQ& aBufQ, TInt aSize)
    {
    TInt allocated = 0;

#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST
    allocated = AllocFromFreeList(aBufQ, aSize);
    // See comment in AllocFromFreeList. RCommsBufQ::Transfer adjust the 
    // end of the commsbuf. We have to do this in our algorithm when we allocate from
    // the system shared pool. To avoid conflicts mark the size as 0. 
    aSize = (allocated > aSize) ? 0 : (aSize - allocated);
#endif

    while(aSize > 0)  
        {
        RCommsBuf* buf = Alloc();
        if(buf)
            {
            aBufQ.Append(buf);
            aSize -= buf->RawSize();    
            }
        else
            {
            break;  
            }                                   
        }
    return aSize;    // Return the pending allocation size.
    }

TInt CSystemSharedBufPool::AllocUnderflow(RCommsBufQ& aBufQ, TInt aSize)
	{	
	TInt allocated = 0;    
#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST
	TInt toAllocate = aSize < BufSize() ? aSize : (aSize - (aSize % BufSize()));   
	allocated = AllocFromFreeList(aBufQ, toAllocate);
    // See comment in AllocFromFreeList. RCommsBufQ::Transfer adjust the 
    // end of the commsbuf. We have to do this in our algorithm when we allocate from
    // the system shared pool. To avoid conflicts mark the size as 0. 
	aSize = (allocated > aSize) ? 0 : (aSize - allocated);
#endif
	// Either there was no buffers available in the freelist or we allocated partially from
	// the freelist. See we have still to allocate. 
	// If we are doing a zero size alloc and freelist doesn't contain
	// any buffers we have to check whether we did actually allocate or not from the freelist.
	if((allocated == 0) || (aSize >= BufSize()))
	    {
	    do  
	        {
	        RCommsBuf* buf = Alloc();
	        if(buf)
	            {
	            aBufQ.Prepend(buf);
	            aSize -= buf->RawSize();    
	            }
	        else
	            {
	            break;  
	            }	        	                    
	        }while((aSize - BufSize()) >= 0);	    
	    }
	return aSize;
	}
	
RCommsBuf* CSystemSharedBufPool::Alloc()
	{
	RShBuf buf;
	if(buf.Alloc(iPool) != KErrNone)
	    {
		return NULL;
		}
	TUint8* metaStart = buf.Ptr() + BufSize();
	return new(metaStart) RCommsBuf(-BufSize(), BufSize(), buf.Handle(), Id());
	}

void CSystemSharedBufPool::Free(RCommsBuf* aBuf)
	{
	TBool released = EFalse;
#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST
	released = ReleaseToFreeList(aBuf);
#endif
	if(!released)
	    {
	    RShBuf buf;
	    buf.SetHandle(aBuf->Handle());
	    buf.Close();	    
	    }
	}

TInt CSystemSharedBufPool::Compare(const CSystemSharedBufPool& aLhs, const CSystemSharedBufPool& aRhs)
	{
	if(aLhs.BufSize() == aRhs.BufSize())		
		{
		return 0;			
		}
	else if(aLhs.BufSize() > aRhs.BufSize())
		{
		return -1;
		}
	else
		{
		return 1;	
		}
	}
	

// -----------------------------------------------------------------------------------------------

T3StageAllocator::T3StageAllocator(RPointerArray<CSystemSharedBufPool>& aPools, TInt aSize, TInt aMinSize, TInt aMaxSize)
: iPools(aPools),
iSize(aSize),
iBiggestPoolIndex(KErrNotFound),
iSmallestPoolIndex(KErrNotFound),
iMarkedPoolIndex(KErrNotFound),
iZeroBufSize(iSize == 0)
	{
	Init(aMinSize, aMaxSize);
	}

RCommsBuf* T3StageAllocator::Do()
	{
	// If the criteria didn't match we have nothing to allocate	
	if(iBiggestPoolIndex == KErrNotFound || iSmallestPoolIndex == KErrNotFound)
		{
		return NULL;			
		}
		
	// Note that our pools are ordered from big to small.
	if(iSize == 0)
		{
		// Note. This is a special case. If the size requested is 0 then we allocate smallest 
		// buffer as possible provided the availability of buffers on the pools. 
		ZeroSizedAlloc();	
		}
	else
		{
		ForwardAlloc1stStage();	
		BackwardAlloc2ndStage();
        // We have to do the 3rd stage allocation only if we set the iIntermediatePoolIndex. 
		// The setting is happening at the end of 1st stage, if further allocation is required 
		// in the 2nd and probably in the 3rd stage
		if(iMarkedPoolIndex != KErrNotFound)
		   {		   
		   // We already traversed till the iIntermediatePoolIndex so we increment the 
		   // index by 1
		   ++iMarkedPoolIndex;
	       ForwardAlloc3rdStage();
		   }
		}
	// Check allocation is partially failed 
	if (iSize > 0 && (!iBufQ.IsEmpty()))
		{
		iBufQ.Free();
		}

	// Adjust the end of the commsbuf if we hit with -ve size. Happens when the request buffer size
	// is less than the actual pool buffer size
	if(iSize < 0)
		{
		iBufQ.Last()->AdjustDataEnd(iSize);
		}			
	return iBufQ.First();
	}

void T3StageAllocator::Init(TInt aMinSize, TInt aMaxSize)
	{			
	TInt poolCount = iPools.Count();
	TInt index = 0;

	while(index < poolCount)
		{
		CSystemSharedBufPool* pool = iPools[index];		
		if((pool->BufSize() >= aMinSize) && (pool->BufSize() <= aMaxSize))
			{
			if(iBiggestPoolIndex == KErrNotFound)
				{
				iBiggestPoolIndex = index;	
				}
			
			iSmallestPoolIndex = index;						
			}		
		++index;
		}
	iMarkedPoolIndex = iSmallestPoolIndex;
	}

void T3StageAllocator::ForwardAlloc1stStage()
	{	
	// We are going to do a forward traversal on the array.
	TInt traversalIndex = iBiggestPoolIndex;
	while(iSize > 0 && traversalIndex <= iSmallestPoolIndex)
		{
		CSystemSharedBufPool* currentPool = iPools[traversalIndex];
        TInt nextPoolSize = 0;
		if(traversalIndex + 1 <= iSmallestPoolIndex)
			{
            nextPoolSize = iPools[traversalIndex + 1]->BufSize();		
			}
			
        // If we still need to allocate then check whether the next pool buffers size 
        // smaller than that we needed.
        if(nextPoolSize < iSize)
            {
            // Yes? Force an allocation with the current pool.
            TInt remains = currentPool->AllocUnderflow(iBufQ, iSize);
            if(remains == iSize)
                {
                iMarkedPoolIndex = traversalIndex;                
                break;
                }
            iSize = remains;
            }
        else
            {
            ++traversalIndex;
            }
		}			
	}

void T3StageAllocator::BackwardAlloc2ndStage()
	{
	// We are going to do a backward traversal on the array.	
	TInt traversalIndex = iMarkedPoolIndex - 1;
	
	// Reverse allocation allocates from any pool that has the buffers
	while((iSize > 0) && iBiggestPoolIndex <= traversalIndex)
		{
		iSize = iPools[traversalIndex]->AllocUnderflow(iBufQ, iSize);
		--traversalIndex;
		}		
	}

void T3StageAllocator::ForwardAlloc3rdStage()
	{
    // We are going to do a forward traversal on the array
	// from the marked pool index position.
    TInt traversalIndex = iMarkedPoolIndex;

    while(iSize > 0 && traversalIndex <= iSmallestPoolIndex)
        {        
        iSize = iPools[traversalIndex]->AllocOverflow(iBufQ, iSize);
        ++traversalIndex;
        }   
    }

void T3StageAllocator::ZeroSizedAlloc()
    {    
    // We are going to do a backward traversal on the array.    
    TInt traversalIndex = iMarkedPoolIndex;    
    // We need only one buffer.
    while(iBufQ.IsEmpty() && iBiggestPoolIndex <= traversalIndex)
        {
        iSize = iPools[traversalIndex]->AllocUnderflow(iBufQ, iSize);
        --traversalIndex;
        }           
    }

// -----------------------------------------------------------------------------------------------

MCommsBufPondIntf* CSystemSharedBufPond::New(RArray <TCommsBufPoolCreateInfo>& aPoolInfo)
	{
	CSystemSharedBufPond* self = new CSystemSharedBufPond;
	if(self)
		{
		if(self->Construct(aPoolInfo) != KErrNone)
			{
			delete self;
			self = NULL;
			}
		}
	return self;
	}

TInt CSystemSharedBufPond::Construct(RArray <TCommsBufPoolCreateInfo>& aPoolInfo)
	{
	for (TInt i = 0; i < aPoolInfo.Count(); ++i)
	    {
	    CSystemSharedBufPool* commsPoolInfo = CSystemSharedBufPool::New(*this, aPoolInfo[i]);
        TInt err = (commsPoolInfo == NULL) ? KErrNoMemory : iPools.Append(commsPoolInfo);
        if(err != KErrNone)
            {
            return err;
            }
	    }
	// Sort the pool from bigger to small order
	TLinearOrder<CSystemSharedBufPool> order(CSystemSharedBufPool::Compare);
	iPools.Sort(order);
	
	iAsyncAlloc = CSystemSharedAsyncAlloc::New(iPools);
	if(!iAsyncAlloc)
		{
		return KErrNoMemory;			
		}		
	return KErrNone;
	}

CSystemSharedBufPond::~CSystemSharedBufPond()
	{
	iPools.ResetAndDestroy();
	iPools.Close();
	delete iAsyncAlloc;
	}

// From MCommsBufManagerIntf
RCommsBuf* CSystemSharedBufPond::FromHandle(TInt aHandle)
	{
	RShBuf buf;
	buf.SetHandle(aHandle);
	TInt bufSize = buf.Size() - sizeof(RCommsBuf);
	TUint8* metaStart = buf.Ptr() + bufSize;	
	return new(metaStart)RCommsBuf();
	}

RCommsBuf* CSystemSharedBufPond::Alloc(TInt aSize, TInt aMinBufSize, TInt aMaxBufSize)
	{	
	// check args
	// - regarding use of TInt instead of TUint, refer comments in CMBufPoolManager::AddL
	__ASSERT_ALWAYS(aSize >= 0, CommsBuf::Panic(EMBuf_SillyAlloc));
	__ASSERT_DEBUG(aMinBufSize >= 0, CommsBuf::Panic(EMBuf_NegativeMinMBufSize));
	__ASSERT_DEBUG(aMaxBufSize >= 0, CommsBuf::Panic(EMBuf_NegativeMaxMBufSize));
	__ASSERT_DEBUG(aMaxBufSize >= aMinBufSize, CommsBuf::Panic(EMBuf_MinExceedsMaxMBufSize));
	
	
	T3StageAllocator allocator(iPools, aSize, aMinBufSize, aMaxBufSize);
	return allocator.Do();
	}	
			
TInt CSystemSharedBufPond::BytesAvailable() const
	{
	TInt totalBytesAvbl = 0;
	TInt poolsCount = iPools.Count();
	for (TInt i = 0; i < poolsCount; ++i)
		{
		totalBytesAvbl += (iPools[i]->Pool().FreeCount() * iPools[i]->BufSize());
#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST
        totalBytesAvbl += (iPools[i]->FreeListCount() * iPools[i]->BufSize());
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST     
		}
	return totalBytesAvbl;
	}

TInt CSystemSharedBufPond::BytesAvailable(TInt aSize) const
	{
	TInt totalBytesAvbl = 0;
	TInt poolsCount = iPools.Count();
	for (TInt i = 0; i < poolsCount; ++i)
		{
		if(iPools[i]->BufSize() == aSize)
			{
			totalBytesAvbl = (iPools[i]->Pool().FreeCount() * iPools[i]->BufSize());
#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST
            totalBytesAvbl += (iPools[i]->FreeListCount() * iPools[i]->BufSize());
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST		
			break;				
			}
		}
	return totalBytesAvbl;	
	}

TInt CSystemSharedBufPond::NextBufSize(TInt aSize) const
	{
	TInt poolsCount = iPools.Count() - 1;	
	for (TInt i = poolsCount; i >= 0; --i)
		{		
		if (iPools[i]->BufSize() > aSize)
			{
			return iPools[i]->BufSize();
			}
		}
	return KErrNotFound;	
	}

TInt CSystemSharedBufPond::LargestBufSize() const
	{
	return iPools[0]->BufSize();
	}

void CSystemSharedBufPond::StartRequest(CCommsBufAsyncRequest& aRequest)
	{
	iAsyncAlloc->StartRequest(aRequest);		
	}

void CSystemSharedBufPond::CancelRequest(CCommsBufAsyncRequest& aRequest)
	{
	iAsyncAlloc->CancelRequest(aRequest);
	}

void CSystemSharedBufPond::Free(RCommsBuf* aBuf)
	{
	while(aBuf != NULL)
		{
		RCommsBuf* nextBuf = aBuf->Next();		
		aBuf->SetNext(NULL);
		CSystemSharedBufPool* pool = static_cast<CSystemSharedBufPool*>(aBuf->Pool());
		pool->Free(aBuf);		
		aBuf = nextBuf;	
		}
	}

void CSystemSharedBufPond::SetContext()
	{
	}

void CSystemSharedBufPond::Release(RLibrary& /*aLib*/)
	{
	delete this;
	}

MCommsBufPondDbg& CSystemSharedBufPond::CommsBufPondDbg()
	{		
	return *this;
	}

RCommsBuf* CSystemSharedBufPond::__DbgBufChain()
    {
    return NULL;
    }

RCommsBuf* CSystemSharedBufPond::__DbgBufChain(TUint /* aBufSize */)
    {
    return NULL;
    }
    
void CSystemSharedBufPond::__DbgSetPoolLimit(TInt /* aCount */)
    {
    
    }

void CSystemSharedBufPond::__DbgSetPoolLimit(TInt /* aCount */, TUint /* aBufSize */)
    {
    
    }

void CSystemSharedBufPond::__DbgSetFailAfter(TInt /* aCount */)
    {
    
    }

TUint CSystemSharedBufPond::__DbgGetBufSpace()
    {
    return 0;
    }

TUint CSystemSharedBufPond::__DbgGetBufSpace(TUint /* aBufSize */)
    {
    return 0;
    }

TUint CSystemSharedBufPond::__DbgGetBufTotal()
    {
    return 0;
    }

TUint CSystemSharedBufPond::__DbgGetBufTotal(TUint /* aMufSize */)
    {
    return 0;
    }

TInt CSystemSharedBufPond::__DbgGetHeapSize()
    {
    return 0;
    }


/**
@purpose Writes flattened pond structure to a descriptor for transfer to a commsbufs aware driver
@param aStore Descriptor in to which the pond structure is to be flattened
*/
TInt CSystemSharedBufPond::Store(TDes8& aStore) const
	{
	// todo_cdg needs to be fixed as assumes alignment of TDes8 which need not be word aligned at all
	// Need enough space to store the max number of pools
	if(aStore.Length() < sizeof(TCommsPond))
		{
#ifdef _DEBUG
		CommsBuf::Panic(EMBuf_InsufficientSpaceToStorePond);
#endif
		return KErrArgument;
		}

	// Map basic pond structure on top of flat buffer we are writing to
	TCommsPond* pond = const_cast<TCommsPond*>(reinterpret_cast<const TCommsPond*>(aStore.Ptr()));
	
	TInt numPools = iPools.Count();
	for(TInt i = 0; i < numPools; i++)
		{
		CSystemSharedBufPool* pool = iPools[i];
		RShPool shPool = pool->Pool();
		pond->iPoolRecords[i] = TPoolRecord(shPool.Handle(), (TInt)pool, pool->BufSize());
		}
	pond->iNumPools = numPools;
	return KErrNone;
	}
 
