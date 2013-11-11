
// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TS_SimPhBk.CPP
// This tests the GSM Phone Book functions including phonebook searching.
// The PhoneBook search functions are implemented in the T_SEARCH files.
// 
//

/**
 @file
*/

#include "Te_SimPhBk.h"
#include "Te_SimPhBkConstants.h"

#define __TEST_CANCEL__

#define __TEST_WRITE_TO_NEXT_AVAIL__


#define SIM_TSY

//------------------------------------------------------------------
//						** TEST STEPS ***
//------------------------------------------------------------------

CTestPhoneInfo::CTestPhoneInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("testPhoneInfo"));
	}
//------------------------------------------------------------------

TVerdict CTestPhoneInfo::doTestStepL( void )
/**
 * This function gets information about the phone: the Phone Manufacturer's Id, the
 * Model and Revision ID, the Serial number and the number of phone books supported,
 * as well as information associated with each supported phone book.
 * The functions are called on a RBasicGsmPhone object.
 */
	{
	RMobilePhone::TMobilePhoneIdentityV1 aPhoneId;
	TRequestStatus aStatus;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 aPhoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg aPhoneStoreInfoPckg(aPhoneStoreInfo);

	iPhone.GetPhoneId(aStatus, aPhoneId);
	User::WaitForRequest(aStatus);
	if(aStatus == KErrNotSupported)
		{
		INFO_PRINTF1(_L("iPhone.GetPhoneId(aStatus, aPhoneId) not supported"));
		}

	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelMeMissedPhoneBook);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"MISSED_CALLS\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"MISSED_CALLS\" not supported by this phone"));
	    }


	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelMeReceivedPhoneBook);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"RECEIVED_CALLS\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"RECEIVED_CALLS\" not supported by this phone"));
	    }

	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelMeAdnPhoneBook);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"ME_PHONEBOOK\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"ME_PHONEBOOK\" not supported by this phone"));
	    }

	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelIccVoiceMailBox);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"VOICE_MAILBOX\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"VOICE_MAILBOX\" not supported by this phone"));
	    }

	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelIccAdnPhoneBook);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"SIM_PHONEBOOK\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"SIM_PHONEBOOK\" not supported by this phone"));
	    }

	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelCombinedAdnPhoneBook);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"COMBINED_SIM_ME\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"COMBINED_SIM_ME\" not supported by this phone"));
	    }

	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelIccFdnPhoneBook);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"FIXED_DIAL_NUM\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"FIXED_DIAL_NUM\" not supported by this phone"));
	    }

	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelIccSdnPhoneBook);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"SERV_DIAL_NUM\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"SERV_DIAL_NUM\" not supported by this phone"));
	    }

	iPhone.GetPhoneStoreInfo(aStatus, aPhoneStoreInfoPckg, KETelIccBdnPhoneBook);
	User::WaitForRequest(aStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"BARRED_DIAL_NUM\" returned %d")), aStatus.Int());
	if(aStatus.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		CheckPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("Phone Book \"BARRED_DIAL_NUM\" not supported by this phone"));
	    }
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestGetCapsAndInfo::CTestGetCapsAndInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("testGetCapsAndInfo"));
	}
//------------------------------------------------------------------

TVerdict CTestGetCapsAndInfo::doTestStepL( void )
/**
 * This function gets the SIM phone book's static capabilities (as well as the ME &
 * MT phone books, if supported), and information associated with the phone book.
 * It also validates the obtained information.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-003.04 Test retrieval of the phone book store info and capabilities of an ME-based phonebook of abbreviated dialling numbers.
 * @test GT83-TPHBK-003.06 Test retrieval of the phone book store info and capabilities of an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-003.11 Test retrieval of the phone book store info and capabilities of a combination of the ADN stored in ME and ICC
 * @test GT83-TPHBK-003.13 Test cancelling a request to retrieve phone book store info and capabilities
 */
	{
	// SIM phone book - mandatory
	TInt ret=KErrNone;
	TRequestStatus aStatus;
	RMobilePhoneBookStore simPhBk;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aInfoPckg(aInfo);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 *aInfob;

	ret=simPhBk.Open(iPhone, KETelIccAdnPhoneBook());
	if( KErrNone ==ret )
		{

		CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-001.06")) ;
		simPhBk.GetInfo(aStatus, aInfoPckg);
		User::WaitForRequest(aStatus);
		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.06")) ;
		aInfob = &aInfoPckg();

		//MKV  Print_TMobilePhoneBookInfoV1(aInfoPckg()) ;

		TESTL(aInfob->iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(!aInfob->iName.Compare(KETelIccAdnPhoneBook()));
		simPhBk.Close();

		}
	else
		{
		INFO_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		simPhBk.Close();
		}

	// ME phone book - optional support by the phones
	RMobilePhoneBookStore mePhBk;
	ret=mePhBk.Open(iPhone, KETelMeAdnPhoneBook());

	if (ret==KErrNone)
		{
		CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-001.06")) ;
		INFO_PRINTF1(_L("ME phonebook supported"));
		iMESupported=ETrue;

		mePhBk.GetInfo(aStatus, aInfoPckg);
		User::WaitForRequest(aStatus);
		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.04")) ;

		aInfob = &aInfoPckg();

		TESTL(aInfob->iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(!aInfob->iName.Compare(KETelMeAdnPhoneBook()));
		mePhBk.Close();
		CHECKPOINT(0, KErrNone, _L("GT83-TPHBK-002.06")) ;
		}
	else
		{
		INFO_PRINTF1(_L("Does SIM.TSY support KETelMeAdnPhoneBook?")) ;
		mePhBk.Close();
		}

	// Combined ME and SIM phone book - optional support by the phones
	if (iMTSupported)
		{
		RMobilePhoneBookStore combPhBk;
		ret=combPhBk.Open(iPhone,  KETelCombinedAdnPhoneBook());
		if (ret==KErrNone)
			{
			INFO_PRINTF1(_L("Combined ME & SIM phonebook supported"));
			iMTSupported=ETrue;

			combPhBk.GetInfo(aStatus, aInfoPckg);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.11")) ;

			aInfob = &aInfoPckg();

			TESTL(aInfob->iCaps && RMobilePhoneStore::KCapsWriteAccess);
			TESTL(!aInfob->iName.Compare(KETelCombinedAdnPhoneBook()));
			}
		else
			{
			INFO_PRINTF1(_L("Does SIM.TSY support KETelCombinedAdnPhoneBook?")) ;
			}
			combPhBk.Close();

		// Combined ME and SIM phone book
		if (iMTSupported)
			{
			RMobilePhoneBookStore combPhBk;
			TESTL(combPhBk.Open(iPhone, KETelCombinedAdnPhoneBook())==KErrNone);

			combPhBk.GetInfo(aStatus, aInfoPckg);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.11")) ;

			aInfob = &aInfoPckg();

			TESTL(aInfob->iMaxNumLength >0);
			TESTL(aInfob->iMaxTextLength >=0);
			TESTL(!aInfob->iName.Compare(KETelCombinedAdnPhoneBook()));
			TESTL(aInfob->iCaps & RMobilePhoneStore::KCapsWriteAccess);

	#ifdef __TEST_CANCEL__
			combPhBk.GetInfo(aStatus, aInfoPckg);
			combPhBk.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
			User::WaitForRequest(aStatus);
			INFO_PRINTF2(_L("Async MT PhoneBook GetInfo Cancel Status = %d"),aStatus.Int());
			TESTL(aStatus==KErrCancel || aStatus==KErrNone);
			TEST_CHECKL((aStatus==KErrCancel || aStatus==KErrNone), ETrue, _L("GT83-TPHBK-003.13"));
	#endif

			combPhBk.Close();
			}
		}
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestReadAndWrite::CTestReadAndWrite()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("testReadAndWrite"));
	}
//------------------------------------------------------------------
void CTestReadAndWrite::WriteToMEPhBkL()
/**
 * This function is called from the testReadAndWrite() function. Writes to the 1st 4
 * locations in the ME PhoneBook. Any existing entries in these locations will be overwritten.
 * @test GT83-TPHBK-001.04 Test opening a ME-based phonebook of abbreviated dialling numbers.
 * @test GT83-TPHBK-007.04 "Test writing a single phonebook entry to a specified location in a ME-based phonebook of abbreviated dialling numbers, where the location is empty"
 */
	{

	const TInt KEntries=4;

	RMobilePhoneBookStore mePhBk;
	TRequestStatus aStatus;
	CHECKPOINT(mePhBk.Open(iPhone,KETelMeAdnPhoneBook), KErrNone, _L("GT83-TPHBK-001.04"));

	INFO_PRINTF1(_L("Write to the ME PhoneBook..."));

	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data

	_LIT8(KTel,"123456%03d");
	_LIT8(KText,"TelNo %03d");
	TBuf8<13> text(KTel), number(KText);
	TBuf<13> showText;

	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	TInt index(0);
	TInt idx(1);

	while (index <= KEntries)
		{
		// First specify starting index and set iNumofEntries to 1, since it is only
		// possible to write one entry at a time

		//index = idx; // set index

		number.FillZ(); // Reset previous values
		number.Zero();
		text.FillZ();
		text.Zero();
		pbData.FillZ();
		pbData.Zero();

		// convert number into TLV format and append it to allocated buffer
		number.Format(KTel(), index);
		TInt ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, number);
		TESTL(ret == KErrNone) ;

		// convert number type into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);

		text.Format(KText(), index);
		showText.Copy(text) ;
		INFO_PRINTF2(_L("Writing %S"), &showText);

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, text);

		mePhBk.Write(aStatus, pbData, idx);
		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-007.04"));
		User::WaitForRequest(aStatus);
		index++;
		}

	mePhBk.Close();

	delete pbBuffer;
	}
//------------------------------------------------------------------

TVerdict CTestReadAndWrite::doTestStepL( void )
/**
 * This function creates an array of names and telephone numbers which are to be used as
 * phone book entries. It then writes these entries synchronously to the SIM phone book
 * and reads them back. If the Combined Phone book is supported, entries are first written
 * to the ME phonebook and then read from the Combined Phone book. This is because the MT
 * (SIM + ME) phonebook does not have write access.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.11 Test closing a combination of the ADN stored in ME and ICC
 * @test GT83-TPHBK-001.11 Test opening a combination of the ADN stored in ME and ICC
 * @test GT83-TPHBK-006.06 Test retrieval of the first entry in an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-006.11 Test retrieval of the first entry in a combination of the ADN stored in ME and ICC
 * @test GT83-TPHBK-007.06 "Test writing a single phonebook entry to a specified location in an ICC-based phonebook containing voice mailbox numbers, where the location is empty"
 * @test GT83-TPHBK-007.11 "Test writing a single phonebook entry to a specified location in an ICC-based phonebook containing the fixed dialling numbers list, when the phone is in a mode that allows editing the fixed dialling numbers, where the location is empty"
*/
	{
	TInt ret=KErrNone;
	TRequestStatus aStatus;
	RMobilePhoneBookStore* simPhBk = new(ELeave) RMobilePhoneBookStore();
	CleanupStack::PushL(simPhBk);

	// Create array of names and numbers
	const TText16* names[]={(TText16*)KAnnText.iBuf, (TText16*)KMattSText.iBuf, (TText16*)KAndyText.iBuf,
						  (TText16*)KMattMText.iBuf,(TText16*)KPaulText.iBuf, (TText16*)KMumAndDadText.iBuf,
						  (TText16*)KSymbolText.iBuf,(TText16*)KTextTooLong.iBuf};

	const TText16* numbers[]={(TText16*)KAnnPhoneNum.iBuf, (TText16*)KMattSPhoneNum.iBuf, (TText16*)KAndyPhoneNum.iBuf,
							(TText16*)KMattMPhoneNum.iBuf, (TText16*)KPaulPhoneNum.iBuf, (TText16*)KMumAndDadPhoneNum.iBuf,
							(TText16*)KSymbolPhoneNum.iBuf,(TText16*)KNumberTooLong.iBuf};

	TBuf<KBufSize> entryNumber(numbers[0]);
	TBuf<KBufSize> entryName(names[0]);

	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(pbBuffer);

	// Write the entries to the SIM Phone Book
	TBuf8<KPBDataClientBuf>* pbData = new(ELeave) TBuf8<KPBDataClientBuf>;	// Client reserved space for phonebook data
	CleanupStack::PushL(pbData);
	
	TBuf8<KPBDataClientBuf>* pbReadData1 = new(ELeave) TBuf8<KPBDataClientBuf>;	// Client reserved space for phonebook data
	CleanupStack::PushL(pbReadData1);

	TBuf8<KPBDataClientBuf>* pbReadData2 = new(ELeave) TBuf8<KPBDataClientBuf>;	// Client reserved space for phonebook data
	CleanupStack::PushL(pbReadData2);

	TBuf8<KPBDataClientBuf>* pbReadEmpty = new(ELeave) TBuf8<KPBDataClientBuf>;	// Client reserved space for phonebook data
	CleanupStack::PushL(pbReadEmpty);

	TInt requestedIndex(0), numOfEntries(1);

	// Read the entries back 
	TPtrC number(NULL,0);
	TPtrC text(NULL,0);
 
	TUint8 aTagValue(0);
	CPhoneBookBuffer::TPhBkTagType aDataType;
	TPtrC field(NULL,0);
	TUint i(0) ;
	TInt counter(0);
	TUint type;

	ret=simPhBk->Open(iPhone, KETelIccAdnPhoneBook());

	if( ret != KErrNone)
		{
		INFO_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		simPhBk->Close();
		}
	else
		{
		TUint  size(KPPBEntriesNum);
		TInt writeIndex(0);
		TUint index;

		CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-001.06")) ;

		pbBuffer->Set(pbData); // Set it to point to the Client buffer

		for (i=1; i<=size; ++i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.FillZ();
			entryNumber.Zero();
			entryName.FillZ();
			entryName.Zero();
			pbData->FillZ();
			pbData->Zero();

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// First specify starting index and set iNumofEntries to 1, since it is only
			// possible to write one entry at a time

			writeIndex = i; // Note that TSY will overwrite any existing data. It is up to
							// the Sync to check whether it is the first available entry

			// convert number into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, entryNumber);
			TESTL(ret == KErrNone) ;

			// convert type into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);	// AMC - "Unknown" numbers are written, so unknown numbers will be read in the tests below.
			TESTL(ret == KErrNone) ;

			// convert text into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, entryName);
			TESTL(ret == KErrNone) ;

			simPhBk->Write(aStatus, *pbData, writeIndex);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-007.06")); //???????
			}


		// Reads Forwads and it reads all entries individually 
		for(i=1; i<=size; ++i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.FillZ();
			entryNumber.Zero();
			entryName.FillZ();
			entryName.Zero();
			pbData->FillZ();
			pbData->Zero();

			// reset all loop variables
			index = 0;
			type = 0;
			counter = 0;
			aTagValue = 0;

			requestedIndex = i;
			simPhBk->Read(aStatus, requestedIndex, numOfEntries, *pbReadData1);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.06")) ;

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// now decode PB data received in TLV format
			pbBuffer->Set(pbReadData1); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data
			while(counter < pbReadData1->Length())
				{
				TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
				if(ret==KErrNotFound) // Reached the end of phonebook data
					break;
				TESTL(ret == KErrNone) ;
				switch(aDataType)
					{
				case CPhoneBookBuffer::EPhBkTypeInt8:
					{
					TUint8 int8(0);
					pbBuffer->GetValue(int8);
					counter = counter + 2; // add tag-field only in case that is beginning of new field
					if(aTagValue == RMobilePhoneBookStore::ETagPBTonNpi)
						{
						type = int8;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt16:
					{
					TUint16 int16(0);
					pbBuffer->GetValue(int16);
					counter = counter + 3; // 2-byte integer + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
						{
						index = int16;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt32:
					{
					TUint32 int32(0);
					pbBuffer->GetValue(int32);
					counter = counter + 5; // 4-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeDes16:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBText)
						{
						text.Set(field);
						}
					else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
						{
						number.Set(field);
						}
					}
					break;

				default:
					break;
					} // end switch aDataType

				} // end while


			TESTL(TInt(type)==RMobilePhone::EUnknownNumber);
			TESTL(number.Compare(entryNumber)==KErrNone);
			TESTL(text.Compare(entryName)==KErrNone);
			TESTL(index == i) ;
		} // end for


		// Try to read an empty entry
		requestedIndex = i+1; // Now set index to point to last entry in phonebook
		simPhBk->Read(aStatus, requestedIndex, numOfEntries, *pbReadEmpty);
		User::WaitForRequest(aStatus);
		CHECKPOINT(aStatus.Int(), KErrNotFound, _L("GT83-TPHBK-006.06")) ; // If asked to read an empty entry, TSY will not return an error
								 // It is now clients responsibility to ensure a valid index is supplied
		// Read Backwards, one by one
		for(i=size; i>0; --i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.FillZ();
			entryNumber.Zero();
			entryName.FillZ();
			entryName.Zero();
			pbData->FillZ();
			pbData->Zero();

			requestedIndex = i;
			simPhBk->Read(aStatus, requestedIndex, numOfEntries, *pbReadData2);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.06")) ;

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// reset all loop variables
			index = 0;
			type = 0;
			counter = 0;
			aTagValue = 0;

			// now decode PB data received in TLV format
			pbBuffer->Set(pbReadData2); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data
			while(counter < pbReadData2->Length())
				{
				TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
				if(ret==KErrNotFound) // Reached the end of phonebook data
					break;
				TESTL(ret == KErrNone) ;
				switch(aDataType)
					{
				case CPhoneBookBuffer::EPhBkTypeInt8:
					{
					TUint8 int8(0);
					counter = counter + 2; // add tag-field only in case that is beginning of new field
					pbBuffer->GetValue(int8); //
					if(aTagValue == RMobilePhoneBookStore::ETagPBTonNpi)
						{
						type = int8;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt16:
					{
					TUint16 int16(0);
					pbBuffer->GetValue(int16);
					counter = counter + 3; // 2-byte integer + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
						{
						index = int16;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt32:
					{
					TUint32 int32(0);
					pbBuffer->GetValue(int32);
					counter = counter + 5; // 4-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeDes16:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBText)
						{
						text.Set(field);
						}
					else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
						{
						number.Set(field);
						}
					}
					break;

				default:
					break;
					} // end switch aDataType

				} // end while


			TESTL(TInt(type)==RMobilePhone::EUnknownNumber);	// AMC - modified to match the Phonebook number "type" written at the start of this test program.
			TESTL(number.Compare(entryNumber)==KErrNone);
			TESTL(text.Compare(entryName)==KErrNone);
			TESTL(index == i) ;
			} // end for

		// Try to read below the phonebook limits
		requestedIndex = i-1;
		simPhBk->Read(aStatus, requestedIndex, numOfEntries, *pbReadEmpty);
		User::WaitForRequest(aStatus);
		INFO_PRINTF2(_L("DEBUG_LOG: aStatus == %d"), aStatus.Int());
  		//CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.06")) ; // If asked to read an empty entry, TSY will not return an error
		TEST((aStatus.Int()==KErrNone) || (aStatus.Int()==KErrArgument)); 
								 // It is now clients responsibility to ensure a valid index is supplied

		// Read by index, uses the Read() method
		for(i=1; i<=size; ++i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.FillZ();
			entryNumber.Zero();
			entryName.FillZ();
			entryName.Zero();
			pbData->FillZ();
			pbData->Zero();

			requestedIndex = i;
			simPhBk->Read(aStatus, requestedIndex, numOfEntries, *pbReadData1);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.06")) ;

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// reset all loop variables
			index = 0;
			type = 0;
			counter = 0;
			aTagValue = 0;

			// now decode PB data received in TLV format
			pbBuffer->Set(pbReadData1); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data
			while(counter < pbReadData1->Length())
				{
				TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
				if(ret==KErrNotFound) // Reached the end of phonebook data
					break;
				TESTL(ret == KErrNone) ;
				switch(aDataType)
					{
				case CPhoneBookBuffer::EPhBkTypeInt8:
					{
					TUint8 int8(0);
					counter = counter + 2; // add tag-field only in case that is beginning of new field
					pbBuffer->GetValue(int8); //
					if(aTagValue == RMobilePhoneBookStore::ETagPBTonNpi)
						{
						type = int8;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt16:
					{
					TUint16 int16(0);
					pbBuffer->GetValue(int16);
					counter = counter + 3; // 2-byte integer + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
						{
						index = int16;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt32:
					{
					TUint32 int32(0);
					pbBuffer->GetValue(int32);
					counter = counter + 5; // 4-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeDes16:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBText)
						{
						text.Set(field);
						}
					else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
						{
						number.Set(field);
						}
					}
					break;

				default:
					break;
					} // end switch aDataType

				} // end while


			TESTL(TInt(type)==RMobilePhone::EUnknownNumber);	// AMC - modified to match the Phonebook number "type" written at the start of this test program.
			TESTL(number.Compare(entryNumber)==KErrNone);
			TESTL(text.Compare(entryName)==KErrNone);
			TESTL(index == i) ;
			}

		simPhBk->Close();
		}
// From Combined phone book

	if (iMTSupported)
		{
		//
		// This part of the test reads the 1st few entries in the combined Phone Book (ME+
		// SIM). As the Combined Phonebook does not (should not) have 'WriteAccess', some
		// entries will first be written to the ME phone book. The SIM phone book
		// already contains the entries written above.
		//
		WriteToMEPhBkL(); //writing 5 entries to ME ph/book

		RMobilePhoneBookStore* combPhBk = new(ELeave) RMobilePhoneBookStore();
		CleanupStack::PushL(combPhBk);

		ret=combPhBk->Open(iPhone, KETelCombinedAdnPhoneBook);
		CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-001.11")) ;

		const TUint KEntries=4;		// Same value in WriteToMEPhBk()
		const TInt KBufSize=20;

		_LIT(KTel,"123456%03d");
		_LIT(KText,"TelNo %03d");
		TBuf<KBufSize> TelNum;
		TBuf<KBufSize> Text;

		// Read index locations 1 to KEntries
		TUint index=1;
		i = KEntries ;
		while (index<=KEntries)
			{
			requestedIndex = i;
			TelNum.Format(KTel,index);
			Text.Format(KText,index);

			combPhBk->Read(aStatus, requestedIndex, numOfEntries, *pbReadData2);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.11")) ;
			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// reset all loop variables
			index = 0;
			type = 0;
			counter = 0;
			aTagValue = 0;

			// now decode PB data received in TLV format
			pbBuffer->Set(pbReadData2); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data
			while(counter < pbReadData2->Length())
				{
				TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
				if(ret==KErrNotFound) // Reached the end of phonebook data
					break;
				TESTL(ret == KErrNone) ;
				switch(aDataType)
					{
				case CPhoneBookBuffer::EPhBkTypeInt8:
					{
					TUint8 int8(0);
					counter = counter + 1; // add tag-field only in case that is beginning of new field
					if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
						{
						pbBuffer->GetValue(int8); //
						counter = counter + 1; // 1-byte integer
						index = int8;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt16:
					{
					TUint16 int16(0);
					pbBuffer->GetValue(int16);
					counter = counter + 3; // 2-byte integer + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBTonNpi)
						{
						type = int16;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt32:
					{
					TUint32 int32(0);
					pbBuffer->GetValue(int32);
					counter = counter + 5; // 4-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeDes16:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBText)
						{
						text.Set(field);
						}
					else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
						{
						number.Set(field);
						}
					}
					break;

				default:
					break;
					} // end switch aDataType
			} // end while


			TESTL(TInt(type)==RMobilePhone::EUnknownNumber);	// AMC - modified to match the Phonebook number "type" written at the start of this test program.
			TESTL(number.Compare(TelNum)==KErrNone);
			TESTL(text.Compare(Text)==KErrNone);
			TESTL(index == i) ;
			index++;
			}  // end while

		// Entry to write - this should be read only...
		requestedIndex = 5;

		// convert number into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
		TESTL(ret == KErrNone) ;

		// convert number type into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);
		TESTL(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KAnnText);
		TESTL(ret == KErrNone) ;

		combPhBk->Write(aStatus, *pbData, requestedIndex);
		User::WaitForRequest(aStatus);
		if (aStatus!=KErrNotSupported)
			{
			INFO_PRINTF1(_L("A write has completed successfully to the MT PhoneBook."));
			INFO_PRINTF1(_L("** The MT PhoneBook should NOT have WriteAccess!! ** "));
			}
		else
			{
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-007.11")) ;
			}

		combPhBk->Close();
		CleanupStack::PopAndDestroy(combPhBk);
		}

	CleanupStack::PopAndDestroy(pbReadEmpty);
	CleanupStack::PopAndDestroy(pbReadData2);
	CleanupStack::PopAndDestroy(pbReadData1);
	CleanupStack::PopAndDestroy(pbData);
	CleanupStack::PopAndDestroy(pbBuffer);
	CleanupStack::PopAndDestroy(simPhBk);
	
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestDelete::CTestDelete()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("testDelete"));
	}
//------------------------------------------------------------------

TVerdict CTestDelete::doTestStepL( void )
/**
 * This function deletes the first 7 entries from the SIM phone book
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-006.06 Test retrieval of the first entry in an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-008.06 Test deleting an individual phonebook entry from an ICC-based phonebook of abbreviated dialling numbers
 */
	{
	TRequestStatus aStatus;
	RMobilePhoneBookStore simPhBk;
	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data
	TInt index(0), numOfEntries(0);

	TInt ret = simPhBk.Open(iPhone, KETelIccAdnPhoneBook) ;
	if(	ret ==KErrNone)
		{
		TInt i=0;
		TInt size=KPPBEntriesNum;

		for(i=1; i<=size; ++i)
			{
			simPhBk.Delete(aStatus, i);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-008.06")) ;
			}

		// now try to read first (empty) entry
		index = 1;
		simPhBk.Read(aStatus, index, numOfEntries, pbData);
		User::WaitForRequest(aStatus); //hs
		CHECKPOINT(aStatus.Int(), KErrNotFound, _L("GT83-TPHBK-006.06")) ; 
		// If asked to read an emptry entry, TSY will return KErrNotFound error
		// It is now client's responsibility to ensure a valid index is supplied
		}
	else
		{
		INFO_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		}

	simPhBk.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestNotifications::CTestNotifications()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("testNotifications"));
	}
//------------------------------------------------------------------

TVerdict CTestNotifications::doTestStepL( void )
/**
 * This function opens the SIM and MT (if supported) phonebooks and posts a request for
 * notification. Since this notification is not supported by any phonebook it should receive
 * KErrNotSupported.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-007.06 "Test writing a single phonebook entry to a specified location in an ICC-based phonebook containing voice mailbox numbers, where the location is empty"
 * @test GT83-TPHBK-008.06 Test deleting an individual phonebook entry from an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-008.16 Test cancelling deleting individual phonebook entries from a phonebook
 * @test GT83-TPHBK-007.17 Test writing a single phonebook entry to a combination of the ADN stored in ME and ICC
 * @test GT83-TPHBK-008.13 Test deleting an individual phonebook entry from a combination of the ADN stored in ME and ICC
*/
	{
	RMobilePhoneBookStore simPhBk, combPhBk;
	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data
	TInt index(0), ret(0);
	TRequestStatus aStatus, status1, status2;
	TInt index1 = 0;
	TInt index2 = 0;
	TUint32 eventToReturn;

	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read/write phonebook entries
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	ret = simPhBk.Open(iPhone, KETelIccAdnPhoneBook) ;

	if(ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		}
	else
		{

		simPhBk.NotifyStoreEvent(status1, eventToReturn,index1);	// Will Notify if an entry is added
		TESTL(status1.Int() == KRequestPending) ;

		if (iMTSupported && combPhBk.Open(iPhone, KETelCombinedAdnPhoneBook)==KErrNone)
			{
			TESTL(combPhBk.Open(iPhone, KETelCombinedAdnPhoneBook)==KErrNone);
			combPhBk.NotifyStoreEvent(status2, eventToReturn, index2); // Same here ;-)
			}

		index = 1; // write to the first slot
		// convert number into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
		TESTL(ret == KErrNone) ;

		// convert number type into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);
		TESTL(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KAnnText);
		TESTL(ret == KErrNone) ;

		simPhBk.Write(aStatus, pbData, index);
		User::WaitForRequest(aStatus); // Write request
		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-007.06")) ;
		User::WaitForRequest(status1); // Notification

		if (status1==KErrNotSupported)
			{
			// Notification is not supported by phonebook store
			INFO_PRINTF1(_L("Phonebook Store Write Notification is NOT Supported"));
			}
		else
			{
			// This should happen since this notification is supported by phonebook store
			INFO_PRINTF1(_L("Phonebook Store Write Notification IS Supported - Succeeded"));
			}

		if (iMTSupported)
			{

			combPhBk.Write(aStatus, pbData, index);
			User::WaitForRequest(aStatus); // Write request
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-007.17")) ;

			User::WaitForRequest(status2);

			if (status2==KErrNotSupported)
				{
				// Notification is not supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Write Notification is NOT Supported"));
				}
			else
				{
				// This should happen since this notification is not supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Write Notification IS Supported - Succeeded"));
				}
			}

		status1= KErrNone; // reset status variable
		simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);

		if (iMTSupported)
			{
			combPhBk.NotifyStoreEvent(status2, eventToReturn, index2);
			}

		// Now delete that entry
		simPhBk.Delete(aStatus, 1);
		User::WaitForRequest(aStatus);
		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-008.06")) ;

		User::WaitForRequest(status1);
		if (status1==KErrNotSupported)
			{
			// Notification is not supported by phonebook store
			INFO_PRINTF1(_L("Phonebook Store Delete Notification is NOT Supported"));
			}
		else
			{
			// This should not happen since this notification is not supported by phonebook store
			INFO_PRINTF1(_L("Phonebook Store Delete Notification IS Supported - Succeeded"));
			}

		if (iMTSupported)
			{
			// Now delete that entry
			combPhBk.Delete(aStatus, 1);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-008.013")) ;

			User::WaitForRequest(status2);
			if (status2==KErrNotSupported)
				{
				// Notification is not supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Delete Notification is NOT Supported"));
				}
			else
				{
				// This should happen since this notification is supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Delete Notification IS Supported - Succeeded"));
				}
			}

		// EntryChangedNotificationCancel Request
		status1= KErrNone; // reset status variable
		simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);
		simPhBk.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
		User::WaitForRequest(status1);
		CHECKPOINT(status1.Int(), KErrCancel, _L("GT83-TPHBK-008.16")) ;

		// Cancel Notification Request after 2 seconds
		status1= KErrNone; // reset status variable
		simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);
		User::After(2000000L);
		simPhBk.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
		User::WaitForRequest(status1);

		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Cancel Status = %d")), status1.Int());
		TESTL(status1==KErrCancel);

		// Now write the same entry again
		simPhBk.Write(aStatus, pbData, index);
		User::WaitForRequest(aStatus);
		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-0")) ;

		simPhBk.Delete(aStatus, 1); // Deleting the last added entry so that the test ends "clean".
		User::WaitForRequest(aStatus);
		if (iMTSupported)
			{
			combPhBk.Close();
			}
		}

	simPhBk.Close();

	delete pbBuffer;
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestInvalidOps::CTestInvalidOps()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("testInvalidOps"));
	}
//------------------------------------------------------------------

TVerdict CTestInvalidOps::doTestStepL( void )
/**
 * Stress test. Tries to perform invalid operations on the phonebook to make sure the
 * phone errors these operations accordingly.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-003.06 Test retrieval of the phone book store info and capabilities of an ICC-based phonebook of abbreviated dialling numbers
 */
	{
	RMobilePhoneBookStore simPhBk;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1	aInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1	*bInfo;
	TRequestStatus aStatus;

	TInt ret = simPhBk.Open(iPhone, KETelIccAdnPhoneBook) ;
	if(ret != KErrNone)
		{
		ERR_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		simPhBk.Close() ;
		return EFail;
		}

	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data
	TInt maxNumber, index(0), numEntries(1);

	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read/write phonebook entries
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	// Read and write past the end of phone book - First ask the phone what is the maximum number of
	// entries, then tries to write past the max index!

	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aInfoPckg(aInfo);
	simPhBk.GetInfo(aStatus, aInfoPckg);
	User::WaitForRequest(aStatus);
	CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.06")) ;
	bInfo = &aInfoPckg();

	maxNumber = bInfo->iTotalEntries;

	// Write an entry beyond the returned phonebook limit
	index = maxNumber + 1;

	// convert number into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
	TESTL(ret == KErrNone) ;

	// convert number type into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);
	TESTL(ret == KErrNone) ;

	// convert text into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KAnnText);
	TESTL(ret == KErrNone) ;

	simPhBk.Write(aStatus, pbData, index);
	User::WaitForRequest(aStatus); // Write request
	TESTL(aStatus == KErrArgument) ;  // Should not accept Phone book entry maxNumber + 1
							// Anuli (12/12/01) changed from KErrGeneral to KErrArgument
	pbData.FillZ(); // Clear the buffer
	pbData.Zero();

	// Read that entry
	index=maxNumber + 1;
	simPhBk.Read(aStatus, index, numEntries, pbData);
	User::WaitForRequest(aStatus);
	TESTL(aStatus==KErrArgument); // There should be no entry written

	// Delete an entry beyond the returned phonebook limit
	simPhBk.Delete(aStatus, maxNumber + 1000L); // Testing to delete an impossible indexed entry
	User::WaitForRequest(aStatus);
	TESTL(aStatus == KErrArgument) ;

	// Write text field too long - 62 character is more than 60 defined in config file
	index = 1;

	// convert number into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
	TESTL(ret == KErrNone) ;

	// convert number type into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);
	TESTL(ret == KErrNone) ;

	// convert text into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KTextTooLong);
	TESTL(ret == KErrNone) ;

	simPhBk.Write(aStatus, pbData, index);
	User::WaitForRequest(aStatus); // Write request
	if (aStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Phone book does not error writing a too long name tag!"));
		simPhBk.Delete(aStatus, index);
		User::WaitForRequest(aStatus);
		TESTL(aStatus == KErrNone) ;
		}
	else
		{
		TESTL(aStatus == KErrOverflow) ;
		}

	// Write number field too long - assume 80 characters is more than 50 defined in config file
	index = 1;
	// convert number into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KNumberTooLong);
	TESTL(ret == KErrNone) ;

	// convert number type into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);
	TESTL(ret == KErrNone) ;

	// convert text into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KAnnText);
	TESTL(ret == KErrNone) ;

	simPhBk.Write(aStatus, pbData, index);
	User::WaitForRequest(aStatus); // Write request
	if (aStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Phone book does not error writing a too long number field!"));
		simPhBk.Delete(aStatus, index);
		User::WaitForRequest(aStatus);
		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-0")) ;
		}
	else
		{
		CHECKPOINT(aStatus.Int(), KErrOverflow, _L("GT83-TPHBK-0")) ;
		}


	// Write International TON! Some phones do not support this.
	index = 1;

	// convert number into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
	CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-0")) ;

	// convert number type into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EInternationalNumber);
	CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-0")) ;

	// convert text into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KAnnText);
	CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-0")) ;

	simPhBk.Write(aStatus, pbData, index);
	User::WaitForRequest(aStatus); // Write request
	if (aStatus==KErrNone)
		{
		index=maxNumber + 1;
		simPhBk.Read(aStatus, index, numEntries, pbData);
		User::WaitForRequest(aStatus);
		INFO_PRINTF1(_L("Phone book does not error write with International TOA "));
		simPhBk.Delete(aStatus, 1);
		User::WaitForRequest(aStatus);
		}
	else
		CHECKPOINT(aStatus.Int(), KErrGeneral, _L("GT83-TPHBK-0")) ;

	User::After(5000000L);

	simPhBk.Close();
	delete pbBuffer;
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestAdvancedPhBk::CTestAdvancedPhBk()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("testAdvancedPhBk"));
	}
//------------------------------------------------------------------

TVerdict CTestAdvancedPhBk::doTestStepL( void )
/**
 * This function opens an Advanced Gsm SIM PhoneBook, then proceeds to write and then
 * delete 'KFillTo' entries from the phone book.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-003.06 Test retrieval of the phone book store info and capabilities of an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-007.06 "Test writing a single phonebook entry to a specified location in an ICC-based phonebook containing voice mailbox numbers, where the location is empty"
 * @test GT83-TPHBK-008.06 Test deleting an individual phonebook entry from an ICC-based phonebook of abbreviated dialling numbers
 */
	{
	const TInt KFillTo=30;

	TInt ret=KErrNone;
	TRequestStatus aStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aPhoneStoreInfob;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aPhoneStoreInfoPckgb(aPhoneStoreInfob);
	RMobilePhoneBookStore simPhBk;

	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data

	_LIT(KTel,"1632960000%02d");
	_LIT(KText,"Tel %02d");
	TBuf<15> text(KTel), number(KText);

	TInt index(1);

	ret=simPhBk.Open(iPhone, KETelIccAdnPhoneBook());
	if( ret == 	KErrNone)
		{
		CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-001.06")) ;

		simPhBk.DeleteAll(aStatus);
		User::WaitForRequest(aStatus);

		// Write the entries
		INFO_PRINTF1(_L("Fill the 1st 30 locations in the SIM phonebook..."));

		simPhBk.GetInfo(aStatus, aPhoneStoreInfoPckgb);
		User::WaitForRequest(aStatus);

		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.06")) ;
		aPhoneStoreInfob = aPhoneStoreInfoPckgb();
		TInt used=aPhoneStoreInfob.iUsedEntries ;

		TESTL(used==0);

		CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
		pbBuffer->Set(&pbData); // Set it to point to the Client buffer

		// Fields supported by this phonebook are returned to client as capabilities
		// in aPhoneStoreInfoPckgb. It is assumed that all GSM phonebook fields (index, tel num,
		// type and name) are supported even though that part of the code has to be tested as well.

		while (used<KFillTo)
			{
			// First specify starting index and set iNumofEntries to 1, since it is only
			// possible to write one entry at a time

			number.FillZ(); // Reset previous values
			number.Zero();
			text.FillZ();
			text.Zero();
			pbData.FillZ();
			pbData.Zero();

			// convert number into TLV format and append it to allocated buffer
			number.Format(KTel(), index);
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, number);
			TESTL(ret == KErrNone) ;

			// convert number type into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);

			text.Format(KText(), index);
			INFO_PRINTF2(_L("Writing \'%S\'"),&text);
			// convert text into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, text);

			simPhBk.Write(aStatus, pbData, index);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-007.06"));
			used++;
			index++;
			} // end while

		delete pbBuffer;
		}
	else
		{
		INFO_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		}
	simPhBk.Close();

	// Delete the entries //
	INFO_PRINTF1(_L("Re-opening phonebook..."));
	ret=simPhBk.Open(iPhone, KETelIccAdnPhoneBook());
	if( ret == KErrNone)
		{
		CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-001.06")) ;
		simPhBk.GetInfo(aStatus, aPhoneStoreInfoPckgb);
		User::WaitForRequest(aStatus);
		CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.06")) ;
		aPhoneStoreInfob = aPhoneStoreInfoPckgb();
		INFO_PRINTF1(_L("Deleting the entries...")); 
		for (int iy=1;iy<=KFillTo;++iy)
			{
			INFO_PRINTF2(_L("Deleting %03d"),iy);
			simPhBk.Delete(aStatus, iy);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-008.06"));
			}
		}
	else
		{
		INFO_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		}
	simPhBk.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CRetrievalOfWholePhoneBook::CRetrievalOfWholePhoneBook()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("RetrievalOfWholePhoneBook"));
	}
//------------------------------------------------------------------

TVerdict CRetrievalOfWholePhoneBook::doTestStepL( void )
/**
 * Detected Network test. This function gets the number of available networks
 * and information on these networks. Also tests some Cancel Requests.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-003.06 Test retrieval of the phone book store info and capabilities of an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-004.06 "Obtain total number of slots and number of slots used, using GetInfo method. Test  retrieval of the whole list of phone book entries from an ICC-based phonebook of abbreviated dialling numbers"
 */
	{
	INFO_PRINTF1(_L("==============================="));
	INFO_PRINTF1(_L(" Test retrieval of a Phonebook")); 
	INFO_PRINTF1(_L("==============================="));
	INFO_PRINTF1(_L("This test can take up to a minute to complete..."));

	RMobilePhoneBookStore simPhBk;
	TInt ret = simPhBk.Open(iPhone, KETelIccAdnPhoneBook) ;
	if(ret != KErrNone)
		{
		ERR_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		simPhBk.Close() ;
		return EFail;
		}

	// First write 15 entries to the phone
	INFO_PRINTF1(_L("Writing 15 entries to the phone"));
	WriteBatchOfEntriesToThePhoneL();

	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read phonebook data
	TBuf8<KPBDataClientBuf> pbReadData1; // Client reserved space for phonebook data
	TRequestStatus aStatus;

	TUint index(1);
	TPtrC8 number(NULL,0);
	TPtrC8 text(NULL,0);

	TUint8 aTagValue(0);
	CPhoneBookBuffer::TPhBkTagType aDataType;
	TPtrC8 field(NULL,0);
	TInt counter(0);


	// Get total number of phonebook entires
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aInfoPckg(aInfo);
	simPhBk.GetInfo(aStatus, aInfoPckg);
	User::WaitForRequest(aStatus);
	CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.06")) ;

	INFO_PRINTF2(_L("Start retrieving %d phonebook entries"), aInfo.iTotalEntries);

	TInt numOfEntries(aInfo.iTotalEntries-1);
	simPhBk.Read(aStatus, 1, numOfEntries, pbReadData1);
	User::WaitForRequest(aStatus);
	CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-004.06")) ;

	// pbReadData1 will contain a batch of retrieved entries, so start by parsing through its
	// data to retrieve individual entries
	pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
	pbBuffer->StartRead();       // Start reading received phonebook data
	while(counter < pbReadData1.Length())
		{
		TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
		if(ret==KErrNotFound) // Reached the end of phonebook data
			break;
		TESTL(ret == KErrNone) ;
		switch(aDataType)
			{
			case CPhoneBookBuffer::EPhBkTypeInt8:
				{
				TUint8 int8(0);
				counter = counter + 1; // add tag-field only in case that is beginning of new field
				if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
					{
					pbBuffer->GetValue(int8); //
					counter = counter + 1; // 1-byte integer
					index = int8;
					}
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeInt16:
				{
				TUint16 int16(0);
				pbBuffer->GetValue(int16);
				counter = counter + 3; // 2-byte integer + tag-field
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeInt32:
				{
				TUint32 int32(0);
				pbBuffer->GetValue(int32);
				counter = counter + 5; // 4-byte integer + tag-field
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeDes16:
				{
				pbBuffer->GetValue(field);
				counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
				if(aTagValue == RMobilePhoneBookStore::ETagPBText)
					{
					text.Set(field);
					}
				else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
					{
					number.Set(field);
					}
				}
				break;

			default:
				break;
			} // end switch aDataType

		// Print each individual entry
		if((aTagValue == RMobilePhoneBookStore::ETagPBNewEntry) && (counter>1)) // Only print if all fields retrieved
			{
			INFO_PRINTF4(_L("%d = >%S<  >%S<"), index, &text, &number); // It is not displaying data corectly on the console
			}
		} // end while

	simPhBk.Close();
	delete pbBuffer;
	INFO_PRINTF1(_L("Deleting the entries"));
	ClearPhoneBookL(KETelIccAdnPhoneBook());
	return TestStepResult();
	}
//------------------------------------------------------------------

CRetrievalOfBatchOfPhoneBookEntries::CRetrievalOfBatchOfPhoneBookEntries()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("RetrievalOfBatchOfPhoneBookEntries"));
	}
//------------------------------------------------------------------

TVerdict CRetrievalOfBatchOfPhoneBookEntries::doTestStepL( void )
/**
 * Test of retrieval of a batch of phonebook entries. This method is mainly used to test
 * removal of partially populated phonebook entries by the MMTSY.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-005.06 Test retieval of half of the entries from an ICC-based phonebook of abbreviated dialling numbers
 */
	{
	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L(" Test retrieval of a Batch of Phonebook Entries")); 
	INFO_PRINTF1(_L("================================================"));

	RMobilePhoneBookStore simPhBk;
	TInt ret = simPhBk.Open(iPhone, KETelIccAdnPhoneBook) ;
	if(ret != KErrNone)
		{
		ERR_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		simPhBk.Close() ;
		return EFail;
		}

	// First write 15 entries to the phone
	INFO_PRINTF1(_L("Writing 15 entries to the phone"));
	WriteBatchOfEntriesToThePhoneL();

	TRequestStatus aStatus;
	TBuf8<KPBDataClientSmallBuf> pbReadData1; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read phonebook data

	INFO_PRINTF1(_L("This test can take up to a minute to complete..."));
	TInt startIndex = 3; // Where to start in the phonebook
	TInt batchSize = 4; // How many entries to get.

	INFO_PRINTF3(_L("Start retrieving a batch of %d phonebook entries with starting index %d"), batchSize, startIndex);

	simPhBk.Read(aStatus, startIndex, batchSize, pbReadData1);
	User::WaitForRequest(aStatus);
	CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-005.06")) ;

	TUint index(0);
	TUint temp(0);
	TPtrC8 number(NULL,0);
	TPtrC8 text(NULL,0);

	TUint8 aTagValue(0);
	CPhoneBookBuffer::TPhBkTagType aDataType;
	TPtrC8 field(NULL,0);
	TInt counter(0);

	// pbReadData1 will contain a batch of retrieved entries, so start by parsing through its
	// data to retrieve/decode individual entries
	pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
	pbBuffer->StartRead();       // Start reading received phonebook data
	while(counter < pbReadData1.Length())
		{
		TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
		if(ret==KErrNotFound) // Reached the end of phonebook data
			break;
		TESTL(ret == KErrNone) ;
		switch(aDataType)
			{
			case CPhoneBookBuffer::EPhBkTypeInt8:
				{
				TUint8 int8(0);
				counter = counter + 1; // add tag-field only in case that is beginning of new field
				if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
					{
					pbBuffer->GetValue(int8); //
					counter = counter + 1; // 1-byte integer
					index = int8;
					}
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeInt16:
				{
				TUint16 int16(0);
				pbBuffer->GetValue(int16);
				counter = counter + 3; // 2-byte integer + tag-field
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeInt32:
				{
				TUint32 int32(0);
				pbBuffer->GetValue(int32);
				counter = counter + 5; // 4-byte integer + tag-field
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeDes16:
				{
				pbBuffer->GetValue(field);
				counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
				if(aTagValue == RMobilePhoneBookStore::ETagPBText)
					{
					text.Set(field);
					}
				else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
					{
					number.Set(field);
					}
				}
				break;

			default:
				break;
			} // end switch aDataType

		// Print each individual entry
		if((aTagValue == RMobilePhoneBookStore::ETagPBNewEntry) && (counter>1)) // Only print if all fields retrieved
			{
			temp++;
			INFO_PRINTF4(_L("%d = >%S<  >%S<"), index, &text, &number); // It is not displaying data corectly on the console
			}
		} // end while

	simPhBk.Close();
	delete pbBuffer;

	INFO_PRINTF1(_L("Deleting the entries"));
	ClearPhoneBookL(KETelIccAdnPhoneBook());
	return TestStepResult();
	}
//------------------------------------------------------------------

CRetrievalOfHalfOfPhoneBookEntries::CRetrievalOfHalfOfPhoneBookEntries()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("RetrievalOfHalfOfPhoneBookEntries"));
	}
//------------------------------------------------------------------

TVerdict CRetrievalOfHalfOfPhoneBookEntries::doTestStepL( void )
/**
 * Test of retrieval of a half of phonebook entries. This method is mainly used to test
 * removal of partially populated phonebook entries by the MMTSY.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-003.06 Test retrieval of the phone book store info and capabilities of an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-005.06 Test retieval of half of the entries from an ICC-based phonebook of abbreviated dialling numbers
 */
	{
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aPhoneStoreInfob;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aPhoneStoreInfoPckgb(aPhoneStoreInfob);

	INFO_PRINTF1(_L("================================================"));
	INFO_PRINTF1(_L(" Test retrieval of a Batch of Phonebook Entries")); 
	INFO_PRINTF1(_L("================================================"));

	RMobilePhoneBookStore simPhBk;
	TInt ret = simPhBk.Open(iPhone,KETelIccAdnPhoneBook) ;
	if(ret != KErrNone)
		{
		ERR_PRINTF1(_L("Does SIM.TSY support KETelIccAdnPhoneBook?")) ;
		simPhBk.Close() ;
		return EFail;
		}

	// First write 15 entries to the phone
	INFO_PRINTF1(_L("Writing 15 entries to the phone"));
	WriteBatchOfEntriesToThePhoneL();

	TRequestStatus aStatus;
	TBuf8<KPBDataClientBuf> pbReadData1; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read phonebook data

	INFO_PRINTF1(_L("This test can take up to a minute to complete..."));
	TInt startIndex = 3; // Where to start in the phonebook
	TInt batchSize = 4; // How many entries to get.

	simPhBk.GetInfo(aStatus, aPhoneStoreInfoPckgb);
	User::WaitForRequest(aStatus);
	CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.06")) ;

	aPhoneStoreInfob = aPhoneStoreInfoPckgb() ;
	batchSize = (aPhoneStoreInfob.iTotalEntries)/2; //50%

	INFO_PRINTF3(_L("Start retrieving a batch of %d phonebook entries with starting index %d"), batchSize, startIndex);

	simPhBk.Read(aStatus, startIndex, batchSize, pbReadData1);
	User::WaitForRequest(aStatus);
	CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-005.06")) ;

	TUint index(0);
	TUint temp(0);
	TPtrC8 number(NULL,0);
	TPtrC8 text(NULL,0);

	TUint8 aTagValue(0);
	CPhoneBookBuffer::TPhBkTagType aDataType;
	TPtrC8 field(NULL,0);
	TInt counter(0);

	// pbReadData1 will contain a batch of retrieved entries, so start by parsing through its
	// data to retrieve/decode individual entries
	pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
	pbBuffer->StartRead();       // Start reading received phonebook data
	while(counter < pbReadData1.Length())
		{
		TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
		if(ret==KErrNotFound) // Reached the end of phonebook data
			break;
		TESTL(ret == KErrNone) ;
		switch(aDataType)
			{
			case CPhoneBookBuffer::EPhBkTypeInt8:
				{
				TUint8 int8(0);
				counter = counter + 1; // add tag-field only in case that is beginning of new field
				if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
					{
					pbBuffer->GetValue(int8); //
					counter = counter + 1; // 1-byte integer
					index = int8;
					}
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeInt16:
				{
				TUint16 int16(0);
				pbBuffer->GetValue(int16);
				counter = counter + 3; // 2-byte integer + tag-field
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeInt32:
				{
				TUint32 int32(0);
				pbBuffer->GetValue(int32);
				counter = counter + 5; // 4-byte integer + tag-field
				}
				break;

			case CPhoneBookBuffer::EPhBkTypeDes16:
				{
				pbBuffer->GetValue(field);
				counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
				if(aTagValue == RMobilePhoneBookStore::ETagPBText)
					{
					text.Set(field);
					}
				else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
					{
					number.Set(field);
					}
				}
				break;

			default:
				break;
			} // end switch aDataType

		// Print each individual entry
		if((aTagValue == RMobilePhoneBookStore::ETagPBNewEntry) && (counter>1)) // Only print if all fields retrieved
			{
			temp++;
			INFO_PRINTF4(_L("%d = >%S<  >%S<"), index, &text, &number); // It is not displaying data corectly on the console
			}
		} // end while

	simPhBk.Close();
	delete pbBuffer;

	INFO_PRINTF1(_L("Deleting the entries"));
	ClearPhoneBookL(KETelIccAdnPhoneBook());
	return TestStepResult();
	}

//------------------------------------------------------------------

CTestAllPhBooks::CTestAllPhBooks()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestAllPhBooks"));
	}
//------------------------------------------------------------------

TVerdict CTestAllPhBooks::doTestStepL( void )
	{
//  Added during port to test execute
//  Not part of original test
//  This test step seems to rely on state from previous steps which it should not
//  Should be reviewed!!!!!!
//  Contractor - December 2005
//	vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	CSimPhBkTestStepBase::ClearPhoneBookL(KETelIccAdnPhoneBook());
//	^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	//I'm very sorry for such code,
	//but I have troubles with CW, thus when I
	//have my personal copy I'll sort it out better way.

	iPhBkNames[0].iPhBkName = KETelMeDialledPhoneBook ;
	iPhBkNames[0].iPhBkID =	1 ;	//"ME_DIALLEDPHONEBOOK");

	iPhBkNames[1].iPhBkName = KETelMeMissedPhoneBook ;
	iPhBkNames[1].iPhBkID =	2 ;		//"ME_MISSEDPHONEBOOK");

	iPhBkNames[2].iPhBkName = KETelMeReceivedPhoneBook ;
	iPhBkNames[2].iPhBkID =	3 ;//"ME_RECEIVEDPHONEBOOK");

	iPhBkNames[3].iPhBkName = KETelMeAdnPhoneBook;
	iPhBkNames[3].iPhBkID =	4 ;		//"ME_ADNPHONEBOOK");

	iPhBkNames[4].iPhBkName = KETelIccVoiceMailBox;
	iPhBkNames[4].iPhBkID =	5 ;	//"ICC_VOICEMAILBOX");

	iPhBkNames[5].iPhBkName = KETelIccAdnPhoneBook;
	iPhBkNames[5].iPhBkID =	6 ;	//"ICC_ADNPHONEBOOK");

	iPhBkNames[6].iPhBkName = KETelIccFdnPhoneBook;
	iPhBkNames[6].iPhBkID =	7 ;	//"ICC_FDNPHONEBOOK");

	iPhBkNames[7].iPhBkName = KETelIccSdnPhoneBook;
	iPhBkNames[7].iPhBkID =	8 ;	//"ICC_SDNPHONEBOOK");

	iPhBkNames[8].iPhBkName = KETelIccBdnPhoneBook;
	iPhBkNames[8].iPhBkID =	10 ;	//"ICC_BDNPHONEBOOK");

	iPhBkNames[9].iPhBkName = KETelCombinedAdnPhoneBook;
	iPhBkNames[9].iPhBkID =	11 ;	//"COMB_ADNPHONEBOOK");

	iPhBkNames[10].iPhBkName = KETelTaAdnPhoneBook;
	iPhBkNames[10].iPhBkID =	12 ;		//"TA_ADNPHONEBOOK");
	TBuf<32> BookName;
		 
		for(TUint index = 0;  index<(sizeof(iPhBkNames)/sizeof(TPhBkNameAndID)); index++)
			{
			BookName.Copy(iPhBkNames[index].iPhBkName);
			INFO_PRINTF1(BookName);
			//if(IsPhBookSupportedByPhone(iPhone,  BookName)) //Is phone book supported by phone?
				{
				INFO_PRINTF1(_L("RW..."));
				TestReadAndWritePhBkL(BookName, iPhBkNames[index].iPhBkID) ;
				INFO_PRINTF1(_L("Delete..."));
				ClearPhoneBookL(BookName, iPhBkNames[index].iPhBkID) ;
				INFO_PRINTF1(_L("Get info..."));
				GetPhoneBookInfoL(BookName, iPhBkNames[index].iPhBkID) ;
				}
			} ;
	return TestStepResult();
	}

//------------------------------------------------------------------

TPtrC CTestAllPhBooks::MakeCheckPointName(TInt aMajorNum, TInt aMinorNum )
/** Make test id 
 * @param text buf > 20 symbols
 * @param major number
 * @param minor number
*/
	{
	TBuf<32> theBuf ;
	//clear old string
	theBuf.Copy(_L("GT83-TPHBK-"));
	theBuf.AppendFormat(_L("%03d.%03d"), aMajorNum, aMinorNum) ;
	return theBuf.Ptr() ;
	}

//------------------------------------------------------------------

void CTestAllPhBooks::Print_TMobilePhoneBookInfoV1(RMobilePhoneBookStore::TMobilePhoneBookInfoV1& aArg) 
/** Display value of RMobilePhoneBookStore::TMobilePhoneBookInfoV1
 * @param value to print
*/
	{
		INFO_PRINTF1(_L("\tTMobilePhoneBookInfoV1:")) ;
		INFO_PRINTF2(_L("iMaxNumLength = %d"), aArg.iMaxNumLength) ;
		INFO_PRINTF2( _L("iMaxTextLength = %d"), aArg.iMaxTextLength) ;
		Print_TMobilePhoneBookLocation(aArg.iLocation) ;
		INFO_PRINTF2(_L("iChangeCounter = %d"), aArg.iChangeCounter) ;
		INFO_PRINTF2(_L("iIdentity = %S"), &aArg.iIdentity) ; 
		Print_TMobilePhoneBookCaps(aArg.iCaps) ;

	}

//------------------------------------------------------------------

void CTestAllPhBooks::Print_TMobilePhoneBookLocation(RMobilePhoneBookStore::TMobilePhoneBookLocation& aArg)
/** Display value of enum RMobilePhoneBookStore::TMobilePhoneBookLocation to screen.
 * @param value to print
*/
	{
	switch(aArg)
		{
		case RMobilePhoneBookStore::ELocationUnknown:
			INFO_PRINTF1(_L("Value of enum is RMobilePhoneBookStore::ELocationUnknown")) ;
		break ;
		case RMobilePhoneBookStore::ELocationIccMemory:
			INFO_PRINTF1(_L("Value of enum is RMobilePhoneBookStore::ELocationIccMemory")) ;
		break ;
		case RMobilePhoneBookStore::ELocationPhoneMemory:
			INFO_PRINTF1(_L("Value of enum is RMobilePhoneBookStore::ELocationPhoneMemory")) ;
		break ;
		case RMobilePhoneBookStore::ELocationExternalMemory:
			INFO_PRINTF1(_L("Value of enum is RMobilePhoneBookStore::ELocationExternalMemory")) ;
		break ;
		case RMobilePhoneBookStore::ELocationCombinedMemory:
			INFO_PRINTF1(_L("Value of enum is RMobilePhoneBookStore::ELocationCombinedMemory")) ;
		break ;
		}
	}

//------------------------------------------------------------------

void CTestAllPhBooks::Print_TMobilePhoneBookCaps(TUint32 aArg)
/** Display value of enum RMobilePhoneBookStore::TMobilePhoneBookCaps to screen.
 * @param value to print
*/
	{
	if(aArg & RMobilePhoneBookStore::KCapsRestrictedWriteAccess)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneBookCaps::KCapsRestrictedWriteAccess")) ;
		}
	if(aArg & RMobilePhoneBookStore::KCapsSecondNameUsed       )
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneBookCaps::KCapsSecondNameUsed       ")) ;
		}
	if(aArg & RMobilePhoneBookStore::KCapsAdditionalNumUsed    )
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneBookCaps::KCapsAdditionalNumUsed    ")) ;
		}
	if(aArg & RMobilePhoneBookStore::KCapsGroupingUsed	)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneBookCaps::KCapsGroupingUsed	")) ;
		}
	if(aArg & RMobilePhoneBookStore::KCapsEntryControlUsed   )
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneBookCaps::KCapsEntryControlUsed   ")) ;
		}
	if(aArg & RMobilePhoneBookStore::KCapsEmailAddressUsed   )
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneBookCaps::KCapsEmailAddressUsed   ")) ;
		}
	if(aArg & RMobilePhoneBookStore::KCapsBearerCapUsed	)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneBookCaps::KCapsBearerCapUsed	")) ;
		}
	if(aArg & RMobilePhoneBookStore::KCapsSynchronisationUsed)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneBookCaps::KCapsSynchronisationUsed")) ;
		}
	}

//------------------------------------------------------------------

TInt CTestAllPhBooks::GetPhoneBookInfoL(const TDesC& aPhoneBookName, TInt aMinorNum)
/**
 * This function gets phone book info and prints it.
 * @param ph/book name
 * @param test ID minor number
 * @return error code
 * @test GT83-TPHBK-001.00 Test opening a phone book (Generic)
 * @test GT83-TPHBK-002.00 Test closing a phone book (Generic)
 * @test GT83-TPHBK-003.00 Test retrieval of the phone book store info and capabilities (generic)
 */
	{
	TRequestStatus aStatus;
	RMobilePhoneBookStore thePhBk;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aInfoPckg(aInfo);
	TInt ret=thePhBk.Open(iPhone, aPhoneBookName);
	//TEST_CHECKL(ret, KErrNone, _L("GT83-TPHBK-001.00")) ;
	if(ret == KErrNotSupported)
		{
		INFO_PRINTF2(_L("%S phone book isn't supported!"), &aPhoneBookName) ;
		thePhBk.Close() ;
		return ret;
		}
	else if(ret != KErrNone )
		{	//It doesn have any sence to continue.
		INFO_PRINTF2(_L("I can't open %S phone book!"), &aPhoneBookName) ;
		//TEST_CHECKL(ret, KErrNone, MakeCheckPointName(1, aMinorNum)) ;
		thePhBk.Close() ;
		return ret;
		}

	thePhBk.GetInfo(aStatus, aInfoPckg);
	User::WaitForRequest(aStatus);
	//TEST_CHECKL(ret, KErrNone, _L("GT83-TPHBK-003.00")) ;
	CHECKPOINT(aStatus.Int(), KErrNone, MakeCheckPointName(3, aMinorNum)) ;
	aInfo = aInfoPckg() ;
	if(aStatus == KErrNone)
		{ //Does it have any sence to print?
		Print_TMobilePhoneBookInfoV1(aInfo) ;
		}
	thePhBk.Close() ;
	//TEST_CHECKL(ret, KErrNone, _L("GT83-TPHBK-002.00")) ;
	return ret;
}

//---------------------------------------------------------------------------

TInt CTestAllPhBooks::ClearPhoneBookL(const TDesC& aPhoneBookName, TInt aMinorNum)
/**
 * This function deletes the first 7 entries in the specified Phone Book.
 * @param ph/book name
 * @param test ID minor number
 * @return error code
 * @test GT83-TPHBK-001.00 Test opening a phone book (Generic)
 * @test GT83-TPHBK-002.00 Test closing a phone book (Generic)
 * @test GT83-TPHBK-008.00 Test deleting individual entries from the phone book (Generic)
 */
	{
	TRequestStatus aStatus;
	RMobilePhoneBookStore thePhBk;
	TInt ret=thePhBk.Open(iPhone, aPhoneBookName);
	//TEST_CHECKL(ret, KErrNone, _L("GT83-TPHBK-001.00")) ;
	if(ret == KErrNotSupported)
		{
		INFO_PRINTF2(_L("%S phone book isn't supported!"), &aPhoneBookName) ;
		thePhBk.Close() ;
		return ret;
		}
	else if(ret != KErrNone )
		{	//It doesn have any sence to continue.
		INFO_PRINTF2(_L("I can't open %S phone book!"), &aPhoneBookName) ;
		//TEST_CHECKL(ret, KErrNone, MakeCheckPointName(1, aMinorNum)) ;
		thePhBk.Close() ;
		return ret;
		}

	INFO_PRINTF2(_L("Atempt to delete %S phonebook 7 first entries..."), &aPhoneBookName);

	TInt i;
	for (i=1; i<=7; i++)
		{
		thePhBk.Delete(aStatus, i);
		User::WaitForRequest(aStatus);
		if(aStatus!=KErrNotFound)
			{
				break; //The ph/book just empty!
			}
		else if (aStatus!=KErrNone)
			{
			//Something happend
			//TEST_CHECKL(1, KErrNone, _L("GT83-TPHBK-008.00")) ;
			CHECKPOINT(ret, KErrNone, MakeCheckPointName(8, aMinorNum)) ;
			break;
			}
		}

	thePhBk.Close();
	//TEST_CHECKL(ret, KErrNone, _L("GT83-TPHBK-002.00")) ;
	return ret;
	}

//-------------------------------------------------------------------------------

void CTestAllPhBooks::TestReadAndWritePhBkL(const TDesC& aPhoneBookName, TInt aMinorNum )
/**
 * This function creates an array of names and telephone numbers which are to be used as
 * phone book entries. It then writes these entries synchronously to the SIM phone book
 * and reads them back. 
 * @param phone book name
 * @param minor test id number 
 * @test GT83-TPHBK-001.00 Test opening a phone book (Generic)
 * @test GT83-TPHBK-003.00 Test retrieval of the phone book store info and capabilities (generic)
 * @test GT83-TPHBK-006.00 Test retrieval of individual entries from the phone book (Generic)
 * @test GT83-TPHBK-006.06 Test retrieval of the first entry in an ICC-based phonebook of abbreviated dialling numbers
 */
	{
	TInt ret=KErrNone;
	TRequestStatus aStatus;
	RMobilePhoneBookStore simPhBk;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aInfoPckg(aInfo);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 *aInfob;

	TBuf8<KPBDataClientBuf> pbData, pbReadData1, pbReadData2, pbReadEmpty; // Client reserved space for phonebook data
	TInt requestedIndex(0), numOfEntries(1);
	TInt i(0), size(KPPBEntriesNum);

	ret=simPhBk.Open(iPhone,  aPhoneBookName);
	//TEST_CHECKL(ret, KErrNone, _L("GT83-TPHBK-001.00")) ;

	if(ret == KErrNotSupported)
		{
		INFO_PRINTF2(_L("%S phone book isn't supported!"), &aPhoneBookName) ;
		simPhBk.Close() ;
		return ;
		}
	else if(ret != KErrNone )
		{	//It doesn have any sence to continue.
		INFO_PRINTF2(_L("I can't open %S phone book!"), &aPhoneBookName) ;
		//TEST_CHECKL(ret, KErrNone, MakeCheckPointName(1, aMinorNum)) ;
		simPhBk.Close() ;
		return ;
		}

	simPhBk.GetInfo(aStatus, aInfoPckg);
	User::WaitForRequest(aStatus);
	//TEST_CHECKL(aStatus.Int(), KErrNone, _L("GT83-TPHBK-003.00")) ;
	CHECKPOINT(ret, KErrNone, MakeCheckPointName(3, aMinorNum)) ;
	aInfob = &aInfoPckg();


	// Create array of names and numbers
	const TText* names[]={(TText16*)KAnnText.iBuf, (TText16*)KMattSText.iBuf, (TText16*)KAndyText.iBuf,
						  (TText16*)KMattMText.iBuf,(TText16*)KPaulText.iBuf, (TText16*)KMumAndDadText.iBuf,
						  (TText16*)KSymbolText.iBuf,(TText16*)KTextTooLong.iBuf};

	const TText* numbers[]={(TText16*)KAnnPhoneNum.iBuf, (TText16*)KMattSPhoneNum.iBuf, (TText16*)KAndyPhoneNum.iBuf,
							(TText16*)KMattMPhoneNum.iBuf, (TText16*)KPaulPhoneNum.iBuf, (TText16*)KMumAndDadPhoneNum.iBuf,
							(TText16*)KSymbolPhoneNum.iBuf,(TText16*)KNumberTooLong.iBuf};


	TBuf<KBufSize> entryNumber(numbers[0]);
	TBuf<KBufSize> entryName(names[0]);

	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	//Is it posible to write anything ?
	if (aInfob->iCaps & RMobilePhoneStore::KCapsWriteAccess)
		{
		// Write the entries to the SIM Phone Book
		TInt writeIndex(0);

		for (i=1; i<=size; ++i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.FillZ();
			entryNumber.Zero();
			entryName.FillZ();
			entryName.Zero();
			pbData.FillZ();
			pbData.Zero();

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// First specify starting index and set iNumofEntries to 1, since it is only
			// possible to write one entry at a time

			writeIndex = i; // Note that TSY will overwrite any existing data. It is up to
							// the Sync to check whether it is the first available entry

			// convert number into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, entryNumber);
			TESTL(ret == KErrNone) ;

			// convert type into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);
			TESTL(ret == KErrNone) ;

			// convert text into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, entryName);
			TESTL(ret == KErrNone) ;

			simPhBk.Write(aStatus, pbData, writeIndex);
			//TEST_CHECKL(ret, KErrNone, _L("GT83-TPHBK-07.00")) ;
			User::WaitForRequest(aStatus);
			if(aStatus == KErrNotSupported)
				{
				INFO_PRINTF2(_L("Write operation for %S phone book is not supporded!"), &aPhoneBookName) ;
				break ;
				}
			TEST_CHECKL((aStatus == KErrNone || aStatus == KErrNotSupported), ETrue, MakeCheckPointName(7, aMinorNum)) ;
			}
		}
	else
		{
			INFO_PRINTF2(_L("Write operation for %S phone book is not supporded!"), &aPhoneBookName) ;
		}

	if (aInfob->iCaps & RMobilePhoneStore::KCapsReadAccess)
		{//Is posible to read this ph/book and does it have any etries?
		/* Read the entries back */
		TInt index;
		TPtrC number(NULL,0);
		TPtrC text(NULL,0);

		TUint8 aTagValue(0);
		CPhoneBookBuffer::TPhBkTagType aDataType;
		TPtrC field(NULL,0);
		TInt counter(0);

		//How many records could we read?
		if(-1 == aInfob->iUsedEntries)
			{ //Is phone book empty?	
			size = 0;
			}
		else
			{
			size = (size < aInfob->iUsedEntries )? size: aInfob->iUsedEntries; 
			}
		/* Reads Forwads and it reads all entries individually */
		for(i=1; i<=size; ++i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.FillZ();
			entryNumber.Zero();
			entryName.FillZ();
			entryName.Zero();
			pbData.FillZ();
			pbData.Zero();

			// reset all loop variables
			index = 0;
			counter = 0;
			aTagValue = 0;

			requestedIndex = i;
			simPhBk.Read(aStatus, requestedIndex, numOfEntries, pbReadData1);
			User::WaitForRequest(aStatus);
			//TEST_CHECKL(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.00")) ;
			CHECKPOINT(ret, KErrNone, MakeCheckPointName(6, aMinorNum)) ;

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// now decode PB data received in TLV format
			pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data
			while(counter < pbReadData1.Length())
				{
				TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
				if(ret==KErrNotFound) // Reached the end of phonebook data
					break;
				TESTL(ret == KErrNone) ;
				switch(aDataType)
					{
				case CPhoneBookBuffer::EPhBkTypeInt8:
					{
					TUint8 int8(0);
					counter = counter + 1; // add tag-field only in case that is beginning of new field
					if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
						{
						pbBuffer->GetValue(int8); //
						counter = counter + 1; // 1-byte integer
						index = int8;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt16:
					{
					TUint16 int16(0);
					pbBuffer->GetValue(int16);
					counter = counter + 3; // 2-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt32:
					{
					TUint32 int32(0);
					pbBuffer->GetValue(int32);
					counter = counter + 5; // 4-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeDes16:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBText)
						{
						text.Set(field);
						}
					else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
						{
						number.Set(field);
						}
					}
					break;

				default:
					break;
					} // end switch aDataType

				} // end while


			//TESTL(TInt(type)==RMobilePhone::EUnknownNumber);
			TESTL(number.Compare(entryNumber)==KErrNone);
			TESTL(text.Compare(entryName)==KErrNone);
			TESTL(index == i) ;
		} // end for


		// Try to read an empty entry
		requestedIndex = i+1; // Now set index to point to last entry in phonebook
		simPhBk.Read(aStatus, requestedIndex, numOfEntries, pbReadEmpty);
		User::WaitForRequest(aStatus);
#ifndef SIM_TSY
		CHECKPOINT(ret, KErrNone, MakeCheckPointName(6, aMinorNum)) ;
		//TEST_CHECKL(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.00")) ; // If asked to read an empty entry, TSY will not return an error
								 // It is now clients responsibility to ensure a valid index is supplied
#endif
		// Read Backwards, one by one
		for(i=size; i>0; --i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.FillZ();
			entryNumber.Zero();
			entryName.FillZ();
			entryName.Zero();
			pbData.FillZ();
			pbData.Zero();

			requestedIndex = i;
			simPhBk.Read(aStatus, requestedIndex, numOfEntries, pbReadData2);
			User::WaitForRequest(aStatus);
			//TEST_CHECKL(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.06")) ;
			CHECKPOINT(ret, KErrNone, MakeCheckPointName(6, aMinorNum)) ;

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// reset all loop variables
			index = 0;
			counter = 0;
			aTagValue = 0;

			// now decode PB data received in TLV format
			pbBuffer->Set(&pbReadData2); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data
			while(counter < pbReadData2.Length())
				{
				TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
				if(ret==KErrNotFound) // Reached the end of phonebook data
					break;
				TESTL(ret == KErrNone) ;
				switch(aDataType)
					{
				case CPhoneBookBuffer::EPhBkTypeInt8:
					{
					TUint8 int8(0);
					counter = counter + 1; // add tag-field only in case that is beginning of new field
					if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
						{
						pbBuffer->GetValue(int8); //
						counter = counter + 1; // 1-byte integer
						index = int8;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt16:
					{
					TUint16 int16(0);
					pbBuffer->GetValue(int16);
					counter = counter + 3; // 2-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt32:
					{
					TUint32 int32(0);
					pbBuffer->GetValue(int32);
					counter = counter + 5; // 4-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeDes16:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBText)
						{
						text.Set(field);
						}
					else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
						{
						number.Set(field);
						}
					}
					break;

				default:
					break;
					} // end switch aDataType

				} // end while


	//		TESTL(TInt(type)==RMobilePhone::ENationalNumber);
			TESTL(number.Compare(entryNumber)==KErrNone);
			TESTL(text.Compare(entryName)==KErrNone);
			TESTL(index == i ) ;
			} // end for

		// Try to read below the phonebook limits
		requestedIndex = i-1;
		simPhBk.Read(aStatus, requestedIndex, numOfEntries, pbReadEmpty);
		User::WaitForRequest(aStatus);
#ifndef SIM_TSY
		TESTL(aStatus==KErrNotFound || aStatus==KErrGeneral);	// this entry should be empty
#endif
		// Read by index, uses the Read() method
		for(i=1; i<=size; ++i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.FillZ();
			entryNumber.Zero();
			entryName.FillZ();
			entryName.Zero();
			pbData.FillZ();
			pbData.Zero();

			requestedIndex = i;
			simPhBk.Read(aStatus, requestedIndex, numOfEntries, pbReadData1);
			User::WaitForRequest(aStatus);
			//TEST_CHECKL(aStatus.Int(), KErrNone, _L("GT83-TPHBK-006.00")) ;
			CHECKPOINT(ret, KErrNone, MakeCheckPointName(6, aMinorNum)) ;

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// reset all loop variables
			index = 0;
			counter = 0;
			aTagValue = 0;

			// now decode PB data received in TLV format
			pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data
			while(counter < pbReadData1.Length())
				{
				TInt ret = pbBuffer->GetTagAndType(aTagValue, aDataType);
				if(ret==KErrNotFound) // Reached the end of phonebook data
					break;
				TESTL(ret == KErrNone) ;
				switch(aDataType)
					{
				case CPhoneBookBuffer::EPhBkTypeInt8:
					{
					TUint8 int8(0);
					counter = counter + 1; // add tag-field only in case that is beginning of new field
					if(aTagValue == RMobilePhoneBookStore::ETagPBAdnIndex)
						{
						pbBuffer->GetValue(int8); //
						counter = counter + 1; // 1-byte integer
						index = int8;
						}
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt16:
					{
					TUint16 int16(0);
					pbBuffer->GetValue(int16);
					counter = counter + 3; // 2-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeInt32:
					{
					TUint32 int32(0);
					pbBuffer->GetValue(int32);
					counter = counter + 5; // 4-byte integer + tag-field
					}
					break;

				case CPhoneBookBuffer::EPhBkTypeDes16:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 3; // add desc length + length-field + tag-field
					if(aTagValue == RMobilePhoneBookStore::ETagPBText)
						{
						text.Set(field);
						}
					else if(aTagValue == RMobilePhoneBookStore::ETagPBNumber)
						{
						number.Set(field);
						}
					}
					break;

				default:
					break;
					} // end switch aDataType

				} // end while


	//		TESTL(TInt(type)==RMobilePhone::ENationalNumber);
			TESTL(number.Compare(entryNumber)==KErrNone);
			TESTL(text.Compare(entryName)==KErrNone);
			TESTL(index == i) ;
			}
		} //Is posible to read this ph/book?
	else
		{
			INFO_PRINTF2(_L("Read operation for %S phone book is not supporded!"), &aPhoneBookName) ;
		}
	simPhBk.Close();

	delete pbBuffer;
	}

