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
 @file lbsnetinternaltest11Step.cpp
*/
#include "lbsnetinternaltest11Step.h"
#include "Te_lbsnetinternaltestSuiteDefs.h"
#include "lbsnetinternalapi.h"

Clbsnetinternaltest11Step::~Clbsnetinternaltest11Step()
/**
 * Destructor
 */
	{
	}

Clbsnetinternaltest11Step::Clbsnetinternaltest11Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetinternaltest11Step);
	}

TVerdict Clbsnetinternaltest11Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 11 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetinternaltestSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetinternaltest11Step::doTestStepL()
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
		// EChannelNG2SUPLPSY pipe (channel) and checks that all the messages arrive 
		// at the other end at the EChannelSUPLPSY2NG channel.
		// Note that in real life, messages like TLbsCellLocationRequestMsg will
		// only be sent form the SUPLPSY to NG. But this test is actually just testing the interface and
		// so it is a valid thing to do!
		
		// messages will be sent across the netinternal api before being checked
		RLbsNetChannel netGatewayChannel;
		RLbsNetChannel suplPsyChannel;
		TRequestStatus status;

		netGatewayChannel.OpenL(RLbsNetChannel::EChannelSUPLPSY2NG, *this);
		CleanupClosePushL(netGatewayChannel);
				
		suplPsyChannel.OpenL(RLbsNetChannel::EChannelNG2SUPLPSY, *this);
		CleanupClosePushL(suplPsyChannel);
		
		//
		// TLbsCellLocationRequestMsg
		//
		iSentMsg = CreateTLbsCellLocationRequestMsgL();
		suplPsyChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
		
		//
		// TLbsCellLocationResponseMsg
		//
		iSentMsg = CreateTLbsCellLocationResponseMsgL();
		suplPsyChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
		
		
		// TLbsCellLocationCancelMsg
		//
		iSentMsg = CreateTLbsCellLocationCancelMsgL();
		suplPsyChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
		
		// End of test
		//		
		CleanupStack::PopAndDestroy(2, &netGatewayChannel);
		}
	  return TestStepResult();
	}



TVerdict Clbsnetinternaltest11Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	CTe_lbsnetinternaltestSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

/* Called when the message is received; check that the received message is the same as the one sent.
*/
void Clbsnetinternaltest11Step::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	TEST((aChannelId == RLbsNetChannel::EChannelSUPLPSY2NG) || (aChannelId == RLbsNetChannel::EChannelNG2SUPLPSY));
	TRAP_IGNORE(TestNetInternalMessagesEquivalentL(aMessage, *iSentMsg));
	CActiveScheduler::Stop();
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest11Step::CreateTLbsCellLocationRequestMsgL()
	{
	const TUint KProtocolModuleUidValue = 0x1028374B;
	const TUid KProtocolModuleUid = { KProtocolModuleUidValue };
	TLbsNetPosRequestOptionsInt options;
	options.SetNewClientConnected(ETrue);
	
	TLbsCellLocationRequestMsg* locReqMsg = new (ELeave) TLbsCellLocationRequestMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00000100), 
							 	   		KProtocolModuleUid, options);
	
	return locReqMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest11Step::CreateTLbsCellLocationResponseMsgL()
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
	TLbsCellLocationResponseMsg* cellLocRespMsg = new (ELeave) TLbsCellLocationResponseMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00000200),
										                                                   KErrNone,positionInfo);
	return cellLocRespMsg;
	}

TLbsNetInternalMsgBase* Clbsnetinternaltest11Step::CreateTLbsCellLocationCancelMsgL()
	{
	TLbsCellLocationCancelMsg* cellLocCancelMsg = new (ELeave) TLbsCellLocationCancelMsg(TLbsNetSessionIdInt(KTe_lbsnetinternaltestSuiteUid, 0x00000300),
														  KErrCancel);
	return cellLocCancelMsg;
	}





