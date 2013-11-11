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
 @file cconflictstep5.cpp
*/

#include <lbspositioninfo.h>
#include "cconflictstep5.h"
#include "te_testprotocolmodulesuitedefs.h"

_LIT(KX3pDestination, "x3pstep5@there.com");
const TUint KX3pSessionId = 0x505;
const TInt  KX3pPriorityPush = 0x02;
const TInt  KX3pPriorityMenu = 0x08;
const TInt  KX3pPriorityTimer = 0x10;

/** Destructor
*/
CConflictStep5::~CConflictStep5()
	{
	}


/** Constructor
*/
CConflictStep5::CConflictStep5()
	{
	SetTestStepName(KConflictStep5);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CConflictStep5::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CConflictStep5 test step.
Verify Test Protocol Module performance for a conflict situation:
First:  X3P 
Second: X3P

@return TVerdict test result code
*/
TVerdict CConflictStep5::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tConflict scenario: X3P / X3P"));
	INFO_PRINTF1(_L("\t********************************************************************"));

	INFO_PRINTF1(_L("\t--------------------"));
	INFO_PRINTF1(_L("\tX3P PUSH / X3P TIMER"));
	INFO_PRINTF1(_L("\t--------------------"));
	INFO_PRINTF1(_L("- START (a) -"));

	// Initiate X3P
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation (PUSH)"));
	TLbsAsistanceDataGroup dataRequestMask = (TUint) EAssistanceDataReferenceTime;
	TLbsNetSessionId sessionId(TUid::Uid(KDummyUid), KX3pSessionId);
	iModule->RequestTransmitLocation(sessionId, KX3pDestination(), KX3pPriorityPush);

	// Check network receives X3P request
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::ERegisterLcsMoLr) ||
		KX3pDestination() != iNetworkObserver->X3pDestination())
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
	SetDummyAssistanceData(assistData, dataRequestMask);
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
		MLbsNetworkProtocolObserver::EServiceTransmitThirdParty != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Initiate X3P Timer
	TLbsNetSessionId sessionId2(TUid::Uid(0x87654321), 0x5252);
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation (TIMER)"));
	iModule->RequestTransmitLocation(sessionId2, KX3pDestination(), KX3pPriorityTimer);

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	TInt  reason = KErrNone;
	TPositionInfo posInfo;
	iModule->RespondLocationRequest(sessionId, reason, posInfo);
					
	// Check network receives Location Response
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementReportLocation))
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
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EReleaseLcsMoLr))
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
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId != iGatewayObserver->SessionIdValue())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete for X3P PUSH"));
								
	// Check network receives X3P request
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::ERegisterLcsMoLr) ||
		KX3pDestination() != iNetworkObserver->X3pDestination())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                    X3P TIMER -->           RegisterLcsMoLr -> NET"));
	
	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);

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
		MLbsNetworkProtocolObserver::EServiceTransmitThirdParty != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	reason = KErrNone;
	iModule->RespondLocationRequest(sessionId2, reason, posInfo);
					
	// Check network receives Location Response
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementReportLocation))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                  MeasurementReportLocation -> NET"));
	
	// Generate network result
	// Note: This incorporates a network-based location too
	INFO_PRINTF1(_L("\t                              ProcessFacilityLcsMoLrResultL <- NET"));
	iNetworkObserver->ProcessFacilityLcsMoLrResultL(KErrNone, &resultLoc);
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EReleaseLcsMoLr))
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
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId2 != iGatewayObserver->SessionIdValue())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete for X3P TIMER"));
								
	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
		iNetworkObserver->IsMoreObserverActivity())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	INFO_PRINTF1(_L("- END (a) -"));

	INFO_PRINTF1(_L("\t--------------------"));
	INFO_PRINTF1(_L("\tX3P PUSH / X3P MENU"));
	INFO_PRINTF1(_L("\t--------------------"));
	INFO_PRINTF1(_L("- START (b) -"));

	// Have to reset the observer callback information
	iGatewayObserver->ResetWasObserverCalled();
	iNetworkObserver->ResetWasObserverCalled();

	// Initiate X3P
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation (PUSH)"));
	iModule->RequestTransmitLocation(sessionId, KX3pDestination(), KX3pPriorityPush);

	// Check network receives X3P request
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::ERegisterLcsMoLr) ||
		KX3pDestination() != iNetworkObserver->X3pDestination())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                            RegisterLcsMoLr -> NET"));
	
	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);

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
		MLbsNetworkProtocolObserver::EServiceTransmitThirdParty != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Initiate X3P Menu
	TLbsNetSessionId sessionId3(TUid::Uid(0x87654321), 0x5353);
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation (MENU)"));
	iModule->RequestTransmitLocation(sessionId3, KX3pDestination(), KX3pPriorityMenu);
	
	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId3 != iGatewayObserver->SessionIdValue() ||
		KErrServerBusy != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete (KErrServerBusy) for X3P MENU"));

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	reason = KErrNone;
	iModule->RespondLocationRequest(sessionId, reason, posInfo);
					
	// Check network receives Location Response
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementReportLocation))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                  MeasurementReportLocation -> NET"));
	
	// Generate network result
	// Note: This incorporates a network-based location too
	INFO_PRINTF1(_L("\t                              ProcessFacilityLcsMoLrResultL <- NET"));
	iNetworkObserver->ProcessFacilityLcsMoLrResultL(KErrNone, &resultLoc);
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EReleaseLcsMoLr))
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
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId != iGatewayObserver->SessionIdValue())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete for X3P PUSH"));
								
	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
		iNetworkObserver->IsMoreObserverActivity())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	INFO_PRINTF1(_L("- END (b) -"));

	INFO_PRINTF1(_L("\t--------------------"));
	INFO_PRINTF1(_L("\tX3P TIMER / X3P PUSH"));
	INFO_PRINTF1(_L("\t--------------------"));
	INFO_PRINTF1(_L("- START (c) -"));

	// Have to reset the observer callback information
	iGatewayObserver->ResetWasObserverCalled();
	iNetworkObserver->ResetWasObserverCalled();

	// Initiate X3P
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation (TIMER)"));
	iModule->RequestTransmitLocation(sessionId, KX3pDestination(), KX3pPriorityTimer);
	
	// Check network receives X3P request
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::ERegisterLcsMoLr) ||
		KX3pDestination() != iNetworkObserver->X3pDestination())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                            RegisterLcsMoLr -> NET"));
	
	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);

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
		MLbsNetworkProtocolObserver::EServiceTransmitThirdParty != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Initiate X3P Push
	TLbsNetSessionId sessionId4(TUid::Uid(0x87654321), 0x5454);
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation (PUSH)"));
	iModule->RequestTransmitLocation(sessionId4, KX3pDestination(), KX3pPriorityPush);
	
	// Check network receives error for its location request
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementControlFailure) ||
		KErrPositionHighPriorityReceive != iNetworkObserver->MeasurementFailure())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tMeasurementControlFailure (KErrPositionHighPriorityReceive) -> NET"));

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	reason = KErrNone;
	iModule->RespondLocationRequest(sessionId, reason, posInfo);
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EReleaseLcsMoLr) ||
		KErrPositionHighPriorityReceive != iNetworkObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t           ReleaseLcsMoLr (KErrPositionHighPriorityReceive) -> NET"));

	// Check network receives X3P request
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::ERegisterLcsMoLr) ||
		KX3pDestination() != iNetworkObserver->X3pDestination())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                            RegisterLcsMoLr -> NET"));
	
	// Generate network Measurement Control
	INFO_PRINTF1(_L("\t                          ProcessMeasurementControlLocation <- NET"));
	iNetworkObserver->ProcessMeasurementControlLocationL(refLoc, assistData, quality);

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
		MLbsNetworkProtocolObserver::EServiceTransmitThirdParty != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Generate Location Response
	INFO_PRINTF1(_L("\tLBS -> RespondLocationRequest"));
	reason = KErrNone;
	iModule->RespondLocationRequest(sessionId4, reason, posInfo);
	
	// Check network receives Location Response
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EMeasurementReportLocation))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t                                  MeasurementReportLocation -> NET"));
	
	// Generate network result
	// Note: This incorporates a network-based location too
	INFO_PRINTF1(_L("\t                              ProcessFacilityLcsMoLrResultL <- NET"));
	iNetworkObserver->ProcessFacilityLcsMoLrResultL(KErrNone, &resultLoc);
	
	// Check network session completed
	if (EFail == CheckNetworkCallbackL(CNetworkObserver::EReleaseLcsMoLr))
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
	if (EFail == CheckGatewayCallbackL(CGatewayObserver::EProcessSessionComplete) ||
		sessionId4 != iGatewayObserver->SessionIdValue())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete for X3P PUSH"));
								
	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
		iNetworkObserver->IsMoreObserverActivity())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	INFO_PRINTF1(_L("- END (c) -"));

	SetTestStepResult(EPass);
	return TestStepResult();
	}


/** Perform post test actions
@return TVerdict test result code
*/
TVerdict CConflictStep5::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
