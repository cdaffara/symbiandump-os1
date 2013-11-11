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
 @file cnetlocstep10.cpp
*/

#include <lbspositioninfo.h>
#include "cnetlocstep10.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CNetLocStep10::~CNetLocStep10()
	{
	}


/** Constructor
*/
CNetLocStep10::CNetLocStep10()
	{
	SetTestStepName(KNetLocStep10);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CNetLocStep10::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CNetLocStep10 test step.
This test verifies that the Test Protocol Module correctly handles 
an Network Based Location sequence cancelled by the network.

@return TVerdict test result code
*/
TVerdict CNetLocStep10::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tNetwork Based Location basic procedure cancelled by network"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate Network Based Location
	INFO_PRINTF1(_L("\tLBS -> RequestNetworkLocation"));
	TLbsNetSessionId sessionId(TUid::Uid(0x12345678), 0xaaaa);
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
	
	// Cancel
	INFO_PRINTF1(_L("\t            ProcessMeasurementControlLocationError (CANCEL) <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationErrorL(KErrCancel);

	// Check network session completed
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr) ||
		KErrNone != iNetworkObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                             ReleaseLcsMoLr -> NET"));
	
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
TVerdict CNetLocStep10::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
