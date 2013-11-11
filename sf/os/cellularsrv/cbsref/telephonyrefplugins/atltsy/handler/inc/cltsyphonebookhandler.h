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

#ifndef __CLTSYDISPATCHPHONEBOOKHANDLER_H_
#define __CLTSYDISPATCHPHONEBOOKHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>
#include "misdispatchinterfacesupported.h"
#include "ltsymacros.h"

// FORWARD DECLARATIONS
class CCtsyDispatcherCallback;
class CGlobalPhonemanager;
// Can be removed in real LTSY implementation
MTEST_FORWARD_DECLARE_MOCKLTSYENGINE

// CLASS DECLARATION

/**
 * Implements interfaces from the CTSY Dispatcher. The CTSY Dispatcher uses these
 * interfaces to make requests to the Licensee LTSY.
 */
class CLtsyPhonebookHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchPhonebookStoreReadEntry,
							public MLtsyDispatchPhonebookStoreDeleteEntry,
							public MLtsyDispatchPhonebookStoreCache,
							public MLtsyDispatchPhonebookStoreCacheCancel,
							public MLtsyDispatchPhonebookStoreGetPhonebookInfo,
							public MLtsyDispatchPhonebookStoreInitialise,
							public MLtsyDispatchPhonebookStoreDeleteAll,
							public MLtsyDispatchPhonebookSmsStoreReadAll,
							public MLtsyDispatchPhonebookStoreWriteEntry,
							public MLtsyDispatchPhonebookGetPhoneStoreInfo,
							public MLtsyDispatchPhonebookSmsStoreDeleteEntry,
							public MLtsyDispatchPhonebookSmsStoreDeleteAll,
							public MLtsyDispatchPhonebookSmsStoreGetInfo,
							public MLtsyDispatchPhonebookSmsStoreReadEntry,
							public MLtsyDispatchPhonebookSmsStoreWriteEntry
			
	{
public:

	virtual ~CLtsyPhonebookHandler();
	static CLtsyPhonebookHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone);
	static CLtsyPhonebookHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	


    // From MLtsyDispatchPhonebookStoreReadEntry
    virtual TInt HandleStoreReadEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndex, TInt aNumberOfSlots);

    // From MLtsyDispatchPhonebookStoreDeleteEntry
    virtual TInt HandleStoreDeleteEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, TInt aIndex);

    // From MLtsyDispatchPhonebookStoreCache
    virtual TInt HandleStoreCacheReqL(DispatcherPhonebook::TPhonebook aPhonebook);
    
    // From MLtsyDispatchPhonebookStoreCacheCancel
    virtual TInt HandleStoreCacheCancelReqL(DispatcherPhonebook::TPhonebook aPhonebook);

    // From MLtsyDispatchPhonebookStoreGetPhonebookInfo
    virtual TInt HandleStoreGetPhonebookInfoReqL(DispatcherPhonebook::TPhonebook aPhonebook);

    // From MLtsyDispatchPhonebookStoreInitialise
    virtual TInt HandleStoreInitialiseReqL();

    // From MLtsyDispatchPhonebookStoreDeleteAll
    virtual TInt HandleStoreDeleteAllReqL(DispatcherPhonebook::TPhonebook aPhonebook);

    // From MLtsyDispatchPhonebookStoreWriteEntry
    virtual TInt HandleStoreWriteEntryReqL(DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aEntry);

    // From MLtsyDispatchPhonebookGetPhoneStoreInfo
    virtual TInt HandleGetPhoneStoreInfoReqL(const TDesC& aStoreName);
    
    // From MLtsyDispatchPhonebookSmsStoreReadAll
    virtual TInt HandleSmsStoreReadAllReqL();

    // From MLtsyDispatchPhonebookSmsStoreDeleteEntry
    virtual TInt HandleSmsStoreDeleteEntryReqL(TInt aIndex);

    // From MLtsyDispatchPhonebookSmsStoreDeleteAll
    virtual TInt HandleSmsStoreDeleteAllReqL();

    // From MLtsyDispatchPhonebookSmsStoreGetInfo
    virtual TInt HandleSmsStoreGetInfoReqL();

    // From MLtsyDispatchPhonebookSmsStoreReadEntry
    virtual TInt HandleSmsStoreReadEntryReqL(TInt aIndex);

    // From MLtsyDispatchPhonebookSmsStoreWriteEntry
    virtual TInt HandleSmsStoreWriteEntryReqL(const RMobileSmsStore::TMobileGsmSmsEntryV1& aMobileGsmEntry);
public:
	
	/*virtual void LtsyIPCDataCallBack(TInt aIpc, TInt aError, TAny* aParams);
	
	virtual void LtsyINDDataCallBack(TInt aInd, TInt aError, TAny* aParams);*/
private:
	
	CLtsyPhonebookHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone);
	void ConstructL();

private:
	
	// Not owned
	
	/**
	 * Callback object in the CTSY Dispatcher.
	 * 
	 * Used to complete a request back to the CTSY Dispatcher.
	 */
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;
	CGlobalPhonemanager*     iGloblePhone;
public:


	}; // class CLtsyPhonebookHandler

#endif // __CLTSYDISPATCHPHONEBOOKHANDLER_H_

