// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cphonebookdispatcherTraces.h"
#endif

#include "cphonebookdispatcher.h"

#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>


#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"

#include <ctsy/serviceapi/cmmgsmphonestorageutility.h> //for CStorageInfoData

#include <ctsy/ltsy/cphonebookentry.h>

#include <mpbutil.h>
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>

CPhonebookDispatcher::CPhonebookDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot),
		iPhonebookCachePtrs(KPhonebookTypes),
		iPhonebookReadPtrs(KPhonebookTypes),
		iInitialisePhonebook(DispatcherPhonebook::EUnknown)
	{
	} // CPhonebookDispatcher::CPhonebookDispatcher

	  
CPhonebookDispatcher::~CPhonebookDispatcher()
	{
	iPhonebookCachePtrs.Reset();
	iPhonebookReadPtrs.Reset();
	if(iSmsListArray)
		{
		iSmsListArray->ResetAndDestroy();
		}
	delete iSmsListArray;
	} // CPhonebookDispatcher::~CPhonebookDispatcher


CPhonebookDispatcher* CPhonebookDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CPhonebookDispatcher* self =
		new (ELeave) CPhonebookDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CPhonebookDispatcher::NewLC


CPhonebookDispatcher* CPhonebookDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CPhonebookDispatcher* self =
		CPhonebookDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CPhonebookDispatcher::NewL


void CPhonebookDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	iSmsListArray = new ( ELeave ) CArrayPtrFlat< TSmsMsg >( 1 );
	// Get the Licensee LTSY interfaces related to Phonebook functionality
	// from the factory
	
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId))
		{
		TAny* storeReadEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId,
		       	storeReadEntryInterface);
		iLtsyDispatchPhonebookStoreReadEntry =
				static_cast<MLtsyDispatchPhonebookStoreReadEntry*>(storeReadEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookStoreReadEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId))
		{
		TAny* storeDeleteEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId,
		       	storeDeleteEntryInterface);
		iLtsyDispatchPhonebookStoreDeleteEntry =
				static_cast<MLtsyDispatchPhonebookStoreDeleteEntry*>(storeDeleteEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookStoreDeleteEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId))
		{
		TAny* storeCacheInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId,
		       	storeCacheInterface);
		iLtsyDispatchPhonebookStoreCache =
				static_cast<MLtsyDispatchPhonebookStoreCache*>(storeCacheInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookStoreCache, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId))
		{
		TAny* storeGetPhonebookInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId,
		       	storeGetPhonebookInfoInterface);
		iLtsyDispatchPhonebookStoreGetPhonebookInfo =
				static_cast<MLtsyDispatchPhonebookStoreGetPhonebookInfo*>(storeGetPhonebookInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookStoreGetPhonebookInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId))
		{
		TAny* storeInitialiseInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId,
		       	storeInitialiseInterface);
		iLtsyDispatchPhonebookStoreInitialise =
				static_cast<MLtsyDispatchPhonebookStoreInitialise*>(storeInitialiseInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookStoreInitialise, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId))
		{
		TAny* storeDeleteAllInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId,
		       	storeDeleteAllInterface);
		iLtsyDispatchPhonebookStoreDeleteAll =
				static_cast<MLtsyDispatchPhonebookStoreDeleteAll*>(storeDeleteAllInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookStoreDeleteAll, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId))
		{
		TAny* smsStoreReadAllInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId,
		       	smsStoreReadAllInterface);
		iLtsyDispatchPhonebookSmsStoreReadAll =
				static_cast<MLtsyDispatchPhonebookSmsStoreReadAll*>(smsStoreReadAllInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookSmsStoreReadAll, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId))
		{
		TAny* storeWriteEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId,
		       	storeWriteEntryInterface);
		iLtsyDispatchPhonebookStoreWriteEntry =
				static_cast<MLtsyDispatchPhonebookStoreWriteEntry*>(storeWriteEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookStoreWriteEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId))
		{
		TAny* getPhoneStoreInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId,
		       	getPhoneStoreInfoInterface);
		iLtsyDispatchPhonebookGetPhoneStoreInfo =
				static_cast<MLtsyDispatchPhonebookGetPhoneStoreInfo*>(getPhoneStoreInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookGetPhoneStoreInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId))
		{
		TAny* smsStoreDeleteEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId,
		       	smsStoreDeleteEntryInterface);
		iLtsyDispatchPhonebookSmsStoreDeleteEntry =
				static_cast<MLtsyDispatchPhonebookSmsStoreDeleteEntry*>(smsStoreDeleteEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookSmsStoreDeleteEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId))
		{
		TAny* smsStoreDeleteAllInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId,
		       	smsStoreDeleteAllInterface);
		iLtsyDispatchPhonebookSmsStoreDeleteAll =
				static_cast<MLtsyDispatchPhonebookSmsStoreDeleteAll*>(smsStoreDeleteAllInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookSmsStoreDeleteAll, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId))
		{
		TAny* smsStoreGetInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId,
		       	smsStoreGetInfoInterface);
		iLtsyDispatchPhonebookSmsStoreGetInfo =
				static_cast<MLtsyDispatchPhonebookSmsStoreGetInfo*>(smsStoreGetInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookSmsStoreGetInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId))
		{
		TAny* smsStoreReadEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId,
		       	smsStoreReadEntryInterface);
		iLtsyDispatchPhonebookSmsStoreReadEntry =
				static_cast<MLtsyDispatchPhonebookSmsStoreReadEntry*>(smsStoreReadEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookSmsStoreReadEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId))
		{
		TAny* smsStoreWriteEntryInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId,
		       	smsStoreWriteEntryInterface);
		iLtsyDispatchPhonebookSmsStoreWriteEntry =
				static_cast<MLtsyDispatchPhonebookSmsStoreWriteEntry*>(smsStoreWriteEntryInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookSmsStoreWriteEntry, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId))
		{
		TAny* storeCacheCancelInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,
		       	storeCacheCancelInterface);
		iLtsyDispatchPhonebookStoreCacheCancel =
				static_cast<MLtsyDispatchPhonebookStoreCacheCancel*>(storeCacheCancelInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhonebookStoreCacheCancel, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	for(TInt i = 0; i < KPhonebookTypes; ++i)
		{
		iPhonebookCachePtrs.AppendL(NULL); //really only Fdn and Adn pointers will be stored with the current CTSY
		iPhonebookReadPtrs.AppendL(NULL);
		}

	} // CPhonebookDispatcher::ConstructL

CPhoneBookStoreEntry* CPhonebookDispatcher::CreatePhoneBookStoreEntryLC(const CPhoneBookEntry& aPhoneBookEntry) const
/**
 * Creates a CTSY CPhoneBookStoreEntry from a dispatcher CPhoneBookEntry, the created
 * entry is left on the cleanup stack.
 * 
 * @param aPhoneBookEntry The entry to create a CPhoneBookStoreEntry from.
 * @return A pointer to the created CTSY entry.
 */
	{
	CPhoneBookStoreEntry* ctsyPhoneBookStoreEntry = new (ELeave) CPhoneBookStoreEntry();
	CleanupStack::PushL(ctsyPhoneBookStoreEntry);
	ctsyPhoneBookStoreEntry->ConstructL();
	
	//the record number
	ctsyPhoneBookStoreEntry->iLocation = aPhoneBookEntry.GetIndex();

	//allocate the buffer and copy the name
	ctsyPhoneBookStoreEntry->iText = HBufC::NewL(aPhoneBookEntry.GetFirstName().Length());
	*(ctsyPhoneBookStoreEntry->iText) = aPhoneBookEntry.GetFirstName();

	//allocate the buffer and copy the dialling number
	ctsyPhoneBookStoreEntry->iNumber = HBufC::NewL(aPhoneBookEntry.GetDiallingNumber().Length());
	*(ctsyPhoneBookStoreEntry->iNumber) = aPhoneBookEntry.GetDiallingNumber();

	TInt i = 0;
	//add the email address's
	const TInt KEmailAddresssCount = aPhoneBookEntry.GetEmailAddresss().Count();
	for (i = 0; i < KEmailAddresssCount; ++i)
		{
		ctsyPhoneBookStoreEntry->iEmail->AppendL(aPhoneBookEntry.GetEmailAddresss()[i]);
		}
	
	//add the second names
	const TInt KSecondNamesCount = aPhoneBookEntry.GetSecondNames().Count();
	for (i = 0; i < KSecondNamesCount; ++i)
		{
		ctsyPhoneBookStoreEntry->iSne->AppendL(aPhoneBookEntry.GetSecondNames()[i]);
		}
	
	//add the additional numbers
	const TInt KAdditionalNumbersCount = aPhoneBookEntry.GetAdditionalNumbers().Count();
	for (i = 0; i < KAdditionalNumbersCount; ++i)
		{
		ctsyPhoneBookStoreEntry->iAnr->AppendL(aPhoneBookEntry.GetAdditionalNumbers()[i]);
		}
	
	return ctsyPhoneBookStoreEntry;
	}

TInt CPhonebookDispatcher::FillPhoneBookEntry(const CPhoneBookStoreEntry& aPhoneBookStoreEntry, CPhoneBookEntry& aPhoneBookEntry) const
/**
 * Fills a dispatcher CPhoneBookEntry from a CTSY CPhoneBookStoreEntry.
 * 
 * @param aPhoneBookStoreEntry The CTSY phonebook entry to read from.
 * @param aPhoneBookEntry The dispatcher entry to be written to.
 * @return Error code, KErrNone is successful.
 */
	{
	TInt error = KErrNone;
	
	aPhoneBookEntry.SetIndex(aPhoneBookStoreEntry.iLocation);
	if(aPhoneBookStoreEntry.iText && (error == KErrNone))
		{
		error = aPhoneBookEntry.SetFirstName(*aPhoneBookStoreEntry.iText);
		}
	
	if(aPhoneBookStoreEntry.iNumber && (error == KErrNone))
		{
		error = aPhoneBookEntry.SetDiallingNumber(*aPhoneBookStoreEntry.iNumber);
		}
	
	if(aPhoneBookStoreEntry.iEmail && (error == KErrNone))
		{
		const TInt KEmailCount = aPhoneBookStoreEntry.iEmail->MdcaCount();
		
		for(TInt i = 0; i < KEmailCount && (error == KErrNone); ++i)
			{
			error = aPhoneBookEntry.AddEmailAddress(aPhoneBookStoreEntry.iEmail->MdcaPoint(i));
			}
		}
	
	if(aPhoneBookStoreEntry.iSne && (error == KErrNone))
		{
		const TInt KSecondNameCount = aPhoneBookStoreEntry.iSne->MdcaCount();
		
		for(TInt i = 0; i < KSecondNameCount && (error == KErrNone); ++i)
			{
			error = aPhoneBookEntry.AddSecondName(aPhoneBookStoreEntry.iSne->MdcaPoint(i));
			}
		}
	
	if(aPhoneBookStoreEntry.iAnr && (error == KErrNone))
		{
		const TInt KAdditionalNumberCount = aPhoneBookStoreEntry.iAnr->MdcaCount();
		
		for(TInt i = 0; i < KAdditionalNumberCount && (error == KErrNone); ++i)
			{
			error = aPhoneBookEntry.AddAdditionalNumber(aPhoneBookStoreEntry.iAnr->MdcaPoint(i));
			}
		}
	
	return error;
	}

void CPhonebookDispatcher::FillCtsyPhoneBookStoreL(const TDesC8& aPhonebookEntries, 
													CArrayPtrSeg<CPhoneBookStoreEntry>& aCtsyPhoneBook)
/**
 * Fills the CTSY Store from the phonebook entries contained in a descriptor TLV.
 *  
 * 
 * @param aPhonebookEntries The TLV entries.
 * @param aCtsyPhoneBook The CTSY phonebook to fill.
 */
	{
	//we need to create a copy of aPhoneBookEntry as the CPhoneBookBuffer can only be set with a TDes
	RBuf8 readBuffer;
	CleanupClosePushL(readBuffer);
	readBuffer.CreateL(aPhonebookEntries);
	
	CPhoneBookBuffer* phoneBookBuffer = new (ELeave) CPhoneBookBuffer();
	CleanupStack::PushL(phoneBookBuffer);
	phoneBookBuffer->Set(&readBuffer);
	phoneBookBuffer->StartRead();
	
	CPhoneBookEntry* phoneBookEntry = CPhoneBookEntry::NewLC();

	TBool entryRead = EFalse;

	while(phoneBookBuffer->RemainingReadLength() > 0)
		{
		User::LeaveIfError(phoneBookEntry->InternalizeFromTlvEntry(*phoneBookBuffer,entryRead));
		
		CPhoneBookStoreEntry* phoneBookStoreEntry = CreatePhoneBookStoreEntryLC(*phoneBookEntry);
		aCtsyPhoneBook.AppendL(phoneBookStoreEntry); //takes ownership
		CleanupStack::Pop(phoneBookStoreEntry);

		phoneBookEntry->Reset();
		entryRead = ETrue;
		}
	
	CleanupStack::PopAndDestroy(phoneBookEntry);
	CleanupStack::PopAndDestroy(phoneBookBuffer);
	CleanupStack::PopAndDestroy(&readBuffer);
	}

const TDesC& CPhonebookDispatcher::PhonebookName(DispatcherPhonebook::TPhonebook aPhonebookType) const
/**
 * Converts phonebook enum type to a phonebook name descriptor.
 * 
 * @param The phonebook type.
 * @return A TDeC reference containing the associated ETel name for the phonebook.
 */
	{
	switch(aPhonebookType)
		{
		case DispatcherPhonebook::EIccAdn:
			return KETelIccAdnPhoneBook;
		case DispatcherPhonebook::EIccBdn:
			return KETelIccBdnPhoneBook;
		case DispatcherPhonebook::EIccSdn:
			return KETelIccSdnPhoneBook;
		case DispatcherPhonebook::EIccFdn:
			return KETelIccFdnPhoneBook;
		case DispatcherPhonebook::EIccVmb:
			return KETelIccVoiceMailBox;
		case DispatcherPhonebook::EIccMbdn:
			return KETelIccMbdnPhoneBook;
		case DispatcherPhonebook::EUnknown:
		default:
			return KNullDesC;
		}
	}

DispatcherPhonebook::TPhonebook CPhonebookDispatcher::Phonebook(const TDesC& aPhonebookName) const
/**
 * Converts a phonebook name descriptor into an enum type.
 * 
 * @param aPhonebookName The phonebook name.
 * @return The phonebook enum type.
 */
	{
	DispatcherPhonebook::TPhonebook phonebook = DispatcherPhonebook::EUnknown;
	
	if(aPhonebookName == KETelIccFdnPhoneBook)
		{
		phonebook = DispatcherPhonebook::EIccFdn;
		}
	else if(aPhonebookName == KETelIccAdnPhoneBook)
		{
		phonebook = DispatcherPhonebook::EIccAdn;
		}
	else if(aPhonebookName == KETelIccSdnPhoneBook)
		{
		phonebook = DispatcherPhonebook::EIccSdn;
		}
	else if(aPhonebookName == KETelIccBdnPhoneBook)
		{
		phonebook = DispatcherPhonebook::EIccBdn;
		}
	else if(aPhonebookName == KETelIccVoiceMailBox)
		{
		phonebook = DispatcherPhonebook::EIccVmb;
		}
	else if(aPhonebookName == KETelIccMbdnPhoneBook)
		{
		phonebook = DispatcherPhonebook::EIccMbdn;
		}
	
	return phonebook;
	}

void CPhonebookDispatcher::PhonebookStoreToStorageInfo(const DispatcherPhonebook::TPhonebookStoreInfoV1& aPhonebookStoreInfo, CStorageInfoData& aStorageInfoData) const
/**
 * Write data into a CTSY CStorageInfoData structure from a TPhonebookStoreInfoV1 dispatcher structure.
 * 
 * @param aPhonebookStoreData The dispatcher structure to read from.
 * @param aStorageInfoData The CTSY structure to write to.
 */
{
	aStorageInfoData.iADNNumOfEntries 			= aPhonebookStoreInfo.iAdnTotalEntries;
	aStorageInfoData.iADNTextLengthMax 			= aPhonebookStoreInfo.iAdnMaximumTextLength;
	aStorageInfoData.iADNNumberLengthMax 		= aPhonebookStoreInfo.iAdnMaximumNumberLength;

	aStorageInfoData.iSNENumOfEntriesPerEntry 	= aPhonebookStoreInfo.iSecondNameMaximumFieldsPerEntry;
	aStorageInfoData.iSNETextLengthMax			= aPhonebookStoreInfo.iSecondNameMaximumLength;
	
	aStorageInfoData.iEmailNumOfEntriesPerEntry = aPhonebookStoreInfo.iEmailMaximumFieldsPerEntry;
	aStorageInfoData.iEmailTextLengthMax 		= aPhonebookStoreInfo.iEmailMaximumLength;

	aStorageInfoData.iFDNNumOfEntries 			= aPhonebookStoreInfo.iFdnTotalEntries;
	aStorageInfoData.iFDNTextLengthMax 			= aPhonebookStoreInfo.iFdnMaximumTextLength;
	aStorageInfoData.iFDNNumberLengthMax 		= aPhonebookStoreInfo.iFdnMaximumNumberLength;

	aStorageInfoData.iSDNNumOfEntries			= aPhonebookStoreInfo.iSdnTotalEntries;
	aStorageInfoData.iSDNTextLengthMax 			= aPhonebookStoreInfo.iSdnMaximumTextLength;
	aStorageInfoData.iSDNNumberLengthMax 		= aPhonebookStoreInfo.iSdnMaximumNumberLength;

	aStorageInfoData.iVMBXNumOfEntries 			= aPhonebookStoreInfo.iVmbTotalEntries;
	aStorageInfoData.iVMBXTextLengthMax 		= aPhonebookStoreInfo.iVmbMaximumTextLength;
	aStorageInfoData.iVMBXNumberLengthMax 		= aPhonebookStoreInfo.iVmbMaximumNumberLength;
	aStorageInfoData.iVMBXCaps 					= aPhonebookStoreInfo.iVmbCapabilities;
  
	aStorageInfoData.iANRNumOfEntriesPerEntry 	= aPhonebookStoreInfo.iAdditionalNumberMaximumFieldsPerEntry;
	aStorageInfoData.iANRNumLengthMax 			= aPhonebookStoreInfo.iAdditionalNumberMaximumLength;
	
	aStorageInfoData.iMBDNNumOfEntries 			= aPhonebookStoreInfo.iMbdnTotalEntries;
	aStorageInfoData.iMBDNTextLengthMax 		= aPhonebookStoreInfo.iMbdnMaximumTextLength;
	aStorageInfoData.iMBDNNumberLengthMax 		= aPhonebookStoreInfo.iMbdnMaximumNumberLength;
	aStorageInfoData.iMBDNCaps 					= aPhonebookStoreInfo.iMbdnCapabilities;
	}

void CPhonebookDispatcher::FillSmsStoreListL(const DispatcherPhonebook::TSmsData& aSmsData, TInt aIndex, CArrayPtrFlat< TSmsMsg >& aSmsListArray)
/*
 * Append a TSmsMsg to a sms array.
 * 
 * @param aSmsData A entry of TSmSData where the information will be extracted from to form a TSmsMsg entry.
 * @param aIndex The index of the entry.
 * @param aSmsListArray The array where the TSmsMsg entry will be append to.
 */
	{
	TSmsMsg* smsMsg = new (ELeave) TSmsMsg();
	CleanupStack::PushL(smsMsg);
	
	smsMsg->iSmsMsg.Copy(aSmsData.iSmsMsg); 
	smsMsg->iServiceCentre.Copy(aSmsData.iServiceCentre); 
	smsMsg->iMobileScTON   = aSmsData.iMobileScTON;  
	smsMsg->iMobileScNPI   = aSmsData.iMobileScNPI;
	smsMsg->iMessageStatus = aSmsData.iMessageStatus;
	smsMsg->iLocation = aIndex;
	aSmsListArray.AppendL(smsMsg, 1); // aSmsListArray has taken the ownership of the object
	
	CleanupStack::Pop(smsMsg);

	}

void CPhonebookDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CPhonebookDispatcher::SetDispatcherHolder

TInt CPhonebookDispatcher::DispatchStoreReadEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyPhoneBookStoreReadIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookStoreReadEntry)
		{
		const CPhoneBookDataPackage& phoneBookDataPackage = *static_cast<const CPhoneBookDataPackage*>(aDataPackage);
		
		TName phonebookName(KNullDesC);
		phoneBookDataPackage.GetPhoneBookName(phonebookName);
		__ASSERT_DEBUG(phonebookName != KNullDesC, CtsyDispatcherPanic(EInvalidParameter));
		
		RMobilePhoneBookStore::TPBIndexAndNumEntries* indexAndEntries = NULL;
		
		phoneBookDataPackage.UnPackData(indexAndEntries);
		
		__ASSERT_DEBUG(indexAndEntries, CtsyDispatcherPanic(EInvalidNullPtr));
		
		const DispatcherPhonebook::TPhonebook phonebookType = Phonebook(phonebookName);
		__ASSERT_DEBUG(phonebookType != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
		

		
		ret = iLtsyDispatchPhonebookStoreReadEntry->HandleStoreReadEntryReqL(phonebookType,indexAndEntries->iIndex,indexAndEntries->iNumSlots);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchStoreReadEntryL

TInt CPhonebookDispatcher::DispatchStoreDeleteEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyPhoneBookStoreDeleteIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookStoreDeleteEntry)
		{
		const CPhoneBookDataPackage& phoneBookDataPackage = *static_cast<const CPhoneBookDataPackage*>(aDataPackage);
		
		TName phonebookName(KNullDesC);
		phoneBookDataPackage.GetPhoneBookName(phonebookName);
		__ASSERT_DEBUG(phonebookName != KNullDesC, CtsyDispatcherPanic(EInvalidParameter));
		__ASSERT_DEBUG(Phonebook(phonebookName) != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
		
		TInt deleteEntryIndex;
		phoneBookDataPackage.UnPackData(deleteEntryIndex);
			
		ret = iLtsyDispatchPhonebookStoreDeleteEntry->HandleStoreDeleteEntryReqL(Phonebook(phonebookName),deleteEntryIndex);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchStoreDeleteEntryL

TInt CPhonebookDispatcher::DispatchStoreCacheL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyPhoneBookStoreCacheIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPhonebookStoreCache)
		{
		const CPhoneBookDataPackage& phoneBookDataPackage = *static_cast<const CPhoneBookDataPackage*>(aDataPackage);
		
		TName phonebookName(KNullDesC);
		phoneBookDataPackage.GetPhoneBookName(phonebookName);
		__ASSERT_DEBUG(phonebookName != KNullDesC, CtsyDispatcherPanic(EInvalidParameter));
		__ASSERT_DEBUG(Phonebook(phonebookName) != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
		
		const DispatcherPhonebook::TPhonebook phonebookType = Phonebook(phonebookName);
		__ASSERT_DEBUG(phonebookType != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
	
		// Call Handle... method in Licensee LTSY
		ret = iLtsyDispatchPhonebookStoreCache->HandleStoreCacheReqL(Phonebook(phonebookName));
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchStoreCacheL

TInt CPhonebookDispatcher::DispatchStoreGetInfoL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyPhoneBookStoreGetInfoIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookStoreGetPhonebookInfo)
		{
		const CPhoneBookDataPackage& phoneBookDataPackage = *static_cast<const CPhoneBookDataPackage*>(aDataPackage);

		TName phonebookName;
		phoneBookDataPackage.GetPhoneBookName(phonebookName);
		__ASSERT_DEBUG(phonebookName != KNullDesC, CtsyDispatcherPanic(EInvalidParameter));
		__ASSERT_DEBUG(Phonebook(phonebookName) != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
		
		ret = iLtsyDispatchPhonebookStoreGetPhonebookInfo->HandleStoreGetPhonebookInfoReqL(Phonebook(phonebookName));
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchStoreGetInfoL

TInt CPhonebookDispatcher::DispatchStoreInitialiseL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyPhoneBookStoreInitIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookStoreInitialise)
		{
		const CPhoneBookDataPackage& phoneBookDataPackage = *static_cast<const CPhoneBookDataPackage*>(aDataPackage);

		__ASSERT_DEBUG(iInitialisePhonebook == DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));

		TName phonebookName;
		phoneBookDataPackage.GetPhoneBookName(phonebookName);
		
		iInitialisePhonebook = Phonebook(phonebookName);
		
		__ASSERT_DEBUG(iInitialisePhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
		
		ret = iLtsyDispatchPhonebookStoreInitialise->HandleStoreInitialiseReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchStoreInitialiseL

TInt CPhonebookDispatcher::DispatchStoreDeleteAllL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyPhoneBookStoreDeleteAllIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookStoreDeleteAll)
		{
		const CPhoneBookDataPackage& phoneBookDataPackage = *static_cast<const CPhoneBookDataPackage*>(aDataPackage);

		TName phonebookName(KNullDesC);
		phoneBookDataPackage.GetPhoneBookName(phonebookName);
		__ASSERT_DEBUG(phonebookName != KNullDesC, CtsyDispatcherPanic(EInvalidParameter));
		__ASSERT_DEBUG(Phonebook(phonebookName) != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
		
		ret = iLtsyDispatchPhonebookStoreDeleteAll->HandleStoreDeleteAllReqL(Phonebook(phonebookName));
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchStoreDeleteAllL

TInt CPhonebookDispatcher::DispatchSmsStoreReadAllL()
/**
 * Pass request for EMobilePhoneStoreReadAllPhase1 on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookSmsStoreReadAll)
		{
		ret = iLtsyDispatchPhonebookSmsStoreReadAll->HandleSmsStoreReadAllReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchSmsStoreReadAllL

TInt CPhonebookDispatcher::DispatchStoreWriteEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyPhoneBookStoreWriteIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookStoreWriteEntry)
		{
		const CPhoneBookDataPackage& phoneBookDataPackage = *static_cast<const CPhoneBookDataPackage*>(aDataPackage);

		TName phonebookName(KNullDesC);
		phoneBookDataPackage.GetPhoneBookName(phonebookName);
		__ASSERT_DEBUG(phonebookName != KNullDesC, CtsyDispatcherPanic(EInvalidParameter));
		__ASSERT_DEBUG(Phonebook(phonebookName) != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));

		TInt index;
		CPhoneBookStoreEntry* ctsyPhonebookEntry = NULL;
		
		phoneBookDataPackage.UnPackData(index,ctsyPhonebookEntry);
		__ASSERT_DEBUG(ctsyPhonebookEntry, CtsyDispatcherPanic(EInvalidNullPtr));
		
		//this will be used to convert the CPhoneBookStoreEntry to a TDesC8 containing a TLV
		CPhoneBookEntry* dispatchPhoneBookEntry = CPhoneBookEntry::NewLC();
		
		//convert CPhoneBookStoreEntry to a CPhoneBookEntry
		User::LeaveIfError(FillPhoneBookEntry(*ctsyPhonebookEntry,*dispatchPhoneBookEntry));
		ctsyPhonebookEntry = NULL; //don't need this anymore (CTSY owned)
		dispatchPhoneBookEntry->SetIndex(index);
		
		//the buffer for the TLV
		RBuf8 tlvEntry;
		CleanupClosePushL(tlvEntry);
		tlvEntry.CreateL(dispatchPhoneBookEntry->TlvLength());
		
		CPhoneBookBuffer* phoneBookBuffer = new (ELeave) CPhoneBookBuffer();
		CleanupStack::PushL(phoneBookBuffer);
		phoneBookBuffer->Set(&tlvEntry);
		//create the TLV
		User::LeaveIfError(dispatchPhoneBookEntry->ExternalizeToTlvEntry(*phoneBookBuffer));

		CleanupStack::PopAndDestroy(phoneBookBuffer);
				
		ret = iLtsyDispatchPhonebookStoreWriteEntry->HandleStoreWriteEntryReqL(Phonebook(phonebookName),tlvEntry);
		
		CleanupStack::PopAndDestroy(&tlvEntry);
		CleanupStack::PopAndDestroy(dispatchPhoneBookEntry);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchStoreWriteEntryL

TInt CPhonebookDispatcher::DispatchGetPhoneStoreInfoL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneGetPhoneStoreInfo
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookGetPhoneStoreInfo)
		{
		//The CTSY treats it as a RMobilePhone::TMobileName but really ETel sends it as a TName
		RMobilePhone::TMobileName* storeNamePtr = NULL; 
		aDataPackage->UnPackData(&storeNamePtr);
		__ASSERT_DEBUG(storeNamePtr, CtsyDispatcherPanic(EInvalidNullPtr));

		ret = iLtsyDispatchPhonebookGetPhoneStoreInfo->HandleGetPhoneStoreInfoReqL(*storeNamePtr);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchGetPhoneStoreInfoL

TInt CPhonebookDispatcher::DispatchSmsStoreDeleteEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneStoreDelete
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Unpack data for this request inside the if statement if required, 
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY

	TInt index;
	aDataPackage->UnPackData(index);
	if (iLtsyDispatchPhonebookSmsStoreDeleteEntry)
		{
		ret = iLtsyDispatchPhonebookSmsStoreDeleteEntry->HandleSmsStoreDeleteEntryReqL(index);
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchSmsStoreDeleteEntryL

TInt CPhonebookDispatcher::DispatchSmsStoreDeleteAllL()
/**
 * Pass request for EMobilePhoneStoreDeleteAll on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookSmsStoreDeleteAll)
		{
		ret = iLtsyDispatchPhonebookSmsStoreDeleteAll->HandleSmsStoreDeleteAllReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchSmsStoreDeleteAllL

TInt CPhonebookDispatcher::DispatchSmsStoreGetInfoL()
/**
 * Pass request for EMobilePhoneStoreGetInfo on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call handle method for EMobilePhoneStoreGetInfo in Licensee LTSY
	if (iLtsyDispatchPhonebookSmsStoreGetInfo)
		{
		ret = iLtsyDispatchPhonebookSmsStoreGetInfo->HandleSmsStoreGetInfoReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchSmsStoreGetInfoL

TInt CPhonebookDispatcher::DispatchSmsStoreReadEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneStoreRead
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Unpack data for this request inside the if statement if required, 
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	TInt index;
	aDataPackage->UnPackData(index);
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookSmsStoreReadEntry)
		{
		ret = iLtsyDispatchPhonebookSmsStoreReadEntry->HandleSmsStoreReadEntryReqL(index);
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchSmsStoreReadEntryL

TInt CPhonebookDispatcher::DispatchSmsStoreWriteEntryL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneStoreWrite
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	 __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
 
	TDesC8** data = NULL;
	TInt* index = NULL;
	
	aDataPackage->UnPackData(&data,&index);

	if(!data || !index)
		{
		ret = KErrCorrupt;
		}
	else
		{
		RMobileSmsStore::TMobileGsmSmsEntryV1& mobileGsmSmsEntry = (static_cast<RMobileSmsStore::TMobileGsmSmsEntryV1Pckg&>(**data))();
		
		//the CTSY packs index which is also set in the Gsm Sms Entry, this index is not passed though the interface
		//as a client can examine it from the MobileGsmSmsEntry, we assert here in case the CTSY sets one but not
		//the other
		__ASSERT_DEBUG(*index == mobileGsmSmsEntry.iIndex,CtsyDispatcherPanic(ESlotIndexesDontMatch));
		if (*index != mobileGsmSmsEntry.iIndex)
			{
			//for urel build
			ret = KErrArgument;
			}
		// Call Handle... method in Licensee LTSY
		else if (iLtsyDispatchPhonebookSmsStoreWriteEntry)
			{
			ret = iLtsyDispatchPhonebookSmsStoreWriteEntry->HandleSmsStoreWriteEntryReqL(mobileGsmSmsEntry);
			}
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchSmsStoreWriteEntryL

TInt CPhonebookDispatcher::DispatchStoreCacheCancelL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsyPhoneBookStoreCacheCancelIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhonebookStoreCacheCancel)
		{
		const CPhoneBookDataPackage& phoneBookDataPackage = *static_cast<const CPhoneBookDataPackage*>(aDataPackage);

		TName phonebookName(KNullDesC);
		phoneBookDataPackage.GetPhoneBookName(phonebookName);
		__ASSERT_DEBUG(phonebookName != KNullDesC, CtsyDispatcherPanic(EInvalidParameter));
		__ASSERT_DEBUG(Phonebook(phonebookName) != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
		
		ret = iLtsyDispatchPhonebookStoreCacheCancel->HandleStoreCacheCancelReqL(Phonebook(phonebookName));
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhonebookDispatcher::DispatchStoreCacheCancelL


//
// Callback handlers follow
//



void CPhonebookDispatcher::CallbackStoreResetCache(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreResetCacheInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook to reset the cache for.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPhonebook=%d"), aError, aPhonebook);

	__ASSERT_DEBUG(aPhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
	
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(PhonebookName(aPhonebook));

	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreResetCacheIPC, &phoneBookDataPackage, aError);
	
	} // CPhonebookDispatcher::CallbackStoreResetCache

void CPhonebookDispatcher::CallbackStoreSetFdnPhonebookInfoInd(TInt aError, TInt aTotalEntries, TInt aMaxTextLength, TInt aMaxNumberLength)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreSetFdnPhonebookInfoInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aTotalEntries The total number of entries in the Fdn phonebook.
 * @param aMaxTextLength The maximum text length in the Fdn phonebook.
 * @param aMaxNumberLength The maximum number length in the Fdn phonebook.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aTotalEntries=%d, aMaxTextLength=%d, aMaxNumberLength=%d"), 
							aError, aTotalEntries, aMaxTextLength, aMaxNumberLength);

	TName phoneBookName(KETelIccFdnPhoneBook);
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(phoneBookName);
	
	TPBFdnInfo pbFdnInfo;
	pbFdnInfo.iFDNNumOfEntries 		= aTotalEntries;
	pbFdnInfo.iFDNNumberLengthMax 	= aMaxNumberLength;
	pbFdnInfo.iFDNTextLengthMax 	= aMaxTextLength;

	phoneBookDataPackage.PackData(&pbFdnInfo);

	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreFdnInfoIPC, &phoneBookDataPackage, aError);
	
	} // CPhonebookDispatcher::CallbackStoreSetFdnPhonebookInfoInd

void CPhonebookDispatcher::CallbackStoreReadEntry(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aPhonebookEntries)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreReadEntryComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook of which the read request was made and which the enties are for.
 * @param aPhonebookEntries The read entries from the phonebook encoded in a TLV, this can either be encoded directly 
 * 	via a CPhoneBookBuffer() or via the CPhoneBookEntry::ExternalizeToTlvEntry() utility.
 *
 * @see RMobilePhoneBookStore::Read()
 * @see CPhoneBookBuffer()
 * @see CPhoneBookEntry::ExternalizeToTlvEntry()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPhonebook=%d"), aError, aPhonebook);
	
	__ASSERT_DEBUG(aPhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));	
	
	if (iPhonebookReadPtrs[aPhonebook] != NULL)
		{
		iPhonebookReadPtrs[aPhonebook] = NULL;
		}
	
	if((aError == KErrNone) && (aPhonebook != DispatcherPhonebook::EUnknown))
		{
		//fill the CTSY pointer
        TRAP ( aError, 
                CArrayPtrSeg<CPhoneBookStoreEntry>* readEntries = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>(1);
                iPhonebookReadPtrs[aPhonebook] = readEntries;
                FillCtsyPhoneBookStoreL(aPhonebookEntries,*(iPhonebookReadPtrs[aPhonebook])));
		}
	
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(PhonebookName(aPhonebook));
	phoneBookDataPackage.PackData(&(iPhonebookReadPtrs[aPhonebook]));
	
	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreReadIPC, &phoneBookDataPackage, aError);
		
	} // CPhonebookDispatcher::CallbackStoreReadEntry

void CPhonebookDispatcher::CallbackStoreDeleteEntry(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aMaxNumberLength)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteEntryComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook of which the delete request was made.
 * @param aMaxNumberLength The new maximum number length possible in the phonebook, this field is
 * only relevant to Fdn, Adn and Vmb phonebooks.
 *
 * @see RMobilePhoneBookStore::Delete()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPhonebook=%d, aMaxNumberLength=%d"), aError, aPhonebook, aMaxNumberLength);

	__ASSERT_DEBUG(aPhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
	
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(PhonebookName(aPhonebook));
	
	TPBEntryInfo pbEntryInfo;
	pbEntryInfo.iLocation = 0; //not used the CTSY
	pbEntryInfo.iMaxNumLength = aMaxNumberLength;
	
	phoneBookDataPackage.PackData(&pbEntryInfo);
	
	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreDeleteIPC, &phoneBookDataPackage, aError);
	
	} // CPhonebookDispatcher::CallbackStoreDeleteEntry

void CPhonebookDispatcher::CallbackStoreCache(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aPhonebookEntries)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreCacheComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook of which the cache entries are for (and the request was made from).
 * @param aPhonebookEntries All entries from the phonebook encoded in a TLV, this can either be encoded directly 
 * 	via a CPhoneBookBuffer() or via the CPhoneBookEntry::ExternalizeToTlvEntry() utility.
 *
 * @see CPhoneBookBuffer()
 * @see CPhoneBookEntry::ExternalizeToTlvEntry()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPhonebook=%d"), aError, aPhonebook);
	
	__ASSERT_DEBUG(aPhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
	
	if (iPhonebookCachePtrs[aPhonebook] != NULL)
		{
		iPhonebookCachePtrs[aPhonebook] = NULL;
		}
	
	if((aError == KErrNone) && (aPhonebook != DispatcherPhonebook::EUnknown)/* && (iPhonebookCachePtrs[aPhonebook])*/)
		{
        TRAP(aError,
                CArrayPtrSeg<CPhoneBookStoreEntry>* readEntries = new(ELeave) CArrayPtrSeg<CPhoneBookStoreEntry>(1);
                iPhonebookCachePtrs[aPhonebook] = readEntries;
                FillCtsyPhoneBookStoreL(aPhonebookEntries, *(iPhonebookCachePtrs[aPhonebook])));
		
		if(aError)
			{
			iPhonebookCachePtrs[aPhonebook]->ResetAndDestroy();
			}
		}
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(PhonebookName(aPhonebook));
	phoneBookDataPackage.PackData(&(iPhonebookCachePtrs[aPhonebook]));

	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreCacheIPC, &phoneBookDataPackage, aError);
	} // CPhonebookDispatcher::CallbackStoreCache

void CPhonebookDispatcher::CallbackStoreGetPhonebookInfo(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aUsedEntries)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreGetPhonebookInfoComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook name of which the phonebook information is for.
 * @param aUsedEntries The number of used entries in the phonebook.
 *
 * @see RMobilePhoneBookStore::GetInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPhonebook=%d, aUsedEntries=%d"), aError, aPhonebook, aUsedEntries);

	__ASSERT_DEBUG(aPhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
	
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(PhonebookName(aPhonebook));
	
	phoneBookDataPackage.PackData(&aUsedEntries);

	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreGetInfoIPC, &phoneBookDataPackage, aError);
	
	} // CPhonebookDispatcher::CallbackStoreGetInfo

void CPhonebookDispatcher::CallbackStoreInitialise(TInt aError, const DispatcherPhonebook::TPhonebookStoreInfoV1& aPhonebookStoreInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreInitialiseComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebookStoreInfo The information relating to the whole phonebook store.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	__ASSERT_DEBUG(iInitialisePhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
	
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(PhonebookName(iInitialisePhonebook));
	iInitialisePhonebook = DispatcherPhonebook::EUnknown;
		
	//CTSY C-Type class really a T-Type class so on the stack
	CStorageInfoData storageInfoData;
	
	//convert the TPhonebookStoreInfoV1 to the CTSY CStorageInfoData
	PhonebookStoreToStorageInfo(aPhonebookStoreInfo,storageInfoData);
	
	phoneBookDataPackage.PackData(&storageInfoData);
	
	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreInitIPC, &phoneBookDataPackage, aError);
	
	} // CPhonebookDispatcher::CallbackStoreInitialise

void CPhonebookDispatcher::CallbackStoreDeleteAll(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aMaxNumberLength)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteAllComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhonebook The phonebook name for which the delete all request was made.
 * @param aMaxNumberLength The new maximum number length possible in the phonebook, this field is
 * only relevant to Fdn, Adn and Vmb phonebooks.
 *
 * @see RMobilePhoneBookStore::DeleteAll()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPhonebook=%d, aMaxNumberLength=%d"), aError, aPhonebook, aMaxNumberLength);

	__ASSERT_DEBUG(aPhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
	
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(PhonebookName(aPhonebook));
	
	TPBEntryInfo pbEntryInfo;
	pbEntryInfo.iLocation = 0; //not used in the CTSY
	pbEntryInfo.iMaxNumLength = aMaxNumberLength;
	
	phoneBookDataPackage.PackData(&pbEntryInfo);
	
	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreDeleteAllIPC, &phoneBookDataPackage, aError);
	} // CPhonebookDispatcher::CallbackStoreDeleteAll

void CPhonebookDispatcher::CallbackSmsStoreReadAll(TInt aError, const DispatcherPhonebook::TSmsData& aSmsData, TInt aIndex, TBool aMoreToCome, TBool aReceivedClass2ToBeResent)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadAllComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSmsData SMS data. 
 * @param aIndex index of the SMS entry. 
 * @param aMoreToCome ETrue if there will be other SMS entries, EFalse otherwise.   
 * @param aReceivedClass2ToBeResent ETrue if received class is to be resent, else EFalse.
 * 
 * @see CRetrieveMobilePhoneSmsList::Start()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndex=%d"), aError, aIndex);
	TInt errorCode = aError;

	if (errorCode == KErrNone)
		{
		TRAP(errorCode, FillSmsStoreListL(aSmsData, aIndex, *iSmsListArray));
		}

	if (!aMoreToCome || errorCode != KErrNone)
		{
		CMmDataPackage dataPackage;
		dataPackage.PackData(&iSmsListArray, &aReceivedClass2ToBeResent);

		iMessageManagerCallback.Complete(EMobilePhoneStoreReadAllPhase1, &dataPackage, aError);
		iSmsListArray->ResetAndDestroy();
		}
	
	} // CPhonebookDispatcher::CallbackSmsStoreReadAll

void CPhonebookDispatcher::CallbackStoreWriteEntry(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TUint16 aIndex, TInt aMaxNumberLength)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookStoreWriteEntryComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * KErrGsmSimServSneFull, KErrGsmSimServAnrFull, or KErrGsmSimServEmailFull indicates one of the
 * services is full and the field of the entry (e.g. Email) could not be written. 
 * In the cache case the field (e.g. Email) is not written to the cache but the other
 * fields are written, the request is then completed with the error code.  In the non-cache case
 * the request is completed with the error code.
 * @param aPhonebook The phonebook name for which the write request was made.
 * @param aIndex The index to which the entry was written in the phonebook.
 * @param aMaxNumberLength The new maximum number length possible in the phonebook, this field is
 * only relevant to Fdn, Adn and Vmb phonebooks.
 *
 * @see RMobilePhoneBookStore::Write() 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aPhonebook=%d, aIndex=%d, aMaxNumberLength=%d"), aError, aPhonebook, aIndex, aMaxNumberLength);

	__ASSERT_DEBUG(aPhonebook != DispatcherPhonebook::EUnknown, CtsyDispatcherPanic(EInvalidParameter));
	
	CPhoneBookDataPackage phoneBookDataPackage;
	phoneBookDataPackage.SetPhoneBookName(PhonebookName(aPhonebook));
	
	TPBEntryInfo pbEntryInfo;
	pbEntryInfo.iLocation 		= aIndex;
	pbEntryInfo.iMaxNumLength 	= aMaxNumberLength;
	
	phoneBookDataPackage.PackData(&pbEntryInfo);
	
	iMessageManagerCallback.Complete(EMmTsyPhoneBookStoreWriteIPC, &phoneBookDataPackage, aError);
	
	} // CPhonebookDispatcher::CallbackStoreWriteEntry

void CPhonebookDispatcher::CallbackGetPhoneStoreInfo(TInt aError, const RMobilePhoneStore::TMobilePhoneStoreInfoV1& aPhoneStoreInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookGetPhoneStoreInfoComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aPhoneStoreInfo The phone store information.
 *
 * @see RMobilePhone::GetPhoneStoreInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&const_cast<RMobilePhoneStore::TMobilePhoneStoreInfoV1&>(aPhoneStoreInfo));
	
	iMessageManagerCallback.Complete(EMobilePhoneGetPhoneStoreInfo, &dataPackage, aError);
	
	} // CPhonebookDispatcher::CallbackGetPhoneStoreInfo

void CPhonebookDispatcher::CallbackSmsStoreDeleteEntry(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteEntryComp()
 *
 * @param aError The error code to be returned.
 * 
 * @see RMobileSmsStore::Delete()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneStoreDelete, aError);
	
	} // CPhonebookDispatcher::CallbackSmsStoreDeleteEntry

void CPhonebookDispatcher::CallbackSmsStoreDeleteAll(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteAllComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 *
 * @see RMobileSmsStore::DeleteAll()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	iMessageManagerCallback.Complete(EMobilePhoneStoreDeleteAll, aError);
	
	} // CPhonebookDispatcher::CallbackSmsStoreDeleteAll

void CPhonebookDispatcher::CallbackSmsStoreGetInfo(TInt aError, TInt aTotalEntries, TInt aUsedEntries)				
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreGetInfoComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aTotalEntries total number of entries in the SMS storage.
 * @param aUsedEntries total number of used entries in the SMS storage.
 *
 * @see RMobileSmsStore::GetInfo()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aTotalEntries=%d, aUsedEntries=%d"), aError, aTotalEntries, aUsedEntries);
	__ASSERT_DEBUG(aTotalEntries <= KMaxTotalEntries, CtsyDispatcherPanic(EInvalidParameter));
	
	// Pack the data to return to the Common TSY			
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aTotalEntries, &aUsedEntries);
	
	if(aTotalEntries <= KMaxTotalEntries)
		{
		iMessageManagerCallback.Complete(EMobilePhoneStoreGetInfo, &dataPackage, aError);		
		}
	else
		{
		iMessageManagerCallback.Complete(EMobilePhoneStoreGetInfo, &dataPackage, KErrCorrupt);
		}
	
	} // CPhonebookDispatcher::CallbackSmsStoreGetInfo

void CPhonebookDispatcher::CallbackSmsStoreReadEntry(TInt aError, const DispatcherPhonebook::TSmsData& aSmsData)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadEntryComp()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aSmsData SMS data. 
 *
 * @see RMobileSmsStore::Read()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	
	TSmsMsg smsMsg;
	smsMsg.iSmsMsg.Copy(aSmsData.iSmsMsg); 
	smsMsg.iServiceCentre.Copy(aSmsData.iServiceCentre); 
	smsMsg.iMobileScTON   = aSmsData.iMobileScTON;  
	smsMsg.iMobileScNPI   = aSmsData.iMobileScNPI;
	smsMsg.iMessageStatus = aSmsData.iMessageStatus;
		
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<TSmsMsg*>(&smsMsg));
	iMessageManagerCallback.Complete(EMobilePhoneStoreRead, &dataPackage, aError);
	
	} // CPhonebookDispatcher::CallbackSmsStoreReadEntry

void CPhonebookDispatcher::CallbackSmsStoreWriteEntry(TInt aError, TInt aLocation, TBool aReceivedClass2ToBeResent)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreWriteEntryComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aLocation location of the entry.
 * @param aReceivedClass2ToBeResent ETrue if received class is to be resent, else EFalse.  
 *
 * @see RMobileSmsStore::Write()
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aLocation=%d, aReceivedClass2ToBeResent=%d"), aError, aLocation, aReceivedClass2ToBeResent);
	__ASSERT_DEBUG(aLocation <= KMaxTotalEntries, CtsyDispatcherPanic(EInvalidParameter));
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aLocation, &aReceivedClass2ToBeResent);
	
	if(aLocation <= KMaxTotalEntries)
		{
		iMessageManagerCallback.Complete(EMobilePhoneStoreWrite, &dataPackage, aError);
		}
	else
		{
		iMessageManagerCallback.Complete(EMobilePhoneStoreWrite, &dataPackage, KErrCorrupt);
		}
		
	} // CPhonebookDispatcher::CallbackSmsStoreWriteEntry

void CPhonebookDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
/**
 * Part of the MDispatcherCallback interface. Used to complete requests handled
 * synchronously by the Licensee LTSY asynchronously back to the Common TSY.
 *  
 * @param aIpcDataPackage Package encapsulating the request.
 * 
 * @see MDispatcherCallback::CallbackSync
 */
	{
	TSYLOGENTRYEXIT;
	
	switch (aIpcDataPackage.iIpc)
		{
	
	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEBOOKDISPATCHER_CALLBACKSYNC_1, "WARNING: CPhonebookDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CPhonebookDispatcher::CallbackSync



