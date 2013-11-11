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
// @file ctlbsassdatastepcanceltracking.cpp
// This is the class implementation for the Module Information Tests
// EPOC includes.
// 
//

// LBS includes. 
#include <lbs.h>
#include <lbs/lbsnetcommon.h>
#include <lbssatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <e32math.h>

// LBS test includes.
#include "ctlbsconflictstepcanceltracking.h"
#include <lbs/test/tlbsutils.h>


/**	Test case development notes
	===========================

	It was decided to aid readablity each test class shall only support a single test case.
	This canceltracking has been produced to show how a typical conflict test case should be coded.


	Net Sim and Sequence Events
	---------------------------
	Every test case implementation will use the ctlbsconflictstep class which supports Net Sim
	interactions and a way to ensure Net Sim events and client completion events have a occured
	in the correct order. An array is used to hold the expected sequence event values, and a second
	array contains the actual events produced during the test. Once the test moves into the EWaiting
	state typically after all the client requests have been made, the event arrays are compared to
	ensure the correct events were given.

	To set the expected sequence events populate the array in the SetExpectedSeq() function.
	

	Async wappers
	-------------
	Each Lbs client async request is wappared in a active object class, to allow a number of
	outstanding async requests during the test. Any async calls you wish to make should also
	be wrapped in the same way, see CT_LbsDoX3P and CT_LbsDoPosUpdate for canceltrackings.
	These classes are constructed in the test class ConstructL, and maybe started or cancelled
	during the test at any point.
	

	Verify Position Information
	---------------------------
	A test should verify each client call which expected a location to be returned. The
	location data should be validation to ensure it's correct. See VerifyPosInfos.
*/


/**
Static Constructor
*/
CT_LbsConflictStep_canceltracking* CT_LbsConflictStep_canceltracking::New(CT_LbsConflictServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsConflictStep_canceltracking* testStep = new CT_LbsConflictStep_canceltracking(aParent);
	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
	return testStep;
	}


/**
 * Constructor
 */
CT_LbsConflictStep_canceltracking::CT_LbsConflictStep_canceltracking(CT_LbsConflictServer& aParent) : CT_LbsConflictStep(aParent)
	{
	SetTestStepName(KLbsConflictStep_canceltracking);
	}


void CT_LbsConflictStep_canceltracking::ConstructL()
	{
	// Create the base class objects.
	CT_LbsConflictStep::ConstructL();
	
	// Self locate async wrapper.
	iDoPosUpdate = CT_LbsDoPosUpdate::NewL(this);

	// X3P async wrapper.
	iDoX3P = CT_LbsDoX3P::NewL(this);
	}


/**
 * Destructor
 */
CT_LbsConflictStep_canceltracking::~CT_LbsConflictStep_canceltracking()
	{
	
	iDoPosUpdate->Cancel();
	delete iDoPosUpdate;

	iDoX3P->Cancel();
	delete iDoX3P;
	}


/** Called at the start of the test...

	Each test case MUST implement this.

	The table below describes the expcted order that the callbacks should be called
	during the test.
	
	Note that the tests deliberateley do NOT check the order that the client requests
	are completed. This is because the delivery order is not guaranteed by LBS.
	The tests will check that all client requests are completed with the correct error
	code and position at the end of the test.
*/
void CT_LbsConflictStep_canceltracking::SetExpectedSeq()
	{
	// Self locate MOLR (iState = ERegPosUpdate)
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);	// --> To Net
	
	switch (iTestCaseId)
		{
		case 11:
			{
			SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);	// --> To Net
			SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);		// --> To Net
			break;
			}
		case 12:
			{
			SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure); // --> To Net
			break;
			}
		}
	
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net

	// followed by a X3P MOLR (after X3P register iState = ERegX3P),
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net	
	// now iState = ERegX3P from ERegPosUpdate,
	
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementControlLocation);	// <-- From Net
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportLocation);	// --> To Net
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);			// <-- From Net
	// but test fails bercause not yet implemented in LBS!
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net

	// (after X3P release iState = EWaiting).

	SetVerifyAdhocEvent(EClient_Got_PosUpdate_Complete);
	SetVerifyAdhocEvent(EClient_Got_X3P_Complete);

	}



/** Called at the end of the test to verify the correct position data has been returned to the
	client.
	
	Each test case SHOULD implement a version of this.
*/
void CT_LbsConflictStep_canceltracking::VerifyPosInfos()
	{
	T_LbsUtils utils;
    RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	RPointerArray<TAny>& currPosInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* currPosInfo;
	
	// Verify both the self locate and X3P MOLR position information.
	
	// Verify entry 0 for the self locate. We expect a position containing NaNs.
	currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[0]);
	TPosition pos;
	currPosInfo->GetPosition(pos);
	if (!Math::IsNaN(pos.Latitude()))
		{
		INFO_PRINTF1(_L("Failed test, Position does not contain NANs"));

		SetTestStepResult(EFail);
		}
	if (!Math::IsNaN(pos.Longitude()))
		{
		INFO_PRINTF1(_L("Failed test, Position does not contain NANs"));

		SetTestStepResult(EFail);
		}

	// Verify entry 1 for the X3P. We expect a real location value, compare using the data
	// sent to the test APGS module.
	TPositionInfo* verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[1]);
	if (!utils.Compare_PosInfo(*verifyPosInfo, *currPosInfo))
		{
		INFO_PRINTF1(_L("Failed test, X3P position incorrect."));
		SetTestStepResult(EFail);
		}
	}


/** A standard TEF test case doTestStepL, this SHOULD only support a single test case.

	Typically the function will look much like this.
 */
TVerdict CT_LbsConflictStep_canceltracking::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsConflictStep_canceltracking::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId);
		
		// Setup the expected sequence events for the test.
		SetExpectedSeq();
				
		// Open and setup net sim.
		OpenNetSim(this);

		// Kick off the test abort and keep alive timers.
		TTimeIntervalMicroSeconds32 abortInterval(KLbsAbortPeriod);
		TTimeIntervalMicroSeconds32 keepAliveInterval(KLbsKeepAlivePeriod);

		iAbortTimer->SetTimer(abortInterval);
		iKeepAliveTimer->SetTimer(keepAliveInterval);

		// Kick off test.
		CActiveScheduler::Start();

		// Verify location data.
		VerifyPosInfos();

		// Clean up.
		CloseNetSim();
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsConflictStep_canceltracking::doTestStepL()"));

	return TestStepResult();
	}


/** NetSim callbacks given for a MoLr, which we invoke as a result of the notify position update.
*/
void CT_LbsConflictStep_canceltracking::Connected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Connected();
	
	// Kick off first pos update.
	
	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* posInfo = new(ELeave) TPositionInfo();

	T_LbsUtils utils;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

	posInfoArr.Append(posInfo);

	switch (iTestCaseId)
		{
		case 10:
		case 11:
		case 12:
			{
			
			//ExitEmergencyState();
			
			TPositionUpdateOptions optsA;
			// old TTimeIntervalMicroSeconds interval = 30 * 1000000; // tracking!
			TTimeIntervalMicroSeconds interval = 4 * 1000000; // tracking!
	
			optsA.SetUpdateInterval(interval);
			
			
			TPositionUpdateOptions tempOpts;
			// set a fairly long interval (1 minute) to ensure the 2nd request doesn't complete too quicky:
			tempOpts.SetUpdateInterval(4*1000000);
			tempOpts.SetUpdateTimeOut(30*1000000);
			//iPositioner.SetUpdateOptions(tempOpts);
			//iPositioner.GetUpdateOptions(tempOpts);	
	
			iDoPosUpdate->SetOptions(tempOpts);
			//iDoPosUpdate->GetOptions(tempOpts);
			break;
			}
		}
			
 	INFO_PRINTF1(_L("Start first tracking NPUD"));
	iDoPosUpdate->StartL(*posInfo);
	iState = EReqPosUpdate;


	
	}


void CT_LbsConflictStep_canceltracking::Disconnected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Disconnected();
	}


void CT_LbsConflictStep_canceltracking::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	// Determine and record the sequence event. A blank aData indicates a self locate
	// MOLR, otherwise we have a X3P MOLR. Also the current test state indicates which
	// type MOLR is being carried out.
	if (aData != KNullDesC)
		{
		INFO_PRINTF1(_L("Got - X3P MOLR - NetSim Notify Register Lcs MoLr - Event."));
		SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);
		
 		if (iTestCaseId == 13)
 			{
 			
 			if (aData.Compare(KTransmitTimerDestination))
				{
				INFO_PRINTF1(_L("Failed test, bad X3P register data."));
				SetTestStepResult(EFail);
				}
 			}
 		else
 			{
 			
				
			// Verify telephone number.
			if (aData.Compare(KTransmitTimerDestination))
				{
				INFO_PRINTF1(_L("Failed test, bad X3P register data."));
				SetTestStepResult(EFail);
				}
 			}
 			
		// Mark as started now it has, to ensure the self MOLR are not recorded incorrectly.
		iState = EReqX3P;
		}

	else if ((aData == KNullDesC) && (iState == EReqPosUpdate))
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Register Lcs MoLr - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);
		}
	
	}


void CT_LbsConflictStep_canceltracking::NotifyReleaseLcsMoLr(TInt aReason)
	{
	TInt expectedErr = KErrNone;

	// Determine and set sequence event.
	
	if (iDoX3PPushWhenMoLrReleased)
		{
		INFO_PRINTF1(_L("self MOLR - NetSim Notify Release Lcs MoLr - Event."));

		iDoX3PPushWhenMoLrReleased = EFalse;
		// Start X3P.
		TLbsTransmitPositionOptions transOpts;


		iDoX3P->SetOptions(transOpts);	// Set timeout value to 0, to disable the timeout.
					
		// now make a conflict occur by doing a high priority X3P
		// the next (second) NPUD should be completed with KErrHighPriorityRec
		iKErrHighPriorityRecExpected = ETrue;

		RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		TPositionInfo* posInfo = new(ELeave) TPositionInfo();
		posInfoArr.Append(posInfo);

		INFO_PRINTF1(_L("Do a X3P Push to conflict with current tracking self locate"));

		iDoX3P->StartL(KTransmitPushDestination, KTransmitPushPriority, *posInfo);
		posInfo = new(ELeave) TPositionInfo();
		posInfoArr.Append(posInfo);

		iKErrHighPriorityRecExpected = ETrue; // the next NPUD should fail as we are doing X3P PUSH 

		INFO_PRINTF1(_L("Start second tracking NPUD"));
		iDoPosUpdate->StartL(*posInfo);

		
		}
	else
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Release Lcs MoLr - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);

        if (iTestCaseId == 14)
        	{
            expectedErr = KErrPositionHighPriorityReceive;
        	}
        else if (iTestCaseId == 12)
        	{
            expectedErr = KErrCancel;
        	}
        else
        	{
        	expectedErr = KErrNone;
			}
		}
	else if(iState == EReqX3P)
		{
		INFO_PRINTF1(_L("Got - X3P MOLR - NetSim Notify Release Lcs MoLr - Event."));
		SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);
		expectedErr = KErrNone;
		iState = EWaiting; // End of test.
		}
	
	// Verify reason code.
	if (aReason != expectedErr)
		{	
		INFO_PRINTF2(_L("Failed test, bad release reason %d."), aReason);
		SetTestStepResult(EFail);
		}
	}


void CT_LbsConflictStep_canceltracking::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	T_LbsUtils utils;	
	TInt err;
	
	// Determine and record sequence event.
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Measurement Control Location - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);
		}
		
	else if(iState == EReqX3P)
		{
		INFO_PRINTF1(_L("Got - X3P MOLR - NetSim Notify Measurement Control Location - Event."));
		SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementControlLocation);
		}


	// Verify the reference position.
	TPositionInfo verifyRefPosInfo;
	
	verifyRefPosInfo.SetPosition(iRefPos);
	if (!utils.Compare_PosInfo(verifyRefPosInfo, aPosition))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		}

   
    // Verify the assistance data.	
	RLbsAssistanceDataBuilderSet& data = const_cast<RLbsAssistanceDataBuilderSet&>(aData);
	RUEPositioningGpsReferenceTimeBuilder* refTimeBuilder = NULL;
	
	data.GetDataBuilder(refTimeBuilder);

	// Create a reader from the builder's data to allow us to verify the actual
	// assistance data.
	RUEPositioningGpsReferenceTimeReader refTimeReader;
		
	TRAP(err, refTimeReader.OpenL());
	if (err == KErrNone)
		{
		refTimeReader.DataBuffer() = refTimeBuilder->DataBuffer();
		
		if (!utils.VerifySimpleAssistanceData(refTimeReader))
			{
			INFO_PRINTF1(_L("Failed test, assistance data incorrect."));
			SetTestStepResult(EFail);
			}
		refTimeReader.Close();
		}
			
	else
		{
		INFO_PRINTF2(_L("Failed test, assistance data reader err %d."), err);
		SetTestStepResult(EFail);
		}


	// TODO: Check if we can verify aQuality in any way.
	(void)aQuality;
	}


void CT_LbsConflictStep_canceltracking::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	(void)aResult;

	// Unexpected callback for this test record the sequence event to fail test.
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Release Lcs Location Notification - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
		}
		
	else
		{
		INFO_PRINTF1(_L("Got - X3P MOLR - NetSim Notify Release Lcs Location Notification - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
		}
	}


void CT_LbsConflictStep_canceltracking::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Facility Lcs MoLr Result - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
		}
	else
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Facility Lcs MoLr Result - Event."));
		SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
		}

	// Verify reason code..	
	if (aReason != KErrNone)
		{	
		INFO_PRINTF2(_L("Failed test, bad release reason %d."), aReason);
		SetTestStepResult(EFail);
		}

	// Verify the real position returned from the network, this will be the same position
	// we sent to the network as the result of the MO-LR, thus use the entry given by
	// the test module.
	T_LbsUtils utils;
	RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* verifyRealPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	
	if (!utils.Compare_PosInfo(*verifyRealPosInfo, aPosition))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
	    } 
	}


void CT_LbsConflictStep_canceltracking::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	// Determine and record sequence event.
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Measurement Report - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);
		}
		
	else if(iState == EReqX3P)
		{
		INFO_PRINTF1(_L("Got - X3P MOLR - NetSim Notify Measurement Report - Event."));
		SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportLocation);
		}

		
	// Verify the real position given to the network, this will be the same position
	// returned as the result of the MO-LR, thus use the entry given by
	// the test module.	
	T_LbsUtils utils;
    RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	TPositionInfo* verifyRealPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	
	if (!utils.Compare_PosInfo(*verifyRealPosInfo, aPosition))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		} 
    }


void CT_LbsConflictStep_canceltracking::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	// Unexpected callback for this test, record the sequence event to fail test.
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Measurement Report Request More Assistance Data - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
		}
		
	else
		{
		INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Measurement Report Request More Assistance Data - Event."));
		SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
		}
	}


void CT_LbsConflictStep_canceltracking::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF2(_L("Got - Self MOLR - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
		}
		
	else
		{
		INFO_PRINTF2(_L("Got - X3P MOLR(PUSH) - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
		SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
		}

	TInt expectedErr = KErrNone;
    if (iTestCaseId == 12)
    	{
        expectedErr = KErrCancel;
    	}

	// Verify reason code.
	if (aReason != expectedErr)
		{	
		INFO_PRINTF2(_L("Failed test, bad control failure reason %d."), aReason);
		SetTestStepResult(EFail);
		}

	
	}


/**	Notify position update callback.

	The notify position update has completed.
*/
void CT_LbsConflictStep_canceltracking::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("Got - Client Notify Update Complete - Callback Event."));
	SetCurrentSeqEvent(EClient_Got_PosUpdate_Complete);

	if (iKErrServerBusyExpected)
		{
		iKErrServerBusyExpected	 = EFalse;
			if (KErrServerBusy == aStatus.Int())
			{
			INFO_PRINTF1(_L("KErrServerBusy as expected"));
			}
		else
			{
			INFO_PRINTF2(_L("Failed test, expected pos update result KErrServerBusy but got err = %d."), aStatus.Int());
			SetTestStepResult(EFail);
			}
		}
	else
	if (iKErrHighPriorityRecExpected)
		{
		iKErrHighPriorityRecExpected = EFalse;
		if (KErrPositionHighPriorityReceive == aStatus.Int())
			{
			INFO_PRINTF1(_L("KErrPositionHighPriorityReceive as expected"));
			}
		else
			{
			INFO_PRINTF2(_L("Failed test, expected pos update result KErrorHighPriorityReceived but got err = %d."), aStatus.Int());
			SetTestStepResult(EFail);
			}
		}
	else
	if (iCancelSubState)
		{
			iCancelSubState = EFalse;
			// should be KErrCancel after a cancel
			if (KErrCancel == aStatus.Int())
				{
				INFO_PRINTF1(_L("KErrCancel as expected"));

				//iState = EReqX3P;
				// Create a posinfo and store in our shared array for later verification.
				RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
				TPositionInfo* posInfo = new(ELeave) TPositionInfo();

				posInfoArr.Append(posInfo);
		
				// Start TIMER X3P.
				TLbsTransmitPositionOptions transOpts;
		
				iDoX3P->SetOptions(transOpts);	// Set timeout value to 0, to disable the timeout.
				INFO_PRINTF1(_L("Start X3P(TIMER)"));
				iDoX3P->StartL(KTransmitTimerDestination, KTransmitTimerPriority, *posInfo);
		
				}
				else
				{
				INFO_PRINTF2(_L("Failed test, pos update result err = %d."), aStatus.Int());
				SetTestStepResult(EFail);
					
				}

		}
	else if (iState == EReqPosUpdate)
		{
		// no need to check  posItion here!
		
		// Create a posinfo and store in our shared array for later verification.
		RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		TPositionInfo* posInfo = new(ELeave) TPositionInfo();

		T_LbsUtils utils;
		utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

		posInfoArr.Append(posInfo);

		switch (iTestCaseId)
			{
			case 10:
				{
				INFO_PRINTF1(_L("Start second tracking NPUD"));
				iDoPosUpdate->StartL(*posInfo);
				
				INFO_PRINTF1(_L("Cancel tracking NPUD stops tracking!"));
				iDoPosUpdate->CancelRequest(); // stops tracking!
				iCancelSubState = ETrue;
				break;
				}
			
			case 11:
				{
				TPositionUpdateOptions optsA;
				TTimeIntervalMicroSeconds interval = 0; // stops tracking!
				optsA.SetUpdateInterval(interval);
				INFO_PRINTF1(_L("Tracking Interval = 0 stops tracking!"));
				iDoPosUpdate->SetOptions(optsA);
				
				RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
				TPositionInfo* posInfo = new(ELeave) TPositionInfo();
	
				posInfoArr.Append(posInfo);
		
				// Start X3P.
				TLbsTransmitPositionOptions transOpts;
		
				iDoX3P->SetOptions(transOpts);	// Set timeout value to 0, to disable the timeout.
				INFO_PRINTF1(_L("Start X3P(TIMER)"));
	
				
				iDoX3P->StartL(KTransmitTimerDestination, KTransmitTimerPriority, *posInfo);
		
				break;
				}
			case 12:
				{
				INFO_PRINTF1(_L("ClosePositioner - stops tracking!"));
							
				RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
				TPositionInfo* posInfo = new(ELeave) TPositionInfo();
				posInfoArr.Append(posInfo);
	
				iDoPosUpdate->ClosePositioner(); // stops tracking!
				
				// Start X3P.
				TLbsTransmitPositionOptions transOpts;
		
				iDoX3P->SetOptions(transOpts);	// Set timeout value to 0, to disable the timeout.
				
				// JCM: Note that low priority X3P should be queued and never return KErrServerBusy!
				// JCM: with the delay below the X3P succeeds!
				//INFO_PRINTF1(_L("Wait 5 seconds!!!!!!!!!!!!!!!!"));
				//User::After(5000000);
				//INFO_PRINTF1(_L("Start X3P(TIMER)"));
				
				iDoX3P->StartL(KTransmitTimerDestination, KTransmitTimerPriority, *posInfo);
		
				break;
				}
	
			}
		}
	}	
	

/** X3P transmit callback.

	The X3P transmit request has completed.
*/	
void CT_LbsConflictStep_canceltracking::MT_LbsDoX3PCallback(TInt aTransmitId, TRequestStatus& aStatus)
	{
	(void)aTransmitId;

	INFO_PRINTF1(_L("Got - Client X3P Complete - Callback Event."));
	SetCurrentSeqEvent(EClient_Got_X3P_Complete);
	

	if (KErrNone != aStatus.Int())
		{
		INFO_PRINTF2(_L("Failed test, X3P transmit request err = %d."), aStatus.Int());
		SetTestStepResult(EFail);
		//iState = EWaiting;
		}
		else
		{
		INFO_PRINTF2(_L("Test Passed, err = %d."), aStatus.Int());
		//iState =  EReqX3P;	
		}
		
	}
		

