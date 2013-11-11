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
#include "testltsyphbkdelete.h"

CTestLtsyPhbkDelete::CTestLtsyPhbkDelete(CTestLtsyModel& aTestModel)
	: CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkDelete::~CTestLtsyPhbkDelete()
	{
	}

CTestLtsyPhbkDelete* CTestLtsyPhbkDelete::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkDelete* self = new (ELeave)CTestLtsyPhbkDelete(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkDelete* CTestLtsyPhbkDelete::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkDelete* self=CTestLtsyPhbkDelete::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkDelete::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkDelete::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkDelete::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkDelete::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkDelete::doTestStepL called"));
	TInt index = 10;
	TRAPD(err,DeletePhonebookL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete phonebook"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkDelete::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkDelete::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkDelete::DeletePhonebookL(TInt aIndex)
	{
	TRequestStatus status;
	PhoneBookStore().Delete(status,aIndex);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("Delete() request status = %d"),r);
	}

// end of file
