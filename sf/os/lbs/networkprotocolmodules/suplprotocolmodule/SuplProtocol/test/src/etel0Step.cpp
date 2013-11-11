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
 @file etel0Step.cpp
 @internalTechnology
*/

#include <simtsy.h> 
#include "lbsdevloggermacros.h"

#include "etel0Step.h"

#include "te_suplprotocolsuitedefs.h"
#include "suplposinit.h"
#include "lbssystemcontroller.h"

const TInt KInitializationTimeout = 2000000; //2sec
const TInt KPropogationTimeout = 5000000; //5sec

CEtel0Step::CEtel0Step()
	{
	SetTestStepName(KEtel0Step);
	}

CEtel0Step::~CEtel0Step()
	{
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CEtel0Step::doTestStepPreambleL()
	{
	LBSLOG(ELogP1,"CEtel0Step::doTestStepPreambleL - Start");
	
	RLbsSystemController lbsSysController;
	lbsSysController.OpenL(KLbsSuplTestUid);
	//Start the LBS sub-system if it is not running
	if(lbsSysController.RequestSystemStartup()!= ELbsStartupSuccess)
		{
		lbsSysController.Close();
		User::Leave(KErrNotReady); 
		}
	
	//We do not need Lbs to be running - not to interfere with our tests
	//User::LeaveIfError(lbsSysController.RequestCompleteCloseDown());
	
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
	 
	LBSLOG(ELogP1,"CEtel0Step::doTestStepPreambleL - End");
	return TestStepResult();
	}


/** Perform etel0Step test step.
This test verifies that the SUPL Protocol Module correctly handles the etel network information such as 
Mobile Country Code, Mobile Network Code, cell id, timing advance, IMSI.
@return TVerdict test result code
*/
TVerdict CEtel0Step::doTestStepL()
	{
	LBSLOG(ELogP1,"CEtel0Step::doTestStepL - Start");
	
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("\tTest of ETEL params on MOLR"));
	INFO_PRINTF1(_L("\tFor this test Watchers MUST be turned off!"));
	INFO_PRINTF1(_L("\t********************************************************************"));
	INFO_PRINTF1(_L("- START -"));
	
	
	RTelServer server;
	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);
			
	RTelServer::TPhoneInfo phoneInfo;
	User::LeaveIfError(server.GetPhoneInfo(0, phoneInfo));
			
	RMobilePhone phone;
	User::LeaveIfError(phone.Open(server, phoneInfo.iName));
	CleanupClosePushL(phone);
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg networkInfoPckg(networkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
		
	for (TUint i = 0; i < 8; i++)
		{
		TUint sectionNumber = i;
		TBuf<16> configSection;
		_LIT(KSectionName, "Section%d");
		configSection.AppendFormat(KSectionName, sectionNumber);
		
		phone.NotifyCurrentNetworkChange(iStatus, networkInfoPckg, area);
		User::WaitForRequest(iStatus);	

		INFO_PRINTF2(_L("Section: %d ********************************************************************"),i);
	
		if (4 == i)
		{
		INFO_PRINTF1(_L("This one tests the case where the CountryCode is out of range"));
		INFO_PRINTF1(_L("The values expected are those set by the previous step (section=3)"));
		}
		else if (5 == i)
		{
		INFO_PRINTF1(_L("This one tests the case where the NetworkId is out of range"));
		INFO_PRINTF1(_L("The values expected are those set by a previous step (section=3)"));
		}


		INFO_PRINTF4(_L("etel: phone.NotifyCurrentNetworkChange: AreaKnown[%d] LocatuionAreaCode[%d] CellId[%d]"),area.iAreaKnown ,area.iLocationAreaCode,area.iCellId );
	
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
		User::LeaveIfNull(iNetworkObserver);

		// Wait for network observer callback
		iNetworkObserver->WaitForCallBack();
			
		// Check that callback matches the expected type
		if((CSuplNetworkTestObserver::ESendSuplPosInitSendReq != iNetworkObserver->CallBackResult()) ||
				!iNetworkObserver->WasObserverCalled())
			{
			SetTestStepResult(EFail);
			}


		TInt type = ESuplLocationTypeGsm;
		GetIntFromConfig(configSection, _L("Type"), type);

		TInt mcc = 0;
		GetIntFromConfig(configSection, _L("MCC"), mcc);
		
		TInt mnc = 0;
		GetIntFromConfig(configSection, _L("MNC"), mnc);
		
		TInt lac = 0;
		GetIntFromConfig(configSection, _L("LAC"), lac);
				
		TInt cellId = 0;
		GetIntFromConfig(configSection, _L("CellId"), cellId);
		
		TInt ta = 0;
		GetIntFromConfig(configSection, _L("TA"), ta);
		
		INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t SUPL POS INIT -> NET"));
		
		CSuplPosInit* msg = static_cast<CSuplPosInit*>(iNetworkObserver->ReceivedMessage());
		CSuplLocationId& locId = msg->GetLocationId();
		switch(locId.iStatus)
			{
			case ESuplLocStatusStale:
				INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Location id status=[Stale]"));
				break;
			case ESuplLocStatusCurrent:
				INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Location id status=[Current]"));
				break;
			case ESuplLocStatusUnknown:
				INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Location id status=[Unknown]"));
				break;
			}
				
		switch(locId.iType)
			{
			case ESuplLocationTypeGsm:
				{
				INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Location id type=[GSM]"));
				CSuplGsmCellInfo* cellInfo = locId.iGsmCellInfo;
				INFO_PRINTF6(_L("\t\t\t\t\t\t\t\t Cell info MCC=[%d] MNC=[%d] LAC=[%d] CellId=[%d] TA=[%d]"),
						cellInfo->iRefMCC, cellInfo->iRefMNC, cellInfo->iRefLAC, cellInfo->iRefCI, cellInfo->iTA);
				if(type!=locId.iType || mcc!=cellInfo->iRefMCC || mnc!=cellInfo->iRefMNC || lac!=cellInfo->iRefLAC || 
				   cellId!=cellInfo->iRefCI || ta!=cellInfo->iTA)
					{
					INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Fail!"));

					SetTestStepResult(EFail);			
					}
				break;
				}
			case ESuplLocationTypeWcdma:
				{
				INFO_PRINTF1(_L("\t\t\t\t\t\t\t\t Location id type=[WCDMA]"));
				CSuplWcdmaCellInfo* cellInfo = locId.iWcdmaCellInfo;
				INFO_PRINTF4(_L("\t\t\t\t\t\t\t\t Cell info MCC=[%d] MNC=[%d] CellId=[%d]"),
										cellInfo->iRefMCC, cellInfo->iRefMNC, cellInfo->iRefUC);	
				if(type!=locId.iType || mcc!=cellInfo->iRefMCC || mnc!=cellInfo->iRefMNC || cellId!=cellInfo->iRefUC)
					{
					SetTestStepResult(EFail);			
					}
				break;
				}
			default:
				INFO_PRINTF2(_L("\t\t\t\t\t\t\t\t Wrong location id type=[%d]"), locId.iType);
				SetTestStepResult(EFail);
				break;
			}
		
		
		
		ResetNetworkObserverL();
		
		if(TestStepResult()!=EPass)
			{
			return TestStepResult();
			}

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
		}


	CleanupStack::PopAndDestroy(&phone);
	CleanupStack::PopAndDestroy(&server);


		
	LBSLOG(ELogP1,"CEtel0Step::doTestStepL - End");
	return TestStepResult();

	}

TVerdict CEtel0Step::doTestStepPostambleL()
	{
	LBSLOG(ELogP1,"CEtel0Step::doTestStepPostambleL - Start");
	delete iTimer;
	iTimer=0;
	
	delete iSchedulerWait;
	iSchedulerWait = 0;
	
	delete iModule;
	iModule=0;
	
	CTe_suplprotocolSuiteStepBase::doTestStepPostambleL();
	LBSLOG(ELogP1,"CEtel0Step::doTestStepPostambleL - End");
	return TestStepResult();
	}

void CEtel0Step::OnTimerEventL(TInt /*aTimerId*/)
	{
	iSchedulerWait->AsyncStop();
	}

TInt CEtel0Step::OnTimerError(TInt /*aTimerId*/, TInt /*aError*/)
	{
	iSchedulerWait->AsyncStop();
	return KErrNone;
	}
	

