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
#include "TE_mmmemenstore.h"
#include "testdef.h"

CTestMemEnStore::CTestMemEnStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMemEnStore"));
	}

enum TVerdict CTestMemEnStore::doTestStepL()
	{
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test RMobileENStore coping with OOM"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileENStore enStore;
	ret=enStore.Open(mmPhone);
    TEST(ret==KErrNone);

	CMemTestGetENList* getENList=CMemTestGetENList::NewLC(enStore, this);
    getENList->Start();
    CActiveScheduler::Start();

	CleanupStack::PopAndDestroy();

	enStore.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneENList handles OOM
//
/**************************************************************/

CTestMemEnStore::CMemTestGetENList* CTestMemEnStore::CMemTestGetENList::NewLC(RMobilePhoneStore& aStore, CTestMemEnStore* aTestMemEnStore)
	{
    CMemTestGetENList* r = new (ELeave) CMemTestGetENList(aStore, aTestMemEnStore);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemEnStore::CMemTestGetENList::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemEnStore::CMemTestGetENList::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetENList::Start();
	}
	
CTestMemEnStore::CMemTestGetENList::CMemTestGetENList(RMobilePhoneStore& aStore, CTestMemEnStore* aTestMemEnStore) :
	CTestGetENList(aStore, aTestMemEnStore), iTestMemEnStore(aTestMemEnStore)
	{

	}

void CTestMemEnStore::CMemTestGetENList::RunL()
	{
	CTestGetENList::RunL();
	__UHEAP_RESET;
	iTestMemEnStore->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneENList OOM Test required %d allocations to succeed"),iTestMemEnStore->iTestCount++,iFailEvery);
	}

TInt CTestMemEnStore::CMemTestGetENList::RunError(TInt aError)
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

