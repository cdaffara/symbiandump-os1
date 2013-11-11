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
* RMBufPoolChain decleration
* 
*
*/



/**
 @file
 @internalTechnology
*/

#ifndef MBUF_POOL_CHAIN_H
#define MBUF_POOL_CHAIN_H

#include <e32def.h>     // for std types

// forward decleration(s) to avoid circular dependency by including this file
// - this is a symptom of multiple class declerations within the same header file
//   eg. es_mbman.h required for CMBufPool, but also contains CMBufManager which inturn requires RMBufPoolChain defined in this file!
class RMBufPoolChain;
#include <es_mbman.h>       // for CMBufPool


// this class is internal to MBM, but alas this can't be enforced by C++ (except by not exporting its header file)
class RMBufPoolChain
	{
	friend class CMBufPoolManager;	// to allow access to private members
   
	RMBufPoolChain(TInt aMBufSize, CMBufPoolManager& aMBufPoolManager); // to be used only in searching for existing poolchain
	RMBufPoolChain(TInt aMBufSize, TInt aInitialAllocation, TInt aMinGrowth, TInt aHighGrowthThresholdPercentage, CMBufPoolManager& aMBufPoolManager);

	void OpenL();
	void Close();
	
public:	
	
	TInt GrowthSize(TInt aSize);
	void ExtendFreeList(CMBufPool* aPool);  // assumed that the caller has already acquired the lock, refer comments within the definition
	
	TInt Alloc(RMBufQ& aList, TInt aSize, TBool aIsAllocPool);
	static TInt Compare(const TInt* aMBufSize, const RMBufPoolChain& aRHSPoolChain);
	static TInt Compare(const RMBufPoolChain& aLHSPoolChain, const RMBufPoolChain& aRHSPoolChain);
	CMBufPoolManager& MBufPoolManager();
	
private:

	RWorkerLock iLockFreeList;				// lock for free list
    TInt iLenFree;					// number of bytes free within the pool chain
    TInt iLenTotal;				// number of bytes allocated (free + used) within the pool chain
    RMBufQ iFreeMBufs;				// free list of mbufs ready to be consumed, use of plural naming implies the queue (hence the name isn't appended)
    TDblQue<CMBufPool> iPools;

   	const TInt iMBufSize;          // size (in bytes) of contained mbufs, also stored within mbuf but kept here for encapsulation & ease of reference
    const TInt iInitialAllocation; // initial allocation size in num of MBufs (not bytes)
    TInt iMinGrowth;  				// minimum (automatic) incremental allocation in num of MBufs (not bytes) - actual allocate may be larger (refer )
    TInt iGrowthThreshold; 		// if the num of free mbufs is <= this value, then a a new pool is automatically allocated

	TBool iSignalled;				// background growth in progress
	TInt iNumMBufsToAdd;			// background growth
	TBool iNoMoreSpace;				// last background growth failed
	TInt iLastGrowth;				// last attempted growth
	TInt iMaxGrowthAttempt;			// largest growth attempt

	TInt iDbgPoolLimit;				// limit size of pool chain
	
	CMBufPoolManager& iMBufPoolManager;
	};

#endif  // MBUF_POOL_CHAIN_H
