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
// CLtsyPhonebookHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>
#include <mpbutil.h>
#include <etelmm.h>


#include "cltsyphonebookhandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"
#include "globalphonemanager.h"
#include "atsmsstoreinfo.h"
#include "atsmsstorewrite.h"
#include "atsmsstoreread.h"
#include "atsmsstoredelete.h"
#include "atsmsstorereadall.h"
#include "atphbkgetphonestoreinfo.h"
#include "atphbkstoreinitialise.h"
#include "atphbkstorewrite.h"
#include "atphbkstoreread.h"
#include "atphbkstorecache.h"
#include "atphbkstoredelete.h"


CLtsyPhonebookHandler::CLtsyPhonebookHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
    :iCtsyDispatcherCallback(aCtsyDispatcherCallback),iGloblePhone(aGloblePhone)
	{
	}

	
CLtsyPhonebookHandler::~CLtsyPhonebookHandler()
	{

	}


CLtsyPhonebookHandler* CLtsyPhonebookHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookHandler* self = new (ELeave) CLtsyPhonebookHandler(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyPhonebookHandler* CLtsyPhonebookHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookHandler* self=
			CLtsyPhonebookHandler::NewLC(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyPhonebookHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	} // CLtsyPhonebookHandler::ConstructL

TBool CLtsyPhonebookHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried.
 * @return indication whether interface is supported or not.
 */
	{

	switch(aDispatchApiId)
		{
		case KLtsyDispatchPhonebookStoreReadEntryApiId: 
			return ETrue;	
		case KLtsyDispatchPhonebookStoreDeleteEntryApiId: 
		 	return ETrue;	
		case KLtsyDispatchPhonebookStoreCacheApiId: 
			return ETrue;	
		case KLtsyDispatchPhonebookStoreCacheCancelApiId: 
			return ETrue;		
//		case KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId: 
//			return ETrue;	
		case KLtsyDispatchPhonebookStoreInitialiseApiId: 
			return ETrue;			
		case KLtsyDispatchPhonebookStoreDeleteAllApiId: 
			return ETrue;	
		case KLtsyDispatchPhonebookStoreWriteEntryApiId: 
			return ETrue;	
		case KLtsyDispatchPhonebookGetPhoneStoreInfoApiId: 
			return ETrue;		
		case KLtsyDispatchPhonebookSmsStoreReadAllApiId: 
			return ETrue;	
		case KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId: 
			return ETrue;	
		case KLtsyDispatchPhonebookSmsStoreDeleteAllApiId: 
			return ETrue;	
		case KLtsyDispatchPhonebookSmsStoreGetInfoApiId: 
			return ETrue;	
		case KLtsyDispatchPhonebookSmsStoreReadEntryApiId: 
			return ETrue;		
		case KLtsyDispatchPhonebookSmsStoreWriteEntryApiId: 
			return ETrue;	
		default:
			return EFalse;
		}
	}

void CLtsyPhonebookHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1.
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{

	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	}


TInt CLtsyPhonebookHandler::HandleStoreReadEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndex, TInt aNumberOfSlots)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookStoreReadEntryComp()
 *
 * @param aPhonebook The phonebook to read entries from.
 * @param aIndex Specifies the index to start reading from, must be >= 1.
 * @param aNumSlots Specifies the number of slots to read, must be >= 1 and <= total number of slots.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 * 
 * @see RMobilePhoneBookStore::Read()
 */
	{
	TSYLOGENTRYEXIT;
	
	if((aPhonebook != DispatcherPhonebook::EIccAdn)&&
			(aPhonebook != DispatcherPhonebook::EIccAdn))
		{
		return TSYLOGSETEXITERR(KErrNotSupported);
		}

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkStoreRead* atPhbkStoreRead = CAtPhbkStoreRead::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkStoreRead);
		atPhbkStoreRead->SetPhonebookType(aPhonebook);
		atPhbkStoreRead->SetReadParam(aIndex,aNumberOfSlots);
		atPhbkStoreRead->StartRequest();
	    }
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreReadEntryReqL


TInt CLtsyPhonebookHandler::HandleStoreDeleteEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndex)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteEntryComp()
 *
 * @param aPhonebook The phonebook to delete an entry from.
 * @param aIndex The entry index in the phonebook to delete.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 * 
 * @see RMobilePhoneBookStore::Delete() 
 */
	{
	TSYLOGENTRYEXIT;
	
	if((aPhonebook != DispatcherPhonebook::EIccAdn)&&
			(aPhonebook != DispatcherPhonebook::EIccAdn))
		{
		return TSYLOGSETEXITERR(KErrNotSupported);
		}
	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkStoreDelete* atPhbkStoreDelete = CAtPhbkStoreDelete::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkStoreDelete);
		atPhbkStoreDelete->SetPhonebookType(aPhonebook);
		atPhbkStoreDelete->SetDeleteIndex(aIndex);
		atPhbkStoreDelete->StartRequest();
	    }
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreDeleteEntryReqL


TInt CLtsyPhonebookHandler::HandleStoreCacheReqL(DispatcherPhonebook::TPhonebook aPhonebook)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookStoreCacheComp()
 *
 * @param aPhonebook The phonebook to cache.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	if((aPhonebook != DispatcherPhonebook::EIccAdn)&&
			(aPhonebook != DispatcherPhonebook::EIccAdn))
		{
		return TSYLOGSETEXITERR(KErrNotSupported);
		}
	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkStoreCache* atPhbkStoreCache = CAtPhbkStoreCache::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkStoreCache);
		atPhbkStoreCache->SetPhonebookType(aPhonebook);
		atPhbkStoreCache->StartRequest();
	    }
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreCacheReqL


TInt CLtsyPhonebookHandler::HandleStoreGetPhonebookInfoReqL(DispatcherPhonebook::TPhonebook /*aPhonebook*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookStoreGetPhonebookInfoComp()
 *
 * @param aPhonebook The phonebook to retrieve the information from.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 * 
 * @see RMobilePhoneBookStore::GetInfo() 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreGetPhonebookInfoReqL


TInt CLtsyPhonebookHandler::HandleStoreInitialiseReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookStoreInitialiseComp()
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

    TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkStoreInitialise* atPhbkStoreInitialise = CAtPhbkStoreInitialise::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkStoreInitialise);
		atPhbkStoreInitialise->StartRequest();
	    }
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreInitialiseReqL


TInt CLtsyPhonebookHandler::HandleStoreDeleteAllReqL(DispatcherPhonebook::TPhonebook /*aPhonebook*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteAllComp()
 *
 * @param aPhonebook The phonebook to delete all entries from.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 * 
 * @see RMobilePhoneBookStore::DeleteAll() 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreDeleteAllReqL


TInt CLtsyPhonebookHandler::HandleSmsStoreReadAllReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadAllComp().
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsStoreReadAll* atSmsStoreReadAll = CAtSmsStoreReadAll::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSmsStoreReadAll);
		atSmsStoreReadAll->StartRequest();
	    }
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleSmsStoreReadAllReqL


TInt CLtsyPhonebookHandler::HandleStoreWriteEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aEntry)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookStoreWriteEntryComp()
 *
 * @param aPhonebook The phonebook to be written to.
 * @param aEntry The entry to be written, this is coded as a TLV, this can be decoded either
 * directly via a CPhoneBookBuffer() or via the CPhoneBookEntry::InternalizeFromTlvEntry() utility.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 * 
 * @see RMobilePhoneBookStore::Write()
 * @see CPhoneBookBuffer()
 * @see CPhoneBookEntry::InternalizeFromTlvEntry()
 */
	{
	TSYLOGENTRYEXIT;
	if((aPhonebook != DispatcherPhonebook::EIccAdn)&&
			(aPhonebook != DispatcherPhonebook::EIccAdn))
		{
		return TSYLOGSETEXITERR(KErrNotSupported);
		}
	
    TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkStoreWrite* atPhbkStoreWrite = CAtPhbkStoreWrite::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkStoreWrite);
		atPhbkStoreWrite->SetPhonebookType(aPhonebook);
		atPhbkStoreWrite->SetPhonebookEntryL(aEntry);
		atPhbkStoreWrite->StartRequest();
	    }              
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreWriteEntryReqL


TInt CLtsyPhonebookHandler::HandleGetPhoneStoreInfoReqL(const TDesC& aStoreName)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookGetPhoneStoreInfoComp()
 *
 * @param aStoreName The store to retrieve the information from.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 * 
 * @see RMobilePhone::GetPhoneStoreInfo()
 */
	{
	TSYLOGENTRYEXIT;
	if((aStoreName.CompareF(KETelIccFdnPhoneBook) != KErrNone)
		&&(aStoreName.CompareF(KETelIccAdnPhoneBook) != KErrNone)
		&&(aStoreName.CompareF(KETelIccSmsStore) != KErrNone))
		{
		return TSYLOGSETEXITERR(KErrNotSupported);
		}
	
	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkGetPhoneStoreInfo* atPhbkGetPhoneStoreInfo =CAtPhbkGetPhoneStoreInfo::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkGetPhoneStoreInfo);
		atPhbkGetPhoneStoreInfo->SetPhoneStoreParam(aStoreName);
		atPhbkGetPhoneStoreInfo->StartRequest();
	    }
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleGetPhoneStoreInfoReqL


TInt CLtsyPhonebookHandler::HandleSmsStoreDeleteEntryReqL(TInt aIndex)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteEntryComp().
 *
 * @param aIndex index of the entry to be deleted from the SMS store.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	
	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsStoreDelete* atSmsStoreDelete = CAtSmsStoreDelete::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSmsStoreDelete);
		atSmsStoreDelete->SetDelIndex(aIndex);
		atSmsStoreDelete->StartRequest();
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleSmsStoreDeleteEntryReqL


TInt CLtsyPhonebookHandler::HandleSmsStoreDeleteAllReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteAllComp().
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsStoreDeleteAll* atSmsStoreDeleteAll = CAtSmsStoreDeleteAll::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSmsStoreDeleteAll);
		atSmsStoreDeleteAll->StartRequest();
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleSmsStoreDeleteAllReqL


TInt CLtsyPhonebookHandler::HandleSmsStoreGetInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreGetInfoComp().
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsStoreInfo* atSmsStoreInfo = CAtSmsStoreInfo::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSmsStoreInfo);
		atSmsStoreInfo->StartRequest();
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleSmsStoreGetInfoReqL


TInt CLtsyPhonebookHandler::HandleSmsStoreReadEntryReqL(TInt aIndex)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadEntryComp().
 *
 * @param aIndex index of the entry to be read.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsStoreRead* atSmsStoreRead = CAtSmsStoreRead::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSmsStoreRead);
		atSmsStoreRead->SetMsgIndex(aIndex);
		atSmsStoreRead->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleSmsStoreReadEntryReqL


TInt CLtsyPhonebookHandler::HandleSmsStoreWriteEntryReqL(const RMobileSmsStore::TMobileGsmSmsEntryV1& aMobileGsmEntry)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreWriteEntryComp().
 *
 * @param aMobileGsmEntry Defines contents of a fixed-size GSM SMS entry to be stored.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsStoreWrite* atSmsStoreWrite = CAtSmsStoreWrite::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSmsStoreWrite);
		atSmsStoreWrite->SeGsmSmsEntry(aMobileGsmEntry);
		atSmsStoreWrite->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleSmsStoreWriteEntryReqL


TInt CLtsyPhonebookHandler::HandleStoreCacheCancelReqL(DispatcherPhonebook::TPhonebook /*aPhonebook*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookStoreCacheCancel()
 *
 * @param aPhonebook The phonebook to cancel the pending cache.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	
	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreCacheCancelReqL

