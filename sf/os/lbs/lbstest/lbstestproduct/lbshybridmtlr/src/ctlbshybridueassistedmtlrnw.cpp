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
#include "ctlbshybridueassistedmtlrnw.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

/**
Static Constructor
*/
CT_LbsHybridUEAssistedMTLRNW* CT_LbsHybridUEAssistedMTLRNW::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedMTLRNW* testStep = new CT_LbsHybridUEAssistedMTLRNW(aParent);
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
CT_LbsHybridUEAssistedMTLRNW::CT_LbsHybridUEAssistedMTLRNW(CT_LbsHybridMTLRServer& aParent) : CT_LbsHybridMTLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssitedMTLRNW);
	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0004);
	}


void CT_LbsHybridUEAssistedMTLRNW::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();
	}


/**
 * Destructor
 */
CT_LbsHybridUEAssistedMTLRNW::~CT_LbsHybridUEAssistedMTLRNW()
	{
	delete iController;
	delete iProxy;
	}

TVerdict CT_LbsHybridUEAssistedMTLRNW::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedMTLRNW::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;

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
	TESTL(iProxy->WaitForResponse(ArgUtils::Alpha2()-timeElapsed-KDelta) == ENetMsgTimeoutExpired); 
	TESTL(iProxy->WaitForResponse(2*KDelta) == ENetMsgRespondLocationRequest); 
	getSessionId = NULL;
	TInt getReason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason==KErrNone); 
	CleanupStack::PopAndDestroy(cleanupCnt);
		
//NHR's timer alpha2 times out -> Hybrid Positioning Start
	quality = ArgUtils::Quality();  // set timeout t = 1s, should eventually be read from ini file?
	const TInt KN(2); // TODO number of times to send the measurement from GPS to NW, should eventually be read from ini file?
	for(TInt i=0;i<KN;i++)
		{
		iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
		timerStart.HomeTime();
	
		// >> RequestAssistanceData(0)
		TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
		TESTL(dataGroup == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
	 	timerStop.HomeTime();
	 	microseconds = timerStop.MicroSecondsFrom(timerStart); 
		timeElapsed = microseconds.Int64();
		
		// >> RespondLocationRequest()
		//Test that we do not get response before t has expired
		TESTL(iProxy->WaitForResponse(KTTimeout-timeElapsed-KDelta) == ENetMsgTimeoutExpired);
		TESTL(iProxy->WaitForResponse(2*KDelta) == ENetMsgRespondLocationRequest);
		getSessionId = NULL;
		getReason = KErrNone;
		getPositionInfo = NULL;
		cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
		TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
		TESTL(getReason==KErrNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
		}
//Hybrid Positioning Stop

// MTLR Session Completion Start
	// << ProcessSessionComplete()
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

void CT_LbsHybridUEAssistedMTLRNW::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRNW::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedMTLRNW::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
	{
	if(iState==EPrivacyCheckOk)
		{
		iState=ERefLocReceived;	
		INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRNW::ProcessNetworkPositionUpdate(RefPosition)"));
		}
	else if(iState==ERefLocReceived)
		{
		WARN_PRINTF1(_L("&gt;&gt;Unexpected call: CT_LbsHybridUEAssistedMTLRNW::ProcessNetworkPositionUpdate(GPSLocation)"));	
		// A second call to this callback should never occur - in this test case the GPS module
		// is never able to produce a fix of sufficient quality - only positions of
		// sufficient quality should cause this function to be called
		TEST(EFalse);
		}
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedMTLRNW::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRNW::ProcessRequestComplete()"));
	TEST(iState==ERefLocReceived);
	iState=ERequestComplete; 
	ReturnToTestStep();
	}




