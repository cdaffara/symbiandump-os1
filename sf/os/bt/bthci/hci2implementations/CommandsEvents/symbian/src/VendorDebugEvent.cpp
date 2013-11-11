// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of TVendorDebugEvent.
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/vendordebugevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>

#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

EXPORT_C TVendorDebugEvent::TVendorDebugEvent(const TDesC8& aEventData)
  : THCIEventBase(aEventData)
	{
	}

EXPORT_C TVendorDebugEvent::TVendorDebugEvent(THCIEventCode aEventCode,
			const TDesC8& aEventPayload, TDes8& aEventData)
  : THCIEventBase(aEventCode, aEventPayload.Length(), aEventData)
	{
	PutString(aEventPayload, aEventData);			// vendor-defined payload data
	iEventData.Set(aEventData);
	}

EXPORT_C TVendorDebugEvent& TVendorDebugEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EVendorDebugEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TVendorDebugEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}
	
EXPORT_C TPtrC8 TVendorDebugEvent::VendorDebugData() const
	{
	return AsString(2);
	}

EXPORT_C TPtrC8 TVendorDebugEvent::VendorDebugEvent() const
	{
	return AsString(0);
	}
