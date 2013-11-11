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
// on Mon, 26 Nov 2007 13:42:05 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/extendedinquiryresultevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TExtendedInquiryResultEvent::TExtendedInquiryResultEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TExtendedInquiryResultEvent::TExtendedInquiryResultEvent(TUint8 aNumResponses, const TBTDevAddr& aBDADDR, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, TUint32 aClassOfDevice, THCIClockOffset aClockOffset, THCIRssi aRSSI, const TDesC8& aExtendedInquiryResponse, TDes8& aEventData)
	: THCIEventBase(EExtendedInquiryResultEvent, 255, aEventData)
	{
	
	PutTUint8(aNumResponses, aEventData);
	PutDevAddr(aBDADDR, aEventData);
	PutTUint8(aPageScanRepetitionMode, aEventData);
	PutTUint8(aReserved, aEventData);
	PutTUint32(aClassOfDevice, 3, aEventData);
	PutTUint16(aClockOffset, aEventData);
	PutTInt8(aRSSI, aEventData);
	PutPaddedString(aExtendedInquiryResponse, 240, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TExtendedInquiryResultEvent::~TExtendedInquiryResultEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TExtendedInquiryResultEvent& TExtendedInquiryResultEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EExtendedInquiryResultEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TExtendedInquiryResultEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint8 TExtendedInquiryResultEvent::NumResponses() const
	{
	return AsTUint8(2);
	}

EXPORT_C TBTDevAddr TExtendedInquiryResultEvent::BDADDR() const
	{
	return AsDevAddr(3);
	}

EXPORT_C THCIPageScanRepetitionMode TExtendedInquiryResultEvent::PageScanRepetitionMode() const
	{
	return AsTUint8(9);
	}

EXPORT_C THCIReservedOctet TExtendedInquiryResultEvent::Reserved() const
	{
	return AsTUint8(10);
	}

EXPORT_C TUint32 TExtendedInquiryResultEvent::ClassOfDevice() const
	{
	return AsTUint32(11, 3);
	}

EXPORT_C THCIClockOffset TExtendedInquiryResultEvent::ClockOffset() const
	{
	return AsTUint16(14);
	}

EXPORT_C THCIRssi TExtendedInquiryResultEvent::RSSI() const
	{
	return AsTInt8(16);
	}

EXPORT_C TPtrC8 TExtendedInquiryResultEvent::ExtendedInquiryResponse() const
	{
	return AsString(17);
	}


