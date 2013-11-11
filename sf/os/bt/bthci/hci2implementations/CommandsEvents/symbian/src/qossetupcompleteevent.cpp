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

#include <bluetooth/hci/qossetupcompleteevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TQOSSetupCompleteEvent::TQOSSetupCompleteEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[2]); // Status field is in octet 3
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TQOSSetupCompleteEvent::TQOSSetupCompleteEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aPeakBandwidth, TUint32 aLatency, TUint32 aDelayVariation, TDes8& aEventData)
	: THCIEventBase(EQOSSetupCompleteEvent, 21, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutTUint8(aFlags, aEventData);
	PutTUint8(aServiceType, aEventData);
	PutTUint32(aTokenRate, 4, aEventData);
	PutTUint32(aPeakBandwidth, 4, aEventData);
	PutTUint32(aLatency, 4, aEventData);
	PutTUint32(aDelayVariation, 4, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TQOSSetupCompleteEvent::~TQOSSetupCompleteEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TQOSSetupCompleteEvent& TQOSSetupCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EQOSSetupCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TQOSSetupCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TQOSSetupCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(3);
	}

EXPORT_C TUint8 TQOSSetupCompleteEvent::Flags() const
	{
	return AsTUint8(5);
	}

EXPORT_C TUint8 TQOSSetupCompleteEvent::ServiceType() const
	{
	return AsTUint8(6);
	}

EXPORT_C TUint32 TQOSSetupCompleteEvent::TokenRate() const
	{
	return AsTUint32(7, 4);
	}

EXPORT_C TUint32 TQOSSetupCompleteEvent::PeakBandwidth() const
	{
	return AsTUint32(11, 4);
	}

EXPORT_C TUint32 TQOSSetupCompleteEvent::Latency() const
	{
	return AsTUint32(15, 4);
	}

EXPORT_C TUint32 TQOSSetupCompleteEvent::DelayVariation() const
	{
	return AsTUint32(19, 4);
	}


