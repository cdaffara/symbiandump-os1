/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Buffer Manager for Protocols
* 
*
*/



/**
 @file
*/

#if !defined(__MBuf_inl__)
#define __MBuf_inl__

////////////////////////////////////////////////////////////////////////////////
// MBUF
////////////////////////////////////////////////////////////////////////////////

#ifndef __KERNEL_MODE__
/**
Fills with binary zeroes (i.e. 0x00), replacing any existing content.
*/
inline void RMBuf::FillZ()
	{ Mem::FillZ(iBuffer, iSize); }

/**
Returns the size of the MBuf
@return the size
*/
inline TInt RMBuf::Size() const
	{ return iSize; }

/**
Returns a pointer to the MBuf data buffer
@return A pointer to the MBuf data buffer
*/
inline TUint8* RMBuf::Buffer()
	{ return iBuffer; }

/**
Returns a TUnit8 pointer to the begining of MBuf
@return A pointer to the begining of MBuf
*/
inline TUint8* RMBuf::Ptr()
	{ return iBuffer+iOffset; }

/**
Returns a TUint8 pointer to the end of the MBuf
@return A pointer to the end of the MBuf
*/
inline TUint8* RMBuf::EndPtr()
	{ return Ptr()+iLength; }

/**
Returns a const pointer to the MBuf data buffer
@return A const pointer to the MBuf data buffer
*/
inline const TUint8* RMBuf::Buffer() const
	{ return iBuffer; }

/**
Returns a const TUnit8 pointer to the begining of MBuf
@return A const pointer to the begining of MBuf
*/
inline const TUint8* RMBuf::Ptr() const
	{ return iBuffer+iOffset; }

/**
Returns a const TUint8 pointer to the end of the MBuf
@return A const pointer to the end of the MBuf
*/
inline const TUint8* RMBuf::EndPtr() const
	{ return Ptr()+iLength; }

/**
Returns the offset of the MBuf
@return the offset
*/
inline TInt RMBuf::Offset() const
	{ return iOffset; }

/**
Returns the length of the MBuf
@return the length
*/
inline TInt RMBuf::Length() const
	{ return iLength; }

/**
Returns the offset of the last byte of the MBuf
@return the offset
*/
inline TInt RMBuf::End() const
	{ return iOffset+iLength; }

/**
Returns the Type of the MBuf (EMBufFree, EMBufData, EMBufHeader)
@return type of the MBuf
*/
inline TMBufType RMBuf::Type() const
	{ return static_cast<TMBufType>(iType); }
#endif
/**
Returns the Next MBuf (as MBuf*)
@return a pointer to the next MBuf
*/
inline RMBuf* RMBuf::Next()
	{ return iNext; }

/**
Returns the next packet in the chain (as a reference to RMBufChain)
@return A reference to the next Packet (as a reference to RMBufChain)
*/
inline RMBufChain& RMBuf::NextPkt()
	{ return iNextPkt; }

/**
Returns the Next MBuf (as const MBuf*)
@return a pointer to the next MBuf
*/
inline const RMBuf* RMBuf::Next() const
	{ return iNext; }

/**
Returns the next packet in the chain (as a const reference to RMBufChain)
@return A reference to the next Packet (as a const reference to RMBufChain)
*/
inline const RMBufChain& RMBuf::NextPkt() const
	{ return iNextPkt; }

/**
Links a MBuf
@param aBuf the buf to be linked
*/
inline void RMBuf::Link(RMBuf* aBuf)
	{ iNext = aBuf; }

/**
Unlinks the following MBuf
*/
inline void RMBuf::Unlink()
	{ iNext = NULL; }

/**
Links a packet to the chain
@param aChain the chain to be linked
*/
inline void RMBuf::LinkPkt(RMBufChain &aChain)
	{ iNextPkt = aChain; }
#ifndef __KERNEL_MODE__
/**
Unlinks the packet next to the MBuf
*/
inline void RMBuf::UnlinkPkt()
	{ iNextPkt.Init(); }

/**
Sets the type of the MBuf (EMBufFree, EMBufData, EMBufHeader)
@param aType A type
*/
inline void RMBuf::SetType(TMBufType aType)
	{ iType = aType; }

/**
Sets the offset
@param aOffset A offset
*/
inline void RMBuf::SetOffset(TInt aOffset)
	{ iOffset = aOffset; }

/**
Sets the length
@param aLength the length
*/
inline void RMBuf::SetLength(TInt aLength)
	{ iLength = aLength; }

/**
Sets data (actually sets the length and the offset)
@param aOffset the offset
@param aLength the length of the data (# of bytes)
*/
inline void RMBuf::SetData(TInt aOffset, TInt aLength)
	{ SetOffset(aOffset);  SetLength(aLength); }

/**
Adjust the begining of the MBuf
*/
inline void RMBuf::AdjustStart(TInt aLen)
	{ iOffset += aLen;  iLength -= aLen; }

/**
Adjust the end of the MBuf
*/
inline void RMBuf::AdjustEnd(TInt aLen)
	{ iLength += aLen; }

#ifdef _DEBUG_SOCKET_FUNCTIONS
/**
Put the data into the MBuf
@param aVal The value to put
@param aOffset The Offset (=0 if not stated)
*/
inline void RMBuf::Put(TUint8 aVal, TInt aOffset)
	{ __DbgPut(aVal, aOffset); }

/**
Get the data from the MBuf
@param aOffset the offset (=0 if not stated)
@return the data in TUint8
*/
inline TUint8 RMBuf::Get(TInt aOffset) const
	{ return __DbgGet(aOffset); }
#else

/**
Put the data into the MBuf
@param aVal The value to put
@param aOffset The Offset (=0 if not stated)
*/
inline void RMBuf::Put(TUint8 aVal, TInt aOffset)
	{ *(Ptr()+aOffset) = aVal; }

/**
Get the data from the MBuf
@param aOffset the offset (=0 if not stated)
@return the data in TUint8
*/
inline TUint8 RMBuf::Get(TInt aOffset) const
	{ return *(Ptr()+aOffset); }
#endif

////////////////////////////////////////////////////////////////////////////////
// MBUF ITERATOR
////////////////////////////////////////////////////////////////////////////////

/**
Constructor
@param aChain where iterator points to
*/
inline TMBufIter::TMBufIter(RMBuf* aChain)
	: iCurrent(aChain)
	{ }

/**
Constructor
@param aChain where iterator points to
*/
inline TMBufIter::TMBufIter(RMBufChain& aChain)
	: iCurrent(aChain.First()) // depends upon RMBufChain::operator RMBuf* ()
	{ }

/**
Provides access to the MBuf
*/
inline TMBufIter& TMBufIter::operator =(RMBuf* aChain)
	{ iCurrent = aChain; return *this; }

/**
Provides access to the MBuf Chain
*/
inline TMBufIter& TMBufIter::operator =(RMBufChain& aChain)
	{ iCurrent = aChain.First(); return *this; }

/**
iterate to next position operator
*/
inline RMBuf* TMBufIter::operator ++(TInt)
	{ RMBuf* m; if (m = iCurrent, m!=NULL) iCurrent=iCurrent->Next(); return m; }

/**
Returns MBuf
@return a MBuf
*/
inline TMBufIter::operator RMBuf*()
	{ return iCurrent; }

/**
if there are more objects
@return True if there is more objects
*/
inline TBool TMBufIter::More()
	{ return iCurrent!=NULL; }

/**
Returns MBuf
@return a MBuf
*/
inline RMBuf* TMBufIter::MBuf()
	{ return iCurrent; }

#endif // KERNEL_MODE

#endif // __MBuf_inl__
