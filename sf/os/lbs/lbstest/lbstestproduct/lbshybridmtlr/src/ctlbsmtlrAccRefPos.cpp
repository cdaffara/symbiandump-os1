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
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsloccommon.h>
#include <lbserrors.h>

// LBS test includes.
#include "ctlbsmtlrAccRefPos.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>

/**
Static Constructor
*/
CT_LbsMtlrAccRefPos* CT_LbsMtlrAccRefPos::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMtlrAccRefPos* testStep = new CT_LbsMtlrAccRefPos(aParent);
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
CT_LbsMtlrAccRefPos::CT_LbsMtlrAccRefPos(CT_LbsHybridMTLRServer& aParent) : CT_LbsHybridMTLRStep(aParent)
	{
	SetTestStepName(KLbsMTLRAccurateReferencePosition);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	iPositionUpdateRef = EFalse;
	}


void CT_LbsMtlrAccRefPos::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();
	}


/**
 * Destructor
 */
CT_LbsMtlrAccRefPos::~CT_LbsMtlrAccRefPos()
	{
	delete iController;
	delete iProxy;
	}

TVerdict CT_LbsMtlrAccRefPos::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsMtlrAccRefPos::doTestStepL()"));	
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;


	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	CleanupStack::PopAndDestroy(iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status));
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
			
	// Initiate MTLR Start
	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask 
		= MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// << ProcessPrivacyRequest()
	const TBool KEmergency(ETrue);
	TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
	TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
	iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId, &KEmergency, &privacy, &requestInfo);

	// >> Callback from RespondNetworkLocationRequest(ERequestAccepted)
	CheckForObserverEventTestsL(KTimeOut, *this);
	

	// >> Respond Privacy Request
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
	TLbsNetSessionId* sessionId = NULL;
	CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
	TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &sessionId, &getPrivacy);
	TESTL(sessionId->SessionNum()==iSessionId.SessionNum());
	TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
	CleanupStack::PopAndDestroy(cleanupCnt);
	// Initiate MTLR End

// MTLR Reference Position Notification Start
	// << ProcessLocationUpdate()
	TPositionInfo refPosition = ArgUtils::ReferencePositionInfo();
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosition);
// MTLR Reference Position Notification End

// MTLR Assistance Data Notification Start
	// << ProcessAssistanceData()
	TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason(KErrNone);
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy();
// MTLR Assistance Data Notification End

	// >> Callback from ProcessNetworkPostionUpdate(refPosition)
	CheckForObserverEventTestsL(KTimeOut, *this);

// MTLR Session Completion Start
	// << ProcessSessionComplete()
	reason = KErrNone;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

	// >> Callback from ProcessRequestComplete()
	CheckForObserverEventTestsL(KTimeOut, *this);
	// Verify that the last callback was to ProcessSessionComplete()
	TESTL(iState==ERequestComplete);
// MTLR Session Completion End

	return TestStepResult();
	}

void CT_LbsMtlrAccRefPos::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMtlrAccRefPos::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
}

void CT_LbsMtlrAccRefPos::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridMTLR::ProcessNetworkPositionUpdate()"));
	if(!iPositionUpdateRef)
		{
		TEST(iState == EPrivacyCheckOk);
		iState = ERefLocReceived;	
		iPositionUpdateRef = ETrue;	
		}
	else
		{
		// Not expecting anything else.
		TEST(EFalse);
		}
	ReturnToTestStep();
}

void CT_LbsMtlrAccRefPos::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMtlrAccRefPos::ProcessRequestComplete()"));
	TEST(iState == ERefLocReceived);
	iState = ERequestComplete; 
	ReturnToTestStep();
}
