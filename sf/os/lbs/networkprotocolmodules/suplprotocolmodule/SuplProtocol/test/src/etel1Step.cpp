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
//

/**
 @file etel1Step.cpp
 @internalTechnology
*/

#include <simtsy.h> 
#include "lbsdevloggermacros.h"

#include "etel1Step.h"
#include "lbssystemcontroller.h"


const TInt KInitializationTimeout = 1000000; //1 sec

CEtel1Step::CEtel1Step()
	{
	SetTestStepName(KEtel1Step);
	}

CEtel1Step::~CEtel1Step()
	{
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CEtel1Step::doTestStepPreambleL()
	{
	LBSLOG(ELogP1,"CEtel1Step::doTestStepPreambleL - Start");
	
	RLbsSystemController lbsSysController;
	lbsSysController.OpenL(KLbsSuplTestUid);
	//Start the LBS sub-system if it is not running
	if(lbsSysController.RequestSystemStartup()!= ELbsStartupSuccess)
		{
		lbsSysController.Close();
		User::Leave(KErrNotReady); 
		}
	
	//We do not need Lbs to be running - not to interfere with our tests
	User::LeaveIfError(lbsSysController.RequestCompleteCloseDown());	
	
	lbsSysController.Close();
	
	// Call base class method for pre test actions	
	CTe_suplprotocolSuiteStepBase::doTestStepPreambleL();
	 
	TLbsNetProtocolModuleParams param(*iGatewayObserver);
	iModule = CSuplGatewayInterface::NewL(reinterpret_cast<TAny*>(&param));	
	
	iSchedulerWait = new(ELeave) CActiveSchedulerWait;
	iTimer = CLbsCallbackTimer::NewL(*this);
	
	//Start active scheduler for a while to complete the async initialization calls
	iTimer->EventAfter(TTimeIntervalMicroSeconds32(KInitializationTimeout), 0);
	iSchedulerWait->Start();
	 
	LBSLOG(ELogP1,"CEtel1Step::doTestStepPreambleL - End");
	return TestStepResult();
	}


/** Perform etel0Step test step.
This test verifies that the SUPL Protocol Module correctly handles the etel network information such as 
Mobile Country Code, Mobile Network Code, cell id, timing advance, IMSI.
@return TVerdict test result code
*/
TVerdict CEtel1Step::doTestStepL()
	{
	LBSLOG(ELogP1,"CEtel1Step::doTestStepL - Start");
	
	TLbsHostSettingsId settingsId;
	TLbsHostSettingsSupl settings;
	CLbsHostSettingsStore* hostStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(hostStore);
	
	User::LeaveIfError(hostStore->RewindHostSettings());
	
	while(hostStore->GetNextHostSettings(settings, settingsId)==KErrNone)
		{
		User::LeaveIfError(hostStore->DeleteHostSettings(settingsId));
		}	
	CleanupStack::PopAndDestroy();
		
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tTest of ETEL params on MOLR"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));	
		
	// Make the request a tracking one
	INFO_PRINTF1(_L("\tLBS -> AdviceSystemStatus (Tracking ON)"));
	iModule->AdviceSystemStatus(CLbsNetworkProtocolBase::ESystemStatusClientTracking);
			
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
	
	settingsId = iNetworkObserver->HostSettingsId();
	hostStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(hostStore);
		
	// Get the host settings
	User::LeaveIfError(hostStore->GetHostSettings(settingsId, settings));
	CleanupStack::PopAndDestroy();
	
	TLbsHostNameAddress hostAddr;
	settings.GetHostNameAddress(hostAddr);
	
	_LIT8(KHslpAddresFormat, "h-slp.mnc%03d.mcc%03d.pub.3gppnetwork.org");
	TLbsHostNameAddress expectedAddr;
	
	TInt mcc = 0;
	GetIntFromConfig(ConfigSection(), _L("MCC"), mcc);
			
	TInt mnc = 0;
	GetIntFromConfig(ConfigSection(), _L("MNC"), mnc);
	
	expectedAddr.Format(KHslpAddresFormat, mnc, mcc);
	
	if(hostAddr!=expectedAddr)
		{
		// give a warning of what went wrong
		TBuf<64> expected16;
		expected16.Copy(expectedAddr);
		TBuf16<64> actual16;
		actual16.Copy(hostAddr);
		INFO_PRINTF3(_L("address expected %S, found was %S\n"), &expected16, &actual16);

		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// Simulate the connection is up (inject that event)
	INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Connected <- NET"));
	iNetworkObserver->InjectConnectedIndication(iNetworkObserver->SessionId());
			
	// Check Connection Manager receives a request to send a SUPL START
	if(EFail == CheckNetworkCallbackL(CSuplNetworkTestObserver::ESuplStartSendReq))
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
	User::LeaveIfNull(iNetworkObserver);

	// Wait for network observer callback
	iNetworkObserver->WaitForCallBack();
				
	// Check that callback matches the expected type
	if((CSuplNetworkTestObserver::ESendSuplPosInitSendReq != iNetworkObserver->CallBackResult()) ||
		!iNetworkObserver->WasObserverCalled())
		{
		SetTestStepResult(EFail);
		}
			
	ResetNetworkObserverL();

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

	INFO_PRINTF1(_L("\tLBS -> AdviceSystemStatus (Tracking OFF)"));
	iModule->AdviceSystemStatus(CLbsNetworkProtocolBase::ESystemStatusNone);

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
			
	LBSLOG(ELogP1,"CEtel0Step::doTestStepL - End");
	return TestStepResult();
	}

TVerdict CEtel1Step::doTestStepPostambleL()
	{
	LBSLOG(ELogP1,"CEtel1Step::doTestStepPostambleL - Start");
	
	
	//Clear the repository that keeps the host settings
	CRepository* rep = CRepository::NewL(KLbsHostSettingsSuplStoreId);
	rep->Reset();
	delete rep;
	
	delete iModule;
	iModule=0;	
	
	delete iTimer;
	iTimer=0;
		
	delete iSchedulerWait;
	iSchedulerWait = 0;	
	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	LBSLOG(ELogP1,"CEtel1Step::doTestStepPostambleL - End");
	return TestStepResult();
	}

void CEtel1Step::OnTimerEventL(TInt /*aTimerId*/)
	{
	iSchedulerWait->AsyncStop();
	}

TInt CEtel1Step::OnTimerError(TInt /*aTimerId*/, TInt /*aError*/)
	{
	iSchedulerWait->AsyncStop();
	return KErrNone;
	}
	

