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
 @file lbsnetgatewaymultiprivacystep.cpp
*/
#include "lbsnetgatewaymultiprivacystep.h"
#include "Te_lbsnetgatewaySuiteDefs.h"
#include "lbsnetinternalapi.h"
#include "lbsnrhngmsgs.h"
#include "lbscommoninternaldatatypes.h"

_LIT8(KRequesterId, "08450000000");
_LIT8(KClientName, "abcdefghijkl.com");
_LIT8(KClientExternalId, "test_externalclientid");
_LIT(KDestination, "0800000000");

const TLbsNetTransmitLocationRequestMsg KTransmitLocReq(TLbsNetSessionIdInt(KTe_LbsNetGatewayTestProtocolUid, 10002020), 
														 KDestination, 
														 1);
const TLbsNetTransmitLocationCancelMsg KTransmitLocCancel(TLbsNetSessionIdInt(KTe_LbsNetGatewayTestProtocolUid, 10002020), 
														   KErrCancel);
const TLbsNetMtLrReponseMsg KMtlrResp(TLbsNetSessionIdInt(KTe_LbsNetGatewayTestProtocolUid, 10003030), 
		TLbsNetworkEnumInt::EPrivacyResponseAccepted, KErrNone);


Clbsnetgatewaymultiprivacystep::~Clbsnetgatewaymultiprivacystep()
/**
 * Destructor
 */
	{
	}

Clbsnetgatewaymultiprivacystep::Clbsnetgatewaymultiprivacystep() :
	iState(EMultiPrivacyCheckBuffer)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgatewaymultiprivacystep);
	}

TVerdict Clbsnetgatewaymultiprivacystep::doTestStepPreambleL()
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
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;
    iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgatewaymultiprivacystep::doTestStepL()
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
		// Test the buffering of requests in standalone privacy 
		// mode.
		//
		// Buffering is different in this mode, because more than one
		// request can be buffered, so there is no cancelling of the 
		// current request if a new one comes into the NG.
		//
		// Note: before this step is run, the NG must have been started
		// with the KLbsSettingMaximumExternalLocateRequests admin setting
		// set > 1. (This is handled by the lbsnetgatewaystartstep, from
		// the .ini file).
		//
		
		//
		// Test 1 : Simply send a privacy request through
		//
		
		//
		// Simple single privacy request
		//
		TNetGatewayMsg* msg = CreatePrivacyRequestMsgLC(TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 20001000));
		iMsgArray.AppendL(msg);
		CleanupStack::Pop(msg);
		iStubPMChannelArray->SendNetProtocolMessageL(*msg);
		iActiveWait.Start(); // Wait for message on NRH channel

		//
		// Multiple privacy requests
		//
		// Initial request to fill the channel
		iState = EMultiPrivacyInitialRequest;
		msg = CreatePrivacyRequestMsgLC(TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 20001010));
		iMsgArray.AppendL(msg);
		CleanupStack::Pop(msg);
		iStubPMChannelArray->SendNetProtocolMessageL(*msg);
		iActiveWait.Start(); // Wait for message on NRH channel
		}
	  return TestStepResult();
	}



TVerdict Clbsnetgatewaymultiprivacystep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// Close NRH, A-GPS, stub PM interfaces
	iStubPMChannelArray->Close();
	delete iNetGatewayChannel;
		
	// Uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();

	return TestStepResult();
	}

void Clbsnetgatewaymultiprivacystep::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	TEST(aChannelId == RLbsNetChannel::EChannelNRH2NG);
	
	switch (iState)
		{
		case EMultiPrivacyInitialRequest:
			{
			for (TInt i = 0; i < 10; i++)
				{
				TNetGatewayMsg* msg = CreatePrivacyRequestMsgLC(TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, (20001020 + i)));
				iMsgArray.AppendL(msg);
				CleanupStack::Pop(msg);
				iStubPMChannelArray->SendNetProtocolMessageL(*msg);
				}

			for (TInt i = 0; i < 10; i++)
				{
				TNetGatewayMsg* msg = CreateSessionCompleteMsgLC(TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, (20001020 + i)));
				iMsgArray.AppendL(msg);
				CleanupStack::Pop(msg);
				iStubPMChannelArray->SendNetProtocolMessageL(*msg);
				}
			
			iState = EMultiPrivacyCheckBuffer;
			// Intentional fall-through
			}
		case EMultiPrivacyCheckBuffer:
		default:
			{
			// messages should arrive here in the exact same order they were sent,
			// so check against the first message in the array.
			TEST(iMsgArray.Count() > 0);
			const TNetGatewayMsg* msg = iMsgArray[0];
			iMsgArray.Remove(0);
			TestMessagesAreEquivalentL(aMessage, *msg);
			delete msg;
			
			// If we have recieved all the expected messages, stop the 
			// active scheduler loop
			if (iMsgArray.Count() == 0)
				{
				if (iActiveWait.IsStarted())
					{
					iActiveWait.AsyncStop();	
					}
				}
			break;
			}
		}
	}
	
void Clbsnetgatewaymultiprivacystep::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
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
		default:
			{
			// Check that the incoming message carries the same information as
			// the message that was sent
			TestMessagesAreEquivalentL(*iNetInternalMessage, aMessage);			
			}
		}
	}

TNetGatewayMsg* Clbsnetgatewaymultiprivacystep::CreatePrivacyRequestMsgLC(const TLbsNetSessionId& aSessionId)
	{
	TNetGatewayMsg* msg = new (ELeave) TNetGatewayMsg;
	CleanupStack::PushL(msg);
	
	SNetMsgProcessPrivacyRequest* privReq = reinterpret_cast<SNetMsgProcessPrivacyRequest*>(msg->Data());
	new (privReq) SNetMsgProcessPrivacyRequest;
	msg->iType = ENetMsgProcessPrivacyRequest;
	privReq->iSessionId = aSessionId;
	privReq->iEmergency = EFalse;
	TLbsNetPosRequestPrivacy privacy;
	privacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
	privacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	privReq->iPrivacy = privacy;
	privReq->iRequestInfo.SetRequesterId(KRequesterId);
	privReq->iRequestInfo.SetClientName(KClientName);
	privReq->iRequestInfo.SetClientExternalId(KClientExternalId);
	
	return msg;
	}

TNetGatewayMsg* Clbsnetgatewaymultiprivacystep::CreateSessionCompleteMsgLC(const TLbsNetSessionId& aSessionId)
	{
	TNetGatewayMsg* msg = new (ELeave) TNetGatewayMsg;
	CleanupStack::PushL(msg);
	
	SNetMsgProcessSessionComplete* sessComp = reinterpret_cast<SNetMsgProcessSessionComplete*>(msg->Data());
	msg->iType = ENetMsgProcessSessionComplete;
	sessComp->iSessionId = aSessionId;
	sessComp->iReason = KErrNone;
	
	return msg;
	}


