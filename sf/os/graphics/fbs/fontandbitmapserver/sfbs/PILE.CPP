// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hal.h>
#include <fbs.h>
#include <bitmap.h>
#include "UTILS.H"


GLREF_C void Panic(TFbsPanic aPanic);

LOCAL_C TInt GranularRoundUp(TInt aCount)
	{
	return (aCount + 7) & ~7;
	}

CChunkPile::CChunkPile(const RChunk& aChunk)
:	iChunk(aChunk)
	{}

EXPORT_C CChunkPile::~CChunkPile()
	{
	iChunk.Close();
	iSmallCells.Close();
	iFreeSmallCellLinks.Close();
	iLargeCells.Close();
	iFreeLargeCellLinks.Close();
	iLock.Close();
	}

EXPORT_C CChunkPile* CChunkPile::NewL(const RChunk& aChunk)
	{
	CChunkPile* thisptr=new(ELeave) CChunkPile(aChunk);
	CleanupStack::PushL(thisptr);
	thisptr->ConstructL();
	CleanupStack::Pop();
	return(thisptr);
	}

void CChunkPile::ConstructL()
	{
	TInt ret = iChunk.Duplicate(RThread());
	__ASSERT_ALWAYS(ret == KErrNone, Panic(EFbsPanicChunkError));
	HAL::Get(HAL::EMemoryPageSize, iPageSize);
	__ASSERT_ALWAYS(iPageSize > 4, Panic(EFbsPanicChunkError));
	iPageMask = iPageSize - 1; // assume page size is a power of 2
	__ASSERT_ALWAYS(KFbServLargeChunkMinPhysicalSize > 0, Panic(EFbsPanicChunkError));
	User::LeaveIfError(iChunk.Commit(iPageSize, KFbServLargeChunkMinPhysicalSize));
	iSmallCells.AppendL(iChunk.Base() + iPageSize); // prevent a data offset of 0 from chunk base
	iSmallCells.AppendL(iChunk.Base() + iPageSize + KFbServLargeChunkMinPhysicalSize);
	iFreeSmallCellLinks.AppendL(0);
	iLargeSectionBottom = iChunk.MaxSize() >> KFbServLargeChunkSizeShifter;
	iLargeCells.AppendL(iChunk.Base() + iLargeSectionBottom);
	iLargeCells.AppendL(iChunk.Base() + iChunk.MaxSize());
	iFreeLargeCellLinks.AppendL(0);
	User::LeaveIfError(iLock.CreateLocal());
	}

EXPORT_C TUint8* CChunkPile::ChunkBase() const
	{
	return(iChunk.Base());
	}

EXPORT_C TInt CChunkPile::VirtualSize()
	{
	TInt maxmem = 0;
	HAL::Get(HALData::EMemoryRAM, maxmem);
	ASSERT(maxmem > 0);
	return Min(Max(KFbServLargeChunkMinVirtualSize, maxmem << KFbServLargeChunkSizeShifter), KFbServLargeChunkMaxVirtualSize);
	}

EXPORT_C TUint8* CChunkPile::Alloc(TInt aSize)
	{
	__ASSERT_ALWAYS(aSize > 0 && aSize < KMaxTInt / 2, Panic(EFbsPanicChunkError));
	TUint8* cell;
	iLock.Wait();
	if (aSize < KMaxLargeBitmapAlloc)
		{
		aSize = Align4(aSize); // round size up to nearest four bytes
		TInt err = DoAlloc(cell, aSize, iSmallCells, iFreeSmallCellLinks, EFalse);
		if (err != KErrNone && cell != NULL)
			{
			// OOM after the small section grew? if so decommit as much physical memory as possible
			TInt topIndex = iSmallCells.Count() - 1;
			if (cell == iSmallCells[topIndex - 1])
				ShrinkSmallSection((iSmallCells[topIndex] - cell) & ~iPageMask);
			cell = NULL;
			}
		}
	else
		{
		aSize = (aSize + iPageMask) & ~iPageMask; // round size up to nearest page boundary
		TInt err = DoAlloc(cell, aSize, iLargeCells, iFreeLargeCellLinks, ETrue);
		if (err != KErrNone && cell != NULL)
			{
			iChunk.Decommit(cell - iChunk.Base(), aSize);
			cell = NULL;
			}
		}
	iLock.Signal();
	return cell;
	}

EXPORT_C void CChunkPile::Free(TAny* aCell)
	{
	iLock.Wait();
	if (static_cast<TUint8*>(aCell) < iChunk.Base() + iLargeSectionBottom)
		DoFree(static_cast<TUint8*>(aCell), iSmallCells, iFreeSmallCellLinks, EFalse);
	else
		DoFree(static_cast<TUint8*>(aCell), iLargeCells, iFreeLargeCellLinks, ETrue);
	iLock.Signal();
	}

TInt CChunkPile::DoAlloc(TUint8*& aCell, TInt aSize, RPointerArray<TUint8>& aCells, RArray<TInt>& aFreeCellLinks, TBool aLarge)
/*
This function tries to allocate a cell of aSize bytes and returns its address in aCell if successful.
In case of failure aCell is either NULL, if there is not enough memory for a cell of that size,
or it contains the address of a free cell of at least aSize bytes,
if there is not enough memory for the internal structures used by CChunkPile.
*/
	{
	__ASSERT_DEBUG(aCells.Count() >= 2, Panic(EFbsPanicChunkError));
	aCell = NULL;
	TInt cellIndex = 0;
	TInt freeCount = aFreeCellLinks.Count();
	for (TInt freeIndex = 0; freeIndex < freeCount; ++freeIndex)
		{
		TInt freeCellLink = aFreeCellLinks[freeIndex];
		cellIndex += freeCellLink;
		TInt size = aCells[cellIndex + 1] - aCells[cellIndex];
		if (size >= aSize)
			{
			if (aLarge)
				{
				TInt err = iChunk.Commit(aCells[cellIndex] - iChunk.Base(), aSize);
				if (err != KErrNone)
					return err;
				}
			aCell = aCells[cellIndex];
			if (size == aSize) // exact fit?
				{
				aFreeCellLinks.Remove(freeIndex);
				if (freeIndex < aFreeCellLinks.Count())
					aFreeCellLinks[freeIndex] += freeCellLink;
				}
			else
				{
				// make sure there are always enough empty slots in aFreeCellLinks
				TInt err = aFreeCellLinks.Reserve(GranularRoundUp((aCells.Count() + 1) >> 1));
				if (err != KErrNone)
					return err;
				err = aCells.Insert(aCell + aSize, cellIndex + 1);
				if (err != KErrNone)
					return err;
				++aFreeCellLinks[freeIndex];
				}
			return KErrNone;
			}
		}
	if (aLarge)
		return KErrNoMemory;
	TInt err = GrowSmallSection(aSize, cellIndex);
	if (err != KErrNone)
		return err;
	TInt topIndex = iSmallCells.Count() - 1;
	aCell = iSmallCells[topIndex - 1];
	if (iSmallCells[topIndex] - aCell == aSize) // exact fit?
		{
		iFreeSmallCellLinks.Remove(iFreeSmallCellLinks.Count() - 1);
		}
	else
		{
		// make sure there are always enough empty slots in aFreeCellLinks
		err = iFreeSmallCellLinks.Reserve(GranularRoundUp((iSmallCells.Count() + 1) >> 1));
		if (err != KErrNone)
			return err;
		err = iSmallCells.Insert(aCell + aSize, topIndex);
		if (err != KErrNone)
			return err;
		++iFreeSmallCellLinks[iFreeSmallCellLinks.Count() - 1];
		}
	return KErrNone;
	}

void CChunkPile::DoFree(TUint8* aCell, RPointerArray<TUint8>& aCells, RArray<TInt>& aFreeCellLinks, TBool aLarge)
	{
	TInt cellIndex = aCells.FindInAddressOrder(aCell);
	__ASSERT_DEBUG(cellIndex >= 0 && cellIndex < aCells.Count() - 1, Panic(EFbsPanicChunkError));
	if (cellIndex < 0 || cellIndex == aCells.Count() - 1)
		return;
	TInt prevIndex = KMaxTInt;
	TInt nextIndex = 0;
	TInt freeCount = aFreeCellLinks.Count();
	TInt freeIndex;
	for (freeIndex = 0; freeIndex < freeCount; ++freeIndex)
		{
		nextIndex += aFreeCellLinks[freeIndex];
		if (nextIndex >= cellIndex)
			{
			__ASSERT_DEBUG(nextIndex > cellIndex, Panic(EFbsPanicChunkError));
			if (nextIndex == cellIndex)
				return;
			break;
			}
		prevIndex = nextIndex;
		}
	if (aLarge)
		iChunk.Decommit(aCell - iChunk.Base(), aCells[cellIndex + 1] - aCell);
	if (prevIndex == cellIndex - 1 && nextIndex == cellIndex + 1)
		{
		aFreeCellLinks.Remove(freeIndex);
		aCells.Remove(cellIndex + 1);
		aCells.Remove(cellIndex--);
		}
	else if (prevIndex == cellIndex - 1)
		{
		if (freeIndex < freeCount)
			--aFreeCellLinks[freeIndex];
		aCells.Remove(cellIndex--);
		}
	else if (nextIndex == cellIndex + 1)
		{
		--aFreeCellLinks[freeIndex];
		aCells.Remove(cellIndex + 1);
		}
	else
		{
		if (freeIndex < freeCount)
			aFreeCellLinks[freeIndex] = nextIndex - cellIndex;
		aFreeCellLinks.Insert(freeIndex > 0 ? cellIndex - prevIndex : cellIndex, freeIndex); // can't fail
		}
	if (aLarge)
		return;
	TInt topIndex = aCells.Count() - 1;
	if (cellIndex == topIndex - 1)
		{
		// last cell is free and has just grown, shrink with granularity iPageSize << KFbServLargeChunkGrowByShifter
		TInt roundMask = (iPageSize << KFbServLargeChunkGrowByShifter) - 1;
		ShrinkSmallSection((aCells[topIndex] - aCells[cellIndex]) & ~roundMask);
		}
	}

TInt CChunkPile::GrowSmallSection(TInt aSize, TInt aLastFreeCell)
/*
This function tries to grow the section for small cells with granularity iPageSize << KFbServLargeChunkGrowByShifter.
It appends or expands the last free cell so that it has at least aSize bytes if successful.
aLastFreeCell must be the index of the last free cell in the small section or 0 if there is none.
*/
	{
	TInt roundMask = (iPageSize << KFbServLargeChunkGrowByShifter) - 1;
	TInt topIndex = iSmallCells.Count() - 1;
	TInt maxGrowBy = (iChunk.Base() + iLargeSectionBottom) - iSmallCells[topIndex];
	__ASSERT_DEBUG(maxGrowBy >= 0, Panic(EFbsPanicChunkError));
	if (iFreeSmallCellLinks.Count() > 0 && aLastFreeCell == topIndex - 1)
		{
		// last cell is free
		TInt growBy = (iSmallCells[aLastFreeCell] + aSize) - iSmallCells[topIndex];
		__ASSERT_DEBUG(growBy > 0, Panic(EFbsPanicChunkError));
		if (growBy > maxGrowBy)
			return KErrNoMemory;
		growBy = Min((growBy + roundMask) & ~roundMask, maxGrowBy);
		TInt err = iChunk.Commit(iSmallCells[topIndex] - iChunk.Base(), growBy);
		if (err != KErrNone)
			return err;
		iSmallCells[topIndex] += growBy;
		}
	else
		{
		// last cell is not free
		if (aSize > maxGrowBy)
			return KErrNoMemory;
		TInt growBy = Min((aSize + roundMask) & ~roundMask, maxGrowBy);
		TInt err = iSmallCells.Reserve(GranularRoundUp(iSmallCells.Count() + 1));
		if (err != KErrNone)
			return err;
		err = iFreeSmallCellLinks.Reserve(GranularRoundUp((iSmallCells.Count() + 1) >> 1));
		if (err != KErrNone)
			return err;
		err = iChunk.Commit(iSmallCells[topIndex] - iChunk.Base(), growBy);
		if (err != KErrNone)
			return err;
		iSmallCells.Append(iSmallCells[topIndex] + growBy); // can't fail
		iFreeSmallCellLinks.Append(topIndex - aLastFreeCell); // can't fail
		}
	return KErrNone;
	}

void CChunkPile::ShrinkSmallSection(TInt aShrinkBy)
	{
	TInt topIndex = iSmallCells.Count() - 1;
	TInt maxShrinkBy = (iSmallCells[topIndex] - iChunk.Base()) - (KFbServLargeChunkMinPhysicalSize + iPageSize);
	__ASSERT_DEBUG(maxShrinkBy >= 0, Panic(EFbsPanicChunkError));
	aShrinkBy = Min(aShrinkBy, maxShrinkBy);
	if (aShrinkBy > 0)
		{
		iChunk.Decommit((iSmallCells[topIndex] -= aShrinkBy) - iChunk.Base(), aShrinkBy);
		if (iSmallCells[topIndex] == iSmallCells[topIndex - 1])
			{
			iSmallCells.Remove(topIndex);
			iFreeSmallCellLinks.Remove(iFreeSmallCellLinks.Count() - 1);
			}
		}
	}
