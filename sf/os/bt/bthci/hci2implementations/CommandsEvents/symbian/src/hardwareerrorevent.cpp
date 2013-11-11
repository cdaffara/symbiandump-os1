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

#include <bluetooth/hci/hardwareerrorevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C THardwareErrorEvent::THardwareErrorEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C THardwareErrorEvent::THardwareErrorEvent(TUint8 aHardwareCode, TDes8& aEventData)
	: THCIEventBase(EHardwareErrorEvent, 1, aEventData)
	{
	
	PutTUint8(aHardwareCode, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C THardwareErrorEvent::~THardwareErrorEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C THardwareErrorEvent& THardwareErrorEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EHardwareErrorEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<THardwareErrorEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint8 THardwareErrorEvent::HardwareCode() const
	{
	return AsTUint8(2);
	}


