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
 @file lbsnetgateway4Step.cpp
*/
#include "lbsnetgateway4Step.h"
#include "Te_lbsnetgatewaySuiteDefs.h"

		
Clbsnetgateway4Step::~Clbsnetgateway4Step()
/**
 * Destructor
 */
	{
	}

Clbsnetgateway4Step::Clbsnetgateway4Step() :
	iTestStep(ETestStepNotUsed),
	iSessionId(KTe_LbsNetGatewayTestProtocolUid, 1000)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgateway4Step);
	}

TVerdict Clbsnetgateway4Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Clbsnetgateway4Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
	// Connect to NRH, A-GPS, stub PM interfaces
	iNetGatewayChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);

	InitialiseUidArray();
    
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;
    iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgateway4Step::doTestStepL()
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
		// Stress-test the interface between the Network Gateway
		// and the Network Request Handler. The interface between
		// them only allows for one pending message at a time, so 
		// if the network protocol module floods the Network Gateway
		// with requests it should handle it gracefully.
		// 
		// Note: this test will eventually check that the Network
		// Gateway will always pass on relevant messages to the NRH,
		// for example in the case of an emergency request.
		
		// Pre-set the messages used
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1000);
		iLocReq.iEmergency = EFalse;
		iLocReq.iService = MLbsNetworkProtocolObserver::EServiceNetworkLocation;
		iLocReq.iQuality = TLbsNetPosRequestQuality();
		iLocReq.iMethod = TLbsNetPosRequestMethod();
		
		iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1001);
		iPrivReq.iEmergency = EFalse;
		TLbsNetPosRequestPrivacy privacy;
		privacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
		privacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
		iPrivReq.iPrivacy = privacy;
		_LIT8(KRequesterId, "yourmum");
		_LIT8(KClientName, "sharon");
		_LIT8(KClientExternalId, "07697456324");
		iPrivReq.iRequestInfo.SetRequesterId(KRequesterId);
		iPrivReq.iRequestInfo.SetClientName(KClientName);
		iPrivReq.iRequestInfo.SetClientExternalId(KClientExternalId);
		
		iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1002);
		iSessComp.iReason = KErrNone;
		
		//
		// 1) : Check that a privacy request is buffered correctly.
		//
		
		//
		// 1.1) : Buffer is empty
		//
		INFO_PRINTF1(_L("Step: 1.1) : Buffer is empty"));
		
		iTestStep = ETestStepLocReqBufferEmpty;
		
		// Initial message to fill up the LbsNetInternalApi channel
		iMsg.iType = ENetMsgProcessLocationRequest;
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1000);
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		
		//
		// 1.2) : Buffer contains a previous privacy request
		//
		INFO_PRINTF1(_L("Step: 1.2) : Buffer contains a previous privacy request"));
		iTestStep = ETestStepLocReqPendingPrivReq;
		
		// message to fill the channel up
		iMsg.iType = ENetMsgProcessLocationRequest;
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1002);
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
				
		//
		// 1.3) : Buffer contains a previous location request
		//
		INFO_PRINTF1(_L("Step: 1.3) : Buffer contains a previous location request"));
		iTestStep = ETestStepLocReqPendingLocReq;
		
		// message to fill the channel up
		iMsg.iType = ENetMsgProcessLocationRequest;
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1005);
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}

		//
		// 1.4) : Buffer contains a previous cancel request
		//
		INFO_PRINTF1(_L("Step: 1.4) : Buffer contains a previous cancel request"));
		iTestStep = ETestStepLocReqPendingCancel;
		
		// message to fill the channel up
		iMsg.iType = ENetMsgProcessLocationRequest;
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1008);
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		
		//
		// 1.5) : Buffer contains a matching privacy and location request
		//
		INFO_PRINTF1(_L("Step: 1.5) : Buffer contains a matching privacy and location request"));
		iTestStep = ETestStepLocReqPendingPrivReqAndLocReq;
		
		// message to fill the channel up
		iMsg.iType = ENetMsgProcessLocationRequest;
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1011);
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}		
		
		//
		// 2) : Check that a location request is buffered correctly.
		//

		//
		// 2.1) : Buffer is empty
		//
		INFO_PRINTF1(_L("Step: 2.1) : Buffer is empty"));
		iTestStep = ETestStepPrivReqBufferEmpty;
		
		// Initial message to fill up the LbsNetInternalApi channel
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1020);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		
		//
		// 2.2) : Buffer contains a previous privacy request
		//
		INFO_PRINTF1(_L("Step: 2.2) : Buffer contains a previous privacy request"));
		iTestStep = ETestStepPrivReqPendingPrivReq;
		
		// message to fill the channel up
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1022);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
				
		//
		// 2.3) : Buffer contains a previous location request
		//
		INFO_PRINTF1(_L("Step: 2.3) : Buffer contains a previous location request"));
		iTestStep = ETestStepPrivReqPendingLocReq;
		
		// message to fill the channel up
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1025);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}

		//
		// 2.4) : Buffer contains a previous cancel request
		//
		INFO_PRINTF1(_L("Step: 2.4) : Buffer contains a previous cancel request"));
		iTestStep = ETestStepPrivReqPendingCancel;
		
		// message to fill the channel up
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1028);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		
		//
		// 2.5) : Buffer contains a matching privacy and location request
		//
		INFO_PRINTF1(_L("Step: 2.5) : Buffer contains a matching privacy and location request"));
		iTestStep = ETestStepPrivReqPendingPrivReqAndLocReq;
		
		// message to fill the channel up
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1031);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}

		//   
		// 3) : Check that a cancel request is buffered correctly.
		//

		//
		// 3.1) : Buffer is empty
		//
		INFO_PRINTF1(_L("Step: 3.1) : Buffer is empty"));
		iTestStep = ETestStepSessCompBufferEmpty;
		
		// Initial message to fill up the LbsNetInternalApi channel
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1040);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		
		//
		// 3.2) : Buffer contains a previous privacy request
		//
		INFO_PRINTF1(_L("Step: 3.2) : Buffer contains a previous privacy request"));
		iTestStep = ETestStepSessCompPendingPrivReq;
		
		// message to fill the channel up
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1042);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
				
		//
		// 3.3) : Buffer contains a previous location request
		//
		INFO_PRINTF1(_L("Step: 3.3) : Buffer contains a previous location request"));
		iTestStep = ETestStepSessCompPendingLocReq;
		
		// message to fill the channel up
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1045);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}

		//
		// 3.4) : Buffer contains a previous cancel request
		//
		INFO_PRINTF1(_L("Step: 3.4) : Buffer contains a previous cancel request"));
		iTestStep = ETestStepSessCompPendingCancel;
		
		// message to fill the channel up
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1048);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		
		//
		// 3.5) : Buffer contains a matching privacy and location request
		//
		INFO_PRINTF1(_L("Step: 3.5) : Buffer contains a matching privacy and location request"));
		iTestStep = ETestStepSessCompPendingPrivReqAndLocReq;
		
		// message to fill the channel up
		iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1051);
		iMsg.iType = ENetMsgProcessLocationRequest;
		Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
		iExpectedMessages[iNumExpectedMessages++] = iMsg;
		iStubPMChannelArray->SendNetProtocolMessageL(iMsg);
		CActiveScheduler::Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}

		}
	  return TestStepResult();
	}



TVerdict Clbsnetgateway4Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Clbsnetgateway4Step"));
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

void Clbsnetgateway4Step::FinishTestStep()
	{
	iNumExpectedMessages = 0;
	iNumReceivedMessages = 0;		
	CActiveScheduler::Stop();	
	}

/* Check that incoming messages are the ones we expected to get.
*/
void Clbsnetgateway4Step::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	TEST(aChannelId == RLbsNetChannel::EChannelNRH2NG);
	// compare the recieved message to the one we expected to get
	if (iNumExpectedMessages > 0)
		{
		TRAPD(err, TestMessagesAreEquivalentL(aMessage, iExpectedMessages[iNumReceivedMessages++]));
		
		if(err!=KErrNone)
			{
			User::Panic(Klbsnetgateway4Step, err);
			}
		
		if (EFail == TestStepResult())
			{
			iTestStep = ETestStepNotUsed;
			FinishTestStep();
			return;		
			}		
		}
	
	TInt err = KErrNone;
	if (iNumReceivedMessages == iNumExpectedMessages)
		{
		switch (iTestStep)
			{
			case ETestStepLocReqBufferEmpty:
				{
				// privacy request to go in the buffer
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1001);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
			
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepLocReqPendingPrivReq:
				{		
				// privacy message to go in the buffer
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1003);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAPD(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg));
				if(err!=KErrNone)
					{
					User::Panic(Klbsnetgateway4Step, err);
					}
				
				// second privacy request to go into the buffer		
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1004);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepLocReqPendingLocReq:
				{
				// location message to go in the buffer; this should still be present
				// even when the privacy request below has been sent.
				iMsg.iType = ENetMsgProcessLocationRequest;
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1006);
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// new privacy request				
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1007);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepLocReqPendingCancel:
				{
				// cancel/complete message to go in the buffer
				iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1009);
				iSessComp.iReason = KErrNone;
				iMsg.iType = ENetMsgProcessSessionComplete;
				Mem::Copy(iMsg.Data(), &iSessComp, sizeof(SNetMsgProcessSessionComplete));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// new privacy request		
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1010);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepLocReqPendingPrivReqAndLocReq:
				{
				// All of the following requests should go into the buffer and be received by the NRH
				
				// privacy message to go in the buffer
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1012);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);

				// location message to go in the buffer
				iMsg.iType = ENetMsgProcessLocationRequest;
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1012);
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// privacy request to go into the buffer
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1013);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepPrivReqBufferEmpty:
				{
				// New location request
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1021);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepPrivReqPendingPrivReq:
				{
				// privacy message to go in the buffer
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1023);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// location request should go into the buffer, but not overwrite
				// the privacy request which is already in there.
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1023);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepPrivReqPendingLocReq:
				{
				// location message to go in the buffer; this should not be removed
				// from the buffer when the location request below is sent.		
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1026);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// new location request
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1027);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepPrivReqPendingCancel:
				{
				// cancel/complete message to go in the buffer
				iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1029);
				iMsg.iType = ENetMsgProcessSessionComplete;
				Mem::Copy(iMsg.Data(), &iSessComp, sizeof(SNetMsgProcessSessionComplete));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// new location request		
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1030);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepPrivReqPendingPrivReqAndLocReq:
				{
				// All of the following requests should go into the buffer and be received by the NRH
				
				// privacy message to go in the buffer
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1032);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);

				// location message to go in the buffer
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1032);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// new location request to go in the buffer
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1033);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepSessCompBufferEmpty:
				{
				// New session complete
				iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1041);
				iMsg.iType = ENetMsgProcessSessionComplete;
				Mem::Copy(iMsg.Data(), &iSessComp, sizeof(SNetMsgProcessSessionComplete));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepSessCompPendingPrivReq:
				{
				// privacy message to go in the buffer
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1043);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// session complete should not clear out itself 
				// and the corresponding privacy request.
				iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1043);
				iMsg.iType = ENetMsgProcessSessionComplete;
				Mem::Copy(iMsg.Data(), &iSessComp, sizeof(SNetMsgProcessSessionComplete));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepSessCompPendingLocReq:
				{
				// location message to go in the buffer; this should not be removed
				// from the buffer when the session complete below is sent.		
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1046);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// session complete
				iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1046);
				iMsg.iType = ENetMsgProcessSessionComplete;
				Mem::Copy(iMsg.Data(), &iSessComp, sizeof(SNetMsgProcessSessionComplete));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepSessCompPendingCancel:
				{
				// cancel/complete message to go in the buffer
				iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1049);
				iMsg.iType = ENetMsgProcessSessionComplete;
				Mem::Copy(iMsg.Data(), &iSessComp, sizeof(SNetMsgProcessSessionComplete));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// new session complete		
				iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1050);
				iMsg.iType = ENetMsgProcessSessionComplete;
				Mem::Copy(iMsg.Data(), &iSessComp, sizeof(SNetMsgProcessSessionComplete));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepSessCompPendingPrivReqAndLocReq:
				{
				// privacy message to go in the buffer
				iPrivReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1052);
				iMsg.iType = ENetMsgProcessPrivacyRequest;
				Mem::Copy(iMsg.Data(), &iPrivReq, sizeof(SNetMsgProcessPrivacyRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);

				// location message to go in the buffer
				iLocReq.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1052);
				iMsg.iType = ENetMsgProcessLocationRequest;
				Mem::Copy(iMsg.Data(), &iLocReq, sizeof(SNetMsgProcessLocationRequest));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				
				// new session complete
				iSessComp.iSessionId = TLbsNetSessionId(KTe_LbsNetGatewayTestProtocolUid, 1052);
				iMsg.iType = ENetMsgProcessSessionComplete;
				Mem::Copy(iMsg.Data(), &iSessComp, sizeof(SNetMsgProcessSessionComplete));
				iExpectedMessages[iNumExpectedMessages++] = iMsg;
				TRAP(err, iStubPMChannelArray->SendNetProtocolMessageL(iMsg);)
				if(err)
					User::Panic(Klbsnetgateway4Step, err);
				iTestStep = ETestStepNotUsed;
				break;
				}
			case ETestStepNotUsed:
			default:
				{
				break;
				}
			}

			// If no new messages need to be waited for then finish
			// the test step.
			if (iNumReceivedMessages == iNumExpectedMessages)
				{
				FinishTestStep();
				}		
		}
	}


