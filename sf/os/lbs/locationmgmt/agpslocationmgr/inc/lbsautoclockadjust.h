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
//

/**
 @file
 @internalTechnology
*/
#ifndef LBS_AUTO_CLOCK_ADJUST_H
#define LBS_AUTO_CLOCK_ADJUST_H

// System
#include <e32std.h>
#include <centralrepository.h>

// Lbs
#include <lbs/gpssetclockbase.h>
#include <lbssatellite.h>
#include <lbs/lbsadmin.h>
#include <lbs.h>


class CAutoClockAdjust : public CBase, public MLbsAdminObserver
	{
public:
	static CAutoClockAdjust* NewL();
	~CAutoClockAdjust();
	
	void LocationUpdate(TInt aStatus, TPositionSatelliteInfo& aPosInfo);
	
protected:
	CAutoClockAdjust();
	void ConstructL();
	
	void ReadAdminSettingsL();
	void ReadLocalSettingsL();
	void WriteLastAutoAdjustTime();
	TInt CalculateTimeCorrection(const TPositionSatelliteInfo &aSatInfo, TTimeIntervalMicroSeconds &aTimeCorr);

private:
	// From MLbsAdminApiObserver
	virtual void OnSettingUpdateEvent(TInt aError, const TLbsAdminSetting& aSetting);
	
	CLbsAdmin::TClockAdjust			iClockAdjustSetting;
	TLbsSetClockModuleId 			iSetClockModuleId;
	TLbsTimeIntervalMilliSeconds		iAdjustInterval;
	TLbsTimeIntervalMilliSeconds		iAdjustThreshold;
	
	TTime							iLastAdjustment;

	CLbsAdmin*			iAdmin;
	CRepository* 		iCenRep;
	CGPSSetClockBase*	iSetClockImpl;
	};

#endif // LBS_AUTO_CLOCK_ADJUST_H
