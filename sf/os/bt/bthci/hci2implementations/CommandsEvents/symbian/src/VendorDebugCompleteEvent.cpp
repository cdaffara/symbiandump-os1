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
// Implementation of TVendorDebugCompleteEvent.
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/vendordebugcompleteevent.h>
#include "symbiancommandseventsutils.h"

#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

EXPORT_C TVendorDebugCompleteEvent::TVendorDebugCompleteEvent(const TDesC8& aEventData)
	:
	THCICommandCompleteEvent(aEventData)
	{
	}

EXPORT_C TVendorDebugCompleteEvent::TVendorDebugCompleteEvent(TUint8 aNumHCICommandPackets,
		THCIOpcode aCommandOpcode, const TDesC8& aEventPayload, TDes8& aEventData)
	:
	THCICommandCompleteEvent(aEventPayload.Length() + KCommandCompleteCommonFieldsLength,
			aNumHCICommandPackets, aCommandOpcode, aEventData)
	{
	PutString(aEventPayload, aEventData);
	iEventData.Set(aEventData);
	}

EXPORT_C TVendorDebugCompleteEvent& TVendorDebugCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_ALWAYS(aEvent.EventCode()==ECommandCompleteEvent,
					PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));

	__ASSERT_ALWAYS(((THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() & KOGFMask) == KVendorDebugOGF),
					PANIC(KSymbianCommandsEventsPanicCat, EInvalidVendorDebugCommandOpcode));
	return *(reinterpret_cast<TVendorDebugCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

EXPORT_C TPtrC8 TVendorDebugCompleteEvent::VendorDebugData() const
	{
	return AsString(5);
	}

EXPORT_C TPtrC8 TVendorDebugCompleteEvent::VendorDebugEvent() const
	{
	return AsString(0);
	}

