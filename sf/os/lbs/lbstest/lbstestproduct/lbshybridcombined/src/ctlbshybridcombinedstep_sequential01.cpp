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
// This is the class implementation for the Module Information Tests
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
#include "ctlbshybridcombinedstep_sequential01.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsHybridCombinedStep_Sequential01* CT_LbsHybridCombinedStep_Sequential01::New(CT_LbsHybridCombinedServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridCombinedStep_Sequential01* testStep = new CT_LbsHybridCombinedStep_Sequential01(aParent);
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
CT_LbsHybridCombinedStep_Sequential01::CT_LbsHybridCombinedStep_Sequential01(CT_LbsHybridCombinedServer& aParent) : CT_LbsHybridCombinedStep_Base(aParent)
	{
	SetTestStepName(KLbsHybridCombinedStep_Sequential01);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	iSessionId2.SetSessionOwner(KRequestUid);
	iSessionId2.SetSessionNum(0x0002);
	}


void CT_LbsHybridCombinedStep_Sequential01::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridCombinedStep_Base::ConstructL();
    iController = CLbsPrivacyController::NewL(*this);
    iProxy = CNetProtocolProxy::NewL();
	}

/**
 * Destructor
 */
CT_LbsHybridCombinedStep_Sequential01::~CT_LbsHybridCombinedStep_Sequential01()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsHybridCombinedStep_Sequential01::doTestStepL()
	{
 	INFO_PRINTF1(_L("CT_LbsHybridCombinedStep_Sequential01::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);
	const TInt KTimeOut = 60*1000*1000;

	// data declarations common to MOLR & MTLR
    TBool emergency = EFalse;
    TInt reason = KErrNone;
    MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    TLbsNetPosRequestQuality quality = ArgUtils::Quality();
    TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
    TPositionSatelliteInfo* getPositionInfo = NULL;
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;
    
    // MTLR STUFF
    TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
    TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
    
    // receive the cababilities message
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// Do the MOLR...
	RPositionServer server;
	TInt connectError = server.Connect();
	TESTL(KErrNone == connectError);
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

	// <<(App) NotifyPositionUpdate().
	pWatch->IssueNotifyPositionUpdate();

	// >> RequestSelfLocation()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);
 
	// check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...
	
	// Process the response.
	TLbsNetSessionId* 					sessionId = NULL;
	TLbsNetPosRequestOptionsAssistance*	opts = NULL;
	
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

	TBool qualitycheck = 	ArgUtils::CompareQuality(	opts, 
														ETrue, 
														KMinHorizontalAcc, 
														KMinVerticalAcc, 
														KDefaultMaxFixTime,
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
	activeServiceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask1);
	
	// << ProcessLocationUpdate(SessionId, RefPosition)
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

	// TEST: Get NotifyPositionUpdate app side - get the ref pos.
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	pWatch->IssueNotifyPositionUpdate();

	// >> RequestAssistanceData - as a result of the second client request.
	TLbsAsistanceDataGroup dataMask;	
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	// check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...

	// << ProcessAssistanceData()
	dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); // assistanceData
	
	// << ProcessLocationRequest() - this will cancel the client req.
    service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	quality = ArgUtils::Quality();
	method   = ArgUtils::RequestHybridMethod();
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
  
	// check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...

	
	// >> RequestAssistanceData - as a result of the NRH request.
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
	TESTL(dataMask == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

   	// >> RespondLocationRequest()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest);
	sessionId = NULL;
	reason = KErrNone;
	getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &getPositionInfo);
	TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(reason == KErrNone);

 	// << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, getPositionInfo);

	CleanupStack::PopAndDestroy(cleanupCnt);
	
	// Client recv - the gps position determined by the gps module.
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState == EGpsLocReceived);
	iState = ERequestComplete;
	
	// << ProcessSessionComplete()
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ProcessStatusUpdate()
	activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

	// Wait for 10 seconds to ensure no additional responses turn up.
	TInt delta = 10 * 1000 * 1000;
	TNetProtocolResponseType mType = iProxy->WaitForResponse(delta);
	TESTL(mType == ENetMsgTimeoutExpired);
	
 
    // Done. Now cleanup...
    // all associated with MOLR Positioning Client 
    CleanupStack::PopAndDestroy(pWatch);
    CleanupStack::PopAndDestroy(); // pos
    CleanupStack::PopAndDestroy(); // server     
	
	// ******************************************

    INFO_PRINTF1(_L("Starting an MTLR..."));   
    TESTL(TestStepResult() == EPass);

    // << ProcessStatusUpdate()
    activeServiceMask1 = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
    iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask1);

    // << ProcessPrivacyRequest()
    emergency = ETrue;
    privacy    = ArgUtils::Privacy();
    requestInfo = ArgUtils::RequestInfo();
    iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId2, &emergency, &privacy, &requestInfo);

    // >> Callback from RespondNetworkLocationRequest(ERequestAccepted)
    CheckForObserverEventTestsL(KTimeOut, *this);
    
    // >> Respond Privacy Request
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);

    TLbsNetSessionId* getSessionId = NULL;
    CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
    cleanupCnt = 0;
    cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
    TESTL(getSessionId->SessionNum()==iSessionId2.SessionNum());
    TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
    CleanupStack::PopAndDestroy(cleanupCnt);

    // << ProcessLocationUpdate()
    TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
    iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId2, &positionInfo);

    // << ProcessAssistanceData()
    TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
    RLbsAssistanceDataBuilderSet assistanceData2;
    ArgUtils::PopulateLC(assistanceData2);
    reason = KErrNone;
    iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData2, &reason);
    CleanupStack::PopAndDestroy();

    // << ProcessLocationRequest()
    service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
    quality = ArgUtils::QualityAlpha2();
    method   = ArgUtils::RequestHybridMethod();
    iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId2, &emergency, &service, &quality, &method);

    // >> Callback from ProcessNetworkPostionUpdate(refPosition)
    CheckForObserverEventTestsL(KTimeOut, *this);

    // >> Callback from ProcessNetworkPostionUpdate(GPS Location)
    CheckForObserverEventTestsL(KTimeOut, *this);
    
    // check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...
    
    // >> RequestAssistanceData(0)
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
    TLbsAsistanceDataGroup dataGroup;
    CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup));
    TESTL(dataGroup == EAssistanceDataNone);

    // >> RespondLocationRequest()
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 
    getSessionId = NULL;
    TInt getReason = KErrNone;
    getPositionInfo = NULL;
    cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
    TESTL(getSessionId->SessionNum() == iSessionId2.SessionNum());
    TESTL(getReason == KErrNone);
    CleanupStack::PopAndDestroy(cleanupCnt);
    
    // << ProcessSessionComplete()
    reason = KErrNone;
    iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId2, &reason);

    // << ProcessStatusUpdate()
    activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone; 
    iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

    // >> Callback from ProcessRequestComplete()
    CheckForObserverEventTestsL(KTimeOut, *this);

    // Verify that the last callback was to ProcessSessionComplete()
    TESTL(iState==ERequestComplete);

	return TestStepResult();
	}



// MPosServerObserver
void CT_LbsHybridCombinedStep_Sequential01::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsHybridCombinedStep_Sequential01::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify error.
	TEST(aErr == KErrNone);

	// Verify position.

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		iState = ERefLocReceived;
		
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



void CT_LbsHybridCombinedStep_Sequential01::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkLocationRequest()"));
    TEST(iState==ERequestComplete);
    iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
    iState = EPrivacyCheckOk;
    ReturnToTestStep();
}

void CT_LbsHybridCombinedStep_Sequential01::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkPositionUpdate()"));
    if(!iPositionUpdateRef)
        {
        TEST(iState==EPrivacyCheckOk);
        iState=ERefLocReceived; 
        iPositionUpdateRef = ETrue; 
        }
    else
        {
        //This function is called back twice. First time when it returns the ref loc and second time when it returns a Gps loc
        //We need to test iState for different state
        TEST(iState==ERefLocReceived);
        iState=EGpsLocReceived;
        }
    ReturnToTestStep();
}

void CT_LbsHybridCombinedStep_Sequential01::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessRequestComplete()"));
    TEST(iState==EGpsLocReceived);
    iState=ERequestComplete; 
    ReturnToTestStep();
}

