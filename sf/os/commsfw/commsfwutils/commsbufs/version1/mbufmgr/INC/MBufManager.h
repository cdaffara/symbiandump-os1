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

#if !defined(__MBufManager_h__)
#define __MBufManager_h__

#include <comms-infras/mbufsizeallocator.h>	// for MBufSizeAllocator

/**
MBuf Private Heap Limits

@internalTechnology
*/
const TUint KMBufDefaultHeapSize	= 256 * 1024;

class Nif;
class CRequestAsyncAlloc;
class CMBufPoolManager;
class CMBufMemoryAllocator;

class CMBufManager : public CBase
/**
@internalTechnology
*/
	{
friend class Nif;

private:
	CMBufManager();
public:
	IMPORT_C ~CMBufManager();
	IMPORT_C static CMBufManager* NewL(TInt aMaxHeapSize, MMBufSizeAllocator* &aMBufSizeAllocator);

	IMPORT_C RMBuf* AllocL(TInt aSize);
	IMPORT_C RMBuf* Alloc(TInt aSize);

	IMPORT_C void Free(RMBuf* aMBuf);
	IMPORT_C static void Panic(TMBufPanic aPanic);
	
	void CallBackAfterFree();

	void StartRequest(RMBufAsyncRequest& aRequest);
	void CancelRequest(RMBufAsyncRequest& aRequest);

	static CDeltaTimer* Timer();

	// Used to reference the single process-wide MBufManager from different threads+DLLs
	// Don't try to inline these - if TLS is in use then they need to be in this DLL
	IMPORT_C static CMBufManager* Context(); // Panics if no context!
	// A new thread/DLL needs to call this when the TLS handling is in use, ie in the
	// thread startup function, having obtained a pointer to the CMBufManager somehow
	// (eg caller passed in). Consider it as the thread/dll introducing itself to the
	// MBufMgr. "Pleased to meet you..."
	IMPORT_C void SetContext();

	RMBuf* Alloc(TInt aSize, TInt aMinMBufSize);
	RMBuf* Alloc(TInt aSize, TInt aMinMBufSize, TInt aMaxMBufSize);
	RMBuf* Alloc(TInt aSize, TInt aMinMBufSize, TInt aMaxMBufSize, TBool aIsAllocPool);
	RMBuf* Alloc(TInt aSize, const RMBufChain& aMBufChain);

	// Metrics
	TInt BytesAvailable() const;
	TInt BytesAvailable(TInt aSize) const;
	TInt NextMBufSize(TInt aSize) const;
	TInt LargestMBufSize() const;
	
	void WatchDogReset();
	
private:

	void ConstructL(TInt aMaxHeapSize);

	static TInt WatchDogExpire(TAny* aPtr);
	static TInt FreeCallBack(TAny* aPtr);
	void CompleteAsyncAllocs(TBool aIsAllocPool);

private:

	RWorkerLock iAsynAllocLock;

    CMBufPoolManager* iMBufPoolManager;

	TDblQue<RMBufAsyncRequest> iAllocsPending;
	CDeltaTimer* iTimer;					// The global delta timer.
	TThreadId iTimerThreadId;
	TDeltaTimerEntry iWatchDog;
	TBool iWatchDogIsPending;
	CAsyncCallBack* iFreeCB;
	CRequestAsyncAlloc* iRequestAsyncAlloc;

public:
	IMPORT_C static void __DbgCheckChain(RMBuf* aMBuf, TMBufType aType, TInt aLength, TInt aSize);
	IMPORT_C TInt __DbgCheckBuffer(RMBuf* aBuf);
    IMPORT_C RMBuf* __DbgMBufChain();
	IMPORT_C RMBuf* __DbgMBufChain(TUint aMBufSize);
	IMPORT_C void __DbgSetPoolLimit(TInt aCount);
	IMPORT_C void __DbgSetPoolLimit(TInt aCount, TUint aMBufSize);
	IMPORT_C void __DbgSetFailAfter(TInt aCount=0);
	IMPORT_C TUint __DbgGetBufSpace();
	IMPORT_C TUint __DbgGetBufSpace(TUint aMBufSize);
	IMPORT_C TUint __DbgGetBufTotal();
	IMPORT_C TUint __DbgGetBufTotal(TUint aMBufSize);
	IMPORT_C TInt __DbgGetHeapSize();

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
