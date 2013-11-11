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
// netfinaldataDataBus.cpp
// 
//

/**
 @file
 @InternalComponent
*/

#include "cfinalnetdatabus.h"
#include "lbsdevloggermacros.h"

/**
Create a net final data bus, this should be called from the location cache

@return The pointer to the location data bus
@see CLocationCache
*/
CFinalNetDataBus* CFinalNetDataBus::NewL(MDataBusObserver& aObserver)	
	{
	CFinalNetDataBus* self = new (ELeave) CFinalNetDataBus(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	} 
	
/**
constructor, note the location data bus is owned by location cache and used by CRequestQ, and it is a module-based CActive object.
Set the data bus as standard priority by default.
Add itself to Active Scheduler

@param aModuleId Position module Id
@see CRequestQ
@see CLocationCache
*/
CFinalNetDataBus::CFinalNetDataBus(MDataBusObserver& aObserver) : CActive(CActive::EPriorityStandard), iObs(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/**
Open the RProperty object wrappered by the data bus.
Location data bus is implemented as Publish & Subscribe mechanism.
*/
void CFinalNetDataBus::ConstructL()
	{
	iFinalNetDataBus.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionFinal);
	}

/**
Cancel any outstanding request to the CActive object.
Close the RProperty.
*/
CFinalNetDataBus::~CFinalNetDataBus()
	{
	Cancel(); // just in case...
	iFinalNetDataBus.Close();
	}


/**
Subscribe to the data bus. It will be called by objects which listen to the data bus.
It will eventually call RProperty::Subscribe()
Set active the data bus active object
Panic if there are no data bus observers already set.
*/
void CFinalNetDataBus::Subscribe()
	{
	if(!IsActive())
		{
		iFinalNetDataBus.NotifyNetworkLocationUpdate(iStatus);
		SetActive();
		}
	}

/**
Cancel the subscribe to the data bus. Cancel any outstanding request to the CActive object.
Panic if there are no data bus observers already set.
*/
void CFinalNetDataBus::UnSubscribe()
	{
	Cancel();
	}

/**
Get the last position from the bus 
 */
TInt CFinalNetDataBus::GetLastPositionInfo(TPositionInfo& aFNPInfo, TTime& aActualTime)
	{
	TLbsNetSessionIdInt sessionId;
	TTime targetTime;
	
	TInt error = iFinalNetDataBus.GetPositionInfo(sessionId, aFNPInfo, targetTime, aActualTime);
	if(error == KErrNone && (aFNPInfo.PositionMode() & TPositionModuleInfo::ETechnologyTerminal))
		{
		error = KErrNotFound;	// filter out terminal assisted FNPs
		}
	return error;
	}

/**
from CActive, the main event processing loop
Whenever there is a new event, i.e. the position info data is published to the data bus, notify the data bus observer by LocationDataAvailableL()
*/
void CFinalNetDataBus::RunL()
	{
	TLbsNetSessionIdInt sessionId;
	TTime targetTime;
	TTime actualTime;
	
	TInt error = iFinalNetDataBus.GetPositionInfo(sessionId, iPositionInfo, targetTime, actualTime);
	
	if(iPositionInfo.PositionMode() & TPositionModuleInfo::ETechnologyNetwork)// filter out terminal assisted FNPs
		{
		iObs.DataUpdate(iPositionInfo, iStatus.Int(), error, actualTime);
		}
	}
	
/**
from CActive
Cancels the request for position update notification
It will eventually cancel an outstanding subscription request for this property handle.
*/
void CFinalNetDataBus::DoCancel()
	{
	iFinalNetDataBus.CancelNotifyNetworkLocationUpdate();
	}
	
/**
from CActive
Simply fwds any errors to the observers
*/
TInt CFinalNetDataBus::RunError(TInt aError)
	{
	return iObs.DataError(aError, iStatus.Int());
	}

