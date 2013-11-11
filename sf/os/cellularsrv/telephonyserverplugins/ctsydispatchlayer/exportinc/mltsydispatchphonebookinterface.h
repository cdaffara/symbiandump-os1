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
// This file contains all the interfaces classes that can be implemented by
// the Licensee LTSY relating to Phonebook related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHPHONEBOOKINTERFACE_H_
#define MLTSYDISPATCHPHONEBOOKINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>

#include <ctsy/serviceapi/cmmsmsutility.h>

namespace DispatcherPhonebook
	{
	
	/**
	 * This namespace contains all types relating to the phonebook dispatcher.
	 */
	
	/**
	 * Enum containing all the supported phonebook types
	 */
	enum TPhonebook
		{
		EIccAdn = 0, 	//Abbriviated Dailling Numbers
		EIccBdn,		//Barred Dailling Numbers
		EIccSdn,		//Service Dialling numbers
		EIccFdn,		//Fixed Dialling numbers
		EIccVmb,		//Voice Mail Box
		EIccMbdn,		//Mailbox dialling numbers
		EUnknown
		};
	
	class TPhonebookStoreInfoV1
		{
		
		/**
		 * This structure contains the details required by the CTSY about the various
		 * phonebook stores, it is used in the initialisation procedure.
		 * 
		 * Total Entries 			- The total number of entries the store can contain.
		 * Maximum Text Length 		- The maximum length of a text field in the store.
		 * Maximum Number Length 	- The maximum length of a number in the store.
		 * Capabilities 			- The Bit field of the stores capabilities (RMobilePhoneStore::TMobilePhoneStoreCaps).
		 * Maximum Fields Per Entry - The maximum number of fields in a given entry.
		 */
		
	public:
		
		TPhonebookStoreInfoV1() :
			iAdnTotalEntries(-1),
			iAdnMaximumTextLength(-1),
			iAdnMaximumNumberLength(-1),
			iFdnTotalEntries(-1),
			iFdnMaximumTextLength(-1),
			iFdnMaximumNumberLength(-1),
			iSdnTotalEntries(-1),
			iSdnMaximumTextLength(-1),
			iSdnMaximumNumberLength(-1),
			iVmbTotalEntries(-1),
			iVmbMaximumTextLength(-1),
			iVmbMaximumNumberLength(-1),
			iVmbCapabilities(0),
			iMbdnTotalEntries(-1),
			iMbdnMaximumTextLength(-1),
			iMbdnMaximumNumberLength(-1),
			iMbdnCapabilities(0),
			iSecondNameMaximumFieldsPerEntry(-1),
			iSecondNameMaximumLength(-1),
			iEmailMaximumFieldsPerEntry(-1),
			iEmailMaximumLength(-1),
			iAdditionalNumberMaximumFieldsPerEntry(-1),
			iAdditionalNumberMaximumLength(-1)
			{
			}
		
		//Abbriviated Dailling Numbers
		TInt iAdnTotalEntries; 
		TInt iAdnMaximumTextLength;
		TInt iAdnMaximumNumberLength;

		//Fixed Dialling numbers
		TInt iFdnTotalEntries;
		TInt iFdnMaximumTextLength;
		TInt iFdnMaximumNumberLength;

		//Service Dialling numbers
		TInt iSdnTotalEntries;
		TInt iSdnMaximumTextLength;
		TInt iSdnMaximumNumberLength;

		//Voice Mail Box
		TInt iVmbTotalEntries;
		TInt iVmbMaximumTextLength;
		TInt iVmbMaximumNumberLength;
		TInt iVmbCapabilities;

		//Mailbox dialling numbers
		TInt iMbdnTotalEntries;
		TInt iMbdnMaximumTextLength;
		TInt iMbdnMaximumNumberLength;
		TInt iMbdnCapabilities;
		
		//3G Information - Second Names
		TInt iSecondNameMaximumFieldsPerEntry;
		TInt iSecondNameMaximumLength;

		//3G Information - Email Address
		TInt iEmailMaximumFieldsPerEntry;
		TInt iEmailMaximumLength;

		//3G Information - Additional Number
		TInt iAdditionalNumberMaximumFieldsPerEntry;
		TInt iAdditionalNumberMaximumLength;
		};
	
	
	struct TSmsData
		{
	public:	
	    TBuf8<231> iSmsMsg;
	    TBuf<KMaxAddressBufferSize> iServiceCentre;
	    RMobilePhone::TMobileTON iMobileScTON;
	    RMobilePhone::TMobileNPI iMobileScNPI;
	    TInt8 iMessageStatus;
		};
	
	}


class MLtsyDispatchPhonebookStoreReadEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookStoreReadEntryApiId = KDispatchPhonebookFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneBookStoreReadIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookStoreReadEntryComp()
	 *
	 * Implementation of this interface should request to read a number of entries from a given phonebook.
	 * The entries are returned in the callback.
	 *
	 *
	 * @param aPhonebook The phonebook to read entries from.
	 * @param aIndex Specifies the index to start reading from, must be >= 1.
	 * @param aNumSlots Specifies the number of slots to read, must be >= 1 and <= total number of slots.
	 * 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhoneBookStore::Read()
	 */
	virtual TInt HandleStoreReadEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndex, TInt aNumberOfSlots) = 0;

	}; // class MLtsyDispatchPhonebookStoreReadEntry



class MLtsyDispatchPhonebookStoreDeleteEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookStoreDeleteEntryApiId = KDispatchPhonebookFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneBookStoreDeleteIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteEntryComp()
	 *
	 * Implementation of this interface should request to delete an entry from a given phonebook, the
	 * callback indicates when the operation has been completed.
	 *
	 *
	 * @param aPhonebook The phonebook to delete an entry from.
	 * @param aIndex The entry index in the phonebook to delete.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhoneBookStore::Delete()
	 * 
	 */
	virtual TInt HandleStoreDeleteEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndex) = 0;

	}; // class MLtsyDispatchPhonebookStoreDeleteEntry



class MLtsyDispatchPhonebookStoreCache : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookStoreCacheApiId = KDispatchPhonebookFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneBookStoreCacheIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookStoreCacheComp()
	 *
	 * Implementation of this interface should request to cache a given phonebook, the entire 
	 * phonebook entries are returned in the callback.
	 *
	 *
	 * @param aPhonebook The phonebook to cache.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreCacheReqL(DispatcherPhonebook::TPhonebook aPhonebook) = 0;

	}; // class MLtsyDispatchPhonebookStoreCache



class MLtsyDispatchPhonebookStoreCacheCancel : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookStoreCacheCancelApiId = KDispatchPhonebookFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneBookStoreCacheCancelIPC
	 * request from the CTSY.
	 *
	 * It is a request call that has no complete call.
	 *
	 *
	 * Implementation of this interface should request to cancel an outstanding cache of a given phonebook.
	 *
	 * @param aPhonebook The phonebook to cancel the pending cache.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreCacheCancelReqL(DispatcherPhonebook::TPhonebook aPhonebook) = 0;

	}; // class MLtsyDispatchPhonebookStoreCacheCancel



class MLtsyDispatchPhonebookStoreGetPhonebookInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId = KDispatchPhonebookFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneBookStoreGetInfoIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookStoreGetPhonebookInfoComp()
	 *
	 * Implementation of this interface should request to get the information of a given phonebook. The 
	 * callback should be completed with the information.
	 * 
	 * This call to the LTSY is only used when the CTSY requires the information about the number of used entries
	 * in a phonebook, thus calls to RMobilePhoneBookStore::GetInfo() on cached phonebooks can be completed immediately 
	 * as the CTSY will already hold this information.
	 *
	 *
	 * @param aPhonebook The phonebook to retrieve the information from.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhoneBookStore::GetInfo()
	 * 
	 */
	virtual TInt HandleStoreGetPhonebookInfoReqL(DispatcherPhonebook::TPhonebook aPhonebook) = 0;

	}; // class MLtsyDispatchPhonebookStoreGetPhonebookInfo



class MLtsyDispatchPhonebookStoreInitialise : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookStoreInitialiseApiId = KDispatchPhonebookFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneBookStoreInitIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookStoreInitialiseComp()
	 *
	 * Implementation of this interface should request to initialise the phonebook store, the callback should
	 * be completed when store initialisation is completed.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreInitialiseReqL() = 0;

	}; // class MLtsyDispatchPhonebookStoreInitialise



class MLtsyDispatchPhonebookStoreDeleteAll : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookStoreDeleteAllApiId = KDispatchPhonebookFuncUnitId + 7;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneBookStoreDeleteAllIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookStoreDeleteAllComp()
	 *
	 * Implementation of this interface should request to delete all entries from a given phonebook, the
	 * callback indicates when the operation is completed.
	 *
	 *
	 * @param aPhonebook The phonebook to delete all entries from.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhoneBookStore::DeleteAll()
	 * 
	 */
	virtual TInt HandleStoreDeleteAllReqL(DispatcherPhonebook::TPhonebook aPhonebook) = 0;

	}; // class MLtsyDispatchPhonebookStoreDeleteAll



class MLtsyDispatchPhonebookStoreWriteEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookStoreWriteEntryApiId = KDispatchPhonebookFuncUnitId + 8;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneBookStoreWriteIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookStoreWriteEntryComp()
	 *
	 * Implementation of this interface should request to write an entry to a given phonebook.
	 * The callback indicates when the operation is complete.
	 *
	 *
	 * @param aPhonebook The phonebook to be written to.
	 * @param aEntry The entry to be written, this is coded as a TLV, this can be decoded either
	 * directly via a CPhoneBookBuffer() or via the CPhoneBookEntry::InternalizeFromTlvEntry() utility.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhoneBookStore::Write()
	 * @see CPhoneBookBuffer()
	 * @see CPhoneBookEntry::InternalizeFromTlvEntry()
	 * 
	 */
	virtual TInt HandleStoreWriteEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aEntry) = 0;

	}; // class MLtsyDispatchPhonebookStoreWriteEntry



class MLtsyDispatchPhonebookGetPhoneStoreInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookGetPhoneStoreInfoApiId = KDispatchPhonebookFuncUnitId + 9;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetPhoneStoreInfo
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookGetPhoneStoreInfoComp()
	 *
	 * Implementation of this interface should request to get the information of a given store. The 
	 * callback should be completed with the information. 
	 *
	 *
	 * @param aStoreName The name of the store to retrieve the information from (e.g. KETelIccFdnPhoneBook).
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhone::GetPhoneStoreInfo()
	 */
	virtual TInt HandleGetPhoneStoreInfoReqL(const TDesC& aStoreName) = 0;

	}; // class MLtsyDispatchPhonebookGetPhoneStoreInfo



class MLtsyDispatchPhonebookSmsStoreReadAll : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookSmsStoreReadAllApiId = KDispatchPhonebookFuncUnitId + 10;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneStoreReadAllPhase1
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadAllComp()
	 *
	 * Implementation of this interface should handle phase 1 of reading all entries in the SMS storage.
	 *
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmsStoreReadAllReqL() = 0;

	}; // class MLtsyDispatchPhonebookSmsStoreReadAll



class MLtsyDispatchPhonebookSmsStoreDeleteEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId = KDispatchPhonebookFuncUnitId + 11;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneStoreDelete
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteEntryComp()
	 *
	 * Implementation of this interface should handle deletion of an entry in the SMS storage.
	 *
	 * @param aIndex index of the entry to be deleted.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmsStoreDeleteEntryReqL(TInt aIndex) = 0;

	}; // class MLtsyDispatchPhonebookSmsStoreDeleteEntry



class MLtsyDispatchPhonebookSmsStoreDeleteAll : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookSmsStoreDeleteAllApiId = KDispatchPhonebookFuncUnitId + 12;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneStoreDeleteAll
	 * request from the CTSY.
	 *
	 * Implementation of this interface should handle deletion of all entries in the SMS storage.
	 *
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreDeleteAllComp()
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmsStoreDeleteAllReqL() = 0;

	}; // class MLtsyDispatchPhonebookSmsStoreDeleteAll



class MLtsyDispatchPhonebookSmsStoreGetInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookSmsStoreGetInfoApiId = KDispatchPhonebookFuncUnitId + 13;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneStoreGetInfo
	 * request from the CTSY.
	 *
	 * Implementation of this interface should handle retrieval information of the SMS storage.
	 *
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreGetInfoComp()
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmsStoreGetInfoReqL() = 0;

	}; // class MLtsyDispatchPhonebookSmsStoreGetInfo



class MLtsyDispatchPhonebookSmsStoreReadEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookSmsStoreReadEntryApiId = KDispatchPhonebookFuncUnitId + 14;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneStoreRead
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreReadEntryComp()
	 *
	 * Implementation of this interface should handle reading of an entry in the SMS storage.
	 *
	 * @param aIndex the index of the entry to read from.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmsStoreReadEntryReqL(TInt aIndex) = 0;

	}; // class MLtsyDispatchPhonebookSmsStoreReadEntry



class MLtsyDispatchPhonebookSmsStoreWriteEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookSmsStoreWriteEntryApiId = KDispatchPhonebookFuncUnitId + 15;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneStoreWrite
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookSmsStoreWriteEntryComp()
	 *
	 * Implementation of this interface should handle the storing of a Gsm Sms entry.
	 *
	 * Defines contents of a fixed-size, stored GSM SMS entry.
	 *
	 * @param aMobileGsmEntry Defines contents of a fixed-size GSM SMS entry to be stored.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmsStoreWriteEntryReqL(const RMobileSmsStore::TMobileGsmSmsEntryV1& aMobileGsmEntry) = 0;

	}; // class MLtsyDispatchPhonebookSmsStoreWriteEntry


#endif /*MLTSYDISPATCHPHONEBOOKINTERFACE_H_*/
