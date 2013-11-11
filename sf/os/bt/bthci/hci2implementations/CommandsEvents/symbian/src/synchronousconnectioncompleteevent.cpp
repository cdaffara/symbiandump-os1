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

#include <bluetooth/hci/synchronousconnectioncompleteevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TSynchronousConnectionCompleteEvent::TSynchronousConnectionCompleteEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[2]); // Status field is in octet 3
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TSynchronousConnectionCompleteEvent::TSynchronousConnectionCompleteEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, const TBTDevAddr& aBDADDR, TUint8 aLinkType, TUint8 aTransmissionInterval, TUint8 aRetransmissionWindow, TUint16 aRxPacket_Length, TUint16 aTxPacket_Length, TUint8 aAirMode, TDes8& aEventData)
	: THCIEventBase(ESynchronousConnectionCompleteEvent, 17, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	PutDevAddr(aBDADDR, aEventData);
	PutTUint8(aLinkType, aEventData);
	PutTUint8(aTransmissionInterval, aEventData);
	PutTUint8(aRetransmissionWindow, aEventData);
	PutTUint16(aRxPacket_Length, aEventData);
	PutTUint16(aTxPacket_Length, aEventData);
	PutTUint8(aAirMode, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TSynchronousConnectionCompleteEvent::~TSynchronousConnectionCompleteEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TSynchronousConnectionCompleteEvent& TSynchronousConnectionCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ESynchronousConnectionCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TSynchronousConnectionCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TSynchronousConnectionCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(3);
	}

EXPORT_C TBTDevAddr TSynchronousConnectionCompleteEvent::BDADDR() const
	{
	return AsDevAddr(5);
	}

EXPORT_C TUint8 TSynchronousConnectionCompleteEvent::LinkType() const
	{
	return AsTUint8(11);
	}

EXPORT_C TUint8 TSynchronousConnectionCompleteEvent::TransmissionInterval() const
	{
	return AsTUint8(12);
	}

EXPORT_C TUint8 TSynchronousConnectionCompleteEvent::RetransmissionWindow() const
	{
	return AsTUint8(13);
	}

EXPORT_C TUint16 TSynchronousConnectionCompleteEvent::RxPacket_Length() const
	{
	return AsTUint16(14);
	}

EXPORT_C TUint16 TSynchronousConnectionCompleteEvent::TxPacket_Length() const
	{
	return AsTUint16(16);
	}

EXPORT_C TUint8 TSynchronousConnectionCompleteEvent::AirMode() const
	{
	return AsTUint8(18);
	}


