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

#include <bluetooth/hci/linkkeyrequestevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TLinkKeyRequestEvent::TLinkKeyRequestEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TLinkKeyRequestEvent::TLinkKeyRequestEvent(const TBTDevAddr& aBDADDR, TDes8& aEventData)
	: THCIEventBase(ELinkKeyRequestEvent, 6, aEventData)
	{
	
	PutDevAddr(aBDADDR, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TLinkKeyRequestEvent::~TLinkKeyRequestEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TLinkKeyRequestEvent& TLinkKeyRequestEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ELinkKeyRequestEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TLinkKeyRequestEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TBTDevAddr TLinkKeyRequestEvent::BDADDR() const
	{
	return AsDevAddr(2);
	}


