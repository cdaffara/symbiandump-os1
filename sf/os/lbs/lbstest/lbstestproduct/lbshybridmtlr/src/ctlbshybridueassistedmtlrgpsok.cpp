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
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbshybridueassistedmtlrgpsok.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>



/**
Static Constructor
*/
CT_LbsHybridUEAssistedMTLRGPSOK* CT_LbsHybridUEAssistedMTLRGPSOK::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedMTLRGPSOK* testStep = new CT_LbsHybridUEAssistedMTLRGPSOK(aParent);
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
CT_LbsHybridUEAssistedMTLRGPSOK::CT_LbsHybridUEAssistedMTLRGPSOK(CT_LbsHybridMTLRServer& aParent) : CT_LbsHybridMTLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssistedMTLRGPSOK);
	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsHybridUEAssistedMTLRGPSOK::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();	
	}


/**
 * Destructor
 */
CT_LbsHybridUEAssistedMTLRGPSOK::~CT_LbsHybridUEAssistedMTLRGPSOK()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsHybridUEAssistedMTLRGPSOK::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedMTLRGPSOK::doTestStepL()"));	
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
	const TInt KSmallTimeOut = 3*1000*1000;
	
	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status); 
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

	// >> Callback from RespondNetworkLocationRequest(ERequestAccepted)
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
	CheckForObserverEventTestsL(KSmallTimeOut, *this);
	
// >> RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
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
	TESTL(iProxy->WaitForResponse(KAlpha2Timeout-timeElapsed-KDelta) == ENetMsgTimeoutExpired); 
	TESTL(iProxy->WaitForResponse(2*KDelta) == ENetMsgRespondLocationRequest); 

	getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason==KErrNone); 
	CleanupStack::PopAndDestroy(cleanupCnt); //getSessionId,getPositionInfo

//NHR's timer alpha2 times out -> Hybrid Positioning Start	
	//<< ProcessLocationRequest() 
	quality = ArgUtils::Quality(); //Set timeout to t 
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	
	// >> RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// >> RespondLocationRequest()
	TESTL(iProxy->WaitForResponse(KTTimeout) == ENetMsgRespondLocationRequest); 
	getSessionId = NULL;
	getReason = KErrNone;
	getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason==KErrNone);
	CleanupStack::PopAndDestroy(cleanupCnt);//getSessionId,getPositionInfo
		
	//<< ProcessNetworkPositionUpdate() | GPS Pos[Accurate Fix]
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState==EGpsLocReceived);
//Hybrid Positioning Stop

// MTLR Session Completion Start
	// << ProcessSessionComplete()
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

	// >> Callback from ProcessRequestComplete()
	CheckForObserverEventTestsL(KTimeOut, *this);

	// Verify that the last callback was to ProcessSessionComplete()
	TESTL(iState==ERequestComplete);
// MTLR Session Completion Stop

	return TestStepResult();
	}
	
void CT_LbsHybridUEAssistedMTLRGPSOK::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRGPSOK::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
	}
   
void CT_LbsHybridUEAssistedMTLRGPSOK::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& aPosInfo)
	{
	if(iState==EPrivacyCheckOk)
		{
		iState=ERefLocReceived;	
		INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRGPSOK::ProcessNetworkPositionUpdate(RefPosition)"));
		}
	else if(iState==ERefLocReceived)
		{
		// Test for $update,1,2,51.5015,-0.105,50,2,3*
		TPosition getPos;
		aPosInfo.GetPosition(getPos);
		if(getPos.Latitude()==51.5015 && getPos.Longitude()==-0.105 && getPos.Altitude()==50 && getPos.HorizontalAccuracy()==2 && getPos.VerticalAccuracy()==3) 		
			{
			INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRGPSOK::ProcessNetworkPositionUpdate(GpsPosition)"));
			iState=EGpsLocReceived;	
			}
		}
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedMTLRGPSOK::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRGPSOK::ProcessRequestComplete()"));
	TEST(iState==EGpsLocReceived);
	iState=ERequestComplete; 
	ReturnToTestStep();
	}
