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
// on Wed, 25 Jul 2007 17:00:37 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef READLOCALNAMECOMPLETEEVENT_H
#define READLOCALNAMECOMPLETEEVENT_H

#include <bluetooth/hci/commandcompleteevent.h>

/**
This class represents command completion event for the ReadLocalName HCI command
*/

class TReadLocalNameCompleteEvent : public THCICommandCompleteEvent
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TReadLocalNameCompleteEvent(const TDesC8& aEventData);
	// Construct an event object to generate a faked event using the supplied parameters
	IMPORT_C TReadLocalNameCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, const TDesC8& aLocalName, TDes8& aEventData);

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TReadLocalNameCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TPtrC8 LocalName() const;
	
	};
	
#endif // READLOCALNAMECOMPLETEEVENT_H
