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
#include "TE_mmmemnamstore.h"
#include "testdef.h"

CTestMemNamStore::CTestMemNamStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMemNamStore"));
	}

enum TVerdict CTestMemNamStore::doTestStepL()
	{
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test how Mobile NAM Storage Functionality handles OOM"));

	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileNamStore namStore;
	ret=namStore.Open(mmPhone);
    TEST(ret==KErrNone);

	CMemTestGetNamList* getNamList=CMemTestGetNamList::NewLC(namStore, this);
    getNamList->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();

	RMobileNamStore::TMobileNamEntryV1 storeEntry;

	CMobilePhoneNamList* namList = CMobilePhoneNamList::NewL();
	CleanupStack::PushL(namList);

	CTestNamStore::InitNamEntry(storeEntry);
	storeEntry.iIndex = DMMTSY_NAM_STORE_INDEX1;

	namList->AddEntryL(storeEntry);
	storeEntry.iIndex = DMMTSY_NAM_STORE_INDEX2;
	namList->AddEntryL(storeEntry);

	TRequestStatus reqStatus;
	TInt err = KErrNone;
	TInt failNext = 0;
	do
		{
		__UHEAP_FAILNEXT(++failNext);
		TRAP(err,namStore.StoreAllL(reqStatus, DMMTSY_NAM_STORE_NAMID, namList));
		if (err == KErrNone)
			{
			User::WaitForRequest(reqStatus);
			err = reqStatus.Int();
			}
		TEST(err == KErrNone || err == KErrNoMemory);
		} while (err != KErrNone);
	__UHEAP_RESET;
	INFO_PRINTF3(_L("Test %d - OOM test on RMobileNamStore::StoreAllL required %d allocations to succeed"),iTestCount++,failNext);
//	INFO_PRINTF2(_L("OOM test on RMobileNamStore::StoreAllL OK"));
	CleanupStack::PopAndDestroy();

	namStore.Close();
	mmPhone.Close();	
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneNamList handles OOM
//
/**************************************************************/

CTestMemNamStore::CMemTestGetNamList* CTestMemNamStore::CMemTestGetNamList::NewLC(RMobilePhoneStore& aStore, CTestMemNamStore* aTestMemNamStore)
	{
    CMemTestGetNamList* r = new (ELeave) CMemTestGetNamList(aStore, aTestMemNamStore);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemNamStore::CMemTestGetNamList::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemNamStore::CMemTestGetNamList::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetNamList::Start();
	}
	
CTestMemNamStore::CMemTestGetNamList::CMemTestGetNamList(RMobilePhoneStore& aStore, CTestMemNamStore* aTestMemNamStore) :
	CTestGetNamList(aStore, aTestMemNamStore), iTestMemNamStore(aTestMemNamStore)
	{

	}

void CTestMemNamStore::CMemTestGetNamList::RunL()
	{
	CTestGetNamList::RunL();
	__UHEAP_RESET;
	iTestMemNamStore->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneNamList OOM Test required %d allocations to succeed"),iTestMemNamStore->iTestCount++,iFailEvery);
	}

TInt CTestMemNamStore::CMemTestGetNamList::RunError(TInt aError)
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

