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
// cltsydispatchphonebookonhandler.h
//

/**
 @file
 @internalAll 
*/

#ifndef __CLTSYDISPATCHPHONEBOOKONHANDLER_H_
#define __CLTSYDISPATCHPHONEBOOKONHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchphonebookoninterface.h>
#include "misdispatchinterfacesupported.h"
#include "ltsymacros.h"

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

// FORWARD DECLARATIONS
class CCtsyDispatcherCallback;

// Can be removed in real LTSY implementation
MTEST_FORWARD_DECLARE_MOCKLTSYENGINE

// CLASS DECLARATION

/**
 * Implements interfaces from the CTSY Dispatcher. The CTSY Dispatcher uses these
 * interfaces to make requests to the Licensee LTSY.
 */
class CLtsyPhonebookOnHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchPhonebookOnStoreRead,
							public MLtsyDispatchPhonebookOnStoreDeleteAll,
							public MLtsyDispatchPhonebookOnStoreReadEntry,
							public MLtsyDispatchPhonebookOnStoreGetInfo,
							public MLtsyDispatchPhonebookOnStoreGetReadStoreSize,
							public MLtsyDispatchPhonebookOnStoreDeleteEntry,
							public MLtsyDispatchPhonebookOnStoreWriteEntry,
							public MLtsyDispatchPhonebookOnStoreWrite,
							public MLtsyDispatchPhonebookOnStoreGetStoreSize
							
	{
public:

	virtual ~CLtsyPhonebookOnHandler();
	static CLtsyPhonebookOnHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsyPhonebookOnHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	


    // From MLtsyDispatchPhonebookOnStoreRead
    virtual TInt HandleStoreReadReqL(TInt aIndex);

    // From MLtsyDispatchPhonebookOnStoreDeleteAll
    virtual TInt HandleStoreDeleteAllReqL();

    // From MLtsyDispatchPhonebookOnStoreReadEntry
    virtual TInt HandleStoreReadEntryReqL(TInt aIndex);

    // From MLtsyDispatchPhonebookOnStoreGetInfo
    virtual TInt HandleStoreGetInfoReqL();

    // From MLtsyDispatchPhonebookOnStoreGetReadStoreSize
    virtual TInt HandleStoreGetReadStoreSizeReqL();

    // From MLtsyDispatchPhonebookOnStoreDeleteEntry
    virtual TInt HandleStoreDeleteEntryReqL(TInt aIndex);

    // From MLtsyDispatchPhonebookOnStoreWriteEntry
    virtual TInt HandleStoreWriteEntryReqL(const RMobileONStore::TMobileONEntryV1& aMobileOnEntry);

    // From MLtsyDispatchPhonebookOnStoreWrite
    virtual TInt HandleStoreWriteReqL(const RMobileONStore::TMobileONEntryV1& aMobileOnEntry);

    // From MLtsyDispatchPhonebookOnStoreGetStoreSize
    virtual TInt HandleStoreGetStoreSizeReqL();


private:
	
	CLtsyPhonebookOnHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	void ConstructL();

private:
	
	// Not owned
	
	/**
	 * Callback object in the CTSY Dispatcher.
	 * 
	 * Used to complete a request back to the CTSY Dispatcher.
	 */
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;
	
public:
	// Can be removed in real LTSY implementation.
	MTEST_DECLARE_MOCKLTSYENGINE

	}; // class CLtsyPhonebookOnHandler

#endif // __CLTSYDISPATCHPHONEBOOKONHANDLER_H_

