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
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsloccommon.h>
#include <lbserrors.h>

// LBS test includes.
#include "ctlbshybridmtlrfutile.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

/**
Static Constructor
*/
CT_LbsHybridMTLRFutile* CT_LbsHybridMTLRFutile::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridMTLRFutile* testStep = new CT_LbsHybridMTLRFutile(aParent);
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
CT_LbsHybridMTLRFutile::CT_LbsHybridMTLRFutile(CT_LbsHybridMTLRServer& aParent) : CT_LbsHybridMTLRStep(aParent)
	{
	SetTestStepName(KLbsHybridMTLRFutile);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	iPositionUpdateRef = EFalse;
	}


void CT_LbsHybridMTLRFutile::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();
	iAGpsHandler = CT_LbsAGpsHandler::NewL(this);
	}


/**
 * Destructor
 */
CT_LbsHybridMTLRFutile::~CT_LbsHybridMTLRFutile()
	{
	delete iAGpsHandler;
	delete iController;
	delete iProxy;
	}

/** 
 * HYBRID MTLR FUTILE SCENARIOS:
 * 	Test Case Id 1: Hybrid LBS MTLR UE Based - Futile
 * 	Test Case Id 2: Hybrid LBS MTLR UE Assisted - Futile
 * 	Test Case Id 3: LBS MTLR UE Based - Futile
*/
TVerdict CT_LbsHybridMTLRFutile::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridMTLRFutile::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Carryout unique test actions.
	if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
		{
		INFO_PRINTF2(_L("Test id %d."), iTestCaseId);

// Comment out while lbs is restarted in this test's script
//		if (iTestCaseId == 1)
			{
			// >> AdviceSystemStatus(0)
			TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
			CLbsNetworkProtocolBase::TLbsSystemStatus status;
			CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status));
			TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
			}
			
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
		
		TInt cleanupCnt;

		// >> Respond Privacy Request
		TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
		TLbsNetSessionId* sessionId = NULL;
		CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
		cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &sessionId, &getPrivacy);
		TESTL(sessionId->SessionNum()==iSessionId.SessionNum());
		TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
		CleanupStack::PopAndDestroy(cleanupCnt);
	// Initiate MTLR End

	// MTLR Reference Position Notification Start
		// << ProcessLocationUpdate()
		TPositionInfo refPosition = ArgUtils::ReferencePositionInfo();
		iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosition);
	// MTLR Reference Position Notification End

	// MTLR Assistance Data Notification Start
		// << ProcessAssistanceData()
		TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
		RLbsAssistanceDataBuilderSet assistanceData;
		ArgUtils::PopulateLC(assistanceData);
		TInt reason = KErrNone;
		iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
		CleanupStack::PopAndDestroy();
	// MTLR Assistance Data Notification End

	// MTLR Network Location Request Start
		// << ProcessLocationRequest()
		MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
		TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2();
		TLbsNetPosRequestMethod method;
		if ((iTestCaseId == 1) || (iTestCaseId == 2) || (iTestCaseId == 4))
			{
			method = ArgUtils::RequestHybridMethod();
			}
		else if (iTestCaseId == 3)
			{
 			method = ArgUtils::RequestMethod();
			}
		iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	// MTLR Network Location Request End
		
		//Start the timer
		TTime timerStart;
		timerStart.HomeTime();

		// >> Callback from ProcessNetworkPostionUpdate(refPosition)
		CheckForObserverEventTestsL(KTimeOut, *this);
		
	// MTLR Assistance Data Notification Start
		// >> RequestAssistanceData(0)
		TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		TLbsAsistanceDataGroup dataGroup;
		CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup));
		TESTL(dataGroup == EAssistanceDataNone);
	// MTLR Assistance Data Notification End

		TPositionGpsMeasurementInfo* measurementInfo = NULL;
		
		if ((iTestCaseId == 2) || (iTestCaseId == 4))
			{
			//Find the time elapsed from timer
			TTimeIntervalMicroSeconds microseconds;
		 	TTime timerStop;
		 	timerStop.HomeTime();
		 	microseconds = timerStop.MicroSecondsFrom(timerStart); 
			TInt64 timeElapsed = microseconds.Int64();
				
			// >> RespondLocationRequest()
			//Test that we do not get response before alpha2 has expired
			TESTL(iProxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - KDelta) == ENetMsgTimeoutExpired); 
			TESTL(iProxy->WaitForResponse(2 * KDelta) == ENetMsgRespondLocationRequest); 

			sessionId = NULL;
			reason = KErrNone;
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(reason == KErrNone); 
			// Check it is a measurement
			TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
			CleanupStack::PopAndDestroy(cleanupCnt); //sessionId, measurementInfo
		
		// Force a nother GPS update - this should be the futile one, and it will be returned
		// in response to the location request below.
			if (iTestCaseId == 4)
				{
				iAGpsHandler->SendRequestForcedUpdate();
				CActiveScheduler::Start();
				}

		//NHR's timer alpha2 times out -> Hybrid Positioning Start	
			//<< ProcessLocationRequest() 
			quality = ArgUtils::Quality(); //Set timeout t to KTimeOut
			iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
			
		// MTLR Assistance Data Notification Start
			// >> RequestAssistanceData(0)
			if (iTestCaseId == 2)
				{
				TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
				CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup));
				TESTL(dataGroup == EAssistanceDataNone);
				}
		// MTLR Assistance Data Notification End
			}
		
		// >> RespondLocationRequest()
		// Error returned by module should be KPositionCalculationFutile
		// the alpha or t timeout
		if ((iTestCaseId == 1) || (iTestCaseId == 3))
			{
			TESTL(iProxy->WaitForResponse(ArgUtils::Alpha2()) == ENetMsgRespondLocationRequest); 
			}
		else if ((iTestCaseId == 2) || (iTestCaseId == 4))
			{
			TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 
			}
		sessionId = NULL;
		reason = KErrNone;
		if ((iTestCaseId == 1) || (iTestCaseId == 2) || (iTestCaseId == 4))
			{
			// Check it is a measurement
			measurementInfo = NULL;
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
			TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
			}
		else if (iTestCaseId == 3)
			{
			// check it is a position
			TPositionInfo* posInfo = NULL;
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &posInfo);
			TESTL(posInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
			}
		TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
		TESTL(reason == KPositionCalculationFutile);
		CleanupStack::PopAndDestroy(cleanupCnt); //sessionId, measurementInfo\positionInfo
		
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
	// MTLR Session Completion End
		}

	return TestStepResult();
	}

void CT_LbsHybridMTLRFutile::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridMTLRFutile::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
}

void CT_LbsHybridMTLRFutile::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridMTLR::ProcessNetworkPositionUpdate()"));
	if(!iPositionUpdateRef)
		{
		TEST(iState == EPrivacyCheckOk);
		iState = ERefLocReceived;	
		iPositionUpdateRef = ETrue;	
		}
	else
		{
		// None of this tests shoud get here
		TEST(EFalse);
		}
	ReturnToTestStep();
}

void CT_LbsHybridMTLRFutile::ProcessRequestComplete(TUint aRequestId, TInt aReason)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridMTLRFutile::ProcessRequestComplete()"));

	TEST(iState == ERefLocReceived);
	TEST(aReason == KErrNone);
	TEST(aRequestId == iSessionId.SessionNum());
	iState = ERequestComplete; 
	ReturnToTestStep();
}

void CT_LbsHybridMTLRFutile::ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse)
	{
	if (TT_LbsAGpsResponseMsg::EModuleResponseOk != aResponse)
		{
		SetTestStepResult(EPass);
		}

	CActiveScheduler::Stop();
	}




