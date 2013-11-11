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

#include <bluetooth/hci/flowspecificationcompleteevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TFlowSpecificationCompleteEvent::TFlowSpecificationCompleteEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[2]); // Status field is in octet 3
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TFlowSpecificationCompleteEvent::TFlowSpecificationCompleteEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aFlowDirection, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aTokenBucketSize, TUint32 aPeakBandwidth, TUint32 aAccessLatency, TDes8& aEventData)
	: THCIEventBase(EFlowSpecificationCompleteEvent, 22, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutTUint8(aFlags, aEventData);
	PutTUint8(aFlowDirection, aEventData);
	PutTUint8(aServiceType, aEventData);
	PutTUint32(aTokenRate, 4, aEventData);
	PutTUint32(aTokenBucketSize, 4, aEventData);
	PutTUint32(aPeakBandwidth, 4, aEventData);
	PutTUint32(aAccessLatency, 4, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TFlowSpecificationCompleteEvent::~TFlowSpecificationCompleteEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TFlowSpecificationCompleteEvent& TFlowSpecificationCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EFlowSpecificationCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TFlowSpecificationCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TFlowSpecificationCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(3);
	}

EXPORT_C TUint8 TFlowSpecificationCompleteEvent::Flags() const
	{
	return AsTUint8(5);
	}

EXPORT_C TUint8 TFlowSpecificationCompleteEvent::FlowDirection() const
	{
	return AsTUint8(6);
	}

EXPORT_C TUint8 TFlowSpecificationCompleteEvent::ServiceType() const
	{
	return AsTUint8(7);
	}

EXPORT_C TUint32 TFlowSpecificationCompleteEvent::TokenRate() const
	{
	return AsTUint32(8, 4);
	}

EXPORT_C TUint32 TFlowSpecificationCompleteEvent::TokenBucketSize() const
	{
	return AsTUint32(12, 4);
	}

EXPORT_C TUint32 TFlowSpecificationCompleteEvent::PeakBandwidth() const
	{
	return AsTUint32(16, 4);
	}

EXPORT_C TUint32 TFlowSpecificationCompleteEvent::AccessLatency() const
	{
	return AsTUint32(20, 4);
	}


