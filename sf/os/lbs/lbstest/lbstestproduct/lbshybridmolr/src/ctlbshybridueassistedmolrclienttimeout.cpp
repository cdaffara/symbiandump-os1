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
#include "ctlbshybridueassistedmolrclienttimeout.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsUEAssistedMOLRClientTimeout* CT_LbsUEAssistedMOLRClientTimeout::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsUEAssistedMOLRClientTimeout* testStep = new CT_LbsUEAssistedMOLRClientTimeout(aParent);
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
CT_LbsUEAssistedMOLRClientTimeout::CT_LbsUEAssistedMOLRClientTimeout(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsUEAssistedMOLRClientTimeout);
	
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
		
	iState = EInitializing;
	}


void CT_LbsUEAssistedMOLRClientTimeout::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsUEAssistedMOLRClientTimeout::~CT_LbsUEAssistedMOLRClientTimeout()
	{
	}


TVerdict CT_LbsUEAssistedMOLRClientTimeout::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsUEAssistedMOLRClientTimeout::doTestStepL()"));	
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
	const TInt KClientTimeout = 60*1000*1000;
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KClientTimeout));
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
													KClientTimeout,
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

// Start timer to run along side Client request timer
	TTime clientTimerStart;
	clientTimerStart.HomeTime();

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
// Client Send -  a self-locate request.
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
	
	TTime loopStartTime;
	loopStartTime.HomeTime();
	microseconds = loopStartTime.MicroSecondsFrom(clientTimerStart);
	TInt clientTimeElapsed = microseconds.Int64();
	for (TInt i = 0; clientTimeElapsed < (KClientTimeout + 10*1000*1000); i++)
		{
		// Recalculate time elapsed
		loopStartTime.HomeTime();
		microseconds = loopStartTime.MicroSecondsFrom(clientTimerStart);
		clientTimeElapsed = microseconds.Int64();
		// Fail if we pass client timeout without receiving timeout message
		TESTL(clientTimeElapsed < (KClientTimeout + 10*1000*1000));

// Send <-- ProcessLocationRequest - time t.
		proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

// Recv --> RequestAssistanceData.
		TNetProtocolResponseType response;
		response = proxy->WaitForResponse(KTimeOut);
		if (response == ENetMsgCancelSelfLocation)
			{
			sessionId = NULL;
			TInt cancelReason = KErrNone;
			CleanupStack::PopAndDestroy(cleanupCnt = proxy->GetArgsLC(ENetMsgCancelSelfLocation, &sessionId, &cancelReason));
			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(cancelReason == KErrTimedOut);
			CleanupStack::PopAndDestroy(cleanupCnt);
			sessionId = NULL;
			break;
			}
		TESTL(response == ENetMsgRequestAssistanceData); 
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
		
		// Expect measurement
		measurementInfo = NULL;

// Recv --> RespondLocationRequest.
		response = proxy->WaitForResponse(t + delta);
		if (response == ENetMsgCancelSelfLocation)
			{
			sessionId = NULL;
			TInt cancelReason = KErrNone;
			cleanupCnt = proxy->GetArgsLC(ENetMsgCancelSelfLocation, &sessionId, &cancelReason);
			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(cancelReason == KErrTimedOut);
			CleanupStack::PopAndDestroy(cleanupCnt);
			sessionId = NULL;
			break;
			}
		TESTL(response == ENetMsgRespondLocationRequest);

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
		response = proxy->WaitForResponse(KTimeOut);
		if (response == ENetMsgCancelSelfLocation)
			{
			sessionId = NULL;
			TInt cancelReason = KErrNone;
			cleanupCnt = proxy->GetArgsLC(ENetMsgCancelSelfLocation, &sessionId, &cancelReason);
			TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
			TESTL(cancelReason == KErrTimedOut);
			CleanupStack::PopAndDestroy(cleanupCnt);
			sessionId = NULL;
			break;
			}
		TESTL(response == ENetMsgRequestAssistanceData); 
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
		}

// Client Recv - NotifyPositionUpdate (KPositionQualityLoss, InaccurateGPS Position)
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == EGpsLocReceived);

// ***Start of workaround ***
// Production code functionality to request final GPS Position from LBS not implemented
// So we omit this request
	WARN_PRINTF1(_L("Workaround present - Production code not present => Omission of Protocol Module request for final GPS position"));
/*

// Send <-- ProcessLocationRequest - to request final GPS position from LBS
	// Special values for quality to request final GPS position from LBS
	TLbsNetPosRequestQuality finalQuality;
	finalQuality.SetMaxFixTime(t);
	finalQuality.SetMaxFixAge(t);
	finalQuality.SetMinHorizontalAccuracy(-1);
	finalQuality.SetMinVerticalAccuracy(-1);
	
	// Special value for request method to request final GPS position from LBS
	TLbsNetPosRequestMethod uebasedMethod;
	TLbsNetPosMethod method1;
	method1.SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	const TLbsNetPosMethod posMethods [] =
		{
		method1
		};
	uebasedMethod.SetPosMethods(posMethods, 1);
	
	MLbsNetworkProtocolObserver::TLbsNetProtocolService	finalService = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &finalService, &finalQuality, &uebasedMethod);
	
// Recv --> RespondLocationRequest - to receive final GPS position from LBS	
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest);
	TPositionInfo* finalPositionInfo = NULL;
	proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &finalPositionInfo);

	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrNone);

	// Expect to receive final position which is an inaccurate GPS position, 
	// not the reference position, not measured results
	TESTL(finalPositionInfo->PositionClassType() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
	TPosition finalPosition;
	finalPositionInfo->GetPosition(finalPosition);
	TESTL((finalPosition.Latitude() == 50.2454) && (finalPosition.Longitude() == 0.1668) && (finalPosition.Altitude() == 1.0));
	TESTL((finalPosition.HorizontalAccuracy() == 2000) && (finalPosition.VerticalAccuracy() == 2000));

	CleanupStack::PopAndDestroy(cleanupCnt);
	sessionId = NULL;
	finalPositionInfo = NULL;
*/
// ***End of workaround ***
				
// Send <-- ProcessLocationUpdate - return network calculated pos.
	TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);

// Send <-- ProcessSessionComplete(KErrTimedOut)
	reason = KErrTimedOut;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);


// Send <-- ProcessStatusUpdate.
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

	// Wait for 10 seconds to ensure no additional responses turn up. Ignore any Requests for Assistance data.
	// because, depending on timings,  the session complete may force the AGPS manager to reissue a location request 
	// to the integration module
	delta = 10 * 1000 * 1000;
	TNetProtocolResponseType mType = proxy->WaitForResponse(delta,EResponseFilterMsgRequestAssistanceData);
	TESTL(mType == ENetMsgTimeoutExpired);

// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);
		
	return TestStepResult();
	}



// MPosServerObserver
void CT_LbsUEAssistedMOLRClientTimeout::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	INFO_PRINTF1(_L("&gt;&gt; Unexpected call to GetLastKnownPosition()"));
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsUEAssistedMOLRClientTimeout::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	INFO_PRINTF1(_L("&gt;&gt; CT_LbsUEAssistedMOLRClientTimeout::OnNotifyPositionUpdate"));
	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		INFO_PRINTF1(_L("&gt;&gt; NotifyPositionUpdate(ReferencePosition)"));
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
		INFO_PRINTF1(_L("&gt;&gt; Final position: NotifyPositionUpdate(GPS Inaccurate Position)"));
		iState = EGpsLocReceived;

		if(iExpectedApiBehaviour == EApiVariant2)
			{
			TEST(aErr == KErrTimedOut);		
			}
		else
			{
			TEST(aErr == KPositionQualityLoss);		
			TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
			const TPositionInfo& posInfo = static_cast<const TPositionInfo&>(aPosInfo);
			TPosition pos;
			posInfo.GetPosition(pos);
			TESTL((pos.Latitude() == 50.2454) && (pos.Longitude() == 0.1668) && (pos.Altitude() == 1.0));
			TESTL((pos.HorizontalAccuracy() == 2000) && (pos.VerticalAccuracy() == 2000));
			}
		}
	
	// Not expecting anything else.
	else
		{
		INFO_PRINTF1(_L("&gt;&gt; Unexpected call to NotifyPositionUpdate()"));
		TEST(EFalse);
		}

	ReturnToTestStep();
	}

