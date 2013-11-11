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
 @file lbsnetgateway3Step.cpp
*/

#include <e32debug.h>

#include "lbsnetgateway3Step.h"
#include "Te_lbsnetgatewaySuiteDefs.h"

Clbsnetgateway3Step::~Clbsnetgateway3Step()
/**
 * Destructor
 */
	{
	}

Clbsnetgateway3Step::Clbsnetgateway3Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgateway3Step);
	}

TVerdict Clbsnetgateway3Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Clbsnetgateway3Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	
	// Connect to StubPM and NLM interfaces
	// Connect to NLM, A-GPS, stub PM interfaces
	iNetworkLocationChannel = CNetworkLocationChannel::NewL(*this);
	iNetGatewayChannel = CNetGatewayChannel::NewL(*this, RLbsNetChannel::EChannelNRH2NG);

    InitialiseUidArray();
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;
    iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
	iReferencePositionUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgateway3Step::doTestStepL()
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
		// Tests to exercise the functionality of the 
		// Network Location Manager Channel interface
		// in the Network Gateway.
		//
		// Tests work by sending a message through the
		// test protocol module and looking for the 
		// correct message/data being sent to the 
		// Network Location Manager. And vice-versa, to
		// test all of the functions in the interface.
		//

		// 
		// Test 1) Network Location Request
		//
		TLbsNetPosRequestQualityInt quality;
		iNetLocMsg = new (ELeave) TLbsNetLocNetworkLocationRequestMsg(
									TLbsNetSessionIdInt(KLbsNetLocManagerUid, 0x10000001),
									ETrue, 
									quality);
		CleanupDeletePushL(iNetLocMsg);
		User::LeaveIfError(iNetworkLocationChannel->SendNetworkLocationMessage(*iNetLocMsg));
		iSchedWait->Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		CleanupStack::PopAndDestroy(iNetLocMsg);
		
		//
		// Test 2) Network Location Cancel
		//
		iNetLocMsg = new (ELeave) TLbsNetLocNetworkLocationCancelMsg(
									TLbsNetSessionIdInt(KLbsNetLocManagerUid, 0x10000001),
									KErrCancel);
		CleanupDeletePushL(iNetLocMsg);
		User::LeaveIfError(iNetworkLocationChannel->SendNetworkLocationMessage(*iNetLocMsg));
		iSchedWait->Start();
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		CleanupStack::PopAndDestroy(iNetLocMsg);
		

		//
		// Test 3) Network Location Complete
		//
		// Note: Expect two 'session complete' messages,
		//       one on the network location message bus and
		//       one on the network request message bus, because
		//       the session complete always gets sent to the NRH,
		//       and is echoed to the Network Location Manager.
		//
		
		iNetworkLocationResponse = EFalse;
		iNetChannelMessage = EFalse;
		
		SNetMsgProcessSessionComplete comp;
		comp.iSessionId = TLbsNetSessionId(KLbsNetLocManagerUid, 0x10000001);
		comp.iReason = KErrTimedOut;
		iTestPmMsg.iType = ENetMsgProcessSessionComplete;
		Mem::Copy(iTestPmMsg.Data(), &comp, sizeof(SNetMsgProcessSessionComplete));
		iStubPMChannelArray->SendNetProtocolMessageL(iTestPmMsg);
		if(!iNetworkLocationResponse || !iNetChannelMessage)
			{
			iSchedWait->Start();
			}
		if(!iNetworkLocationResponse || !iNetChannelMessage)
			{
			iSchedWait->Start();
			}
		
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		
		//
		// Test 4) Publishing Reference location sent by network
		//
		iNetworkLocationResponse = EFalse;
		iNetChannelMessage = EFalse;

		SNetMsgProcessLocationUpdate netLoc;
		TPosition position;
		position.SetCoordinate(TReal64(36.756), TReal64(123.546), TReal32(74.263));
		position.SetAccuracy(TReal32(0.5), TReal32(0.5));
		position.SetCurrentTime();
		netLoc.iReferenceLocation.SetPosition(position);		
		iTestPmMsg.iType = ENetMsgProcessLocationUpdate;
		Mem::Copy(iTestPmMsg.Data(), &netLoc, sizeof(SNetMsgProcessLocationUpdate));
		iStubPMChannelArray->SendNetProtocolMessageL(iTestPmMsg);

		// Check that the reference position is sent to the NRH
		if(!iNetworkLocationResponse || !iNetChannelMessage)
			{
			iSchedWait->Start();
			}
		if (EFail == TestStepResult())
			{
			return TestStepResult();
			}
		}
	  return TestStepResult();
	}



TVerdict Clbsnetgateway3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step	
	
	// Disconnect from StubPM and NLM interfaces
	iReferencePositionUpdates.Close();	
	iStubPMChannelArray->Close();
	delete iNetworkLocationChannel;
	delete 	iNetGatewayChannel;
	
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

void Clbsnetgateway3Step::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
	{
	TRAP_IGNORE(TestMessagesAreEquivalentL(*iNetLocMsg, aMessage));
	if(iSchedWait->IsStarted())
		{
		iSchedWait->AsyncStop();
		}
	}
	
void Clbsnetgateway3Step::ProcessNetworkLocationResponseMessage(const TLbsNetLocMsgBase& aMessage)
	{
	TRAP_IGNORE(TestMessagesAreEquivalentL(aMessage, iTestPmMsg));
	if(iSchedWait->IsStarted())
		{
		iSchedWait->AsyncStop();
		}
	iNetworkLocationResponse = ETrue;
	}

void Clbsnetgateway3Step::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	TEST(aChannelId == RLbsNetChannel::EChannelNRH2NG);
	TRAP_IGNORE(TestMessagesAreEquivalentL(aMessage, iTestPmMsg));
	if(iSchedWait->IsStarted())
		{
		iSchedWait->AsyncStop();
		}
	iNetChannelMessage = ETrue;
	}


