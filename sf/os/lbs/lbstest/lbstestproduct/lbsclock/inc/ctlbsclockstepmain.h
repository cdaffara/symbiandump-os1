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
// @file ctlbsclockstepmain.h
// This contains the header file for Clock Tests
// 
//

#ifndef __CT_LBS_CLOCK_MAIN_STEP_H__
#define __CT_LBS_CLOCK_MAIN_STEP_H__

// EPOC includes.
#include <e32base.h>

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsadmin.h>

// LBS test includes.
#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbstimerutils.h>
#include "ctlbsclockstep.h"
#include "ctlbsclockserver.h"
#include "ctlbsclockplugindata.h"


// Literals used
_LIT(KLbsClockStep_Main, "LbsClockStep_Main");

/** Callback flags held as bits in a 32 bit word.
*/
typedef TUint32 TLbsClockCallbackFlags;

const TLbsClockCallbackFlags KLbsClockCallback_Got_AdjustClock_Stage1	= 0x0001;
const TLbsClockCallbackFlags KLbsClockCallback_Got_AdjustClock_Stage2	= 0x0002;
const TLbsClockCallbackFlags KLbsClockCallback_Got_TimerNotification	= 0x0004;
const TLbsClockCallbackFlags KLbsClockCallback_Got_AdminNotification	= 0x0008;
const TLbsClockCallbackFlags KLbsClockCallback_Got_ClockPluginInUse		= 0x0010; /** Clock Plugin was used to carry out clock adjust. */

class CT_LbsClockStep_Main :	public CT_LbsClockStep,
								public CActive,
								public MLbsAdminObserver,
								public MT_LbsTimerUtilsObserver,
								public MT_LbsClockPluginDataBusObserver
	{
public:
	~CT_LbsClockStep_Main();

	static CT_LbsClockStep_Main* New(CT_LbsClockServer& aParent);
	virtual TVerdict doTestStepL();


protected:
	//Function from MT_LbsDoPosUpdateObserver - Module out data bus updated.
//	void HandleDataBusUpdateL(const TModuleDataOut& aModuleDataOut);

	//	Functions from CActive.
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

	// Function from MLbsAdminObserver.
	virtual void OnSettingUpdateEvent(TInt aError, const TLbsAdminSetting& aSetting);

	// Function from MT_LbsTimerUtilsObserver.
	virtual void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);

	// Function from MT_lbsClockpluginDataBusObserver.
	virtual void HandleDataBusUpdateL(const TClockPluginDataOut& aClockPluginDataOut);
	
private:
	CT_LbsClockStep_Main(CT_LbsClockServer& aParent);
	void ConstructL();

	void SetCallbackFlag(TLbsClockCallbackFlags aCallbackFlag);

	void SetTimeOffsetL();
	void SetModuleTimeoutL(TTimeIntervalMicroSeconds aTimeout);
	void SetAutoAdjustOnL();

	void DetermineModuleTimeToFirstFixL();

	void CheckForTestHalt();

	void StartAutoAdjustTesting();

	void AdjustClock(TBool aUseTimeout, TTimeIntervalMicroSeconds aTimeoutValue);

	TBool CompareTimes(T_LbsUtils::TComparisonAccuracyType aCmpAccuracy);

	void SetClientTimeout();
	TBool ValidateTimeout(const TTimeIntervalMicroSeconds aTimeoutValue);

	enum 	{
				EWaitingAdminNotify,
				EWaiting,
				ESoakRefPos,
				EClockPluginComplete,
				EClockPluginTimeout,
				EClockAdjustPending_Stage1,
				EClockAdjustPending_Stage2,
				EDone
			};

	/** State machine variable for the test step.
	*/
	TInt iState;


	/** Used to keep a record off all the callback events.
	*/
	TLbsClockCallbackFlags iCallbackFlags;
	

	/** Expected callback flags at test end.
	*/
	TLbsClockCallbackFlags iFlagsToHaltOn;


	
	/** Current test case being executed.
	*/
	TInt iTestCaseId;


	/** Plugin data bus monitor, to allow the plugin to inform the test harness it was called.
	*/
	CT_LbsClockPluginDataBusMonitor* iPluginDataBusMonitor;

	/** The AGPS mode used by the AGPS module. Set to ETrue for prefered terminal, EFalse for
		all others.
	*/
	TBool iUseGpsPreferTerminalBased;

	/** Self locate server - used to invoke the automatic clock adjust.
	*/
	RPositionServer iServer;
	RPositioner iPositioner;


	/** Admin data base.
	*/
	CLbsAdmin* iAdmin;
		

	/** Network reference position.
	
		Used for auto adjust tests when AGPS module is request assistance data, which will
		result in an additional position be returned when calling NoiftyPositionUpdate.
	*/
	TPositionInfo iRefPosInfo;


	CT_LbsTimerUtils* iVerifyTimer;
	TTime iVerifyTime;

	/** The offset to alter the current time by.
	*/
	TTimeIntervalMicroSeconds iTimeStampOffset;

	/** The current system time when the client adjust clock was called.
		Used to allow timeout verification.
	*/	
	TTime iAdjustClockCallTime;
	
	/** The client timeout value to used when calling adjust clock.
	*/
	TTimeIntervalMicroSeconds iClientTimeout;
	
	/** Time to first fix of the A-GPS module.
	*/
	TTimeIntervalMicroSeconds iGpsTimeToFirstFix;
	};

#endif //__CT_LBS_CLOCK_MAIN_STEP_H__
