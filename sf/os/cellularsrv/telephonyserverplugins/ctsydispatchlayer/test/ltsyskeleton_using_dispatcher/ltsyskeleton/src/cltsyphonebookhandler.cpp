// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cltsydispatchphonebookhandler.cpp
//

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>



#include "cltsyphonebookhandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

CLtsyPhonebookHandler::CLtsyPhonebookHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsyPhonebookHandler::~CLtsyPhonebookHandler()
	{
	}


CLtsyPhonebookHandler* CLtsyPhonebookHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookHandler* self = new (ELeave) CLtsyPhonebookHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyPhonebookHandler* CLtsyPhonebookHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookHandler* self=
			CLtsyPhonebookHandler::NewLC(aCtsyDispatcherCallback);
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
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_SUPPORT_MOCKLTSYENGINE(KDispatchPhonebookFuncUnitId, aDispatchApiId);
	
	/* uncomment when implementing for real LTSY
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;
		
		default:
			return EFalse;
		}
	*/
	}

void CLtsyPhonebookHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1.
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_IND_SUPPORT_MOCKLTSYENGINE(KDispatchPhonebookFuncUnitId, aIdGroup, aIndIdBitMask);
	
	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	/*
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchPhoneNotifyNetworkModeChangeIndId | 
						KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId | 
						KLtsyDispatchPhoneNotifySignalStrengthChangeIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	*/
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

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId,
									aPhonebook,
									aIndex,
									aNumberOfSlots);

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

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId,
									aPhonebook,
									aIndex);

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

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId,
									aPhonebook);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreCacheReqL


TInt CLtsyPhonebookHandler::HandleStoreGetPhonebookInfoReqL(DispatcherPhonebook::TPhonebook aPhonebook)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId,
									aPhonebook);

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

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreInitialiseReqL


TInt CLtsyPhonebookHandler::HandleStoreDeleteAllReqL(DispatcherPhonebook::TPhonebook aPhonebook)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId,
									aPhonebook);

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

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId);

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

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId,
									aPhonebook,
									&aEntry);

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

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId,
									&aStoreName);

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

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId, aIndex);

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

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId);

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

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId);

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

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, KDispatchPhonebookFuncUnitId, MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId, aIndex);

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

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId,
									&aMobileGsmEntry);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleSmsStoreWriteEntryReqL


TInt CLtsyPhonebookHandler::HandleStoreCacheCancelReqL(DispatcherPhonebook::TPhonebook aPhonebook)
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchPhonebookFuncUnitId, 
									MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId,
									aPhonebook);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookHandler::HandleStoreCacheCancelReqL





