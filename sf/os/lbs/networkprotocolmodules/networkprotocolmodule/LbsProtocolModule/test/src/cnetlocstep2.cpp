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
 @file cnetlocstep2.cpp
*/

#include <lbspositioninfo.h>
#include "cnetlocstep2.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CNetLocStep2::~CNetLocStep2()
	{
	}


/** Constructor
*/
CNetLocStep2::CNetLocStep2()
	{
	SetTestStepName(KNetLocStep2);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CNetLocStep2::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CNetLocStep2 test step.
This test verifies that the Test Protocol Module correctly handles 
an Network Based Location sequence with cancel before location 
response received from LBS.

@return TVerdict test result code
*/
TVerdict CNetLocStep2::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tNetwork Based Location - cancel before location response"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate Network Based Location
	INFO_PRINTF1(_L("\tLBS -> RequestNetworkLocation"));
	TLbsNetSessionId sessionId(TUid::Uid(0x12345678), 0x2222);
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
	
	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	TLbsAsistanceDataGroup dataRequestMask = (TUint) EAssistanceDataReferenceTime;
	TPositionInfo refLoc;
	RLbsAssistanceDataBuilderSet assistData;
	CleanupClosePushL(assistData);
	assistData.OpenL();
	SetDummyAssistanceData(assistData, dataRequestMask);
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
				CGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceNetworkLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Generate Cancellation
	INFO_PRINTF1(_L("\tLBS -> CancelNetworkLocation (REASON)"));
	iModule->CancelNetworkLocation(sessionId, KErrGeneral);

	// Check network receives cancel response
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementControlFailure) ||
		KErrCancel != iNetworkObserver->MeasurementFailure())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                         MeasurementControlFailure (CANCEL) -> NET"));
	
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
TVerdict CNetLocStep2::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
