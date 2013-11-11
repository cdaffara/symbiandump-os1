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

#include <bluetooth/hci/connectioncompleteevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TConnectionCompleteEvent::TConnectionCompleteEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[2]); // Status field is in octet 3
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TConnectionCompleteEvent::TConnectionCompleteEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, const TBTDevAddr& aBDADDR, TUint8 aLinkType, TUint8 aEncryptionMode, TDes8& aEventData)
	: THCIEventBase(EConnectionCompleteEvent, 11, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutDevAddr(aBDADDR, aEventData);
	PutTUint8(aLinkType, aEventData);
	PutTUint8(aEncryptionMode, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TConnectionCompleteEvent::~TConnectionCompleteEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TConnectionCompleteEvent& TConnectionCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EConnectionCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TConnectionCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TConnectionCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(3);
	}

EXPORT_C TBTDevAddr TConnectionCompleteEvent::BDADDR() const
	{
	return AsDevAddr(5);
	}

EXPORT_C TUint8 TConnectionCompleteEvent::LinkType() const
	{
	return AsTUint8(11);
	}

EXPORT_C TUint8 TConnectionCompleteEvent::EncryptionMode() const
	{
	return AsTUint8(12);
	}


