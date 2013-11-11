// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32debug.h>
#include <e32base.h>
#include <e32math.h>
#include <s32file.h>
#include <bautils.h>
#include <hal.h>
#include <e32hal.h>
#include "graphicsmemoryhogger.h"

_LIT(KTGraphicsMemoryHoggerPanic, "TGfxMemHog");

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("Graphics Memory Hogger::E32Main - entry"));

	CTrapCleanup* TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, StartTestL());
	if (err)
		{
		User::Panic(KTGraphicsMemoryHoggerPanic,err);
		}

    delete TheTrapCleanup;

	RDebug::Print(_L("Graphics Memory Hogger::E32Main - exit"));
	return KErrNone;
	}

LOCAL_C void StartTestL(void)
	{
	RDebug::Print(_L("Graphics Memory Hogger::StartTestL - entry"));
	
	TMemoryInfoV1Buf membuf;
	UserHal::MemoryInfo(membuf);
	const TReal maxmem = membuf().iTotalRamInBytes;
	RDebug::Print(_L("GfxMemHog::TotalRamInBytes: %f"), maxmem);
	
	TInt pageSize = 0;
	HAL::Get(HAL::EMemoryPageSize, pageSize);
	RDebug::Print(_L("GfxMemHog::pageSize: %d"), pageSize);
	
	TInt maxHeap = 0;
	HAL::Get(HALData::EMemoryRAM, maxHeap);
	_LIT(KTGfxMemoryHogger, "GfxMemHog");

	RHeap* myHeap;
	myHeap = UserHeap::ChunkHeap(&KTGfxMemoryHogger, pageSize, maxmem);

	const TInt KOneSecond = 1000000;
	
	TInt newSize = pageSize;
	// allocate an initial cell with size equal to the page size
	TAny * myCell = myHeap->Alloc(newSize);
	TAny * myCell2 = NULL;
	
	// for each cycle increase the memory allocated to the cell until
	// the max is reached, at which point reset back to a cell of the
	// size of a page. A second loop is used to write to memory locations
	// in each page of the cell to ensure that the page is 'paged in'
	FOREVER
		{
		newSize += pageSize*10;
		TInt size = myHeap->Size();
		
		myCell2 = myHeap->ReAlloc(myCell, newSize);
		
		if (myCell2 == NULL)
			{
			RDebug::Print(_L("GfxMemHog::ERROR: size reached = %d"), size);
			newSize = pageSize; //reset
			myCell = myHeap->ReAlloc(myCell, newSize);
			
			if (myCell == NULL)
				{
				RDebug::Print(_L("GfxMemHog::ERROR: Could not ReAlloc(myCell)"));
				break;
				}
			}
		else
			{
			RDebug::Print(_L("GfxMemHog::myHeap.Size() = %d"), myHeap->Size());
			
			TInt index = 0;
			TInt stop = (myHeap->AllocLen(myCell) - pageSize)/4;
			RDebug::Print(_L("GfxMemHog::stop = %d"), stop);
			RDebug::Print(_L("GfxMemHog::myCell.AllocLen() = %d"), myHeap->AllocLen(myCell));
			TInt loopCount = 0;
			
			// write to each page of memory in the heap
			while(index<stop)
				{
				loopCount++;
				TInt * myData = ((TInt *) myCell2) + index;
				*myData = index;
				index += pageSize/4;
				}
			RDebug::Print(_L("GfxMemHog::Loop count = %d"), loopCount);			
			}
		
		User::After(KOneSecond);
		}
	
		
	RDebug::Print(_L("Graphics Memory Hogger::StartTestL - exit"));
	}

