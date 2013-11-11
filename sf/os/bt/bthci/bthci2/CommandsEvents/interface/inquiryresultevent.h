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
// on Wed, 25 Jul 2007 17:00:41 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef INQUIRYRESULTEVENT_H
#define INQUIRYRESULTEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the InquiryResult HCI Event
*/
class TInquiryResultEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TInquiryResultEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TInquiryResultEvent(TUint8 aNumResponses, const TArray< TBTDevAddr >& aBDADDR, const TArray< THCIPageScanRepetitionMode >& aPageScanRepetitionMode, const TArray< THCIReservedOctet >& aReserved1, const TArray< THCIReservedOctet >& aReserved2, const TArray< TUint32 >& aClassOfDevice, const TArray< THCIClockOffset >& aClockOffset, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TInquiryResultEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TInquiryResultEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TUint8 NumResponses() const;
	IMPORT_C TBTDevAddr BDADDR(TInt aIndex) const;
	IMPORT_C THCIPageScanRepetitionMode PageScanRepetitionMode(TInt aIndex) const;
	IMPORT_C THCIReservedOctet Reserved1(TInt aIndex) const;
	IMPORT_C THCIReservedOctet Reserved2(TInt aIndex) const;
	IMPORT_C TUint32 ClassOfDevice(TInt aIndex) const;
	IMPORT_C THCIClockOffset ClockOffset(TInt aIndex) const;
	
	};
	
#endif // INQUIRYRESULTEVENT_H
