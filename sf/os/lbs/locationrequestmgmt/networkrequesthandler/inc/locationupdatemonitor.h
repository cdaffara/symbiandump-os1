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

#ifndef LOCATIONUPDATEMONITOR_H_
#define LOCATIONUPDATEMONITOR_H_

#include <e32std.h>
#include <lbs/lbsextendedsatellite.h>
#include "LbsInternalInterface.h"

/**
*/
class MLbsLocUpdateObserver
	{
public:
	virtual void OnPositionUpdate(
			TBool aConflictControl,
			TInt aReason,
			const TPositionExtendedSatelliteInfo& aPosInfo,
			const TTime& aActualTime) = 0;	
	};

/**
*/
class CLbsLocUpdateMonitor : public CActive
	{
public:	
	static CLbsLocUpdateMonitor* NewL(
			MLbsLocUpdateObserver& aObserver);
	~CLbsLocUpdateMonitor();
	 
public:	
	void StartMonitor();
	void StopMonitor();
	
	void SendLocationRequest(
			TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice,
			const TLbsNetPosRequestMethodInt& aMethod,
			const TLbsLocRequestQualityInt& aQuality);
	void SendCancelRequest(
			TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice);
	void SendPowerAdviceRequest(
			TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice);
	
	void GetPosition(
			TBool& aConflictControl,
			TInt& aReason,
			TPositionExtendedSatelliteInfo& aPosInfo,
			TTime& aActualTime);

protected:
	CLbsLocUpdateMonitor();
	CLbsLocUpdateMonitor(MLbsLocUpdateObserver& aObserver);
	void ConstructL();
	
protected: // From CActive
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();	
	
private:
	MLbsLocUpdateObserver& iObserver;
	RLbsPositionUpdates iLbsPositionUpdateMonitor;
	RLbsPositionUpdateRequests iLbsPositionUpdateRequestor;
	};

#endif /*LOCATIONUPDATEMONITOR_H_*/
