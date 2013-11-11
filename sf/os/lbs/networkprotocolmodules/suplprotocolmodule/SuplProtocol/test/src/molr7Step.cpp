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
 @file molr7Step.cpp
 @internalTechnology
*/
#include "molr7Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 * Destructor
 */
Cmolr7Step::~Cmolr7Step()

	{
	}
	
/**
 * Constructor
 */
Cmolr7Step::Cmolr7Step()

	{
	SetTestStepName(Kmolr7Step);
	}
	
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr7Step::doTestStepPreambleL()

	{
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();

	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	return TestStepResult();
	}

/** Perform Cmolr7Step test step.
This test verifies that the SUPL Protocol Module correctly handles 
MO-LR Self Locate cancellation.
Cancellation occurs after a SUPL END with
a position (SUPL END is not ignored as it makes it to the 
state machine ahead of the cancel, instead, the cancel is
ignored).

@return TVerdict test result code
*/
TVerdict Cmolr7Step::doTestStepL()
	{
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR cancelled by LBS after SUPL END is received"));
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
	
	// Inject a SUPL RESPONSE
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL RESPONSE <- NET"));
	CSuplMessageBase* resp = BuildSuplResponseL(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), resp);
	
	// Check gateway receives Location Request
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceSelfLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
		
	// LBS Requests assistance data
	INFO_PRINTF1(_L("\tLBS -> RequestAssistanceData ()"));
	TLbsNetSessionIdArray dummyIdArray;		
	iModule->RequestAssistanceData(dataRequestMask1, dummyIdArray);
	
	// Check Connection Manager receives a request to send a SUPL POS INIT
	// with the assistance data mask requested by the gateway
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosInitSendReq))
	{
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS INIT -> NET"));
	// Inject a SUPL END (with position)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL END <- NET"));
	CSuplMessageBase* suplEnd = BuildSuplEndL(ETrue);
	
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplEnd);

	// Check gateway receives Location Update
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationUpdate()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessLocationUpdate))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// LBS Cancels MO-LR (has no effect)
	INFO_PRINTF1(_L("\tLBS -> CancelSelfLocation"));	
	iModule->CancelSelfLocation(sessionId1, KErrCancel);

	// Check gateway receives a session complete indication
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessSessionComplete))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessSessionComplete"));
	
	// LBS Cancels MO-LR (has no effect)
	INFO_PRINTF1(_L("\tLBS -> CancelSelfLocation"));	
	iModule->CancelSelfLocation(sessionId1, KErrCancel);

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



TVerdict Cmolr7Step::doTestStepPostambleL()
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
