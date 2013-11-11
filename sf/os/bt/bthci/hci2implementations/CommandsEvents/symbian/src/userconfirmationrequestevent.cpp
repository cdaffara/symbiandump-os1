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
// on Wed, 05 Dec 2007 14:54:32 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/userconfirmationrequestevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TUserConfirmationRequestEvent::TUserConfirmationRequestEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TUserConfirmationRequestEvent::TUserConfirmationRequestEvent(const TBTDevAddr& aBDADDR, TUint32 aNumericValue, TDes8& aEventData)
	: THCIEventBase(EUserConfirmationRequestEvent, 10, aEventData)
	{
	
	PutDevAddr(aBDADDR, aEventData);
	PutTUint32(aNumericValue, 4, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TUserConfirmationRequestEvent::~TUserConfirmationRequestEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TUserConfirmationRequestEvent& TUserConfirmationRequestEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EUserConfirmationRequestEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TUserConfirmationRequestEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TBTDevAddr TUserConfirmationRequestEvent::BDADDR() const
	{
	return AsDevAddr(2);
	}

EXPORT_C TUint32 TUserConfirmationRequestEvent::NumericValue() const
	{
	return AsTUint32(8, 4);
	}


