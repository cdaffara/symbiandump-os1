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
#include "testltsyopensms.h"

CTestLtsyOpenSms::CTestLtsyOpenSms(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsyOpenSms::~CTestLtsyOpenSms()
	{
	}

CTestLtsyOpenSms* CTestLtsyOpenSms::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyOpenSms* self = new (ELeave)CTestLtsyOpenSms(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyOpenSms* CTestLtsyOpenSms::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyOpenSms* self=CTestLtsyOpenSms::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyOpenSms::ConstructL()
	{

	}

TVerdict CTestLtsyOpenSms::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyOpenSms::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyOpenSms::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyOpenSms::doTestStepL called"));
	TRAPD(err,OpenSmsL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when open sms"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyOpenSms::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyOpenSms::doTestStepPostambleL called"));
	return TestStepResult();
	}
