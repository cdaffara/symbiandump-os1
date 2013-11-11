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
//

/**
 @file
 @internalAll 
*/

#ifndef __CSECURITYDISPATCHER_H_
#define __CSECURITYDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>


#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchSecurityGetSecurityCodeInfo;
class MLtsyDispatchSecurityGetLockInfo;
class MLtsyDispatchSecurityAbortSecurityCode;
class MLtsyDispatchSecurityGetCurrentActivePin;
class MLtsyDispatchSecuritySetLockSetting;
class MLtsyDispatchSecurityVerifySecurityCode;
class MLtsyDispatchSecurityGetPin1DisableSupported;
class MLtsyDispatchSecurityCheckSecurityCode;
class MLtsyDispatchSecurityDisablePhoneLock;
class MLtsyDispatchSecurityGetCipheringInfo;
class MLtsyDispatchSecurityIsSecurityCodeBlocked;
class MLtsyDispatchSecurityCheckSecurityCodeCancel;

// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to Security related requests to the Licensee LTSY.
 */
class CSecurityDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CSecurityDispatcher();
	
	static CSecurityDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CSecurityDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchGetSecurityCodeInfoL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetLockInfoL(const CMmDataPackage* aDataPackage);
	TInt DispatchAbortSecurityCodeL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetCurrentActivePinL();
	TInt DispatchSetLockSettingL(const CMmDataPackage* aDataPackage);
	TInt DispatchVerifySecurityCodeL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetPin1DisableSupportedL();
	TInt DispatchCheckSecurityCodeL(const CMmDataPackage* aDataPackage);
	TInt DispatchDisablePhoneLockL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetCipheringInfoL();
	TInt DispatchIsSecurityCodeBlockedL(const CMmDataPackage* aDataPackage);
	TInt DispatchCheckSecurityCodeCancelL(const CMmDataPackage* aDataPackage);
	
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackBootNotifySecurityReady(TInt aError);
	void CallbackNotifySecurityEvent(TInt aError, RMobilePhone::TMobilePhoneSecurityEvent aSecEvent);
	void CallbackNotifyLockInfoChange(TInt aError, RMobilePhone::TMobilePhoneLockStatus aStatus, RMobilePhone::TMobilePhoneLockSetting aSetting, RMobilePhone::TMobilePhoneLock aLock);
	void CallbackGetSecurityCodeInfo(TInt aError, RMobilePhone::TMobilePhoneSecurityCode aSecCode, TInt aRemainingEntryAttempts);
	void CallbackGetLockInfo(TInt aError, RMobilePhone::TMobilePhoneLockStatus aLockStatus, RMobilePhone::TMobilePhoneLockSetting aLockSetting);
	void CallbackAbortSecurityCode(TInt aError);
	void CallbackGetCurrentActivePin(TInt aError, RMobilePhone::TMobilePhoneSecurityCode aSecCode);
	void CallbackSetLockSetting(TInt aError);
	void CallbackVerifySecurityCode(TInt aError);
	void CallbackGetPin1DisableSupported(TInt aError, TBool aIsSupported);
	void CallbackNotifyCipheringInfoChange(TInt aError, const RMmCustomAPI::TCipheringInfo& aInfo);
	void CallbackCheckSecurityCode(TInt aError);
	void CallbackDisablePhoneLock(TInt aError);
	void CallbackGetCipheringInfo(TInt aError, TBool aIndStatus);
	void CallbackIsSecurityCodeBlocked(TInt aError, TBool aIsBlocked);
	void CallbackCheckSecurityCodeCancel(TInt aError);
	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);
	
private:

	CSecurityDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();
	
private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchSecurityGetSecurityCodeInfo* iLtsyDispatchSecurityGetSecurityCodeInfo;
    MLtsyDispatchSecurityGetLockInfo* iLtsyDispatchSecurityGetLockInfo;
    MLtsyDispatchSecurityAbortSecurityCode* iLtsyDispatchSecurityAbortSecurityCode;
    MLtsyDispatchSecurityGetCurrentActivePin* iLtsyDispatchSecurityGetCurrentActivePin;
    MLtsyDispatchSecuritySetLockSetting* iLtsyDispatchSecuritySetLockSetting;
    MLtsyDispatchSecurityVerifySecurityCode* iLtsyDispatchSecurityVerifySecurityCode;
    MLtsyDispatchSecurityGetPin1DisableSupported* iLtsyDispatchSecurityGetPin1DisableSupported;
    MLtsyDispatchSecurityCheckSecurityCode* iLtsyDispatchSecurityCheckSecurityCode;
    MLtsyDispatchSecurityDisablePhoneLock* iLtsyDispatchSecurityDisablePhoneLock;
    MLtsyDispatchSecurityGetCipheringInfo* iLtsyDispatchSecurityGetCipheringInfo;
    MLtsyDispatchSecurityIsSecurityCodeBlocked* iLtsyDispatchSecurityIsSecurityCodeBlocked;    
    MLtsyDispatchSecurityCheckSecurityCodeCancel* iLtsyDispatchSecurityCheckSecurityCodeCancel;
    
	}; // class CSecurityDispatcher

#endif // __CSECURITYDISPATCHER_H_
	
