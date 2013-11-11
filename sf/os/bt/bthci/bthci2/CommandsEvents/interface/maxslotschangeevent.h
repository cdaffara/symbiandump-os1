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

#ifndef MAXSLOTSCHANGEEVENT_H
#define MAXSLOTSCHANGEEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the MaxSlotsChange HCI Event
*/
class TMaxSlotsChangeEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TMaxSlotsChangeEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TMaxSlotsChangeEvent(THCIConnectionHandle aConnectionHandle, TUint8 aLMPMaxSlots, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TMaxSlotsChangeEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TMaxSlotsChangeEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint8 LMPMaxSlots() const;
	
	};
	
#endif // MAXSLOTSCHANGEEVENT_H
