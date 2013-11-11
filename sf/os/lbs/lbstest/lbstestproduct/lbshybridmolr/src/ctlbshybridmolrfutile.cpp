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
// This is the class implementation for the Hybrid MO-LR Futile UE-Based and UE-Assisted Test scenario
// 
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbshybridmolrfutile.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

const TInt KShorterMaxFixTime = 30*1000*1000;

/**
Static Constructor
*/
CT_LbsHybridMOLRFutile* CT_LbsHybridMOLRFutile::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridMOLRFutile* testStep = new CT_LbsHybridMOLRFutile(aParent);
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
CT_LbsHybridMOLRFutile::CT_LbsHybridMOLRFutile(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsHybridMOLRFutile);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsHybridMOLRFutile::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}


/**
 * Destructor
 */
CT_LbsHybridMOLRFutile::~CT_LbsHybridMOLRFutile()
	{
	}

/** 
 * HYBRID MOLR FUTILE SCENARIOS:
 * 	Test Case Id 1: Hybrid LBS MOLR UE Based - Futile
 * 	Test Case Id 2: Hybrid LBS MOLR UE Assisted - Futile
 * 	Test Case Id 3: Hybrid LBS MOLR UE Assisted - Futile - Facility Timeout
 * 	Test Case Id 4: LBS MOLR UE Based - Futile
*/
TVerdict CT_LbsHybridMOLRFutile::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridMOLRFutile::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Carryout unique test actions.
	if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
		{
		INFO_PRINTF2(_L("Test id %d."), iTestCaseId);
		
		// Create Network Protocol Proxy
		CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
		CleanupStack::PushL(proxy);
		TInt cleanupCnt;

// Comment out while lbs is restarted in this test's script
//		if (iTestCaseId == 1)
			{
			// >> AdviceSystemStatus(0)
			INFO_PRINTF1(_L("Wait for AdviceSystemStatus"));
			TNetProtocolResponseType type = proxy->WaitForResponse(KTimeOut);
			INFO_PRINTF2(_L("returned from WaitForResponse %d"), type);

			TESTL(type == ENetMsgGetCurrentCapabilitiesResponse);
			CLbsNetworkProtocolBase::TLbsSystemStatus status;
			cleanupCnt = proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
			TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
			CleanupStack::PopAndDestroy(cleanupCnt);
			}

		// Start Test Step
		RPositionServer server;
		TESTL(KErrNone == server.Connect());
		CleanupClosePushL(server);	

		RPositioner pos;
		TESTL(KErrNone == pos.Open(server));
		CleanupClosePushL(pos);

		TInt maxFixTime;

		if (iTestCaseId == 3 && iExpectedApiBehaviour == EApiVariant2)
			{
			// Set the max fix time for the client request to be shorter to ensure the location server times out before the test
			maxFixTime = KShorterMaxFixTime;
			}
		else
			{
			// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
			maxFixTime = KDefaultMaxFixTime;
			}
		TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(maxFixTime));
		pos.SetUpdateOptions(posOpts);

		CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

		// Request a self-locate MoLr.
		pWatch->IssueNotifyPositionUpdate();
		
		// >> RequestSelfLocation()
		TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

		// Process the response.
		TLbsNetSessionId* 					sessionId = NULL;
		TLbsNetPosRequestOptionsAssistance*	opts = NULL;
	
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);
		
		TBool qualitycheck = ArgUtils::CompareQuality(	opts, 
														ETrue, 
														KMinHorizontalAcc, 
														KMinVerticalAcc, 
														maxFixTime,
														0, 
														EAssistanceDataReferenceTime, 
														(TPositionModuleInfo::ETechnologyTerminal 
														| TPositionModuleInfo::ETechnologyAssisted)
													);
		TESTL(qualitycheck);
		
		iSessionId.SetSessionNum(sessionId->SessionNum());
		iSessionId.SetSessionOwner(sessionId->SessionOwner());

		CleanupStack::PopAndDestroy(cleanupCnt);
		sessionId = NULL;
		opts = NULL;	
		
		// << ProcessStatusUpdate(EServiceSelfLocation)
		MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
		proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

		// << ProcessLocationUpdate(SessionId, RefPosition)
		TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
		proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

		// TEST: Get the ref pos app side.
		CheckForObserverEventTestsL(KTimeOut, *this);

		// << ProcessAssistanceData()
		TLbsAsistanceDataGroup dataMask = EAssistanceDataReferenceTime;
		RLbsAssistanceDataBuilderSet assistanceData;
		ArgUtils::PopulateLC(assistanceData);
		TInt reason = KErrNone;
		proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
		CleanupStack::PopAndDestroy(); // assistanceData

		// << ProcessLocationRequest(SessionId, HybridMode, alpha2)
		TBool emergency = EFalse;
		MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
		TLbsNetPosRequestQuality quality = ArgUtils::Quality(); 
		quality.SetMaxFixTime(ArgUtils::Alpha2());
		TLbsNetPosRequestMethod method;
		if (iTestCaseId == 4)
			{
 			method = ArgUtils::RequestMethod();
			}
		else
			{
			method = ArgUtils::RequestHybridMethod();
			}
		proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

		// Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
		TTime startTime;
		startTime.HomeTime();

		// >> RequestAssistanceData(0)
		TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);

		// << NotifyPositionUpdate()
		pWatch->IssueNotifyPositionUpdate();

		// >> RequestAssistanceData(0)
		TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);

		TPositionGpsMeasurementInfo* measurementInfo = NULL;
			
		if ((iTestCaseId == 2) || (iTestCaseId == 3))
			{
			//Find the time elapsed from timer
			TTimeIntervalMicroSeconds microseconds;
			TTime timerStop;
			timerStop.HomeTime();
			microseconds = timerStop.MicroSecondsFrom(startTime); 
			TInt64 timeElapsed = microseconds.Int64();
			TInt delta = 2 * 1000 * 1000; // 2 s
				
			// >> RespondLocationRequest()
			// Test that we do not get response before alpha2 has expired
			TESTL(proxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - delta) == ENetMsgTimeoutExpired); 

			// Wait for and process the response.
			TESTL(proxy->WaitForResponse(2 * delta) == ENetMsgRespondLocationRequest); 

			sessionId = NULL;
			reason = KErrNone;
			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(reason == KErrNone); 
			// Check it is a measurement
			TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
			CleanupStack::PopAndDestroy(cleanupCnt); //sessionId, measurementInfo

			// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager reissuing a location request when it's
			//									max fix time timer expries.
			TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
			cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
			TESTL(dataMask == EAssistanceDataNone);
			CleanupStack::PopAndDestroy(cleanupCnt);

			const TInt t = 8 * 1000 * 1000; // 8 secs.
			quality.SetMaxFixTime(t);

			// << ProcessLocationRequest(SessionId, HybridMode, t)
			proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

			// >> RequestAssistanceData(0)
			TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
			cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
			TESTL(dataMask == EAssistanceDataNone);
			CleanupStack::PopAndDestroy(cleanupCnt);
			}
			
		// >> RespondLocationRequest()
		// Error returned by module should be KPositionCalculationFutile
		// the alpha or t timeout
		if ((iTestCaseId == 1) || (iTestCaseId == 4))
			{
			TESTL(proxy->WaitForResponse(ArgUtils::Alpha2()) == ENetMsgRespondLocationRequest); 
			}
		else if ((iTestCaseId == 2) || (iTestCaseId == 3))
			{
			TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 
			}
		sessionId = NULL;
		reason = KErrNone;
		if ((iTestCaseId == 1) || (iTestCaseId == 2) || (iTestCaseId == 3))
			{
			// Check it is a measurement
			measurementInfo = NULL;
			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
			TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
			}
		else if (iTestCaseId == 4)
			{
			// check it is a position
			TPositionInfo* posInfo = NULL;
			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &posInfo);
			TESTL(posInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
			}
		TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
		TESTL(reason == KPositionCalculationFutile);
		CleanupStack::PopAndDestroy(cleanupCnt); //sessionId, measurementInfo\positionInfo

		if ((iTestCaseId == 1) || (iTestCaseId == 2) || (iTestCaseId == 4))
			{
			// <<  ProcessLocationUpdate - return network calculated pos
			TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
			proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);		

			// TEST: Get the final network position	
			CheckForObserverEventTestsL(KTimeOut, *this);		
			TESTL(iState == EFnNwLocReceived);

			// << ProcessSessionComplete(SessionId, KErrNone)	
			reason = KErrNone;
			proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

			// << ENetMsgProcessStatusUpdate()
			serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
			proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);
			
			// >> CancelSelfLocation.
			TInt KTimeout = 10 * 1000000; // 10 seconds
			TESTL(proxy->WaitForResponse(KTimeout) == ENetMsgCancelSelfLocation);
			
			// Wait for 10 seconds to ensure no additional responses turn up.
			// Note that after a session complete an assistance dtaat request may or
			// may not arrive ... depending on timings
			TInt delta = 10 * 1000 * 1000;
			TNetProtocolResponseType mType = proxy->WaitForResponse(delta,EResponseFilterMsgRequestAssistanceData);
			TESTL(mType == ENetMsgTimeoutExpired);

			}
		else if (iTestCaseId == 3)
			{			
		    // Simulate Facility wait timer expiring
		    TInt KTimeout = 10 * 1000000; // 10 seconds
		    TESTL(proxy->WaitForResponse(KTimeout) == ENetMsgTimeoutExpired);

			// << ProcessSessionComplete(SessionId, KErrPositionSystemFailure)	
			reason = KErrTimedOut;
			proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

			// << ENetMsgProcessStatusUpdate()
			serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
			proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

			// Client recv - the gps position determined by the gps module.
			CheckForObserverEventTestsL(KTimeOut, *this);
			TESTL(iState == EGpsLocReceived);

			// Listen for 10 seconds that there are no further messages
		    TESTL(proxy->WaitForResponse(KTimeout) == ENetMsgTimeoutExpired);
			}

	

	// Done. Now cleanup...
		CleanupStack::PopAndDestroy(pWatch);
		CleanupStack::PopAndDestroy(); // pos
		CleanupStack::PopAndDestroy(); // server	
		CleanupStack::PopAndDestroy(proxy);
		}
	return TestStepResult();
	}


// MPosServerObserver
void CT_LbsHybridMOLRFutile::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsHybridMOLRFutile::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	
	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		// Verify error.
		TEST(aErr == KErrNone);
		
		iState = ERefLocReceived;

		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);

		// check for nan in ref positions
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Expecting network pos 
	else if (iState == ERefLocReceived)
		{
		// Verify error.
		if(iExpectedApiBehaviour == EApiVariant2)
			{
			if(iTestCaseId != 3)
				{
				TEST(aErr == KErrNone);
				}
			else
				{
				TEST(aErr == KPositionQualityLoss);
				}
			}
		else
			{
			TEST(aErr == KPositionQualityLoss);
			}

		if ((iTestCaseId == 1) || (iTestCaseId == 2) || (iTestCaseId == 4))
			{
			iState = EFnNwLocReceived;
			TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
			}
		else if (iTestCaseId == 3)
			{
			iState = EGpsLocReceived;
			if(iExpectedApiBehaviour == EApiVariant1)// Note that on Variant2 position is not filled in when KPositionQualityLoss returned, so we don't check any details
				{
				TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
				}
			}
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}
