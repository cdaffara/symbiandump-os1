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

#include <bluetooth/hci/numberofcompletedpacketsevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TNumberOfCompletedPacketsEvent::TNumberOfCompletedPacketsEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TNumberOfCompletedPacketsEvent::TNumberOfCompletedPacketsEvent(TUint8 aNumberofHandles, const TArray< THCIConnectionHandle >& aConnectionHandle, const TArray< THCINumOfCompletedPackets >& aHCNumOfCompletedPackets, TDes8& aEventData)
	: THCIEventBase(ENumberOfCompletedPacketsEvent, aNumberofHandles * 4 + 1, aEventData)
	{
	
	PutTUint8(aNumberofHandles, aEventData);
	for(int i=0;i<aNumberofHandles;++i)
		{
		PutConnectionHandle(aConnectionHandle[i], aEventData);
		PutTUint16(aHCNumOfCompletedPackets[i], aEventData);
		}
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TNumberOfCompletedPacketsEvent::~TNumberOfCompletedPacketsEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TNumberOfCompletedPacketsEvent& TNumberOfCompletedPacketsEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ENumberOfCompletedPacketsEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TNumberOfCompletedPacketsEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint8 TNumberOfCompletedPacketsEvent::NumberofHandles() const
	{
	return AsTUint8(2);
	}

EXPORT_C THCIConnectionHandle TNumberOfCompletedPacketsEvent::ConnectionHandle(TInt aIndex) const
	{
	return AsConnectionHandle(3, 4, aIndex);
	}

EXPORT_C THCINumOfCompletedPackets TNumberOfCompletedPacketsEvent::HCNumOfCompletedPackets(TInt aIndex) const
	{
	return AsTUint16(5, 4, aIndex);
	}


