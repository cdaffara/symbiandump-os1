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
// Description: Asynchronous allocation from the system shared buffer pool
//

#ifndef __SYSTEMSHAREDASYNCALLOC_H__
#define __SYSTEMSHAREDASYNCALLOC_H__

#include <es_commsbuf.h>
#include <elements/rworkerlock.h>

/**
Notifies the asyncronous request completion or error

@internalTechnology
*/
class MAsyncAllocNotifier 
	{
	public:
	virtual void OnCompletion() =0;
	virtual void OnError(TInt aError) =0;
	};

class CSystemSharedBufPool;

/**
Asynchronous allocation from the single system shared pool  

@internalTechnology

*/
NONSHARABLE_CLASS(CSystemSharedPoolAsyncAlloc) : public CActive
	{
	friend class CSystemSharedAsyncAlloc;
	public:
	CSystemSharedPoolAsyncAlloc(CSystemSharedBufPool& aPool, MAsyncAllocNotifier& aNotifer);	
	~CSystemSharedPoolAsyncAlloc();
	
	void Activate(TInt aFreeBufs);
	void Deactivate();
	private:
	// From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	
	private:
	CSystemSharedBufPool&	iPool;
	MAsyncAllocNotifier&	iNotifier;
	};

class CSystemSharedBufPool;
/**
Manages the asynchronous allocation from the system shared pools.
@internalTechnology

*/
NONSHARABLE_CLASS(CSystemSharedAsyncAlloc) : public CActive, public MAsyncAllocNotifier
	{
	enum TAsyncRequestState
	    {
	    EMakeRequest = 0,
	    ECancelRequest,
	    };
	
	public:
	static CSystemSharedAsyncAlloc* New(const RPointerArray<CSystemSharedBufPool>& aPools);
	~CSystemSharedAsyncAlloc();
	void StartRequest(CCommsBufAsyncRequest& aRequest);
	void CancelRequest(CCommsBufAsyncRequest& aRequest);
	
	private:
	CSystemSharedAsyncAlloc();
	TInt Construct(const RPointerArray<CSystemSharedBufPool>& aPools);	
	void MakePoolAsyncRequest();
	void MakePoolRequest();
	void CancelPoolAsyncRequest();
	void CancelAllPoolRequest();
	
	void ActivateSelf();
	void CompleteSelf();
	void CancelSelf();
	
   // From MAsyncAllocNotifier
    virtual void OnCompletion();
    virtual void OnError(TInt aError);  

    // From CActive
    void RunL();
    void DoCancel();

    void CompleteAsyncAllocs(); 
    
	
	private:
	RPointerArray<CSystemSharedPoolAsyncAlloc> 	iAsyncAllocPools;
	TAsyncRequestState                  iRequestState;
    RThread                             iOwnerThread;
    TBool                               iSignalled;
    RWorkerLock                         iAsynAllocLock;
    TDblQue<CCommsBufAsyncRequest>      iAllocsPending;     
	};

#endif // __SYSTEMSHAREDASYNCALLOC_H__
