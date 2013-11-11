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
#include "TE_mmlist.h"
#include "testdef.h"

CTestMMLists::CTestMMLists()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMLists"));
	}

enum TVerdict CTestMMLists::doTestStepL()
{
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test API List class functionality"));

	TInt err = KErrNone;
	INFO_PRINTF1(_L("MM List classes"));

	CMobilePhoneBroadcastIdList *list = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);

	TEST(list->Enumerate() == 0);

	TEST(list->MaxNumberEntries() == -1);
	

	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 entry;

	TInt i;

	for (i = 0; i < 10; i++)
		{
		entry.iId = STATIC_CAST(TInt16,i);
		list->AddEntryL(entry);
		}

	TEST(list->Enumerate() == 10);

	list->SetMaxNumberEntries(10);
	entry.iId = 11;
	TRAP(err,list->AddEntryL(entry));
	TEST(err == CMobilePhoneListBase::EListMaxNumberReached);

	INFO_PRINTF1(_L("AddEntryL ok"));

	for (i = 0; i < 10; i++)
		{
		const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& entry2 = 
			list->GetEntryL(i);
		TEST(entry2.iId == i);
		}

	TRAP(err,list->GetEntryL(11));
	TEST(err == CMobilePhoneListBase::EListIndexOutOfRange);

	INFO_PRINTF1(_L("GetEntryL ok"));

	INFO_PRINTF2(_L("Test %d - CMobilePhoneListBase::Enumerate passed"), iTestCount++);	
	INFO_PRINTF2(_L("Test %d - CMobilePhoneListBase::MaxNumberEntries passed"), iTestCount++);	
	INFO_PRINTF2(_L("Test %d - CMobilePhoneListBase::AddEntryL passed"), iTestCount++);	
	INFO_PRINTF2(_L("Test %d - CMobilePhoneListBase::SetMaxNumberEntries passed"), iTestCount++);	
	INFO_PRINTF2(_L("Test %d - CMobilePhoneListBase::GetEntryL passed"), iTestCount++);	

	TBuf8<100> tmpBuffer;
	list->StoreL(tmpBuffer);

	CleanupStack::PopAndDestroy();

	list = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);
	list->RestoreL(tmpBuffer);

	TEST(list->Enumerate() == 10);

	for (i = 0; i < 10; i++)
		{
		const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& entry2 = 
			list->GetEntryL(i);
		TEST(entry2.iId == i);
		}

	INFO_PRINTF2(_L("Test %d - CMobilePhoneListBase::StoreL passed"), iTestCount++);
	INFO_PRINTF2(_L("Test %d - CMobilePhoneListBase::RestoreL passed"), iTestCount++);

	list->DeleteEntryL(0);
	list->DeleteEntryL(8);

	TEST(list->Enumerate() == 8);

	for (i = 0; i < 8; i++)
	{
		const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& entry2 = 
			list->GetEntryL(i);
		TEST(entry2.iId == i+1);
	}

	TRAP(err,list->DeleteEntryL(-1));
	TEST(err == CMobilePhoneListBase::EListIndexOutOfRange);

	TEST(list->Enumerate() == 8);

//	test.Next(_L("Delete Entry ok"));
	INFO_PRINTF2(_L("Test %d - CMobilePhoneEditableList::DeleteEntryL passed"), iTestCount++);

	entry.iId = 29;
	list->ChangeEntryL(1,entry);

	TEST(list->GetEntryL(1).iId == 29);

	entry.iId = 2;
	list->ChangeEntryL(1,entry);


	TRAP(err,list->ChangeEntryL(1000,entry));
	TEST(err == CMobilePhoneListBase::EListIndexOutOfRange);

//	test.Next(_L("Change Entry ok"));
	INFO_PRINTF2(_L("Test %d - CMobilePhoneEditableList::ChangeEntryL passed"), iTestCount++);

	entry.iId = 0;
	list->InsertEntryL(0,entry);

	entry.iId = 9;
	TRAP(err,list->InsertEntryL(9,entry));
	TEST(err == CMobilePhoneListBase::EListIndexOutOfRange);

	for (i = 0; i < 9; i++)
		{
		const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& entry2 = 
			list->GetEntryL(i);
		TEST(entry2.iId == i);
		}

	list->SetMaxNumberEntries(9);
	TRAP(err,list->InsertEntryL(0,entry));
	TEST(err == CMobilePhoneListBase::EListMaxNumberReached);

//	test.Next(_L("Insert Entry ok"));
	INFO_PRINTF2(_L("Test %d - CMobilePhoneEditableList::InsertEntryL passed"), iTestCount++);

	CleanupStack::PopAndDestroy();

	list = CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);
	
	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 start,end;

	start.iId = 0;
	end.iId = 49;

	list->AddRangeEntryL(start,end);

	start.iId = 49;
	end.iId = 100;

	list->AddRangeEntryL(start,end);
	TEST(list->Enumerate() == 100);

	for (i = 0; i < 100; i++)
		{
		const RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1& entry2 = 
			list->GetEntryL(i);
		TEST(entry2.iId == i);
		}
	
	list->SetMaxNumberEntries(110);

	TRAP(err,list->AddRangeEntryL(start,end));
	TEST(err == CMobilePhoneListBase::EListMaxNumberReached);

	TRAP(err,list->AddRangeEntryL(end,start));
	TEST(err == CMobilePhoneListBase::EBadRange);

//	test.Next(_L("AddRangeEntry ok"));
	INFO_PRINTF2(_L("Test %d - CMobilePhoneCmbiList::AddRangeEntryL passed"), iTestCount++);
	CleanupStack::PopAndDestroy();
	INFO_PRINTF1(_L(""));

	return TestStepResult();
	}
