/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* CMBufPoolManager decleration
* 
*
*/

/**
@file
@internalTechnology
*/

#ifndef MBUF_POOL_MANAGER_H
#define MBUF_POOL_MANAGER_H

#include <e32def.h>				// for std types
#include "MBufSizeAllocator.h"	// for MMBufSizeAllocator
#include "MBufManager.h"		// for CMBufManager

class CMBufPoolChain;
class CMBufPool;
NONSHARABLE_CLASS(CMBufPoolManager) : public CActive, public MMBufSizeAllocator
	{

public:
	static CMBufPoolManager* NewL(TInt aMaxAllocSize, CMBufManager& aMBufManager);
	virtual ~CMBufPoolManager();

	CMBufMemoryAllocator& Memory();
	CMBufManager&	Manager();
	void WaitForNewPoolRequest();
	TInt BackgroundAllocateNewPool(CMBufPoolChain& aPoolChain, TInt aNumMBufs);

	void AddL(TInt aMBufSize, TInt aInitialAllocation, TInt aMinGrowth, TInt aGrowthThreshold, TInt aPoolCeiling);
	TInt Commit();

	RMBuf* Alloc(TInt aSize, TInt aMinMBufSize, TInt aMaxMBufSize, TBool aIsAllocPool);	
	TInt AllocPool(CMBufPoolChain& aPoolChain, TInt aSize, TBool aIsSynchronous);
	void Free(RMBuf* aMBuf, TBool aIsPreAlloc);
	TUint LargestMBufSize() {return iLargestMBufSize;}
	TInt NextMBufSize(TInt aSize);
	TInt BytesAvailable();
	TInt BytesAvailable(TInt aSize);
	TInt BytesAllocated();
	
	TInt __DbgCheckBuffer(RMBuf* aBuf);
	TUint __DbgGetBufSpace();
	TUint __DbgGetBufSpace(TUint aMBufSize);
	TUint __DbgGetBufTotal();
	TUint __DbgGetBufTotal(TUint aMBufSize);
	RMBuf* __DbgMBufChain();
	RMBuf* __DbgMBufChain(TUint aMBufSize);
	void __DbgSetPoolLimit(TInt aCount);
	void __DbgSetPoolLimit(TInt aCount, TUint aMBufSize);
	
private:
	CMBufPoolManager(TInt aMaxAllocSize, CMBufManager& aMBufManager);
	void ConstructL(TInt aMaxAllocSize);

	virtual void DoCancel();
	virtual void RunL();
	
	void LinearSearchAlloc(RMBufQ& aList, TInt aSize, TInt aMinMBufSize, TInt aMaxMBufSize, TBool aIsAllocPool);

	virtual void CreatePool(CMBufPool* &aNewpool, CMBufPoolChain& aMBufPoolChain);

    RPointerArray<CMBufPoolChain> iPoolChains;

	RWorkerLock iPoolChainLock;
	RThread iMBufMgrOwnerThread;
	TBool iSignalled;
	TInt iNextMBufSize;				// Next requested growth request

	TInt iLargestMBufSize;

	TInt iMaxAllocSize;
	TInt iAllocSize;

	TBool iCommitted;

	CMBufManager& iMBufManager;		// for callback after freeing
	CMBufMemoryAllocator* iMemoryAllocator;
	
	};

#endif  // MBUF_POOL_MANAGER_H
