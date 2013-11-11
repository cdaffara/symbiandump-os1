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
// on Wed, 05 Dec 2007 14:54:31 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef USERPASSKEYNOTIFICATIONEVENT_H
#define USERPASSKEYNOTIFICATIONEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the UserPasskeyNotification HCI Event
*/
class TUserPasskeyNotificationEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TUserPasskeyNotificationEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TUserPasskeyNotificationEvent(const TBTDevAddr& aBDADDR, TUint32 aPasskey, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TUserPasskeyNotificationEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TUserPasskeyNotificationEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TUint32 Passkey() const;
	
	};
	
#endif // USERPASSKEYNOTIFICATIONEVENT_H
