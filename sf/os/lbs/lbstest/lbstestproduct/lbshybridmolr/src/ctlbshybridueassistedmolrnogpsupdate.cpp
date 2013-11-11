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
// This is the class implementation for the MO-LR - scenario Tests
// 
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbslocerrors.h>

// LBS test includes.
#include "ctlbshybridueassistedmolrnogpsupdate.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

/**
Static Constructor
*/
CT_LbsHybridUEAssistedMOLRNoGPSUpdate* CT_LbsHybridUEAssistedMOLRNoGPSUpdate::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedMOLRNoGPSUpdate* testStep = new CT_LbsHybridUEAssistedMOLRNoGPSUpdate(aParent);
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
CT_LbsHybridUEAssistedMOLRNoGPSUpdate::CT_LbsHybridUEAssistedMOLRNoGPSUpdate(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssistedMOLRNoGPSUpdate);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsHybridUEAssistedMOLRNoGPSUpdate::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}


/**
 * Destructor
 */
CT_LbsHybridUEAssistedMOLRNoGPSUpdate::~CT_LbsHybridUEAssistedMOLRNoGPSUpdate()
	{
	}

// Hybrid - UE Assisted MO-LR No GPS Update during time t.

TVerdict CT_LbsHybridUEAssistedMOLRNoGPSUpdate::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedMOLRNoGPSUpdate::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

	// waiting for >> AdviceSystemStatus(0) - GetCurrentCapabilitiesResponse
	INFO_PRINTF1(_L("waiting for << NotifyPositionUpdate()"));
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	//  >> AdviceSystemStatus(0) - GetCurrentCapabilitiesResponse
	INFO_PRINTF1(_L("<< NotifyPositionUpdate()"));


	// Start Test Step
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	
	INFO_PRINTF1(_L("Connected to server"));

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);
	INFO_PRINTF1(_L("Subsession created"));


	// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
	// after 30 secs the postion request will be timed out
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(30* 1000 * 1000));

	pos.SetUpdateOptions(posOpts);
	
	INFO_PRINTF1(_L("Setupdate options timeout set to 30 seconds"));

	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

	// Request a self-locate MoLr.
	pWatch->IssueNotifyPositionUpdate();
	INFO_PRINTF1(_L("<< NotifyPositionUpdate()"));
	// >> RequestSelfLocation()
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);
	INFO_PRINTF1(_L(">> RequestSelfLocation()"));	

	// Process the response.
	TLbsNetSessionId* 					sessionId = NULL;
	TLbsNetPosRequestOptionsAssistance*	opts = NULL;
	TInt								cleanupCnt;
	
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);
	
	iSessionId.SetSessionNum(sessionId->SessionNum());
	iSessionId.SetSessionOwner(sessionId->SessionOwner());

	CleanupStack::PopAndDestroy(cleanupCnt);
	sessionId = NULL;
	opts = NULL;	
	
	// << ProcessStatusUpdate(EServiceSelfLocation)
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// << ProcessLocationUpdate(SessionId, RefPosition)
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);
	INFO_PRINTF1(_L("<< ProcessLocationUpdate(SessionId, RefPosition)"));	


	// TEST: Get the ref pos app side.
	CheckForObserverEventTestsL(KTimeOut, *this);

	// << ProcessAssistanceData()
	TLbsAsistanceDataGroup dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); // assistanceData
	INFO_PRINTF1(_L("<< ProcessAssistanceData()"));	


	// << ProcessLocationRequest(SessionId, HybridMode, alpha2)
	TBool emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality quality = ArgUtils::Quality(); 
	quality.SetMaxFixTime(ArgUtils::Alpha2());
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	INFO_PRINTF1(_L("<< ProcessLocationRequest(SessionId, HybridMode, alpha2)"));	

	// Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
	TTime startTime;
	startTime.HomeTime();

	// >> RequestAssistanceData(0)
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	INFO_PRINTF1(_L(">> RequestAssistanceData(0)"));	

	CleanupStack::PopAndDestroy(proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask));
	TESTL(dataMask == EAssistanceDataNone);

	// << NotifyPositionUpdate()
	pWatch->IssueNotifyPositionUpdate();
	INFO_PRINTF1(_L("<< NotifyPositionUpdate()"));	

	// >> RequestAssistanceData(0)
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	CleanupStack::PopAndDestroy(proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask));
	TESTL(dataMask == EAssistanceDataNone);
	INFO_PRINTF1(_L(">> RequestAssistanceData(0)"));	


	// Determine the value to take off the alpha2 value. This is required because we had to wait for the assistance data response.
	TTimeIntervalMicroSeconds microseconds;
	TTime stopTime;
	stopTime.HomeTime();
	microseconds = stopTime.MicroSecondsFrom(startTime); 
	TInt64 timeElapsed = microseconds.Int64();
	TInt delta = 2 * 1000 * 1000; // 2 secs.

	// >> RespondLocationRequest()
	TESTL(proxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - delta) == ENetMsgTimeoutExpired);
	INFO_PRINTF1(_L(">> RespondLocationRequest()"));	

	// Wait for and process the response.
	TESTL(proxy->WaitForResponse(2 * delta) == ENetMsgRespondLocationRequest);
	INFO_PRINTF1(_L("Wait for and process the response"));	

	sessionId = NULL;
	TPositionGpsMeasurementInfo* measurementInfo = NULL;
	cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrNone);
	CleanupStack::PopAndDestroy(cleanupCnt); //sessionId, measurementInfo

	// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
	//									max fix time timer expries.
	INFO_PRINTF1(_L("Wait for ENetMsgRequestAssistanceData"));	

	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	INFO_PRINTF1(_L("Got ENetMsgRequestAssistanceData"));	

	proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);

	const TInt t = 4 * 1000 * 1000; // 4 secs, hybrid A-GPS module will deliver update after 6 + 1 seconds.
	quality.SetMaxFixTime(t);

	// << ProcessLocationRequest(SessionId, HybridMode, t)
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	INFO_PRINTF1(_L("<< ProcessLocationRequest(SessionId, HybridMode, t)"));	

	// >> RequestAssistanceData(0)
	INFO_PRINTF1(_L("Wait for >> RequestAssistanceData(0) again"));	

	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	INFO_PRINTF1(_L(">> RequestAssistanceData(0)"));	

	proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);

	// >> RespondLocationRequest().
	TESTL(proxy->WaitForResponse(t + delta) == ENetMsgRespondLocationRequest);
	INFO_PRINTF1(_L(">> RespondLocationRequest()"));	

	// Process response. Note measurement data is un-defined (and not verified) when reason contains
	// an error, which is expected here.
	sessionId = NULL;
	measurementInfo = NULL;
	cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);			
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrPositionNoGpsUpdate);
	CleanupStack::PopAndDestroy(cleanupCnt);// sessionId, measurementInfo/positionInfo
	
	// >> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
	//									max fix time timer expries.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData);
	INFO_PRINTF1(_L(">>> RequestAssistanceData"));	

	proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
		
	// << ProcessSessionComplete(SessionId, KErrNone)	
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	INFO_PRINTF1(_L("<< ProcessSessionComplete(SessionId, KErrNone)"));	


	// << ENetMsgProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);
	INFO_PRINTF1(_L("<< ENetMsgProcessStatusUpdate()"));	

	
	// Wait for the LBS responses due to the Session Complete. NPUD still outstanding so resend 
	// requests for SelfLocation and we may also get a request for Assistance Data. This is timing 
	// related since the PM's SessionComplete is sent to the AGPS Manager and the NRH, and the NRH
	// then sends a cancel message to the AGPS manager.  These events trigger active objects and
	// both AOs will make a RequestLocationUpdate call to the Int. Module.  Another AO in the Int.
	// module will respond with an assistance data request.  However, depending on timing, the 2nd
	// call to RequestLocationUpdate may cancel the AO set off by the 1st call.  Thus we may get 1
	// (most likely) or 2 assistance data requests.  These will result in a SelfLocation request 
	// always being sent to the PM but sometimes the PM may get a request for Assistance Data.
		
	// Some time later (imperically < 20 secs but could be worked out by going through the test code)
	// the NPUD maxFixTime timer will fire which will cause the SelfLocation request to be cancelled.
	
	// Sometimes it can happen that the session complete arrives quickly enough and no 
	// further response is given by LBS. This was seen in SMP.
		
		TInt selfLocationCount = 0;
		TInt assistanceDataCount = 0;
		TInt cancelCount = 0;
		TInt othersCount = 0;
		TBool noResponse(EFalse);
		TNetProtocolResponseType response = ENetMsgNullResponse;
		
		while ((cancelCount == 0) && (othersCount == 0) && !noResponse)
			{
			response = proxy->WaitForResponse(20*1000*1000);  // wait for a max of 20secs
			switch (response)
				{
				case ENetMsgTimeoutExpired:
					{
					noResponse = ETrue;
					INFO_PRINTF1(_L(">> No further response was received"));
					break;
					}
				case ENetMsgCancelSelfLocation:
					{
					cancelCount++;
					INFO_PRINTF1(_L(">> Wait for CancelSelfLocation"));	
					break;
					}
				case ENetMsgRequestSelfLocation:
					{
					selfLocationCount++;
					INFO_PRINTF1(_L(">> Wait for RequestSelfLocation"));	
					break;
					}
				case ENetMsgRequestAssistanceData:
					{
					assistanceDataCount++;
					INFO_PRINTF1(_L(">> Wait for RequestAssistanceData"));	
					break;
					}
				default:
					{
					othersCount++;
					INFO_PRINTF2(_L(">> Unexpected message %d"), response);	
					break;
					}
				}
			}
		if(!noResponse)
			{ // these tests should be done only if there was a response received
			TESTL(othersCount == 0);
			TESTL(selfLocationCount == 1);
			TESTL(cancelCount == 1);
			}
		
	CheckForObserverEventTestsL(KTimeOut, *this);

	// Listen for 10 seconds that there are note further messages
    TESTL(proxy->WaitForResponse(10*1000*1000) == ENetMsgTimeoutExpired);

	// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);

	INFO_PRINTF1(_L("All DONE!"));	
	
	return TestStepResult();
	
	
	}


// MPosServerObserver
void CT_LbsHybridUEAssistedMOLRNoGPSUpdate::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedMOLRNoGPSUpdate::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	INFO_PRINTF1(_L("RunL() - OnNotifyPositionUpdate()"));


	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		// Verify error.
		TEST(aErr == KErrNone);

		iState = ERefLocReceived;

		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		INFO_PRINTF1(_L("RunL() - Got ref location"));

		}
	
	else
		{
		// expecting a position but not of the requitred quality
		INFO_PRINTF2(_L("RunL() - second %d"), aErr);
		if(iExpectedApiBehaviour == EApiVariant2)
			{ // for variant 2 behaviour
			TEST(aErr == KErrTimedOut);
			}
		else
			{
			TEST(aErr == KPositionQualityLoss);
			}

		TEST(ETrue);
		}

	ReturnToTestStep();
	}
