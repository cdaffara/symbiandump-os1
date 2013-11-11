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
#include "ctlbsueassistedmolrselfgpslatefacilitytimeout.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout* CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout* testStep = new CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout(aParent);
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
CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout::CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsUEAssistedMOLRSelfGPSLateFacTimeout);
	
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	
//	iPositionUpdateRef = EFalse;
	}


void CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout::~CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout()
	{
	}


TVerdict CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsUEAssistedMOLRSelfGPSLate::doTestStepL()"));	
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
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KMOLRFixTime));
	pos.SetUpdateOptions(posOpts);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);


// Client Send - a self-locate request.
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

	// Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
	TTime startTime;
	startTime.HomeTime();


// Recv --> RequestAssistanceData.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Client Send - a self-locate request.
	pWatch->IssueNotifyPositionUpdate();

// Rev --> RequestAssistanceData - we get this as the result of client notify update request.
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


// Recv --> RespondLocationRequest.
	// First ensure we don't recv response before Alpha2.
	TInt delta = 2 * 1000 * 1000; // 2 secs.
	TESTL(proxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - delta) == ENetMsgTimeoutExpired);
	
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
	const TInt t = 8 * 1000 * 1000; // 8 secs.
	quality.SetMaxFixTime(t);
	
	for (TInt i = 0; i < 2; i++)
		{
// Send <-- ProcessLocationRequest - time t.
		proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

// Recv --> RequestAssistanceData.
		TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);

// Recv --> RespondLocationRequest - first measurement, second position.
		TESTL(proxy->WaitForResponse(t + delta) == ENetMsgRespondLocationRequest);

		sessionId = NULL;
		
		// Expect measurement first time.
		if (i == 0)
			{
			measurementInfo = NULL;

			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);			

			// Check it is a measurement.
			TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);


// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issueing a location request when it's
//									max fix time timer expries.
			TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData);
			TInt cleanupCnt2 = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
			TESTL(dataMask == EAssistanceDataNone);
			CleanupStack::PopAndDestroy(cleanupCnt2);
			}
		
		// Expect position second time.
		else
			{
			TPositionInfo* positionInfo = NULL;
			
			cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
			
			// Check it is a position.
			TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
			}

		TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
		TESTL(reason == KErrNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
		}


// Send <-- ProcessLocationUpdate - return network calculated pos - this message is not sent 
// this is due to the facility message not being sent and a timeout has occurred.
/*  
	TPositionInfo networkPosInfo = MolrArgUtils::NetworkPositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);
*/

// Can wait for a number of seconds
    TInt KTimeout = 10 * 1000000; // 10 seconds
    TESTL(proxy->WaitForResponse(KTimeout) == ENetMsgTimeoutExpired);
    
    /*
    Other option is to have a delay, in case there is something put on the data bus
    RTimer timer;
    
    TInt tStat = timer.CreateLocal();
    TESTL(tStat == KErrNone);
    TRequestStatus s = KErrGeneral;
    TTime time;
    time.UniversalTime();
    timer.AtUTC(s, time + TTimeIntervalSeconds(10));
    TESTL(s == KRequestPending);
    User::WaitForRequest(s);
    TESTL(s == KErrNone);
    timer.Close();
    */

// Send <-- ProcessSessionComplete.
	reason = KErrTimedOut;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);


// Send <-- ProcessStatusUpdate. NOTE: this is missing from the sequence diagram.
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);


// Client recv - the gps position determined by the gps module.
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == EGpsLocReceived);

// Check no further data is being being sent.
	TESTL(proxy->WaitForResponse(KTimeout) == ENetMsgTimeoutExpired);


// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);

		
	return TestStepResult();
	}




// MPosServerObserver
void CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsUEAssistedMOLRSelfGPSLateFacTimeout::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
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
		
		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
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

