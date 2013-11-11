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
// on Wed, 25 Jul 2007 17:00:36 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef READBUFFERSIZECOMPLETEEVENT_H
#define READBUFFERSIZECOMPLETEEVENT_H

#include <bluetooth/hci/commandcompleteevent.h>

/**
This class represents command completion event for the ReadBufferSize HCI command
*/

class TReadBufferSizeCompleteEvent : public THCICommandCompleteEvent
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TReadBufferSizeCompleteEvent(const TDesC8& aEventData);
	// Construct an event object to generate a faked event using the supplied parameters
	IMPORT_C TReadBufferSizeCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, TUint16 aHCACLDataPacketLength, TUint8 aHCSynchronousDataPacketLength, TUint16 aHCTotalNumACLDataPackets, TUint16 aHCTotalNumSynchronousDataPackets, TDes8& aEventData);

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TReadBufferSizeCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TUint16 HCACLDataPacketLength() const;
	IMPORT_C TUint8 HCSynchronousDataPacketLength() const;
	IMPORT_C TUint16 HCTotalNumACLDataPackets() const;
	IMPORT_C TUint16 HCTotalNumSynchronousDataPackets() const;
	
	};
	
#endif // READBUFFERSIZECOMPLETEEVENT_H
