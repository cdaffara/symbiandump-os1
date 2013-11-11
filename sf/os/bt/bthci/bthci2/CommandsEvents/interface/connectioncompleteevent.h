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

#ifndef CONNECTIONCOMPLETEEVENT_H
#define CONNECTIONCOMPLETEEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the ConnectionComplete HCI Event
*/
class TConnectionCompleteEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TConnectionCompleteEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TConnectionCompleteEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, const TBTDevAddr& aBDADDR, TUint8 aLinkType, TUint8 aEncryptionMode, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TConnectionCompleteEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TConnectionCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TUint8 LinkType() const;
	IMPORT_C TUint8 EncryptionMode() const;
	
	};
	
#endif // CONNECTIONCOMPLETEEVENT_H
