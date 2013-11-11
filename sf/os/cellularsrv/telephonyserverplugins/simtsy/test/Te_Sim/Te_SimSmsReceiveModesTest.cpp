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

#include "Te_SimSmsReceiveModesTest.h"

CSimSmsReceiveModesTest::CSimSmsReceiveModesTest() 
	{ 
	SetTestStepName(_L("SmsReceiveModesTest"));
	}

TVerdict CSimSmsReceiveModesTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginSmsReceiveModesTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(2);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iSmsMessaging.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened SMS Messaging object"));
	
	INFO_PRINTF1(_L("Testing Retrieving SMS Messaging Store Informtion"));
	TRequestStatus stat0, statNotif, statStore, statReceive;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 storeInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg storeInfoPckg(storeInfo);
	iSmsMessaging.GetMessageStoreInfo(stat0,0,storeInfoPckg);
	User::WaitForRequest(stat0);
	TESTL(storeInfo.iUsedEntries==1);
	TESTL(iSmsStore.Open(iSmsMessaging,storeInfo.iName)==KErrNone);
	INFO_PRINTF1(_L("Opened SMS Store object"));

	INFO_PRINTF1(_L("Testing GetCaps"));
	RMobileSmsMessaging::TMobileSmsCapsV1 caps;
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg capsPckg(caps);
	iSmsMessaging.GetCaps(capsPckg);
	TESTL(caps.iSmsMode==RMobileSmsMessaging::KCapsGsmSms);
	TESTL(caps.iSmsControl==KSmsMessagingCaps);

	RMobileSmsMessaging::TMobileSmsReceiveMode rxMode;
	iSmsMessaging.GetReceiveMode(rxMode);
	TESTL(rxMode==RMobileSmsMessaging::EReceiveUnstoredClientAck);

	rxMode = RMobileSmsMessaging::EReceiveStored;
	iSmsMessaging.SetReceiveMode(stat0,rxMode);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);

	TUint32 event;
	TInt index;
	iSmsStore.NotifyStoreEvent(statNotif,event,index);

	INFO_PRINTF1(_L("Testing SMS PDU Retrieval"));
	TSmsPdu smsPdu;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attrib;
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg attribPckg(attrib);
	iSmsMessaging.ReceiveMessage(stat0,smsPdu,attribPckg);
	INFO_PRINTF1(_L("1st receive Request sent"));
	
	//test message received #1	
	User::WaitForRequest(stat0);
	INFO_PRINTF1(_L("Message 1 received"));
	User::WaitForRequest(statNotif);
	INFO_PRINTF1(_L("Message added to store"));

	TESTL(smsPdu.Length()==sizeof(KTestPduRx1));
	TInt i;
	for(i=0;i<smsPdu.Length();i++)
		TESTL(smsPdu[i]==KTestPduRx1[i]);
	TESTL(attrib.iGsmServiceCentre.iTelNumber==KScaNumber1);
	TESTL(attrib.iGsmServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber);
	TESTL(attrib.iGsmServiceCentre.iNumberPlan==RMobilePhone::EIsdnNumberPlan);
	TESTL(attrib.iStatus==RMobileSmsMessaging::EMtMessageStored);
	TESTL(attrib.iStoreIndex==2);
	TESTL(attrib.iStore==KSmsStoreName);

	TESTL(event & RMobilePhoneStore::KStoreEntryAdded);//store added
	TESTL(event & RMobilePhoneStore::KStoreFull);
	INFO_PRINTF2(_L("1 message added to store at index %d"), index);

	//check message was stored
	RMobileSmsStore::TMobileGsmSmsEntryV1 smsEntryforRead;
	smsEntryforRead.iIndex=2;
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsEntryforReadPckg(smsEntryforRead);
	INFO_PRINTF1(_L("Testing SMS Store Read"));
	iSmsStore.Read(statStore, smsEntryforReadPckg);
	User::WaitForRequest(statStore);

	//receive 2nd phone stored message but no memory
	iSmsStore.NotifyStoreEvent(statNotif,event,index);
	iSmsMessaging.ReceiveMessage(stat0,smsPdu,attribPckg);
	INFO_PRINTF1(_L("2nd receive Request sent"));

	User::After(5000000L);//wait here for message to be discarded in tsy because store is full
	iSmsStore.Delete(statStore, 2);//delete message at index2
	User::WaitForRequest(statStore);
	INFO_PRINTF1(_L("Message 2 deleted"));
	TESTL(statStore==KErrNone);
	
	//test message received #2	
	User::WaitForRequest(stat0);
	INFO_PRINTF1(_L("Message 2 received"));
	TESTL(stat0==KErrNone);
	User::WaitForRequest(statNotif);	

	for(i=0;i<smsPdu.Length();i++)
		TESTL(smsPdu[i]==KTestPduRx2[i]);
	TESTL(attrib.iGsmServiceCentre.iTelNumber==KScaNumber2);
	TESTL(attrib.iGsmServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber);
	TESTL(attrib.iGsmServiceCentre.iNumberPlan==RMobilePhone::EIsdnNumberPlan);
	TESTL(attrib.iStatus==RMobileSmsMessaging::EMtMessageStored);
	//We wont be notified of store event as message store full	

	//set to new receive mode
	rxMode = RMobileSmsMessaging::EReceiveUnstoredPhoneAck;
	iSmsMessaging.SetReceiveMode(stat0,rxMode);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	
	INFO_PRINTF1(_L("Testing getReceiveMode"));
	iSmsMessaging.GetReceiveMode(rxMode);
	TESTL(rxMode==RMobileSmsMessaging::EReceiveUnstoredPhoneAck);

	iSmsMessaging.ReceiveMessage(stat0,smsPdu,attribPckg);
	INFO_PRINTF1(_L("3rd receive Request sent"));
	
	//test message 3
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	INFO_PRINTF1(_L("Message 3 received"));
	for(i=0;i<smsPdu.Length();i++)
		TESTL(smsPdu[i]==KTestPduRx3[i]);
	TESTL(attrib.iGsmServiceCentre.iTelNumber==KScaNumber3);
	TESTL(attrib.iGsmServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber);
	TESTL(attrib.iGsmServiceCentre.iNumberPlan==RMobilePhone::EIsdnNumberPlan);
	TESTL(attrib.iStatus==RMobileSmsMessaging::EMtMessageUnstoredPhoneAck);

	//test change receive mode in invalid smsTx state
	rxMode = RMobileSmsMessaging::EReceiveUnstoredClientAck;
	iSmsMessaging.SetReceiveMode(stat0,rxMode);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	iSmsMessaging.ReceiveMessage(statReceive,smsPdu,attribPckg);
	INFO_PRINTF1(_L("4th receive Request sent"));

	User::After(3000000L);
	rxMode = RMobileSmsMessaging::EReceiveUnstoredPhoneAck;
	iSmsMessaging.SetReceiveMode(stat0,rxMode);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNotSupported);//can't change mode in this state

	rxMode = RMobileSmsMessaging::EReceiveStored;
	iSmsMessaging.SetReceiveMode(stat0,rxMode);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNotSupported);//can't change mode in this state
	
	User::WaitForRequest(statReceive);

	iSmsStore.Close();
	iSmsMessaging.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

