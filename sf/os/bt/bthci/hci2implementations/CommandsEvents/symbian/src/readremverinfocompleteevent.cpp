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

#include <bluetooth/hci/readremverinfocompleteevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TReadRemVerInfoCompleteEvent::TReadRemVerInfoCompleteEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[2]); // Status field is in octet 3
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TReadRemVerInfoCompleteEvent::TReadRemVerInfoCompleteEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, TUint8 aLMPVersion, TUint16 aManufacturerName, TUint16 aLMPSubversion, TDes8& aEventData)
	: THCIEventBase(EReadRemVerInfoCompleteEvent, 8, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutTUint8(aLMPVersion, aEventData);
	PutTUint16(aManufacturerName, aEventData);
	PutTUint16(aLMPSubversion, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TReadRemVerInfoCompleteEvent::~TReadRemVerInfoCompleteEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TReadRemVerInfoCompleteEvent& TReadRemVerInfoCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EReadRemVerInfoCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TReadRemVerInfoCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TReadRemVerInfoCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(3);
	}

EXPORT_C TUint8 TReadRemVerInfoCompleteEvent::LMPVersion() const
	{
	return AsTUint8(5);
	}

EXPORT_C TUint16 TReadRemVerInfoCompleteEvent::ManufacturerName() const
	{
	return AsTUint16(6);
	}

EXPORT_C TUint16 TReadRemVerInfoCompleteEvent::LMPSubversion() const
	{
	return AsTUint16(8);
	}


