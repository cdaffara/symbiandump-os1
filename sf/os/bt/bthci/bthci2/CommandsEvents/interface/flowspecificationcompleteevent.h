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

#ifndef FLOWSPECIFICATIONCOMPLETEEVENT_H
#define FLOWSPECIFICATIONCOMPLETEEVENT_H

#include <bluetooth/hci/event.h>

/**
This class represents the FlowSpecificationComplete HCI Event
*/
class TFlowSpecificationCompleteEvent : public THCIEventBase
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TFlowSpecificationCompleteEvent(const TDesC8& aEventData);
	// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
	IMPORT_C TFlowSpecificationCompleteEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aFlowDirection, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aTokenBucketSize, TUint32 aPeakBandwidth, TUint32 aAccessLatency, TDes8& aEventData);
	// Destructor
	IMPORT_C ~TFlowSpecificationCompleteEvent();

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TFlowSpecificationCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint8 Flags() const;
	IMPORT_C TUint8 FlowDirection() const;
	IMPORT_C TUint8 ServiceType() const;
	IMPORT_C TUint32 TokenRate() const;
	IMPORT_C TUint32 TokenBucketSize() const;
	IMPORT_C TUint32 PeakBandwidth() const;
	IMPORT_C TUint32 AccessLatency() const;
	
	};
	
#endif // FLOWSPECIFICATIONCOMPLETEEVENT_H
