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
#include "testltsyclosesms.h"

CTestLtsyCloseSms::CTestLtsyCloseSms(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsyCloseSms::~CTestLtsyCloseSms()
	{
	}

CTestLtsyCloseSms* CTestLtsyCloseSms::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyCloseSms* self = new (ELeave)CTestLtsyCloseSms(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyCloseSms* CTestLtsyCloseSms::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyCloseSms* self=CTestLtsyCloseSms::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyCloseSms::ConstructL()
	{

	}

TVerdict CTestLtsyCloseSms::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyCloseSms::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyCloseSms::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyCloseSms::doTestStepL called"));
	TRAPD(err,CloseSmsL(););
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when close sms"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyCloseSms::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyCloseSms::doTestStepPostambleL called"));
	return TestStepResult();
	}
