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
// on Thu, 29 May 2008 15:23:56 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/iocapabilityresponseevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TIOCapabilityResponseEvent::TIOCapabilityResponseEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TIOCapabilityResponseEvent::TIOCapabilityResponseEvent(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements, TDes8& aEventData)
	: THCIEventBase(EIOCapabilityResponseEvent, 9, aEventData)
	{
	
	PutDevAddr(aBDADDR, aEventData);
	PutTUint8(aIOCapability, aEventData);
	PutTUint8(aOOBDataPresent, aEventData);
	PutTUint8(aAuthenticationRequirements, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TIOCapabilityResponseEvent::~TIOCapabilityResponseEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TIOCapabilityResponseEvent& TIOCapabilityResponseEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EIOCapabilityResponseEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TIOCapabilityResponseEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TBTDevAddr TIOCapabilityResponseEvent::BDADDR() const
	{
	return AsDevAddr(2);
	}

EXPORT_C TUint8 TIOCapabilityResponseEvent::IOCapability() const
	{
	return AsTUint8(8);
	}

EXPORT_C TUint8 TIOCapabilityResponseEvent::OOBDataPresent() const
	{
	return AsTUint8(9);
	}

EXPORT_C TUint8 TIOCapabilityResponseEvent::AuthenticationRequirements() const
	{
	return AsTUint8(10);
	}


