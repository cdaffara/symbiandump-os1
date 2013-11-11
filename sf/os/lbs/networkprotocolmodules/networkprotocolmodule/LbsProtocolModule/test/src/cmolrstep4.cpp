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
 @file cmolrstep4.cpp
*/

#include <lbspositioninfo.h>
#include "cmolrstep4.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CMoLrStep4::~CMoLrStep4()
	{
	}


/** Constructor
*/
CMoLrStep4::CMoLrStep4()
	{
	SetTestStepName(KMoLrStep4);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep4::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMoLrStep4 test step.
This test verifies that the Test Protocol Module correctly handles 
MO-LR Self Locate cancellation.
Cancellation occurs before network sends measurement and control.

@return TVerdict test result code
*/
TVerdict CMoLrStep4::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR with Assistance data cancel before measurement control"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0x4444);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataReferenceTime;
	options1.SetDataRequestMask(dataRequestMask1);
	INFO_PRINTF1(_L("\tLBS -> RequestSelfLocation"));	
	iModule->RequestSelfLocation(sessionId1, options1);

	// Check network receives MO-LR request
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::ERegisterLcsMoLr))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                            RegisterLcsMoLr -> NET"));
	
	// Cancel MO-LR
	INFO_PRINTF1(_L("\tLBS -> CancelSelfLocation (CANCEL)"));	
	iModule->CancelSelfLocation(sessionId1, KErrCancel);

	// Check network session completed
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr) ||
		KErrCancel != iNetworkObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                    ReleaseLcsMoLr (CANCEL) -> NET"));

	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessSessionComplete))
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
TVerdict CMoLrStep4::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
