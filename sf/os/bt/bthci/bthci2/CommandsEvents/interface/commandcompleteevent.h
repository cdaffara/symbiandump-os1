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


#ifndef THCICOMMANDCOMPLETEEVENT_H
#define THCICOMMANDCOMPLETEEVENT_H

#include <bluetooth/hci/event.h>

NONSHARABLE_CLASS(THCICommandCompleteEvent) : public THCIEventBase
	{
public:
	IMPORT_C TUint8 NumHCICommandPackets() const;

	IMPORT_C THCIOpcode CommandOpcode() const;

	/**
	Casts a base event type to this specific class type

	@param aEvent An instance of the HCI event data base class to be cast.
	@return The event instance (passsed as the function parameter) as a this specific class type.
	*/
	IMPORT_C static THCICommandCompleteEvent& Cast(const THCIEventBase& aEvent);

public:
	// Command Complete Event layout constants - lengths of fields that occur at the start
	// of all Command Complete events (after the fields common to all events)
	const static TUint KNumHCICommandPacketsLength = 1;
	const static TUint KCommandOpcodeLength = 2;
	const static TUint KCommandCompleteCommonFieldsLength =
				KNumHCICommandPacketsLength + KCommandOpcodeLength;

protected:
	/**
	This is used for creating a faked event. An emtpy event data buffer is supplied which must be populated.
	@internalComponent This is publishedPartner really.
	*/
	THCICommandCompleteEvent(TUint8 aParameterTotalLength,
			TUint8 aNumHCICommandPackets, THCIOpcode aCommandOpcode, TDes8& aEventData);

	/**
	This is used for creating an event object to wrap an existing event data buffer
	@internalComponent This is publishedPartner really.
	*/
	THCICommandCompleteEvent(const TDesC8& aEventData);
	};

#endif // THCICOMMANDCOMPLETEEVENT_H

