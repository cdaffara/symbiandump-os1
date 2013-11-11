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

#if !defined(__MBufChain_h__)
#define __MBufChain_h__

#include <comms-infras/mbufallocator.h>

class RMBuf;

/**
MBUF CHAIN

@internalComponent
*/
#define MBUF_TODATA(chain, type) \
	(chain.Align(sizeof(type)), (type *)chain.First()->Ptr())

/**
Constant to indicate as much as possible, or whole chain
@internalComponent
*/
const TInt KMBufAll = KMaxTInt;

class RMBufChain
/**
@publishedPartner
@released
*/
	{
	__DECLARE_CLEANUP
public:
	inline RMBufChain();
	inline RMBufChain(RMBuf* aChain);
	//
	inline void Init();
	inline TBool IsEmpty() const;
	inline RMBufChain& Next();
	inline const RMBufChain& Next() const;
	inline void Link(RMBufChain& aChain);
	inline void Unlink();
	inline RMBuf* First();
	inline const RMBuf* First() const;
	IMPORT_C RMBuf* Last() const;
	inline RMBufChain &operator =(RMBuf* aChain);
	//
	IMPORT_C RMBuf* Remove();
	IMPORT_C void Assign(RMBufQ& aQueue);
	IMPORT_C void Assign(RMBufChain& aChain);
	IMPORT_C void Append(RMBufChain& aChain);
	IMPORT_C void AppendL(TInt aLen);
	IMPORT_C TInt Append(TInt aLen);
	IMPORT_C TInt Append(TInt aLen, RMBufAllocator &aRMBufAllocator);

	IMPORT_C void PrependL(TInt aLen);
	IMPORT_C TInt Prepend(TInt aLen);
	IMPORT_C TInt Prepend(TInt aLen, RMBufAllocator& aRMBufAllocator);

	IMPORT_C void Prepend(RMBuf* aBuf);
	IMPORT_C void Prepend(RMBufChain& aChain);
	IMPORT_C void AllocL(TInt aLen = 0);
	IMPORT_C TInt Alloc(TInt aLen = 0);
	IMPORT_C TInt Alloc(TInt aLen, TInt aMinMBufSize);
	IMPORT_C TInt Alloc(TInt aLen, TInt aMinMBufSize, TInt aMaxMBufSize);
	IMPORT_C void AllocL(TInt aLen, RMBufAllocator& aRMBufAllocator);
	IMPORT_C TInt Alloc(TInt aLen, RMBufAllocator& aRMBufAllocator);
	IMPORT_C TInt Alloc(TInt aLen, TInt aMinMBufSize, RMBufAllocator& aRMBufAllocator);
	IMPORT_C TInt Alloc(TInt aLen, TInt aMinMBufSize, TInt aMaxMBufSize, RMBufAllocator& aRMBufAllocator);

	IMPORT_C void ReAllocL(TInt aLen);
	IMPORT_C TInt ReAlloc(TInt aLen);
	IMPORT_C void CreateL(const TDesC8& aDes, TInt aHdrLen=0);
	IMPORT_C TInt Create(const TDesC8& aDes, TInt aHdrLen=0);
 	IMPORT_C void SplitL(TInt anOffset, RMBufChain& newChain);
 	IMPORT_C TInt Split(TInt anOffset, RMBufChain& newChain);
 	IMPORT_C TInt Split(TInt anOffset, RMBufChain& newChain, RMBufAllocator& aRMBufAllocator);

	IMPORT_C TInt Align(TInt aSize);
	IMPORT_C void Free();
	IMPORT_C void FillZ(TInt aLen=KMBufAll);
	IMPORT_C void CopyL(RMBufChain& newChain, TInt aOffset=0, TInt aLen=KMBufAll) const;
	IMPORT_C TInt Copy(RMBufChain& newChain, TInt aOffset=0, TInt aLen=KMBufAll) const;
 	IMPORT_C void CopyIn(const TDesC8& aDes, TInt aOffset=0);
	IMPORT_C void CopyOut(TDes8 &aDes, TInt aOffset=0) const;
	IMPORT_C void TrimStart(TInt nBytes);
	IMPORT_C void TrimEnd(TInt anOffset);
	IMPORT_C TBool Goto(TInt anOffset, RMBuf* &resBuf, TInt& resOffset, TInt& resLength, RMBuf* &resPrevBuf) const;
	inline TBool Goto(TInt anOffset, RMBuf* &resBuf, TInt& resOffset, TInt& resLength) const;
	IMPORT_C TInt NumBufs() const;
	IMPORT_C TInt Length() const;
private:
	TInt Alloc(TInt aLen, const RMBufChain& aMBufChain);
	TInt Alloc(TInt aLen, const RMBufChain& aMBufChain, RMBufAllocator& aRMBufAllocator);
protected:
	RMBuf* iNext;
	};

#endif

#if !defined(__MBufChain_Ignore_Inlines__)
#define __MBuf_Ignore_Inlines__
#include <comms-infras/mbuf.h>
#undef __MBuf_Ignore_Inlines__
#include <comms-infras/mbufchain.inl>
#endif // __MBufchain_Ignore_Inlines__

