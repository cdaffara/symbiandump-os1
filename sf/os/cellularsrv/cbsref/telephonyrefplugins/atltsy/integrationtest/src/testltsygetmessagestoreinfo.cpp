//
// testltsygetmessagestoreinfo.cpp
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

#include "testltsygetmessagestoreinfo.h"

CTestLtsyGetMessageStoreInfo::CTestLtsyGetMessageStoreInfo(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsyGetMessageStoreInfo::~CTestLtsyGetMessageStoreInfo()
	{
	}

CTestLtsyGetMessageStoreInfo* CTestLtsyGetMessageStoreInfo::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyGetMessageStoreInfo* self = new (ELeave)CTestLtsyGetMessageStoreInfo(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyGetMessageStoreInfo* CTestLtsyGetMessageStoreInfo::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyGetMessageStoreInfo* self=CTestLtsyGetMessageStoreInfo::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyGetMessageStoreInfo::ConstructL()
	{

	}

TVerdict CTestLtsyGetMessageStoreInfo::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyGetMessageStoreInfo::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyGetMessageStoreInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyGetMessageStoreInfo::doTestStepL called"));
	TRAPD(err,GetMessageStoreInfoL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when get message store info"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyGetMessageStoreInfo::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyGetMessageStoreInfo::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyGetMessageStoreInfo::GetMessageStoreInfoL()
	{
	TInt  smsIndex(0);             //smsIndex always is zero
	TRequestStatus status;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg smsInfoPckg(iInfo);
	Sms().GetMessageStoreInfo(status,smsIndex,smsInfoPckg);
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("GetMessageStoreInfo() request status = %d"),r);
	CheckMessageStoreInfoL();
	INFO_PRINTF4(_L("GetMessageStoreInfo MobileTypeName = %S, TotalEntries=%d,UsedEntries=%d\n"),&iInfo.iName,iInfo.iTotalEntries,iInfo.iUsedEntries );
	}

void CTestLtsyGetMessageStoreInfo::CheckMessageStoreInfoL()
	{
	TBool matched = ETrue;
	if (!iInfo.iName.Compare(KETelIccSmsStore) == 0)
		{
		INFO_PRINTF2(_L("The tel sms store name %s could not match with expect one"),&(iInfo.iName));
		matched = EFalse;
		}
	if(!matched)
		{
		SetTestStepResult(EFail);
		}
	}
