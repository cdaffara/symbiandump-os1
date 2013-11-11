// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains tests for RMobileSmsStore API
// 
//

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmmemsmsstore.h"
#include "testdef.h"

CTestMemSmsStore::CTestMemSmsStore()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMemSmsStore"));
	}

enum TVerdict CTestMemSmsStore::doTestStepL()
	{	
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile SMS Storage Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileSmsMessaging smsMessaging;
	ret=smsMessaging.Open(mmPhone);
    TEST(ret==KErrNone);

	RMobileSmsStore smsStore;
	ret=smsStore.Open(smsMessaging, KETelIccSmsStore);
    TEST(ret==KErrNone);

	CMemTestGetSmsList* getSmsList=CMemTestGetSmsList::NewLC(smsStore,ETrue,this);
    getSmsList->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();
	
	getSmsList=CMemTestGetSmsList::NewLC(smsStore,EFalse,this);
    getSmsList->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();

	smsStore.Close();
	smsMessaging.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}


/**************************************************************/
//
// Testing how CRetrieveMobilePhoneSmsList handles OOM
//
/**************************************************************/

CTestMemSmsStore::CMemTestGetSmsList* CTestMemSmsStore::CMemTestGetSmsList::NewLC(RMobilePhoneStore& aStore, TBool aBatch, CTestMemSmsStore* aTestMemSmsStore)
	{
    CMemTestGetSmsList* r = new (ELeave) CMemTestGetSmsList(aStore,aBatch,aTestMemSmsStore);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemSmsStore::CMemTestGetSmsList::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemSmsStore::CMemTestGetSmsList::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetSmsList::Start();
	}
	
CTestMemSmsStore::CMemTestGetSmsList::CMemTestGetSmsList(RMobilePhoneStore& aStore, TBool aBatch, CTestMemSmsStore* aTestMemSmsStore) :
	CTestGetSmsList(aStore,aBatch,DMMTSY_SMS_STORE_GSMENTRY_TYPE,aTestMemSmsStore), iTestMemSmsStore(aTestMemSmsStore)
	{
	}

void CTestMemSmsStore::CMemTestGetSmsList::RunL()
	{
	CTestGetSmsList::RunL();
	__UHEAP_RESET;
	iTestMemSmsStore->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneSmsList OOM Test required %d allocations to succeed"),iTestMemSmsStore->iTestCount++,iFailEvery);
	}

TInt CTestMemSmsStore::CMemTestGetSmsList::RunError(TInt aError)
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

