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
// CLtsySimHandler

#ifndef __CLTSYDISPATCHSIMHANDLER_H_
#define __CLTSYDISPATCHSIMHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
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
class CLtsySimHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchSimGetApnControlListServiceStatus,
							public MLtsyDispatchSimDeleteApnName,
							public MLtsyDispatchSimEnumerateApnEntries,
							public MLtsyDispatchSimChangeSecurityCode,
							public MLtsyDispatchSimSetFdnSetting,
							public MLtsyDispatchSimGetCustomerServiceProfile,
							public MLtsyDispatchSimGetSubscriberId,
							public MLtsyDispatchSimAppendApnName,
							public MLtsyDispatchSimGetActiveIccApplicationType,
							public MLtsyDispatchSimSetIccMessageWaitingIndicators,
							public MLtsyDispatchSimSetApnControlListServiceStatus,
							public MLtsyDispatchSimGetApnName,
							public MLtsyDispatchSimSimRefreshDone,
							public MLtsyDispatchSimGetServiceTable,
							public MLtsyDispatchSimGetIccMessageWaitingIndicators,
							public MLtsyDispatchSimSimLockActivate,
							public MLtsyDispatchSimSimLockDeActivate,
							public MLtsyDispatchSimGetAnswerToReset,
							public MLtsyDispatchSimGetSimCardReaderStatus,
							public MLtsyDispatchSimGetSimAuthenticationEapSimData,
							public MLtsyDispatchSimGetSimAuthenticationEapAkaData,
							public MLtsyDispatchSimPowerSimOff,
							public MLtsyDispatchSimPowerSimOn,
							public MLtsyDispatchSimReadSimFile,
							public MLtsyDispatchSimSendApduRequest,
							public MLtsyDispatchSimSendApduRequestV2,
							public MLtsyDispatchSimSimWarmReset,
							public MLtsyDispatchSimSetSimMessageStatusRead
							
	{
public:

	virtual ~CLtsySimHandler();
	static CLtsySimHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsySimHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	


    // From MLtsyDispatchSimGetApnControlListServiceStatus
    virtual TInt HandleGetApnControlListServiceStatusReqL();

    // From MLtsyDispatchSimDeleteApnName
    virtual TInt HandleDeleteApnNameReqL(TUint32 aIndex);

    // From MLtsyDispatchSimEnumerateApnEntries
    virtual TInt HandleEnumerateApnEntriesReqL();

    // From MLtsyDispatchSimChangeSecurityCode
    virtual TInt HandleChangeSecurityCodeReqL(RMobilePhone::TMobilePhoneSecurityCode aSecCode,
			   const RMobilePhone::TMobilePhonePasswordChangeV1& aPasswds);

    // From MLtsyDispatchSimSetFdnSetting
    virtual TInt HandleSetFdnSettingReqL(RMobilePhone::TMobilePhoneFdnSetting aFdnSetting);

    // From MLtsyDispatchSimGetCustomerServiceProfile
    virtual TInt HandleGetCustomerServiceProfileReqL();

    // From MLtsyDispatchSimGetSubscriberId
    virtual TInt HandleGetSubscriberIdReqL();

    // From MLtsyDispatchSimAppendApnName
    virtual TInt HandleAppendApnNameReqL(const RMobilePhone::TAPNEntryV3& aEntry);

    // From MLtsyDispatchSimGetActiveIccApplicationType
    virtual TInt HandleGetActiveIccApplicationTypeReqL();

    // From MLtsyDispatchSimSetIccMessageWaitingIndicators
    virtual TInt HandleSetIccMessageWaitingIndicatorsReqL(const RMobilePhone::TMobilePhoneMessageWaitingV1& aIndicators);

    // From MLtsyDispatchSimSetApnControlListServiceStatus
    virtual TInt HandleSetApnControlListServiceStatusReqL(RMobilePhone::TAPNControlListServiceStatus aStatus);

    // From MLtsyDispatchSimGetApnName
    virtual TInt HandleGetApnNameReqL(TUint32 aIndex);

    // From MLtsyDispatchSimSimRefreshDone
    virtual TInt HandleSimRefreshDoneReqL(TInt aError);
    
    // From MLtsyDispatchPhoneGetServiceTable
    virtual TInt HandleGetServiceTableReqL(RMobilePhone::TMobilePhoneServiceTable aServiceTable);
    
	// From MLtsyDispatchSimGetIccMessageWaitingIndicators
	virtual TInt HandleGetIccMessageWaitingIndicatorsReqL();
	
    // From MLtsyDispatchSimSimLockActivate
    virtual TInt HandleSimLockActivateReqL(const TDesC& aPassword, RMmCustomAPI::TLockNumber aLockNumber);

    // From MLtsyDispatchSimSimLockDeActivate
    virtual TInt HandleSimLockDeActivateReqL(const TDesC& aPassword, RMmCustomAPI::TLockNumber aLockNumber);

    // From MLtsyDispatchSimGetAnswerToReset
    virtual TInt HandleGetAnswerToResetReqL(const TDesC8& aAnswerToReset);

    // From MLtsyDispatchSimGetSimCardReaderStatus
    virtual TInt HandleGetSimCardReaderStatusReqL();

    // From MLtsyDispatchSimGetSimAuthenticationEapSimData
    virtual TInt HandleGetSimAuthenticationEapSimDataReqL(const TDesC8& aRandomParameters, TInt aRFStateInfo);

    // From MLtsyDispatchSimGetSimAuthenticationEapAkaData
    virtual TInt HandleGetSimAuthenticationEapAkaDataReqL(const TDesC8& aRandomParameters, const TDesC8& aAUTN, TInt aRFStateInfo);
    
    // From MLtsyDispatchSimPowerSimOff
    virtual TInt HandlePowerSimOffReqL();

    // From MLtsyDispatchSimPowerSimOn
    virtual TInt HandlePowerSimOnReqL();

    // From MLtsyDispatchSimReadSimFile
    virtual TInt HandleReadSimFileReqL(const TDesC8& aPath, TUint16 aOffset, TUint16 aSize);

    // From MLtsyDispatchSimSendApduRequest
    virtual TInt HandleSimSendApduRequestReqL(TUint8 aServiceType, TUint8 aCardReaderNumber, TUint8 aApplicationType, const TDesC8& aCommandData);
    
    // From MLtsyDispatchSimSendApduRequestV2
    virtual TInt HandleSimSendApduRequestV2ReqL(TUint8 aCardReaderId, const TDesC8& aCommandData);

    // From MLtsyDispatchSimSimWarmReset
    virtual TInt HandleSimWarmResetReqL();

    // From MLtsyDispatchSimSetSimMessageStatusRead
    virtual TInt HandleSetSimMessageStatusReadReqL(const TTime& aTimestamp, TInt aTimezoneDiff);


private:
	
	CLtsySimHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
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


	}; // class CLtsySimHandler

#endif // __CLTSYDISPATCHSIMHANDLER_H_

