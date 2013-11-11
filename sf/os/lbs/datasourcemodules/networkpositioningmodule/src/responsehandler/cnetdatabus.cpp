// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// NetDataBus.cpp
// 
//

/**
 @file
 @InternalComponent
*/

#include "cnetdatabus.h"
#include "lbsdevloggermacros.h"
#include <lbssatellite.h>

#ifdef _DEBUG
_LIT(KNetDataBus, "CNetDataBus");
#endif

/**
Create a location data bus, this should be called from the response handler

@return The pointer to the location data bus
@see CLocationCache
*/
CNETDataBus* CNETDataBus::NewL(MDataBusObserver* aObserver, const TUid& aModuleId)	
	{
	CNETDataBus* self = new (ELeave) CNETDataBus(aObserver, aModuleId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	} 
	
/**
constructor, note the location data bus and is a CActive object.
Set the data bus as standard priority by default.
Add itself to Active Scheduler

@param aModuleId Position module Id
@see CRequestQ
@see CLocationCache
*/
CNETDataBus::CNETDataBus(MDataBusObserver* aObserver, const TUid& aModuleId) : CActive(CActive::EPriorityStandard), iObs(aObserver), iModuleId(aModuleId)
	{
	CActiveScheduler::Add(this);
	}

/**
Open the RProperty object wrappered by the data bus.
Location data bus is implemented as Publish & Subscribe mechanism.
*/
void CNETDataBus::ConstructL()
	{
	iNETDataBus.OpenL(iModuleId);
	}

/**
Cancel any outstanding request to the CActive object.
Close the RProperty.
*/
CNETDataBus::~CNETDataBus()
	{
	Cancel();
	iNETDataBus.Close();
	}

/**
Subscribe to the data bus. It will be called by objects which listen to the data bus.
It will eventually call RProperty::Subscribe()
Set active the data bus active object
Panic if there are no data bus observers already set.
*/
void CNETDataBus::Subscribe()
	{
	if(!IsActive())
		{
		__ASSERT_DEBUG(iObs, User::Panic(KNetDataBus, ENetDataBusObserverNotSet));
		iNETDataBus.NotifyPositionUpdate(iStatus);
		SetActive();
		}
	}

/**
Cancel the subscribe to the data bus. Cancel any outstanding request to the CActive object.
Panic if there are no data bus observers already set.
*/
void CNETDataBus::UnSubscribe()
	{
	__ASSERT_DEBUG(iObs, User::Panic(KNetDataBus, ENetDataBusObserverNotSet));
	Cancel();
	}


/**
Get the last position from the bus 
 */
TInt CNETDataBus::GetLastPositionInfo(TPositionInfo& aPosInfo, TTime& aActualTime)
	{
	TBool conflictControl;
	TTime targetTime;
	TUint attributes;
	TUint mode;
	
	TInt error = iNETDataBus.GetPositionInfo(conflictControl, &aPosInfo, aPosInfo.PositionClassSize(), targetTime, aActualTime, attributes, mode);
	
	return error;
	}

/**
from CActive, the main event processing loop
Whenever there is a new event, i.e. the position info data is published to the data bus, notify the data bus observer
*/
void CNETDataBus::RunL()
	{
	TTime actualTime;

	TInt error = GetLastPositionInfo(iPositionInfo, actualTime);

	iObs->DataUpdate(iPositionInfo, iStatus.Int(), error, actualTime);
	}
	
/**
from CActive
Cancels the request for position update notification
It will eventually cancel an outstanding subscription request for this property handle.
*/
void CNETDataBus::DoCancel()
	{
	iNETDataBus.CancelNotifyPositionUpdate();
	}
	
/**
from CActive
Simply fwds any errors to the observers
*/
TInt CNETDataBus::RunError(TInt aError)
	{
	return iObs->DataError(aError, iStatus.Int());
	}


