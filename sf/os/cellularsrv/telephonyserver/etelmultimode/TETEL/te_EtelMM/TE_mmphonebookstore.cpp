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
#include "TE_mmphonebookstore.h"
#include "testdef.h"

CTestPhonebookStore::CTestPhonebookStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestPhonebookStore"));
	}

enum TVerdict CTestPhonebookStore::doTestStepL()
//
// Test Phonebook Functions
//
    {
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Phone Book Number Storage Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);
	
	RMobilePhoneBookStore phoneBookStore;
	ret=phoneBookStore.Open(mmPhone,DMMTSY_PHONEBOOK_STORE_NAME);
    TEST(ret==KErrNone);
	phoneBookStore.Close();
	
	//Test the overloaded method also.
	ret=phoneBookStore.Open(mmPhone,DMMTSY_PHONEBOOK_STORE_NAME, DMMTSY_PHONEBOOK_TYPE);
    TEST(ret==KErrNone);

	TRequestStatus reqStatus;

	// Get Network store information

	// asynchronous

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 asyncInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg asyncInfoPckg(asyncInfo);
	phoneBookStore.GetInfo(reqStatus, asyncInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidatePhoneBookInfo(asyncInfo);

	// asynchronous & cancel

	phoneBookStore.GetInfo(reqStatus, asyncInfoPckg);
	phoneBookStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::GetInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
		
	else
		{
		ValidatePhoneBookInfo(asyncInfo);
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::GetInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
	
	// asynchronous

	RMobilePhoneBookStore::TMobilePhoneBookInfoV2 asyncV2Info;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV2Pckg asyncV2InfoPckg(asyncV2Info);
	phoneBookStore.GetInfo(reqStatus, asyncV2InfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidatePhoneBookInfo(asyncV2Info);

	// asynchronous & cancel

	phoneBookStore.GetInfo(reqStatus, asyncV2InfoPckg);
	phoneBookStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::GetInfo V2Table (async & cancel (Cancelled Request)) passed"), iTestCount++);
		
	else
		{
		ValidatePhoneBookInfo(asyncV2Info);
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::GetInfo V2Table (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}
		
	// asynchronous V5
 	RMobilePhoneBookStore::TMobilePhoneBookInfoV5 asyncV5Info;
 	RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg asyncV5InfoPckg(asyncV5Info);
 	phoneBookStore.GetInfo(reqStatus, asyncV5InfoPckg);
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus.Int()==KErrNone);
 	ValidatePhoneBookInfo(asyncV5Info);
 
 	// asynchronous V5 & cancel
 	phoneBookStore.GetInfo(reqStatus, asyncV5InfoPckg);
 	phoneBookStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
 	if (reqStatus.Int()==KErrCancel)
 		{
 		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::GetInfo V5Info (async & cancel (Cancelled Request)) passed"), iTestCount++);	
 		}
 		
 	else
 		{
 		ValidatePhoneBookInfo(asyncV5Info);
 		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::GetInfo V5Info (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
 		}	

//	test.Next(_L("OK: RMobilePhoneBookStore's Get Info"));

	// Read one entry

	// asynchronous

	TMobilePhoneBookData aPBData; 
	TInt index(DMMTSY_PHONEBOOK_STORE_INDEX1);
	TInt numOfEntries(DMMTSY_PHONE_ONE_ENTRY);

	phoneBookStore.Read(reqStatus,index,numOfEntries,aPBData);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	ValidatePhoneBookRead(index,numOfEntries,aPBData);

	// cancel
	phoneBookStore.Read(reqStatus,index,numOfEntries,aPBData);
	phoneBookStore.CancelAsyncRequest(EMobilePhoneBookStoreRead);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		{
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::Read (async & cancel (Cancelled Request)) passed"), iTestCount++);	
		}
	else
		{
		ValidatePhoneBookRead(index,numOfEntries,aPBData);
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::Read (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobilePhoneBookStore's Read Entry"));

	// Write one entry

	TMobilePhoneBookData aPBData2;

	InitPhoneBookEntry(numOfEntries,aPBData2);

	// asynchronous

	index = DMMTSY_PHONEBOOK_STORE_INDEX2;
	numOfEntries = DMMTSY_PHONE_ONE_ENTRY;

	phoneBookStore.Write(reqStatus, aPBData2, index);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(index==DMMTSY_PHONEBOOK_STORE_INDEX1);

	// aynchronous & cancel

	index=DMMTSY_PHONEBOOK_STORE_INDEX2;
	phoneBookStore.Write(reqStatus, aPBData2, index);
	phoneBookStore.CancelAsyncRequest(EMobilePhoneBookStoreWrite);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)
		{
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::Write (async & cancel (Cancelled Request)) passed"), iTestCount++);
		}
	else
		{
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::Write (async & cancel (Request Not Cancelled)) passed"), iTestCount++);		
		}

//	test.Next(_L("OK: RMobilePhoneBookStore's Write Entry"));

	// Delete one entry

	// asynchronous	

	phoneBookStore.Delete(reqStatus, DMMTSY_PHONEBOOK_STORE_INDEX1);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// cancel

	phoneBookStore.Delete(reqStatus, DMMTSY_PHONEBOOK_STORE_INDEX1);
	phoneBookStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)		
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::Delete (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::Delete (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
	
	// Get a number of Phonebook entries

	// asynchronous

	TMobilePhoneBookData aPBMultipleData; 
	index = DMMTSY_PHONEBOOK_STORE_INDEX1; // Starting index
	numOfEntries = DMMTSY_PHONE_NUM_OF_ENTRIES; // Number of entries to be read

	phoneBookStore.Read(reqStatus, index, numOfEntries, aPBMultipleData);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::Read (batch) (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		ValidatePhoneBookMultipleRead(index, numOfEntries, aPBMultipleData);
		INFO_PRINTF2(_L("Test %d - RMobilePhoneBookStore::Read (batch)(async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	phoneBookStore.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

void CTestPhonebookStore::ValidatePhoneBookInfo(const RMobilePhoneBookStore::TMobilePhoneBookInfoV1 &aInfo)
	{
	TEST(aInfo.iName.Compare(DMMTSY_PHONEBOOK_STORE_NAME) == 0);
	TEST(aInfo.iType == DMMTSY_PHONEBOOK_STORE_TYPE);
	TEST(aInfo.iTotalEntries == DMMTSY_PHONEBOOK_STORE_ENTRIES);
	TEST(aInfo.iUsedEntries == DMMTSY_PHONEBOOK_STORE_USED);
	TEST(aInfo.iCaps == DMMTSY_PHONEBOOK_STORE_CAPS);
	TEST(aInfo.iMaxNumLength == DMMTSY_PHONEBOOK_MAXNL);
	TEST(aInfo.iMaxTextLength == DMMTSY_PHONEBOOK_MAXTL);
	TEST(aInfo.iLocation == DMMTSY_PHONEBOOK_LOCATION);
	TEST(aInfo.iChangeCounter == DMMTSY_PHONEBOOK_CHANGECOUNT);
	TEST(aInfo.iIdentity == DMMTSY_PHONEBOOK_IDENTITY);
	TEST(aInfo.ExtensionId()==RMobilePhoneStore::KETelMobilePhonebookStoreV1);
	}

void CTestPhonebookStore::ValidatePhoneBookInfo(const RMobilePhoneBookStore::TMobilePhoneBookInfoV2 &aInfo)
	{
	TEST(aInfo.iName.Compare(DMMTSY_PHONEBOOK_STORE_NAME) == 0);
	TEST(aInfo.iType == DMMTSY_PHONEBOOK_STORE_TYPE);
	TEST(aInfo.iTotalEntries == DMMTSY_PHONEBOOK_STORE_ENTRIES);
	TEST(aInfo.iUsedEntries == DMMTSY_PHONEBOOK_STORE_USED);
	TEST(aInfo.iCaps == DMMTSY_PHONEBOOK_STORE_CAPS);
	TEST(aInfo.iMaxNumLength == DMMTSY_PHONEBOOK_MAXNL);
	TEST(aInfo.iMaxTextLength == DMMTSY_PHONEBOOK_MAXTL);
	TEST(aInfo.iLocation == DMMTSY_PHONEBOOK_LOCATION);
	TEST(aInfo.iChangeCounter == DMMTSY_PHONEBOOK_CHANGECOUNT);
	TEST(aInfo.iIdentity == DMMTSY_PHONEBOOK_IDENTITY);
	TEST(aInfo.iPhBkMode.Compare(DMMTSY_PHONEBOOK_TYPE) == 0);
	}

void CTestPhonebookStore::ValidatePhoneBookInfo(const RMobilePhoneBookStore::TMobilePhoneBookInfoV5 &aInfo)
	{
	TEST(aInfo.iName.Compare(DMMTSY_PHONEBOOK_STORE_NAME) == 0);
	TEST(aInfo.iType == DMMTSY_PHONEBOOK_STORE_TYPE);
	TEST(aInfo.iTotalEntries == DMMTSY_PHONEBOOK_STORE_ENTRIES);
	TEST(aInfo.iUsedEntries == DMMTSY_PHONEBOOK_STORE_USED);
	TEST(aInfo.iCaps == DMMTSY_PHONEBOOK_STORE_CAPS);
	TEST(aInfo.iMaxNumLength == DMMTSY_PHONEBOOK_MAXNL);
	TEST(aInfo.iMaxTextLength == DMMTSY_PHONEBOOK_MAXTL);
	TEST(aInfo.iLocation == DMMTSY_PHONEBOOK_LOCATION);
	TEST(aInfo.iChangeCounter == DMMTSY_PHONEBOOK_CHANGECOUNT);
	TEST(aInfo.iIdentity == DMMTSY_PHONEBOOK_IDENTITY);
	TEST(aInfo.iPhBkMode.Compare(DMMTSY_PHONEBOOK_TYPE) == 0);
	TEST(aInfo.iMaxSecondNames == DMMTSY_PHONEBOOK_MAXSECONDNAMES);
	TEST(aInfo.iMaxTextLengthSecondName == DMMTSY_PHONEBOOK_MAXTEXTLENSECONDNAME);
	TEST(aInfo.iMaxAdditionalNumbers == DMMTSY_PHONEBOOK_MAXADDNUM);
	TEST(aInfo.iMaxTextLengthAdditionalNumber == DMMTSY_PHONEBOOK_MAXTEXTLENADDNUM);
	TEST(aInfo.iMaxNumLengthAdditionalNumber == DMMTSY_PHONEBOOK_MAXNUMLENADDNUM);
	TEST(aInfo.iMaxGroupNames == DMMTSY_PHONEBOOK_MAXGROUPNAMES);
	TEST(aInfo.iMaxTextLengthGroupName == DMMTSY_PHONEBOOK_MAXTEXTLENGROUPNAME);
	TEST(aInfo.iMaxEmailAddr == DMMTSY_PHONEBOOK_MAXEMAILADDR);
	TEST(aInfo.iMaxTextLengthEmailAddr == DMMTSY_PHONEBOOK_MAXTEXTLENEMAILADDR);
	}

void CTestPhonebookStore::ValidatePhoneBookRead(const TInt aIndex, const TInt aNumEntries, const TMobilePhoneBookData &aPBData)
	{
	TEST(aIndex == DMMTSY_PHONEBOOK_STORE_INDEX1);
	TEST(aNumEntries == DMMTSY_PHONE_ONE_ENTRY);
	TEST(aPBData.Compare(DMMTSY_PHONE_BOOK_DATA1) == 0);
	}

void CTestPhonebookStore::InitPhoneBookEntry(TInt &aNumOfEntries, TMobilePhoneBookData &aPBData)
	{
	aNumOfEntries = DMMTSY_PHONE_ONE_ENTRY;
	aPBData = DMMTSY_PHONE_BOOK_DATA2;
	}

void CTestPhonebookStore::ValidatePhoneBookMultipleRead(const TInt aIndex, const TInt aNumEntries, const TMobilePhoneBookData &aPBData)
	{
	TEST(aIndex == DMMTSY_PHONEBOOK_STORE_INDEX1);
	TEST(aNumEntries == DMMTSY_PHONE_NUM_OF_ENTRIES);
	TEST(aPBData.Compare(DMMTSY_PHONE_BOOK_MULTIPLE_DATA) == 0);
	}



