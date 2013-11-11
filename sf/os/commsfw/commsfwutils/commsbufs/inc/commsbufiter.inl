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

inline TCommsBufIter::TCommsBufIter(RCommsBufChain& aChain)
: iCurrent(aChain.First())
/**
Construct an iterator for the RCommsBuf using the supplied chain
*/
	{
	}

inline TCommsBufIter::TCommsBufIter(RCommsBuf* aBuf)
: iCurrent(aBuf)
/**
Construct an iterator for the RCommsBuf using the supplied RCommsBuf
*/
	{
	}

inline TCommsBufIter TCommsBufIter::operator++ (TInt)
/**
Advance the iterator
*/
	{
	TCommsBufIter it(iCurrent);
	if (iCurrent != NULL)
		iCurrent = iCurrent->Next();
	return it;
	}
inline TCommsBufIter& TCommsBufIter::operator++ ()
/**
Advance the iterator
*/
	{
	if (iCurrent != NULL)
		iCurrent = iCurrent->Next();
	return *this;
	}

inline RCommsBuf* TCommsBufIter::Current()
/**
Returns the pointer to  the current RCommsBuf
*/
	{
	return iCurrent;
	}

inline TBool TCommsBufIter::AtEnd() const
/**
Check if the iterator is at the end. If so, further calls to TCommsBufIter::Current returns NULL.

@return ETrue if the iterator is reached the end
*/
	{
	return iCurrent == NULL;
	}

inline TCommsBufIter::TCommsBufIter()
: iCurrent(NULL)
/**
The constructor
*/
	{
	}
