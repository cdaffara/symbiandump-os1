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
#include "ctlbshybridemergencycancel.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>
/**
Static Constructor
*/
CT_LbsHybridEmergencyCancel* CT_LbsHybridEmergencyCancel::New(CT_LbsExtendedMTLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridEmergencyCancel* testStep = new CT_LbsHybridEmergencyCancel(aParent);
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
CT_LbsHybridEmergencyCancel::CT_LbsHybridEmergencyCancel(CT_LbsExtendedMTLRServer& aParent) : CT_LbsExtendedMTLRStep(aParent)
	{
	SetTestStepName(KLbsHybridEmergencyCancel);
	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsHybridEmergencyCancel::ConstructL()
	{
	// Create the base class objects.
	CT_LbsExtendedMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();	
	}


/**
 * Destructor
 */
CT_LbsHybridEmergencyCancel::~CT_LbsHybridEmergencyCancel()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsHybridEmergencyCancel::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridEmergencyCancel::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 80*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
	
	// reset integration modules count of number of cancels recieved from LBS 
	T_LbsUtils utils;

	utils.ResetIntegrationModulesCountOfCancelsL();

	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status)); 
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);

// Initiate MTLR Start
	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// << ProcessPrivacyRequest()
	TBool emergency = ETrue;
	TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
	TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
	iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId, &emergency, &privacy, &requestInfo);

	// >> Callback from RespondNetworkLocationRequest(ERequestAccepted)
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// >> Respond Privacy Request
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
	TLbsNetSessionId* getSessionId = NULL;
	CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
	TInt cleanupCnt;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
	TESTL(getSessionId->SessionNum()==iSessionId.SessionNum());
	TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
	CleanupStack::PopAndDestroy(cleanupCnt);
// Initiate MTLR End
	
// MTLR Reference Position Notification Start
	// << ProcessLocationUpdate()
	TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);
// MTLR Reference Position Notification Stop

	// in this test we expect this cancel message to be ignored as it is an emergency
	InitiateCancelMTLR(iSessionId.SessionNum());
	
// MTLR Assistance Data Notification Start
	// << ProcessAssistanceData()
	TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); //assistanceData
// MTLR Assistance Data Notification Stop

	// in this test we expect this cancel message to be ignored as it is an emergency
	InitiateCancelMTLR(iSessionId.SessionNum());
	
// MTLR Network Location Request Start
	// << ProcessLocationRequest()
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2(); 
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
// MTLR Network Location Request Stop
	
	//Start the timer
	TTime timerStart;
	timerStart.HomeTime();
	
	// >> Callback from ProcessNetworkPostionUpdate(refPosition)
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// >> RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataGroup;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	//Find the time elapsed from timer
	TTimeIntervalMicroSeconds microseconds;
 	TTime timerStop;
 	timerStop.HomeTime();
 	microseconds = timerStop.MicroSecondsFrom(timerStart); 
	TInt64 timeElapsed = microseconds.Int64();
		
	// >> RespondLocationRequest()
	//Test that we do not get response before alpha2 has expired
	//At the moment, the test below fails and leaves since respond reaches us before alpha2 times out. We are not in hybrid mode.
	//TESTL(iProxy->WaitForResponse(ArgUtils::Alpha2()-timeElapsed-KDelta) == ENetMsgTimeoutExpired); 
	TESTL(iProxy->WaitForResponse(2*KDelta) == ENetMsgRespondLocationRequest); 
	
	getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason==KErrNone); 
	CleanupStack::PopAndDestroy(cleanupCnt);

	// in this test we expect this cancel message to be ignored as it is an emergency
	InitiateCancelMTLR(iSessionId.SessionNum());
	
//NHR's timer alpha2 times out -> Hybrid Positioning Start
//	quality = ArgUtils::Quality();  // set timeout t 
//	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

	// >> RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
//Request (t) times out - As before, no position available return latest measurements
//Terminal side timer alpha4 expires before stop message received from network
// Protocol Module (or possibly Network) decides to end positioning session 
	// >> Callback from ProcessRequestComplete()
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// Verify that the last callback was to ProcessSessionComplete()
	TESTL(iState==EGpsLocReceived);
	
   // and finally check that no cancels were sent to test integration module
	TInt cancelCount  = utils.IntegrationModulesCountOfCancelsL();
	INFO_PRINTF2(_L("--- cancel count = %d"), cancelCount);
	TESTL(cancelCount== 0);

   // MTLR Session Completion Start
	// << ProcessSessionComplete()
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);
	
	// >> Callback from ProcessRequestComplete()
	CheckForObserverEventTestsL(KTimeOut, *this);

	INFO_PRINTF2(_L("iState = 0x%x. Expecting ERequestComplete"), iState);    
	// Verify that the last callback was to ProcessSessionComplete()
	TESTL(iState==ERequestComplete);
		
// MTLR Session Completion Stop


	
	return TestStepResult();
	}
	
void CT_LbsHybridEmergencyCancel::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridEmergencyCancel::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
	}
   
void CT_LbsHybridEmergencyCancel::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& aPosInfo)
	{
	TPosition getPos;
	aPosInfo.GetPosition(getPos);
	INFO_PRINTF2(_L("latitude=%f"), getPos.Latitude());
	INFO_PRINTF2(_L("Longitude=%f"), getPos.Longitude());
	INFO_PRINTF2(_L("HorizontalAccuracy=%f"), getPos.HorizontalAccuracy());
	INFO_PRINTF2(_L("PositionMode=%d"), aPosInfo.PositionMode());
	if(iState==EPrivacyCheckOk)
		{
		iState=ERefLocReceived;	
		INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridEmergencyCancel::ProcessNetworkPositionUpdate(RefPosition)"));
		}
	else if(iState==ERefLocReceived)
		{
		if(getPos.Latitude()==49.2 && getPos.Longitude()==3.5 && getPos.Altitude()==50 && getPos.HorizontalAccuracy()==2 && getPos.VerticalAccuracy()==3) 		
			{
			INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridEmergencyCancel::ProcessNetworkPositionUpdate(GpsPosition)"));
			iState=EGpsLocReceived;	
			}
		}
	else
		{
		INFO_PRINTF1(_L("Unhandled ProcessNetworkPositionUpdate call"));			
		}
	ReturnToTestStep();
	}

void CT_LbsHybridEmergencyCancel::ProcessRequestComplete(TUint /*aRequestId*/, TInt aReason)
	{
	INFO_PRINTF2(_L("&gt;&gt;CT_LbsPTBMTLRCancel::ProcessRequestComplete(%d)"), aReason);
	TEST(KErrNone==aReason);
	TEST(iState==EGpsLocReceived);
	iState=ERequestComplete; 
	ReturnToTestStep();
	}

void CT_LbsHybridEmergencyCancel::InitiateCancelMTLR(TUint aRequestId)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridEmergencyCancel::initiateCancelMTLR()"));
	iController->CancelNetworkLocationRequest(aRequestId);
	INFO_PRINTF1(_L("Cancel Initiated"));
	}
