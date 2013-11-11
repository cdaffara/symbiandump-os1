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
// on Mon, 02 Jun 2008 11:36:19 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SNIFFSUBRATINGEVENT_H
#define SNIFFSUBRATINGEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the SniffSubrating HCI Event
*/
class TSniffSubratingEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TSniffSubratingEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TSniffSubratingEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, TBasebandTime aMaximumTransmitLatency, TBasebandTime aMaximumReceiveLatency, TBasebandTime aMinimumRemoteTimeout, TBasebandTime aMinimumLocalTimeout, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TSniffSubratingEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TSniffSubratingEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TBasebandTime MaximumTransmitLatency() const;
	IMPORT_C TBasebandTime MaximumReceiveLatency() const;
	IMPORT_C TBasebandTime MinimumRemoteTimeout() const;
	IMPORT_C TBasebandTime MinimumLocalTimeout() const;
	
	};
	
#endif // SNIFFSUBRATINGEVENT_H
