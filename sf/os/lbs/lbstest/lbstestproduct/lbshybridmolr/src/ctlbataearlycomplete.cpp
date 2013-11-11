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
#include <lbs/lbsadmin.h>

// LBS test includes.
#include "ctlbsuebasedmolrself.h"
#include "ctlbataearlycomplete.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

#include  "lbs/test/ctlbsagpshandler.h"

/**
Static Constructor
*/
CT_LbsATAEarlyComplete* CT_LbsATAEarlyComplete::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsATAEarlyComplete* testStep = new CT_LbsATAEarlyComplete(aParent);
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
CT_LbsATAEarlyComplete::CT_LbsATAEarlyComplete(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsATAEarlyComplete);
	
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
		
	iState = EInitializing;
	}


void CT_LbsATAEarlyComplete::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsATAEarlyComplete::~CT_LbsATAEarlyComplete()
	{
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_LbsATAEarlyComplete::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Test Preamble. CT_LbsATAEarlyComplete"));

	// create the test channel handler
	iAGpsHandler = CT_LbsAGpsHandler::NewL(this);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CT_LbsATAEarlyComplete::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("Test Postamble. CT_LbsATAEarlyComplete"));

	delete iAGpsHandler;
	iAGpsHandler = NULL;
		
	return TestStepResult();
	}
	

TVerdict CT_LbsATAEarlyComplete::doTestStepL()
	{
   	INFO_PRINTF1(_L("&lt;&lt;CT_LbsATAEarlyComplete::doTestStepL()"));

    INFO_PRINTF1(_L("CT_LbsATAEarlyComplete::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_SetupRoamSelfLocate::doTestStepL()"));
   
   	CLbsAdmin* adminApi = CLbsAdmin::NewL();
   	CleanupStack::PushL(adminApi);
   	
   	// Switch on the self locate API when roaming.
   	CLbsAdmin::TSelfLocateService serviceSetting = CLbsAdmin::ESelfLocateUnknown;
   
   	TInt err = adminApi->Set(KLbsSettingRoamingSelfLocate, CLbsAdmin::ESelfLocateOn);
   	User::LeaveIfError(err);
   	err = adminApi->Get(KLbsSettingRoamingSelfLocate, serviceSetting);	
   	User::LeaveIfError(err);
   	
   	if(serviceSetting != CLbsAdmin::ESelfLocateOn)
   		{
   		INFO_PRINTF1(_L("Self locate API admin setting didn't work"));
   		SetTestStepResult(EFail);
   		}					
   	
   	// Set gps mode when roaming.
   	CLbsAdmin::TGpsMode gpsModeSetting = CLbsAdmin::EGpsModeUnknown;
   	
  	err = adminApi->Set(KLbsSettingRoamingGpsMode, CLbsAdmin::EGpsAlwaysTerminalAssisted);
   	User::LeaveIfError(err);
   	err = adminApi->Get(KLbsSettingRoamingGpsMode, gpsModeSetting);
   	User::LeaveIfError(err);
   	
 	if (gpsModeSetting != CLbsAdmin::EGpsAlwaysTerminalAssisted)
   		{
   		INFO_PRINTF1(_L("Gps mode admin setting didn't work"));
   		SetTestStepResult(EFail);
   		}
   		
	CLbsAdmin::TSpecialFeature	ignoreAccuracy;
   	TInt err1 = adminApi->Get(KLbsSpecialFeatureIgnoreAccuracy, ignoreAccuracy);
	if (err1 != KErrNone)
		{
	  	INFO_PRINTF1(_L("&lt;&lt KLbsSpecialFeatureIgnoreAccuracy is absent in admin;"));
		}	
		else
		{
	  	INFO_PRINTF2(_L("&lt;&lt KLbsSpecialFeatureIgnoreAccuracy is in admin and it's set to %d;"),ignoreAccuracy);
		}	

  	//INFO_PRINTF1(_L("&lt;&lt Setting KLbsSpecialFeatureIgnoreAccuracy to OFF"));
	//err = adminApi->Set(KLbsSpecialFeatureIgnoreAccuracy, CLbsAdmin::ESpecialFeatureOff;);
   		
   	CleanupStack::PopAndDestroy(adminApi);
   
   	// Allow the setting to be propagated
   	User::After(2*1000*1000);
   
	const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

	// Recv --> GetCurrentCapabilitiesResponse.
	// Soak up the Lbs System Status (produced by A-GPS Manager startup).}
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);


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
													(TPositionModuleInfo::ETechnologyNetwork 
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
	TLbsNetPosRequestMethod								method = ArgUtils::RequestTAPMethod();
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
	
	
	// Send <-- ProcessLocationUpdate - return network calculated pos.
	TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);

	// Client Recv: Second update position - This should return with KPositionEarlyComplete
	CheckForObserverEventTestsL(KTimeOut, *this);

	// Wait to ensure no additional responses turn up.
	delta = 10 * 1000 * 1000;
	TNetProtocolResponseType rtype = proxy->WaitForResponse(delta);
	
	if (rtype == ENetMsgTimeoutExpired)
		{
		INFO_PRINTF1(_L("No additional messages turned up"));
	
 		}
	else
		{
		INFO_PRINTF1(_L("Got unexpected messages"));
 		}
		
	TESTL(rtype == ENetMsgCancelSelfLocation); 

	// Send <-- ProcessSessionComplete.
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);


	// Carryout unique test actions (the ALT sections - see sequence diagrams.
	TInt testCaseId;
	if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
		{
		switch (testCaseId)
			{
			// Test case Stop_OutsideMoLr
			case 6:
				{
				User::After(2*1000*1000);
				
				iAGpsHandler->SendRequestModuleOption(ELbsHybridModuleOptions_DisableReqAssistData, ETrue);

				// Client Send: Notify Position Update
				pWatch->IssueNotifyPositionUpdate();
		
				// now ensure that intgration moulde does NOT RequestAssistanceData
				// and thus LBS will NOT send RequestSelfLopcation to protocol module
				// and a MoLr will NOT be started 
				
	 			// Client Send: Complete Request
				TESTL(pos.CompleteRequest(EPositionerNotifyPositionUpdate) == KErrNone);
	
				// Client Recv: Second update position - This should return with KPositionEarlyComplete
				CheckForObserverEventTestsL(KTimeOut, *this);

				TESTL(iState == EGotCompleteRequestPosition);
				
				}
				break;
					
			// Test case Stop_InsideMoLr
			case 5:
				{
				// Client Send: Notify Position Update

				User::After(2*1000*1000);

				pWatch->IssueNotifyPositionUpdate();

				rtype = proxy->WaitForResponse(KTimeOut);
				TESTL(rtype == ENetMsgRequestSelfLocation);

				// Process the response.
				sessionId = NULL;
				opts = NULL;
	
				cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

				qualitycheck = ArgUtils::CompareQuality(	opts, 
											EFalse, 
											KMinHorizontalAcc, 
											KMinVerticalAcc, 
											KMOLRFixTime,
											0, 
											EAssistanceDataNone, 
											(TPositionModuleInfo::ETechnologyNetwork 
											| TPositionModuleInfo::ETechnologyAssisted)
										);
		
				TESTL(qualitycheck);
	
				iSessionId.SetSessionNum(sessionId->SessionNum());
				iSessionId.SetSessionOwner(sessionId->SessionOwner());

				CleanupStack::PopAndDestroy(cleanupCnt);
				sessionId = NULL;
				opts = NULL;	
	

				// Send <-- ProcessLocationRequest.
				TBool	emergency = EFalse;
				MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
				TLbsNetPosRequestQuality quality = ArgUtils::Quality();
				TLbsNetPosRequestMethod method = ArgUtils::RequestTAPMethod();
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

    			//Client Send: CompleteRequest
				TESTL(pos.CompleteRequest(EPositionerNotifyPositionUpdate) == KErrNone);
	
				// Client Recv: Second update position - This should return with KPositionEarlyComplete
				CheckForObserverEventTestsL(KTimeOut, *this);

				TESTL(iState == EDone);

				// Wait for and process the response. Should be aRespondLocationRequest(KErrNone,FinalNetworkPosition)
				rtype = proxy->WaitForResponse(KTimeOut);
				TESTL(rtype == ENetMsgRespondLocationRequest);

				sessionId = NULL;
				TPositionInfo* positionInfo = NULL;
			
				cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
			
				// Check the response.
				TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass));
				TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
				TESTL(reason == KErrNone);
				CleanupStack::PopAndDestroy(cleanupCnt);

				// Send <-- ProcessSessionComplete.
				reason = KErrNone;
				proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);	
				}
				break;
			default:
				{
				User::Leave(KErrArgument);	
				}
				break;
			}
		}
	else
		{
	INFO_PRINTF1(_L("<FONT><B>Failed to read from ini file correctly</B></FONT>"));
	SetTestStepResult(EFail);
		}

	// Wait for 10 seconds to ensure no additional responses turn up.
	delta = 10 * 1000 * 1000;
	TNetProtocolResponseType mType = proxy->WaitForResponse(delta);
	TESTL(mType == ENetMsgTimeoutExpired);
	
	// Done. Now cleanup...
	CleanupStack::PopAndDestroy(pWatch);
	CleanupStack::PopAndDestroy(); // pos
	CleanupStack::PopAndDestroy(); // server	
	CleanupStack::PopAndDestroy(proxy);
		
	return TestStepResult();
	}



// MPosServerObserver
void CT_LbsATAEarlyComplete::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsATAEarlyComplete::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify position.

	if (iState == EInitializing)
		{
		
		INFO_PRINTF2(_L("OnNotifyPositionUpdate, Expecting KPositionQualityLoss and got err %d."), aErr);	

		TEST(aErr == KPositionQualityLoss);
		TInt testCaseId;
		if (GetIntFromConfig(ConfigSection(), KTestCaseId, testCaseId))
			{
			T_LbsUtils utils;
			TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
		
			TEST(utils.Compare_PosInfo(aPosInfo, networkPosInfo));
			TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
			TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
				
			TEST(aPosInfo.ModuleId() == TUid::Uid(0x10281D43)); // id of NetLocManager
			}
		iState = EGotCompleteRequestPosition;
		}
	else if (iState == EGotCompleteRequestPosition)
		{
		TEST(aErr == KPositionEarlyComplete);
		T_LbsUtils utils;
		TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
		
		TEST(utils.Compare_PosInfo(aPosInfo, networkPosInfo));
		TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
		TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
			
		TEST(aPosInfo.ModuleId() == TUid::Uid(0x10281D43)); // id of NetLocManager
		
		iState = EDone;
		}
	else if (iState == EDone)
		{
		// not used
		}
	else
		{
		TEST(EFalse);
		}
		

		
	ReturnToTestStep();
	}

// Response back from A-GPS hybrid module to notify the test it has got the timeout request.
void CT_LbsATAEarlyComplete::ProcessAGpsResponseMessage(const TT_LbsAGpsResponseMsg::TModuleResponseType aResponse)
	{
	if (TT_LbsAGpsResponseMsg::EModuleResponseOk != aResponse)
		{
		// fail test could not config module!
		INFO_PRINTF2(_L("Unable to configure hybrid module, err %d."), aResponse);	
		SetTestStepResult(EFail);						
		}
		
		// Return back to test.
		CActiveScheduler::Stop();	
	}
