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

#include <comms-infras/mbufallocator.h>

// iType field of an MBuf
enum TMBufType
	{
	EMBufFree, EMBufData, EMBufHeader
	};

class RMBufPoolChain;

class RMBuf
/**
MBUF
@publishedPartner
@released
*/
	{
	friend class CMBufPool;				// For access to new operator

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

/**
 * @internalTechnology
 */
	inline RMBufPoolChain* MBufPoolChain(){return iPoolChain;}
	
private:
#ifndef __KERNEL_MODE__
	TAny *operator new(TUint aSize, RMBuf* aPtr);
	void operator delete(TAny* aPtr);
	void operator delete(TAny* aPtr, RMBuf*);
	RMBuf(TUint8* aBuffer, RMBufPoolChain* aPoolChain, TInt aSize);
// Forbidden ops
	RMBuf();
	RMBuf(const RMBuf &);
	RMBuf& operator=(const RMBuf &);
#else
    RMBuf() : iBuffer(NULL), iSize(0), iNext(NULL), iPoolChain(NULL) {}
#endif
//

private:
	TUint8* const iBuffer;  // pointer to the actual buffer
	TInt iOffset;			// the offset to the start of the data within the buffer
	TInt iLength;			// the length of the data within the buffer
	const TInt iSize;       // size of the mbuf
	RMBuf* iNext;			// the next mbuf in the chain
	RMBufChain iNextPkt;
	TUint8 iType;		    // containing type information from the TMBufType enumeration
	TUint8 iRefCount;		// reference counter. Currently unused but ready for future implementation.
	TUint16 iSpare16Bits;	// 16 spare bit for future use
	RMBufPoolChain* const iPoolChain; // reserved for a pointer to the poolchain
//
private:
	IMPORT_C void __DbgPut(TUint8 aVal, TInt aOffset=0);
	IMPORT_C TUint8 __DbgGet(TInt aOffset=0) const;
	};

class TMBufIter
/**
MBUF ITERATOR
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
	RMBuf* iCurrent;
	};

#endif // __rmbuf_h__

#if !defined(__MBuf_Ignore_Inlines__)
#define __MBufchain_Ignore_Inlines__
#include <comms-infras/mbufchain.h>
#undef __MBufchain_Ignore_Inlines__
#include <comms-infras/mbuf.inl>
#endif // __rmbuf_Ignore_Inlines__
