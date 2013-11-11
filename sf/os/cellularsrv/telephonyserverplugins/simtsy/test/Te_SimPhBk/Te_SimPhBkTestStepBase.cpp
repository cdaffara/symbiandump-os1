// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimPhBkTestStepBase.h"
#include "Te_SimPhBkConstants.h"

TVerdict CSimPhBkTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}

	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to initialise the phone"));
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	RPhone::TCaps phoneCaps;
	ret = iPhone.GetCaps(phoneCaps) ;
	if ((KErrNone != ret) || !(phoneCaps.iFlags & RPhone::KCapsVoice))
		{
		INFO_PRINTF2(_L("This phone does not support Voice Calls (caps=0x%x)"), phoneCaps.iFlags);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CSimPhBkTestStepBase::doTestStepPostambleL()
	{
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CSimPhBkTestStepBase::CleanUp(TAny *aPtr)
/**
*  Calls iLine.Close() & iCall.Close in  leave case.
* @param this poiter pass cleanup stack when calls the function
*/
	{
	 CSimPhBkTestStepBase* ptr = ((CSimPhBkTestStepBase*)aPtr) ;
	 ptr->iCall.Close() ;
	 ptr->iLine.Close() ;
	}

CSimPhBkTestStepBase::operator TCleanupItem()
/**
* Makes TCleanupItem from current instance of CTestSimPhBkStep
*/
	{
	return TCleanupItem(CSimPhBkTestStepBase::CleanUp,this);
    }

TInt CSimPhBkTestStepBase::ClearPhoneBookL(const TDesC& aPhoneBookName)
/**
 * This function deletes the first 15 entries in the specified Phone Book.
 * It is called by the clearPhoneBooks() function.
 * @test GT83-TPHBK-008.00 Test deleting an individual phonebook entry from a ME-based phonebook of abbreviated dialling numbers.
 * @test GT83-TPHBK-002.06	GT83-AT-070	Test closing an ICC-based phonebook of abbreviated dialling numbers
 */
	{
	TRequestStatus aStatus;
	RMobilePhoneBookStore thePhBk;
	TInt ret=thePhBk.Open(iPhone,aPhoneBookName);
	CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-001.00")) ;

	if (ret!=KErrNone)
		return ret;

	INFO_PRINTF2(_L("Deleting %S phonebook 15 first entries..."), &aPhoneBookName);
	TInt i;
	for (i=1; i<=15; i++)
		{
		thePhBk.Delete(aStatus, i);
		User::WaitForRequest(aStatus);
		if (aStatus!=KErrNone && aStatus!=KErrNotFound)
			{
			//it's checked by "if"
			CHECKPOINT(1, KErrNone, _L("GT83-TPHBK-008.00")) ;
			break;
			}
		}

	thePhBk.Close();
	CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-002.00")) ;
	return ret;
	}

void CSimPhBkTestStepBase::CheckPhoneBookInfoL(RMobilePhoneStore::TMobilePhoneStoreInfoV1& aInfo)
/**
 * This function is called by the testPhoneInfo() function. It checks
 * the validity of the information in the TPhoneBookInfo struct.
 */
	{
	TESTL(aInfo.iTotalEntries>0);

	// A phone book containing the recently dialled calls (RO)
	if (!aInfo.iName.Compare(KETelMeDialledPhoneBook()))
		{
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess); // But we should be able to read
		INFO_PRINTF1(_L("\"DIALLED_CALLS\" tested RO successfully"));
		}

	// A phone book containing missed calls. (RO)
	else if (!aInfo.iName.Compare(KETelMeMissedPhoneBook()))
		{
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"MISSED_CALLS\" tested RO successfully"));
		}

	//A phone book containing received calls (RO)
	else if (!aInfo.iName.Compare(KETelMeReceivedPhoneBook()))
		{
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"RECEIVED_CALLS\" tested RO successfully"));
		}

	// A ME based phone book of abbreviated dialling numbers (RW)
	else if (!aInfo.iName.Compare(KETelMeAdnPhoneBook()))
		{
		iMESupported=ETrue;
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"ME_PHONEBOOK\" tested RW successfully"));
		}

	//A phone book containing voice mailbox numbers (RO)
	else if (!aInfo.iName.Compare(KETelIccVoiceMailBox()))
		{
		iMESupported=ETrue;
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"VOICE_MAILBOX\" tested RO successfully"));
		}

	//A SIM based phone book of abbreviated dialling numbers (RW)
	else if (!aInfo.iName.Compare(KETelIccAdnPhoneBook()))
		{
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"SIM_PHONEBOOK\" tested RW successfully"));
		}


	//A combination of ME and SIM based phone books (RO)
	else if (!aInfo.iName.Compare(KETelCombinedAdnPhoneBook()))
		{
		iMTSupported=ETrue;
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"COMBINED_SIM_ME\" tested RO successfully"));
		}

	//A phone book containing the fixed dialling list(RO)
	else if (!aInfo.iName.Compare(KETelIccFdnPhoneBook()))
		{
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"FIXED_DIAL_NUM\" tested RO successfully"));
		}

	//A phone book containing service dialling number list (RO)
	else if (!aInfo.iName.Compare(KETelIccSdnPhoneBook()))
		{
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"SERV_DIAL_NUM\" tested RO successfully"));
		}

	//A phone book containing the barred dialling numbers (RO)
	else if (!aInfo.iName.Compare(KETelIccBdnPhoneBook()))
		{
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess);
		TESTL(aInfo.iCaps & RMobilePhoneStore::KCapsReadAccess);
		INFO_PRINTF1(_L("\"BARRED_DIAL_NUM\" tested RO successfully"));
		}
	}

void CSimPhBkTestStepBase::WriteBatchOfEntriesToThePhoneL()
/**
 * This function writes 15 entries to the phone.
 * @test GT83-TPHBK-001.06 Test opening an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-002.06 Test closing an ICC-based phonebook of abbreviated dialling numbers
 * @test GT83-TPHBK-007.06 "Test writing a single phonebook entry to a specified location in an ICC-based phonebook containing voice mailbox numbers, where the location is empty"
 */
	{
	TInt ret=KErrNone;
	TRequestStatus aStatus;
	RMobilePhoneBookStore simPhBk;
	TBuf8<KPBDataClientBuf> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer=new(ELeave) CPhoneBookBuffer();

	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	ret=simPhBk.Open(iPhone, KETelIccAdnPhoneBook());
	CHECKPOINT(ret, KErrNone, _L("GT83-TPHBK-001.06")) ;

	// Create array of names and numbers
	const TText16* names[]={(TText16*)KAnnText.iBuf, (TText16*)KMattSText.iBuf, (TText16*)KAndyText.iBuf,
						  (TText16*)KMattMText.iBuf,(TText16*)KPaulText.iBuf, (TText16*)KMumAndDadText.iBuf,
						  (TText16*)KSymbolText.iBuf,(TText16*)KHenrikText.iBuf,(TText16*)KHenrikSVText.iBuf,
						  (TText16*)KMikaelText.iBuf,(TText16*)KIngeText.iBuf,(TText16*)KPatrickText.iBuf,
						  (TText16*)KPontusText.iBuf,(TText16*)KAndersText.iBuf,(TText16*)KHajText.iBuf};

	const TText16* numbers[]={(TText16*)KAnnPhoneNum.iBuf, (TText16*)KMattSPhoneNum.iBuf, (TText16*)KAndyPhoneNum.iBuf,
							(TText16*)KMattMPhoneNum.iBuf, (TText16*)KPaulPhoneNum.iBuf, (TText16*)KMumAndDadPhoneNum.iBuf,
							(TText16*)KSymbolPhoneNum.iBuf,(TText16*)KHenrikPhoneNum.iBuf,(TText16*)KHenrikSVPhoneNum.iBuf,
							(TText16*)KMikaelPhoneNum.iBuf,(TText16*)KIngePhoneNum.iBuf,(TText16*)KPatrickPhoneNum.iBuf,
							(TText16*)KPontusPhoneNum.iBuf,(TText16*)KAndersPhoneNum.iBuf,(TText16*)KHajPhoneNum.iBuf};


	// Write the entries to the SIM Phone Book
	TInt size(KPPBEntriesNum);
	TInt i(0), index(0);

	TBuf<KBufSize> entryNumber(numbers[0]);
	TBuf<KBufSize> entryName(names[0]);

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

		// convert number into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBNumber, entryNumber);
		TESTL(ret == KErrNone) ;

		// convert type into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBTonNpi, (TUint8)RMobilePhone::EUnknownNumber);
		TESTL(ret == KErrNone) ;

		// convert text into TLV format and append it to allocated buffer
		ret=pbBuffer->PutTagAndValue((TUint8)RMobilePhoneBookStore::ETagPBText, entryName);
		TESTL(ret == KErrNone) ;

		index=RMobilePhoneStore::TMobilePhoneStoreEntryV1::KIndexNotUsed;
		// Note that KIndexNotUsed cannot be used anymore to write to next available phonebook slot
		simPhBk.Write(aStatus, pbData, index);
		User::WaitForRequest(aStatus);

		if (aStatus!=KErrNone) // First write request should fail
			{
			// This time client needs to specify the phonebook slot
			index=i;
			simPhBk.Write(aStatus, pbData, index);
			User::WaitForRequest(aStatus);
			CHECKPOINT(aStatus.Int(), KErrNone, _L("GT83-TPHBK-007.06")) ;
			}
		} // end for

	simPhBk.Close();
	delete pbBuffer;
	}
