// tallochelper.cpp
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

#include <fshell/ioutils.h>
#include <fshell/common.mmh>
#include <fshell/heaputils.h>
#include <fshell/ltkutils.h>

using namespace IoUtils;
using LtkUtils::RAllocatorHelper;

class CCmdTAllocHelper : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdTAllocHelper();
private:
	CCmdTAllocHelper();
	void PrintStats(const TDesC& aDesc);

	void AllocL(TInt aSize, TInt aNumber=1);
	void Free(TInt aNumber=1);
	void FormatSize(TDes& aBuf, TInt64 aSize);
	static TBool WalkCallback(RAllocatorHelper& aAlloc, TAny* aContext, RAllocatorHelper::TExtendedCellType aType, TLinAddr aAddress, TInt aLength);
	static void RHeapWalkCallback(TAny* aContext, RHeap::TCellType aType, TAny* aCell, TInt aLength);

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RAllocatorHelper iAlloc;
	RArray<TAny*> iAllocations;
	TBool iBytes;
	TBool iWaitOnExit;

	TInt iAllocationsAccordingToRHeapWalker;
	};

EXE_BOILER_PLATE(CCmdTAllocHelper)

CCommandBase* CCmdTAllocHelper::NewLC()
	{
	CCmdTAllocHelper* self = new(ELeave) CCmdTAllocHelper();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdTAllocHelper::~CCmdTAllocHelper()
	{
	iAlloc.Close();
	Free(iAllocations.Count());
	iAllocations.Close();
	}

CCmdTAllocHelper::CCmdTAllocHelper()
	: CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdTAllocHelper::Name() const
	{
	_LIT(KName, "tallochelper");	
	return KName;
	}

const TDesC& CCmdTAllocHelper::Description() const
	{
	_LIT(KDescription, "Test code for RAllocatorHelper");
	return KDescription;
	}

void CCmdTAllocHelper::ArgumentsL(RCommandArgumentList& /*aArguments*/)
	{
	}

void CCmdTAllocHelper::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iBytes, 'b', _L("bytes"), _L("Display sizes in bytes rather than rounded off KB or MB"));
	aOptions.AppendBoolL(iWaitOnExit, 'w', _L("wait"), _L("Wait indefinitely before exiting - for testing remote heap walking"));
	}

void CCmdTAllocHelper::DoRunL()
	{
	LeaveIfErr(iAlloc.Open(&User::Allocator()), _L("Couldn't open RAllocatorHelper"));
	iAllocations.ReserveL(128);

	Printf(_L("Udeb allocator=%d\r\n"), iAlloc.AllocatorIsUdeb());

	PrintStats(_L("Initial stats"));
	// And do an RHeap walk, just to compare
	User::Allocator().DebugFunction(128, (TAny*)&RHeapWalkCallback, this);
	Printf(_L("allocated size according to RHeap::Walk: %d\r\n"), iAllocationsAccordingToRHeapWalker);

	AllocL(64*1024);
	PrintStats(_L("Stats after 1 64K alloc"));
	User::Allocator().DebugFunction(128, (TAny*)&RHeapWalkCallback, this); // debug


	AllocL(4, 50);
	PrintStats(_L("Stats after 50 small allocs"));

	/*
	// Open a new RAllocatorHelper cos the stats will be cached otherwise
	RAllocatorHelper h; h.Open(&User::Allocator());
	iAllocationsAccordingToRHeapWalker = 0;
	User::Allocator().DebugFunction(128, (TAny*)&RHeapWalkCallback, this);
	Printf(_L("allocated size according to RHeap::Walk: %d\r\n"), iAllocationsAccordingToRHeapWalker);
	Printf(_L("iTotalAllocSize is %d\r\n"), h.AllocatedSize());
	h.Close();
	*/

	if (!iWaitOnExit)
		{
		Free(50);
		PrintStats(_L("Stats after freeing them"));
		Complete();
		}
	}

void CCmdTAllocHelper::PrintStats(const TDesC& aDesc)
	{
	iAlloc.RefreshDetails();
	TBuf<16> allocSize, freeSize, dlaSize, slabSize, freeDlaSize, partiallyFreeSlabSize, freeSlabSize, pageSize;
	FormatSize(allocSize, iAlloc.SizeForCellType(RAllocatorHelper::EAllocationMask));
	FormatSize(freeSize, iAlloc.SizeForCellType(RAllocatorHelper::EFreeMask));
	FormatSize(dlaSize, iAlloc.SizeForCellType(RAllocatorHelper::EDlaAllocation));
	FormatSize(slabSize, iAlloc.SizeForCellType(RAllocatorHelper::ESlabAllocation));
	FormatSize(freeDlaSize, iAlloc.SizeForCellType(RAllocatorHelper::EDlaFreeCell));
	FormatSize(partiallyFreeSlabSize, iAlloc.SizeForCellType(RAllocatorHelper::ESlabFreeCell));
	FormatSize(freeSlabSize, iAlloc.SizeForCellType(RAllocatorHelper::ESlabFreeSlab));
	FormatSize(pageSize, iAlloc.SizeForCellType(RAllocatorHelper::EPageAllocation));

	Printf(_L("\r\n%S:\r\n\
Alloc: %d (%S), DLA: %d (%S) Slab: %d (%S) Paged: %d (%S)\r\n\
Free: %d (%S), DLA: %d (%S) Slab: %d (%S) Free slabs: %d (%S)\r\n"), 
		&aDesc, iAlloc.AllocationCount(), &allocSize,
		iAlloc.CountForCellType(RAllocatorHelper::EDlaAllocation), &dlaSize,
		iAlloc.CountForCellType(RAllocatorHelper::ESlabAllocation), &slabSize,
		iAlloc.CountForCellType(RAllocatorHelper::EPageAllocation), &pageSize,
		iAlloc.CountForCellType(RAllocatorHelper::EFreeMask), &freeSize,
		iAlloc.CountForCellType(RAllocatorHelper::EDlaFreeCell), &freeDlaSize,
		iAlloc.CountForCellType(RAllocatorHelper::ESlabFreeCell), &partiallyFreeSlabSize,
		iAlloc.CountForCellType(RAllocatorHelper::ESlabFreeSlab), &freeSlabSize
		);

	iAlloc.Walk(&WalkCallback, this); // Check cell lengths match up
	}

void CCmdTAllocHelper::AllocL(TInt aSize, TInt aNumber)
	{
	while (aNumber--)
		{
		TAny* result = User::AllocL(aSize);
		TInt err = iAllocations.Append(result);
		if (err)
			{
			User::Free(result);
			User::Leave(KErrNoMemory);
			}
		}
	}

void CCmdTAllocHelper::Free(TInt aNumber)
	{
	while (aNumber--)
		{
		TInt i = iAllocations.Count() - 1;
		User::Free(iAllocations[i]);
		iAllocations.Remove(i);
		}
	}

void CCmdTAllocHelper::FormatSize(TDes& aBuf, TInt64 aSize)
	{
	if (iBytes)
		{
		aBuf.Num(aSize);
		}
	else
		{
		LtkUtils::FormatSize(aBuf, aSize);
		}
	}

TBool CCmdTAllocHelper::WalkCallback(RAllocatorHelper& /*aAlloc*/, TAny* aContext, RAllocatorHelper::TExtendedCellType aType, TLinAddr aAddress, TInt aLength)
	{
	if (aType & RAllocatorHelper::EAllocationMask)
		{
		TInt lenFromAllocLen = User::AllocLen((TAny*)aAddress);
		if (lenFromAllocLen != aLength)
			{
			static_cast<CCmdTAllocHelper*>(aContext)->PrintWarning(_L("Walker reports cell 0x%08x size %d but AllocLen says %d\r\n"), aAddress, aLength, lenFromAllocLen);
			}
		}

	return ETrue; // Keep walking
	}

void CCmdTAllocHelper::RHeapWalkCallback(TAny* aContext, RHeap::TCellType aType, TAny* aCell, TInt aLength)
	{
	if (aType == RHeap::EGoodAllocatedCell)
		{
		CCmdTAllocHelper* self = static_cast<CCmdTAllocHelper*>(aContext);
		self->iAllocationsAccordingToRHeapWalker += aLength;
		}
	}
