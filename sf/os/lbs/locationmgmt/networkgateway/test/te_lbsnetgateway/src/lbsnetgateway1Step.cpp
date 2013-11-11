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
 @file lbsnetgateway1Step.cpp
*/
#include "lbsnetgateway1Step.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "lbsnetinternalapi.h"
#include "lbsnrhngmsgs.h"
#include "lbscommoninternaldatatypes.h"

_LIT8(KRequesterId, "08450000001");
_LIT8(KClientName, "abcdefghijkl.com");
_LIT8(KClientExternalId, "test_externalclientid");
_LIT(KDestination, "0800999999");

const TLbsNetTransmitLocationCancelMsg KTransmitLocCancel(TLbsNetSessionIdInt(KTe_lbsnetgatewaySuiteUid, 10002020), 
														   KErrCancel);
const TLbsNetMtLrReponseMsg KMtlrResp(TLbsNetSessionIdInt(KTe_LbsNetGatewayTestProtocolUid, 10003030), 
									   TLbsNetworkEnumInt::EPrivacyResponseAccepted, KErrNone);
const TLbsNetCancelFromPrivacyControllerMsg KCancelExtLocate(TLbsNetSessionIdInt(KTe_LbsNetGatewayTestProtocolUid, 10005050), 
														   KErrCancel);

Clbsnetgateway1Step::~Clbsnetgateway1Step()
/**
 * Destructor
 */
	{
	}

Clbsnetgateway1Step::Clbsnetgateway1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgateway1Step);
	}

TVerdict Clbsnetgateway1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Clbsnetgateway1Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	

	InitialiseUidArray();
	
	// Connect to NRH, stub PM interfaces
	iNetGatewayChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;
	iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgateway1Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("*** Clbsnetgateway1Step::doTestStepL() >> </b>"));
	  if (TestStepResult()==EPass)
		{
		//
		// Test the sending and receiving of each type of NRH message.
		// All messages in both directions (NRH->NG->PM) and
		// (PM->NG->NRH) are tested and we organise them in type of session.
		// Otherwise the gateway doesn't know where to send them.
		//
		
		//
		// Section 1 : MTLR  operations
		//
		//
		// 1) ProcessPrivacyRequest()
		//
		SNetMsgProcessPrivacyRequest* privReq = reinterpret_cast<SNetMsgProcessPrivacyRequest*>(iMsg.Data());
		new (privReq) SNetMsgProcessPrivacyRequest;
		iMsg.iType = ENetMsgProcessPrivacyRequest;
		privReq->iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 20001010);
		privReq->iEmergency = EFalse;
		iPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
		iPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
		privReq->iPrivacy = iPrivacy;
		privReq->iRequestInfo.SetRequesterId(KRequesterId);
		privReq->iRequestInfo.SetClientName(KClientName);
		privReq->iRequestInfo.SetClientExternalId(KClientExternalId);
		iNetProtocolMessage = iMsg;
		INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(iMsg); >> </b>"));
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(iMsg); << </b>"));
		INFO_PRINTF1(_L("*** CActiveScheduler::Start(); >> </b>"));
		CActiveScheduler::Start();
		INFO_PRINTF1(_L("*** CActiveScheduler::Start(); << </b>"));
		
		

		
		//
		// 2) ProcessLocationRequest
		//
		SNetMsgProcessLocationRequest* locReq = reinterpret_cast<SNetMsgProcessLocationRequest*>(iMsg.Data());
		iMsg.iType = ENetMsgProcessLocationRequest;
		locReq->iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 20002020);
		locReq->iEmergency = EFalse;
		locReq->iService = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
		locReq->iQuality.SetMaxFixTime(TTimeIntervalMicroSeconds(30000000));
		locReq->iQuality.SetMinHorizontalAccuracy(TReal32(250.0));
		locReq->iQuality.SetMinVerticalAccuracy(TReal32(250.0));
		TLbsNetPosMethod methodArray[2];
		methodArray[0].SetPosMethod(TUid::Uid(0x1028224E), (TPositionModuleInfo::ETechnologyTerminal 
															| TPositionModuleInfo::ETechnologyAssisted));
		methodArray[1].SetPosMethod(TUid::Uid(0x1028224E), (TPositionModuleInfo::ETechnologyNetwork 
															| TPositionModuleInfo::ETechnologyAssisted));
		locReq->iMethod.SetPosMethods(methodArray, (sizeof(methodArray) / sizeof(TLbsNetPosMethod)));
		iNetProtocolMessage = iMsg;
		INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(iMsg); >> </b>"));
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(iMsg); << </b>"));
		INFO_PRINTF1(_L("*** CActiveScheduler::Start(); >> </b>"));
		CActiveScheduler::Start();
		INFO_PRINTF1(_L("*** CActiveScheduler::Start(); << </b>"));

		
		//
		// 3) TLbsNetCancelExternalLocationMsg
		//
		iNetInternalMessage = &KCancelExtLocate;
		INFO_PRINTF1(_L("*** iNetGatewayChannel->SendMessageAndWaitForResponse(KCancelExtLocate) </b>"));
		iNetGatewayChannel->SendMessageAndWaitForResponse(KCancelExtLocate);
				
		//
		// 4) TLbsNetMtLrReponseMsg
		//
		iNetInternalMessage = &KMtlrResp;
		INFO_PRINTF1(_L("*** iNetGatewayChannel->SendMessageAndWaitForResponse(KMtlrResp) </b>"));
		iNetGatewayChannel->SendMessageAndWaitForResponse(KMtlrResp);

		//
		// 5) TLbsNetLocationResponseMsg carrying a TPositionInfo
		//

		iPosition.SetCoordinate(TReal64(36.756), TReal64(123.546), TReal32(74.263));
		iPosition.SetAccuracy(TReal32(0.5), TReal32(0.5));
		iPosition.SetCurrentTime();
		iPositionInfo.SetPosition(iPosition);

		iTimeStamp.UniversalTime();
		TLbsNetLocationResponseMsg* netLocResp = new(ELeave) TLbsNetLocationResponseMsg(TLbsNetSessionIdInt(KTe_LbsNetGatewayTestProtocolUid, 10001010),
											  KErrNone, 
											  iPositionInfo,
											  iTimeStamp);
		iNetInternalMessage = netLocResp;
		INFO_PRINTF1(_L("*** iNetGatewayChannel->SendMessageAndWaitForResponse(*netLocResp) </b>"));
		iNetGatewayChannel->SendMessageAndWaitForResponse(*netLocResp);
		delete netLocResp;
		
		//
		// 6) TLbsNetLocationResponseMsg carrying a TPositionGpsMeasurementInfo
		//
		iGpsPositionInfo.ClearMeasurementData();
		iGpsPositionInfo.SetGpsTimeOfWeek(1053);
		
		TPositionGpsMeasurementData gpsData;
		gpsData.SetSatelliteId(1);
		gpsData.SetCarrierNoiseRatio(8);
		gpsData.SetDoppler(34);
		gpsData.SetWholeGpsChips(257);
		gpsData.SetFractionalGpsChips(34);
		gpsData.SetMultiPathIndicator(TPositionGpsMeasurementData::EMultiPathMedium);
		gpsData.SetPseudoRangeRmsError(0x1000);		
		iGpsPositionInfo.AppendMeasurementData(gpsData);
		
		iTimeStamp.UniversalTime();
		netLocResp = new(ELeave) TLbsNetLocationResponseMsg(TLbsNetSessionIdInt(KTe_LbsNetGatewayTestProtocolUid, 10001010),
															KErrNone, 
															iGpsPositionInfo,
															iTimeStamp);
		iNetInternalMessage = netLocResp;
		INFO_PRINTF1(_L("*** iNetGatewayChannel->SendMessageAndWaitForResponse(*netLocResp) </b>"));
		iNetGatewayChannel->SendMessageAndWaitForResponse(*netLocResp);
		delete netLocResp;


		// Section 2 X3P


		// 7) TLbsNetTransmitLocationRequestMsg without technology option
		//
		TLbsNetTransmitLocationRequestMsg* netTransmitReq 
			= new(ELeave) TLbsNetTransmitLocationRequestMsg(TLbsNetSessionIdInt(KTe_lbsnetgatewaySuiteUid, 10002020), 
	 																										KDestination, 
																										 	1);
		iNetInternalMessage = netTransmitReq;
		INFO_PRINTF1(_L("*** iNetGatewayChannel->SendMessageAndWaitForResponse(*netLocResp) </b>"));
		iNetGatewayChannel->SendMessageAndWaitForResponse(*netTransmitReq);
		delete netTransmitReq;

		//
		// 8) TLbsNetTransmitLocationRequestMsg with technology option
		//
		TLbsNetPosRequestOptionsTechnologyInt netPosOptTechnology;
		netPosOptTechnology.SetPosMode(TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted);
		netTransmitReq = new(ELeave) TLbsNetTransmitLocationRequestMsg(TLbsNetSessionIdInt(KTe_lbsnetgatewaySuiteUid, 10002020), 
	 																																 KDestination, 
																																 	 1,
																																	 netPosOptTechnology);
		iNetInternalMessage = netTransmitReq;
		INFO_PRINTF1(_L("*** iNetGatewayChannel->SendMessageAndWaitForResponse(*netTransmitReq) </b>"));
		iNetGatewayChannel->SendMessageAndWaitForResponse(*netTransmitReq);
		delete netTransmitReq;

		
		//
		// 9) TLbsNetTransmitLocationCancelMsg
		//
		iNetInternalMessage = &KTransmitLocCancel;
		INFO_PRINTF1(_L("*** iNetGatewayChannel->SendMessageAndWaitForResponse(KTransmitLocCancel) </b>"));
		iNetGatewayChannel->SendMessageAndWaitForResponse(KTransmitLocCancel);
		

		//
		// 10) ProcessSessionComplete
		//
		SNetMsgProcessSessionComplete* sessComp = reinterpret_cast<SNetMsgProcessSessionComplete*>(iMsg.Data());
		iMsg.iType = ENetMsgProcessSessionComplete;
		sessComp->iSessionId = TLbsNetSessionId(KTe_lbsnetgatewaySuiteUid, 20003030);
		sessComp->iReason = KErrArgument;
		iNetProtocolMessage = iMsg;
		INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(iMsg) >> </b>"));
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(iMsg) << </b>"));
		INFO_PRINTF1(_L("*** CActiveScheduler::Start() >> </b>"));
		CActiveScheduler::Start();
		INFO_PRINTF1(_L("*** CActiveScheduler::Start() << </b>"));


		//
		// Section 3 : Messages not associated with a particular session
		//

		
		//
		// 11) GetCurrentCapabilties 
		//


		//	Regular gateway should support this message
		SNetMsgGetCurrentCapabilitiesRequest* capReq = reinterpret_cast<SNetMsgGetCurrentCapabilitiesRequest*>(iMsg.Data());
		iMsg.iType = ENetMsgGetCurrentCapabilitiesRequest;
		iNetProtocolMessage = iMsg;
		INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(iMsg) >> </b>"));
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		INFO_PRINTF1(_L("*** iStubPMChannel.SendNetProtocolMessageL(iMsg) << </b>"));
		INFO_PRINTF1(_L("*** CActiveScheduler::Start() >> </b>"));
		CActiveScheduler::Start();
		INFO_PRINTF1(_L("*** CActiveScheduler::Start() << </b>"));


		
		}
	  INFO_PRINTF1(_L("*** Clbsnetgateway1Step::doTestStepL() << "));
	  return TestStepResult();
	}



TVerdict Clbsnetgateway1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Clbsnetgateway1Step"));
	// process something post setting to the test step

	// Close NRH, A-GPS, stub PM interfaces
	iStubPMChannelArray->Close();
	delete iNetGatewayChannel;
		
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

void Clbsnetgateway1Step::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	TEST(aChannelId == RLbsNetChannel::EChannelNRH2NG);
	TRAP_IGNORE(TestMessagesAreEquivalentL(aMessage, iNetProtocolMessage));
	CActiveScheduler::Stop();
	}
	
void Clbsnetgateway1Step::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
	{
	switch (aMessage.iType)
		{
		case ENetMsgGetCurrentCapabilitiesResponse:
			{
			TEST(iNetProtocolMessage.iType == ENetMsgGetCurrentCapabilitiesRequest);
			CActiveScheduler::Stop();
			break;
			}
		case ENetMsgProcessPrivacyRequest:
		case ENetMsgProcessLocationRequest:
		case ENetMsgProcessSessionComplete:
		case ENetMsgProcessAssistanceData:
		case ENetMsgProcessLocationUpdate:
		case ENetMsgGetCurrentCapabilitiesRequest:
		case ENetMsgProcessStatusUpdate:
		case ENetMsgRespondPrivacyRequest:
		case ENetMsgRespondLocationRequest:
		case ENetMsgRequestTransmitLocation:
		case ENetMsgCancelTransmitLocation:
		case ENetMsgRequestAssistanceData:
		case ENetMsgRequestSelfLocation:
		case ENetMsgCancelSelfLocation:
		case ENetMsgRequestNetworkLocation:
		case ENetMsgCancelNetworkLocation:
		case ENetMsgCancelExternalLocation:
		default:
			{
			// Check that the incoming message carries the same information as
			// the message that was sent
			TRAP_IGNORE(TestMessagesAreEquivalentL(*iNetInternalMessage, aMessage));			
			}
		}
	}
