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
// Description:
//

inline RCommsBufQ::RCommsBufQ()
/**
The constructor
*/
	{ iNext = NULL; iLast = NULL; }

inline RCommsBufQ::RCommsBufQ(RCommsBuf* aFirst, RCommsBuf* aLast)	
/**
The constructor
*/ 
	{
	iNext = aFirst; iLast = aLast;
	}


inline TBool RCommsBufQ::IsEmpty()
/**
Check if the queue is empty

  @return ETrue if empty otherwise EFalse
*/
	{
	return iNext==NULL;
	}

inline RCommsBuf* RCommsBufQ::First()
/**
Returns the first RCommsBuf from the queue
*/
	{
	return iNext;
	}

inline RCommsBuf* RCommsBufQ::Last()
/**
Returns the last RCommsBuf from the queue
*/
	{
	return iLast;
	}

