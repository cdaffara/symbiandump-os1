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
//

#if !defined(__MBufChain_inl__)
#define __MBufChain_inl__

//
// MBUF CHAIN
//

/**
Constructor
*/
inline RMBufChain::RMBufChain()
	{}

/**
Constructor
*/
inline RMBufChain::RMBufChain(RCommsBuf* aChain) : RCommsBufChain(aChain)
	{}


/**
Rerurns the first in the chain
@return the first in the chain
*/
inline RMBuf* RMBufChain::First()
{ return static_cast<RMBuf*>(iNext); }

/**
Returns the first in the chain
@return the first in the chain
*/
inline const RMBuf* RMBufChain::First() const
{ return static_cast<const RMBuf*>(iNext); }

/**
operator =
*/
inline RMBufChain& RMBufChain::operator =(RMBuf* aChain)
	{ iNext = aChain; return *this; }

/**
Initializes the members (iNext = NULL)
*/
inline void RMBufChain::Init()
	{ iNext = NULL; }

inline TBool RMBufChain::IsEmpty() const
/**
Check if empty, if empty returns true
*/
	{
	return RCommsBufChain::IsEmpty();
	}

#ifndef __KERNEL_MODE__
/**
Links a chain to the chain in the chain
@param aChain The chain to be linked
*/
inline void RMBufChain::Link(RMBufChain &aChain)
	{ if (!IsEmpty()) (static_cast<RMBuf*>(iNext))->LinkPkt(aChain); }

/**
Unlink the nfirst in the chain
*/
inline void RMBufChain::Unlink()
	{ if (!IsEmpty()) (static_cast<RMBuf*>(iNext))->UnlinkPkt(); }

/**
Returns next in the chain if there is next else returns the current
*/
inline RMBufChain& RMBufChain::Next()
	{ return IsEmpty() ? *this : (static_cast<RMBuf*>(iNext))->NextPkt(); }

/**
Returns next in the chain if there is next else returns the current
*/
inline const RMBufChain& RMBufChain::Next() const
	{ return IsEmpty() ? *this : (static_cast<RMBuf*>(iNext))->NextPkt(); }

/** 	  	 
Goto specified byte offset into an Mbuf chain. Used as part of copyin/out, split etc to position 	  	 
MBuf pointer and offset from start of iBuffer. 	  	 
@param aOffset The offset 	  	 
@param resBuf result buffer 	  	 
@param resOffset result offset 	  	 
@param resLength result length 	  	 
@param resPrevBuf result previous Buf in the chain 	  	 
@return ETrue if successful 	  	 
*/
inline TBool RMBufChain::Goto(TInt aOffset, RMBuf* &resBuf, TInt& resOffset, TInt& resLength) const
	{
	 RMBuf* prev;  return Goto(aOffset, resBuf, resOffset, resLength, prev);
	}

inline TInt RMBufChain::Copy(RMBufChain& newChain) const
    {
    return Copy(newChain, 0, KMBufAll, 0);
    }

inline TInt RMBufChain::Copy(RMBufChain& newChain, TInt aOffset) const
    {
    return Copy(newChain, aOffset, KMBufAll, 0);
    }

inline TInt RMBufChain::Copy(RMBufChain& newChain, TInt aOffset, TInt aLen) const
	{
	return Copy(newChain, aOffset, aLen, 0);
	}

#endif // KERNEL_MODE

#endif // __MBufChain_inl__
