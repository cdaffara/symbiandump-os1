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
#include "ctlbshybridcombinedstep_sequential02.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsHybridCombinedStep_Sequential02* CT_LbsHybridCombinedStep_Sequential02::New(CT_LbsHybridCombinedServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridCombinedStep_Sequential02* testStep = new CT_LbsHybridCombinedStep_Sequential02(aParent);
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
CT_LbsHybridCombinedStep_Sequential02::CT_LbsHybridCombinedStep_Sequential02(CT_LbsHybridCombinedServer& aParent) : CT_LbsHybridCombinedStep_Base(aParent)
	{
	SetTestStepName(KLbsHybridCombinedStep_Sequential02);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	iSessionId2.SetSessionOwner(KRequestUid);
	iSessionId2.SetSessionNum(0x0002);
	}


void CT_LbsHybridCombinedStep_Sequential02::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridCombinedStep_Base::ConstructL();
    iController = CLbsPrivacyController::NewL(*this);
    iProxy = CNetProtocolProxy::NewL();
	}

/**
 * Destructor
 */
CT_LbsHybridCombinedStep_Sequential02::~CT_LbsHybridCombinedStep_Sequential02()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsHybridCombinedStep_Sequential02::doTestStepL()
	{
 	INFO_PRINTF1(_L("CT_LbsHybridCombinedStep_Sequential02::doTestStepL()"));	
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
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask2 = MLbsNetworkProtocolObserver::EServiceNone;
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

	// Create Client Objects...
	RPositionServer server;
	TInt connectError = server.Connect();
	TESTL(KErrNone == connectError);
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

	// Client Send - self locate request
	pWatch->IssueNotifyPositionUpdate();

	// >> RequestSelfLocation()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

	// check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...
	// TODO: this test is being queried - should the flag indicate any client request,
	//       or just AGPS client requests...
	
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
														EAssistanceDataNone, 
														(TPositionModuleInfo::ETechnologyTerminal)
													);
	TESTL(qualitycheck);
	
	iSessionId.SetSessionNum(sessionId->SessionNum());
	iSessionId.SetSessionOwner(sessionId->SessionOwner());
	
	CleanupStack::PopAndDestroy(cleanupCnt);
	sessionId = NULL;
	opts = NULL;

	
	
	
	// Send <-- ProcessStatusUpdate.
	serviceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask1);
	
	// Wait for AGPs module to send update
    TInt KTimeout = 1000000; // 1 seconds
    TESTL(iProxy->WaitForResponse(KTimeout) == ENetMsgTimeoutExpired);
	
	// check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...
	
	// Protocol Module Send <-- ProcessSessionComplete.
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// Protocol Module Send <-- ProcessStatusUpdate.
	serviceMask1 = MLbsNetworkProtocolObserver::EServiceNone;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask1);

	// Check client has received the gps position determined by the gps module
	CheckForObserverEventTestsL(KTimeOut, *this);

	// Wait for 10 seconds to ensure no additional responses turn up.
	TInt delta = 10 * 1000 * 1000;
	TNetProtocolResponseType mType = iProxy->WaitForResponse(delta);
	TESTL(mType == ENetMsgTimeoutExpired);
 
    // Done. Now cleanup...
    // all associated with MOLR Positioning Client 
    CleanupStack::PopAndDestroy(pWatch);
    CleanupStack::PopAndDestroy(); // pos
    CleanupStack::PopAndDestroy(); // server     

	// **************************************************
	
	
    INFO_PRINTF1(_L("Starting an MTLR..."));   
    TESTL(TestStepResult() == EPass);

    // << ProcessStatusUpdate()
    serviceMask1 = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
    iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask1);

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
    serviceMask2 = MLbsNetworkProtocolObserver::EServiceNone; 
    iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask2);

    // >> Callback from ProcessRequestComplete()
    CheckForObserverEventTestsL(KTimeOut, *this);

	return TestStepResult();
	}



// MPosServerObserver
void CT_LbsHybridCombinedStep_Sequential02::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsHybridCombinedStep_Sequential02::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify error.
	TEST(aErr == KErrNone);

	// Verify position.

	// Expecting GPS pos.
	if (iState == EInitializing)
		{
		iState = EGpsLocReceived;
		
		TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyTerminal);
		}
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}



void CT_LbsHybridCombinedStep_Sequential02::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkLocationRequest()"));
    iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
    iState = EPrivacyCheckOk;
    ReturnToTestStep();
}

void CT_LbsHybridCombinedStep_Sequential02::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkPositionUpdate()"));
    if(!iPositionUpdateRef)
        {
        iState=ERefLocReceived; 
        iPositionUpdateRef = ETrue; 
        }
    else
        {
        iState=EGpsLocReceived;
        }
    ReturnToTestStep();
}

void CT_LbsHybridCombinedStep_Sequential02::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessRequestComplete()"));
    iState=ERequestComplete; 
    ReturnToTestStep();
}

