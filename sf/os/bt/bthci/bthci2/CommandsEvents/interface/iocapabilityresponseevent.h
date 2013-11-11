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
// on Thu, 29 May 2008 15:23:56 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef IOCAPABILITYRESPONSEEVENT_H
#define IOCAPABILITYRESPONSEEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the IOCapabilityResponse HCI Event
*/
class TIOCapabilityResponseEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TIOCapabilityResponseEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TIOCapabilityResponseEvent(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TIOCapabilityResponseEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TIOCapabilityResponseEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TUint8 IOCapability() const;
	IMPORT_C TUint8 OOBDataPresent() const;
	IMPORT_C TUint8 AuthenticationRequirements() const;
	
	};
	
#endif // IOCAPABILITYRESPONSEEVENT_H
