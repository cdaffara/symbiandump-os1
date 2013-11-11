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
//

/**
 @file
 @publishedPartner
 @released
*/


#ifndef VENDORDEBUGEVENT_H
#define VENDORDEBUGEVENT_H

#include <bluetooth/hci/event.h>

class TVendorDebugEvent : public THCIEventBase
	{
public:
	/**
	Constructor for creating an event object to wrap existing event data
	@param aEventData the existing event data
	*/
	IMPORT_C TVendorDebugEvent(const TDesC8& aEventData);
	/**
	Constructor for faking a Vendor Debug event
	@param aEventCode the event code for the event
	@param aEventPayload the vendor-defined data content of the event
	@param aEventData buffer where the faked event data will be constructed (must persist as long as the object)
	*/
	IMPORT_C TVendorDebugEvent(THCIEventCode aEventCode, const TDesC8& aEventPayload,
			TDes8& aEventData);

	/**
	Static downcasting method
	*/
	IMPORT_C static TVendorDebugEvent& Cast(const THCIEventBase& aEvent);
	/**
	Accessor method returning the vendor-defined payload of the event
	*/
	IMPORT_C TPtrC8 VendorDebugData() const;
	/**
	Accessor method returning the whole of the event data including the event code and parameter total length fields
	*/
	IMPORT_C TPtrC8 VendorDebugEvent() const;
	};

#endif // VENDORDEBUGEVENT_H
