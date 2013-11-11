// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csecuritydispatcherTraces.h"
#endif

#include <ctsy/rmmcustomapi.h>
#include "csecuritydispatcher.h"

#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/serviceapi/cmmutility.h>

#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "tdispatcherholder.h"

CSecurityDispatcher::CSecurityDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot)
	{
	} // CSecurityDispatcher::CSecurityDispatcher

	  
CSecurityDispatcher::~CSecurityDispatcher()
	{
	} // CSecurityDispatcher::~CSecurityDispatcher


CSecurityDispatcher* CSecurityDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSecurityDispatcher* self =
		new (ELeave) CSecurityDispatcher(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CSecurityDispatcher::NewLC


CSecurityDispatcher* CSecurityDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot)
	{
	TSYLOGENTRYEXIT;
	CSecurityDispatcher* self =
		CSecurityDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aRequestAsyncOneShot);
	CleanupStack::Pop (self);
	return self;
	} // CSecurityDispatcher::NewL


void CSecurityDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to Security functionality
	// from the factory
	
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId))
		{
		TAny* getSecurityCodeInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId,
		       	getSecurityCodeInfoInterface);
		iLtsyDispatchSecurityGetSecurityCodeInfo =
				static_cast<MLtsyDispatchSecurityGetSecurityCodeInfo*>(getSecurityCodeInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityGetSecurityCodeInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId))
		{
		TAny* getLockInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId,
		       	getLockInfoInterface);
		iLtsyDispatchSecurityGetLockInfo =
				static_cast<MLtsyDispatchSecurityGetLockInfo*>(getLockInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityGetLockInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId))
		{
		TAny* abortSecurityCodeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId,
		       	abortSecurityCodeInterface);
		iLtsyDispatchSecurityAbortSecurityCode =
				static_cast<MLtsyDispatchSecurityAbortSecurityCode*>(abortSecurityCodeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityAbortSecurityCode, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId))
		{
		TAny* getCurrentActivePinInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId,
		       	getCurrentActivePinInterface);
		iLtsyDispatchSecurityGetCurrentActivePin =
				static_cast<MLtsyDispatchSecurityGetCurrentActivePin*>(getCurrentActivePinInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityGetCurrentActivePin, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId))
		{
		TAny* setLockSettingInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId,
		       	setLockSettingInterface);
		iLtsyDispatchSecuritySetLockSetting =
				static_cast<MLtsyDispatchSecuritySetLockSetting*>(setLockSettingInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecuritySetLockSetting, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId))
		{
		TAny* verifySecurityCodeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId,
		       	verifySecurityCodeInterface);
		iLtsyDispatchSecurityVerifySecurityCode =
				static_cast<MLtsyDispatchSecurityVerifySecurityCode*>(verifySecurityCodeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityVerifySecurityCode, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId))
		{
		TAny* getPin1DisableSupportedInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId,
		       	getPin1DisableSupportedInterface);
		iLtsyDispatchSecurityGetPin1DisableSupported =
			static_cast<MLtsyDispatchSecurityGetPin1DisableSupported*>(getPin1DisableSupportedInterface);
  		__ASSERT_DEBUG(iLtsyDispatchSecurityGetPin1DisableSupported, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
  		
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId))
		{
		TAny* checkSecurityCodeInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId,
		       	checkSecurityCodeInterface);
		iLtsyDispatchSecurityCheckSecurityCode =
				static_cast<MLtsyDispatchSecurityCheckSecurityCode*>(checkSecurityCodeInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityCheckSecurityCode, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId))
		{
		TAny* disablePhoneLockInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId,
		       	disablePhoneLockInterface);
		iLtsyDispatchSecurityDisablePhoneLock =
				static_cast<MLtsyDispatchSecurityDisablePhoneLock*>(disablePhoneLockInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityDisablePhoneLock, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId))
		{
		TAny* getCipheringInfoInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId,
		       	getCipheringInfoInterface);
		iLtsyDispatchSecurityGetCipheringInfo =
				static_cast<MLtsyDispatchSecurityGetCipheringInfo*>(getCipheringInfoInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityGetCipheringInfo, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId))
		{
		TAny* isSecurityCodeBlockedInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId,
		       	isSecurityCodeBlockedInterface);
		iLtsyDispatchSecurityIsSecurityCodeBlocked =
				static_cast<MLtsyDispatchSecurityIsSecurityCodeBlocked*>(isSecurityCodeBlockedInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityIsSecurityCodeBlocked, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchSecurityFuncUnitId, MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId))
		{
		TAny* checkSecurityCodeCancelInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
				MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId,
		       	checkSecurityCodeCancelInterface);
		iLtsyDispatchSecurityCheckSecurityCodeCancel =
				static_cast<MLtsyDispatchSecurityCheckSecurityCodeCancel*>(checkSecurityCodeCancelInterface);
        __ASSERT_DEBUG(iLtsyDispatchSecurityCheckSecurityCodeCancel, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	  		
	} // CSecurityDispatcher::ConstructL

void CSecurityDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CSecurityDispatcher::SetDispatcherHolder

TInt CSecurityDispatcher::DispatchGetSecurityCodeInfoL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneGetSecurityCodeInfo
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
   
    __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityGetSecurityCodeInfo)
		{
		RMobilePhone::TMobilePhoneSecurityCode secCode;
		aDataPackage->UnPackData(secCode);
		ret = iLtsyDispatchSecurityGetSecurityCodeInfo->HandleGetSecurityCodeInfoReqL(secCode);				
		}		

	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchGetSecurityCodeInfoL

TInt CSecurityDispatcher::DispatchGetLockInfoL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneGetLockInfo
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

    __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityGetLockInfo)
		{
		RMobilePhone::TMobilePhoneLock lockType;
		aDataPackage->UnPackData(lockType);
		ret = iLtsyDispatchSecurityGetLockInfo->HandleGetLockInfoReqL(lockType);				
		}
			
	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchGetLockInfoL

TInt CSecurityDispatcher::DispatchAbortSecurityCodeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneAbortSecurityCode
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

   __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityAbortSecurityCode)
		{
		RMobilePhone::TMobilePhoneSecurityCode secCode;
		aDataPackage->UnPackData(secCode);
		ret = iLtsyDispatchSecurityAbortSecurityCode->HandleAbortSecurityCodeReqL(secCode);				
		}

	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchAbortSecurityCodeL

TInt CSecurityDispatcher::DispatchGetCurrentActivePinL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	
	if (iLtsyDispatchSecurityGetCurrentActivePin)
		{
		ret = iLtsyDispatchSecurityGetCurrentActivePin->HandleGetCurrentActivePinReqL();
		}
	 	 
	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchGetCurrentActivePinL

TInt CSecurityDispatcher::DispatchSetLockSettingL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetLockSetting
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

    __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSecuritySetLockSetting)
		{
		TLockAndSetting* lockAndSetting = NULL;
		RMobilePhone::TMobilePassword* mobilePassword = NULL;
		aDataPackage->UnPackData(&lockAndSetting,&mobilePassword);
		
		//these should never be NULL
		__ASSERT_DEBUG(lockAndSetting, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(lockAndSetting->iLock, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(lockAndSetting->iSetting, CtsyDispatcherPanic(EInvalidNullPtr));
		
		//mobile password maybe null, as the CTSY has not packed one, if so pass 
		//down a reference to a null descriptor
		//ternary operator used so that KNullDesC does not have to have its const cast away
		const TDesC* mobilePasswordPtr = mobilePassword ? mobilePassword : &KNullDesC;

		ret = iLtsyDispatchSecuritySetLockSetting->HandleSetLockSettingReqL(*(lockAndSetting->iLock),*(lockAndSetting->iSetting),*mobilePasswordPtr);				
		}		

	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchSetLockSettingL

TInt CSecurityDispatcher::DispatchVerifySecurityCodeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneVerifySecurityCode
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

    __ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityVerifySecurityCode)
		{
		//sec code also has to be unpacked as a pointer due to CMmDataPackage's
		RMobilePhone::TMobilePhoneSecurityCode* secCode = NULL;
		RMobilePhone::TCodeAndUnblockCode* codes = NULL;
		aDataPackage->UnPackData(&secCode, &codes);
			
		//should never be NULL
		__ASSERT_DEBUG(codes, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(secCode, CtsyDispatcherPanic(EInvalidNullPtr));
			
		ret = iLtsyDispatchSecurityVerifySecurityCode->HandleVerifySecurityCodeReqL(*secCode, *codes);
		}

	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchVerifySecurityCodeL

TInt CSecurityDispatcher::DispatchGetPin1DisableSupportedL()
/**
 * Pass request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityGetPin1DisableSupported)
		{
		ret = iLtsyDispatchSecurityGetPin1DisableSupported->HandleGetPin1DisableSupportedReqL();
		}
			
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetPin1DisableSupportedL
	
TInt CSecurityDispatcher::DispatchCheckSecurityCodeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomCheckSecurityCodeIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));	

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityCheckSecurityCode)
		{
		TUint8* codeID = NULL;
		RMobilePhone::TMobilePassword* secCode = NULL;
		aDataPackage->UnPackData(&codeID, &secCode);
			
		__ASSERT_DEBUG(codeID, CtsyDispatcherPanic(EInvalidNullPtr));	
		__ASSERT_DEBUG(secCode, CtsyDispatcherPanic(EInvalidNullPtr));	
			
		DispatcherSecurity::TSecurityCodeId secCodeId = static_cast<DispatcherSecurity::TSecurityCodeId> (*codeID);
		ret = iLtsyDispatchSecurityCheckSecurityCode->HandleCheckSecurityCodeReqL(secCodeId, *secCode);
		}	

	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchCheckSecurityCodeL

TInt CSecurityDispatcher::DispatchDisablePhoneLockL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomDisablePhoneLockIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));		

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityDisablePhoneLock)
		{		
		RMobilePhone::TMobilePassword* secCode = NULL;
		aDataPackage->UnPackData(&secCode);
		__ASSERT_DEBUG(secCode, CtsyDispatcherPanic(EInvalidNullPtr));
			
		ret = iLtsyDispatchSecurityDisablePhoneLock->HandleDisablePhoneLockReqL(*secCode);			
		}

	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchDisablePhoneLockL

TInt CSecurityDispatcher::DispatchGetCipheringInfoL()
/**
 * Pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityGetCipheringInfo)
		{
		ret = iLtsyDispatchSecurityGetCipheringInfo->HandleGetCipheringInfoReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchGetCipheringInfoL

TInt CSecurityDispatcher::DispatchIsSecurityCodeBlockedL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomIsBlockedIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));			

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityIsSecurityCodeBlocked)
		{
		TUint8 codeID;
		aDataPackage->UnPackData(codeID);
		DispatcherSecurity::TSecurityCodeId secCodeId = static_cast<DispatcherSecurity::TSecurityCodeId> (codeID);
		ret = iLtsyDispatchSecurityIsSecurityCodeBlocked->HandleIsSecurityCodeBlockedReqL(secCodeId);			
		}
		
	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchIsSecurityCodeBlockedL

TInt CSecurityDispatcher::DispatchCheckSecurityCodeCancelL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomCheckSecurityCodeCancelIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));		

	// Can Handle... method in Licensee LTSY
	if (iLtsyDispatchSecurityCheckSecurityCodeCancel)
		{
		TUint8 codeID;
		TBool flag; // it will not be sent to the LTSY
		aDataPackage->UnPackData(codeID, flag);
		DispatcherSecurity::TSecurityCodeId secCodeId = static_cast<DispatcherSecurity::TSecurityCodeId> (codeID);
		ret = iLtsyDispatchSecurityCheckSecurityCodeCancel->HandleCheckSecurityCodeCancelReqL(secCodeId);			
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CSecurityDispatcher::DispatchCheckSecurityCodeCancelL


//
// Callback handlers follow
//



void CSecurityDispatcher::CallbackBootNotifySecurityReady(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityBootNotifySecurityReadyInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMmTsyBootNotifySecurityReadyIPC, aError);
	
	} // CSecurityDispatcher::CallbackBootNotifySecurityReady

void CSecurityDispatcher::CallbackNotifySecurityEvent(TInt aError, RMobilePhone::TMobilePhoneSecurityEvent aSecEvent)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityNotifySecurityEventInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSecEvent Security event.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSecEvent=%d"), aError, aSecEvent);

	CMmDataPackage dataPackage;		
	dataPackage.PackData(&aSecEvent);			             
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifySecurityEvent, &dataPackage, aError);
	
	} // CSecurityDispatcher::CallbackNotifySecurityEvent

void CSecurityDispatcher::CallbackNotifyLockInfoChange(TInt aError,
				RMobilePhone::TMobilePhoneLockStatus aStatus, RMobilePhone::TMobilePhoneLockSetting aSetting, RMobilePhone::TMobilePhoneLock aLock)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityNotifyLockInfoChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aStatus Status of the lock (unknown, locked, unlocked or blocked).
 * @param aSetting Lock setting (unknown, enabled, disabled or replaced).
 * @param aLock Lock information.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d, aSetting=%d, aLock=%d"), 
			                 aError, aStatus, aSetting, aLock);

    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
    lockInfo.iStatus = aStatus;
    lockInfo.iSetting = aSetting;
	CMmDataPackage dataPackage;		
	dataPackage.PackData(&lockInfo, &aLock);			             
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyLockInfoChange, &dataPackage, aError);
	
	} // CSecurityDispatcher::CallbackNotifyLockInfoChange

void CSecurityDispatcher::CallbackGetSecurityCodeInfo(TInt aError,
				RMobilePhone::TMobilePhoneSecurityCode aSecCode, TInt aRemainingEntryAttempts)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityGetSecurityCodeInfoComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSecCode Security code.
 * @param aRemainingEntryAttempts Number of remaining security code entry attempts before the corresponding lock gets blocked.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSecCode=%d, aRemainingEntryAttempts=%d"), aError, aSecCode, aRemainingEntryAttempts);

	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeInfo;
    codeInfo.iRemainingEntryAttempts = aRemainingEntryAttempts;
	CMmDataPackage dataPackage;		
	dataPackage.PackData(&aSecCode, &codeInfo);			             
	
	iMessageManagerCallback.Complete(EMobilePhoneGetSecurityCodeInfo, &dataPackage, aError);
	
	} // CSecurityDispatcher::CallbackGetSecurityCodeInfo

void CSecurityDispatcher::CallbackGetLockInfo(TInt aError,
				RMobilePhone::TMobilePhoneLockStatus aLockStatus, RMobilePhone::TMobilePhoneLockSetting aLockSetting)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityGetLockInfoComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aLockStatus Lock status (unknown, locked, unlocked or blocked).
 * @param aLockSetting Lock setting (unknown, enabled, disabled or replaced).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aLockStatus=%d, aLockSetting=%d"), aError, aLockStatus, aLockSetting);

	CMmDataPackage dataPackage;
	dataPackage.PackData(&aLockStatus, &aLockSetting);
		
	iMessageManagerCallback.Complete(EMobilePhoneGetLockInfo, &dataPackage,  aError);
	
	} // CSecurityDispatcher::CallbackGetLockInfo

void CSecurityDispatcher::CallbackAbortSecurityCode(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityAbortSecurityCodeComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneAbortSecurityCode, aError);
	
	} // CSecurityDispatcher::CallbackAbortSecurityCode

void CSecurityDispatcher::CallbackGetCurrentActivePin(TInt aError, RMobilePhone::TMobilePhoneSecurityCode aSecCode)				
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityGetCurrentActivePinComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSecCode Security code.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aSecCode=%d"), aError, aSecCode);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aSecCode);	
	
	iMessageManagerCallback.Complete(EMmTsySecurityGetSimActivePinStateIPC, &dataPackage, aError);
	
	} // CSecurityDispatcher::CallbackGetCurrentActivePin

void CSecurityDispatcher::CallbackSetLockSetting(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecuritySetLockSettingComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aLockStatus Phone lock status.
 * @param aLockSetting The phone lock setting.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

    // next 2 params are ignored by the CTSY:
    RMobilePhone::TMobilePhoneLockStatus lockStatus = RMobilePhone::EStatusLocked;
    RMobilePhone::TMobilePhoneLockSetting lockSetting = RMobilePhone::ELockSetEnabled;
	CMmDataPackage dataPackage;
	dataPackage.PackData(&lockStatus, &lockSetting);
		
	iMessageManagerCallback.Complete(EMobilePhoneSetLockSetting, &dataPackage,  aError);
		
	} // CSecurityDispatcher::CallbackSetLockSetting

void CSecurityDispatcher::CallbackVerifySecurityCode(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityVerifySecurityCodeComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneVerifySecurityCode, aError);
	
	} // CSecurityDispatcher::CallbackVerifySecurityCode
	
void CSecurityDispatcher::CallbackGetPin1DisableSupported(TInt aError, TBool aIsSupported)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * EMmTsyPhoneGetPin1DisableSupportedIPC back to the Common TSY.
 * 
 * @param aError The error code to complete back. 
 * @param aIsSupported Is SIM support disabling of PIN1.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d aIsSupported=%d"), aError, aIsSupported);

	// Package up data to return to Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aIsSupported);

	// Now complete the actual get request
	iMessageManagerCallback.Complete(EMmTsyPhoneGetPin1DisableSupportedIPC, &dataPackage, aError);		
	} // CSecurityDispatcher::CallbackGetPin1DisableSupported
	
void CSecurityDispatcher::CallbackNotifyCipheringInfoChange(TInt aError, const RMmCustomAPI::TCipheringInfo& aInfo)				
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityNotifyCipheringInfoChangeInd().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aInfo Ciphering information data structure: ciphering indicator and ciphering status.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aInfo.iIndStatus=%d, aInfo.iCiphStatus=%d"), aError, aInfo.iIndStatus, aInfo.iCiphStatus);
	
	// Package up data to return to Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<TBool*>(&aInfo.iIndStatus), 
			             const_cast<TBool*>(&aInfo.iCiphStatus));	
	
	iMessageManagerCallback.Complete(ECustomNotifyCipheringInfoChangeIPC, &dataPackage, aError);
	
	} // CSecurityDispatcher::CallbackNotifyCipheringInfoChange

void CSecurityDispatcher::CallbackCheckSecurityCode(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECustomCheckSecurityCodeIPC, aError);
	
	} // CSecurityDispatcher::CallbackCheckSecurityCode

void CSecurityDispatcher::CallbackDisablePhoneLock(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityDisablePhoneLockComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECustomDisablePhoneLockIPC, aError);
	
	} // CSecurityDispatcher::CallbackDisablePhoneLock

void CSecurityDispatcher::CallbackGetCipheringInfo(TInt aError, TBool aIndStatus)				
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityGetCipheringInfoComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aIndStatus Ciphering indicator status.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIndStatus=%d"), aError, aIndStatus);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aIndStatus);	

	// Pack the data to return to the Common TSY
	iMessageManagerCallback.Complete(ECustomGetCipheringInfoIPC, &dataPackage, aError);
	
	} // CSecurityDispatcher::CallbackGetCipheringInfo

void CSecurityDispatcher::CallbackIsSecurityCodeBlocked(TInt aError, TBool aIsBlocked)				
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityIsSecurityCodeBlockedComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aIsBlocked Is security code blocked.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aIsBlocked=%d"), aError, aIsBlocked);
	
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aIsBlocked);		
	
	iMessageManagerCallback.Complete(ECustomIsBlockedIPC, &dataPackage, aError);
	
	} // CSecurityDispatcher::CallbackIsSecurityCodeBlocked	

void CSecurityDispatcher::CallbackCheckSecurityCodeCancel(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSecurityCheckSecurityCodeCancelComp().
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(ECustomCheckSecurityCodeCancelIPC, aError);
	
	} // CSecurityDispatcher::CallbackCheckSecurityCodeCancel

void CSecurityDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
/**
 * Part of the MDispatcherCallback interface. Used to complete requests handled
 * synchronously by the Licensee LTSY asynchronously back to the Common TSY.
 *  
 * @param aIpcDataPackage Package encapsulating the request.
 * 
 * @see MDispatcherCallback::CallbackSync
 */
	{
	TSYLOGENTRYEXIT;
	
	switch (aIpcDataPackage.iIpc)
		{
	
	default:
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSECURITYDISPATCHER_CALLBACKSYNC_1, "WARNING: CSecurityDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	
	} // CSecurityDispatcher::CallbackSync



