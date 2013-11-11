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
// Description: Asynchronous allocation support to allocate shared buffer(s)
//

#ifndef __COMMSBUFASYNCREQUEST_H__
#define __COMMSBUFASYNCREQUEST_H__

#include <e32base.h>

#include <es_commsbuf.h>
#include <comms-infras/commsbufq.h>

class RCommsBufChain;
class CCommsBufAsyncRequest;
/**
Class that specialises the allocation of RCommsBuf asynchronously.

@publishedPartner
*/
class RCommsBufAsyncRequest
	{
    friend class CSystemSharedBufPond;  
	friend class RCommsBufPond;
	friend class CMBufManager;
	friend class RMBufAsyncRequest;
	public:
	IMPORT_C RCommsBufAsyncRequest();
	IMPORT_C ~RCommsBufAsyncRequest();
    IMPORT_C TInt Open(TCommsBufAllocator& aAllocator);
    IMPORT_C void Close();
	IMPORT_C void Alloc(RCommsBufChain& aChain, TInt aSize, TRequestStatus& aStatus);
	IMPORT_C void Alloc(RCommsBufChain& aChain, TInt aSize, TInt aMinSize, TInt aMaxSize, TRequestStatus& aStatus);
	IMPORT_C void Cancel();
	
	protected:
	IMPORT_C void Complete(TInt aCode);	
	
	private:
    IMPORT_C RCommsBufAsyncRequest(CCommsBufAsyncRequest* aImpl);
	private:
	CCommsBufAsyncRequest*     iAsyncReqImpl;
	};

#endif // __COMMSBUFASYNCREQUEST_H__
