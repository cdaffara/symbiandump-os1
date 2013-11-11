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
// @file ctlbshaivalidatesteproot.cpp
// This is the class implementation for the Module Information Tests
// 
//

// EPOC includes.
#include <e32property.h>
#include <ecom/ecom.h> 

// LBS includes. 
#include <lbscommon.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbsmoduleinfo.h>

// LBS test includes.
#include "ctlbshaivalidatesteproot.h"
#include <lbs/test/ctlbstimerutils.h>
#include "lbstestloggermacros.h"
#include "ctlbs.h"
#include "lbstestutilities.h" 

// 'short time' to wait after expected time for location update to allow for processing time:
const TTimeIntervalMicroSeconds32 KProcessTimeMargin = 500000;	// half second

// time we wait after valid time window to verify that no more updates come 
const TTimeIntervalMicroSeconds32 KStrayUpdatesTime   = 30*1000000; // 30s

// time after loading by which we expect module to have reported quality status
const TTimeIntervalMicroSeconds32 KExpectFirstQualityStatusTime = KExpectFirstDeviceStatusTime;


// config settings we read
_LIT(KDelayBetweenRequests, "delay_between_requests");	
_LIT(KNumAdditionalRequests, "num_additional_requests");
_LIT(KUpdsCancelledByTime, "updates_cancelledby_time");	
_LIT(KCancelTime, "cancel_time");
_LIT(KInactiveTime, "inactive_time");
_LIT(KStandbyTime, "standby_time");
_LIT(KTgtTimeOffsetFromNow_2, "tgttime_offset_2");
_LIT(KMaxFixTime_2, "maxfixtime_2");
_LIT(KAccuracyHoriz_2, "accuracy_horiz_2");
_LIT(KAccuracyVert_2, "accuracy_vert_2");
_LIT(KShutdownRequestTime, "shutdownrequest_time");


/**
 * Destructor
 */
CT_LbsAGPSHAIValidateStepRoot::~CT_LbsAGPSHAIValidateStepRoot()
	{
	// Make sure we didn't leave any timers running:
	CancelAllTimers();
	
	// In case there was a leave before we 'unload' the modules:
	UnLoadHAIModule();

	DeleteAllTimers();
	}
	

void CT_LbsAGPSHAIValidateStepRoot::CreateAllTimersL()
	{
	// Create timers. Note that we are doing this here for simplicity/clarity even tho' they may not all be used
	iStateTimer = CT_LbsTimerUtils::NewL(this, KStateTimerId);	
	iAfterTgtTimeTimer = CT_LbsTimerUtils::NewL(this, KShortlyAfterTargetTimeTimerId);
	iNewRequestTimer = CT_LbsTimerUtils::NewL(this, KStartSecondRequestTimerId);
	iInactiveTimer = CT_LbsTimerUtils::NewL(this, KInactiveTimerId);
	iNormalQualityTimer = CT_LbsTimerUtils::NewL(this, KNormalQualityTimerId);
	iCancelTimer = CT_LbsTimerUtils::NewL(this, KCancelTimerId);	
	iStandbyTimer = CT_LbsTimerUtils::NewL(this, KStandbyTimerId);	
	iUpdatesStoppedTimer = CT_LbsTimerUtils::NewL(this, KUpdatesStoppedTimerId);
	iFirstDeviceStatusTimer = CT_LbsTimerUtils::NewL(this, KFirstDeviceStatusTimerId);
	iFirstQualityStatusTimer = CT_LbsTimerUtils::NewL(this, KFirstQualityStatusTimerId);	
	iShutdownRequestTimer = CT_LbsTimerUtils::NewL(this, KShutdownRequestTimerId);			
	}


void CT_LbsAGPSHAIValidateStepRoot::DeleteAllTimers()
	{
	delete iStateTimer;
	delete iAfterTgtTimeTimer;
	delete iInactiveTimer;
	delete iNormalQualityTimer;
	delete iStandbyTimer;
	delete iNewRequestTimer;
	delete iCancelTimer;
	delete iUpdatesStoppedTimer;
	delete iFirstDeviceStatusTimer;
	delete iFirstQualityStatusTimer;
	delete iShutdownRequestTimer;
	}
/*
	Cancel all timers. 
*/
void CT_LbsAGPSHAIValidateStepRoot::CancelAllTimers()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::CancelAllTimers()");	
	if (iStateTimer && iStateTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iStateTimer is still running. Cancelling timer.");	
		iStateTimer->CancelTimer();
		}
	if (iAfterTgtTimeTimer && iAfterTgtTimeTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iAfterTgtTimeTimer is still running. Cancelling timer.");			
		iAfterTgtTimeTimer->CancelTimer();
		}
	if (iInactiveTimer && iInactiveTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iInactiveTimer is still running. Cancelling timer.");	
		iInactiveTimer->CancelTimer();
		}
	if(iNormalQualityTimer && iNormalQualityTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iNormalQualityTimer is still running. Cancelling timer.");
		iNormalQualityTimer->CancelTimer();
		}
	if(iStandbyTimer && iStandbyTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iStandbyTimer is still running. Cancelling timer.");
		iStandbyTimer->CancelTimer();
		}
	if (iNewRequestTimer && iNewRequestTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iNewRequestTimer is still running. Cancelling timer.");
		iNewRequestTimer->CancelTimer();
		}	
	if(iCancelTimer && iCancelTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iCancelTimer is still running. Cancelling timer.");
		iCancelTimer->CancelTimer();
		}
	if(iUpdatesStoppedTimer && iUpdatesStoppedTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iUpdatesStoppedTimer is still running. Cancelling timer.");
		iUpdatesStoppedTimer->CancelTimer();
		}
	if(iFirstDeviceStatusTimer && iFirstDeviceStatusTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iFirstDeviceStatusTimer is still running. Cancelling timer.");
		iFirstDeviceStatusTimer->CancelTimer();
		}
	if(iFirstQualityStatusTimer && iFirstQualityStatusTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iFirstQualityStatusTimerr is still running. Cancelling timer.");
		iFirstQualityStatusTimer->CancelTimer();
		}	
	if(iShutdownRequestTimer && iShutdownRequestTimer->IsActive())
		{
		TESTLOG(ELogP3, "NOTE: iShutdownRequestTimer is still running. Cancelling timer.");
		iShutdownRequestTimer->CancelTimer();
		}	
		
	
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::CancelAllTimers()");		
	}
	
	

/**
 * Constructor
 */
CT_LbsAGPSHAIValidateStepRoot::CT_LbsAGPSHAIValidateStepRoot(CT_LbsAGPSHAIServerRoot& aParent) : iParent(aParent), iStartAnotherRequestFlag(EFalse)
	{	
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::CT_LbsAGPSHAIValidateStepRoot()");
	SetTestStepName(KLbsAGPSHAIValidateStep);
	
	// install scheduler if there isn't one already installed
	if(!CActiveScheduler::Current())
		{
		CActiveScheduler::Install(iParent.iScheduler);	
		}
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::CT_LbsAGPSHAIValidateStepRoot()");		
	}


	
/**
2nd phase Constructor that may leave
*/
void CT_LbsAGPSHAIValidateStepRoot::ConstructL()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::ConstructL()");	
	CreateAllTimersL();
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::ConstructL()");	
	}
	
/*
	Load the HAI Module (ECOM plugin) and tell it to use autonomous mode
*/
void CT_LbsAGPSHAIValidateStepRoot::LoadHAIModuleL()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::LoadHAIModuleL()");	
	TPositionModuleId 	id = {KLbsModuleManagerPairUidValue};
	
   	TUid dataSourceId;
	
	RLbsModuleInfo modInfoApi;
	CleanupClosePushL(modInfoApi);
	modInfoApi.OpenL();
	
	TInt err;
	err = modInfoApi.GetDataSourceId(id, dataSourceId);
	if(KErrNone != err)
		{
		TESTLOG2(ELogP3, "FAILED: Couldn't get datasource id found for HAI module with id 0x%x\n", id);
		ERR_PRINTF2(_L("<font><b>FAILED: Couldn't get datasource id found for HAI module with id 0x%x.</b></font>\n"), id);
		SetTestStepResult(EFail);
		}
	else
		{
		
		TESTLOG2(ELogP2, "Using HAI module with DataSourceId = %d\n", dataSourceId);
		 
		TRAPD(err, iHAIModule = CLbsLocationSourceGpsBase::NewL(*this, dataSourceId));
		if(KErrNone != err)
			{
			TESTLOG2(ELogP3, "FAILED: Could not Create AGPS Module Implementation. Failed with error %d\n", err);
			ERR_PRINTF2(_L("<font><b>FAILED: Could not Create AGPS Module Implementation. Got error %d.</font></b>\n"), err);
			SetTestStepResult(EFail);
			}
		else
			{
			TLbsGpsOptions gpsOptions;
			
			TESTLOG(ELogP2, "Module Loaded\n");
			
			gpsOptions.SetGpsMode(CLbsAdmin::EGpsAutonomous);
			// Set the gps mode to autonomous:
			iHAIModule->SetGpsOptions(gpsOptions);	
			
			TESTLOG(ELogP2, "Module gps option set to autonomous\n");
			}
			
		}
		
	CleanupStack::PopAndDestroy(&modInfoApi);
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::LoadHAIModuleL()");	
	}
	
	
void CT_LbsAGPSHAIValidateStepRoot::UnLoadHAIModule()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::UnloadHAIModuleL()");		
	delete iHAIModule;
	REComSession::FinalClose();
	iHAIModule = NULL;
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::UnloadHAIModuleL()");	
	}


CLbsLocationSourceGpsBase* CT_LbsAGPSHAIValidateStepRoot::HAIModule()
	{
	return iHAIModule;
	}
	
/*
	Verify results that should be true for every test
		- Some update (potentially empty/partial depending on data quality reported) was received before maxfix time. 
		- The update returned meets the module's expected accuracy (reported in module info) if normal data quality was reported at some point during the valid update window.

*/
void CT_LbsAGPSHAIValidateStepRoot::VerifyStandardResults()	
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::VerifyStandardResults()");	
	// verify results
	if(!iGotPos)
		{
		SetTestStepResult(EFail);
		TESTLOG(ELogP3, "FAILED: No location update of any kind received before maxfixtime\n");
		ERR_PRINTF1(_L("<font><b>FAILED: No location update of any kind received before maxfixtime.</b></font>\n"));
		}
	else	// we got some sort of position
		{
		if(!iTriedToCancelLastRequest)		
		{
		// log the last update reported for manual inspection:
		LogPosition(iLastReportedSatPosition);	
		}
		
		if(iDeviceStatusErrorReported)
			{
			TESTLOG(ELogP3, "WARNING: Module reported device status error during update request, negative results are inconclusive\n");
			WARN_PRINTF1(_L("<font><b>WARNING: Module reported device status error during update request, negative results are inconclusive.</b></font>\n"));			
			}
			
		// If 'normal' data quality was reported during update window, then we should have received an update with expected accuracy
		if(!iGotExpectedAccuracy && iDataQualityNormalReported)
			{
			TESTLOG(ELogP3, "FAILED: Update with module's expected accuracy was not received even tho' normal data quality was reported during request\n");
			ERR_PRINTF1(_L("<font><b>FAILED: We did not get an update with module's expected accuracy even tho' normal data quality was reported during request.</font></b>"));
			SetTestStepResult(EFail);
			}
		
		// warn if data quality was not acheived
		if(!iTriedToCancelLastRequest)	// if request was cancelled we don't expect to achieve data quality
			{
			if(!iGotRequestedAccuracy && (iLastRequestedQuality.MinHorizontalAccuracy() >= iParent.iModuleInfoQuality.HorizontalAccuracy()) && (iLastRequestedQuality.MinVerticalAccuracy() >= iParent.iModuleInfoQuality.VerticalAccuracy()))	// we didn't get the (reasonable) accuracy we asked for
				{
				if(!iDataQualityPartialReported && iLastReportedStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityPartial)
					{
					TESTLOG(ELogP3, "WARNING: Module did not ever report partial data quality\n");
					WARN_PRINTF1(_L("<font><b>WARNING: Module did not ever report partial data quality. This may be due to very poor conditions.</b></font>\n"));					
					}
				else if(!iDataQualityNormalReported && iLastReportedStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityNormal)	// partial quality reported, but not normal
					{
					TESTLOG(ELogP3, "WARNING: Module did not ever report normal data quality\n");
					WARN_PRINTF1(_L("<font><b>WARNING: Module did not ever report normal data quality. This may be due to poor conditions.</b></font>\n"));			
					
					TESTLOG3(ELogP3, "WARNING: Module did not produce an update of the requested accuracy (%fm horiz / %fm vert) in partial data quality conditions\n", iLastRequestedQuality.MinHorizontalAccuracy(), iLastRequestedQuality.MinVerticalAccuracy());
					WARN_PRINTF3(_L("<font><b>WARNING: Module did not produce an update of the requested accuracy (%fm horiz / %fm vert) in partial data quality conditions.</b></font>\n"), iLastRequestedQuality.MinHorizontalAccuracy(), iLastRequestedQuality.MinVerticalAccuracy());					
					}
				}			
			}	
		}
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::VerifyStandardResults()");				
	}
 
		
// log a position to TEF log and debug log:
void CT_LbsAGPSHAIValidateStepRoot::LogPosition(TPositionInfo aPosInfo)
	{
	TPosition pos;
	aPosInfo.GetPosition(pos);
	// TO DO: these numbers are TReal64s; how can they be properly displayed?
	INFO_PRINTF4(_L("<font><b>Last Position received for request was: %f lat, %f long, %f alt</font></b>"), pos.Latitude(), pos.Longitude(), pos.Altitude());
	TESTLOG4(ELogP2,"Last Position received for request was: %f lat, %f long, %f alt", pos.Latitude(), pos.Longitude(), pos.Altitude());

	}
	
// log a position to debug log:
void CT_LbsAGPSHAIValidateStepRoot::LogLastRequestedPositionDetails()
	{
	//TESTLOG(ELogP2,"TgtTime is:");
	LbsTestUtilities::PrintfDateTimeToDebugLog(iRequestedTgtTime);
	
	}	
		
	
/*
	Resets flags set during request
*/
void CT_LbsAGPSHAIValidateStepRoot::ResetRequestFlags()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::ResetRequestFlags()");		
	
	// reset flags from previous request:
	iGotPos = EFalse;
	iGotExpectedAccuracy = EFalse;
	iGotRequestedAccuracy = EFalse;	
	
	iTriedToCancelLastRequest = EFalse;
	iExpectUpdatesStopped = EFalse;

	iDataQualityNormalReported = EFalse;			
	iDataQualityPartialReported = EFalse;
		
	if(iLastReportedStatus.DeviceStatus() != TPositionModuleStatus::EDeviceError)
		{
		iDeviceStatusErrorReported = EFalse;
		}
	
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::ResetRequestFlags()");				
	}	
	
	

/*
Start a 'shutdown request' timer. We expect to have received a Shutdown() call before this fires
*/	
void CT_LbsAGPSHAIValidateStepRoot::StartShutdownRequestTimer()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStep_Main::StartShutdownRequestTimer()");				

	TInt configValue;
	if(!GetIntFromConfig(ConfigSection(), KShutdownRequestTime, configValue))
		{
		// timer for 'device status change':	
		User::Panic(_L("unconfigured devicestatuschange_time.\n"), KErrArgument);					
		}
	TESTLOG2(ELogP2, "Starting 'shutdown request' timer of %d seconds\n", configValue/1000000);
	__ASSERT_DEBUG(!iShutdownRequestTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStep_Main::StartShutdownRequestTimer: Timer already running"), KErrGeneral));	
	TTimeIntervalMicroSeconds32 temp = configValue;

	// start first device status timer:
	iShutdownRequestTimer->SetTimer(temp);


	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStep_Main::StartShutdownRequestTimer()");	
	}
		
/*
	Starts a max fix timer, based on the last request details, unless flag indicates not to
*/
void CT_LbsAGPSHAIValidateStepRoot::StartMaxFixStateTimer()
	{
	// start maxfix timer:
	TESTLOG(ELogP2, "Starting Max Fix timer\n");
	__ASSERT_DEBUG(!iStateTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartMaxFixStateTimer: State Timer already running"), KErrGeneral));		
	TESTLOG(ELogP2, "State -> EStateAwaitingUpdates\n");
	iState = EStateAwaitingUpdates;
	iStateTimer->SetTimer(iRequestedTgtTime + iLastRequestedQuality.MaxFixTime() + KProcessTimeMargin);	
	}
	
/*
	Starts a target time timer, based on the last request details
*/
void CT_LbsAGPSHAIValidateStepRoot::StartTgtTimeStateTimer()
	{
	TESTLOG(ELogP2, "Starting Tgt Time timer\n");
	__ASSERT_DEBUG(!iStateTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartTgtTimeStateTimer: State Timer already running"), KErrGeneral));	
	TESTLOG(ELogP2, "State -> EStateAwaitingTgtTime\n");
	iState = EStateAwaitingTgtTime;
	// start target time timer:
	// note: (low res) timer may fire a fraction of a second after the time specified, so allow for that with margin
	iStateTimer->SetTimer(iRequestedTgtTime - KProcessTimeMargin);
	}
	
/*
	Starts a stray updates timer
*/
void CT_LbsAGPSHAIValidateStepRoot::StartStrayUpdsStateTimer()
	{		
	TESTLOG2(ELogP2, "Starting Stray Updates timer of %d micsoseconds\n", KStrayUpdatesTime.Int() );
	__ASSERT_DEBUG(!iStateTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartStrayUpdsStateTimer: State Timer already running"), KErrGeneral));	
	TESTLOG(ELogP2, "State -> EStateAwaitingStrayUpdates\n");
	iState = EStateAwaitingStrayUpdates;
	// start stray updates timer:
	iStateTimer->SetTimer(KStrayUpdatesTime);
	}	
	
	
/*
	Starts an 'inactive' timer (by which time we expect device to be in 'inactive' state), based on configured time
*/
void CT_LbsAGPSHAIValidateStepRoot::StartInactiveTimer()
	{
	__ASSERT_DEBUG(!iInactiveTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartInactiveTimer: Timer already running"), KErrGeneral));
		
	TInt configValue;
	if(!GetIntFromConfig(ConfigSection(), KInactiveTime, configValue))
		{
		// timer for 'inactive' status:	
		User::Panic(_L("unconfigured inactive_time.\n"), KErrArgument);					
		}
	TESTLOG2(ELogP2, "Starting Inactive timer of %d seconds\n", configValue/1000000);
	
	TTimeIntervalMicroSeconds32 temp = configValue;
	iInactiveTimer->SetTimer(temp);
	}	
	
/*
	Starts an 'normal quality' timer (by which time we expect device to be in 'normal quality' state)
	Note: it is assumed here that TTFF is a reasonable time to use here
*/
void CT_LbsAGPSHAIValidateStepRoot::StartNormalQualityTimer()
	{	
	TTimeIntervalMicroSeconds32 normalqualitytime = static_cast<TTimeIntervalMicroSeconds32>(I64LOW(iParent.iModuleInfoQuality.TimeToFirstFix().Int64()));
	TESTLOG2(ELogP2, "Starting 'Normal Quality' timer of %d microseconds\n", normalqualitytime.Int());	
	__ASSERT_DEBUG(!iNormalQualityTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartNormalQualityTimer: Timer already running"), KErrGeneral));	
	iNormalQualityTimer->SetTimer(normalqualitytime);	
	}	
	
/*
	Starts a 'standby' timer (by which time we expect device to be in low power ('standby' or 'ready') state), based on configured time
*/
void CT_LbsAGPSHAIValidateStepRoot::StartStandbyTimer()
	{
	TInt configValue;
	if(!GetIntFromConfig(ConfigSection(), KStandbyTime, configValue))
		{
		// timer for 'standby' status:	
		User::Panic(_L("unconfigured standby_time.\n"), KErrArgument);					
		}
	TESTLOG2(ELogP2, "Starting Standby timer of %d seconds\n", configValue/1000000);
	__ASSERT_DEBUG(!iStandbyTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartStandbyTimer: Timer already running"), KErrGeneral));		
	TTimeIntervalMicroSeconds32 temp = configValue;
	iStandbyTimer->SetTimer(temp);
	}	
	
/*
	Starts a 'first device status' timer (by which time we expect module to have reported device status)
*/
void CT_LbsAGPSHAIValidateStepRoot::StartFirstDeviceStatusTimer()
	{
	TESTLOG2(ELogP2, "Starting 'First Device Status' timerof %d microseconds\n", KExpectFirstDeviceStatusTime.Int());
	__ASSERT_DEBUG(!iFirstDeviceStatusTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartFirstDeviceStatusTimer: Timer already running"), KErrGeneral));		
	// start first device status timer:
	iFirstDeviceStatusTimer->SetTimer(KExpectFirstDeviceStatusTime);
	}	



/*
	Starts an 'immediate' request based on the settings in module info:
	Starts a timer to fire when maxfixtime reached, unless this has been suppressed
*/
void CT_LbsAGPSHAIValidateStepRoot::StartImmediateDefaultRequest()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::StartImmediateDefaultRequest()");				
	// set 'immediate' target time
	iRequestedTgtTime.UniversalTime();	
	
	// set 'default' quality of request:
	// maxfix time 
	TTimeIntervalMicroSeconds maxfixtime = iParent.iModuleInfoQuality.TimeToFirstFix();
		
	iLastRequestedQuality.SetMaxFixTime(maxfixtime);

	// use default accuracy
	iLastRequestedQuality.SetMinHorizontalAccuracy(iParent.iModuleInfoQuality.HorizontalAccuracy());
	iLastRequestedQuality.SetMinVerticalAccuracy(iParent.iModuleInfoQuality.VerticalAccuracy());					

	TESTLOG(ELogP2, "Requesting Location Update with tgt time:\n");
	LogLastRequestedPositionDetails();
	// request location update
	iHAIModule->RequestLocationUpdate(iRequestedTgtTime, iLastRequestedQuality);	
	
	// cancel any outstanding timers start maxfix time timer and timer for first expected update:	
	CancelAllTimers();	
	iAfterTgtTimeTimer->SetTimer(KFirstUpdateDelay);
	// start max fix timer:
	StartMaxFixStateTimer();
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::StartImmediateDefaultRequest()");				
	}

/*
	Starts a request based on the config settings for 'first' request:
	Assumes that new target time will be in config 
	quality may be configured. If not configured, uses module info values
	Starts a timer to fire when targettime or (if latter 'immediate') maxfixtime reached
	
*/
void CT_LbsAGPSHAIValidateStepRoot::StartFirstRequest()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::StartFirstRequest()");
	TInt configValue, configValue2;
		
	// set target time
	iRequestedTgtTime.UniversalTime();	
	// add offset from config:
	if(!GetIntFromConfig(ConfigSection(), KTgtTimeOffsetFromNow, configValue))
		{
		User::Panic(_L("Unconfigured tgttime_offset_2.\n"), KErrArgument);					
		}
	TTimeIntervalMicroSeconds offset = configValue;
	iRequestedTgtTime += offset;

	// set quality of request:
	// maxfix time 
	TTimeIntervalMicroSeconds maxfixtime;
	if(GetIntFromConfig(ConfigSection(), KMaxFixTime, configValue)) //configured
		{
		maxfixtime = configValue;
		}
	else	// use 'default' (TTFF)
		{
		maxfixtime = iParent.iModuleInfoQuality.TimeToFirstFix();
		}
		
	LastRequestedQuality().SetMaxFixTime(maxfixtime);
	
	// accuracy
	if(GetIntFromConfig(ConfigSection(), KAccuracyHoriz, configValue) && GetIntFromConfig(ConfigSection(), KAccuracyVert, configValue2))
		{
		// accuracy configured must always be >= expected accuracy in module info
		LastRequestedQuality().SetMinHorizontalAccuracy(configValue);
		LastRequestedQuality().SetMinVerticalAccuracy(configValue2);
		if((LastRequestedQuality().MinHorizontalAccuracy() < iParent.iModuleInfoQuality.HorizontalAccuracy()) || LastRequestedQuality().MinVerticalAccuracy() < iParent.iModuleInfoQuality.VerticalAccuracy())
			{
			TESTLOG(ELogP2, "NOTE: Requesting Location Update with accuracy higher than module's expected accuracy\n");
			}				
		}
	else
		{
		// use default accuracy
		LastRequestedQuality().SetMinHorizontalAccuracy(iParent.iModuleInfoQuality.HorizontalAccuracy());
		LastRequestedQuality().SetMinVerticalAccuracy(iParent.iModuleInfoQuality.VerticalAccuracy());					
		}
	
	TESTLOG(ELogP2, "Requesting Location Update with tgt time:\n");
	LogLastRequestedPositionDetails();
		
	// request location update
	HAIModule()->RequestLocationUpdate(iRequestedTgtTime, LastRequestedQuality());	
		
	if(offset.Int64() > 0)	// tgttime in the future
		{
		StartTgtTimeStateTimer();	
		}
	else	// immediate
		{
		iAfterTgtTimeTimer->SetTimer(KFirstUpdateDelay);
		StartMaxFixStateTimer();
		}
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::StartFirstRequest()");		
	}
	
/*
	Starts a new request based on the config settings for 'second' request:
	Assumes that new target time will be in config (unless using previous tgttime again), and quality may be configured. If quality not configured, uses module info values
	Starts a timer to fire when targettime or (if latter 'immediate') maxfixtime reached
*/
void CT_LbsAGPSHAIValidateStepRoot::StartAnotherRequest()
	{		
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::StartAnotherRequest()");				
		
	ResetRequestFlags();
	
	TInt configValue, configValue2;
	TTime now;
	now.UniversalTime();
	
	iPreviousRequestedTgtTime = iRequestedTgtTime;
	
	if(!iSameTgtTimeAsPrevious)
		{
		// set target time
		iRequestedTgtTime.UniversalTime();	
		// add offset from config:
		if(!GetIntFromConfig(ConfigSection(), KTgtTimeOffsetFromNow_2, configValue))
			{
			User::Panic(_L("Unconfigured tgttime_offset_2.\n"), KErrArgument);					
			}
		TTimeIntervalMicroSeconds offset;
		offset = configValue;
		iRequestedTgtTime += offset;
		}
	else	// make sure target time hasn't already passed since we're using it again here
		{
		ASSERT(iRequestedTgtTime > now);
		}

	// set quality of request:
	// maxfix time 
	TTimeIntervalMicroSeconds maxfixtime;
	if(GetIntFromConfig(ConfigSection(), KMaxFixTime_2, configValue))
		{
		maxfixtime = configValue;
		}
	else
		{
		maxfixtime = iParent.iModuleInfoQuality.TimeToFirstFix();
		}
		
	iLastRequestedQuality.SetMaxFixTime(maxfixtime);
	
	// accuracy
	if(GetIntFromConfig(ConfigSection(), KAccuracyHoriz_2, configValue) && GetIntFromConfig(ConfigSection(), KAccuracyVert_2, configValue2))
		{
		iLastRequestedQuality.SetMinHorizontalAccuracy(configValue);
		iLastRequestedQuality.SetMinVerticalAccuracy(configValue2);
		if((iLastRequestedQuality.MinHorizontalAccuracy() < iParent.iModuleInfoQuality.HorizontalAccuracy()) || iLastRequestedQuality.MinVerticalAccuracy() < iParent.iModuleInfoQuality.VerticalAccuracy())
			{
			TESTLOG(ELogP2, "NOTE: Requesting Location Update with accuracy higher than module's expected accuracy\n");
			}		
		}
	else
		{
		// use default accuracy
		iLastRequestedQuality.SetMinHorizontalAccuracy(iParent.iModuleInfoQuality.HorizontalAccuracy());
		iLastRequestedQuality.SetMinVerticalAccuracy(iParent.iModuleInfoQuality.VerticalAccuracy());					
		}
	
	TESTLOG(ELogP2, "Requesting Location Update with tgt time:\n");
	LogLastRequestedPositionDetails();
	
	// request location update
	iHAIModule->RequestLocationUpdate(iRequestedTgtTime, iLastRequestedQuality);	
	
	CancelAllTimers();
	
	// start appropriate timer	
	if(iRequestedTgtTime > now)	// tgttime in the future
		{
		StartTgtTimeStateTimer();	
		}
	else	// immediate
		{
		iAfterTgtTimeTimer->SetTimer(KFirstUpdateDelay);
		StartMaxFixStateTimer();
		}
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::StartAnotherRequest()");		
	}


// shutdown module
void CT_LbsAGPSHAIValidateStepRoot::FinishTest()	
	{	
	TESTLOG(ELogP2, "State -> EStateAwaitingShutdownRequest\n");
	iState = EStateAwaitingShutdownRequest;						
	iHAIModule->AdvisePowerMode(CLbsLocationSourceGpsBase::EPowerModeClose);
	}

//------------------------ from MT_LbsTimerUtilsObserver -----------------------------	
	
// called when timer fires
void CT_LbsAGPSHAIValidateStepRoot::HandleTimerL(TInt aTimerId, const TTime& aTargetTime)
	{
	(void)aTargetTime;
	
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::HandleTimerL()");		
	
	switch(aTimerId)
		{	
 		case KShutdownRequestTimerId:
 			{
 			TESTLOG(ELogP2, "'Shutdown request' time reached");
 			// we expect the device to have requested to be shutdown by now
 			if(!iShutdownRequestReceivedAndIgnored)
 				{
 				SetTestStepResult(EFail);
 				TESTLOG(ELogP3, "FAILED: Device did not request to be shutdown within time limit\n");
 				ERR_PRINTF1(_L("<font><b>FAILED: Device did not request to be shutdown within time limit.</b></font>\n"));				
 				FinishTest();
 				}
 			else
 				{
 				// don't ignore again next time
 				iIgnoreShutdownRequest = EFalse;				
 				StartImmediateDefaultRequest();
 				}
 			}
 			break;
		
		case KShortlyAfterTargetTimeTimerId:	// 'shortly after' target time
			{
			TESTLOG(ELogP2, "'Shortly after target time' time reached");
			// we should have received an update (possibly blank) at this point
			if(!iGotPos)
				{
				TESTLOG(ELogP3, "WARNING: Possible hardware fault. Shortly after targettime and no updates have yet been received\n");
				WARN_PRINTF1(_L("<font><b>WARNING: Possible hardware fault. Shortly after targettime and no updates have yet been received</b></font>\n"));
				}
			}
			break;
			
		case KFirstDeviceStatusTimerId:	// we expect to have received first device status report by now
			{
			TESTLOG(ELogP2, "'First Device Status' time reached");
			if(TPositionModuleStatus::EDeviceUnknown == iLastReportedStatus.DeviceStatus())
				{
				TESTLOG(ELogP2, "WARNING: No device status update received shortly after loading module.");
				}
			if(TPositionModuleStatus::EDeviceInactive != iLastReportedStatus.DeviceStatus())	// module hasn't 'settled' yet
				{
				// start a timer when we expect 'inactive' status to be acheived:
				StartInactiveTimer();
				}
			else	// 'Inactive' device status received
				{
				FinishTest();
				}
			}
			break;
			
		case KFirstQualityStatusTimerId:	// we expect to have received first device status report by now
			{
			TESTLOG(ELogP2, "'First Quality Status' time reached");
			if(TPositionModuleStatus::EDataQualityUnknown == iLastReportedStatus.DataQualityStatus())
				{
				TESTLOG(ELogP2, "WARNING: No data quality status update received shortly after loading module.");
				}
			if(TPositionModuleStatus::EDataQualityNormal != iLastReportedStatus.DataQualityStatus())
				{
				// start a timer when we expect 'normal data quality' status to be acheived:
				StartNormalQualityTimer();
				}
			else	// 'Inactive' device status received
				{
				FinishTest();
				}			
			}
			break;
			
		case KInactiveTimerId:	// we expect device to be in 'inactive' state by now
			{
			TESTLOG(ELogP2, "'Inactive' time reached\n");
			iInactivityTimerExpired = ETrue;
			if(TPositionModuleStatus::EDeviceInactive != iLastReportedStatus.DeviceStatus())
				{
				SetTestStepResult(EFail);
				TESTLOG(ELogP3, "FAILED: Device did not settle to 'Inactive' status\n");
				ERR_PRINTF1(_L("<font><b>FAILED: Device did not settle to 'Inactive' status.</b></font>\n"));														
				}
			}
			if(iFinishOnInactiveExpected)
				{
				FinishTest();
				}
			break;
			
		case KNormalQualityTimerId:	// we expect device to be in 'normal quality' state by now, under reasonable conditions
			{
			TESTLOG(ELogP2, "'Normal Quality' time reached\n");
			if(TPositionModuleStatus::EDataQualityUnknown == iLastReportedStatus.DataQualityStatus())
				{
				SetTestStepResult(EFail);
				TESTLOG(ELogP3, "FAILED: No data quality status update received.");
				ERR_PRINTF1(_L("<font><b>FAILED: No data quality status update received.</b></font>"));				
				}
			else if(TPositionModuleStatus::EDataQualityNormal != iLastReportedStatus.DataQualityStatus())
				{
				TESTLOG(ELogP3, "WARNING: Device did not settle to 'Normal Data Quality' status\n");
				WARN_PRINTF1(_L("<font><b>WARNING: Device did not settle to 'Normal Data Quality' status.</b></font>\n"));														
				}
			FinishTest();
			}
			break;			
		
		case KStandbyTimerId:	// we expect to be in 'standby' or 'ready' state by now
			{
			iStandbyTimerExpired = ETrue;
			TESTLOG(ELogP2, "'Standby' time reached\n");
			if(TPositionModuleStatus::EDeviceStandBy != iLastReportedStatus.DeviceStatus() && TPositionModuleStatus::EDeviceReady != iLastReportedStatus.DeviceStatus())
				{
				SetTestStepResult(EFail);
				TESTLOG(ELogP3, "FAILED: Device did not go to 'Standby' or 'Ready' status\n");
				ERR_PRINTF1(_L("<font><b>FAILED: Device did not go to 'Standby' or 'Ready' status.</b></font>\n"));
				}
			else if(TPositionModuleStatus::EDeviceStandBy != iLastReportedStatus.DeviceStatus())
				{
				TESTLOG(ELogP3, "WARNING: Device did not go to 'Standby' status. This may have implications for power use\n");
				WARN_PRINTF1(_L("<font><b>WARNING: Device did not go to 'Standby' status. This may have implications for power use</b></font>\n"));				
				}
			}
			break;		
			
		case KStartSecondRequestTimerId:	// start a request based on details in config:
			{
			TESTLOG(ELogP2, "'Start New Request' Timer Fired");
			StartAnotherRequest();
			}
			break;
		
		case KCancelTimerId: // cancel the last request:
			{
			TESTLOG(ELogP2, "'Cancel' Timer Fired. Cancelling last request");
			
			iHAIModule->CancelLocationRequest();
			iTriedToCancelLastRequest = ETrue;
			
			// start a timer when we expect updates to have stopped
			TInt configValue;
			if(!GetIntFromConfig(ConfigSection(), KUpdsCancelledByTime, configValue))
				{
				// timer for starting second request:	
				User::Panic(_L("unconfigured cancel time.\n"), KErrArgument);
				}
			TTimeIntervalMicroSeconds32 temp = configValue;
			iUpdatesStoppedTimer->SetTimer(temp);
			
			}
			break;
			
		case KUpdatesStoppedTimerId:
			{
			TESTLOG(ELogP2, "'UpdatesStopped' Timer Fired. We don't expect to see any more updates for this request");
			iExpectUpdatesStopped = ETrue;
			}
			break;

		case KStateTimerId:
			{
			TESTLOG(ELogP2, "'State' Timer Fired");
			switch(iState)
				{	
				case EStateAwaitingTgtTime:	// target time has been reached
					{
					TESTLOG(ELogP2, "Target time reached\n");
					// we should start receiving updates after a 'short time' from now:
					iAfterTgtTimeTimer->SetTimer(KFirstUpdateDelay);
					StartMaxFixStateTimer();
					}
					break;
					
				case EStateAwaitingUpdates:	// maxfix time has been reached
				case EStateGotAccuratePos:
					{
					TESTLOG(ELogP2, "Maxfix time reached\n");
					if(iStrayUpdatesTimer)
						{
						StartStrayUpdsStateTimer();
						}
					else if(iStartNewRequestOnPreviousFinished)
						{	
						TESTLOG(ELogP2, "'Start another request' flag set\n");
						// verify the first request as expected
						VerifyStandardResults();
										
						// start next request
						iNumAdditionalRequests--;			
						iStartNewRequestOnPreviousFinished = iNumAdditionalRequests? ETrue : EFalse;
						iTracking = ETrue;
						StartAnotherRequest();

						}
					else
						{
						FinishTest();
						}
					}
					break;
					
				case EStateAwaitingStrayUpdates:	// end of stray updates time has been reached
					TESTLOG(ELogP2, "Stray Updates timer fired\n");
					FinishTest();
					break;
				}
			}
		}	// end switch
		
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::HandleTimerL()");			
	}
	
//------------------------ end MT_LbsTimerUtilsObserver -----------------------------
	
	
//------------------------ from MLbsLocationSourceGpsObserver -----------------------------

TVersion CT_LbsAGPSHAIValidateStepRoot::Version()
	{
	return TVersion(1,0,0);
	}

// from MLbsLocationSourceGpsObserver
// new location arrives from integration module ...
void CT_LbsAGPSHAIValidateStepRoot::UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosInfoArray[], TInt aNumItems, const TTime& aTargetTime)
	{	
	(void)aNumItems;
	
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::UpdateLocation()");		
	__ASSERT_DEBUG(aPosInfoArray != NULL,User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot"), KErrArgument));
	__ASSERT_DEBUG(aNumItems == 1,User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot"), KErrArgument));
	__ASSERT_DEBUG((aPosInfoArray[0]->PositionClassType() | EPositionSatelliteInfoClass),User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot"), KErrArgument));

	if(KErrNone != aStatus)
		{
			// in MOAP case check here for the error returned from the 'network'; this error should be set here.
#ifdef TESTING_ON_MOAP
		if(iAssistanceDataError != aStatus)
			{	
#endif			
			TESTLOG2(ELogP3, "FAILED: UpdateLocation called with an unexpected error, %d\n", aStatus);
			ERR_PRINTF2(_L("<font><b>FAILED: UpdateLocation returned an error, %d</b></font>\n"), aStatus);
			SetTestStepResult(EFail);
#ifdef TESTING_ON_MOAP
			}
		else
			{
			TESTLOG2(ELogP3, "UpdateLocation called with expected error, %d\n", aStatus);	
			}
#endif			
		}
	else
		{
		
	if(TPositionModuleStatus::EDeviceActive != iLastReportedStatus.DeviceStatus())
		{	
		TESTLOG(ELogP3, "WARNING: Location Update Received when device status is not 'Active'\n");
		
		if(TPositionModuleStatus::EDeviceReady != iLastReportedStatus.DeviceStatus())
			{
			TESTLOG(ELogP3, "FAILED: Location Update Received when device status is not 'Active' or 'Standby'\n");
			SetTestStepResult(EFail);
			}
		}
		
	if((iRequestedTgtTime == 0) || (iRequestedTgtTime != aTargetTime))	// unsolicited update
		{
		if(aTargetTime == 0)	// 'legal' unsolicited update
			{
			TESTLOG(ELogP3, "WARNING: Unsolicited Location Update Received\n");
			WARN_PRINTF1(_L("<font><b>WARNING: Unsolicited Location Update Received.</b></font>\n"));
			}
		else if(aTargetTime == iPreviousRequestedTgtTime)
			{
			TESTLOG(ELogP3, "WARNING: received an update for the last-but-one request\n");
			WARN_PRINTF1(_L("<font><b>WARNING: received an update for the last-but-one request</font></b>\n"));
			}
		else
			{
			TESTLOG(ELogP3, "WARNING: Location Update Received with unmatching target time\n");
			WARN_PRINTF1(_L("<font><b>WARNING: Location Update Received with unmatching target time.</b></font>\n"));
			}
		}
	else	// this is a response to our update request
		{
		TESTLOG(ELogP2, "Got Response to Location Request\n");
		
		if(iExpectUpdatesStopped)
			{
			TESTLOG(ELogP3, "WARNING: (Power drain / performance): Location Updates still being received some time after request was cancelled\n");
			WARN_PRINTF1(_L("<font><b>WARNING: (Power drain / performance) Location Updates still being received some time after request was cancelled.</b></font>\n"));	
			}
		switch(iState)
			{
			case EStateAwaitingTgtTime:
				{
				SetTestStepResult(EFail);
				TESTLOG(ELogP3, "FAILED: Location Update Received before target time\n");
				ERR_PRINTF1(_L("<font><b>FAILED: Location Update Received before target time.</b></font>\n"));
				}
				break;
				
			case EStateAwaitingUpdates:	// we are in window between targettime and maxfixtime
				{
				iGotPos = ETrue;
									
				// Store the last update received (overwrite previous updates) for manual inspection
				const TPositionSatelliteInfo* satPositionPtr = (reinterpret_cast<const TPositionSatelliteInfo*>(aPosInfoArray[0]));
				iLastReportedSatPosition = *satPositionPtr;
									
				TPosition pos;
				iLastReportedSatPosition.GetPosition(pos);
						
				TESTLOG3(ELogP2, "position received has accuracy %fm horiz * %fm vert\n", pos.HorizontalAccuracy(), pos.VerticalAccuracy() );						
				TESTLOG2(ELogP2, "last reported data quality was %d\n", iLastReportedStatus.DataQualityStatus());
				// Compare accuracy to see if it matches expected accuracy for this module:
				if(pos.HorizontalAccuracy() <= iParent.iModuleInfoQuality.HorizontalAccuracy() && pos.VerticalAccuracy() <= iParent.iModuleInfoQuality.HorizontalAccuracy())
					{
					iGotExpectedAccuracy = ETrue;
					iGotRequestedAccuracy = ETrue;
					TESTLOG(ELogP2, "Got update with module's expected accuracy\n");
					if(!(iLastReportedStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityNormal))
						{
						TESTLOG(ELogP3, "FAILED: Last data quality reported was not 'normal' yet update meets expected accuracy from module info\n");
						ERR_PRINTF1(_L("<font><b>FAILED: Last data quality reported was not 'normal' yet update meets expected accuracy from module info.</font></b>\n"));
						SetTestStepResult(EFail);
						}
					}						
				// Compare accuracy to see if it matches what we asked for:
				else if(pos.HorizontalAccuracy() <= iLastRequestedQuality.MinHorizontalAccuracy() && pos.VerticalAccuracy() <= iLastRequestedQuality.MinVerticalAccuracy())
					{
					iGotRequestedAccuracy = ETrue;
					TESTLOG(ELogP2, "Got update with desired accuracy\n");
					if(iStartInactiveTimerOnGotAccPos)
						{
						StartInactiveTimer();
						}
					else if(iStartDistantRequestAndStandbyTimerOnGotAccPos)
						{
						TESTLOG(ELogP2, "'Start another request on accurate position received' flag set\n");
												
						StartAnotherRequest();	
						
						StartStandbyTimer();
						
						}
					else 
						{
						TESTLOG(ELogP2, "State -> EStateGotAccuratePos\n");
						iState = EStateGotAccuratePos;
						}
					}
											
				// verify that update is what we'd expect given current data quality		
				if(iLastReportedStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityNormal)
					{
					// the update should meet expected accuracy (as per module info)
					if(!iGotExpectedAccuracy)
						{
						TESTLOG(ELogP3, "FAILED: Last data quality reported was 'normal' yet update does not meet expected accuracy from module info\n");
						ERR_PRINTF1(_L("<font><b>FAILED: Last data quality reported was 'normal' yet update does not meet expected accuracy from module info.</font></b>\n"));
						SetTestStepResult(EFail);
						}
					}
				else if(iLastReportedStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityPartial)
					{
					// the update should contain at least lat and long. 
					if( (Math::IsNaN(pos.Latitude())) || (Math::IsNaN(pos.Longitude())))
						{
						TESTLOG(ELogP3, "FAILED: Latitude and Longitude should not be NAN unless data quality EDataQualityLoss is reported\n");
						ERR_PRINTF1(_L("<font><b>FAILED: Latitude and Longitude should not be NAN unless data quality EDataQualityLoss is reported.</b></font>\n"));						
						SetTestStepResult(EFail);
						}
					}
				else // data quality loss
					{
					__ASSERT_DEBUG((iLastReportedStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityLoss), User::Panic(_L("Unknown data quality reported. \n"), KErrArgument));
					if( (!Math::IsNaN(pos.Latitude())) && (!Math::IsNaN(pos.Longitude())))
						{
						TESTLOG(ELogP3, "FAILED: Current data quality is EDataQualityLoss, yet neither Latitude nor Longitude is NaN\n");
						ERR_PRINTF1(_L("<font><b>FAILED: Current data quality is EDataQualityLoss, yet neither Latitude and Longitude is NaN.</b></font>\n"));						
						SetTestStepResult(EFail);						
						}
					}
					
				if(iGotRequestedAccuracy && iStartNewRequestOnPreviousFinished)
					{	
					TESTLOG(ELogP2, "'Start another request on got acc pos for previous' flag set\n");
					// verify the first request as expected
					VerifyStandardResults();
									
					// start next request
					iNumAdditionalRequests--;			
					iStartNewRequestOnPreviousFinished = iNumAdditionalRequests? ETrue : EFalse;
					iTracking = ETrue;
					iStartAnotherRequestFlag = ETrue;
					}
					
				}
				break;
				
			case EStateGotAccuratePos:
				{
				TESTLOG(ELogP3, "WARNING: (Power drain / performance): Location Update received after specified quality met\n");
				WARN_PRINTF1(_L("<font><b>WARNING: (Power drain / performance) Location Update received after specified quality met.</b></font>\n"));
				}
				break;
				
			case EStateAwaitingStrayUpdates:
				{
				TESTLOG(ELogP3, "WARNING: (Power drain / performance): Location Update received after specified quality met\n");
				WARN_PRINTF1(_L("<font><b>WARNING: (Power drain / performance) Location Update received after maxfix time.</b></font>\n"));
				}
				break;
			
				default:
					{
					User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::UpdateLocation() state not recognised"), KErrNotSupported);
					}
				}
			}
		}
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::UpdateLocation()");
	}


// from MLbsLocationSourceGpsObserver
void CT_LbsAGPSHAIValidateStepRoot::UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus aDeviceStatus)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::UpdateDeviceStatus()");		
	iLastReportedStatus.SetDeviceStatus(aDeviceStatus);
	TESTLOG2(ELogP2, "Device reported status %d\n", aDeviceStatus);

	switch(aDeviceStatus)
		{
		case TPositionModuleStatus::EDeviceError:
			{
			iDeviceStatusErrorReported = ETrue;
			}
			break;
						
		case TPositionModuleStatus::EDeviceDisabled:
			{
			ERR_PRINTF1(_L("<font><b>FAILED: Module reported device status 'disabled'.</font></b>"));
			SetTestStepResult(EFail);			
			}
			break;			
		}

	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::UpdateDeviceStatus()");			
	}
	 
// from MLbsLocationSourceGpsObserver	
void CT_LbsAGPSHAIValidateStepRoot::UpdateDataQualityStatus(TPositionModuleStatus::TDataQualityStatus aDataQuality)
	{	
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::UpdateDataQualityStatus()");			
	TESTLOG2(ELogP2, "Device reported data quality %d\n", aDataQuality);
	iLastReportedStatus.SetDataQualityStatus(aDataQuality);
	
  	if(TPositionModuleStatus::EDataQualityNormal == aDataQuality)
  		{
  		TESTLOG(ELogP2, "DataQualityPartial Reported"); 
  		iDataQualityNormalReported = ETrue;
  		}	
  	else if(TPositionModuleStatus::EDataQualityPartial == aDataQuality)
		{
		TESTLOG(ELogP2, "DataQualityPartial Reported");
		iDataQualityPartialReported = ETrue;
		}
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::UpdateDataQualityStatus()");			
	}

// from MLbsLocationSourceGpsObserver
// Integration module informs us that we may now delete it 
void CT_LbsAGPSHAIValidateStepRoot::Shutdown()
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::Shutdown()");			

	TBool warnMsg = EFalse;

	if(iState == EStateAwaitingShutdownRequest)
		{
		// End test.
		if(!iIgnoreShutdownRequest)
			{
			TESTLOG(ELogP2, "State -> EStateDone\n");
			iState = EStateDone;
			__ASSERT_DEBUG(!(iAfterTgtTimeTimer->IsActive()) && !(iStateTimer->IsActive()), User::Panic(_L("Timer running\n"), KErrGeneral));
			CActiveScheduler::Stop();
			}
		
		// Received shutdown notification from module unexpectedly again.
		else if (iShutdownRequestReceivedAndIgnored)
			{
			warnMsg = ETrue;
			}
			
 		else
 			{
 			iShutdownRequestReceivedAndIgnored = ETrue;
 			}			
		}
	else
		{
		warnMsg = ETrue;
		}

	// Warn of un-expected shutdown notifications.
	if (warnMsg)
		{
 		TESTLOG(ELogP3, "WARNING: Module requesting to be shutdown when we don't expect it to\n");
 		WARN_PRINTF1(_L("<font><b>WARNING: Module requesting to be shutdown when we don't expect it to.</font></b>"));
		}

	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::Shutdown()");					
	}

// from MLbsLocationSourceGpsObserver
// request from integration module
TInt CT_LbsAGPSHAIValidateStepRoot::GetAssistanceDataItem(TLbsAssistanceDataItem /*aItem*/, RDataReaderRootBase& /*aDataRoot*/, TTime& /*aTimeStamp*/)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::GetAssistanceDataItem()");
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::GetAssistanceDataItem()");
	return KErrNotSupported;
	}


// from MLbsLocationSourceGpsObserver
// request from integration module
TInt CT_LbsAGPSHAIValidateStepRoot::GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem /*aItem*/, TTime& /*aTimeStamp*/)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::GetAssistanceDataItemTimeStamp()");
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::GetAssistanceDataItemTimeStamp()");			
	return KErrNotSupported;
	}


// from MLbsLocationSourceGpsObserver
// Integration module requests assistance data from network
void CT_LbsAGPSHAIValidateStepRoot::RequestAssistanceData(TLbsAsistanceDataGroup /* aDataItemMask */)
	{
	TESTLOG(ELogP1, ">> CT_LbsAGPSHAIValidateStepRoot::RequestAssistanceData()");				
	TESTLOG(ELogP1, "<< CT_LbsAGPSHAIValidateStepRoot::RequestAssistanceData()");				
	}


//----------------------- end MLbsLocationSourceGpsObserver --------------------------

//------------------------ New test case methods -------------------------------------

void CT_LbsAGPSHAIValidateStepRoot::DoHAIAdviseCloseTestCase()
	{					
	// when device asks to be shutdown we want to ignore it 
	IgnoreShutdownRequest() = ETrue;
	
	TESTLOG(ELogP2, "State -> EStateAwaitingShutdownRequest\n");
	CT_LbsAGPSHAIValidateStepRoot::State() = EStateAwaitingShutdownRequest;
	
	HAIModule()->AdvisePowerMode(CLbsLocationSourceGpsBase::EPowerModeClose);
	
	StartShutdownRequestTimer();
	
	// start scheduler
	CActiveScheduler::Start();										
	}
	
void CT_LbsAGPSHAIValidateStepRoot::DoLoadUnLoadTestCase()
	{
	// there is nothing else to do for this test step!
	}
	
void CT_LbsAGPSHAIValidateStepRoot::DoImmediateRequestTestCase()
	{
	// tese case code
	StartFirstRequest();	
	iStrayUpdatesTimer = ETrue;
	// start scheduler
	CActiveScheduler::Start();
	VerifyStandardResults();
	}

void CT_LbsAGPSHAIValidateStepRoot::DoSecondLocationRequestTestCase()
	{
	// tese case code
	TInt configValue;
	StartFirstRequest();
	iStrayUpdatesTimer = ETrue;
	if(!GetIntFromConfig(ConfigSection(), KDelayBetweenRequests, configValue))
		{
		User::Panic(_L("unconfigured delay_between_requests.\n"), KErrArgument);
		}
	// timer for starting second request:
	TTimeIntervalMicroSeconds32 temp = configValue;
	iNewRequestTimer->SetTimer(temp);
	// start scheduler
	CActiveScheduler::Start();	
	VerifyStandardResults();
	}


void CT_LbsAGPSHAIValidateStepRoot::DoLowAccuracyLocationRequestTestCase()
	{
	// test case code
	iSameTgtTimeAsPrevious = ETrue;
	TInt configValue;
	StartFirstRequest();
	iStrayUpdatesTimer = ETrue;
	if(!GetIntFromConfig(ConfigSection(), KDelayBetweenRequests, configValue))
		{
		// timer for starting second request:	
		User::Panic(_L("unconfigured delay_between_requests.\n"), KErrArgument);
		}
	TTimeIntervalMicroSeconds32 temp = configValue;
	iNewRequestTimer->SetTimer(temp);
	// start scheduler
	CActiveScheduler::Start();	
	VerifyStandardResults();
	}
	
void CT_LbsAGPSHAIValidateStepRoot::DoTrackingRequestTestCase()
	{
	// test case code
	StartImmediateDefaultRequest();	
	iStartNewRequestOnPreviousFinished = ETrue;
	// how many more requests?
	if(!GetIntFromConfig(ConfigSection(), KNumAdditionalRequests, iNumAdditionalRequests))
		{
		User::Panic(_L("unconfigured num_additional_requests.\n"), KErrArgument);	
		}
	// start scheduler
	CActiveScheduler::Start();
	}
	

void CT_LbsAGPSHAIValidateStepRoot::DoImmediateRequestWithMaxFixTimeTestCase()
	{
	// test case code	
	StartImmediateDefaultRequest();
	// start scheduler
	CActiveScheduler::Start();	
	VerifyStandardResults();
	// verify that we received an update of expected accuracy by maxfixtime:
	if(!iGotExpectedAccuracy)
		{
		TESTLOG(ELogP3, "WARNING: Update with module's expected accuracy was not received by TTFF\n");
		WARN_PRINTF1(_L("<font><b>WARNING: We did not get an update with module's expected accuracy by TTFF. This could be due to conditions, but verify that TTFF in module info is valid.</font></b>"));
		}
	}
	
void CT_LbsAGPSHAIValidateStepRoot::DoFarOffLocationRequestTestCase()
	{
	// test case code		
	StartFirstRequest();
	// start scheduler
	CActiveScheduler::Start();	
	VerifyStandardResults();
	// verify that we got an update of the module's expected 
	if(!iGotExpectedAccuracy)
		{
		TESTLOG(ELogP3, "WARNING: Update with module's expected accuracy was not received by TTFF\n");
		WARN_PRINTF1(_L("<font><b>WARNING: We did not get an update with module's expected accuracy by TTFF. This could be due to conditions, but verify that TTFF in module info is valid and that module does not always wait for targettime before retrieving data from hw.</font></b>"));
		}
	}
	
void CT_LbsAGPSHAIValidateStepRoot::DoImmediateRequestWithCancelTestCase()
	{
	// test case code			
	StartFirstRequest();
	TInt configValue;
	if(!GetIntFromConfig(ConfigSection(), KCancelTime, configValue))
		{
		// timer for cancelling request:	
		User::Panic(_L("unconfigured cancel time.\n"), KErrArgument);
		}
	TTimeIntervalMicroSeconds32 temp = configValue;
	iCancelTimer->SetTimer(temp);
	// start scheduler
	CActiveScheduler::Start();
	VerifyStandardResults();
	}
	
void CT_LbsAGPSHAIValidateStepRoot::DoHAISettlesToInActiveTestCase()
	{
	// test case code	
	StartFirstDeviceStatusTimer();
	// since we have no request, we need this to stop the test:
	iFinishOnInactiveExpected = ETrue;
	// start scheduler
	CActiveScheduler::Start();	
	}
	
void CT_LbsAGPSHAIValidateStepRoot::DoHAIToggleActiveStatusTestCase()
	{
	// test case code		
	iStartInactiveTimerOnGotAccPos = ETrue;
	StartFirstRequest();
	// start scheduler
	CActiveScheduler::Start();
	if(!iGotRequestedAccuracy)
		{
		WARN_PRINTF1(_L("<font><b>NOTE: Accurate position never acheived. Test Result is inconclusive.</b></font>\n"));	
		TESTLOG(ELogP3, "NOTE: Accurate position never acheived. Test Result is inconclusive\n");																		
		}
	else if(!iInactivityTimerExpired)
		{
		WARN_PRINTF1(_L("<font><b>NOTE: Test finished before Inactive timer fired. Test Result is inconclusive.</b></font>\n"));	
		TESTLOG(ELogP3, "NOTE: Test finished before Inactive timer fired. Test Result is inconclusive\n");												
		}
	VerifyStandardResults();
	}

// helper
TLbsLocRequestQuality& CT_LbsAGPSHAIValidateStepRoot::LastRequestedQuality()
	{
	return iLastRequestedQuality;
	}
	
TLbsLocRequestQuality CT_LbsAGPSHAIValidateStepRoot::LastRequestedQuality() const
	{
	return iLastRequestedQuality;
	}
	
// helper
TPositionModuleStatus::TDeviceStatus& CT_LbsAGPSHAIValidateStepRoot::ExpectedDeviceStatus()
	{
	return iExpectedDeviceStatus;
	}
	
TPositionModuleStatus::TDeviceStatus CT_LbsAGPSHAIValidateStepRoot::ExpectedDeviceStatus() const
	{
	return iExpectedDeviceStatus;
	}

// helper
TPositionModuleStatus::TDeviceStatus& CT_LbsAGPSHAIValidateStepRoot::AltExpectedDeviceStatus()
	{
	return iAltExpectedDeviceStatus;
	}
	
TPositionModuleStatus::TDeviceStatus CT_LbsAGPSHAIValidateStepRoot::AltExpectedDeviceStatus() const
	{
	return iAltExpectedDeviceStatus;
	}

// helper
TBool& CT_LbsAGPSHAIValidateStepRoot::IgnoreShutdownRequest()
	{
	return iIgnoreShutdownRequest;
	}
	
TBool CT_LbsAGPSHAIValidateStepRoot::IgnoreShutdownRequest() const
	{
	return iIgnoreShutdownRequest;
	}
	
// helper
CT_LbsAGPSHAIValidateStepRoot::TState& CT_LbsAGPSHAIValidateStepRoot::State()
	{
	return iState;
	}
	
CT_LbsAGPSHAIValidateStepRoot::TState CT_LbsAGPSHAIValidateStepRoot::State() const
	{
	return iState;
	}

// helper	
TBool& CT_LbsAGPSHAIValidateStepRoot::StrayUpdatesTimer()
	{
	return iStrayUpdatesTimer;
	}
	
TBool CT_LbsAGPSHAIValidateStepRoot::StrayUpdatesTimer() const
	{
	return iStrayUpdatesTimer;
	}
	
// helper
TPositionModuleStatus& CT_LbsAGPSHAIValidateStepRoot::LastReportedStatus()
	{
	return iLastReportedStatus;
	}
	
TPositionModuleStatus CT_LbsAGPSHAIValidateStepRoot::LastReportedStatus() const
	{
	return iLastReportedStatus;
	}	
	
// helper
TInt& CT_LbsAGPSHAIValidateStepRoot::AssistanceDataError()
	{
	return iAssistanceDataError;
	}
	
TInt CT_LbsAGPSHAIValidateStepRoot::AssistanceDataError() const
	{
	return iAssistanceDataError;
	}
	
TBool& CT_LbsAGPSHAIValidateStepRoot::Tracking()
	{
	return iTracking;
	}
	
TBool CT_LbsAGPSHAIValidateStepRoot::Tracking() const
	{
	return iTracking;
	}
			
void CT_LbsAGPSHAIValidateStepRoot::DoHAIPowerDropTestCase()
	{
	// test case code		
	iStartDistantRequestAndStandbyTimerOnGotAccPos = ETrue;
	StartFirstRequest();
	// start scheduler
	CActiveScheduler::Start();	
	if(!iStandbyTimerExpired)
		{
		WARN_PRINTF1(_L("<font><b>NOTE: Test finished before Inactive timer fired. Test Result is inconclusive.</b></font>\n"));						
		TESTLOG(ELogP3, "NOTE: Test finished before Inactive timer fired. Test Result is inconclusive\n");												
		}
	VerifyStandardResults();
	}
	
void CT_LbsAGPSHAIValidateStepRoot::DoHAIDataQualityStatusTestCase()
	{
	// test case code		
 	StartFirstQualityStatusTimer();
	// start scheduler
	CActiveScheduler::Start();
	}
	
/*
	Starts a 'first quality status' timer (by which time we expect module to have reported quality status)
*/
void CT_LbsAGPSHAIValidateStepRoot::StartFirstQualityStatusTimer()
	{
	TESTLOG2(ELogP2, "Starting 'First Quality Status' timer of %d microseconds\n", KExpectFirstQualityStatusTime.Int());
	__ASSERT_DEBUG(!iFirstQualityStatusTimer->IsActive(), User::Panic(_L("CT_LbsAGPSHAIValidateStepRoot::StartFirstQualityStatusTimer: Timer already running"), KErrGeneral));	
	// start first device status timer:
	iFirstQualityStatusTimer->SetTimer(KExpectFirstQualityStatusTime);
	}
	
