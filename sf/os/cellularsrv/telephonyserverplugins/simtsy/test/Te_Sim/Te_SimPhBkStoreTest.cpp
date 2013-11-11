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

#include "Te_SimPhBkStoreTest.h"

CSimPhBkStoreTest::CSimPhBkStoreTest() 
	{ 
	SetTestStepName(_L("PhBkStoreTest"));
	}

TVerdict CSimPhBkStoreTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkStoreTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(7);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	
	TUint32 caps;
	INFO_PRINTF1(_L("Testing Get ICC Access Capabilites from Phone level."));
	iPhone.GetIccAccessCaps(caps);
	TESTL(caps==7);

	INFO_PRINTF1(_L("Testing Get Phonebook store info from Phone level."));
	TRequestStatus stat0,stat1;
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

	iPhone.GetPhoneStoreInfo(stat0,phoneInfoPckg,KETelIccFdnPhoneBook);
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrNotFound, CHP_PHBK_CASE("F.8"));

	ret=iPhonebook.Open(iPhone,KETelIccAdnPhoneBook);
	CHECKPOINT_EXL(ret==KErrNone, CHP_PHBK_CASE("F.1"));
	INFO_PRINTF1(_L("Opened Phonebook object"));

	INFO_PRINTF1(_L("Retrieve Phonebook Information"));
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 info;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg infoPckg(info);
	iPhonebook.GetInfo(stat0,infoPckg);
	User::WaitForRequest(stat0);
	CHECKPOINT_EXL(stat0==KErrNone, CHP_PHBK_CASE("F.7"));
	
	INFO_PRINTF1(_L("Testing Phonebook Read"));
	TESTL(info.iMaxNumLength==50);
	TESTL(info.iMaxTextLength==60);
	TESTL(info.iLocation==RMobilePhoneBookStore::ELocationIccMemory);
	TESTL(info.iChangeCounter==0);
	TESTL(info.iType==RMobilePhoneStore::EPhoneBookStore);
	TESTL(info.iTotalEntries==100);
	TESTL(info.iUsedEntries==2);
	TESTL(info.iCaps==KPhBkStoreCaps);
	TESTL(info.iName.Match(KETelIccAdnPhoneBook)==0);

	TInt numEntries=1;
	TBuf8<100> phBkBuf;
	iPhonebook.Read(stat0,0,numEntries,phBkBuf);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	CHECKPOINTL(stat0, KErrNone, CHP_PHBK_CASE("F.3"));

	TESTL(phBkBuf.Length()==sizeof(KTestPhonebookA));
	for(TInt i=0;i<phBkBuf.Length();i++)
	    {
	    TESTL(phBkBuf[i]==KTestPhonebookA[i]);
	    }
		

	INFO_PRINTF1(_L("Retrieve V5 Phonebook Information"));
 	RMobilePhoneBookStore::TMobilePhoneBookInfoV5 asyncV5Info;
 	RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg asyncV5InfoPckg(asyncV5Info);
 	iPhonebook.GetInfo(stat0, asyncV5InfoPckg);
 	User::WaitForRequest(stat0);
 	CHECKPOINTL(stat0, KErrNone, CHP_PHBK_CASE("F.4"));
 	
 	TESTL(asyncV5Info.iMaxNumLength==50);
 	TESTL(asyncV5Info.iMaxTextLength==60);
 	TESTL(asyncV5Info.iLocation==RMobilePhoneBookStore::ELocationIccMemory);
 	TESTL(asyncV5Info.iChangeCounter==0);
 	TESTL(asyncV5Info.iType==RMobilePhoneStore::EPhoneBookStore);
 	TESTL(asyncV5Info.iTotalEntries==100);
 	TESTL(asyncV5Info.iUsedEntries==2);
 	TESTL(asyncV5Info.iCaps==KPhBkStoreCaps);
 	TESTL(asyncV5Info.iName.Match(KETelIccAdnPhoneBook)==0);
 	TESTL(asyncV5Info.iPhBkMode.Length()==0);
 	TESTL(asyncV5Info.iMaxSecondNames==0);
 	TESTL(asyncV5Info.iMaxTextLengthSecondName==0);
 	TESTL(asyncV5Info.iMaxAdditionalNumbers==0);
 	TESTL(asyncV5Info.iMaxTextLengthAdditionalNumber==0);
 	TESTL(asyncV5Info.iMaxNumLengthAdditionalNumber==0);
 	TESTL(asyncV5Info.iMaxGroupNames==0);
 	TESTL(asyncV5Info.iMaxTextLengthGroupName==0);
 	TESTL(asyncV5Info.iMaxEmailAddr==0);
 	TESTL(asyncV5Info.iMaxTextLengthEmailAddr==0);

	INFO_PRINTF1(_L("Testing Phonebook Write"));
	TUint32 notifyEvent;
	TInt notifyIndex;
	iPhonebook.NotifyStoreEvent(stat1,notifyEvent,notifyIndex);
	TInt index=7;
	phBkBuf.Copy(KTestPhonebookB,sizeof(KTestPhonebookB));
	iPhonebook.Write(stat0,phBkBuf,index);
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrNone, CHP_PHBK_CASE("F.5"));

	User::WaitForRequest(stat1);
	CHECKPOINTL(stat1, KErrNone, CHP_PHBK_CASE("F.13"));
	TESTL(notifyEvent==(RMobilePhoneStore::KStoreHasSpace |
						RMobilePhoneStore::KStoreEntryAdded));
	TESTL(notifyIndex==7);

	
	INFO_PRINTF1(_L("Testing Phonebook Out of Bounds Write"));
	index=101;
	iPhonebook.Write(stat0,phBkBuf,index);
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrArgument, CHP_PHBK_CASE("F.6"));

	//attept to write a name that is too long
	INFO_PRINTF1(_L("Write an entry with too long name"));
	TBuf8<300> phBigBuf;
	index=5;
	phBigBuf.Copy(KTestPhonebookBigName,sizeof(KTestPhonebookBigName));
	iPhonebook.Write(stat0,phBigBuf,index);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrOverflow);

	//attempt to write a number that is too long
	INFO_PRINTF1(_L("Write an entry with too long number"));
	phBigBuf.Copy(KTestPhonebookBigNumber,sizeof(KTestPhonebookBigNumber));
	iPhonebook.Write(stat0,phBigBuf,index);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrOverflow);

	INFO_PRINTF1(_L("Retrieve Phonebook Information again"));
	iPhonebook.GetInfo(stat0,infoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);

	INFO_PRINTF1(_L("Testing Phonebook Read"));
	TESTL(info.iMaxNumLength==50);
	TESTL(info.iMaxTextLength==60);
	TESTL(info.iLocation==RMobilePhoneBookStore::ELocationIccMemory);
	TESTL(info.iChangeCounter==0);
	TESTL(info.iType==RMobilePhoneStore::EPhoneBookStore);
	TESTL(info.iTotalEntries==100);
	TESTL(info.iUsedEntries==3);
	TESTL(info.iCaps==KPhBkStoreCaps);
	TESTL(info.iName.Match(KETelIccAdnPhoneBook)==0);

	INFO_PRINTF1(_L("Testing Individual Delete"));
	iPhonebook.NotifyStoreEvent(stat1,notifyEvent,notifyIndex);
	iPhonebook.Delete(stat0,7);
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrNone, CHP_PHBK_CASE("F.9"));
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);
	TESTL(notifyEvent==(RMobilePhoneStore::KStoreHasSpace |
						RMobilePhoneStore::KStoreEntryDeleted));
	TESTL(notifyIndex==7);

	iPhonebook.GetInfo(stat0,infoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(info.iUsedEntries==2);

	INFO_PRINTF1(_L("Testing Delete All"));
	iPhonebook.NotifyStoreEvent(stat1,notifyEvent,notifyIndex);
	iPhonebook.DeleteAll(stat0);
	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrNone, CHP_PHBK_CASE("F.11"));

	iPhonebook.GetInfo(stat0,infoPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	TESTL(info.iUsedEntries==0);
	
	User::WaitForRequest(stat1);
	TESTL(stat1==KErrNone);
	TESTL(notifyEvent==(RMobilePhoneStore::KStoreHasSpace |
						RMobilePhoneStore::KStoreEmpty |
						RMobilePhoneStore::KStoreEntryDeleted));
	TESTL(notifyIndex==-1);
	
	iPhonebook.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

