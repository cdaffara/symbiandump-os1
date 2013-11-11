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
// Heap Memory Allocation for CMBufPoolAllocator 
// 
//

/**
 @file
 @internalComponent
*/

#include <es_mbman.h>
#include "MBufMemoryAllocator.h"
#include "MBufPool.h"
#include <cflog.h>

#ifdef __CFLOG_ACTIVE
__CFLOG_STMT(_LIT8(KComponent, "MemoryAllocator");)
__CFLOG_STMT(_LIT8(KSubsysMBufMgr, "MBufMgr");) // subsystem name
#endif

CMBufMemoryAllocator* CMBufMemoryAllocator::NewL(TInt aMaxHeapSize)
//
// Construction - first phase
//
	{
	CMBufMemoryAllocator* memAllocator = new(ELeave) CMBufMemoryAllocator;
	CleanupStack::PushL(memAllocator);
	memAllocator->ConstructL(aMaxHeapSize);
	CleanupStack::Pop();
	
	return memAllocator;
	}

CMBufMemoryAllocator::CMBufMemoryAllocator()
	: CBase()
//
// Constructor
//
	{
#ifdef __CFLOG_ACTIVE
	__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufManager %x:\tCMBufMemoryAllocator()"),this));
#endif
	}

void CMBufMemoryAllocator::ConstructL(TInt aMaxHeapSize)
//
// Construction - second phase
//
	{
	iHeap = UserHeap::ChunkHeap(NULL, 0, aMaxHeapSize);
	if (iHeap==NULL)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_2(KSubsysMBufMgr, KComponent, _L8("CMBufManager::CMBufMemoryAllocator %x:\tCreateL(0, aMaxHeapSize %d) leaving with KErrNoMemory"), this, aMaxHeapSize);
#endif
		User::Leave(KErrNoMemory);
		}
	}

CMBufMemoryAllocator::~CMBufMemoryAllocator()
//
// Destructor
//
	{
	iHeap->Close();
	}

TAny* CMBufMemoryAllocator::Alloc(TInt aSize)
//
// Allocates memory from the heap
//
	{
	TAny* pMemory;
	pMemory = iHeap->Alloc(aSize);

	return pMemory;
	}

void CMBufMemoryAllocator::Free(TAny* aPtr)
//
// Returns memory to the heap
//
	{
	CMBufPool* pool = (CMBufPool* ) aPtr;
	
	iHeap->Free(pool);
	}

TInt CMBufMemoryAllocator::AllocBytes()
//
// Number of bytes allocated from the heap
//
	{
	TInt allocated;
	iHeap->AllocSize(allocated);
	
	return allocated;
	}
