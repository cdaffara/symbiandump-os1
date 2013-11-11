//
// testltsygetmessagestoreinfo.cpp
//
// Copyright (c) Symbian Software Ltd. Your copyright notice.  All rights reserved.
//

#include "testltsysmsstoregetinfo.h"

CTestLtsySmsStoreGetInfo::CTestLtsySmsStoreGetInfo(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsStoreGetInfo::~CTestLtsySmsStoreGetInfo()
	{
	}

CTestLtsySmsStoreGetInfo* CTestLtsySmsStoreGetInfo::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsStoreGetInfo* self = new (ELeave)CTestLtsySmsStoreGetInfo(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsStoreGetInfo* CTestLtsySmsStoreGetInfo::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsStoreGetInfo* self=CTestLtsySmsStoreGetInfo::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsStoreGetInfo::ConstructL()
	{

	}

TVerdict CTestLtsySmsStoreGetInfo::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsStoreGetInfo::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsStoreGetInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsStoreGetInfo::doTestStepL called"));
	TRAPD(err,GetSmsStoreInfoL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when get sms store info"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsStoreGetInfo::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsStoreGetInfo::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsySmsStoreGetInfo::GetSmsStoreInfoL()
	{
	TRequestStatus status;
	TPckg<RMobilePhoneBookStore::TMobilePhoneBookInfoV1> smsInfoPckg(iInfo);
	SmsStore().GetInfo(status,smsInfoPckg);
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("GetInfo() request status = %d"),r);
	CheckSmsStoreInfoL();
	INFO_PRINTF4(_L("GetInfo() MobileTypeName = %S, TotalEntries=%d,UsedEntries=%d\n"),&iInfo.iName,iInfo.iTotalEntries,iInfo.iUsedEntries );
	}

void CTestLtsySmsStoreGetInfo::CheckSmsStoreInfoL()
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
