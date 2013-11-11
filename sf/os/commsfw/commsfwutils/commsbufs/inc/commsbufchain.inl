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


inline RCommsBufChain::RCommsBufChain()
: iNext(NULL)
	{
	}
	
inline RCommsBufChain::RCommsBufChain(RCommsBuf* aHeadBuf)
: iNext(aHeadBuf)
	{
	}

inline TBool RCommsBufChain::IsEmpty() const
/**
Check if the chain is empty. If empty returns ETrue otherwise EFalse
*/
	{
	return (iNext == NULL);
	}

inline RCommsBuf* RCommsBufChain::First()
/**
Returns the first RCommsBuf in the chain.
*/
	{
	return iNext;	
	}

inline RCommsBuf* RCommsBufChain::Goto(TInt aDataOffset, TInt& aBufDataOffset, TInt& aBufDataLen) const
 /**
Goto specified byte offset into a CommsBuf chain. Used as part of copyin/out, split etc to position
RCommsBuf pointer and offset from start of iBuffer.

@param anOffset The offset
@param resBuf result buffer
@param resOffset result offset
@param resLength result length
@param resPrevBuf result previous RCommsBuf in the chain
@return ETrue if successful
*/
	{
	RCommsBuf* buf;
	return Goto(aDataOffset, aBufDataOffset, aBufDataLen, buf);
	}

