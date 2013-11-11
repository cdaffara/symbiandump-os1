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
#include "testltsyphbkread.h"
//const defination
_LIT(KExpectedText, "TelNo 010");
_LIT(KExpectedNumber,"+123456010");
const TUint KExpectedIndex = 10;

CTestLtsyPhbkRead::CTestLtsyPhbkRead(CTestLtsyModel& aTestModel)
	: CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkRead::~CTestLtsyPhbkRead()
	{
	}

CTestLtsyPhbkRead* CTestLtsyPhbkRead::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkRead* self = new (ELeave)CTestLtsyPhbkRead(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkRead* CTestLtsyPhbkRead::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkRead* self=CTestLtsyPhbkRead::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkRead::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkRead::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkRead::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkRead::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkRead::doTestStepL called"));
	TInt index = 10;
	TRAPD(err,ReadEntryL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when read phonebook"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkRead::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkRead::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkRead::ReadEntryL(TInt aIndex)
	{
	TRequestStatus status;
	TBuf8<800> outputPbData;
	TUint16 index(0);
	TUint8 tonNpi;
	TPtrC16 number(NULL,0);
	TPtrC16 text(NULL,0);
	TUint8 tagValue(0);
	CPhoneBookBuffer::TPhBkTagType dataType;
	
	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read phonebook data
	CleanupStack::PushL(pbBuffer);
	PhoneBookStore().Read(status,aIndex,1,outputPbData);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("Write() request status = %d"),r);
	// Set pbBuffer to point to received phonebook data
	pbBuffer->Set(&outputPbData); 
	// Start reading received phonebook data
	pbBuffer->StartRead();       
	while (pbBuffer->GetTagAndType(tagValue,dataType)==KErrNone)
		{
		switch (tagValue)
			{
		case RMobilePhoneBookStore::ETagPBAdnIndex:
			pbBuffer->GetValue(index);			
			break;
		case RMobilePhoneBookStore::ETagPBTonNpi:
			pbBuffer->GetValue(tonNpi);			
			break;
		case RMobilePhoneBookStore::ETagPBText:
			pbBuffer->GetValue(text);			
			break;
		case RMobilePhoneBookStore::ETagPBNumber:
			pbBuffer->GetValue(number);			
			break;
		default:
			// An unsupported field type - just skip this value
			pbBuffer->SkipValue(dataType);
			break;
			}
		}
	if((index != KExpectedIndex)||(text.MatchF(KExpectedText)==KErrNotFound)||
			                      (number.MatchF(KExpectedNumber)==KErrNotFound))
		{
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy();             
	}

// end of file
