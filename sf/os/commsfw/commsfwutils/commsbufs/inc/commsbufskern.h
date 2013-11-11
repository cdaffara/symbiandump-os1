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

#ifndef __COMMSBUFS_KERNEL_LIBRARY_H__
#define __COMMSBUFS_KERNEL_LIBRARY_H__


#include <e32cmn.h>

#include <comms-infras/commsbuf.h>
#include <comms-infras/commsbufpond.h>

class TShBuf;

/**
Kernel side representation of commsbuf metadata. Maps on top of fields that user side and kernel side share an interest in.
@publishedPartner
*/
class DCommsBuf : public TCommsBuf
	{
	friend class DCommsPond;

public:
	DCommsBuf()
		{};

	IMPORT_C static TInt AcceptFromClient(DThread* aClient, TInt aHandle, DCommsBuf* aSanitised);
	IMPORT_C TInt TransferToClient(DThread* aClient);
	IMPORT_C void Free();

	inline TInt Length() const;
	inline void SetLength(TInt aLength);

	inline TInt Offset() const;
	inline void SetOffset(TInt aOffset);

	inline TUint8* Ptr() const;

private:
	DCommsBuf(TShBuf* aShBuf, TInt aCommsPoolHandle, TInt aBufSize);
	DCommsBuf(TShBuf* aShBuf, TInt aCommsPoolHandle, TInt aBufSize, TInt aLength);

	inline TAny* operator new(TUint aSize, const TUint8* aPtr);
	inline void operator delete(TAny* aPtr, const TUint8*);
	};


class TCommsPond;
class TPoolRecord;

/**
Kernel side representation of an RCommsPond. A collection of shared buffer pools.
@publishedPartner
*/
class DCommsPond
	{
public:
	IMPORT_C static DCommsPond* New();
	~DCommsPond();

	IMPORT_C TInt Load(TPondTransferBuf& aPondStore, DThread* aClient);
	IMPORT_C void Unload();

	IMPORT_C TInt SetDefaultAllocPool(TInt aRequiredBufferSize);

	IMPORT_C TInt Alloc(DCommsBuf& aBuf);
	IMPORT_C void Free(DCommsBuf& aBuf);

private:
	DCommsPond() : iInitialised(EFalse), iDefaultAllocPool(NULL) {}

	TBool iInitialised;
	TCommsPond* iPond;
	TPoolRecord* iDefaultAllocPool;
	};


/**
Panic codes associated with kernel side commsbufs operations
@publishedPartner
*/
enum TKernCommsBufsPanicCodes
	{
	EPondNotReady = 0,
	EPondAlreadyLoaded = 1,
	EPondCorrupt = 2,
	ECommsBufDimensionsOutOfBounds = 3,
	ECommsBufMetadataCorrupt = 4
	};

/**
Standard panic operations. Encapsulates panic catgory.
@publishedPartner
*/
class KernCommsBuf
	{
public:
	IMPORT_C static void PanicClient(DThread* aClient, TInt aReason);
	IMPORT_C static void Fault(TInt aReason);
	};


#include <comms-infras/commsbufskern.inl>

#endif // __COMMSBUFS_KERNEL_LIBRARY_H__

