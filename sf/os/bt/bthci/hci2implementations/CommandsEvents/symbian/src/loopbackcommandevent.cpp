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

#include <bluetooth/hci/loopbackcommandevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TLoopbackCommandEvent::TLoopbackCommandEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TLoopbackCommandEvent::TLoopbackCommandEvent(const TDesC8& aHCICommandPacket, TDes8& aEventData)
	: THCIEventBase(ELoopbackCommandEvent, aEventData.Length(), aEventData)
	{
	
	PutString(aHCICommandPacket, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TLoopbackCommandEvent::~TLoopbackCommandEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TLoopbackCommandEvent& TLoopbackCommandEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ELoopbackCommandEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TLoopbackCommandEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TPtrC8 TLoopbackCommandEvent::HCICommandPacket() const
	{
	return AsString(2);
	}


