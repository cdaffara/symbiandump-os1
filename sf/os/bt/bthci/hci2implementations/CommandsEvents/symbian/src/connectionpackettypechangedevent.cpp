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
// on Wed, 25 Jul 2007 17:00:42 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/connectionpackettypechangedevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TConnectionPacketTypeChangedEvent::TConnectionPacketTypeChangedEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[2]); // Status field is in octet 3
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TConnectionPacketTypeChangedEvent::TConnectionPacketTypeChangedEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, TUint16 aPacketType, TDes8& aEventData)
	: THCIEventBase(EConnectionPacketTypeChangedEvent, 5, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutTUint16(aPacketType, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TConnectionPacketTypeChangedEvent::~TConnectionPacketTypeChangedEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TConnectionPacketTypeChangedEvent& TConnectionPacketTypeChangedEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EConnectionPacketTypeChangedEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TConnectionPacketTypeChangedEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TConnectionPacketTypeChangedEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(3);
	}

EXPORT_C TUint16 TConnectionPacketTypeChangedEvent::PacketType() const
	{
	return AsTUint16(5);
	}


