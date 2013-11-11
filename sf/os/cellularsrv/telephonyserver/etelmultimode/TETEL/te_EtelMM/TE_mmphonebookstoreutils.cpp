// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmphonebookstoreutils.h"
#include "testdef.h"

CTestPhonebookStoreUtils::CTestPhonebookStoreUtils()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestPhonebookStoreU"));
	}

enum TVerdict CTestPhonebookStoreUtils::doTestStepL()
	{
	//
	// Create a buffer
	//
	TBuf8<200> buf;
	CPhoneBookBuffer* phonebookBuffer=new(ELeave) CPhoneBookBuffer();

	//
	// Setup the phonebook buffer class
	//
	phonebookBuffer->Set(&buf);

	//
	// Write first entry
	//
	// A tag is 1 byte, each string requires a 16 bit length value and
	// all other values are as sized. Unicode strings must be word aligned
	// on a 4 byte boundary.
	//
	// Add new entry tag to the phonebook buffer
	//
	TInt  bufferLength = 0;
	TInt  result;
	
	result = phonebookBuffer->AddNewEntryTag();
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte

	// Add a tag and integer to the phonebook buffer
	TUint16 index1=1; 
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex,index1);
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte
	bufferLength += 2; // 16 bit value is 2 bytes

	// Add a tag and a descriptor to the phonebook buffer
	_LIT16(KName,"First Name"); 
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText,KName);
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte
	bufferLength += 2; // 16 bit length value is 2 bytes
	bufferLength += (4 - (bufferLength % 4)) %4; // Word align Unicode strings
	bufferLength += (KName.iTypeLength * 2); // Unicode sting

	_LIT16(KNumber,"01632960000"); 
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,KNumber);
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte
	bufferLength += 2; // 16 bit length value is 2 bytes
	bufferLength += (4 - (bufferLength % 4)) %4; // Word align Unicode strings
	bufferLength += (KNumber.iTypeLength * 2); // Unicode sting

	TUint8 tonNpi=0x91; // international TON/NPI 
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,tonNpi);
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte
	bufferLength += 1; // 8 bit length value is 1 byte

	// Try adding some invalid tags...
	_LIT8(KText8Tag, "8 bit text"); 
	_LIT16(KText16Tag, "16 bit text"); 
	TUint8   uint8Tag(10);
	TUint16  uint16Tag(55555);
	TUint32  uint32Tag(10203040);

	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, KText8Tag);
	TEST(result == KErrArgument);
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, KText16Tag);
	TEST(result == KErrArgument);
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex, uint8Tag);
	TEST(result == KErrArgument);
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, uint16Tag);
	TEST(result == KErrArgument);
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex, uint32Tag);
	TEST(result == KErrArgument);

	//
	// Write second entry
	//
	// Add new entry tag to the phonebook buffer
	//
	result = phonebookBuffer->AddNewEntryTag();
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte

	// Add a tag and integer to the phonebook buffer
	TUint16 index2=2;
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex,index2);
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte
	bufferLength += 2; // 16 bit value is 2 bytes

	// Add a tag and a descriptor to the phonebook buffer
	_LIT16(KName2,"Second Name");
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText,KName2);
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte
	bufferLength += 2; // 16 bit length value is 2 bytes
	bufferLength += (4 - (bufferLength % 4)) %4; // Word align Unicode strings
	bufferLength += (KName2.iTypeLength * 2); // Unicode sting

	_LIT16(KNumber2,"01632960000");
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,KNumber2);
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte
	bufferLength += 2; // 16 bit length value is 2 bytes
	bufferLength += (4 - (bufferLength % 4)) %4; // Word align Unicode strings
	bufferLength += (KNumber2.iTypeLength * 2); // Unicode sting

	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,tonNpi);
	TEST(result == KErrNone);
	bufferLength += 1; // Tag equals 1 byte
	bufferLength += 1; // 8 bit length value is 1 byte

	//
	// Write a partial third entry
	//
	// Add new entry tag to the phonebook buffer
	//
	result = phonebookBuffer->AddNewEntryTag();
	TEST(result == KErrNone);

	// Add a tag and integer to the phonebook buffer
	TUint16 index3=3;
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex,index3);
	TEST(result == KErrNone);

	// Add a tag and a descriptor to the phonebook buffer
	_LIT16(KName3,"Third Name");
	result = phonebookBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText,KName3);
	TEST(result == KErrNone);

	//
	// Now remove third entry from the buffer - this is just to test removing mechanism
	//
	result = phonebookBuffer->RemovePartialEntry();
	TEST(result == KErrNone);

	//
	// Start reading the buffer
	//
	phonebookBuffer->StartRead();

	//
	// Get buffer length
	//
	TInt  phonebookBufferLen = phonebookBuffer->BufferLength();
	TEST(phonebookBufferLen == bufferLength);

	//
	// First Entry
	// Read Tag Value and Data Type
	//
	TUint8 tag;
	CPhoneBookBuffer::TPhBkTagType dataType;
	
	result = phonebookBuffer->GetTagAndType(tag, dataType); // new entry tag
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBNewEntry);

	// Read and Validate the integer
	result = phonebookBuffer->GetTagAndType(tag, dataType); // data tag
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBAdnIndex);
	TEST(dataType==CPhoneBookBuffer::EPhBkTypeInt16);

	TUint16 readIndex1;
	result = phonebookBuffer->GetValue(readIndex1);
	TEST(result == KErrNone);
	TEST(readIndex1==index1);

	// Read and Validate the descriptor
	result = phonebookBuffer->GetTagAndType(tag, dataType);
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBText);
	TEST(dataType==CPhoneBookBuffer::EPhBkTypeDes16);

	TBuf16<100> readBuf;
	TPtrC16 readBufPtr(readBuf);
	result = phonebookBuffer->GetValue(readBufPtr);
	TEST(result == KErrNone);
	TEST(readBufPtr.Compare(KName)==0);

	// Read and Validate the next descriptor
	result = phonebookBuffer->GetTagAndType(tag, dataType);
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBNumber);
	TEST(dataType==CPhoneBookBuffer::EPhBkTypeDes16);

	result = phonebookBuffer->GetValue(readBufPtr);
	TEST(result == KErrNone);
	TEST(readBufPtr.Compare(KNumber)==0);

	// Read and Validate the byte
	result = phonebookBuffer->GetTagAndType(tag, dataType); // data tag
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBTonNpi);
	TEST(dataType==CPhoneBookBuffer::EPhBkTypeInt8);

	TUint8 readTonNpi;
	result = phonebookBuffer->GetValue(readTonNpi);
	TEST(result == KErrNone);
	TEST(readTonNpi==tonNpi);

	//
	// Second Entry
	//
	result = phonebookBuffer->GetTagAndType(tag, dataType); // new entry tag
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBNewEntry);

	// Read and Validate the integer
	result = phonebookBuffer->GetTagAndType(tag, dataType); // data tag
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBAdnIndex);
	TEST(dataType==CPhoneBookBuffer::EPhBkTypeInt16);

	TUint16 readIndex2;
	result = phonebookBuffer->GetValue(readIndex2);
	TEST(result == KErrNone);
	TEST(readIndex2==index2);

	// Read and Validate the descriptor
	result = phonebookBuffer->GetTagAndType(tag, dataType);
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBText);
	TEST(dataType==CPhoneBookBuffer::EPhBkTypeDes16);

	result = phonebookBuffer->GetValue(readBufPtr);
	TEST(result == KErrNone);
	TEST(readBufPtr.Compare(KName2)==0);

	// Read and Validate the next descriptor
	result = phonebookBuffer->GetTagAndType(tag, dataType);
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBNumber);
	TEST(dataType==CPhoneBookBuffer::EPhBkTypeDes16);

	result = phonebookBuffer->GetValue(readBufPtr);
	TEST(result == KErrNone);
	TEST(readBufPtr.Compare(KNumber2)==0);

	// Read and Validate the byte
	result = phonebookBuffer->GetTagAndType(tag, dataType); // data tag
	TEST(result == KErrNone);
	TEST(tag==RMobilePhoneBookStore::ETagPBTonNpi);
	TEST(dataType==CPhoneBookBuffer::EPhBkTypeInt8);

	result = phonebookBuffer->GetValue(readTonNpi);
	TEST(result == KErrNone);
	TEST(readTonNpi==tonNpi);

	//
	// Check that buffer is now finished
	//
	result = phonebookBuffer->GetTagAndType(tag, dataType); // data tag
	TEST(result==KErrNotFound);

	delete phonebookBuffer;

	return TestStepResult();
	}


