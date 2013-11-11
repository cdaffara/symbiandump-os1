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
// on Wed, 18 Jun 2008 11:15:34 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/nopcompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TNopCompleteEvent::TNopCompleteEvent(const TDesC8& aEventData)
	: THCICommandCompleteEvent(aEventData)
	{
	
	}

// Construct an event object to generate a faked event using the supplied parameters
EXPORT_C TNopCompleteEvent::TNopCompleteEvent(TUint8 aNumHCICommandPackets, TDes8& aEventData)
	: THCICommandCompleteEvent(0 + KCommandCompleteCommonFieldsLength, aNumHCICommandPackets, KNopOpcode, aEventData)
	{
	
	
	iEventData.Set(aEventData);
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TNopCompleteEvent& TNopCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ECommandCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	__ASSERT_DEBUG(THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() == KNopOpcode, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TNopCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event


