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
 @file molr29Step.cpp
 @internalTechnology
*/
#include "molr29Step.h"
#include "te_suplprotocolsuitedefs.h"

/**
 Destructor
 */
Cmolr29Step::~Cmolr29Step()

	{
	}

/**
 Constructor
 */
Cmolr29Step::Cmolr29Step()
	{
	SetTestStepName(Kmolr29Step);
	}

/**
 @return - TVerdict code
 Override of base class virtual
 */
TVerdict Cmolr29Step::doTestStepPreambleL()
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
TVerdict Cmolr29Step::doTestStepL()
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

	// Inject a SUPL END (with error)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL END <- NET"));
	CSuplMessageBase* suplEnd = BuildSuplEndL(EFalse, KErrCancel);
	iNetworkObserver->InjectSuplMessage(iNetworkObserver->SessionId(), suplEnd);

	// Check gateway session completes AND a notification of assistance data
	// error has been received for the 
	if (EFail == CheckGatewayCallbackL(CSuplGatewayObserver::EProcessSessionComplete))
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


	// ****************** Check that an assistance data error report has been sent to LBS
	// Note: this happened just after EProcessSessionComplete and before EDisconnectReq above,
	// but since it happens in the same RunL loop as EProcessSessionComplete, it can't be detected
	// as an independent callback with CheckGatewayCallbackL. Therefore, all we can do is check that
	// assistance data error was reported at some point.
	if ((KErrNotFound != iGatewayObserver->AssistanceDataError()) ||
		(EAssistanceDataBadSatList & iGatewayObserver->AssistanceDataErrorMask() != EAssistanceDataBadSatList) 
		)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("\tLBS <- ProcessAssistanceData(KErrNotFound)"));
	
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
TVerdict Cmolr29Step::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
