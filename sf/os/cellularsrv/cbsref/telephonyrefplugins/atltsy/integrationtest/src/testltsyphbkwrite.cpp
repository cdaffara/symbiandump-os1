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
#include "testltsyphbkwrite.h"
//const define
const TUint8 KTypeOfNumber=0x81;

CTestLtsyPhbkWrite::CTestLtsyPhbkWrite(CTestLtsyModel& aTestModel)
	: CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkWrite::~CTestLtsyPhbkWrite()
	{
	}

CTestLtsyPhbkWrite* CTestLtsyPhbkWrite::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkWrite* self = new (ELeave)CTestLtsyPhbkWrite(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkWrite* CTestLtsyPhbkWrite::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkWrite* self=CTestLtsyPhbkWrite::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkWrite::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkWrite::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkWrite::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkWrite::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkWrite::doTestStepL called"));
	TInt index = 10;
	TRAPD(err,WriteEntryL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write phonebook"),err);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkWrite::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkWrite::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkWrite::WriteEntryL(TInt aIndex)
	{
	TRequestStatus status;
	TBuf8<800> pbData; // Client reserved space for phonebook data
	TInt ret;
	    
	_LIT16(KTel,"123456%03d");
	_LIT16(KText,"TelNo %03d");
	TBuf16<20> text(KTel), number(KText);

	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	pbBuffer->AddNewEntryTag();
	// convert number into TLV format and append it to allocated buffer
	TInt index = aIndex;
	ret = pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex,(TUint16)index);
	if( ret != KErrNone )
		{
		CleanupStack::PopAndDestroy();
		SetTestStepResult(EFail);
		}
	number.Format(KTel(), index);
	ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, number);
//	ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNewEntry, number);
	if( ret != KErrNone )
		{
		CleanupStack::PopAndDestroy();
		SetTestStepResult(EFail);
		}
	// convert number type into TLV format and append it to allocated buffer
	TUint8 tonNpi=KTypeOfNumber; 
	ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, tonNpi);
	if(ret!=KErrNone)
		{
		CleanupStack::PopAndDestroy();
		SetTestStepResult(EFail);
		}

	text.Format(KText(), index);
// convert text into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, text);
	if(ret != KErrNone)
		{
		CleanupStack::PopAndDestroy();
		SetTestStepResult(EFail);
		}
	PhoneBookStore().Write(status, pbData,index);
	User::WaitForRequest(status);
	TInt r=status.Int();
	INFO_PRINTF2(_L("Write() request status = %d"),r);
	CleanupStack::PopAndDestroy();
	}

// end of file
