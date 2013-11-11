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

#include "Te_SimPhBkStoreUICCLockedTest.h"

CSimPhBkStoreUICCLockedTest::CSimPhBkStoreUICCLockedTest() 
	{ 
	SetTestStepName(_L("StoreUICCLockedTest"));
	}

TVerdict CSimPhBkStoreUICCLockedTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkStoreUICCLockedTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(3);

	TInt ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("return code: %d"),ret);
	TESTL(ret==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));
 	INFO_PRINTF1(_L("Testing Get Phonebook store info from Phone level."));
	TRequestStatus stat0, statNotSecu;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneInfoPckg(phoneInfo);
	iPhone.GetPhoneStoreInfo(stat0,phoneInfoPckg,KETelIccAdnPhoneBook, KEtelUSimPhoneBookType);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(phoneInfo.iType==RMobilePhoneStore::EPhoneBookStore);
	TESTL(phoneInfo.iTotalEntries==100);
	TESTL(phoneInfo.iUsedEntries==2);
	TESTL(phoneInfo.iCaps==KPhBkStoreCaps);
	TESTL(phoneInfo.iName.Match(KETelIccAdnPhoneBook)==0);

	TESTL(iPhonebook.Open(iPhone,KETelIccAdnPhoneBook,KEtelUSimPhoneBookType)==KErrNone);
	INFO_PRINTF1(_L("Opened Phonebook object"));
	
	INFO_PRINTF1(_L("Retrieve Phonebook Information "));
	RMobilePhoneBookStore::TMobilePhoneBookInfoV2 info;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV2Pckg infoPckg(info);
	iPhonebook.GetInfo(stat0,infoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrAccessDenied);

	INFO_PRINTF1(_L("Reading phonebook entries from a locked phone (PIN1,HiddenKey)"));	
	TInt numEntries=1;
	TBuf8<1000> phBkBuf;
	iPhonebook.Read(stat0,0,numEntries,phBkBuf);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrAccessDenied);
	
	//Enter Pin1 to unlock the phone
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePassword pass;
	_LIT8(pswrd, "1111");
	pass.Copy(pswrd);

	INFO_PRINTF1(_L("Verifying the PIN1"));
	RMobilePhone::TMobilePhoneSecurityEvent event;
	iPhone.NotifySecurityEvent(statNotSecu, event);
	iPhone.VerifySecurityCode(stat0, type, pass,pass);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);	
	User::WaitForRequest(statNotSecu);
	TESTL(event==RMobilePhone::EPin1Verified);
	
	INFO_PRINTF1(_L("Reading phonebook entries from a locked phone (HiddenKey)"));
	iPhonebook.Read(stat0,1,numEntries,phBkBuf);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);

	//Read the hidden entry
	iPhonebook.Read(stat0,2,numEntries,phBkBuf);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrAccessDenied);

	INFO_PRINTF1(_L("Verifying the HiddenKey"));
	type=RMobilePhone::ESecurityHiddenKey;
	iPhone.NotifySecurityEvent(statNotSecu, event);
	iPhone.VerifySecurityCode(stat0, type, pass,pass);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);	
	User::WaitForRequest(statNotSecu);
	TESTL(event==RMobilePhone::EHiddenKeyVerified);
	
	INFO_PRINTF1(_L("Read the hidden entry"));
	//Read the hidden entry
	iPhonebook.Read(stat0,2,numEntries,phBkBuf);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	
	INFO_PRINTF1(_L("Lock using PIN1"));
	//Timer-based notification
	iPhone.NotifySecurityEvent(statNotSecu, event);
	User::WaitForRequest(statNotSecu);
	TESTL(event==RMobilePhone::EPin1Required);
	
	iPhonebook.Read(stat0,0,numEntries,phBkBuf);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrAccessDenied);
	
	iPhonebook.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

