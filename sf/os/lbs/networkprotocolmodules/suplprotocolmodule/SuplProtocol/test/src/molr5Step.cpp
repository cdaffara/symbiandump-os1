// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file molr5Step.cpp
 @internalTechnology
*/
#include "molr5Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 * Destructor
 */
Cmolr5Step::~Cmolr5Step()

	{
	}
	
/**
 * Constructor
 */
Cmolr5Step::Cmolr5Step()

	{
	SetTestStepName(Kmolr5Step);
	}
	
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr5Step::doTestStepPreambleL()

	{
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}


/** Perform Cmolr5Step test step.
This test verifies that the SUPL Protocol Module correctly handles 
MO-LR Self Locate cancellation.
Cancellation occurs just before receiving a SUPL RESPONSE.
(SUPL RESPONSE should be ignored).

@return TVerdict test result code
*/
TVerdict Cmolr5Step::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR cancelled by LBS before SUPL RESPONSE is received"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));
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

	// Check Connection Manager receives a request for connecting
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::EConnectReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t ConnectionRequest -> NET"));

	// Simulate the connection is up (inject that event)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Connected <- NET"));
    iNetworkObserver->InjectConnectedIndication(iNetworkObserver->SessionId());
	
	// Check Connection Manager receives a request to send a SUPL START
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESuplStartSendReq))
	{
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL START -> NET"));
	
	// LBS Cancels MO-LR
	INFO_PRINTF1(_L("\tLBS -> CancelSelfLocation"));	
	iModule->CancelSelfLocation(sessionId1, KErrCancel);	

	// Check the Connection Manager receives a request to send a SUPL END 
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplEndSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL END -> NET"));

	// Inject a SUPL RESPONSE
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL RESPONSE <- NET"));
	CSuplMessageBase* resp = BuildSuplResponseL(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), resp);

		// Check gateway session completed
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessSessionComplete))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));

	// Check Connection Manager receives a disconnection request
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::EDisconnectReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t DisconnectRequest -> NET"));
	
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



TVerdict Cmolr5Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

