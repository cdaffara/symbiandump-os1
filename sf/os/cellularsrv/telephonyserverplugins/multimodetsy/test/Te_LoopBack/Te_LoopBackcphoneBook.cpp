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
// Phase 1 - variable name change only - still compiles
// 
//

/**
 @file
*/

#include <e32test.h>
#include <etelmm.h>
#include <mpbutil.h>	
#include "Te_LoopBackcphoneBook.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

// Entries to write to the iPhone book
_LIT(KMuddyWatersText,"MuddyW");
_LIT(KArethaFText,"ArethaF");
_LIT(KMuddyWatersPhoneNum,"02220111");
_LIT(KArethaFPhoneNum,"03334567");


//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDrivePhoneBook* CTestDrivePhoneBook::NewL(const TScriptList aScriptListEntry)
	{
	CTestDrivePhoneBook* phBk=new(ELeave) CTestDrivePhoneBook(aScriptListEntry);
	CleanupStack::PushL(phBk);
	phBk->ConstructL();
	CleanupStack::Pop();
	return phBk;
	}

CTestDrivePhoneBook::CTestDrivePhoneBook(const TScriptList aScriptListEntry) : 
	iMESupported(EFalse), iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDrivePhoneBook::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDrivePhoneBook::DriveETelApiL()
//
// This function contains the real meat of the Client-side test code
//
	{
 	_LIT(KMmPhoneName,"GsmPhone1");
	
	// Open and Initialise the iPhone
 	TESTL(iMmPhone.Open(iServer,KMmPhoneName)==KErrNone);

	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iMmPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

 	ERR_PRINTF1(_L("Phone Id, Available Phone books and Get Info"));
 	GetPhoneInfoL();

 	ERR_PRINTF1(_L("Get PhoneBook Capabilities and Information"));
  	testGetPhBkCapsAndInfoL(); 

 	ERR_PRINTF1(_L("Deleting the 1st 5 entries in the SM and ME (if supported) phonebooks"));
 	TInt ret;
 	ret=testClearPhBkL(KETelIccAdnPhoneBook);
 	TESTL(ret==KErrNone);
 	if (iMESupported)
		{
 		ret=testClearPhBkL(KETelMeAdnPhoneBook);
 		TESTL(ret==KErrNone);
		}

 	ERR_PRINTF1(_L("Phone book Read and Write"));
 	testPhoneBookReadAndWrite(); // New method using CPhoneBookBuffer

 	ERR_PRINTF1(_L("Advanced Phone book tests"));
	testAdvancedPhoneBook();  // New method using CPhoneBookBuffer

	ERR_PRINTF1(_L("Phone book Notifications"));
 	testPhoneBookNotifications(); // New method using CPhoneBookBuffer

	ERR_PRINTF1(_L("Phone book Read Cancel Requests"));
	testPhoneBookReadCancel(); // New method using CPhoneBookBuffer

	ERR_PRINTF1(_L("Phone book Read & Write International Numbers"));
	testPhoneBookReadAndWriteInternationalL();

	ERR_PRINTF1(_L("Phone book Multithread Write"));
	testPhoneBookMultiWriteL();

	iMmPhone.Close();
	return KErrNone;
	}


void CTestDrivePhoneBook::GetPhoneInfoL()
/**
 * Retrieve iPhone Information
 */
	{

	TRequestStatus stat;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iMmPhone.GetPhoneId(stat,phoneId);
	User::WaitForRequest(stat);
	TESTL(stat == KErrNone);

	// check version id ?
	INFO_PRINTF2(_L("Extension ID : %d"), phoneId.ExtensionId());
	INFO_PRINTF2(_L("\nManufacturer ID : %S"), &phoneId.iManufacturer);
	INFO_PRINTF2(_L("Model ID : %S"), &phoneId.iModel);
	INFO_PRINTF2(_L("Revision ID : %S"), &phoneId.iRevision);
	INFO_PRINTF2(_L("Serial number : %S\n"), &phoneId.iSerialNumber);

	// common mode
	TInt numAvail=0;  // counts number of phonebooks
	IsPhoneBookAvailable (KETelMeAdnPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelMeDialledPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelMeMissedPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelMeReceivedPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelCombinedAdnPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelIccAdnPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelIccFdnPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelIccSdnPhoneBook, numAvail);
	// gsm mode
	IsPhoneBookAvailable (KETelTaAdnPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelIccBdnPhoneBook, numAvail);
	IsPhoneBookAvailable (KETelIccVoiceMailBox, numAvail);
	INFO_PRINTF2(_L("%d Phonebooks available.\n"), numAvail);

}

TRequestStatus CTestDrivePhoneBook::IsPhoneBookAvailable(const TDesC& aPhoneBook,
                                                         TInt &aCount)
	{
	TRequestStatus stat;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPkg(info);

	iMmPhone.GetPhoneStoreInfo(stat,infoPkg,aPhoneBook);
	User::WaitForRequest(stat);
	if (stat == KErrNone)	// available
		{
		INFO_PRINTF2(_L("%S Phone Book is available.\n"), &aPhoneBook);
		aCount++;
		}
	else
		{
		INFO_PRINTF3(_L("%S Phone Book is NOT available. Status %d.\n"), &aPhoneBook, stat.Int());
		}

	return stat;
	}

void CTestDrivePhoneBook::testGetPhBkCapsAndInfoL()
/**
 * This function:
 *		Gets the SIM and ME iPhone books static capabilities,
 *		Information associated with the iPhone books,
 *		and also Validates the obtained information.
 */
	{
	// SIM iPhone book - mandatory
	TInt ret=KErrNone;
	RMobilePhoneBookStore simPhBk;

	ret=simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook);
	TESTL(ret==KErrNone);

	// Get SM Phone book Capabilities

	TRequestStatus stat=0;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPkg(info);

	simPhBk.GetInfo(stat,infoPkg);
	User::WaitForRequest(stat);
	TESTL(stat == KErrNone);
	info = infoPkg();
	
	// Need to Read entries 

	TESTL((info.iCaps & RMobilePhoneStore::KCapsWriteAccess)!=0);
	TESTL(info.iName == KETelIccAdnPhoneBook);
	TESTL(info.iType == RMobilePhoneStore::EPhoneBookStore); 
	TESTL(info.iTotalEntries > 0);

	simPhBk.Close();

	
	RMobilePhoneBookStore mePhBk;
	ret=mePhBk.Open(iMmPhone,KETelMeAdnPhoneBook);
	
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("ME phonebook supported\n"));
		iMESupported=ETrue;

		mePhBk.GetInfo(stat,infoPkg);
		User::WaitForRequest(stat);
		TESTL(stat == KErrNone);
		info = infoPkg();
		TESTL((info.iCaps & RMobilePhoneStore::KCapsWriteAccess)!=0);
		
		// Need to read entries
		TESTL(info.iName == KETelMeAdnPhoneBook);
		TESTL(info.iType == RMobilePhoneStore::EPhoneBookStore);
		TESTL(info.iTotalEntries > 0);

		// GetInfo Cancel Request
		INFO_PRINTF1(_L("GetInfo Cancel Request..."));
		mePhBk.GetInfo(stat,infoPkg);
		mePhBk.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
		User::WaitForRequest(stat);
		INFO_PRINTF2(_L("GetInfo Cancel Status = %d"), stat.Int());
		TESTL(stat==KErrNone || stat==KErrCancel);
	
		mePhBk.Close();
		}
	else 
		INFO_PRINTF1(_L("ME phonebook is NOT supported"));
	}

TInt CTestDrivePhoneBook::testClearPhBkL(const TDesC& aPhoneBookName)
/**
 * This function deletes the first 5 entries in the SM and ME (if supported) phonebooks.
 */
	{	
	TInt ret=KErrNone;

	RMobilePhoneBookStore thePhBk;
	TRequestStatus stat;

	// Open the SM iPhone book
	ret=thePhBk.Open(iMmPhone,aPhoneBookName);
	TESTL(ret==KErrNone);

	INFO_PRINTF2(_L("Deleting %S phonebook entries..."), &aPhoneBookName);
	TInt i;
	for (i=1; i<=5; i++)
		{
	
		thePhBk.Delete(stat,i);
		User::WaitForRequest(stat);

		if (stat!=KErrNone && stat!=KErrNotFound)
			break;
		}

// Check that KErrNotFound is returned when reading an empty phonebook.
	TBuf8<50> buf;
	thePhBk.Read(stat,1,99,buf);
	User::WaitForRequest(stat);
	TESTL(stat==KErrNotFound);

	thePhBk.Close();
	return ret;
	}

void CTestDrivePhoneBook::ReadAndWriteL()
/**
 * This function writes a few entries to the SM phonebook and then reads them back.
 */
	{
	TInt ret=KErrNone;
	RMobilePhoneBookStore simPhBk;

	// Open the SM iPhone book
	ret=simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook);
	TESTL(ret==KErrNone);
	simPhBk.Close();
	}

void CTestDrivePhoneBook::testAdvancedPhBkL() 
/**
 * This function opens an Advanced GSM ME PhoneBook, writes a few entries to it, and then
 * deletes these entries.
 */
	{
	TInt ret=KErrNone;
	RMobilePhoneBookStore mePhBk;
	ret=mePhBk.Open(iMmPhone,KETelMeAdnPhoneBook);
	TESTL(ret==KErrNone);

	mePhBk.Close();
	}

 void CTestDrivePhoneBook::testPhBkNotificationsL()
/**
 * This function opens the ME phonebook and posts a request for notification when an 
 * entry is modified.
 */
	{
	RMobilePhoneBookStore mePhBk;
	const TInt Pause=2000000;		// 2 second pause
	TRequestStatus writeStatus = KErrNone;

	TESTL(mePhBk.Open(iMmPhone,KETelMeAdnPhoneBook)==KErrNone);

	TRequestStatus status;
	TInt index=KErrNone;
	TUint32 eventChange;

	// Post a notification request
	//mePhBk.EntryChangedNotification(status,index);
	mePhBk.NotifyStoreEvent(status,eventChange,index);

	if (writeStatus==KErrCancel)
		{
		INFO_PRINTF1(_L("This is unexpected!!!. If the test carries on, this means that\n"));
		INFO_PRINTF1(_L("the Write request has been transmitted to the iPhone regardless of\n"));		
		INFO_PRINTF1(_L("of the apparent success of the Write cancel request.\n\n"));
		}
	else
		TESTL(writeStatus==KErrNone);	// The Cancel request was not successful

	// Completion of the notification request posted earlier
	User::WaitForRequest(status);
	TESTL(status==KErrNone);
	TESTL(index==1);

	// Post another notification request
	mePhBk.NotifyStoreEvent(status,eventChange,index);
	
	// Delete and DeleteCancel Request. Wait 2 seconds before posting the Cancel request.
	// Enables the request to get through to the TSY (simplifies the script).
	INFO_PRINTF1(_L("Delete Cancel Request..."));
	TRequestStatus deleteStatus;
	mePhBk.Delete(deleteStatus,index);
	User::After(Pause);		// Wait 2s, give the request time to get through to the TSY.
	mePhBk.CancelAsyncRequest(EMobilePhoneStoreDelete);
	User::WaitForRequest(deleteStatus);
	INFO_PRINTF2(_L("Delete Cancel Status = %d"), deleteStatus.Int());

	if (deleteStatus==KErrCancel)
		{
		INFO_PRINTF1(_L("This is unexpected!!!. If the test carries on, this means that"));
		INFO_PRINTF1(_L("the Delete request has been transmitted to the iPhone regardless"));
		INFO_PRINTF1(_L("of the apparent success of the Delete cancel request."));
		}
	else
		{
		TESTL(deleteStatus==KErrNone);	// The Cancel request was not successful
		}

	// Completion of the 2nd notification request posted, (dependent on the Delete request above)
	User::WaitForRequest(status);
	TESTL(status==KErrNone);
	TESTL(index==1);

	// EntryChangedNotificationCancel Request
	mePhBk.NotifyStoreEvent(status,eventChange,index);
	mePhBk.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	User::WaitForRequest(status);

	TESTL(status==KErrCancel || status==KErrNone);
	INFO_PRINTF2(_L("EntryChangedNotificationCancel Status = %d"), status.Int());

	mePhBk.Close();
	}		


void CTestDrivePhoneBook::testPhBkReadCancelL()
/**
 * This function writes a few entries to the SM phonebook, and then does some Read Requests
 * which are cancelled.
 */
	{
	TInt ret=KErrNone;
	RMobilePhoneBookStore simPhBk;

	// Open the SM iPhone book
	ret=simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook);
	TESTL(ret==KErrNone);

	// Close the iPhone book
	simPhBk.Close();
	}



//****************************************************************************
//						NEW Methods based on CPhoneBookBuffer class
//****************************************************************************

// testing the PhoneBook ReadAndWrite

void CTestDrivePhoneBook::testPhoneBookReadAndWrite()
	{
	TRAP_IGNORE(DotestPhoneBookReadAndWriteL());		// trap but ignore error
	}

void CTestDrivePhoneBook::DotestPhoneBookReadAndWriteL()
	{
	TInt ret=KErrNone;
	TRequestStatus aStatus;
	RMobilePhoneBookStore simPhBk;

	TBuf8<(RMobileSmsMessaging::KGsmTpduSize*3)> pbData, pbReadData1;
	TInt requestedIndex(0), numOfEntries(1);

	TESTL(simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook()) == KErrNone);

	const TText16* names[]={(TText16*)KMuddyWatersText.iBuf, (TText16*)KArethaFText.iBuf};
	const TText16* numbers[]={(TText16*)KMuddyWatersPhoneNum.iBuf, (TText16*)KArethaFPhoneNum.iBuf};

	const TInt KBufSize=20;

	TBuf16<KBufSize> entryNumber(numbers[0]);
	TBuf16<KBufSize> entryName(names[0]);

	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	// Write the entries to the SIM Phone Book
	TInt writeIndex(0);
	TUint i(0), size(2); // Only entries are written

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
 		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)129/****RMobilePhone::EUnknownNumber***/);
 		TESTL(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, entryName);
		TESTL(ret == KErrNone) ;

		simPhBk.Write(aStatus,pbData,writeIndex);
		User::WaitForRequest(aStatus);
		TESTL(aStatus==KErrNone);
		}

		/* Read the entries back */
		TPtrC16 number(NULL,0);
		TPtrC16 text(NULL,0);

		TUint8 aTagValue(0);
		CPhoneBookBuffer::TPhBkTagType aDataType;
		TPtrC16 field(NULL,0);
		TInt counter(0);

		/* Reads Forwards and to read all entries individually */
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
			counter = 0;
			aTagValue = 0;

			requestedIndex = i;
			simPhBk.Read(aStatus, requestedIndex, numOfEntries, pbReadData1);
			User::WaitForRequest(aStatus);
			TESTL(aStatus.Int()== KErrNone) ;

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

		 
				case CPhoneBookBuffer::EPhBkTypeDes8:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 1; // add desc length + length-field + tag-field
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

			TBuf<128> pName;
			TBuf<128> pNumber;

			pName = text;
			pNumber = number;

			INFO_PRINTF2(_L("Reading \'%S\'"),&pName);
			INFO_PRINTF2(_L("Reading \'%S\'\n"),&pNumber);

			   
		} // end for

		simPhBk.Close();
		delete pbBuffer;
}


// testing Advanced Phone book Read Write

void CTestDrivePhoneBook::testAdvancedPhoneBook() 
	{
	TRAP_IGNORE(DotestAdvancedPhoneBookL());		// trap but ignore error
	}

void CTestDrivePhoneBook::DotestAdvancedPhoneBookL() 
	{
	/**
	 * This function opens an Advanced GSM ME PhoneBook, writes a few entries to it, and then
	 * deletes these entries.
	 */
	const TInt KFillTo=2;

	TInt ret=KErrNone;
	TRequestStatus aStatus;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 aPhoneStoreInfob;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg aPhoneStoreInfoPckgb(aPhoneStoreInfob);
	RMobilePhoneBookStore simPhBk;

	TBuf8<(RMobileSmsMessaging::KGsmTpduSize*3)> pbData; // Client reserved space for phonebook data

	_LIT(KTel,"0123456789%02d");
	_LIT(KText,"Tel %02d");
	TBuf<15> text(KTel), number(KText);
	TInt index(1);

	TESTL(simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook()) == KErrNone);
	

	// Write the entries
	INFO_PRINTF1(_L("\nFill the 1st 30 locations in the SIM phonebook..."));
	simPhBk.GetInfo(aStatus, aPhoneStoreInfoPckgb);
	User::WaitForRequest(aStatus);
	TESTL(aStatus.Int()== KErrNone) ;
	aPhoneStoreInfob = aPhoneStoreInfoPckgb();
	TESTL(aPhoneStoreInfob.iTotalEntries>0);
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
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, number);
		TESTL(ret == KErrNone) ;

		// convert number type into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)129);
		TESTL(ret == KErrNone) ;

		text.Format(KText(), index);
		INFO_PRINTF2(_L("Writing \'%S\'\n"),&text);
		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, text);
		TESTL(ret == KErrNone) ;

		simPhBk.Write(aStatus, pbData,index);
		User::WaitForRequest(aStatus);
		TESTL(aStatus.Int()== KErrNone);
		used++;
		index++;
		} // end while

	simPhBk.Close();
	delete pbBuffer;

	// Delete the entries //
	INFO_PRINTF1(_L("Re-opening phonebook...")) ;
	
	TESTL(simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook()) == KErrNone);
	
	simPhBk.GetInfo(aStatus, aPhoneStoreInfoPckgb);
	User::WaitForRequest(aStatus);
	TESTL(aStatus.Int()== KErrNone);
	aPhoneStoreInfob = aPhoneStoreInfoPckgb();
	INFO_PRINTF1(_L("Deleting the entries...")); 
	for (int iy=1;iy<=KFillTo;++iy)
		{
		INFO_PRINTF2(_L("Deleting %03d\n"),iy);
		simPhBk.Delete(aStatus, iy);
		User::WaitForRequest(aStatus);
		TESTL(aStatus.Int()== KErrNone );
		}

	simPhBk.Close();

	}



// testing PhoneBook Notifications 

void CTestDrivePhoneBook::testPhoneBookNotifications()
	{
	TRAP_IGNORE(DotestPhoneBookNotificationsL());		// trap but ignore erorr
	}

void CTestDrivePhoneBook::DotestPhoneBookNotificationsL()
	{

	/**
	 * This function opens the ME phonebook and posts a request for notification when an 
	 * entry is modified.
	 */
	RMobilePhoneBookStore simPhBk;//, combPhBk;
	TBuf8<(RMobileSmsMessaging::KGsmTpduSize*3)> pbData; // Client reserved space for phonebook data
	TInt index(0), ret(0);

	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer(); // used to read/write phonebook entries
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	TESTL(simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook)==KErrNone);

	TRequestStatus aStatus, status1;//, status2;
	TInt index1 = 0;
	TUint32 eventToReturn;
	simPhBk.NotifyStoreEvent(status1, eventToReturn,index1);	// Will Notify if an entry is added

	index = 1; // write to the first slot
	// convert number into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KMuddyWatersPhoneNum);
	TESTL(ret == KErrNone) ;

	// convert number type into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi,(TUint8)129);
	TESTL(ret == KErrNone) ;

	// convert text into TLV format and append it to allocated buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KMuddyWatersText);
	TESTL(ret == KErrNone) ;

	simPhBk.Write(aStatus,pbData,index);
	User::WaitForRequest(aStatus); // Write request
	TESTL(aStatus.Int()== KErrNone) ;

	User::WaitForRequest(status1); // Notification
	TEST(status1==KErrNotSupported);
	if (status1==KErrNotSupported)
		{
		// Notification is not supported by phonebook store - as expected
		INFO_PRINTF1(_L("Phonebook Store Write Notification is NOT Supported - as expected\n"));
		}
	else
		{
		// This should not happen since this notification is not supported by phonebook store
		INFO_PRINTF1(_L("Phonebook Store Write Notification IS Supported - not expected\n"));
		}


	status1= KErrNone; // reset status variable
	simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);

	// Now delete that entry
	simPhBk.Delete(aStatus, 1);
	User::WaitForRequest(aStatus);
	TESTL(aStatus.Int()== KErrNone);

	User::WaitForRequest(status1);
	TEST(status1==KErrNotSupported);
	if (status1==KErrNotSupported)
		{
		// Notification is not supported by phonebook store - as expected
		INFO_PRINTF1(_L("Phonebook Store Delete Notification is NOT Supported - as expected\n"));
		}
	else
		{
		// This should not happen since this notification is not supported by phonebook store
		INFO_PRINTF1(_L("Phonebook Store Delete Notification IS Supported - not expected\n"));
		}

	
	// EntryChangedNotificationCancel Request
	status1= KErrNone; // reset status variable
	simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);
	simPhBk.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	User::WaitForRequest(status1);

	TEST(status1==KErrNotSupported);
	if (status1==KErrNotSupported)
		{
		// Notification is not supported by phonebook store - as expected
		INFO_PRINTF1(_L("Phonebook Store Notification is NOT Supported - as expected\n"));
		}
	else
		{
		// This should not happen since this notification is not supported by phonebook store
		INFO_PRINTF1(_L("Phonebook Store Notification cancelled - not expected\n"));
		}

	// Cancel Notification Request after 2 seconds
	status1= KErrNone; // reset status variable
	simPhBk.NotifyStoreEvent(status1, eventToReturn, index1);

	// Now write the same entry again
	simPhBk.Write(aStatus,pbData, index);
	User::WaitForRequest(aStatus);
	TESTL(aStatus.Int()==KErrNone) ;

	User::After(2000000L);
	simPhBk.CancelAsyncRequest(EMobilePhoneStoreNotifyStoreEvent);
	User::WaitForRequest(status1);

	INFO_PRINTF2(_L("Cancel Status = %d\n"), status1.Int());

	TESTL(status1==KErrCancel || status1==KErrNotSupported || status1==KErrNone);

	simPhBk.Delete(aStatus, 1); // Deleting the last added entry so that the test ends "clean".
	User::WaitForRequest(aStatus);

	simPhBk.Close();

	delete pbBuffer;

	}



// Testing Phonebook Read cancel

void CTestDrivePhoneBook::testPhoneBookReadCancel()
	{
	TRAP_IGNORE(DotestPhoneBookReadCancelL());		// trap but ignore error
	}

void CTestDrivePhoneBook::DotestPhoneBookReadCancelL()
	{

	/**
	 * This function writes a few entries to the SM phonebook, and then does some Read Requests
	 * which are cancelled.
	 */

	TInt ret=KErrNone;
	TRequestStatus aStatus;
	RMobilePhoneBookStore simPhBk;

	TBuf8<(RMobileSmsMessaging::KGsmTpduSize*3)> pbData, pbReadData1;//, pbReadData2, pbReadEmpty; // Client reserved space for phonebook data
	TInt requestedIndex(0), numOfEntries(1);

	TESTL(simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook()) == KErrNone);

	const TText16* names[]={(TText16*)KMuddyWatersText.iBuf, (TText16*)KArethaFText.iBuf};
	const TText16* numbers[]={(TText16*)KMuddyWatersPhoneNum.iBuf, (TText16*)KArethaFPhoneNum.iBuf};


	const TInt KBufSize=20;

	TBuf16<KBufSize> entryNumber(numbers[0]);
	TBuf16<KBufSize> entryName(names[0]);

	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	// Write the entries to the SIM Phone Book
	TInt writeIndex(0);
	TUint i(0), size(2);

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
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)129);
		TESTL(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, entryName);
		TESTL(ret == KErrNone) ;

		simPhBk.Write(aStatus,pbData,writeIndex);
		User::WaitForRequest(aStatus);
		TESTL(aStatus==KErrNone);
		}

		/* Read the entries back */
		TPtrC16 number(NULL,0);
		TPtrC16 text(NULL,0);

		TUint8 aTagValue(0);
		CPhoneBookBuffer::TPhBkTagType aDataType;
		TPtrC16 field(NULL,0);
		TInt counter(0);

		/* Reads 1 entry */
		
		//first reset the following three local buffers to delete any existing data
		entryNumber.FillZ();
		entryNumber.Zero();
		entryName.FillZ();
		entryName.Zero();
		pbData.FillZ();
		pbData.Zero();

		// reset all loop variables
		counter = 0;
		aTagValue = 0;


		requestedIndex = 1;
		simPhBk.Read(aStatus, requestedIndex, numOfEntries, pbReadData1);
 
		// Cancel the Read
		simPhBk.CancelAsyncRequest(EMobilePhoneBookStoreRead);
		User::WaitForRequest(aStatus);
		INFO_PRINTF2(_L("Read First Entry Cancel Status = %d\n\n"), aStatus.Int());

		// If the request was successfully cancelled, then do read the first entry, 'want 
		// the phonebook pointer to point to a valid entry for the ReadNext() operation.
		TESTL(aStatus == KErrCancel || aStatus == KErrNone);
		if (aStatus==KErrCancel)
			{
			INFO_PRINTF1(_L("Read First Entry Cancel Successful. Read First Entry Again.\n"));
			simPhBk.Read(aStatus, requestedIndex, 1, pbReadData1);
			User::WaitForRequest(aStatus);
			TESTL(aStatus==KErrNone);
			}

			// reset all loop variables
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
 
			 	case CPhoneBookBuffer::EPhBkTypeDes8:
					{
					pbBuffer->GetValue(field);
					counter = counter + field.Length() + 1; // add desc length + length-field + tag-field
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


		TBuf<128> pName;
		TBuf<128> pNumber;

		pName = text;
		pNumber = number;


		INFO_PRINTF2(_L("Reading  \'%S\'"),&pName);
		INFO_PRINTF2(_L("Reading \'%S\'\n"),&pNumber);
		
	simPhBk.Close();
	delete pbBuffer;
	}

void CTestDrivePhoneBook::testPhoneBookReadAndWriteInternationalL()
	{
	TInt ret=KErrNone;
	TRequestStatus status;
	RMobilePhoneBookStore simPhBk;

	TBuf8<200> writeBuf,readBuf;
	_LIT(KInterNum1,"441234");
	_LIT(KInterName1,"ABCD");
	_LIT(KInterNum2,"445678");
	_LIT(KInterName2,"EFGH");

	TESTL(simPhBk.Open(iMmPhone,KETelIccAdnPhoneBook()) == KErrNone);

	/* Write entries */
	CPhoneBookBuffer* pbBuffer=NULL;
	TRAP_IGNORE(pbBuffer = new(ELeave) CPhoneBookBuffer());		// TRAP but ignore error
	TESTL(pbBuffer!=NULL);

	pbBuffer->Set(&writeBuf); // Set it to point to the Client buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KInterNum1);
	TESTL(ret == KErrNone) ;
 	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)145);
 	TESTL(ret == KErrNone) ;
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KInterName1);
	TESTL(ret == KErrNone) ;

	TInt writeIndex=1;
	simPhBk.Write(status,writeBuf,writeIndex);
	User::WaitForRequest(status);
	TESTL(status==KErrNone);

	pbBuffer->Set(&writeBuf); // Set it to point to the Client buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, KInterNum2);
	TESTL(ret == KErrNone) ;
 	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)145);
 	TESTL(ret == KErrNone) ;
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, KInterName2);
	TESTL(ret == KErrNone) ;

	writeIndex=2;
	simPhBk.Write(status,writeBuf,writeIndex);
	User::WaitForRequest(status);
	TESTL(status==KErrNone);

	/* Read the entries back */
	simPhBk.Read(status, 1, 2, readBuf);
	User::WaitForRequest(status);
	TESTL(status.Int()==KErrNone);

	TInt index=0;
	pbBuffer->Set(&readBuf); // Set pbBuffer to point to received phonebook data
	pbBuffer->StartRead();       // Start reading received phonebook data
	for(;;)
		{
		TUint8 tagValue;
		CPhoneBookBuffer::TPhBkTagType dataType;
		ret=pbBuffer->GetTagAndType(tagValue,dataType);
		if(ret!=KErrNone)
			break;
		switch(tagValue)
			{
		case RMobilePhoneBookStore::ETagPBNewEntry:
			index++;
			break;

		case RMobilePhoneBookStore::ETagPBAdnIndex:
				{
				TUint16 val;
				pbBuffer->GetValue(val);
				TESTL(val==index);
				}
			break;

		case RMobilePhoneBookStore::ETagPBText:
				{
				TPtrC text;
				pbBuffer->GetValue(text);
				if(index==1)
				{
					TESTL(text==KInterName1);
				}
				else
				{
					TESTL(text==KInterName2);
				}
				}
			break;

		case RMobilePhoneBookStore::ETagPBNumber:
				{
				TPtrC number;
				pbBuffer->GetValue(number);
				if(index==1)
				{
					TESTL(number==KInterNum1);
				}
				else
				{
					TESTL(number==KInterNum2);
				}
				}
			break;

		case RMobilePhoneBookStore::ETagPBTonNpi:
				{
				TUint8 ton;
				pbBuffer->GetValue(ton);
				TESTL(ton==145);
				}
			break;

		default:
			TESTL(KErrGeneral);
			break;
			}
		}

	simPhBk.Close();
	delete pbBuffer;
	}

void CTestDrivePhoneBook::testPhoneBookMultiWriteL()
	{
	_LIT(KThreadName,"MultiPhBkWriteTest%d");
	const TInt KStackSize=0x4000;
	const TInt KHeapSize=0x4000;
	const TInt KMaxHeapSize=0x8000;
	const TInt KNumThreads=2;

	RThread thread[KNumThreads];
	TRequestStatus status[KNumThreads];
	TInt ret=KErrNone;

	TInt i=0;
	for(i=0;i<KNumThreads;i++)
		{
		TName name;
		name.Format(KThreadName,i);
		ret=thread[i].Create(name,CTestDrivePhoneBook::ThreadFnWritePhoneBookEntry,KStackSize,KHeapSize,KMaxHeapSize,NULL);
		if(ret!=KErrNone)
			break;
		}

	if(ret!=KErrNone)
		{
		for(i=0;i<KNumThreads;i++)
			thread[i].Close();
		TESTL(ret==KErrNone);
		return;
		}

	for(i=0;i<KNumThreads;i++)
		thread[i].Logon(status[i]);

	for(i=0;i<KNumThreads;i++)
		thread[i].Resume();

	for(i=0;i<KNumThreads;i++)
		{
		User::WaitForRequest(status[i]);
		TESTL(status[i]==KErrNone);
		}
	}

TInt CTestDrivePhoneBook::ThreadFnWritePhoneBookEntry(TAny*)
	{
	_LIT(KNum1,"12345678");
	_LIT(KName1,"ABCDEFGH");
	_LIT(KPhoneName,"GsmPhone1");

	CTrapCleanup* cleanup=CTrapCleanup::New();
	if(cleanup==NULL)
		return KErrNoMemory;

	RTelServer telServer;
	TInt ret=telServer.Connect();
	if(ret!=KErrNone)
		return ret;

	RMobilePhone mmPhone;
	ret=mmPhone.Open(telServer,KPhoneName);
	if(ret!=KErrNone)
		return ret;

	RMobilePhoneBookStore simPhBk;
	ret=simPhBk.Open(mmPhone,KETelIccAdnPhoneBook());
	if(ret!=KErrNone)
		return ret;

	CPhoneBookBuffer* pbBuffer = new CPhoneBookBuffer();
	if(pbBuffer==NULL)
		return KErrNoMemory;
	TBuf8<200> writeBuf;

	pbBuffer->Set(&writeBuf); // Set it to point to the Client buffer
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber,KNum1);
	if(ret!=KErrNone)
		return ret;
 	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi,(TUint8)129);
	if(ret!=KErrNone)
		return ret;
	ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText,KName1);
	if(ret!=KErrNone)
		return ret;

	TInt writeIndex=1;
	TRequestStatus status;
	simPhBk.Write(status,writeBuf,writeIndex);
	User::WaitForRequest(status);
	if(status!=KErrNone)
		return status.Int();

	delete pbBuffer;
	delete cleanup;
	simPhBk.Close();
	mmPhone.Close();
	telServer.Close();
	return KErrNone;
	}

//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestPhoneBook* CTestPhoneBook::NewL(const TScript* aScript)
	{
	CTestPhoneBook* phBk=new(ELeave) CTestPhoneBook(aScript);
	CleanupStack::PushL(phBk);
	phBk->ConstructL();
	CleanupStack::Pop();
	return phBk;
	}

CTestPhoneBook::CTestPhoneBook(const TScript* aScript) : iScript(aScript)
	{}

void CTestPhoneBook::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestPhoneBook::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestPhoneBook::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestPhoneBook::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
