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
// Description: Comms buf iterator
//

#ifndef __COMMSBUFITER_H__
#define __COMMSBUFITER_H__

#include <comms-infras/commsbuf.h>
class TCommsBufIter
/**
Iterator to iterate the RCommsBuf within a chain

@publishedPartner
@prototype
*/
	{
	friend class RCommsBufChain;
	friend class TMBufIter;

	public:
	inline TCommsBufIter(RCommsBufChain& aChain);
	
	inline TCommsBufIter operator++ (TInt);
	inline TCommsBufIter& operator++ ();
	inline RCommsBuf* Current();
	inline TBool AtEnd() const;

	private:
	inline TCommsBufIter();
	inline TCommsBufIter(RCommsBuf* aBuf);
	RCommsBuf* iCurrent;
	};

#include <comms-infras/commsbufiter.inl>
#endif
