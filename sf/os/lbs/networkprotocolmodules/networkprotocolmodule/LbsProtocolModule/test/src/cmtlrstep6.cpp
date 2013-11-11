// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file cmtlrstep6.cpp
*/

#include <lbspositioninfo.h>
#include "cmtlrstep6.h"
#include "te_testprotocolmodulesuitedefs.h"

const TReal32 KLocReqHorizAcc = 0xF6F6F6F6;
const TReal32 KRefPosHorizAcc = 0x6F6F6F6F;
_LIT8(KRequesterId, "MTLR6 Test Request");

/** Destructor
*/
CMtLrStep6::~CMtLrStep6()
	{
	}


/** Constructor
*/
CMtLrStep6::CMtLrStep6()
	{
	SetTestStepName(KMtLrStep6);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMtLrStep6::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMtLrStep6 test step.
This test verifies that the Test Protocol Module correctly handles 
MT-LR with privacy rejected after measurement control sent by network.

@return TVerdict test result code
*/
TVerdict CMtLrStep6::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMTLR - privacy rejected after measurement control sent by network"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate MT-LR
	INFO_PRINTF1(_L("\t                     ProcessRegisterLcsLocationNotification <- NET"));
	TLbsExternalRequestInfo reqInfo;
	reqInfo.SetRequesterId(KRequesterId());
	TLbsNetPosRequestPrivacy privacyReq;
	privacyReq.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	iNetworkObserver->ProcessRegisterLcsLocationNotificationL(reqInfo, privacyReq);
	
	// Check gateway receives privacy request
	TBuf8<KLbsMaxRequesterIdSize> id;
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessPrivacyRequest))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	iGatewayObserver->RequesterId(id);
	TLbsNetSessionId sessionId = iGatewayObserver->SessionIdValue();
	if (TLbsNetPosRequestPrivacy::ERequestActionAllow != iGatewayObserver->PrivacyNotifType().RequestAction() ||
		KRequesterId() != id)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessPrivacyRequest"));

	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	TPositionInfo refLoc;
	TPosition refPos;
	refPos.SetHorizontalAccuracy(KRefPosHorizAcc);
	refLoc.SetPosition(refPos);
	RLbsAssistanceDataBuilderSet assistData;
	CleanupClosePushL(assistData);
	assistData.OpenL();
	SetDummyAssistanceData(assistData, EAssistanceDataReferenceTime);
	TLbsNetPosRequestQuality quality;
	quality.SetMinHorizontalAccuracy(KLocReqHorizAcc);
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);
	CleanupStack::PopAndDestroy(&assistData);

	// Generate privacy reject response
	INFO_PRINTF1(_L("\tLBS -> RespondPrivacyRequest (REJECT)"));
	iModule->RespondPrivacyRequest(iGatewayObserver->SessionIdValue(),
									CLbsNetworkProtocolBase::EPrivacyResponseRejected);

	// Check gateway receives Assistance data
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessAssistanceData) ||
		EAssistanceDataReferenceTime != iGatewayObserver->AssistanceDataSetMask())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData"));

	// Check gateway receives Network Based location
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationUpdate) ||
		KRefPosHorizAcc != iGatewayObserver->ReferencePos().HorizontalAccuracy())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate"));

	// Check gateway receives Location Request
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationRequest) ||
		iGatewayObserver->IsLocEmergency() ||
		MLbsNetworkProtocolObserver::EServiceMobileTerminated != iGatewayObserver->LocType() ||
		KLocReqHorizAcc != iGatewayObserver->LocQuality().MinHorizontalAccuracy() ||
		sessionId != iGatewayObserver->SessionIdValue())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Check network receives privacy response
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsLocationNotification) ||
		CLbsNetworkProtocolBase::EPrivacyResponseRejected != iNetworkObserver->PrivacyResponse())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                    ReleaseLcsLocationNotification (REJECT) -> NET"));

	// Generate Location Response with error
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	TInt  reason = KErrPermissionDenied;
	TPositionInfo mobilePosInfo;
	iModule->RespondLocationRequest(iGatewayObserver->SessionIdValue(),
									reason, mobilePosInfo);
					
	// Check network receives Location error response
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EMeasurementControlFailure) ||
		KErrPermissionDenied != iNetworkObserver->MeasurementFailure())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                          MeasurementControlFailure (Error) -> NET"));
	
	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessSessionComplete) ||
		KErrNone != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));

	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
		iNetworkObserver->IsMoreObserverActivity())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	INFO_PRINTF1(_L("- END -"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/** Perform post test actions
@return TVerdict test result code
*/
TVerdict CMtLrStep6::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
