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
#include "TE_mmmembroadcast.h"
#include "testdef.h"

CTestMemBroadcastMessaging::CTestMemBroadcastMessaging()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMemBrdcastMess"));
	}

enum TVerdict CTestMemBroadcastMessaging::doTestStepL()
//
// Test Broadcast Messaging Functions with Out Of Memory
//
	{
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test RMobileBroadcastMessaging coping with OOM"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileBroadcastMessaging cbsMessaging;
	ret=cbsMessaging.Open(mmPhone);
    TEST(ret==KErrNone);
	
	CMemTestGetCbmiList* getCbmiList=CMemTestGetCbmiList::NewLC(cbsMessaging, this);
    getCbmiList->Start();
    CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();
//	INFO_PRINTF2(_L("CbmiList list retrieval OOM tests ok"));

	// Test StoreBroadcastIdListL

	CMobilePhoneBroadcastIdList* list = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);

	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 entry;
	for (TInt i = 0; i < DMMTSY_CBMI_STORE_COUNT; i++)
		{
		entry.iId = DMMTSY_CBMI_ID1;
		list->AddEntryL(entry);
		}

	TRequestStatus reqStatus;
	TInt err = KErrNone;
	TInt failNext = 0;
	RMobileBroadcastMessaging::TMobileBroadcastIdType iIdType=DMMTSY_BROADCAST_ID_TYPE;
	do
		{
		__UHEAP_FAILNEXT(++failNext);
		TRAP(err,cbsMessaging.StoreBroadcastIdListL(reqStatus,list,iIdType));
		if (err == KErrNone)
			{
			User::WaitForRequest(reqStatus);
			err = reqStatus.Int();
			}
		TEST(err == KErrNone || err == KErrNoMemory);
		} while (err != KErrNone);
	__UHEAP_RESET;
	INFO_PRINTF3(_L("Test %d - OOM test on RMobileBroadcastMessaging::StoreBroadcastIdListL required %d allocations"),iTestCount++,failNext);
//	INFO_PRINTF2(_L("OOM test on RMobileBroadcastMessaging::StoreBroadcastIdListL OK"));
	CleanupStack::PopAndDestroy();

	cbsMessaging.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}

/**************************************************************/
//
// Testing how CRetrieveMobilePhoneCbmiList handles OOM
//
/**************************************************************/

CTestMemBroadcastMessaging::CMemTestGetCbmiList* CTestMemBroadcastMessaging::CMemTestGetCbmiList::NewLC(RMobileBroadcastMessaging &aMessaging, CTestMemBroadcastMessaging* aTestMemBroadcastMessaging)
	{
    CMemTestGetCbmiList* r = new (ELeave) CMemTestGetCbmiList(aMessaging, aTestMemBroadcastMessaging);
	CleanupStack::PushL(r);
	r->ConstructL();
    return r;	
	}

void CTestMemBroadcastMessaging::CMemTestGetCbmiList::Start()
	{
	iFailEvery = 1;
	StartMemTest();
	}

void CTestMemBroadcastMessaging::CMemTestGetCbmiList::StartMemTest()
	{
	__UHEAP_FAILNEXT(iFailEvery);
	CTestGetCbmiList::Start();
	}
	
CTestMemBroadcastMessaging::CMemTestGetCbmiList::CMemTestGetCbmiList(RMobileBroadcastMessaging &aMessaging, CTestMemBroadcastMessaging* aTestMemBroadcastMessaging) :
	CTestGetCbmiList(aMessaging, aTestMemBroadcastMessaging), iTestMemBroadcastMessaging(aTestMemBroadcastMessaging)
	{

	}

void CTestMemBroadcastMessaging::CMemTestGetCbmiList::RunL()
	{
	CTestGetCbmiList::RunL();
//	__UHEAP_RESET;
	iTestMemBroadcastMessaging->INFO_PRINTF3(_L("Test %d - CRetrieveMobilePhoneCbmiList OOM Test required %d allocations to succeed"),iTestMemBroadcastMessaging->iTestCount++,iFailEvery);
	}

TInt CTestMemBroadcastMessaging::CMemTestGetCbmiList::RunError(TInt aError)
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


