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
// commandcompletevent.cpp
// 
//

/**
 @file
*/

#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciopcodes.h>

#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

THCICommandCompleteEvent::THCICommandCompleteEvent(TUint8 aParameterTotalLength,
		TUint8 aNumHCICommandPackets, THCIOpcode aCommandOpcode, TDes8& aEventData)
	: THCIEventBase(ECommandCompleteEvent, aParameterTotalLength, aEventData)
	{
	PutTUint8(aNumHCICommandPackets, aEventData);	// Num_HCI_Command_Packets
	PutTUint16(aCommandOpcode, aEventData);			// Command_Opcode
	// It is the responsibility of the most derived class to set iEventData
	// from aEventData after event-specific data has been added in.	
	}
	
THCICommandCompleteEvent::THCICommandCompleteEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
  	{
  	}

EXPORT_C TUint8 THCICommandCompleteEvent::NumHCICommandPackets() const
	{
	//Command credits are 3rd byte (octet) of the iEventData
	return(iEventData[2]);
	}

EXPORT_C THCIOpcode THCICommandCompleteEvent::CommandOpcode() const
	{
  	// Command_Opcode is in the 4th and 5th octet of a Command_Complete event
	return AsTUint16(3);
	}
	
EXPORT_C THCICommandCompleteEvent& THCICommandCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_ALWAYS(aEvent.EventCode()==ECommandCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<THCICommandCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}
