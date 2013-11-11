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
 @file molr26Step.cpp
 @internalTechnology
*/
#include "molr26Step.h"
#include "te_suplprotocolsuitedefs.h"
#include <lbs/lbshostsettings.h>
#include <centralrepository.h>

Cmolr26Step::~Cmolr26Step()
	{
	}

Cmolr26Step::Cmolr26Step()
	{
	SetTestStepName(Kmolr26Step);
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr26Step::doTestStepPreambleL()

	{
	 
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();
 
	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));
 
	return TestStepResult();
	}

/** Perform CMoLrStep1 test step.
This test verifies that the SUPL Protocol Module correctly handles 
an Cell-Based MO-LR Self Locate sequence .

@return TVerdict test result code
*/
TVerdict Cmolr26Step::doTestStepL()
	{
	
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR host selection - preferred host selected"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

 	//Clear the repository that keeps the host settings
	CRepository* rep = CRepository::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(rep);
	User::LeaveIfError(rep->Reset());
	CleanupStack::PopAndDestroy(rep);

	CLbsHostSettingsStore* slpSettingsStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(slpSettingsStore);
	
	// Create a "network-provisioned" entry which is not the default host
	INFO_PRINTF1(_L("[Network-provisioned host created]"));
	TLbsHostSettingsSupl networkSettings;
	TLbsHostNameAddress netHostAddr((TText8*)"supl.provider.com");
	TLbsHostName netHostName((TText8*)"netSuplServer");
	TLbsProviderId netHostProvider ((TText8*)"provider");
	TLbsHostSettingsId netSettingsId;
	networkSettings.SetHostNameAddress(netHostAddr);
	networkSettings.SetName(netHostName);
	networkSettings.SetProviderId(netHostProvider);
	networkSettings.SetPortId(KLbsSuplUseDefaultPortId);
	networkSettings.SetConnectionPoint(1,TLbsHostSettingsSupl::ELbsConnectionTypeNap);
	networkSettings.SetAuthModesMOLR(TLbsHostSettingsSupl::EAuthTls);
	networkSettings.SetAuthModesMTLR(TLbsHostSettingsSupl::EAuthTls);
	
	if (KErrNone != slpSettingsStore->CreateHostSettings(networkSettings, KLbsHostSettingsDevProvCreatorId, netSettingsId))
		{
		SetTestStepResult(EFail);
		return TestStepResult();	
		}
	
	// Create a "user-provisioned" entry which is the default host
	INFO_PRINTF1(_L("[User-provisioned default host created]"));
	TLbsHostSettingsSupl defaultSettings;
	TLbsHostNameAddress defHostAddr((TText8*)"supl.thirdparty.com");
	TLbsHostName defHostName((TText8*)"defSuplServer");
	TLbsProviderId defHostProvider ((TText8*)"provider");
	TLbsHostSettingsId defSettingsId; 
	defaultSettings.SetHostNameAddress(defHostAddr);
	defaultSettings.SetName(defHostName);
	defaultSettings.SetProviderId(defHostProvider);
	defaultSettings.SetPortId(KLbsSuplUseDefaultPortId);
	TLbsHostCreatorId KTestMadeUpUid = {0x101010};
	defaultSettings.SetConnectionPoint(1,TLbsHostSettingsSupl::ELbsConnectionTypeNap);
	defaultSettings.SetAuthModesMOLR(TLbsHostSettingsSupl::EAuthTls);
	defaultSettings.SetAuthModesMTLR(TLbsHostSettingsSupl::EAuthTls);

	if (KErrNone != slpSettingsStore->CreateHostSettings(defaultSettings, KTestMadeUpUid, defSettingsId))
		{
		SetTestStepResult(EFail);
		return TestStepResult();					
		}
		
	if (KErrNone != slpSettingsStore->SetDefaultHostSettings(defSettingsId))
		{
		SetTestStepResult(EFail);
		return TestStepResult();					
		}
	
	
	// Initiate MO-LR
	INFO_PRINTF1(_L("\tLBS -> RequestNetworkLocation"));
	TLbsNetSessionId sessionId1(TUid::Uid(0x12345678), 0x1111);
	TLbsNetPosRequestOptions options1;
	options1.SetNewClientConnected(ETrue);
	TLbsNetPosRequestQuality quality1;
	options1.SetRequestQuality(quality1);
	iModule->RequestNetworkLocation(sessionId1, options1);

    // Check Connection Manager receives a request for connecting to the 
    // host whose settings Id is defSettingsId
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::EConnectReq) ||
		defSettingsId != iNetworkObserver->HostSettingsId())
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

	// Check gateway receives a session complete indication
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
	
	// Delete the entries in the SUPL HostSettings store created by this test
	slpSettingsStore->DeleteHostSettings(netSettingsId);
	slpSettingsStore->DeleteHostSettings(defSettingsId);
	
	CleanupStack::PopAndDestroy(slpSettingsStore);
	
	INFO_PRINTF1(_L("- END -"));
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr26Step::doTestStepPostambleL()
	{
	delete iModule;

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
