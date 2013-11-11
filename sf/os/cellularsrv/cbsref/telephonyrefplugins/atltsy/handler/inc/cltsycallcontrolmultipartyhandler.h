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
// CLtsyCallControlMultipartyHandler

#ifndef __CLTSYDISPATCHCALLCONTROLMULTIPARTYHANDLER_H_
#define __CLTSYDISPATCHCALLCONTROLMULTIPARTYHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolmultipartyinterface.h>
#include "misdispatchinterfacesupported.h"
#include "ltsymacros.h"



// FORWARD DECLARATIONS
class CCtsyDispatcherCallback;

// Can be removed in real LTSY implementation
MTEST_FORWARD_DECLARE_MOCKLTSYENGINE

// CLASS DECLARATION

/**
 * Implements interfaces from the CTSY Dispatcher. The CTSY Dispatcher uses these
 * interfaces to make requests to the Licensee LTSY.
 */
class CLtsyCallControlMultipartyHandler : 	public CBase,
							public MIsDispatchInterfaceSupported,
							public MLtsyDispatchCallControlMultipartyConferenceHangUp,
							public MLtsyDispatchCallControlMultipartyConferenceAddCall,
							public MLtsyDispatchCallControlMultipartyCreateConference,
							public MLtsyDispatchCallControlMultipartyConferenceSwap,
							public MLtsyDispatchCallControlMultipartyConferenceGoOneToOne
	{
public:

	virtual ~CLtsyCallControlMultipartyHandler();
	static CLtsyCallControlMultipartyHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsyCallControlMultipartyHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);



    // From MLtsyDispatchCallControlMultipartyConferenceHangUp
    virtual TInt HandleConferenceHangUpReqL(const RArray<TInt>& aCallIds);

    // From MLtsyDispatchCallControlMultipartyConferenceAddCall
    virtual TInt HandleConferenceAddCallReqL(TInt aCallId, TInt aExistingCallId);

    // From MLtsyDispatchCallControlMultipartyCreateConference
    virtual TInt HandleCreateConferenceReqL(TInt aHeldCallId, TInt aSecondCallId);

    // From MLtsyDispatchCallControlMultipartyConferenceSwap
    virtual TInt HandleConferenceSwapReqL(TInt aHeldCallId, TInt aConnectedCallId);

    // From MLtsyDispatchCallControlMultipartyConferenceGoOneToOne
    virtual TInt HandleConferenceGoOneToOneReqL(TInt aCallId);

private:

	CLtsyCallControlMultipartyHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
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


	}; // class CLtsyCallControlMultipartyHandler

#endif // __CLTSYDISPATCHCALLCONTROLMULTIPARTYHANDLER_H_

