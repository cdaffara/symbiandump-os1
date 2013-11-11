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
 @file cmolrstep8.cpp
*/

#include <lbspositioninfo.h>
#include "cmolrstep8.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CMoLrStep8::~CMoLrStep8()
	{
	}


/** Constructor
*/
CMoLrStep8::CMoLrStep8()
	{
	SetTestStepName(KMoLrStep8);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep8::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMoLrStep8 test step.
This test verifies that the Test Protocol Module correctly handles 
an MO-LR Self Locate sequence.

@return TVerdict test result code
*/
TVerdict CMoLrStep8::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR timeout waiting for location response"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));
	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0x8888);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataReferenceTime;
	options1.SetDataRequestMask(dataRequestMask1);
	options1.SetPosMode(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
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
	
	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	TPositionInfo refLoc;
	RLbsAssistanceDataBuilderSet assistData;
	CleanupClosePushL(assistData);
	assistData.OpenL();
	SetDummyAssistanceData(assistData, dataRequestMask1);
	TLbsNetPosRequestQuality quality;
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);
	CleanupStack::PopAndDestroy(&assistData);

	// Check gateway receives Assistance data
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessAssistanceData))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData"));

	// Check gateway receives Network Based location
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationUpdate))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate"));

	// Check gateway receives Location Request
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationRequest))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));
	TLbsNetSessionId sessionId = iGatewayObserver->SessionIdValue();

	// Delay activity by 70 secs
	INFO_PRINTF1(_L("*** DELAY 70 SECONDS ***"));
	User::After(TTimeIntervalMicroSeconds32(70000000));

	// Check network receives timeout response
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementControlFailure) ||
		KErrTimedOut != iNetworkObserver->MeasurementFailure())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                        MeasurementControlFailure (TIMEOUT) -> NET"));

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
TVerdict CMoLrStep8::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
