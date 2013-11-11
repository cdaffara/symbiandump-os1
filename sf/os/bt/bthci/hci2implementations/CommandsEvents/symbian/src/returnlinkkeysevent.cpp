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
// on Wed, 25 Jul 2007 17:00:42 (time stamp)
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/hci/returnlinkkeysevent.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hciopcodes.h>
#include "symbiancommandseventsutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

// Construct an event object to wrap existing event data received from the Controller
EXPORT_C TReturnLinkKeysEvent::TReturnLinkKeysEvent(const TDesC8& aEventData)
	: THCIEventBase(aEventData)
	{
	
	}

// Construct a faked event, storing the supplied event parameters into the supplied empty event data buffer.
EXPORT_C TReturnLinkKeysEvent::TReturnLinkKeysEvent(TUint8 aNumKeys, const TArray< TBTDevAddr >& aBDADDR, const TArray< TBTLinkKey >& aLinkKey, TDes8& aEventData)
	: THCIEventBase(EReturnLinkKeysEvent, aNumKeys * 22 + 1, aEventData)
	{
	
	PutTUint8(aNumKeys, aEventData);
	for(int i=0;i<aNumKeys;++i)
		{
		PutDevAddr(aBDADDR[i], aEventData);
		PutLinkKey(aLinkKey[i], aEventData);
		}
	iEventData.Set(aEventData);
	}

// Destructor
EXPORT_C TReturnLinkKeysEvent::~TReturnLinkKeysEvent()
	{
	}

// The static Cast method is used to obtain a pointer to the derived class object
EXPORT_C TReturnLinkKeysEvent& TReturnLinkKeysEvent::Cast(const THCIEventBase& aEvent)
	{
	__ASSERT_DEBUG(aEvent.EventCode() == EReturnLinkKeysEvent, PANIC(KSymbianCommandsEventsPanicCat, EWrongEventCode));
	return *(reinterpret_cast<TReturnLinkKeysEvent*>(&const_cast<THCIEventBase&>(aEvent)));
	}

// Accessor methods for the parameters of the event

EXPORT_C TUint8 TReturnLinkKeysEvent::NumKeys() const
	{
	return AsTUint8(2);
	}

EXPORT_C TBTDevAddr TReturnLinkKeysEvent::BDADDR(TInt aIndex) const
	{
	return AsDevAddr(3, 22, aIndex);
	}

EXPORT_C TBTLinkKey TReturnLinkKeysEvent::LinkKey(TInt aIndex) const
	{
	return AsLinkKey(9, 22, aIndex);
	}


