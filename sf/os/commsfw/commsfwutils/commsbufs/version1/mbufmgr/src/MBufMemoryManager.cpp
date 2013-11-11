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
// Heap Memory Allocation for CMBufPoolManager 
// 
//

/**
 @file
 @internalComponent
*/

#include <es_mbman.h>
#include "MBufMemoryManager.h"
#include "MBufPool.h"
#include "MBuf_Log.h" // for __FLOG_*

#ifdef __FLOG_ACTIVE
_LIT8(KComponent, "MemoryManager");
#endif

CMBufMemoryManager* CMBufMemoryManager::NewL(TInt aMaxHeapSize)
//
// Construction - first phase
//
	{
	CMBufMemoryManager* memAllocator = new(ELeave) CMBufMemoryManager;
	CleanupStack::PushL(memAllocator);
	memAllocator->ConstructL(aMaxHeapSize);
	CleanupStack::Pop();
	
	return memAllocator;
	}

CMBufMemoryManager::CMBufMemoryManager()
	: CBase()
//
// Constructor
//
	{
	__FLOG_OPEN(KSubsysMBufMgr, KComponent);
	__FLOG_1(_L8("CMBufManager %x:\tCMBufMemoryManager()"),this);

	}

void CMBufMemoryManager::ConstructL(TInt aMaxHeapSize)
//
// Construction - second phase
//
	{
	iHeap = UserHeap::ChunkHeap(NULL, 0, aMaxHeapSize);
	if (iHeap==NULL)
		{
		__FLOG_2(_L8("CMBufManager::CMBufMemoryManager %x:\tCreateL(0, aMaxHeapSize %d) leaving with KErrNoMemory"), this, aMaxHeapSize);
		User::Leave(KErrNoMemory);
		}
	}

CMBufMemoryManager::~CMBufMemoryManager()
//
// Destructor
//
	{
	iHeap->Close();

	__FLOG_CLOSE;
	}

TAny* CMBufMemoryManager::Alloc(TInt aSize)
//
// Allocates memory from the heap
//
	{
	TAny* pMemory;
	pMemory = iHeap->Alloc(aSize);

	return pMemory;
	}

void CMBufMemoryManager::Free(TAny* aPtr)
//
// Returns memory to the heap
//
	{
	CMBufPool* pool = (CMBufPool* ) aPtr;
	
	iHeap->Free(pool);
	}

TInt CMBufMemoryManager::AllocBytes()
//
// Number of bytes allocated from the heap
//
	{
	TInt allocated;
	iHeap->AllocSize(allocated);
	
	return allocated;
	}

