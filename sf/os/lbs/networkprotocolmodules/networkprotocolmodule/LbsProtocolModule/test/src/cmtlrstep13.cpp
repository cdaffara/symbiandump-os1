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
 @file cmtlrstep13.cpp
*/

#include <lbspositioninfo.h>
#include "cmtlrstep13.h"
#include "te_testprotocolmodulesuitedefs.h"

const TReal32 KLocReqHorizAcc = 0xFDFDFDFD;
const TReal32 KRefPosHorizAcc = 0xDFDFDFDF;
_LIT8(KRequesterId, "MTLR13 Test Request");

/** Destructor
*/
CMtLrStep13::~CMtLrStep13()
	{
	}


/** Constructor
*/
CMtLrStep13::CMtLrStep13()
	{
	SetTestStepName(KMtLrStep13);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMtLrStep13::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMtLrStep13 test step.
This test verifies that the Test Protocol Module correctly handles 
an MT-LR sequence that is cancelled after preceding measure control.

@return TVerdict test result code
*/
TVerdict CMtLrStep13::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMTLR basic procedure cancelled after measure control"));
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
	if (TLbsNetPosRequestPrivacy::ERequestActionAllow != iGatewayObserver->PrivacyNotifType().RequestAction() ||
		KRequesterId() != id)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessPrivacyRequest"));

	// Generate privacy response
	INFO_PRINTF1(_L("\tLBS -> RespondPrivacyRequest"));
	iModule->RespondPrivacyRequest(iGatewayObserver->SessionIdValue(),
									CLbsNetworkProtocolBase::EPrivacyResponseAccepted);

	// Check network receives privacy response
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsLocationNotification) ||
		CLbsNetworkProtocolBase::EPrivacyResponseAccepted != iNetworkObserver->PrivacyResponse())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                             ReleaseLcsLocationNotification -> NET"));

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
		KLocReqHorizAcc != iGatewayObserver->LocQuality().MinHorizontalAccuracy())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Cancel MT-LR
	INFO_PRINTF1(_L("\t            ProcessMeasurementControlLocationError (CANCEL) <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationErrorL(KErrCancel);

	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		KErrCancel != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete (CANCEL)"));

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
TVerdict CMtLrStep13::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
