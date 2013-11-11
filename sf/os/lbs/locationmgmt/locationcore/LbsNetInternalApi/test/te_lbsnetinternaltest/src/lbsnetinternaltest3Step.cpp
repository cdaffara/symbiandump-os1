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
 @file lbsnetinternaltest3Step.cpp
*/
#include "lbsnetinternaltest3Step.h"
#include "Te_lbsnetinternaltestSuiteDefs.h"
#include "lbsnetinternalapi.h"

Clbsnetinternaltest3Step::~Clbsnetinternaltest3Step()
/**
 * Destructor
 */
	{
	}

Clbsnetinternaltest3Step::Clbsnetinternaltest3Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetinternaltest3Step);
	}

TVerdict Clbsnetinternaltest3Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetinternaltestSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetinternaltest3Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//
		// Test the get and set function(s) of each of the message types
		//
		// Note that ths test injects all the different message types into the 
		// EChannelNG2NRH pipe (channel) and checks that all the messages arrive 
		// at the other end at the EChannelNRH2NG channel.
		// Note that in real life, messages like TLbsNetTransmitLocationRequestMsg will
		// only be sent form the NRH to NG. But this test is actually just testing the interface and
		// so it is a valid thing to do!
		
		// messages will be sent across the netinternal api before being checked
		RLbsNetChannel netGatewayChannel;
		RLbsNetChannel netRequestChannel;
		TRequestStatus status;

		netGatewayChannel.OpenL(RLbsNetChannel::EChannelNRH2NG, *this);
		CleanupClosePushL(netGatewayChannel);
				
		netRequestChannel.OpenL(RLbsNetChannel::EChannelNG2NRH, *this);
		CleanupClosePushL(netRequestChannel);
		
		//
		// TLbsNetLocationRequestMsg
		//
		iSentMsg = CreateTLbsNetLocationRequestMsgL();
		netRequestChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
		
		//
		// TLbsNetLocationResponseMsg
		//
		iSentMsg = CreateTLbsNetLocationResponseMsgL();
		netRequestChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
		
		//
		// TLbsNetTransmitLocationRequestMsg
		//
		iSentMsg = CreateTLbsNetTransmitLocationRequestMsgL();
		netRequestChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;

		//
		// TLbsNetTransmitLocationCancelMsg
		//
		iSentMsg = CreateTLbsNetTransmitLocationCancelMsgL();
		netRequestChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
		
		//
		// TLbsNetMtLrRequestMsg
		//
		iSentMsg = CreateTLbsNetMtLrRequestMsgL();
		netRequestChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;

		//
		// TLbsNetMtLrReponseMsg
		//
		iSentMsg = CreateTLbsNetMtLrReponseMsgL();
		netRequestChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;

		//
		// TLbsNetSessionCompleteMsg
		//
		iSentMsg = CreateTLbsNetSessionCompleteMsgL();
		netRequestChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
						
		//
		// TLbsNetCancelFromPrivacyControllerMsg
		//
		iSentMsg = CreateTLbsNetECancelFromPrivacyControllerMsgL();
		netRequestChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
							
		// 
		// End of test
		//		
		CleanupStack::PopAndDestroy(2, &netGatewayChannel);
		}
	  return TestStepResult();
	}



TVerdict Clbsnetinternaltest3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetinternaltestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

/* Called when the message is received; check that the received message is the same as the one sent.
*/
void Clbsnetinternaltest3Step::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	TEST((aChannelId == RLbsNetChannel::EChannelNRH2NG) || (aChannelId == RLbsNetChannel::EChannelNG2NRH));
	TRAP_IGNORE(TestNetInternalMessagesEquivalentL(aMessage, *iSentMsg));
	CActiveScheduler::Stop();
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest3Step::CreateTLbsNetLocationRequestMsgL()
	{
	TLbsNetPosRequestQualityInt quality;
	quality.SetMaxFixTime(TTimeIntervalMicroSeconds(1500000));
	quality.SetMinHorizontalAccuracy(TReal32(10.0));
	quality.SetMinVerticalAccuracy(TReal32(15.0));
	TLbsNetPosMethodInt method;
	method.SetPosMethod(TUid::Uid(0x10282253), 
						TPositionModuleInfo::ETechnologyTerminal);
	TLbsNetPosRequestMethodInt methods;
	methods.SetPosMethods(&method, 1);	
	TLbsNetLocationRequestMsg* locReqMsg = new (ELeave) TLbsNetLocationRequestMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00001000), 
							 	   		EFalse, 
							 	   		TLbsNetworkEnumInt::EServiceTransmitThirdParty,
								   		quality,
								   		methods);
	
	return locReqMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest3Step::CreateTLbsNetLocationResponseMsgL()
	{
	TCoordinate coord(TReal64(86456.4),
					  TReal64(99543.3),
					  TReal32(626.4));
	TLocality locality(coord,
					   TReal32(50.0),
					   TReal32(25.0));
	TPosition position(locality,
					   TTime(_L("20060601:123700")));
	TPositionInfo positionInfo;
	positionInfo.SetPosition(position);
	TTime timeStamp;
	timeStamp.UniversalTime();
	TLbsNetLocationResponseMsg* locRespMsg = new (ELeave) TLbsNetLocationResponseMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00001010),
										  KErrNone,
										  positionInfo,
										  timeStamp);
	return locRespMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest3Step::CreateTLbsNetTransmitLocationRequestMsgL()
	{
	_LIT(KDestination, "08745484950");
	TLbsNetTransmitLocationRequestMsg* transmitLocReqMsg = new (ELeave) TLbsNetTransmitLocationRequestMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00001020),
										   				KDestination,
										   				1);
	return transmitLocReqMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest3Step::CreateTLbsNetTransmitLocationCancelMsgL()
	{
	TLbsNetTransmitLocationCancelMsg* transmitLocCancelMsg = new (ELeave) TLbsNetTransmitLocationCancelMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00001020),
														  KErrCancel);
	return transmitLocCancelMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest3Step::CreateTLbsNetMtLrRequestMsgL()
	{
	_LIT8(KRequesterId, "thepolice");
	_LIT8(KClientName, "officersmith");
	_LIT8(KClientExternalId, "pc1234");
	TLbsNetPosRequestPrivacyInt privacy;
	privacy.SetRequestAdvice(TLbsNetPosRequestPrivacyInt::ERequestAdviceNotify);
	privacy.SetRequestAction(TLbsNetPosRequestPrivacyInt::ERequestActionAllow);
	TLbsExternalRequestInfo requestInfo;
	requestInfo.SetRequesterId(KRequesterId);
	requestInfo.SetClientName(KClientName);
	requestInfo.SetClientExternalId(KClientExternalId);
	TLbsNetMtLrRequestMsg* mtlrReqMsg = new (ELeave) TLbsNetMtLrRequestMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00001030),
									 EFalse,
									 privacy,
							   		 requestInfo);
	return mtlrReqMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest3Step::CreateTLbsNetMtLrReponseMsgL()
	{
	TLbsNetMtLrReponseMsg* mtlrRespMsg = new (ELeave) TLbsNetMtLrReponseMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00001040),
							   		  TLbsNetworkEnumInt::EPrivacyResponseAccepted, KErrNone);
	return mtlrRespMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest3Step::CreateTLbsNetSessionCompleteMsgL()
	{
	TLbsNetSessionCompleteMsg* sessionCompMsg = new (ELeave) TLbsNetSessionCompleteMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00001050),
											 KErrNone);
	return sessionCompMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest3Step::CreateTLbsNetECancelFromPrivacyControllerMsgL()
	{
	TLbsNetCancelFromPrivacyControllerMsg* sessionCompMsg = new (ELeave) TLbsNetCancelFromPrivacyControllerMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00001060),
											 KErrNone);
	return sessionCompMsg;
	}

