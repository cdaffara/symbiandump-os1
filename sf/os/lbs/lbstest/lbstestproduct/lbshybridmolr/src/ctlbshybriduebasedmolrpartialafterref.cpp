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
// This is the class implementation for the MO-LR - Accurate GPS Result scenario Tests
// 
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbshybriduebasedmolrpartialafterref.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsHybridUEBasedMOLRPartialAfterRef* CT_LbsHybridUEBasedMOLRPartialAfterRef::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEBasedMOLRPartialAfterRef* testStep = new CT_LbsHybridUEBasedMOLRPartialAfterRef(aParent);
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
CT_LbsHybridUEBasedMOLRPartialAfterRef::CT_LbsHybridUEBasedMOLRPartialAfterRef(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEBasedMOLRPartialAfterRef);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsHybridUEBasedMOLRPartialAfterRef::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}


/**
 * Destructor
 */
CT_LbsHybridUEBasedMOLRPartialAfterRef::~CT_LbsHybridUEBasedMOLRPartialAfterRef()
	{
	}


TVerdict CT_LbsHybridUEBasedMOLRPartialAfterRef::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEBasedMOLRPartialAfterRef::doTestStepL()"));	
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
	
	// Start Actual Test Step
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);

	// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KMOLRFixTime), 0 , ETrue);
	pos.SetUpdateOptions(posOpts);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);


	// Client Send: Request a self-locate MoLr.
	pWatch->IssueNotifyPositionUpdate();


	//Recv: RequestSelfLocation()
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

	// Process the response.
	TLbsNetSessionId* 					sessionId = NULL;
	TLbsNetPosRequestOptionsAssistance*	opts = NULL;
	
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

	TBool qualitycheck = ArgUtils::CompareQuality(	opts, 
													ETrue, 
													KMinHorizontalAcc, 
													KMinVerticalAcc, 
													KMOLRFixTime,
													0, 
													EAssistanceDataReferenceTime, 
													(TPositionModuleInfo::ETechnologyTerminal 
													| TPositionModuleInfo::ETechnologyAssisted)
												);					
	TESTL(qualitycheck);
				
	iSessionId.SetSessionNum(sessionId->SessionNum());
	iSessionId.SetSessionOwner(sessionId->SessionOwner());

	CleanupStack::PopAndDestroy(cleanupCnt);
	
	//Send: ProcessStatusUpdate(EServiceSelfLocation)
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);



	// Client Recv: Get the ref pos app side.
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == EGpsPartialInitReceived);


	// Client Send: Request a self-locate MoLr.
	pWatch->IssueNotifyPositionUpdate();


	//Send: ProcessLocationUpdate(SessionId, RefPosition)
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

	// Client Recv: Get Nan AGPS position.
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == ERefLocReceived);


	//Send: ProcessAssistanceData()
	TLbsAsistanceDataGroup dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy();


	//Send: ProcessLocationRequest(SessionId, HybridMode, alpha2)
	TBool emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality quality = ArgUtils::Quality(); 
	quality.SetMaxFixTime(ArgUtils::Alpha2());
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

	// Recv --> RequestAssistanceData, as the result of the NRH request.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	//Client Send: Notify Position Update
	pWatch->IssueNotifyPositionUpdate();

	//Recv: RequestAssistanceData(Mask=0) - as a result of location server request
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	//Client Send: CompleteRequest
	TESTL(pos.CompleteRequest(EPositionerNotifyPositionUpdate) == KErrNone);
	
	// Client Recv: Second Partial update position - This should return with KPositionEarlyComplete
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == ERefPartialEarlyReceived);

	//Recv: Wait for and process the response.
	// ignoring any request for assistance data that we may receive
	// because depending on timings  we may or may not get these 
	TESTL(proxy->WaitForResponse(KTimeOut,EResponseFilterMsgRequestAssistanceData) == ENetMsgRespondLocationRequest);

	TPositionInfo* positionInfo = NULL;
	sessionId = NULL;
	//TPositionGpsMeasurementInfo* measurementInfo = NULL;
	cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
	TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass));
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	
	//Send: ProcessLocationUpdate(SessionId, FinalNetworkPosition)
	TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);


	//Send: ProcessSessionComplete(SessionId, KErrNone)	
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);


	//Send: ENetMsgProcessStatusUpdate()
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);
	
	// now wait and check that we don't get any unexpected messages
	// discarding assistance data requests
	TESTL(proxy->WaitForResponse(KTimeOut,EResponseFilterMsgRequestAssistanceData) == ENetMsgTimeoutExpired);

	//Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);
		
	return TestStepResult();
	}


// MPosServerObserver
void CT_LbsHybridUEBasedMOLRPartialAfterRef::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsHybridUEBasedMOLRPartialAfterRef::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting first partial update, NaN position.
	if (iState == EInitializing)
		{
		iState = EGpsPartialInitReceived;
		
		TEST(aErr == KPositionPartialUpdate);
		TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));

		// check for nan in partial updates
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToNan(posInfo));
		}

	// Expecting ref pos.
	else if (iState == EGpsPartialInitReceived)
		{
		iState = ERefLocReceived;

		TEST(aErr == KPositionPartialUpdate);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		
		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Expecting first partial gps position.
	else if (iState == ERefLocReceived)
		{
		iState = ERefPartialEarlyReceived;

		TEST(aErr == KPositionEarlyComplete);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		}

	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}
