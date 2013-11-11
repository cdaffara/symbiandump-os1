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
// This file was generated automatically from the template completeeventheader.tmpl
// on Wed, 09 May 2007 15:39:11 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef READEXTENDEDINQUIRYRESPONSECOMPLETEEVENT_H
#define READEXTENDEDINQUIRYRESPONSECOMPLETEEVENT_H

#include <bluetooth/hci/commandcompleteevent.h>

/**
This class represents command completion event for the ReadExtendedInquiryResponse HCI command
*/

class TReadExtendedInquiryResponseCompleteEvent : public THCICommandCompleteEvent
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TReadExtendedInquiryResponseCompleteEvent(const TDesC8& aEventData);
	// Construct an event object to generate a faked event using the supplied parameters
	IMPORT_C TReadExtendedInquiryResponseCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, TUint8 aFECRequired, const TDesC8& aExtendedInquiryResponse, TDes8& aEventData);

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TReadExtendedInquiryResponseCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TUint8 FECRequired() const;
	IMPORT_C TPtrC8 ExtendedInquiryResponse() const;
	
	};
	
#endif // READEXTENDEDINQUIRYRESPONSECOMPLETEEVENT_H
