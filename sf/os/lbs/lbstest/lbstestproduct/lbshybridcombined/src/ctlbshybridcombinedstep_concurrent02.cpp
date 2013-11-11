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
#include "ctlbshybridcombinedstep_concurrent02.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsHybridCombinedStep_Concurrent02* CT_LbsHybridCombinedStep_Concurrent02::New(CT_LbsHybridCombinedServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridCombinedStep_Concurrent02* testStep = new CT_LbsHybridCombinedStep_Concurrent02(aParent);
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
CT_LbsHybridCombinedStep_Concurrent02::CT_LbsHybridCombinedStep_Concurrent02(CT_LbsHybridCombinedServer& aParent) : CT_LbsHybridCombinedStep_Base(aParent)
	{
	SetTestStepName(KLbsHybridCombinedStep_Concurrent02);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	iSessionId2.SetSessionOwner(KRequestUid);
	iSessionId2.SetSessionNum(0x0002);
	iClientPosUpdateCount =0;
	iNetworkPosUpdateCount =0;
	iState = EInitializing;
    }


void CT_LbsHybridCombinedStep_Concurrent02::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridCombinedStep_Base::ConstructL();
    iController = CLbsPrivacyController::NewL(*this);
    iProxy = CNetProtocolProxy::NewL();
	}

/**
 * Destructor
 */
CT_LbsHybridCombinedStep_Concurrent02::~CT_LbsHybridCombinedStep_Concurrent02()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsHybridCombinedStep_Concurrent02::doTestStepL()
	{
 	INFO_PRINTF1(_L("CT_LbsHybridCombinedStep_Concurrent02::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);
	const TInt KTimeOut = 80*1000*1000;

	// data declarations common to MOLR & MTLR
    TBool emergency = EFalse;
    TInt reason = KErrNone;
    MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    TLbsNetPosRequestQuality quality = ArgUtils::Quality();
    TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
    TPositionSatelliteInfo* getPositionInfo = NULL;
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask2 = MLbsNetworkProtocolObserver::EServiceNone;
    TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
    TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
    
    // Protocol Module receives the cababilities message as LBS starts up...
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	INFO_PRINTF1(_L("ENetMsgGetCurrentCapabilitiesResponse got"));
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

	
	// Client Send - self locate request... Autonomous mode selected in Admin...
	pWatch->IssueNotifyPositionUpdate();
    INFO_PRINTF1(_L("NotifyPositionUpdate sent"));
	// LBS->PM :: RequestSelfLocation()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);
    INFO_PRINTF1(_L("RequestSelfLocation got"));
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

	// PM->LBS :: ProcessStatusUpdate()
	serviceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask1);
	INFO_PRINTF1(_L("ProcessStatusUpdate sent"));
	// Check client has received the gps position determined by the gps module
	// Callback  from OnNotifyPositionUpdate
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// Start an MT-LR (TB requested...)
        
        // PM->LBS :: ProcessStatusUpdate()
        serviceMask1 |= MLbsNetworkProtocolObserver::EServiceMobileTerminated;
        iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask1);
        INFO_PRINTF1(_L("ProcessStatusUpdate sent"));
        // PM->LBS :: ProcessPrivacyRequest()
        emergency = ETrue;
        privacy    = ArgUtils::Privacy();
        requestInfo = ArgUtils::RequestInfo();
        iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId2, &emergency, &privacy, &requestInfo);
        INFO_PRINTF1(_L("ProcessPrivacyRequest sent"));
        
		// LBS->PM :: ProcessNetworkLocationRequest
        CheckForObserverEventTestsL(KTimeOut, *this);
        TEST(iState == EPrivacyCheckOk);
        INFO_PRINTF2(_L("iState=%d"),iState);
		TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
        INFO_PRINTF1(_L("RespondPrivacyRequest got"));
        // Check the privacy response (not really necessary here...)
        TLbsNetSessionId* getSessionId = NULL;
        CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
        cleanupCnt = 0;
        cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
        TESTL(getSessionId->SessionNum()==iSessionId2.SessionNum());
        TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
        CleanupStack::PopAndDestroy(cleanupCnt);
        
        // LBS->PM :: ProcessLocationUpdate() (will this result in the client NPUD being completed?!)
        TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
        iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId2, &positionInfo);
        INFO_PRINTF1(_L("ProcessLocationUpdate sent"));
        // LBS->PM :: ProcessAssistanceData()
        TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
        RLbsAssistanceDataBuilderSet assistanceData2;
        ArgUtils::PopulateLC(assistanceData2);
        reason = KErrNone;
        iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData2, &reason);
        CleanupStack::PopAndDestroy();
        INFO_PRINTF1(_L("ProcessAssistanceData sent"));
        
		// LBS->PM :: ProcessLocationRequest()
        service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
        quality = ArgUtils::QualityAlpha2();
        method   = ArgUtils::RequestHybridMethod();
        iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId2, &emergency, &service, &quality, &method);
        INFO_PRINTF1(_L("ProcessLocationRequest sent"));
        // >> Callback from ProcessNetworkPostionUpdate(refPosition)
        CheckForObserverEventTestsL(KTimeOut, *this);
        INFO_PRINTF2(_L("iNetworkPosUpdateCount is %d."), iNetworkPosUpdateCount);
		TEST(iNetworkPosUpdateCount==1);

        // >> Callback from ProcessNetworkPostionUpdate(GPS Location)
        CheckForObserverEventTestsL(KTimeOut, *this);
        INFO_PRINTF2(_L("iNetworkPosUpdateCount is %d."), iNetworkPosUpdateCount);
		TEST(iNetworkPosUpdateCount==2);
        
        // check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...
        // TODO: uncomment this out when fixed...
        
        // LBS->PM RequestAssistanceData(0)
        TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
        TLbsAsistanceDataGroup dataGroup;
        CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup));
        TESTL(dataGroup == EAssistanceDataNone);
		
        // LBS->PM RespondLocationRequest()
        TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest); 
        getSessionId = NULL;
        TInt getReason = KErrNone;
        getPositionInfo = NULL;
        cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
        TESTL(getSessionId->SessionNum() == iSessionId2.SessionNum());
        TESTL(getReason == KErrNone);
        CleanupStack::PopAndDestroy(cleanupCnt);
        INFO_PRINTF1(_L("RespondLocationRequest got"));
        
	// Wait for AGPs module to send update
    //TInt KTimeout = 1000000; // 1 seconds
    //TESTL(iProxy->WaitForResponse(KTimeout) == ENetMsgTimeoutExpired);
	
	// check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...
	
	// PM->LBS :: ProcessSessionComplete (session1)
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	INFO_PRINTF1(_L("ProcessSessionComplete for session1 sent"));
    // PM->LBS :: ProcessSessionComplete (session2)
    reason = KErrNone;
    iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId2, &reason);
    INFO_PRINTF1(_L("ProcessSessionComplete for session2 sent"));
    // PM->LBS :: ProcessStatusUpdate()
    serviceMask2 = MLbsNetworkProtocolObserver::EServiceNone; 
    iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask2);
    INFO_PRINTF1(_L("ProcessStatusUpdate got"));
		
	TEST(iClientPosUpdateCount==1);
    TEST(iNetworkPosUpdateCount==2);
	
    // >> Callback from ProcessRequestComplete()
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

	return TestStepResult();
	}



// MPosServerObserver
void CT_LbsHybridCombinedStep_Concurrent02::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsHybridCombinedStep_Concurrent02::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& /*aPosInfo*/)
	{
	INFO_PRINTF1(_L("CT_LbsHybridCombinedStep_Concurrent02::OnNotifyPositionUpdate()"));
	// Verify error.
	TEST(aErr == KErrNone);
	iClientPosUpdateCount++;
	ReturnToTestStep();
	}



void CT_LbsHybridCombinedStep_Concurrent02::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkLocationRequest()"));
    iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
    INFO_PRINTF1(_L("RespondNetworkLocationRequest sent"));
	iState = EPrivacyCheckOk;
    ReturnToTestStep();
}

void CT_LbsHybridCombinedStep_Concurrent02::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkPositionUpdate()"));
    iNetworkPosUpdateCount++;
    ReturnToTestStep();
}

void CT_LbsHybridCombinedStep_Concurrent02::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessRequestComplete()"));
    iState=ERequestComplete; 
    ReturnToTestStep();
}

