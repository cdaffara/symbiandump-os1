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

#include <bluetooth/hci/inquiryresultevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TInquiryResultEvent::TInquiryResultEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TInquiryResultEvent::TInquiryResultEvent(TUint8 aNumResponses, const TArray< TBTDevAddr >& aBDADDR, const TArray< THCIPageScanRepetitionMode >& aPageScanRepetitionMode, const TArray< THCIReservedOctet >& aReserved1, const TArray< THCIReservedOctet >& aReserved2, const TArray< TUint32 >& aClassOfDevice, const TArray< THCIClockOffset >& aClockOffset, TDes8& aEventData)
	: THCIEventBase(EInquiryResultEvent, aNumResponses * 14 + 1, aEventData)
	{
	
	PutTUint8(aNumResponses, aEventData);
	for(int i=0;i<aNumResponses;++i)
		{
		PutDevAddr(aBDADDR[i], aEventData);
		PutTUint8(aPageScanRepetitionMode[i], aEventData);
		PutTUint8(aReserved1[i], aEventData);
		PutTUint8(aReserved2[i], aEventData);
		PutTUint32(aClassOfDevice[i], 3, aEventData);
		PutTUint16(aClockOffset[i], aEventData);
		}
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TInquiryResultEvent::~TInquiryResultEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TInquiryResultEvent& TInquiryResultEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EInquiryResultEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TInquiryResultEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint8 TInquiryResultEvent::NumResponses() const
	{
	return AsTUint8(2);
	}

EXPORT_C TBTDevAddr TInquiryResultEvent::BDADDR(TInt aIndex) const
	{
	return AsDevAddr(3, 14, aIndex);
	}

EXPORT_C THCIPageScanRepetitionMode TInquiryResultEvent::PageScanRepetitionMode(TInt aIndex) const
	{
	return AsTUint8(9, 14, aIndex);
	}

EXPORT_C THCIReservedOctet TInquiryResultEvent::Reserved1(TInt aIndex) const
	{
	return AsTUint8(10, 14, aIndex);
	}

EXPORT_C THCIReservedOctet TInquiryResultEvent::Reserved2(TInt aIndex) const
	{
	return AsTUint8(11, 14, aIndex);
	}

EXPORT_C TUint32 TInquiryResultEvent::ClassOfDevice(TInt aIndex) const
	{
	return AsTUint32(12, 14, aIndex, 3);
	}

EXPORT_C THCIClockOffset TInquiryResultEvent::ClockOffset(TInt aIndex) const
	{
	return AsTUint16(15, 14, aIndex);
	}


