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
// @file ctlbsconflictstep.cpp
// This is the class implementation for the LBS Conflict Test Step Base
// 
//

#include "ctlbsconflictstep.h"


const TInt KAssistanceDataProviderPluginUidValue = 0x10281D77; // Test data provider only.

#define REFPOS_LAT				2
#define REFPOS_LONG				3
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	100
#define REFPOS_VERT_ACCURACY	100

#define NETWORK_MAX_FIX_TIME	5000000
#define NETWORK_HORZ_ACCURACY	10
#define NETWORK_VERT_ACCURACY	10




CT_LbsConflictStep::~CT_LbsConflictStep()
	{
	iKeepAliveTimer->Cancel();
	delete iKeepAliveTimer;
	
	iAbortTimer->Cancel();
	delete iAbortTimer;
	
	// Clear arrays.
	iVerifySeqArr.Close();
	iVerifyAdhocArr.Close();
	iCurrentSeqArr.Close();
	}


CT_LbsConflictStep::CT_LbsConflictStep(CT_LbsConflictServer& aParent) : iState(EConnecting), iParent(aParent)
	{
	}

void CT_LbsConflictStep::ConstructL()
	{
	// Create the keep alive and abort timers.
	iKeepAliveTimer = CT_LbsTimerUtils::NewL(this, KKeepAliveTimerId);		
	iAbortTimer = CT_LbsTimerUtils::NewL(this, KAbortTimerId);
	}



/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsConflictStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsNetSimStep::doTestStepPreambleL()"));
	SetTestStepResult(EPass);

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsConflictStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsNetSimStep::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


void CT_LbsConflictStep::SetVerifySeqEvent(TInt aSeqEventId)
	{
	iVerifySeqArr.Append(aSeqEventId);
	}

void CT_LbsConflictStep::SetVerifyAdhocEvent(TInt aAdhocEventId)
	{
	iVerifyAdhocArr.Append(aAdhocEventId);
	}

void CT_LbsConflictStep::SetCurrentSeqEvent(TInt aSeqEventId)
	{
	
	iCurrentSeqArr.Append(aSeqEventId);
	}

TBool CT_LbsConflictStep::VerifySeqEvents()
	{
	// Only compare the sequence arrays once all the events have been fired.
	if ((iVerifySeqArr.Count() + iVerifyAdhocArr.Count()) == iCurrentSeqArr.Count())
		{
		TInt verify;
		TInt curr;

		for (TInt i=0,j = 0; i < iCurrentSeqArr.Count(); ++i)
			{
			// i is used as an index into the 'actual events' array,
			// j is used as an index into the 'expected sequential events' array 
			
			curr = iCurrentSeqArr[i];
			verify = KErrNotFound;
			
			if (j < iVerifySeqArr.Count())
				{
				// only get the next element from the 'expected sequential events' array if one exists 
				verify = iVerifySeqArr[j];
				}
							
			if (verify == curr)
				{
				// sussessful comparison between iCurrentSeqArr[i] and iVerifySeqArr[j] so move on to next element
				++j;
				}
			else if (KErrNotFound == iVerifyAdhocArr.Find(curr)) // see if 'actual events' array element is an adhoc event 
				{
				// current 'actual events' array element not in either of the 'expected events' arrays so return failure
				return EFalse;
				}
			// at this point the current 'actual events' array element is an adhoc event so move on to next element
			}
		}
	
	else
		return EFalse;
	
	return ETrue;
	}


/** Keep alive timer callback.

	Check the state machine to determine when to halt the test. Once all the callbacks
	have been received the test can end.

	We have to call async functions from here also, as the callbacks are executed in another thread.
*/
void CT_LbsConflictStep::HandleTimerL(TInt aTimerId, const TTime& aTargetTime)
	{
	// Not used.
	(void)aTargetTime;


	// Has the test timed out.
	if (KAbortTimerId == aTimerId)
		{
		INFO_PRINTF1(_L("Test failed, test abort timer fired, not all callbacks recviced."));
		SetTestStepResult(EFail);

		iState = EAborted;
		iKeepAliveTimer->Cancel();
		CActiveScheduler::Stop();
		}
	
	// Check for test finish.
	else if (KKeepAliveTimerId == aTimerId)
		{
		// We stop test from here when in the correct state and, there should not be anything outstanding.
		if (iState == EWaiting && VerifySeqEvents())
			{
			iAbortTimer->Cancel();
		
			iState = EDone;
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


TInt CT_LbsConflictStep::OpenNetSim(MLbsNetSimTestObserver* aObserver)
	{
	// Connect to net sim.
	TInt err = iNetSim.ConnectL(aObserver);
	if (err)
		{
		return err;
		}


	// Set the reference position, which also be used for any required verification.
	iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	iRefPos.SetCurrentTime();

	if (!iNetSim.SetReferenceLocation(iRefPos))
		{
		iNetSim.Close();

		return KErrGeneral;
		}	
		

	// Set the position quality required by the network.
	TTimeIntervalMicroSeconds maxFixTime(NETWORK_MAX_FIX_TIME);
	TLbsNetPosRequestQuality netPosQuality;

	netPosQuality.SetMaxFixTime(maxFixTime);
	netPosQuality.SetMinHorizontalAccuracy(NETWORK_HORZ_ACCURACY);
	netPosQuality.SetMinVerticalAccuracy(NETWORK_VERT_ACCURACY);
	
	if (!iNetSim.SetQuality(netPosQuality))
		{
		iNetSim.Close();

		return KErrGeneral;
	    }


	// Set plugin to use.
	TUid pluginUid = TUid::Uid(KAssistanceDataProviderPluginUidValue);
	if (!iNetSim.SetAssistanceDataProvider(pluginUid))
		{
		iNetSim.Close();

		return KErrGeneral;
		}
		
	return err;
	}


void CT_LbsConflictStep::CloseNetSim()
	{
	iNetSim.Close();	
	}	


void CT_LbsConflictStep::EnterEmergencyState()
	{
	iNetSim.SetEmergenyStatus(ETrue);	
	}	

void CT_LbsConflictStep::ExitEmergencyState()
	{
	iNetSim.SetEmergenyStatus(EFalse);	
	}	

void CT_LbsConflictStep::StartNetworkPrivacyRequest()
	{
	TLbsNetPosRequestPrivacy type;
	type.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
	type.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	
	TLbsExternalRequestInfo extReqInfo;
	
	_LIT8(KRequesterId, "08457484950");
	_LIT8(KClientName, "thetrainline.com");
	_LIT8(KClientExternalId, "test_externalclientid");

	extReqInfo.SetRequesterId(KRequesterId);
	extReqInfo.SetClientName(KClientName);
	extReqInfo.SetClientExternalId(KClientExternalId);

	iNetSim.StartNetworkPrivacyRequest(type, extReqInfo);
	}
	
/** NetSim callbacks.
*/
void CT_LbsConflictStep::Connected()
	{
	INFO_PRINTF1(_L("Got - NetSim Connect - Callback Event."));
	}


void CT_LbsConflictStep::Disconnected()
	{
	INFO_PRINTF1(_L("Got - NetSim Notify Disconnected - Callback Event."));
	}


void CT_LbsConflictStep::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	
	INFO_PRINTF1(_L("Got - NetSim Notify Register Lcs MoLr - Callback Event."));
	}


void CT_LbsConflictStep::NotifyReleaseLcsMoLr(TInt aReason)
	{
	(void)aReason;
		
	INFO_PRINTF1(_L("Got - NetSim Notify Release Lcs MoLr - Callback Event."));
	}


void CT_LbsConflictStep::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	(void)aPosition;
	(void)aData;
	(void)aQuality;
	
	INFO_PRINTF1(_L("Got - NetSim Notify Measurement Control Location - Callback Event."));
	}


void CT_LbsConflictStep::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	(void)aResult;

	INFO_PRINTF1(_L("Got - NetSim Notify Release Lcs Location Notification - Callback Event."));
	}


void CT_LbsConflictStep::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	(void)aReason;
	(void)aPosition;

	INFO_PRINTF1(_L("Got - NetSim Notify Facility Lcs MoLr Result - Callback Event."));
	}


void CT_LbsConflictStep::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	(void)aPosition;

	INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report - Callback Event."));
	}


void CT_LbsConflictStep::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	INFO_PRINTF1(_L("Got - Net Sim Notify Measurement Report Request More Assistance Data - Callback Event."));
	}


void CT_LbsConflictStep::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	INFO_PRINTF2(_L("Got - Net Sim Notify Measurement Report Control Failure - Callback Event. Reason = %d"), aReason);
	}


void CT_LbsConflictStep::NotifyError(MLbsNetSimTestObserver::EFunction aFunction, int aError)
	{
	INFO_PRINTF3(_L("Got - Net Sim Notify Error - Callback Event. Function = %d, Error = %d"), aFunction, aError);

	}


void CT_LbsConflictStep::ProcessMeasurementControlLocationError(TInt aError)
	{
	INFO_PRINTF2(_L("Got - Net Sim Process Measurement Control Location Error - Callback Event. Reason = %d"), aError);
	}

