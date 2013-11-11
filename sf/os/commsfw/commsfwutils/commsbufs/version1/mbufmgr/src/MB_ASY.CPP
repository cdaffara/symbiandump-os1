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
// 
//

/**
 @file
 @internalComponent
*/

#include <es_mbman.h>
#include <cflog.h>

//
// ASYNC ALLOCATOR SUPPORT
//
#ifdef __CFLOG_ACTIVE
__CFLOG_STMT(_LIT8(KSubsysMBufMgrAsyncAlloc, "AsyncAlloc"));
__CFLOG_STMT(_LIT8(KSubsysMBufMgr, "MBufMgr");) // subsystem name
#endif

/**
Constructor inisialiser.
*/
EXPORT_C RMBufAsyncRequest::RMBufAsyncRequest()
: iLength(0),
  iChain(NULL),
  iStatusPtr(NULL),
  iLogger(0)
	{
	iMBufs.Init();
	iLink.iPrev = &iLink;
	iLink.iNext = &iLink;
	}

/**
Destructor.
*/
EXPORT_C RMBufAsyncRequest::~RMBufAsyncRequest()
	{
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
	iLength = aLength;
	iChain = &aChain;
	iStatusPtr = &aStatus;
	*iStatusPtr = KRequestPending;
	// Try to satify the request syncronously
	TInt err = aChain.Alloc(aLength);
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
		__CFLOG_2(KSubsysMBufMgr, KSubsysMBufMgrAsyncAlloc, _L8("RMBufAsyncRequest %x:\tAlloc(aLength %d) starting request"), this, aLength);
#endif
		CMBufManager::Context()->StartRequest(*this);
		}
	else
		{
#ifdef __CFLOG_ACTIVE
		__CFLOG_3(KSubsysMBufMgr, KSubsysMBufMgrAsyncAlloc, _L8("RMBufAsyncRequest %x:\tAlloc(aLength %d) failed with %d"), this, aLength, ret);
#endif
		}
	}

/**
Cancels outstanding request.
*/
EXPORT_C void RMBufAsyncRequest::Cancel()
	{
#ifdef __CFLOG_ACTIVE
	__CFLOG_1(KSubsysMBufMgr, KSubsysMBufMgrAsyncAlloc, _L8("RMBufAsyncRequest %x:\tCancel()"), this);
#endif
	CMBufManager::Context()->CancelRequest(*this);
	}

/**
Completes the request.
@param aCode The completion code (status).
*/
void RMBufAsyncRequest::Complete(TInt aCode)
	{
#ifdef __CFLOG_ACTIVE
	__CFLOG_2(KSubsysMBufMgr, KSubsysMBufMgrAsyncAlloc, _L8("RMBufAsyncRequest %x:\tComplete(aCode %d)"), this, aCode);
#endif
	iLink.Deque();
	if (aCode==KErrNone)
		iChain->Assign(iMBufs);
	else
		iMBufs.Free();
	iThread.RequestComplete(iStatusPtr, aCode);
	iThread.Close();
	}

