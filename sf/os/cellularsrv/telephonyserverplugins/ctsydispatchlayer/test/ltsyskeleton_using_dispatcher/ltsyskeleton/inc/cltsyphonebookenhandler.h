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
// cltsydispatchphonebookenhandler.h
//

/**
 @file
 @internalAll 
*/

#ifndef __CLTSYDISPATCHPHONEBOOKENHANDLER_H_
#define __CLTSYDISPATCHPHONEBOOKENHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchphonebookeninterface.h>
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
class CLtsyPhonebookEnHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchPhonebookEnStoreGetInfo,
							public MLtsyDispatchPhonebookEnStoreReadAll,
							public MLtsyDispatchPhonebookEnStoreReadEntry
							
	{
public:

	virtual ~CLtsyPhonebookEnHandler();
	static CLtsyPhonebookEnHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsyPhonebookEnHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	


    // From MLtsyDispatchPhonebookEnStoreGetInfo
    virtual TInt HandleStoreGetInfoReqL();

    // From MLtsyDispatchPhonebookEnStoreReadAll
    virtual TInt HandleStoreReadAllReqL();

    // From MLtsyDispatchPhonebookEnStoreReadEntry
    virtual TInt HandleStoreReadEntryReqL(TInt aIndex);


private:
	
	CLtsyPhonebookEnHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
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

	}; // class CLtsyPhonebookEnHandler

#endif // __CLTSYDISPATCHPHONEBOOKENHANDLER_H_

