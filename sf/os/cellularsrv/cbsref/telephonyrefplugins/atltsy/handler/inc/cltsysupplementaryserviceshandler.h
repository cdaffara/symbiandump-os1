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
// CLtsySupplementaryServicesHandler

#ifndef __CLTSYDISPATCHSUPPLEMENTARYSERVICESHANDLER_H_
#define __CLTSYDISPATCHSUPPLEMENTARYSERVICESHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchsupplementaryservicesinterface.h>
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
class CLtsySupplementaryServicesHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck,
							public MLtsyDispatchSupplementaryServicesGetCallWaitingStatus,
							public MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest,
							public MLtsyDispatchSupplementaryServicesGetCallBarringStatus,
							public MLtsyDispatchSupplementaryServicesSetCallBarringStatus,
							public MLtsyDispatchSupplementaryServicesSetCallForwardingStatus,
							public MLtsyDispatchSupplementaryServicesSetCallWaitingStatus,
							public MLtsyDispatchSupplementaryServicesSetSsPassword,
							public MLtsyDispatchSupplementaryServicesGetCallForwardingStatus,
							public MLtsyDispatchSupplementaryServicesSendUssdMessage,
							public MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck,
                            public MLtsyDispatchSupplementaryServicesSendUssdRelease,
                            public MLtsyDispatchSupplementaryServicesClearBlacklist
							
	{
public:

	virtual ~CLtsySupplementaryServicesHandler();
	static CLtsySupplementaryServicesHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsySupplementaryServicesHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	


    // From MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck
    virtual TInt HandleSendNetworkServiceRequestNoFdnCheckReqL(const TDesC& aRequest);

    // From MLtsyDispatchSupplementaryServicesGetCallWaitingStatus
    virtual TInt HandleGetCallWaitingStatusReqL();

    // From MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest
    virtual TInt HandleSendNetworkServiceRequestReqL(const TDesC& aRequest);

    // From MLtsyDispatchSupplementaryServicesGetCallBarringStatus
    virtual TInt HandleGetCallBarringStatusReqL(RMobilePhone::TMobilePhoneCBCondition aCondition);

    // From MLtsyDispatchSupplementaryServicesSetCallBarringStatus
    virtual TInt HandleSetCallBarringStatusReqL(RMobilePhone::TMobilePhoneCBCondition aCondition,
        RMobilePhone::TMobilePhoneServiceAction aAction,
        RMobilePhone::TMobileService aGroup,
        const TDesC& aPassword);

    // From MLtsyDispatchSupplementaryServicesSetCallForwardingStatus
    virtual TInt HandleSetCallForwardingStatusReqL(RMobilePhone::TMobilePhoneCFCondition aCondition,
    		RMobilePhone::TMobileService aServiceGroup,
    		RMobilePhone::TMobilePhoneServiceAction aAction,
    		const TDesC& aNumber,
    		TInt aTimeout);

    // From MLtsyDispatchSupplementaryServicesSetCallWaitingStatus
    virtual TInt HandleSetCallWaitingStatusReqL(RMobilePhone::TMobileService aService,
    		RMobilePhone::TMobilePhoneServiceAction aAction);

    // From MLtsyDispatchSupplementaryServicesSetSsPassword
    virtual TInt HandleSetSsPasswordReqL(const TDesC& aOldPassword,
			const TDesC& aNewPassword,
    		MLtsyDispatchSupplementaryServicesSetSsPassword::TServiceType aService);

    // From MLtsyDispatchSupplementaryServicesGetCallForwardingStatus
    virtual TInt HandleGetCallForwardingStatusReqL(RMobilePhone::TMobilePhoneCFCondition aCondition, RMobilePhone::TMobileService aService);

    // From MLtsyDispatchSupplementaryServicesSendUssdMessage
    virtual TInt HandleSendUssdMessageReqL(const TDesC8& aData, TUint8 aDcs, 
			RMobileUssdMessaging::TMobileUssdDataFormat aFormat, RMobileUssdMessaging::TMobileUssdMessageType aType);
    
    // From MLtsyDispatchSupplementaryServicesSendUssdMessagenoFdnCheck
    virtual TInt HandleSendUssdMessageNoFdnCheckReqL(const TDesC8& aData, TUint8 aDcs, 
			RMobileUssdMessaging::TMobileUssdDataFormat aFormat, RMobileUssdMessaging::TMobileUssdMessageType aType);

    // From MLtsyDispatchSupplementaryServicesSendUssdRelease
    virtual TInt HandleSendUssdReleaseReqL();

    // From MLtsyDispatchSupplementaryServicesClearBlacklist
    virtual TInt HandleClearBlacklistReqL();


private:
	
	CLtsySupplementaryServicesHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
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


	}; // class CLtsySupplementaryServicesHandler

#endif // __CLTSYDISPATCHSUPPLEMENTARYSERVICESHANDLER_H_

