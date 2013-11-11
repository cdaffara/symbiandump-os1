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
// Description: Comms buffer queue
//

#ifndef __COMMSBUFQ_H__
#define __COMMSBUFQ_H__

#include <e32base.h>
#include <comms-infras/commsbufchain.h>
class RCommsBuf;
class RCommsBufChain;

/**
A linked list of RCommsBuf

Note: Currently the class is tagged as internal technology. In order to move this class
to other categories a detailed analysis of APIs to be conducted

@internalTechnology
*/
class RCommsBufQ
	{
	friend class CSystemSharedBufPool;
	__DECLARE_CLEANUP
public:
	inline RCommsBufQ();
	IMPORT_C RCommsBufQ(RCommsBuf* aChain);

	inline RCommsBufQ(RCommsBuf* aFirst, RCommsBuf* aLast);

	inline TBool IsEmpty();

	inline RCommsBuf* First();

	inline RCommsBuf* Last();

	IMPORT_C void Init();
	IMPORT_C void Assign(RCommsBufQ& aQueue);
	IMPORT_C void Assign(RCommsBufChain& aChain);
	IMPORT_C void Append(RCommsBuf* aBuf);
	IMPORT_C void Append(RCommsBufQ& aQueue);
	IMPORT_C void Append(RCommsBufChain& aChain);
	IMPORT_C void Prepend(RCommsBuf* aBuf);
	IMPORT_C void Prepend(RCommsBufQ& aQueue);
	IMPORT_C void Prepend(RCommsBufChain& aChain);
	IMPORT_C RCommsBuf* Remove();
	IMPORT_C void Free();

	protected:
	RCommsBuf* iNext;
	RCommsBuf* iLast;

	protected:
	IMPORT_C TInt Transfer(RCommsBufQ& aQueue, TInt aSize, TInt aBufSize, TInt& aCount);
	IMPORT_C RCommsBuf* RemoveLast();
	};

#include <comms-infras/commsbufq.inl>
#endif // __COMMSBUFQ_H__




