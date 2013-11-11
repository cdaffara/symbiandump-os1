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
// the Licensee LTSY relating to Security related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHSECURITYINTERFACE_H_
#define MLTSYDISPATCHSECURITYINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>
#include <ctsy/serviceapi/cmmutility.h>

namespace DispatcherSecurity
	{
	
	/**
	 * This namespace contains all types relating to the Security dispatcher.
	 */

	enum TSecurityCodeId
	    {   
	    ESecCodeSecurity = 1,
	    ESecCodePin,
	    ESecCodePuk,
	    ESecCodePin2,
	    ESecCodePuk2,
	    ESecCodeUpin = 7    
	    };

	}

class MLtsyDispatchSecurityGetSecurityCodeInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSecurityGetSecurityCodeInfoApiId = KDispatchSecurityFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetSecurityCodeInfo
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityGetSecurityCodeInfoComp()
	 *
	 * Implementation of this interface should allow a client to retrieve the current number or remaining entry attempts of a
     *  security code.
	 *
	 * @param aSecCode The security code whose information is to be retrieved.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetSecurityCodeInfoReqL(RMobilePhone::TMobilePhoneSecurityCode aSecCode) = 0;

	}; // class MLtsyDispatchSecurityGetSecurityCodeInfo



class MLtsyDispatchSecurityGetLockInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSecurityGetLockInfoApiId = KDispatchSecurityFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetLockInfo
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityGetLockInfoComp()
	 *
	 * Implementation of this interface should retrieve the current status and setting of a lock.
	 *
	 * @param aLockType The lock type whose information is to be retrieved.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetLockInfoReqL(RMobilePhone::TMobilePhoneLock aLockType) = 0;

	}; // class MLtsyDispatchSecurityGetLockInfo



class MLtsyDispatchSecurityAbortSecurityCode : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSecurityAbortSecurityCodeApiId = KDispatchSecurityFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneAbortSecurityCode
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityAbortSecurityCodeComp()
	 *
	 * Implementation of this interface should inform the phone that the user has cancelled an outstanding 
     *  "get security code" or "verify security code" request.
	 *
	 * @param aSecCode Specifies which code request has been cancelled.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleAbortSecurityCodeReqL(RMobilePhone::TMobilePhoneSecurityCode aSecCode) = 0;

	}; // class MLtsyDispatchSecurityAbortSecurityCode



class MLtsyDispatchSecurityGetCurrentActivePin : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSecurityGetCurrentActivePinApiId = KDispatchSecurityFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsySecurityGetSimActivePinStateIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityGetCurrentActivePinComp()
	 *
	 * Implementation of this interface should prepare the updating the information of the currently active PIN.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCurrentActivePinReqL() = 0;

	}; // class MLtsyDispatchSecurityGetCurrentActivePin



class MLtsyDispatchSecuritySetLockSetting : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSecuritySetLockSettingApiId = KDispatchSecurityFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetLockSetting
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecuritySetLockSettingComp()
	 *
	 * Implementation of this interface should allow a client to change the current setting of a lock.
	 *
	 * @param aLock The lock to change.
	 * @param aSetting The new settings for the lock.
	 * @param aPassword The password paramaters.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhone::SetLockSetting()
	 */
	virtual TInt HandleSetLockSettingReqL(RMobilePhone::TMobilePhoneLock aLock, RMobilePhone::TMobilePhoneLockSetting aSetting, const TDesC& aPassword) = 0;

	}; // class MLtsyDispatchSecuritySetLockSetting



class MLtsyDispatchSecurityVerifySecurityCode : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSecurityVerifySecurityCodeApiId = KDispatchSecurityFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneVerifySecurityCode
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityVerifySecurityCodeComp()
	 *
	 * Implementation of this interface should send a security code requiring verification to the phone.
	 * 
	 *  If the security code required is one of the unblock codes (either PUK1 or 
	 *  PUK2), then the user must supply a new PIN1 or PIN2 code as well as the associated 
	 *  unblocking code. In this case, aUnblockCode will contain the unblocking code 
     *  and aCode will contain the new PIN1 or PIN2 value. If the security code required 
	 *  is NOT one of the unblock codes, then the user only has to supply the one 
	 *  security code. In this case, aUnblockCode will be invalid and aCode will contain 
	 *  the required security code (PIN1, PIN2 or Phone Password).
	 *
	 * @param aSecCode The type of code that is to be verified.
	 * @param aCodes The actual code and if a PUK1 or PUK2 is specified, the PUK code.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleVerifySecurityCodeReqL(
				RMobilePhone::TMobilePhoneSecurityCode aSecCode,
				const RMobilePhone::TCodeAndUnblockCode& aCodes) = 0;

	}; // class MLtsyDispatchSecurityVerifySecurityCode

class MLtsyDispatchSecurityGetPin1DisableSupported : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchSecurityGetPin1DisableSupportedApiId = KDispatchSecurityFuncUnitId + 7;
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyPhoneGetPin1DisableSupportedIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityGetPin1DisableSupportedComp()
	 *
	 * Implementation of this interface should complete whether the SIM supports disabling of PIN1.
	 * This is retrieved from either the EFsst (defined in 3GPP TS 31.102) or
	 * EFust (defined in 3GPP TS 11.11) tables.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetPin1DisableSupportedReqL() = 0;

	}; // class MLtsyDispatchSimGetPin1DisableSupported
	
class MLtsyDispatchSecurityCheckSecurityCode : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchSecurityCheckSecurityCodeApiId = KDispatchSecurityFuncUnitId + 8;
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomCheckSecurityCodeIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeComp()
	 * 
	 * Implementation of this interface should anwser the check security code request.
	 *
	 * @param aCodeID Code ID. 
	 * @param aSecCode Security code (maximum length = KMaxMobilePasswordSize).
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleCheckSecurityCodeReqL(DispatcherSecurity::TSecurityCodeId aSecCodeID, const TDes& aSecCode) = 0;

	}; // class MLtsyDispatchSecurityCheckSecurityCode


class MLtsyDispatchSecurityDisablePhoneLock : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchSecurityDisablePhoneLockApiId = KDispatchSecurityFuncUnitId + 9;
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomDisablePhoneLockIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityDisablePhoneLockComp()
	 *
	 * Implementation of this interface should disable phone lock.
	 *
	 * @param aSecCode Security code (maximum length = KMaxMobilePasswordSize).
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleDisablePhoneLockReqL(const TDesC& aSecCode) = 0;

	}; // class MLtsyDispatchSecurityDisablePhoneLock

class MLtsyDispatchSecurityGetCipheringInfo : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchSecurityGetCipheringInfoApiId = KDispatchSecurityFuncUnitId + 10;
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetCipheringInfoIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityGetCipheringInfoComp()
	 * 
	 * Implementation of this interface should allow retrieving ciphering info: indicator and ciphering status.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCipheringInfoReqL() = 0;

	}; // class MLtsyDispatchSecurityGetCipheringInfo


class MLtsyDispatchSecurityIsSecurityCodeBlocked : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchSecurityIsSecurityCodeBlockedApiId = KDispatchSecurityFuncUnitId + 11;
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomIsBlockedIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityIsSecurityCodeBlockedComp()
	 *
	 * Implementation of this interface should support retrieving synchronously the information
	 * if the security code is blocked or not. 
     *
	 * @param aCodeID Code ID.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleIsSecurityCodeBlockedReqL(DispatcherSecurity::TSecurityCodeId aSecCodeID) = 0;

	}; // class MLtsyDispatchSecurityIsSecurityCodeBlocked

class MLtsyDispatchSecurityCheckSecurityCodeCancel : public MLtsyDispatchInterface
	{
public:
	static const TInt KLtsyDispatchSecurityCheckSecurityCodeCancelApiId = KDispatchSecurityFuncUnitId + 12;	
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomCheckSecurityCodeCancelIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeCancelComp()
	 * 
	 * Implementation of this interface should cancel check security code request.
	 *
	 * @param aCodeID Code ID.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleCheckSecurityCodeCancelReqL(DispatcherSecurity::TSecurityCodeId aSecCodeID) = 0;

	}; // class MLtsyDispatchSecurityCheckSecurityCodeCancel

	

#endif /*MLTSYDISPATCHSECURITYINTERFACE_H_*/
