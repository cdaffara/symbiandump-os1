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
// the Licensee LTSY relating to PhonebookEn related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHPHONEBOOKENINTERFACE_H_
#define MLTSYDISPATCHPHONEBOOKENINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>


class MLtsyDispatchPhonebookEnStoreGetInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookEnStoreGetInfoApiId = KDispatchPhonebookEnFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyENStoreGetInfoIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookEnStoreGetInfoComp()
	 *
	 * Implementation of this interface should handle retrieving of the information of the EN storage.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreGetInfoReqL() = 0;

	}; // class MLtsyDispatchPhonebookEnStoreGetInfo



class MLtsyDispatchPhonebookEnStoreReadAll : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookEnStoreReadAllApiId = KDispatchPhonebookEnFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyENStoreReadAllPhase1IPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadAllComp()
	 *
	 * Implementation of this interface should handle reading all the entries from the EN storage.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreReadAllReqL() = 0;

	}; // class MLtsyDispatchPhonebookEnStoreReadAll



class MLtsyDispatchPhonebookEnStoreReadEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhonebookEnStoreReadEntryApiId = KDispatchPhonebookEnFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyENStoreReadIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadEntryComp()
	 *
	 * Implementation of this interface should handle reading of an entry from the EN storage with a given index.
	 *
	 *
	 * @param aIndex The index of the EN entry.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreReadEntryReqL(TInt aIndex) = 0;

	}; // class MLtsyDispatchPhonebookEnStoreReadEntry


#endif /*MLTSYDISPATCHPHONEBOOKENINTERFACE_H_*/
