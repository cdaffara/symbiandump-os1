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
#include "ctlbshybridcombinedstep_tracking02.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>

const TInt KTestMaxFixTime = 30*1000*1000;

/**
Static Constructor
*/
CT_LbsHybridCombinedStep_Tracking02* CT_LbsHybridCombinedStep_Tracking02::New(CT_LbsHybridCombinedServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridCombinedStep_Tracking02* testStep = new CT_LbsHybridCombinedStep_Tracking02(aParent);
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
CT_LbsHybridCombinedStep_Tracking02::CT_LbsHybridCombinedStep_Tracking02(CT_LbsHybridCombinedServer& aParent) : CT_LbsHybridCombinedStep_Base(aParent)
	{
	SetTestStepName(KLbsHybridCombinedStep_Tracking02);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	iSessionId2.SetSessionOwner(KRequestUid);
	iSessionId2.SetSessionNum(0x0002);
	iClientPosUpdateCount =0;
	iNetworkPosUpdateCount =0;
	}


void CT_LbsHybridCombinedStep_Tracking02::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridCombinedStep_Base::ConstructL();
    iController = CLbsPrivacyController::NewL(*this);
    iProxy = CNetProtocolProxy::NewL();
	}

/**
 * Destructor
 */
CT_LbsHybridCombinedStep_Tracking02::~CT_LbsHybridCombinedStep_Tracking02()
	{
	delete iController;
	delete iProxy;
	}



TVerdict CT_LbsHybridCombinedStep_Tracking02::doTestStepL()
	{
 	INFO_PRINTF1(_L("CT_LbsHybridCombinedStep_Tracking02::doTestStepL()"));	
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
    TNetProtocolResponseType mType;
    
    // Protocol Module receives the cababilities message as LBS starts up...
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
    INFO_PRINTF1(_L("ENetMsgGetCurrentCapabilitiesResponse got"));
	// Create Client Objects...
	RPositionServer server;
	TInt connectError = server.Connect();
	TESTL(KErrNone == connectError);
	CleanupClosePushL(server);	
	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);
	
	// set client update options
	TBool trackingEnabled = ETrue;
	if (trackingEnabled)
	{
		T_LbsUtils utils;
	    TPtrC configFileName;
	    _LIT(KUpdateOptionsFile, "agps_module_update_file");
	    GetStringFromConfig(ConfigSection(), KUpdateOptionsFile, configFileName);
	    utils.GetConfigured_ModuleUpdateOptionsL(configFileName, ConfigSection(), iUpdateOpts);
	    pos.SetUpdateOptions(iUpdateOpts);
	}
	
	// Client Send - self locate request... TB mode selected in Admin...
	pWatch->IssueNotifyPositionUpdate();
    INFO_PRINTF1(_L("NotifyPositionUpdate sent"));
    // Get current time.
    iClientInitialRequestTime.UniversalTime();
	
	// LBS->PM :: RequestSelfLocation()
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);
    INFO_PRINTF1(_L("RequestSelfLocation got"));

	// check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...
	
        // Process the response.
        TLbsNetSessionId* 					sessionId = NULL;
        TLbsNetPosRequestOptionsAssistance*	opts = NULL;
        cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);
        TBool qualitycheck = 	ArgUtils::CompareQuality(	opts, 
                                                            ETrue, 
                                                            KMinHorizontalAcc, 
                                                            KMinVerticalAcc, 
                                                            KTestMaxFixTime,
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

    // PM->LBS ProcessStatusUpdate(EServiceSelfLocation)
    serviceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask1);
    INFO_PRINTF1(_L("ProcessStatusUpdate sent"));
    // PM->LBS ProcessLocationUpdate(SessionId, RefPosition)
    TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
    iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);
    INFO_PRINTF1(_L("ProcessLocationUpdate sent"));
    // LBS->CLIENT OnNotifyPositionUpdate()
    CheckForObserverEventTestsL(KTimeOut, *this);
    TEST(iClientPosUpdateCount==1);
    
    // second request... the interval is 5 seconds..
    // CLIENT->LBS IssueNotifyPositionUpdate();
    pWatch->IssueNotifyPositionUpdate();
    INFO_PRINTF1(_L("second NotifyPositionUpdate sent"));
    
        // an MTLR now happens with Max Fix Time before the client interval expires..
        //    (max fix time == 12 seconds)
    
        // << ProcessStatusUpdate()
        serviceMask1 |= MLbsNetworkProtocolObserver::EServiceMobileTerminated;
        iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask1);
        INFO_PRINTF1(_L("ProcessStatusUpdate sent"));
        // PM->LBS ProcessPrivacyRequest()
        emergency = ETrue;
        privacy    = ArgUtils::Privacy();
        requestInfo = ArgUtils::RequestInfo();
        iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId2, &emergency, &privacy, &requestInfo);
        INFO_PRINTF1(_L("ProcessPrivacyRequest sent"));
        // LBS->NRH Callback from RespondNetworkLocationRequest(ERequestAccepted)
        CheckForObserverEventTestsL(KTimeOut, *this);
        
        // NRH->PM Respond Privacy Request
        TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
        INFO_PRINTF1(_L("RespondPrivacyRequest got")); 
		
        TLbsNetSessionId* getSessionId = NULL;
        CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
        cleanupCnt = 0;
        cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
        TESTL(getSessionId->SessionNum()==iSessionId2.SessionNum());
        TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
        CleanupStack::PopAndDestroy(cleanupCnt);
    
        // PM->LBS ProcessLocationUpdate()
        TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
        iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId2, &positionInfo);
        INFO_PRINTF1(_L("ProcessLocationUpdate sent"));
        // PM->LBS ProcessAssistanceData()
        TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
        RLbsAssistanceDataBuilderSet assistanceData2;
        ArgUtils::PopulateLC(assistanceData2);
        reason = KErrNone;
        iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData2, &reason);
        CleanupStack::PopAndDestroy();
        INFO_PRINTF1(_L("ProcessAssistanceData sent"));
        
		// PM->LBS ProcessLocationRequest(TB) (max fix time == 12 seconds)
        service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
        quality = ArgUtils::QualityAlpha2(); 
        method   = ArgUtils::RequestTerminalBasedMethod();
        iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId2, &emergency, &service, &quality, &method);
        INFO_PRINTF1(_L("PM->LBS ProcessLocationRequest(TB) sent"));
            
        // LBS->NRH Callback from ProcessNetworkPostionUpdate(refPosition)
        CheckForObserverEventTestsL(KTimeOut, *this);
        
        // LBS->NRH Callback from ProcessNetworkPostionUpdate(GPS Location)
        CheckForObserverEventTestsL(KTimeOut, *this);
        // check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...
        
        // LBS->PM RequestAssistanceData(0)
        TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
        INFO_PRINTF1(_L("RequestAssistanceData got"));
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
        // PM->LBS ProcessSessionComplete()
        reason = KErrNone;
        iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId2, &reason);
        INFO_PRINTF1(_L("ProcessSessionComplete sent"));
        // PM->LBS ProcessStatusUpdate() - still doing the self locate...
        serviceMask2 = MLbsNetworkProtocolObserver::EServiceSelfLocation; 
        iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask2);
        INFO_PRINTF1(_L("ProcessStatusUpdate sent"));
        // LBS->NRH Callback from ProcessRequestComplete()
        CheckForObserverEventTestsL(KTimeOut, *this);

        
    // the GPS Module should wait *nearly* 10 seconds (the tracking interval
    // for this test) before continuing to process the location request...

    // LBS->PM CurrentCapabilitiesResponse - informing the PM we're tracking...
    mType = iProxy->WaitForResponse(KTimeOut);
    TESTL(mType == ENetMsgGetCurrentCapabilitiesResponse);
    
    // LBS->PM RequestAssistanceData()
    TLbsAsistanceDataGroup dataMask;    
    mType = iProxy->WaitForResponse(KTimeOut);
    TESTL(mType == ENetMsgRequestAssistanceData); 
    cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
    TESTL(dataMask == EAssistanceDataNone);
    CleanupStack::PopAndDestroy(cleanupCnt);
    
    // check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...

    // PM->LBS ProcessAssistanceData()
    dataMask = EAssistanceDataReferenceTime;
    RLbsAssistanceDataBuilderSet assistanceData;
    ArgUtils::PopulateLC(assistanceData);
    reason = KErrNone;
    iProxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
    CleanupStack::PopAndDestroy(); // assistanceData
    
    // PM->LBS ProcessLocationRequest() (NRH Request)
    service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
    quality = ArgUtils::Quality();
    method   = ArgUtils::RequestHybridMethod();
    iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);

    // LBS->PM RequestAssistanceData()
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
    cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
    TESTL(dataMask == EAssistanceDataNone);
    CleanupStack::PopAndDestroy(cleanupCnt);
    
    // check the Client AGPS Usage Flag is as expected at the NPE Hybrid GPS module...

    // LBS->PM RespondLocationRequest()
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondLocationRequest);
    sessionId = NULL;
    reason = KErrNone;
    getPositionInfo = NULL;
    cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &getPositionInfo);
    TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
    TESTL(reason == KErrNone);

    // PM->LBS ProcessLocationUpdate(SessionId, FinalNetworkPosition)
    iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, getPositionInfo);

    CleanupStack::PopAndDestroy(cleanupCnt);
    
    // Client recv - the gps position determined by the gps module.
    CheckForObserverEventTestsL(KTimeOut, *this);
    TESTL(iClientPosUpdateCount==2);
    
    // End of second client request ********************
 
    // close the client session
    pos.Close();
    
    // LBS->PM CurrentCapabilitiesResponse - informing the PM we're no longer tracking...
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);

    // LBS->PM CurrentCapabilitiesResponse - cancel the self location request.
    TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgCancelSelfLocation);

    // PM->LBS ProcessSessionComplete()
    reason = KErrNone;
    iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

    // PM->LBS ProcessStatusUpdate()
    serviceMask2 = MLbsNetworkProtocolObserver::EServiceNone; 
    iProxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask2);
    
    // Wait for 10 seconds to ensure no additional responses turn up.
    TInt delta = 10 * 1000 * 1000;
    mType = iProxy->WaitForResponse(delta);
    TESTL(mType == ENetMsgTimeoutExpired);
    
    
    // Done. Now cleanup...
    // all associated with MOLR Positioning Client 
    CleanupStack::PopAndDestroy(pWatch);
    CleanupStack::PopAndDestroy(); // pos
    CleanupStack::PopAndDestroy(); // server
    
	return TestStepResult();
	}



// MPosServerObserver
void CT_LbsHybridCombinedStep_Tracking02::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsHybridCombinedStep_Tracking02::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& /*aPosInfo*/)
	{
	INFO_PRINTF1(_L("CT_LbsHybridCombinedStep_Tracking02::OnNotifyPositionUpdate()"));
	// Verify error.
	TEST(aErr == KErrNone);
	iClientUpdateTime.UniversalTime();
	iClientPosUpdateCount++;
	ReturnToTestStep();
	}



void CT_LbsHybridCombinedStep_Tracking02::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkLocationRequest()"));
    iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
    ReturnToTestStep();
}

void CT_LbsHybridCombinedStep_Tracking02::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessNetworkPositionUpdate()"));
    iNetworkPosUpdateCount++;
    ReturnToTestStep();
}

void CT_LbsHybridCombinedStep_Tracking02::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
{
    INFO_PRINTF1(_L("&gt;&gt;CT_LbsUEBasedMTLR::ProcessRequestComplete()"));
    ReturnToTestStep();
}

