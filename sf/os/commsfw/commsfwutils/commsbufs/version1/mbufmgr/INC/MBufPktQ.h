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

#if !defined(__MBufPktQ_h__)
#define __MBufPktQ_h__

class RMBufPktQ
/**
MBUF PACKET QUEUE
@publishedPartner
@released
*/
	{
	__DECLARE_CLEANUP
public:
	inline TBool IsEmpty() const;
	inline const RMBufChain& First() const;
	inline const RMBufChain& Last() const;
	IMPORT_C void Init();
	IMPORT_C void Assign(RMBufPktQ& aQueue);
	IMPORT_C void Free();
	IMPORT_C TBool Remove(RMBufChain& aChain);
	IMPORT_C void Append(RMBufChain& aChain);
	IMPORT_C void Append(RMBufPktQ& aQueue);
	IMPORT_C void Prepend(RMBufChain& aChain);
private:
	friend class TMBufPktQIter;
	void Insert(RMBufChain& aNew, RMBufChain& aPrev);
	void Remove(RMBufChain& aNew, RMBufChain& aPrev);
	//
protected:
	RMBufChain iNext;
	RMBufChain iLast;
	};

class TMBufPktQIter
/**
MBUF PACKET QUEUE ITERATOR
@publishedPartner
@released
*/
	{
public:
	inline TMBufPktQIter(RMBufPktQ& aQueue);
	inline void Set(RMBufPktQ& aQueue);
	inline void SetToFirst();
	inline TBool More() const;
	inline const RMBufChain& Current() const;
	IMPORT_C const RMBufChain& operator++(TInt);
	IMPORT_C void Insert(RMBufChain& aNewChain);
	IMPORT_C void Remove(RMBufChain& aNewChain);
	IMPORT_C void TidyAfterUpdate();
	//
protected:
	// Must be regarded as a const chain, hence the const API
	RMBufChain iPrev;
	RMBufPktQ* iQueue;
	RMBufChain iCurrent;
	};
#endif

#if !defined(__MBufPktQ_Ignore_Inlines__)
#include <comms-infras/mbufpktq.inl>
#endif // __MBufPktQ_Ignore_Inlines__
