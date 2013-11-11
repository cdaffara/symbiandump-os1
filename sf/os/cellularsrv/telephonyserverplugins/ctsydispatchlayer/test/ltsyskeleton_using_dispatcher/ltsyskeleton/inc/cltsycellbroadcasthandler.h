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
// cltsydispatchcellbroadcasthandler.h
//

/**
 @file
 @internalAll 
*/

#ifndef __CLTSYDISPATCHCELLBROADCASTHANDLER_H_
#define __CLTSYDISPATCHCELLBROADCASTHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchcellbroadcastinterface.h>
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
class CLtsyCellBroadcastHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchCellBroadcastSetBroadcastFilterSetting,
							public MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage,
							public MLtsyDispatchCellBroadcastReceiveMessageCancel,
							public MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing,
							public MLtsyDispatchCellBroadcastDeleteSimCbTopic
							
	{
public:

	virtual ~CLtsyCellBroadcastHandler();
	static CLtsyCellBroadcastHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsyCellBroadcastHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	
	
    // From MLtsyDispatchCellBroadcastSetBroadcastFilterSetting
    virtual TInt HandleSetBroadcastFilterSettingReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter aFilterSetting);

    // From MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage
    virtual TInt HandleActivateBroadcastReceiveMessageReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter aFilterSetting);

    // From MLtsyDispatchCellBroadcastReceiveMessageCancel
    virtual TInt HandleReceiveMessageCancelReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter aFilterSetting);
    
    // From MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing
    virtual TInt HandleStartSimCbTopicBrowsingReqL();
    
    // From MLtsyDispatchCellDeleteSimCbTopic
    virtual TInt HandleDeleteSimCbTopicReqL(TUint aIndex, TBool aDeleteFlag);

private:
	
	CLtsyCellBroadcastHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
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

	}; // class CLtsyCellBroadcastHandler

#endif // __CLTSYDISPATCHCELLBROADCASTHANDLER_H_

