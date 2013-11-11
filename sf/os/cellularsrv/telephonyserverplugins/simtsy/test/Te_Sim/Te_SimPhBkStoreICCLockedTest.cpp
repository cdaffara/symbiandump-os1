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

#include "Te_SimPhBkStoreICCLockedTest.h"

CSimPhBkStoreICCLockedTest::CSimPhBkStoreICCLockedTest() 
	{ 
	SetTestStepName(_L("StoreICCLockedTest"));
	}

TVerdict CSimPhBkStoreICCLockedTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkStoreICCLockedTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(3);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

 	INFO_PRINTF1(_L("Testing Get Phonebook store info from Phone level."));
	TRequestStatus stat0, statNotSecu;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneInfoPckg(phoneInfo);
	iPhone.GetPhoneStoreInfo(stat0,phoneInfoPckg,KETelIccAdnPhoneBook);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(phoneInfo.iType==RMobilePhoneStore::EPhoneBookStore);
	TESTL(phoneInfo.iTotalEntries==100);
	TESTL(phoneInfo.iUsedEntries==2);
	TESTL(phoneInfo.iCaps==KPhBkStoreCaps);
	TESTL(phoneInfo.iName.Match(KETelIccAdnPhoneBook)==0);

	TESTL(iPhonebook.Open(iPhone,KETelIccAdnPhoneBook)==KErrNone);
	INFO_PRINTF1(_L("Opened Phonebook object"));
	
	INFO_PRINTF1(_L("Retrieve Phonebook Information "));
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 info;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg infoPckg(info);
	iPhonebook.GetInfo(stat0,infoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrAccessDenied);

	//unsuccessful read because the ICC is locked(timer)
	TInt numEntries=1;
	TBuf8<100> phBkBuf;
	iPhonebook.Read(stat0,0,numEntries,phBkBuf);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrAccessDenied);
	
	//Enter Pin1 to unlock the phone
	RMobilePhone::TMobilePhoneSecurityCode type=RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePassword pass,badpass;
	_LIT8(pswrd, "1111");
	_LIT8(badpswrd, "2222");
	pass.Copy(pswrd);
	badpass.Copy(badpswrd);

	RMobilePhone::TMobilePhoneSecurityEvent event;
	iPhone.NotifySecurityEvent(statNotSecu, event);
	iPhone.VerifySecurityCode(stat0, type,badpass,badpass);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrArgument);	
	iPhone.VerifySecurityCode(stat0, type, pass,pass);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);	
	User::WaitForRequest(statNotSecu);
	TESTL(event==RMobilePhone::EPin1Verified);

	type=RMobilePhone::ESecurityCodePin2;
	iPhone.NotifySecurityEvent(statNotSecu, event);
	iPhone.VerifySecurityCode(stat0, type,badpass,badpass);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrArgument);	
	iPhone.VerifySecurityCode(stat0, type, pass,pass);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);	
	User::WaitForRequest(statNotSecu);
	TESTL(event==RMobilePhone::EPin2Verified);

	type=RMobilePhone::ESecurityHiddenKey;
	iPhone.NotifySecurityEvent(statNotSecu, event);
	iPhone.VerifySecurityCode(stat0, type,badpass,badpass);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrArgument);	
	iPhone.VerifySecurityCode(stat0, type, pass,pass);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);	
	User::WaitForRequest(statNotSecu);
	TESTL(event==RMobilePhone::EHiddenKeyVerified);

	iPhonebook.Read(stat0,0,numEntries,phBkBuf);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);

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

