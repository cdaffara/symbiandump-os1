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
//

/**
 @file
 @InternalComponent
*/

#include "cagpsdatabus.h"
#include "lbsdevloggermacros.h"
#include "mdatabusobserver.h"
#include <lbs/lbsextendedsatellite.h>

#ifdef _DEBUG
_LIT(KLocDataBus, "CAgpsDataBus");
#endif

/**
Create a location data bus, this should be called from the location cache

@return The pointer to the location data bus
@see CLocationCache
*/
CAgpsDataBus* CAgpsDataBus::NewL(MDataBusObserver& aObserver, const TUid& aModuleId)	
	{
	CAgpsDataBus* self = new (ELeave) CAgpsDataBus(aObserver, aModuleId);
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
CAgpsDataBus::CAgpsDataBus(MDataBusObserver& aObserver, const TUid& aModuleId) : CActive(CActive::EPriorityStandard), iModuleId(aModuleId), iObs(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/**
Open the RProperty object wrappered by the data bus.
Location data bus is implemented as Publish & Subscribe mechanism.
*/
void CAgpsDataBus::ConstructL()
	{
	iAGPSDataBus.OpenL(iModuleId);
	}

/**
Cancel any outstanding request to the CActive object.
Close the RProperty.
*/
CAgpsDataBus::~CAgpsDataBus()
	{
	Cancel();
	iAGPSDataBus.Close();
	}

/**
Subscribe to the data bus. It will be called by objects which listen to the data bus.
It will eventually call RProperty::Subscribe()
Set active the data bus active object
Panic if there are no data bus observers already set.
*/
void CAgpsDataBus::Subscribe()
	{
	if(!IsActive())
		{
		iAGPSDataBus.NotifyPositionUpdate(iStatus);
		SetActive();
		}
	}

/**
Cancel the subscribe to the data bus. Cancel any outstanding request to the CActive object.
Panic if there are no data bus observers already set.
*/
void CAgpsDataBus::UnSubscribe()
	{
	Cancel();
	}

/**
Get the last position from the bus 
 
 */
TInt CAgpsDataBus::GetLastPositionInfo(TPositionSatelliteInfo& aSatelliteInfo, TTime& aActualTime, TUint& aAttributes, TUint& aMode)
	{
	TBool conflictControl;
	TTime targetTime;

	TInt error = iAGPSDataBus.GetPositionInfo(conflictControl, &aSatelliteInfo, aSatelliteInfo.PositionClassSize(), targetTime, aActualTime, aAttributes, aMode);
	
	return error;
	}


/**
from CActive, the main event processing loop
Whenever there is a new event, i.e. the position info data is published to the data bus, notify the data bus observer by LocationDataAvailableL()
*/
void CAgpsDataBus::RunL()
	{
	TTime actualTime;
	TPositionExtendedSatelliteInfo satelliteInfo;
	TUint attributes = 0;
	TUint mode = 0;
	
	TInt error = GetLastPositionInfo(satelliteInfo, actualTime, attributes, mode);	

	iObs.DataUpdate(satelliteInfo, iStatus.Int(), error, actualTime, attributes, mode);

	}
	
/**
from CActive
Cancels the request for position update notification
It will eventually cancel an outstanding subscription request for this property handle.
*/
void CAgpsDataBus::DoCancel()
	{
	iAGPSDataBus.CancelNotifyPositionUpdate();
	}
	
/**
from CActive
Simply fwds any errors to the observers
*/
TInt CAgpsDataBus::RunError(TInt aError)
	{
	return iObs.DataError(aError, iStatus.Int());
	}
