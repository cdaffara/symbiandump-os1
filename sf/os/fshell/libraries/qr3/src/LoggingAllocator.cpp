// LoggingAllocator.cpp
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
#include <fshell/LoggingAllocator.h>
#include <fshell/extrabtrace.h>
//#include <e32debug.h>

//#define LOG(args...) RDebug::Printf(args)
#define LOG(args...)

// Subcategories for use with BTrace::ETest1 when EOldFormatLogging is defined
enum TSubCategories
	{
	EAlloc,
	EAllocCont,
	EAllocHeaven, // AllocHeaven means "This cell has leaked"
	EAllocHeavenData,
	EFree,
	EFreeCont,
	EFreeCellCorrupted,
	};

const TInt KHeapCategory = 14; // BTrace::EHeap
enum THeap
	{
	EHeapCreate,
	EHeapChunkCreate,
	EHeapAlloc,
	EHeapReAlloc,
	EHeapFree,
	// My additions go after this
	ELoggingAllocatorInstalled = 128,
	EHeapPrimingFinished,
	EHeapExtendedAlloc,
	EHeapExtendedFree,
	EHeapExtendedRealloc,
	};

const TInt KUidUninstallLoggingAllocator = 0x10285BAB;
const TInt KMaxDeferredFrees = 100;

RLoggingAllocator::RLoggingAllocator(TUint aFlags)
	: RAllocator(), iA(NULL), iBreakOnAllocCount(0), iFlags(aFlags)
	{
	if (iFlags & EDeferFree) iFlags |= EScribbleFrees; // The only reason (currently) why you defer them is to do the check, which requires them to be scribbled
	iPid = RProcess().Id(); // Cache this to save a few exec calls
	}

void RLoggingAllocator::Free(TAny* aPtr)
	{
	if (!aPtr) return; // Don't care about tracing attempts to free the null pointer (this is what rheap does too)
	TraceFree(iA, aPtr);
	TInt allocLen = AllocLen(aPtr);

	if (iFlags & EScribbleFrees)
		{
		// Don't scribble after calling free - the heap might have been shrunk meaning we'd crash
		memset(aPtr, 0xDF, allocLen);
		}
	if (iFlags & EDeferFree)
		{
		CheckDeferredFrees();
		iLock.Wait();
		if (iDeferredFrees.Count() == KMaxDeferredFrees)
			{
			iA->Free(iDeferredFrees[0].iPtr);
			iDeferredFrees.Remove(0);
			}
		SDeferredFreeCell cell = {aPtr, allocLen};
		iDeferredFrees.Append(cell); // Can't fail because we preallocate the memory
		iLock.Signal();
		}
	else
		{
		iA->Free(aPtr);
		}

	if (iFlags & EOldFormatLogging)
		{
		BTrace8(KHeapCategory, EHeapFree, iA, aPtr);
		}
	}

TAny* RLoggingAllocator::Alloc(TInt aSize)
	{
	CheckDeferredFrees();
	TAny* cellPtr = iA->Alloc(aSize);
	TraceAlloc(iA, cellPtr, aSize);
	if (cellPtr && (iFlags & EOldFormatLogging))
		{
		TUint32 remainder[] = { AllocLen(cellPtr), aSize };
		BTraceN(KHeapCategory, EHeapAlloc, iA, cellPtr, remainder, sizeof(remainder));
		}
	return cellPtr;
	}

void RLoggingAllocator::DoTraceAllocEvent(RAllocator* aAllocator, TAny* aCellPtr, TInt aAllocEvent, TInt aRequestedSize, TAny* aOldPtr)
	{
	// Important we don't reference iA in this function, only aAllocator
	TUint32 t = 0;
	TUint32* sp = &t;
	TThreadStackInfo stackInfo;
	TInt err = RThread().StackInfo(stackInfo);
	if (err == KErrNone && (iFlags & EOldFormatLogging))
		{
		if (aAllocEvent != EHeapAlloc && aAllocEvent != EHeapFree) return; // We don't do stacktraces for reallocs in the old format
		TBuf8<KMaxBTraceDataArray> buf;
		const TInt KNumBtraceFrames = 2;
		for (TInt i = 0; i < KNumBtraceFrames; ++i)
			{
			while (((TUint32)sp < (TUint32)stackInfo.iBase) && (buf.Size() <= (buf.MaxSize() - 4)))
				{
				if ((*sp >= 0x70000000) && (*sp <= 0x8fffffff)) // Is this a valid ROM or RAM address? Note, the address ranges are for the 2GB EKA2 memory map.
					{
					buf.Append(TPtrC8((TUint8*)sp, 4));
					}
				++sp;
				}
			if (buf.Size() > 0)
				{
				TUint8 subcat = (aAllocEvent == EHeapAlloc) ? EAlloc : EFree;
				BTraceN(BTrace::ETest1, i == 0 ? subcat : subcat+1, (TUint)RThread().Id().Id(), aCellPtr, buf.Ptr(), buf.Size());
				buf.Zero();
				}
			}
		buf.FillZ(); // To avoid leaving a bunch of addresses kicking around on the stack to be misinterpretted later on.
		}
	else if (err == KErrNone)
		{
		// New format
		const TInt KBufSize = 160;
		TUint8 buf[KBufSize];
		TUint32* ptr = (TUint32*)buf;
		TUint32 const*const end = (TUint32*)(buf + KBufSize);
		// uint 0 is aAllocator, argument a1 to BTraceBig below
		*(ptr++) = (TLinAddr)aCellPtr; // uint 1
		TBool alreadyFreed = (aAllocEvent == EHeapFree && aRequestedSize == -1); // Can't call AllocLen on a freed cell (we only catch the frees from ReAlloc after the cell's already been deallocated)
		 // uint 2 is cell size, unless it's not a valid cell in which case it's zero (for a failed NULL alloc) or -1 for a free that's already been freed
		if (alreadyFreed)
			{
			*(ptr++) = 0xFFFFFFFFu;
			}
		else if (aCellPtr == NULL)
			{
			*(ptr++) = 0;
			}
		else
			{
			*(ptr++) = aAllocator->AllocLen(aCellPtr);
			}
		*(ptr++) = (aAllocEvent == EHeapFree) ? 0xFFFFFFFFu : aRequestedSize; // uint 3
		*(ptr++) = (TUint)RThread().Id(); // uint 4
		*(ptr++) = iPid; // uint 5
		if (aAllocEvent == EHeapReAlloc) *(ptr++) = (TUint)aOldPtr; // uint 6, for reallocs only
		while (((TUint32)sp < (TUint32)stackInfo.iBase) && ptr != end)
			{
			TUint addr = *sp;
			if ((addr >= 0x70000000) && (addr <= 0x8fffffff)) // Is this a valid ROM or RAM address? Note, the address ranges are for the 2GB EKA2 memory map.
				{
				*(ptr++) = addr;
				}
			sp++;
			}
		TUint8 subcat;
		switch (aAllocEvent)
			{
			case EHeapAlloc: subcat = EHeapExtendedAlloc; break;
			case EHeapFree: subcat = EHeapExtendedFree; break;
			case EHeapReAlloc: subcat = EHeapExtendedRealloc; break;
			default:
				return; // Shouldn't get this
			}
		BTraceBig(KHeapCategory, subcat, aAllocator, buf, (TLinAddr)ptr - (TLinAddr)buf);
		Mem::FillZ(buf, KBufSize); // To avoid leaving a bunch of addresses kicking around on the stack to be misinterpretted later on.
		}

#ifdef _DEBUG
	if (iBreakOnAllocCount && iHelper.AllocCountForCell(aCellPtr) == iBreakOnAllocCount)
		{
		iBreakOnAllocCount = *&iBreakOnAllocCount; // Something to break on
		}
#endif
	}

EXPORT_C void RLoggingAllocator::StaticTraceAlloc(RAllocator* aAllocator, TAny* aCellPtr, TInt aRequestedSize)
	{
	RLoggingAllocator(0).TraceAlloc(aAllocator, aCellPtr, aRequestedSize);
	//if (iFlags & EOldFormatLogging)
	//	{
	//	TUint32 remainder[] = { aAllocator->AllocLen(aCellPtr), aRequestedSize };
	//	BTraceN(KHeapCategory, EHeapAlloc, aAllocator, aCellPtr, remainder, sizeof(remainder));
	//	}
	}

EXPORT_C void RLoggingAllocator::StaticTraceFree(RAllocator* aAllocator, TAny* aCellPtr)
	{
	RLoggingAllocator(0).TraceFree(aAllocator, aCellPtr);
	//if (iFlags & EOldFormatLogging)
	//	{
	//	BTrace8(KHeapCategory, EHeapFree, aAllocator, aCellPtr);
	//	}
	}

TAny* RLoggingAllocator::ReAlloc(TAny* aPtr, TInt aSize, TInt aMode)
	{
	TAny* res = iA->ReAlloc(aPtr, aSize, aMode);
	if (res != aPtr)
		{
		// If the realloc has actually reallocated, we need to trace the free, to be compatible with how RHeap does it.
		if (res && aPtr)
			{
			// It is technically legal to call ReAlloc passing in a null pointer - it behaves the same as alloc
			TraceFree(iA, aPtr, ETrue);
			if (iFlags & EOldFormatLogging)
				{
				BTrace8(KHeapCategory, EHeapFree, iA, aPtr);
				}
			}
		}
	TraceRealloc(res, aPtr, aSize);
	if (res && (iFlags & EOldFormatLogging))
		{
		TUint32 remainder [] = { AllocLen(res), aSize, (TUint32)aPtr };
		BTraceN(KHeapCategory, EHeapReAlloc, iA, res, remainder, sizeof(remainder));
		}
	return res;
	}

TInt RLoggingAllocator::AllocLen(const TAny* aCell) const
	{
	return iA->AllocLen(aCell);
	}

TInt RLoggingAllocator::Compress()
	{
	return iA->Compress();
	}

void RLoggingAllocator::Reset()
	{
	iA->Reset();
	}

TInt RLoggingAllocator::AllocSize(TInt& aTotalAllocSize) const
	{
	return iA->AllocSize(aTotalAllocSize);
	}

TInt RLoggingAllocator::Available(TInt& aBiggestBlock) const
	{
	return iA->Available(aBiggestBlock);
	}


TInt RLoggingAllocator::DebugFunction(TInt aFunc, TAny* a1, TAny* a2)
	{
	LOG("LA: DebugFunction %d", aFunc);
	if (aFunc == RAllocator::EMarkEnd)
		{
		// First we need to work out the nesting level of the underlying heap, even though we don't have a pointer
		// to said underlying heap. So allocate a cell, and pull the nest level out from there.
		// If that returns an error, it's not a udeb heap so we don't continue
		TAny* testAlloc = iA->Alloc(4);
		if (testAlloc)
			{
			TInt nestingLevel;
			TInt err = iHelper.GetCellNestingLevel(testAlloc, nestingLevel);
			iA->Free(testAlloc);
			if (!err)
				{
				LOG("LA: Doing walk, nestinglevel = %d", nestingLevel);
				iHelper.Walk(&HeavenWalk, (TAny*)nestingLevel);
				// It doesn't actually matter what Walk returns, either the data got written or it didn't
				}
			}
		
		// Actually do the real markEnd that does more than just output the allocHeavens
		return iA->DebugFunction(aFunc, a1, a2);
		}
#ifndef __KERNEL_MODE__
	else if (aFunc == KUidUninstallLoggingAllocator)
		{
		RAllocator* current = &User::Allocator();
		if (current == this)
			{
			User::SwitchAllocator(iA);
			Destroy();
			return KErrNone;
			}
		else
			{
			return KErrNotFound;
			}
		}
#endif
	else if (aFunc == KTempDisableLogging)
		{
		RAllocator** result = (RAllocator**)a1;
		*result = &User::Allocator();
		User::SwitchAllocator(iA);
		return KErrNone;
		}
	else
		{
		return iA->DebugFunction(aFunc, a1, a2);
		}
	}

TBool RLoggingAllocator::TraceExistingAllocs(TAny* aContext, RAllocatorHelper::TCellType aType, TLinAddr aCell, TInt aLen)
	{
	RLoggingAllocator* self = (RLoggingAllocator*)aContext;
	if (aType == RAllocatorHelper::EAllocation)
		{
		TAny* cellPtr = (TAny*)aCell;
		if (self->iFlags & EOldFormatLogging)
			{
			TUint32 remainder[] = { aLen, aLen };
			BTraceN(KHeapCategory, EHeapAlloc, self->iA, cellPtr, remainder, sizeof(remainder));
			}
		else
			{
			TUint32 remainder[] = { aLen, aLen, RThread().Id(), self->iPid };
			TUint8 subcat = EHeapExtendedAlloc;
			BTraceN(KHeapCategory, subcat, self->iA, cellPtr, remainder, sizeof(remainder));
			}
		}
	return ETrue; // Keep going
	}

EXPORT_C TInt RLoggingAllocator::Install()
	{
	return Install(0);
	}

EXPORT_C TInt RLoggingAllocator::Install(TUint aFlags)
	{
	RLoggingAllocator* dontCareResult = NULL;
	return New(aFlags, NULL, dontCareResult);
	}

EXPORT_C TInt RLoggingAllocator::New(TUint aFlags, RAllocator* aOrigAllocator, RLoggingAllocator*& aResult)
	{
	LOG("RLoggingAllocator::Install %x", aFlags);
	RLoggingAllocator* a = new RLoggingAllocator(aFlags);
	if (!a) return KErrNoMemory;

	TInt err = a->iLib.Load(_L("LoggingAllocator"));
	if (err)
		{
		LOG("LA: Failed to RLibrary load ourselves");
		a->Destroy();
		return err;
		}

	err = a->iLock.CreateLocal();
	if (err)
		{
		LOG("LA: Failed to create lock");
		a->Destroy();
		return err;
		}
	if (aFlags & EDeferFree)
		{
		err = a->iDeferredFrees.Reserve(KMaxDeferredFrees);
		if (err)
			{
			LOG("LA: No mem to reserve deferred free list");
			a->Destroy();
			return err;
			}
		}

	// Do this *before* switching the allocator, in case we're using atrace which means it will have to alloc for the first trace
	BTrace12(KHeapCategory, (TUint)ELoggingAllocatorInstalled, aOrigAllocator ? aOrigAllocator : &User::Allocator(), TUint(RThread().Id()), a->iPid);

	// Ditto
	err = a->iHelper.Open(aOrigAllocator ? aOrigAllocator : &User::Allocator());
	LOG("LA: RAllocatorHelper Open returned %d", err);

	RAllocator* old = aOrigAllocator;
	if (old == NULL)
		{
		// If the caller passed in an allocator, we shouldn't switch the default allocator
		old = User::SwitchAllocator(a);
		}
	a->iA = old;

	if (!err)
		{
		//err = a->DebugFunction(RHeap::EWalk, (TAny*)&TraceExistingAllocs, a);
		err = a->iHelper.Walk(TraceExistingAllocs, a);
		}
	LOG("LA: HeapWalk returned %d", err);
	if (err == KErrNone)
		{
		BTrace12(KHeapCategory, (TUint)EHeapPrimingFinished, a->iA, TUint(RThread().Id()), a->iPid);
		}
	a->iHelper.SetCellNestingLevel(a, -1); // This is so we are immune from being included in any leak detection (because we have to leak the allocator in order that that it is still in use when the markend happens)
	aResult = a;
	return KErrNone;
	}

EXPORT_C TInt RLoggingAllocator::Uninstall()
	{
	return User::Allocator().DebugFunction(KUidUninstallLoggingAllocator, NULL, NULL);
	}

void TraceAllocHeaven(TAny* aCell, TInt aLen)
	{
	TUint threadId = (TUint)RThread().Id().Id();
	BTrace12(BTrace::ETest1, EAllocHeaven, threadId, aCell, aLen);
	BTraceN(BTrace::ETest1, EAllocHeavenData, threadId, aCell, aCell, aLen);
	}

TBool RLoggingAllocator::HeavenWalk(RAllocatorHelper& aHelper, TAny* aPtr, RAllocatorHelper::TCellType aType, TLinAddr aCell, TInt aLen)
	{
	// If iA wasn't an RHeap (and more specifically, a UDEB RHeap) then we ensure this function does not get called. 
	// Therefore we can safely assume that it is

	// This function taken from RHeap::WalkCheckCell
	
	TInt nestingLevel = reinterpret_cast<TInt>(aPtr);

	switch(aType)
		{
		case RHeap::EGoodAllocatedCell:
			{
			TInt cellLevel;
			if (aHelper.GetCellNestingLevel((TAny*)aCell, cellLevel) == KErrNone && cellLevel == nestingLevel)
				{
				TraceAllocHeaven((TAny*)aCell, aLen);
				}
			break;
			}
		default:
			break;
		}
	return ETrue;
	}

_LIT(KPan, "LoggingAllocator");

void RLoggingAllocator::CheckDeferredFrees()
	{
	if ((iFlags & EDeferFreeAndCheckScribbles) != EDeferFreeAndCheckScribbles) return; // Both bits of EDeferFreeAndCheckScribbles must be set for us to be able to check

	iLock.Wait();
	const TInt n = iDeferredFrees.Count();
	for (TInt i = 0; i < n; i++)
		{
		const TUint* ptr = (const TUint*)iDeferredFrees[i].iPtr;
		const TUint* end = (const TUint*)((TLinAddr)ptr + Min(16, iDeferredFrees[i].iLen)); // Check the first few words
		if (iA->AllocLen(ptr) != iDeferredFrees[i].iLen)
			{
			// Then the length of the cell has been corrupted
			iLock.Signal();
			User::Panic(KPan, (TUint)ptr);
			}
		while (ptr != end)
			{
			//RDebug::Printf("Checking %x", ptr);
			if (*ptr != 0xDFDFDFDF)
				{
				// Someone is using this cell after it's been 'deleted'
				//TODO btrace this
				iLock.Signal();
				User::Panic(KPan, (TUint)iDeferredFrees[i].iPtr);
				}
			ptr++;
			}
		}
	iLock.Signal();
	}

RLoggingAllocator::~RLoggingAllocator()
	{
	// We must not be the current allocator when this is called!
	__ASSERT_ALWAYS(&User::Allocator() != this, User::Panic(KPan, 1));
	iLock.Close();
	iDeferredFrees.Close();
	iLib.Close();
	iHelper.Close();
	}

void RLoggingAllocator::Destroy()
	{
	// Keep the lib around until after the destructor has finished, otherwise you risk the DLL being unloaded midway through the destructor chain
	RLibrary lib = iLib;
	iLib.SetHandle(0);
	delete this;
	lib.Close();
	}

inline void RLoggingAllocator::TraceFree(RAllocator* aAllocator, TAny* aCellPtr, TBool aCellIsAlreadyFreed)
	{
	DoTraceAllocEvent(aAllocator, aCellPtr, EHeapFree, aCellIsAlreadyFreed ? -1 : 0);
	}

inline void RLoggingAllocator::TraceAlloc(RAllocator* aAllocator, TAny* aCellPtr, TInt aRequestedSize)
	{
	DoTraceAllocEvent(aAllocator, aCellPtr, EHeapAlloc, aRequestedSize);
	}

inline void RLoggingAllocator::TraceRealloc(TAny *aNewPtr, TAny* aOldPtr, TInt aNewRequestedSize)
	{
	DoTraceAllocEvent(iA, aNewPtr, EHeapReAlloc, aNewRequestedSize, aOldPtr);
	}
