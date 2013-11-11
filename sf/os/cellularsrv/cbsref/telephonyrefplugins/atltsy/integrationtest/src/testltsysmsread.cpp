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
#include "testltsysmsread.h"
#include "testltsysmsatutil.h"

CTestLtsySmsRead::CTestLtsySmsRead(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsRead::~CTestLtsySmsRead()
	{
	}

CTestLtsySmsRead* CTestLtsySmsRead::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsRead* self = new (ELeave)CTestLtsySmsRead(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsRead* CTestLtsySmsRead::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsRead* self=CTestLtsySmsRead::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsRead::ConstructL()
	{

	}

TVerdict CTestLtsySmsRead::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsRead::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsRead::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsRead::doTestStepL called"));
	TInt index(6);
	TRAPD(err,ReadSmsL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete sms"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsRead::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsRead::doTestStepPostambleL called"));
	return TestStepResult();
	}
void CTestLtsySmsRead::ReadSmsL(TInt aIndex)
	{
	TRequestStatus status;
	TPckg<RMobileSmsStore::TMobileGsmSmsEntryV1> pckgEntryV1(iEntryV1);
	iEntryV1.iIndex = aIndex;		
	SmsStore().Read(status, pckgEntryV1);
	User::WaitForRequest(status);
	TInt r = status.Int();
	INFO_PRINTF2(_L("Delete() request status = %d"),r);
	TBuf<400>  msgAsciiData;
	TBuf8<400> msgAscii; 
	SmsAtUtil::AppendDataToAscii(msgAscii,iEntryV1.iMsgData);
	msgAsciiData.Copy( msgAscii );
	INFO_PRINTF4(_L("SmsStoreRead index=%d,status=%d,msgData=%S\n"),
			        iEntryV1.iIndex,iEntryV1.iMsgStatus,&msgAsciiData);
	}

//End of file
