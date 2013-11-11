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
// LBS includes. 
// 
//

#include <lbs/test/lbsnetextendedprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
// LBS test includes.
#include "ctlbsextendedmtlrcancel.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>
/**
Static Constructor
*/
CT_LbsExtendedMTLRCancel* CT_LbsExtendedMTLRCancel::New(CT_LbsExtendedMTLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsExtendedMTLRCancel* testStep = new CT_LbsExtendedMTLRCancel(aParent);
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
CT_LbsExtendedMTLRCancel::CT_LbsExtendedMTLRCancel(CT_LbsExtendedMTLRServer& aParent) : CT_LbsExtendedMTLRStep(aParent)
	{
	SetTestStepName(KLbsExtendedMtlrCancel);
	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsExtendedMTLRCancel::ConstructL()
	{
	// Create the base class objects.
	CT_LbsExtendedMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();	
	}


/**
 * Destructor
 */
CT_LbsExtendedMTLRCancel::~CT_LbsExtendedMTLRCancel()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsExtendedMTLRCancel::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsExtendedMTLRCancel::doTestStepL()"));	
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
	const TInt KSmallTimeOut = 3*1000*1000;

	// reset integration modules count of number of cancels recieved from LBS 
	T_LbsUtils utils;

	utils.ResetIntegrationModulesCountOfCancelsL();

	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status); 
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

// Initiate MTLR Start
	// << ProcessStatusUpdate()
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);

	// << ProcessPrivacyRequest()
	const TBool emergency = EFalse;
	TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
	TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
	iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId, &emergency, &privacy, &requestInfo);

	// >> Callback from RespondNetworkLocationRequest(ERequestAccepted)
	CheckForObserverEventTestsL(KTimeOut, *this);
	
	// >> Respond Privacy Request
	TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
	TLbsNetSessionId* getSessionId = NULL;
	CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
	TESTL(getSessionId->SessionNum()==iSessionId.SessionNum());
	TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
	CleanupStack::PopAndDestroy(cleanupCnt);//getSessionId
// Initiate MTLR End

// MTLR Reference Position Notification Start
	// << ProcessLocationUpdate()
	TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
	iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);
// MTLR Reference Position Notification Stop

// MTLR Assistance Data Notification Start
	// << ProcessAssistanceData()
	TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
	RLbsAssistanceDataBuilderSet assistanceData;
	ArgUtils::PopulateLC(assistanceData);
	TInt reason = KErrNone;
	iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(); //assistanceData
	
	// wait for a time to allow the REF location to be delivered to the privacy controller
	User::After(KSmallTimeOut);
	
	// in this test we cancel without inputing a location request
	InitiateCancelMTLR(iSessionId.SessionNum());
	
	// >> CancelExternalLocation()
	TInt response = iProxy->WaitForResponse(KSmallTimeOut);
	TESTL(response == ENetMsgCancelExternalLocation);
	
	// it is the Extended  Protcol Module so we expect the CancelExternalLocation to arrive
	TInt getReason = KErrNone;
	getSessionId = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgCancelExternalLocation, &getSessionId, &getReason);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason==KErrCancel);
	CleanupStack::PopAndDestroy(cleanupCnt); //getSessionId,getPositionInfo
					
// MTLR Session Completion Start
	// << ProcessSessionComplete() 
	reason = KErrCancel;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	
	// >> Callback from ProcessNetworkPositionUpdate(RefPosition)
	CheckForObserverEventTestsL(KTimeOut, *this);
	TESTL(iState==ERefLocReceived);
	
		// >> Callback from ProcessRequestComplete()
	CheckForObserverEventTestsL(KTimeOut, *this);

	// Verify that the last callback was to ProcessSessionComplete()
	TESTL(iState==ERequestComplete);
	
	response = iProxy->WaitForResponse(2*1000*1000);
	TESTL(response == ENetMsgTimeoutExpired);
	
	// and finally check that no cancels were sent to test integration module

	TInt cancelCount  = utils.IntegrationModulesCountOfCancelsL();

	INFO_PRINTF2(_L("--- cancel count = %d"), cancelCount);

	TESTL(cancelCount== 0);

// MTLR Session Completion Stop

	return TestStepResult();
	}
	
void CT_LbsExtendedMTLRCancel::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsExtendedMTLRCancel::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
	}
   
void CT_LbsExtendedMTLRCancel::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& aPosInfo)
	{
	if(iState==EPrivacyCheckOk)
		{
		// check that it is the REF postion we are expecting!
		if (aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork)
			{
			TPosition getPos;
			aPosInfo.GetPosition(getPos);
			if (getPos.Longitude()==-0.096) // other tests check out all the postion data - so not done here
				{
				INFO_PRINTF1(_L("&gt;&gt;CT_LbsExtendedMTLRCancel::ProcessNetworkPositionUpdate(RefPosition)"));
				iState=ERefLocReceived;
				}
			}
		}
	ReturnToTestStep();
	}

void CT_LbsExtendedMTLRCancel::ProcessRequestComplete(TUint /*aRequestId*/, TInt aReason)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsExtendedMTLRCancel::ProcessRequestComplete()"));
	TEST(iState==ERefLocReceived);
	TEST(aReason ==KErrCancel);

	iState=ERequestComplete; 
	ReturnToTestStep();
	}

void CT_LbsExtendedMTLRCancel::InitiateCancelMTLR(TUint aRequestId)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsExtendedMTLRCancel::initiateCancelMTLR()"));
	iController->CancelNetworkLocationRequest(aRequestId);
	INFO_PRINTF1(_L("Cancel Initiated"));
	}
