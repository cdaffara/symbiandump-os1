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
// This is the class implementation for the MO-LR - Error Assistance Tests
// 
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsadmin.h>

// LBS test includes.
#include "ctlbsmolrerrorassistance.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>


/**
Static Constructor
*/
CT_LbsMolrErrorAssistance* CT_LbsMolrErrorAssistance::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMolrErrorAssistance* testStep = new CT_LbsMolrErrorAssistance(aParent);
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
CT_LbsMolrErrorAssistance::CT_LbsMolrErrorAssistance(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsMOLRErrorAssistance);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsMolrErrorAssistance::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}


/**
 * Destructor
 */
CT_LbsMolrErrorAssistance::~CT_LbsMolrErrorAssistance()
	{
	}


TVerdict CT_LbsMolrErrorAssistance::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsMolrErrorAssistance::doTestStepL()"));
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

    const TInt KTimeOut = 60*1000*1000;

	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

    INFO_PRINTF1(_L(" >> AdviceSystemStatus(0)"));
	// >> AdviceSystemStatus(0) - GetCurrentCapabilitiesResponse
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = proxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	// Start Test Step
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	
	CPosServerWatcher *pWatch = NULL;
	TInt reason = KErrNone;
	CleanupClosePushL(pos);
	// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KMOLRFixTime));
	pos.SetUpdateOptions(posOpts);
	
	pWatch = CPosServerWatcher::NewLC(pos, *this);

    INFO_PRINTF1(_L("Issuing NPUD"));
	// Request a self-locate MoLr.
	pWatch->IssueNotifyPositionUpdate();

	// >> RequestSelfLocation()
    INFO_PRINTF1(_L(" >> RequestSelfLocation"));
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

	// Process the response.
	TLbsNetSessionId* 					sessionId = NULL;
	TLbsNetPosRequestOptionsAssistance*	opts = NULL;
	
	cleanupCnt = proxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

	TBool qualitycheck = 	ArgUtils::CompareQuality(	opts, 
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
	
	// << ProcessStatusUpdate(EServiceSelfLocation)
    INFO_PRINTF1(_L(" << ProcessStatusUpdate(EServiceSelfLocation)"));
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// << ProcessLocationUpdate(SessionId, RefPosition)
    INFO_PRINTF1(_L(" << ProcessLocationUpdate(SessionId, RefPosition)"));
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

	// TEST: Get the ref pos app side.
	CheckForObserverEventTestsL(KTimeOut, *this);
    INFO_PRINTF1(_L("Issuing NPUD"));
    pWatch->IssueNotifyPositionUpdate();

    INFO_PRINTF1(_L(" << ProcessSessionComplete(SessionId, KErrNotFound)"));
    reason = KErrNotFound; 
    proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	
    // >> RequestAssistanceData(0)
    INFO_PRINTF1(_L(" >> RequestAssistanceData(0)"));
    TLbsAsistanceDataGroup dataMask = EAssistanceDataNone;
    TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
    cleanupCnt = proxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
    TESTL(dataMask == EAssistanceDataNone);
    CleanupStack::PopAndDestroy(cleanupCnt);
    
    // TEST: Get the Autonomous position app side.
    CheckForObserverEventTestsL(KTimeOut, *this);

    // << ENetMsgProcessStatusUpdate()
    INFO_PRINTF1(_L(" << ProcessStatusUpdate(0)"));
    MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
    proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);
    
    // Wait for 10 seconds to ensure no additional responses turn up.
    INFO_PRINTF1(_L("Waiting to check no further messages happened"));
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
void CT_LbsMolrErrorAssistance::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
    INFO_PRINTF1(_L("Last known position received"));
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsMolrErrorAssistance::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
    INFO_PRINTF1(_L("Position update received"));
	// Verify error.
	if (aErr == KErrCancel)
		{
		iState = ERefLocReceived;
		}
	else
		{
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
            TEST(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
        // Expecting network pos.
        else if (iState == ERefLocReceived)
            {
            iState = EGpsLocReceived;

            TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal));
            }
	
	// Not expecting anything else.
        else
            {
            TEST(EFalse);
            }
		}
	ReturnToTestStep();
	}
