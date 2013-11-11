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
//

/**
@file

*/

#if !defined(__MBufAsyncRequest_h__)
#define __MBufAsyncRequest_h__

#define ALIGN_MASK    0x03
#define ALIGN_UP(n)   (((n)+(ALIGN_MASK)) & ~ALIGN_MASK)
#define ALIGN_DOWN(n) ((n) & ~ALIGN_MASK)
#define IS_ALIGNED(p) ((((TUint32)(p)) & ALIGN_MASK)==0)

#include <comms-infras/commsbufasyncrequest.h>

class CCommsBufAsyncRequest;
#ifndef __KERNEL_MODE__
class CMBufAsyncReq;
class RMBufAsyncRequest : public RCommsBufAsyncRequest
/**
ASYNC ALLOCATOR SUPPORT
@publishedPartner
@released
*/
	{
	friend class CMBufManager;	// for access to iLink
public:
	IMPORT_C RMBufAsyncRequest();
	IMPORT_C ~RMBufAsyncRequest();
	IMPORT_C void Alloc(RMBufChain& aChain, TInt aLength, TRequestStatus& aStatus);
	IMPORT_C void Cancel();
	IMPORT_C RMBufAsyncRequest(const RMBufAsyncRequest& aObj);
	IMPORT_C RMBufAsyncRequest& operator=(const RMBufAsyncRequest& aRhs);
protected:
	IMPORT_C void Complete(TInt aCode);
	IMPORT_C TInt* Length();
	IMPORT_C RMBufQ* MBufQ();
	IMPORT_C RMBufChain* MBufChain();
	IMPORT_C TRequestStatus* RequestStatusPtr();
	IMPORT_C RThread* Thread();
	IMPORT_C TDblQueLink* DblQueLink();
	};
#endif // __KERNEL_MODE__

#endif // __MBufAsyncRequest_h__
