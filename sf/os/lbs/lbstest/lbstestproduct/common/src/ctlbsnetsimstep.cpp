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
// @file ctlbsnetsimstep.cpp
// This is the class implementation for the LBS Test Step Base
// 
//

#include "ctlbsnetsimstep.h"


/**
 * Constructor
 */
CT_LbsNetSimStep::CT_LbsNetSimStep() : iState(EConnecting)
	{
	}


/**
 * If parameter is supplied, it will cause the test to continue running for this period after all expected flags have been set
 */ 
void CT_LbsNetSimStep::ConstructL(TTimeIntervalMicroSeconds32 aHaltTimerInterval)
	{
	// Create the keep alive timer.
	iKeepAliveTimer = CT_LbsTimerUtils::NewL(this, KKeepAliveTimerId);		
	// Create the abort timer.
	iAbortTimer = CT_LbsTimerUtils::NewL(this, KAbortTimerId);
		
	iExtendedTimerInterval = aHaltTimerInterval;
	}

/**
 * Can be used to set the extended time interval if not known at construct time.
 * // This will cause the test to continue running for this period after all expected flags have been set
 */
void CT_LbsNetSimStep::SetExtendedTimerInterval(TTimeIntervalMicroSeconds32 aHaltTimerInterval)
	{
	iExtendedTimerInterval = aHaltTimerInterval;	
	}
	
	
CT_LbsNetSimStep::~CT_LbsNetSimStep()
	{
	iKeepAliveTimer->Cancel();
	delete iKeepAliveTimer;
	iAbortTimer->Cancel();
	delete iAbortTimer;
	
	}

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsNetSimStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsNetSimStep::doTestStepPreambleL()"));
	SetTestStepResult(EPass);

	_LIT(KExpectedApiBehaviour, "expected_api_behaviour");
	TInt expectedApiBehaviour;
	if(GetIntFromConfig(ConfigSection(), KExpectedApiBehaviour, expectedApiBehaviour))
		{
		iExpectedApiBehaviour = static_cast<TExpectedApiBehaviour>(expectedApiBehaviour);;
		}
	else
		{
		iExpectedApiBehaviour = EApiVariant1;
		}
	
	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsNetSimStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsNetSimStep::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


/** Used to mark each callback that has fired.
*/
void CT_LbsNetSimStep::SetCallbackFlag(TLbsCallbackFlags aCallbackFlag)
	{
//	INFO_PRINTF2(_L("CT_LbsNetSimStep::SetCallbackFlag: setting callback flag 0x%x"), aCallbackFlag);
	iCallbackFlags |= aCallbackFlag;
	}


/** Used to determine which callback or callbacks have been fired.
*/
TBool CT_LbsNetSimStep::TestCallbackFlags(TLbsCallbackFlags aExpectedFlags)
	{
	if (iCallbackFlags == aExpectedFlags)
		{
		return ETrue;
		}
	return EFalse;
	}


/** Keep alive timer callback.

	Check the state machine to determine when to halt the test. Once all the callbacks
	have been received the test can end.

	We have to call async functions from here also, as the callbacks are executed in another thread.
*/
void CT_LbsNetSimStep::HandleTimerL(TInt aTimerId, const TTime& aTargetTime)
	{
	// Not used.
	(void)aTimerId;
	(void)aTargetTime;

	if(KAbortTimerId == aTimerId)
		{
		INFO_PRINTF1(_L("Abort timer fired"));
		iState = EAborted;
		iKeepAliveTimer->Cancel();
		CActiveScheduler::Stop();
		}
	else
		{	
		// Check for test finish.
		
		ASSERT(aTimerId == KKeepAliveTimerId);
		// We stop test from here when in the correct state and, there should not be anything outstanding.
		if (iState == EWaiting && TestCallbackFlags(iFlagsToHaltOn))
			{
			if(iExtendedTimerInterval.Int())	// we need an extended timer
				{
				INFO_PRINTF1(_L("Starting extended timer"));
				iState = EExtendedWaiting;
				iKeepAliveTimer->SetTimer(iExtendedTimerInterval);
				}
			else
				{
				iState = EDone;
				iAbortTimer->Cancel();	// note: this is ok since it will do nothing if timer never started
				CActiveScheduler::Stop();
				}
			}
		// expected callbacks have all fired, but waiting for an extended period of time before stopping
		else if(iState == EExtendedWaiting)
			{
			INFO_PRINTF1(_L("Extended timer fired"));
			iState = EDone;
			iAbortTimer->Cancel();
			CActiveScheduler::Stop();
			}
		// Keep going, still waiting for callbacks.	
		else
			{
			TTimeIntervalMicroSeconds32 interval(KLbsKeepAlivePeriod);
				
			iKeepAliveTimer->SetTimer(interval);
			}
		}
	}


/** NetSim callbacks.
*/
void CT_LbsNetSimStep::Connected()
	{
	INFO_PRINTF1(_L("Got - NetSim Connect - Callback Event."));
	
	SetCallbackFlag(KLbsCallback_NetSim_Got_Connect);
	
	iState = EWaiting;
	}


void CT_LbsNetSimStep::Disconnected()
	{
	INFO_PRINTF1(_L("Got - NetSim Notify Disconnected - Callback Event."));
	}


void CT_LbsNetSimStep::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	
	INFO_PRINTF1(_L("Got - NetSim Notify Register Lcs MoLr - Callback Event."));
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr);
	}


void CT_LbsNetSimStep::NotifyReleaseLcsMoLr(TInt aReason)
	{
	(void)aReason;
		
	INFO_PRINTF1(_L("Got - NetSim Notify Release Lcs MoLr - Callback Event."));
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr);
	}


void CT_LbsNetSimStep::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	
	INFO_PRINTF1(_L("Got - NetSim Notify Measurement Control Location - Callback Event."));
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation);
	}


void CT_LbsNetSimStep::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	(void)aResult;

	INFO_PRINTF1(_L("Got - NetSim Notify Release Lcs Location Notification - Callback Event."));
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyReleaseLcsLocationNotification);
	}


void CT_LbsNetSimStep::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	(void)aReason;
	(void)aPosition;

	INFO_PRINTF1(_L("Got - NetSim Notify Facility Lcs MoLr Result - Callback Event."));
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult);
	}


void CT_LbsNetSimStep::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	(void)aPosition;

	INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report - Callback Event."));
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation);
	}


void CT_LbsNetSimStep::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report Request More Assistance Data - Callback Event."));
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
	}


void CT_LbsNetSimStep::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	INFO_PRINTF2(_L("Got - Net Sim Notify Measurement Report Control Failure - Callback Event. Reason = %d"), aReason);
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure);
	}

void CT_LbsNetSimStep::NotifyError(MLbsNetSimTestObserver::EFunction aFunction, int aError)
	{
	INFO_PRINTF3(_L("Got - Net Sim Notify Error - Callback Event. Function = %d, Error = %d"), aFunction, aError);
	SetCallbackFlag(KLbsCallback_NetSim_Got_NotifyError);
	}

void CT_LbsNetSimStep::ProcessMeasurementControlLocationError(TInt aError)
	{
	INFO_PRINTF2(_L("Got - Net Sim Process Measurement Control Location Error - Callback Event. Reason = %d"), aError);
	SetCallbackFlag(KLbsCallback_NetSim_Got_ProcessMeasurementControlLocationError);	
	}
