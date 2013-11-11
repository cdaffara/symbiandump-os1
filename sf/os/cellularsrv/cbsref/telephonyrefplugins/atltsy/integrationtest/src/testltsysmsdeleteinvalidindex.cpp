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
#include "testltsysmsdeleteinvalidindex.h"

CTestLtsySmsDeleteInvalidIndex::CTestLtsySmsDeleteInvalidIndex(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsDeleteInvalidIndex::~CTestLtsySmsDeleteInvalidIndex()
	{
	}

CTestLtsySmsDeleteInvalidIndex* CTestLtsySmsDeleteInvalidIndex::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsDeleteInvalidIndex* self = new (ELeave)CTestLtsySmsDeleteInvalidIndex(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsDeleteInvalidIndex* CTestLtsySmsDeleteInvalidIndex::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsDeleteInvalidIndex* self=CTestLtsySmsDeleteInvalidIndex::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsDeleteInvalidIndex::ConstructL()
	{

	}

TVerdict CTestLtsySmsDeleteInvalidIndex::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDeleteInvalidIndex::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsDeleteInvalidIndex::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDeleteInvalidIndex::doTestStepL called"));
	TInt index(0);
	TRAPD(err,DeleteSmsL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete sms"),err);
	    SetTestStepResult(EFail);
		}
	index = 46;
	TRAPD(err1,DeleteSmsL(index));
	if(err1 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete sms"),err1);
	    SetTestStepResult(EFail);
		}
	
	index = -1;
	TRAPD(err2,DeleteSmsL(index));
	if(err2 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete sms"),err2);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsDeleteInvalidIndex::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDeleteInvalidIndex::doTestStepPostambleL called"));
	return TestStepResult();
	}
void CTestLtsySmsDeleteInvalidIndex::DeleteSmsL(TInt aIndex)
	{
	TRequestStatus status;
	SmsStore().Delete(status,aIndex);
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("Delete() request status = %d"),r);
	}

//End of file
