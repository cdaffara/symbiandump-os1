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
// on Mon, 16 Jul 2007 15:02:19 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/inquiryresultwithrssievent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TInquiryResultwithRSSIEvent::TInquiryResultwithRSSIEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TInquiryResultwithRSSIEvent::TInquiryResultwithRSSIEvent(TUint8 aNumResponses, const TArray< TBTDevAddr >& aBDADDR, const TArray< THCIPageScanRepetitionMode >& aPageScanRepetitionMode, const TArray< THCIReservedOctet >& aReserved, const TArray< TUint32 >& aClassOfDevice, const TArray< THCIClockOffset >& aClockOffset, const TArray< THCIRssi >& aRSSI, TDes8& aEventData)
	: THCIEventBase(EInquiryResultwithRSSIEvent, aNumResponses * 14 + 1, aEventData)
	{
	
	PutTUint8(aNumResponses, aEventData);
	for(int i=0;i<aNumResponses;++i)
		{
		PutDevAddr(aBDADDR[i], aEventData);
		PutTUint8(aPageScanRepetitionMode[i], aEventData);
		PutTUint8(aReserved[i], aEventData);
		PutTUint32(aClassOfDevice[i], 3, aEventData);
		PutTUint16(aClockOffset[i], aEventData);
		PutTInt8(aRSSI[i], aEventData);
		}
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TInquiryResultwithRSSIEvent::~TInquiryResultwithRSSIEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TInquiryResultwithRSSIEvent& TInquiryResultwithRSSIEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EInquiryResultwithRSSIEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TInquiryResultwithRSSIEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint8 TInquiryResultwithRSSIEvent::NumResponses() const
	{
	return AsTUint8(2);
	}

EXPORT_C TBTDevAddr TInquiryResultwithRSSIEvent::BDADDR(TInt aIndex) const
	{
	return AsDevAddr(3, 14, aIndex);
	}

EXPORT_C THCIPageScanRepetitionMode TInquiryResultwithRSSIEvent::PageScanRepetitionMode(TInt aIndex) const
	{
	return AsTUint8(9, 14, aIndex);
	}

EXPORT_C THCIReservedOctet TInquiryResultwithRSSIEvent::Reserved(TInt aIndex) const
	{
	return AsTUint8(10, 14, aIndex);
	}

EXPORT_C TUint32 TInquiryResultwithRSSIEvent::ClassOfDevice(TInt aIndex) const
	{
	return AsTUint32(11, 14, aIndex, 3);
	}

EXPORT_C THCIClockOffset TInquiryResultwithRSSIEvent::ClockOffset(TInt aIndex) const
	{
	return AsTUint16(14, 14, aIndex);
	}

EXPORT_C THCIRssi TInquiryResultwithRSSIEvent::RSSI(TInt aIndex) const
	{
	return AsTInt8(16, 14, aIndex);
	}


