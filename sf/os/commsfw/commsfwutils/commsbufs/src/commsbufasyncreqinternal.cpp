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

#include "es_commsbuf_internal.h"
#include <comms-infras/commsbufpondop.h>
#include "commsbufasyncreqinternal.h"
#include "commsbufpondintf.h"
 
EXPORT_C CCommsBufAsyncRequest::CCommsBufAsyncRequest(TCommsBufAllocator& aAllocator)
:iAllocator(aAllocator)
    {
    iLink.iPrev = &iLink;
    iLink.iNext = &iLink;    
    }
 
EXPORT_C CCommsBufAsyncRequest::~CCommsBufAsyncRequest()
    {
    
    }

void CCommsBufAsyncRequest::Alloc(RCommsBufChain& aChain, TInt aSize, TInt aMinSize, TInt aMaxSize, TRequestStatus& aStatus)
    {
    iSize = aSize;
    iMinSize = aMinSize;
    iMaxSize = aMaxSize;
    iChain = &aChain;
    iStatusPtr = &aStatus;
    *iStatusPtr = KRequestPending;
    // Try to satify the request syncronously
    TInt err = aChain.Alloc(iSize, iAllocator);
    if (err==KErrNone)
        {
#ifdef __CFLOG_ACTIVE
        __CFLOG_2(KSubsysMBufMgr, KSubsysMBufMgrAsyncAlloc, _L8("RMBufAsyncRequest %x:\tAlloc(aLength %d) request satisfied immediately"), this, aLength);
#endif
        User::RequestComplete(iStatusPtr, KErrNone);
        return;
        }

    // Sync alloc failed, so setup the async alloc request
    TThreadId id = RThread().Id();
    TInt ret = iThread.Open(id);
    if(ret == KErrNone)
        {
#ifdef __CFLOG_ACTIVE
        __CFLOG_2(KSubsysMBufMgr, KSubsysMBufMgrAsyncAlloc, _L8("RCommsBufAsyncRequest %x:\tAlloc(aLength %d) starting request"), this, aSize);
#endif
        iAllocator.iPond.StartRequest(*this);
        }
    else
        {
#ifdef __CFLOG_ACTIVE
        __CFLOG_3(KSubsysMBufMgr, KSubsysMBufMgrAsyncAlloc, _L8("RCommsBufAsyncRequest %x:\tAlloc(aLength %d) failed with %d"), this, aSize, ret);
#endif
        }    
    }

void CCommsBufAsyncRequest::Cancel()
    {
    iAllocator.iPond.CancelRequest(*this);
    }
    
EXPORT_C void CCommsBufAsyncRequest::Complete(TInt aCode)
    {
#ifdef __CFLOG_ACTIVE
    __CFLOG_2(KSubsysMBufMgr, KSubsysMBufMgrAsyncAlloc, _L8("RCommsBufAsyncRequest %x:\tComplete(aCode %d)"), this, aCode);
#endif
    iLink.Deque();
    if (aCode==KErrNone)
        {
        // In future we may have another smarter way of doing this.
        // Currently RCommsBufQ is internal
        RCommsBufChain chain(iBufQ.First());       
        iChain->Assign(chain);
        iBufQ.Init();
        }
    else
        {
        iBufQ.Free();  
        }
        
    iThread.RequestComplete(iStatusPtr, aCode);
    iThread.Close();  
    }

