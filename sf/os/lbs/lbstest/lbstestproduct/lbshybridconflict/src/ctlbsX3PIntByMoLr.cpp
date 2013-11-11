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
// ctlbsMoLrIntByX3P.cpp
// This is the class implementation for the Mo-Lr Interrupted By X3P.cpp Result scenario Tests
// 
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbsX3PIntByMoLr.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


const TInt KN(2); // Number of times to send the measurement from GPS to NW

/**
Static Constructor
*/
CT_LbsX3PIntByMoLr* CT_LbsX3PIntByMoLr::New(CT_LbsHybridConflictServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsX3PIntByMoLr* testStep = new CT_LbsX3PIntByMoLr(aParent);
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
CT_LbsX3PIntByMoLr::CT_LbsX3PIntByMoLr(CT_LbsHybridConflictServer& aParent) : CT_LbsHybridConflictStep(aParent)
	{
	SetTestStepName(KLbsX3PIntByMoLr);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsX3PIntByMoLr::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridConflictStep::ConstructL();
	}


/**
 * Destructor
 */
CT_LbsX3PIntByMoLr::~CT_LbsX3PIntByMoLr()
	{
	}

// Hybrid - UE Assisted MO-LR GPS ok

TVerdict CT_LbsX3PIntByMoLr::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsX3PIntByMoLr::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

	// >> AdviceSystemStatus(0) - GetCurrentCapabilitiesResponse
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	// Start Test Step
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);


	// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(240000000));
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
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
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
	// Determine the value to take off the alpha2 value. This is required because we had to wait for the assistance data response.
	TTimeIntervalMicroSeconds microseconds;
	TTime stopTime;
	stopTime.HomeTime();
	microseconds = stopTime.MicroSecondsFrom(startTime); 
	TInt64 timeElapsed = microseconds.Int64();
	TInt delta = 2 * 1000 * 1000; // 2 secs.

	// >> RespondLocationRequest()
	TESTL(proxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - delta) == ENetMsgTimeoutExpired);

	// Wait for and process the response.
	TESTL(proxy->WaitForResponse(2 * delta) == ENetMsgRespondLocationRequest);		// DONT get because the measurement data bus has not been created...

	sessionId = NULL;
	TPositionGpsMeasurementInfo* measurementInfo = NULL;
	cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrNone);
	CleanupStack::PopAndDestroy(cleanupCnt);//sessionId, measurementInfo

	// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
	//									max fix time timer expries.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	const TInt t = 8 * 1000 * 1000; // 8 secs.
	quality.SetMaxFixTime(t);

	TPositionInfo* positionInfo = NULL;
	
	for (TInt i = 0; i < KN; i++)
		{
		// << ProcessLocationRequest(SessionId, HybridMode, t)
		proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

		// >> RequestAssistanceData(0)
		TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);

		// >> RespondLocationRequest() - first measurement, second position.
		TESTL(proxy->WaitForResponse(t + delta) == ENetMsgRespondLocationRequest);

		sessionId = NULL;
		
		// Expect measurement first time.
		if (i < (KN-1))
			{
			measurementInfo = NULL;
			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);			

			// Check it is measurement
			TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);

			// >> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
			//									max fix time timer expries.
			TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData);
			
			cleanupCnt += proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);

			TESTL(dataMask == EAssistanceDataNone);
			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(reason == KErrNone);
			}
		
		// Expect position second time.
		else
			{
			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
			
			// check it is a position
			TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));

			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(reason == KErrNone);
			
			// Client recv - the gps position determined by the gps module.
			CheckForObserverEventTestsL(KTimeOut, *this);
			TESTL(iState == EGpsLocReceived);
			
			// << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
			// Return modules' position as FinalNetworkPosition
			proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, positionInfo);
			}

		CleanupStack::PopAndDestroy(cleanupCnt);// sessionId, measurementInfo/positionInfo
		}
		

	// << ProcessSessionComplete(SessionId, KErrNone)	
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ENetMsgProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);
	

// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);
		
	return TestStepResult();
	}


// MPosServerObserver
void CT_LbsX3PIntByMoLr::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsX3PIntByMoLr::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify error.
	TEST(aErr == KErrNone);

	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;

		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		}
		
	// Expecting network pos.
	else if (iState == ERefLocReceived)
		{
		iState = EGpsLocReceived;

		TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}
