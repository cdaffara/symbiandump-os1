// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent 
*/

template<class TInfo, TInt QueueSize> TNotificationQueue<TInfo, QueueSize>::TNotificationQueue()
/**
Constructor
*/
	: iQueueLength(0), iQueueStart(0)
	{
	}

template<class TInfo, TInt QueueSize> void TNotificationQueue<TInfo, QueueSize>::Reset()
/**
Reset notification queue, throwing away any stored notifications
*/
	{
	iQueueLength = 0;
	iQueueStart = 0;
	}

template<class TInfo, TInt QueueSize>
void TNotificationQueue<TInfo, QueueSize>::ResetAndDestroy()
/**
Destroys the pointed memory cells and resets the queue.
*/
    {
    for (TUint i = 0; i < QueueSize; ++i)
        {
        delete iQueue[i];
        iQueue[i] = NULL;
        }
    Reset();
    }

template<class TInfo, TInt QueueSize> void TNotificationQueue<TInfo, QueueSize>::Enque(const TInfo& aInfo)
/**
Place a InterfaceChange notification on the queue
@param aInfo notification information
*/
	{
	TUint index;

	if (IsEmpty())
		{
		index = iQueueStart;
		iQueueLength++;
		}
    else
		{
		index = (iQueueStart + iQueueLength) % QueueSize;

		// Have we reached capacity?
		if (index == iQueueStart)	// Yes, so overwrite
			iQueueStart = (iQueueStart + 1) % QueueSize;
		else
			iQueueLength++;

		}
		
	iQueue[index] = aInfo;		// default assignment operator used
	}

template<class TInfo, TInt QueueSize> TBool TNotificationQueue<TInfo, QueueSize>::Deque(TInfo& aInfo)
/**
De-queue the next InterfaceChange notification and return it

@param aInfo returned notification info
@return ETrue if a notification info was returned, EFalse otherwise (ie: queue was empty).
*/
	{
	if (!IsEmpty())
		{
		aInfo = iQueue[iQueueStart];	// default assignment operator used
		iQueueLength--;
		iQueueStart = (iQueueStart + 1) % QueueSize;
		return ETrue;
		}
	else
		return EFalse;
	}

template<class TInfo, TInt QueueSize>
inline TInfo& TNotificationQueue<TInfo, QueueSize>::GetTheOldestElem()
/**
In the case if pointers are stored in the queue and it is full the oldest
element can be deleted with this method to avoid memory leaking.
*/
    {    
    return iQueue[iQueueStart];
    }

