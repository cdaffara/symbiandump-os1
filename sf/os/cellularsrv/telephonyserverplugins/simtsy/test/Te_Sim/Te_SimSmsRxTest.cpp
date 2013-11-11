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

#include "Te_SimSmsRxTest.h"

CSimSmsRxTest::CSimSmsRxTest() 
	{ 
	SetTestStepName(_L("SmsRxTest"));
	}

TVerdict CSimSmsRxTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginSmsRxTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(8);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSmsMessaging.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened SMS Messaging object"));

	INFO_PRINTF1(_L("Testing GetCaps"));
	RMobileSmsMessaging::TMobileSmsCapsV1 caps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg capsPckg(caps);
	iSmsMessaging.GetCaps(capsPckg);
	TESTL(caps.iSmsMode==RMobileSmsMessaging::KCapsGsmSms);
	TESTL(caps.iSmsControl==KSmsMessagingCaps);

	RMobileSmsMessaging::TMobileSmsReceiveMode rxMode;
	iSmsMessaging.GetReceiveMode(rxMode);
	TESTL(rxMode==RMobileSmsMessaging::EReceiveUnstoredClientAck);
	CHECKPOINTL(rxMode,RMobileSmsMessaging::EReceiveUnstoredClientAck,CHP_SMS_CASE("C.1"));

	INFO_PRINTF1(_L("Testing SMS PDU Retrieval"));
	TSmsPdu smsPdu;
	TRequestStatus stat0;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPckg(attrib);
	iSmsMessaging.ReceiveMessage(stat0,smsPdu,attribPckg);
	INFO_PRINTF1(_L("1st receive Request sent"));
	
	//test message received 1	
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0,KErrNone,CHP_SMS_CASE("C.4"));
	INFO_PRINTF1(_L("Message received"));
	TESTL(smsPdu.Length()==sizeof(KTestPduA));
	TInt i;
	for(i=0;i<smsPdu.Length();i++)
		TESTL(smsPdu[i]==KTestPduA[i]);
	TESTL(attrib.iGsmServiceCentre.iTelNumber==KScaNumberA);
	TESTL(attrib.iGsmServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber);
	TESTL(attrib.iGsmServiceCentre.iNumberPlan==RMobilePhone::EIsdnNumberPlan);

	//ack message
	iSmsMessaging.AckSmsStored(stat0,KTestDeliveryReport,EFalse);
	iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingAckSmsStored);//no implementation-does nothing
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	CHECKPOINTL(stat0,KErrNone,CHP_SMS_CASE("C.7"));
	INFO_PRINTF1(_L("Message 1 ack"));

	iSmsMessaging.ReceiveMessage(stat0,smsPdu,attribPckg);
	INFO_PRINTF1(_L("2nd receive Request sent"));
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0,KErrNone,CHP_SMS_CASE("C.5"));
	INFO_PRINTF1(_L("Message 2 received"));

	TESTL(smsPdu.Length()==sizeof(KTestPduB));
	for(i=0;i<smsPdu.Length();i++)
		TESTL(smsPdu[i]==KTestPduB[i]);

	TESTL(attrib.iGsmServiceCentre.iTelNumber==KScaNumberB);
	TESTL(attrib.iGsmServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber);
	TESTL(attrib.iGsmServiceCentre.iNumberPlan==RMobilePhone::EIsdnNumberPlan);

	iSmsMessaging.AckSmsStored(stat0,KTestDeliveryReport,ETrue);//store is full
	iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingNackSmsStored);//no implementation-does nothing
	INFO_PRINTF1(_L("Message 2 Nack (full)"));
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	CHECKPOINTL(stat0,KErrNone,CHP_SMS_CASE("C.8"));

	iSmsMessaging.ResumeSmsReception(stat0);
	iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingResumeSmsReception);
	User::WaitForRequest(stat0);
	INFO_PRINTF1(_L("Resume reception "));
	TESTL(stat0==KErrNone);
	CHECKPOINTL(stat0,KErrNone,CHP_SMS_CASE("C.10"));

	iSmsMessaging.ReceiveMessage(stat0,smsPdu,attribPckg);
	INFO_PRINTF1(_L("3rd receive Request sent"));
	User::WaitForRequest(stat0);
	INFO_PRINTF1(_L("Message 3 received"));
	

	iSmsMessaging.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

