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
//#include "ctlbsuebasedmolrself.h"
#include "ctlbshybridueassistedmolrpartial.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsHybridUEAssistedMOLRPartial* CT_LbsHybridUEAssistedMOLRPartial::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedMOLRPartial* testStep = new CT_LbsHybridUEAssistedMOLRPartial(aParent);
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
CT_LbsHybridUEAssistedMOLRPartial::CT_LbsHybridUEAssistedMOLRPartial(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssistedMOLRPartial);
	
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	}


void CT_LbsHybridUEAssistedMOLRPartial::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsHybridUEAssistedMOLRPartial::~CT_LbsHybridUEAssistedMOLRPartial()
	{
	}


TVerdict CT_LbsHybridUEAssistedMOLRPartial::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedMOLRPartial::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;
	
	_LIT(KEarlyCompleteSupported, "early_complete_supported");
	// For the S60 Loc Server the test will return KErrNone instead of KErrNotFound.
	if(!GetIntFromConfig(ConfigSection(), KEarlyCompleteSupported, iEarlyCompleteSupported))
		{
		// we support early complete by default unless configured otherwise
		iEarlyCompleteSupported = ETrue;
		}
	
	// To ensure the location server does not timeout the partial update requests from the client
	// smaller timeout values are used (new client timer added because of CR1041).
	const TInt KAlpha2TimeOutValue = 8 * 1000 * 1000;
	const TInt KTTimeOutvalue = 4 * 1000 * 1000;

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

	// Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
	TTime startTime;
	startTime.HomeTime();


//  Recv --> RequestAssistanceData, as the result of the 2nd NPUD request.
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

// Send <-- ProcessLocationRequest.
	TBool												emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality							quality = ArgUtils::Quality();
	TLbsNetPosRequestMethod								method = ArgUtils::RequestHybridMethod();
	quality.SetMaxFixTime(KAlpha2TimeOutValue/*MolrArgUtils::Alpha2()*/);
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);


// Client recv - to get the first partial update.
	CheckForObserverEventTestsL(KTimeOut, *this);



// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
//									max fix time timer expries.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Recv --> RespondLocationRequest.

	// Determine the value to take off the alpha2 value. This is required because we had to wait for the assistance data response.
	TTimeIntervalMicroSeconds microseconds;
	TTime stopTime;
	stopTime.HomeTime();
	microseconds = stopTime.MicroSecondsFrom(startTime); 
	TInt64 timeElapsed = microseconds.Int64();

	// First ensure we don't recv response before Alpha2.
	TInt delta = 2 * 1000 * 1000; // 2 secs.
	TESTL(proxy->WaitForResponse(KAlpha2TimeOutValue/*MolrArgUtils::Alpha2()*/ - timeElapsed - delta) == ENetMsgTimeoutExpired);
	
	// Wait for and process the response.
	TESTL(proxy->WaitForResponse(2 * delta) == ENetMsgRespondLocationRequest);

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
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Send <-- ProcessLocationRequest - time t.
	quality.SetMaxFixTime(KTTimeOutvalue/*KTTimeout*/);

	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);


// Recv --> RequestAssistanceData.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
// Client Send - a self-locate request, for the second partial update.
	pWatch->IssueNotifyPositionUpdate();


// Rev --> RequestAssistanceData, as the result of the second partial update.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	if(iEarlyCompleteSupported)
		{
		// CompleteRequest, for the second partial request.
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
	else	// if early complete not supported, just stop asking for updates.
		{
		// In this case the NRH will send measurements to the network again rather than a position
// Recv --> RespondLocationRequest - the first partial gps position.
			TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest);
		
			sessionId = NULL;
			
			TPositionGpsMeasurementInfo* measurementInfo = NULL;
			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(reason == KErrNone);
			TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
						
			CleanupStack::PopAndDestroy(cleanupCnt);	
		
		}
	
// Send <-- ProcessLocationUpdate - return network calculated pos
	TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);


// Send <-- ProcessSessionComplete.
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);


// Send <-- ProcessStatusUpdate.
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);


// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);

		
	return TestStepResult();
	}




// MPosServerObserver
void CT_LbsHybridUEAssistedMOLRPartial::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsHybridUEAssistedMOLRPartial::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
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

		_LIT(KExpectedRefError, "expected_ref_error");
		TInt expected_ref_error;
		// For the S60 Loc Server the test will return KErrNone instead of KErrNotFound.
		if(!GetIntFromConfig(ConfigSection(), KExpectedRefError, expected_ref_error))
			{
			expected_ref_error = KPositionPartialUpdate;
			}		
		TEST(aErr == expected_ref_error);
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
	else if (iState == EGpsPartialLocReceived && iEarlyCompleteSupported)
		{			
		iState = EGpsPartialEarlyReceived;
		
		TEST(aErr == KPositionEarlyComplete);
		}
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}

