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

#ifndef REMOTEHOSTSUPPORTEDFEATURESNOTIFICATIONEVENT_H
#define REMOTEHOSTSUPPORTEDFEATURESNOTIFICATIONEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the RemoteHostSupportedFeaturesNotification HCI Event
*/
class TRemoteHostSupportedFeaturesNotificationEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TRemoteHostSupportedFeaturesNotificationEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TRemoteHostSupportedFeaturesNotificationEvent(const TBTDevAddr& aBDADDR, TUint64 aHostSupportedFeatures, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TRemoteHostSupportedFeaturesNotificationEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TRemoteHostSupportedFeaturesNotificationEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TUint64 HostSupportedFeatures() const;
	
	};
	
#endif // REMOTEHOSTSUPPORTEDFEATURESNOTIFICATIONEVENT_H
