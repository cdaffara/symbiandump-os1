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
// @file ctlbsmtlrprivcntrlstepmain.h
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_MTLR_PRIV_CNTRL_STEP_H__
#define __CT_LBS_MTLR_PRIV_CNTRL_STEP_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsadmin.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/test/lbsnetsimtest.h>

// LBS test includes.
#include "ctlbsmtlrstep.h"
#include "ctlbsmtlrserver.h"

// Literals used
_LIT(KLbsMTLRPrivCntrlStep_Main, "LbsMTLRPrivCntrlStep_Main");

// Callbacks flags.

const TLbsCallbackFlags KLbsCallback_Got_PrivCntrlNetworkLocationRequest= 0x1000;	// network location request is received by the LBS system
const TLbsCallbackFlags KLbsCallback_Got_PrivCntrlNetworkPositionUpdate	= 0x2000;	// position update (reference / final) is ready
const TLbsCallbackFlags KLbsCallback_Got_PrivCntrlRequestComplete		= 0x4000;	// network location request (e.g. MT-LR) has been completed
//const TLbsCallbackFlags KLbsCallback_Got_AdminSetting					= 0x8000;	// got callback for admin setting change

const TLbsCallbackFlags KLbsCallback_PrivCntrl = 	KLbsCallback_Got_PrivCntrlNetworkLocationRequest |
													KLbsCallback_Got_PrivCntrlNetworkPositionUpdate |
													KLbsCallback_Got_PrivCntrlRequestComplete;


class CT_LbsMTLRPrivCntrlStep_Main : public	CT_LbsMTLRStep, MLbsPrivacyObserver, MLbsAdminObserver
	{
public:
	~CT_LbsMTLRPrivCntrlStep_Main();

	static CT_LbsMTLRPrivCntrlStep_Main* New(CT_LbsMTLRServer& aParent);
	virtual TVerdict doTestStepL();

	// From CT_LbsNetSimStep via CT_LbsMTLRStep
	void Connected();
	void Disconnected();
	void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
	void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	void NotifyMeasurementReportControlFailure(TInt aReason);
	void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);

	// From MLbsPrivacyObserver:	
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);	

	// From MLbsAdminObserver:
	void OnSettingUpdateEvent(TInt aError, const TLbsAdminSetting& aSetting);
	
private:
	void VerifyRequestorDetails(const TLbsExternalRequestInfo& aRequestInfo);
		
	CT_LbsMTLRPrivCntrlStep_Main(CT_LbsMTLRServer& aParent);
	void ConstructL();
	
	enum  {	EAdminWait = CT_LbsNetSimStep::ELast, EStartMTLRPrivReq,
			EStartMTLRLocReq, ENetPosUpdCountWait};
	
	// For changing admin settings:			
	CLbsAdmin*	iLbsAdmin;
	
	// the lbs privacy controller interface:
	CLbsPrivacyController* iLbsPrivacyController;
	
	// number of times that the privacy controller's ProcessNetworkPositionUpdate() callback has been called for this test case
	TUint iProcessNetPosUpdCount;	
	
	
	
	// Should we use bad request Id
	TBool iUseBadRequestId;
	
	TBool iRespondWithBadId;
	TBool iEarlyCancelWithBadId;
	TBool iLateCancelWithBadId;	
	TBool iRespondAndCancelWithBadId;
	
	// cause test module to stall for this amount of time (defaults to 0)
	TTimeIntervalMicroSeconds iTestModuleStallTime;
	
	// extend the keepalive timer for this amount after all callbacks received
	TTimeIntervalMicroSeconds32	iExtendedTimerTime;
	
	
	// Current test case being executed:
	TInt iTestCaseId;
	//Current profileid being used
	TInt iProfileId;
	// this set of member vars defines an indicidual test case:
	
	// How test should configure the external locate service
	CLbsAdmin::TExternalLocateService	iConfiguredExtLocateService;
	
	// privacy controller's response (accept/reject/ignore) to priv request:	
	CLbsPrivacyController::TRequestVerificationResult iPrivResult;
	
	// expected response sent to network 
	CLbsNetworkProtocolBase::TLbsPrivacyResponse iExpectedNetworkResult;	
		
	TUint iExpectedNetPosUpdCount;
	
	// whether privacy controller should respond to a privacy request (as configured above)
	TBool iRespondToPrivacyRequest;
	TBool iCancelAfterResponding;
	TUint iSessionId;
	
	// whether we should kick off a location request when notified that response to privacy request has been sent to network
	TBool	iStartLocationRequest;
	
	// configure emergency request? 
	TBool iConfigureEmergencyRequest;
	
	// configure 'roaming'?
	TBool iConfigureRoaming;
	// configure 'quality profile'?
	TBool iConfigureProfile;
	// type of network request to configure (requestaction/requestadvice)
	TLbsNetPosRequestPrivacy::TLbsRequestAction	iConfiguredRequestAction;	
	TLbsNetPosRequestPrivacy::TLbsRequestAdvice	iConfiguredRequestAdvice;
	
	TUint iExpectedCompleteRequestId;
	
	// expecting control failure with this reason:	
	TInt	iExpectedControlFailureReason;
		
	// should the privacy controller cancel the test on receiving 
	TBool	iCancelBeforeFinalPosUpd;

	};

#endif //__CT_LBS_MTLR_PRIV_CNTRL_STEP_H__
