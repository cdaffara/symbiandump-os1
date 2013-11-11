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
// Contains tests for RMobileSmsMessaging API
// 
//

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmmemsms.h"
#include "TE_mmsms.h"
#include "testdef.h"


CTestMemSmsMessaging::CTestMemSmsMessaging()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMemSmsMessaging"));
	}

enum TVerdict CTestMemSmsMessaging::doTestStepL()
	{
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test RMobileSmsMessaging coping with OOM"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileSmsMessaging smsMessaging;
	ret=smsMessaging.Open(mmPhone);
    TEST(ret==KErrNone);
	
	CMemTestGetSmspList* getSmspList=CMemTestGetSmspList::NewLC(smsMessaging, this);
    getSmspList->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();
//	INFO_PRINTF2(_L("SmspList list retrieval OOM tests ok"));

	// Test StoreBroadcastIdListL

	CMobilePhoneSmspList* list = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(list);

	RMobileSmsMessaging::TMobileSmspEntryV1 entry;
//	CTestSmsMessaging::InitSmspEntry(entry);
	iTestSmsMessaging->InitSmspEntry(entry);
	entry.iIndex = DMMTSY_SMSP_STORE_INDEX1;
	list->AddEntryL(entry);
	entry.iIndex = DMMTSY_SMSP_STORE_INDEX2;
	list->AddEntryL(entry);
	
	TRequestStatus reqStatus;
	TInt err = KErrNone;
	TInt failNext = 0;
	do
		{
		__UHEAP_FAILNEXT(++failNext);
		TRAP(err,smsMessaging.StoreSmspListL(reqStatus,list));
		if (err == KErrNone)
			{
			User::WaitForRequest(reqStatus);
			err = reqStatus.Int();
			}
		TEST(err == KErrNone || err == KErrNoMemory);
		} while (err != KErrNone);
	__UHEAP_RESET;
	INFO_PRINTF3(_L("Test %d - OOM test on RMobileSmsMessaging::StoreSmspListL required %d allocations to succeed"),iTestCount++,failNext);
//	test.Next(_L("OOM test on RMobileSmsMessaging::StoreSmspListL OK"));
	CleanupStack::PopAndDestroy();

	smsMessaging.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneSmspList handles OOM
//
/**************************************************************/

CTestMemSmsMessaging::CMemTestGetSmspList* CTestMemSmsMessaging::CMemTestGetSmspList::NewLC(RMobileSmsMessaging& aMessaging, CTestMemSmsMessaging* aTestMemSmsMessaging)
	{
    CMemTestGetSmspList* r = new (ELeave) CMemTestGetSmspList(aMessaging, aTestMemSmsMessaging);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemSmsMessaging::CMemTestGetSmspList::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemSmsMessaging::CMemTestGetSmspList::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetSmspList::Start();
	}
	
CTestMemSmsMessaging::CMemTestGetSmspList::CMemTestGetSmspList(RMobileSmsMessaging& aMessaging, CTestMemSmsMessaging* aTestMemSmsMessaging) :
	CTestGetSmspList(aMessaging, aTestMemSmsMessaging), iTestMemSmsMessaging(aTestMemSmsMessaging)
	{

	}

void CTestMemSmsMessaging::CMemTestGetSmspList::RunL()
	{
	CTestGetSmspList::RunL();
	__UHEAP_RESET;
	iTestMemSmsMessaging->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneSmspList OOM Test required %d allocations to succeed"),iTestMemSmsMessaging->iTestCount++,iFailEvery);
	}

TInt CTestMemSmsMessaging::CMemTestGetSmspList::RunError(TInt aError)
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

