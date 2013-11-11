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
//

/**
 @file Te_lbsprivfwinternaltestSuiteStepBase.cpp
*/

#include "te_lbsprivfwtestchannelteststepbase.h"
//#include "te_lbsprivfwinternaltestSuiteDefs.h"

// Device driver constants

TVerdict CTe_LbsPrivfwTestChannelTestStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	RLbsTestChannel::InitializeL(RLbsTestChannel::EChannelHandler2Proxy);
	RLbsTestChannel::InitializeL(RLbsTestChannel::EChannelProxy2Handler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsPrivfwTestChannelTestStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	
	RLbsTestChannel::ShutDownL(RLbsTestChannel::EChannelHandler2Proxy);
	RLbsTestChannel::ShutDownL(RLbsTestChannel::EChannelProxy2Handler);
	
	delete iScheduler;
	
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_LbsPrivfwTestChannelTestStepBase::~CTe_LbsPrivfwTestChannelTestStepBase()
	{
	}

CTe_LbsPrivfwTestChannelTestStepBase::CTe_LbsPrivfwTestChannelTestStepBase()
	{
	}

/** Logs the contents of the given TLbsprivfwinternalMsgBase to the test execute logger.
*/
//void CTe_LbsPrivfwTestChannelTestStepBase::LogMessageInfo(RLbsTestChannel::TLbsPrivFwChannelId aChannelId, const TLbsNetPrivFwMsgBase& aMessage)
//	{
//	switch (aChannelId)
//		{
//		case (RLbsTestChannel::EChannelHandler2Proxy):
//			INFO_PRINTF1(_L("Message Received on channel EChannelHandler2Proxy"));
//			break;
//			
//		case (RLbsTestChannel::EChannelProxy2Handler):
//			INFO_PRINTF1(_L("Message Received on channel EChannelProxy2Handler"));
//			break;
//			
//		default:
//			INFO_PRINTF2(_L("Message Received on an unknown channel - %d"), aChannelId);
//			SetTestStepResult(EFail);
//			break;
//		}
//		
//	INFO_PRINTF2(_L("Message Type (%d)"), aMessage.Type());
//	
//	switch (aMessage.Type())
//		{
//	
//		case TLbsNetLocationRequestMsg::EPrivacyResponse:
//			{
//			const TLbsNetLocationResponseMsg& msg = static_cast<const TLbsNetLocationResponseMsg&>(aMessage);
//			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
//												   msg.SessionId().SessionNum());
//
//			break;
//			}
//
//		case TLbsNetLocationRequestMsg::EPrivacyRequest:
//			{
//			const TLbsNetLocationRequestMsg& msg = static_cast<const TLbsNetLocationRequestMsg&>(aMessage);
//			INFO_PRINTF3(_L("SessionId (%d, %d)"), msg.SessionId().SessionOwner().iUid, 
//												   msg.SessionId().SessionNum());
//			
//			break;
//			}
//
//		default:
//			{
//			INFO_PRINTF1(_L("Unknown message type!"));
//			break;
//			}
//		}	
//	}

/** Default message handler for messages from the network gateway. Simply logs the message data.
*/
//void CTe_LbsPrivfwTestChannelTestStepBase::ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
//	{
//	INFO_PRINTF1(_L("CTe_LbsPrivfwTestChannelTestStepBase::ProcessChannelMessage : "));
//	//LogMessageInfo(aChannelId, aMessage);
//	CActiveScheduler::Stop();
//	}

void CTe_LbsPrivfwTestChannelTestStepBase::TestPrivFwTestChannelMessagesEquivalentL(
		const TLbsTestChannelMsgBase& aMsg, 	// Message recieved by callback
		const TLbsTestChannelMsgBase& aMsg2) 	// Copy of message originall sent
	{
	TESTL(aMsg.Type() == aMsg2.Type());
	
	switch (aMsg.Type())
		{
		case TLbsTestChannelMsgBase::EMessageNotifyRequest:
			{
			//const TLbsPrivacyRequestMsg& msg = static_cast<const TLbsPrivacyRequestMsg&>(aMsg);
			//const TLbsPrivacyRequestMsg& msg2 = static_cast<const TLbsPrivacyRequestMsg&>(aMsg2);
			TESTL(aMsg.RequestId() == aMsg2.RequestId());
			//msg.iExternalRequestInfo2->
			break;
			}
//		case TLbsTestChannelMsgBase::EPrivacyResponse:
//			{
//			const TLbsPrivacyResponseMsg& msg = static_cast<const TLbsPrivacyResponseMsg&>(aMsg);
//			const TLbsPrivacyResponseMsg& msg2 = static_cast<const TLbsPrivacyResponseMsg&>(aMsg2);
//			TESTL(msg.SessionId() == msg2.RequestId());
//			break;
//			}

		default:
			{
			break;
			}
		}
	}

