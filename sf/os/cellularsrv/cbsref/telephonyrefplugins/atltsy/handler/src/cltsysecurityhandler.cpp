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

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>

#include "cltsysecurityhandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"



CLtsySecurityHandler::CLtsySecurityHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsySecurityHandler::~CLtsySecurityHandler()
	{
	}


CLtsySecurityHandler* CLtsySecurityHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySecurityHandler* self = new (ELeave) CLtsySecurityHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsySecurityHandler* CLtsySecurityHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySecurityHandler* self=
			CLtsySecurityHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsySecurityHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsySecurityHandler::ConstructL

TBool CLtsySecurityHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{

	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;
		
		default:
			return EFalse;
		}
	}

void CLtsySecurityHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{

	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	/*
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchPhoneNotifyNetworkModeChangeIndId | 
						KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId | 
						KLtsyDispatchPhoneNotifySignalStrengthChangeIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	*/
	}


TInt CLtsySecurityHandler::HandleGetSecurityCodeInfoReqL(RMobilePhone::TMobilePhoneSecurityCode /*aSecCode*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityGetSecurityCodeInfoComp()
 *
 * @param aSecCode Security code.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleGetSecurityCodeInfoReqL


TInt CLtsySecurityHandler::HandleGetLockInfoReqL(RMobilePhone::TMobilePhoneLock /*aLockType*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityGetLockInfoComp()
 *
 * @param aLockType Phone lock type.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleGetLockInfoReqL


TInt CLtsySecurityHandler::HandleAbortSecurityCodeReqL(RMobilePhone::TMobilePhoneSecurityCode /*aSecCode*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityAbortSecurityCodeComp()
 *
 * @param aSecCode Security code.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleAbortSecurityCodeReqL


TInt CLtsySecurityHandler::HandleGetCurrentActivePinReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityGetCurrentActivePinComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleGetCurrentActivePinReqL


TInt CLtsySecurityHandler::HandleSetLockSettingReqL(RMobilePhone::TMobilePhoneLock /*aLock*/, RMobilePhone::TMobilePhoneLockSetting /*aSetting*/, const TDesC& /*aPassword*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecuritySetLockSettingComp()
 *
 * @param aLock The lock to change.
 * @param aSetting The new settings for the lock.
 * @param aPassword The password paramaters.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleSetLockSettingReqL


TInt CLtsySecurityHandler::HandleVerifySecurityCodeReqL(
		RMobilePhone::TMobilePhoneSecurityCode /*aSecCode*/,
		const RMobilePhone::TCodeAndUnblockCode& /*aCodes*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityVerifySecurityCodeComp()
 *
 * @param aSecCode Security code.
 * @param aCodes Code and unblock code.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleVerifySecurityCodeReqL

TInt CLtsySecurityHandler::HandleGetPin1DisableSupportedReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityGetPin1DisableSupportedComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//TO DO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleGetPin1DisableSupportedReqL

TInt CLtsySecurityHandler::HandleCheckSecurityCodeReqL(DispatcherSecurity::TSecurityCodeId /*aSecCodeID*/, const TDes& /*aSecCode*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeComp()
 *
 * @param aSecCodeID Code ID.
 * @param aSecCode Security code (maximum length = KMaxMobilePasswordSize).
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleCheckSecurityCodeReqL


TInt CLtsySecurityHandler::HandleDisablePhoneLockReqL(const TDesC& /*aSecCode*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityDisablePhoneLockComp()
 *
 * @param aSecCode Security password (maximum length = KMaxMobilePasswordSize).
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleDisablePhoneLockReqL


TInt CLtsySecurityHandler::HandleGetCipheringInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityGetCipheringInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleGetCipheringInfoReqL


TInt CLtsySecurityHandler::HandleIsSecurityCodeBlockedReqL(DispatcherSecurity::TSecurityCodeId /*aSecCodeID*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityIsSecurityCodeBlockedComp()
 *
 * @param aSecCodeID Code ID.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleIsSecurityCodeBlockedReqL

TInt CLtsySecurityHandler::HandleCheckSecurityCodeCancelReqL(DispatcherSecurity::TSecurityCodeId /*aSecCodeID*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeCancelComp()
 *
 * @param aCodeID Code ID.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySecurityHandler::HandleCheckSecurityCodeCancelReqL
