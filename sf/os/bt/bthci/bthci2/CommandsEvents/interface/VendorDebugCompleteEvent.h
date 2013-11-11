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
//

/**
 @file
 @publishedPartner
 @released
*/


#ifndef VENDORDEBUGCOMPLETEEVENT_H
#define VENDORDEBUGCOMPLETEEVENT_H

#include <bluetooth/hci/commandcompleteevent.h>

NONSHARABLE_CLASS(TVendorDebugCompleteEvent) : public THCICommandCompleteEvent
	{
public:
	IMPORT_C TVendorDebugCompleteEvent(const TDesC8& aEventData);
	IMPORT_C TVendorDebugCompleteEvent(TUint8 aNumHCICommandPackets, THCIOpcode aCommandOpcode,
			const TDesC8& aEventPayload, TDes8& aEventData);

	IMPORT_C static TVendorDebugCompleteEvent& Cast(const THCIEventBase& aEvent);
	IMPORT_C TPtrC8 VendorDebugData() const;
	IMPORT_C TPtrC8 VendorDebugEvent() const;
	};

#endif // VENDORDEBUGCOMPLETEEVENT_H
