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
// on Wed, 25 Jul 2007 17:00:36 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/readautomaticflushtimeoutcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TReadAutomaticFlushTimeoutCompleteEvent::TReadAutomaticFlushTimeoutCompleteEvent(const TDesC8& aEventData)
	: THCICommandCompleteEvent(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[5]); // Status field is in octet 6
	}

// Construct an event object to generate a faked event using the supplied parameters
EXPORT_C TReadAutomaticFlushTimeoutCompleteEvent::TReadAutomaticFlushTimeoutCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, THCIConnectionHandle aConnectionHandle, TUint16 aFlushTimeout, TDes8& aEventData)
	: THCICommandCompleteEvent(5 + KCommandCompleteCommonFieldsLength, aNumHCICommandPackets, KReadAutomaticFlushTimeoutOpcode, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutTUint16(aFlushTimeout, aEventData);
	iEventData.Set(aEventData);
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TReadAutomaticFlushTimeoutCompleteEvent& TReadAutomaticFlushTimeoutCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ECommandCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	__ASSERT_DEBUG(THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() == KReadAutomaticFlushTimeoutOpcode, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TReadAutomaticFlushTimeoutCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TReadAutomaticFlushTimeoutCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(6);
	}

EXPORT_C TUint16 TReadAutomaticFlushTimeoutCompleteEvent::FlushTimeout() const
	{
	return AsTUint16(8);
	}


