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
// @file ctlbsassdatastepmain.cpp
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
#include "ctlbsconflictstepx3pmenupush.h"
#include <lbs/test/tlbsutils.h>


/**	Test case development notes
	===========================

	It was decided to aid readablity each test class shall only support a single test case.
	This example has been produced to show how a typical conflict test case should be coded.


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
	be wrapped in the same way, see CT_LbsDoX3P and CT_LbsDoPosUpdate for examples.
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
CT_LbsConflictStep_X3PMenuPush* CT_LbsConflictStep_X3PMenuPush::New(CT_LbsConflictServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsConflictStep_X3PMenuPush* testStep = new CT_LbsConflictStep_X3PMenuPush(aParent);
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
CT_LbsConflictStep_X3PMenuPush::CT_LbsConflictStep_X3PMenuPush(CT_LbsConflictServer& aParent) : CT_LbsConflictStep(aParent)
	{
	SetTestStepName(KLbsConflictStep_X3PMenuPush);
	}


void CT_LbsConflictStep_X3PMenuPush::ConstructL()
	{
	// Create the base class objects.
	CT_LbsConflictStep::ConstructL();
	
	// X3P async wrappers.
	iDoX3PMenu = CT_LbsDoX3P::NewL(this, KLbsConflictX3PMenuId);	
	iDoX3PPush = CT_LbsDoX3P::NewL(this, KLbsConflictX3PPushId);
	}


/**
 * Destructor
 */
CT_LbsConflictStep_X3PMenuPush::~CT_LbsConflictStep_X3PMenuPush()
	{
	iDoX3PMenu->Cancel();
	delete iDoX3PMenu;

	iDoX3PPush->Cancel();
	delete iDoX3PPush;
	}


/** Expected sequence events.
*/
void CT_LbsConflictStep_X3PMenuPush::SetExpectedSeq()
	{
	// X3P Menu (iState = EReqX3pMenu)
	SetVerifySeqEvent(EX3P_MENU_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net
	SetVerifySeqEvent(EX3P_MENU_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net
	
	// followed by a X3P Push (after X3P Push register iState = ERegX3PPush),
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);				// --> To Net	
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementControlLocation);	// <-- From Net
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportLocation);	// --> To Net
	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);

	SetVerifySeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);				// --> To Net
	
	// (after X3P Push release iState = EWaiting).

	SetVerifyAdhocEvent(EClient_Got_X3P_Complete);
	SetVerifyAdhocEvent(EClient_Got_X3P_Complete);

	}



/** Called at the end of the test to verify the correct position data has been returned to the
	client.
	
	Each test case SHOULD implement a version of this.
*/
void CT_LbsConflictStep_X3PMenuPush::VerifyPosInfos()
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


/** Helper function to start X3Ps.
*/
void CT_LbsConflictStep_X3PMenuPush::StartX3PL(TInt aTransmitId)
	{
	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* posInfo = new(ELeave) TPositionInfo();

	//T_LbsUtils utils;
	// utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

	posInfoArr.Append(posInfo);


	// Start X3P,
	TLbsTransmitPositionOptions transOpts;	// Set timeout value to 0 (done by the default constructor), to disable the timeout.

	// Menu.	
	if (aTransmitId == KLbsConflictX3PMenuId)
		{	
		iDoX3PMenu->SetOptions(transOpts);
		iDoX3PMenu->StartL(KTransmitMenuDestination, KTransmitMenuPriority, *posInfo);
		}
		
	// Push.
	else if (aTransmitId == KLbsConflictX3PPushId)
		{
		iDoX3PPush->SetOptions(transOpts);
		iDoX3PPush->StartL(KTransmitPushDestination, KTransmitPushPriority, *posInfo);
		}
		
	else
		{
		User::LeaveIfError(KErrNotSupported);
		}
	}


/** Run the test.
 */
TVerdict CT_LbsConflictStep_X3PMenuPush::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsConflictStep_X3PMenuPush::doTestStepL()"));

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

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsConflictStep_X3PMenuPush::doTestStepL()"));

	return TestStepResult();
	}


/** NetSim callbacks given for a MoLr, which we invoke as a result of the notify position update.
*/
void CT_LbsConflictStep_X3PMenuPush::Connected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Connected();
	
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;

	T_LbsUtils utils;
	utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.

	// Kick off first X3P.
	StartX3PL(KLbsConflictX3PMenuId);
	iState = EReqX3PMenu;
	}


void CT_LbsConflictStep_X3PMenuPush::Disconnected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Disconnected();
	}


void CT_LbsConflictStep_X3PMenuPush::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	// Determine and record the sequence event. A blank aData indicates a self locate
	// un-expected MOLR, otherwise we have a X3P MOLR.
	if (aData != KNullDesC)
		{
		// Process X3P Menu register.
		if (iState == EReqX3PMenu)
			{
			INFO_PRINTF1(_L("Got - X3P(MENU) MOLR - NetSim Notify Register Lcs MoLr - Event."));
			SetCurrentSeqEvent(EX3P_MENU_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);

			// Verify telephone number.
			if (aData.Compare(KTransmitMenuDestination))
				{
				INFO_PRINTF1(_L("Failed test, bad X3P Menu register data."));
				SetTestStepResult(EFail);
				}

			// Start X3P Push (second X3P).
			StartX3PL(KLbsConflictX3PPushId);
			}

		// Process X3P Push register.			
		else
			{
			INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Register Lcs MoLr - Event."));
			SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);

			// Verify telephone number.
			if (aData.Compare(KTransmitPushDestination))
				{
				INFO_PRINTF1(_L("Failed test, bad X3P Push register data."));
				SetTestStepResult(EFail);
				}
			
			// Indicate we have really started the second X3P.
//			iState = EReqX3PPush;			
			}		
		}

	else
		{
		INFO_PRINTF1(_L("Got unexpected - Self MOLR - NetSim Notify Register Lcs MoLr - Event."));
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);
		}
	}


void CT_LbsConflictStep_X3PMenuPush::NotifyReleaseLcsMoLr(TInt aReason)
	{
	TInt expectedErr = KErrNone;

	// Determine and set sequence event.
	if (iState == EReqX3PMenu)
		{
		INFO_PRINTF1(_L("Got - X3P(MENU) MOLR - NetSim Notify Release Lcs MoLr - Event."));
		SetCurrentSeqEvent(EX3P_MENU_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);
		
		expectedErr = KErrPositionHighPriorityReceive;
		
		// Indicate we should have started the second X3P by now.
		iState = EReqX3PPush;
		}
		
	else if(iState == EReqX3PPush)
		{
		INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Release Lcs MoLr - Event."));
		SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);

		iState = EWaiting; // End of test.
		}
	
	// Verify reason code.
	if (aReason != expectedErr)
		{	
		INFO_PRINTF2(_L("Failed test, bad release reason %d."), aReason);
		SetTestStepResult(EFail);
		}
	}


void CT_LbsConflictStep_X3PMenuPush::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	T_LbsUtils utils;	
	TInt err;
	
	// Determine and record sequence event.
	if (iState == EReqX3PMenu)
		{
		INFO_PRINTF1(_L("Got - X3P(MENU) MOLR - NetSim Notify Measurement Control Location - Event."));
		// cannot record this event since it may/maynot occur depending on timing but ...
		//		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation);
		// ... if we do get one of these, we must get a failure message so add that to adhoc list
		SetVerifyAdhocEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
		}
		
	else if(iState == EReqX3PPush)
		{
		INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Measurement Control Location - Event."));
		SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementControlLocation);
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


void CT_LbsConflictStep_X3PMenuPush::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	(void)aResult;

	// Unexpected callback for this test log the sequence event to fail test.
	if (iState == EReqX3PMenu)
		{
		INFO_PRINTF1(_L("Got - X3P(MENU) MOLR - NetSim Notify Release Lcs Location Notification - Event."));
//		SetCurrentSeqEvent(EX3P_MENU_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
		}
		
	else
		{
		INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Release Lcs Location Notification - Event."));
//		SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
		}
	}


void CT_LbsConflictStep_X3PMenuPush::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	// Determine and record sequence event.
	if (iState == EReqX3PMenu)
		{
		INFO_PRINTF1(_L("Got - X3P(MENU) MOLR - NetSim Notify Facility Lcs MoLr Result - Event."));
		SetCurrentSeqEvent(EX3P_MENU_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
		}
		
	else
		{
		INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Facility Lcs MoLr Result - Event."));
		SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
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


void CT_LbsConflictStep_X3PMenuPush::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	// Determine and record sequence event.
	if (iState == EReqX3PMenu)
		{
		INFO_PRINTF1(_L("Got - X3P(MENU) MOLR - NetSim Notify Measurement Report - Event."));
		SetCurrentSeqEvent(EX3P_MENU_MOLR_NetSim_Got_NotifyMeasurementReportLocation);
		}
		
	else
		{
		INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Measurement Report - Event."));
		SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportLocation);
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


void CT_LbsConflictStep_X3PMenuPush::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	// Unexpected callback for this test, record the sequence event to fail test.
	if (iState == EReqX3PMenu)
		{
		INFO_PRINTF1(_L("Got - X3P(MENU) MOLR - NetSim Notify Measurement Report Request More Assistance Data - Event."));
		SetCurrentSeqEvent(EX3P_MENU_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
		}
		
	else
		{
		INFO_PRINTF1(_L("Got - X3P(PUSH) MOLR - NetSim Notify Measurement Report Request More Assistance Data - Event."));
		SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData);
		}
	}


void CT_LbsConflictStep_X3PMenuPush::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	// Unexpected callback for this test, record the sequence event to fail test.
	if (iState == EReqX3PMenu)
		{
		INFO_PRINTF2(_L("Got - X3P(MENU) MOLR - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
		SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
		if(aReason != KErrPositionHighPriorityReceive)
			{
			SetTestStepResult(EFail);
			}
		}
		
	else
		{
		INFO_PRINTF2(_L("Got - X3P(PUSH) MOLR - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
		SetCurrentSeqEvent(EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure);
		}
	}

	

/** X3P transmit callback.

	The X3P transmit request has completed.
*/	
void CT_LbsConflictStep_X3PMenuPush::MT_LbsDoX3PCallback(TInt aTransmitId, TRequestStatus& aStatus)
	{
	TInt expectedErr = KErrNone;
	SetCurrentSeqEvent(EClient_Got_X3P_Complete);
	
	if (aTransmitId == KLbsConflictX3PMenuId)
		{
		INFO_PRINTF1(_L("Got - Client X3P(MENU) Complete - Callback Event."));

		expectedErr = KErrPositionHighPriorityReceive;
		}
		
	else if (aTransmitId == KLbsConflictX3PPushId)
		{
		INFO_PRINTF1(_L("Got - Client X3P(PUSH) Complete - Callback Event."));
		}

	// Verify error code.
	if (expectedErr != aStatus.Int())
		{
		INFO_PRINTF2(_L("Failed test, X3P transmit request err = %d."), aStatus.Int());
		SetTestStepResult(EFail);
		}
	}
		

