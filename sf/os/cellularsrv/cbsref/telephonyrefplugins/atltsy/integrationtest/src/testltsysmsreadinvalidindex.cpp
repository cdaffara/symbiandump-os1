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
#include "testltsysmsreadinvalidindex.h"
#include "testltsysmsatutil.h"

CTestLtsySmsReadInvalidIndex::CTestLtsySmsReadInvalidIndex(CTestLtsyModel& aTestModel)
	: CTestLtsySmsBase(aTestModel)
	{

	}

CTestLtsySmsReadInvalidIndex::~CTestLtsySmsReadInvalidIndex()
	{
	}

CTestLtsySmsReadInvalidIndex* CTestLtsySmsReadInvalidIndex::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsReadInvalidIndex* self = new (ELeave)CTestLtsySmsReadInvalidIndex(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsySmsReadInvalidIndex* CTestLtsySmsReadInvalidIndex::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsySmsReadInvalidIndex* self=CTestLtsySmsReadInvalidIndex::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsySmsReadInvalidIndex::ConstructL()
	{

	}

TVerdict CTestLtsySmsReadInvalidIndex::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsReadInvalidIndex::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsySmsReadInvalidIndex::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsReadInvalidIndex::doTestStepL called"));
	TInt index(0);
	TRAPD(err,ReadSmsL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete sms"),err);
	    SetTestStepResult(EFail);
		}
	
	index = -2;
	TRAPD(err1,ReadSmsL(index));
	if(err1 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete sms"),err1);
	    SetTestStepResult(EFail);
		}
	
	index = 50;
	TRAPD(err2,ReadSmsL(index));
	if(err2 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when delete sms"),err2);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsySmsReadInvalidIndex::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsySmsReadInvalidIndex::doTestStepPostambleL called"));
	return TestStepResult();
	}
void CTestLtsySmsReadInvalidIndex::ReadSmsL(TInt aIndex)
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
