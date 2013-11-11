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
// Description: Collection of shared buffers and allocator support
// to speed up the allocation 
//

#ifndef __COMMSBUFPOND_H__ 
#define __COMMSBUFPOND_H__

/**
Standard aligned buffer for transfer of pond info between comms and kernel commsbuf drivers
*/
typedef TBuf8<144> TPondTransferBuf;


#ifndef __KERNEL_MODE__

#include <e32base.h>
#include <e32shbuf.h>

class TCommsBufPoolCreateInfo;
class RCommsBuf;
class MCommsBufPondIntf;
class RCommsBufAsyncRequest;
class RCommsBufPond;

/**
TCommsBufAllocator provides a handle to the allocator to speed up the allocation without 
internally accessing the Thread Local Storage. 

@publishedPartner
@prototype
*/
class TCommsBufAllocator
	{
	friend class RCommsBuf;
	friend class RCommsBufChain;
	friend class RCommsBufPond;
	friend class RMBufAllocator;
	friend class RMBufChain;
	friend class RMBuf;
	friend class CCommsBufAsyncRequest;
	
	private:
	inline TCommsBufAllocator(RCommsBufPond aPond);
	MCommsBufPondIntf&	iPond;
	};

class CSystemSharedBufPond;
/**
The RCommsBufPond represents a collection of RShPools. Each pool in the pond has a different
size of buffer. The pond is responsible for creating, allocating and freeing these pools. 

@publishedPartner
@prototype
*/
class RCommsBufPond
	{
	friend class RCommsBufPondInternalOp;
	friend class TCommsBufAllocator;
	friend class RCommsBuf;
	friend class TCommsBufPondTLSOp;
	friend class TCommsBufPondDbg;

	public:
	IMPORT_C RCommsBufPond();

	IMPORT_C RCommsBuf* Alloc(TInt aSize, TInt aMinBufSize, TInt aMaxBufSize);
	IMPORT_C RCommsBuf* FromHandle(TInt aHandle);
	IMPORT_C TInt Store(TPondTransferBuf& aStore) const;

	IMPORT_C TInt BytesAvailable() const;
	IMPORT_C TInt BytesAvailable(TInt aSize) const;
	IMPORT_C TInt NextBufSize(TInt aSize) const;
	IMPORT_C TInt LargestBufSize() const;
	
	IMPORT_C TCommsBufAllocator Allocator();
	
	inline TBool IsNull() const;
	private:
    IMPORT_C void StartRequest(RCommsBufAsyncRequest& aRequest);
    IMPORT_C void CancelRequest(RCommsBufAsyncRequest& aRequest);
	
    CSystemSharedBufPond& Pond() const;
	inline	RCommsBufPond(MCommsBufPondIntf* aPondImpl);
	protected:
	MCommsBufPondIntf* iPondImpl;	// The pond implementation. This can be the legacy MBufMgr or the new system wide
									// shared buffer implementation
	};

#endif // __KERNEL_MODE__

#endif // __COMMSBUFPOND_H__
#include <comms-infras/commsbufpond.inl>
