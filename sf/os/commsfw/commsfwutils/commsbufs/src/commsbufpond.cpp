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
#include <comms-infras/commsbufasyncrequest.h>
#include "commsbufpondintf.h"

EXPORT_C RCommsBufPond::RCommsBufPond()
:iPondImpl(NULL)
/**
The constructor
*/
	{
		
		
	}

EXPORT_C RCommsBuf* RCommsBufPond::Alloc(TInt aSize, TInt aMinBufSize, TInt aMaxBufSize)
/**
Allocates memory for a RCommsBuf object. More than one buffer may be allocated and linked, 
if the requested buffer size doesn't directly satisfy with the configured buffer sizes

@param 	aSize 		The requested total size
@param 	aMinBufSize The size that the allocated buffer must atleast have
@param 	aMinBufSize The size that the allocated buffer can have

@return Allocated RCommsBuf pointer on success otherwise NULL
*/
	{
	return iPondImpl->Alloc(aSize, aMinBufSize, aMaxBufSize);	
	}

EXPORT_C TInt RCommsBufPond::BytesAvailable() const
/**
Returns the total bytes available in the available pools
*/
	{
	return iPondImpl->BytesAvailable();
	}
	
EXPORT_C TInt RCommsBufPond::BytesAvailable(TInt aSize) const
/**
Returns the total bytes available in a pool with a given size.
*/
	{
	return iPondImpl->BytesAvailable(aSize);	
	}
	
EXPORT_C TInt RCommsBufPond::NextBufSize(TInt aSize) const
/**
Returns the buffer size that is greater than the given size.
*/
	{
	return iPondImpl->NextBufSize(aSize);		
	}
	
EXPORT_C TInt RCommsBufPond::LargestBufSize() const
/**
Larget buffer size available in the available pools
*/
	{
	return iPondImpl->LargestBufSize();	
	}

EXPORT_C TCommsBufAllocator RCommsBufPond::Allocator()
/**
Returns a handle to the allocator
*/
	{
	return TCommsBufAllocator(*this);		
	}

EXPORT_C void RCommsBufPond::StartRequest(RCommsBufAsyncRequest& aRequest)
/**
Issue an asynchronous request to create a RCommsBufChain

@param aRequest	A new request
*/
	{
	iPondImpl->StartRequest(*(aRequest.iAsyncReqImpl));		
	}

EXPORT_C void RCommsBufPond::CancelRequest(RCommsBufAsyncRequest& aRequest)
/**
Cancel the issued request

@param aRequest	A  request
*/
	{
	iPondImpl->CancelRequest(*(aRequest.iAsyncReqImpl));				
	}


EXPORT_C RCommsBuf* RCommsBufPond::FromHandle(TInt aHandle)
/**
Constructs the RCommsBuf from the supplied handle

TInt aHandle RShBuf handle
*/
	{
	return iPondImpl->FromHandle(aHandle);
	}


/**
@purpose Writes flattened pond structure to a descriptor for transfer to a commsbufs aware driver
@param aStore Descriptor in to which the pond structure will be flattened
*/
EXPORT_C TInt RCommsBufPond::Store(TPondTransferBuf& aStore) const
	{
	// Just forward to the implementation of the pond
	return iPondImpl->Store(aStore);
	}

