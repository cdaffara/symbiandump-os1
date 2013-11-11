// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Buffer Manager for Protocols
//

#if !defined(__MBuf_inl__)
#define __MBuf_inl__

//
// MBUF
//


inline TAny* RMBuf::operator new(TUint /* aSize */, TUint8* aPtr)
	{
	return aPtr;
	}

inline void RMBuf::operator delete(TAny*, TUint8* )
	{
	}


#ifndef __KERNEL_MODE__
/**
Fills with binary zeroes (i.e. 0x00), replacing any existing content.
*/
inline void RMBuf::FillZ()
	{ Mem::FillZ(RawBase(), iCommsBufMetaData.iRawSize); }

/**
Returns the size of the MBuf
@return the size
*/
inline TInt RMBuf::Size() const
{ return RCommsBuf::RawSize(); }

/**
Returns a pointer to the MBuf data buffer
@return A pointer to the MBuf data buffer
*/
inline TUint8* RMBuf::Buffer()
{ return (TUint8*)RCommsBuf::RawBase(); }

/**
Returns a TUnit8 pointer to the begining of MBuf
@return A pointer to the begining of MBuf
*/
inline TUint8* RMBuf::Ptr()
{ return RCommsBuf::Ptr(); }

/**
Returns a TUint8 pointer to the end of the MBuf
@return A pointer to the end of the MBuf
*/
inline TUint8* RMBuf::EndPtr()
{ return Ptr()+RCommsBuf::Length(); }

/**
Returns a const pointer to the MBuf data buffer
@return A const pointer to the MBuf data buffer
*/
inline const TUint8* RMBuf::Buffer() const
{ return RCommsBuf::RawBase(); }

/**
Returns a const TUnit8 pointer to the begining of MBuf
@return A const pointer to the begining of MBuf
*/
inline const TUint8* RMBuf::Ptr() const
{ return (TUint8*)RCommsBuf::RawBase()+RCommsBuf::Offset(); }

/**
Returns a const TUint8 pointer to the end of the MBuf
@return A const pointer to the end of the MBuf
*/
inline const TUint8* RMBuf::EndPtr() const
{ return Ptr()+RCommsBuf::Length(); }

/**
Returns the offset of the MBuf
@return the offset
*/
inline TInt RMBuf::Offset() const
{ return RCommsBuf::Offset(); }

/**
Returns the length of the MBuf
@return the length
*/
inline TInt RMBuf::Length() const
{ return RCommsBuf::Length(); }

/**
Returns the offset of the last byte of the MBuf
@return the offset
*/
inline TInt RMBuf::End() const
{ return RCommsBuf::Offset()+RCommsBuf::Length(); }

/**
Returns the Type of the MBuf (EMBufFree, EMBufData, EMBufHeader)
@return type of the MBuf
*/
inline TMBufType RMBuf::Type() const
	{ return static_cast<TMBufType>(iCommsBufMetaData.iType); }
#endif
/**
Returns the Next MBuf (as MBuf*)
@return a pointer to the next MBuf
*/
inline RMBuf* RMBuf::Next()
	{ return static_cast<RMBuf*>(iCommsBufMetaData.iNext); }

/**
Returns the next packet in the chain (as a reference to RMBufChain)
@return A reference to the next Packet (as a reference to RMBufChain)
*/
inline RMBufChain& RMBuf::NextPkt()
	{ return static_cast<RMBufChain&>(iCommsBufMetaData.iNextPkt); }

/**
Returns the Next MBuf (as const MBuf*)
@return a pointer to the next MBuf
*/
inline const RMBuf* RMBuf::Next() const
	{ return static_cast<const RMBuf*>(RCommsBuf::Next()); }

/**
Returns the next packet in the chain (as a const reference to RMBufChain)
@return A reference to the next Packet (as a const reference to RMBufChain)
*/
inline const RMBufChain& RMBuf::NextPkt() const
	{ return static_cast<const RMBufChain&>(iCommsBufMetaData.iNextPkt); }

/**
Links a MBuf
@param aBuf the buf to be linked
*/
inline void RMBuf::Link(RMBuf* aBuf)
{ RCommsBuf::SetNext(aBuf); }

/**
Unlinks the following MBuf
*/
inline void RMBuf::Unlink()
{ RCommsBuf::SetNext(NULL); }

/**
Links a packet to the chain
@param aChain the chain to be linked
*/
inline void RMBuf::LinkPkt(RMBufChain &aChain)
	{ iCommsBufMetaData.iNextPkt = aChain; }
#ifndef __KERNEL_MODE__
/**
Unlinks the packet next to the MBuf
*/
inline void RMBuf::UnlinkPkt()
	{ NextPkt().Init(); }

/**
Sets the type of the MBuf (EMBufFree, EMBufData, EMBufHeader)
@param aType A type
*/
inline void RMBuf::SetType(TMBufType aType)
	{ iCommsBufMetaData.iType = aType; }

/**
Sets the offset
@param aOffset A offset
*/
inline void RMBuf::SetOffset(TInt aOffset)
	{ iCommsBufMetaData.iOffset = iCommsBufMetaData.iRawDataOffset + aOffset; }

/**
Sets the length
@param aLength the length
*/
inline void RMBuf::SetLength(TInt aLength)
	{ iCommsBufMetaData.iLength = aLength; }

/**
Sets data (actually sets the length and the offset)
@param aOffset the offset
@param aLength the length of the data (# of bytes)
*/
inline void RMBuf::SetData(TInt aOffset, TInt aLength)
{	RCommsBuf::SetDataRange(aOffset, aLength); }

/**
Adjust the begining of the MBuf
*/
inline void RMBuf::AdjustStart(TInt aLen)
{	RCommsBuf::AdjustDataStart(aLen); }

/**
Adjust the end of the MBuf
*/
inline void RMBuf::AdjustEnd(TInt aLen)
{ RCommsBuf::AdjustDataEnd(aLen); }

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

//
// MBUF ITERATOR
//

/**
Constructor
@param aChain where iterator points to
*/
inline TMBufIter::TMBufIter(RMBuf* aChain)
	: iIterator(aChain)
	{ }

/**
Constructor
@param aChain where iterator points to
*/
inline TMBufIter::TMBufIter(RMBufChain& aChain)
	: iIterator(aChain)
	{ }

/**
Provides access to the MBuf
*/
inline TMBufIter& TMBufIter::operator =(RMBuf* aChain)
	{ iIterator = aChain; return *this; }

/**
Provides access to the MBuf Chain
*/
inline TMBufIter& TMBufIter::operator =(RMBufChain& aChain)
	{ iIterator = aChain; return *this; }

/**
iterate to next position operator
*/
inline RMBuf* TMBufIter::operator ++(TInt)
	{
	RMBuf* buf = static_cast<RMBuf*>(iIterator.Current());
	++iIterator;
	return buf;
	}

/**
Returns MBuf
@return a MBuf
*/
inline TMBufIter::operator RMBuf* () 
	{ 	return MBuf (); 
	}

/**
if there are more objects
@return True if there is more objects
*/
inline TBool TMBufIter::More()
	{ return !(iIterator.AtEnd()); }

/**
Returns MBuf
@return a MBuf
*/
inline RMBuf* TMBufIter::MBuf()
	{ return static_cast<RMBuf*>(iIterator.Current()); }

#endif // KERNEL_MODE

#endif // __MBuf_inl__
