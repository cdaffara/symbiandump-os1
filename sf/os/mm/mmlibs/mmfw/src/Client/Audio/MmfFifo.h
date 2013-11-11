// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFFIFO_H_
#define __MMFFIFO_H_

// standard EPOC include header files
#include <e32base.h>
#include <e32std.h>


/**
	Class for storing stream data 
*/
class TMMFFifoItemBase
	{
public:
	static const TInt	iOffset;
private:
	TSglQueLink			iSlink;
	};

template<class T>
class TMMFFifoItem : public TMMFFifoItemBase
	{
public:
	inline TMMFFifoItem(T& aData);
	inline T& GetData();
private:
	T& iData;
	};

/**
	Class for processing stream data queue (add, get, remove etc)
*/
class CMMFFifoBase	: public CBase
	{
public:
	virtual ~CMMFFifoBase();
	TBool IsEmpty();
	void AddToFifo(TMMFFifoItemBase& aItem);
protected:
	CMMFFifoBase();
	void RemoveFirstItemBase();
	TMMFFifoItemBase* GetBase();
private:
	TSglQue<TMMFFifoItemBase>		iFifo;
	TSglQueIter<TMMFFifoItemBase>	iFifoIter;
	};

template<class T>
class CMMFFifo : public CMMFFifoBase
	{
public:
	inline void RemoveFirstItem();
	inline TMMFFifoItem<T>* Get();
	};


template <class T>
TMMFFifoItem<T>::TMMFFifoItem(T& aData)
: iData(aData)
	{}

template <class T>
T& TMMFFifoItem<T>::GetData()
	{ return iData; }

//

template <class T>
void CMMFFifo<T>::RemoveFirstItem()
	{ RemoveFirstItemBase(); }

template <class T>
inline TMMFFifoItem<T>* CMMFFifo<T>::Get()
	{ return STATIC_CAST(TMMFFifoItem<T>*, GetBase()); }


#endif
