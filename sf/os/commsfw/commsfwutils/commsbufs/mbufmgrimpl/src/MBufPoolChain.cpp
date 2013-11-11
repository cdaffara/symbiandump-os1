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
// Maintains a free queue of single size RMBuf's on behalf of CMBufPoolManager
// 
//

/** 	  	 
@file 	  	 
@internalComponent 	  	 
*/



#include "MBufPoolChain.h"
#include "MBufPool.h"
#include "MBufPoolManager.h"	
#include "commsbufpool.h"


CMBufPoolChain::CMBufPoolChain(TInt aMBufSize, TInt aInitialAllocation, TInt aMinGrowth, TInt aGrowthThreshold, TInt aPoolCeiling, CMBufPoolManager& aMBufPoolManager) 
    : CCommsBufPool(aMBufPoolManager.Manager(), aMBufSize),
    iLenFree(0),
    iLenTotal(0),    
      iInitialAllocation(aInitialAllocation), 
      iMinGrowth(aMinGrowth), 
      iGrowthThreshold(aGrowthThreshold), 
      iSignalled(EFalse),
      iNumMBufsToAdd(0),
      iNoMoreSpace(0),
      iLastGrowth(0),
      iMaxGrowthAttempt(0),
      iDbgPoolLimit(0),
      iPoolCeiling(aPoolCeiling),
      iMBufPoolManager(aMBufPoolManager)      
	{
	}
CMBufPoolChain::CMBufPoolChain(TInt aMBufSize, CMBufPoolManager& aMBufPoolManager)
: CCommsBufPool(aMBufPoolManager.Manager(), aMBufSize),
 iInitialAllocation(0),
 iMBufPoolManager(aMBufPoolManager)
	{
		
	}

CMBufPoolChain* CMBufPoolChain::NewL(TInt aMBufSize, TInt aInitialAllocation, TInt aMinGrowth, TInt aGrowthThreshold, TInt aPoolCeiling, CMBufPoolManager& aMBufPoolManager)
	{
	CMBufPoolChain* self = new(ELeave)CMBufPoolChain(aMBufSize, aInitialAllocation, aMinGrowth, aGrowthThreshold, aPoolCeiling, aMBufPoolManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

void CMBufPoolChain::ConstructL()
	{
	iFreeMBufs.Init();
	User::LeaveIfError(iLockFreeList.CreateLocal());
	iPools.Reset();
	iPools.SetOffset(_FOFF(CMBufPool, iLink));		
	}
	
CMBufPoolChain::~CMBufPoolChain()
	{
	iLockFreeList.Close();
	}



// get the growth amount
TInt CMBufPoolChain::GrowthSize(TInt aSize)
{
	// small request so grow by minimum size
	if (aSize < iMinGrowth)
		{
		return iMinGrowth;
		}
	
	// Have we tried as big as this in the past?
	if ( (iMaxGrowthAttempt > 0) && (aSize >= iMaxGrowthAttempt) )
		{
		// try smaller size again
		aSize = iMaxGrowthAttempt;
		}
	
	// Attempt to grow the pool by requested amount
	if (!iNoMoreSpace)
		{
		return aSize;
		}
	
	// Last growth attempt failed so check if its worth
	// trying again
	if ( iLastGrowth < (iMinGrowth + 2))
		{
		return 0;
		}
	
	// Last growth attempt failed so lets try a smaller
	iMaxGrowthAttempt = iMinGrowth + (iLastGrowth - iMinGrowth) / 2;
	
	return iMaxGrowthAttempt;
}

// merge new pool into the pool chain
void CMBufPoolChain::ExtendFreeList(CMBufPool* aPool)
	{
	// The caller must already have acquired the freelist lock
	// - justification;
	//   a. limitation in RWorkerLock's RFastLock which does not support nested requests by the same thread without hanging the thread
	//   b. performance is adversely affected by continuoulsy locking & releasing the lock by the caller, refer ::Alloc
	// - assumption is checked for debug builds, but not for release builds as it is too expensive & indicates a serous coding error that
	//   could/should of been fixed within a debug build anyhow
	__ASSERT_DEBUG(iLockFreeList.IsOwned(), CMBufManager::Panic(EMBuf_FreeLockNotOwned));
	
	iFreeMBufs.Append(aPool->MBufQ());
    iLenFree += aPool->Count() * iBufSize;
    iLenTotal += aPool->Count() * iBufSize;
	}

// allocate a chain of MBufs from the pool chain
TInt CMBufPoolChain::Alloc(RMBufQ& aList, TInt aSize, TBool aIsAllocPool)
	{

	// Lock the freelist so that the transfer of mbufs and statistics update is an atomic operation
	iLockFreeList.Wait();

	TInt transfered = iFreeMBufs.Transfer(aList, aSize, BufSize());
	iLenFree -= transfered;
	
	// If necessary gather data from the poolChain within the freelist lock to ensure integrity of statistics used
	TInt preAllocationLength = 0;
	if (aIsAllocPool)
		{
		preAllocationLength = iGrowthThreshold - (iLenFree/iBufSize); // determine in terms of number of mbufs.
		}

	// adjust the growth if necessary
	TInt growth = 0;
	if (preAllocationLength >= 0)
		{
		growth = GrowthSize(preAllocationLength);
		}
	
	// Now free the freelist lock
	iLockFreeList.Signal();
		
	// check if the growth threshold trigger has been exceeded, and if so perform a background pool (pre)allocation
	if (aIsAllocPool)
		{
		if (preAllocationLength >= 0)
			{
			// a failed background (pre)allocation shall not error the user since they shouldn't know (and hence care), thus
			// the return code is only used to avoid a perpetual loop
			if (growth > 0)
				{
				if (KErrNoMBufs == iMBufPoolManager.AllocPool(*this, growth, EFalse))
					{
					iMaxGrowthAttempt = growth;
					}
				}
			}
		}
	return transfered;
	}

// compare function to find the suitable pool chain
TInt CMBufPoolChain::Compare(const TInt* aMBufSize, const CMBufPoolChain& aRHSPoolChain)
	{
	if (*aMBufSize < aRHSPoolChain.BufSize())
		{
		return -1;
		}
	else if (*aMBufSize > aRHSPoolChain.BufSize())
		{
		return 1;
		}
	return 0;
	}

TInt CMBufPoolChain::Compare(const CMBufPoolChain& aLHSPoolChain, const CMBufPoolChain& aRHSPoolChain)
	{
	return Compare((TInt*)&(aLHSPoolChain.iBufSize), aRHSPoolChain);	
	}

CMBufPoolManager& CMBufPoolChain::MBufPoolManager()
	{
	return iMBufPoolManager;
	}

TBool CMBufPoolChain::CanExtend() const
	{
	return iLenTotal < iPoolCeiling;		
	}
	
TInt CMBufPoolChain::ExtendSize() const
	{
	if(iLenTotal == 0)
		{
		// Always return the initial allocation value if we haven't allocated anything yet.
		return iInitialAllocation;			
		}

	
	TInt toGrow = (iPoolCeiling - iLenTotal)/iBufSize;	
	return (toGrow < iMinGrowth) ? toGrow : iMinGrowth ;	
	}

TBool CMBufPoolChain::HonourPoolCeiling() const
	{
	return iPoolCeiling != KErrNotFound;	
	}

	
