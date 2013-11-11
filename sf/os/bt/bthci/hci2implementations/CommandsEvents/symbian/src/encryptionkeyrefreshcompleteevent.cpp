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
// This file was generated automatically from the template eventsource.tmpl
// on Thu, 29 May 2008 15:23:55 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/encryptionkeyrefreshcompleteevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TEncryptionKeyRefreshCompleteEvent::TEncryptionKeyRefreshCompleteEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	iErrorCode = THCIErrorCode(iEventData[2]); // Status field is in octet 3
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TEncryptionKeyRefreshCompleteEvent::TEncryptionKeyRefreshCompleteEvent(THCIErrorCode aStatus, THCIConnectionHandle aConnectionHandle, TDes8& aEventData)
	: THCIEventBase(EEncryptionKeyRefreshCompleteEvent, 3, aEventData)
	{
	iErrorCode = aStatus;
	PutTUint8(aStatus, aEventData);
	PutConnectionHandle(aConnectionHandle, aEventData);
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TEncryptionKeyRefreshCompleteEvent::~TEncryptionKeyRefreshCompleteEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TEncryptionKeyRefreshCompleteEvent& TEncryptionKeyRefreshCompleteEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EEncryptionKeyRefreshCompleteEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TEncryptionKeyRefreshCompleteEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C THCIConnectionHandle TEncryptionKeyRefreshCompleteEvent::ConnectionHandle() const
	{
	return AsConnectionHandle(3);
	}


