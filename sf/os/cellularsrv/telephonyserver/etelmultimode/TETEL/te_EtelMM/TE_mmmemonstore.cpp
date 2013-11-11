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
#include "TE_mmmemonstore.h"
#include "testdef.h"

CTestMemOnStore::CTestMemOnStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMemOnStore"));
	}

enum TVerdict CTestMemOnStore::doTestStepL()
	{
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test RMobileONStore coping with OOM"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileONStore onStore;
	ret=onStore.Open(mmPhone);
    TEST(ret==KErrNone);

	CMemTestGetONList* getONList=CMemTestGetONList::NewLC(onStore, this);
    getONList->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();
	
	RMobileONStore::TMobileONEntryV1 storeEntry;

	CMobilePhoneONList* onList = CMobilePhoneONList::NewL();
	CleanupStack::PushL(onList);

	CTestOnStore::InitONEntry(storeEntry);
	storeEntry.iIndex = DMMTSY_ON_STORE_INDEX1;

	onList->AddEntryL(storeEntry);
	storeEntry.iIndex = DMMTSY_ON_STORE_INDEX2;
	onList->AddEntryL(storeEntry);

	TRequestStatus reqStatus;
	TInt err = KErrNone;
	TInt failNext = 0;
	do
		{
		__UHEAP_FAILNEXT(++failNext);
		TRAP(err,onStore.StoreAllL(reqStatus, onList));
		if (err == KErrNone)
			{
			User::WaitForRequest(reqStatus);
			err = reqStatus.Int();
			}
		TEST(err == KErrNone || err == KErrNoMemory);
		} while (err != KErrNone);
	__UHEAP_RESET;
	INFO_PRINTF3(_L("Test %d - OOM test on RMobileOnStore::StoreAllL required %d allocations to succeed"),iTestCount++,failNext);
//	INFO_PRINTF2(_L("OOM test on RMobileOnStore::StoreAllL OK"));
	CleanupStack::PopAndDestroy();


	onStore.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));
	
	return TestStepResult();
	}

CTestMemOnStore::CMemTestGetONList* CTestMemOnStore::CMemTestGetONList::NewLC(RMobilePhoneStore& aStore, CTestMemOnStore* aTestMemOnStore)
	{
    CMemTestGetONList* r = new (ELeave) CMemTestGetONList(aStore, aTestMemOnStore);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemOnStore::CMemTestGetONList::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemOnStore::CMemTestGetONList::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetONList::Start();
	}
	
CTestMemOnStore::CMemTestGetONList::CMemTestGetONList(RMobilePhoneStore& aStore, CTestMemOnStore* aTestMemOnStore) :
	CTestGetONList(aStore, aTestMemOnStore), iTestMemOnStore(aTestMemOnStore)
	{

	}

void CTestMemOnStore::CMemTestGetONList::RunL()
	{
	CTestGetONList::RunL();
	__UHEAP_RESET;
	iTestMemOnStore->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneONList OOM Test required %d allocations to succeed"),iTestMemOnStore->iTestCount++,iFailEvery);
	}

TInt CTestMemOnStore::CMemTestGetONList::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		iFailEvery++;
		StartMemTest();
		return KErrNone;
		}
	else
		return aError;
	}

