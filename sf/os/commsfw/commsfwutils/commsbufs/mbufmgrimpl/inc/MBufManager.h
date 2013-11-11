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
//

/**
@file
@internalTechnology
*/

#if !defined(__MBufManager_h__)
#define __MBufManager_h__

#include "mbufsizeallocator.h"	// for MBufSizeAllocator
#include "commsbufpondintf.h"
#include "commsbufponddbg.h"
#include <es_mbuf.h>
#include <elements/rworkerlock.h>
#include "commsbufasyncreqinternal.h"

class Nif;
class CRequestAsyncAlloc;
class CMBufPoolManager;
class CMBufMemoryAllocator;

NONSHARABLE_CLASS(CMBufManager) : public CBase, public MCommsBufPondIntf, public MCommsBufPondDbg
/**
@file
@internalTechnology
*/
	{
friend class Nif;

private:
	CMBufManager();
public:
	virtual ~CMBufManager();
	IMPORT_C static MCommsBufPondIntf* New(RArray <TCommsBufPoolCreateInfo>& aPoolInfo, RLibrary& aLibrary, TInt aMaxHeapSize);

	RMBuf* AllocL(TInt aSize);
	RMBuf* Alloc(TInt aSize);

	static void Panic(TMBufPanic aPanic);
	
	void CallBackAfterFree();


	static CDeltaTimer* Timer();

	// Used to reference the single process-wide MBufManager from different threads+DLLs
	// Don't try to inline these - if TLS is in use then they need to be in this DLL
	static CMBufManager* Context(); // Panics if no context!

	RMBuf* Alloc(TInt aSize, TInt aMinMBufSize);
	RMBuf* Alloc(TInt aSize, TInt aMinMBufSize, TInt aMaxMBufSize, TBool aIsAllocPool);
	RMBuf* Alloc(TInt aSize, const RMBufChain& aMBufChain);

	virtual RMBuf* Alloc(TInt aSize, TInt aMinBufSize, TInt aMaxBufSize);
	virtual RMBuf* FromHandle(TInt aHandle);
	virtual TInt Store(TDes8& aStore) const;
	virtual void Free(RCommsBuf* aBuf);
	virtual TInt BytesAvailable() const;
	virtual TInt BytesAvailable(TInt aSize) const;
	virtual TInt NextBufSize(TInt aSize) const;
	virtual TInt LargestBufSize() const;
	virtual void StartRequest(CCommsBufAsyncRequest& aRequest);
	virtual void CancelRequest(CCommsBufAsyncRequest& aRequest);
	// A new thread/DLL needs to call this when the TLS handling is in use, ie in the
	// thread startup function, having obtained a pointer to the CMBufManager somehow
	// (eg caller passed in). Consider it as the thread/dll introducing itself to the
	// MBufMgr. "Pleased to meet you..."
	virtual void SetContext();
	virtual void Release(RLibrary& aLib);
	virtual MCommsBufPondDbg& CommsBufPondDbg();
	
	void WatchDogReset();
	
private:
	CMBufManager(RLibrary& aLibrary);
	void ConstructL(RArray <TCommsBufPoolCreateInfo>& aPoolInfo, TInt aMaxHeapSize);
	void ConstructL(TInt aMaxHeapSize);

	static TInt WatchDogExpire(TAny* aPtr);
	static TInt FreeCallBack(TAny* aPtr);
	void CompleteAsyncAllocs(TBool aIsAllocPool);

private:

	RWorkerLock iAsynAllocLock;

    CMBufPoolManager* iMBufPoolManager;

	TDblQue<CCommsBufAsyncRequest> iAllocsPending;
	CDeltaTimer* iTimer;					// The global delta timer.
	TThreadId iTimerThreadId;
	TDeltaTimerEntry iWatchDog;
	TBool iWatchDogIsPending;
	CAsyncCallBack* iFreeCB;
	CRequestAsyncAlloc* iRequestAsyncAlloc;
	RLibrary			iLibrary;
public:
	static void __DbgCheckChain(RMBuf* aMBuf, TMBufType aType, TInt aLength, TInt aSize);
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
	
	TInt __DbgCheckBuffer(RMBuf* aBuf);

private:
	TInt iDbgFailAfter;		// fail allocation request of pool within pool chain

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	static TBool AddToPerfLog(TAny* aSelf, TDes8& aBuffer, TDes8Overflow* aOverflowHandler);

	enum
		{
		KBucketSize = 128,	// Should be a power of 2 for better perf
		KNumBuckets = 13
		};
	TUint iBuckets[KNumBuckets];
	TUint iNumOOBs;
#endif
	};

#endif
