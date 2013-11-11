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
 @file molr27Step.cpp
 @internalTechnology
*/
#include "molr27Step.h"
#include "te_suplprotocolsuitedefs.h"
#include <lbs/lbshostsettings.h>
#include <centralrepository.h>


Cmolr27Step::~Cmolr27Step()
	{
	}

Cmolr27Step::Cmolr27Step()
	{
	SetTestStepName(Kmolr27Step);
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr27Step::doTestStepPreambleL()

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
TVerdict Cmolr27Step::doTestStepL()
	{
	
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tMOLR host selection - last modified network-provisioned host selected"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));

 	//Clear the repository that keeps the host settings
	CRepository* rep = CRepository::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(rep);
	User::LeaveIfError(rep->Reset());
	CleanupStack::PopAndDestroy(rep);

	CLbsHostSettingsStore* slpSettingsStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(slpSettingsStore);
	
	
	// Delete the Default host if present
	TLbsHostSettingsSupl defSettings;
	TLbsHostSettingsId 	defSettingsId;
	if (KErrNone == slpSettingsStore->GetDefaultHostSettings(defSettings, defSettingsId))
		{
		slpSettingsStore->DeleteHostSettings(defSettingsId);
		}
	
	
	// Create a "network-provisioned" entry which is not the default host
	INFO_PRINTF1(_L("[First network-provisioned host created]"));
	TLbsHostSettingsSupl networkSettings;
	TLbsHostNameAddress netHostAddr((TText8*)"supl1.provider.com");
	TLbsHostName netHostName((TText8*)"netSuplServer1");
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
	
	// Create a "user-provisioned" entry which is NOT set as default host
	INFO_PRINTF1(_L("[User-provisioned host created]"));
	TLbsHostSettingsSupl userSettings;
	TLbsHostNameAddress userHostAddr((TText8*)"supl.thirdparty.com");
	TLbsHostName userHostName((TText8*)"userSuplServer");
	TLbsProviderId userHostProvider ((TText8*)"provider");
	TLbsHostSettingsId userSettingsId; 
	userSettings.SetHostNameAddress(userHostAddr);
	userSettings.SetName(userHostName);
	userSettings.SetProviderId(userHostProvider);
	userSettings.SetPortId(KLbsSuplUseDefaultPortId);
	TLbsHostCreatorId KTestMadeUpUid = {0x101010};
	userSettings.SetConnectionPoint(1,TLbsHostSettingsSupl::ELbsConnectionTypeNap);
	userSettings.SetAuthModesMOLR(TLbsHostSettingsSupl::EAuthTls);
	userSettings.SetAuthModesMTLR(TLbsHostSettingsSupl::EAuthTls);
	
	if (KErrNone != slpSettingsStore->CreateHostSettings(userSettings, KTestMadeUpUid, userSettingsId))
		{
		SetTestStepResult(EFail);
		return TestStepResult();					
		}
	
	// Create another "network-provisioned" entry which is not the default host entry.
	// Since this is the "last modified" network entry, and no other entry has
	// been set as "default", this is the host entry that the MOLR state machine 
	// should select for connecting to.
	INFO_PRINTF1(_L("[Second network-provisioned host created]"));
	TLbsHostSettingsSupl networkSettings2;
	TLbsHostNameAddress netHostAddr2((TText8*)"supl2.provider.com");
	TLbsHostName netHostName2((TText8*)"netSuplServer2");
	TLbsProviderId netHostProvider2((TText8*)"provider");
	TLbsHostSettingsId netSettingsId2;
	networkSettings2.SetHostNameAddress(netHostAddr2);
	networkSettings2.SetName(netHostName2);
	networkSettings2.SetProviderId(netHostProvider2);
	networkSettings2.SetPortId(KLbsSuplUseDefaultPortId);
	networkSettings2.SetConnectionPoint(1,TLbsHostSettingsSupl::ELbsConnectionTypeNap);
	networkSettings2.SetAuthModesMOLR(TLbsHostSettingsSupl::EAuthTls);
	networkSettings2.SetAuthModesMTLR(TLbsHostSettingsSupl::EAuthTls);
	
	
	if (KErrNone != slpSettingsStore->CreateHostSettings(networkSettings2, KLbsHostSettingsDevProvCreatorId, netSettingsId2))
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
    // host whose settings Id is netSettingsId2
	if (EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::EConnectReq) ||
		netSettingsId2 != iNetworkObserver->HostSettingsId())
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
	slpSettingsStore->DeleteHostSettings(userSettingsId);
	slpSettingsStore->DeleteHostSettings(netSettingsId2);
	
	CleanupStack::PopAndDestroy(slpSettingsStore);
	
	INFO_PRINTF1(_L("- END -"));
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict Cmolr27Step::doTestStepPostambleL()
	{
	delete iModule;

 	// Reset the repository that keeps the host settings
 	// to its initial state
	CRepository* rep = CRepository::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(rep);
	User::LeaveIfError(rep->Reset());
	CleanupStack::PopAndDestroy(rep);

	// Call base class method for post test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
