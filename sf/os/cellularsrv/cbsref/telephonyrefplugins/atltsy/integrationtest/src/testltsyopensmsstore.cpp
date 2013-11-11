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
#include "testltsyopensmsstore.h"

CTestLtsyOpenSmsStore::CTestLtsyOpenSmsStore(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsyOpenSmsStore::~CTestLtsyOpenSmsStore()
	{
	}

CTestLtsyOpenSmsStore* CTestLtsyOpenSmsStore::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyOpenSmsStore* self = new (ELeave)CTestLtsyOpenSmsStore(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyOpenSmsStore* CTestLtsyOpenSmsStore::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyOpenSmsStore* self=CTestLtsyOpenSmsStore::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyOpenSmsStore::ConstructL()
	{

	}

TVerdict CTestLtsyOpenSmsStore::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyOpenSmsStore::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyOpenSmsStore::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyOpenSmsStore::doTestStepL called"));
	TRAPD(err,OpenSmsStoreL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when open sms store"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyOpenSmsStore::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyOpenSmsStore::doTestStepPostambleL called"));
	return TestStepResult();
	}
