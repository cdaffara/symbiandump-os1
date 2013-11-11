// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Implementation of system shared buffer allocation and buffer
// management.
//

#ifndef __SYSTEMSHAREDBUFS_H__
#define __SYSTEMSHAREDBUFS_H__

#include "commsbufpondintf.h"
#include "commsbufponddbg.h"
#include "commsbufpool.h"
#include "systemsharedasyncalloc.h"
#include "commsbufq.h"
#include "commsbufponddbg.h"
#include <e32shbuf.h>

class TCommsBufPoolCreateInfo;
class RCommsBuf;
class RShPool;
class MCommsBufPondIntf;
/**

Representation of a single system shared pool.Allocates/free from the system shared pool or from 
the freelist if enabled) 

@internalTechnology
 
*/
NONSHARABLE_CLASS(CSystemSharedBufPool) : public CCommsBufPool
	{
	public:
	static CSystemSharedBufPool* New(MCommsBufPondIntf& aPondIntf, const TCommsBufPoolCreateInfo& aCreateInfo);
	
#ifdef  SYMBIAN_ZEROCOPY_BUF_FREELIST
	TInt FreeListCount();
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST
	
	~CSystemSharedBufPool();
	
	TInt AllocUnderflow(RCommsBufQ& aBufQ, TInt aSize);	
	TInt AllocOverflow(RCommsBufQ& aBufQ, TInt aSize);
	
	RCommsBuf* Alloc();
	void Free(RCommsBuf* aBuf);
	inline RShPool Pool() const;

	static TInt Compare(const CSystemSharedBufPool& aLhs, const CSystemSharedBufPool& aRhs);	

	private:
	inline CSystemSharedBufPool(MCommsBufPondIntf& aPondIntf, TInt aSize);
    TInt Construct(const TCommsBufPoolCreateInfo& aCreateInfo); 
    
#ifdef	SYMBIAN_ZEROCOPY_BUF_FREELIST
	inline CSystemSharedBufPool(MCommsBufPondIntf& aPondIntf, TInt aSize, TInt aFreeListCount);	
	TInt AllocFromFreeList(RCommsBufQ& BufQ, TInt aSize);
	TBool ReleaseToFreeList(RCommsBuf* aBuf); 
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST
	
	private:
	RShPool		iPool;
#ifdef SYMBIAN_ZEROCOPY_BUF_FREELIST
	RWorkerLock iFreeListLock;
	RCommsBufQ  iFreeList;
	TInt        iMaxFreeListCount;
	TInt        iFreeListCount;
#endif // SYMBIAN_ZEROCOPY_BUF_FREELIST
	};

/**
Implements the 3 stage buffer allocation algorithm using the system wide shared buffer APIs or
from the freelist (if enabled)

Description of the 3 stage allocator.

Requirements:
1/ The algorithm MUST try to allocate from the pool that has bigger or equal size buffers in 
preference to the pools that has smaller size buffers ie; 
2/ The algorithm MUST return fewest bytes of memory that satisfy the request and doesn't conflict 
with 1/
3/ More than one buffer may be allocated and linked, if the requested buffer size doesn't directly 
satisfy with the configured buffer sizes.

Input parameters:
Size            - Requested total size 
Minimum Size    - The size that the allocated buffer(s) must atleast have                       
Maximum Size    - The size that the allocated buffer(s) must not exceed     
            

Working Model:
The algorithm relies on the big to small order. If the order is not obeyed/adhered the algorithm will 
fail to allocate the buffers from the right pools and more buffers maybe allocated than needed.The 
pools are sorted during initialization time.        

The first step in the algorithm is to clip the pool array position based on the given min and max size. 
Clipping is done to ensure that allocation happens from the pools that satisfies the given min and 
max condition, and no further checking is needed on the pools to see whether the pool buffer size 
satisfies the min and max values. Once the clipping is done the algorithm works in 4 stages.   

The following terms are used in the description of the algorithm. The terms should be read as:
biggest pool ---  The pool that has the bigger buffer size, relative to the given max value
smallest pool --- The pool that has the smallest buffer size, relative to the given min value 

The reader should note that there is no gurantee that the requested allocation size will be exactly or 
nearly matching to the pool sizes and this applies to each stage that operatea on the allocation size.
 
The first stage execution depends on the input parameters and lower and upper index. Subsequent stages 
exectuion depend on the outcome of the prior stage.
    
The 4 stages are:
1/ Traverse the pool array forward from the biggest pool index till the traversal reaches the smallest 
pool index. When the requested size is greater than the buffer size of the "next" pool allocate as much 
as possible from the "current" pool and decrement the requested size if there is an allocation. If there 
is no allocation happens from the "current" pool mark the "current" pool index and exit the stage.
 At this point,
 a/ We allocated completely as requested or 
 b/ We would have been passed through and didn't allocate from the bigger pools that may have buffer for 
 allocation in order to satisfy the Credo 2/. But we MUST satisfy the Credo 1/ and in order to satisfy the 
 Credo 1/ we need to traverse back from the point where we stopped the traversal, hence need to mark the 
 "current" pool index. 
2/ If the first stage allocation cannot complete due to the non-availability of buffer in the pools or with 
1.b/ traverse the array backward starting from the marked pool index till the traversal reaches the biggest 
pool index. Allocate from any pool where buffers are available by satisfying Credo 1/.
3/ If the second stage allocation cannot complete due to the non-availability of buffers then we have to 
increment the marked pool index by 1 as the marked pool has been checked for buffer availability on the 2nd 
stage. We have the following scenario:
 a/ The pending allocation size (maybe partial or the size "actually" requested)is greater than the marked 
 pool index buffer size
 b/ Allocation MUST satisfy the Credo 1/

Traverse the array forward from the marked pool index till the traversal reaches the smallest pool index. 
Allocate from any pool where buffers are available.   

Special condition:
If the requested size is 0 we will allocate from the pool that has the smallest buffer size, provided the
availability of buffers in the pool. The zero sized allocation takes a different path (using ZeroSizedAlloc Fn.)
and does not run through the stages as described above.

Reasoning:

We MUST satisfy the requirement 1 and we SHOULD try to achieve the requirement 2 then we have the following 
that we encounter during allocation

1/ No gurantee that we get buffers from the pool that has the best size for the allocation.
2/ We might skip a pool to identify the more best size and we may not be able to allocate from that pool
due to the non-availablity of the buffers. We would be needing to traverse back on the pool array.
3/ No gurantee that we can allocate we traverse back because some other threads woule have been executed inbetween
and allocate the buffers and the pool might became empty. and vice versa.

Drawbacks:

1/ More looping happens with the algorithm on certain cases when the bigger size pools became empty.

Conclusion:
1/ Couple of more loops on certain cases does not add much problems as the total no. of elements in the pools   
are expected to be ~10 hence does not cause much problems in terms of performance.
2/ All depends on how the pool buffer sizes are configured. It is expected that the smaller size buffer numbers 
in the pool will be lesser than bigger sizes. So most time we get good allocation.
3/ More problems are on the allocation of system shared buffer from the Kernal as the algorithm has to allocate
one by one and ask the pool each time. If there is no buffers available on the requested pool there will be more 
system calls 

Based on the above arguments the conclusion is that algorithm (and the couple of more loops) doesn't cause much 
problems


Future & suggestions:
It maybe possible to fine tune the algorithm. But any futuer enhancements should consider 
1/ the min and max  conditions
2/ Non-deterministic configured pool sizes

One of the thing that would definitly improve the algorithm is to request the kernel to allocate a series of
RShBuf's. This would reduce the no. of system calls that is currently happening on the system. The current freelist
implementation tries to address this issue to an extent.

@internalTechnology
*/

class T3StageAllocator
	{
	public:
	    T3StageAllocator(RPointerArray<CSystemSharedBufPool>& aPools, TInt aSize, TInt aMinSize, TInt aMaxSize);
	RCommsBuf* Do();
	
	private:
	
	void Init(TInt aMinSize, TInt aMaxSize);
	
	void ForwardAlloc1stStage();
	void BackwardAlloc2ndStage();
	void ForwardAlloc3rdStage();
	
	void ZeroSizedAlloc();
	
	private:
	RPointerArray<CSystemSharedBufPool>& iPools;
	TInt iSize;
	TInt iBiggestPoolIndex;
	TInt iSmallestPoolIndex;
	TInt iMarkedPoolIndex;	
	RCommsBufQ iBufQ;
	TBool iZeroBufSize;	
	};

/**
Implements the buffer management using the system wide shared buffers

@internalTechnology
*/
NONSHARABLE_CLASS(CSystemSharedBufPond) : public CBase, public MCommsBufPondIntf, public MCommsBufPondDbg
	{
	friend class RCommsBufPond;

	public:
	static MCommsBufPondIntf* New(RArray <TCommsBufPoolCreateInfo>& aPoolInfo);
	~CSystemSharedBufPond();
	
	private:
	
	TInt Construct(RArray <TCommsBufPoolCreateInfo>& aPoolInfo);

	// From MCommsBufPondIntf
	virtual RCommsBuf* Alloc(TInt aSize, TInt aMinBufSize, TInt aMaxBufSize);
	virtual RCommsBuf* FromHandle(TInt aHandle);
	virtual TInt Store(TDes8& aStore) const;
	virtual void Free(RCommsBuf* aBuf);
	virtual TInt BytesAvailable() const;
	virtual TInt BytesAvailable(TInt aSize) const;
	virtual TInt NextBufSize(TInt aSize) const;
	virtual TInt LargestBufSize() const;
	virtual void StartRequest(CCommsBufAsyncRequest& aRequest);
	virtual void CancelRequest(CCommsBufAsyncRequest& aRequest);
	virtual void SetContext();
	virtual void Release(RLibrary& aLib);
	virtual MCommsBufPondDbg& CommsBufPondDbg();

	// From MCommsBufPondDbg
    virtual RCommsBuf* __DbgBufChain();
    virtual RCommsBuf* __DbgBufChain(TUint aBufSize);
    virtual void __DbgSetPoolLimit(TInt aCount);
    virtual void __DbgSetPoolLimit(TInt aCount, TUint aBufSize);
    virtual void __DbgSetFailAfter(TInt aCount=0);
    virtual TUint __DbgGetBufSpace();
    virtual TUint __DbgGetBufSpace(TUint aBufSize);
    virtual TUint __DbgGetBufTotal();
    virtual TUint __DbgGetBufTotal(TUint aMufSize);
    virtual TInt __DbgGetHeapSize();

	private:
	RPointerArray<CSystemSharedBufPool> iPools;
	CSystemSharedAsyncAlloc*			iAsyncAlloc;
  	};

#include "systemsharedbufs.inl"
#endif // __SYSTEMSHAREDBUFS_H__


