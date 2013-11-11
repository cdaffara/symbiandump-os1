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
#include "testltsyclosesmsstore.h"

CTestLtsyCloseSmsStore::CTestLtsyCloseSmsStore(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsyCloseSmsStore::~CTestLtsyCloseSmsStore()
	{
	}

CTestLtsyCloseSmsStore* CTestLtsyCloseSmsStore::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyCloseSmsStore* self = new (ELeave)CTestLtsyCloseSmsStore(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyCloseSmsStore* CTestLtsyCloseSmsStore::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyCloseSmsStore* self=CTestLtsyCloseSmsStore::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyCloseSmsStore::ConstructL()
	{

	}

TVerdict CTestLtsyCloseSmsStore::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyCloseSmsStore::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyCloseSmsStore::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyCloseSmsStore::doTestStepL called"));
	TRAPD(err,CloseSmsStoreL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when close sms store"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyCloseSmsStore::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyCloseSmsStore::doTestStepPostambleL called"));
	return TestStepResult();
	}
