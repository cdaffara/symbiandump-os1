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
// CLtsySecurityHandler

#ifndef __CLTSYDISPATCHSECURITYHANDLER_H_
#define __CLTSYDISPATCHSECURITYHANDLER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
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
class CLtsySecurityHandler : 	public CBase,
							public MIsDispatchInterfaceSupported, 
							public MLtsyDispatchSecurityGetSecurityCodeInfo,
							public MLtsyDispatchSecurityGetLockInfo,
							public MLtsyDispatchSecurityAbortSecurityCode,
							public MLtsyDispatchSecurityGetCurrentActivePin,
							public MLtsyDispatchSecuritySetLockSetting,
							public MLtsyDispatchSecurityVerifySecurityCode,
							public MLtsyDispatchSecurityGetPin1DisableSupported,
							public MLtsyDispatchSecurityCheckSecurityCode,
							public MLtsyDispatchSecurityDisablePhoneLock,
							public MLtsyDispatchSecurityGetCipheringInfo,
							public MLtsyDispatchSecurityIsSecurityCodeBlocked,
							public MLtsyDispatchSecurityCheckSecurityCodeCancel							
	{
public:

	virtual ~CLtsySecurityHandler();
	static CLtsySecurityHandler* NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	static CLtsySecurityHandler* NewLC(CCtsyDispatcherCallback& aCtsyDispatcherCallback);

	// From MIsDispatchInterfaceSupported
	virtual TBool IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId);
	void IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	


    // From MLtsyDispatchSecurityGetSecurityCodeInfo
    virtual TInt HandleGetSecurityCodeInfoReqL(RMobilePhone::TMobilePhoneSecurityCode aSecCode);

    // From MLtsyDispatchSecurityGetLockInfo
    virtual TInt HandleGetLockInfoReqL(RMobilePhone::TMobilePhoneLock aLockType);

    // From MLtsyDispatchSecurityAbortSecurityCode
    virtual TInt HandleAbortSecurityCodeReqL(RMobilePhone::TMobilePhoneSecurityCode aSecCode);

    // From MLtsyDispatchSecurityGetCurrentActivePin
    virtual TInt HandleGetCurrentActivePinReqL();

    // From MLtsyDispatchSecuritySetLockSetting
    virtual TInt HandleSetLockSettingReqL(RMobilePhone::TMobilePhoneLock aLock, RMobilePhone::TMobilePhoneLockSetting aSetting, const TDesC& aPassword);

    // From MLtsyDispatchSecurityVerifySecurityCode
    virtual TInt HandleVerifySecurityCodeReqL(RMobilePhone::TMobilePhoneSecurityCode aSecCode,
    										  const RMobilePhone::TCodeAndUnblockCode& aCodes);

	// From MLtsyDispatchSecurityGetPin1DisableSupported
	virtual TInt HandleGetPin1DisableSupportedReqL();
	
    // From MLtsyDispatchSecurityCheckSecurityCode
    virtual TInt HandleCheckSecurityCodeReqL(DispatcherSecurity::TSecurityCodeId aSecCodeID, const TDes& aSecCode);

    // From MLtsyDispatchSecurityDisablePhoneLock
    virtual TInt HandleDisablePhoneLockReqL(const TDesC& aSecCode);

    // From MLtsyDispatchSecurityGetCipheringInfo
    virtual TInt HandleGetCipheringInfoReqL();

    // From MLtsyDispatchSecurityIsSecurityCodeBlocked
    virtual TInt HandleIsSecurityCodeBlockedReqL(DispatcherSecurity::TSecurityCodeId aSecCodeID);
    
    // From MLtsyDispatchSecurityCheckSecurityCodeCancel
    virtual TInt HandleCheckSecurityCodeCancelReqL(DispatcherSecurity::TSecurityCodeId aSecCodeID);    

private:
	
	CLtsySecurityHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback);
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


	}; // class CLtsySecurityHandler

#endif // __CLTSYDISPATCHSECURITYHANDLER_H_

