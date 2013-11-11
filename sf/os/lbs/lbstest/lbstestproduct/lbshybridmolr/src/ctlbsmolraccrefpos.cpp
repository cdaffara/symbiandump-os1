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
//

// LBS includes. 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>

// LBS test includes.
#include "ctlbsmolraccrefpos.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

/**
Static Constructor
*/
CT_LbsMOLRAccRefPos* CT_LbsMOLRAccRefPos::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMOLRAccRefPos* testStep = new CT_LbsMOLRAccRefPos(aParent);
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
CT_LbsMOLRAccRefPos::CT_LbsMOLRAccRefPos(CT_LbsHybridMOLRServer& aParent) : CT_LbsHybridMOLRStep(aParent)
	{
	SetTestStepName(KLbsMOLRAccurateReferencePosition);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsMOLRAccRefPos::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}


/**
 * Destructor
 */
CT_LbsMOLRAccRefPos::~CT_LbsMOLRAccRefPos()
	{
	}

TVerdict CT_LbsMOLRAccRefPos::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridMOLRFutile::doTestStepL()"));	
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;

	// Carryout unique test actions.
		
	// Create Network Protocol Proxy
	CNetProtocolProxy* proxy = CNetProtocolProxy::NewL();
	CleanupStack::PushL(proxy);

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
	CleanupClosePushL(pos);

	// Set the max fix time for the client request to ensure the location server does not complete the request too soon during the test.
	TPositionUpdateOptions 
		posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(KMOLRFixTime));
	pos.SetUpdateOptions(posOpts);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

	// Request a self-locate MoLr.
	pWatch->IssueNotifyPositionUpdate();

	// >> RequestSelfLocation()
	TESTL(proxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

	// Process the response.
	TLbsNetSessionId* sessionId = NULL;
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
	
	// << ProcessStatusUpdate(EServiceSelfLocation)
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask serviceMask = 
		MLbsNetworkProtocolObserver::EServiceSelfLocation;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

	// << ProcessLocationUpdate(SessionId, RefPosition)
	TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

	// TEST: Get the ref pos app side.
	CheckForObserverEventTestsL(KTimeOut, *this);

	// << ProcessAssistanceData()
	TLbsAsistanceDataGroup dataMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason(KErrNone);
	proxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); // assistanceData

	// <<  ProcessLocationUpdate - return network calculated pos
	TPositionInfo networkPosInfo = ArgUtils::MolrNetworkPositionInfo();
	proxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &networkPosInfo);		

	// TEST: Get the final network position(= Ref Position)
	TESTL(iState == ERefLocReceived);

	// << ProcessSessionComplete(SessionId, KErrNone)	
	reason = KErrNone;
	proxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ENetMsgProcessStatusUpdate()
	serviceMask = MLbsNetworkProtocolObserver::EServiceNone;
	proxy->CallL(ENetMsgProcessStatusUpdate, &serviceMask);

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
void CT_LbsMOLRAccRefPos::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}

void CT_LbsMOLRAccRefPos::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	// Verify position.
	TEST(aPosInfo.PositionClassType() == EPositionInfoClass);

	// Expecting ref pos.
	if (iState == EInitializing)
		{
		// Verify error.
		TEST(aErr == KErrNone);
		iState = ERefLocReceived;
		TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
		
		// check for refpos details
		const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
		TESTL(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
		}
		
	// Not expecting anything else.
	else
		{
		TEST(EFalse);
		}

	ReturnToTestStep();
	}
