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

#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
// LBS test includes.
#include "ctlbshybridueassistedmtlrcancel.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"
#include <lbs/test/activeyield.h>
/**
Static Constructor
*/
CT_LbsHybridUEAssistedMTLRCancel* CT_LbsHybridUEAssistedMTLRCancel::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note that we do not use ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsHybridUEAssistedMTLRCancel* testStep = new CT_LbsHybridUEAssistedMTLRCancel(aParent);
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
CT_LbsHybridUEAssistedMTLRCancel::CT_LbsHybridUEAssistedMTLRCancel(CT_LbsHybridMTLRServer& aParent) : CT_LbsHybridMTLRStep(aParent)
	{
	SetTestStepName(KLbsHybridUEAssistedMTLRCancel);
	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);
	}


void CT_LbsHybridUEAssistedMTLRCancel::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iProxy = CNetProtocolProxy::NewL();	
	}


/**
 * Destructor
 */
CT_LbsHybridUEAssistedMTLRCancel::~CT_LbsHybridUEAssistedMTLRCancel()
	{
	delete iController;
	delete iProxy;
	}


TVerdict CT_LbsHybridUEAssistedMTLRCancel::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsHybridUEAssistedMTLRCancel::doTestStepL()"));	
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);

	const TInt KTimeOut = 60*1000*1000;
	const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
	const TInt KSmallTimeOut = 3*1000*1000;
	
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
// MTLR Assistance Data Notification Stop

// MTLR Network Location Request Start
	// << ProcessLocationRequest()
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2(); 
	TLbsNetPosRequestMethod method   = ArgUtils::RequestTerminalBasedMethod();
	iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
// MTLR Network Location Request Stop

	//Start the timer
	TTime timerStart;
	timerStart.HomeTime();
	
	// >> Callback from ProcessNetworkPostionUpdate(refPosition)
	CheckForObserverEventTestsL(KSmallTimeOut, *this);
	
// >> RequestAssistanceData(0)
	TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
	TLbsAsistanceDataGroup dataGroup;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
	TESTL(dataGroup == EAssistanceDataNone);
	CleanupStack::PopAndDestroy(cleanupCnt);
	
	InitiateCancelMTLR(iSessionId.SessionNum());
	// >> RespondLocationRequest()
	TInt response = iProxy->WaitForResponse(KSmallTimeOut);
	TESTL(response == ENetMsgRespondLocationRequest);
	
	
	getSessionId = NULL;
	TInt getReason = KErrCancel;
	TPositionSatelliteInfo* getPositionInfo = NULL;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
	TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
	TESTL(getReason==KErrCancel);
	CleanupStack::PopAndDestroy(cleanupCnt); //getSessionId,getPositionInfo

// MTLR Session Completion Start
	// << ProcessSessionComplete() 
	reason = KErrCancel;
	iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

	// >> Callback from ProcessRequestComplete()
	CheckForObserverEventTestsL(KTimeOut, *this);

	// Verify that the last callback was to ProcessSessionComplete()
	TESTL(iState==ERequestComplete);
// MTLR Session Completion Stop

	return TestStepResult();
	}
	
void CT_LbsHybridUEAssistedMTLRCancel::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRCancel::ProcessNetworkLocationRequest()"));
	TEST(iState==EInitializing);
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
	}
   
void CT_LbsHybridUEAssistedMTLRCancel::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& aPosInfo)
	{
	if(iState==EPrivacyCheckOk)
		{
		iState=ERefLocReceived;	
		INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRCancel::ProcessNetworkPositionUpdate(RefPosition)"));
		}
	else if(iState==ERefLocReceived)
		{
		// Test for $update,1,2,51.5015,-0.105,50,2,3*
		TPosition getPos;
		aPosInfo.GetPosition(getPos);
		if(getPos.Latitude()==51.5015 && getPos.Longitude()==-0.105 && getPos.Altitude()==50 && getPos.HorizontalAccuracy()==2 && getPos.VerticalAccuracy()==3) 		
			{
			INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRCancel::ProcessNetworkPositionUpdate(GpsPosition)"));
			iState=EGpsLocReceived;	
			}
		}
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedMTLRCancel::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRCancel::ProcessRequestComplete()"));
	TEST(iState==ERefLocReceived);
	iState=ERequestComplete; 
	ReturnToTestStep();
	}

void CT_LbsHybridUEAssistedMTLRCancel::InitiateCancelMTLR(TUint aRequestId)
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsHybridUEAssistedMTLRCancel::initiateCancelMTLR()"));
	iController->CancelNetworkLocationRequest(aRequestId);
	INFO_PRINTF1(_L("Cancel Initiated"));
	}
