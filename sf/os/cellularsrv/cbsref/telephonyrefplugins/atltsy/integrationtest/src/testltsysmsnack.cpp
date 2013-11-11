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
#include "testltsysmsnack.h"
#include "testltsysmsatutil.h"

CTestLtsySmsNack::CTestLtsySmsNack(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsNack::~CTestLtsySmsNack()
	{
	}

CTestLtsySmsNack* CTestLtsySmsNack::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsNack* self = new (ELeave)CTestLtsySmsNack(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsNack* CTestLtsySmsNack::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsNack* self=CTestLtsySmsNack::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsNack::ConstructL()
	{

	}

TVerdict CTestLtsySmsNack::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsNack::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsNack::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsNack::doTestStepL called"));
	TRAPD(err,NackSmsStoredL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when nack sms store"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsNack::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsNack::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsySmsNack::NackSmsStoredL()
	{
	_LIT8(KTest,"9f05");
	TBuf8<12> nackAsciiTest = KTest();
	TBuf8<12> nackTest;
	SmsAtUtil::ConvertAsciiToBinary(nackAsciiTest,nackTest);
	TRequestStatus status;
	TInt rpCause(2);  
	
	Sms().NackSmsStored(status,nackTest,rpCause);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("AckSmsStored() request status = %d"),r);
	}

//End of file
