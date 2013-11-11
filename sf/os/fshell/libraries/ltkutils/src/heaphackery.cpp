// heaphackery.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
// Contributors:
// Adrian Issott (Nokia) - Updates for kernel-side alloc helper
//

#ifdef TEST_HYBRIDHEAP_ASSERTS
#define private public
#include <e32def.h>
#include "slab.h"
#include "page_alloc.h"
#include "heap_hybrid.h"
#endif

#include "heaputils.h"

#ifdef __KERNEL_MODE__

#include <kern_priv.h>
#define MEM Kern
__ASSERT_COMPILE(sizeof(LtkUtils::RUserAllocatorHelper) == 10*4);
#define KERN_ENTER_CS() NKern::ThreadEnterCS()
#define KERN_LEAVE_CS() NKern::ThreadLeaveCS()
#define LOG(args...)
#define HUEXPORT_C
#else

#include <e32std.h>
#define MEM User
#define KERN_ENTER_CS()
#define KERN_LEAVE_CS()
//#include <e32debug.h>
//#define LOG(args...) RDebug::Printf(args)
#define LOG(args...)

#ifdef STANDALONE_ALLOCHELPER
#define HUEXPORT_C
#else
#define HUEXPORT_C EXPORT_C
#endif

#endif // __KERNEL_MODE__

using LtkUtils::RAllocatorHelper;
const TUint KPageSize = 4096;
__ASSERT_COMPILE(sizeof(RAllocatorHelper) == 9*4);

// RAllocatorHelper

HUEXPORT_C RAllocatorHelper::RAllocatorHelper()
	: iAllocatorAddress(0), iAllocatorType(EUnknown), iInfo(NULL), iValidInfo(0), iTempSlabBitmap(NULL), iPageCache(NULL), iPageCacheAddr(0)
#ifdef __KERNEL_MODE__
	, iChunk(NULL)
#endif
	{
	}

namespace LtkUtils
	{
	class THeapInfo
		{
	public:
		THeapInfo()
			{
			ClearStats();
			}

		void ClearStats()
			{
			memclr(this, sizeof(THeapInfo));
			}

		TInt iAllocatedSize; // number of bytes in allocated cells (excludes free cells, cell header overhead)
		TInt iCommittedSize; // amount of memory actually committed (includes cell header overhead, gaps smaller than an MMU page)
		TInt iAllocationCount; // number of allocations currently
		TInt iMaxCommittedSize; // or thereabouts
		TInt iMinCommittedSize;
		TInt iUnusedPages;
		TInt iCommittedFreeSpace;
		// Heap-only stats
		TInt iHeapFreeCellCount;
		// Hybrid-only stats
		TInt iDlaAllocsSize;
		TInt iDlaAllocsCount;
		TInt iDlaFreeSize;
		TInt iDlaFreeCount;
		TInt iSlabAllocsSize;
		TInt iSlabAllocsCount;
		TInt iPageAllocsSize;
		TInt iPageAllocsCount;
		TInt iSlabFreeCellSize;
		TInt iSlabFreeCellCount;
		TInt iSlabFreeSlabSize;
		TInt iSlabFreeSlabCount;
		};
	}

const TInt KTempBitmapSize = 256; // KMaxSlabPayload / mincellsize, technically. Close enough.

#ifdef __KERNEL_MODE__

TLinAddr LtkUtils::RAllocatorHelper::GetKernelAllocator(DChunk* aKernelChunk) 
	{
	TLinAddr allocatorAddress;
#ifdef __WINS__
	allocatorAddress = (TLinAddr)aKernelChunk->Base();
#else
	// Copied from P::KernelInfo
	const TRomHeader& romHdr=Epoc::RomHeader();
	const TRomEntry* primaryEntry=(const TRomEntry*)Kern::SuperPage().iPrimaryEntry;
	const TRomImageHeader* primaryImageHeader=(const TRomImageHeader*)primaryEntry->iAddressLin;
	TLinAddr stack = romHdr.iKernDataAddress + Kern::RoundToPageSize(romHdr.iTotalSvDataSize);
	TLinAddr heap = stack + Kern::RoundToPageSize(primaryImageHeader->iStackSize);
	allocatorAddress = heap;
#endif
	return allocatorAddress;
	}

TInt RAllocatorHelper::OpenKernelHeap()
	{
	_LIT(KName, "SvHeap");
	NKern::ThreadEnterCS();
	DObjectCon* chunkContainer = Kern::Containers()[EChunk];
	chunkContainer->Wait();
	const TInt chunkCount = chunkContainer->Count();
	DChunk* foundChunk = NULL;
	for(TInt i=0; i<chunkCount; i++)
		{
		DChunk* chunk = (DChunk*)(*chunkContainer)[i];
		if (chunk->NameBuf() && chunk->NameBuf()->Find(KName) != KErrNotFound)
			{
			// Found it. No need to open it, we can be fairly confident the kernel heap isn't going to disappear from under us
			foundChunk = chunk;
			break;
			}
		}
	iChunk = foundChunk;
	chunkContainer->Signal();

	iAllocatorAddress = GetKernelAllocator(foundChunk);

	// It looks like DChunk::iBase/DChunk::iFixedBase should both be ok for the kernel chunk
	// aChunkMaxSize is only used for trying the middle of the chunk for hybrid allocatorness, and the kernel heap doesn't use that (thankfully). So we can safely pass in zero.
	TInt err = OpenChunkHeap((TLinAddr)foundChunk->Base(), 0); 

	if (!err) err = FinishConstruction();
	NKern::ThreadLeaveCS();
	return err;
	}

#else

HUEXPORT_C TInt RAllocatorHelper::Open(RAllocator* aAllocator)
	{
	iAllocatorAddress = (TLinAddr)aAllocator;
	TInt udeb = EuserIsUdeb();
	if (udeb < 0) return udeb; // error

	TInt err = IdentifyAllocatorType(udeb);
	if (!err)
		{
		err = FinishConstruction(); // Allocate everything up front
		}
	if (!err)
		{
		// We always stealth our own allocations, again to avoid tripping up allocator checks
		SetCellNestingLevel(iInfo, -1);
		SetCellNestingLevel(iTempSlabBitmap, -1);
		SetCellNestingLevel(iPageCache, -1);
		}
	return err;
	}

#endif

TInt RAllocatorHelper::FinishConstruction()
	{
	TInt err = KErrNone;
	KERN_ENTER_CS();
	if (!iInfo)
		{
		iInfo = new THeapInfo;
		if (!iInfo) err = KErrNoMemory;
		}
	if (!err && !iTempSlabBitmap)
		{
		iTempSlabBitmap = (TUint8*)MEM::Alloc(KTempBitmapSize);
		if (!iTempSlabBitmap) err = KErrNoMemory;
		}
	if (!err && !iPageCache)
		{
		iPageCache = MEM::Alloc(KPageSize);
		if (!iPageCache) err = KErrNoMemory;
		}

	if (err)
		{
		delete iInfo;
		iInfo = NULL;
		MEM::Free(iTempSlabBitmap);
		iTempSlabBitmap = NULL;
		MEM::Free(iPageCache);
		iPageCache = NULL;
		}
	KERN_LEAVE_CS();
	return err;
	}

TInt RAllocatorHelper::ReadWord(TLinAddr aLocation, TUint32& aResult) const
	{
	// Check if we can satisfy the read from the cache
	if (aLocation >= iPageCacheAddr)
		{
		TUint offset = aLocation - iPageCacheAddr;
		if (offset < KPageSize)
			{
			aResult = ((TUint32*)iPageCache)[offset >> 2];
			return KErrNone;
			}
		}

	// If we reach here, not in page cache. Try and read in the new page
	if (iPageCache)
		{
		TLinAddr pageAddr = aLocation & ~(KPageSize-1);
		TInt err = ReadData(pageAddr, iPageCache, KPageSize);
		if (!err)
			{
			iPageCacheAddr = pageAddr;
			aResult = ((TUint32*)iPageCache)[(aLocation - iPageCacheAddr) >> 2];
			return KErrNone;
			}
		}

	// All else fails, try just reading it uncached
	return ReadData(aLocation, &aResult, sizeof(TUint32));
	}

TInt RAllocatorHelper::ReadByte(TLinAddr aLocation, TUint8& aResult) const
	{
	// Like ReadWord but 8-bit

	// Check if we can satisfy the read from the cache
	if (aLocation >= iPageCacheAddr)
		{
		TUint offset = aLocation - iPageCacheAddr;
		if (offset < KPageSize)
			{
			aResult = ((TUint8*)iPageCache)[offset];
			return KErrNone;
			}
		}

	// If we reach here, not in page cache. Try and read in the new page
	if (iPageCache)
		{
		TLinAddr pageAddr = aLocation & ~(KPageSize-1);
		TInt err = ReadData(pageAddr, iPageCache, KPageSize);
		if (!err)
			{
			iPageCacheAddr = pageAddr;
			aResult = ((TUint8*)iPageCache)[(aLocation - iPageCacheAddr)];
			return KErrNone;
			}
		}

	// All else fails, try just reading it uncached
	return ReadData(aLocation, &aResult, sizeof(TUint8));
	}


TInt RAllocatorHelper::WriteWord(TLinAddr aLocation, TUint32 aWord)
	{
	// Invalidate the page cache if necessary
	if (aLocation >= iPageCacheAddr && aLocation - iPageCacheAddr < KPageSize)
		{
		iPageCacheAddr = 0;
		}

	return WriteData(aLocation, &aWord, sizeof(TUint32));
	}

TInt RAllocatorHelper::ReadData(TLinAddr aLocation, TAny* aResult, TInt aSize) const
	{
	// RAllocatorHelper base class impl is for allocators in same address space, so just copy it
	memcpy(aResult, (const TAny*)aLocation, aSize);
	return KErrNone;
	}

TInt RAllocatorHelper::WriteData(TLinAddr aLocation, const TAny* aData, TInt aSize)
	{
	memcpy((TAny*)aLocation, aData, aSize);
	return KErrNone;
	}

#ifdef __KERNEL_MODE__

LtkUtils::RUserAllocatorHelper::RUserAllocatorHelper()
	: iThread(NULL)
	{}

void LtkUtils::RUserAllocatorHelper::Close()
	{
	NKern::ThreadEnterCS();
	if (iThread)
		{
		iThread->Close(NULL);
		}
	iThread = NULL;
	RAllocatorHelper::Close();
	NKern::ThreadLeaveCS();
	}

TInt LtkUtils::RUserAllocatorHelper::ReadData(TLinAddr aLocation, TAny* aResult, TInt aSize) const
	{
	return Kern::ThreadRawRead(iThread, (const TAny*)aLocation, aResult, aSize);
	}

TInt LtkUtils::RUserAllocatorHelper::WriteData(TLinAddr aLocation, const TAny* aData, TInt aSize)
	{
	return Kern::ThreadRawWrite(iThread, (TAny*)aLocation, aData, aSize);
	}

TInt LtkUtils::RUserAllocatorHelper::TryLock()
	{
	return KErrNotSupported;
	}

void LtkUtils::RUserAllocatorHelper::TryUnlock()
	{
	// Not supported
	}

TInt LtkUtils::RUserAllocatorHelper::OpenUserHeap(TUint aThreadId, TLinAddr aAllocatorAddress, TBool aEuserIsUdeb)
	{
	NKern::ThreadEnterCS();
	DObjectCon* threads = Kern::Containers()[EThread];
	threads->Wait();
	iThread = Kern::ThreadFromId(aThreadId);
	if (iThread && iThread->Open() != KErrNone)
		{
		// Failed to open
		iThread = NULL;
		}
	threads->Signal();
	NKern::ThreadLeaveCS();
	if (!iThread) return KErrNotFound;
	iAllocatorAddress = aAllocatorAddress;
	TInt err = IdentifyAllocatorType(aEuserIsUdeb);
	if (err) Close();
	return err;
	}

LtkUtils::RKernelCopyAllocatorHelper::RKernelCopyAllocatorHelper()
	: iCopiedChunk(NULL), iOffset(0)
	{}

TInt LtkUtils::RKernelCopyAllocatorHelper::OpenCopiedHeap(DChunk* aOriginalChunk, DChunk* aCopiedChunk, TInt aOffset)
	{
	TInt err = aCopiedChunk->Open();
	if (!err)
		{
		iCopiedChunk = aCopiedChunk;
		iOffset = aOffset;

		// We need to set iAllocatorAddress to point to the allocator in the original chunk and not the copy
		// because all the internal pointers will be relative to that. Instead we use iOffset in the Read / Write Data 
		// calls
		iAllocatorAddress = GetKernelAllocator(aOriginalChunk);
		
		// It looks like DChunk::iBase/DChunk::iFixedBase should both be ok for the kernel chunk
		// aChunkMaxSize is only used for trying the middle of the chunk for hybrid allocatorness, and the kernel heap doesn't use that (thankfully). So we can safely pass in zero.
		err = OpenChunkHeap((TLinAddr)aCopiedChunk->Base(), 0);
		}
	
	return err;
	}

DChunk* LtkUtils::RKernelCopyAllocatorHelper::OpenUnderlyingChunk()
	{
	// We should never get here
	__NK_ASSERT_ALWAYS(EFalse);
	return NULL;
	}

void LtkUtils::RKernelCopyAllocatorHelper::Close()
	{
	if (iCopiedChunk)
		{
		NKern::ThreadEnterCS();
		iCopiedChunk->Close(NULL);
		iCopiedChunk = NULL;
		NKern::ThreadLeaveCS();
		}
	iOffset = 0;
	RAllocatorHelper::Close();
	}

TInt LtkUtils::RKernelCopyAllocatorHelper::ReadData(TLinAddr aLocation, TAny* aResult, TInt aSize) const
	{
	memcpy(aResult, (const TAny*)(aLocation+iOffset), aSize);
	return KErrNone;
	}

TInt LtkUtils::RKernelCopyAllocatorHelper::WriteData(TLinAddr aLocation, const TAny* aData, TInt aSize)
	{
	memcpy((TAny*)(aLocation+iOffset), aData, aSize);
	return KErrNone;
	}

TInt LtkUtils::RKernelCopyAllocatorHelper::TryLock()
	{
	return KErrNotSupported;
	}

void LtkUtils::RKernelCopyAllocatorHelper::TryUnlock()
	{
	// Not supported
	}

#endif // __KERNEL_MODE__

TInt RAllocatorHelper::OpenChunkHeap(TLinAddr aChunkBase, TInt aChunkMaxSize)
	{
#ifdef __KERNEL_MODE__
	// Must be in CS
	// Assumes that this only ever gets called for the kernel heap. Otherwise goes through RUserAllocatorHelper::OpenUserHeap.
	TInt udeb = EFalse; // We can't figure this out until after we've got the heap
	TBool isTheKernelHeap = ETrue;
#else
	// Assumes the chunk isn't the kernel heap. It's not a good idea to try messing with the kernel heap from user side...
	TInt udeb = EuserIsUdeb();
	if (udeb < 0) return udeb; // error
	TBool isTheKernelHeap = EFalse;
#endif

	if (iAllocatorAddress == 0)
		{
		// Subclasses with more knowledge about the layout of the allocator within the chunk may have already set the iAllocatorAddress (eg kernel heap's allocator doesn't start at the chunk base)
		iAllocatorAddress = aChunkBase;
		}

	TInt err = IdentifyAllocatorType(udeb, isTheKernelHeap);
	if (err == KErrNone && iAllocatorType == EAllocator)
		{
		// We've no reason to assume it's an allocator because we don't know the iAllocatorAddress actually is an RAllocator*
		err = KErrNotFound;
		}
	if (err && aChunkMaxSize > 0 && iAllocatorAddress == aChunkBase)
		{
		TInt oldErr = err;
		TAllocatorType oldType = iAllocatorType;
		// Try middle of chunk, in case it's an RHybridHeap
		iAllocatorAddress += aChunkMaxSize / 2;
		err = IdentifyAllocatorType(udeb, isTheKernelHeap);
		if (err || iAllocatorType == EAllocator)
			{
			// No better than before
			iAllocatorAddress = aChunkBase;
			iAllocatorType = oldType;
			err = oldErr;
			}
		}
#ifdef __KERNEL_MODE__
	if (err == KErrNone)
		{
		// Now we know the allocator, we can figure out the udeb-ness
		RAllocator* kernelAllocator = reinterpret_cast<RAllocator*>(iAllocatorAddress);
		kernelAllocator->DebugFunction(RAllocator::ESetFail, (TAny*)9999, (TAny*)0); // Use an invalid fail reason - this should have no effect on the operation of the heap
		TInt err = kernelAllocator->DebugFunction(7, NULL, NULL); // 7 is RAllocator::TAllocDebugOp::EGetFail
		if (err == 9999)
			{
			// udeb new hybrid heap
			udeb = ETrue;
			}
		else if (err == KErrNotSupported)
			{
			// Old heap - fall back to slightly nasty non-thread-safe method
			kernelAllocator->DebugFunction(RAllocator::ESetFail, (TAny*)RAllocator::EFailNext, (TAny*)1);
			TAny* res = Kern::Alloc(4);
			if (!res) udeb = ETrue;
			Kern::Free(res);
			}
		else
			{
			// it's new urel
			}

		// Put everything back
		kernelAllocator->DebugFunction(RAllocator::ESetFail, (TAny*)RAllocator::ENone, (TAny*)0);
		// And update the type now we know the udeb-ness for certain
		err = IdentifyAllocatorType(udeb, isTheKernelHeap);
		}
#endif
	return err;
	}


// The guts of RAllocatorHelper

enum TWhatToGet
	{
	ECommitted = 1,
	EAllocated = 2,
	ECount = 4,
	EMaxSize = 8,
	EUnusedPages = 16,
	ECommittedFreeSpace = 32,
	EMinSize = 64,
	EHybridStats = 128,
	};

class RHackAllocator : public RAllocator
	{
public:
	using RAllocator::iHandles;
	using RAllocator::iTotalAllocSize;
	using RAllocator::iCellCount;
	};

class RHackHeap : public RHeap
	{
public:
	// Careful, only allowed to use things that are still in the new RHeap, and are still in the same place
	using RHeap::iMaxLength;
	using RHeap::iChunkHandle;
	using RHeap::iLock;
	using RHeap::iBase;
	using RHeap::iAlign;
	using RHeap::iTop;
	};

const TInt KChunkSizeOffset = 30*4;
const TInt KPageMapOffset = 141*4;
//const TInt KDlOnlyOffset = 33*4;
const TInt KMallocStateOffset = 34*4;
const TInt KMallocStateTopSizeOffset = 3*4;
const TInt KMallocStateTopOffset = 5*4;
const TInt KMallocStateSegOffset = 105*4;
const TInt KUserHybridHeapSize = 186*4;
const TInt KSparePageOffset = 167*4;
const TInt KPartialPageOffset = 165*4;
const TInt KFullSlabOffset = 166*4;
const TInt KSlabAllocOffset = 172*4;
const TInt KSlabParentOffset = 1*4;
const TInt KSlabChild1Offset = 2*4;
const TInt KSlabChild2Offset = 3*4;
const TInt KSlabPayloadOffset = 4*4;
const TInt KSlabsetSize = 4;

#ifdef TEST_HYBRIDHEAP_ASSERTS
__ASSERT_COMPILE(_FOFF(RHybridHeap, iChunkSize) == KChunkSizeOffset);
__ASSERT_COMPILE(_FOFF(RHybridHeap, iPageMap) == KPageMapOffset);
__ASSERT_COMPILE(_FOFF(RHybridHeap, iGlobalMallocState) == KMallocStateOffset);
__ASSERT_COMPILE(sizeof(malloc_state) == 107*4);
__ASSERT_COMPILE(_FOFF(malloc_state, iTopSize) == KMallocStateTopSizeOffset);
__ASSERT_COMPILE(_FOFF(malloc_state, iTop) == KMallocStateTopOffset);
__ASSERT_COMPILE(_FOFF(malloc_state, iSeg) == KMallocStateSegOffset);
__ASSERT_COMPILE(sizeof(RHybridHeap) == KUserHybridHeapSize);
__ASSERT_COMPILE(_FOFF(RHybridHeap, iSparePage) == KSparePageOffset);
__ASSERT_COMPILE(_FOFF(RHybridHeap, iPartialPage) == KPartialPageOffset);
__ASSERT_COMPILE(_FOFF(RHybridHeap, iSlabAlloc) == KSlabAllocOffset);
__ASSERT_COMPILE(_FOFF(slab, iParent) == KSlabParentOffset);
__ASSERT_COMPILE(_FOFF(slab, iChild1) == KSlabChild1Offset);
__ASSERT_COMPILE(_FOFF(slab, iChild2) == KSlabChild2Offset);
__ASSERT_COMPILE(_FOFF(slab, iPayload) == KSlabPayloadOffset);
__ASSERT_COMPILE(sizeof(slabset) == KSlabsetSize);
#endif

TInt RAllocatorHelper::TryLock()
	{
#ifdef __KERNEL_MODE__
	NKern::ThreadEnterCS();
	DMutex* m = *(DMutex**)(iAllocatorAddress + _FOFF(RHackHeap, iLock));
	if (m) Kern::MutexWait(*m);
	return KErrNone;
#else
	if (iAllocatorType != EUnknown && iAllocatorType != EAllocator)
		{
		RFastLock& lock = *reinterpret_cast<RFastLock*>(iAllocatorAddress + _FOFF(RHackHeap, iLock));
		lock.Wait();
		return KErrNone;
		}
	return KErrNotSupported;
#endif
	}

void RAllocatorHelper::TryUnlock()
	{
#ifdef __KERNEL_MODE__
	DMutex* m = *(DMutex**)(iAllocatorAddress + _FOFF(RHackHeap, iLock));
	if (m) Kern::MutexSignal(*m);
	NKern::ThreadLeaveCS();
#else
	if (iAllocatorType != EUnknown && iAllocatorType != EAllocator)
		{
		RFastLock& lock = *reinterpret_cast<RFastLock*>(iAllocatorAddress + _FOFF(RHackHeap, iLock));
		lock.Signal();
		}
#endif
	}

HUEXPORT_C void RAllocatorHelper::Close()
	{
	KERN_ENTER_CS();
	iAllocatorType = EUnknown;
	iAllocatorAddress = 0;
	delete iInfo;
	iInfo = NULL;
	iValidInfo = 0;
	MEM::Free(iTempSlabBitmap);
	iTempSlabBitmap = NULL;
	MEM::Free(iPageCache);
	iPageCache = NULL;
	iPageCacheAddr = 0;
	KERN_LEAVE_CS();
	}

TInt RAllocatorHelper::IdentifyAllocatorType(TBool aAllocatorIsUdeb, TBool aIsTheKernelHeap)
	{
	iAllocatorType = EUnknown;

	TUint32 handlesPtr = 0;
	TInt err = ReadWord(iAllocatorAddress + _FOFF(RHackAllocator, iHandles), handlesPtr);

	if (err) return err;
	if (aIsTheKernelHeap || 
		handlesPtr == iAllocatorAddress + _FOFF(RHackHeap, iChunkHandle) || 
		handlesPtr == iAllocatorAddress + _FOFF(RHackHeap, iLock))
		{
		// It's an RHeap of some kind - I doubt any other RAllocator subclass will use iHandles in this way
		TUint32 base = 0;
		err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iBase), base);
		if (err) return err;
		TInt objsize = (TInt)base - (TInt)iAllocatorAddress;
		if (objsize <= 32*4)
			{
			// Old RHeap
			iAllocatorType = aAllocatorIsUdeb ? EUdebOldRHeap : EUrelOldRHeap;
			}
		else
			{
			// new hybrid heap - bigger than the old one. Likewise figure out if udeb or urel.
			iAllocatorType = aAllocatorIsUdeb ? EUdebHybridHeap : EUrelHybridHeap;
			}
		}
	else
		{
		iAllocatorType = EAllocator;
		}
	return KErrNone;
	}

HUEXPORT_C TInt RAllocatorHelper::SetCellNestingLevel(TAny* aCell, TInt aNestingLevel)
	{
	TInt err = KErrNone;

	switch (iAllocatorType)
		{
		case EUdebOldRHeap:
		case EUdebHybridHeap:
			// By this reckoning, they're in the same place amazingly
			{
			TLinAddr nestingAddr = (TLinAddr)aCell - 8;
			err = WriteWord(nestingAddr, aNestingLevel);
			break;
			}
		default:
			break;
		}
	return err;
	}

HUEXPORT_C TInt RAllocatorHelper::GetCellNestingLevel(TAny* aCell, TInt& aNestingLevel)
	{
	switch (iAllocatorType)
		{
		case EUdebOldRHeap:
		case EUdebHybridHeap:
			// By this reckoning, they're in the same place amazingly
			{
			TLinAddr nestingAddr = (TLinAddr)aCell - 8;
			return ReadWord(nestingAddr, (TUint32&)aNestingLevel);
			}
		default:
			return KErrNotSupported;
		}
	}

TInt RAllocatorHelper::RefreshDetails(TUint aMask)
	{
	TInt err = FinishConstruction();
	if (err) return err;

	// Invalidate the page cache
	iPageCacheAddr = 0;

	TryLock();
	err = DoRefreshDetails(aMask);
	TryUnlock();
	return err;
	}

const TInt KHeapWalkStatsForOldHeap = (EUnusedPages|ECommittedFreeSpace);
const TInt KHeapWalkStatsForNewHeap = (EAllocated|ECount|EUnusedPages|ECommittedFreeSpace|EHybridStats);

TInt RAllocatorHelper::DoRefreshDetails(TUint aMask)
	{
	TInt err = KErrNotSupported;
	switch (iAllocatorType)
		{
		case EUrelOldRHeap:
		case EUdebOldRHeap:
			{
			if (aMask & ECommitted)
				{
				// The old RHeap::Size() used to use iTop - iBase, which was effectively chunkSize - sizeof(RHeap)
				// I think that for CommittedSize we should include the size of the heap object, just as it includes
				// the size of heap cell metadata and overhead. Plus it makes sure the committedsize is a multiple of the page size
				TUint32 top = 0;
				//TUint32 base = 0;
				//err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iBase), base);
				//if (err) return err;
				err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iTop), top);
				if (err) return err;

				//iInfo->iCommittedSize = top - base;
				iInfo->iCommittedSize = top - iAllocatorAddress;
				iValidInfo |= ECommitted;
				}
			if (aMask & EAllocated)
				{
				TUint32 allocSize = 0;
				err = ReadWord(iAllocatorAddress + _FOFF(RHackAllocator, iTotalAllocSize), allocSize);
				if (err) return err;
				iInfo->iAllocatedSize = allocSize;
				iValidInfo |= EAllocated;
				}
			if (aMask & ECount)
				{
				TUint32 count = 0;
				err = ReadWord(iAllocatorAddress + _FOFF(RHackAllocator, iCellCount), count);
				if (err) return err;
				iInfo->iAllocationCount = count;
				iValidInfo |= ECount;
				}
			if (aMask & EMaxSize)
				{
				TUint32 maxlen = 0;
				err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iMaxLength), maxlen);
				if (err) return err;
				iInfo->iMaxCommittedSize = maxlen;
				iValidInfo |= EMaxSize;
				}
			if (aMask & EMinSize)
				{
				TUint32 minlen = 0;
				err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iMaxLength) - 4, minlen); // This isn't a typo! iMinLength is 4 bytes before iMaxLength, on old heap ONLY
				if (err) return err;
				iInfo->iMinCommittedSize = minlen;
				iValidInfo |= EMinSize;
				}
			if (aMask & KHeapWalkStatsForOldHeap)
				{
				// Need a heap walk
				iInfo->ClearStats();
				iValidInfo = 0;
				err = DoWalk(&WalkForStats, NULL);
				if (err == KErrNone) iValidInfo |= KHeapWalkStatsForOldHeap;
				}
			return err;
			}
		case EUrelHybridHeap:
		case EUdebHybridHeap:
			{
			TBool needWalk = EFalse;
			if (aMask & ECommitted)
				{
				// RAllocator::Size uses iChunkSize - sizeof(RHybridHeap);
				// We can't do exactly the same, because we can't calculate sizeof(RHybridHeap), only ROUND_UP(sizeof(RHybridHeap), iAlign)
				// And if fact we don't bother and just use iChunkSize
				TUint32 chunkSize = 0;
				err = ReadWord(iAllocatorAddress + KChunkSizeOffset, chunkSize);
				if (err) return err;
				//TUint32 baseAddr = 0;
				//err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iBase), baseAddr);
				//if (err) return err;
				iInfo->iCommittedSize = chunkSize; // - (baseAddr - iAllocatorAddress);
				iValidInfo |= ECommitted;
				}
			if (aMask & (EAllocated|ECount))
				{
				if (iAllocatorType == EUdebHybridHeap)
					{
					// Easy, just get them from the counter
					TUint32 totalAlloc = 0;
					err = ReadWord(iAllocatorAddress + _FOFF(RHackAllocator, iTotalAllocSize), totalAlloc);
					if (err) return err;
					iInfo->iAllocatedSize = totalAlloc;
					iValidInfo |= EAllocated;

					TUint32 cellCount = 0;
					err = ReadWord(iAllocatorAddress + _FOFF(RHackAllocator, iCellCount), cellCount);
					if (err) return err;
					iInfo->iAllocationCount = cellCount;
					iValidInfo |= ECount;
					}
				else
					{
					// A heap walk is needed
					needWalk = ETrue;
					}
				}
			if (aMask & EMaxSize)
				{
				TUint32 maxlen = 0;
				err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iMaxLength), maxlen);
				if (err) return err;
				iInfo->iMaxCommittedSize = maxlen;
				iValidInfo |= EMaxSize;
				}
			if (aMask & EMinSize)
				{
				TUint32 minlen = 0;
				err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iAlign) + 4*4, minlen); // iMinLength is in different place to old RHeap
				if (err) return err;
				iInfo->iMinCommittedSize = minlen;
				iValidInfo |= EMinSize;
				}
			if (aMask & (EUnusedPages|ECommittedFreeSpace|EHybridStats))
				{
				// EAllocated and ECount have already been taken care of above
				needWalk = ETrue;
				}

			if (needWalk)
				{
				iInfo->ClearStats();
				iValidInfo = 0;
				err = DoWalk(&WalkForStats, NULL);
				if (err == KErrNone) iValidInfo |= KHeapWalkStatsForNewHeap;
				}
			return err;
			}
		default:
			return KErrNotSupported;
		}
	}

TInt RAllocatorHelper::CheckValid(TUint aMask)
	{
	if ((iValidInfo & aMask) == aMask)
		{
		return KErrNone;
		}
	else
		{
		return RefreshDetails(aMask);
		}
	}

HUEXPORT_C TInt RAllocatorHelper::CommittedSize()
	{
	TInt err = CheckValid(ECommitted);
	if (err) return err;
	return iInfo->iCommittedSize;
	}

HUEXPORT_C TInt RAllocatorHelper::AllocatedSize()
	{
	TInt err = CheckValid(EAllocated);
	if (err) return err;
	return iInfo->iAllocatedSize;
	}

HUEXPORT_C TInt RAllocatorHelper::AllocationCount()
	{
	TInt err = CheckValid(ECount);
	if (err) return err;
	return iInfo->iAllocationCount;
	}

HUEXPORT_C TInt RAllocatorHelper::RefreshDetails()
	{
	return RefreshDetails(iValidInfo);
	}

HUEXPORT_C TInt RAllocatorHelper::MaxCommittedSize()
	{
	TInt err = CheckValid(EMaxSize);
	if (err) return err;
	return iInfo->iMaxCommittedSize;
	}

HUEXPORT_C TInt RAllocatorHelper::MinCommittedSize()
	{
	TInt err = CheckValid(EMinSize);
	if (err) return err;
	return iInfo->iMinCommittedSize;
	}

HUEXPORT_C TInt RAllocatorHelper::AllocCountForCell(TAny* aCell) const
	{
	TUint32 allocCount = 0;
	switch (iAllocatorType)
		{
		case EUdebOldRHeap:
		case EUdebHybridHeap: // Both are in the same place, amazingly
			{
			TLinAddr allocCountAddr = (TLinAddr)aCell - 4;
			TInt err = ReadWord(allocCountAddr, allocCount);
			if (err) return err;
			return (TInt)allocCount;
			}
		default:
			return KErrNotSupported;
		}
	}

struct SContext3
	{
	RAllocatorHelper::TWalkFunc3 iOrigWalkFn;
	TAny* iOrigContext;
	};

TBool RAllocatorHelper::DispatchClientWalkCallback(RAllocatorHelper& aHelper, TAny* aContext, RAllocatorHelper::TExtendedCellType aCellType, TLinAddr aCellPtr, TInt aCellLength)
	{
	WalkForStats(aHelper, NULL, aCellType, aCellPtr, aCellLength);
	SContext3* context = static_cast<SContext3*>(aContext);
	return (*context->iOrigWalkFn)(aHelper, context->iOrigContext, aCellType, aCellPtr, aCellLength);
	}

HUEXPORT_C TInt RAllocatorHelper::Walk(TWalkFunc3 aCallbackFn, TAny* aContext)
	{
	// Might as well take the opportunity of updating our stats at the same time as walking the heap for the client
	SContext3 context = { aCallbackFn, aContext };

	TInt err = FinishConstruction(); // In case this hasn't been done yet
	if (err) return err;

	TryLock();
	err = DoWalk(&DispatchClientWalkCallback, &context);
	TryUnlock();
	return err;
	}

TInt RAllocatorHelper::DoWalk(TWalkFunc3 aCallbackFn, TAny* aContext)
	{
	TInt err = KErrNotSupported;
	switch (iAllocatorType)
		{
		case EUdebOldRHeap:
		case EUrelOldRHeap:
			err = OldSkoolWalk(aCallbackFn, aContext);
			break;
		case EUrelHybridHeap:
		case EUdebHybridHeap:
			err = NewHotnessWalk(aCallbackFn, aContext);
			break;
		default:
			err = KErrNotSupported;
			break;
		}
	return err;
	}

struct SContext
	{
	RAllocatorHelper::TWalkFunc iOrigWalkFn;
	TAny* iOrigContext;
	};

struct SContext2
	{
	RAllocatorHelper::TWalkFunc2 iOrigWalkFn;
	TAny* iOrigContext;
	};

#define New2Old(aNew) (((aNew)&RAllocatorHelper::EAllocationMask) ? RAllocatorHelper::EAllocation : ((aNew)&RAllocatorHelper::EFreeMask) ? RAllocatorHelper::EFreeSpace : RAllocatorHelper::EBadness)

TBool DispatchOldTWalkFuncCallback(RAllocatorHelper& /*aHelper*/, TAny* aContext, RAllocatorHelper::TExtendedCellType aCellType, TLinAddr aCellPtr, TInt aCellLength)
	{
	SContext* context = static_cast<SContext*>(aContext);
	return (*context->iOrigWalkFn)(context->iOrigContext, New2Old(aCellType), aCellPtr, aCellLength);
	}

TBool DispatchOldTWalk2FuncCallback(RAllocatorHelper& aHelper, TAny* aContext, RAllocatorHelper::TExtendedCellType aCellType, TLinAddr aCellPtr, TInt aCellLength)
	{
	SContext2* context = static_cast<SContext2*>(aContext);
	return (*context->iOrigWalkFn)(aHelper, context->iOrigContext, New2Old(aCellType), aCellPtr, aCellLength);
	}

HUEXPORT_C TInt RAllocatorHelper::Walk(TWalkFunc aCallbackFn, TAny* aContext)
	{
	// For backwards compatability insert a compatability callback to map between the different types of callback that clients requested
	SContext context = { aCallbackFn, aContext };
	return Walk(&DispatchOldTWalkFuncCallback, &context);
	}

HUEXPORT_C TInt RAllocatorHelper::Walk(TWalkFunc2 aCallbackFn, TAny* aContext)
	{
	SContext2 context = { aCallbackFn, aContext };
	return Walk(&DispatchOldTWalk2FuncCallback, &context);
	}


TInt RAllocatorHelper::OldSkoolWalk(TWalkFunc3 aCallbackFn, TAny* aContext)
	{
	TLinAddr pC = 0;
	TInt err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iBase), pC); // pC = iBase; // allocated cells
	if (err) return err;
	TLinAddr pF = iAllocatorAddress + _FOFF(RHackHeap, iAlign) + 3*4; // pF = &iFree; // free cells

	TLinAddr top = 0;
	err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iTop), top);
	if (err) return err;
	const TInt KAllocatedCellHeaderSize = iAllocatorType == EUdebOldRHeap ? 12 : 4;
	TInt minCell = 0;
	err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iAlign) + 4, (TUint32&)minCell);
	if (err) return err;
	TInt align = 0;
	err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iAlign), (TUint32&)align);
	if (err) return err;

	FOREVER
		{
		err = ReadWord(pF+4, pF); // pF = pF->next; // next free cell
		if (err) return err;
		TLinAddr pFnext = 0;
		if (pF) err = ReadWord(pF + 4, pFnext);
		if (err) return err;

		if (!pF)
			{
			pF = top; // to make size checking work
			}
		else if (pF>=top || (pFnext && pFnext<=pF) )
			{
			// free cell pointer off the end or going backwards
			//Unlock();
			(*aCallbackFn)(*this, aContext, EHeapBadFreeCellAddress, pF, 0);
			return KErrCorrupt;
			}
		else
			{
			TInt l; // = pF->len
			err = ReadWord(pF, (TUint32&)l);
			if (err) return err;
			if (l<minCell || (l & (align-1)))
				{
				// free cell length invalid
				//Unlock();
				(*aCallbackFn)(*this, aContext, EHeapBadFreeCellSize, pF, l);
				return KErrCorrupt;
				}
			}
		
		while (pC!=pF)				// walk allocated cells up to next free cell
			{
			TInt l; // pC->len;
			err = ReadWord(pC, (TUint32&)l);
			if (err) return err;
			if (l<minCell || (l & (align-1)))
				{
				// allocated cell length invalid
				//Unlock();
				(*aCallbackFn)(*this, aContext, EHeapBadAllocatedCellSize, pC, l);
				return KErrCorrupt;
				}
			TBool shouldContinue = (*aCallbackFn)(*this, aContext, EHeapAllocation, pC + KAllocatedCellHeaderSize, l - KAllocatedCellHeaderSize);
			if (!shouldContinue) return KErrNone;
			
			//SCell* pN = __NEXT_CELL(pC);
			TLinAddr pN = pC + l;
			if (pN > pF)
				{
				// cell overlaps next free cell
				//Unlock();
				(*aCallbackFn)(*this, aContext, EHeapBadAllocatedCellAddress, pC, l);
				return KErrCorrupt;
				}
			pC = pN;
			}
		if (pF == top)
			break;		// reached end of heap
		TInt pFlen = 0;
		err = ReadWord(pF, (TUint32&)pFlen);
		if (err) return err;
		pC = pF + pFlen; // pC = __NEXT_CELL(pF);	// step to next allocated cell
		TBool shouldContinue = (*aCallbackFn)(*this, aContext, EHeapFreeCell, pF, pFlen);
		if (!shouldContinue) return KErrNone;
		}
	return KErrNone;
	}

HUEXPORT_C TInt RAllocatorHelper::CountUnusedPages()
	{
	TInt err = CheckValid(EUnusedPages);
	if (err) return err;
	return iInfo->iUnusedPages;
	}

HUEXPORT_C TInt RAllocatorHelper::CommittedFreeSpace()
	{
	TInt err = CheckValid(ECommittedFreeSpace);
	if (err) return err;
	return iInfo->iCommittedFreeSpace;
	}

#define ROUND_DOWN(val, pow2) ((val) & ~((pow2)-1))
#define ROUND_UP(val, pow2) ROUND_DOWN((val) + (pow2) - 1, (pow2))

HUEXPORT_C TLinAddr RAllocatorHelper::AllocatorAddress() const
	{
	return iAllocatorAddress;
	}

TBool RAllocatorHelper::WalkForStats(RAllocatorHelper& aSelf, TAny* /*aContext*/, TExtendedCellType aType, TLinAddr aCellPtr, TInt aCellLength)
	{
	//ASSERT(aCellLength >= 0);
	THeapInfo& info = *aSelf.iInfo;

	TInt pagesSpanned = 0; // The number of pages that fit entirely inside the payload of this cell
	if ((TUint)aCellLength > KPageSize)
		{
		TLinAddr nextPageAlignedAddr = ROUND_UP(aCellPtr, KPageSize);
		pagesSpanned = ROUND_DOWN(aCellPtr + aCellLength - nextPageAlignedAddr, KPageSize) / KPageSize;
		}

	if (aSelf.iAllocatorType == EUrelOldRHeap || aSelf.iAllocatorType == EUdebOldRHeap)
		{
		if (aType & EFreeMask)
			{
			info.iUnusedPages += pagesSpanned;
			info.iCommittedFreeSpace += aCellLength;
			info.iHeapFreeCellCount++;
			}
		}
	else
		{
		if (aType & EAllocationMask)
			{
			info.iAllocatedSize += aCellLength;
			info.iAllocationCount++;
			}
		else if (aType & EFreeMask)
			{
			// I *think* that DLA will decommit pages from inside free cells...
			TInt committedLen = aCellLength - (pagesSpanned * KPageSize);
			info.iCommittedFreeSpace += committedLen;
			}

		switch (aType)
			{
			case EDlaAllocation:
				info.iDlaAllocsSize += aCellLength;
				info.iDlaAllocsCount++;
				break;
			case EPageAllocation:
				info.iPageAllocsSize += aCellLength;
				info.iPageAllocsCount++;
				break;
			case ESlabAllocation:
				info.iSlabAllocsSize += aCellLength;
				info.iSlabAllocsCount++;
				break;
			case EDlaFreeCell:
				info.iDlaFreeSize += aCellLength;
				info.iDlaFreeCount++;
				break;
			case ESlabFreeCell:
				info.iSlabFreeCellSize += aCellLength;
				info.iSlabFreeCellCount++;
				break;
			case ESlabFreeSlab:
				info.iSlabFreeSlabSize += aCellLength;
				info.iSlabFreeSlabCount++;
				break;
			default:
				break;
			}
		}

	return ETrue;
	}

#define PAGESHIFT 12

TUint RAllocatorHelper::PageMapOperatorBrackets(unsigned ix, TInt& err) const
	{
	//return 1U&(iBase[ix>>3] >> (ix&7));
	TUint32 basePtr = 0;
	err = ReadWord(iAllocatorAddress + KPageMapOffset, basePtr);
	if (err) return 0;

	TUint8 res = 0;
	err = ReadByte(basePtr + (ix >> 3), res);
	if (err) return 0;

	return 1U&(res >> (ix&7));
	}


TInt RAllocatorHelper::PageMapFind(TUint start, TUint bit, TInt& err)
	{
	TUint32 iNbits = 0;
	err = ReadWord(iAllocatorAddress + KPageMapOffset + 4, iNbits);
	if (err) return 0;

	if (start<iNbits) do
		{
		//if ((*this)[start]==bit)
		if (PageMapOperatorBrackets(start, err) == bit || err)
			return start;
		} while (++start<iNbits);
	return -1;
	}

TUint RAllocatorHelper::PagedDecode(TUint pos, TInt& err)
	{
	unsigned bits = PageMapBits(pos,2,err);
	if (err) return 0;
	bits >>= 1;
	if (bits == 0)
		return 1;
	bits = PageMapBits(pos+2,2,err);
	if (err) return 0;
	if ((bits & 1) == 0)
		return 2 + (bits>>1);
	else if ((bits>>1) == 0)
		{
		return PageMapBits(pos+4, 4,err);
		}
	else
		{
		return PageMapBits(pos+4, 18,err);
		}
	}

TUint RAllocatorHelper::PageMapBits(unsigned ix, unsigned len, TInt& err)
	{
	int l=len;
	unsigned val=0;
	unsigned bit=0;
	while (--l>=0)
		{
		//val |= (*this)[ix++]<<bit++;
		val |= PageMapOperatorBrackets(ix++, err) << bit++;
		if (err) return 0;
		}
	return val;
	}

enum TSlabType { ESlabFullInfo, ESlabPartialInfo, ESlabEmptyInfo };

#ifndef TEST_HYBRIDHEAP_ASSERTS
#define MAXSLABSIZE		56
#define	SLABSHIFT		10
#define	SLABSIZE		(1 << SLABSHIFT)
const TInt KMaxSlabPayload = SLABSIZE - KSlabPayloadOffset;
#endif

TInt RAllocatorHelper::NewHotnessWalk(TWalkFunc3 aCallbackFn, TAny* aContext)
	{
	// RHybridHeap does paged, slab then DLA, so that's what we do too
	// Remember Kernel RHybridHeaps don't even have the page and slab members

	TUint32 basePtr;
	TInt err = ReadWord(iAllocatorAddress + _FOFF(RHackHeap, iBase), basePtr);
	if (err) return err;
	if (basePtr < iAllocatorAddress + KUserHybridHeapSize)
		{
		// Must be a kernel one - don't do page and slab
		}
	else
		{
		// Paged
		TUint32 membase = 0;
		err = ReadWord(iAllocatorAddress + KPageMapOffset + 8, membase);
		if (err) return err;

		TBool shouldContinue = ETrue;
		for (int ix = 0;(ix = PageMapFind(ix,1,err)) >= 0 && err == KErrNone;)
			{
			int npage = PagedDecode(ix, err);
			if (err) return err;
			// Introduce paged buffer to the walk function 
			TLinAddr bfr = membase + (1 << (PAGESHIFT-1))*ix;
			int len = npage << PAGESHIFT;
			if ( (TUint)len > KPageSize )
				{ // If buffer is not larger than one page it must be a slab page mapped into bitmap
				if (iAllocatorType == EUdebHybridHeap)
					{
					bfr += 8;
					len -= 8;
					}
				shouldContinue = (*aCallbackFn)(*this, aContext, EPageAllocation, bfr, len);
				if (!shouldContinue) return KErrNone;
				}
			ix += (npage<<1);
			}
		if (err) return err;

		// Slab
		TUint32 sparePage = 0;
		err = ReadWord(iAllocatorAddress + KSparePageOffset, sparePage);
		if (err) return err;
		if (sparePage)
			{
			//Walk(wi, iSparePage, iPageSize, EGoodFreeCell, ESlabSpare); // Introduce Slab spare page to the walk function 
			// This counts as 4 spare slabs
			for (TInt i = 0; i < 4; i++)
				{
				shouldContinue = (*aCallbackFn)(*this, aContext, ESlabFreeSlab, sparePage + SLABSIZE*i, SLABSIZE);
				if (!shouldContinue) return KErrNone;
				}
			}

		//TreeWalk(&iFullSlab, &SlabFullInfo, i, wi);
		TInt err = TreeWalk(iAllocatorAddress + KFullSlabOffset, ESlabFullInfo, aCallbackFn, aContext, shouldContinue);
		if (err || !shouldContinue) return err;
		for (int ix = 0; ix < (MAXSLABSIZE>>2); ++ix)
			{
			TUint32 partialAddr = iAllocatorAddress + KSlabAllocOffset + ix*KSlabsetSize;
			//TreeWalk(&iSlabAlloc[ix].iPartial, &SlabPartialInfo, i, wi);
			err = TreeWalk(partialAddr, ESlabPartialInfo, aCallbackFn, aContext, shouldContinue);
			if (err || !shouldContinue) return err;
			}
		//TreeWalk(&iPartialPage, &SlabEmptyInfo, i, wi);
		TreeWalk(iAllocatorAddress + KPartialPageOffset, ESlabEmptyInfo, aCallbackFn, aContext, shouldContinue);
		}

	// DLA
#define CHUNK_OVERHEAD (sizeof(TUint))
#define CHUNK_ALIGN_MASK (7) 
#define CHUNK2MEM(p)        ((TLinAddr)(p) + 8)
#define MEM2CHUNK(mem)      ((TLinAddr)(p) - 8)
/* chunk associated with aligned address A */
#define ALIGN_OFFSET(A)\
	((((TLinAddr)(A) & CHUNK_ALIGN_MASK) == 0)? 0 :\
	((8 - ((TLinAddr)(A) & CHUNK_ALIGN_MASK)) & CHUNK_ALIGN_MASK))
#define ALIGN_AS_CHUNK(A)   ((A) + ALIGN_OFFSET(CHUNK2MEM(A)))
#define CINUSE_BIT 2
#define INUSE_BITS 3

	TUint32 topSize = 0;
	err = ReadWord(iAllocatorAddress + KMallocStateOffset + KMallocStateTopSizeOffset, topSize);
	if (err) return err;

	TUint32 top = 0;
	err = ReadWord(iAllocatorAddress + KMallocStateOffset + KMallocStateTopOffset, top);
	if (err) return err;

	TInt max = ((topSize-1) & ~CHUNK_ALIGN_MASK) - CHUNK_OVERHEAD;
	if ( max < 0 )
		max = 0;
	
	TBool shouldContinue = (*aCallbackFn)(*this, aContext, EDlaFreeCell, top, max);
	if (!shouldContinue) return KErrNone;
	
	TUint32 mallocStateSegBase = 0;
	err = ReadWord(iAllocatorAddress + KMallocStateOffset + KMallocStateSegOffset, mallocStateSegBase);
	if (err) return err;

	for (TLinAddr q = ALIGN_AS_CHUNK(mallocStateSegBase); q != top; /*q = NEXT_CHUNK(q)*/)
		{
		TUint32 qhead = 0;
		err = ReadWord(q + 4, qhead);
		if (err) return err;
		//TInt sz = CHUNKSIZE(q);
		TInt sz = qhead & ~(INUSE_BITS);
		if (!(qhead & CINUSE_BIT))
			{
			//Walk(wi, CHUNK2MEM(q), sz, EGoodFreeCell, EDougLeaAllocator); // Introduce DL free buffer to the walk function 
			shouldContinue = (*aCallbackFn)(*this, aContext, EDlaFreeCell, CHUNK2MEM(q), sz);
			if (!shouldContinue) return KErrNone;
			}
		else
			{
			//Walk(wi, CHUNK2MEM(q), (sz- CHUNK_OVERHEAD), EGoodAllocatedCell, EDougLeaAllocator); // Introduce DL allocated buffer to the walk function 
			TLinAddr addr = CHUNK2MEM(q);
			TInt size = sz - CHUNK_OVERHEAD;
			if (iAllocatorType == EUdebHybridHeap)
				{
				size -= 8;
				addr += 8;
				}
			shouldContinue = (*aCallbackFn)(*this, aContext, EDlaAllocation, addr, size);
			if (!shouldContinue) return KErrNone;
			}
		// This is q = NEXT_CHUNK(q) expanded
		q = q + sz;
		}
	return KErrNone;
	}

TInt RAllocatorHelper::TreeWalk(TUint32 aSlabRoot, TInt aSlabType, TWalkFunc3 aCallbackFn, TAny* aContext, TBool& shouldContinue)
	{
	const TSlabType type = (TSlabType)aSlabType;

	TUint32 s = 0;
	TInt err = ReadWord(aSlabRoot, s);
	if (err) return err;
	//slab* s = *root;
	if (!s)
		return KErrNone;
	
	for (;;)
		{
		//slab* c;
		//while ((c = s->iChild1) != 0)
		//	s = c;		// walk down left side to end
		TUint32 c;
		for(;;)
			{
			err = ReadWord(s + KSlabChild1Offset, c);
			if (err) return err;
			if (c == 0) break;
			else s = c;
			}
		for (;;)
			{
			//TODOf(s, i, wi);
			//TODO __HEAP_CORRUPTED_TEST_STATIC
			TUint32 h;
			err = ReadWord(s, h); // = aSlab->iHeader;
			if (err) return err;
			TUint32 size = (h&0x0003f000)>>12; //SlabHeaderSize(h);
			TUint debugheadersize = 0;
			if (iAllocatorType == EUdebHybridHeap) debugheadersize = 8;
			TUint32 usedCount = (((h&0x0ffc0000)>>18) + 4) / size; // (SlabHeaderUsedm4(h) + 4) / size;
			switch (type)
				{
				case ESlabFullInfo:
					{
					TUint32 count = usedCount;
					TUint32 i = 0;
					while ( i < count )
						{
						TUint32 addr = s + KSlabPayloadOffset + i*size; //&aSlab->iPayload[i*size];
						shouldContinue = (*aCallbackFn)(*this, aContext, ESlabAllocation, addr + debugheadersize, size - debugheadersize);
						if (!shouldContinue) return KErrNone;
						i++;
						}
					break;
					}
				case ESlabPartialInfo:
					{
					//TODO __HEAP_CORRUPTED_TEST_STATIC
					TUint32 count = KMaxSlabPayload / size;
					TUint32 freeOffset = (h & 0xff) << 2;
					if (freeOffset == 0)
						{
						// TODO Shouldn't happen for a slab on the partial list
						}
					memset(iTempSlabBitmap, 1, KTempBitmapSize); // Everything defaults to in use
					TUint wildernessCount = count - usedCount;
					while (freeOffset)
						{
						wildernessCount--;
						TInt idx = (freeOffset-KSlabPayloadOffset)/size;
						LOG("iTempSlabBitmap freeOffset %d index %d", freeOffset, idx);
						iTempSlabBitmap[idx] = 0; // Mark it as free

						TUint32 addr = s + freeOffset;
						TUint8 nextCell = 0;
						err = ReadByte(addr, nextCell);
						if (err) return err;
						freeOffset = ((TUint32)nextCell) << 2;
						}
					memset(iTempSlabBitmap + count - wildernessCount, 0, wildernessCount); // Mark the wilderness as free
					for (TInt i = 0; i < count; i++)
						{
						TLinAddr addr = s + KSlabPayloadOffset + i*size;
						if (iTempSlabBitmap[i])
							{
							// In use
							shouldContinue = (*aCallbackFn)(*this, aContext, ESlabAllocation, addr + debugheadersize, size - debugheadersize);
							}
						else
							{
							// Free
							shouldContinue = (*aCallbackFn)(*this, aContext, ESlabFreeCell, addr, size);
							}
						if (!shouldContinue) return KErrNone;
						}
					break;
					}
				case ESlabEmptyInfo:
					{
					// Check which slabs of this page are empty
					TUint32 pageAddr = ROUND_DOWN(s, KPageSize);
					TUint32 headerForPage = 0;
					err = ReadWord(pageAddr, headerForPage);
					if (err) return err;
					TUint32 slabHeaderPageMap = (headerForPage & 0x00000f00)>>8; // SlabHeaderPagemap(unsigned h)
					for (TInt slabIdx = 0; slabIdx < 4; slabIdx++)
						{
						if (slabHeaderPageMap & (1<<slabIdx))
							{
							TUint32 addr = pageAddr + SLABSIZE*slabIdx + KSlabPayloadOffset; //&aSlab->iPayload[i*size];
							shouldContinue = (*aCallbackFn)(*this, aContext, ESlabFreeSlab, addr, KMaxSlabPayload);
							if (!shouldContinue) return KErrNone;
							}
						}
					break;
					}
				}

			//c = s->iChild2;
			err = ReadWord(s + KSlabChild2Offset, c);
			if (err) return err;

			if (c)
				{	// one step down right side, now try and walk down left
				s = c;
				break;
				}
			for (;;)
				{	// loop to walk up right side
				TUint32 pp = 0;
				err = ReadWord(s + KSlabParentOffset, pp);
				if (err) return err;
				//slab** pp = s->iParent;
				if (pp == aSlabRoot)
					return KErrNone;
#define SlabFor(x) ROUND_DOWN(x, SLABSIZE)
				s = SlabFor(pp);
				//if (pp == &s->iChild1)
				if (pp == s + KSlabChild1Offset)
					break;
				}
			}
		}
	}

// Really should be called TotalSizeForCellType(...)
HUEXPORT_C TInt RAllocatorHelper::SizeForCellType(TExtendedCellType aType)
	{
	if (aType & EBadnessMask) return KErrArgument;
	if (aType == EAllocationMask) return AllocatedSize();

	if (iAllocatorType == EUdebOldRHeap || iAllocatorType == EUrelOldRHeap)
		{
		switch (aType)
			{
			case EHeapAllocation:
				return AllocatedSize();
			case EHeapFreeCell:
			case EFreeMask:
				return CommittedFreeSpace();
			default:
				return KErrNotSupported;
			}
		}
	else if (iAllocatorType == EUrelHybridHeap || iAllocatorType == EUdebHybridHeap)
		{
		TInt err = CheckValid(EHybridStats);
		if (err) return err;

		switch (aType)
			{
			case EHeapAllocation:
			case EHeapFreeCell:
				return KErrNotSupported;
			case EDlaAllocation:
				return iInfo->iDlaAllocsSize;
			case EPageAllocation:
				return iInfo->iPageAllocsSize;
			case ESlabAllocation:
				return iInfo->iSlabAllocsSize;
			case EDlaFreeCell:
				return iInfo->iDlaFreeSize;
			case ESlabFreeCell:
				return iInfo->iSlabFreeCellSize;
			case ESlabFreeSlab:
				return iInfo->iSlabFreeSlabSize;
			case EFreeMask:
				// Note this isn't the same as asking for CommittedFreeSpace(). SizeForCellType(EFreeMask) may include decommitted pages that lie inside a free cell
				return iInfo->iDlaFreeSize + iInfo->iSlabFreeCellSize + iInfo->iSlabFreeSlabSize;
			default:
				return KErrNotSupported;
			}
		}
	else
		{
		return KErrNotSupported;
		}
	}

HUEXPORT_C TInt RAllocatorHelper::CountForCellType(TExtendedCellType aType)
	{
	if (aType & EBadnessMask) return KErrArgument;
	if (aType == EAllocationMask) return AllocationCount();

	if (iAllocatorType == EUdebOldRHeap || iAllocatorType == EUrelOldRHeap)
		{
		switch (aType)
			{
			case EHeapAllocation:
				return AllocationCount();
			case EHeapFreeCell:
			case EFreeMask:
				{
				TInt err = CheckValid(ECommittedFreeSpace);
				if (err) return err;
				return iInfo->iHeapFreeCellCount;
				}
			default:
				return KErrNotSupported;
			}
		}
	else if (iAllocatorType == EUrelHybridHeap || iAllocatorType == EUdebHybridHeap)
		{
		TInt err = CheckValid(EHybridStats);
		if (err) return err;

		switch (aType)
			{
			case EHeapAllocation:
			case EHeapFreeCell:
				return KErrNotSupported;
			case EDlaAllocation:
				return iInfo->iDlaAllocsCount;
			case EPageAllocation:
				return iInfo->iPageAllocsCount;
			case ESlabAllocation:
				return iInfo->iSlabAllocsCount;
			case EDlaFreeCell:
				return iInfo->iDlaFreeCount;
			case ESlabFreeCell:
				return iInfo->iSlabFreeCellCount;
			case ESlabFreeSlab:
				return iInfo->iSlabFreeSlabCount;
			case EFreeMask:
				// This isn't a hugely meaningful value, but if that's what they asked for...
				return iInfo->iDlaFreeCount + iInfo->iSlabFreeCellCount + iInfo->iSlabFreeSlabCount;
			default:
				return KErrNotSupported;
			}
		}
	else
		{
		return KErrNotSupported;
		}
	}

HUEXPORT_C TBool LtkUtils::RAllocatorHelper::AllocatorIsUdeb() const
	{
	return iAllocatorType == EUdebOldRHeap || iAllocatorType == EUdebHybridHeap;
	}


HUEXPORT_C const TDesC& LtkUtils::RAllocatorHelper::Description() const
	{
	_LIT(KRHeap, "RHeap");
	_LIT(KRHybridHeap, "RHybridHeap");
	_LIT(KUnknown, "Unknown");
	switch (iAllocatorType)
		{
		case EUrelOldRHeap:
		case EUdebOldRHeap:
			return KRHeap;
		case EUrelHybridHeap:
		case EUdebHybridHeap:
			return KRHybridHeap;
		case EAllocator:
		case EUnknown:
		default:
			return KUnknown;
		}
	}

#ifdef __KERNEL_MODE__

DChunk* LtkUtils::RAllocatorHelper::OpenUnderlyingChunk()
	{
	// Enter and leave in CS and with no locks held. On exit the returned DChunk has been Open()ed.
	TInt err = iChunk->Open();
	if (err) return NULL;
	return iChunk;
	}

DChunk* LtkUtils::RUserAllocatorHelper::OpenUnderlyingChunk()
	{
	if (iAllocatorType != EUrelOldRHeap && iAllocatorType != EUdebOldRHeap && iAllocatorType != EUrelHybridHeap && iAllocatorType != EUdebHybridHeap) return NULL;
	// Note RUserAllocatorHelper doesn't use or access RAllocatorHelper::iChunk, because we figure out the chunk handle in a different way.
	// It is for this reason that iChunk is private, to remove temptation
	
	// Enter and leave in CS and with no locks held. On exit the returned DChunk has been Open()ed.
	TUint32 chunkHandle = 0;
	TInt err = ReadData(iAllocatorAddress + _FOFF(RHackHeap, iChunkHandle), &chunkHandle, sizeof(TUint32));
	if (err) return NULL;

	NKern::LockSystem();
	DChunk* result = (DChunk*)Kern::ObjectFromHandle(iThread, chunkHandle, EChunk);
	if (result && result->Open() != KErrNone)
		{
		result = NULL;
		}
	NKern::UnlockSystem();
	return result;
	}

LtkUtils::RAllocatorHelper::TType LtkUtils::RAllocatorHelper::GetType() const
	{
	switch (iAllocatorType)
		{
		case EUrelOldRHeap:
		case EUdebOldRHeap:
			return ETypeRHeap;
		case EUrelHybridHeap:
		case EUdebHybridHeap:
			return ETypeRHybridHeap;
		case EAllocator:
		case EUnknown:
		default:
			return ETypeUnknown;
		}
	}

#else

TInt LtkUtils::RAllocatorHelper::EuserIsUdeb()
	{
	TAny* buf = User::Alloc(4096);
	if (!buf) return KErrNoMemory;
	RAllocator* dummyHeap = UserHeap::FixedHeap(buf, 4096, 4, ETrue);
	if (!dummyHeap) return KErrNoMemory; // Don't think this can happen

	dummyHeap->__DbgSetAllocFail(RAllocator::EFailNext, 1);
	TAny* ptr = dummyHeap->Alloc(4);
	// Because we specified singleThreaded=ETrue we can allow dummyHeap to just go out of scope here
	User::Free(buf);

	if (ptr)
		{
		// Clearly the __DbgSetAllocFail had no effect so we must be urel
		// We don't need to free ptr because it came from the dummy heap
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

#ifndef STANDALONE_ALLOCHELPER

#include <fshell/ltkutils.h>
HUEXPORT_C void LtkUtils::MakeHeapCellInvisible(TAny* aCell)
	{
	RAllocatorHelper helper;
	TInt err = helper.Open(&User::Allocator());
	if (err == KErrNone)
		{
		helper.SetCellNestingLevel(aCell, -1);
		helper.Close();
		}
	}
#endif // STANDALONE_ALLOCHELPER

#endif
