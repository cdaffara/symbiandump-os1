// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 

//user include
#include "testltsyphbkgetstoreinfo.h"

CTestLtsyPhbkGetStoreInfo::CTestLtsyPhbkGetStoreInfo(CTestLtsyModel& aTestModel)
	: CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkGetStoreInfo::~CTestLtsyPhbkGetStoreInfo()
	{
	}

CTestLtsyPhbkGetStoreInfo* CTestLtsyPhbkGetStoreInfo::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkGetStoreInfo* self = new (ELeave)CTestLtsyPhbkGetStoreInfo(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkGetStoreInfo* CTestLtsyPhbkGetStoreInfo::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkGetStoreInfo* self=CTestLtsyPhbkGetStoreInfo::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkGetStoreInfo::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkGetStoreInfo::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkGetStoreInfo::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkGetStoreInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkGetStoreInfo::doTestStepL called"));
	TRAPD(err,GetStoreInfoL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when get phonebook store info"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkGetStoreInfo::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkGetStoreInfo::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkGetStoreInfo::GetStoreInfoL()
	{
	TRequestStatus status;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	iModel.SharedEnv().Phone().GetPhoneStoreInfo(status, phoneStoreInfoPckg, KETelIccSmsStore);
//	iModel.SharedEnv().Phone().GetPhoneStoreInfo(status, phoneStoreInfoPckg, KETelIccFdnPhoneBook);
//	iiModel.SharedEnv().Phone().GetPhoneStoreInfo(status, phoneStoreInfoPckg, KETelIccAdnPhoneBook);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("GetPhoneStoreInfo() request status = %d"),r);
	}

// end of file
