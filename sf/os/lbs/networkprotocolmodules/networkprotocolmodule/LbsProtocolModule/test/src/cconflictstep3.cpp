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
 @file cconflictstep3.cpp
*/

#include <lbspositioninfo.h>
#include "cconflictstep3.h"
#include "te_testprotocolmodulesuitedefs.h"

_LIT(KX3pDestination, "x3pstep3@there.com");
const TUint KX3pSessionId = 0x303;
const TInt  KX3pPriorityTimer = 0x10;

/** Destructor
*/
CConflictStep3::~CConflictStep3()
	{
	}


/** Constructor
*/
CConflictStep3::CConflictStep3()
	{
	SetTestStepName(KConflictStep3);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CConflictStep3::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CConflictStep3 test step.
Verify Test Protocol Module performance for a conflict situation:
First:  MO-LR
Second: X3P Timer

@return TVerdict test result code
*/
TVerdict CConflictStep3::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tConflict scenario: MOLR / X3P Timer"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0x3030);
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
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessAssistanceData))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData"));

	// Check gateway receives Network Based location
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessLocationUpdate))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate"));

	// Check gateway receives Location Request
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceSelfLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Start an X3P (Timer)
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation (Timer)"));
	TLbsNetSessionId sessionId2(TUid::Uid(KDummyUid), KX3pSessionId);
	iModule->RequestTransmitLocation(sessionId2, KX3pDestination(), KX3pPriorityTimer);

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	TInt  reason = KErrNone;
	TPositionInfo posInfo;
	iModule->RespondLocationRequest(iGatewayObserver->SessionIdValue(),
									reason, posInfo);

	// Check network receives error for its location request
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementReportLocation) ||
		KErrNone != iNetworkObserver->MeasurementFailure())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                       MeasurementReportLocation (KErrNone) -> NET"));
	
	// Generate network result
	// Note: This incorporates a network-based location too
	INFO_PRINTF1(_L("\t                              ProcessFacilityLcsMoLrResultL <- NET"));
	TPositionInfo resultLoc;
	iNetworkObserver->ProcessFacilityLcsMoLrResultL(KErrNone, &resultLoc);
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EReleaseLcsMoLr))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                             ReleaseLcsMoLr -> NET"));
	
	// Check gateway receives Network based Loc
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessLocationUpdate))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate"));

	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId1 != iGatewayObserver->SessionIdValue())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete for MOLR"));

	// Check network receives X3P request
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::ERegisterLcsMoLr) ||
		KX3pDestination() != iNetworkObserver->X3pDestination())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                               X3P-->       RegisterLcsMoLr -> NET"));

	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);

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
		MLbsNetworkProtocolObserver::EServiceTransmitThirdParty != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	INFO_PRINTF1(_L("*** Remainder of X3P is ignored for this test"));

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
TVerdict CConflictStep3::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
