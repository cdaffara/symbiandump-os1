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
 @file lbsprivfwinternaltestchannelStep.cpp
*/
#include "te_lbsprivfwtestchanneltestchannelstep.h"
#include "tlbsprivfwtestchannel.h"
//#include "Te_lbsprivfwinternaltestSuiteDefs.h"

_LIT( KTxtRequestorId,	"OfficerBean" );
_LIT( KTxtClientExternalId,	"pc1234" );
_LIT( KTxtClientName,	"Police" );

CTe_LbsPrivfwTestChannelTestChannelStep::~CTe_LbsPrivfwTestChannelTestChannelStep()
/**
 * Destructor
 */
	{
	}

CTe_LbsPrivfwTestChannelTestChannelStep::CTe_LbsPrivfwTestChannelTestChannelStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsPrivfwTestChannelTestChannelStep);
	}

TVerdict CTe_LbsPrivfwTestChannelTestChannelStep::doTestStepPreambleL()
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


TVerdict CTe_LbsPrivfwTestChannelTestChannelStep::doTestStepL()
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

		CTestRequestChannel *proxyChannel;
		CTestRequestChannel *handlerChannel;

		proxyChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelHandler2Proxy);
		handlerChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelProxy2Handler);

		TUint reqId = 10;
		//const TLbsPrivFwNotifyRequestMsg msg(reqId);
		iSentMsg = new TLbsPrivFwNotifyRequestMsg(reqId);
	
		proxyChannel->SendPrivFwRequestMessage(*iSentMsg);
		
		CActiveScheduler::Start();
		
		}
	  return TestStepResult();
	}

//void CTe_LbsPrivfwTestChannelTestChannelStep::ProcessTestChannelRequestMessage(const TLbsTestChannelMsgBase& aMessage)
//	{
//	CActiveScheduler::Stop();
//	}



TVerdict CTe_LbsPrivfwTestChannelTestChannelStep::doTestStepPostambleL()
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
//void CTe_LbsPrivfwTestChannelTestChannelStep::ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
//	{
//	TEST((aChannelId == RLbsTestChannel::EChannelProxy2Handler) || (aChannelId == RLbsTestChannel::EChannelHandler2Proxy));
//	TRAP_IGNORE(TestPrivFwTestChannelMessagesEquivalentL(aMessage, *iSentMsg));
//	CActiveScheduler::Stop();
//	}

void CTe_LbsPrivfwTestChannelTestChannelStep::ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
	{
	TEST((aChannelId == RLbsTestChannel::EChannelProxy2Handler) || (aChannelId == RLbsTestChannel::EChannelHandler2Proxy));
	TRAP_IGNORE(TestPrivFwTestChannelMessagesEquivalentL(aMessage, *iSentMsg));
	CActiveScheduler::Stop();
	}

TLbsTestChannelMsgBase* CTe_LbsPrivfwTestChannelTestChannelStep::CreateTLbsPrivacyRequestMsgL()
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


TLbsTestChannelMsgBase* CTe_LbsPrivfwTestChannelTestChannelStep::CreateTLbsPrivacyResponseMsgL()
	{
	TUint requestId = 1;
	TLbsPrivFwNotifyRequestMsg* locReqMsg = new (ELeave) TLbsPrivFwNotifyRequestMsg(requestId);
//	// initialise locreqmsg here

	return locReqMsg;
//	return NULL;
	}

