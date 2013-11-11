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
// on Mon, 02 Jun 2008 14:47:51 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef LINKSUPERVISIONTIMEOUTCHANGEDEVENT_H
#define LINKSUPERVISIONTIMEOUTCHANGEDEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the LinkSupervisionTimeoutChanged HCI Event
*/
class TLinkSupervisionTimeoutChangedEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TLinkSupervisionTimeoutChangedEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TLinkSupervisionTimeoutChangedEvent(THCIConnectionHandle aConnectionHandle, TBasebandTime aLinkSupervisionTimeout, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TLinkSupervisionTimeoutChangedEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TLinkSupervisionTimeoutChangedEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TBasebandTime LinkSupervisionTimeout() const;
	
	};
	
#endif // LINKSUPERVISIONTIMEOUTCHANGEDEVENT_H
