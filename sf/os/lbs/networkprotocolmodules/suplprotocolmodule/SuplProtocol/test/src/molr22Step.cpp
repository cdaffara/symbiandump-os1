// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file molr22Step.cpp
 @internalTechnology
*/
#include "molr22Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 Destructor
 */
Cmolr22Step::~Cmolr22Step()

	{
	}

/**
 Constructor
 */
Cmolr22Step::Cmolr22Step()
	{
	SetTestStepName(Kmolr22Step);
	}

/**
 @return - TVerdict code
 Override of base class virtual
 */
TVerdict Cmolr22Step::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();
	 
	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	 
	return TestStepResult();
	}


/** Perform CMoLrStep1 test step.
This test verifies that the SUPL Protocol Module correctly handles 
an error notification from the RRLP state machine and the SUPL
session is terminated with SUPL END.

The RRLP error notification is induced by not injecting a Location
Response from LBS before the MaxFixTime expires.

@return TVerdict test result code
*/
TVerdict Cmolr22Step::doTestStepL()
	{
 	
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR session terminated after RRLP error (LBS results timer expiry)"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));
	// Initiate MO-LR
	TLbsNetSessionId sessionId1(TUid::Uid(0x87654322), 0x1111);
	TLbsNetPosRequestOptionsAssistance options1;
	options1.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	TLbsAsistanceDataGroup dataRequestMask1 = EAssistanceDataBadSatList;
	TLbsAsistanceDataGroup dataRequestMask2 = EAssistanceDataReferenceLocation;
	// Request both sets of assitance data. They'll be delivered in different
	// SUPL POS messages later on in the test.
	TLbsAsistanceDataGroup dataRequestMask = dataRequestMask1|dataRequestMask2;
	options1.SetDataRequestMask(dataRequestMask); 
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
	iModule->RequestAssistanceData(dataRequestMask, dummyIdArray);
	
	// Check Connection Manager receives a request to send a SUPL POS INIT
	// with the assistance data mask requested by the gateway
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosInitSendReq))
	{
	SetTestStepResult(EFail);
	return TestStepResult();
	}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS INIT -> NET"));

	// Inject a SUPL POS (RRLP ASSIST DATA) with some of the Assistance data requested
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Assitance Data <- NET"));
	CSuplMessageBase* suplPos = BuildSuplPosAssitDataL(dataRequestMask2, ETrue); // ETrue = more assistance data underway
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplPos);	

	// Check the Connection Manager receives a request to send a SUPL POS (ack to assistance data)
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Assistance Data Ack -> NET"));

	// Inject a SUPL POS (RRLP MEASURE POSITION REQUEST) wich carries
	// assistance data
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Measure Position Request <- NET"));
	suplPos = BuildSuplPosRequestL(TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted, dataRequestMask1);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplPos);	

	// Check gateway receives a reference location due to having requested
	// and received assistance data type Reference Location
	// TBD !!!
	// Check gateway receives the assistance data types contained in both SUPL POS messages
	// received earlier.
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessAssistanceData) ||
		(dataRequestMask & iGatewayObserver->AssistanceDataSetMask() != dataRequestMask))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// Check gateway receives a location request (due to the RRLP Measure Position Request)
	//
	INFO_PRINTF1(_L("\tLBS <- ProcessLocationRequest()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessLocationRequest) ||
		MLbsNetworkProtocolObserver::EServiceSelfLocation != iGatewayObserver->LocType())
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// ****************************************************************************************
	// Do not fake LBS sending results (position) to SPM.
	// By omitting a call to iModule->RespondLocationRequest(sessionId1,KErrNone,mobilePosInfo)
	// the RRLP state machine will time out waiting for LBS results after aprox 3 seconds 
	// (max fix time set in BuildSuplPosRequestL()). The time out should result in an error
	// notification to SUPL Protocol state machine, which should cancel the SUPL session.
	// ****************************************************************************************

	// Check the Connection Manager receives a request to send a SUPL END 
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplEndSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL END -> NET"));
		
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


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr22Step::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
