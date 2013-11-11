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
#include "testltsysmsdeleteall.h"

CTestLtsySmsDeleteAll::CTestLtsySmsDeleteAll(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsDeleteAll::~CTestLtsySmsDeleteAll()
	{
	}

CTestLtsySmsDeleteAll* CTestLtsySmsDeleteAll::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsDeleteAll* self = new (ELeave)CTestLtsySmsDeleteAll(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsDeleteAll* CTestLtsySmsDeleteAll::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsDeleteAll* self=CTestLtsySmsDeleteAll::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsDeleteAll::ConstructL()
	{

	}

TVerdict CTestLtsySmsDeleteAll::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDeleteAll::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsDeleteAll::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDeleteAll::doTestStepL called"));
	TRAPD(err,DeleteAllSmsL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete all"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsDeleteAll::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsDeleteAll::doTestStepPostambleL called"));
	return TestStepResult();
	}
void CTestLtsySmsDeleteAll::DeleteAllSmsL()
	{
	TRequestStatus status;
	SmsStore().DeleteAll( status );
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("DeleteAll() request status = %d"),r);
	}

//End of file
