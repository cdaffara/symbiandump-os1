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
// on Thu, 29 May 2008 15:16:48 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef READDEFAULTERRONEOUSDATAREPORTINGCOMPLETEEVENT_H
#define READDEFAULTERRONEOUSDATAREPORTINGCOMPLETEEVENT_H

#include <bluetooth/hci/commandcompleteevent.h>

/**
This class represents command completion event for the ReadDefaultErroneousDataReporting HCI command
*/

class TReadDefaultErroneousDataReportingCompleteEvent : public THCICommandCompleteEvent
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TReadDefaultErroneousDataReportingCompleteEvent(const TDesC8& aEventData);
	// Construct an event object to generate a faked event using the supplied parameters
	IMPORT_C TReadDefaultErroneousDataReportingCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, TUint8 aErroneousDataReporting, TDes8& aEventData);

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TReadDefaultErroneousDataReportingCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C TUint8 ErroneousDataReporting() const;
	
	};
	
#endif // READDEFAULTERRONEOUSDATAREPORTINGCOMPLETEEVENT_H
