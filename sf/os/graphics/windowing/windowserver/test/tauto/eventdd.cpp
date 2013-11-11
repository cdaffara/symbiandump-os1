// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <kernel/kernel.h>
#include "eventdd.h"

/**
  Logical Device (factory class) for 'EventDD'
*/
class DEventFactory : public DLogicalDevice
	{
public:
	DEventFactory();
	//Pure virtual funcitons from DLogicalDevice
	TInt Install();
	void GetCaps(TDes8& aDes) const;
	TInt Create(DLogicalChannelBase*& aChannel);
	};

class DEventDD : public DLogicalChannelBase
	{
public:
	//Pure virtual function from DLogicalChannelBase
	TInt Request(TInt aReqNo,TAny* a1,TAny* a2);
private:
	TInt DoControl(TInt aFunction, TAny* a1, TAny* a2);
	TInt SendEvent(TRawEvent* aEvent);
	};


/* DLL Factory Function */

DECLARE_STANDARD_LDD()
	{
	return new DEventFactory;
	}


/* DEventFactory */

DEventFactory::DEventFactory() :DLogicalDevice()
	{
	iVersion=REventDD::VersionRequired();	//Set version number for this device
	}

TInt DEventFactory::Install()
	{
	return SetName(&REventDD::DriverName());
	}

void DEventFactory::GetCaps(TDes8& aDes) const
	{
	Kern::InfoCopy(aDes,KNullDesC8);
	}

TInt DEventFactory::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel=new DEventDD;
	return aChannel?KErrNone:KErrNoMemory;
	}


/* DEventDD */	// Logical Channel

TInt DEventDD::Request(TInt aReqNo,TAny* a1,TAny* a2)
	{
	// Decode the message type and dispatch it to the relevent handler function...
	// only using synchronous control messages
	if (static_cast<TUint>(aReqNo)<static_cast<TUint>(KMaxTInt))
		return DoControl(aReqNo,a1,a2);
	return KErrNotSupported;
	}

/**
  Process synchronous 'control' requests
*/
TInt DEventDD::DoControl(TInt aFunction,TAny* a1,TAny* /*a2*/)
	{
	TInt ret=KErrNotSupported;

	switch (aFunction)
		{
	case REventDD::ESendEvent:
		ret=SendEvent(static_cast<TRawEvent*>(a1));
		break;
	default:;
		}
	return ret;
	}

TInt DEventDD::SendEvent(TRawEvent* aEvent)
	{
	TRawEvent event;
	kumemget(&event,aEvent,sizeof(TRawEvent));	//fetch event from user memory
	NKern::ThreadEnterCS();
	TInt err=Kern::AddEvent(event);
	NKern::ThreadLeaveCS();
	return err;
	}
