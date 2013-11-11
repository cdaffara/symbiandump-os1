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

#include "Te_SimSmsTxTest.h"

CSimSmsTxTest::CSimSmsTxTest() 
	{ 
	SetTestStepName(_L("SmsTxTest"));
	}

TVerdict CSimSmsTxTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginSmsTxTest"));

 	CreateConfigFileL(_L("c:\\config2.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSmsMessaging.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened SMS Messaging object"));

	RMobileSmsMessaging::TMobileSmsCapsV1 caps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg capsPckg(caps);
	iSmsMessaging.GetCaps(capsPckg);
	TESTL(caps.iSmsMode==RMobileSmsMessaging::KCapsGsmSms);
	TESTL(caps.iSmsControl==KSmsMessagingCaps);

	TRequestStatus stat0, statReceive, statSend;
	
	RMobileSmsMessaging::TMobileSmsReceiveMode rxMode;
	rxMode = RMobileSmsMessaging::EReceiveUnstoredPhoneAck;
	iSmsMessaging.SetReceiveMode(stat0,rxMode);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	
	TSmsPdu smsPdu;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPckg(attrib);
	iSmsMessaging.ReceiveMessage(statReceive,smsPdu,attribPckg);
	INFO_PRINTF1(_L("1st receive Request sent"));
	
	RMobileSmsMessaging::TMobileSmsSendAttributesV1 attribSend;
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg attribSendPckg(attribSend);
	TSmsPdu smsPduSent;
	smsPduSent.Copy(KTestSendPduA,sizeof(KTestSendPduA));
	iSmsMessaging.SendMessage(statSend,smsPduSent,attribSendPckg);
	INFO_PRINTF1(_L("1st Send Request sent"));
	User::WaitForRequest(statSend);
	CHECKPOINTL(statSend,KErrNone,CHP_SMS_CASE("C.11"));
	INFO_PRINTF1(_L("Message 1s sent"));

	//test sms sent atributes
	TUint16 msgRef = 12;//test value
	TESTL(attribSend.iMsgRef==msgRef);
	
	 
	TESTL(attribSend.iSubmitReport.Length()==sizeof(KTestSubmitReport));
	for(TInt j =0;j<attribSend.iSubmitReport.Length();j++)
		TESTL(attribSend.iSubmitReport[j]==KTestSubmitReport[j]);

//test message received 1	
	User::WaitForRequest(statReceive);
	CHECKPOINTL(statReceive,KErrNone,CHP_SMS_CASE("C.6"));	

	INFO_PRINTF1(_L("Message 1r received"));
	TESTL(smsPdu.Length()==sizeof(KTestPduA));
	TInt i;
	for(i=0;i<smsPdu.Length();i++)
		TESTL(smsPdu[i]==KTestPduA[i]);
	TESTL(attrib.iGsmServiceCentre.iTelNumber==KScaNumberA);
	TESTL(attrib.iGsmServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber);
	TESTL(attrib.iGsmServiceCentre.iNumberPlan==RMobilePhone::EIsdnNumberPlan);

//request receive again
	INFO_PRINTF1(_L("request receive a 2nd message"));
	iSmsMessaging.ReceiveMessage(statReceive,smsPdu,attribPckg);

//send message 2
	smsPduSent.Copy(KTestSendPduB,sizeof(KTestSendPduB));
	iSmsMessaging.SendMessage(statSend,smsPduSent,attribSendPckg);
	INFO_PRINTF1(_L("2nd Send Request sent"));
	User::WaitForRequest(statSend);
	TESTL(statSend==KErrCommsLineFail);//forced error
	CHECKPOINTL(statSend,KErrCommsLineFail,CHP_SMS_CASE("C.12"));
	INFO_PRINTF1(_L("Message 2s sent"));

//Receive has not completed yet need 1 more message sent

	User::After(3000000L);
	TESTL(statReceive!=KErrNone);

//send message 3
	iSmsMessaging.SendMessage(statSend,smsPduSent,attribSendPckg);
	INFO_PRINTF1(_L("3rd Send Request sent"));
	User::WaitForRequest(statSend);
	TESTL(statSend==KErrCorrupt);//different pdus
	CHECKPOINTL(statSend,KErrCorrupt,CHP_SMS_CASE("C.13"));
	INFO_PRINTF1(_L("Message 3s sent"));

//pDU retreival of message received 2
	User::WaitForRequest(statReceive);
	INFO_PRINTF1(_L("Message 2r received"));

	TESTL(smsPdu.Length()==sizeof(KTestPduB));
	for(i=0;i<smsPdu.Length();i++)
		TESTL(smsPdu[i]==KTestPduB[i]);
	TESTL(attrib.iGsmServiceCentre.iTelNumber==KScaNumberB);
	TESTL(attrib.iGsmServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber);
	TESTL(attrib.iGsmServiceCentre.iNumberPlan==RMobilePhone::EIsdnNumberPlan);

//receive a 3rd message
	iSmsMessaging.ReceiveMessage(statReceive,smsPdu,attribPckg);
	INFO_PRINTF1(_L("3rd receive Request sent"));
	User::WaitForRequest(statReceive);
	INFO_PRINTF1(_L("Message 3r received"));


//the 4th message receive request doesn't complete as the config files specifies there are no more to trigger
	iSmsMessaging.ReceiveMessage(statReceive,smsPdu,attribPckg);
	INFO_PRINTF1(_L("4th receive Request sent"));
	User::After(3000000L);//wait 3 sec
	TESTL(statReceive!=KErrNone);

	INFO_PRINTF1(_L("sending 4th message"));
	iSmsMessaging.SendMessage(statSend,smsPduSent,attribSendPckg);
	iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingSendMessage);
	User::WaitForRequest(statSend);
	TESTL(statSend==KErrCancel);
	CHECKPOINTL(statSend,KErrCancel,CHP_SMS_CASE("C.14"));	
	INFO_PRINTF1(_L("Message 4s Cancelled"));

	//Even if cancelled the SmsTx counter is incremented and incoming messages are triggered
	User::WaitForRequest(statReceive);
	TESTL(statReceive==KErrNone);

	iSmsMessaging.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	return TestStepResult();
	}

