// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmussd.h"
#include "testdef.h"

CTestUssdMessaging::CTestUssdMessaging()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestUssdMessaging"));
	}

enum TVerdict CTestUssdMessaging::doTestStepL()
//
// Test USSD Messaging Functions
//
    {
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile USSD Messaging Functionality"));
//	LOGTEXT(_L8("Test Mobile USSD Messaging Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileUssdMessaging ussdMessaging;
	ret=ussdMessaging.Open(mmPhone);
    TEST(ret==KErrNone);

	TRequestStatus reqStatus;

	// Get USSD messaging caps
	RMobileUssdMessaging::TMobileUssdCapsV1 ussdCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg ussdCapsPckg(ussdCaps);

	TEST(ussdMessaging.GetCaps(ussdCapsPckg)==KErrNone);

	ussdCaps=ussdCapsPckg();
	TEST(ussdCaps.iUssdTypes==DMMTSY_USSD_MESSAGING_TYPE_CAPS);

	INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::GetCaps (sync) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileUssdMessaging's Get Ussd caps"));

	// Receiving Ussd messages
	
	RMobileUssdMessaging::TGsmUssdMessageData msg, sendMsg;
	RMobileUssdMessaging::TMobileUssdAttributesV1 atts, sendAtts;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg attsPckg(atts);
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg sendAttsPckg(sendAtts);

	// asynchronous

	ussdMessaging.ReceiveMessage(reqStatus, msg, attsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateUssdMessage(msg,atts);
	
	// asynchronous & cancel 

	ussdMessaging.ReceiveMessage(reqStatus, msg, attsPckg);
	ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingReceiveMessage);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::ReceiveMessage (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateUssdMessage(msg,atts);
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::ReceiveMessage (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	// Send Message
	
	// asynchronous

	sendMsg = DMMTSY_USSD_MESSAGE_PDU;
	sendAtts.iFlags = DMMTSY_USSD_ATTRIBUTE_FLAGS;
	sendAtts.iFormat = DMMTSY_USSD_ATTRIBUTE_FORMAT;
	sendAtts.iType = DMMTSY_USSD_ATTRIBUTE_TYPE;
	sendAtts.iDcs = DMMTSY_USSD_ATTRIBUTE_DCS;

	ussdMessaging.SendMessage(reqStatus, sendMsg, sendAttsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	ussdMessaging.SendMessage(reqStatus, sendMsg, sendAttsPckg);
	ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingSendMessage);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::SendMessage (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::SendMessage (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileUssdMessaging's Receive & Send Message"));

	// Send Message NoFdnCheck
	
	// asynchronous

	sendMsg = DMMTSY_USSD_MESSAGE_PDU;
	sendAtts.iFlags = DMMTSY_USSD_ATTRIBUTE_FLAGS;
	sendAtts.iFormat = DMMTSY_USSD_ATTRIBUTE_FORMAT;
	sendAtts.iType = DMMTSY_USSD_ATTRIBUTE_TYPE;
	sendAtts.iDcs = DMMTSY_USSD_ATTRIBUTE_DCS;

	ussdMessaging.SendMessageNoFdnCheck(reqStatus, sendMsg, sendAttsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	ussdMessaging.SendMessageNoFdnCheck(reqStatus, sendMsg, sendAttsPckg);
	ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingSendMessageNoFdnCheck);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::SendMessageNoFdnCheck (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::SendMessageNoFdnCheck (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileUssdMessaging's Receive & Send Message with No Fdn Check"));

	//Send Release 
	RMobilePhone::TMobilePhoneSendSSRequestV3 ssRequest;
	RMobilePhone::TMobilePhoneSendSSRequestV3Pckg ssRequestPckg(ssRequest);
	
	//asynchronous
	
	ussdMessaging.SendRelease(reqStatus, ssRequestPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(ssRequest.iOpCode == DMMTSY_PHONE_NOTIFY_OPCODE);
	TEST(ssRequest.iAdditionalInfo.Compare(DMMTSY_PHONE_NOTIFY_ADDINFO)==0);
	
	// asynchronous & cancel
	
	ussdMessaging.SendRelease(reqStatus, ssRequestPckg);
	ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingSendRelease);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::SendRelease (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::SendRelease (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	//Notify Network Release (deprecated version) 
	
	//asynchronous
	
	ussdMessaging.NotifyNetworkRelease(reqStatus, ssRequestPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(ssRequest.iOpCode == DMMTSY_PHONE_NOTIFY_OPCODE);
	TEST(ssRequest.iAdditionalInfo.Compare(DMMTSY_PHONE_NOTIFY_ADDINFO)==0);
	
	// asynchronous & cancel
	
	ussdMessaging.NotifyNetworkRelease(reqStatus, ssRequestPckg);
	ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingNotifyNetworkRelease);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::NotifyNetworkRelease (deprecated version) (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::NotifyNetworkRelease (deprecated version) (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	//Notify Network Release
		
	//asynchronous
	
	ussdMessaging.NotifyNetworkRelease(reqStatus, ssRequestPckg, sendAttsPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(ssRequest.iOpCode == DMMTSY_PHONE_NOTIFY_OPCODE);
	TEST(ssRequest.iAdditionalInfo.Compare(DMMTSY_PHONE_NOTIFY_ADDINFO)==0);
	TEST(sendAtts.iFlags  == DMMTSY_USSD_ATTRIBUTE_FLAGS);
	TEST(sendAtts.iFormat == DMMTSY_USSD_ATTRIBUTE_FORMAT);
	TEST(sendAtts.iType   == DMMTSY_USSD_ATTRIBUTE_TYPE);
	TEST(sendAtts.iDcs    == DMMTSY_USSD_ATTRIBUTE_DCS);
	
	// asynchronous & cancel
	
	ussdMessaging.NotifyNetworkRelease(reqStatus, ssRequestPckg, sendAttsPckg);
	ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingNotifyNetworkRelease);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::NotifyNetworkRelease (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileUssdMessaging::NotifyNetworkRelease (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	ussdMessaging.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}


void CTestUssdMessaging::ValidateUssdMessage(const TDesC8& aMsg, const RMobileUssdMessaging::TMobileUssdAttributesV1 &aAtts)
	{
	TEST(aMsg.Compare(DMMTSY_USSD_MESSAGE_PDU) == 0);
	TEST(aAtts.iFlags == DMMTSY_USSD_ATTRIBUTE_FLAGS);
	TEST(aAtts.iFormat == DMMTSY_USSD_ATTRIBUTE_FORMAT);
	TEST(aAtts.iType == DMMTSY_USSD_ATTRIBUTE_TYPE);
	TEST(aAtts.iDcs == DMMTSY_USSD_ATTRIBUTE_DCS);
	}

