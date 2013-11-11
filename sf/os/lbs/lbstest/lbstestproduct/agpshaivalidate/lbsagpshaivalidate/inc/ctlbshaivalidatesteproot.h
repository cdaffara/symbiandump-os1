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
// @file ctlbshaivalidatesteproot.h
// 
//

#ifndef __CTLBSHAIVALIDATESTEPROOT_H__
#define __CTLBSHAIVALIDATESTEPROOT_H__

// LBS includes.
#include <lbs.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbscommon.h>
#include <lbssatellite.h>


// LBS test includes.
#include <lbs/test/ctlbstimerutils.h>
#include "ctlbsagpshaivalidatestepbase.h"
#include "ctlbsagpshaiserverroot.h"
#include "ctlbs.h"


// Literals used
_LIT(KLbsAGPSHAIValidateStep, "LbsAGPSHAIValidateStep_Main");
_LIT(KTestCaseId, "tc_id");
_LIT(KAccuracyHoriz, "accuracy_horiz");
_LIT(KAccuracyVert, "accuracy_vert");
_LIT(KTgtTimeOffsetFromNow, "tgttime_offset");
_LIT(KMaxFixTime, "maxfixtime");

enum TTimerId
	{
	KStateTimerId = 1,
	KShortlyAfterTargetTimeTimerId,
	KStartSecondRequestTimerId,
	KCancelTimerId,
	KUpdatesStoppedTimerId,
	KInactiveTimerId,
	KStandbyTimerId,
	KFirstDeviceStatusTimerId,
	KFirstQualityStatusTimerId,
	KNormalQualityTimerId,
	KShutdownRequestTimerId,
	
	// add new timers here
	
	KLastRootTimerId,	// for derived steps to use
	};
	
// time we wait for after target time before we warn that an update has not been produced
const TTimeIntervalMicroSeconds32 KFirstUpdateDelay   = 3*1000000; // 3s
// time after loading by which we expect module to have reported device status
const TTimeIntervalMicroSeconds32 KExpectFirstDeviceStatusTime = 10000000;	// 10 seconds


class CT_LbsAGPSHAIValidateStepRoot : 	public CT_LbsAgpsHAIValidateStepBase, 
										public MT_LbsTimerUtilsObserver, 
										public MLbsLocationSourceGpsObserver
	{
public:
	~CT_LbsAGPSHAIValidateStepRoot();

	// from MT_LbsTimerUtilsObserver:
	void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);
		
	// from MLbsLocationSourceGpsObserver:
	TVersion Version();
	virtual void UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosition[], TInt aNumItems, const TTime& aTargetTime);
	void UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus);
	void UpdateDataQualityStatus(TPositionModuleStatus::TDataQualityStatus aDataQuality);
	// these MUST be virtual so the dervied class can overload
	virtual TInt GetAssistanceDataItem(TLbsAssistanceDataItem aItem, RDataReaderRootBase& aDataRoot, TTime& aTimeStamp);
	virtual TInt GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp);
	virtual void RequestAssistanceData(TLbsAsistanceDataGroup aDataItemMask);
	void Shutdown();


protected:
	// current state. 
	enum TState
		{
		EStateAwaitingTgtTime,				// request has been issued, but target time not yet reached
		EStateAwaitingUpdates,				// targettime reached, accurate position not yet received & maxfix timer not fired
		EStateGotAccuratePos,				// position of requested accuracy received (max fix timer not fired)
		EStateAwaitingStrayUpdates,			// max fix timer fired and we waiting to make sure no more ('stray') updates come in
		EStateAwaitingShutdownRequest,		// we've asked the module to close and are waiting for it to comply
		EStateDone							// finished
		};
		
	CT_LbsAGPSHAIValidateStepRoot(CT_LbsAGPSHAIServerRoot& aParent);
	void ConstructL();

	void LoadHAIModuleL();
	void UnLoadHAIModule();
	// set/getters:
	CLbsLocationSourceGpsBase*	HAIModule();
	TLbsLocRequestQuality& LastRequestedQuality();
	TLbsLocRequestQuality LastRequestedQuality() const;
	TPositionModuleStatus::TDeviceStatus ExpectedDeviceStatus() const;
	TPositionModuleStatus::TDeviceStatus& ExpectedDeviceStatus();
	TPositionModuleStatus::TDeviceStatus AltExpectedDeviceStatus() const;
	TPositionModuleStatus::TDeviceStatus& AltExpectedDeviceStatus();	
	TBool& IgnoreShutdownRequest();
	TBool IgnoreShutdownRequest() const;
	// get/set the state
	CT_LbsAGPSHAIValidateStepRoot::TState& State();
	CT_LbsAGPSHAIValidateStepRoot::TState State() const;
	TPositionModuleStatus& LastReportedStatus();	
	TPositionModuleStatus LastReportedStatus() const;
	// get/set whether to use stray updates timer:
	TBool& StrayUpdatesTimer();
	TBool StrayUpdatesTimer() const;
	// get/set the error we should return in response to a request for assistance data from module:
	TInt& AssistanceDataError();
	TInt AssistanceDataError() const;
	// get set whether module should be currently tracking:
	TBool& Tracking();
	TBool Tracking() const;
	

	// test case methods.
	// protected cos I want to call these from derived classes
	void DoLoadUnLoadTestCase();
	void DoImmediateRequestTestCase();
	void DoSecondLocationRequestTestCase();
	void DoLowAccuracyLocationRequestTestCase();
	void DoTrackingRequestTestCase();
	void DoImmediateRequestWithMaxFixTimeTestCase();
	void DoFarOffLocationRequestTestCase();
	void DoImmediateRequestWithCancelTestCase();
	void DoHAISettlesToInActiveTestCase();
	void DoHAIToggleActiveStatusTestCase();
	void DoHAIPowerDropTestCase();
	void DoHAIDataQualityStatusTestCase();
	void DoHAIAdviseCloseTestCase();
	
	// now moved the protected so they can be called from derived classes
	// verify that results (flags set / last location returned) are as expected
	void VerifyStandardResults();
	// logs the quality (lat/long/accuracy) of last request
	void LogLastRequestedPositionDetails();
	void StartTgtTimeStateTimer();
	void StartMaxFixStateTimer();
	void StartFirstDeviceStatusTimer();
	void StartFirstRequest();
	void StartFirstQualityStatusTimer();
			
	void CreateAllTimersL();
	
	void CancelAllTimers();
	
	void DeleteAllTimers();
	
	void ResetRequestFlags();

	void StartAnotherRequest();
	
	void StartShutdownRequestTimer();
			
	void StartStrayUpdsStateTimer();
			
	void StartInactiveTimer();
	
	void StartNormalQualityTimer();
	
	void StartStandbyTimer();
		
	void StartImmediateDefaultRequest()	;
	
	void FinishTest();
		
	// logs position (lat, long, alt) to TEF log
	void LogPosition(TPositionInfo aPos);

protected:
		
	CT_LbsAGPSHAIServerRoot& iParent;
	// this needs to be protected not private
	TInt iTestCaseId;
	TBool iStartAnotherRequestFlag;

private:

	CLbsLocationSourceGpsBase*	iHAIModule;	
		
	// TO DO - how big a deal is it to use all these different timers? I could be much more RAM efficient if necessary by re-using some
	// timer that causes us to move from one test state to another
	CT_LbsTimerUtils*	iStateTimer;
				
	// timer that indicates we should have reached 'inactive' device status
	CT_LbsTimerUtils*	iInactiveTimer;

	// timer that indicates we should have reached 'normal' data quality status (under good conditions)
	CT_LbsTimerUtils*	iNormalQualityTimer;
	
	// timer that indicates we should have reached 'Standby' device status
	CT_LbsTimerUtils*	iStandbyTimer;
	
	// timer that indicates we have reached 'shortly after' tgt time:
	CT_LbsTimerUtils*	iAfterTgtTimeTimer;
	
	// timer that indicates we should cancel the lat request:
	CT_LbsTimerUtils*	iCancelTimer;
	
	// timer that indicates we should've stopped receiving updates
	CT_LbsTimerUtils* 	iUpdatesStoppedTimer;
	
	// timer to indicate we should start a new request (implicitly cancelling another)
	CT_LbsTimerUtils*	iNewRequestTimer;
			
		// timer that indicates we should have received first device status report
	CT_LbsTimerUtils*	iFirstDeviceStatusTimer;
	
	// timer that indicates we should have received first quality status report
	CT_LbsTimerUtils*	iFirstQualityStatusTimer;	

 	// timer to indicate that we should have received a shutdown request (after advisepowermode(close))
 	CT_LbsTimerUtils*	iShutdownRequestTimer;						
					
	TState iState;

	// Tgt time passed to HAI module
	TTime iRequestedTgtTime;
	
	// the last data quality (normal/partial/loss) and device status (active/inactive/standby etc) reported by the HAI module 
	TPositionModuleStatus iLastReportedStatus;
			
	// Tgt time passed to HAI module in last-but-one request
	TTime iPreviousRequestedTgtTime;	
	
	// Last requested quality
	TLbsLocRequestQuality iLastRequestedQuality;	
		
	// number of requests still to be started	
	TInt iNumAdditionalRequests;
		
	// last position returned from HAI module:
	TPositionSatelliteInfo	iLastReportedSatPosition;
		
	// ************* Test Case progress flags: *************
		
	// whether the inactivity timer was started and fired
	TBool iInactivityTimerExpired;
	
	// whether we've received a request from the HAI module to shutdown after AdvisePowerMode(close)
	TBool iShutdownRequestReceivedAndIgnored;		
	
	TBool iTracking;
				
	// ************* End Test Case progress flags *************		
	
	
	// ************* Request status flags: note: if you add a flag here, reset it in ResetRequestFlags() *************
	
	// whether we have received any position for current request
	TBool iGotPos;
			
	// whether we have received a location with the requested accuracy
	TBool iGotRequestedAccuracy;

	// whether we have received a location with the minimum (as per module info) accuracy
	TBool iGotExpectedAccuracy;
			
	// whether normal data quality was reported during latest request
	TBool iDataQualityNormalReported;
	
	// whether partial data quality was reported during latest request
	TBool iDataQualityPartialReported;
	
	// whether device error was reported during last location request
	TBool iDeviceStatusErrorReported;
		
	// whether the last request has been cancelled long enough ago that updates should have stopped:
	TBool iExpectUpdatesStopped;
	
	// set when call to cancel previous request is made (to suppress warnings about reported data quality):
	TBool iTriedToCancelLastRequest;
	
	// ************* End Request status flags *************
		
	// ************* Test Case setup: *************
	
	// whether to finish the test when we get the 'inactive' timer
	TBool iFinishOnInactiveExpected;
		
	// whether we should wait for the device to settle to 'Inactive' after getting an update of the required accuracy
	TBool iStartInactiveTimerOnGotAccPos;
			
	// whether to use the same target time as the last request:
	TBool iSameTgtTimeAsPrevious;
	
	// whether we ignore the HAI module when it next asks to be shutdown:
	TBool	iIgnoreShutdownRequest;
	
	// whether the test case requires a stray updates timer:
	TBool	iStrayUpdatesTimer;	
	
	// whether we should start the next request (implicit cancel?) when we reach the maxtime for the previous request
	TBool iStartNewRequestOnPreviousFinished;

	// whether we should start a new 'far off' request and a 'standby' timer after getting an update of the required accuracy
	TBool iStartDistantRequestAndStandbyTimerOnGotAccPos;

	// whether the standby timer was started and fired
	TBool iStandbyTimerExpired;

	// the device status update that we expect to receive (as a result of AdvisePowerMode())	
	TPositionModuleStatus::TDeviceStatus iExpectedDeviceStatus;
	
	// device status to expect if above not supported
	TPositionModuleStatus::TDeviceStatus iAltExpectedDeviceStatus;
	
	// The error we returned to the GPS module in AssistanceDataEvent() when it asks for assistance data
	TInt iAssistanceDataError;

	// ************* End Test Case setup: *************
	};

#endif //__CT_LBS_DATA_SRC_STEP_AGPS_H__
