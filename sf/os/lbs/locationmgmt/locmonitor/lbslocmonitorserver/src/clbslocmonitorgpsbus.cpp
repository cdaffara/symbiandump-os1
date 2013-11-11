/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "clbslocmonitorgpsbus.h"
#include "lbsdevloggermacros.h"
#include "lbsprocessuiddefs.h"

/**
Create a location data bus

@return The pointer to the location data bus
@param The observer that wants to be sent the position.
*/
CLbsLocMonitorGpsBus* CLbsLocMonitorGpsBus::NewL(MLbsLocMonitorGpsBusObserver& aObserver)	
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorGpsBus::NewL");
	CLbsLocMonitorGpsBus* self = new (ELeave) CLbsLocMonitorGpsBus(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	} 
	
CLbsLocMonitorGpsBus::CLbsLocMonitorGpsBus(MLbsLocMonitorGpsBusObserver& aObserver):
						 CActive(CActive::EPriorityStandard),
						 iObserver(aObserver)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorGpsBus::CLbsLocMonitorGpsBus");
	CActiveScheduler::Add(this);
	}

/**
 Second phase constructor. Open the GPS updates bus
 and request notifications of updates.
*/
void CLbsLocMonitorGpsBus::ConstructL()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorGpsBus::ConstructL");
	iGpsPositionBus.OpenL(KLbsGpsLocManagerUid);
	iGpsPositionBus.NotifyPositionUpdate(iStatus);
	SetActive();
	}

/**
Cancel any outstanding request to the CActive object.
*/
CLbsLocMonitorGpsBus::~CLbsLocMonitorGpsBus()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorGpsBus::~CLbsLocMonitorGpsBus");
	Cancel();
	iGpsPositionBus.Close();
	}


/**
From CActive

*/
void CLbsLocMonitorGpsBus::RunL()
	{
	LBSLOG(ELogP1, "LocMonitor - CLbsLocMonitorGpsBus::RunL");
	TInt status = iStatus.Int();
	
	// re-subscribe for update notifications
	iGpsPositionBus.NotifyPositionUpdate(iStatus);
	SetActive();
	
	if(KErrNone == status)
		{
		TPositionInfo positionInfo;
		TBool whatever1; //not used
		TTime whatever2, whatever3; //not used

		// Take position from bus and send to observer
		if (KErrNone == iGpsPositionBus.GetPositionInfo(whatever1, &positionInfo,
														positionInfo.PositionClassSize(),
														whatever2, whatever3))
			{
			TPosition position;
			positionInfo.GetPosition(position);
			iObserver.GpsPositionAvailableL(position);
			}
		else
			{
			// nothing to do	
			}
		}
	else
		{
		// nothing to do	
		}
	}
	
/**
from CActive
Cancels the request for position update notification.

@internalComponent
@released
*/
void CLbsLocMonitorGpsBus::DoCancel()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorGpsBus::DoCancel");
	iGpsPositionBus.CancelNotifyPositionUpdate();
	}

/**
from CActive

@internalComponent
@released
*/
TInt CLbsLocMonitorGpsBus::RunError(TInt aError)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorGpsBus::RunError");
	return aError;
	}


