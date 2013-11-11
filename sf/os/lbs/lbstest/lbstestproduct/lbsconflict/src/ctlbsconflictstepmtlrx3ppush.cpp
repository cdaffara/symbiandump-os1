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
// @file ctlbsconflictstepmtlrx3ppush.cpp
// This is the class implementation for a MTLR - X3P Push conflict test.
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
#include "ctlbsconflictstepmtlrx3ppush.h"
#include <lbs/test/tlbsutils.h>


/** Static Constructor
*/
CT_LbsConflictStep_MTLRX3PPush* CT_LbsConflictStep_MTLRX3PPush::New(CT_LbsConflictServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsConflictStep_MTLRX3PPush* testStep = new CT_LbsConflictStep_MTLRX3PPush(aParent);
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


/** Constructor
 */
CT_LbsConflictStep_MTLRX3PPush::CT_LbsConflictStep_MTLRX3PPush(CT_LbsConflictServer& aParent) : CT_LbsConflictStep(aParent)
	{
	SetTestStepName(KLbsConflictStep_MTLRX3PPush);
	}


void CT_LbsConflictStep_MTLRX3PPush::ConstructL()
	{
	// Create the base class objects.
	CT_LbsConflictStep::ConstructL();
	
	// MTLR privacy controller.
	iPrivacyController = CLbsPrivacyController::NewL(*this);	

	// X3P async wrapper.
	iDoX3PPush = CT_LbsDoX3P::NewL(this);
	}


/** Destructor
 */
CT_LbsConflictStep_MTLRX3PPush::~CT_LbsConflictStep_MTLRX3PPush()
	{
	iPrivacyController->CancelNetworkLocationRequest(0);
	delete iPrivacyController;

	iDoX3PPush->Cancel();
	delete iDoX3PPush;
	}


/** Expected sequence events.
*/
void CT_LbsConflictStep_MTLRX3PPush::SetExpectedSeq()
	{
	// MTLR (iState = EStartMTLRPrivReq)
	SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkLocationRequest);
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
	
	// after location notification (iState = EStartMTLRLocReq)
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyMeasurementControlLocation);
	SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkPositionUpdate);	// Expect reference position.
	SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkPositionUpdate);	// Expect real position.
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyMeasurementReportLocation);
	SetVerifySeqEvent(EClient_Priv_Got_ProcessRequestComplete);

	// after MTLR complete (iState = EWaiting).

	SetVerifyAdhocEvent(EClient_Got_X3P_Complete);
	}


/** Run the test.
 */
TVerdict CT_LbsConflictStep_MTLRX3PPush::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsConflictStep_MTLRX3PPush::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
		// Setup the expected sequence events for the test.
		SetExpectedSeq();
				
		// Open and setup net sim.
		OpenNetSim(this);

		// Setup emergency locate service and pause to ensure value is set.
		CLbsAdmin*	admin = CLbsAdmin::NewL();
		CleanupStack::PushL(admin);
		User::LeaveIfError(admin->Set(KLbsSettingHomeEmergencyLocate, CLbsAdmin::EExternalLocateOn));
		User::After(5000000);
		CleanupStack::PopAndDestroy(admin);

		// Kick off the test abort and keep alive timers.
		TTimeIntervalMicroSeconds32 abortInterval(KLbsAbortPeriod);
		TTimeIntervalMicroSeconds32 keepAliveInterval(KLbsKeepAlivePeriod);

		iAbortTimer->SetTimer(abortInterval);
		iKeepAliveTimer->SetTimer(keepAliveInterval);

		// Kick off test.
		CActiveScheduler::Start();

		// Clean up.
		CloseNetSim();
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsConflictStep_MTLRX3PPush::doTestStepL()"));

	return TestStepResult();
	}


/** NetSim callbacks.
*/
void CT_LbsConflictStep_MTLRX3PPush::Connected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Connected();

	// Start a MT-LR request.
	_LIT8(KTestRequesterId, "LbsConflict test requester id");
	_LIT8(KTestClientName, "LbsConflict test client name");
	_LIT8(KTestClientExternalId, "LbsConflict test client external id");

	TLbsExternalRequestInfo reqInfo; 	
	reqInfo.SetRequesterId(KTestRequesterId);	
	reqInfo.SetClientName(KTestClientName);
	reqInfo.SetClientExternalId(KTestClientExternalId);			

	TLbsNetPosRequestPrivacy reqType;
	reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);

	iNetSim.SetEmergenyStatus(ETrue);
				
	// kick off the privacy request:
	iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);

	iState = EStartMTLRPrivReq;
	}


void CT_LbsConflictStep_MTLRX3PPush::Disconnected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Disconnected();
	}


void CT_LbsConflictStep_MTLRX3PPush::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	(void)aData;
	
	INFO_PRINTF1(_L("Failed test, got unexpected - NetSim Notify Register Lcs MoLr - Event."));
	SetTestStepResult(EFail);		
	}


void CT_LbsConflictStep_MTLRX3PPush::NotifyReleaseLcsMoLr(TInt aReason)
	{
	(void)aReason;
	
	INFO_PRINTF1(_L("Failed test, got unexpected - NetSim Notify Release Lcs MoLr - Event."));
	SetTestStepResult(EFail);
	}


void CT_LbsConflictStep_MTLRX3PPush::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	T_LbsUtils utils;	
	TInt err;
	
	// Determine and record sequence event.
	if (iState == EStartMTLRLocReq)
		{
		INFO_PRINTF1(_L("Got - MTLR - NetSim Notify Measurement Control Location - Event."));
		SetCurrentSeqEvent(EMTLR_NetSim_Got_NotifyMeasurementControlLocation);
		}

	else
		{
		INFO_PRINTF1(_L("Failed test, got unxpected - NetSim Notify Measurement Control Location - Event."));
		SetTestStepResult(EFail);
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


void CT_LbsConflictStep_MTLRX3PPush::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	// Record the sequence event.
	if (iState == EStartMTLRPrivReq)
		{
		INFO_PRINTF1(_L("Got - MTLR - NetSim Notify Release Lcs Location Notification - Event."));
		SetCurrentSeqEvent(EMTLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
		}
		
	else
		{
		INFO_PRINTF1(_L("Failed test, got unxpected - NetSim Notify Release Lcs Location Notification - Event."));
		SetTestStepResult(EFail);
		}
					
	// Should allways expect privacy requests to be excepted.
	if (CLbsNetworkProtocolBase::EPrivacyResponseAccepted == aResult)
		{
		// Now invoke MT-LR location request.
		iNetSim.StartNetworkLocationRequest();
		
		iState = EStartMTLRLocReq;
		}

	// Un-expected response.
	else
		{
		INFO_PRINTF1(_L("Failed test, should have excepted MT-LR privacy request."));
		SetTestStepResult(EFail);
		}
	}


void CT_LbsConflictStep_MTLRX3PPush::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	(void)aReason;
	(void)aPosition;

	INFO_PRINTF1(_L("Failed test, got unxpected - NetSim Notify Facility Lcs MoLr Result - Event."));
	SetTestStepResult(EFail);
	}


void CT_LbsConflictStep_MTLRX3PPush::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{	
	// Determine and record sequence event.
	if(iState == EWaitMTLRLocRes)
		{
		INFO_PRINTF1(_L("Got - MTLR - NetSim Notify Measurement Report - Event."));
		SetCurrentSeqEvent(EMTLR_NetSim_Got_NotifyMeasurementReportLocation);
		}

	else
		{
		INFO_PRINTF1(_L("Failed test, got unxpected - NetSim Notify Measurement Report - Event."));
		SetTestStepResult(EFail);
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


void CT_LbsConflictStep_MTLRX3PPush::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	INFO_PRINTF1(_L("Failed test, got unxpected - NetSim Notify Measurement Report Request More Assistance Data - Event."));
	SetTestStepResult(EFail);
	}


void CT_LbsConflictStep_MTLRX3PPush::NotifyMeasurementReportControlFailure(TInt aReason)
	{	
	INFO_PRINTF2(_L("Failed test, got unxpected - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
	SetTestStepResult(EFail);
	}


/** MTLR Privacy controller callbacks.
*/
void CT_LbsConflictStep_MTLRX3PPush::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType)
	{
	// We not aiming to test MT-LR in general here so only carryout minimal verification - verify the request id only.
	(void)aRequestInfo;
	(void)aNotificationType;

	iMtlrRequestId = aRequestId;

	// Record sequence event.
	INFO_PRINTF2(_L("Got - Privacy Process Network Location Request. Request Id = %d."), aRequestId);
	SetCurrentSeqEvent(EClient_Priv_Got_ProcessNetworkLocationRequest);
	
	// Start the MTLR location request.
	iPrivacyController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);


	// Start the X3P.

	// Create a posinfo and store in our shared array for later verification.
	RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* posInfo = new(ELeave) TPositionInfo();

	posInfoArr.Append(posInfo);
	
	// Transmit X3P.
	TLbsTransmitPositionOptions transOpts;
	
	iDoX3PPush->SetOptions(transOpts);	// Set timeout value to 0, to disable the timeout.
	iDoX3PPush->StartL(KTransmitPushDestination, KTransmitPushPriority, *posInfo);
	}


void CT_LbsConflictStep_MTLRX3PPush::ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo)
	{
	// Record sequence event.
	INFO_PRINTF2(_L("Got - Privacy Process Network Position Update. Request Id = %d."), aRequestId);
	SetCurrentSeqEvent(EClient_Priv_Got_ProcessNetworkPositionUpdate);
	
	// Verify the request id.
	if (aRequestId != iMtlrRequestId)
		{
		INFO_PRINTF3(_L("Failed test, incorrect request ID, got %d, expecting %d."), aRequestId, iMtlrRequestId);
		SetTestStepResult(EFail);
		}

	
	// Verify the position.

	// First time around the position will be the reference, second the actual position.
	T_LbsUtils utils;

	if (iState == EStartMTLRLocReq)
		{
		// Verify the reference position.
		TPositionInfo verifyRefPosInfo;
	
		verifyRefPosInfo.SetPosition(iRefPos);
		if (!utils.Compare_PosInfo(verifyRefPosInfo, aPosInfo))
			{
			INFO_PRINTF1(_L("Failed test, reference position incorrect."));
			SetTestStepResult(EFail);
			}
		
		iState = EWaitMTLRLocRes;
		}

	else if (iState == EWaitMTLRLocRes)
		{
		RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		TPositionInfo* verifyRealPosInfo = reinterpret_cast<TPositionInfo*>(verifyPosInfoArr[0]);
	
		if (!utils.Compare_PosInfo(*verifyRealPosInfo, aPosInfo))
			{
			INFO_PRINTF1(_L("Failed test, real position incorrect."));
			SetTestStepResult(EFail);
	    	} 
		}
	}


void CT_LbsConflictStep_MTLRX3PPush::ProcessRequestComplete(TUint aRequestId, TInt aReason)
	{	
	// Record sequence event.
	INFO_PRINTF3(_L("Got - Privacy Process Request Complete. Request Id = %d, Reason = %d."), aRequestId, aReason);
	SetCurrentSeqEvent(EClient_Priv_Got_ProcessRequestComplete);

	// Verify the request id.
	if (aRequestId != iMtlrRequestId)
		{
		INFO_PRINTF3(_L("Failed test, incorrect request ID, got %d, expecting %d."), aRequestId, iMtlrRequestId);
		SetTestStepResult(EFail);
		}
		
	// Verify the reason code.
	if (aReason)
		{
		INFO_PRINTF2(_L("Failed test, incorrect reason code, got %d, expecting KErrNone."), aReason);
		SetTestStepResult(EFail);
		}
	
	iState = EWaiting;
	}


/** X3P transmit callback.

	The X3P transmit request has completed.
*/	
void CT_LbsConflictStep_MTLRX3PPush::MT_LbsDoX3PCallback(TInt aTransmitId, TRequestStatus& aStatus)
	{
	(void)aTransmitId;

	SetCurrentSeqEvent(EClient_Got_X3P_Complete);
	INFO_PRINTF1(_L("Got - Client X3P Push Complete - Callback Event."));

	// Verify X3P push position, entry 0 of the array. We expect a position containing NaNs.
	RPointerArray<TAny>& currPosInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
	TPositionInfo* currPosInfo;

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

	// Ensure it completes with correct err.
	if (KErrServerBusy != aStatus.Int())
		{
		INFO_PRINTF2(_L("Failed test, X3P transmit request err = %d, expecting KErrServerBusy."), aStatus.Int());
		SetTestStepResult(EFail);
		iState = EWaiting;
		}
	}
		

