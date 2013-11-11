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
// Description: Comms specific specialization of a shared buffer
//

#ifndef __COMMSBUF_H__
#define __COMMSBUF_H__

#ifndef __KERNEL_MODE__
#include <e32base.h>
#include <comms-infras/commsbufchain.h>
#include <comms-infras/commsbufpond.h>
#include <comms-infras/commsbufpanic.h>
#endif

class TCommsBufAllocator;
class MCommsBufManagerIntf;
class CCommsBufPool;
class RCommsBuf;
class TShBuf;

/**
Common (kernel and user-side) CommsBuf meta data.  
@publishedPartner
*/
class TCommsBuf
	{
	friend class RCommsBuf;
	friend class RMBuf;
	friend class DCommsBuf;

private:
	enum {KCanaryDefault = 0xC9C9C9C9};
	enum {KCommsBufMetadataSize  = (16 * sizeof(TInt))};

	inline TCommsBuf();
	inline TCommsBuf(TInt aRawDataOffset, TUint aBufSize, TInt aHandle, TInt aPool);
	inline TCommsBuf(TInt aRawDataOffset, TUint aBufSize, TInt aPool);

	TInt iFrontCanary;	// For overwrite identification
	TInt iOffset;	// The offset of the data within the buffer
	TInt iLength;	// The length of the data
	RCommsBuf* iNext;		// Next RCommsBuf
	TInt iPool;		// Pool identifier that "this" buffer belongs to. Typically a pointer to the CCommsBufPool
	TInt iHandle;	// RShBuf handle. 	
	TInt iRawDataOffset; // Raw data offset from the metadata start (backward), metadata start - Raw data offset = RawBase
	TInt iRawSize;		// The raw size of the buffer
#ifndef __KERNEL_MODE__
	RCommsBufChain iNextPkt;	// Next packet. 
	TUint8 iType; // Type of the MBuf. Used by RMBuf
	TUint8 iReservedBytes[3];
#else
	TShBuf* iShBuf;
	TUint8* iCommsBufPtr;
#endif
	/** This large chunk of reserved words is effectively padding up to 64 bytes, ie a multiple
	of current target cache line length. As the next buffer begins on the next cache line 
	(the space would only then be wasted.), the only conceivable cost is their initialisation. 
	They are initialised in the interests of forward compatibility.*/
	TInt iReservedWords[5];
	TInt iBackCanary;	// For underwrite identification
	};

#ifndef __KERNEL_MODE__
class RCommsBuf
/**
Represents the COMMS specific metadata and payload of a shared buffer

@publishedPartner
@prototype
*/
	{

	friend class RCommsBufChain;
	friend class TCommsBufIter;
	friend class RCommsBufQ;
	friend class CMBufPool;
	friend class RMBufChain;
	friend class CMBufPoolManager;
	friend class CSystemSharedBufPool;
	friend class CSystemSharedBufPond;
	friend class RCommsBufAccessor;

	public:
	IMPORT_C static RCommsBuf* Alloc(TUint aSize, TCommsBufAllocator& aAccessor);

	// Simple access to the data
	inline TPtrC8 DesC8() const;
	/* inline TPtr8 RCommsBuf::Des8()
	This member function was withdrawn because there was no way for changes in the length to be reflected
	in the underlying descriptor, making it likely that bugs arise. Users who need a modifiable descriptor 
	to access the contents of a single RCommsBuf can construct their own, eg with a RCommsBuf "rb": 
		TPtr8 des(cb.Ptr(), cb.Length(), cb.Length() + cb.AppendLimit());
	However they must bear in mind that any change of length of that descriptor will not be reflected in 
	the RCommsBuf, eg could write: 
		UpdateData(des);   // some processing function that needs a TDes8 
		cb.SetDataRange(cb.Offset(), des.Length());
	Or if only a const descriptor is used then use the DesC8() member function instead.
	*/
	
	inline const TUint8* RawBase() const;	
	inline TUint8* RawBase();	
	inline TInt RawSize() const;

	inline const TUint8* Ptr() const;
	inline TUint8* Ptr();
	inline TInt	Length() const;
	inline TInt Offset() const;

	inline TUint8 operator[] (TInt aPos) const;
	inline TUint8& operator[] (TInt aPos);

	inline void Reset();  // Reset to empty buffer ( zero offset to data, zero data length )

	IMPORT_C void Write(const TDesC8& aSrc, TInt aOffset =0);
	IMPORT_C void Read(TDes8& aDest, TInt aOffset =0) const;

	inline TInt AppendLimit() const; // Maximum number of bytes which can be appended
	IMPORT_C void Append(const TDesC8& aSrc);
	
	inline TInt PrependLimit() const; // Maximum number of bytes which can be prepended
	IMPORT_C void Prepend(const TDesC8& aSrc);	

	// Raw manipulation 
	inline void SetDataRange(TInt aOffset, TInt aLength);
	inline void AdjustDataStart(TInt aDelta);
	inline void AdjustDataEnd(TInt aDelta);

	// Free the RCommsBuf
	IMPORT_C void Free();
	
	protected:

	inline RCommsBuf* Next();
	inline const RCommsBuf* Next() const;
	inline void SetNext(RCommsBuf* aBuf);
	inline void SetOffset(TInt aOffset);
	
	inline RCommsBuf();
	inline RCommsBuf(TInt aRawDataOffset, TUint aBufSize, TInt aHandle, TInt aPool);
	inline RCommsBuf(TInt aRawDataOffset, TUint aBufSize, TInt aPool);

	private:

	inline TAny* operator new(TUint aSize, const TUint8* aPtr) __NO_THROW;
	inline void operator delete(TAny* aPtr, const TUint8*) __NO_THROW;
	// Forbidden ops
	RCommsBuf(const RCommsBuf &);
	RCommsBuf& operator=(const RCommsBuf &);
	
	inline CCommsBufPool* Pool() const;
	inline TInt Handle() const;
	
	inline void AssertCommsBufInvariants() const;
	
#define __ASSERT_COMMSBUF() \
    AssertCommsBufInvariants()
	
	protected:
	TCommsBuf	iCommsBufMetaData;
	};


/**
Provides explicit access to special commsbuf fields
@publishedPartner
@prototype
*/
class RCommsBufAccessor
	{
public:
	inline RCommsBufAccessor(const RCommsBuf& aCommsBuf);
	inline TInt Handle() const;

protected:
	const RCommsBuf& iCommsBuf;
	};

#endif // __KERNEL_MODE__
#include <comms-infras/commsbuf.inl>
#endif // __COMMSBUF_H__


