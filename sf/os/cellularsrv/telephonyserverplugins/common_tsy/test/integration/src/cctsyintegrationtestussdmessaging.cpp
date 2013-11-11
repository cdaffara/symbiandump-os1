// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test step definitions for the UssdMessaging functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestussdmessaging.h"
#include "cetelsessionmgr.h"



CCTSYIntegrationTestUssdMessagingBase::CCTSYIntegrationTestUssdMessagingBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iSmsStoreTsyTestHelper(*this),iNetworkTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestUssdMessagingBase::~CCTSYIntegrationTestUssdMessagingBase()
/*
 * Destructor
 */
	{
	}


TInt CCTSYIntegrationTestUssdMessagingBase::SimulateNetworkSendsReleaseCompleteL()
/**
 * Helper function
 *
 * Simulate network sends a release complete message. Not implemented!
 *
 *
 * @return KErrNone if everything is OK, the error code in any other case
 */
	{
    // This function should call whatever APIs are required to tell the simulator to simulate network sends a release complete message
	DisplayUserInteractionPromptL(_L("Now we should simulate a Request Complete message. Press any key to continue."), ETimeLong);
	return KErrNone;
	}

TInt CCTSYIntegrationTestUssdMessagingBase::SimulateNetworkSendsNotifyMessageL(RMobileUssdMessaging::TMobileUssdMessageType /*aType*/, TInt /*aLength*/)
/**
 * Helper function
 *
 * Simulate network sends a USSD notify message. Not implemented!
 *
 * @param aType The message type
 * @param aLength The number of charecters in the messege
 *
 * @return KErrNone if everything is OK, the error code in any other case
 */
	{
    // This function should call whatever APIs are required to tell the simulator to simulate network sends a USSD notify message
	DisplayUserInteractionPromptL(_L("Now we should simulate a Notify message. Press any key to continue."), ETimeLong);
	return KErrNone;
	}

TInt CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL(RMobileUssdMessaging &aUssdMessaging, TDes8& aMsg,RMobileUssdMessaging::TMobileUssdMessageType aMessageType)
/**
 * Send a message, Simulate network complete, Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length
 *
 * @param aUssdMessaging a reference to the USSD Messaging session.
 * @param aMsg The message USSD string
 * @param aMessageType The type of message; if type is EUssdMORequest, simulate requestcomplete, otherwise, simulate
 *
 * @return KErrNone if everything is OK, the error code in any other case
 */
	{
	TInt ret=KErrNone;
	TInt last;

    // start listening for incoming messages
	TExtEtelRequestStatus recvMsgStatus(aUssdMessaging,EMobileUssdMessagingReceiveMessage);
	CleanupStack::PushL(recvMsgStatus);
	RMobileUssdMessaging::TGsmUssdMessageData recvUssdMsg;
	RMobileUssdMessaging::TMobileUssdAttributesV1 recvAttributes;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg recvAttributesPckg(recvAttributes);
	aUssdMessaging.ReceiveMessage(recvMsgStatus,recvUssdMsg,recvAttributesPckg);

	// Encode USSD string into a TGsmUssdMessageData buffer.
	TBuf8<200> sevenBitStr;
	iSmsStoreTsyTestHelper.GsmLibSmsPackMessage( sevenBitStr, aMsg, aMsg.Length());
	RMobileUssdMessaging::TGsmUssdMessageData sendUssdMsg = sevenBitStr;

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=encoded TGsmUssdMessageData and  aMsgAttributes.iDcs= 0x00 (No class, uncompressed) ,aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=aMessageType
	TExtEtelRequestStatus sendMsgStatus(aUssdMessaging,EMobileUssdMessagingSendMessage);
	CleanupStack::PushL(sendMsgStatus);
	RMobileUssdMessaging::TMobileUssdAttributesV1 sendAttributes;
	sendAttributes.iDcs=0x00;
	sendAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs;
	sendAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString;
	sendAttributes.iType=aMessageType;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttributesPckg(sendAttributes);
	aUssdMessaging.SendMessage(sendMsgStatus, sendUssdMsg, sendAttributesPckg);
    last = WaitForRequestWithTimeOut(sendMsgStatus, ETimeLong);
	ASSERT_EQUALS(last,
			KErrNone, _L("RMobileUssdMessaging::SendMessage did not complete."));
	if(last!=KErrNone)
		{
		ret=last;
		}
    last = sendMsgStatus.Int();
    ASSERT_EQUALS(last,KErrNone,
    		_L("RMobileUssdMessaging::SendMessage completed with incorrect error"));
	if(last!=KErrNone)
		{
		ret=last;
		}

    // Post network release notification
    TExtEtelRequestStatus releaseMsgStatus(aUssdMessaging,EMobileUssdMessagingNotifyNetworkRelease);
	CleanupStack::PushL(releaseMsgStatus);
	RMobilePhone::TMobilePhoneSendSSRequestV3 ssRequest;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg ssRequestPckg(ssRequest);
	aUssdMessaging.NotifyNetworkRelease(releaseMsgStatus,ssRequestPckg,sendAttributesPckg);

	// Simulated network sends a release complete
    last = SimulateNetworkSendsReleaseCompleteL();
    ASSERT_EQUALS(last,KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNetworkSendsReleaseCompleteL returned an error"));
	if(last!=KErrNone)
		{
		ret=last;
		}

    // Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
    last = WaitForRequestWithTimeOut(releaseMsgStatus, ETimeLong);
	ASSERT_EQUALS(last,
			KErrNone, _L("RMobileUssdMessaging::NotifyNetworkRelease did not complete."));
	if(last!=KErrNone)
		{
		ret=last;
		}
	ASSERT_TRUE(recvUssdMsg.Length()>0,_L("RMobileUssdMessaging::NotifyNetworkRelease completed with length 0"));
	if(recvUssdMsg.Length()==0)
    	{
    	ret = KErrGeneral;
    	}

	// Pop:
	//	releaseMsgStatus
	//	sendMsgStatus
	//	recvMsgStatus
	CleanupStack::PopAndDestroy(3,&recvMsgStatus);

    return ret;
	}

TInt CCTSYIntegrationTestUssdMessagingBase::SendSimulateNotifyCheckCompleteL(RMobileUssdMessaging &aUssdMessaging, TDes8& aMsg,RMobileUssdMessaging::TMobileUssdMessageType aMessageType,RMobileUssdMessaging::TMobileUssdMessageType aSimulateType, TInt aLength)
/**
 * Send a message, Simulate network notify, Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length
 *
 * @param aUssdMessaging a reference to the USSD Messaging session.
 * @param aMsg The message USSD string
 * @param aMessageType The type of message; if type is EUssdMORequest, simulate requestcomplete, otherwise, simulate
 * @param aSimulateType the simulated message type
 * @param aLength the length of the simulated message.
 *
 * @return KErrNone if everything is OK, the error code in any other case
 */
	{
	TInt ret=KErrNone;
	TInt last;

    // start listening for incoming messages
	TExtEtelRequestStatus recvMsgStatus(aUssdMessaging,EMobileUssdMessagingReceiveMessage);
	CleanupStack::PushL(recvMsgStatus);
	RMobileUssdMessaging::TGsmUssdMessageData recvUssdMsg;
	RMobileUssdMessaging::TMobileUssdAttributesV1 recvAttributes;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg recvAttributesPckg(recvAttributes);
	aUssdMessaging.ReceiveMessage(recvMsgStatus,recvUssdMsg,recvAttributesPckg);

	// Encode USSD string into a TGsmUssdMessageData buffer.
	TBuf8<200> sevenBitStr;
	iSmsStoreTsyTestHelper.GsmLibSmsPackMessage( sevenBitStr, aMsg, aMsg.Length());
	RMobileUssdMessaging::TGsmUssdMessageData sendUssdMsg = sevenBitStr;

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=encoded TGsmUssdMessageData and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=aMessageType
	TExtEtelRequestStatus sendMsgStatus(aUssdMessaging,EMobileUssdMessagingSendMessage);
	CleanupStack::PushL(sendMsgStatus);
	RMobileUssdMessaging::TMobileUssdAttributesV1 sendAttributes;
	sendAttributes.iDcs=0x00;
	sendAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs;
	sendAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString;
	sendAttributes.iType=aMessageType;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttributesPckg(sendAttributes);
	aUssdMessaging.SendMessage(sendMsgStatus, sendUssdMsg, sendAttributesPckg);
    last = WaitForRequestWithTimeOut(sendMsgStatus, ETimeLong);
	ASSERT_EQUALS(last,
			KErrNone, _L("RMobileUssdMessaging::SendMessage did not complete."));
	if(last!=KErrNone)
		{
		ret=last;
		}
    last = sendMsgStatus.Int();
    ASSERT_EQUALS(last,KErrNone,
    		_L("RMobileUssdMessaging::SendMessage completed with incorrect error"));
	if(last!=KErrNone)
		{
		ret=last;
		}

    // Post network release notification
    TExtEtelRequestStatus releaseMsgStatus(aUssdMessaging,EMobileUssdMessagingNotifyNetworkRelease);
	CleanupStack::PushL(releaseMsgStatus);
	RMobilePhone::TMobilePhoneSendSSRequestV3 ssRequest;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg ssRequestPckg(ssRequest);
	aUssdMessaging.NotifyNetworkRelease(releaseMsgStatus,ssRequestPckg,sendAttributesPckg);

	// Simulated network sends a release complete
    last = SimulateNetworkSendsNotifyMessageL(aSimulateType,aLength);
    ASSERT_EQUALS(last,KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNetworkSendsReleaseCompleteL returned an error"));
	if(last!=KErrNone)
		{
		ret=last;
		}

    // Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
    last = WaitForRequestWithTimeOut(releaseMsgStatus, ETimeLong);
	ASSERT_EQUALS(last,
			KErrNone, _L("RMobileUssdMessaging::NotifyNetworkRelease did not complete."));
	if(last!=KErrNone)
		{
		ret=last;
		}
    last = releaseMsgStatus.Int();
	ASSERT_EQUALS(last,
			KErrNone, _L("RMobileUssdMessaging::NotifyNetworkRelease returned an error"));
	if(last!=KErrNone)
		{
		ret=last;
		}
    ASSERT_EQUALS(recvAttributes.iType,aSimulateType,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with incorrect USSD message type"));
    ASSERT_TRUE(recvUssdMsg.Length() > 0,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with msglength <=0"));
    if((recvUssdMsg.Length()==0)||(recvAttributes.iType!=aSimulateType))
    	{
    	ret = KErrGeneral;
    	}

	// Pop:
	//	releaseMsgStatus
	//	sendMsgStatus
	//	recvMsgStatus
	CleanupStack::PopAndDestroy(3,&recvMsgStatus);

    return ret;

	}

TInt CCTSYIntegrationTestUssdMessagingBase::SimulateNotifyCheckReceiveL(RMobileUssdMessaging &aUssdMessaging,RMobileUssdMessaging::TMobileUssdMessageType aSimulateType, TInt aLength)
/**
 * Simulated network sends a USSD notify message, Receive MT notify from network, Check received notification
 *
 * @param aUssdMessaging a reference to the USSD Messaging session.
 * @param aSimulateType the simulated message type
 * @param aLength the length of the simulated message.
 * @param aCheckType the type of the recived notification
 * @return KErrNone if everything is OK, the error code in any other case
 */
	{
	TInt ret=KErrNone;
	TInt last;

    // start listening for incoming messages
	TExtEtelRequestStatus recvMsgStatus(aUssdMessaging,EMobileUssdMessagingReceiveMessage);
	CleanupStack::PushL(recvMsgStatus);
	RMobileUssdMessaging::TGsmUssdMessageData recvUssdMsg;
	RMobileUssdMessaging::TMobileUssdAttributesV1 recvAttributes;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg recvAttributesPckg(recvAttributes);
	aUssdMessaging.ReceiveMessage(recvMsgStatus,recvUssdMsg,recvAttributesPckg);

	// Simulated network sends a USSD notify message.
    last = SimulateNetworkSendsNotifyMessageL(aSimulateType,aLength);
    ASSERT_EQUALS(last,KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNetworkSendsNotifyMessageL returned an error"));
	if(last!=KErrNone)
		{
		ret=last;
		}

	// ===  Receive MT notify from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=aCheckType
    last = WaitForRequestWithTimeOut(recvMsgStatus, ETimeLong);
	ASSERT_EQUALS(last,
			KErrNone, _L("RMobileUssdMessaging::ReceiveMessage did not complete."))	;
	if(last!=KErrNone)
		{
		ret=last;
		}
    last = recvMsgStatus.Int();
    ASSERT_EQUALS(last,KErrNone,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with incorrect error"));
	if(last!=KErrNone)
		{
		ret=last;
		}
    ASSERT_EQUALS(recvAttributes.iType,aSimulateType,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with incorrect USSD message type"));
    ASSERT_TRUE(recvUssdMsg.Length() > 0,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with msglength <=0"));
    if((recvUssdMsg.Length()==0)||(recvAttributes.iType!=aSimulateType))
    	{
    	ret = KErrGeneral;
    	}

    // Pop:
	//	recvMsgStatus
	CleanupStack::PopAndDestroy(1,&recvMsgStatus);

    return ret;

	}
CCTSYIntegrationTestUssdMessaging0001::CCTSYIntegrationTestUssdMessaging0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0001::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0001::~CCTSYIntegrationTestUssdMessaging0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0001
 * @SYMFssID BA/CTSY/USSDM-0001
 * @SYMTestCaseDesc Get USSD Caps
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::GetCaps
 * @SYMTestExpectedResults Pass - Correct caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify correct USSD caps returned.
 *
 * @return - TVerdict code
 */
	{

	//Get USSD Caps
	//Verify correct USSD caps returned.
	//
	// SET UP
	//
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);


	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	ASSERT_EQUALS(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone, _L("RMobileUssdMessaging::GetCaps returned error"));
    ASSERT_EQUALS(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps =  RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	TUint32 unwantedCaps = KNoUnwantedBits;
	ASSERT_BITS_SET(ussdCapsPk().iUssdTypes, expectedCaps, unwantedCaps, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));

	//
	// TEST END
	//

    StartCleanup();


	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0001");
	}



CCTSYIntegrationTestUssdMessaging0002::CCTSYIntegrationTestUssdMessaging0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0002::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0002::~CCTSYIntegrationTestUssdMessaging0002()
/**
 * Destructor.
 */
	{
	}


TVerdict CCTSYIntegrationTestUssdMessaging0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0002
 * @SYMFssID BA/CTSY/USSDM-0002
 * @SYMTestCaseDesc Send a valid USSD request.
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::GetCaps, RMobileUssdMessaging::SendMessage, RMobileUssdMessaging::ReceiveMessage
 * @SYMTestExpectedResults Pass - Message sent and reply received from network.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify message sent and reply received.
 *
 * @return - TVerdict code
 */
	{
	//Send a valid USSD request.
	//Verify message sent and reply received..
	//
	// SET UP
	//

	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_TRUE_L(mobileUssdMessaging.GetCaps(ussdCapsPk)==KErrNone,
					_L("RMobileUssdMessaging::GetCaps returned error"));
    CHECK_TRUE_L(ussdCapsPk().iUssdFormat==RMobileUssdMessaging::KCapsPackedString ,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));

	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	TUint32 expectedCaps =  RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	TUint32 unwantedCaps = KNoUnwantedBits;
	CHECK_BITS_SET_L(ussdCapsPk().iUssdTypes, expectedCaps, unwantedCaps, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));

	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//


	// ===  Send a valid USSD request ===

	// Encode a the USSD request "*#30#" (Caller ID presentation) into a TGsmUssdMessageData buffer.
	TBuf8<100> eightBitStr = _L8("*#30#");
	TBuf8<100> sevenBitStr;
	iSmsStoreTsyTestHelper.GsmLibSmsPackMessage( sevenBitStr, eightBitStr, eightBitStr.Length());
	
	RMobileUssdMessaging::TGsmUssdMessageData sendUssdMsg = sevenBitStr;
	RMobileUssdMessaging::TMobileUssdAttributesV1 sendAttributes;

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with
	// aMsgData=encoded TGsmUssdMessageData and
	// aMsgAttributes.iDcs=0x00 (No class, uncompressed),
	// aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType
	//				 		|  RMobileUssdMessaging::KUssdDataFormat
	//						| RMobileUssdMessaging::KUssdMessageDcs ,
	// aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString ,
	// aMsgAttributes.iType=RMobileUssdMessaging::EUssdMORequest
	TExtEtelRequestStatus sendMsgStatus(mobileUssdMessaging,EMobileUssdMessagingSendMessage);
	CleanupStack::PushL(sendMsgStatus);
	sendAttributes.iDcs = 0x00;
	sendAttributes.iFlags = RMobileUssdMessaging::KUssdMessageType
					 		| RMobileUssdMessaging::KUssdDataFormat
							| RMobileUssdMessaging::KUssdMessageDcs;
	sendAttributes.iFormat = RMobileUssdMessaging::EFormatPackedString;
	sendAttributes.iType = RMobileUssdMessaging::EUssdMORequest;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttributesPckg(sendAttributes);

	// start listening for incoming messages
	TExtEtelRequestStatus recvMsgStatus(mobileUssdMessaging,EMobileUssdMessagingReceiveMessage);
	CleanupStack::PushL(recvMsgStatus);
	RMobileUssdMessaging::TGsmUssdMessageData recvUssdMsg;
	RMobileUssdMessaging::TMobileUssdAttributesV1 recvAttributes;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg recvAttributesPckg(recvAttributes);
	mobileUssdMessaging.ReceiveMessage(recvMsgStatus,recvUssdMsg,recvAttributesPckg);

    // Post network release notification
    TExtEtelRequestStatus releaseMsgStatus(mobileUssdMessaging,EMobileUssdMessagingNotifyNetworkRelease);
	CleanupStack::PushL(releaseMsgStatus);
	RMobilePhone::TMobilePhoneSendSSRequestV3 ssRequest;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg ssRequestPckg(ssRequest);
	mobileUssdMessaging.NotifyNetworkRelease(releaseMsgStatus,ssRequestPckg,sendAttributesPckg);

	// send message
	mobileUssdMessaging.SendMessage(sendMsgStatus, sendUssdMsg, sendAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendMsgStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::SendMessage did not complete."));
    ASSERT_EQUALS(sendMsgStatus.Int(),KErrNone,
    		_L("RMobileUssdMessaging::SendMessage completed with incorrect error"));

    // ===  Receive network response ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTReply
	ASSERT_EQUALS(WaitForRequestWithTimeOut(recvMsgStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::ReceiveMessage did not complete."));
    ASSERT_EQUALS(recvMsgStatus.Int(),KErrNone,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with incorrect error"));
    ASSERT_TRUE(recvUssdMsg.Length() > 0,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with msglength <=0"));
    ASSERT_EQUALS(recvAttributes.iType,RMobileUssdMessaging::EUssdMTReply,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with incorrect USSD message type"));

	// ===  Receive network RELEASE COMPLETE message ===

	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(releaseMsgStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::NotifyNetworkRelease did not complete."));
    ASSERT_EQUALS(releaseMsgStatus.Int(),KErrNone,
    		_L("RMobileUssdMessaging::NotifyNetworkRelease completed with incorrect error"));
    ASSERT_TRUE(recvUssdMsg.Length() > 0,
    		_L("RMobileUssdMessaging::NotifyNetworkRelease completed with msglength <=0"));

	//
	// TEST END
	//

    StartCleanup();

	// Pop:
	//	releaseMsgStatus
	//	recvMsgStatus
	//	sendMsgStatus
	CleanupStack::PopAndDestroy(3,&sendMsgStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0002");
	}



CCTSYIntegrationTestUssdMessaging0003::CCTSYIntegrationTestUssdMessaging0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0003::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0003::~CCTSYIntegrationTestUssdMessaging0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0003
 * @SYMFssID BA/CTSY/USSDM-0003
 * @SYMTestCaseDesc Send an empty USSD request.
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::NotifyNetworkRelease
 * @SYMTestExpectedResults Pass - Message sent and reply received from network.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify message sent and reply received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	//

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10068);
	User::Leave(KErrNone);
	
	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_EQUALS_L(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone,
			_L("RMobileUssdMessaging::GetCaps returned error"));
	CHECK_EQUALS_L(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));


	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//


	// ===  Send a an empty USSD request ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=empty buffer and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMORequest

	// ===  Receive network RELEASE COMPLETE message ===
	// Simulated network sends a release complete
	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	TBuf8<100> eightBitStr = _L8("");
	ASSERT_EQUALS(SendSimulateCompleteCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMORequest),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL returned an error"));

	//
	// TEST END
	//

    StartCleanup();

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0003");
	}



CCTSYIntegrationTestUssdMessaging0004::CCTSYIntegrationTestUssdMessaging0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0004::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0004::~CCTSYIntegrationTestUssdMessaging0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0004
 * @SYMFssID BA/CTSY/USSDM-0004
 * @SYMTestCaseDesc Send a 24 character USSD request.
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::NotifyNetworkRelease
 * @SYMTestExpectedResults Pass - Message sent and reply received from network.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify message sent and reply received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10069);
	User::Leave(KErrNone);


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_TRUE_L(mobileUssdMessaging.GetCaps(ussdCapsPk)==KErrNone,_L("RMobileUssdMessaging::GetCaps returned error"));
	CHECK_TRUE_L(ussdCapsPk().iUssdFormat==RMobileUssdMessaging::KCapsPackedString ,_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));


	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//


	// ===  Send a valid USSD request ===

	// Encode 24 character USSD string (consisting of 24 "*") into a TGsmUssdMessageData buffer.
	TBuf8<100> eightBitStr = _L8("");
	for(TInt i=0;i<23;++i)
		{
		eightBitStr.Append('*');
		}
    eightBitStr.Append('#');

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=encoded TGsmUssdMessageData and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMORequest
	// ===  Receive network RELEASE COMPLETE message ===
	// Simulated network sends a release complete
	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	ASSERT_EQUALS(SendSimulateCompleteCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMORequest),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL returned an error"));

	//
	// TEST END
	//

    StartCleanup();


	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0004");
	}



CCTSYIntegrationTestUssdMessaging0005::CCTSYIntegrationTestUssdMessaging0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0005::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0005::~CCTSYIntegrationTestUssdMessaging0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0005
 * @SYMFssID BA/CTSY/USSDM-0005
 * @SYMTestCaseDesc Send a USSD request of length KGsmUssdDataSize
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::NotifyNetworkRelease
 * @SYMTestExpectedResults Pass - Message sent and reply received from network.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify message sent and reply received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10070);
	User::Leave(KErrNone);


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_EQUALS_L(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone,
			_L("RMobileUssdMessaging::GetCaps returned error"));
	CHECK_EQUALS_L(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));


	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//


	// ===  Send a valid USSD request ===

	// Encode and fill a TGsmUssdMessageData buffer with "*" characters.
	// $CTSYProblem This test cannot be run since LTSY crash when the message size is 50 character or more
    CHECK_TRUE_L(EFalse,_L("$CTSYProblem This test cannot be run since LTSY crash when the message size is 50 character or more"));
    TBuf8<200> eightBitStr = _L8("");
	for(TInt i=0;i<181;++i)
		{
		eightBitStr.Append('*');
		}
    eightBitStr.Append('#');

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=encoded TGsmUssdMessageData and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMORequest
	// ===  Receive network RELEASE COMPLETE message ===
	// Simulated network sends a release complete
	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	ASSERT_EQUALS(SendSimulateCompleteCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMORequest),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL returned an error"));

	//
	// TEST END
	//

    StartCleanup();


	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0005");
	}



CCTSYIntegrationTestUssdMessaging0006::CCTSYIntegrationTestUssdMessaging0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0006::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0006::~CCTSYIntegrationTestUssdMessaging0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0006
 * @SYMFssID BA/CTSY/USSDM-0006
 * @SYMTestCaseDesc Receive a USSD notify from network
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::NotifyNetworkRelease, RMobileUssdMessaging::SendMessage
 * @SYMTestExpectedResults Pass - Network USSD message received correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify message sent and reply received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_EQUALS_L(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone,
			_L("RMobileUssdMessaging::GetCaps returned error"));
	CHECK_EQUALS_L(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));


	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//


	// ===  Part 1 - Network sends a USSD notify then release complete when acknowledgement received from phone ===

	// Simulated network sends a USSD notify message (message type EUssdMTNotify).

	// ===  Receive MT notify from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTNotify
	ASSERT_EQUALS(SimulateNotifyCheckReceiveL(mobileUssdMessaging,RMobileUssdMessaging::EUssdMTNotify),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNotifyCheckReceiveL returned an error"));

    // ===  Reply to network with MO acknowledgement ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=empty TGsmUssdMessageData and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMOAcknowledgement

	// ===  Receive network RELEASE COMPLETE message ===
	// Simulated network sends a release complete
	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	TBuf8<100> eightBitStr = _L8("");
	ASSERT_EQUALS(SendSimulateCompleteCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMOAcknowledgement),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL returned an error"));

	// ===  Part 2 - Network sends a USSD notify then release complete when acknowledgement received from phone ===

	// Simulated network sends a USSD notify message (message type EUssdMTNotify).

	// ===  Receive MT notify from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTNotify
	ASSERT_EQUALS(SimulateNotifyCheckReceiveL(mobileUssdMessaging,RMobileUssdMessaging::EUssdMTNotify),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNotifyCheckReceiveL returned an error"));

	// ===  Reply to network with MO acknowledgement ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=empty TGsmUssdMessageData and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMOAcknowledgement

	// Simulated network sends a USSD notify message (message type EUssdMTNotify).

	// ===  Receive MT notify from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTNotify
	ASSERT_EQUALS(SendSimulateNotifyCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMOAcknowledgement,RMobileUssdMessaging::EUssdMTNotify),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateNotifyCheckCompleteL returned an error"));

	// ===  Reply to network with MO acknowledgement ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=empty TGsmUssdMessageData and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMOAcknowledgement

	// ===  Receive network RELEASE COMPLETE message ===
	// Simulated network sends a release complete
	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	ASSERT_EQUALS(SendSimulateCompleteCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::RMobileUssdMessaging::EUssdMOAcknowledgement),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL returned an error"));


	//
	// TEST END
	//

    StartCleanup();


	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0006");
	}



CCTSYIntegrationTestUssdMessaging0007::CCTSYIntegrationTestUssdMessaging0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0007::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0007::~CCTSYIntegrationTestUssdMessaging0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0007
 * @SYMFssID BA/CTSY/USSDM-0007
 * @SYMTestCaseDesc Receive a USSD notify from network of 182 characters
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::SendRelease, RMobileUssdMessaging::SendMessage
 * @SYMTestExpectedResults Pass - Network USSD message received correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify message sent and reply received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_EQUALS_L(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone,
			_L("RMobileUssdMessaging::GetCaps returned error"));
    CHECK_EQUALS_L(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));



	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//

	// Simulated network sends a USSD notify message (message type EUssdMTNotify) of 182 characters.

	// ===  Receive MT notify from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTNotify
	ASSERT_EQUALS(SimulateNotifyCheckReceiveL(mobileUssdMessaging,RMobileUssdMessaging::EUssdMTNotify,182),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNotifyCheckReceiveL returned an error"));

	// ===  Reply to network with MO acknowledgement ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=empty TGsmUssdMessageData and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMOAcknowledgement
	TExtEtelRequestStatus sendMsgStatus(mobileUssdMessaging,EMobileUssdMessagingSendMessage);
	CleanupStack::PushL(sendMsgStatus);
	RMobileUssdMessaging::TGsmUssdMessageData sendUssdMsg = _L8("");
	RMobileUssdMessaging::TMobileUssdAttributesV1 sendAttributes;
	sendAttributes.iDcs=0x00;
	sendAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs;
	sendAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString;
	sendAttributes.iType=RMobileUssdMessaging::EUssdMOAcknowledgement;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttributesPckg(sendAttributes);
	mobileUssdMessaging.SendMessage(sendMsgStatus, sendUssdMsg, sendAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendMsgStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::SendMessage did not complete."));
    ASSERT_EQUALS(sendMsgStatus.Int(),KErrNone,
    		_L("RMobileUssdMessaging::SendMessage completed with incorrect error"));

	// ===  Send a release complete to the network ===

	// Check RMobileUssdMessaging::SendRelease completes with KErrNone
	TExtEtelRequestStatus sendReleaseStatus(mobileUssdMessaging,EMobileUssdMessagingSendRelease);
	CleanupStack::PushL(sendReleaseStatus);
	RMobilePhone::TMobilePhoneSendSSRequestV3 requestInfo;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg requestInfoPckg(requestInfo);
    mobileUssdMessaging.SendRelease(sendReleaseStatus,requestInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendReleaseStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::SendRelease did not complete."));
    ASSERT_EQUALS(sendReleaseStatus.Int(),KErrNone,
    		_L("RMobileUssdMessaging::SendRelease completed with incorrect error"));


	//
	// TEST END
	//

    StartCleanup();

    // Pop:
	//	sendReleaseStatus
	//	sendMsgStatus
	CleanupStack::PopAndDestroy(2,&sendMsgStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0007");
	}



CCTSYIntegrationTestUssdMessaging0008::CCTSYIntegrationTestUssdMessaging0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0008::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0008::~CCTSYIntegrationTestUssdMessaging0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0008
 * @SYMFssID BA/CTSY/USSDM-0008
 * @SYMTestCaseDesc Receive a USSD request from the network.
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::NotifyNetworkRelease, RMobileUssdMessaging::SendMessage
 * @SYMTestExpectedResults Pass - Network USSD message received correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify messages received and replies can be sent.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_EQUALS_L(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone,
			_L("RMobileUssdMessaging::GetCaps returned error"));
	CHECK_EQUALS_L(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));


	//
	// SET UP END
	//

	StartTest();
	//
	// TEST START
	//


	// ===  Part 1 - network sends USSD request, phone replies and network releases ===

	// Simulated network sends a USSD notify message (message type EUssdMTRequest)

	// ===  Receive MT request from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTRequest
	ASSERT_EQUALS(SimulateNotifyCheckReceiveL(mobileUssdMessaging,RMobileUssdMessaging::EUssdMTRequest),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNotifyCheckReceiveL returned an error"));

	// ===  Send a USSD reply ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=a valid encoded string and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTReply
	TBuf8<100> eightBitStr = _L8("*#30#");

	// ===  Receive network RELEASE COMPLETE message ===
	// Simulated network sends a release complete
	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id2 = %d</font>"), 10035);       
	CHECK_TRUE_L(EFalse, _L("This test has been made to leave intentially - ***Send message fail, and ctsy cancel it later****"));
	ASSERT_EQUALS(SendSimulateCompleteCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMTReply),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL returned an error"));

	// ===  Part 2 - network sends USSD request, phone replies, network sends another request and phone replies again, then network releases ===

	// Simulated network sends a USSD notify message (message type EUssdMTRequest)

	// ===  Receive MT request from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTRequest
	ASSERT_EQUALS(SimulateNotifyCheckReceiveL(mobileUssdMessaging,RMobileUssdMessaging::EUssdMTRequest),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNotifyCheckReceiveL returned an error"));

	// ===  Send a USSD reply ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=a valid encoded string and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTReply
	eightBitStr = _L8("*#30#");

	// Simulated network sends a USSD notify message (message type EUssdMTRequest)

	// ===  Receive MT request from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTRequest
	ASSERT_EQUALS(SendSimulateNotifyCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMTReply ,RMobileUssdMessaging::EUssdMTRequest),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateNotifyCheckCompleteL returned an error"));

	// ===  Send a USSD reply ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=a valid encoded string and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTReply
	eightBitStr = _L8("*#30#");

	// ===  Receive network RELEASE COMPLETE message ===
	// Simulated network sends a release complete
	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	ASSERT_EQUALS(SendSimulateCompleteCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMTReply ),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL returned an error"));


	//
	// TEST END
	//

    StartCleanup();

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0008");
	}



CCTSYIntegrationTestUssdMessaging0009::CCTSYIntegrationTestUssdMessaging0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0009::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0009::~CCTSYIntegrationTestUssdMessaging0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0009
 * @SYMFssID BA/CTSY/USSDM-0009
 * @SYMTestCaseDesc Receive a USSD request from the network of 182 characters long.
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::NotifyNetworkRelease, RMobileUssdMessaging::SendMessage
 * @SYMTestExpectedResults Pass - Network USSD message received correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify message received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_EQUALS_L(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone,
			_L("RMobileUssdMessaging::GetCaps returned error"));
	CHECK_EQUALS_L(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));



	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//

	// Simulated network sends a USSD notify message (message type EUssdMTRequest) of 182 characters

	// ===  Receive MT request from network ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTRequest
	ASSERT_EQUALS(SimulateNotifyCheckReceiveL(mobileUssdMessaging,RMobileUssdMessaging::EUssdMTRequest,182),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SimulateNotifyCheckReceiveL returned an error"));


	// ===  Send a USSD reply ===

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=a valid encoded string and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTReply
	TBuf8<100> eightBitStr = _L8("*#30#");

	// ===  Receive network RELEASE COMPLETE message ===
	// Simulated network sends a release complete
	// Check RMobileUssdMessaging::NotifyNetworkRelease completes with aMsgData with length > 0
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id2 = %d</font>"), 10035);       
	CHECK_TRUE_L(EFalse, _L("This test has been made to leave intentially - ***Send message fail, and ctsy cancel it later****"));
	ASSERT_EQUALS(SendSimulateCompleteCheckCompleteL(mobileUssdMessaging,eightBitStr,RMobileUssdMessaging::EUssdMTReply),KErrNone,_L("CCTSYIntegrationTestUssdMessagingBase::SendSimulateCompleteCheckCompleteL returned an error"));

	//
	// TEST END
	//

    StartCleanup();


	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0009");
	}



CCTSYIntegrationTestUssdMessaging0010::CCTSYIntegrationTestUssdMessaging0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0010::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0010::~CCTSYIntegrationTestUssdMessaging0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0010
 * @SYMFssID BA/CTSY/USSDM-0010
 * @SYMTestCaseDesc Terminate a USSD exchange.
 * @SYMTestPriority High
 * @SYMTestActions RMobileUssdMessaging::ReceiveMessage, RMobileUssdMessaging::SendRelease
 * @SYMTestExpectedResults Pass - KErrNotSupported when sending USSD to unsupported network
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify sending of release complete message is successful.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_EQUALS_L(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone,
			_L("RMobileUssdMessaging::GetCaps returned error"));
	CHECK_EQUALS_L(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TInt ret = iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
			_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));


	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//


	// ===  Send a valid USSD request ===

	// Encode a the USSD request "*#30#" (Caller ID presentation) into a TGsmUssdMessageData buffer.
	TBuf8<100> eightBitStr = _L8("*#30#");
	TBuf8<100> sevenBitStr;
	iSmsStoreTsyTestHelper.GsmLibSmsPackMessage( sevenBitStr, eightBitStr, eightBitStr.Length());
	RMobileUssdMessaging::TGsmUssdMessageData sendUssdMsg = sevenBitStr;

	// start listening for incoming messages
	TExtEtelRequestStatus recvMsgStatus(mobileUssdMessaging,EMobileUssdMessagingReceiveMessage);
	CleanupStack::PushL(recvMsgStatus);
	RMobileUssdMessaging::TGsmUssdMessageData recvUssdMsg;
	RMobileUssdMessaging::TMobileUssdAttributesV1 recvAttributes;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg recvAttributesPckg(recvAttributes);
	mobileUssdMessaging.ReceiveMessage(recvMsgStatus,recvUssdMsg,recvAttributesPckg);

	// Send a USSD message to the network with RMobileUssdMessaging::SendMessage with aMsgData=encoded TGsmUssdMessageData and  aMsgAttributes.iDcs=0x00 (No class, uncompressed),aMsgAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs , aMsgAttributes.iFormat=RMobileUssdMessaging::EFormatPackedString , aMsgAttributes.iType=RMobileUssdMessaging::EUssdMORequest
	TExtEtelRequestStatus sendMsgStatus(mobileUssdMessaging,EMobileUssdMessagingSendMessage);
	CleanupStack::PushL(sendMsgStatus);
	RMobileUssdMessaging::TMobileUssdAttributesV1 sendAttributes;
	sendAttributes.iDcs=0x00;
	sendAttributes.iFlags=RMobileUssdMessaging::KUssdMessageType |  RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageDcs;
	sendAttributes.iFormat=RMobileUssdMessaging::RMobileUssdMessaging::EFormatPackedString;
	sendAttributes.iType=RMobileUssdMessaging::EUssdMORequest;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttributesPckg(sendAttributes);
	mobileUssdMessaging.SendMessage(sendMsgStatus, sendUssdMsg, sendAttributesPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendMsgStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::SendMessage did not complete."));
    ASSERT_EQUALS(sendMsgStatus.Int(),KErrNone,
    		_L("RMobileUssdMessaging::SendMessage completed with incorrect error"));

	// ===  Receive network response ===

	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgData with length > 0
	// Check RMobileUssdMessaging::ReceiveMessage completes with aMsgAttributes.iType=RMobileUssdMessaging::EUssdMTReply
	ASSERT_EQUALS(WaitForRequestWithTimeOut(recvMsgStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::ReceiveMessage did not complete."));
    ASSERT_EQUALS(recvMsgStatus.Int(),KErrNone,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with incorrect error"));
    ASSERT_TRUE(recvUssdMsg.Length() > 0,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with msglength <=0"));
    ASSERT_EQUALS(recvAttributes.iType,RMobileUssdMessaging::EUssdMTReply,
    		_L("RMobileUssdMessaging::ReceiveMessage completed with incorrect USSD message type"));

	// ===  Send release complete  ===

	// Check RMobileUssdMessaging::SendRelease completes with KErrNone
	TExtEtelRequestStatus sendReleaseStatus(mobileUssdMessaging,EMobileUssdMessagingSendRelease);
	CleanupStack::PushL(sendReleaseStatus);
	RMobilePhone::TMobilePhoneSendSSRequestV3 requestInfo;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg requestInfoPckg(requestInfo);
    mobileUssdMessaging.SendRelease(sendReleaseStatus,requestInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(sendReleaseStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::SendRelease did not complete."));
    ASSERT_EQUALS(sendReleaseStatus.Int(),KErrNone,
    		_L("RMobileUssdMessaging::SendRelease completed with incorrect error"));

	//
	// TEST END
	//

    StartCleanup();

	// Pop:
	//	sendReleaseStatus
	//	sendMsgStatus
	//	recvMsgStatus
	CleanupStack::PopAndDestroy(3,&recvMsgStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0010");
	}



CCTSYIntegrationTestUssdMessaging0011::CCTSYIntegrationTestUssdMessaging0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestUssdMessagingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestUssdMessaging0011::GetTestStepName());
	}

CCTSYIntegrationTestUssdMessaging0011::~CCTSYIntegrationTestUssdMessaging0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestUssdMessaging0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-USSDM-0011
 * @SYMFssID BA/CTSY/USSDM-0011
 * @SYMTestCaseDesc Cancel receiving a USSD message.
 * @SYMTestPriority High
 * @SYMTestActions
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	// Check RMobileUssdMessaging::GetCaps returns iUssdFormat of KCapsPackedString
	// Check RMobileUssdMessaging::GetCaps returns iUssdTypes of KCapsMOUssd | KCapsMTUssd
	RMobileUssdMessaging& mobileUssdMessaging = iEtelSessionMgr.GetUssdMessagingL(KMainServer, KMainPhone,KMainUssdMessaging);
	RMobileUssdMessaging::TMobileUssdCapsV1   ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPk(ussdCaps);
	CHECK_EQUALS_L(mobileUssdMessaging.GetCaps(ussdCapsPk), KErrNone,
			_L("RMobileUssdMessaging::GetCaps returned error"));
	CHECK_EQUALS_L(ussdCaps.iUssdFormat, (TUint32)RMobileUssdMessaging::KCapsPackedString,
    		_L("RMobileUssdMessaging::GetCaps returned incorrect iUssdFormat"));
	TUint32 expectedCaps = 	RMobileUssdMessaging::KCapsMOUssd | RMobileUssdMessaging::KCapsMTUssd;
	CHECK_BITS_SET_L(ussdCaps.iUssdTypes, expectedCaps, KNoUnwantedBits, _L("RMobileUssdMessaging::GetCaps returned incorrect iUssdTypes"));


	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//


	// Make an RMobileUssdMessaging::ReceiveMessage request
	TExtEtelRequestStatus recvMsgStatus(mobileUssdMessaging,EMobileUssdMessagingReceiveMessage);
	CleanupStack::PushL(recvMsgStatus);
	RMobileUssdMessaging::TGsmUssdMessageData recvUssdMsg;
	RMobileUssdMessaging::TMobileUssdAttributesV1 recvAttributes;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg recvAttributesPckg(recvAttributes);
	mobileUssdMessaging.ReceiveMessage(recvMsgStatus,recvUssdMsg,recvAttributesPckg);

	// Cancel the request with RTelSubSession::CancelAsyncRequest(EMobileUssdMessagingReceiveMessage);
	mobileUssdMessaging.CancelAsyncRequest(EMobileUssdMessagingReceiveMessage);

	// Check receive message request completes with KErrCancel
	ASSERT_EQUALS(WaitForRequestWithTimeOut(recvMsgStatus, ETimeLong),
			KErrNone, _L("RMobileUssdMessaging::ReceiveMessage did not complete."));
    ASSERT_EQUALS(recvMsgStatus.Int(),KErrCancel,
    		_L("RMobileUssdMessaging::ReceiveMessage did not completed with KErrCancel"));


	//
	// TEST END
	//

    StartCleanup();

	// Pop:
	//	recvMsgStatus
	CleanupStack::PopAndDestroy(1,&recvMsgStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestUssdMessaging0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestUssdMessaging0011");
	}



