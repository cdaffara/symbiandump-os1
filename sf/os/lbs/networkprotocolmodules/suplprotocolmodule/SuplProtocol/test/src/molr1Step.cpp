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
 @file molr1Step.cpp
 @internalTechnology
*/
#include "molr1Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 Destructor
 */
Cmolr1Step::~Cmolr1Step()

	{
	}

/**
 Constructor
 */
Cmolr1Step::Cmolr1Step()
	{
	SetTestStepName(Kmolr1Step);
	}

/**
 @return - TVerdict code
 Override of base class virtual
 */
TVerdict Cmolr1Step::doTestStepPreambleL()
	{
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();
	 
	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
	
	
	//Clear the repository that keeps the host settings
	//CRepository* rep = CRepository::NewL(KLbsHostSettingsSuplStoreId);
	//CleanupStack::PushL(rep);
	//User::LeaveIfError(rep->Reset());
	//CleanupStack::PopAndDestroy(rep);

	//CLbsHostSettingsStore* slpSettingsStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	//CleanupStack::PushL(slpSettingsStore);
	
	// Create a "user-provisioned" entry which is the default host.
	// This will be the host selected by this testcase and most testcases
	// following, so it is intentionally not erased in the postamble.
	//INFO_PRINTF1(_L("[User-provisioned default host created]"));
	//TLbsHostSettingsSupl defaultSettings;
	//TLbsHostNameAddress defHostAddr((TText8*)"supl.thirdparty.com");
	//TLbsHostName defHostName((TText8*)"defSuplServer");
	//TLbsProviderId defHostProvider ((TText8*)"provider");
	//TLbsHostSettingsId defSettingsId; 
	//defaultSettings.SetHostNameAddress(defHostAddr);
	//defaultSettings.SetName(defHostName);
	//defaultSettings.SetProviderId(defHostProvider);
	//defaultSettings.SetPortId(KLbsSuplUseDefaultPortId);
	//TLbsHostCreatorId KTestMadeUpUid = {0x101010};
	//defaultSettings.SetConnectionPoint(1,TLbsHostSettingsSupl::ELbsConnectionTypeNap);
	//defaultSettings.SetAuthModesMOLR(TLbsHostSettingsSupl::EAuthTls);
	//defaultSettings.SetAuthModesMTLR(TLbsHostSettingsSupl::EAuthTls);
	
//	if (KErrNone != slpSettingsStore->CreateHostSettings(defaultSettings, KTestMadeUpUid, defSettingsId))
//		{
//		SetTestStepResult(EFail);
//		return TestStepResult();					
//		}
		
//	if (KErrNone != slpSettingsStore->SetDefaultHostSettings(defSettingsId))
//		{
//		SetTestStepResult(EFail);
//		return TestStepResult();					
//		}
		
//	CleanupStack::PopAndDestroy(slpSettingsStore);
	
	 
	return TestStepResult();
	}


/** Perform CMoLrStep1 test step.
This test verifies that the SUPL Protocol Module correctly handles 
an MO-LR Self Locate Terminal Based sequence (assistance data
delivered via an RRLP payload).

@return TVerdict test result code
*/
TVerdict Cmolr1Step::doTestStepL()
	{
	
 	
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR basic procedure followed - Assistance Data in RRLP message"));
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

	// Inject a SUPL POS with some of the Assistance data requested
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Assitance Data <- NET"));
	CSuplMessageBase* suplPos = BuildSuplPosAssitDataL(EAssistanceDataAquisitionAssistance|EAssistanceDataBadSatList, EFalse);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplPos);	



	// Check gateway receives the assistance data types requested.
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData()"));
	if (EFail == CheckGatewayCallbackL(
				CSuplGatewayObserver::EProcessAssistanceData) ||
		(dataRequestMask1 & iGatewayObserver->AssistanceDataSetMask() != dataRequestMask1))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// Check the Connection Manager receives a request to send a SUPL POS (ack to assistance data)
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESendSuplPosSendReq))
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS - RRLP Assistance Data Ack -> NET"));

	
	// Inject a SUPL END (without position)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL END <- NET"));
	CSuplMessageBase* suplEnd = BuildSuplEndL(EFalse);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplEnd);
	
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
TVerdict Cmolr1Step::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
