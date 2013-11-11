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
#include "TE_mmenstore.h"
#include "testdef.h"

CTestEnStore::CTestEnStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestEnStore"));
	}

enum TVerdict CTestEnStore::doTestStepL()
//
// Test Emergency Number Functions
//
    {	
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Emergency Number Storage Functionality"));
//	LOGTEXT(_L8("Test Mobile Emergency Number Storage Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileENStore enStore;
	ret=enStore.Open(mmPhone);
    TEST(ret==KErrNone);

	TRequestStatus reqStatus;

	// Get Network store information

	// asynchronous

	RMobilePhoneStore::TMobilePhoneStoreInfoV1 asyncInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg asyncInfoPckg(asyncInfo);
	enStore.GetInfo(reqStatus, asyncInfoPckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	ValidateENInfo(asyncInfo);

	// asynchronous & cancel

	enStore.GetInfo(reqStatus, asyncInfoPckg);
	enStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileENStore::GetInfo (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		ValidateENInfo(asyncInfo);
		INFO_PRINTF2(_L("Test %d - RMobileENStore::GetInfo (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileNetworkStore's Get Info"));

	// Read one entry

	RMobileENStore::TMobileENEntryV1 asyncEntry1;

	// asynchronous

	asyncEntry1.iIndex = DMMTSY_EN_STORE_INDEX1;
	RMobileENStore::TMobileENEntryV1Pckg asyncEntry1Pckg(asyncEntry1);

	enStore.Read(reqStatus,asyncEntry1Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	ValidateENRead(asyncEntry1);


	// cancel

	asyncEntry1.iIndex = DMMTSY_EN_STORE_INDEX1;
	enStore.Read(reqStatus, asyncEntry1Pckg);
	enStore.CancelAsyncRequest(EMobilePhoneStoreRead);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileENStore::Read (async & cancel (Cancelled Request)) passed"), iTestCount++);	
	else
		{
		ValidateENRead(asyncEntry1);
		INFO_PRINTF2(_L("Test %d - RMobileENStore::Read (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileENStore's Read Entry"));

	// Write one entry

	RMobileENStore::TMobileENEntryV1 entry2;
	RMobileENStore::TMobileENEntryV1Pckg entry2Pckg(entry2);

	InitENEntry(entry2);

	// asynchronous

	entry2.iIndex=DMMTSY_EN_STORE_INDEX2;
	enStore.Write(reqStatus, entry2Pckg);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// aynchronous & cancel

	enStore.Write(reqStatus, entry2Pckg);
	enStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileENStore::Write (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileENStore::Write (async & cancel (Request Not Cancelled)) passed"), iTestCount++);		

//	test.Next(_L("OK: RMobileENStore's Write Entry"));

	// Delete one entry

	// asynchronous	

	enStore.Delete(reqStatus, DMMTSY_EN_STORE_INDEX1);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// cancel

	enStore.Delete(reqStatus, DMMTSY_EN_STORE_INDEX1);
	enStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);

	if (reqStatus.Int()==KErrCancel)		
		INFO_PRINTF2(_L("Test %d - RMobileENStore::Delete (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileENStore::Delete (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

	// Delete all entries

	// asynchronous

	enStore.DeleteAll(reqStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);

	// asynchronous & cancel

	enStore.DeleteAll(reqStatus);
	enStore.CancelAsyncRequest(EMobilePhoneStoreDeleteAll);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileENStore::DeleteAll (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileENStore::DeleteAll (async & cancel (Request Not Cancelled)) passed"), iTestCount++);

//	test.Next(_L("OK: RMobileENStore's Delete Entries"));

	
	// Notify Entry Changed

	// asynchronous
	TInt index;

	TUint32 event;
	enStore.NotifyStoreEvent(reqStatus, event, index);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(index==DMMTSY_EN_STORE_INDEX2);
	TEST(event==DMMTSY_EN_STORE_EVENT);

	// asynchronous & cancel

	enStore.NotifyStoreEvent(reqStatus, event, index);
	enStore.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileENStore::NotifyStoreEvent (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(index==DMMTSY_EN_STORE_INDEX2);
		TEST(event==DMMTSY_EN_STORE_EVENT);
		INFO_PRINTF2(_L("Test %d - RMobileENStore::NotifyStoreEvent (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

//	test.Next(_L("OK: RMobileENStore's Notify Store Event change"));

	// Read all Network entries

	//Test deletion of list object whilst async request in progress
 	CRetrieveMobilePhoneENList* retrieve=CRetrieveMobilePhoneENList::NewL(enStore);
    retrieve->Start(reqStatus);
    delete retrieve;
 	retrieve = NULL;
 	User::WaitForRequest(reqStatus);
 	TEST(reqStatus == KErrCancel);

	// Get Network list using asynchronous AO version

	CTestGetENList* getENList=CTestGetENList::NewLC(enStore, this);
    getENList->Start();
    CActiveScheduler::Start();

	// Lets just ensure that the same retrieve object can be used to retrieve more than one list

	getENList->Start();
    CActiveScheduler::Start();


	// I only need to delete the retrieval object because the destructor
	// of this will also delete the list object.


	// Now test the cancelling
    getENList->Start();

	CTestCanceller* cancelGetENList = getENList->RetrieveCancelPtr();
	cancelGetENList->Call();

    CActiveScheduler::Start();
	TBool cancelled = (getENList->iStatus == KErrCancel);

	// Lets just ensure that a object can be used to retrieve a list after a previous transaction
	// had been cancelled.

	getENList->Start();
    CActiveScheduler::Start();

	if (cancelled)
		INFO_PRINTF2(_L("Test %d - RMobileENStore::ReadAll (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		INFO_PRINTF2(_L("Test %d - RMobileENStore::ReadAll (async & cancel (Request Not Cancelled)) passed"), iTestCount++);


    CleanupStack::PopAndDestroy();  // getENList

//	test.Next(_L("OK: RMobileENStore's Read All Entries"));

	enStore.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	};

void CTestEnStore::ValidateENInfo(const RMobilePhoneStore::TMobilePhoneStoreInfoV1 &aInfo)
	{
	TEST(aInfo.iName.Compare(DMMTSY_EN_STORE_NAME) == 0);
	TEST(aInfo.iType==DMMTSY_EN_STORE_TYPE);
	TEST(aInfo.iTotalEntries==DMMTSY_EN_STORE_ENTRIES);
	TEST(aInfo.iUsedEntries==DMMTSY_EN_STORE_USED);
	TEST(aInfo.iCaps==DMMTSY_EN_STORE_CAPS);
	TEST(aInfo.ExtensionId()==RMobilePhoneStore::KETelMobilePhoneStoreV1);
	}

void CTestEnStore::ValidateENEntry(const RMobileENStore::TMobileENEntryV1 &aEntry)
	{
	TEST(aEntry.iNetworkSpecific == DMMTSY_EN_STORE_NS);
	TEST(aEntry.iMode == DMMTSY_EN_STORE_MODE);
	TEST(aEntry.iCountryCode.Compare(DMMTSY_EN_STORE_CC) == 0);
	TEST(aEntry.iIdentity.Compare(DMMTSY_EN_STORE_ID) == 0);
	TEST(aEntry.iNumber.Compare(DMMTSY_EN_STORE_NUM) == 0);
	TEST(aEntry.iAlphaId.Compare(DMMTSY_EN_STORE_AMBULANCE_ID) == 0);
	TEST(aEntry.iCallType == DMMTSY_EN_STORE_CALL_TYPE);
	}

void CTestEnStore::ValidateENRead(const RMobileENStore::TMobileENEntryV1 &aEntry)
	{
	ValidateENEntry(aEntry);
	}

void CTestEnStore::InitENEntry(RMobileENStore::TMobileENEntryV1& aEntry)
	{
	aEntry.iNetworkSpecific = DMMTSY_EN_STORE_NS;
	aEntry.iMode            = DMMTSY_EN_STORE_MODE;
	aEntry.iCountryCode     = DMMTSY_EN_STORE_CC;
	aEntry.iIdentity        = DMMTSY_EN_STORE_ID;
	aEntry.iNumber          = DMMTSY_EN_STORE_NUM;
	aEntry.iAlphaId         = DMMTSY_EN_STORE_AMBULANCE_ID;
	aEntry.iCallType        = DMMTSY_EN_STORE_CALL_TYPE;
	}

/**************************************************************/
//
// Testing asynchronous retrieve of EN list
//
/**************************************************************/

CTestEnStore::CTestGetENList* CTestEnStore::CTestGetENList::NewLC(RMobilePhoneStore& aStore, CTestEnStore* aTestEnStore)
	{
    CTestGetENList* r=new(ELeave) CTestGetENList(aStore, aTestEnStore);
    CleanupStack::PushL(r);
    r->ConstructL();
    return r;
    }
      
CTestEnStore::CTestGetENList::CTestGetENList(RMobilePhoneStore& aStore, CTestEnStore* aTestEnStore)
    : CActive(EPriorityNormal), iStore(aStore), iTestEnStore(aTestEnStore)
    {}
      

void CTestEnStore::CTestGetENList::ConstructL()
	{
    CActiveScheduler::Add(this);
	iRetrieve=CRetrieveMobilePhoneENList::NewL(iStore);
	iCanceller=new (ELeave) CTestCanceller(this);
    }
         
CTestEnStore::CTestGetENList::~CTestGetENList()
    {
    delete iRetrieve;
	delete iCanceller;
	}
      
void CTestEnStore::CTestGetENList::Start()
    {
    iRetrieve->Start(iStatus);
	TRequestStatus tmpStatus;
	iRetrieve->Start(tmpStatus);
	User::WaitForRequest(tmpStatus);
	iTestEnStore->TEST(tmpStatus.Int() == KErrInUse);
    SetActive();
    }       
      
CTestCanceller* CTestEnStore::CTestGetENList::RetrieveCancelPtr()
	{
	return iCanceller;
	}
     
void CTestEnStore::CTestGetENList::RunL()
    {
    TInt ret=iStatus.Int();

    User::LeaveIfError(ret);
	ret=CheckResults();
	User::LeaveIfError(ret);

    CActiveScheduler::Stop();
	}
      

void CTestEnStore::CTestGetENList::DoCancel()
    {
    iRetrieve->Cancel();
    }       
      

TInt CTestEnStore::CTestGetENList::CheckResults()
    {
	TInt ret=KErrNone;

	CMobilePhoneENList* enList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, enList=iRetrieve->RetrieveListL(););
	
	if (enList)
		{
		iTestEnStore->TEST(enList->Enumerate()==DMMTSY_EN_STORE_COUNT);

		// Get each detected network entry
		RMobileENStore::TMobileENEntryV1 entry;

		for (TInt i=0; i<DMMTSY_EN_STORE_COUNT; ++i)
			{
			TRAP(ret,entry=enList->GetEntryL(i));
			if (ret != KErrNone)
				break;
			switch(i)
				{
				case 0:
					{
					iTestEnStore->ValidateENEntry(entry);
					iTestEnStore->TEST(entry.iIndex==DMMTSY_EN_STORE_INDEX1);
					break;
					}
				case 1:
					{
					iTestEnStore->ValidateENEntry(entry);
					iTestEnStore->TEST(entry.iIndex==DMMTSY_EN_STORE_INDEX2);
					break;
					}
				default:
					break;
				}
			}
		// now free-up enList
		delete enList;
		}

   return ret;
   }
