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
 @file cmtlrstep5.cpp
*/

#include <lbspositioninfo.h>
#include "cmtlrstep5.h"
#include "te_testprotocolmodulesuitedefs.h"

_LIT8(KRequesterId, "MTLR5 Test Request");

/** Destructor
*/
CMtLrStep5::~CMtLrStep5()
	{
	}


/** Constructor
*/
CMtLrStep5::CMtLrStep5()
	{
	SetTestStepName(KMtLrStep5);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMtLrStep5::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMtLrStep5 test step.
This test verifies that the Test Protocol Module correctly handles 
MT-LR privacy rejection, before measurement control sent by network.

@return TVerdict test result code
*/
TVerdict CMtLrStep5::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMTLR - privacy rejected before measurement control"));
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

	// Generate privacy rejection response
	INFO_PRINTF1(_L("\tLBS -> RespondPrivacyRequest (REJECT)"));
	iModule->RespondPrivacyRequest(iGatewayObserver->SessionIdValue(),
									CLbsNetworkProtocolBase::EPrivacyResponseRejected);

	// Check network receives privacy response
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsLocationNotification) ||
		CLbsNetworkProtocolBase::EPrivacyResponseRejected != iNetworkObserver->PrivacyResponse())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                    ReleaseLcsLocationNotification (REJECT) -> NET"));

	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessSessionComplete) ||
		KErrNone != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));

	// Delay activity by 15 secs
	INFO_PRINTF1(_L("*** DELAY 15 SECONDS ***"));
	User::After(TTimeIntervalMicroSeconds32(15000000));

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
TVerdict CMtLrStep5::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
