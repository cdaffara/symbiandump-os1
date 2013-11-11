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
#include "testltsyphbkdeleteinvalidindex.h"

CTestLtsyPhbkDeleteInvalidIndex::CTestLtsyPhbkDeleteInvalidIndex(CTestLtsyModel& aTestModel)
	: CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkDeleteInvalidIndex::~CTestLtsyPhbkDeleteInvalidIndex()
	{
	}

CTestLtsyPhbkDeleteInvalidIndex* CTestLtsyPhbkDeleteInvalidIndex::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkDeleteInvalidIndex* self = new (ELeave)CTestLtsyPhbkDeleteInvalidIndex(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkDeleteInvalidIndex* CTestLtsyPhbkDeleteInvalidIndex::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkDeleteInvalidIndex* self=CTestLtsyPhbkDeleteInvalidIndex::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkDeleteInvalidIndex::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkDeleteInvalidIndex::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkDeleteInvalidIndex::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkDeleteInvalidIndex::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkDeleteInvalidIndex::doTestStepL called"));
	TInt index = 251;
	TRAPD(err,DeletePhonebookL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete phonebook"),err);
	    SetTestStepResult(EFail);
		}
	
	index = 0;
	TRAPD(err1,DeletePhonebookL(index));
	if(err1 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete phonebook"),err1);
	    SetTestStepResult(EFail);
		}
	
	index = -5;
	TRAPD(err2,DeletePhonebookL(index));
	if(err2 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete phonebook"),err2);
	    SetTestStepResult(EFail);
		}
		
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkDeleteInvalidIndex::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkDeleteInvalidIndex::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkDeleteInvalidIndex::DeletePhonebookL(TInt aIndex)
	{
	TRequestStatus status;
	PhoneBookStore().Delete(status,aIndex);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("Delete() request status = %d"),r);
	}

// end of file
