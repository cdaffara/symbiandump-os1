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
// on Wed, 25 Jul 2007 17:00:37 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/readlocalversioninfocompleteevent.h>
#include <bluetooth/hci/hciframe.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TReadLocalVersionInfoCompleteEvent::TReadLocalVersionInfoCompleteEvent(const TDesC8& aEventData)
	: THCICommandCompleteEvent(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[5]); // Status field is in octet 6
	}

// Construct an event object to generate a faked event using the supplied parameters
EXPORT_C TReadLocalVersionInfoCompleteEvent::TReadLocalVersionInfoCompleteEvent(THCIErrorCode aStatus, TUint8 aNumHCICommandPackets, TUint8 aVersion, TUint16 aRevision, TUint8 aLMPVersion, TUint16 aManufacturerName, TUint16 aLMPSubversion, TDes8& aEventData)
	: THCICommandCompleteEvent(9 + KCommandCompleteCommonFieldsLength, aNumHCICommandPackets, KReadLocalVersionInfoOpcode, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutTUint8(aVersion, aEventData);
	PutTUint16(aRevision, aEventData);
	PutTUint8(aLMPVersion, aEventData);
	PutTUint16(aManufacturerName, aEventData);
	PutTUint16(aLMPSubversion, aEventData);
	iEventData.Set(aEventData);
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TReadLocalVersionInfoCompleteEvent& TReadLocalVersionInfoCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == ECommandCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	__ASSERT_DEBUG(THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() == KReadLocalVersionInfoOpcode, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TReadLocalVersionInfoCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint8 TReadLocalVersionInfoCompleteEvent::Version() const
	{
	return AsTUint8(6);
	}

EXPORT_C TUint16 TReadLocalVersionInfoCompleteEvent::Revision() const
	{
	return AsTUint16(7);
	}

EXPORT_C TUint8 TReadLocalVersionInfoCompleteEvent::LMPVersion() const
	{
	return AsTUint8(9);
	}

EXPORT_C TUint16 TReadLocalVersionInfoCompleteEvent::ManufacturerName() const
	{
	return AsTUint16(10);
	}

EXPORT_C TUint16 TReadLocalVersionInfoCompleteEvent::LMPSubversion() const
	{
	return AsTUint16(12);
	}


