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
// Buffer Manager for Protocols
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/cfperfmetrics.h>
#include <es_mbman.h>
#include <es_prot.h>	// for ESocketTimerPriority
#include <e32hal.h>
#include <comms-infras/cfmacro.h>
#include "MBufPoolChain.h"
#include "MBufTimer.h"
#include "MBufPool.h"
#include "MBufSizeAllocator.h"
#include "MBufPoolManager.h"
#include "MBufMemoryAllocator.h"
#include <cflog.h>

#ifdef __CFLOG_ACTIVE
__CFLOG_STMT(_LIT8(KComponent, "Manager");)
__CFLOG_STMT(_LIT8(KSubsysMBufMgr, "MBufMgr");) // subsystem name
#endif

/**
MBuf Private Heap Limits

@internalTechnology
*/
static const TInt KMBufFreePriority = 20;

// The asynchronous allocations have to be requested by the thread that created the MBufMgr
// This we do by having a requester object in that thread which gets completed by the other
// threads to trigger the request
NONSHARABLE_CLASS(CRequestAsyncAlloc) : public CActive
	{
public:
	static CRequestAsyncAlloc* NewL();
	virtual ~CRequestAsyncAlloc();

	void StartWaitForRequest();
	void MakeRequest();
private:
	CRequestAsyncAlloc();
	void ConstructL();

	virtual void DoCancel();
	virtual void RunL();
	RCriticalSection iCritSec;
	RThread iMBufMgrOwnerThread;
	TBool iSignalled;
	};

CRequestAsyncAlloc::CRequestAsyncAlloc()
: CActive(0)
	{
	}

CRequestAsyncAlloc* CRequestAsyncAlloc::NewL()
	{
	CRequestAsyncAlloc* This = new(ELeave) CRequestAsyncAlloc;
	CleanupStack::PushL(This);
	This->ConstructL();
	CleanupStack::Pop(This);
	return This;
	}

void CRequestAsyncAlloc::ConstructL()
	{
	User::LeaveIfError(iCritSec.CreateLocal());
	iMBufMgrOwnerThread.Duplicate(RThread());
	CActiveScheduler::Add(this);
	}

CRequestAsyncAlloc::~CRequestAsyncAlloc()
	{
	Deque();
	iCritSec.Close();
	iMBufMgrOwnerThread.Close();
	}

void CRequestAsyncAlloc::StartWaitForRequest()
	{
	__ASSERT_DEBUG(!IsActive(), CMBufManager::Panic(EMBuf_AlreadyActive));
	iStatus = KRequestPending;
	SetActive();
	}

void CRequestAsyncAlloc::MakeRequest()
	{
	NETWORKING_ATOMIC(;) // Codepattern below may have SMP implications
	// This is being most likely being called from the context of some thread other than the
	// MBufMgr owner, and there's a distinct risk of more than one thread being OOB at the same
	// time since it's a common pool. So here we have to guard against being completed multiple
	// times, using a critical section because there's a tiny but real risk of a completion from
	// another thread in between a test upon IsActive() and our completion of it, which could give
	// a stray event panic
	if(!iSignalled)
		{
		// We're almost certainly the only thread doing this; now check again inside the critsec to be sure
		iCritSec.Wait();
		if(!iSignalled)
			{
			iSignalled = ETrue;
			TRequestStatus* pStatus = &iStatus;
			iMBufMgrOwnerThread.RequestComplete(pStatus, KErrNone);
			}
		iCritSec.Signal();
		}
	}


void CRequestAsyncAlloc::DoCancel()
	{
	NETWORKING_ATOMIC(;) // Codepattern below may have SMP implications
	if(!iSignalled)
		{
		// We're almost certainly the only thread doing this; now check again inside the critsec to be sure
		iCritSec.Wait();
		if(!iSignalled)
			{
			iSignalled = ETrue;
			TRequestStatus* pStatus = &iStatus;
			User::RequestComplete(pStatus, KErrCancel);
			}
		iCritSec.Signal();
		}
	}

void CRequestAsyncAlloc::RunL()
	{
	// Now that we're in the thread that owns the timer we can reset it
	CMBufManager::Context()->WatchDogReset();
	// Ready for the next cry of OOB distress
	iStatus = KRequestPending;
	iSignalled = EFalse;
	SetActive();
	}

//
// MBUF MANAGER
//

CMBufManager::CMBufManager()
/**
MBUF MANAGER
*/
	: CBase()
	{
#ifdef __CFLOG_ACTIVE
	__CFLOG_1(KSubsysMBufMgr, KComponent, _L8("CMBufManager %x:\tCMBufManager()"), this);
#endif

	iAllocsPending.SetOffset(_FOFF(RMBufAsyncRequest,iLink));
#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	CommsFW::CPerfMetricStore::AddClient(this, AddToPerfLog);
#endif
	}


EXPORT_C void CMBufManager::Panic(TMBufPanic aPanic)
/**
For use by mbuf related classes
*/
	{
	_LIT(mbuf,"MBuf");
	User::Panic(mbuf, aPanic);
	}


EXPORT_C CMBufManager::~CMBufManager()
//
//
//
	{
#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	CommsFW::CPerfMetricStore::RemoveClient(this);
#endif
	
	if (iRequestAsyncAlloc)
		{
		iRequestAsyncAlloc->Cancel();
		delete iRequestAsyncAlloc;
		iRequestAsyncAlloc = NULL;
		}

	while (!iAllocsPending.IsEmpty())
		{
        CancelRequest(*iAllocsPending.First());
        }

	if (iFreeCB!=NULL)
		{
        delete iFreeCB;
        }

	iAsynAllocLock.Close();

	delete iTimer;

	if (iMBufPoolManager)
		{
		delete iMBufPoolManager;
		}

	Dll::SetTls(NULL);
	}

// overload
// - aMBufSizeAllocator overload used to provide the caller (ie. the owner of the CMBufManager) restricted access to the pool manager
EXPORT_C CMBufManager* CMBufManager::NewL(TInt aMaxHeapSize, MMBufSizeAllocator* &aMBufSizeAllocator)
	{
	__ASSERT_ALWAYS(Dll::Tls() == NULL, Panic(EMBuf_AlreadyInit));
	CMBufManager* mgr = new(ELeave) CMBufManager;
	Dll::SetTls(mgr);
	CleanupStack::PushL(mgr);
	mgr->ConstructL(aMaxHeapSize);
	CleanupStack::Pop();

	// the iMBufPoolManager object is owned by CNBufManager, and thus the aMBufSizeAllocator reference should not be destroyed by
	// the caller code
	aMBufSizeAllocator = mgr->iMBufPoolManager;

	return mgr;
	}

void CMBufManager::ConstructL(TInt aMaxHeapSize)
//
//
//
	{
	User::LeaveIfError(iAsynAllocLock.CreateLocal());

	TCallBack c(FreeCallBack, this);

	// create an instance of the mbuf pool manager
	// - used to handle all pool allocation manipulation, also used as a concrete class for limited public interfaces exposed
	//   to the client

	iMBufPoolManager = CMBufPoolManager::NewL(aMaxHeapSize, *this);
	CleanupStack::PushL(iMBufPoolManager);
	
	iFreeCB = new(ELeave) CAsyncCallBack(c, KMBufFreePriority);
	CleanupStack::PushL(iFreeCB);
	
	iTimer=CDeltaTimer::NewL(EMBufMgrTimerPriority, KMbufManTimerGranularity);
	CleanupStack::PushL(iTimer);
	
	iTimerThreadId = RThread().Id();

	iRequestAsyncAlloc = CRequestAsyncAlloc::NewL();
	
	iRequestAsyncAlloc->StartWaitForRequest();
	
	CleanupStack::Pop(iTimer); 
	CleanupStack::Pop(iFreeCB); 
	CleanupStack::Pop(iMBufPoolManager); 
	}

EXPORT_C CMBufManager* CMBufManager::Context()
	// There is a single system-wide MBuf manager.
	{
	CMBufManager* pMgr = STATIC_CAST(CMBufManager*, Dll::Tls());
	__ASSERT_ALWAYS(pMgr != NULL, Panic(EMBuf_NoManager));
	return pMgr;
	}

EXPORT_C void CMBufManager::SetContext()
	// There is a single system-wide MBuf manager.
	{
	Dll::SetTls(this);
	}

TInt CMBufManager::BytesAvailable() const
    {
	__ASSERT_DEBUG(iMBufPoolManager!=NULL, Panic(EMBuf_NoPoolManager));
	return iMBufPoolManager->BytesAvailable();
    }

TInt CMBufManager::BytesAvailable(TInt aSize) const
    {
    __ASSERT_DEBUG(iMBufPoolManager!=NULL, Panic(EMBuf_NoPoolManager));
	return iMBufPoolManager->BytesAvailable(aSize);
    }


#ifdef _MBUF_TEST
// misc. sanity checks - all pool chains
EXPORT_C void CMBufManager::__DbgCheckChain(RMBuf* aMBuf, TMBufType aType, TInt aLength, TInt aSize)
//
// Check that an MBuf chain if of the required length and that all bufs are
// of the requred type.
//
	{
	CMBufManager* mgr = CMBufManager::Context();

	TInt siz=0, len=0;

	RMBuf* m;
	RMBuf* p = NULL;
	TMBufIter iter(aMBuf);

	while (m = iter++, m!=NULL)
		{
		mgr->__DbgCheckBuffer(m);

		len += m->Length();
		siz += m->Size();
		if (m->Type()!=aType)
			Panic(EMBuf_CheckFailType);
		p = m; // so we know what the previous one was when it panics under the debugger
		(void)p->Last();
		}

	if (aSize!=0 && siz!=aSize)
		Panic(EMBuf_CheckFailSize);

	if (aLength!=0 && len!=aLength)
		Panic(EMBuf_CheckFailLength);
	}


EXPORT_C TInt CMBufManager::__DbgCheckBuffer(RMBuf* aBuf)
//
// For each pool within the corresponding chain, try to locate aBuf
//
	{
	return iMBufPoolManager->__DbgCheckBuffer(aBuf);
	}

#else	//#ifdef _MBUF_TEST
EXPORT_C void CMBufManager::__DbgCheckChain(RMBuf* /*aMBuf*/, TMBufType /*aType*/, TInt /*aLength*/, TInt /*aSize*/)
{
   return;
}

EXPORT_C TInt CMBufManager::__DbgCheckBuffer(RMBuf* /*aBuf*/)
{
   return KErrNone;
}

#endif


void CMBufManager::WatchDogReset()
	{
	if (iWatchDogIsPending)
		{
		iWatchDogIsPending=EFalse;
		MBufTimer::Remove(iWatchDog);
		}
	if(!iAllocsPending.IsEmpty())
		{
		TCallBack c(WatchDogExpire, this);
		iWatchDogIsPending=ETrue;
		iWatchDog.Set(c);
		MBufTimer::Queue(KMBufWatchDogTime,iWatchDog);
		}
	}

TInt CMBufManager::WatchDogExpire(TAny* aPtr)
	{
	((CMBufManager*)aPtr)->iWatchDogIsPending=EFalse;
	((CMBufManager*)aPtr)->CompleteAsyncAllocs(EFalse);		// if required, do not allocate new pools
	((CMBufManager*)aPtr)->CompleteAsyncAllocs(ETrue);		// if required, do allocate new pools
	return 0;
	}


void CMBufManager::StartRequest(RMBufAsyncRequest& aRequest)
//
//
//
	{
	iAsynAllocLock.Wait();
	iAllocsPending.AddLast(aRequest);
	iAsynAllocLock.Signal();
	iRequestAsyncAlloc->MakeRequest();
	}


void CMBufManager::CancelRequest(RMBufAsyncRequest& aRequest)
//
//
//
	{
	aRequest.iLink.Deque();
	aRequest.Complete(KErrCancel);
	
	if (iRequestAsyncAlloc)
	   {
	   iRequestAsyncAlloc->MakeRequest();
	   }
	}

// attempt to complete outstanding asynchronous allocation requests
// - typically called after some mbufs have been freed
void CMBufManager::CompleteAsyncAllocs(TBool aIsAllocPool)
	{
	TBool reset = ETrue;
	RMBufAsyncRequest *req;

#ifdef __CFLOG_ACTIVE
	__CFLOG_1(KSubsysMBufMgr, KComponent, _L8("CMBufManager %x:\tCompleteAsyncAllocs() called"), this);
#endif

	// first lock the pending list
	iAsynAllocLock.Wait();
	TDblQueIter<RMBufAsyncRequest> iter(iAllocsPending);

	// attempt to allocate each outstanding asynchronous allocation request
	// - deliberately so, the request is attempted without extending the pool (as this is done from a watchdog timer as a last resort)
	while (req = iter++, req != NULL)
		{
		// if null size specified, then allocate a default sized mbuf
		// - to avoid a SC break, the default size is hard coded to K_MBufSmallSize for consumers that assume that this length will be returned
		TInt reqSize = req->iLength;
		if (reqSize == 0) 	// trs; does it make sense to request an allocation without specifying a length? kept as is to avoid a SC break
			reqSize = KMBufSmallSize;


		RMBuf* mBufs = Alloc(reqSize, 0, KMaxTInt, aIsAllocPool);
		if (mBufs)
			{
			req->iMBufs = mBufs;
			req->Complete(KErrNone);

			if (aIsAllocPool)	// not done for pool allocation growth to ensure FC with the factored out implementation; CompleteLargeRequests()
				reset = EFalse;
			}
		}

	iAsynAllocLock.Signal();

	// only reset the watch dog if there are no pending requests left or a request was completed, otherwise big pending requests might
	// be stalled by a continual trickle of small allocs and frees.
	if (iAllocsPending.IsEmpty() || reset)
		WatchDogReset();
	}

// refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
EXPORT_C RMBuf* CMBufManager::AllocL(TInt aSize)
//
// Allocate and initialise a chain of MBufs
// Total data length is set to exact size.
//
	{
	RMBuf* buf = Alloc(aSize);
	if(!buf)
		{
		User::Leave(KErrNoMBufs);
		}
	return buf;
	}

TInt CMBufManager::LargestMBufSize() const
    /** Returns the size of the largest MBuf that the manager can provide.
        @return the size of the largest MBuf that the manager can provide.
    */
	{
	// iLargestMBufSize is needed for legacy functionallity when the MBufSize is not specified
	// in RMBufChain::Align(TInt aSize).  It is updated in MBufPoolManager as new pools are created
	__ASSERT_DEBUG(iMBufPoolManager!=NULL, Panic(EMBuf_NoPoolManager));
	return iMBufPoolManager->LargestMBufSize();
	}

TInt CMBufManager::NextMBufSize(TInt aSize) const
    /** Used to obtains the sizes of the MBufs that the manager can provide.
        @param the size to start searching from.
        @return the size of the next MBuf that is greater than aSize, KErrNotFound if there is no MBuf bigger than aSize.
    */
	{
	__ASSERT_DEBUG(iMBufPoolManager!=NULL, Panic(EMBuf_NoPoolManager));
	return iMBufPoolManager->NextMBufSize(aSize);
	}

// allocate and initialise a chain of MBufs
// - total data length is set to exact size
// - overloaded variants are deliberately not exported because;
//   a. likely that this interface will be deprecated in the future and thus we don't want to unnecessarily extend it (ie. more maintenance)
//   b. easy to export them down the track, but not so easy to go the other way
RMBuf* CMBufManager::Alloc(TInt aSize, const RMBufChain& aMBufChain)
	{
	// select min/max mbuf size constraints based upon an existing mbuf
	if (aMBufChain.First())
		return Alloc(aSize, aMBufChain.First()->Size(), aMBufChain.First()->Size());
	else
		return Alloc(aSize);
	}
EXPORT_C RMBuf* CMBufManager::Alloc(TInt aSize)
	{
	return Alloc(aSize, 0, KMaxTInt);
	}
RMBuf* CMBufManager::Alloc(TInt aSize, TInt aMinMBufSize)
	{
	return Alloc(aSize, aMinMBufSize, KMaxTInt);
	}
RMBuf* CMBufManager::Alloc(TInt aSize, TInt aMinMBufSize, TInt aMaxMBufSize)
	{
	return Alloc(aSize, aMinMBufSize, aMaxMBufSize, ETrue);
	}
RMBuf* CMBufManager::Alloc(TInt aSize, TInt aMinMBufSize, TInt aMaxMBufSize, TBool aIsAllocPool)
	{
	// check args
	// - regarding use of TInt instead of TUint, refer comments in CMBufPoolManager::AddL
	__ASSERT_ALWAYS(aSize >= 0, Panic(EMBuf_SillyAlloc));
	__ASSERT_DEBUG(aMinMBufSize >= 0, Panic(EMBuf_NegativeMinMBufSize));
	__ASSERT_DEBUG(aMaxMBufSize >= 0, Panic(EMBuf_NegativeMaxMBufSize));
	__ASSERT_DEBUG(aMaxMBufSize >= aMinMBufSize, Panic(EMBuf_MinExceedsMaxMBufSize));

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	TInt bucket = Min(aSize / KBucketSize, KNumBuckets - 1);
#endif
#ifdef _MBUF_TEST
// Silly value here - the point is that the value won't be changed by this, so we can
// emulate what happens if the system repeatedly fails to allocate the memory.
	if (iDbgFailAfter != 0 && --iDbgFailAfter == 0)
		{
#ifdef SYMBIAN_NETWORKING_PERFMETRICS
// REQ7862 fix up the iLock this might be free list lock from pool manager - perhaps all this goes to pool manager
		iLock.Wait();
		++iBuckets[bucket];
		++iNumOOBs;
// REQ7862 fix up the iLock this might be free list lock from pool manager - perhaps all this goes to pool manager
		iLock.Signal();
#endif
		return NULL;
		}
#endif

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	++iBuckets[bucket];
#endif



	return iMBufPoolManager->Alloc(aSize, aMinMBufSize, aMaxMBufSize, aIsAllocPool);
	}

// return a chain of MBufs to the pool
EXPORT_C void CMBufManager::Free(RMBuf* aMBuf)
	{
	aMBuf->Free();
	}

void CMBufManager::CallBackAfterFree()
	{
	if (!iAllocsPending.IsEmpty())
		iFreeCB->CallBack();
	}

TInt CMBufManager::FreeCallBack(TAny* aPtr)
	{
	((CMBufManager*)aPtr)->CompleteAsyncAllocs(EFalse);	// attempt allocation without attempting to allocate any new pools
	return 0;
	}

CDeltaTimer* CMBufManager::Timer()
//
// return context for the global timer.
//
	{
	return Context()->iTimer;
	}

// retrieve free space for all pool chains
EXPORT_C TUint CMBufManager::__DbgGetBufSpace()
    {
#ifdef _MBUF_TEST
    return iMBufPoolManager->__DbgGetBufSpace();
#else
    return 0;
#endif
    }

// get free space for pool chain with matching mbuf size
EXPORT_C TUint CMBufManager::__DbgGetBufSpace(TUint aMBufSize)
    {
#ifdef _MBUF_TEST
    return iMBufPoolManager->__DbgGetBufSpace(aMBufSize);
#else
    aMBufSize = aMBufSize;
    return 0;
#endif
    }

// get used space for all pool chains
EXPORT_C TUint CMBufManager::__DbgGetBufTotal()
    {
#ifdef _MBUF_TEST
    return iMBufPoolManager->__DbgGetBufTotal();
#else
    return 0;
#endif
    }

// get used space for pool chain with matching mbuf size
EXPORT_C TUint CMBufManager::__DbgGetBufTotal(TUint aMBufSize)
    {
#ifdef _MBUF_TEST
    return iMBufPoolManager->__DbgGetBufTotal(aMBufSize);
#else
    aMBufSize = aMBufSize;
    return 0;
#endif
    }

// return the first mbuf in the free list belong to the first chain
EXPORT_C RMBuf* CMBufManager::__DbgMBufChain()
    {
#ifdef _MBUF_TEST
    return iMBufPoolManager->__DbgMBufChain();
#else
    return NULL;
#endif
    }
// return the first mbuf in the free list belonging to the chain of the specified mbuf size
EXPORT_C RMBuf* CMBufManager::__DbgMBufChain(TUint aMBufSize)
    {
#ifdef _MBUF_TEST
    return iMBufPoolManager->__DbgMBufChain(aMBufSize);
#else
    aMBufSize = aMBufSize;
    return NULL;
#endif
    }

// update the max pool limit (debug only) - use the first pool chain if none specified
EXPORT_C void CMBufManager::__DbgSetPoolLimit(TInt aCount)
	{
#ifdef _MBUF_TEST
	iMBufPoolManager->__DbgSetPoolLimit(aCount);
#else
	aCount = aCount;
#endif
	}

// update the max pool limit (debug only) - for the specified mbuf size
EXPORT_C void CMBufManager::__DbgSetPoolLimit(TInt aCount, TUint aMBufSize)
	{
#ifdef _MBUF_TEST
	iMBufPoolManager->__DbgSetPoolLimit(aCount, aMBufSize);
#else
	aCount = aCount;
	aMBufSize = aMBufSize;
#endif
	}

// set a fail allocation count
EXPORT_C void CMBufManager::__DbgSetFailAfter(TInt aCount)
	{
#ifdef _MBUF_TEST
	iDbgFailAfter = aCount;
#else
	aCount = aCount;
#endif
	}

// get the allocation size - note only valid if called from the CMBufManager owner thread
EXPORT_C TInt CMBufManager::__DbgGetHeapSize()
	{
#ifdef _MBUF_TEST
	if (iMBufPoolManager)
		return iMBufPoolManager->BytesAllocated();
	else
		return 0;
#else
    return 0;
#endif
	}


#ifdef SYMBIAN_NETWORKING_PERFMETRICS

TBool CMBufManager::AddToPerfLog(TAny* aSelf, TDes8& aBuffer, TDes8Overflow* aOverflowHandler)
	{
	CMBufManager* self = static_cast<CMBufManager*>(aSelf);
	__ASSERT_COMPILE(KNumBuckets == 13);	// cross-check against below
	_LIT8(KFormat, "MBuf OOB:%u, reqs:%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u");
	aBuffer.AppendFormat(KFormat, aOverflowHandler, self->iNumOOBs, self->iBuckets[0],
		self->iBuckets[1], self->iBuckets[2], self->iBuckets[3], self->iBuckets[4], self->iBuckets[5], self->iBuckets[6],
		self->iBuckets[7], self->iBuckets[8], self->iBuckets[9], self->iBuckets[10], self->iBuckets[11], self->iBuckets[12]);
	return EFalse;
	}

#endif


EXPORT_C RMBufAllocator::RMBufAllocator() 
	: iManager(*CMBufManager::Context()) 
		{}


EXPORT_C TInt RMBufAllocator::BytesAvailable() const
/**
Obtains the total available bytes available in the mbuf system in it's entirety.
@return the number of available bytes within all of the MBuf pools.
*/
    {
	return iManager.BytesAvailable();
	}

EXPORT_C TInt RMBufAllocator::BytesAvailable(TInt aSize) const
/**
Obtains the total bytes available in the pool of the given sized MBuf.
@param aSize one of the sizes returned by RMBufAllocator::NextMBufSize().
@return the number of available bytes within the MBuf pool of MBufs given by the size aSize.
*/
    {
	return iManager.BytesAvailable(aSize);
	}

EXPORT_C TInt RMBufAllocator::NextMBufSize(TInt aSize) const
/**
Returns the first MBuf size that is greater than aSize, returns KErrNotFound if there isn't an MBuf
whose size is larger than aSize. Can be used to find the size of each of the MBuf pools by starting
at 0 and repeatedly passing in the result of the previous call until KErrNotFound occurs.
@param aSize a starting size to search upwards from.
@return the size of the first MBuf whose size is greater than aSize.
*/
    {
	return iManager.NextMBufSize(aSize);
	}

EXPORT_C TInt RMBufAllocator::LargestMBufSize() const
/**
@return the size of the largest MBuf that is registered with the sytem.
*/
    {
	return iManager.LargestMBufSize();
	}

#ifndef __NOT_OWN_MBUFMGR_DLL  // Used by t_esock in the PPP testsuite

#endif

