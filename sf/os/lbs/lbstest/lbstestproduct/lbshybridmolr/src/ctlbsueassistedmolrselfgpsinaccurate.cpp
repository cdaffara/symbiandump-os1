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
// LBS test includes.
#include "ctlbsuebasedmolrself.h"
#include "ctlbsueassistedmolrselfgpsinaccurate.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsUEAssistedMOLRSelfGPSInaccurate* CT_LbsUEAssistedMOLRSelfGPSInaccurate::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsUEAssistedMOLRSelfGPSInaccurate* testStep = new CT_LbsUEAssistedMOLRSelfGPSInaccurate(aParent);
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
CT_LbsUEAssistedMOLRSelfGPSInaccurate::CT_LbsUEAssistedMOLRSelfGPSInaccurate(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsUEAssistedMOLRSelfGPSInaccurate);
	
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
		
	iState = EInitializing;
	}


void CT_LbsUEAssistedMOLRSelfGPSInaccurate::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsUEAssistedMOLRSelfGPSInaccurate::~CT_LbsUEAssistedMOLRSelfGPSInaccurate()
	{
	}


TVerdict CT_LbsUEAssistedMOLRSelfGPSInaccurate::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsUEAssistedMOLRSelfGPSInaccurate::doTestStepL()"));	
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
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(240000000));
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
	
	for (TInt i = 0; i < 2; i++)
		{
// Send <-- ProcessLocationRequest - time t.
		proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

// Recv --> RequestAssistanceData.
		TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
		cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
		TESTL(dataMask == EAssistanceDataNone);
		CleanupStack::PopAndDestroy(cleanupCnt);
		
		// Expect measurement first time.
		if (i == 0)
			{
			measurementInfo = NULL;

// Recv --> RespondLocationRequest.
			TESTL(proxy->WaitForResponse(t + delta) == ENetMsgRespondLocationRequest);

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
			cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
			TESTL(dataMask == EAssistanceDataNone);
			CleanupStack::PopAndDestroy(cleanupCnt);
		}
		
		// Expect position second time.
		else
			{
			// Allow time for the agps module to return an inaccurate pos.
			// which is ignored by LBS
			TESTL(proxy->WaitForResponse(delta) == ENetMsgTimeoutExpired);


			// the whole point of this test is for the protocol module to inject a final network postions before
			// the AGPS module produces an accurate postions and before LBS timesout trying to do so!

// Send <-- ProcessLocationUpdate - return network calculated pos.
			TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
			proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);
			}
		}
	
		
// Send <-- ProcessSessionComplete.
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);


// Send <-- ProcessStatusUpdate.
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

// OPTIONAL One of Recv --> RequestAssistanceData. 
// OPTIONAL One of Recv --> CancelSelfLocation. 
	CheckLBSMessagesSentToProtocolModuleL(proxy);

// Recv --> CancelSelfLocation. 
	// NB: CancelSelfLocate at PM may or may not occur depending on order of Cancel from LS after receiving FNP and arrival of sessioncomplete at the AGPS Manager
	delta = 3 * 1000 * 1000;
	TNetProtocolResponseType resp = proxy->WaitForResponse(delta,EResponseFilterMsgRequestAssistanceData);
	TESTL(resp == ENetMsgCancelSelfLocation || resp == ENetMsgTimeoutExpired);


// Client recv - the gps position determined by the gps module.
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == EFinalLocReceived);


// Wait to ensure no additional responses turn up.

	TESTL(proxy->WaitForResponse(delta) == ENetMsgTimeoutExpired);
	

// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);
		
	return TestStepResult();
	}



// MPosServerObserver
void CT_LbsUEAssistedMOLRSelfGPSInaccurate::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsUEAssistedMOLRSelfGPSInaccurate::CheckLBSMessagesSentToProtocolModuleL(CNetProtocolProxy* aProxy)
	{
	// LBS may, or may not,  send a CancelSelfLocate to the Protolcol Module
	// depending on order of Cancel from Location Server  after receiving the FNP 
	// and arrival of Sessioncomplete at the AGPS Manager
	// Similarly, an assistance data request may or may not arrive depeding on timings
	// of the arrival of the network location request at the integration module.
	TBool notDone(ETrue); 	TBool hadCancel(EFalse); TBool hadAssistRequest(EFalse);
	while (notDone)
		{
		TNetProtocolResponseType resp = aProxy->WaitForResponse(2 * 1000 * 1000);
		switch (resp)
				{
				case ENetMsgTimeoutExpired:
					{
					notDone = EFalse;
					}
					break;
				case ENetMsgRequestAssistanceData:
					{
					if (hadAssistRequest)
						{
						INFO_PRINTF1(_L("Fail: had two assistance data requests"));
						TESTL(EFalse);
						}
					else
						{
						hadAssistRequest = ETrue;
						}
					}
					break;
				case ENetMsgCancelSelfLocation:
					{
					if (hadCancel)
						{
						INFO_PRINTF1(_L("Fail: had two cancels"));
						TESTL(EFalse);
						}
					else
						{
						hadCancel = ETrue;
						}
					}
					break;
				
				default:
					INFO_PRINTF2(_L("Fail: unexpected message from LBS %d"),resp);
					TESTL(EFalse);
					break;
				}
		}

	}




void CT_LbsUEAssistedMOLRSelfGPSInaccurate::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify position.


	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;

		TEST(aErr == KErrNone);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}

	// Expect either the final network A-GPS position, depending on test case.
	else if (iState == ERefLocReceived)
		{
		iState = EFinalLocReceived;

		TEST(aErr == KPositionQualityLoss);
		
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			switch (testCaseId)
				{
				// Expect the final network position.
				case 1:
					{
					T_LbsUtils utils;
					TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
		
					TEST(utils.Compare_PosInfo(aPosInfo, networkPosInfo));
					TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
					TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
					TEST(aPosInfo.ModuleId() == TUid::Uid(0x1028fABE)); // id of Network PSY
					}
					break;
				
				// Expect the A-GPS position.
				case 2:
					{
					TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
					TEST(aPosInfo.PositionClassType() == EPositionInfoClass | EPositionCourseInfoClass | EPositionSatelliteInfoClass);
					TEST(aPosInfo.ModuleId() == TUid::Uid(0x10281D44)); // id of AGPSManager

					}
					break;
				
				default:
					TEST(EFalse);
					break;
				}
			}
		
		else
			{
			TEST(EFalse);
			}	
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}


