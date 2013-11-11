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
#include "testltsysmsack.h"

CTestLtsySmsAck::CTestLtsySmsAck(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsAck::~CTestLtsySmsAck()
	{
	}

CTestLtsySmsAck* CTestLtsySmsAck::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsAck* self = new (ELeave)CTestLtsySmsAck(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsAck* CTestLtsySmsAck::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsAck* self=CTestLtsySmsAck::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsAck::ConstructL()
	{

	}

TVerdict CTestLtsySmsAck::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsAck::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsAck::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsAck::doTestStepL called"));
	TRAPD(err,AckSmsStoredL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when ack sms store"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsAck::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsAck::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsySmsAck::AckSmsStoredL()
	{
	TBool full(EFalse);
	TRequestStatus status;
	_LIT8(KNullAck,"00");
	Sms().AckSmsStored(status,KNullAck(),full);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("AckSmsStored() request status = %d"),r);
	}

//End of file
