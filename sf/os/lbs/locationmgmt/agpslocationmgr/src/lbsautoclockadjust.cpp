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

// System
#include <e32std.h>
#include <ecom/ecom.h>
#include <centralrepository.h>

// Lbs
#include <lbs.h>
#include <lbs/lbsadmin.h>
#include <lbssatellite.h>
#include "lbsprocessuiddefs.h"
#include "lbsdevloggermacros.h"

// Component
#include "lbsautoclockadjust.h"
#include "lbsagpsmanagerconfigkeys.h"


CAutoClockAdjust* CAutoClockAdjust::NewL()
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::NewL()\n");
	CAutoClockAdjust* self = new(ELeave) CAutoClockAdjust();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void CAutoClockAdjust::ConstructL()
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::ConstructL()\n");
	// Config
	// Dynamic Set Clock module selection is not supported.
	iAdmin = CLbsAdmin::NewL(*this, KLbsSettingClockAdjustInterval | KLbsSettingClockAdjustThreshold |
									KLbsSettingClockAdjust);
	ReadAdminSettingsL();
	
	iCenRep = CRepository::NewL(KLbsGpsLocManagerUid);
	ReadLocalSettingsL();
	
	// Load a plug-in or the default impl
	LBSLOG(ELogP9, "->S CGpsSetClockBase::NewL() ClockModule\n");
	LBSLOG2(ELogP9, "  > TPositionModuleId iSetClockModuleId  = 0x%08X\n", iSetClockModuleId.iUid);
	iSetClockImpl = CGPSSetClockBase::NewL(iSetClockModuleId);
	}

CAutoClockAdjust::CAutoClockAdjust():
iClockAdjustSetting(CLbsAdmin::EClockAdjustUnknown),
iSetClockModuleId(KLbsSetClockNullModuleId),
iAdjustInterval(0),
iAdjustThreshold(0),
iLastAdjustment(0),
iAdmin(0),
iSetClockImpl(0)
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::CAutoClockAdjust()\n");
	}

CAutoClockAdjust::~CAutoClockAdjust()
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::~CAutoClockAdjust()\n");
	delete iAdmin;
	iAdmin = NULL;
	
	if (iCenRep != NULL)
		{
		WriteLastAutoAdjustTime();
		}
		
	delete iCenRep;
	iCenRep = NULL;
	
	LBSLOG(ELogP9, "->S CGpsSetClockBase::~CGpsSetClockBase() ClockModule\n");
	delete iSetClockImpl;
	iSetClockImpl = NULL;
	REComSession::FinalClose();	
	}
	

/** Adjusts system time if required. The decision whether the adjustment is needed or not
is based on the following criterias:
 - satellite time must be present in the location update
 - time threshold must be exceeded
 - time from a last adjustment is greater than a defined interval.
 
@param aStatus An error code.
@param TPositionSatelliteInfo Position and time information. 
							  If clock adjustment takes place the TPosition::iTime is 
							  re-set to the satellite time.
@see CLbsAdmin
@see TPositionSatelliteInfo
*/
void CAutoClockAdjust::LocationUpdate(TInt aStatus, TPositionSatelliteInfo& aPosInfo)
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::LocationUpdate()\n");
	TTimeIntervalMicroSeconds timeCorr;
	TTime sysTime;
	TInt err;
	
	// If adjustment on, no error, and satellite information present
	if ((iClockAdjustSetting == CLbsAdmin::EClockAdjustOn) && (aStatus == KErrNone) &&
	    ((aPosInfo.PositionClassType() & EPositionSatelliteInfoClass) == EPositionSatelliteInfoClass))
		{
		// Is is time do do another time adjustment?
		sysTime.UniversalTime();
		if (Abs(sysTime.MicroSecondsFrom(iLastAdjustment).Int64()) > (1000*iAdjustInterval))
			{
			const TPositionSatelliteInfo& satInfo = static_cast<const TPositionSatelliteInfo&>(aPosInfo);
			err = CalculateTimeCorrection(satInfo, timeCorr);
			if (err == KErrNone)
				{
				// Is threshold exceeded? 
				if (Abs(timeCorr.Int64()) > (1000*iAdjustThreshold))
					{
					sysTime.UniversalTime();
					sysTime += timeCorr;
					LBSLOG(ELogP9, "->S CGpsSetClockBase::SetUTCTime() ClockModule\n");
					LBSLOG5(ELogP9, "  > TTime sysTime  = %02d:%02d:%02d.%06d\n", sysTime.DateTime().Hour(), 
																				sysTime.DateTime().Minute(),
																				sysTime.DateTime().Second(),
																				sysTime.DateTime().MicroSecond());

					err = iSetClockImpl->SetUTCTime(sysTime);
					LBSLOG2(ELogP9, "  Return  = %d\n", err);

					if (err == KErrNone)
						{				
						// Sync the position time with the satellite time
						// to avoid re-adjusting the system time by the manual clock adjustment component.
						TPosition pos;
						aPosInfo.GetPosition(pos);
						pos.SetTime(aPosInfo.SatelliteTime());
						aPosInfo.SetPosition(pos);
						LBSLOG2(ELogP2, "ACTION: Clock Adjusted by %ld\n", timeCorr.Int64());
						}
					}
					
				if (err == KErrNone)
					{				
					// Remember the current time even if threshold not exceeded
					iLastAdjustment = sysTime;
					}
				else
					{
					LBSLOG_WARN2(ELogP3, "Clock Adjustment failed. Error: %d\n", err);
					}
				}
			}
		}
	}
	

/** Calculates a difference between the system UTC time and the GPS based UTC time.
Accuracy of the calculations is affected by:
   - time required to process the satellite information in the GPS HW
   - time required to transfer the information over a HW link from the GPS HW to a mobile phone
   - time required to receive the information by a Symbian OS drivers and decode by the AGPS
     integration module

@param aSatInfo A satellite information with the GPS based UTC time
@param aTimeCorr A time correction that must be applied to the System UTC time. Not valid if the method returns an error.
@return KErrNone if sucedded. 
        KErrNotSupported if the satellite (GPS based UTC time) or the position 
        reception time is not present (0).
@see TPositionSatelliteInfo
@internalComponent
*/
TInt CAutoClockAdjust::CalculateTimeCorrection(const TPositionSatelliteInfo &aSatInfo, TTimeIntervalMicroSeconds &aTimeCorr)
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::CalculateTimeCorrection()\n");
	// The System UTC time when the location has been received from a GPS
	TTime recTime;
	// GPS based UTC time (satellite time)
	TTime satTime;

	TPosition pos;
	TInt err = KErrNone;
		
	aSatInfo.GetPosition(pos);
	recTime = pos.Time();
	satTime = aSatInfo.SatelliteTime();
	
	if ((recTime == 0) || (satTime == 0))
		{
		err = KErrNotSupported;
		}
	else
		{
		aTimeCorr = satTime.MicroSecondsFrom(recTime);
		}
	
	return err;
	}

/** Read clock configuration.
@see CLbsAdmin*/
void CAutoClockAdjust::ReadAdminSettingsL()
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::ReadAdminSettingsL()\n");
	User::LeaveIfError(iAdmin->Get(KLbsSettingClockAdjust, 			iClockAdjustSetting));
	LBSLOG2(ELogP2, "   KLbsSettingClockAdjust is: %d\n", iClockAdjustSetting);
	User::LeaveIfError(iAdmin->Get(KLbsSettingClockAdjustThreshold, iAdjustThreshold));
	LBSLOG2(ELogP2, "   KLbsSettingClockAdjustThreshold is: %d\n", iAdjustThreshold);
	User::LeaveIfError(iAdmin->Get(KLbsSettingClockAdjustInterval, 	iAdjustInterval));
	LBSLOG2(ELogP2, "   KLbsSettingClockAdjustInterval is: %d\n", iAdjustInterval);
	User::LeaveIfError(iAdmin->Get(KLbsSettingSetClockModule, 		iSetClockModuleId));
	LBSLOG2(ELogP2, "   KLbsSettingSetClockModule is: 0x%X\n", iSetClockModuleId);
	}


/** Read local clock configuration. */
void CAutoClockAdjust::ReadLocalSettingsL()
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::ReadLocalSettingsL()\n");
	TInt   valHi;
	TInt   valLo;
	TInt   err;
	TInt64 val;
	
	err = iCenRep->Get(KLastAutoClockAdjustmentHi, valHi);
	ASSERT(err == KErrNone);
	if (err != KErrNone)
		{
		// Retry
		err = iCenRep->Reset();
		ASSERT(err == KErrNone);
		User::LeaveIfError(iCenRep->Get(KLastAutoClockAdjustmentHi, valHi));
		}
	User::LeaveIfError(iCenRep->Get(KLastAutoClockAdjustmentLo, valLo));
	
	val = MAKE_TINT64(valHi, valLo);
	iLastAdjustment = val;
	}

/** Write local clock configuration. 
On UREL the method does not return and error/leave even if writing the
time fails. */
void CAutoClockAdjust::WriteLastAutoAdjustTime()
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::WriteLastAutoAdjustTime()\n");
	TInt   valHi = I64HIGH(iLastAdjustment.Int64());
	TInt   valLo = I64LOW(iLastAdjustment.Int64());
	TInt   err;
	
	err = iCenRep->Set(KLastAutoClockAdjustmentHi, valHi);
	
	// For OOM tests also KErrNoMemory is fine
	ASSERT((err == KErrNone) || (err == KErrNoMemory));
	if (err != KErrNone)
		{
		err = iCenRep->Set(KLastAutoClockAdjustmentLo, valLo);
		ASSERT((err == KErrNone) || (err == KErrNoMemory));
		}
	}


/** Config settings observer call-back.
@see CLbsAdmin */
/*virtual*/ void CAutoClockAdjust::OnSettingUpdateEvent(TInt aError, const TLbsAdminSetting& aSetting)
	{
	LBSLOG(ELogP1, "CAutoClockAdjust::OnSettingUpdateEvent()\n");
	LBSLOG4(ELogP3, "NEW EVENT: Setting: 0x%08X%08X changed. err: %d", I64HIGH(aSetting), I64LOW(aSetting), aError); 
	
	if (aError == KErrNone)
		{
		// switch/case not used because of RVCT compiler problem (aSetting is TUint64)
		if (aSetting == KLbsSettingClockAdjust)
			{
			aError = iAdmin->Get(KLbsSettingClockAdjust, iClockAdjustSetting);
			LBSLOG2(ELogP2, "NEW EVENT: KLbsSettingClockAdjust changed to: %d\n", iClockAdjustSetting);
			}
		else if (aSetting == KLbsSettingClockAdjustThreshold)
			{
			aError = iAdmin->Get(KLbsSettingClockAdjustThreshold, iAdjustThreshold);
			LBSLOG2(ELogP2, "NEW EVENT: KLbsSettingClockAdjustThreshold changed to: %d\n", iAdjustThreshold);
			}
		else if (aSetting == KLbsSettingClockAdjustInterval)
			{
			aError = iAdmin->Get(KLbsSettingClockAdjustInterval, 	iAdjustInterval);
			LBSLOG2(ELogP2, "NEW EVENT: KLbsSettingClockAdjustInterval changed to: %d\n", iAdjustInterval);
			}
		else
			{
			ASSERT(EFalse);
			}
		}
		
		// Ignored on UREL
		ASSERT(aError == KErrNone);
	}
