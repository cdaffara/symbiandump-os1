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
// @file ctlbsassdatastepcanceltrackingconflictb.cpp
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
#include "ctlbsconflictstepcanceltrackingconflictb.h"
#include <lbs/test/tlbsutils.h>


/**	Test case development notes
	===========================

	It was decided to aid readablity each test class shall only support a single test case.
	This canceltrackingconflict has been produced to show how a typical conflict test case should be coded.


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
	be wrapped in the same way, see CT_LbsDoX3P and CT_LbsDoPosUpdate for canceltrackingconflicts.
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
CT_LbsConflictStep_canceltrackingconflictb* CT_LbsConflictStep_canceltrackingconflictb::New(CT_LbsConflictServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsConflictStep_canceltrackingconflictb* testStep = new CT_LbsConflictStep_canceltrackingconflictb(aParent);
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
CT_LbsConflictStep_canceltrackingconflictb::CT_LbsConflictStep_canceltrackingconflictb(CT_LbsConflictServer& aParent) : CT_LbsConflictStep(aParent)
	{
	SetTestStepName(KLbsConflictStep_canceltrackingconflictb);
	}


void CT_LbsConflictStep_canceltrackingconflictb::ConstructL()
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
CT_LbsConflictStep_canceltrackingconflictb::~CT_LbsConflictStep_canceltrackingconflictb()
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

//!@SYMTestCaseID	LBS-Conflict-0014

//!	1.	Client sets update interval say, to 4 seconds - thus tracking is enabled.
//!	2.	Client (the test) does a self locate request using NotifyPositionUpdate		
//!	3.	Client waits for request to complete (with KerrNone and a valid position).
//!	4   Client issues a X3P (PUSH) request (in MoLr release callback) then does a 
//!		second tracking NPDU - conflict!
//!	5	Client waits for second self locate request to complete with KErrServerBusy
//!	6	Client waits for X3P (PUSH) to complete with KErrNone
//!	7.	Client does a X3P (TIMER) request in RunL of X3P (PUSH).
//!	8.	Client waits for X3P (TIMER) request to complete with KErrNone


void CT_LbsConflictStep_canceltrackingconflictb::SetExpectedSeq()
	{
	// Self locate MOLR (iState = ERegPosUpdate)
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);	// --> To Net
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);	// <-- To Net
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);		// <-- To Net
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// <-- To Net

	// X3P Push (iState = EReqX3PPush) 
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
	// do a second tracking self locate in above callback - conflict KErrServerBusy!
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementControlLocation);	// <-- From Net
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportLocation);	// --> To Net
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);		// <-- To Net
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net
	
	// followed by a X3P Timer (after X3P Timer register iState = ERegX3PTimer),
	// X3P(Timer) should be queued and handled after the X3P(Push)
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);			// --> To Net	
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementControlLocation);	// <-- From Net
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportLocation);	// --> To Net
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);		// <-- To Net
	SetVerifySeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net
	// both X3P's should complete with KerrNone

	// (after X3P release iState = EWaiting).

	SetVerifyAdhocEvent(EClient_Got_PosUpdate_Complete);
	SetVerifyAdhocEvent(EClient_Got_X3P_Complete);
	// We have 2 self locate requests and 2 x3p requests so must record them both
	SetVerifyAdhocEvent(EClient_Got_PosUpdate_Complete);
	SetVerifyAdhocEvent(EClient_Got_X3P_Complete);

	}



/** Called at the end of the test to verify the correct position data has been returned to the
	client.
	
	Each test case SHOULD implement a version of this.
*/
void CT_LbsConflictStep_canceltrackingconflictb::VerifyPosInfos()
	{
	T_LbsUtils utils;
    RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
	RPointerArray<TAny>& currPosInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* currPosInfo;
	
	// Verify the received self locate and X3P MOLR positions.
	// position [0] is used to hold the first NPUD position result
	// position [1] is used to hold the first XP3, PUSH position result
	// position [2] is used to second NPUD result (was KErrServerBusy)
	// position [3] is used to hold the X3P Timer position result

	// position [0] is used to hold the first NPUD position result
	TPositionInfo* verifyPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[0]);
	// It was a new client so verify got the reference position.
	TPositionInfo verifyRefPosInfo;
	
	verifyRefPosInfo.SetPosition(iRefPos);
	if (!utils.Compare_PosInfo(verifyRefPosInfo, *currPosInfo))
		{
		INFO_PRINTF1(_L("Failed test, position incorrect."));
		SetTestStepResult(EFail);
		}

	// position [1] is used to hold the X3P Push position result
	currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[1]);
	if (!utils.Compare_PosInfo(*verifyPosInfo, *currPosInfo))
		{
		INFO_PRINTF1(_L("Failed test, X3P Push position incorrect."));
		SetTestStepResult(EFail);
		}
	
	// position [2] is used to second NPUD result (was KErrServerBusy)
	// We expect a position containing NaNs.
	currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[2]);
	TPosition pos;
	currPosInfo->GetPosition(pos);
	if (!Math::IsNaN(pos.Latitude()))
		{
		INFO_PRINTF1(_L("Failed test, Second NPUD Position does not contain NANs"));
		INFO_PRINTF1(_L("It should have been completed with KErrServerBusy"));

		SetTestStepResult(EFail);
		}
	if (!Math::IsNaN(pos.Longitude()))
		{
		INFO_PRINTF1(_L("Failed test, Second NPUD Position does not contain NANs"));
		INFO_PRINTF1(_L("It should have been completed with KErrServerBusy"));

		SetTestStepResult(EFail);
		}

		
	// position [3] is used to hold the X3P Timer position result
	currPosInfo = reinterpret_cast<TPositionInfo*>(currPosInfoArr[3]);
	if (!utils.Compare_PosInfo(*verifyPosInfo, *currPosInfo))
		{
		INFO_PRINTF1(_L("Failed test, X3P Timerposition incorrect."));
		SetTestStepResult(EFail);
		}
	}


/** A standard TEF test case doTestStepL, this SHOULD only support a single test case.

	Typically the function will look much like this.
 */
TVerdict CT_LbsConflictStep_canceltrackingconflictb::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsConflictStep_canceltrackingconflict::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
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

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsConflictStep_canceltrackingconflict::doTestStepL()"));

	return TestStepResult();
	}


/** NetSim callbacks given for a MoLr, which we invoke as a result of the notify position update.
*/
void CT_LbsConflictStep_canceltrackingconflictb::Connected()
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

	// Kick off pos update.
			
	TPositionUpdateOptions tempOpts;

	// set a fairly long interval to ensure the 2nd request doesn't complete too quicky:
	tempOpts.SetUpdateInterval(4*1000000);
	tempOpts.SetUpdateTimeOut(2*1000000);

	iDoPosUpdate->SetOptions(tempOpts);
			
 	INFO_PRINTF1(_L("Start first tracking NPUD"));
	iDoPosUpdate->StartL(*posInfo);
	iState = EReqPosUpdate;
	
	}


void CT_LbsConflictStep_canceltrackingconflictb::Disconnected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Disconnected();
	}


void CT_LbsConflictStep_canceltrackingconflictb::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	// Determine and record the sequence event. A blank aData indicates a self locate
	// MOLR, otherwise we have a X3P MOLR. Also the current test state indicates which
	// type MOLR is being carried out.
	if (aData != KNullDesC)
		{
		if (iX3PPush)
			{
			INFO_PRINTF1(_L("Got - X3P PUSH MOLR - NetSim Notify Register Lcs MoLr - Event."));
			SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);
			}
		else
			{
			INFO_PRINTF1(_L("Got - X3P MOLR TIMER- NetSim Notify Register Lcs MoLr - Event."));
			SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);
			}	

 		if (iX3PPush)
 			{
 			
 			if (aData.Compare(KTransmitPushDestination))
				{
				INFO_PRINTF1(_L("Failed test, bad X3P Push register data."));
				SetTestStepResult(EFail);
				}
 			}
 		else
 			{
			if (aData.Compare(KTransmitTimerDestination))
				{
				INFO_PRINTF1(_L("Failed test, bad X3P Timer register data."));
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


void CT_LbsConflictStep_canceltrackingconflictb::NotifyReleaseLcsMoLr(TInt aReason)
	{
	TInt expectedErr = KErrNone;

	// Determine and set sequence event.
	
	if (iDoX3PPushWhenMoLrReleased)
		{
		INFO_PRINTF1(_L("self MOLR - NetSim Notify Release Lcs MoLr - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);

		iDoX3PPushWhenMoLrReleased = EFalse;
		// Start X3P.
		TLbsTransmitPositionOptions transOpts;


		iDoX3P->SetOptions(transOpts);	// Set timeout value to 0, to disable the timeout.
					
		// now make a conflict occur by doing a high priority X3P
		// the next (second) NPUD should be completed with KErrServerBusy
		iKErrServerBusyExpected = ETrue;

		RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		TPositionInfo* posInfo = new(ELeave) TPositionInfo();
		posInfoArr.Append(posInfo);

		INFO_PRINTF1(_L("Do a X3P Push to conflict with current tracking self locate"));
		iX3PPush =ETrue;
		iDoX3P->StartL(KTransmitPushDestination, KTransmitPushPriority, *posInfo);
		iDoAX3PTimer = ETrue;	// when PUSH X3P completes
		
		iKErrServerBusyExpected = ETrue; // the next NPUD should fail as we are doing X3P PUSH 

		INFO_PRINTF1(_L("Start second tracking NPUD once X3P has measurement control"));
		
		//iDoSewcondNPUD = ETrue;
		

		//RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		//posInfo = new(ELeave) TPositionInfo();
		//posInfoArr.Append(posInfo);
		//iDoPosUpdate->StartL(*posInfo);

		
		}
	else
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Release Lcs MoLr - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);

		TInt testCaseId = 0;
		GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId);
		expectedErr = KErrPositionHighPriorityReceive;
   		}
	else 
	if(iState == EReqX3P)
 		{
 		if (iX3PPush)
 			{
 			INFO_PRINTF1(_L("Got - X3P MOLR PUSH- NetSim Notify Release Lcs MoLr - Event."));
			SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);
			iX3PPush =EFalse;	// Timer X3P follows ...
 			}
 		else
 			{
 			INFO_PRINTF1(_L("Got - X3P MOLR TIMERS - NetsSm Notify Release Lcs MoLr - Event."));
			SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);
			// Now we have had the last callback
			// but don't end the test until
			// until/unless had 2nd NPUD has finshed
			iHadLastCallBack = ETrue;
				
			if (!iKErrServerBusyExpected)
				{
				iState = EWaiting;
				}
 	 		}
 		
 		expectedErr = KErrNone;

		}
	
	// Verify reason code.
	if (aReason != expectedErr)
		{	
		INFO_PRINTF2(_L("Failed test, bad release reason %d."), aReason);
		SetTestStepResult(EFail);
		}
	}


void CT_LbsConflictStep_canceltrackingconflictb::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
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
		if(iX3PPush)
			{
			INFO_PRINTF1(_L("Got - X3P MOLR PUSH- NetSim Notify Measurement Control Location - Event."));
			SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementControlLocation);
			
			RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
			TPositionInfo* posInfo = new(ELeave) TPositionInfo();
			posInfoArr.Append(posInfo);
			iDoPosUpdate->StartL(*posInfo);

			}
		else
			{
			INFO_PRINTF1(_L("Got - X3P MOLR TIMER- NetSim Notify Measurement Control Location - Event."));
			SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementControlLocation);
			}
		
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


void CT_LbsConflictStep_canceltrackingconflictb::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	(void)aResult;

	// Unexpected callback for this test, record the sequence event to fail test.
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Release Lcs Location Notification - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
		}
		
	else
		{
		if(iX3PPush)
			{
			INFO_PRINTF1(_L("Got - X3P MOLR - NetSim Notify Release Lcs Location Notification - Event."));
			//SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
			}
		else
			{
			INFO_PRINTF1(_L("Got - X3P MOLR - NetSim Notify Release Lcs Location Notification - Event."));
			//SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
			}
		}
	}


void CT_LbsConflictStep_canceltrackingconflictb::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Facility Lcs MoLr Result - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
		}
	else
		{
		if(iX3PPush)
			{
			INFO_PRINTF1(_L("Got - Self MOLR PUSH- NetSim Notify Facility Lcs MoLr Result - Event."));
			SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
			}
			else
			{
			INFO_PRINTF1(_L("Got - Self MOLR TIMER- NetSim Notify Facility Lcs MoLr Result - Event."));
			SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
			}
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


void CT_LbsConflictStep_canceltrackingconflictb::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	// Determine and record sequence event.
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF1(_L("Got - Self MOLR - NetSim Notify Measurement Report - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation);
		}
		
	else if(iState == EReqX3P)
		{
		if(iX3PPush)
			{
			INFO_PRINTF1(_L("Got - X3P MOLR PUSH - NetSim Notify Measurement Report - Event."));
			SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportLocation);
			}
		else
			{
			INFO_PRINTF1(_L("Got - X3P MOLR TIMER- NetSim Notify Measurement Report - Event."));
			SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportLocation);
			}
			
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


void CT_LbsConflictStep_canceltrackingconflictb::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
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
		if(iX3PPush)
			{
			INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Measurement Report Request More Assistance Data - Event."));
			SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
			}
		else
			{
			INFO_PRINTF1(_L("Got - X3P(TIMER) MOLR - NetSim Notify Measurement Report Request More Assistance Data - Event."));
			SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
			}
				
		}
	}


void CT_LbsConflictStep_canceltrackingconflictb::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	// Unexpected callback for this test, record the sequence event to fail test.
	if (iState == EReqPosUpdate)
		{
		INFO_PRINTF2(_L("Got - Self MOLR - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
		}
		
	else
		{
		if(iX3PPush)
			{
			INFO_PRINTF2(_L("Got - X3P MOLR(PUSH) - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
			SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
			}
		else
			{
			INFO_PRINTF2(_L("Got - X3P MOLR(TIMER) - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
			SetCurrentSeqEvent(EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
	
			}
		}
	}


/**	Notify position update callback.

	The notify position update has completed.
*/
void CT_LbsConflictStep_canceltrackingconflictb::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	INFO_PRINTF1(_L("Got - Client Notify Update Complete - Callback Event."));
	SetCurrentSeqEvent(EClient_Got_PosUpdate_Complete);

	if (iKErrServerBusyExpected)
		{
		iKErrServerBusyExpected = EFalse;
		if (KErrServerBusy == aStatus.Int())
			{
			INFO_PRINTF1(_L("KErrServerBusy as expected"));
			}
		else
			{
			INFO_PRINTF2(_L("Failed test, expected pos update result KErrServerBusy but got err = %d."), aStatus.Int());
			SetTestStepResult(EFail);
			}
		if (iHadLastCallBack)
			{
			iState = EWaiting; // all done, now check positions received
			}
		}


	else if (iState == EReqPosUpdate)
		{
		// we now have the first tracking NPUD result!
		
		// in the MoLr release - do a X3P (PUSH) and then the 
		// the second NPUD should be completed with KErrServerBusy
		iDoX3PPushWhenMoLrReleased = ETrue;
		

		}
	}	
	

/** X3P transmit callback.

	The X3P transmit request has completed.
*/	
void CT_LbsConflictStep_canceltrackingconflictb::MT_LbsDoX3PCallback(TInt aTransmitId, TRequestStatus& aStatus)
	{
	(void)aTransmitId;

	INFO_PRINTF1(_L("Got - Client X3P Complete - Callback Event."));
	SetCurrentSeqEvent(EClient_Got_X3P_Complete);
	
	if (KErrNone != aStatus.Int())
		{
		INFO_PRINTF2(_L("Failed test, X3P transmit request err = %d."), aStatus.Int());
		SetTestStepResult(EFail);

		}
		else
		{
		INFO_PRINTF2(_L("Test Passed, err = %d."), aStatus.Int());
	
		}
		
		
	if (iDoAX3PTimer)
		{
		iDoAX3PTimer = EFalse;
		T_LbsUtils utils;
		RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		TPositionInfo* posInfo = new(ELeave) TPositionInfo();
		posInfoArr.Append(posInfo); // position [3] is used to hold the X3P Timer result

		iState = EReqX3P;
		//iX3PPush = EFalse; // its a X3P Timer! 
		
		INFO_PRINTF1(_L("Start X3P Timer"));
		iDoX3P->StartL(KTransmitTimerDestination, KTransmitTimerPriority, *posInfo);
		}
	}
		

