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
//    Buffer Manager for Protocols

#if !defined(__MBufQ_inl__)
#define __MBufQ_inl__

//
// MBUF QUEUE
//

/**
Constructor
*/
inline RMBufQ::RMBufQ()
	{ Init(); }

/**
Check if the queue is empty
@return True if Empty
*/
inline TBool RMBufQ::IsEmpty()
	{ return iNext==NULL; }

/**
Returns the first in the queue
@return the first in the queue
*/

inline RMBuf* RMBufQ::First()
	{ return static_cast<RMBuf*>(iNext); }

/**
Returns the last in the queue
@return The last in the queue
*/
inline RMBuf* RMBufQ::Last()
	{ return static_cast<RMBuf*>(iLast); }

/**
Constructor
*/
inline RMBufQ::RMBufQ(RMBuf* aFirst, RMBuf* aLast)
	{ iNext = aFirst; iLast = aLast; }

#endif //__MBufQ_inl__
