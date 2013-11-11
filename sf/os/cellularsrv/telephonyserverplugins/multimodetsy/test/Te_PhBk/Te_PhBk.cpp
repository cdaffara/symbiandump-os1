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
//

/**
 @file
*/

#include <e32test.h>
#include <c32comm.h>
#include <etelmm.h>
#include <mmretrieve.h>
#include <mpbutil.h>

#include "Te_PhBk.h"

//------------------------------------------------------------------
//			** TEST PhBk STEPS ***
//------------------------------------------------------------------

CTestPhoneInfo::CTestPhoneInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestPhoneInfo"));
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
	TRequestStatus status;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 aPhoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg aPhoneStoreInfoPckg(aPhoneStoreInfo);

	iPhone.GetPhoneId(status, aPhoneId);
	User::WaitForRequest(status);
	CHECKPOINT(status, KErrNone, _L("*** Failed RMobilePhone::GetPhoneId() ***")) ;

	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Manufacturer ID : %S")), &aPhoneId.iManufacturer);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Model ID : %S")), &aPhoneId.iModel);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Revision ID : %S")), &aPhoneId.iRevision);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Serial number : %S")), &aPhoneId.iSerialNumber);
	User::After(4000000L);	// Give user time to see the test results

	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelMeDialledPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"DIALLED_CALLS\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"DIALLED_CALLS\" not supported by this phone ***"));
	    }


	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelMeMissedPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"MISSED_CALLS\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"MISSED_CALLS\" not supported by this phone ***"));
	    }


	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelMeReceivedPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"RECEIVED_CALLS\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"RECEIVED_CALLS\" not supported by this phone ***"));
	    }

	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelMeAdnPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"ME_PHONEBOOK\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"ME_PHONEBOOK\" not supported by this phone ***"));
	    }

	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelIccVoiceMailBox);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"VOICE_MAILBOX\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"VOICE_MAILBOX\" not supported by this phone ***"));
	    }

	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelIccAdnPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"SIM_PHONEBOOK\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"SIM_PHONEBOOK\" not supported by this phone ***"));
	    }

	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelCombinedAdnPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"COMBINED_SIM_ME\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"COMBINED_SIM_ME\" not supported by this phone ***"));
	    }

	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelIccFdnPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"FIXED_DIAL_NUM\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"FIXED_DIAL_NUM\" not supported by this phone ***"));
	    }

	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelIccSdnPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"SERV_DIAL_NUM\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"SERV_DIAL_NUM\" not supported by this phone ***"));
	    }

	iPhone.GetPhoneStoreInfo(status, aPhoneStoreInfoPckg, KETelIccBdnPhoneBook);
	User::WaitForRequest(status);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Testing \"BARRED_DIAL_NUM\" returned %d")), status.Int());
	if(status.Int() == KErrNone)
	    {
		aPhoneStoreInfo = aPhoneStoreInfoPckg();
		checkPhoneBookInfoL(aPhoneStoreInfo);
	    }
	else
	    {
		INFO_PRINTF1(_L("*** Phone Book \"BARRED_DIAL_NUM\" not supported by this phone ***"));
	    }
	    
	// RAM phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeAdnPhoneBook, KEtelGsmPhoneBookType, _L("RAM phonebook")) == KErrNone );
	// Dialled numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeDialledPhoneBook, KEtelGsmPhoneBookType, _L("Dialled numbers phonebook")) == KErrNone );
	// Missed calls phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeMissedPhoneBook, KEtelGsmPhoneBookType, _L("Missed calls phonebook")) == KErrNone );
	// Received calls phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeReceivedPhoneBook, KEtelGsmPhoneBookType, _L("Received calls phonebook")) == KErrNone );
	// SIM phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccAdnPhoneBook, KEtelGsmPhoneBookType, _L("SIM phonebook")) == KErrNone );
	// Fixed dialling numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccFdnPhoneBook, KEtelGsmPhoneBookType, _L("Fixed dialling numbers phonebook")) == KErrNone );
	// Service dialling numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccSdnPhoneBook, KEtelGsmPhoneBookType, _L("Service dialling numbers phonebook")) == KErrNone );
	// Own number phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelOwnNumberStore, KEtelGsmPhoneBookType, _L("Own number phonebook")) == KErrNone );
	// Emergency number phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelEmergencyNumberStore, KEtelGsmPhoneBookType, _L("Emergency number phonebook")) == KErrNone );
	// SIM dialled numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccLndPhoneBook, KEtelGsmPhoneBookType, _L("SIM dialled numbers phonebook")) == KErrNone );
	// Voice mail
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccVoiceMailBox, KEtelGsmPhoneBookType, _L("Voice mail")) == KErrNone );
	// ICC SMS Store
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccSmsStore, KEtelGsmPhoneBookType, _L("ICC SMS Store")) == KErrNone );
	
	INFO_PRINTF1(_L("\n\nTest using RMobilePhone::GetPhoneStoreInfo() with CDMA mode specified\n"));
	
	// RAM phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeAdnPhoneBook, KEtelCdmaPhoneBookType, _L("RAM phonebook")) == KErrNone );
	// Dialled numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeDialledPhoneBook, KEtelCdmaPhoneBookType, _L("Dialled numbers phonebook")) == KErrNone );
	// Missed calls phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeMissedPhoneBook, KEtelCdmaPhoneBookType, _L("Missed calls phonebook")) == KErrNone );
	// Received calls phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeReceivedPhoneBook, KEtelCdmaPhoneBookType, _L("Received calls phonebook")) == KErrNone );
	// SIM phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccAdnPhoneBook, KEtelCdmaPhoneBookType, _L("SIM phonebook")) == KErrNone );
	// Fixed dialling numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccFdnPhoneBook, KEtelCdmaPhoneBookType, _L("Fixed dialling numbers phonebook")) == KErrNone );
	// Service dialling numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccSdnPhoneBook, KEtelCdmaPhoneBookType, _L("Service dialling numbers phonebook")) == KErrNone );
	// Own number phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelOwnNumberStore, KEtelCdmaPhoneBookType, _L("Own number phonebook")) == KErrNone );
	// Emergency number phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelEmergencyNumberStore, KEtelCdmaPhoneBookType, _L("Emergency number phonebook")) == KErrNone );
	// SIM dialled numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccLndPhoneBook, KEtelCdmaPhoneBookType, _L("SIM dialled numbers phonebook")) == KErrNone );
	// Voice mail
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccVoiceMailBox, KEtelCdmaPhoneBookType, _L("Voice mail")) == KErrNone );
	// ICC SMS Store
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccSmsStore, KEtelCdmaPhoneBookType, _L("ICC SMS Store")) == KErrNone );
	
	INFO_PRINTF1(_L("\n\nTest using RMobilePhone::GetPhoneStoreInfo() with USIM mode specified\n"));
	
	// RAM phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeAdnPhoneBook, KEtelUSimPhoneBookType, _L("RAM phonebook")) == KErrNone );
	// Dialled numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeDialledPhoneBook, KEtelUSimPhoneBookType, _L("Dialled numbers phonebook")) == KErrNone );
	// Missed calls phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeMissedPhoneBook, KEtelUSimPhoneBookType, _L("Missed calls phonebook")) == KErrNone );
	// Received calls phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelMeReceivedPhoneBook, KEtelUSimPhoneBookType, _L("Received calls phonebook")) == KErrNone );
	// SIM phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccAdnPhoneBook, KEtelUSimPhoneBookType, _L("SIM phonebook")) == KErrNone );
	// Fixed dialling numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccFdnPhoneBook, KEtelUSimPhoneBookType, _L("Fixed dialling numbers phonebook")) == KErrNone );
	// Service dialling numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccSdnPhoneBook, KEtelUSimPhoneBookType, _L("Service dialling numbers phonebook")) == KErrNone );
	// Own number phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelOwnNumberStore, KEtelUSimPhoneBookType, _L("Own number phonebook")) == KErrNone );
	// Emergency number phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelEmergencyNumberStore, KEtelUSimPhoneBookType, _L("Emergency number phonebook")) == KErrNone );
	// SIM dialled numbers phonebook
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccLndPhoneBook, KEtelUSimPhoneBookType, _L("SIM dialled numbers phonebook")) == KErrNone );
	// Voice mail
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccVoiceMailBox, KEtelUSimPhoneBookType, _L("Voice mail")) == KErrNone );
	// ICC SMS Store
	TEST( CheckPhoneStoreInfo(iPhone, KETelIccSmsStore, KEtelUSimPhoneBookType, _L("ICC SMS Store")) == KErrNone );
	    
	return TestStepResult();
	}
	
//------------------------------------------------------------------

CTestGetCapsAndInfo::CTestGetCapsAndInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestGetCapsAndInfo"));
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
	TInt ret=KErrNone;
	TRequestStatus status;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aInfoPckg(aInfo);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 *aInfob;

	// SIM phone book - mandatory
	RMobilePhoneBookStore simPhBk;
	ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	TEST_CHECKL(ret, KErrNone, _L("*** Failed to open ICC ADN Phonebook!! ***"));
	simPhBk.GetInfo(status, aInfoPckg);
	User::WaitForRequest(status);
	CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-003.06 - *** Failed RMobilePhoneBookStore::GetInfo() ***")) ;
	aInfob = &aInfoPckg();
	TEST(aInfob->iCaps & RMobilePhoneStore::KCapsWriteAccess);
	TEST(!aInfob->iName.Compare(KETelIccAdnPhoneBook()));
	simPhBk.Close();

	// ME phone book - optional support by the phones
	RMobilePhoneBookStore mePhBk;
	ret=mePhBk.Open(iPhone,  KETelMeAdnPhoneBook());
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("ME phonebook supported"));
		mePhBk.GetInfo(status, aInfoPckg);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-003.04 - *** Failed RMobilePhoneBookStore::GetInfo() ***")) ;
		aInfob = &aInfoPckg();
		TEST(aInfob->iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TEST(!aInfob->iName.Compare(KETelMeAdnPhoneBook()));
		}
	else
		INFO_PRINTF1(_L("*** Failed to open ME ADN Phonebook, Phonebook may not be supported by phone ***"));

	mePhBk.Close();

	// Combined ME and SIM phone book - optional support by the phones
	RMobilePhoneBookStore combPhBk;
	ret=combPhBk.Open(iPhone,  KETelCombinedAdnPhoneBook());
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Combined ME & SIM phonebook supported"));

		combPhBk.GetInfo(status, aInfoPckg);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-003.11 - *** Failed RMobilePhoneBookStore::GetInfo() ***")) ;

		aInfob = &aInfoPckg();

		TEST(aInfob->iCaps && RMobilePhoneStore::KCapsWriteAccess);
		TEST(!aInfob->iName.Compare(KETelCombinedAdnPhoneBook()));
		}
	combPhBk.Close();

		// Combined ME and SIM phone book
	ret=combPhBk.Open(iPhone,  KETelCombinedAdnPhoneBook());
	if(ret==KErrNone)
		{
		combPhBk.GetInfo(status, aInfoPckg);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-003.11 - *** Failed RMobilePhoneBookStore::GetInfo() ***")) ;

		aInfob = &aInfoPckg();

		TEST(aInfob->iMaxNumLength >0);
		TEST(aInfob->iMaxTextLength >=0);
		TEST(!aInfob->iName.Compare(KETelCombinedAdnPhoneBook()));

		#ifdef __TEST_CANCEL__
		combPhBk.GetInfo(status, aInfoPckg);
		combPhBk.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
		User::WaitForRequest(status);
		INFO_PRINTF2(_L("Async MT PhoneBook GetInfo Cancel Status = %d"),status);
		CHECKPOINT_EXPR((status==KErrCancel || status==KErrNone), _L("GT83-TPHBK-003.13 - *** Failed CancelAsyncRequest(EMobilePhoneStoreGetInfo) ***"));
		#endif
		}
	combPhBk.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestReadAndWrite::CTestReadAndWrite()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestReadAndWrite"));
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
	TRequestStatus status;
	TInt ret(KErrNone);
	
	ret=mePhBk.Open(iPhone,  KETelMeAdnPhoneBook);
	if(ret==KErrNone)
		{
		INFO_PRINTF1(_L("Write to the ME PhoneBook..."));

		TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data

		_LIT16(KTel,"123456%03d");
		_LIT16(KText,"TelNo %03d");
		TBuf16<13> text(KTel), number(KText);

		CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
		pbBuffer->Set(&pbData); // Set it to point to the Client buffer

		for(TInt index=1;index<=KEntries;++index)
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
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, number);
			TESTL(ret == KErrNone) ;

			// convert number type into TLV format and append it to allocated buffer
			TUint8 tonNpi=KTypeOfNumber; 
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, tonNpi);
			TESTL(ret == KErrNone);

			text.Format(KText(), index);
			INFO_PRINTF2(_L("Writing \'%S\'"),&text);

			// convert text into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, text);
			TESTL(ret == KErrNone);

			mePhBk.Write(status, pbData, index);
			User::WaitForRequest(status);
			TEST_CHECKL(status.Int(), KErrNone, _L("GT83-TPHBK-007.04 - *** Failed to Write to the ME Phonebook! ***"));
			}
		delete pbBuffer;
		}

	else
		INFO_PRINTF1(_L("*** Failed to open ME ADN Phonebook! ***"));

	mePhBk.Close();
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
 * @test GT83-TPHBK-001.11 Test opening a combination of the ADN stored in ME and ICC
 * @test GT83-TPHBK-002.11 Test closing a combination of the ADN stored in ME and ICC
 * @test GT83-TPHBK-006.06 Test retrieval of entries in an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-006.11 Test retrieval of entries in a combination of the ADN stored in ME and ICC
 * @test GT83-TPHBK-007.06 Test writing a single phonebook entry to a specified location in an ICC-based phonebook containing voice mailbox numbers, where the location is empty
 * @test GT83-TPHBK-007.11 Test writing a single phonebook entry to a specified location in an ICC-based phonebook containing the fixed dialling numbers list, when the phone is in a mode that allows editing the fixed dialling numbers, where the location is empty
*/
	{
	TInt ret=KErrNone;
	TRequestStatus status;
	RMobilePhoneBookStore simPhBk;

	TBuf8<KPBDataClientBuf> pbData, pbReadData1, pbReadData2, pbReadEmpty; // Client reserved space for phonebook data
	TInt requestedIndex(0);

	// Create array of names and numbers
	const TText16* names[]={(TText16*)KAnnText.iBuf, (TText16*)KMattSText.iBuf, (TText16*)KAndyText.iBuf,
						  (TText16*)KMattMText.iBuf,(TText16*)KPaulText.iBuf, (TText16*)KMumAndDadText.iBuf,
						  (TText16*)KSymbolText.iBuf,(TText16*)KTextTooLong.iBuf};

	const TText16* numbers[]={(TText16*)KAnnPhoneNum.iBuf, (TText16*)KMattSPhoneNum.iBuf, (TText16*)KAndyPhoneNum.iBuf,
							(TText16*)KMattMPhoneNum.iBuf, (TText16*)KPaulPhoneNum.iBuf, (TText16*)KMumAndDadPhoneNum.iBuf,
							(TText16*)KSymbolPhoneNum.iBuf,(TText16*)KNumberTooLong.iBuf};


	TBuf16<KBufSize> entryNumber(numbers[0]);
	TBuf16<KBufSize> entryName(names[0]);
	// Motorola T260 requires a pause otherwise it does not respond to the 
	// next AT command.
	User::After(500000);
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	// Write the entries to the SIM Phone Book
	TInt writeIndex(0);
	TInt i(0), size(KPPBEntriesNum);

	TUint8 tagValue(0);
	CPhoneBookBuffer::TPhBkTagType aDataType;


	/* Read the entries back */
	TUint16 index;
	TUint8  tonNpi(0);
	TPtrC16 number(NULL,0);
	TPtrC16 text(NULL,0);



	ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook());
	CHECKPOINT(ret, KErrNone, _L("*** Failed to open ICC ADN Phonebook ***"));
	if(ret==KErrNone)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg dummyInfoPckg(iDummyInfo);
		for (i=1; i<=size; ++i)
			{
			simPhBk.GetInfo(status, dummyInfoPckg);		// Recommended by EtelMM API documentation
			User::WaitForRequest(status);

			//first reset the following three local buffers to delete any existing data
			entryNumber.Zero();
			entryName.Zero();
			pbData.Zero();

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// First specify starting index and set iNumofEntries to 1, since it is only
			// possible to write one entry at a time

			writeIndex = i; // Note that TSY will overwrite any existing data. It is up to
							// the Sync to check whether it is the first available entry

			// convert number into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, entryNumber);
			TEST(ret == KErrNone) ;

			// convert type into TLV format and append it to allocated buffer
			TUint8 tonNpi=KTypeOfNumber; 
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, tonNpi);
			TEST(ret == KErrNone) ;

			// convert text into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, entryName);
			TEST(ret == KErrNone) ;

			simPhBk.Write(status, pbData, writeIndex);
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-007.06 - *** Failed to write to the ICC ADN Phonebook ***"));
			}

		/* Reads Forwads and it reads all entries individually */
		for(i=1; i<=size; ++i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.Zero();
			entryName.Zero();
			pbData.Zero();

			// reset all loop variables
			index = 0;
			tonNpi = 0;
			tagValue = 0;

			requestedIndex = i;
			simPhBk.Read(status, requestedIndex, 1, pbReadData1);
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-006.06 - *** Failed to read from ICC ADN Phonebook ***")) ;

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// now decode PB data received in TLV format
			pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data

			while (pbBuffer->GetTagAndType(tagValue, aDataType)==KErrNone)
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
					pbBuffer->SkipValue(aDataType);
					break;
					}
			}; // end while

			TEST(tonNpi==KTypeOfNumber);
			TEST(number.Compare(entryNumber)==KErrNone);
			TEST(text.Compare(entryName)==KErrNone);
			TEST(index == i) ;
		} // end for

		// Try to read an empty entry - find out which one is empty first
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1 info;
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg infoPckg(info);
		simPhBk.GetInfo(status,infoPckg);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-006.06 - *** Failed RMobilePhoneBookStore::GetInfo() ***")) ;

		// If used entry information not available, use total entries and guess
		// last one is not used.

		requestedIndex = (info.iUsedEntries>=0) ? info.iUsedEntries+1 : info.iTotalEntries;

		simPhBk.Read(status, requestedIndex, 1, pbReadEmpty);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNotFound, _L("GT83-TPHBK-006.06")) ; // If asked to read an empty entry, TSY will return KErrNotFound
								 // It is now clients responsibility to ensure a valid index is supplied

		// Read Backwards, one by one
		for(i=size; i>0; --i)
			{
			//first reset the following three local buffers to delete any existing data
			entryNumber.Zero();
			entryName.Zero();
			pbData.Zero();

			requestedIndex = i;
			simPhBk.Read(status, requestedIndex, 1, pbReadData2);
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-006.06")) ;

			entryNumber=numbers[i-1];
			entryName=names[i-1];

			// reset all loop variables
			index = 0;
			tagValue = 0;

			// now decode PB data received in TLV format
			pbBuffer->Set(&pbReadData2); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data
			while (pbBuffer->GetTagAndType(tagValue, aDataType)==KErrNone)
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
					pbBuffer->SkipValue(aDataType);
					break;
					}
			} // end while

			TEST(tonNpi==KTypeOfNumber);
			TEST(number.Compare(entryNumber)==KErrNone);
			TEST(text.Compare(entryName)==KErrNone);
			TEST(index == i) ;
			} // end for
		} //end if

	simPhBk.Close();

// From Combined phone book

	//
	// This part of the test reads the 1st few entries in the combined Phone Book (ME+
	// SIM). As the Combined Phonebook does not (should not) have 'WriteAccess', some
	// entries will first be written to the ME phone book. The SIM phone book
	// already contains the entries written above.
	//
	WriteToMEPhBkL();

	RMobilePhoneBookStore combPhBk;
	ret=combPhBk.Open(iPhone,  KETelCombinedAdnPhoneBook);
	if(ret==KErrNone)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg dummyInfoPckg(iDummyInfo);
		combPhBk.GetInfo(status, dummyInfoPckg);		// Recommended by EtelMM API documentation
		User::WaitForRequest(status);

		const TInt KEntries=4;		// Same value in WriteToMEPhBk()
		const TInt KBufSize=20;

		// Read index locations 1 to KEntries
		for(TUint16 index=1;index<=KEntries;++index)
			{
			combPhBk.Read(status, index, 1, pbReadData2);
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-006.11 - *** Failed to read from Combined Phonebook ***")) ;

			// now decode PB data received in TLV format
			TPtrC16 telNumPtr;
			TPtrC16 textPtr;

			pbBuffer->Set(&pbReadData2); // Set pbBuffer to point to received phonebook data
			pbBuffer->StartRead();       // Start reading received phonebook data

			while (pbBuffer->GetTagAndType(tagValue, aDataType)==KErrNone)
				{
				switch (tagValue)
					{
				case RMobilePhoneBookStore::ETagPBText:
					pbBuffer->GetValue(textPtr);			
					break;
				case RMobilePhoneBookStore::ETagPBNumber:
					pbBuffer->GetValue(telNumPtr);			
					break;
				default:
					// An unsupported field type - just skip this value
					pbBuffer->SkipValue(aDataType);
					break;
					}
				} 

			//
			// Assemble the data we expect to have read
			_LIT16(KTel,"123456%03d");
			_LIT16(KText,"TelNo %03d");
			TBuf16<KBufSize> telNumExpected;
			TBuf16<KBufSize> textExpected;
			telNumExpected.Format(KTel,index);
			textExpected.Format(KText,index);
						
			//
			// Check the data we actually read with what we expected to read
			TEST(telNumPtr.Compare(telNumExpected)==0);
			TEST(textPtr.Compare(textExpected)==0);
			} 

		// Entry to write - this should be read only...
		requestedIndex = 5;

		// convert number into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
		TEST(ret == KErrNone) ;

		// convert number type into TLV format and append it to allocated buffer
		tonNpi=KTypeOfNumber;
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, tonNpi);
		TEST(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KAnnText);
		TEST(ret == KErrNone) ;
		}
	combPhBk.Close();
	delete pbBuffer;
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestDelete::CTestDelete()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestDelete"));
	}
//------------------------------------------------------------------

TVerdict CTestDelete::doTestStepL( void )
/**
 * This function deletes the first 7 entries from the SIM phone book
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-006.	06 Test retrieval of the first entry in an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-008.06 Test deleting an individual phonebook entry from an ICC-based phonebook of abbreviated dialling numbers
 */
	{
	TRequestStatus status;
	RMobilePhoneBookStore simPhBk;
	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data
	TInt ret(KErrNone);
	
	ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	CHECKPOINT(ret, KErrNone, _L("*** Failed to open ICC ADN Phonebook! ***"));
	if(ret==KErrNone)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1 info;
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg infoPckg(info);
		simPhBk.GetInfo(status, infoPckg);		// Recommended by EtelMM API documentation
		User::WaitForRequest(status);

		for(TInt i=1; i<=KPPBEntriesNum; ++i)
			{
			simPhBk.Delete(status, i);
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-008.06 - ***Failed RMobilePhoneBookStore::Delete()")) ;
			}

		// now try to read first (empty) entry
		simPhBk.Read(status, 1, 1, pbData);
		User::WaitForRequest(status); 
		CHECKPOINT(status.Int(), KErrNotFound, _L("GT83-TPHBK-006.06")) ;
		// If asked to read an emptry entry, TSY will not return an error
		// It is now client's responsibility to ensure a valid index is supplied
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
	SetTestStepName(_L("TestNotifications"));
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
 */
	{
	TRequestStatus status;
	RMobilePhoneBookStore simPhBk, combPhBk;
	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data
	TInt index(0), ret(0);

	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read/write phonebook entries
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	CHECKPOINT(ret, KErrNone, _L("*** Failed to open ICC ADN Phonebook! ***"));
	if(ret==KErrNone)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1 info;
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg infoPckg(info);
		simPhBk.GetInfo(status, infoPckg);		// Recommended by EtelMM API documentation
		User::WaitForRequest(status);

		ret=combPhBk.Open(iPhone,  KETelCombinedAdnPhoneBook);
		if(ret==KErrNone)
			{
			combPhBk.GetInfo(status, infoPckg);		// Recommended by EtelMM API documentation
			User::WaitForRequest(status);

			TRequestStatus status1, status2;
			TInt index1 = 0;
			TInt index2 = 0;
			TUint32 eventToReturn;

			simPhBk.NotifyStoreEvent(status1, eventToReturn,index1);	// Will Notify if an entry is added
			TEST(status1.Int() == KRequestPending) ;

			combPhBk.NotifyStoreEvent(status2, eventToReturn, index2); // Same here ;-)
			TEST(status2.Int() == KRequestPending) ;

			index = 1; // write to the first slot
			// convert number into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
			TEST(ret == KErrNone) ;

			// convert number type into TLV format and append it to allocated buffer
			TUint8 tonNpi=KTypeOfNumber;
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, tonNpi);
			TEST(ret == KErrNone) ;

			// convert text into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, KAnnText);
			TEST(ret == KErrNone) ;

			simPhBk.Write(status, pbData, index);
			User::WaitForRequest(status); // Write request
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-007.06 - *** Failed RMobilePhoneBookStore::Write() ***")) ;

			User::WaitForRequest(status1); // Notification
			if (status1==KErrNotSupported)
				{
				// Notification is not supported by phonebook store - as expected
				INFO_PRINTF1(_L("Phonebook Store Write Notification is NOT Supported"));
				}
			else
				{
				// This should not happen since this notification is not supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Write Notification IS Supported - failed"));
				SetTestStepResult(EFail);
				}

			User::WaitForRequest(status2);
			if (status2==KErrNotSupported)
				{
				// Notification is not supported by phonebook store - as expected
				INFO_PRINTF1(_L("Phonebook Store Write Notification is NOT Supported"));
				}
			else
				{
				// This should not happen since this notification is not supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Write Notification IS Supported - failed"));
				SetTestStepResult(EFail);
				}

			status1= KErrNone; // reset status variable
			simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);

			combPhBk.NotifyStoreEvent(status2, eventToReturn, index2);

			// Now delete that entry
			simPhBk.Delete(status, 1);
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-008.06 - *** Failed RMobilePhoneBookStore::Delete() ***")) ;

			User::WaitForRequest(status1);
			if (status1==KErrNotSupported)
				{
				// Notification is not supported by phonebook store - as expected
				INFO_PRINTF1(_L("Phonebook Store Delete Notification is NOT Supported"));
				}
			else
				{
				// This should not happen since this notification is not supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Delete Notification IS Supported - failed"));
				SetTestStepResult(EFail);
				}

			User::WaitForRequest(status2);
			if (status2==KErrNotSupported)
				{
				// Notification is not supported by phonebook store - as expected
				INFO_PRINTF1(_L("Phonebook Store Delete Notification is NOT Supported"));
				}
			else
				{
				// This should not happen since this notification is not supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Delete Notification IS Supported - failed"));
				SetTestStepResult(EFail);
				}

			// EntryChangedNotificationCancel Request
			status1= KErrNone; // reset status variable
			simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);
			simPhBk.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
			User::WaitForRequest(status1);
			// Isn't supported
			// TEST_CHECKL(status1.Int(), KErrNone, _L("GT83-TPHBK-008.16")) ;

			if (status1==KErrNotSupported)
				{
				// Notification is not supported by phonebook store - as expected
				INFO_PRINTF1(_L("Phonebook Store Notification is NOT Supported"));
				}
			else
				{
				// This should not happen since this notification is not supported by phonebook store
				INFO_PRINTF1(_L("Phonebook Store Notification cancelled"));
				SetTestStepResult(EFail);
				}

			// Cancel Notification Request after 2 seconds
			status1= KErrNone; // reset status variable
			simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);

			// Now write the same entry again
			simPhBk.Write(status, pbData, index);		
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("*** Failed RMobilePhoneBookStore::Write() ***")) ;

			User::After(2000000L);
			simPhBk.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
			User::WaitForRequest(status1);

			INFO_PRINTF2(TRefByValue<const TDesC>(_L("Cancel Status = %d")), status1.Int());

			TEST(status1==KErrCancel || status1==KErrNotSupported);

			simPhBk.Delete(status, 1); // Deleting the last added entry so that the test ends "clean".
			User::WaitForRequest(status);
			}
		combPhBk.Close();
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
	SetTestStepName(_L("TestInvalidOps"));
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
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1	info;
	TRequestStatus status;

	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data
	TInt index(0); 
	TInt numEntries(1);
	TInt ret(KErrNone);

	ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	TEST_CHECKL(ret, KErrNone, _L("*** Failed to open ICC ADN PhoneBook ***"));
	if(ret==KErrNone)
		{
		CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read/write phonebook entries
		pbBuffer->Set(&pbData); // Set it to point to the Client buffer

		// Read and write past the end of phone book - First ask the phone what is the maximum number of
		// entries, then tries to write past the max index!
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg infoPckg(info);
		simPhBk.GetInfo(status, infoPckg);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-003.06 - ***Failed RMobilePhoneBookStore::GetInfo() ***")) ;

		TInt maxNumber = info.iTotalEntries;

		// Write an entry beyond the returned phonebook limit
		index = maxNumber + 1;

		// convert number into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
		TEST(ret == KErrNone) ;

		// convert number type into TLV format and append it to allocated buffer
		TUint8 tonNpi=KTypeOfNumber;
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,tonNpi);
		TEST(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, KAnnText);
		TEST(ret == KErrNone) ;

		simPhBk.Write(status, pbData, index);
		User::WaitForRequest(status); // Write request
		TEST(status!=KErrNone) ;  // Should not accept Phone book entry maxNumber + 1

		pbData.Zero(); // Clear buffer

		// Read that entry
		simPhBk.Read(status, index, numEntries, pbData);
		User::WaitForRequest(status);
		TEST(status!=KErrNone); // There should be no entry written

		// Delete an entry beyond the returned phonebook limit
		simPhBk.Delete(status, index); // Testing to delete an impossible indexed entry
		User::WaitForRequest(status);
		TEST(status!=KErrNone); // There should be no entry deleted

		// Write text field too long - assume 30 characters is too long!
		index = 1;

		// convert number into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, KAnnPhoneNum);
		TEST(ret == KErrNone) ;

		// convert number type into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, tonNpi);
		TEST(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, KTextTooLong);
		TEST(ret == KErrNone) ;

		simPhBk.Write(status, pbData, index);
		User::WaitForRequest(status); // Write request
		if (status==KErrNone)
			{
			INFO_PRINTF1(_L("Phone book does not error writing a too long name tag!"));
			simPhBk.Delete(status, index);
			User::WaitForRequest(status);
			TEST(status == KErrNone) ;
			}
		else
			{
			TEST(status == KErrGeneral) ;
			}

		// Write number field too long - assume 30 characters is too long!
		index = 1;
		// convert number into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, KNumberTooLong);
		TEST(ret == KErrNone) ;

		// convert number type into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi,tonNpi);
		TEST(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, KAnnText);
		TEST(ret == KErrNone) ;

		simPhBk.Write(status, pbData, index);
		User::WaitForRequest(status); // Write request
		if (status==KErrNone)
			{
			INFO_PRINTF1(_L("Phone book does not error writing a too long number field!"));
			simPhBk.Delete(status, index);
			User::WaitForRequest(status);
			TEST(status == KErrNone) ;
			}
		else
			{
			TEST(status == KErrGeneral) ;
			}
		delete pbBuffer;
		}
	simPhBk.Close();
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestAdvancedPhBk::CTestAdvancedPhBk()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestAdvancedPhBk"));
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
	TRequestStatus status;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aPhoneStoreInfob;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aPhoneStoreInfoPckgb(aPhoneStoreInfob);
	RMobilePhoneBookStore simPhBk;

	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data

	_LIT16(KTel,"1632960000%02d");
	_LIT16(KText,"Tel %02d");
	TBuf16<15> text(KTel), number(KText);

	TInt index(1);
	ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	TEST_CHECKL(ret, KErrNone, _L("*** Failed to open ICC ADN PhoneBook ***"));
	if(ret==KErrNone)
		{
		// Write the entries
		INFO_PRINTF1(_L("Fill the 1st 30 locations in the SIM phonebook..."));
		simPhBk.GetInfo(status, aPhoneStoreInfoPckgb);
		User::WaitForRequest(status);
		TEST_CHECKL(status.Int(), KErrNone, _L("GT83-TPHBK-003.06")) ;
		aPhoneStoreInfob = aPhoneStoreInfoPckgb();
		TInt used=aPhoneStoreInfob.iUsedEntries ;

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
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, number);
			TEST(ret == KErrNone) ;

			// convert number type into TLV format and append it to allocated buffer
			TUint8 tonNpi=KTypeOfNumber; 
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, tonNpi);
			TEST(ret == KErrNone) ;

			text.Format(KText(), index);
			INFO_PRINTF2(_L("Writing \'%S\'"),&text);
			// convert text into TLV format and append it to allocated buffer
			ret=pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, text);
			TEST(ret == KErrNone) ;

			simPhBk.Write(status, pbData, index);
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-007.06 - *** Failed RMobilePhoneBookStore::Write() *** "));
			used++;
			index++;
			} // end while
		delete pbBuffer;
		}
	simPhBk.Close();

	// Delete the entries //
	INFO_PRINTF1(_L("Re-opening phonebook..."));
	ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	TEST_CHECKL(ret, KErrNone, _L("*** Failed to open ICC ADN PhoneBook ***"));
	if(ret==KErrNone)
		{
		simPhBk.GetInfo(status, aPhoneStoreInfoPckgb);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-003.06 - *** Failed RMobilePhoneBookStore::GetInfo() ***")) ;
		aPhoneStoreInfob = aPhoneStoreInfoPckgb();
		INFO_PRINTF1(_L("Deleting the entries..."));
		for (int iy=1;iy<=KFillTo;++iy)
			{
			INFO_PRINTF2(_L("Deleting %03d"),iy);
			simPhBk.Delete(status, iy);
			User::WaitForRequest(status);
			CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-008.06 - *** Failed RMobilePhoneBookStore::Delete() ***"));
			}
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
	INFO_PRINTF1(_L(" Test retrieval of a Phonebook "));
	INFO_PRINTF1(_L("==============================="));
	// First write 15 entries to the phone
	INFO_PRINTF1(_L("Writing 15 entries to the phone"));
	WriteBatchOfEntriesToThePhoneL();

	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read phonebook data
	TBuf8<KPBDataClientBuf> pbReadData1; // Client reserved space for phonebook data
	TRequestStatus status;

	TUint16 index(0);
	TUint8 tonNpi;
	TPtrC16 number(NULL,0);
	TPtrC16 text(NULL,0);

	TUint8 tagValue(0);
	CPhoneBookBuffer::TPhBkTagType aDataType;

	RMobilePhoneBookStore simPhBk;
	TInt ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	TEST_CHECKL(ret, KErrNone, _L("*** Failed to open ICC ADN PhoneBook ***"));
	if(ret==KErrNone)
		{	
		// Get total number of phonebook entires
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aInfo;
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aInfoPckg(aInfo);
		simPhBk.GetInfo(status, aInfoPckg);
		User::WaitForRequest(status);
		TEST_CHECKL(status.Int(), KErrNone, _L("GT83-TPHBK-003.06")) ;

		INFO_PRINTF2(_L("Start retrieving %d phonebook entries"), aInfo.iTotalEntries);

		simPhBk.Read(status, 1, aInfo.iTotalEntries, pbReadData1);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-004.06 - *** Failed RMobilePhoneBookStore::Read() ***")) ;

		// pbReadData1 will contain a batch of retrieved entries, so start by parsing through its
		// data to retrieve individual entries
		pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
		pbBuffer->StartRead();       // Start reading received phonebook data
		TInt entryCount=0;
		while (pbBuffer->GetTagAndType(tagValue, aDataType)==KErrNone)
			{
			switch (tagValue)
				{
			case RMobilePhoneBookStore::ETagPBAdnIndex:
				entryCount++;
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
				pbBuffer->SkipValue(aDataType);
				break;
				}

			// Print each individual entry
			if((tagValue == RMobilePhoneBookStore::ETagPBNewEntry) && (entryCount>1)) // Only print if all fields retrieved
				{
				INFO_PRINTF4(_L("%d = >%S<  >%S<"), index, &text, &number); // It is not displaying data corectly on the console
				}
			} // end while
		}
	simPhBk.Close();
	delete pbBuffer;
	INFO_PRINTF1(_L("Deleting the entries "));
	clearPhoneBookL(KETelIccAdnPhoneBook());
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
	INFO_PRINTF1(_L(" Test retrieval of a Batch of Phonebook Entries "));
	INFO_PRINTF1(_L("================================================"));
	// First write 15 entries to the phone
	INFO_PRINTF1(_L("Writing 15 entries to the phone"));
	WriteBatchOfEntriesToThePhoneL();

	RMobilePhoneBookStore simPhBk;
	TRequestStatus status;
	TBuf8<KPBDataClientSmallBuf> pbReadData1; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read phonebook data

	TInt ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	TEST_CHECKL(ret, KErrNone, _L("*** Failed to open ICC ADN PhoneBook ***"));
	if(ret==KErrNone)
		{
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg dummyInfoPckg(iDummyInfo);
		simPhBk.GetInfo(status, dummyInfoPckg);		// Recommended by EtelMM API documentation
		User::WaitForRequest(status);
		TEST_CHECKL(status.Int(), KErrNone, _L("*** Failed RMobilePhoneBookStore::GetInfo() ***")) ;

		INFO_PRINTF1(_L("This test can take up to a minute to complete..."));
		TInt startIndex = 3; // Where to start in the phonebook
		TInt batchSize = 4; // How many entries to get.

		INFO_PRINTF3(_L("Start retrieving a batch of %d phonebook entries with starting index %d"), batchSize, startIndex);

		simPhBk.Read(status, startIndex, batchSize, pbReadData1);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-005.06 - *** Failed RMobilePhoneBookStore::Read() ***")) ;

		TUint16 index(0);
		TUint8 tonNpi;
		TPtrC16 number(NULL,0);
		TPtrC16 text(NULL,0);

		TUint8 tagValue(0);
		CPhoneBookBuffer::TPhBkTagType aDataType;

		// pbReadData1 will contain a batch of retrieved entries, so start by parsing through its
		// data to retrieve/decode individual entries
		pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
		pbBuffer->StartRead();       // Start reading received phonebook data
		TInt entryCount=0;
		while (pbBuffer->GetTagAndType(tagValue, aDataType)==KErrNone)
			{
			switch (tagValue)
				{
			case RMobilePhoneBookStore::ETagPBAdnIndex:
				entryCount++;
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
				pbBuffer->SkipValue(aDataType);
				break;
				}

			// Print each individual entry
			if((tagValue == RMobilePhoneBookStore::ETagPBNewEntry) && (entryCount>1)) // Only print if all fields retrieved
				{
				INFO_PRINTF4(_L("%d = >%S<  >%S<"), index, &text, &number); // It is not displaying data corectly on the console
				}
			} // end while
		}
	simPhBk.Close();
	delete pbBuffer;

	INFO_PRINTF1(_L("Deleting the entries "));
	clearPhoneBookL(KETelIccAdnPhoneBook());
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
	INFO_PRINTF1(_L(" Test retrieval of a Batch of Phonebook Entries "));
	INFO_PRINTF1(_L("================================================"));
	// First write 15 entries to the phone
	INFO_PRINTF1(_L("Writing 15 entries to the phone"));
	WriteBatchOfEntriesToThePhoneL();

	RMobilePhoneBookStore simPhBk;
	TRequestStatus status;
	TBuf8<KPBDataClientBuf> pbReadData1; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read phonebook data

	TInt ret=simPhBk.Open(iPhone,  KETelIccAdnPhoneBook);
	TEST_CHECKL(ret, KErrNone, _L("*** Failed to open ICC ADN PhoneBook ***"));
	if(ret==KErrNone)
		{
		INFO_PRINTF1(_L("This test can take up to a minute to complete..."));

		simPhBk.GetInfo(status, aPhoneStoreInfoPckgb);
		User::WaitForRequest(status);
		TEST_CHECKL(status.Int(), KErrNone, _L("GT83-TPHBK-003.06 - *** Failed RMobilePhoneBookStore::GetInfo() ***")) ;

		aPhoneStoreInfob = aPhoneStoreInfoPckgb() ;
		TInt batchSize = (aPhoneStoreInfob.iTotalEntries)/2; //50%

		INFO_PRINTF2(_L("Start retrieving a batch of %d phonebook entries with starting index 1"), batchSize);

		simPhBk.Read(status, 1, batchSize, pbReadData1);
		User::WaitForRequest(status);
		CHECKPOINT(status.Int(), KErrNone, _L("GT83-TPHBK-005.06 - *** Failed RMobilePhoneBookStore::Read() ***")) ;

		TUint16 index(0);
		TUint8 tonNpi;
		TPtrC16 number(NULL,0);
		TPtrC16 text(NULL,0);

		TUint8 tagValue(0);
		CPhoneBookBuffer::TPhBkTagType aDataType;

		// pbReadData1 will contain a batch of retrieved entries, so start by parsing through its
		// data to retrieve/decode individual entries
		pbBuffer->Set(&pbReadData1); // Set pbBuffer to point to received phonebook data
		pbBuffer->StartRead();       // Start reading received phonebook data
		TInt entryCount=0;
		while (pbBuffer->GetTagAndType(tagValue, aDataType)==KErrNone)
			{
			switch (tagValue)
				{
			case RMobilePhoneBookStore::ETagPBAdnIndex:
				entryCount++;
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
				pbBuffer->SkipValue(aDataType);
				break;
				}

			// Print each individual entry
			if((tagValue == RMobilePhoneBookStore::ETagPBNewEntry) && (entryCount>1)) // Only print if all fields retrieved
				{
				INFO_PRINTF4(_L("%d = >%S<  >%S<"), index, &text, &number); // It is not displaying data corectly on the console
				}
			} // end while
		}
	simPhBk.Close();
	delete pbBuffer;

	INFO_PRINTF1(_L("Deleting the entries "));
	clearPhoneBookL(KETelIccAdnPhoneBook());
	return TestStepResult();
	}
