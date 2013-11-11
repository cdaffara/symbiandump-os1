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
// the Licensee LTSY relating to PhonebookOn related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHPHONEBOOKONINTERFACE_H_
#define MLTSYDISPATCHPHONEBOOKONINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>

class MLtsyDispatchPhonebookOnStoreRead : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreReadApiId = KDispatchPhonebookOnFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreReadIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadComp()
	 *
	 * Implementation of this interface should handle reading a ON entry from the storage.
	 *
	 *
	 * @param aIndex index of the ON entry.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreReadReqL(TInt aIndex) = 0;

	}; // class MLtsyDispatchPhonebookOnStoreRead



class MLtsyDispatchPhonebookOnStoreDeleteAll : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreDeleteAllApiId = KDispatchPhonebookOnFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreDeleteAllIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteAllComp()
	 *
	 * Implementation of this interface should handle deletion of all entries of the storage.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreDeleteAllReqL() = 0;

	}; // class MLtsyDispatchPhonebookOnStoreDeleteAll



class MLtsyDispatchPhonebookOnStoreReadEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreReadEntryApiId = KDispatchPhonebookOnFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreReadEntryIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadEntryComp()
	 *
	 * Implementation of this interface should handle reading an entry from ON storage as one step of reading of all entries.
	 *
	 *
	 * @param aIndex index of the entry,
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreReadEntryReqL(TInt aIndex) = 0;

	}; // class MLtsyDispatchPhonebookOnStoreReadEntry



class MLtsyDispatchPhonebookOnStoreGetInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreGetInfoApiId = KDispatchPhonebookOnFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreGetInfoIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetInfoComp()
	 *
	 * Implementation of this interface should request to get the information of a given storage. The 
	 * callback should be completed with the information.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreGetInfoReqL() = 0;

	}; // class MLtsyDispatchPhonebookOnStoreGetInfo



class MLtsyDispatchPhonebookOnStoreGetReadStoreSize : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId = KDispatchPhonebookOnFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreReadSizeIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetReadStoreSizeComp()
	 *
	 * Implementation of this interface should handle retrieving the size of the storage for read all operation.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreGetReadStoreSizeReqL() = 0;

	}; // class MLtsyDispatchPhonebookOnStoreGetReadStoreSize



class MLtsyDispatchPhonebookOnStoreDeleteEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreDeleteEntryApiId = KDispatchPhonebookOnFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreDeleteIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteEntryComp()
	 *
	 * Implementation of this interface should handle deletion of an ON entry.
	 *
	 *
	 * @param aIndex index of the ON entry.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreDeleteEntryReqL(TInt aIndex) = 0;

	}; // class MLtsyDispatchPhonebookOnStoreDeleteEntry



class MLtsyDispatchPhonebookOnStoreWriteEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreWriteEntryApiId = KDispatchPhonebookOnFuncUnitId + 7;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreWriteEntryIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteEntryComp()
	 *
	 * Implementation of this interface should handle storing of a ON entry as one step of storing of all entries in the ON storage.
	 *
	 *
	 * @param aMobileOnEntry defines contents of a fixed-size ON entry to be stored. 
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreWriteEntryReqL(const RMobileONStore::TMobileONEntryV1& aMobileOnEntry) = 0;

	}; // class MLtsyDispatchPhonebookOnStoreWriteEntry



class MLtsyDispatchPhonebookOnStoreWrite : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreWriteApiId = KDispatchPhonebookOnFuncUnitId + 8;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreWriteIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteComp()
	 *
	 * Implementation of this interface should handle storing of a ON entry.
	 *
	 * @param aMobileOnEntry defines contents of a fixed-size ON entry to be stored. 
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreWriteReqL(const RMobileONStore::TMobileONEntryV1& aMobileOnEntry) = 0;

	}; // class MLtsyDispatchPhonebookOnStoreWrite



class MLtsyDispatchPhonebookOnStoreGetStoreSize : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId = KDispatchPhonebookOnFuncUnitId + 9;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyONStoreWriteSizeIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetStoreSizeComp()
	 *
	 * Implementation of this interface should handle retrieving of the size of the storage.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreGetStoreSizeReqL() = 0;

	}; // class MLtsyDispatchPhonebookOnStoreGetStoreSize


#endif /*MLTSYDISPATCHPHONEBOOKONINTERFACE_H_*/
