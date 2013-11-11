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
 @file ctrackstep1.cpp
*/

#include <lbspositioninfo.h>
#include "ctrackstep1.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CTrackStep1::~CTrackStep1()
	{
	}


/** Constructor
*/
CTrackStep1::CTrackStep1()
	{
	SetTestStepName(KTrackStep1);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CTrackStep1::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CTrackStep1 test step.
This test verifies that the Test Protocol Module correctly handles 
a basic Tracking MO-LR sequence.

@return TVerdict test result code
*/
TVerdict CTrackStep1::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tTracking - basic MO-LR procedure followed"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	INFO_PRINTF1(_L("\tLBS -> AdviceSystemStatus (Tracking ON)"));
	iModule->AdviceSystemStatus(CLbsNetworkProtocolBase::ESystemStatusClientTracking);

	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0x1111);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataBadSatList;
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
				CGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceSelfLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	TInt  reason = KErrNone;
	TPositionInfo posInfo;
	iModule->RespondLocationRequest(iGatewayObserver->SessionIdValue(),
									reason, posInfo);
					
	// Check network receives Location Response
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EMeasurementReportLocation))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                  MeasurementReportLocation -> NET"));
	
	// Generate network result
	// Note: This incorporates a network-based location too
	INFO_PRINTF1(_L("\t                              ProcessFacilityLcsMoLrResultL <- NET"));
	TPositionInfo resultLoc;
	iNetworkObserver->ProcessFacilityLcsMoLrResultL(KErrNone, &resultLoc);
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(
				CNetworkObserver::EReleaseLcsMoLr))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                             ReleaseLcsMoLr -> NET"));
	
	// Check gateway receives Network based Loc
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessLocationUpdate))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate"));

	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessSessionComplete))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));
								
	INFO_PRINTF1(_L("\tLBS -> AdviceSystemStatus (Tracking OFF)"));
	iModule->AdviceSystemStatus(CLbsNetworkProtocolBase::ESystemStatusClientTracking);

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
TVerdict CTrackStep1::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
