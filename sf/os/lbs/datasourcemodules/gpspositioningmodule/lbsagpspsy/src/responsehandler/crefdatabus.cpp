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

#include "crefdatabus.h"
#include "lbsdevloggermacros.h"
#include "mdatabusobserver.h"

#ifdef _DEBUG
_LIT(KRefDataBus, "CRefDataBus");
#endif

/**
Create a reference data bus, this should be called from the location cache

@return The pointer to the reference data bus
@see CLocationCache
*/
CRefDataBus* CRefDataBus::NewL(MDataBusObserver& aObserver)	
	{
	CRefDataBus* self = new (ELeave) CRefDataBus(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	} 
	
/**
constructor, note the reference data bus is owned by location cache and used by CRequestQ, and it is a module-based CActive object.
Set the data bus as standard priority by default.
Add itself to Active Scheduler

@see CRequestQ
@see CLocationCache
*/
CRefDataBus::CRefDataBus(MDataBusObserver& aObserver) : CActive(CActive::EPriorityStandard), iObs(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/**
Open the RProperty object wrappered by the data bus.
Reference data bus is implemented as Publish & Subscribe mechanism.
*/
void CRefDataBus::ConstructL()
	{
	iRefDataBus.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
	}

/**
Cancel any outstanding request to the CActive object.
Close the RProperty.
*/
CRefDataBus::~CRefDataBus()
	{
	Cancel();
	iRefDataBus.Close();
	}

/**
Subscribe to the data bus. It will be called by objects which listen to the data bus.
It will eventually call RProperty::Subscribe()
Set active the data bus active object
Panic if there are no data bus observers already set.
*/
void CRefDataBus::Subscribe()
	{
	if(!IsActive())
		{
		iRefDataBus.NotifyNetworkLocationUpdate(iStatus);
		SetActive();
		}
	}

/**
Cancel the subscribe to the data bus. Cancel any outstanding request to the CActive object.
Panic if there are no data bus observers already set.
*/
void CRefDataBus::UnSubscribe()
	{
	Cancel();
	}

/**
Get the last position from the bus 
 */
TInt CRefDataBus::GetLastPositionInfo(TPositionInfo& aRefInfo, TTime& aActualTime)
	{
	TLbsNetSessionIdInt sessionId;
	TTime targetTime;
	
	TInt error = iRefDataBus.GetPositionInfo(sessionId, aRefInfo, targetTime, aActualTime);
	
	return error;
	}

/**
from CActive, the main event processing loop
Whenever there is a new event, i.e. the position info data is published to the data bus, notify the data bus observer by ReferenceDataAvailableL()
*/
void CRefDataBus::RunL()
	{
	TLbsNetSessionIdInt sessionId;//Not used
	TTime targetTime;
	TTime actualTime;

	TInt error = iRefDataBus.GetPositionInfo(sessionId, iPositionInfo, targetTime, actualTime);
	
	iObs.DataUpdate(iPositionInfo, iStatus.Int(), error, actualTime);

	}
	
/**
from CActive
Cancels the request for position update notification
It will eventually cancel an outstanding subscription request for this property handle.

*/
void CRefDataBus::DoCancel()
	{
	iRefDataBus.CancelNotifyNetworkLocationUpdate();
	}
	
/**
from CActive
Simply fwds any errors to the observers
*/
TInt CRefDataBus::RunError(TInt aError)
	{
	return iObs.DataError(aError, iStatus.Int());
	}

