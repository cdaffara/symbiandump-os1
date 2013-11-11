// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#if !defined(__MBufQ_h__)
#define __MBufQ_h__

class RMBuf;

class RMBufQ
/**
MBUF QUEUE
@publishedPartner
@released
*/
	{
friend class RMBufPoolChain;
	__DECLARE_CLEANUP
public:
	inline RMBufQ();
	IMPORT_C RMBufQ(RMBuf* aChain);

	inline RMBufQ(RMBuf* aFirst, RMBuf* aLast);

	inline TBool IsEmpty();

	inline RMBuf* First();

	inline RMBuf* Last();
	IMPORT_C void Init();
	IMPORT_C void Assign(RMBufQ& aQueue);
	IMPORT_C void Assign(RMBufChain& aChain);
	IMPORT_C void Append(RMBuf* aBuf);
	IMPORT_C void Append(RMBufQ& aQueue);
	IMPORT_C void Append(RMBufChain& aChain);
	IMPORT_C void Prepend(RMBuf* aBuf);
	IMPORT_C void Prepend(RMBufQ& aQueue);
	IMPORT_C void Prepend(RMBufChain& aChain);
	IMPORT_C RMBuf* Remove();
	IMPORT_C void Free();
//
protected:
	RMBuf* iNext;
	RMBuf* iLast;

private:
	TInt Transfer(RMBufQ& aQueue, TInt aSize);
	};

#if !defined(__MBufQ_Ignore_Inlines__)
#include <comms-infras/mbufq.inl>
#endif // __MBufQ_Ignore_Inlines__

#endif
