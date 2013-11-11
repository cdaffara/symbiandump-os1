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
// on Sat, 31 May 2008 18:58:41 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef READLOCALOOBDATACOMPLETEEVENT_H
#define READLOCALOOBDATACOMPLETEEVENT_H

#include <bluetooth/hci/commandcompleteevent.h>
#include <bttypespartner.h>

/**
This class represents command completion event for the ReadLocalOOBData HCI command
*/

class TReadLocalOOBDataCompleteEvent : public THCICommandCompleteEvent
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TReadLocalOOBDataCompleteEvent(const TDesC8& aEventData);
	// Construct an event object to generate a faked event using the supplied parameters
	IMPORT_C TReadLocalOOBDataCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, const TBluetoothSimplePairingHash& aOOBDataC, const TBluetoothSimplePairingRandomizer& aOOBDataR, TDes8& aEventData);

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TReadLocalOOBDataCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TBluetoothSimplePairingHash OOBDataC() const;
	IMPORT_C TBluetoothSimplePairingRandomizer OOBDataR() const;
	
	};
	
#endif // READLOCALOOBDATACOMPLETEEVENT_H
