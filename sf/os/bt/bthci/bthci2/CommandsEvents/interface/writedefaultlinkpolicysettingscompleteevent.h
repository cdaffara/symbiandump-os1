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

#ifndef WRITEDEFAULTLINKPOLICYSETTINGSCOMPLETEEVENT_H
#define WRITEDEFAULTLINKPOLICYSETTINGSCOMPLETEEVENT_H

#include <bluetooth/hci/commandcompleteevent.h>

/**
This class represents command completion event for the WriteDefaultLinkPolicySettings HCI command
*/

class TWriteDefaultLinkPolicySettingsCompleteEvent : public THCICommandCompleteEvent
	{
public:
	// Construct an event object to wrap existing event data received from the Controller
	IMPORT_C TWriteDefaultLinkPolicySettingsCompleteEvent(const TDesC8& aEventData);
	// Construct an event object to generate a faked event using the supplied parameters
	IMPORT_C TWriteDefaultLinkPolicySettingsCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, TDes8& aEventData);

	// The static Cast method is used to obtain a pointer to the derived class object
	IMPORT_C static TWriteDefaultLinkPolicySettingsCompleteEvent& Cast(const THCIEventBase& aEvent);

	// Accessor methods for the parameters of the event
	
	};
	
#endif // WRITEDEFAULTLINKPOLICYSETTINGSCOMPLETEEVENT_H
