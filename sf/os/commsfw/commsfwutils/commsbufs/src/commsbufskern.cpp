// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <comms-infras/commsbufskern.h>
#include "commsbufpond_internal.h"
#include <kernel/kernel.h>

DECLARE_STANDARD_LDD()
	{
	return NULL;
	}


/**
@purpose Fetch a user side handle to underlying shared buffer for a given client and populate commsbuf
metadata ready for transfer to the client. Closes kernel reference to buffer.
*/
EXPORT_C TInt DCommsBuf::TransferToClient(DThread* aClient)
	{
	// Construct the DCommsBuf metadata in the prescribed spot in the buffer
	DCommsBuf* metadataPtr = (DCommsBuf*)iCommsBufPtr;
	*metadataPtr = *this;

	// Initialise the fields that were being used for other purposes kernel side
	metadataPtr->iShBuf = NULL;
	metadataPtr->iCommsBufPtr = NULL;

	// Fetch a handle for the given client we will hand it to and also put the handle in the metadata too
	NKern::ThreadEnterCS();
	TInt result = Kern::ShBufMakeHandleAndOpen(iShBuf, aClient);
	NKern::ThreadLeaveCS();
	
	// Doesn't matter if result is an error code for failed MakeHandle as we close it now anyway
	metadataPtr->iHandle = result;

	// Close _our_ handle to the buffer
	Kern::ShBufClose(iShBuf);

	// We return the handle so the the caller can pass it to the user side client independently
	return result;
	}

/**
Sanitise a buffer arriving from user side and open a reference to it. This involves sanitising the metadata
and copying out a validated version to a kernel private location (aSanitised). That is, check that the fields
that will be accessed kernel side can not cause illegal operations.
@param aClient Client thread that the buffer is being received from.
@param aHandle User side handle to the buffer. Although opaque this is in effect a shbuf handle.
aSanitised Pointer to kernel private memory to take the validated DCommsBuf
@return KErrCorrupt if metadata is unsafe
*/
EXPORT_C TInt DCommsBuf::AcceptFromClient(DThread* aClient, TInt aHandle, DCommsBuf* aSanitised)
	{
	// Open the underlying shbuf buffer
	TShBuf* shbuf;
	NKern::ThreadEnterCS();
	TInt result = Kern::ShBufOpen(shbuf, aClient, aHandle);
	NKern::ThreadLeaveCS();
	if(result != KErrNone)
		{
		// Close the user handle
		NKern::ThreadEnterCS();
		Kern::CloseHandle(aClient, aHandle);
		NKern::ThreadLeaveCS();
		return result;
		}

	TUint8* shBufPtr = Kern::ShBufPtr(shbuf);
	TInt shBufSize = Kern::ShBufSize(shbuf);

	// Copy out first (after all it could be corrupted between checking and copying)
	// The first 8 words are the only ones of interest to us
	TUint8* inPlaceMetadata = (shBufPtr + shBufSize - KCommsBufMetadataSize);
	typedef struct
		{
		TInt iCompulsoryFields[8];
		} TCompulsory;
	*(TCompulsory*)aSanitised = *(TCompulsory*)inPlaceMetadata;

	// Set kernel relevant fields
	aSanitised->iShBuf = shbuf;
	aSanitised->iCommsBufPtr = inPlaceMetadata;

	// Now verify the metadata that matters to us
	TBool valid = EFalse;
#ifndef _DEBUG
	// Speedy UREL checks
	if(((inPlaceMetadata + aSanitised->iOffset) >= shBufPtr)
		&& (aSanitised->iOffset <= 0)
		&& ((aSanitised->iOffset + aSanitised->iLength) <= 0)
		&& (aSanitised->iLength >= 0))
		{
		// Don't bother checking these ones as we can cheaply write them from know values
		aSanitised->iRawSize = (shBufSize - KCommsBufMetadataSize);
		aSanitised->iRawDataOffset = (shBufPtr - inPlaceMetadata);
		valid = ETrue;
		}
#else
	// More prosaic UDEB checks
	if((aSanitised->iFrontCanary == KCanaryDefault)
		&& ((inPlaceMetadata + aSanitised->iOffset) >= shBufPtr)
		&& (aSanitised->iOffset <= 0)
		&& ((aSanitised->iOffset + aSanitised->iLength) <= 0)
		&& (aSanitised->iLength >= 0)
		&& (aSanitised->iRawSize == (shBufSize - KCommsBufMetadataSize))
		&& (aSanitised->iRawDataOffset == (shBufPtr - inPlaceMetadata)))
		{
		valid = ETrue;
		}
#endif

	__ASSERT_DEBUG(valid, KernCommsBuf::Fault(ECommsBufMetadataCorrupt));
	if(!valid)
		{
		// Close the user handle and our kernel reference
		NKern::ThreadEnterCS();
		Kern::CloseHandle(aClient, aHandle);
		NKern::ThreadLeaveCS();
		Kern::ShBufClose(shbuf);
		return KErrCorrupt;
		}

	// Can now close the user side handle
	NKern::ThreadEnterCS();
	Kern::CloseHandle(aClient, aHandle);
	NKern::ThreadLeaveCS();
	
	return KErrNone;
	}

/**
Closes reference to commsbuf
*/
EXPORT_C void DCommsBuf::Free()
	{
	// Close _our_ handle to the buffer
	Kern::ShBufClose(iShBuf);
	}

/**
Constructs DCommsbuf based upon underlying shared buffer.
Should be constructed in kernel private memory not in place in the buffer.
Offset defaults to zero and length to the maximum length
@param aShBuf Underlying shared buffer
@param aCommsPoolHandle Opaque user side handle (as discovered through driver interface)
*/
DCommsBuf::DCommsBuf(TShBuf* aShBuf, TInt aCommsPoolHandle, TInt aBufSize)
	{
	__ASSERT_COMPILE(sizeof(TCommsBuf) == TCommsBuf::KCommsBufMetadataSize);
	__ASSERT_COMPILE(sizeof(DCommsBuf) == sizeof(TCommsBuf));

	__ASSERT_DEBUG(aCommsPoolHandle, KernCommsBuf::Fault(ECommsBufMetadataCorrupt));

	iFrontCanary = KCanaryDefault;
	TUint8* shBufPtr = Kern::ShBufPtr(aShBuf);
	iShBuf = aShBuf;
	iCommsBufPtr = shBufPtr + aBufSize;
	
	iRawDataOffset = -aBufSize;
	iOffset = iRawDataOffset;
	iRawSize = aBufSize;
	iLength = aBufSize;
	iNext = NULL;
	iPool = aCommsPoolHandle;
	iHandle = 0;
	
	iReservedWords[0] = 0;
	iReservedWords[1] = 0;
	iReservedWords[2] = 0;
	iReservedWords[3] = 0;
	iReservedWords[4] = 0;
	iBackCanary = KCanaryDefault;
	}

/**
@purpose Static constructor for DCommsPond
*/
EXPORT_C DCommsPond* DCommsPond::New()
	{
	DCommsPond* self = new DCommsPond();
	if(!self)
		{
		return NULL;
		}
	
	self->iPond = new TCommsPond();
	if(!(self->iPond))
		{
		delete self;
		return NULL;
		}
	else
		{
		return self;
		}
	}

/**
Allocates a commsbuf from the kernel pond. Allocation is from the default shared buffer pool.
The kernel side commsbuf metadata (DCommsBuf) is constructed in-place within the buffer.
This metadata defaults to zero length and zero offset. The user side opaque handle is also set accordingly.
@return KErrNoMemory if no buffers available
*/
EXPORT_C TInt DCommsPond::Alloc(DCommsBuf& aBuf)
	{
	// Pond should have been initialised from comms already
	__ASSERT_DEBUG(iInitialised && iDefaultAllocPool, KernCommsBuf::Fault(EPondNotReady));
	if(!(iInitialised && iDefaultAllocPool))
		{
		return KErrNoMemory;
		}
	
	// Allocate a shbuf from our default pool
	TShBuf* shbuf;
	NKern::ThreadEnterCS();
	TInt result = Kern::ShPoolAlloc(iDefaultAllocPool->iShPool, shbuf, 0);
	NKern::ThreadLeaveCS();
	if(result != KErrNone)
		{
		return KErrNoMemory;
		}

	// Construct the requisite comms metadata in place
	new((TUint8*)(&aBuf)) DCommsBuf(shbuf, iDefaultAllocPool->iCommsOpaqueHandle, iDefaultAllocPool->iCommsBufSize);
	return KErrNone;
	}

/**
@purpose Frees a buffer back to the pond
@param aBuf Commsbuf to be freed
*/
EXPORT_C void DCommsPond::Free(DCommsBuf& aBuf)
	{
	aBuf.Free();
	}

/**
@purpose Loads the pond from a flattened version of it received from comms
@param aPondStore Descriptor containing flattened pond structure
@param aClient The user side thread whose context the pools are already open in
*/
EXPORT_C TInt DCommsPond::Load(TPondTransferBuf& aPondStore, DThread* aClient)
	{
	__ASSERT_DEBUG(!iInitialised, KernCommsBuf::PanicClient(aClient, EPondAlreadyLoaded));
	if(iInitialised)
		{
		return KErrGeneral;
		}

	// Read the pond in from the client
	TPtr8 pondPtr((TUint8*)iPond, sizeof(*iPond));
	TInt r = Kern::ThreadDesRead(aClient, (TAny*)(&aPondStore), pondPtr, 0);
	if(r != KErrNone)
		{
		KernCommsBuf::PanicClient(aClient, EPondCorrupt);
		return r;
		}

	// Sanity check of the pond length
	if((iPond->NumPools() <= 0) || (iPond->NumPools() > TCommsPond::KMaxPoolsPerPond))
		{
#ifdef _DEBUG
		KernCommsBuf::PanicClient(aClient, EPondCorrupt);
#endif
		return KErrCorrupt;
		}
	
	// Open each of the pools - if any can't be opened for any reason then close any opened so far and out of here
	// todo_cdg need complete validation of the pool records
	for(TInt i = 0; i < iPond->NumPools(); i++)
		{
		TPoolRecord* poolRecord = &(iPond->iPoolRecords[i]);

		NKern::ThreadEnterCS();
		TShPool* pool;
		TInt result = Kern::ShPoolOpen(pool, aClient, poolRecord->iCommsShPoolHandle, ETrue, 0);
		NKern::ThreadLeaveCS();
		if(result == KErrNone)
			{
			iPond->iPoolRecords[i].iShPool = pool;
			}
		else
			{
			// Close all pools opened so far and out of here
			for(TInt j = 0; j < i; j++)
				{
				TShPool* poolToClose = iPond->iPoolRecords[j].iShPool;
				Kern::ShPoolClose(poolToClose);
				}
			return result;
			}
		}

	iInitialised = ETrue;
	return KErrNone;
	}

/**
@purpose Unloads the pond. This involves closing all pools and setting the pond to an uninitialised state.
*/
EXPORT_C void DCommsPond::Unload()
	{
	// Close each of the pools and set uninitialised
	TInt numPools = iPond->NumPools();
	for(TInt i = 0; i < numPools; i++)
		{
		TShPool* poolToClose = iPond->iPoolRecords[i].iShPool;
		TInt result = Kern::ShPoolClose(poolToClose);
		}

	iDefaultAllocPool = NULL;
	iInitialised = EFalse;
	}

/**
@purpose Identifies the pool with the smallest buffers just large enough to accomodate a payload of the size given.
The pool selected is then the pool from which default allocations will be performed.
@param aRequiredSize Smallest buffer size acceptable for default allocations from the pond.
*/
EXPORT_C TInt DCommsPond::SetDefaultAllocPool(TInt aRequiredSize)
	{
	__ASSERT_DEBUG(iInitialised, KernCommsBuf::Fault(EPondNotReady));
	__ASSERT_ALWAYS(aRequiredSize >= 0, KernCommsBuf::Fault(EPondNotReady));
	if(!iInitialised)
		{
		return KErrNotReady;
		}

	// Walk our complete list of pools (don't assume they are in any specific order)
	// Find the pool for which the given size will just fit
	TPoolRecord* bestFitPool(NULL);
	TInt bestFit(KMaxTInt32);
	for(TInt i = 0; i < iPond->NumPools(); i++)
		{
		TPoolRecord& currentPool = iPond->iPoolRecords[i];
		TInt currentBufSize = currentPool.iCommsBufSize;
		if(currentBufSize >= aRequiredSize)
			{
			if(currentBufSize < bestFit)
				{
				bestFit = currentBufSize;
				bestFitPool = &currentPool;
				}
			}
		}

	// Set the best fit pool as the default
	if(bestFit != KMaxTInt32)
		{
		iDefaultAllocPool = bestFitPool;
		return KErrNone;
		}

	return KErrNotFound;
	}

EXPORT_C DCommsPond::~DCommsPond()
	{
	delete iPond;
	}

EXPORT_C void KernCommsBuf::PanicClient(DThread* aClient, TInt aReason)
	{
	_LIT(KZCPan, "kernel commsbufs");
	Kern::ThreadKill(aClient, EExitPanic, aReason, KZCPan);
	}

EXPORT_C void KernCommsBuf::Fault(TInt aReason)
	{
	Kern::Fault("kernel commsbufs fault", aReason);
	}

