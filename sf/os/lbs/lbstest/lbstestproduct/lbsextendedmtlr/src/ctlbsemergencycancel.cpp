// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LBS includes. 
// 
//

#include <lbs/test/lbsnetextendedprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
// LBS test includes.
#include "ctlbsemergencycancel.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>
/**
Static Constructor
*/
CT_LbsEmergencyCancel* CT_LbsEmergencyCancel::New(CT_LbsExtendedMTLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsEmergencyCancel* testStep = new CT_LbsEmergencyCancel(aParent);
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
CT_LbsEmergencyCancel::CT_LbsEmergencyCancel(CT_LbsExtendedMTLRServer& aParent) : CT_LbsExtendedMTLRStep(aParent)
	{
	SetTestStepName(KLbsEmergencyCancel);
	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsEmergencyCancel::ConstructL()
	{
	// Create the base class objects.
	CT_LbsExtendedMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();	
	}


/**
 * Destructor
 */
CT_LbsEmergencyCancel::~CT_LbsEmergencyCancel()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsEmergencyCancel::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsEmergencyCancel::doTestStepL()"));

	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
	const TInt KSmallTimeOut = 3*1000*1000;
	
	// reset integration modules count of number of cancels recieved from LBS 
	T_LbsUtils utils;

	utils.ResetIntegrationModulesCountOfCancelsL();
	
	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;	cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status); 
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Initiate MTLR Start
	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// << ProcessPrivacyRequest()
	const TBool emergency = ETrue;
	TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
	TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
	iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId, &emergency, &privacy, &requestInfo);

	// >> Callback ly from RespondNetworkLocationRequest(ERequestAccepted)
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// >> Respond Privacy Request
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
	TLbsNetSessionId* getSessionId = NULL;
	CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
	TESTL(getSessionId->SessionNum()==iSessionId.SessionNum());
	TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
	CleanupStack::PopAndDestroy(cleanupCnt);//getSessionId
// Initiate MTLR End

	
// MTLR Reference Position Notification Start
	// << ProcessLocationUpdate()
	TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);
// MTLR Reference Position Notification Stop

// MTLR Assistance Data Notification Start
	// << ProcessAssistanceData()
	TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); //assistanceData
// MTLR Assistance Data Notification Stop

	// before injecting the location request try to cancel the MTLR, 
	// this should not work, as Emergency is set to ETrue
	InitiateCancelMTLR(iSessionId.SessionNum());
	
// MTLR Network Location Request Start
	// << ProcessLocationRequest()
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2(); 
	TLbsNetPosRequestMethod method   = ArgUtils::RequestTerminalBasedMethod();
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
// MTLR Network Location Request Stop
	
	// >> Callback from ProcessNetworkPostionUpdate(refPosition)
	CheckForObserverEventTestsL(KSmallTimeOut, *this); 
	
// >> RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataGroup;

	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	// Try to cancel the MTLR, this should not work, as Emergency is set to ETrue
	InitiateCancelMTLR(iSessionId.SessionNum());
	
	// >> RespondLocationRequest()

	TInt response = iProxy->WaitForResponse(KSmallTimeOut);
	TESTL(response == ENetMsgRespondLocationRequest); 

	getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason==KErrNone); 
	// it should be an AGPS position
	TESTL(getPositionInfo->PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));

	// now check that position is the AGPS position
	TPosition position;

	getPositionInfo->GetPosition(position);
	TESTL(position.Longitude()== 3.5);
	TESTL(position.HorizontalAccuracy()== 2.0);
		
	CleanupStack::PopAndDestroy(cleanupCnt); //getSessionId,getPositionInfo
	
	// >> Callback from ProcessNetworkPostionUpdate(AGPSPosition)
	CheckForObserverEventTestsL(KTimeOut, *this);

// MTLR Session Completion Start
	// << ProcessSessionComplete()
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	
	
	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

	// >> Callback from ProcessRequestComplete()
	CheckForObserverEventTestsL(KTimeOut*20, *this);

	// Verify that the last callback was to ProcessSessionComplete()
	TESTL(iState==ERequestComplete);
// MTLR Session Completion Stop
	
	// and finally check that no cancel was sent to test integration module
	// because, of course, no location request was sent to the agps module
	// - both cancels should have been ignored

	TInt cancelCount  = utils.IntegrationModulesCountOfCancelsL();

	INFO_PRINTF2(_L("--- cancel count = %d"), cancelCount);

	TESTL(cancelCount== 0);
	
	INFO_PRINTF1(_L("&lt;&lt;CT_LbsEmergencyCancel::doTestStepL()"));
	return TestStepResult();
	}
	
void CT_LbsEmergencyCancel::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsEmergencyCancel::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
	}
   
void CT_LbsEmergencyCancel::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& aPosInfo)
	{
	if(iState==EPrivacyCheckOk)
		{
	
		// check that it is the REF postion we are expecting!
		if (aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork)
			{
			TPosition getPos;
			aPosInfo.GetPosition(getPos);
			if (getPos.Longitude()==-0.096) // and others ...
				{
				iState=ERefLocReceived;
				}
			}
			
		INFO_PRINTF1(_L("&gt;&gt;CT_LbsEmergencyCancel::ProcessNetworkPositionUpdate(RefPosition)"));
		}
	else if(iState==ERefLocReceived)
		{
		// now check that position is the AGPS position
		TBool isAGPSposition = 
		aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);

		TPosition getPos;
		aPosInfo.GetPosition(getPos);
		if(isAGPSposition && getPos.Longitude()==3.5 && getPos.HorizontalAccuracy()==2.0 && getPos.VerticalAccuracy()==3.0) 		
			{
			INFO_PRINTF1(_L("&gt;&gt;CT_LbsEmergencyCancel::ProcessNetworkPositionUpdate(GpsPosition)"));
			iState=EGpsLocReceived;	
			}
		}
	ReturnToTestStep();
	}

void CT_LbsEmergencyCancel::ProcessRequestComplete(TUint /*aRequestId*/, TInt aReason)
	{
	INFO_PRINTF2(_L("&gt;&gt;CT_LbsPTBMTLRCancel::ProcessRequestComplete(%d)"), aReason);
	TEST(KErrNone==aReason);
	TEST(iState==EGpsLocReceived);
	iState=ERequestComplete; 
	ReturnToTestStep();
	}

void CT_LbsEmergencyCancel::InitiateCancelMTLR(TUint aRequestId)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsEmergencyCancel::initiateCancelMTLR()"));
	iController->CancelNetworkLocationRequest(aRequestId);
	INFO_PRINTF1(_L("Cancel Initiated"));
	}
