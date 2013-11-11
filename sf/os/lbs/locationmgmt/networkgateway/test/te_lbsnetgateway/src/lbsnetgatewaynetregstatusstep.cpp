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
 @file lbsnetgatewaynetregstatusstep.cpp
*/

#include <e32debug.h>

#include <simtsy.h>
#include "lbsnetgatewaynetregstatusstep.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "lbsnetinternalapi.h"
#include "lbsagpsngmsgs.h"

_LIT(KDestination, "0800000000");
_LIT8(KRequesterId, "08450000000");
_LIT8(KClientName, "abcdefghijkl.com");
_LIT8(KClientExternalId, "test_externalclientid");


const TLbsNetTransmitLocationRequestMsg KTransmitLocReq(TLbsNetSessionIdInt(KTe_lbsnetgatewaySuiteUid, 10002020), 
														 KDestination, 
														 1);
const TLbsNetTransmitLocationCancelMsg KTransmitLocCancel(TLbsNetSessionIdInt(KTe_lbsnetgatewaySuiteUid, 10002020), 
														   KErrCancel);
const TLbsNetMtLrReponseMsg KMtlrResp(TLbsNetSessionIdInt(KTe_lbsnetgatewaySuiteUid, 10003030), 
									   TLbsNetworkEnumInt::EPrivacyResponseAccepted, KErrNone);


CLbsNetGatewayNetRegStatusStep::~CLbsNetGatewayNetRegStatusStep()
	{
	}

CLbsNetGatewayNetRegStatusStep::CLbsNetGatewayNetRegStatusStep() : 
iCurProtocolUid(KTe_LbsNetGatewayTestProtocolUid)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsNetGatewayNetRegStatusStep);
	}

TVerdict CLbsNetGatewayNetRegStatusStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{	
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
	// Connect to NRH, stub PM interfaces
	iNetGatewayChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);

    InitialiseUidArray();
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel[2];
    iStubPMChannelArray[0].OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
    iStubPMChannelArray[1].OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex2);
    iCurChannel = &(iStubPMChannelArray[0]);
	// Assistance data messages expected to arrive over this interface
	iAgpsChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelAGPS2NG);
	iAssDataCache.OpenL();
	
	// Open the network location manager channel
	iNetworkLocationChannel = CNetworkLocationChannel::NewL(*this);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsNetGatewayNetRegStatusStep::doTestStepL()
	{
	  if (TestStepResult()==EPass)
		{
		//
		// Test that new requests are treated as expected, depending
		// on the network registration status.
		//
		
		TInt loadingConfig;
		if(!GetIntFromConfig(ConfigSection(), _L("ModuleLoadingConfig"), loadingConfig))
			{
			ERR_PRINTF1(_L("Failed to get the ModuleLoadingConfig setting from the test ini file"));
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		
		switch(loadingConfig)
			{
			case EByStatusTheSameProt:
				return MakeByStatusTheSameProtTestsL();
			case EByStatusDiffProtHomeFirst:
				return MakeByStatusDiffProtHomeFirstTestsL();
			case EByStatusDiffProtRoamingFirst:
				return MakeByStatusDiffProtRoamingFirstTestsL();	
			case EAllProtTheSameProt:
				return MakeAllProtTheSameProtTestsL();
			default:	
			case EAllProtDiffProt:
				return MakeAllProtDiffProtL();
			}
		}
	return TestStepResult();
	}

TVerdict CLbsNetGatewayNetRegStatusStep::MakeByStatusTheSameProtTestsL()
	{	
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
					
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
					
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
					
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
	
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
	
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
	
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
	
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
	
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
	
	return TestStepResult();
	}

TVerdict CLbsNetGatewayNetRegStatusStep::MakeByStatusDiffProtHomeFirstTestsL()
	{
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
						
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
						
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
						
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
	
	return TestStepResult();
	}

TVerdict CLbsNetGatewayNetRegStatusStep::MakeByStatusDiffProtRoamingFirstTestsL()
	{
	//Switch to the roaming PM channel first 
	iCurChannel = &(iStubPMChannelArray[1]);
	iCurProtocolUid = KTe_LbsNetGatewayTestProtocolUid2;
	
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
							
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
							
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
							
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
				
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
	
	return TestStepResult();
	}


TVerdict CLbsNetGatewayNetRegStatusStep::MakeAllProtTheSameProtTestsL()
	{
	return MakeByStatusTheSameProtTestsL();
	}

TVerdict CLbsNetGatewayNetRegStatusStep::MakeAllProtDiffProtL()
	{
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);	
	TestNewRequestResponseL(ETrue, ERejectPrivReq);	
						
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
						
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
						
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, ERejectPrivReq);
		
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);	
	
	//Switch to the roaming channel
	iCurChannel = &(iStubPMChannelArray[1]);
	iCurProtocolUid = KTe_LbsNetGatewayTestProtocolUid2;
	
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
							
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, ERejectPrivReq);
							
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
							
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, ERejectPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	TestNewRequestResponseL(ETrue, ERejectPrivReq);
				
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredRoaming);
	TestNewRequestResponseL(EFalse, EAcceptPrivReq);
			
	SetNetworkRegistrationStatusL(RMobilePhone::ERegisteredOnHomeNetwork);
	TestNewRequestResponseL(EFalse, ERejectPrivReq);
	
	return TestStepResult();
	}



TVerdict CLbsNetGatewayNetRegStatusStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetNetworkRegistrationStatusL(RMobilePhone::ENotRegisteredNoService);
	
	// Cleanup any objects left in the arrays
	iNlmMsgArray.ResetAndDestroy();
	iNrhOrAgpsMsgArray.ResetAndDestroy();
	iPmMsgArray.ResetAndDestroy();
	
	// Close NLM, NRH, A-GPS, stub PM interfaces
	delete iNetworkLocationChannel;
	iAssDataCache.Close();
	delete iAgpsChannel;

	iStubPMChannelArray[0].Close();
	iStubPMChannelArray[1].Close();
	delete iNetGatewayChannel;
	
	// Uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();

	return TestStepResult();
	}

/** Change the phone's network registration status.

This sends the new network registration status to the sim.tsy via
the special RProperty for it.
*/
void CLbsNetGatewayNetRegStatusStep::SetNetworkRegistrationStatusL(RMobilePhone::TMobilePhoneRegistrationStatus aNetRegStatus)
	{
	User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, 
									  KPSSimTsyRegStatChange,
									  aNetRegStatus));
	// Short delay to allow the new value to propagate to Network Gateway.
	User::After(500000);
	}

void CLbsNetGatewayNetRegStatusStep::TestNewRequestResponseL(TBool iExpectRequestToBeRejected, 
		TAcceptPrivacyRequest iAcceptPrivReq)
	{
	//
	// Test 1 : Send a self-locate request.
	//
	// INFO_PRINTF1(_L("Test 1 : Send a self-locate request."));
	TLbsAsistanceDataGroup dataGroup = (EAssistanceDataAquisitionAssistance
				  | EAssistanceDataBadSatList
				  | EAssistanceDataNavigationModel
				  | EAssistanceDataReferenceTime
				  | EAssistanceDataIonosphericModel
				  | EAssistanceDataDgpsCorrections
				  | EAssistanceDataReferenceLocation
				  | EAssistanceDataAlmanac
				  | EAssistanceDataPositioningGpsUtcModel);

	TLbsNetPosRequestOptionsAssistanceInt options;
	options.SetDataRequestMask(dataGroup);
	options.SetNewClientConnected(ETrue);
	TLbsNetSessionIdInt sessionId(KLbsGpsLocManagerUid, ++iSessionId);
 	TLbsNetSelfLocationRequestMsg msgSL(sessionId, options);
	iAgpsChannel->SendMessageAndWaitForResponse(msgSL);
	WaitForNextResponse();
	
	if (iExpectRequestToBeRejected)
		{
		// INFO_PRINTF1(_L("Expect request to be rejected"));
		// Expect a (dummy) assistance data event then 2 session completes to the Agps Mgr & NRH
		// INFO_PRINTF3(_L("Message count %d. First message is %d"), iNrhOrAgpsMsgArray.Count(), iNrhOrAgpsMsgArray[0]->Type());
		TESTL(iNrhOrAgpsMsgArray.Count()==1 || iNrhOrAgpsMsgArray.Count()==2 || iNrhOrAgpsMsgArray.Count()==3);
		TESTL(iNrhOrAgpsMsgArray[0]->Type() == TLbsNetInternalMsgBase::EAssistanceDataResponse);	// first msg will always be dummy assist
				
		if(iNrhOrAgpsMsgArray.Count()==1)
			{
			iNrhOrAgpsMsgArray.ResetAndDestroy();
			WaitForNextResponse();	
			// INFO_PRINTF3(_L("Message count %d. Second message is %d"), iNrhOrAgpsMsgArray.Count(), iNrhOrAgpsMsgArray[0]->Type());
			TESTL(iNrhOrAgpsMsgArray.Count() == 1 || iNrhOrAgpsMsgArray.Count()==2);
			if(iNrhOrAgpsMsgArray.Count()==1)
				{
				TESTL(iNrhOrAgpsMsgArray[0]->Type() == TLbsNetInternalMsgBase::ESessionComplete);
				iNrhOrAgpsMsgArray.ResetAndDestroy();				
				WaitForNextResponse();	
				// INFO_PRINTF3(_L("Message count %d. Third message is %d"), iNrhOrAgpsMsgArray.Count(), iNrhOrAgpsMsgArray[0]->Type());
				TESTL(iNrhOrAgpsMsgArray[0]->Type() == TLbsNetInternalMsgBase::ESessionComplete);
				}
			else	// iNrhOrAgpsMsgArray.Count()==2
				{
				TESTL(iNrhOrAgpsMsgArray[0]->Type() == TLbsNetInternalMsgBase::ESessionComplete);	
				TESTL(iNrhOrAgpsMsgArray[1]->Type() == TLbsNetInternalMsgBase::ESessionComplete);				
				}		
			}
		else if(iNrhOrAgpsMsgArray.Count()==2)
			{
			TESTL(iNrhOrAgpsMsgArray[1]->Type() == TLbsNetInternalMsgBase::ESessionComplete);		
			}
		else	//iNrhOrAgpsMsgArray.Count()==3
			{
			TESTL(iNrhOrAgpsMsgArray[1]->Type() == TLbsNetInternalMsgBase::ESessionComplete);	
			TESTL(iNrhOrAgpsMsgArray[2]->Type() == TLbsNetInternalMsgBase::ESessionComplete);	
			}
		iNrhOrAgpsMsgArray.ResetAndDestroy();
		}
	else
		{
		// Expect to get a self-locate request from the PM.
		TESTL(iPmMsgArray.Count() == 1);
		TESTL(iPmMsgArray[0]->iType == ENetMsgRequestSelfLocation);
		iPmMsgArray.ResetAndDestroy();
		}
	
	//
	// Test 2 : Send an assistance data request. Can only try this if start of self locate session was successful
	//
	//
	// INFO_PRINTF1(_L("Test 2 : Send an assistance data request."));
	if (!iExpectRequestToBeRejected)
		{
		dataGroup = (EAssistanceDataAquisitionAssistance
				  | EAssistanceDataBadSatList
				  | EAssistanceDataIonosphericModel
				  | EAssistanceDataAlmanac
				  | EAssistanceDataPositioningGpsUtcModel);
		TLbsNetAssistanceDataRequestMsg msgAD(dataGroup);
		iAgpsChannel->SendMessageAndWaitForResponse(msgAD);
		WaitForNextResponse();

		// Expect to get an assistance request from each PM.
		TESTL(iPmMsgArray.Count() >= 1);
		TESTL(iPmMsgArray[0]->iType == ENetMsgRequestAssistanceData);
		iPmMsgArray.ResetAndDestroy();
		}
	
	//
	// Test 3 : Send a transmit locate request.
	//
	// INFO_PRINTF1(_L("Test 3 : Send a transmit locate request."));
	TLbsNetPosRequestOptionsTechnologyInt netPosOptTechnology;
	netPosOptTechnology.SetPosMode(TPositionModuleInfo::ETechnologyTerminal
								   | TPositionModuleInfo::ETechnologyAssisted);
	TLbsNetTransmitLocationRequestMsg netTransmitReq(
			TLbsNetSessionIdInt(KLbsNetRequestHandlerUid, ++iSessionId), 
			KDestination, 
			1,
			netPosOptTechnology);
	iNetGatewayChannel->SendMessageAndWaitForResponse(netTransmitReq);
	WaitForNextResponse();
	if (iExpectRequestToBeRejected)
		{
		// Expect session complete with error to be sent to NRH.
		TESTL(iNrhOrAgpsMsgArray.Count() == 1);
		TESTL(iNrhOrAgpsMsgArray[0]->Type() == TLbsNetInternalMsgBase::ESessionComplete);
		iNrhOrAgpsMsgArray.ResetAndDestroy();
		}
	else
		{
		// Expect to get an assistance request from each PM.
		TESTL(iPmMsgArray.Count() >= 1);
		TESTL(iPmMsgArray[0]->iType == ENetMsgRequestTransmitLocation);
		iPmMsgArray.ResetAndDestroy();
		}
	//
	// Test 4 : Send a network-based self locate request.
	//
	// INFO_PRINTF1(_L("Test 4 : Send a network-based self locate request."));
	TLbsNetPosRequestQualityInt quality;
	TLbsNetLocNetworkLocationRequestMsg networkLocationMsg(
			TLbsNetSessionIdInt(KLbsNetLocManagerUid, ++iSessionId),
			ETrue, 
			quality);
	User::LeaveIfError(iNetworkLocationChannel->SendNetworkLocationMessage(networkLocationMsg));
	WaitForNextResponse();
	
	if (iExpectRequestToBeRejected)
		{
		// Expect session complete with error to be sent to NRH.
		TESTL(iNlmMsgArray.Count()==1 || iNrhOrAgpsMsgArray.Count()==1);		
		
		TBool isNlmMsg = ETrue;
		if(iNlmMsgArray.Count()==1)
			{
			TESTL(iNlmMsgArray[0]->Type() == TLbsNetLocMsgBase::ENetLocMsgNetworkLocationComplete);
			iNlmMsgArray.ResetAndDestroy();
			}
		else
			{
			TESTL(iNrhOrAgpsMsgArray[0]->Type() == TLbsNetInternalMsgBase::ESessionComplete);
			isNlmMsg = EFalse;
			iNrhOrAgpsMsgArray.ResetAndDestroy();
			}
		// INFO_PRINTF1(_L("Test 4 : iExpectRequestToBeRejected."));
		WaitForNextResponse();
		
		if(isNlmMsg)
			{
			TESTL(iNrhOrAgpsMsgArray.Count()==1);
			TESTL(iNrhOrAgpsMsgArray[0]->Type() == TLbsNetInternalMsgBase::ESessionComplete);
			iNrhOrAgpsMsgArray.ResetAndDestroy();			
			}
		else
			{
			TESTL(iNlmMsgArray.Count()==1);
			TESTL(iNlmMsgArray[0]->Type() == TLbsNetLocMsgBase::ENetLocMsgNetworkLocationComplete);
			iNlmMsgArray.ResetAndDestroy();
			}
		}
	else
		{
		// Expect to get an assistance request from the PM.
		TESTL(iPmMsgArray.Count() == 1);
		TESTL(iPmMsgArray[0]->iType == ENetMsgRequestNetworkLocation);
		iPmMsgArray.ResetAndDestroy();
		}
		
	//
	// Test 5 : Send a MT-LR privacy request.
	//
	// INFO_PRINTF1(_L("Test 5 : Send a MT-LR privacy request."));
	SNetMsgProcessPrivacyRequest* privReq = reinterpret_cast<SNetMsgProcessPrivacyRequest*>(iMsg.Data());
	new (privReq) SNetMsgProcessPrivacyRequest;
	iMsg.iType = ENetMsgProcessPrivacyRequest;
	privReq->iSessionId = TLbsNetSessionId(iCurProtocolUid, 20001010);
	privReq->iEmergency = EFalse;	
	privReq->iPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
	privReq->iPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	privReq->iRequestInfo.SetRequesterId(KRequesterId);
	privReq->iRequestInfo.SetClientName(KClientName);
	privReq->iRequestInfo.SetClientExternalId(KClientExternalId);
	
	__ASSERT_ALWAYS(iCurChannel, User::Invariant());
	
	TRAPD(err, iCurChannel->SendNetProtocolMessageL(iMsg));
	if(err==KErrNone) //If err!=KErrNone that means that PM we need is not loaded yet
		{
		// INFO_PRINTF1(_L("Test 5 : err==KErrNone."));
		WaitForNextResponse();
	
		if(iAcceptPrivReq==EAcceptPrivReq)
			{
			// Expect privacy request to be sent to NRH.
			TESTL(iNrhOrAgpsMsgArray.Count() == 1);
			TESTL(iNrhOrAgpsMsgArray[0]->Type() == TLbsNetInternalMsgBase::EPrivacyRequest);
			iNrhOrAgpsMsgArray.ResetAndDestroy();
			}
		else //iAcceptPrivReq==ERejectPrivReq
			{
			// Expect to get a privacy response (rejection) to the PM.
			TESTL(iPmMsgArray.Count() == 1);
			TESTL(iPmMsgArray[0]->iType == ENetMsgRespondPrivacyRequest);
			iPmMsgArray.ResetAndDestroy();
			}
		}
	}

void CLbsNetGatewayNetRegStatusStep::ProcessNetChannelMessage(
		RLbsNetChannel::TLbsNetChannelId /*aChannelId*/, 
		const TLbsNetInternalMsgBase& aMessage)
	{
	TAny* msg = User::Alloc(aMessage.Size());
	if (msg)
		{
		Mem::Copy(msg, &aMessage, aMessage.Size());
		TInt err = iNrhOrAgpsMsgArray.Append(reinterpret_cast<TLbsNetInternalMsgBase*>(msg));
		if(err!=KErrNone)
			{
			delete msg;
			}
		}
	else
		{
		// Some error...
		}
	
	if (iActiveWait.IsStarted())
		{
		iActiveWait.AsyncStop();
		}
	}
	
void CLbsNetGatewayNetRegStatusStep::ProcessNetProtocolMessage(
		const TNetGatewayMsg& aMessage)
	{
	TAny* msg = User::Alloc(sizeof(TNetGatewayMsg));
	if (msg)
		{
		Mem::Copy(msg, &aMessage, sizeof(TNetGatewayMsg));
		TInt err = iPmMsgArray.Append(reinterpret_cast<TNetGatewayMsg*>(msg));
		if(err!=KErrNone)
			{
			delete msg;
			}
		}
	else
		{
		// Some error...
		}
	
	if (iActiveWait.IsStarted())
		{
		iActiveWait.AsyncStop();
		}
	}

void CLbsNetGatewayNetRegStatusStep::ProcessNetworkLocationResponseMessage(
		const TLbsNetLocMsgBase& aMessage)
	{
	TAny* msg = User::Alloc(aMessage.Size());
	if (msg)
		{
		Mem::Copy(msg, &aMessage, aMessage.Size());
		TInt err = iNlmMsgArray.Append(reinterpret_cast<TLbsNetLocMsgBase*>(msg));
		if(err!=KErrNone)
			{
			delete msg;
			}		
		}
	else
		{
		// Some error...
		}
	
	if (iActiveWait.IsStarted())
		{
		iActiveWait.AsyncStop();
		}
	}

void CLbsNetGatewayNetRegStatusStep::WaitForNextResponse()
	{
	if (iPmMsgArray.Count() == 0
		&& iNrhOrAgpsMsgArray.Count() == 0
		&& iNlmMsgArray.Count() == 0)
		{
		iActiveWait.Start();
		}
	}

