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
// This is the class implementation for the Hybrid MoLr Tests
// 
//


// EPOC includes.
#include <e32math.h>

// LBS includes. 
#include <lbssatellite.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbsnetprotocolbase.h>

// LBS test includes.
#include "ctlbsuebasedmolrself.h"
#include "ctlbsueassistedmolrselfgpsinaccuratetimeout.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

/**
Static Constructor
*/
CT_LbsUEAssistedMOLRSelfGPSInaccurateT* CT_LbsUEAssistedMOLRSelfGPSInaccurateT::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsUEAssistedMOLRSelfGPSInaccurateT* testStep = new CT_LbsUEAssistedMOLRSelfGPSInaccurateT(aParent);
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
CT_LbsUEAssistedMOLRSelfGPSInaccurateT::CT_LbsUEAssistedMOLRSelfGPSInaccurateT(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsUEAssistedMOLRSelfGPSInaccurateT);
	
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
		
	iState = EInitializing;
	}


void CT_LbsUEAssistedMOLRSelfGPSInaccurateT::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsUEAssistedMOLRSelfGPSInaccurateT::~CT_LbsUEAssistedMOLRSelfGPSInaccurateT()
	{
	}


TVerdict CT_LbsUEAssistedMOLRSelfGPSInaccurateT::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsUEAssistedMOLRSelfGPSInaccurateT::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

// Recv --> GetCurrentCapabilitiesResponse.
	// Soak up the Lbs System Status (produced by A-GPS Manager startup). 
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Do the /actual/ test step!

	// Setup location session and position watcher.
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);

	// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
	//TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(240*1000*1000));
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(30*1000*1000));
	pos.SetUpdateOptions(posOpts);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);


// Client Send - a self-locate request.
	pWatch->IssueNotifyPositionUpdate();
	INFO_PRINTF1(_L("<< First NotifyPositionUpdate()"));


// Recv --> RequestSelfLocation.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);
	INFO_PRINTF1(_L(">> Wait for RequestSelfLocation()"));	

	// Process the response.
	TLbsNetSessionId* 					sessionId = NULL;
	TLbsNetPosRequestOptionsAssistance*	opts = NULL;
	
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

	TBool qualitycheck = ArgUtils::CompareQuality(	opts, 
													ETrue, 
													KMinHorizontalAcc, 
													KMinVerticalAcc, 
													30*1000*1000,
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

		
// Send <-- ProcessStatusUpdate.
	MLbsNetworkProtocolObserver::TLbsNetProtocolService serviceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

	
// Send <-- ProcessLocationUpdate.
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);
	INFO_PRINTF1(_L("<< ProcessLocationUpdate(SessionId, RefPosition)"));	
	
// Client recv - the ref position app side.
	CheckForObserverEventTestsL(KTimeOut, *this);


// Send <-- ProcessAssistanceData.
	TLbsAsistanceDataGroup			dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet	data;
	TInt							reason = KErrNone;
	ArgUtils::PopulateLC(data);
	proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &data, &reason);
	CleanupStack::PopAndDestroy(); // data
	INFO_PRINTF1(_L("<< ProcessAssistanceData()"));	


// Send <-- ProcessLocationRequest.
	TBool												emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality							quality = ArgUtils::Quality();
	TLbsNetPosRequestMethod								method = ArgUtils::RequestHybridMethod();
	quality.SetMaxFixTime(ArgUtils::Alpha2());
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	INFO_PRINTF1(_L("<< ProcessLocationRequest(SessionId, HybridMode, alpha2)"));	

	// Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
	TTime startTime;
	startTime.HomeTime();


// Recv --> RequestAssistanceData.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	INFO_PRINTF1(_L(">> Wait for RequestAssistanceData(0)"));	
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Client Send -  a self-locate request.
	pWatch->IssueNotifyPositionUpdate();
	INFO_PRINTF1(_L("<< Issue second NotifyPositionUpdate()"));

// Rev --> RequestAssistanceData - we get this as the result of client notify update request.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	INFO_PRINTF1(_L(">> Wait for RequestAssistanceData(0)"));	
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// Determine the value to take off the alpha2 value. This is required because we had to wait for the assistance data response.
	TTimeIntervalMicroSeconds microseconds;
	TTime stopTime;
	stopTime.HomeTime();
	microseconds = stopTime.MicroSecondsFrom(startTime); 
	TInt64 timeElapsed = microseconds.Int64();


// Recv --> RespondLocationRequest.
	// First ensure we don't recv response before Alpha2.
	TInt delta = 2 * 1000 * 1000; // 2 secs.
	TESTL(proxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - delta) == ENetMsgTimeoutExpired);
	
	// Wait for and process the response.
	TESTL(proxy->WaitForResponse(2 * delta) == ENetMsgRespondLocationRequest);
	INFO_PRINTF1(_L(">> Wait for RespondLocationRequest()"));	

	TPositionGpsMeasurementInfo* measurementInfo = NULL;
	cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrNone);
	TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
	
	CleanupStack::PopAndDestroy(cleanupCnt);
	sessionId = NULL;
	measurementInfo = NULL;


// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
//									max fix time timer expries.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	INFO_PRINTF1(_L(">> Wait for RequestAssistanceData(0)"));	
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	const TInt t = 8 * 1000 * 1000; // 8 secs.
	quality.SetMaxFixTime(t);
	
	for (TInt i = 0; i < 2; i++)
		{
// Send <-- ProcessLocationRequest - time t.
		proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
		INFO_PRINTF1(_L("<< ProcessLocationRequest(SessionId, HybridMode, t)"));	

// Recv --> RequestAssistanceData.
		TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		INFO_PRINTF1(_L(">> Wait for RequestAssistanceData(0)"));	
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
		
		// Expect measurement first time.
		if (i == 0)
			{
			measurementInfo = NULL;

// Recv --> RespondLocationRequest.
			TESTL(proxy->WaitForResponse(t + delta) == ENetMsgRespondLocationRequest);
			INFO_PRINTF1(_L(">> Wait for RespondLocationRequest()"));	

			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);			

			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(reason == KErrNone);
	
			// Check it is measurement and check contents
			TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);

			CleanupStack::PopAndDestroy(cleanupCnt);
			sessionId = NULL;
			measurementInfo = NULL;


// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
//									max fix time timer expries.
			TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
			INFO_PRINTF1(_L(">> Wait for RequestAssistanceData(0)"));	
			cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
			TESTL(dataMask == EAssistanceDataNone);
			CleanupStack::PopAndDestroy(cleanupCnt);
			}
		
		// Expect position second time.
		// second time we don't get response...

		else
			{
			// we may have to let the module return a pos
			TESTL(proxy->WaitForResponse(delta) == ENetMsgTimeoutExpired);
			INFO_PRINTF1(_L(">> Wait for ENetMsgTimeoutExpired"));	
			}
		}

	// At this point we're waiting for the ENetMsgRespondLocationRequest message but we get the
	// protocol module to simulate a timeout by ...
	
// Send <-- ProcessSessionComplete.
	reason = KErrTimedOut;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	INFO_PRINTF1(_L("<< ProcessSessionComplete(SessionId, KErrNone)"));	

// Send <-- ProcessStatusUpdate.
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
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
	
// Some time later (imperically < 10 secs but could be worked out by going through the test code)
// the NPUD maxFixTime timer will fire which will cause the SelfLocation request to be cancelled.
	
	TInt selfLocationCount = 0;
	TInt assistanceDataCount = 0;
	TInt cancelCount = 0;
	TInt othersCount = 0;
	TNetProtocolResponseType response = ENetMsgNullResponse;
	
	while ((cancelCount == 0) && (othersCount == 0))
		{
		response = proxy->WaitForResponse(10*1000*1000);  // wait for a max of 10secs
		switch (response)
			{
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
	TESTL(othersCount == 0);
	TESTL(selfLocationCount == 1);
	TESTL(cancelCount == 1);
	
	
	CheckForObserverEventTestsL(10*1000*1000, *this);
	
	// Listen for 10 seconds that there are note further messages
    TESTL(proxy->WaitForResponse(10*1000*1000) == ENetMsgTimeoutExpired);


// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);
		
	return TestStepResult();
	}


// MPosServerObserver
void CT_LbsUEAssistedMOLRSelfGPSInaccurateT::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsUEAssistedMOLRSelfGPSInaccurateT::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	INFO_PRINTF1(_L("RunL() - OnNotifyPositionUpdate()"));

	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;

		TEST(aErr == KErrNone);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		
		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Expecting network pos.
	else if (iState == ERefLocReceived)
		{
		INFO_PRINTF2(_L("RunL() - second %d"), aErr);
		iState = ENetworkLocReceived;

		if(iExpectedApiBehaviour == EApiVariant2)
			{
			TEST(aErr == KErrNone);
			}
		else
			{
			TEST(aErr == KPositionQualityLoss);
			}
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);	
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}

