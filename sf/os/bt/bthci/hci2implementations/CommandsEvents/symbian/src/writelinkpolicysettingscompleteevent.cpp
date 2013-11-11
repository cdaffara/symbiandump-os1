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
// on Wed, 25 Jul 2007 17:00:37 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/writelinkpolicysettingscompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TWriteLinkPolicySettingsCompleteEvent::TWriteLinkPolicySettingsCompleteEvent(const TDesC8& aEventData)
	: THCICommandCompleteEvent(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[5]); // Status field is in octet 6
	}

// Construct an event object to generate a faked event using the supplied parameters
EXPORT_C TWriteLinkPolicySettingsCompleteEvent::TWriteLinkPolicySettingsCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, THCIConnectionHandle aConnectionHandle, TDes8& aEventData)
	: THCICommandCompleteEvent(3 + KCommandCompleteCommonFieldsLength, aNumHCICommandPackets, KWriteLinkPolicySettingsOpcode, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	iEventData.Set(aEventData);
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TWriteLinkPolicySettingsCompleteEvent& TWriteLinkPolicySettingsCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ECommandCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	__ASSERT_DEBUG(THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() == KWriteLinkPolicySettingsOpcode, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TWriteLinkPolicySettingsCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TWriteLinkPolicySettingsCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(6);
	}


