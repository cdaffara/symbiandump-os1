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
#include "testltsyphbkclose.h"

CTestLtsyPhbkClose::CTestLtsyPhbkClose(CTestLtsyModel& aTestModel)
    : CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkClose::~CTestLtsyPhbkClose()
	{
	}

CTestLtsyPhbkClose* CTestLtsyPhbkClose::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkClose* self = new (ELeave)CTestLtsyPhbkClose(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkClose* CTestLtsyPhbkClose::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkClose* self=CTestLtsyPhbkClose::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkClose::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkClose::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkClose::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkClose::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkClose::doTestStepL called"));
	TRAPD(err,ClosePhonebookL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when close phonebook"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkClose::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkClose::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkClose::ClosePhonebookL()
	{
	PhoneBookStore().Close();
	}

// end of file
