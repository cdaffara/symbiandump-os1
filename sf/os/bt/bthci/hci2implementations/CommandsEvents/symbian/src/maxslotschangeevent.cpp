// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file was generated automatically from the template eventsource.tmpl
// on Wed, 25 Jul 2007 17:00:41 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/maxslotschangeevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TMaxSlotsChangeEvent::TMaxSlotsChangeEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TMaxSlotsChangeEvent::TMaxSlotsChangeEvent(THCIConnectionHandle aConnectionHandle, TUint8 aLMPMaxSlots, TDes8& aEventData)
	: THCIEventBase(EMaxSlotsChangeEvent, 3, aEventData)
	{
	
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutTUint8(aLMPMaxSlots, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TMaxSlotsChangeEvent::~TMaxSlotsChangeEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TMaxSlotsChangeEvent& TMaxSlotsChangeEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EMaxSlotsChangeEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TMaxSlotsChangeEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TMaxSlotsChangeEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(2);
	}

EXPORT_C TUint8 TMaxSlotsChangeEvent::LMPMaxSlots() const
	{
	return AsTUint8(4);
	}


