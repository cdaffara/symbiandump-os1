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
#include "TE_mmonstore.h"
#include "testdef.h"

CTestOnStore::CTestOnStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestOnStore"));
	}

enum TVerdict CTestOnStore::doTestStepL()
//
// Test ON Storage Functions
//
    {
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Own Number Storage Functionality"));
//	LOGTEXT(_L8("Test Mobile Own Number Storage Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileONStore onStore;
	ret=onStore.Open(mmPhone);
    TEST(ret==KErrNone);

	TRequestStatus reqStatus;
	

	// Get ON store information

	// asynchronous

	RMobileONStore::TMobileONStoreInfoV1 asyncInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg asyncInfoPckg(asyncInfo);
	onStore.GetInfo(reqStatus, asyncInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateONInfo(asyncInfo);

	// asynchronous & cancel

	onStore.GetInfo(reqStatus, asyncInfoPckg);
	onStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileONStore::GetInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
		
	else
		{
		ValidateONInfo(asyncInfo);
		INFO_PRINTF2(_L("Test %d - RMobileONStore::GetInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileONStore's Get Info"));

	
	// Read one entry

	RMobileONStore::TMobileONEntryV1 asyncEntry1;

	// asynchronous

	asyncEntry1.iIndex = DMMTSY_ON_STORE_INDEX1;
	RMobileONStore::TMobileONEntryV1Pckg asyncEntry1Pckg(asyncEntry1);

	onStore.Read(reqStatus,asyncEntry1Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	ValidateONRead(asyncEntry1);


	// cancel

	asyncEntry1.iIndex = DMMTSY_ON_STORE_INDEX1;
	onStore.Read(reqStatus, asyncEntry1Pckg);
	onStore.CancelAsyncRequest(EMobilePhoneStoreRead);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileONStore::Read (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		ValidateONRead(asyncEntry1);
		INFO_PRINTF2(_L("Test %d - RMobileONStore::Read (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileONStore's Read Entry"));

	// Write one entry

	RMobileONStore::TMobileONEntryV1 entry2;
	RMobileONStore::TMobileONEntryV1Pckg entry2Pckg(entry2);

	InitONEntry(entry2);

	// asynchronous

	entry2.iIndex=DMMTSY_ON_STORE_INDEX2;
	onStore.Write(reqStatus, entry2Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// aynchronous & cancel

	onStore.Write(reqStatus, entry2Pckg);
	onStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileONStore::Write (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileONStore::Write (async & cancel (Request Not Cancelled)) passed"), iTestCount++);		

//	test.Next(_L("OK: RMobileONStore's Write Entry"));

	// Delete one entry

	// asynchronous	

	onStore.Delete(reqStatus, DMMTSY_ON_STORE_INDEX1);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// cancel

	onStore.Delete(reqStatus, DMMTSY_ON_STORE_INDEX1);
	onStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)		
		INFO_PRINTF2(_L("Test %d - RMobileONStore::Delete (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileONStore::Delete (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Delete all entries

	// asynchronous

	onStore.DeleteAll(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	onStore.DeleteAll(reqStatus);
	onStore.CancelAsyncRequest(EMobilePhoneStoreDeleteAll);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileONStore::DeleteAll (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileONStore::DeleteAll (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileonStore's Delete Entries"));

	
	// Notify Entry Changed

	// asynchronous
	TInt index;

	TUint32 event;
	onStore.NotifyStoreEvent(reqStatus, event, index);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(index==DMMTSY_ON_STORE_INDEX2);
	TEST(event==DMMTSY_ON_STORE_EVENT);

	// asychronous & cancel

	onStore.NotifyStoreEvent(reqStatus, event, index);
	onStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileONStore::NotifyStoreEvent (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(index==DMMTSY_ON_STORE_INDEX2);
		TEST(event==DMMTSY_ON_STORE_EVENT);
		INFO_PRINTF2(_L("Test %d - RMobileONStore::NotifyStoreEvent (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileONStore's Notify Store Event change"));


	// Read all ON entries

	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneONList* retrieve=CRetrieveMobilePhoneONList::NewL(onStore);
    retrieve->Start(reqStatus);
    delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus == KErrCancel);

	// Get ON list using asynchronous AO version
	CTestGetONList* getONList=CTestGetONList::NewLC(onStore, this);
    getONList->Start();
    CActiveScheduler::Start();

	// Ensure that the same AO can be used to retrieve multiple lists.

    getONList->Start();
    CActiveScheduler::Start();

	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy();  // getONList

	// Now test the cancelling

	getONList=CTestGetONList::NewLC(onStore, this);
    getONList->Start();

	CTestCanceller* cancelGetONList = getONList->RetrieveCancelPtr();
	cancelGetONList->Call();

    CActiveScheduler::Start();
	TBool cancelled = (getONList->iStatus == KErrCancel);

    getONList->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileONStore::ReadAll (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileONStore::ReadAll (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);


    CleanupStack::PopAndDestroy();  // getONList

//	test.Next(_L("OK: RMobileONStore's Read All Entries"));

	// Storing a new version of the list


	RMobileONStore::TMobileONEntryV1 storeEntry;

	CMobilePhoneONList* onList = CMobilePhoneONList::NewL();
	CleanupStack::PushL(onList);

	InitONEntry(storeEntry);
	storeEntry.iIndex = DMMTSY_ON_STORE_INDEX1;

	onList->AddEntryL(storeEntry);
	storeEntry.iIndex = DMMTSY_ON_STORE_INDEX2;
	onList->AddEntryL(storeEntry);

	// asynchronous

	onStore.StoreAllL(reqStatus, onList);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// aynchronous & cancel

	onStore.StoreAllL(reqStatus, onList);
	onStore.CancelAsyncRequest(EMobileONStoreStoreAll);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	cancelled = (reqStatus.Int()==KErrCancel);

	// Ensure that a Store operation can be performed after the previous store had been cancelled.

	onStore.StoreAllL(reqStatus, onList);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	
	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileONStore::StoreAll (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileONStore::StoreAll (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	CleanupStack::PopAndDestroy(); // namList

//	test.Next(_L("OK: RMobileONStore's Store All Entries"));

	onStore.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));
	
	return TestStepResult();
	}

void CTestOnStore::ValidateONInfo(const RMobileONStore::TMobileONStoreInfoV1 &aInfo)
	{
	TEST(aInfo.iName.Compare(DMMTSY_ON_STORE_NAME) == 0);
	TEST(aInfo.iType==DMMTSY_ON_STORE_TYPE);
	TEST(aInfo.iTotalEntries==DMMTSY_ON_STORE_ENTRIES);
	TEST(aInfo.iUsedEntries==DMMTSY_ON_STORE_USED);
	TEST(aInfo.iCaps==DMMTSY_ON_STORE_CAPS);

	TEST(aInfo.iNumberLen == DMMTSY_ON_STORE_INFO_GSM_MAX_NUM);
	TEST(aInfo.iTextLen == DMMTSY_ON_STORE_INFO_GSM_MAX_TEXT);
	TEST(aInfo.ExtensionId()==RMobilePhoneStore::KETelMobileONStoreV1);
	}

void CTestOnStore::ValidateONEntry(const RMobileONStore::TMobileONEntryV1 &aEntry)
	{
	TEST(aEntry.iMode == DMMTSY_ON_STORE_ENTRY_MODE);
	TEST(aEntry.iText.Compare(DMMTSY_ON_STORE_ENTRY_TEXT) == 0);
	TEST(aEntry.iNumber.iTypeOfNumber == DMMTSY_PHONE_TON);
	TEST(aEntry.iNumber.iNumberPlan == DMMTSY_PHONE_NPI);
	TEST(aEntry.iNumber.iTelNumber.Compare(DMMTSY_PHONE_TEL_NUMBER) == 0);
	TEST(aEntry.iService == DMMTSY_ON_STORE_ENTRY_SERVICE);
	}

void CTestOnStore::ValidateONRead(const RMobileONStore::TMobileONEntryV1 &aEntry)
	{
	ValidateONEntry(aEntry);
	}

void CTestOnStore::InitONEntry(RMobileONStore::TMobileONEntryV1& aEntry)
	{
	aEntry.iMode = DMMTSY_ON_STORE_ENTRY_MODE;
	aEntry.iText = DMMTSY_ON_STORE_ENTRY_TEXT;
	aEntry.iNumber.iTypeOfNumber = DMMTSY_PHONE_TON;
	aEntry.iNumber.iNumberPlan = DMMTSY_PHONE_NPI;
	aEntry.iNumber.iTelNumber = DMMTSY_PHONE_TEL_NUMBER;
	aEntry.iService = DMMTSY_ON_STORE_ENTRY_SERVICE;
	}

/**************************************************************/
//
// Testing asynchronous retrieve of ON list
//
/**************************************************************/

CTestOnStore::CTestGetONList* CTestOnStore::CTestGetONList::NewLC(RMobilePhoneStore& aStore, CTestOnStore* aTestOnStore)
	{
    CTestGetONList* r=new(ELeave) CTestGetONList(aStore, aTestOnStore);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestOnStore::CTestGetONList::CTestGetONList(RMobilePhoneStore& aStore, CTestOnStore* aTestOnStore)
    : CActive(EPriorityNormal), iStore(aStore), iTestOnStore(aTestOnStore)
    {}
      

void CTestOnStore::CTestGetONList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneONList::NewL(iStore);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestOnStore::CTestGetONList::~CTestGetONList()
    {
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestOnStore::CTestGetONList::Start()
    {
    iRetrieve->Start(iStatus);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus);
	User::WaitForRequest(tmpStatus);
	iTestOnStore->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }       
      
CTestCanceller* CTestOnStore::CTestGetONList::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestOnStore::CTestGetONList::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}
      

void CTestOnStore::CTestGetONList::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      

TInt CTestOnStore::CTestGetONList::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneONList* onList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, onList=iRetrieve->RetrieveListL(););

	if (onList)
		{
		iTestOnStore->TEST(onList->Enumerate()==DMMTSY_ON_STORE_COUNT);

		// Get each detected network entry
		RMobileONStore::TMobileONEntryV1 entry;

		for (TInt i=0; i<DMMTSY_ON_STORE_COUNT; ++i)
			{
			TRAP(ret,entry=onList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestOnStore->ValidateONEntry(entry);
					iTestOnStore->TEST(entry.iIndex==DMMTSY_ON_STORE_INDEX1);
					break;
					}
				case 1:
					{
					iTestOnStore->ValidateONEntry(entry);
					iTestOnStore->TEST(entry.iIndex==DMMTSY_ON_STORE_INDEX2);
					break;
					}
				default:
					break;
				}
			}
		delete onList;
		}

   return ret;
   }
