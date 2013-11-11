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

#if !defined(__MBuf_h__)
#define __MBuf_h__

#include <es_commsbuf.h>
#include <comms-infras/mbufallocator.h>


// iType field of an MBuf
enum TMBufType
	{
	EMBufFree, EMBufData, EMBufHeader
	};

class RMBufPoolChain;

class RMBuf : public RCommsBuf
/**
RMBuf is a thin wrapper over RCommsBuf providing compatibility for existing code.

@publishedPartner
@released
*/
	{
	friend class CMBufPool;
public:
	IMPORT_C void Init(TMBufType aType);
	IMPORT_C RMBuf* Last();
	//
	IMPORT_C static RMBuf* Alloc();
	IMPORT_C static RMBuf* Alloc(TUint aSize);
	IMPORT_C static RMBuf* AllocL();
	IMPORT_C static RMBuf* AllocL(TUint aSize);

	// Methods to avoid calls to thread local storage
	IMPORT_C static RMBuf* Alloc(RMBufAllocator& aRMBufAllocator);
	IMPORT_C static RMBuf* Alloc(TUint aSize, RMBufAllocator& aRMBufAllocator);
	IMPORT_C static RMBuf* AllocL(RMBufAllocator& aRMBufAllocator);
	IMPORT_C static RMBuf* AllocL(TUint aSize, RMBufAllocator& aRMBufAllocator);

	IMPORT_C void Free();

	inline void FillZ();
	//
	inline TUint8* Buffer();
	inline TUint8* Ptr();
	inline TUint8* EndPtr();
	inline RMBuf* Next();
	inline RMBufChain& NextPkt();
	inline TInt Size() const;
	inline TInt Offset() const;
	inline TInt Length() const;
	inline TInt End() const;
	inline TMBufType Type() const;
	inline const TUint8* Buffer() const;
	inline const TUint8* Ptr() const;
	inline const TUint8* EndPtr() const;
	inline const RMBuf* Next() const;
	inline const RMBufChain& NextPkt() const;
	//
	inline void Link(RMBuf* aBuf);
	inline void Unlink();
	inline void LinkPkt(RMBufChain &aChain);
	inline void UnlinkPkt();
	inline void SetType(TMBufType aType);
	inline void SetOffset(TInt aOffset);
	inline void SetLength(TInt aLength);
	inline void SetData(TInt aOffset, TInt aLength);
	inline void AdjustStart(TInt aLen);
	inline void AdjustEnd(TInt aLen);
	inline void Put(TUint8 aVal, TInt aOffset=0);
	inline TUint8 Get(TInt aOffset=0) const;


private:
#ifndef __KERNEL_MODE__
	TAny *operator new ( TUint aSize, TUint8* aPtr );
	void operator delete ( TAny* aPtr, TUint8*  );

// Forbidden ops
	RMBuf();
	RMBuf(const RMBuf &);
	RMBuf& operator=(const RMBuf &);
#else
    RMBuf() : iBuffer(NULL), iSize(0), iNext(NULL), iPoolChain(NULL) {}
#endif
//

private:
	IMPORT_C void __DbgPut(TUint8 aVal, TInt aOffset=0);
	IMPORT_C TUint8 __DbgGet(TInt aOffset=0) const;
	};

class TMBufIter
/**
An iterator to iterate through the RMBuf
@publishedPartner
@released
*/
	{
public:
	TMBufIter();
	inline TMBufIter(RMBuf* aChain);
	inline TMBufIter(RMBufChain& aChain);
	inline TMBufIter& operator =(RMBuf* aChain);
	inline TMBufIter& operator =(RMBufChain& aChain);
	inline RMBuf* operator++(TInt);
	inline operator RMBuf* ();
	inline TBool More();
	inline RMBuf* MBuf();
	//
protected:
	TCommsBufIter iIterator;
	};

#endif // __rmbuf_h__

#if !defined(__MBuf_Ignore_Inlines__)
#define __MBufchain_Ignore_Inlines__
#include <comms-infras/mbufchain.h>
#undef __MBufchain_Ignore_Inlines__
#include <comms-infras/mbuf.inl>
#endif // __rmbuf_Ignore_Inlines__
