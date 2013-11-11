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

#include "Te_SimPhBkUSimErrorOOBTest.h"

CSimPhBkUSimErrorOOBTest::CSimPhBkUSimErrorOOBTest() 
	{ 
	SetTestStepName(_L("USimErrorOOBTest"));
	}

TVerdict CSimPhBkUSimErrorOOBTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPhBkUSimErrorOOBTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(1);

  	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
  	INFO_PRINTF1(_L("Opened phone object"));
  
  	INFO_PRINTF1(_L("Testing Get USim Phonebook store info from Phone level."));
  	TRequestStatus stat0;

  	TESTL(iPhonebook.Open(iPhone,KETelIccAdnPhoneBook,KEtelUSimPhoneBookType)==KErrNone);
  	INFO_PRINTF1(_L("Opened Phonebook object"));

  	INFO_PRINTF1(_L("Retrieve Phonebook Information"));
  	RMobilePhoneBookStore::TMobilePhoneBookInfoV2 info;
  	RMobilePhoneBookStore::TMobilePhoneBookInfoV2Pckg infoPckg(info);
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
  	TESTL(info.iUsedEntries==2);
  	TESTL(info.iCaps==KPhBkStoreCaps);
  	TESTL(info.iName.Match(KETelIccAdnPhoneBook)==0);

  	INFO_PRINTF1(_L("Test simulated errors"));
  	iPhonebook.GetInfo(stat0,infoPckg);
  	User::WaitForRequest(stat0);
  	TESTL(stat0==-1000);

  	iPhonebook.GetInfo(stat0,infoPckg);
  	User::WaitForRequest(stat0);
  	TESTL(stat0==KErrNone);

	iPhonebook.GetInfo(stat0,infoPckg);
  	User::WaitForRequest(stat0);
  	TESTL(stat0==-1001);

  	INFO_PRINTF1(_L("Test Out of Band Store"));
  	TUint32 notifyEvent;
  	TInt notifyIndex;
  	iPhonebook.NotifyStoreEvent(stat0,notifyEvent,notifyIndex);
  	User::WaitForRequest(stat0);
  	TESTL(notifyEvent==(RMobilePhoneStore::KStoreHasSpace |
  						RMobilePhoneStore::KStoreEntryAdded));
  	TESTL(notifyIndex==5);
  
  	iPhonebook.GetInfo(stat0,infoPckg);
  	User::WaitForRequest(stat0);
  	TESTL(stat0==KErrNone);
  	TESTL(info.iUsedEntries==3);
  
  	TInt numEntries=1;
  	TBuf8<159> phBkBuf;
  	iPhonebook.Read(stat0,notifyIndex,numEntries,phBkBuf);
  	User::WaitForRequest(stat0);
  	TESTL(stat0==KErrNone);

  	TESTL(phBkBuf.Length()==sizeof(KTestUSimPhonebookC));
  	for(TInt i=0;i<phBkBuf.Length();i++)
  	    {
  		TESTL(phBkBuf[i]==KTestUSimPhonebookC[i]);
  	    }
  	INFO_PRINTF1(_L("Test Out of Band Delete"));
  	iPhonebook.NotifyStoreEvent(stat0,notifyEvent,notifyIndex);
  	User::WaitForRequest(stat0);
  	TESTL(stat0==KErrNone);
  	TESTL(notifyEvent==(RMobilePhoneStore::KStoreHasSpace |
  						RMobilePhoneStore::KStoreEntryDeleted));
  	TESTL(notifyIndex==5);

	iPhonebook.Read(stat0,101,numEntries,phBkBuf);
  	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrArgument, CHP_PHBK_CASE("F.4"));

	iPhonebook.Delete(stat0,101);
  	User::WaitForRequest(stat0);
	CHECKPOINTL(stat0, KErrArgument, CHP_PHBK_CASE("F.10"));

  	iPhonebook.GetInfo(stat0,infoPckg);
  	User::WaitForRequest(stat0);
  	TESTL(stat0==KErrNone);
  	TESTL(info.iUsedEntries==2);

  	iPhonebook.Close(); //CHP_PHBK_CASE("F.12")
  	iPhone.Close();

	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

