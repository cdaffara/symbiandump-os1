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
// Contributors: Chain of shared buffers
//
// Description:
//

#ifndef __COMMSBUFCHAIN_H__
#define __COMMSBUFCHAIN_H__
#include <e32base.h>

class TCommsBufAllocator;
class RCommsBuf;

/*
Chain of RCommsBufs. 
@publishedPartner
**/

class RCommsBufChain
	{

	friend class TCommsBufIter;
	friend class RCommsBufQ;

	public:
	inline RCommsBufChain();
	inline RCommsBufChain(RCommsBuf* aHeadBuf);

	inline TBool IsEmpty() const;
	
	IMPORT_C TInt Length() const;
	

	IMPORT_C void Assign(RCommsBufChain& aChain);
	
	IMPORT_C void Append(RCommsBufChain& aChain);
	IMPORT_C void Append(RCommsBuf* aBuf);
	IMPORT_C TInt Append(TInt aLen);
	
	IMPORT_C void Prepend(RCommsBufChain& aChain);
	IMPORT_C void Prepend(RCommsBuf* aBuf);
	IMPORT_C TInt Prepend(TInt aLen);

	IMPORT_C TInt Alloc(TInt aSize, TCommsBufAllocator& aAccessor);
	IMPORT_C TInt Alloc(TInt aSize, TInt aMinBufSize, TCommsBufAllocator& aAccessor);
	IMPORT_C TInt Alloc(TInt aSize, TInt aMinBufSize, TInt aMaxBufSize, TCommsBufAllocator& aAccessor);

	IMPORT_C TInt Split(TInt aOffset, RCommsBufChain& aNewChain);

	IMPORT_C TInt Align(TInt aSize);
	
	IMPORT_C void Write(const TDesC8& aDes, TInt aOffset =0);
	IMPORT_C void Read(TDes8& aDes, TInt aOffset =0) const;
	

	IMPORT_C void Free(); // Free the chain.

	IMPORT_C void TrimStart(TInt nBytes);
	IMPORT_C void TrimEnd(TInt anOffset);
	

	protected:
	IMPORT_C RCommsBuf* Goto(TInt aDataOffset, TInt& aBufDataOffset, TInt& aBufDataLen, RCommsBuf* &resPrevBuf) const;
	inline RCommsBuf* Goto(TInt aDataOffset, TInt& aBufDataOffset, TInt& aBufDataLen) const;
	IMPORT_C RCommsBuf* Last() const;

	private:
	inline RCommsBuf* First();
	inline const RCommsBuf* First() const;
	RCommsBuf* Last();
	

	protected:
	RCommsBuf*	iNext;
	};

#include <comms-infras/commsbufchain.inl>
#endif // __COMMSBUFCHAIN_H__


