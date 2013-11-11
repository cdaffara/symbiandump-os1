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

#include <bluetooth/hci/readinquiryscanactivitycompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TReadInquiryScanActivityCompleteEvent::TReadInquiryScanActivityCompleteEvent(const TDesC8& aEventData)
	: THCICommandCompleteEvent(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[5]); // Status field is in octet 6
	}

// Construct an event object to generate a faked event using the supplied parameters
EXPORT_C TReadInquiryScanActivityCompleteEvent::TReadInquiryScanActivityCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, TUint16 aInquiryScanInterval, TUint16 aInquiryScanWindow, TDes8& aEventData)
	: THCICommandCompleteEvent(5 + KCommandCompleteCommonFieldsLength, aNumHCICommandPackets, KReadInquiryScanActivityOpcode, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutTUint16(aInquiryScanInterval, aEventData);
	PutTUint16(aInquiryScanWindow, aEventData);
	iEventData.Set(aEventData);
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TReadInquiryScanActivityCompleteEvent& TReadInquiryScanActivityCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ECommandCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	__ASSERT_DEBUG(THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() == KReadInquiryScanActivityOpcode, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TReadInquiryScanActivityCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint16 TReadInquiryScanActivityCompleteEvent::InquiryScanInterval() const
	{
	return AsTUint16(6);
	}

EXPORT_C TUint16 TReadInquiryScanActivityCompleteEvent::InquiryScanWindow() const
	{
	return AsTUint16(8);
	}


