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
 @internalTechnology
 @released
*/

#include "lbsdevloggermacros.h"
#include "measurementinfomonitor.h"


CLbsMeasurementInfoMonitor::CLbsMeasurementInfoMonitor(
		MLbsMeasurementInfoObserver& aObserver) :
	CActive(EPriorityHigh),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

CLbsMeasurementInfoMonitor::~CLbsMeasurementInfoMonitor()
	{
	Cancel();
	iLbsMeasurementInfoMonitor.Close();
	}

CLbsMeasurementInfoMonitor* CLbsMeasurementInfoMonitor::NewL(
		MLbsMeasurementInfoObserver& aObserver)
	{
	CLbsMeasurementInfoMonitor* self = new (ELeave) CLbsMeasurementInfoMonitor(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLbsMeasurementInfoMonitor::ConstructL()
	{
	iLbsMeasurementInfoMonitor.OpenL();
	}

void CLbsMeasurementInfoMonitor::StartMonitor()
	{
	if (!IsActive())
		{
		iLbsMeasurementInfoMonitor.NotifyGpsMeasurementUpdate(iStatus);
		SetActive();
		}
	}

void CLbsMeasurementInfoMonitor::StopMonitor()
	{
	Cancel();
	}

void CLbsMeasurementInfoMonitor::GetMeasurement(
		TInt& aReason, 
		TPositionGpsMeasurementInfo& aMeasurementResults,
		TTime& aActualTime)
	{
	aReason = iLbsMeasurementInfoMonitor.GetGpsMeasurementInfo(
				&aMeasurementResults, 
				sizeof(aMeasurementResults),
				aActualTime);
	}

void CLbsMeasurementInfoMonitor::RunL()
	{
	// Immediately re-subscribe
	StartMonitor();
	
	// Get the latest update
	TPositionGpsMeasurementInfo measurementInfo;
	TInt measurementInfoErr;
	TTime updateTime;
	GetMeasurement(measurementInfoErr, measurementInfo, updateTime);
	
	// Notify the observer of the update
	iObserver.OnMeasurementInfoUpdate(
			measurementInfoErr,
			measurementInfo,
			updateTime);
	}

TInt CLbsMeasurementInfoMonitor::RunError(TInt aError)
	{
	LBSLOG_ERR2(ELogP3, "CLbsMeasurementInfoMonitor::RunError : %d", aError);
	return KErrNone;
	}

void CLbsMeasurementInfoMonitor::DoCancel()
	{
	iLbsMeasurementInfoMonitor.CancelNotifyGpsMeasurementUpdate();
	}

