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
// @file ctlbsconflictstepmtlrnetlocreq.cpp
// This is the class implementation for a MTLR - Network Location Request conflict test.
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
#include "ctlbsconflictstepmtlrnetlocreq.h"
#include <lbs/test/tlbsutils.h>


/** Static Constructor
*/
CT_LbsConflictStep_MTLRNETLOCREQ* CT_LbsConflictStep_MTLRNETLOCREQ::New(CT_LbsConflictServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsConflictStep_MTLRNETLOCREQ* testStep = new CT_LbsConflictStep_MTLRNETLOCREQ(aParent);
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
CT_LbsConflictStep_MTLRNETLOCREQ::CT_LbsConflictStep_MTLRNETLOCREQ(CT_LbsConflictServer& aParent) : CT_LbsConflictStep(aParent)
	{
	SetTestStepName(KLbsConflictStep_MTLRNETLOCREQ);
	}


void CT_LbsConflictStep_MTLRNETLOCREQ::ConstructL()
	{
	// Create the base class objects.
	CT_LbsConflictStep::ConstructL();
	
	// MTLR privacy controller.
	iPrivacyController = CLbsPrivacyController::NewL(*this);	

	// Self locate async wrapper - NOTE: we're doing a *cellbased* location update
	iDoPosUpdate = CT_LbsDoPosUpdate::NewL(this, TPositionModuleInfo::ETechnologyNetwork);
	}


/** Destructor
 */
CT_LbsConflictStep_MTLRNETLOCREQ::~CT_LbsConflictStep_MTLRNETLOCREQ()
	{
	iPrivacyController->CancelNetworkLocationRequest(0);
	delete iPrivacyController;

	iDoPosUpdate->Cancel();
	delete iDoPosUpdate;
	}


/** Expected sequence events.
*/
void CT_LbsConflictStep_MTLRNETLOCREQ::SetExpectedSeq()
	{
	// note that client flags are commented out here because they vary in order between runs
#ifdef PM_HACK_FOR_INC103104
	// Set expected sequence for this test:
	SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkLocationRequest);
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
	
	// after location notification (iState = EStartMTLRLocReq)
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyMeasurementControlLocation);
	//SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkPositionUpdate);	// Expect reference position.
	
	// Network Based Location
	// SetVerifySeqEvent(EClient_Got_PosUpdate_Complete);
	
	// MTLR contd
	//SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkPositionUpdate);	// Expect real position.
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyMeasurementReportLocation);
	//SetVerifySeqEvent(EClient_Priv_Got_ProcessRequestComplete);
	
#else	// our standard PM
	// Set expected sequence for this test:
	SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkLocationRequest);
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
	
	// after location notification (iState = EStartMTLRLocReq)
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyMeasurementControlLocation);
	//SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkPositionUpdate);	// reference position.
	//SetVerifySeqEvent(EClient_Priv_Got_ProcessNetworkPositionUpdate);	// real position.
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyMeasurementReportLocation);
	//SetVerifySeqEvent(EClient_Priv_Got_ProcessRequestComplete);
			
	// Network Based Location	
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);	
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyMeasurementControlLocation);	
	SetVerifySeqEvent(EMTLR_NetSim_Got_NotifyMeasurementReportLocation);
	//SetVerifySeqEvent(EClient_Got_PosUpdate_Complete);
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);	
	SetVerifySeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);

#endif 
	}


/** Run the test.
 */
TVerdict CT_LbsConflictStep_MTLRNETLOCREQ::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsConflictStep_MTLRNETLOCREQ::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
		// Setup the expected sequence events for the test.
		SetExpectedSeq();
				
		// Open and setup net sim.
		OpenNetSim(this);
		
		// Kick off the test abort and keep alive timers.
		TTimeIntervalMicroSeconds32 abortInterval(120*1000000);	// give it long enough for locserver timeout to fire
		TTimeIntervalMicroSeconds32 keepAliveInterval(KLbsKeepAlivePeriod);

		iAbortTimer->SetTimer(abortInterval);
		iKeepAliveTimer->SetTimer(keepAliveInterval);
		
		// Set up test module so there's a delay in responding to request for fix:
		TModuleDataIn modDataInTimeout; // Used to send test information to the test module.
		const TInt KLbsTestModuleTimeOut = 3000000;	// 3 sec delay
		// The module request type - time out value.
		modDataInTimeout.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
		// Micro seconds time value to delay the return position update from the module.
		modDataInTimeout.iTimeOut = KLbsTestModuleTimeOut;
		T_LbsUtils utils;
		utils.NotifyModuleOfConfigChangeL(modDataInTimeout);
		
		// Kick off test.
		CActiveScheduler::Start();

		// Clean up.
		CloseNetSim();
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsConflictStep_MTLRNETLOCREQ::doTestStepL()"));

	return TestStepResult();
	}


/** NetSim callbacks.
*/
void CT_LbsConflictStep_MTLRNETLOCREQ::Connected()
	{
	// Call base implementation.
	CT_LbsConflictStep::Connected();
	INFO_PRINTF1(_L("Got - Connected"));
	
	// Start a privacy request:
	_LIT8(KTestRequesterId, "LbsConflictMTLRNetLocReq test requester id");
	_LIT8(KTestClientName, "LbsConflictMTLRNetLocReq test client name");
	_LIT8(KTestClientExternalId, "LbsConflictMTLRNetLocReq test client external id");

	TLbsExternalRequestInfo reqInfo; 	
	reqInfo.SetRequesterId(KTestRequesterId);
	reqInfo.SetClientName(KTestClientName);
	reqInfo.SetClientExternalId(KTestClientExternalId);	

	TLbsNetPosRequestPrivacy reqType;
	reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);

	iNetSim.SetEmergenyStatus(EFalse);

	// kick off the privacy request:
	iNetSim.StartNetworkPrivacyRequest(reqType, reqInfo);
	
	}


void CT_LbsConflictStep_MTLRNETLOCREQ::Disconnected()
	{
	INFO_PRINTF1(_L("Got - Disconnected"));
	// Call base implementation.
	CT_LbsConflictStep::Disconnected();
	}



void CT_LbsConflictStep_MTLRNETLOCREQ::NotifyMeasurementControlLocation(const TPositionInfo& aPosition, 
															  const RLbsAssistanceDataBuilderSet& aData, 
															  const TLbsNetPosRequestQuality& aQuality)
	{
	INFO_PRINTF1(_L("Got - NetSim Notify Measurement Control Location"));
	
	(void)aQuality;
	(void)aPosition;
	(void)aData;
	
	SetCurrentSeqEvent(EMTLR_NetSim_Got_NotifyMeasurementControlLocation);
	}


void CT_LbsConflictStep_MTLRNETLOCREQ::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	//
	(void)aResult;
	INFO_PRINTF1(_L("Got - NetSim NotifyReleaseLcsLocationNotification."));
	SetCurrentSeqEvent(EMTLR_NetSim_Got_NotifyReleaseLcsLocationNotification);
	}


void CT_LbsConflictStep_MTLRNETLOCREQ::NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition)
	{
	(void)aReason;
	(void)aPosition;

	INFO_PRINTF1(_L("Got - NetSim Notify Facility Lcs MoLr Result - Event."));
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult);
#ifdef PM_HACK_FOR_INC103104
	SetTestStepResult(EFail);
#endif
	iState = EWaiting;
	}


void CT_LbsConflictStep_MTLRNETLOCREQ::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{
	
	(void)aPosition;
	INFO_PRINTF1(_L("Got - NetSim NotifyMeasurementReportLocation."));
	SetCurrentSeqEvent(EMTLR_NetSim_Got_NotifyMeasurementReportLocation);
    }


void CT_LbsConflictStep_MTLRNETLOCREQ::NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter)
	{
	(void)aFilter;

	INFO_PRINTF1(_L("Got unxpected - NetSim Notify Measurement Report Request More Assistance Data - Event."));
	SetTestStepResult(EFail);
	iState = EWaiting;
	}


void CT_LbsConflictStep_MTLRNETLOCREQ::NotifyMeasurementReportControlFailure(TInt aReason)
	{	
	INFO_PRINTF2(_L("Got unxpected - Net Sim Notify Measurement Report Control Failure - Event. Reason = %d"), aReason);
	SetTestStepResult(EFail);
	iState = EWaiting;
	}


/** MTLR Privacy controller callbacks.
*/
void CT_LbsConflictStep_MTLRNETLOCREQ::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType)
	{
	// We're not aiming to test MT-LR in general here so only carryout minimal verification - verify the request id only.
	(void)aRequestInfo;
	(void)aNotificationType;
	
	// Record sequence event.
	INFO_PRINTF2(_L("Got - Privacy Process Network Location Request. Request Id = %d."), aRequestId);
	SetCurrentSeqEvent(EClient_Priv_Got_ProcessNetworkLocationRequest);
	
	INFO_PRINTF1(_L("Accepting privacy Request/n"));
	iPrivacyController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	
	// Start the MTLR location request. 
	INFO_PRINTF1(_L("Asking NetSim to start MTLR location request/n"));
	iNetSim.StartNetworkLocationRequest();
	}


void CT_LbsConflictStep_MTLRNETLOCREQ::ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo)
	{
	
	// This should be called twice - once for the reference position and once for the gps fix
	(void)aRequestId;
	(void)aPosInfo;	
	// Record sequence event.
	INFO_PRINTF2(_L("Got - Privacy Process Network Position Update. Request Id = %d."), aRequestId);
	
	if(!iProcessNetworkPositionUpdateCount)
		{
		INFO_PRINTF1(_L("Starting Cell-based Request\n"));
		// Start Cell-based location request: 
		iPositionInfo = new(ELeave) TPositionInfo();
		iDoPosUpdate->StartL(*iPositionInfo);
		}
		
	iProcessNetworkPositionUpdateCount++;
	
	// SetCurrentSeqEvent(EClient_Priv_Got_ProcessNetworkPositionUpdate); - lrm order not consistent, check some other way
	// lrm - to do: log the position details
	}


void CT_LbsConflictStep_MTLRNETLOCREQ::ProcessRequestComplete(TUint aRequestId, TInt aReason)
	{	
	
	// This should be called at the end of the MTLR
	INFO_PRINTF3(_L("Got - Privacy Process Request Complete. Request Id = %d, Reason = %d."), aRequestId, aReason);
	if(KErrNone != aReason)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("FAILED: Got - Privacy Process Request Complete with reason %d, expected KErrNone."), aReason);
		}
	// SetCurrentSeqEvent(EClient_Priv_Got_ProcessRequestComplete); - lrm order not consistent, check some other way
	iState = EWaiting;
	}



		
void CT_LbsConflictStep_MTLRNETLOCREQ::MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus)
	{
	TInt err = aStatus.Int();
	
	INFO_PRINTF2(_L("Got - Location Client notified of Update - Completion reason: %d"), err);
	
#ifdef PM_HACK_FOR_INC103104	
	if(KErrServerBusy != err)		
#else
	if(KErrNone != err)
#endif
		{
		INFO_PRINTF2(_L("FAILED: Got unexpected error code %d"), err);
		SetTestStepResult(EFail);
		iState = EWaiting;
		}
		
	//SetCurrentSeqEvent(EClient_Got_PosUpdate_Complete);
	
	delete iPositionInfo;
		
	}	

void CT_LbsConflictStep_MTLRNETLOCREQ::NotifyRegisterLcsMoLr(const TDesC& aData)
	{
	if (aData != KNullDesC)
		{
		INFO_PRINTF1(_L("Got unexpected - NetSim Notify Register Lcs MoLr - Event."));
		SetTestStepResult(EFail);
		iState = EWaiting;
		}
	else
		INFO_PRINTF1(_L("Got - NetSim Notify Register Lcs MoLr - Callback Event."));

	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr);
	}

	

void CT_LbsConflictStep_MTLRNETLOCREQ::NotifyReleaseLcsMoLr(TInt aReason)
	{
	(void)aReason;

	INFO_PRINTF1(_L("Got - NetSim Notify Release Lcs MoLr - Callback Event."));
	SetCurrentSeqEvent(ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr);
	}

