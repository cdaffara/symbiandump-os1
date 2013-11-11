// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released since 9.5
*/

#ifndef SYMBIAN_NODEMESSAGES_AVAILABILITY_H
#define SYMBIAN_NODEMESSAGES_AVAILABILITY_H

#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_selector.h>

namespace ESock
{

/**
Message interface for availability provider nodes.
Nodes wishing to implement this interface must implement all message protocols in the interface.
*/
class TCFAvailabilityProvider
	{
private:
  	enum
 	{
	ECFAvailabilityNotificationRegistration	= 1,
	};
	
protected:

public:
    enum { ERealmId = 0x10285F3E }; //UID allocated on 4/6/08 from KUidNodeMessageRealms 

    /**
	Sent by an availability subscriber (usually a control client of a meta provider) to its
    availability providers registering for availability notifications.
	This message will kick off an availability monitoring activity on its recipient. This
	activity will run forever unless an error occurs or it is cancelled.

       @li Type: Request for TAvailabilityNotification.
	   @li Fallible: Yes

	@param iSubscriptionOptions properties of the registration request.
    */
 	typedef TMessageSigAvailabilitySubscriptionOptions<ECFAvailabilityNotificationRegistration, TCFAvailabilityProvider::ERealmId> TAvailabilityNotificationRegistration;
	};

/**
Message interface for availability client nodes.
Nodes wishing to implement this interface must implement all message protocols in the interface.
*/
class TCFAvailabilityControlClient
	{
  private:
  	enum
 	{
 	ECFAvailabilityNotification		= 1,
	};
protected:

public:
    enum { ERealmId = 0x10285F3D }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
    /**
	Response/Notification sent by an availability provider (usually a meta provider)
	to its availability subscribers registered for availability notifications.
	First TAvailabilityNotification message is an expected response.
	Subsequent TAvailabilityNotification messages are notifications.

	   @li Type: Response/Notification for TAvailabilityNotificationRegistration.
	   @li Fallible:  No

	@param iAvailabilityStatus availability score etc as per TAvailabilityNotificationRegistration.
	*/
 	typedef TMessageSigAvailabilityStatus<ECFAvailabilityNotification, TCFAvailabilityControlClient::ERealmId> TAvailabilityNotification;

	};

} //namespace ESock

#endif //SYMBIAN_NODEMESSAGES_AVAILABILITY_H

