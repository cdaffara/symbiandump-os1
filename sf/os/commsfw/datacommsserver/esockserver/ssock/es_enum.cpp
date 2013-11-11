// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "es_enum.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_enum_internal.h>
#include <es_enum_partner.h>
#endif

EXPORT_C TSubConnectionEvent::TSubConnectionEvent(TSubConnectionEventType aEventType, TUint aLength)
: iEventType(aEventType), iLength(aLength)
/**
Constructor

@param aEventType, Types of events that can occur on a subconnection.
@param aLength, length of the pointer to the handle of the thread's heap.
*/
	{}

EXPORT_C TSubConnectionEvent* TSubConnectionEvent::CloneL() const
/**
Reinterprete cast the pointer to the handle of the thread's heap to the sub connection event and returning a clone of it.

@return clone of a sub connection event.
*/
	{
	TSubConnectionEvent* clone = 
	reinterpret_cast<TSubConnectionEvent*>(User::AllocL(iLength));
	(void) Mem::Copy(clone, this, iLength);
	return clone;
	}

EXPORT_C TUint TSubConnectionEvent::Length() const
/**
Get the length of the pointer to the handle of the thread's heap

@return the length of the pointer to the handle of the thread's heap.
*/
	{
	return iLength;
	}

EXPORT_C TSubConnectionOpenedEvent::TSubConnectionOpenedEvent()
: TSubConnectionEvent(ESubConnectionOpened, sizeof(TSubConnectionEvent))
/**
Constructor
*/
	{}

EXPORT_C TSubConnectionClosedEvent::TSubConnectionClosedEvent()
: TSubConnectionEvent(ESubConnectionClosed, sizeof(TSubConnectionClosedEvent))
/**
Constructor
*/
	{}

EXPORT_C TSubConnectionQoSChangedEvent::TSubConnectionQoSChangedEvent()
: TSubConnectionEvent(ESubConnectionQoSChanged, sizeof(TSubConnectionQoSChangedEvent))
/**
Constructor
*/
	{}

