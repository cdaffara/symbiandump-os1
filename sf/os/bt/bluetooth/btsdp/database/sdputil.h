
// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SDPUTIL_H
#define SDPUTIL_H

#include <e32std.h>
#include <e32base.h>

enum TSdpLeaveError
	{
	ESdpUnexpectedAttributeValueType = -1,
	};

_LIT(KSdpDatabasePanicString, "SDP-Database");
_LIT(KSdpParsePanicString, "SDP-Parse");

/**
Panic values:
	ESdpDbBadAttrValueGetter		Panics if the virtual base class method is called. Must be overriden in derived classes.
	ESdpDbBadSearchPattern			Panics if a bad search pattern is provided.
	ESdpDbRecordBadHandle			Panics if a bad handle to a record is provided.
	ESdpCSdpStackFixIsEmpty			Panics if CSdpStackFix is empty
	ESdpNoAttrValue					Panics if an attribute value is missing.
	ESdpAttrValueBadSize			Panics if an attribute value is a bad size.
	ESDPServerNoDatabase			Panics if there is no server database
	ESdpDbAttributeEncodingFailed	Panics if the attribute cannot be encoded because it is of unknown type.
	ESdpDbStoredAttrValNotEncoded	Panics if the attribute was not correctly encoded.
	ESdpDbOrderedQueueItemIsHead	Panics if the queue iterator has failed.
	ESdpDbOrderedQueuePositionClash	Panics if there is a position clash in the queue.
 */
enum TSdpDbPanic
	{
	ESdpDbBadAttrValueGetter	= 0,
	ESdpDbBadSearchPattern		= 1,
	ESdpDbRecordBadHandle		= 2,
	ESdpCSdpStackFixIsEmpty		= 3,
	ESdpNoAttrValue				= 4,
	ESdpAttrValueBadSize		= 5,
	ESDPServerNoDatabase		= 6,
	ESdpDbAttributeEncodingFailed	= 7,
	ESdpDbStoredAttrValNotEncoded	= 8,
	ESdpDbOrderedQueueItemIsHead	= 9,
	ESdpDbOrderedQueuePositionClash	= 10,
	};

void DbPanic(TSdpDbPanic aCode);

//TSdpAgPanic - see ..\agent\agutil.h


/**
Panic values:
	EGetUintBadDescriptorLength			Panics if the descriptor has a bad length
	EFrameOverrun						Panics if the frame has overrun
	EListOverrun						Panics if the list has overrun
	EGetUint64BadDescriptorLength		Panics if the descriptor does not contain a parsable Uint64
	EGetUint128BadDescriptorLength		Panics if the descriptor does not contain a parsable Uint128
*/
enum TSdpParsePanic
	{
	EGetUintBadDescriptorLength     = 0,
	EFrameOverrun                   = 1,
	EListOverrun                    = 2,
	EGetUint64BadDescriptorLength   = 3,
	EGetUint128BadDescriptorLength  = 4,
	};

void ParsePanic(TSdpParsePanic);

template<class T>
class TOrderedQue : public TDblQue<T>
	{
public:
	inline TOrderedQue(TInt aOffset);
	void AddInOrder(T& aRef);
	void ResetAndDestroy();
	};

const TInt KStackGranularity=4;

template<class T>
class CSdpStackFix : public CArrayFixSeg<T>
	{
public:
	inline CSdpStackFix();
	inline ~CSdpStackFix();

	inline TBool IsEmpty() const;
	inline void PushL(T aItem);
	inline T Pop();
	inline const T& Head() const;
	inline T& Head();
	inline const T& Last() const;		
	};


//
// Inlines
//

template<class T>
inline TOrderedQue<T>::TOrderedQue(TInt aOffset)
: TDblQue<T>(aOffset)
	{
	}

template <class T>
void TOrderedQue<T>::AddInOrder(T& aRef)
	{
	if (TDblQue<T>::IsEmpty() || aRef > (*TDblQue<T>::Last()))
		{
		TDblQue<T>::AddLast(aRef);
		return;
		}

	TDblQueIter<T> iter (*this);
	while (aRef > *iter)
		iter++;

	__ASSERT_DEBUG(!TDblQue<T>::IsHead(iter), DbPanic(ESdpDbOrderedQueueItemIsHead));
	__ASSERT_DEBUG(*iter > aRef, DbPanic(ESdpDbOrderedQueuePositionClash)); // If this fails, there is a position clash!
	PtrAdd((TDblQueLink*)&aRef, TDblQue<T>::iOffset)->AddBefore(PtrAdd((TDblQueLink*)&(*iter), TDblQue<T>::iOffset));
	}

template <class T>
void TOrderedQue<T>::ResetAndDestroy()
	{
	while (!TDblQue<T>::IsEmpty())
		{
		delete TDblQue<T>::First();
		}
	}

template <class T>
inline CSdpStackFix<T>::CSdpStackFix()
: CArrayFixSeg<T>(KStackGranularity)
	{
	CArrayFixSeg<T>::Reset();
	}

template <class T>
inline CSdpStackFix<T>::~CSdpStackFix()
	{ 
	CArrayFixSeg<T>::Reset();
	}

template <class T>
inline TBool CSdpStackFix<T>::IsEmpty() const 
	{ 
	return CArrayFixSeg<T>::Count()==0;
	}

template <class T>
inline void CSdpStackFix<T>::PushL(T aItem) 
	{
	CArrayFixSeg<T>::AppendL(aItem); 
	}

template <class T>
inline T CSdpStackFix<T>::Pop()
	{
	__ASSERT_DEBUG(!IsEmpty(), DbPanic(ESdpCSdpStackFixIsEmpty));
	T item=Head(); 
	CArrayFixSeg<T>::Delete(CArrayFixSeg<T>::Count()-1);
	return item;
	}

template <class T>
inline const T &CSdpStackFix<T>::Head() const 
	{
	__ASSERT_DEBUG(!IsEmpty(), DbPanic(ESdpCSdpStackFixIsEmpty));
	return CArrayFixSeg<T>::At(CArrayFixSeg<T>::Count()-1);
	}

template <class T>
inline T &CSdpStackFix<T>::Head()
	{
	__ASSERT_DEBUG(!IsEmpty(), DbPanic(ESdpCSdpStackFixIsEmpty));
	return CArrayFixSeg<T>::At(CArrayFixSeg<T>::Count()-1);
	}

template <class T>
inline const T &CSdpStackFix<T>::Last() const 
	{
	__ASSERT_DEBUG(!IsEmpty(), DbPanic(ESdpCSdpStackFixIsEmpty));
	return CArrayFixSeg<T>::At(0);
	}
	

#endif
