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

#if !defined(__MBufPool_h__)
#define __MBufPool_h__

//
// MBUF POOLS
//


class CMBufPool : public CBase
/**
@internalTechnology
*/
	{
private:
	CMBufPool(TInt aMBufSize, TInt aNumMBufs, CMBufMemoryAllocator& aMemoryAllocator);
public:
	~CMBufPool();
	static CMBufPool* New(TInt aMBufSize, TInt aNumMBufs, RMBufPoolChain& aMBufPoolChain, CMBufMemoryAllocator& aMemoryAllocator);

	void Merge(RMBufQ& aMainPool);
    inline TInt Count() const {return iNumMBufs;}
    inline TInt MBufSize() const {return iMBufSize;}
    
    TAny *operator new(TUint aSize, TInt aExtra, CMBufMemoryAllocator& aMemoryAllocator) __NO_THROW;
    
	void operator delete(TAny* aPtr) __NO_THROW;
	void operator delete(TAny* aPtr, TInt aExtra, CMBufMemoryAllocator& aMemoryAllocator) __NO_THROW;
	RMBufQ& MBufQ() {return iList;}
//
private:
	friend class RMBufPoolChain;  // for access to iLink
	
	TDblQueLink iLink;
	RMBufQ iList;		    // List of mbufs in this pool
	TUint8* iPool;		    // Allocated memory
	const TInt iNumMBufs;   // Number of mbufs in this pool
    const TInt iMBufSize;   // size of the mbufs held within this pool
//
	CMBufMemoryAllocator& iMemoryAllocator;
    
public:
	IMPORT_C TInt __DbgCheckBuffer(RMBuf* aBuf);
	};

#endif
