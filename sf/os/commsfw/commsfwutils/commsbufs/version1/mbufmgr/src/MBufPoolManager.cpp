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
// CMBufPoolManager.cpp
// Maintains one or more pool chains (RMBufPoolChain) on behalf of CMBufManager
// 
//

/**
 @file
 @internalComponent
*/

#include "ES_MBMAN.H"
#include <comms-infras/cfmacro.h>
#include "MBufPoolManager.h"
#include "MBufPoolChain.h"
#include "MBufPool.h"
#include "MBufMemoryAllocator.h"
#include <cflog.h>

static const TInt KMemoryAlignment = 8;
const TInt KNoOutstandingRequests = -1;

/** Patchable constant detailing the size of cache lines in the target architecture.
    The MBufMgr will align data to cache lines.
*/
EXPORT_C extern const TUint KMBufCacheLineSize = 32;

#define ALIGN_CACHE_MASK    (KMBufCacheLineSize-1)
#define ALIGN_CACHE_UP(n)   (((n)+(ALIGN_MASK)) & ~ALIGN_MASK)
#define ALIGN_CACHE_DOWN(n) ((n) & ~ALIGN_MASK)
#define IS_CACHE_ALIGNED(p) ((((TUint32)(p)) & ALIGN_MASK)==0)

#ifdef __CFLOG_ACTIVE
__CFLOG_STMT(_LIT8(KComponent, "PoolManager");)
__CFLOG_STMT(_LIT8(KSubsysMBufMgr, "MBufMgr");) // subsystem name
__CFLOG_STMT(_LIT8(KComponentPerformance, "performance");)	// component name - used for any sub-optimal performance behaviuor
#endif

CMBufPoolManager* CMBufPoolManager::NewL(TInt aMaxAllocSize, CMBufManager& aMBufManager)
	{
	CMBufPoolManager* This = new(ELeave) CMBufPoolManager(aMaxAllocSize, aMBufManager);
	CleanupStack::PushL(This);
	This->ConstructL(aMaxAllocSize);
	CleanupStack::Pop(This);
	return This;
	}

// ctor
CMBufPoolManager::CMBufPoolManager(TInt aMaxAllocSize, CMBufManager& aMBufManager)
	: CActive(EPriorityStandard),
	  iMaxAllocSize(aMaxAllocSize),
	  iMBufManager(aMBufManager)
	{
#ifdef __CFLOG_ACTIVE
	__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager %x:\tCMBufPoolManager()"), this));
#endif

	iCommitted = EFalse;
	iNextMBufSize = KNoOutstandingRequests;
	}

void CMBufPoolManager::ConstructL(TInt aMaxAllocSize)
	{
	User::LeaveIfError(iPoolChainLock.CreateLocal());
	iMBufMgrOwnerThread.Duplicate(RThread());

	iMemoryAllocator = CMBufMemoryAllocator::NewL(aMaxAllocSize);

	CActiveScheduler::Add(this);

	WaitForNewPoolRequest();
	}

CMBufPoolManager::~CMBufPoolManager()
	{
	// Cancel any outstanding requests
	Cancel();

	for (TInt i = 0; i < iPoolChains.Count(); i++)
		{
		iPoolChains[i].Close();
		}

	iPoolChains.Close();

	delete iMemoryAllocator;

    iPoolChainLock.Close();
	iMBufMgrOwnerThread.Close();
	}

CMBufMemoryAllocator& CMBufPoolManager::Memory()
	{
	return *iMemoryAllocator;
	}

void CMBufPoolManager::WaitForNewPoolRequest()
	{
	__ASSERT_DEBUG(!IsActive(), CMBufManager::Panic(EMBuf_AlreadyActive));
	iStatus = KRequestPending;
	SetActive();
	}

void CMBufPoolManager::DoCancel()
	{
	NETWORKING_ATOMIC(;) // Codepattern below may have SMP implications
	if(!iSignalled)
		{
		// We're almost certainly the only thread doing this; now check again inside the critsec to be sure
		iPoolChainLock.Wait();
		if(!iSignalled)
			{
			iSignalled = ETrue;
			TRequestStatus* pStatus = &iStatus;
			User::RequestComplete(pStatus, KErrCancel);
			}
		iPoolChainLock.Signal();
		}
	}

TInt CMBufPoolManager::BackgroundAllocateNewPool(RMBufPoolChain& aPoolChain,
					TInt aNumMBufs)
	{

	// Build a list of MBuf sizes and allow one request per MBuf pool size....
	// Note its essential that at least one request is serviced, the allocation can
	// then attempt to use the new MBufs created.  Its not essential that every
	// request is serviced, its more important to prevent this single new pool
	// allocation routine from locking out processing on the many threads that use
	// this routine for a long time

	// The first attempted growth by aNumMBufs wins
	NETWORKING_ATOMIC(;) // Codepattern below may have SMP implications
	if (!aPoolChain.iSignalled)
		{

		// This is being most likely being called from the context of some thread other than the
		// MBufMgr owner, and there's a distinct risk of more than one thread being OOB at the same
		// time since it's a common PoolChain. 	So we have to lock here to process this PoolChain
		iPoolChainLock.Wait();

		// We're almost certainly the only thread doing this; now check again inside the critsec to be sure
		if (!aPoolChain.iSignalled)
			{
			// First do a simple test to see if the new pool will be created
			// Note in the future when CMBufPool deletion is implemented iNoMoreSpace
			// should be cleared on all pools to allow growth again.
			if (aPoolChain.iNoMoreSpace)
				{
				if (aPoolChain.iLastGrowth > aNumMBufs)
					{
					// Try this smaller allocation
					aPoolChain.iNoMoreSpace = EFalse;
					}
				else
					{
					iPoolChainLock.Signal();
					return KErrNoMBufs;
					}
				}
			aPoolChain.iLastGrowth = aNumMBufs;
			aPoolChain.iNumMBufsToAdd = aNumMBufs;
			aPoolChain.iSignalled = ETrue;
			}

		}
	else
		{
		// Its possible that first time round we missed this growth attempt
		if (iSignalled)
			{
			// already signalled so just return
			return KErrNone;
			}
		// already signalled but the first time round the growth attempt was missed
		// this is always possible because to stop the background thread slowing the
		// these threads we dont lock in the RunL.  The RunL is always run in the same
		// thread so does not need a lock.
		iPoolChainLock.Wait();
		}

	// Here we have to guard against being completed multiple times because more than
	// one pool chain can be growing at the same time
	if (!iSignalled)
		{
		iSignalled = ETrue;

		// iNextMBufSize is the first freepool for a growth attempt
		iNextMBufSize = aPoolChain.iMBufSize;

		TRequestStatus* pStatus = &iStatus;

		// We dont need to hold onto the lock now since there is no chance
		// that the RequestComplete can be called twice
		iPoolChainLock.Signal();

		iMBufMgrOwnerThread.RequestComplete(pStatus, KErrNone);
		return KErrNone;
		}

	iPoolChainLock.Signal();
	return KErrNone;
	}

void CMBufPoolManager::RunL()
	{
	// Now that we're in the thread that owns pools and ultimately the
	// memory allocation so attempt to create the new pool

	RMBufPoolChain* selectedPoolChain(NULL);
	TInt growThisPoolChain = iNextMBufSize;
	iNextMBufSize = KNoOutstandingRequests;

	// There is a chance that there are other pools to grow as well
	// looking for them now can save future locking and speed allocation.

	// Note we dont have to catch every growth attempt because background
	// allocation should be triggered from an early threshold.  This allocation
	// might satisfy the immediate growth requirement so no need to lock.

	// Ensure that each time RunL is called a different pool growth attempt
	// by stepping through the available pools looking for the next request.
	TInt poolChainCount = iPoolChains.Count();
	TInt lowestRequest = KNoOutstandingRequests;

	for (TInt i = 0; i < poolChainCount; i++)
		{
		RMBufPoolChain* poolInfo = &iPoolChains[i];
		TInt thisMBufSize = poolInfo->iMBufSize;

		// locate this growth attempt
		if (thisMBufSize == growThisPoolChain)
			{
			selectedPoolChain = poolInfo;
			}

		if (poolInfo->iSignalled)
			{
			if (lowestRequest == KNoOutstandingRequests)
				{
				lowestRequest = thisMBufSize;
				}
			if ( (iNextMBufSize == KNoOutstandingRequests) &&
					(thisMBufSize > growThisPoolChain) )
				{
				// found the next freepool to grow
				iNextMBufSize = thisMBufSize;
				break;
				}
			}
		}

	// Might have wrapped round all the PoolChains
	if ( (iNextMBufSize == KNoOutstandingRequests)
			&& (lowestRequest != growThisPoolChain) )
		{
		iNextMBufSize = lowestRequest;
		}

	// Now prepare for the next pool growth if any
	if (iNextMBufSize != KNoOutstandingRequests)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::RunL() - One of several pool growth attempts")));
#endif

		// Still more requests outstanding
		// iSignalled = ETrue;
		iStatus = KRequestPending;

		SetActive();

		// complete the request so we get called again
		TRequestStatus* pStatus = &iStatus;
		User::RequestComplete(pStatus, KErrNone);
		}
	else
		{
		// There are no more outstanding requests so prepare for next
		// pool creation request
		iStatus = KRequestPending;

		// open the door for more allocate requests on other pool chains
		iSignalled = EFalse;
		SetActive();
		}

	// No need to surround the pool creation with a lock
	// allocate a pool & associate with the pool chain
	__ASSERT_DEBUG(selectedPoolChain != NULL, CMBufManager::Panic(EMBuf_NoPoolChain));
	CMBufPool* newpool = NULL;
	CreatePool(newpool, *selectedPoolChain);
	if (newpool == NULL)
    	{
    	// Pool create fails perhaps due to insufficient memory

		// Each buffer size gets its own out of space indicator
    	selectedPoolChain->iNoMoreSpace = ETrue;
    	}
	else
		{
		// add the pool to the pool list
		// No need to lock if synchronous allocation is removed
		iPoolChainLock.Wait();
		selectedPoolChain->iPools.AddLast(*newpool);
		iPoolChainLock.Signal();

   		// first grab the freelist lock
		selectedPoolChain->iLockFreeList.Wait();
		selectedPoolChain->ExtendFreeList(newpool);
		selectedPoolChain->iLockFreeList.Signal();
		}

	// open the door for more allocate requests on this poolchain
	selectedPoolChain->iSignalled = EFalse;

	}


TInt CMBufPoolManager::Commit()
/**
Ensures that no additional pool chains can be created (ie. via MMBufSizeAllocator::AddL())
Calling this method is useful (but not mandated) to ensure that operations within MMBufSizeAllocator::AddL do not create any adverse affects

@return KErrNone or KErrAlreadyExists if already called.
*/
	{
	TInt result = KErrAlreadyExists;
	if (iCommitted == EFalse)
		{
		iCommitted = ETrue;
		result = KErrNone;
		}
	return result;
	}

// create a pool chain with the specified values
// - Deliberately have not exposed the concept of a pool or chain in the api, as these concepts are intended to be hidden within MBM
//   as a private implementation detail.
// - Mbuf size guidelines;
//   1. 128 byte is mandatory for some (poorly writen) legacy code that assumes the existance of mbufs of this size.
//   b. < 128 byte is acceptable, but could break some (poorly written) legacy code that assumes that minimum mbuf size is 128
// - 128 byte guideline is due to K_MBufSmallSize definition having been made public;
//   a. Externally - numerous instances within our codebase (and thus similar assumptions are likely within licensee code) that assume
//		             mbufs of at least this size are available for allocation.
//   b. Internally - When the allocation size is not specified, the value is defaulted to K_MBufSmallSize.  Refer ::AllocL notes.
//                 - RMBufCells are assumed to be no larger than K_MBufSmallSize.
//   Since the first point can't be fixed without a source break, there is little point in not creating a default pool chain of
//   K_MBufSmallSize size in order to rectify the assumptions made within the other points.
void CMBufPoolManager::AddL(TInt aMBufSize, TInt aInitialAllocation, TInt aMinGrowth, TInt aGrowthThreshold)
/**
Populate a free pool of specified size with RMBuf's.  Each RMBuf in this free pool will have the same size.

RMbuf size guidelines:
At least one free pool with 128 byte RMBuf's is mandatory for some legacy code that assumes the existance of mbufs of this size.
RMBuf size < 128 byte is acceptable, but could break some legacy code that assumes that minimum mbuf size is 128
These 128 byte guideline are due to KMBufSmallSize definition having been made public.  Ideally, this constant should not be published.
Instead, inspect the available sizes via RMBufAllocator::NextMBufSize() or the size of this RMBuf with RMBuf::Size().
DO NOT ASSUME UNDER ANY CIRCUMSTANCES THAT ALL MBUFS ARE THE SAME SIZE!

@param aMBufSize the size in bytes of the RMBuf's created.
@param aInitialAllocation the number of RMBuf's in the initial free pool.
@param aMinGrowth the number of RMBuf's added to the free pool.
@param aGrowthThreshold when the free pool has less than this number of RMBuf's it will trigger allocation of more memory for more RMBuf's.
*/
	{
	// check args
	// - on the surface it makes sense to declare the args as unsigned and rely on the compiler's type checking instead of writing code
	//   to manually check (which has an inherit performance penalty), but as requested this was deliberately not done because;
	//   a. concerns about strict compilers which warn/error arithmetic operations with literals that are not explicitly designeated as unsigned
	//      eg. 1U versus 1
	//   b. encourages type casting by user to avoid warnings, which circumvents useful compiler type checking & often leads to unexpected results
	//	    eg. type casting -1 to unsigned using a 2's compliment compiler resulting in an unexpectedingly large allocation of 2GB
	__ASSERT_ALWAYS(aMBufSize > 0, CMBufManager::Panic(EMBuf_PoolManager_NegativeMBufSize));
	__ASSERT_ALWAYS(aInitialAllocation >= 0, CMBufManager::Panic(EMBuf_PoolManager_NegativePoolSize));
	__ASSERT_ALWAYS(aMinGrowth >= 0, CMBufManager::Panic(EMBuf_PoolManager_NegativeMinGrowth));
	__ASSERT_ALWAYS(aGrowthThreshold >= 0, CMBufManager::Panic(EMBuf_PoolManager_NegativeGrowthThreshold));

	// ensure we are not already committed - refer CMBufPoolManager::Commit notes
	if (iCommitted)
		{
		User::Leave(KErrNotReady);
		}

	TInt alignmentPadding = aMBufSize % KMemoryAlignment;
	if (aMBufSize == 0)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::AddAllocSizeL() - Warning! requested mbufSize=0 serves no useful purpose; accepted anyhow")));
#endif
		}
	else if (alignmentPadding != 0)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::AddAllocSizeL() - Warning! requested mbufSize=%d is not (armv5) dword aligned, size has been increased to %d"),
			aMBufSize, aMBufSize + KMemoryAlignment - alignmentPadding));
#endif
		aMBufSize += KMemoryAlignment - alignmentPadding;
		}
	if ((aInitialAllocation * aMBufSize) > iMaxAllocSize)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::AddAllocSizeL() - Warning! requested initialAllocation=%d exceeds the maxAllocSize=%d; accepted anyhow"),
			(aInitialAllocation * aMBufSize), iMaxAllocSize));
#endif
		}
	if (aMinGrowth == 0)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::AddAllocSizeL() - Warning! requested minGrowth=0 increased to the minimum value of 1")));
#endif
		aMinGrowth = 1;
		}
	else if ((aMinGrowth * aMBufSize) > iMaxAllocSize)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::AddAllocSizeL() - Warning! requested minGrowth=%d exceeds the maxAllocSize=%d; accepted anyhow"),
			(aMinGrowth * aMBufSize), iMaxAllocSize));
#endif
		}
	if (aGrowthThreshold > aMinGrowth)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::AddAllocSizeL() - Warning! requested growthThreshold=%d exceeds minGrowth=%d, hence potentially requiring multiple/iterative growth allocation; accepted anyhow"),
			aGrowthThreshold, aMinGrowth));
#endif
		}

	// check pool chain already exists
	TInt index = 0;
	TInt found = KErrNone;
	if (found == KErrNone)
		{
		RMBufPoolChain poolChain(aMBufSize, *this);
		found = iPoolChains.FindInOrder(poolChain, index, TLinearOrder<RMBufPoolChain>(RMBufPoolChain::Compare));
		}
	if (found == KErrNotFound)
		{
		// create a new pool chain
		RMBufPoolChain poolChain1(aMBufSize, aInitialAllocation, aMinGrowth, aGrowthThreshold, *this);
		iPoolChains.InsertL(poolChain1, index);

		RMBufPoolChain* poolChain = &iPoolChains[index];

		// Resets pool since it iNext & iPrev members of TDblQueLinkBase still point to the memory locations before Insert
		poolChain->OpenL();

		// pre-allocate the initial pool
		if (aInitialAllocation > 0)
			{
			// Lock the freelist so that the MBufs can be added to the freepool
			poolChain->iLockFreeList.Wait();

			// Use synchronous allocate to ensure that the initial pool allocation gives immediate feedback
			// to the rootserver or calling method.
			// This can only be called from the owner thread for CMBufManager
			// Thus synchronous AllocPool is possible because no conflict with asynchronous
			// background AllocPool occurs because this also is run from the owner thread
			// for the CMBufManager.  Synchronous AllocPool is not allowed from other threads

			TInt res = AllocPool(*poolChain, aInitialAllocation, ETrue);
			poolChain->iLockFreeList.Signal();
			User::LeaveIfError(res);
			}
		}
	else
		{
		RMBufPoolChain* poolChain = &iPoolChains[index];
		// update existing chain's values - these will take affect the next time they are used (eg. next allocation)
		poolChain->iMinGrowth = aMinGrowth;
		poolChain->iGrowthThreshold = aGrowthThreshold;
		}

	// maintain/update the largest mbuf size - for background allocation
    if (iLargestMBufSize < aMBufSize)
    	{
		iLargestMBufSize = aMBufSize;
    	}
	}

void CMBufPoolManager::CreatePool(CMBufPool* &aNewpool, RMBufPoolChain& aMBufPoolChain)
	{
	aNewpool = CMBufPool::New(aMBufPoolChain.iMBufSize
							, aMBufPoolChain.iNumMBufsToAdd
							, aMBufPoolChain
							, *iMemoryAllocator);
	}

// Search algorithm to find the pool chain satisfying the request size
// Algorithm first narrows down to pools chains with free mbufs to satisfy the request size
// Then, searches through the remaining pool chains starting from the largest possible pool chain
// towards the smallest pool chain until the whole request is satisfied
void CMBufPoolManager::LinearSearchAlloc(RMBufQ& aList, TInt aSize
					, TInt aMinMBufSize, TInt aMaxMBufSize, TBool aIsAllocPool)
	{
	// Check to see if an empty mbuf with zero length is requested
	TBool zeroLengthMBuf = (aSize == 0);

	TInt poolChainMax = iPoolChains.Count() - 1;

	RMBufPoolChain* start = &iPoolChains[0];
	RMBufPoolChain* chain = &iPoolChains[poolChainMax];
	RMBufPoolChain* chosen = NULL;

	// First narrow the pool chains to those that are acceptable
	// Each pool chain represents a free list of mbufs of a particular size
	// The mbuf size must be within aMinMBufSize - aMaxMBufSize range
	// The free pool must have space
	do
		{
		// Look for the free pool with the largest mbufs
		if ( (chain->iMBufSize <= aMaxMBufSize) &&
				(chain->iLenFree > 0) )
			{
			// Check that the chosen free pool is within the limits set
			if (chain->iMBufSize >= aMinMBufSize)
				{
				// This pool chain has space so found the largest mbufs
				chosen = chain;
				break;
				}
			// If reached this point: no pool chain to fulfil the request
			return;
			}
		}
	while (chain-- != start);

	// Check to see a if there is a pool chain to fulfil the request
	if (chosen == NULL)
		{
		return;
		}

	TInt allocated = 0;
	TInt extraMem = 0;

	// Now find the best free pool and allocate the memory
	do
		{
		// At first iteration, this check is false since chain == chosen
		// and chosen is greater than aMinMBufSize
		// Check if chain (iterator - smaller than chosen pool chain)
		// is smaller than requested min size
		if  (chain->iMBufSize < aMinMBufSize)
			{
			break;
			}

		// Test to see if more than one mbuf would be used to
		// satisfy the request
		if ( (chain->iMBufSize < aSize) &&
				(chain->iLenFree > 0) )
			{
			// Check if chosen is available, just to be secure
			// Or we have a chosen chain which could complete in 1 mubuf, so choose it
			if (chosen == NULL || chosen != chain)
				{
				break;
				}

			// Chosen is the next largest mbuf size so this allocation
			// can be satisfied with the chosen free pool
			allocated = chosen->Alloc(aList, aSize, aIsAllocPool);

			// Check if the allocation is complete
			if (allocated >= aSize)
				{
				// Test if no smaller chains
				// or no need to check for smaller
				if ((chain == start) || (allocated == aSize))
					{
					// Allocation is complete
					aSize = 0;
					chosen = NULL;
					break;
					}

				// By swapping the first mbuf with a smaller mbuf it might
				// be possible to fit the remainder into a smaller mbuf that
				// wastes less memory
				extraMem = chosen->iMBufSize + aSize - allocated;

				// Check to see if the remainder can fit into a smaller mbuf
				if ( extraMem <= (chain - 1)->iMBufSize && (chain - 1)->iMBufSize >= aMinMBufSize )
					{
					// It can fit
					// Set request size to this remaining size so that the algorithm
					// will continue searching for this remaining size
					aSize = extraMem;
					// Set chosen to null so that when the search cannot find a
					// smaller mbuf for remaining size the algorithm will know this
					chosen = NULL;
					// No break - continue searching for remainder
					}
				else
					{
					// Remainder is large to fit in a smaller mbuf
					// Allocation is complete
					aSize = 0;
					chosen = NULL;
					break;
					}
				}
			else
				{
				// No space left in the free pool so look for
				// a smaller mbuf to satisfy the request
				aSize -= allocated;
				}
			}

		// Best free pool not found yet
		// Test to see if this free pool is a better choice
		if ( (aSize <= chain->iMBufSize) &&
				(chain->iLenFree > 0) )
			{
			chosen = chain;
			}
		}
	while (chain-- != start);

	// There is a chosen pool but the request is not satisfied yet due to:
	// 1. Requested size is smaller than the smallest pool chain
	// 2. Most suitable pool chain has no free space
	// 3. An empty mbuf with zero length is requested
	// 4. Request size was assigned with remaining size and a pool chain is found
	//    for that remaining size
	if ((chosen != NULL) && ((aSize > 0) || (zeroLengthMBuf)))
		{
		// This chosen free pool will have to do
		RMBufQ bList;
		TInt allocated = chosen->Alloc(bList, aSize, aIsAllocPool);
		// Test for successful allocation
		if (allocated >= aSize)
			{
			// If this allocation is for remaining size swap the extra mbuf with
			// the allocated one
			if (extraMem > 0)
				{
				// Allocation worked so swap this smaller mbuf thus saving space
				RMBuf* first = aList.Remove();
				// Free the first mbuf
				Free(first, ETrue);
				}
			aList.Append(bList);
			aSize = 0;
			}
		}

	// If no chosen pool for remaining size but the allocation was successful
	if (extraMem > 0)
		{
		// Allocation is complete
		aSize = 0;
		}

	// If allocated mbuf is insufficient to fulfill the req empty the list
	if (aSize > 0)
		{
		Free(aList.First(), ETrue);
		aList.Init();
		}
	}

// Finds poolchains that will satisfy the request size using linear search,
// and allocates and initialises a chain of MBufs
RMBuf* CMBufPoolManager::Alloc(TInt aSize, TInt aMinMBufSize, TInt aMaxMBufSize, TBool aIsAllocPool)
	{
	RMBufQ list;

	TInt poolChainsCount = iPoolChains.Count();
	__ASSERT_DEBUG(poolChainsCount != 0, CMBufManager::Panic(EMBuf_NoPoolChain));

#ifdef _DEBUG
	if (poolChainsCount == 0)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::Alloc() - Error! no mbuf pool chain exists")));
#endif
		}
	else if (iLargestMBufSize < aSize)
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponentPerformance, _L8("CMBufPoolManager::Alloc() - Warning! sub-optimal performance; biggest chain doesn't satisfy the request with one mbuf, aSize=%d chainSize=%d"),
			aSize, iLargestMBufSize));
#endif
		}
#endif

	// Do the allocation
	LinearSearchAlloc(list, aSize, aMinMBufSize, aMaxMBufSize, aIsAllocPool);

#ifdef _DEBUG
	if (list.IsEmpty())
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_VAR((KSubsysMBufMgr, KComponent, _L8("CMBufPoolManager::Alloc() - Warning! no matching poolChain found, aSize=%d aMinMBufSize=%d aMaxMBufSize=%d"),
			aSize, aMinMBufSize, aMaxMBufSize));
#endif
		}
#endif

	// At this point, list should be a linked list of all the RMBufs necessary to satisfy the memory request.
	// - mark each one to indicate that it's now being used for buffer data.
	RMBuf* mBuf;
	for (mBuf = list.First(); mBuf != NULL; mBuf = mBuf->Next())
		{
		__ASSERT_DEBUG(mBuf->Type() == EMBufFree, CMBufManager::Panic(EMBuf_AllreadyAlloc));
		mBuf->SetType(EMBufData);
#ifdef _MBUF_TEST
		Mem::Fill(mBuf->Buffer(), mBuf->Size(), '#');
#endif
		}

	return list.First();
	}

// return a chain of MBufs to the pool
void CMBufPoolManager::Free(RMBuf* aMBuf, TBool aIsPreAlloc)
	{

	TBool sameSize = EFalse;

	while (aMBuf != NULL)
		{
		RMBuf* nextMBuf = aMBuf->Next();

		// when aIsPreAlloc is true that means called from Alloc:
		// total allocation for the request is unsuccessful so freeing the allocated MBufs
		__ASSERT_DEBUG(aIsPreAlloc || aMBuf->Type() != EMBufFree, CMBufManager::Panic(EMBuf_AllreadyFree));

		aMBuf->SetType(EMBufFree);
		aMBuf->SetData(0, aMBuf->Size());	// reset length to the size of the mbuf, subsequent ::Alloc can update this if required

		aMBuf->Unlink();

		// update pool chain - eg. free list members
		// placeholder for REQ7864 (de-allocation of mbufs), the return of mbufs to the free list should/could be more intelligent
		// than merely pre-pending as is currently the case.  Eg. examine the pool and corresponding dchunk they belong too to
		// determine if the mbufs should be given priority to be used or de-allocated in the future
		// or alternatively leave marked mbuf's as orphans and mark the mbufs that need to be de-allocated in this way in the memory manager

		RMBufPoolChain* poolChain(aMBuf->MBufPoolChain());
		if (!sameSize)
			{
			poolChain->iLockFreeList.Wait();
		}
		poolChain->iFreeMBufs.Prepend(aMBuf);	// prepends a single mbuf, ie. not any mbufs that may be pointed to by this mbuf
		poolChain->iLenFree += aMBuf->Size();

		if (nextMBuf)
			{
			sameSize = aMBuf->Size() == nextMBuf->Size();
			}

		aMBuf = nextMBuf;

		// free mbufs within the chain
		// - care taken to ensure correct handling of different sized mbufs
		if (nextMBuf == NULL || !sameSize)
			{
			poolChain->iLockFreeList.Signal();
			}
		}

	// when aIsPreAlloc is true that means called from Alloc:
	// total allocation for the request is unsuccessful so freeing the allocated MBufs
	if (!aIsPreAlloc)
		{
		iMBufManager.CallBackAfterFree();
		}
	}

// allocate a memory pool & merge it into the pool chain
TInt CMBufPoolManager::AllocPool(RMBufPoolChain& aPoolChain, TInt aSize, TBool aIsSynchronous)
	{
	// verify that the maximum size will not be exceeded
	if ((iAllocSize + aSize * aPoolChain.iMBufSize) > iMaxAllocSize)
		{
		return KErrNoMBufs;
		}

	if (aIsSynchronous)
		{
		// this is only valid if called from the thread that owns the CMBufManager
		// the RHeap is created in that thread and its handle is only valid in that thread
		CMBufPool* newpool = CMBufPool::New(aPoolChain.iMBufSize,
							aSize, aPoolChain, *iMemoryAllocator);
		if (newpool != NULL)
			{
			iPoolChainLock.Wait();
			aPoolChain.iPools.AddLast(*newpool);
			iPoolChainLock.Signal();

			aPoolChain.ExtendFreeList(newpool);

			iAllocSize += aSize * aPoolChain.iMBufSize;
			return KErrNone;
			}
		return KErrNoMBufs;
		}
	return BackgroundAllocateNewPool(aPoolChain, aSize);
	}

TInt CMBufPoolManager::BytesAvailable()
	// runtime metrics
	// The total number of available bytes that MBufMgr has avaiable in it's free lists.
    {
    TInt bytes = 0;
    TUint poolChainsCount = iPoolChains.Count();
	for (TUint i = 0; i < poolChainsCount; i++)
		{
		bytes += iPoolChains[i].iLenFree;
		}
    return bytes;
    }

TInt CMBufPoolManager::BytesAvailable(TInt aSize)
	// runtime metrics
	// The total number of available bytes that MBufMgr has avaiable in a given free list.
    {
	TInt index = iPoolChains.FindInOrder((TInt)aSize, RMBufPoolChain::Compare);
	if (index != KErrNotFound)
		{
		return iPoolChains[index].iLenFree;
		}
	return 0;
    }

TInt CMBufPoolManager::NextMBufSize(TInt aSize)
	// runtime metrics
    {
	TInt index = 0;
    TUint poolChainsCount = iPoolChains.Count();
    while(index < poolChainsCount)
        {
		if (iPoolChains[index].iMBufSize > aSize )
		    {
		    return iPoolChains[index].iMBufSize;
			}
		++index;
		}
	return KErrNotFound;
    }

TInt CMBufPoolManager::BytesAllocated()
	{
	return iMemoryAllocator->AllocBytes();
	}

#ifdef _MBUF_TEST

TInt CMBufPoolManager::__DbgCheckBuffer(RMBuf* aBuf)
//
// For each pool within the corresponding chain, try to locate aBuf
//
	{
	if (aBuf==NULL)
		{
		return 0;
		}

	TInt n=-1;
	CMBufPool* pool;

    TInt index = iPoolChains.FindInOrder((TInt)aBuf->Size(), RMBufPoolChain::Compare);
	if (index != KErrNotFound)
        {
        RMBufPoolChain* poolChain = &iPoolChains[index];

        TDblQueIter<CMBufPool> list(poolChain->iPools);

		if (poolChain->iPools.IsEmpty())
			{
			return 0;
			}

		// iteratively invoke to locate the required mbuf
		while (n==-1 && (pool = list++, pool!=NULL))
			{
			n = pool->__DbgCheckBuffer(aBuf);
			}

	    if (n<0)
	    	{
	    	CMBufManager::Panic(EMBuf_NotAnMBuf);
	    	}
		}
	return n;
	}

// retrieve free space for all pool chains
TUint CMBufPoolManager::__DbgGetBufSpace()
    {
    int len = 0;
    TUint poolChainsCount = iPoolChains.Count();
	for (TUint i = 0; i < poolChainsCount; i++)
		{
		len += iPoolChains[i].iLenFree;
		}
    return len;
    }

// get free space for pool chain with matching mbuf size
TUint CMBufPoolManager::__DbgGetBufSpace(TUint aMBufSize)
    {
    TInt index = iPoolChains.FindInOrder((TInt)aMBufSize, RMBufPoolChain::Compare);
	if (index != KErrNotFound)
        {
        return iPoolChains[index].iLenFree;
		}
	return 0;
    }

// get used space for all pool chains
TUint CMBufPoolManager::__DbgGetBufTotal()
    {
    int len = 0;
    TUint poolChainsCount = iPoolChains.Count();
	for (TUint i = 0; i < poolChainsCount; i++)
		{
		len += iPoolChains[i].iLenTotal;
		}
    return len;
    }

// get used space for pool chain with matching mbuf size
TUint CMBufPoolManager::__DbgGetBufTotal(TUint aMBufSize)
    {
    TInt index = iPoolChains.FindInOrder((TInt)aMBufSize, RMBufPoolChain::Compare);
	if (index != KErrNotFound)
        {
        return iPoolChains[index].iLenTotal;
		}
	return 0;
    }

// return the first mbuf in the free list belong to the first chain
RMBuf* CMBufPoolManager::__DbgMBufChain()
    {
    if (iPoolChains.Count() > 0)
		{
		return iPoolChains[0].iFreeMBufs.First();
		}
    return NULL;
    }
// return the first mbuf in the free list belonging to the chain of the specified mbuf size
RMBuf* CMBufPoolManager::__DbgMBufChain(TUint aMBufSize)
    {
    TInt index = iPoolChains.FindInOrder((TInt)aMBufSize, RMBufPoolChain::Compare);
	if (index != KErrNotFound)
        {
        return iPoolChains[index].iFreeMBufs.First();
		}
	return 0;
    }

// update the max pool limit (debug only) - use the first pool chain
void CMBufPoolManager::__DbgSetPoolLimit(TInt aCount)
	{
	if (iPoolChains.Count() > 0)
		{
		iPoolChains[0].iDbgPoolLimit = aCount - (aCount % iPoolChains[0].iMBufSize);
		}
	}
// update the max pool limit (debug only) - for the specified mbuf size
void CMBufPoolManager::__DbgSetPoolLimit(TInt aCount, TUint aMBufSize)
	{
    TInt index = iPoolChains.FindInOrder((TInt)aMBufSize, RMBufPoolChain::Compare);
	if (index != KErrNotFound)
        {
        RMBufPoolChain* poolChain = &iPoolChains[index];
    	poolChain->iDbgPoolLimit = aCount - (aCount % poolChain->iMBufSize);
    	}
	}

#endif //#ifdef _MBUF_TEST

//
// MBUF POOLS
//

// overloaded new operator
// - allocates pools on the RHeap instead of the std thread heap
// - placeholder for substituting RHeap with RChunk for either/all;
//   a. creating
//   b. growing - req's handle to existing... handle likely stored within CMBM and accessed via a friend
TAny* CMBufPool::operator new(TUint aSize, TInt aExtra, CMBufMemoryAllocator& aMemoryAllocator) __NO_THROW
	{
	TAny* p = aMemoryAllocator.Alloc(aSize + aExtra);
	return p;
	}

void CMBufPool::operator delete(TAny* aPtr) __NO_THROW
	{
	if (aPtr)
		{
		((CMBufPool* )aPtr)->iMemoryAllocator.Free(aPtr);
		}
	}

/**
 * extra delete operator for exception unwinding in Code Warrior
 */
void CMBufPool::operator delete(TAny* aPtr, TInt /*aExtra*/, CMBufMemoryAllocator& /*aMemoryAllocator*/) __NO_THROW
	{
	CMBufPool::operator delete(aPtr);
	}

CMBufPool::CMBufPool(TInt aMBufSize, TInt aNumMBufs, CMBufMemoryAllocator& aMemoryAllocator)
	: CBase()
	, iNumMBufs(aNumMBufs)
	, iMBufSize(aMBufSize)
	, iMemoryAllocator(aMemoryAllocator)
	{
	}

// Create and return a new mbuf pool
CMBufPool* CMBufPool::New(TInt aMBufSize, TInt aNumMBufs, RMBufPoolChain& aMBufPoolChain, CMBufMemoryAllocator& aMemoryAllocator)
	{
    // allocate pool on the RHeap via an overloaded ::new operator (not obvious here)
	TInt rmbufsize = ALIGN_CACHE_UP(sizeof(RMBuf));
	TInt mbufsize = ALIGN_CACHE_UP(aMBufSize);
	CMBufPool* pool = new(aNumMBufs * (rmbufsize + mbufsize), aMemoryAllocator) CMBufPool(mbufsize, aNumMBufs, aMemoryAllocator);
	if (pool != NULL)
		{
		TUint8* poolSize = ((TUint8*) pool) + sizeof(CMBufPool);

		// buffers must be aligned so that MBUF_ALIGN() will work
	 	__ASSERT_DEBUG(IS_ALIGNED(poolSize), CMBufManager::Panic(EMBuf_NotAligned));

		TInt rmbufsize = ALIGN_CACHE_UP(sizeof(RMBuf));
		TUint8* bufs = poolSize + (aNumMBufs * rmbufsize);
		for (TInt i=0; i<aNumMBufs; i++)
			{
			// overloaded new operator - in place allocation at the specified address
			RMBuf* hdr = (RMBuf*)(poolSize + i * rmbufsize);
			(void) new(hdr) RMBuf(bufs + (i * aMBufSize), &aMBufPoolChain, aMBufSize);
			pool->iList.Append(hdr);
			}
		}
	return pool;
	}

CMBufPool::~CMBufPool()
//
//
//
	{
	iLink.Deque();
	}


void CMBufPool::Merge(RMBufQ &aFreeList)
//
//
//
	{
	aFreeList.Append(iList);
	}

#ifdef _MBUF_TEST
EXPORT_C TInt CMBufPool::__DbgCheckBuffer(RMBuf* aBuf)
//
// Given an mbuf ptr, returns its number in this pool
//
	{
	TUint8* p = (TUint8*)aBuf;
	TUint8* s = ((TUint8*) this) + sizeof(CMBufPool);
	TUint8* e = s + (iNumMBufs * sizeof(RMBuf));

	if (p<s)
		{
		return -1;
		}
	else if (p<e)
		{
		return 1 + ((p-s) / sizeof(RMBuf));
		}
	s = e;
	e = s + (iNumMBufs * aBuf->iSize);
	if (p>=e)
		{
		return -1;
		}
	else
		{
		CMBufManager::Panic(EMBuf_CorruptMBuf);
		}

	return -2 - ((p-s) / aBuf->iSize);
	}
#else
EXPORT_C TInt CMBufPool::__DbgCheckBuffer(RMBuf* /*aBuf*/)
{
   return KErrNone;
}

#endif

