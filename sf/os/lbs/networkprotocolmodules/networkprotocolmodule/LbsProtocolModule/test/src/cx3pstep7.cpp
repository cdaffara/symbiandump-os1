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
 @file cx3pstep7.cpp
*/

#include <lbspositioninfo.h>
#include "cx3pstep7.h"
#include "te_testprotocolmodulesuitedefs.h"


_LIT(KX3pDestination, "x3pstep7@there.com");
const TUint KX3pSessionId = 0x107;
const TInt  KX3pPriority = 0x01;


/** Destructor
*/
CX3pStep7::~CX3pStep7()
	{
	}


/** Constructor
*/
CX3pStep7::CX3pStep7()
	{
	SetTestStepName(KX3pStep7);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CX3pStep7::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CX3pStep7 test step.
This test verifies that the Test Protocol Module correctly handles 
an X3P with timeout after registering for X3P.

@return TVerdict test result code
*/
TVerdict CX3pStep7::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tX3P timeout after registering request"));
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
	
	// Delay activity by 25 secs
	INFO_PRINTF1(_L("*** DELAY 25 SECONDS ***"));
	User::After(TTimeIntervalMicroSeconds32(25000000));

	// Check network session completed with timeout
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr) ||
		KErrTimedOut != iNetworkObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                   ReleaseLcsMoLr (TIMEOUT) -> NET"));

	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId != iGatewayObserver->SessionIdValue() ||
		KErrTimedOut != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete (TIMEOUT)"));

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
TVerdict CX3pStep7::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
