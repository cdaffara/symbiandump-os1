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

#ifndef MEASUREMENTINFOMONITOR_H_
#define MEASUREMENTINFOMONITOR_H_

#include <e32std.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsadmin.h>
#include "LbsInternalInterface.h"

/**
*/
class MLbsMeasurementInfoObserver
	{
public:
	virtual void OnMeasurementInfoUpdate(
			TInt aReason, 
			const TPositionGpsMeasurementInfo& aMeasurementResults,
			const TTime& aActualTime) = 0;	
	};

/**
*/
class CLbsMeasurementInfoMonitor : public CActive
	{
public:	
	static CLbsMeasurementInfoMonitor* NewL(
			MLbsMeasurementInfoObserver& aObserver);
	~CLbsMeasurementInfoMonitor();
	 
public:
	void StartMonitor();
	void StopMonitor();
	
	void GetMeasurement(
			TInt& aReason, 
			TPositionGpsMeasurementInfo& aMeasurementResults,
			TTime& aActualTime);
	
protected:
	CLbsMeasurementInfoMonitor();
	CLbsMeasurementInfoMonitor(MLbsMeasurementInfoObserver& aObserver);
	void ConstructL();
	
protected: // From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();	
	
private:
	MLbsMeasurementInfoObserver& iObserver;
	RLbsGpsMeasurementUpdates iLbsMeasurementInfoMonitor;
	};

#endif /*MEASUREMENTINFOMONITOR_H_*/
