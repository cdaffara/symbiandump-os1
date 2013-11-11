// heaputils.h
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

#ifndef FSHELL_HEAP_UTILS_H
#define FSHELL_HEAP_UTILS_H

#include <e32cmn.h>

#ifdef __KERNEL_MODE__
class DThread;
class DChunk;
#else
class RMemoryAccess;
#endif // __KERNEL_MODE__

#if defined(STANDALONE_ALLOCHELPER) || defined(__KERNEL_MODE__)
#define HUIMPORT_C
#define HUCLASS(x) NONSHARABLE_CLASS(x)
#else
#define HUIMPORT_C IMPORT_C
#define HUCLASS(x) class x
#endif

namespace LtkUtils
	{

class THeapInfo;
	
HUCLASS(RAllocatorHelper) // class RAllocatorHelper
	{
public:
	HUIMPORT_C RAllocatorHelper();
#ifdef __KERNEL_MODE__
	TLinAddr GetKernelAllocator(DChunk* aKernelChunk);
	TInt OpenKernelHeap();
#else
	HUIMPORT_C TInt Open(RAllocator* aAllocator);
#endif
	HUIMPORT_C TInt SetCellNestingLevel(TAny* aCell, TInt aNestingLevel);
	HUIMPORT_C TInt GetCellNestingLevel(TAny* aCell, TInt& aNestingLevel);
	HUIMPORT_C TInt AllocCountForCell(TAny* aCell) const;
	HUIMPORT_C TLinAddr AllocatorAddress() const;
	HUIMPORT_C TInt RefreshDetails();
	
	HUIMPORT_C TInt CommittedSize();
	HUIMPORT_C TInt AllocatedSize();
	HUIMPORT_C TInt AllocationCount();
	HUIMPORT_C TInt MaxCommittedSize();
	HUIMPORT_C TInt MinCommittedSize();
	HUIMPORT_C TInt CountUnusedPages();
	HUIMPORT_C TInt CommittedFreeSpace();

	enum TCellType
		{
		EAllocation, EFreeSpace, EBadness
		};

	enum TExtendedCellType
		{
		EAllocationMask = 0xFF,
		EFreeMask = 0xFF00,
		EBadnessMask = 0xFF000000,

		EHeapAllocation = 1,
		EDlaAllocation = 2,
		EPageAllocation = 3,
		ESlabAllocation = 4,
		
		EHeapFreeCell = 0x0100,
		EDlaFreeCell = 0x0200,
		// There is nothing 'free' in the page allocator
		ESlabFreeCell = 0x0300, // Used to track free cells in partially-filled slabs
		ESlabFreeSlab = 0x0400, // Used to track entirely empty slabs (that don't have a specific cell size)

		EHeapBadFreeCellAddress = 0x01000000,
		EHeapBadFreeCellSize = 0x02000000,
		EHeapBadAllocatedCellSize = 0x03000000,
		EHeapBadAllocatedCellAddress = 0x04000000,
		};
			
	// TBool WalkFunc(TAny* aContext, TCellType aCellType, TLinAddr aCellPtr, TInt aCellLength)
	// aCellPtr points to the start of the cell payload for allocated cells (unlike RHeap's walker, which points to the cell header)
	// aCellLength is the payload length, ie what AllocLen(aCellPtr) would return
	// return ETrue to continue walking, EFalse to stop the walk
	typedef TBool (*TWalkFunc)(TAny*, TCellType, TLinAddr, TInt);
	typedef TBool (*TWalkFunc2)(RAllocatorHelper&, TAny*, TCellType, TLinAddr, TInt);
	typedef TBool (*TWalkFunc3)(RAllocatorHelper&, TAny*, TExtendedCellType, TLinAddr, TInt);
	HUIMPORT_C TInt Walk(TWalkFunc aCallbackFn, TAny* aContext);
	HUIMPORT_C TInt Walk(TWalkFunc2 aCallbackFn, TAny* aContext); // Like the other but the walk func gives you the RAllocatorHelper pointer too
	HUIMPORT_C TInt Walk(TWalkFunc3 aCallbackFn, TAny* aContext); // Like the other but the walk func gives you more details about the allocation type
	HUIMPORT_C TInt SizeForCellType(TExtendedCellType aType);
	HUIMPORT_C TInt CountForCellType(TExtendedCellType aType);
	HUIMPORT_C TBool AllocatorIsUdeb() const;
	HUIMPORT_C const TDesC& Description() const;
	HUIMPORT_C virtual void Close();

#ifdef __KERNEL_MODE__
	virtual DChunk* OpenUnderlyingChunk(); // Must be in CS
	enum TType
		{
		ETypeUnknown,
		ETypeRHeap,
		ETypeRHybridHeap,
		};
	TType GetType() const; // This is for information only, nothing should care about the return value
#endif

protected:
	TInt FinishConstruction();
	TInt IdentifyAllocatorType(TBool aAllocatorIsUdeb, TBool aIsTheKernelHeap=EFalse);
	TInt OpenChunkHeap(TLinAddr aChunkBase, TInt aChunkMaxSize);
#ifndef __KERNEL_MODE__
	static TInt EuserIsUdeb();
#endif
	virtual TInt ReadData(TLinAddr aLocation, TAny* aResult, TInt aSize) const;
	virtual TInt WriteData(TLinAddr aLocation, const TAny* aData, TInt aSize);

#ifndef __KERNEL_MODE__
protected:
#else
public:
#endif	
	virtual TInt TryLock();
	virtual void TryUnlock();

private:
	TInt ReadWord(TLinAddr aLocation, TUint32& aResult) const;
	TInt ReadByte(TLinAddr aLocation, TUint8& aResult) const;
	TInt WriteWord(TLinAddr aLocation, TUint32 aWord);
	TInt RefreshDetails(TUint aMask);
	TInt DoRefreshDetails(TUint aMask);
	TInt CheckValid(TUint aMask);
	TInt DoWalk(TWalkFunc3 aCallbackFn, TAny* aContext);
	TInt OldSkoolWalk(TWalkFunc3 aCallbackFn, TAny* aContext);
	TInt NewHotnessWalk(TWalkFunc3 aCallbackFn, TAny* aContext);
	static TBool DispatchClientWalkCallback(RAllocatorHelper& aHelper, TAny* aContext, TExtendedCellType aCellType, TLinAddr aCellPtr, TInt aCellLength);
	static TBool WalkForStats(RAllocatorHelper& aSelf, TAny* aContext, TExtendedCellType aType, TLinAddr aCellPtr, TInt aCellLength);
	TUint PageMapOperatorBrackets(unsigned ix, TInt& err) const;
	TInt PageMapFind(TUint start, TUint bit, TInt& err);
	TUint PageMapBits(unsigned ix, unsigned len, TInt& err);
	TUint PagedDecode(TUint pos, TInt& err);
	TInt TreeWalk(TUint32 aSlabRoot, TInt aSlabType, TWalkFunc3 aCallbackFn, TAny* aContext, TBool& shouldContinue);
protected:
	TLinAddr iAllocatorAddress;
	enum TAllocatorType
		{
		EUnknown,
		EAllocator,
		EUrelOldRHeap,
		EUdebOldRHeap,
		EUrelHybridHeap,
		EUdebHybridHeap,
		};
	TAllocatorType iAllocatorType;
private:
	THeapInfo* iInfo;
	TUint iValidInfo;
	TUint8* iTempSlabBitmap;
	mutable TAny* iPageCache;
	mutable TLinAddr iPageCacheAddr;
#ifdef __KERNEL_MODE__
	DChunk* iChunk;
	//TUint iSpare[0];
#else
	TUint iSpare[1];
#endif
	};

#ifdef __KERNEL_MODE__

HUCLASS(RUserAllocatorHelper) : public RAllocatorHelper
    {
public:
	RUserAllocatorHelper();
	TInt OpenUserHeap(TUint aThreadId, TLinAddr aAllocatorAddress, TBool aEuserIsUdeb);
	virtual DChunk* OpenUnderlyingChunk(); // Must be in CS
	virtual void Close();

protected:
	virtual TInt ReadData(TLinAddr aLocation, TAny* aResult, TInt aSize) const;
	virtual TInt WriteData(TLinAddr aLocation, const TAny* aData, TInt aSize);
	virtual TInt TryLock();
	virtual void TryUnlock();
private:
	DThread* iThread;
	};

HUCLASS(RKernelCopyAllocatorHelper) : public RAllocatorHelper
	{
public:
	RKernelCopyAllocatorHelper();
	TInt OpenCopiedHeap(DChunk* aOriginalChunk, DChunk* aCopiedChunk, TInt aOffset);
	virtual DChunk* OpenUnderlyingChunk(); // Must be in CS
	virtual void Close();

protected:
	virtual TInt ReadData(TLinAddr aLocation, TAny* aResult, TInt aSize) const;
	virtual TInt WriteData(TLinAddr aLocation, const TAny* aData, TInt aSize);
	virtual TInt TryLock();
	virtual void TryUnlock();
private:
	DChunk* iCopiedChunk;
	TInt iOffset; // from the original kernel heap to the copied heap
	};

#else

HUCLASS(RProxyAllocatorHelper) : public RAllocatorHelper
	{
public:
	HUIMPORT_C RProxyAllocatorHelper();
	HUIMPORT_C TInt Open(RMemoryAccess& aMem, TUint aThreadId);
	HUIMPORT_C TInt OpenChunkHeap(RMemoryAccess& aMem, TAny* aDChunkPtr);
	HUIMPORT_C virtual void Close();

protected:
	virtual TInt ReadData(TLinAddr aLocation, TAny* aResult, TInt aSize) const;
	virtual TInt WriteData(TLinAddr aLocation, const TAny* aData, TInt aSize);
	virtual TInt TryLock();
	virtual void TryUnlock();

private:
	RMemoryAccess* iMemoryAccess;
	TUint iThreadId;
	};

#endif // __KERNEL_MODE__

	} // namespace LtkUtils

#endif
