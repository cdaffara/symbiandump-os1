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
// Description:
//

#include "systemsharedbufs.h"
#include "systemsharedasyncalloc.h"
#include "commsbufasyncreqinternal.h"


CSystemSharedPoolAsyncAlloc::CSystemSharedPoolAsyncAlloc(CSystemSharedBufPool& aPool, MAsyncAllocNotifier& aNotifier)
: CActive(EPriorityStandard),
iPool(aPool),
iNotifier(aNotifier)
	{
	CActiveScheduler::Add(this);
	}

CSystemSharedPoolAsyncAlloc::~CSystemSharedPoolAsyncAlloc()
	{
	Deactivate();	
	}
	
void CSystemSharedPoolAsyncAlloc::Activate(TInt aFreeBufs)
	{
	if(!IsActive())
		{
		iStatus = KRequestPending;
		iPool.Pool().RequestFreeSpaceNotification(aFreeBufs, iStatus);
		SetActive();			
		}
	}

void CSystemSharedPoolAsyncAlloc::Deactivate()
	{		
	Cancel();
	}

void CSystemSharedPoolAsyncAlloc::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	iNotifier.OnCompletion();
	}

TInt CSystemSharedPoolAsyncAlloc::RunError(TInt aError)
	{
	iNotifier.OnError(aError);
	return KErrNone;
	}

void CSystemSharedPoolAsyncAlloc::DoCancel()
	{
	iPool.Pool().CancelFreeSpaceNotification(iStatus);
	}

// -------------------------------------------------------------------------

CSystemSharedAsyncAlloc* CSystemSharedAsyncAlloc::New(const RPointerArray<CSystemSharedBufPool>& aPools)
	{
	CSystemSharedAsyncAlloc* self = new CSystemSharedAsyncAlloc();
	if(self)
		{
		if (self->Construct(aPools) == KErrNone)
			{
			return self;
			}
		else
			{
			delete self;	
			}
		}
	return NULL;	
	}

CSystemSharedAsyncAlloc::CSystemSharedAsyncAlloc()
: CActive(EPriorityStandard),
iRequestState(EMakeRequest),
iSignalled(ETrue)
    {
    CActiveScheduler::Add(this);
    }

CSystemSharedAsyncAlloc::~CSystemSharedAsyncAlloc()
	{
	__ASSERT_DEBUG(iAllocsPending.IsEmpty(), User::Invariant());
	Cancel();
	iAsyncAllocPools.ResetAndDestroy();	
	iAsyncAllocPools.Close();
	iAsynAllocLock.Close();
	iOwnerThread.Close();
	}

TInt CSystemSharedAsyncAlloc::Construct(const RPointerArray<CSystemSharedBufPool>& aPools)
	{
    TInt err = KErrNone;
	for(TInt i = 0; i < aPools.Count(); ++i)
		{
		CSystemSharedPoolAsyncAlloc* asyncPool = new CSystemSharedPoolAsyncAlloc(*(aPools[i]), *this);
		if(asyncPool)
		    {
		    if(iAsyncAllocPools.Append(asyncPool) != KErrNone)
		        {
		        delete asyncPool;
		        err = KErrNoMemory;
		        break;
		        }
		    }
		else
		    {
		    err = KErrNoMemory;
		    break;
		    }		
		}
	if(err == KErrNone)
	    {
	    iAllocsPending.SetOffset(_FOFF(CCommsBufAsyncRequest, iLink));      
	    err = iAsynAllocLock.CreateLocal();
	    if(err == KErrNone)
	        {
	        TThreadId id = RThread().Id();
	        err = iOwnerThread.Open(id);
	        if(err == KErrNone)
	            {
	            // Activate the AO. 
                ActivateSelf();
	            }
	        }
	    }
	return err;
	}

void CSystemSharedAsyncAlloc::StartRequest(CCommsBufAsyncRequest& aRequest)
	{
	iAsynAllocLock.Wait();
	if(iAllocsPending.IsEmpty())
	    {
	    // We do not have anything in the pending alloc queue. 
	    // Kick off the AO.
	    CompleteSelf();
	    }
	iAllocsPending.AddLast(aRequest);
	iAsynAllocLock.Signal();        
	}

void CSystemSharedAsyncAlloc::CancelRequest(CCommsBufAsyncRequest& aRequest)
	{
	iAsynAllocLock.Wait();
    aRequest.iLink.Deque();
    aRequest.Complete(KErrCancel); 
    CompleteSelf();
    iAsynAllocLock.Signal();
	}

void CSystemSharedAsyncAlloc::MakePoolAsyncRequest()
    {
    iAsynAllocLock.Wait();
    MakePoolRequest();
    ActivateSelf();
    iAsynAllocLock.Signal();
    }

void CSystemSharedAsyncAlloc::MakePoolRequest()
    {
    if(!iAllocsPending.IsEmpty())
        {
        CCommsBufAsyncRequest* req = iAllocsPending.First();
        for(TInt i = 0; i < iAsyncAllocPools.Count(); ++i)
            {            
            CSystemSharedPoolAsyncAlloc* asyncPool = iAsyncAllocPools[i];
            TInt poolBufSize = asyncPool->iPool.BufSize();
            if(poolBufSize >= req->iMinSize && poolBufSize <= req->iMaxSize)
                {
                TInt freeBufs = req->iSize / poolBufSize;
                freeBufs = ((req->iSize % asyncPool->iPool.BufSize()) == 0) ? freeBufs : (freeBufs + 1);
                asyncPool->Activate(freeBufs);              
                }
            }
        }    
    }

void CSystemSharedAsyncAlloc::CancelAllPoolRequest()
    {
    for(TInt i = 0; i < iAsyncAllocPools.Count(); ++i)
        {
        iAsyncAllocPools[i]->Deactivate();  
        }
    }

void CSystemSharedAsyncAlloc::CancelPoolAsyncRequest()
    {
    iAsynAllocLock.Wait();
    CancelAllPoolRequest();
    ActivateSelf();
    if(!iAllocsPending.IsEmpty())
        {
        CompleteSelf();
        }
    iAsynAllocLock.Signal();
    }

 void CSystemSharedAsyncAlloc::RunL()
     {
     // 
     switch(iRequestState)
         {
         case EMakeRequest:
         MakePoolAsyncRequest();         
         break;
         
         case ECancelRequest:
         CancelPoolAsyncRequest();
         iRequestState = EMakeRequest;
         break;
         
         default:
         // We should never reach here
         CommsBuf::Panic(EMBuf_AsyncAllocInvalidState);
         break;
         }
     
    }
 
 void CSystemSharedAsyncAlloc::DoCancel()
     {
     TRequestStatus* requestStatusPtr = &iStatus;
     iOwnerThread.RequestComplete(requestStatusPtr, KErrCancel);    
     }

void CSystemSharedAsyncAlloc::ActivateSelf()
    {
    if(iSignalled)
        {
        iStatus = KRequestPending;
        SetActive();
        iSignalled = EFalse;
        }
    }
 
void CSystemSharedAsyncAlloc::CompleteSelf()
    {
    if(!iSignalled)
        {
        TRequestStatus* requestStatusPtr = &iStatus;
        iOwnerThread.RequestComplete(requestStatusPtr, KErrNone);
        iSignalled = ETrue;
        }
    }

void CSystemSharedAsyncAlloc::OnCompletion()
    {
    CompleteAsyncAllocs();
    }

void CSystemSharedAsyncAlloc::OnError(TInt aError)
    {
    // We hit with an error from the system pool.
      // complete all alloc request with that error.
      CCommsBufAsyncRequest* req;
      iAsynAllocLock.Wait();
      if(!iAllocsPending.IsEmpty())
          {
          TDblQueIter<CCommsBufAsyncRequest> iter(iAllocsPending);
          while (req = iter++, req != NULL)
              {
              req->Complete(aError);                     
              }          
          }
       iAsynAllocLock.Signal();                  
    }

void CSystemSharedAsyncAlloc::CompleteAsyncAllocs()
    {
    CCommsBufAsyncRequest *req;

    iAsynAllocLock.Wait();
    TDblQueIter<CCommsBufAsyncRequest> iter(iAllocsPending);
    while (req = iter++, req != NULL)
        {
        RCommsBufChain chain;
        chain.Alloc(req->iSize, req->iMinSize, req->iMaxSize, req->iAllocator);
        if(!chain.IsEmpty())
            {
            req->iBufQ.Assign(chain);
            req->Complete(KErrNone);                    
            }
        }
    // We are in the same thread. Cancel the current pool requests.
    CancelAllPoolRequest();
    // Make the pool request if needed.
    MakePoolRequest();
    iAsynAllocLock.Signal();
    }

