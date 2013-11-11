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
 @file cnetlocstep7.cpp
*/

#include <lbspositioninfo.h>
#include "cnetlocstep7.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CNetLocStep7::~CNetLocStep7()
	{
	}


/** Constructor
*/
CNetLocStep7::CNetLocStep7()
	{
	SetTestStepName(KNetLocStep7);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CNetLocStep7::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CNetLocStep7 test step.
This test verifies that the Test Protocol Module correctly handles 
Network Based Location measurement control error

@return TVerdict test result code
*/
TVerdict CNetLocStep7::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tNetwork Based Location - measurement control error"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate Network Based Location
	INFO_PRINTF1(_L("\tLBS -> RequestNetworkLocation"));
	TLbsNetSessionId sessionId(TUid::Uid(0x12345678), 0x7777);
	TLbsNetPosRequestOptions options;
	options.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality;
	options.SetRequestQuality(quality);
	iModule->RequestNetworkLocation(sessionId, options);

	// Check network receives Network Based Location request
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::ERegisterLcsMoLr))
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
		KErrLocked != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete (error)"));
								
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
TVerdict CNetLocStep7::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
