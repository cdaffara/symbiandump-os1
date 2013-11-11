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
 @file cx3pstep4.cpp
*/

#include <lbspositioninfo.h>
#include "cx3pstep4.h"
#include "te_testprotocolmodulesuitedefs.h"


_LIT(KX3pDestination, "x3pstep4@there.com");
const TUint KX3pSessionId = 0x104;
const TInt  KX3pPriority = 0x01;


/** Destructor
*/
CX3pStep4::~CX3pStep4()
	{
	}


/** Constructor
*/
CX3pStep4::CX3pStep4()
	{
	SetTestStepName(KX3pStep4);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CX3pStep4::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CX3pStep4 test step.
This test verifies that the Test Protocol Module correctly handles 
X3P cancellation.
Cancellation occurs before network sends measurement and control.

@return TVerdict test result code
*/
TVerdict CX3pStep4::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tX3P with cancel before measurement control"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate X3P
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation"));
	TLbsNetSessionId sessionId(TUid::Uid(KDummyUid), KX3pSessionId);
	iModule->RequestTransmitLocation(sessionId, KX3pDestination(), KX3pPriority);

	// Check network receives X3P request
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::ERegisterLcsMoLr) ||
		KX3pDestination() != iNetworkObserver->X3pDestination())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                            RegisterLcsMoLr -> NET"));
	
	// Cancel X3P
	INFO_PRINTF1(_L("\tLBS -> CancelTransmitLocation (REASON)"));	
	iModule->CancelTransmitLocation(sessionId, KErrGeneral);

	// Check network session completed
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr) ||
		KErrGeneral != iNetworkObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                    ReleaseLcsMoLr (REASON) -> NET"));
	
	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId != iGatewayObserver->SessionIdValue() ||
		KErrGeneral != iGatewayObserver->SessionCloseReason())
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
TVerdict CX3pStep4::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
