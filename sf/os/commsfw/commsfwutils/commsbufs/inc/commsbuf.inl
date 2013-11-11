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
// Description:
//

inline TCommsBuf::TCommsBuf()
{
}

#ifndef __KERNEL_MODE__

inline TCommsBuf::TCommsBuf(TInt aRawDataOffset, TUint aBufSize, TInt aHandle, TInt aPool) :
	iFrontCanary(KCanaryDefault),
	iLength(aBufSize),
	iOffset(aRawDataOffset),
	iPool(aPool),
	iHandle(aHandle),
	iRawDataOffset(aRawDataOffset),
	iRawSize(aBufSize),
	iNext(NULL),
	iBackCanary(KCanaryDefault)
	{
	iType = 0,
	iReservedBytes[0] = 0;
	iReservedBytes[1] = 0;
	iReservedBytes[2] = 0;
	iReservedWords[0] = 0;
	iReservedWords[1] = 0;
	iReservedWords[2] = 0;
	iReservedWords[3] = 0;
	iReservedWords[4] = 0;
	}

inline TCommsBuf::TCommsBuf(TInt aRawDataOffset, TUint aBufSize, TInt aPool) :
	iFrontCanary(KCanaryDefault),
	iLength(aBufSize),
	iOffset(aRawDataOffset),
	iPool(aPool),
	iHandle(0),
	iRawDataOffset(aRawDataOffset),
	iRawSize(aBufSize),
	iNext(NULL),
	iBackCanary(KCanaryDefault)
	{
	iType = 0,
	iReservedBytes[0] = 0;
	iReservedBytes[1] = 0;
	iReservedBytes[2] = 0;
	iReservedWords[0] = 0;
	iReservedWords[1] = 0;
	iReservedWords[2] = 0;
	iReservedWords[3] = 0;
	iReservedWords[4] = 0;
	}

inline TAny* RCommsBuf::operator new(TUint /* aSize */, const TUint8* aPtr) __NO_THROW
	{
	__ASSERT_COMPILE(sizeof(RCommsBuf) == TCommsBuf::KCommsBufMetadataSize);
	return (TUint8*)aPtr;
	}

inline void RCommsBuf::operator delete(TAny* /* aPtr */, const TUint8*) __NO_THROW
	{		
	}

inline RCommsBuf::RCommsBuf()
	{
	}

inline RCommsBuf::RCommsBuf(TInt aRawDataOffset, TUint aBufSize, TInt aHandle, TInt aPool)
: iCommsBufMetaData(aRawDataOffset, aBufSize, aHandle, aPool)
	{
	}

inline RCommsBuf::RCommsBuf(TInt aRawDataOffset, TUint aBufSize, TInt aPool)
: iCommsBufMetaData(aRawDataOffset, aBufSize, aPool)
	{		
	}


inline TPtrC8 RCommsBuf::DesC8() const
/**
Creates and returns a non-modifiable pointer descriptor representing the contained data

@return non-modifiable pointer descriptor
*/
	{
	__ASSERT_COMMSBUF();
	return TPtrC8((TUint8*)(this) + iCommsBufMetaData.iOffset, iCommsBufMetaData.iLength);
	}

inline const TUint8* RCommsBuf::RawBase() const
/**
Returns a const ponter to the start of the data represented by the RCommsBuf

 @return A const pointer
*/
	{
	__ASSERT_COMMSBUF();
	return ((TUint8*)this + iCommsBufMetaData.iRawDataOffset);
	}

inline TUint8* RCommsBuf::RawBase()
/**
Returns a ponter to the start of the data represented by the RCommsBuf

 @return A pointer
*/
	{
	__ASSERT_COMMSBUF();
	return ((TUint8*)this + iCommsBufMetaData.iRawDataOffset);
	}

inline TInt RCommsBuf::RawSize() const
/**
Returns the raw size of the RCommsBuf data

  @return the size of the RCommsBuf data
*/
	{
	return iCommsBufMetaData.iRawSize;
	}

inline const TUint8* RCommsBuf::Ptr() const
/**
Returns a const pointer to the actual location of the data start. 
Actual data start location may differ from the start of the data 

@see RCommsBuf::RawBase
@return A const pointer
*/
	{
	__ASSERT_COMMSBUF();
	return ((TUint8*)(this) + iCommsBufMetaData.iOffset);
	}

inline TUint8* RCommsBuf::Ptr()
/**
Returns a pointer to the actual location of the data start. 

@see RCommsBuf::RawBase
@return A pointer
*/
	{
	__ASSERT_COMMSBUF();
    return ((TUint8*)(this) + iCommsBufMetaData.iOffset);
	}


inline TInt	RCommsBuf::Length() const
/**
Returns the length of the RCommsBuf data

  @return the length of the RCommsBuf data
*/
	{
	return iCommsBufMetaData.iLength;
	}

inline TInt RCommsBuf::Offset() const
/**
Returns the offset of the RCommsBuf data

  @return the offset of the RCommsBuf data
*/
	{
	return iCommsBufMetaData.iOffset - iCommsBufMetaData.iRawDataOffset;
	}

inline void RCommsBuf::SetOffset(TInt aOffset)
/**
Sets the RCommsBuf data offset

@param aOffset  An offset
 */
	{
	__ASSERT_ALWAYS((aOffset >= 0) && ((aOffset + iCommsBufMetaData.iLength) <= iCommsBufMetaData.iRawSize), CommsBuf::Panic(EMBuf_NegativeOffset));
	iCommsBufMetaData.iOffset = iCommsBufMetaData.iRawDataOffset + aOffset;
	}

inline TUint8 RCommsBuf::operator[] (TInt aPos) const
	{
	__ASSERT_COMMSBUF();
    __ASSERT_ALWAYS(aPos >= 0 && aPos < Length(), CommsBuf::Panic(EMBuf_IndexOutofRange));
	return (Ptr()[aPos]);
	}

inline TUint8& RCommsBuf::operator[] (TInt aPos)
	{
	__ASSERT_COMMSBUF();
    __ASSERT_ALWAYS(aPos >= 0 && aPos < Length(), CommsBuf::Panic(EMBuf_IndexOutofRange));
	return (((TUint8*)Ptr())[aPos]);
	}

inline void RCommsBuf::Reset()
/**
Resets the length and offset of the RCommsBuf data
*/
	{
	iCommsBufMetaData.iLength = 0;
	iCommsBufMetaData.iOffset = iCommsBufMetaData.iRawDataOffset;
	}


inline TInt RCommsBuf::AppendLimit() const
/**
Returns the length of the bytes that can be appended
*/
	{
    return iCommsBufMetaData.iRawDataOffset + iCommsBufMetaData.iRawSize - iCommsBufMetaData.iOffset - iCommsBufMetaData.iLength;
	}

inline TInt RCommsBuf::PrependLimit() const
/**
Returns the length of the data that can be prepended
*/
	{
	return iCommsBufMetaData.iOffset - iCommsBufMetaData.iRawDataOffset;
	}

inline void RCommsBuf::SetDataRange(TInt aOffset, TInt aLength)
/**
Sets the offset and length of the data

@param aOffset An offset
@param aLength A length
*/
	{
	__ASSERT_ALWAYS((aOffset >= 0) && (aLength >= 0) && ((aOffset + aLength) <= iCommsBufMetaData.iRawSize), CommsBuf::Panic(EMBuf_NegativeOffset));
	iCommsBufMetaData.iOffset = iCommsBufMetaData.iRawDataOffset + aOffset;
	iCommsBufMetaData.iLength = aLength;
	}

inline void RCommsBuf::AdjustDataStart(TInt aDelta)
/**
Adjust the RCommsBuf data offset and length with the given delta value.
ie; the data offset is incremented by the given delta value and the data 
length is decremented by the given delta value
*/
	{
	iCommsBufMetaData.iOffset += aDelta;
	iCommsBufMetaData.iLength -= aDelta;
	__ASSERT_ALWAYS((iCommsBufMetaData.iOffset >= iCommsBufMetaData.iRawDataOffset) && (iCommsBufMetaData.iLength >= 0), CommsBuf::Panic(EMBuf_InvalidLengthOrOffset));
	}

inline void RCommsBuf::AdjustDataEnd(TInt aDelta)
/**
Adjust the RCommsBuf data length with the given delta value. 
ie; the data length is incremented by the given value
*/
	{
	iCommsBufMetaData.iLength += aDelta;
	__ASSERT_ALWAYS((iCommsBufMetaData.iLength >= 0) && ((Offset() + iCommsBufMetaData.iLength) <= iCommsBufMetaData.iRawSize), CommsBuf::Panic(EMBuf_InvalidLengthOrOffset));
	}

inline void RCommsBuf::SetNext(RCommsBuf* aBuf)
	{
	iCommsBufMetaData.iNext = aBuf;		
	}

inline RCommsBuf* RCommsBuf::Next()
	{
	return iCommsBufMetaData.iNext;		
	}

inline const RCommsBuf* RCommsBuf::Next() const
	{
	return iCommsBufMetaData.iNext;	
	}

inline CCommsBufPool* RCommsBuf::Pool() const	
	{
	return reinterpret_cast<CCommsBufPool*>(iCommsBufMetaData.iPool);
	}

inline TInt RCommsBuf::Handle() const
	{
	return iCommsBufMetaData.iHandle;
	}

inline void RCommsBuf::AssertCommsBufInvariants() const
    {
    __ASSERT_ALWAYS(iCommsBufMetaData.iFrontCanary == TCommsBuf::KCanaryDefault && iCommsBufMetaData.iBackCanary == TCommsBuf::KCanaryDefault, CommsBuf::Panic(EMBuf_CorruptMBuf));
    }


RCommsBufAccessor::RCommsBufAccessor(const RCommsBuf& aCommsBuf) : iCommsBuf(aCommsBuf)
	{
	}

TInt RCommsBufAccessor::Handle() const
	{
	return iCommsBuf.Handle();
	}


#endif // __KERNEL_MODE__



