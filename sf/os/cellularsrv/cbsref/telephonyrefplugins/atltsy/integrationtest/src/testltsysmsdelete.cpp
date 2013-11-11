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
#include "testltsysmsdelete.h"

CTestLtsySmsDelete::CTestLtsySmsDelete(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsDelete::~CTestLtsySmsDelete()
	{
	}

CTestLtsySmsDelete* CTestLtsySmsDelete::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsDelete* self = new (ELeave)CTestLtsySmsDelete(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsDelete* CTestLtsySmsDelete::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsDelete* self=CTestLtsySmsDelete::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsDelete::ConstructL()
	{

	}

TVerdict CTestLtsySmsDelete::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDelete::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsDelete::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDelete::doTestStepL called"));
	TInt index(6);
	TRAPD(err,DeleteSmsL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete sms"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsDelete::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDelete::doTestStepPostambleL called"));
	return TestStepResult();
	}
void CTestLtsySmsDelete::DeleteSmsL(TInt aIndex)
	{
	TRequestStatus status;
	SmsStore().Delete(status,aIndex);
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("Delete() request status = %d"),r);
	}

//End of file
