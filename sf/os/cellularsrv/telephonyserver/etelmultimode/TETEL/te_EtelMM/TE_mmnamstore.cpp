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
#include "TE_mmnamstore.h"
#include "testdef.h"

CTestNamStore::CTestNamStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestNamStore"));
	}

enum TVerdict CTestNamStore::doTestStepL()
	{
//
// Test Nam Storage Functions
//
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile NAM Storage Functionality"));
//	LOGTEXT(_L8("Test Mobile NAM Storage Functionality"));

	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileNamStore namStore;
	ret=namStore.Open(mmPhone);
    TEST(ret==KErrNone);

	TRequestStatus reqStatus;


	// Get NAM store information

	// asynchronous

	RMobileNamStore::TMobileNamStoreInfoV1 asyncInfo;
	RMobileNamStore::TMobileNamStoreInfoV1Pckg asyncInfoPckg(asyncInfo);
	namStore.GetInfo(reqStatus, asyncInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateNamInfo(asyncInfo);

	// asynchronous & cancel

	namStore.GetInfo(reqStatus, asyncInfoPckg);
	namStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::GetInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateNamInfo(asyncInfo);
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::GetInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileNamStore's Get Info"));

	// Read one entry

	RMobileNamStore::TMobileNamEntryV1 asyncEntry1;

	// asynchronous

	asyncEntry1.iIndex = DMMTSY_NAM_STORE_INDEX1;
	RMobileNamStore::TMobileNamEntryV1Pckg asyncEntry1Pckg(asyncEntry1);

	namStore.Read(reqStatus,asyncEntry1Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	ValidateNamRead(asyncEntry1);


	// cancel

	asyncEntry1.iIndex = DMMTSY_NAM_STORE_INDEX1;
	namStore.Read(reqStatus, asyncEntry1Pckg);
	namStore.CancelAsyncRequest(EMobilePhoneStoreRead);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Read (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateNamRead(asyncEntry1);
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Read (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileNamStore's Read Entry"));

	// Write one entry

	RMobileNamStore::TMobileNamEntryV1 entry2;
	RMobileNamStore::TMobileNamEntryV1Pckg entry2Pckg(entry2);

	InitNamEntry(entry2);

	// asynchronous

	entry2.iIndex=DMMTSY_NAM_STORE_INDEX2;
	namStore.Write(reqStatus, entry2Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// aynchronous & cancel

	namStore.Write(reqStatus, entry2Pckg);
	namStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Write (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Write (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileNamStore's Write Entry"));

	// Delete one entry

	// asynchronous

	namStore.Delete(reqStatus, DMMTSY_NAM_STORE_INDEX1);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// cancel

	namStore.Delete(reqStatus, DMMTSY_NAM_STORE_INDEX1);
	namStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Delete (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Delete (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Delete all entries

	// asynchronous

	namStore.DeleteAll(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	namStore.DeleteAll(reqStatus);
	namStore.CancelAsyncRequest(EMobilePhoneStoreDeleteAll);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::DeleteAll (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::DeleteAll (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileNamStore's Delete Entries"));


	// Notify Entry Changed

	// asynchronous
	TInt index;

	TUint32 event;
	namStore.NotifyStoreEvent(reqStatus, event, index);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(index==DMMTSY_NAM_STORE_INDEX2);
	TEST(event==DMMTSY_NAM_STORE_EVENT);

	// asychronous & cancel

	namStore.NotifyStoreEvent(reqStatus, event, index);
	namStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::NotifyStoreEvent (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(index==DMMTSY_NAM_STORE_INDEX2);
		TEST(event==DMMTSY_NAM_STORE_EVENT);
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::NotifyStoreEvent (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileNamStore's Notify Store Event change"));


	// Read all NAM entries

	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneNamList* retrieve=CRetrieveMobilePhoneNamList::NewL(namStore);
    retrieve->Start(reqStatus,DMMTSY_NAM_STORE_NAMID);
    delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus == KErrCancel);

	// Get NAM list using asynchronous AO version
	CTestGetNamList* getNamList=CTestGetNamList::NewLC(namStore, this);
    getNamList->Start();
    CActiveScheduler::Start();

	// Ensure that the same object can be used to retrieve more than one list

    getNamList->Start();
    CActiveScheduler::Start();

	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy();  // getNamList

	// Now test the cancelling

	getNamList=CTestGetNamList::NewLC(namStore, this);
    getNamList->Start();

	CTestCanceller* cancelGetNamList = getNamList->RetrieveCancelPtr();
	cancelGetNamList->Call();

    CActiveScheduler::Start();
	TBool cancelled = (getNamList->iStatus == KErrCancel);

	// Ensure that an object which has just been cancelled can be susbsequently used to retrieve
	// a list.

    getNamList->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::ReadAll (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::ReadAll (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);


    CleanupStack::PopAndDestroy();  // getNamList

//	test.Next(_L("OK: RMobileNamStore's Read All Entries"));

	// Test Set SetActiveNam

	namStore.SetActiveNam(reqStatus,DMMTSY_NAM_STORE_NAMID);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone);

	namStore.SetActiveNam(reqStatus,DMMTSY_NAM_STORE_NAMID);
	namStore.CancelAsyncRequest(EMobileNamStoreSetActiveNam);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int() == KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::SetActiveNam (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::SetActiveNam (async & cancel (Request Not Cancelled)) passed"), iTestCount++);




	// Storing a new version of the list

	// asynchronous

	RMobileNamStore::TMobileNamEntryV1 storeEntry;

	CMobilePhoneNamList* namList = CMobilePhoneNamList::NewL();
	CleanupStack::PushL(namList);

	InitNamEntry(storeEntry);
	storeEntry.iIndex = DMMTSY_NAM_STORE_INDEX1;

	namList->AddEntryL(storeEntry);
	storeEntry.iIndex = DMMTSY_NAM_STORE_INDEX2;
	namList->AddEntryL(storeEntry);

	namStore.StoreAllL(reqStatus, DMMTSY_NAM_STORE_NAMID, namList);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	namStore.StoreAllL(reqStatus, DMMTSY_NAM_STORE_NAMID, namList);
	namStore.CancelAsyncRequest(EMobileNamStoreStoreAll);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	cancelled = (reqStatus.Int()==KErrCancel);

	// Lets just test that we can perform a Store operation after the previous store was cancelled

	namStore.StoreAllL(reqStatus, DMMTSY_NAM_STORE_NAMID, namList);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::StoreAll (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::StoreAll (sync & async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	CleanupStack::PopAndDestroy(); // namList

//	test.Next(_L("OK: RMobileNamStore's Store All Entries"));


//	Test Nam store with V4 Nam entries


	// Read one entry
	RMobileNamStore::TMobileNamEntryV4 asyncEntryV4_1;

	// asynchronous

	asyncEntryV4_1.iIndex = DMMTSY_NAM_STORE_INDEX1;
	RMobileNamStore::TMobileNamEntryV4Pckg asyncEntry1PckgV4_1(asyncEntryV4_1);

	namStore.Read(reqStatus,asyncEntry1PckgV4_1);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	ValidateNamRead(asyncEntryV4_1);

	// cancel

	asyncEntryV4_1.iIndex = DMMTSY_NAM_STORE_INDEX1;
	namStore.Read(reqStatus, asyncEntry1PckgV4_1);
	namStore.CancelAsyncRequest(EMobilePhoneStoreRead);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Read (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateNamRead(asyncEntryV4_1);
		SetTestStepResult(EInconclusive);
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Read (async & cancel (Request Not Cancelled)) inconclusive"), iTestCount++);
		}

	// Write one entry

	RMobileNamStore::TMobileNamEntryV4 entryV4_2;
	RMobileNamStore::TMobileNamEntryV4Pckg entryV4_2Pckg(entryV4_2);

	InitNamEntry(entryV4_2);

	// asynchronous

	entryV4_2.iIndex=DMMTSY_NAM_STORE_INDEX2;
	namStore.Write(reqStatus, entryV4_2Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// aynchronous & cancel

	namStore.Write(reqStatus, entryV4_2Pckg);
	namStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Write (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		SetTestStepResult(EInconclusive);
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::Write (async & cancel (Request Not Cancelled)) inconclusive"), iTestCount++);
		}

	// Read all NAM entries

	// Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneNamList* retrieveV4=CRetrieveMobilePhoneNamList::NewL(namStore);
    retrieveV4->StartV4(reqStatus,DMMTSY_NAM_STORE_NAMID);
    delete retrieveV4;
 	retrieveV4 = NULL;
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus == KErrCancel);

	// Get NAM list using asynchronous AO version
	CTestGetNamListV4* getNamListV4=CTestGetNamListV4::NewLC(namStore, this);
    getNamListV4->Start();
    CActiveScheduler::Start();

	// Ensure that the same object can be used to retrieve more than one list

    getNamListV4->Start();
    CActiveScheduler::Start();

	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy(getNamListV4);

	// Now test the cancelling

	getNamListV4=CTestGetNamListV4::NewLC(namStore, this);
    getNamListV4->Start();

	CTestCanceller* cancelGetNamListV4 = getNamListV4->RetrieveCancelPtr();
	cancelGetNamListV4->Call();

    CActiveScheduler::Start();
	cancelled = (getNamListV4->iStatus == KErrCancel);

	// Ensure that an object which has just been cancelled can be susbsequently used to retrieve
	// a list.

    getNamListV4->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::ReadAll (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		SetTestStepResult(EInconclusive);
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::ReadAll (sync & async & cancel (Request Not Cancelled)) inconclusive"), iTestCount++);
		}

    CleanupStack::PopAndDestroy(getNamListV4);

	// Storing a new version of the list

	// asynchronous

	RMobileNamStore::TMobileNamEntryV4 storeEntryV4;

 	CMobilePhoneNamListV4* namListV4 = CMobilePhoneNamListV4::NewL();
	CleanupStack::PushL(namListV4);

	InitNamEntry(storeEntryV4);
	storeEntryV4.iIndex = DMMTSY_NAM_STORE_INDEX1;

	namListV4->AddEntryL(storeEntryV4);
	storeEntryV4.iIndex = DMMTSY_NAM_STORE_INDEX2;
	namListV4->AddEntryL(storeEntryV4);

	namStore.StoreAllL(reqStatus, DMMTSY_NAM_STORE_NAMID, namListV4);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	namStore.StoreAllL(reqStatus, DMMTSY_NAM_STORE_NAMID, namListV4);
	namStore.CancelAsyncRequest(EMobileNamStoreStoreAllV4);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	cancelled = (reqStatus.Int()==KErrCancel);

	// Lets just test that we can perform a Store operation after the previous store was cancelled

	namStore.StoreAllL(reqStatus, DMMTSY_NAM_STORE_NAMID, namListV4);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::StoreAll (sync & async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		SetTestStepResult(EInconclusive);
		INFO_PRINTF2(_L("Test %d - RMobileNamStore::StoreAll (sync & async & cancel (Request Not Cancelled)) inconclusive"), iTestCount++);
		}
	CleanupStack::PopAndDestroy(namListV4);

	// Test retrieving both V1 and V4 NAM lists, one after the other

	CTestGetNamListSimultaneous* getNamListSimultaneous=CTestGetNamListSimultaneous::NewLC(namStore, this);
    getNamListSimultaneous->Start();
    CActiveScheduler::Start();

	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.
    CleanupStack::PopAndDestroy(getNamListSimultaneous);

	namStore.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

void CTestNamStore::ValidateNamInfo(const RMobileNamStore::TMobileNamStoreInfoV1 &aInfo)
	{
	TEST(aInfo.iName.Compare(DMMTSY_NAM_STORE_NAME) == 0);
	TEST(aInfo.iType==DMMTSY_NAM_STORE_TYPE);
	TEST(aInfo.iTotalEntries==DMMTSY_NAM_STORE_ENTRIES);
	TEST(aInfo.iUsedEntries==DMMTSY_NAM_STORE_USED);
	TEST(aInfo.iCaps==DMMTSY_NAM_STORE_CAPS);
	TEST(aInfo.iNamCount == DMMTSY_NAM_STORE_COUNT);
	TEST(aInfo.iActiveNam == DMMTSY_NAM_STORE_ACTIVE);
	TEST(aInfo.ExtensionId()==RMobilePhoneStore::KETelMobileNamStoreV1);
	}

void CTestNamStore::ValidateNamEntry(const RMobileNamStore::TMobileNamEntryV1 &aEntry)
	{
	TEST(aEntry.iNamId == DMMTSY_NAM_STORE_NAMID);
	TEST(aEntry.iParamIdentifier == DMMTSY_NAM_STORE_PARAM);
	TEST(aEntry.iData.Compare(DMMTSY_NAM_STORE_DATA) == 0);
	}


void CTestNamStore::ValidateNamEntry(const RMobileNamStore::TMobileNamEntryV4 &aEntry)
	{
	TEST(aEntry.iNamId == DMMTSY_NAM_STORE_NAMID);
	TEST(aEntry.iParamIdentifier == DMMTSY_NAM_STORE_PARAM);
	TEST(aEntry.iData.Compare(DMMTSY_NAM_STORE_DATA_V4) == 0);
	}

void CTestNamStore::ValidateNamRead(const RMobileNamStore::TMobileNamEntryV1 &aEntry)
	{
	ValidateNamEntry(aEntry);
	}

void CTestNamStore::ValidateNamRead(const RMobileNamStore::TMobileNamEntryV4 &aEntry)
	{
	ValidateNamEntry(aEntry);
	}

void CTestNamStore::InitNamEntry(RMobileNamStore::TMobileNamEntryV1& aEntry)
	{
	aEntry.iNamId = DMMTSY_NAM_STORE_NAMID;
	aEntry.iParamIdentifier = DMMTSY_NAM_STORE_PARAM;
	aEntry.iData = DMMTSY_NAM_STORE_DATA;
	}

void CTestNamStore::InitNamEntry(RMobileNamStore::TMobileNamEntryV4& aEntry)
	{
	aEntry.iNamId = DMMTSY_NAM_STORE_NAMID;
	aEntry.iParamIdentifier = DMMTSY_NAM_STORE_PARAM;
	aEntry.iData = DMMTSY_NAM_STORE_DATA_V4;
	}

/**************************************************************/
//
// Testing asynchronous retrieve of NAM list
//
/**************************************************************/

CTestNamStore::CTestGetNamList* CTestNamStore::CTestGetNamList::NewLC(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore)
	{
    CTestGetNamList* r=new(ELeave) CTestGetNamList(aStore, aTestNamStore);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }

CTestNamStore::CTestGetNamListV4* CTestNamStore::CTestGetNamListV4::NewLC(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore)
	{
    CTestGetNamListV4* r=new(ELeave) CTestGetNamListV4(aStore, aTestNamStore);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }

CTestNamStore::CTestGetNamListSimultaneous* CTestNamStore::CTestGetNamListSimultaneous::NewLC(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore)
	{
    CTestGetNamListSimultaneous* r=new(ELeave) CTestGetNamListSimultaneous(aStore, aTestNamStore);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }

CTestNamStore::CTestGetNamList::CTestGetNamList(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore)
    : CActive(EPriorityNormal), iStore(aStore), iTestNamStore(aTestNamStore)
    {}

CTestNamStore::CTestGetNamListV4::CTestGetNamListV4(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore)
    : CActive(EPriorityNormal), iStore(aStore), iTestNamStore(aTestNamStore)
    {}

CTestNamStore::CTestGetNamListSimultaneous::CTestGetNamListSimultaneous(RMobilePhoneStore& aStore, CTestNamStore* aTestNamStore)
    : CActive(EPriorityNormal), iStore(aStore), iTestNamStore(aTestNamStore)
    {}

void CTestNamStore::CTestGetNamList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneNamList::NewL(iStore);
	iCanceller=new (ELeave) CTestCanceller(this);
    }

void CTestNamStore::CTestGetNamListV4::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneNamList::NewL(iStore);
	iCanceller=new (ELeave) CTestCanceller(this);
    }

void CTestNamStore::CTestGetNamListSimultaneous::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneNamList::NewL(iStore);
	iCanceller=new (ELeave) CTestCanceller(this);
    }

CTestNamStore::CTestGetNamList::~CTestGetNamList()
    {
    delete iRetrieve;
	delete iCanceller;
	}

CTestNamStore::CTestGetNamListV4::~CTestGetNamListV4()
    {
    delete iRetrieve;
	delete iCanceller;
	}

CTestNamStore::CTestGetNamListSimultaneous::~CTestGetNamListSimultaneous()
    {
    delete iRetrieve;
	delete iCanceller;
	}

void CTestNamStore::CTestGetNamList::Start()
    {
    iRetrieve->Start(iStatus,DMMTSY_NAM_STORE_NAMID);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus,DMMTSY_NAM_STORE_NAMID);
	User::WaitForRequest(tmpStatus);
	iTestNamStore->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }

void CTestNamStore::CTestGetNamListV4::Start()
    {
    iRetrieve->StartV4(iStatus,DMMTSY_NAM_STORE_NAMID);
	TRequestStatus tmpStatus;
	iRetrieve->StartV4(tmpStatus,DMMTSY_NAM_STORE_NAMID);
	User::WaitForRequest(tmpStatus);
	iTestNamStore->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }

void CTestNamStore::CTestGetNamListSimultaneous::Start()
    {
    iRetrieve->Start(iStatus,DMMTSY_NAM_STORE_NAMID);
	TRequestStatus tmpStatus;
	iRetrieve->StartV4(tmpStatus,DMMTSY_NAM_STORE_NAMID);
	User::WaitForRequest(tmpStatus);
	iTestNamStore->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }

CTestCanceller* CTestNamStore::CTestGetNamList::RetrieveCancelPtr()
	{
	return iCanceller;
	}

CTestCanceller* CTestNamStore::CTestGetNamListV4::RetrieveCancelPtr()
	{
	return iCanceller;
	}

void CTestNamStore::CTestGetNamList::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}

void CTestNamStore::CTestGetNamListV4::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}

void CTestNamStore::CTestGetNamListSimultaneous::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}

void CTestNamStore::CTestGetNamList::DoCancel()
    {
    iRetrieve->Cancel();
    }

void CTestNamStore::CTestGetNamListV4::DoCancel()
    {
    iRetrieve->Cancel();
    }

void CTestNamStore::CTestGetNamListSimultaneous::DoCancel()
    {
    iRetrieve->Cancel();
    }

TInt CTestNamStore::CTestGetNamList::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneNamList* namList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, namList=iRetrieve->RetrieveListL(););

	if (namList)
		{
		iTestNamStore->TEST(namList->Enumerate()==DMMTSY_NAM_STORE_COUNT);

		// Get each detected network entry
		RMobileNamStore::TMobileNamEntryV1 entry;

		for (TInt i=0; i<DMMTSY_NAM_STORE_COUNT; ++i)
			{
			TRAP(ret,entry=namList->GetEntryL(i));
			if (ret != KErrNone)
				break;

			switch(i)
				{
				case 0:
					{
					iTestNamStore->ValidateNamEntry(entry);
					iTestNamStore->TEST(entry.iIndex==DMMTSY_NAM_STORE_INDEX1);
					break;
					}
				case 1:
					{
					iTestNamStore->ValidateNamEntry(entry);
					iTestNamStore->TEST(entry.iIndex==DMMTSY_NAM_STORE_INDEX2);
					break;
					}
				default:
					break;
				}
			}
		delete namList;
		}

   return ret;
   }

TInt CTestNamStore::CTestGetNamListV4::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneNamListV4* namList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, namList=iRetrieve->RetrieveListV4L(););

	if (namList)
		{
		iTestNamStore->TEST(namList->Enumerate()==DMMTSY_NAM_STORE_COUNT);

		// Get each detected network entry
		RMobileNamStore::TMobileNamEntryV4 entry;

		for (TInt i=0; i<DMMTSY_NAM_STORE_COUNT; ++i)
			{
			TRAP(ret,entry=namList->GetEntryL(i));
			if (ret != KErrNone)
				break;

			switch(i)
				{
				case 0:
					{
					iTestNamStore->ValidateNamEntry(entry);
					iTestNamStore->TEST(entry.iIndex==DMMTSY_NAM_STORE_INDEX1);
					break;
					}
				case 1:
					{
					iTestNamStore->ValidateNamEntry(entry);
					iTestNamStore->TEST(entry.iIndex==DMMTSY_NAM_STORE_INDEX2);
					break;
					}
				default:
					break;
				}
			}
		delete namList;
		}

   return ret;
   }

TInt CTestNamStore::CTestGetNamListSimultaneous::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneNamList* namListV1=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, namListV1=iRetrieve->RetrieveListL(););

	if (namListV1)
		{
		iTestNamStore->TEST(namListV1->Enumerate()==DMMTSY_NAM_STORE_COUNT);

		// Get each detected network entry
		RMobileNamStore::TMobileNamEntryV1 entryV1;

		for (TInt i=0; i<DMMTSY_NAM_STORE_COUNT; ++i)
			{
			TRAP(ret,entryV1=namListV1->GetEntryL(i));
			if (ret != KErrNone)
				break;

			switch(i)
				{
				case 0:
					{
					iTestNamStore->ValidateNamEntry(entryV1);
					iTestNamStore->TEST(entryV1.iIndex==DMMTSY_NAM_STORE_INDEX1);
					break;
					}
				case 1:
					{
					iTestNamStore->ValidateNamEntry(entryV1);
					iTestNamStore->TEST(entryV1.iIndex==DMMTSY_NAM_STORE_INDEX2);
					break;
					}
				default:
					break;
				}
			}
		delete namListV1;
		}

   return ret;
   }
