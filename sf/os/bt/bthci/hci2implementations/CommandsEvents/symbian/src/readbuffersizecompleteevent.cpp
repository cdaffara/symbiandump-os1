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
// This file was generated automatically from the template completeeventsource.tmpl
// on Wed, 25 Jul 2007 17:00:36 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/readbuffersizecompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TReadBufferSizeCompleteEvent::TReadBufferSizeCompleteEvent(const TDesC8& aEventData)
	: THCICommandCompleteEvent(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[5]); // Status field is in octet 6
	}

// Construct an event object to generate a faked event using the supplied parameters
EXPORT_C TReadBufferSizeCompleteEvent::TReadBufferSizeCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, TUint16 aHCACLDataPacketLength, TUint8 aHCSynchronousDataPacketLength, TUint16 aHCTotalNumACLDataPackets, TUint16 aHCTotalNumSynchronousDataPackets, TDes8& aEventData)
	: THCICommandCompleteEvent(8 + KCommandCompleteCommonFieldsLength, aNumHCICommandPackets, KReadBufferSizeOpcode, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutTUint16(aHCACLDataPacketLength, aEventData);
	PutTUint8(aHCSynchronousDataPacketLength, aEventData);
	PutTUint16(aHCTotalNumACLDataPackets, aEventData);
	PutTUint16(aHCTotalNumSynchronousDataPackets, aEventData);
	iEventData.Set(aEventData);
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TReadBufferSizeCompleteEvent& TReadBufferSizeCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ECommandCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	__ASSERT_DEBUG(THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() == KReadBufferSizeOpcode, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TReadBufferSizeCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint16 TReadBufferSizeCompleteEvent::HCACLDataPacketLength() const
	{
	return AsTUint16(6);
	}

EXPORT_C TUint8 TReadBufferSizeCompleteEvent::HCSynchronousDataPacketLength() const
	{
	return AsTUint8(8);
	}

EXPORT_C TUint16 TReadBufferSizeCompleteEvent::HCTotalNumACLDataPackets() const
	{
	return AsTUint16(9);
	}

EXPORT_C TUint16 TReadBufferSizeCompleteEvent::HCTotalNumSynchronousDataPackets() const
	{
	return AsTUint16(11);
	}


