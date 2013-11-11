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
// Description: Pond debug functions.
//

/**
@file
@internalTechnology
*/

#ifndef __COMMSBUFPONDDBF_H__
#define __COMMSBUFPONDDBF_H__

#include <e32base.h>

class RCommsBuf;
class RCommsBufPond;

class MCommsBufPondDbg
	{
	public:
    virtual RCommsBuf* __DbgBufChain() =0;
	virtual RCommsBuf* __DbgBufChain(TUint aBufSize) =0;
	virtual void __DbgSetPoolLimit(TInt aCount) =0;
	virtual void __DbgSetPoolLimit(TInt aCount, TUint aBufSize) =0;
	virtual void __DbgSetFailAfter(TInt aCount=0) =0;
	virtual TUint __DbgGetBufSpace() =0;
	virtual TUint __DbgGetBufSpace(TUint aBufSize) =0;
	virtual TUint __DbgGetBufTotal() =0;
	virtual TUint __DbgGetBufTotal(TUint aMufSize) =0;
	virtual TInt __DbgGetHeapSize() =0;

	};

class TCommsBufPondDbg
	{
	public:
	IMPORT_C TCommsBufPondDbg(RCommsBufPond& aPond);
    IMPORT_C RCommsBuf* __DbgBufChain();
	IMPORT_C RCommsBuf* __DbgBufChain(TUint aBufSize);
	IMPORT_C void __DbgSetPoolLimit(TInt aCount);
	IMPORT_C void __DbgSetPoolLimit(TInt aCount, TUint aBufSize);
	IMPORT_C void __DbgSetFailAfter(TInt aCount=0);
	IMPORT_C TUint __DbgGetBufSpace();
	IMPORT_C TUint __DbgGetBufSpace(TUint aBufSize);
	IMPORT_C TUint __DbgGetBufTotal();
	IMPORT_C TUint __DbgGetBufTotal(TUint aMufSize);
	IMPORT_C TInt __DbgGetHeapSize();
	
	private:
	MCommsBufPondDbg& iCommsBufPondDbg;
	};
#endif // __COMMSBUFPONDDBF_H__

