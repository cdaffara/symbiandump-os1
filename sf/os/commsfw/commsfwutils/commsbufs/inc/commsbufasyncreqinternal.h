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
// Description: Implementation of the asynchronous allocation support for the
// comms buffers
//

#ifndef __COMMSBUFASYNCREQINTERNAL_H__
#define __COMMSBUFASYNCREQINTERNAL_H__

#include <e32base.h>
#include <es_commsbuf.h>
#include <comms-infras/commsbufq.h>
/**
Implementation of the RCommsBufAsyncRequest.
 @internalTechnology
 */
class CCommsBufAsyncRequest : public CBase
    {
    friend class CSystemSharedBufPond;    
    friend class CMBufManager;
    friend class RMBufAsyncRequest;
    friend class RCommsBufAsyncRequest;
    friend class CSystemSharedAsyncAlloc;
    
	public:
    IMPORT_C CCommsBufAsyncRequest(TCommsBufAllocator& aAllocator);
    ~CCommsBufAsyncRequest();
    void Alloc(RCommsBufChain& aChain, TInt aSize, TInt aMinSize, TInt aMaxSize, TRequestStatus& aStatus);
    void Cancel();
    IMPORT_C void Complete(TInt aCode);
    	
    private:    
    TInt                iSize;
    TInt                iMinSize;
    TInt                iMaxSize;
    RCommsBufQ          iBufQ;
    RCommsBufChain*     iChain;
    TRequestStatus*     iStatusPtr;
    RThread             iThread;
    TDblQueLink         iLink;
    TCommsBufAllocator  iAllocator;    
    };

#endif // __COMMSBUFASYNCREQINTERNAL_H__
