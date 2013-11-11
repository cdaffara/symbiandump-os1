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
// This file was generated automatically from the template eventheader.tmpl
// on Wed, 09 May 2007 15:39:25 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef EXTENDEDINQUIRYRESULTEVENT_H
#define EXTENDEDINQUIRYRESULTEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the ExtendedInquiryResult HCI Event
*/
class TExtendedInquiryResultEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TExtendedInquiryResultEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TExtendedInquiryResultEvent(TUint8 aNumResponses, const TBTDevAddr& aBDADDR, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, TUint32 aClassOfDevice, THCIClockOffset aClockOffset, THCIRssi aRSSI, const TDesC8& aExtendedInquiryResponse, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TExtendedInquiryResultEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TExtendedInquiryResultEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TUint8 NumResponses() const;
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C THCIPageScanRepetitionMode PageScanRepetitionMode() const;
	IMPORT_C THCIReservedOctet Reserved() const;
	IMPORT_C TUint32 ClassOfDevice() const;
	IMPORT_C THCIClockOffset ClockOffset() const;
	IMPORT_C THCIRssi RSSI() const;
	IMPORT_C TPtrC8 ExtendedInquiryResponse() const;
	
	};
	
#endif // EXTENDEDINQUIRYRESULTEVENT_H
