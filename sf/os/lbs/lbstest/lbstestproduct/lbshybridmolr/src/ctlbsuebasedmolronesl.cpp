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
// This is the class implementation for the a test
// EPOC includes.
// 
//

// LBS includes. 
#include <lbssatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <e32math.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbsnetprotocolbase.h>

// LBS test includes.
#include "ctlbsuebasedmolronesl.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsUEBasedMOLROneSL* CT_LbsUEBasedMOLROneSL::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsUEBasedMOLROneSL* testStep = new CT_LbsUEBasedMOLROneSL(aParent);
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
CT_LbsUEBasedMOLROneSL::CT_LbsUEBasedMOLROneSL(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsUEBasedMOLROneSL);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	}


void CT_LbsUEBasedMOLROneSL::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsUEBasedMOLROneSL::~CT_LbsUEBasedMOLROneSL()
	{
	}


TVerdict CT_LbsUEBasedMOLROneSL::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsUEBasedMOLROneSL::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Do the /actual/ test step!
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

	// Set the max fix time 
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(5 * 1000 * 1000));
	pos.SetUpdateOptions(posOpts);
	
	// <<(App) NotifyPositionUpdate().
	pWatch->IssueNotifyPositionUpdate();

	// >> RequestSelfLocation()
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

	// Process the response.
	TLbsNetSessionId* 					sessionId = NULL;
	TLbsNetPosRequestOptionsAssistance*	opts = NULL;
	
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

	TBool qualitycheck = 	ArgUtils::CompareQuality(	opts, 
														ETrue, 
														KMinHorizontalAcc, 
														KMinVerticalAcc, 
														5 * 1000 * 1000,
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
	
	// << ProcessStatusUpdate(EServiceSelfLocation)
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask1);

	// << ProcessLocationUpdate(SessionId, RefPosition)
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

	// TEST: Get NotifyPositionUpdate app side - get the ref pos.
	CheckForObserverEventTestsL(KTimeOut, *this);

	pWatch->IssueNotifyPositionUpdate();


	// >> RequestAssistanceData - as a result of the second client request.
	TLbsAsistanceDataGroup dataMask;	
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// << ProcessAssistanceData()
	dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); // assistanceData

	// << ProcessLocationRequest() - this will cancel the client req.
	TBool emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality quality = ArgUtils::Quality();
	// ensure that the quality that we ask for is such that the inaccurate postion recieved from the
	// int module is accurate enough for the NRH
	quality.SetMinHorizontalAccuracy(5000.0);
	quality.SetMinVerticalAccuracy(5000.0);
	
	TLbsNetPosRequestMethod method   = ArgUtils::RequestMethod(); // prefer termainal based mode
	proxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

	// >> RequestAssistanceData - as a result of the NRH request.
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// >> RespondLocationRequest() - the location was accuarate enough for the NRH!
	TInt res1 = proxy->WaitForResponse(KTimeOut);
	TESTL(res1 == ENetMsgRespondLocationRequest);
	sessionId = NULL;
	reason = KErrNone;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = proxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &getPositionInfo);
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrNone);

	// >> RequestAssistanceData - as a result of the NRH request being satified and the AGPS manager re-sissuing the LOC server request
	TInt res = proxy->WaitForResponse(KTimeOut);
	TESTL(res == ENetMsgRequestAssistanceData); 
	
	res = proxy->WaitForResponse(2 * 1000 * 1000); // jcm just to see if more assistance requests on ay
	
 	// << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, getPositionInfo);

	CleanupStack::PopAndDestroy(cleanupCnt);
	
	// here, the AGPS manager has an request outstanding from the location server 
	// because the positon from the intgration module was NOT accurate enough to satisfy it
	// now inject into LBS the session complete. This should NOT cause LBS so send out another 
	// RequestSelfLocation()
	
	// << ProcessSessionComplete()
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	
	
	
	// Client recv pos eventually, after the client request should times-out, and the inaccurate  gps position from the 
	// gps module is returned to the client (RunL(pos)).
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == EGpsLocReceived);
	
	// >> RequestAssistanceData - because when the NRH gets the
	// session complete it will send a cancel to the AGPS manager (even though there is no
	// location request currently active) and this causes the AGPS manager
	// to re-issue the request still pending from the loaction server
	res = proxy->WaitForResponse(KTimeOut);
	TESTL(res == ENetMsgRequestAssistanceData); 

	// Wait for 10 seconds to ensure no additional responses turn up.
	TInt delta = 10 * 1000 * 1000;
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
void CT_LbsUEBasedMOLROneSL::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsUEBasedMOLROneSL::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify error.
//	TEST(aErr == KErrNone); or quality loss

	// Verify position.

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;
		TEST(aErr == KErrNone);
		TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);

		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TEST(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Expecting gps pos.
	else if (iState == ERefLocReceived)
		{
		iState = EGpsLocReceived;
		TEST(aErr == KPositionQualityLoss);
		TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
		TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
		}
	
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}

