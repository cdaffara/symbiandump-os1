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
// on Mon, 02 Jun 2008 11:36:19 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/sniffsubratingevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TSniffSubratingEvent::TSniffSubratingEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[2]); // Status field is in octet 3
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TSniffSubratingEvent::TSniffSubratingEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, TBasebandTime aMaximumTransmitLatency, TBasebandTime aMaximumReceiveLatency, TBasebandTime aMinimumRemoteTimeout, TBasebandTime aMinimumLocalTimeout, TDes8& aEventData)
	: THCIEventBase(ESniffSubratingEvent, 11, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutTUint16(aMaximumTransmitLatency, aEventData);
	PutTUint16(aMaximumReceiveLatency, aEventData);
	PutTUint16(aMinimumRemoteTimeout, aEventData);
	PutTUint16(aMinimumLocalTimeout, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TSniffSubratingEvent::~TSniffSubratingEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TSniffSubratingEvent& TSniffSubratingEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ESniffSubratingEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TSniffSubratingEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TSniffSubratingEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(3);
	}

EXPORT_C TBasebandTime TSniffSubratingEvent::MaximumTransmitLatency() const
	{
	return AsTUint16(5);
	}

EXPORT_C TBasebandTime TSniffSubratingEvent::MaximumReceiveLatency() const
	{
	return AsTUint16(7);
	}

EXPORT_C TBasebandTime TSniffSubratingEvent::MinimumRemoteTimeout() const
	{
	return AsTUint16(9);
	}

EXPORT_C TBasebandTime TSniffSubratingEvent::MinimumLocalTimeout() const
	{
	return AsTUint16(11);
	}


