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
// This file was generated automatically from the template completeeventsource.tmpl
// on Wed, 06 Feb 2008 13:20:00 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/readafhchannelmapcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TReadAFHChannelMapCompleteEvent::TReadAFHChannelMapCompleteEvent(const TDesC8& aEventData)
	: THCICommandCompleteEvent(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[5]); // Status field is in octet 6
	}

// Construct an event object to generate a faked event using the supplied parameters
EXPORT_C TReadAFHChannelMapCompleteEvent::TReadAFHChannelMapCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, THCIConnectionHandle aConnectionHandle, TUint8 aAFHMode, TDesC8 aAFHChannelMap, TDes8& aEventData)
	: THCICommandCompleteEvent(14 + KCommandCompleteCommonFieldsLength, aNumHCICommandPackets, KReadAFHChannelMapOpcode, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutTUint8(aAFHMode, aEventData);
	PutString(aAFHChannelMap, aEventData);
	iEventData.Set(aEventData);
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TReadAFHChannelMapCompleteEvent& TReadAFHChannelMapCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ECommandCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	__ASSERT_DEBUG(THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() == KReadAFHChannelMapOpcode, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TReadAFHChannelMapCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TReadAFHChannelMapCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(6);
	}

EXPORT_C TUint8 TReadAFHChannelMapCompleteEvent::AFHMode() const
	{
	return AsTUint8(8);
	}

EXPORT_C TPtrC8 TReadAFHChannelMapCompleteEvent::AFHChannelMap() const
	{
	return AsString(9);
	}


