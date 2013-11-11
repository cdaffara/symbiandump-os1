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
#include "testltsyphbkwriteinvalidindex.h"
//const define
const TUint8 KTypeOfNumber=0x81;

CTestLtsyPhbkWriteInvalidIndex::CTestLtsyPhbkWriteInvalidIndex(CTestLtsyModel& aTestModel)
	: CTestLtsyPhoneStoreBase(aTestModel)
	{

	}

CTestLtsyPhbkWriteInvalidIndex::~CTestLtsyPhbkWriteInvalidIndex()
	{
	}

CTestLtsyPhbkWriteInvalidIndex* CTestLtsyPhbkWriteInvalidIndex::NewLC(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkWriteInvalidIndex* self = new (ELeave)CTestLtsyPhbkWriteInvalidIndex(aTestModel);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestLtsyPhbkWriteInvalidIndex* CTestLtsyPhbkWriteInvalidIndex::NewL(CTestLtsyModel& aTestModel)
	{
	CTestLtsyPhbkWriteInvalidIndex* self=CTestLtsyPhbkWriteInvalidIndex::NewLC(aTestModel);
	CleanupStack::Pop(self);
	return self;
	}

void CTestLtsyPhbkWriteInvalidIndex::ConstructL()
	{

	}

TVerdict CTestLtsyPhbkWriteInvalidIndex::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkWriteInvalidIndex::doTestStepPreambleL called"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkWriteInvalidIndex::doTestStepL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkWriteInvalidIndex::doTestStepL called"));
	TInt index = -1;
	TRAPD(err,WriteEntryL(index));
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write phonebook"),err);
	    SetTestStepResult(EFail);
		}
	
	index = 0;
	TRAPD(err1,WriteEntryL(index));
	if(err1 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write phonebook"),err1);
	    SetTestStepResult(EFail);
		}
	
	index = 256;
	TRAPD(err2,WriteEntryL(index));
	if(err2 != KErrNone)
		{
		INFO_PRINTF2(_L("The error was returned %d!! when write phonebook"),err2);
	    SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTestLtsyPhbkWriteInvalidIndex::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("CTestLtsyPhbkWriteInvalidIndex::doTestStepPostambleL called"));
	return TestStepResult();
	}

void CTestLtsyPhbkWriteInvalidIndex::WriteEntryL(TInt aIndex)
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
