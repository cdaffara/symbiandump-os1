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

#include <bluetooth/hci/pagescanmodechangeevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TPageScanModeChangeEvent::TPageScanModeChangeEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TPageScanModeChangeEvent::TPageScanModeChangeEvent(const TBTDevAddr& aBDADDR, TUint8 aPageScanMode, TDes8& aEventData)
	: THCIEventBase(EPageScanModeChangeEvent, 7, aEventData)
	{
	
	PutDevAddr(aBDADDR, aEventData);
	PutTUint8(aPageScanMode, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TPageScanModeChangeEvent::~TPageScanModeChangeEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TPageScanModeChangeEvent& TPageScanModeChangeEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EPageScanModeChangeEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TPageScanModeChangeEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TBTDevAddr TPageScanModeChangeEvent::BDADDR() const
	{
	return AsDevAddr(2);
	}

EXPORT_C TUint8 TPageScanModeChangeEvent::PageScanMode() const
	{
	return AsTUint8(8);
	}


