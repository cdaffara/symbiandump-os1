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

#ifndef __WSMEMMGR_H__
#define __WSMEMMGR_H__

#include <e32base.h>

class CWsMemoryManager: public CBase, private RAllocator
	{
public:
	static CWsMemoryManager * NewLC();
	~CWsMemoryManager();
	
	void EnableReserve();
	void DisableReserve();
	
public:	
	static CWsMemoryManager * Static();
	TInt Count() const;
	
	void SetFailNextRetry();
private:
	CWsMemoryManager();
	void ConstructL();
	// implementing RAllocator
	TAny* Alloc(TInt aSize);
	void Free(TAny* aPtr);
	TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode);
	TInt AllocLen(const TAny* aCell) const;
	TInt Compress();
	void Reset();
	TInt AllocSize(TInt& aTotalAllocSize) const;
	TInt Available(TInt& aBiggestBlock) const;
	TInt DebugFunction(TInt aFunc, TAny* a1, TAny* a2);
	

private:
	static CWsMemoryManager * iStatic;
	
private:
	RAllocator* iImpl;
	TBool iReleasing;
	TBool iReserveEnabled;
	TInt iReserveSize;
	TAny* iReserve;
	TBool iFailNextRetry;
  	TTime iCurrentBurstStart;
	TInt iCurrentBurstReleaseCount;
	};

#endif //__WSMEMMGR_H__

