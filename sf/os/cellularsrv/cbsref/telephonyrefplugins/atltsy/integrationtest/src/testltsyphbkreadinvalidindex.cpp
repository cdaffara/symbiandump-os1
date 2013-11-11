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

//system include 
#include <mpbutil.h> 
//user include
#include "testltsyphbkreadinvalidindex.h"



CTestLtsyPhbkReadInvalidIndex::CTestLtsyPhbkReadInvalidIndex(CTestLtsyModel& aTestModel)
	: CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkReadInvalidIndex::~CTestLtsyPhbkReadInvalidIndex()
	{
	}

CTestLtsyPhbkReadInvalidIndex* CTestLtsyPhbkReadInvalidIndex::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkReadInvalidIndex* self = new (ELeave)CTestLtsyPhbkReadInvalidIndex(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkReadInvalidIndex* CTestLtsyPhbkReadInvalidIndex::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkReadInvalidIndex* self=CTestLtsyPhbkReadInvalidIndex::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkReadInvalidIndex::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkReadInvalidIndex::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkReadInvalidIndex::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkReadInvalidIndex::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkReadInvalidIndex::doTestStepL called"));
	TInt index = 0;
	TRAPD(err,ReadEntryL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when read phonebook"),err);
	    SetTestStepResult(EFail);
		}
	
	index = -2;
	TRAPD(err1,ReadEntryL(index));
	if(err1 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when read phonebook"),err1);
	    SetTestStepResult(EFail);
		}
	
    index = 600;
	TRAPD(err2,ReadEntryL(index));
	if(err2 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when read phonebook"),err2);
	    SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkReadInvalidIndex::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkReadInvalidIndex::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkReadInvalidIndex::ReadEntryL(TInt aIndex)
	{
	TRequestStatus status;
	TBuf8<800> outputPbData;
	PhoneBookStore().Read(status,aIndex,1,outputPbData);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("Write() request status = %d"),r);
	}

// end of file
