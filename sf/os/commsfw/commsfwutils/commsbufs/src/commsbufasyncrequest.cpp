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

EXPORT_C RCommsBufAsyncRequest::RCommsBufAsyncRequest()
: iAsyncReqImpl(NULL)
/**
The constructor 
*/
	{
	}

EXPORT_C RCommsBufAsyncRequest::RCommsBufAsyncRequest(CCommsBufAsyncRequest* aImpl)
: iAsyncReqImpl(aImpl)
    {
    }

EXPORT_C RCommsBufAsyncRequest::~RCommsBufAsyncRequest()
/**
The destructor
*/
	{
	}

EXPORT_C TInt RCommsBufAsyncRequest::Open(TCommsBufAllocator& aAllocator)
    {
    if(iAsyncReqImpl == NULL)
        {
        iAsyncReqImpl = new CCommsBufAsyncRequest(aAllocator);
        }
    return (iAsyncReqImpl == NULL) ? KErrNoMemory : KErrNone; 
    }

EXPORT_C void RCommsBufAsyncRequest::Close()
    {
    delete iAsyncReqImpl;
    iAsyncReqImpl = NULL;
    }


EXPORT_C void RCommsBufAsyncRequest::Alloc(RCommsBufChain& aChain, TInt aSize, TRequestStatus& aStatus)	
/**
Allocate a RCommsBuf chain asynhronously. 

@param 	aChain 			The chain
@param 	aLength			The length of the chain
@param 	aStatus			The request status
@param	aAllocator		The handle to the allocator
*/
	{
	RCommsBufAsyncRequest::Alloc(aChain, aSize, 0, KMaxTInt, aStatus);	
	}

EXPORT_C void RCommsBufAsyncRequest::Alloc(RCommsBufChain& aChain, TInt aSize, TInt aMinSize, TInt aMaxSize, TRequestStatus& aStatus)   
/**
Allocate a RCommsBuf chain asynhronously. 

@param  aChain          The chain
@param  aLength         The length of the chain
@param  aStatus         The request status
@param  aAllocator      The handle to the allocator
*/
    {
    __ASSERT_ALWAYS(iAsyncReqImpl != NULL, CommsBuf::Panic(EMBuf_InvalidAsyncReqImpl));
	iAsyncReqImpl->Alloc(aChain, aSize, aMinSize, aMaxSize, aStatus);
    }


EXPORT_C void RCommsBufAsyncRequest::Cancel()
/**
Cancel the asynchronous request
*/
	{
	if (iAsyncReqImpl)
		{
		iAsyncReqImpl->Cancel();
		}
	}

EXPORT_C void RCommsBufAsyncRequest::Complete(TInt aCode)
/**
Complete the request.

@param	aCode	The request completion code
*/
	{
	__ASSERT_ALWAYS(iAsyncReqImpl != NULL, CommsBuf::Panic(EMBuf_InvalidAsyncReqImpl));
	iAsyncReqImpl->Complete(aCode);
	}
