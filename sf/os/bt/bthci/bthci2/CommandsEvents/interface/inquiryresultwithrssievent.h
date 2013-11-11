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
// on Mon, 05 Mar 2007 17:38:38 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef INQUIRYRESULTWITHRSSIEVENT_H
#define INQUIRYRESULTWITHRSSIEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the InquiryResultwithRSSI HCI Event
*/
class TInquiryResultwithRSSIEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TInquiryResultwithRSSIEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TInquiryResultwithRSSIEvent(TUint8 aNumResponses, const TArray< TBTDevAddr >& aBDADDR, const TArray< THCIPageScanRepetitionMode >& aPageScanRepetitionMode, const TArray< THCIReservedOctet >& aReserved, const TArray< TUint32 >& aClassOfDevice, const TArray< THCIClockOffset >& aClockOffset, const TArray< THCIRssi >& aRSSI, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TInquiryResultwithRSSIEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TInquiryResultwithRSSIEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TUint8 NumResponses() const;
	IMPORT_C TBTDevAddr BDADDR(TInt aIndex) const;
	IMPORT_C THCIPageScanRepetitionMode PageScanRepetitionMode(TInt aIndex) const;
	IMPORT_C THCIReservedOctet Reserved(TInt aIndex) const;
	IMPORT_C TUint32 ClassOfDevice(TInt aIndex) const;
	IMPORT_C THCIClockOffset ClockOffset(TInt aIndex) const;
	IMPORT_C THCIRssi RSSI(TInt aIndex) const;
	
	};
	
#endif // INQUIRYRESULTWITHRSSIEVENT_H
