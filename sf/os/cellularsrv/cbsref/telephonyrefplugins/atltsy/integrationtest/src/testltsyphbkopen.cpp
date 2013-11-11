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
#include "testltsyphbkopen.h"

CTestLtsyPhbkOpen::CTestLtsyPhbkOpen(CTestLtsyModel& aTestModel)
	: CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkOpen::~CTestLtsyPhbkOpen()
	{
	}

CTestLtsyPhbkOpen* CTestLtsyPhbkOpen::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkOpen* self = new (ELeave)CTestLtsyPhbkOpen(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkOpen* CTestLtsyPhbkOpen::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkOpen* self=CTestLtsyPhbkOpen::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkOpen::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkOpen::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkOpen::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkOpen::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkOpen::doTestStepL called"));
	TRAPD(err,OpenPhonebookL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when open phonebook"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkOpen::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkOpen::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkOpen::OpenPhonebookL()
	{
	PhoneBookStore().Open(iModel.SharedEnv().Phone(),  KETelIccAdnPhoneBook);
//	PhoneBookStore().Open(iModel.SharedEnv().Phone(),  KETelIccFdnPhoneBook);
	}

// end of file
