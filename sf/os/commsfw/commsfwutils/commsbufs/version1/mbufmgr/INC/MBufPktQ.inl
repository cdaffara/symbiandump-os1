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

#if !defined(__MBufPktQ_inl__)
#define __MBufPktQ_inl__

//
// MBUF PACKET QUEUE
//

/**
Check if the queue is empty
@return True if Empty
*/
inline TBool RMBufPktQ::IsEmpty() const
/**
MBUF PACKET QUEUE
*/
	{ return iNext.IsEmpty(); }

/**
Returns the first in the queue
@return the first in the queue
*/
inline const RMBufChain& RMBufPktQ::First() const
	 { return iNext; }

/**
Returns the last in the queue
@return The last in the queue
*/
inline const RMBufChain& RMBufPktQ::Last() const
	 { return iLast; }

//
// MBUF PACKET QUEUE ITREATOR
//
/**
Constructor
*/
inline TMBufPktQIter::TMBufPktQIter(RMBufPktQ& aQueue)
	: iQueue(&aQueue), iCurrent(aQueue.First())
	{ }

/**
Iterates to a a position of an element in the queue
@param aQueue the elemet to iterate to
*/
inline void TMBufPktQIter::Set(RMBufPktQ& aQueue)
	{ iQueue = &aQueue; SetToFirst(); }

/**
Sets to first position
*/
inline void TMBufPktQIter::SetToFirst()
	{ iCurrent = iQueue->First(); iPrev.Init(); }

/**
Checks if there is more in the queue
@return True if there is more in the queue
*/
inline TBool TMBufPktQIter::More() const
	{ return !iCurrent.IsEmpty(); }

/**
Returns the element in the current position
@return The Mbuf in the current position
*/
inline const RMBufChain& TMBufPktQIter::Current() const
	{ return iCurrent; }

#endif // __MBufPktQ_inl__