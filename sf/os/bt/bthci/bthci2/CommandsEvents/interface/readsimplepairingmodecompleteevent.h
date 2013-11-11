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
// on Wed, 05 Dec 2007 14:40:05 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef READSIMPLEPAIRINGMODECOMPLETEEVENT_H
#define READSIMPLEPAIRINGMODECOMPLETEEVENT_H

#include <bluetooth/hci/commandcompleteevent.h>

/**
This class represents command completion event for the ReadSimplePairingMode HCI command
*/

class TReadSimplePairingModeCompleteEvent : public THCICommandCompleteEvent
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TReadSimplePairingModeCompleteEvent(const TDesC8& aEventData);
	// Construct an event object to generate a faked event using the supplied parameters
	IMPORT_C TReadSimplePairingModeCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, TUint8 aSimplePairingMode, TDes8& aEventData);

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TReadSimplePairingModeCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TUint8 SimplePairingMode() const;
	
	};
	
#endif // READSIMPLEPAIRINGMODECOMPLETEEVENT_H
