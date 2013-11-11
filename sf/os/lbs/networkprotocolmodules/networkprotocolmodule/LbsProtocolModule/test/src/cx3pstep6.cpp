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
 @file cx3pstep6.cpp
*/

#include <lbspositioninfo.h>
#include "cx3pstep6.h"
#include "te_testprotocolmodulesuitedefs.h"


_LIT(KX3pDestination, "x3pstep6@there.com");
const TUint KX3pSessionId = 0x106;
const TInt  KX3pPriority = 0x01;


/** Destructor
*/
CX3pStep6::~CX3pStep6()
	{
	}


/** Constructor
*/
CX3pStep6::CX3pStep6()
	{
	SetTestStepName(KX3pStep6);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CX3pStep6::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CX3pStep6 test step.
This test verifies that the Test Protocol Module correctly handles 
X3P with measurement control error.
Network reports error in response to X3P register request.

@return TVerdict test result code
*/
TVerdict CX3pStep6::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tX3P with Measurement Control error reported by network"));
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
	
	// Generate network Measurement Control error
	INFO_PRINTF1(_L("\t                     ProcessMeasurementControlLocationError <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationErrorL(KErrLocked);
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                             ReleaseLcsMoLr -> NET"));
	
	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId != iGatewayObserver->SessionIdValue() ||
		KErrLocked != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete (ERROR)"));

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
TVerdict CX3pStep6::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
