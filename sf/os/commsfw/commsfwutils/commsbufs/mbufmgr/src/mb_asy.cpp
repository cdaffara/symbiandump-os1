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
// Buffer Manager for Protocols(ASync alloc)

/**
@file

Buffer Manager for Protocols(ASync alloc)

@internalComponent
*/

#include <es_mbuf.h>
#include <cflog.h>
#include <comms-infras/commsbufpond.h>
#include <comms-infras/commsbufpondop.h>
#include "commsbufasyncreqinternal.h"
//
// ASYNC ALLOCATOR SUPPORT
//
/**
Constructor inisialiser.
*/
EXPORT_C RMBufAsyncRequest::RMBufAsyncRequest()
: RCommsBufAsyncRequest()
	{
	}

/**
Destructor.
*/
EXPORT_C RMBufAsyncRequest::~RMBufAsyncRequest()
	{
	RCommsBufAsyncRequest::Close();
	}

/**
Allocates memory for a MBuf Chain
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aChain The chain
@param aLenth The length of the chain
@param aStatus KErrNone if successful.
*/
EXPORT_C void RMBufAsyncRequest::Alloc(RMBufChain& aChain, TInt aLength, TRequestStatus& aStatus)
	{
	TInt err = KErrNone;
	if(iAsyncReqImpl == NULL)
	    {
	    TCommsBufAllocator allocator = TCommsBufPondTLSOp::Get().Allocator();
	    err = RCommsBufAsyncRequest::Open(allocator);    
	    }
	 
	if(err != KErrNone)
	    {
	    TRequestStatus* reqStatusPtr = &aStatus;
	    User::RequestComplete(reqStatusPtr, err);	    
	    }
	else
	    {
        RCommsBufAsyncRequest::Alloc(aChain, aLength, aStatus);
        if(aStatus.Int() == KErrNone)
            {
            // Our allocation is success. Close the implementation.
            RCommsBufAsyncRequest::Close();
            }	    
	    }	
	}

/**
Cancels outstanding request.
*/
EXPORT_C void RMBufAsyncRequest::Cancel()
	{
	RCommsBufAsyncRequest::Cancel();
	}

/**
Completes the request.
@param aCode The completion code (status).
*/
EXPORT_C void RMBufAsyncRequest::Complete(TInt aCode)
	{
	RCommsBufAsyncRequest::Complete(aCode);
	}
/**
 Copy constructor
 */
EXPORT_C RMBufAsyncRequest::RMBufAsyncRequest(const RMBufAsyncRequest& /* aObj */)
    {
    // Note: We do not copy anything here. Note that we are deleting the implementation in
    // the d'tor. So we shouldn't allow to do a copy or assignment of our object.
    }

/**
Assignment operator.
 */
EXPORT_C RMBufAsyncRequest& RMBufAsyncRequest::operator=(const RMBufAsyncRequest& /* aRhs */)
    {
    // See note on the copy c'tor.
    return *this;
    }

EXPORT_C TInt* RMBufAsyncRequest::Length()
    {
    if(iAsyncReqImpl)
        {
        return &iAsyncReqImpl->iSize;
        }
    return NULL;
    }

EXPORT_C RMBufQ* RMBufAsyncRequest::MBufQ()
    {
    if(iAsyncReqImpl)
         {
         return static_cast<RMBufQ*>(&iAsyncReqImpl->iBufQ);
         }
     return NULL;        
    }

EXPORT_C RMBufChain* RMBufAsyncRequest::MBufChain()
    {
    if(iAsyncReqImpl)
        {
        return static_cast<RMBufChain*>(iAsyncReqImpl->iChain);
        }
    return NULL;          
    }

EXPORT_C TRequestStatus* RMBufAsyncRequest::RequestStatusPtr()
    {
    if(iAsyncReqImpl)
        {
        return iAsyncReqImpl->iStatusPtr;
        }
    return NULL;            
    }

EXPORT_C RThread* RMBufAsyncRequest::Thread()
    {
    if(iAsyncReqImpl)        
        {
        return &iAsyncReqImpl->iThread;
        }
    return NULL;   
    }

EXPORT_C TDblQueLink* RMBufAsyncRequest::DblQueLink()
    {
    if(iAsyncReqImpl)        
         {
         return &iAsyncReqImpl->iLink;
         }
     return NULL;   
    }

// -----------------------------------------------------------------------
