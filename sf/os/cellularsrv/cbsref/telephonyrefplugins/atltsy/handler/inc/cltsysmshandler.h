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
// CLtsySmsHandler

#ifndef __CLTSYDISPATCHSMSHANDLER_H_
#define __CLTSYDISPATCHSMSHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchsmsinterface.h>
#include "misdispatchinterfacesupported.h"
#include "ltsymacros.h"

// FORWARD DECLARATIONS
class CCtsyDispatcherCallback;
class CGlobalPhonemanager;
class CAtSmsReceive;
class CAsyncHelperRequest;
// Can be removed in real LTSY implementation
MTEST_FORWARD_DECLARE_MOCKLTSYENGINE

// CLASS DECLARATION
/**
 * Implements interfaces from the CTSY Dispatcher. The CTSY Dispatcher uses these
 * interfaces to make requests to the Licensee LTSY.
 */
class CLtsySmsHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchSmsSendSatSms,
							public MLtsyDispatchSmsGetSmsStoreInfo,
							public MLtsyDispatchSmsGetSmspList,
							public MLtsyDispatchSmsNackSmsStored,
							public MLtsyDispatchSmsAckSmsStored,
							public MLtsyDispatchSmsResumeSmsReception,
							public MLtsyDispatchSmsSendSmsMessage,
							public MLtsyDispatchSmsSendSmsMessageNoFdnCheck,
							public MLtsyDispatchSmsSetMoSmsBearer,
							public MLtsyDispatchSmsStoreSmspListEntry
{
public:

	virtual ~CLtsySmsHandler();
	static CLtsySmsHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone);
	static CLtsySmsHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	


    // From MLtsyDispatchSmsSendSatSms
    virtual TInt HandleSendSatSmsReqL( const TDesC8& aSmsTpdu,
    										   RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
    										   const RMobilePhone::TMobileAddress& aGsmServiceCentre,
    										   TBool aMoreMessages
    										  );
    
    // From MLtsyDispatchSmsGetSmsStoreInfo
    virtual TInt HandleGetSmsStoreInfoReqL(/* TODO: Add request params here */);

    // From MLtsyDispatchSmsGetSmspList
    virtual TInt HandleGetSmspListReqL(/* TODO: Add request params here */);

    // From MLtsyDispatchSmsNackSmsStored
    virtual TInt HandleNackSmsStoredReqL(const TDesC8& aMsgPtr, TInt aRpCause);
    
    // From MLtsyDispatchSmsNackSmsStored
    virtual TInt HandleNackSmsStoredReqL(TInt aRpCause);

    // From MLtsyDispatchSmsAckSmsStored
    virtual TInt HandleAckSmsStoredReqL(const TDesC8& aMsgPtr);
    
    // From MLtsyDispatchSmsAckSmsStored
    virtual TInt HandleAckSmsStoredReqL();

    // From MLtsyDispatchSmsResumeSmsReception
    virtual TInt HandleResumeSmsReceptionReqL(/* TODO: Add request params here */);

    // From MLtsyDispatchSmsSendSmsMessage
    virtual TInt HandleSendSmsMessageReqL( const RMobilePhone::TMobileAddress& aDestination,
        								   const TDesC8& aSmsTpdu,
        								   RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
        								   const RMobilePhone::TMobileAddress& aGsmServiceCentre,
        								   TBool aMoreMessages
        								  ) ;

    // From MLtsyDispatchSmsSendSmsMessageNoFdnCheck
    virtual TInt HandleSendSmsMessageNoFdnCheckReqL( const RMobilePhone::TMobileAddress& aDestination,
    												 const TDesC8& aSmsTpdu,
    											     RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
    											     const RMobilePhone::TMobileAddress& aGsmServiceCentre,
    											     TBool aMoreMessages
    											    ) ;

    // From MLtsyDispatchSmsSetMoSmsBearer
    virtual TInt HandleSetMoSmsBearerReqL(RMobileSmsMessaging::TMobileSmsBearer aBearer);

    // From MLtsyDispatchSmsStoreSmspList
    virtual TInt HandleStoreSmspListEntryReqL(const RMobileSmsMessaging::TMobileSmspEntryV1& aSmsParam);
   
private:
	
	CLtsySmsHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone);
	void ConstructL();

private:
	
	// Not owned
	
	/**
	 * Callback object in the CTSY Dispatcher.
	 * 
	 * Used to complete a request back to the CTSY Dispatcher.
	 */
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;
	CGlobalPhonemanager* iGloblePhone;
	CAtSmsReceive       *iAtSmsReceive;
	CAsyncHelperRequest* iAsyncHelperRequest;
	
	}; // class CLtsySmsHandler

#endif // __CLTSYDISPATCHSMSHANDLER_H_

