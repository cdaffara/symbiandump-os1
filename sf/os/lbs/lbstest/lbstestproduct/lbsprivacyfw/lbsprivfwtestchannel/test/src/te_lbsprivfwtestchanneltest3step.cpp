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
 @file lbsprivfwinternaltest3Step.cpp
*/
#include "te_lbsprivfwtestchanneltest3step.h"
#include "tlbsprivfwtestchannel.h"
//#include "Te_lbsprivfwinternaltestSuiteDefs.h"

_LIT( KTxtRequestorId,	"OfficerBean" );
_LIT( KTxtClientExternalId,	"pc1234" );
_LIT( KTxtClientName,	"Police" );

CTe_LbsPrivfwTestChannelTest3Step::~CTe_LbsPrivfwTestChannelTest3Step()
/**
 * Destructor
 */
	{
	}

CTe_LbsPrivfwTestChannelTest3Step::CTe_LbsPrivfwTestChannelTest3Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsPrivfwTestChannelTest3Step);
	}

TVerdict CTe_LbsPrivfwTestChannelTest3Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_LbsPrivfwTestChannelTestStepBase::doTestStepPreambleL();

	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.

	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTe_LbsPrivfwTestChannelTest3Step::doTestStepL()
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

//		// messages will be sent across the netinternal api before being checked
		RLbsTestChannel netHandlerChannel;
		RLbsTestChannel netProxyChannel;
		TRequestStatus status;

		netHandlerChannel.OpenL(RLbsTestChannel::EChannelHandler2Proxy, *this);
		CleanupClosePushL(netHandlerChannel);
				
		netProxyChannel.OpenL(RLbsTestChannel::EChannelProxy2Handler, *this);
		CleanupClosePushL(netProxyChannel);
		
		//
		// TLbsNetLocationRequestMsg
		//
		iSentMsg = CreateTLbsPrivacyRequestMsgL();
		netHandlerChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;
	
						
		iSentMsg = CreateTLbsPrivacyResponseMsgL();
		netProxyChannel.SendMessage(*iSentMsg, status);
		CActiveScheduler::Start();
		User::WaitForRequest(status);
		delete iSentMsg;

		// 
		// End of test
		//		
		CleanupStack::PopAndDestroy(2, &netHandlerChannel);

//		CTestRequestChannel *proxyChannel;
//		CTestRequestChannel *handlerChannel;
//
//		proxyChannel = CTestRequestChannel::NewL(this);
//	//	handlerChannel = CTestRequestChannel::NewL(this);
//
//		TUint reqId = 10;
//		const TLbsPrivacyRequestMsg msg(reqId);
//		//msg.iTestRequestInfo->
//	
//		proxyChannel->SendPrivFwRequestMessage(msg);
		
		}
	  return TestStepResult();
	}

//void CTe_LbsPrivfwTestChannelTest3Step::ProcessTestChannelRequestMessage(const TLbsTestChannelMsgBase& aMessage)
//	{
//		
//	}

void CTe_LbsPrivfwTestChannelTest3Step::ProcessNetChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
	{
		
	}

TVerdict CTe_LbsPrivfwTestChannelTest3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	CTe_LbsPrivfwTestChannelTestStepBase::doTestStepPostambleL();
	
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}

/* Called when the message is received; check that the received message is the same as the one sent.
*/
void CTe_LbsPrivfwTestChannelTest3Step::ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
	{
	TEST((aChannelId == RLbsTestChannel::EChannelProxy2Handler) || (aChannelId == RLbsTestChannel::EChannelHandler2Proxy));
	TRAP_IGNORE(TestPrivFwTestChannelMessagesEquivalentL(aMessage, *iSentMsg));
	CActiveScheduler::Stop();
	}

TLbsTestChannelMsgBase* CTe_LbsPrivfwTestChannelTest3Step::CreateTLbsPrivacyRequestMsgL()
	{
	TUint requestId = 1;
	TLbsPrivFwNotifyRequestMsg* locReqMsg = new (ELeave) TLbsPrivFwNotifyRequestMsg(requestId);
//			
//	//locReqMsg->iExternalRequestInfo2->SetClientName(KTxtRequestorId);
//	//locReqMsg->iExternalRequestInfo2->SetClientExternalId(KTxtClientExternalId);
//	//locReqMsg->iExternalRequestInfo2->SetRequesterId(KTxtRequestorId);

	return locReqMsg;
//	return NULL;
	}


TLbsTestChannelMsgBase* CTe_LbsPrivfwTestChannelTest3Step::CreateTLbsPrivacyResponseMsgL()
	{
	TUint requestId = 1;
	TLbsPrivFwNotifyRequestMsg* locReqMsg = new (ELeave) TLbsPrivFwNotifyRequestMsg(requestId);
//	// initialise locreqmsg here

	return locReqMsg;
//	return NULL;
	}

