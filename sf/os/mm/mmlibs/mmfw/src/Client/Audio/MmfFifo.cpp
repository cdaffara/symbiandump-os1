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

#include "MmfFifo.h"

const TInt TMMFFifoItemBase::iOffset = _FOFF(TMMFFifoItemBase, iSlink);


/**
 *
 * Construct
 *
 */
CMMFFifoBase::CMMFFifoBase() : iFifo(TMMFFifoItemBase::iOffset), iFifoIter(iFifo)
	{
	}


/**
 *
 *	Destructor
 *
 */
CMMFFifoBase::~CMMFFifoBase()
	{
	TMMFFifoItemBase* item;
	iFifoIter.SetToFirst();
	while((item = iFifoIter++)!=NULL)
		{
		iFifo.Remove(*item);
		delete item;
		};
	}


/**
 *
 *  To remove the first stream data from queue	
 *
 */
void CMMFFifoBase::RemoveFirstItemBase()
	{
	if(!iFifo.IsEmpty())
		{
		iFifo.Remove(*iFifo.First());
		}
	}

/**
 *
 *  To get the first stream data from queue	
 *
 *	@return	"CMMFFifoItem*"	
 *			a pointer point to the stream data
 *
 */
TMMFFifoItemBase* CMMFFifoBase::GetBase()
	{
	TMMFFifoItemBase* firstItem;
	if(iFifo.IsEmpty())
		return NULL;
	firstItem = iFifo.First();
	return firstItem;
	}

/**
 *
 *  To add a stream data to the queue, the data will be added at end of queue
 *
 *	@param	"TMMFFifoItemBase& aItem"	
 *			a reference to data
 *
 */
void CMMFFifoBase::AddToFifo(TMMFFifoItemBase& aItem)
	{
	iFifo.AddLast(aItem);
	}

/**
 *
 *  To check stream data queue empty or not	
 *
 *	@return	"TBool"	
 *			a boolean value to indicate the queue is empty or not (ETrue is empty)
 *
 */
TBool CMMFFifoBase::IsEmpty()
	{
	return iFifo.IsEmpty();
	}






