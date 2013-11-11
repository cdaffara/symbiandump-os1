// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file molr9Step.cpp
 @internalTechnology
*/
#include "molr9Step.h"
#include "te_suplprotocolsuitedefs.h"

_LIT(KX3pDestination, "x3pstep1@there.com");
const TUint KX3pSessionId = 0x101;
const TInt  KX3pPriority = 0x01;
const TInt  KADummyUid = 0x01;

/**
 * Destructor
 */
 Cmolr9Step::~Cmolr9Step()
	{
	}

/**
 * Constructor
 */
Cmolr9Step::Cmolr9Step()
	{
	SetTestStepName(Kmolr9Step);
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr9Step::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();
	 
	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	 
	return TestStepResult();
	}

/** Perform CMoLrStep9 test step.
This test verifies that the SUPL Protocol Module completes a 
self-locate session when the positioning method is Autonomous
and also simultaneous requests to start and cancel a Transmit
to Third party procedure (unsupported by SUPL).
The test tests the ability of the Protocol to store the 
session Ids and send the corresponding completion indications.

@return TVerdict test result code
*/
TVerdict Cmolr9Step::doTestStepL()

	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tAutonomous self-locate request session and transmit to third party"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));
	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x97654321), 0x1111);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	// Make the request an autonomous one
	options1.SetPosMode(TPositionModuleInfo::ETechnologyTerminal);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataBadSatList;
	options1.SetDataRequestMask(dataRequestMask1);
	INFO_PRINTF1(_L("\tLBS -> RequestSelfLocation"));
	iModule->RequestSelfLocation(sessionId1, options1);

	// Attempt to initiate TransmitLocation
	INFO_PRINTF1(_L("\tLBS -> TransmitLocation"));
	TLbsNetSessionId sessionId(TUid::Uid(KADummyUid), KX3pSessionId);
	TLbsNetPosRequestOptions dummyOptions;
	iModule->RequestTransmitLocation(sessionId, KX3pDestination(), KX3pPriority, dummyOptions);

	// Attempt to cancel a TransmilLocation session
	INFO_PRINTF1(_L("\tLBS -> CancelTransmitLocation (REASON)"));	
	iModule->CancelTransmitLocation(iGatewayObserver->SessionIdValue(), KErrGeneral);

	
	// Check that Autonomous MOLR has completed
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessSessionComplete))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));

	// Check that TransmitLocatin has completed
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessSessionComplete))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));

	// Check that TransmitLocatin cancellation is replied to
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessSessionComplete))
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


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr9Step::doTestStepPostambleL()

	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
