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
#include "TE_mmmemlist.h"
#include "testdef.h"

CTestMemMMLists::CTestMemMMLists()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMemMMLists"));
	}

enum TVerdict CTestMemMMLists::doTestStepL()
	{
	iTestCount=1;
	TInt err = KErrNone;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test List classes coping with OOM"));

	CMobilePhoneBroadcastIdList *list = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);

	
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 entry;
	entry.iId = STATIC_CAST(TInt16,1);

	TInt failNext = 0;
	do
		{
		__UHEAP_FAILNEXT(++failNext);
		TRAP(err,list->AddEntryL(entry));
		} while (err != KErrNone);
	__UHEAP_RESET;

	INFO_PRINTF3(_L("Test %d - CMobilePhoneBroadcastIdList::AddEntryL OOM Test required %d allocations to succeed"),iTestCount++,failNext);
	CleanupStack::PopAndDestroy();

	list = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);

	failNext = 0;
	entry.iId = STATIC_CAST(TInt16,0);
	do
		{
		__UHEAP_FAILNEXT(++failNext);
		TRAP(err,list->InsertEntryL(0,entry));
		} while (err == KErrNoMemory);
	__UHEAP_RESET;
	
	INFO_PRINTF3(_L("Test %d - CMobilePhoneBroadcastIdList::InsertEntryL OOM Test required %d allocations to succeed"),iTestCount++,failNext);

	CleanupStack::PopAndDestroy();
	
	list = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);

	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 start,end;

	start.iId = 0;
	end.iId = 49;

	failNext = 0;
	do
		{
		__UHEAP_FAILNEXT(++failNext);
		TRAP(err,list->AddRangeEntryL(start,end));
		} while (err == KErrNoMemory);
	__UHEAP_RESET;
	
	INFO_PRINTF3(_L("Test %d - CMobilePhoneBroadcastIdList::AddRangeEntryL OOM Test required %d allocations to succeed"),iTestCount++,failNext);

	CleanupStack::PopAndDestroy();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}
