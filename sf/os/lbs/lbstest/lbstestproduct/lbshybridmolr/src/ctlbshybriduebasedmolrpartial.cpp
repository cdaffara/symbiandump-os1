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
#include "ctlbshybriduebasedmolrpartial.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsHybridUEBasedMOLRPartial* CT_LbsHybridUEBasedMOLRPartial::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEBasedMOLRPartial* testStep = new CT_LbsHybridUEBasedMOLRPartial(aParent);
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
CT_LbsHybridUEBasedMOLRPartial::CT_LbsHybridUEBasedMOLRPartial(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEBasedMOLRPartial);
	
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	}


void CT_LbsHybridUEBasedMOLRPartial::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsHybridUEBasedMOLRPartial::~CT_LbsHybridUEBasedMOLRPartial()
	{
	}


TVerdict CT_LbsHybridUEBasedMOLRPartial::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEBasedMOLRPartial::doTestStepL()"));	
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
	
	// Switch on partial updates.
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KMOLRFixTime), TTimeIntervalMicroSeconds(0), ETrue);
	pos.SetUpdateOptions(posOpts);	


	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

// Client Send - a self-locate request, for the NaN position.
	pWatch->IssueNotifyPositionUpdate();

// Client recv - the NaN position app side.
	CheckForObserverEventTestsL(KTimeOut, *this);


// Client Send - a self-locate request, for the request reference position.
	pWatch->IssueNotifyPositionUpdate();


// Recv --> RequestSelfLocation.
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
	sessionId = NULL;
	opts = NULL;	

		
// Send <-- ProcessStatusUpdate.
	MLbsNetworkProtocolObserver::TLbsNetProtocolService serviceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

	
// Send <-- ProcessLocationUpdate.
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);
	
// Client recv - the ref position app side.
	CheckForObserverEventTestsL(KTimeOut, *this);


// Send <-- ProcessAssistanceData.
	TLbsAsistanceDataGroup			dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet	data;
	TInt							reason = KErrNone;
	ArgUtils::PopulateLC(data);
	proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &data, &reason);
	CleanupStack::PopAndDestroy(); // data


// Send <-- ProcessLocationRequest.
	TBool												emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality							quality = ArgUtils::Quality();
	TLbsNetPosRequestMethod								method = ArgUtils::RequestHybridMethod();
	quality.SetMaxFixTime(ArgUtils::Alpha2());
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);


// Recv --> RequestAssistanceData, as the result of the NRH request.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Client Send - a self-locate request, for the first partial update.
	pWatch->IssueNotifyPositionUpdate();

// Rev --> RequestAssistanceData, as the result of the first partial update.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
	
// Client recv - to get the first partial update.
	CheckForObserverEventTestsL(KTimeOut, *this);



// Client Send - a self-locate request, for the second partial update.
	pWatch->IssueNotifyPositionUpdate();

// Rev --> RequestAssistanceData, as the result of the second partial update.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	if(iExpectedApiBehaviour == EApiVariant1)// variant2 doesn't support early completes it really 
		{
// CompleteRequest, for the second partial update.
		TESTL(pos.CompleteRequest(EPositionerNotifyPositionUpdate) == KErrNone);
	
// Client recv - to get the second partial, cancelled by the client.
		CheckForObserverEventTestsL(KTimeOut, *this);
		TESTL(iState == EGpsPartialEarlyReceived);
	
// Recv --> RespondLocationRequest - the first partial gps position.
		TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest);
	
		sessionId = NULL;
		TPositionInfo* positionInfo = NULL;
				
		cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
				
		// Check the response.
		TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass));
		TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
		TESTL(reason == KErrNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
		}
	else	// just let things time out because client doesn't make another request
		{
		// the LocServer/PSY will send a cancel to the manager, causing a CancelSelfLocate to the PM
		// PM then sends FNP + SessionComplete
		// However there will still be an outstanding NRH request (until this times out) 
		// so agps module may still produce another update/ask for assistance data
		CheckForObserverEventTestsL(KTimeOut, *this);
		TNetProtocolResponseType response = proxy->WaitForResponse(KTimeOut);
		if(response == ENetMsgRequestAssistanceData)
			{
			response = proxy->WaitForResponse(KTimeOut);
			}
		// Recv --> RespondLocationRequest - the first partial gps position.
		TESTL(response == ENetMsgCancelSelfLocation);
		
		// There could still be assistance data request received by PM here due to outstanding NRH request
		response = proxy->WaitForResponse(KTimeOut);
		if(response == ENetMsgRequestAssistanceData)
			{
			response = proxy->WaitForResponse(KTimeOut);
			}		
		}

// Send <-- ProcessLocationUpdate - return network calculated pos (FNP)
	TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);


// Send <-- ProcessSessionComplete.
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);


// Send <-- ProcessStatusUpdate.
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

	// Wait for 10 seconds to ensure no additional responses turn up.
	TInt delta = 10 * 1000 * 1000;
	TNetProtocolResponseType mType;
	
	mType = proxy->WaitForResponse(delta);
	TESTL(mType == ENetMsgTimeoutExpired);

// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);

		
	return TestStepResult();
	}




// MPosServerObserver
void CT_LbsHybridUEBasedMOLRPartial::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsHybridUEBasedMOLRPartial::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
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
	
		if(iExpectedApiBehaviour == EApiVariant2)
			{
			TEST(aErr == KErrNone);
			}
		else
			{
			TEST(aErr == KPositionPartialUpdate);
			}

		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);

		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Expecting first partial gps position.
	else if (iState == ERefLocReceived)
		{
		iState = EGpsPartialLocReceived;
		
		TEST(aErr == KPositionPartialUpdate);
		TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
		}

	// Expecting early complete, of the second partial gps position.
	else if (iState == EGpsPartialLocReceived)
		{
		if(iExpectedApiBehaviour == EApiVariant1)
			{
			iState = EGpsPartialEarlyReceived;
			
			TEST(aErr == KPositionEarlyComplete);
			}
		else
			{
			iState = EGpsPartialEarlyReceived;
			
			TEST(aErr == KErrNone);
			}
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}

