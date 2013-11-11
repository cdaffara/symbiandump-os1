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
 @file cmolrstep12.cpp
*/

#include <lbspositioninfo.h>
#include "cmolrstep12.h"
#include "te_testprotocolmodulesuitedefs.h"


/** Destructor
*/
CMoLrStep12::~CMoLrStep12()
	{
	}


/** Constructor
*/
CMoLrStep12::CMoLrStep12()
	{
	SetTestStepName(KMoLrStep12);
	}


/** Perform pre test actions
@return TVerdict test result code
*/
TVerdict CMoLrStep12::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform CMoLrStep12 test step.
This test verifies that the Test Protocol Module correctly handles 
network error at various points in MO-LR Self Locate sequence.

@return TVerdict test result code
*/
TVerdict CMoLrStep12::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR with network error at various points"));
	INFO_PRINTF1(_L("\t********************************************************************"));


	// *************************************************************************
	// Network error set prior to commencing MOLR. The first request should
	// fail due to network being unavailable.
	// *************************************************************************
	INFO_PRINTF1(_L("\t-------------------------------------------------------"));
	INFO_PRINTF1(_L("\tNetwork failure prior to MO-LR commencing"));
	INFO_PRINTF1(_L("\t-------------------------------------------------------"));
	INFO_PRINTF1(_L("- START (a) -"));

	// Set network error state
	INFO_PRINTF1(_L("*** NETWORK ERROR STATE SET ***"));
	iNetworkObserver->SetNetworkAlive(EFalse);

	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654321), 0xcccc);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataReferenceTime;
	options1.SetDataRequestMask(dataRequestMask1);
	INFO_PRINTF1(_L("\tLBS -> RequestSelfLocation"));	
	iModule->RequestSelfLocation(sessionId1, options1);

	// Check gateway receives Assistance data error
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessAssistanceData) ||
		KErrDisconnected != iGatewayObserver->AssistanceDataError()
		)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData (KErrDisconnected)"));

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

	INFO_PRINTF1(_L("- END (a) -"));

	INFO_PRINTF1(_L("\t-------------------------------------------------------"));
	INFO_PRINTF1(_L("\tNetwork failure at final stage of MO-LR"));
	INFO_PRINTF1(_L("\t-------------------------------------------------------"));
	INFO_PRINTF1(_L("- START (b) -"));

	// Have to reset the observer callback information
	iGatewayObserver->ResetWasObserverCalled();
	iNetworkObserver->ResetWasObserverCalled();

	// Set network error state
	INFO_PRINTF1(_L("*** NETWORK OK STATE SET ***"));
	iNetworkObserver->SetNetworkAlive(ETrue);

	// Initiate MO-LR
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

	// Set network error state
	INFO_PRINTF1(_L("*** NETWORK ERROR STATE SET ***"));
	iNetworkObserver->SetNetworkAlive(EFalse);

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
				CGatewayObserver::EProcessSessionComplete) ||
		KErrDisconnected != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete (KErrDisconnected)"));

	// Check if more observer activity takes place
	if (iGatewayObserver->IsMoreObserverActivity() ||
		iNetworkObserver->IsMoreObserverActivity())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	INFO_PRINTF1(_L("- END (b) -"));

	INFO_PRINTF1(_L("\t-------------------------------------------------------"));
	INFO_PRINTF1(_L("\tNetwork failure when more assistance data requested"));
	INFO_PRINTF1(_L("\t-------------------------------------------------------"));
	INFO_PRINTF1(_L("- START (c) -"));

	// Have to reset the observer callback information
	iGatewayObserver->ResetWasObserverCalled();
	iNetworkObserver->ResetWasObserverCalled();

	// Set network error state
	INFO_PRINTF1(_L("*** NETWORK OK STATE SET ***"));
	iNetworkObserver->SetNetworkAlive(ETrue);

	// Initiate MO-LR
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
				CGatewayObserver::EProcessLocationRequest))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest"));

	// Set network error state
	INFO_PRINTF1(_L("*** NETWORK ERROR STATE SET ***"));
	iNetworkObserver->SetNetworkAlive(EFalse);

	// Initiate MO-LR
	INFO_PRINTF1(_L("\tLBS -> RequestAssistanceData (MORE)"));	
	iModule->RequestAssistanceData(EAssistanceDataBadSatList);

	// Check gateway receives Assistance data error
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessAssistanceData) ||
		KErrDisconnected != iGatewayObserver->AssistanceDataError()
		)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData (KErrDisconnected)"));

	// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CGatewayObserver::EProcessSessionComplete) ||
		KErrDisconnected != iGatewayObserver->SessionCloseReason())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete (KErrDisconnected)"));

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
TVerdict CMoLrStep12::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_testprotocolmoduleSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
