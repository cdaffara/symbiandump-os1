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

#ifndef __CSUPPLEMENTARYSERVICESDISPATCHER_H_
#define __CSUPPLEMENTARYSERVICESDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>
#include <mmlist.h>
#include <ctsy/rmmcustomapi.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>


#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck;
class MLtsyDispatchSupplementaryServicesGetCallWaitingStatus;
class MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest;
class MLtsyDispatchSupplementaryServicesGetCallBarringStatus;
class MLtsyDispatchSupplementaryServicesSetCallBarringStatus;
class MLtsyDispatchSupplementaryServicesSetCallForwardingStatus;
class MLtsyDispatchSupplementaryServicesSetCallWaitingStatus;
class MLtsyDispatchSupplementaryServicesSetSsPassword;
class MLtsyDispatchSupplementaryServicesGetCallForwardingStatus;
class MLtsyDispatchSupplementaryServicesSendUssdMessage;
class MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck;
class MLtsyDispatchSupplementaryServicesSendUssdRelease;
class MLtsyDispatchSupplementaryServicesClearBlacklist;



// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to SupplementaryServices related requests to the Licensee LTSY.
 */
class CSupplementaryServicesDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CSupplementaryServicesDispatcher();
	
	static CSupplementaryServicesDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CSupplementaryServicesDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchSendNetworkServiceRequestNoFdnCheckL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetCallWaitingStatusL();
	TInt DispatchSendNetworkServiceRequestL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetCallBarringStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetCallBarringStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetCallForwardingStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetCallWaitingStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetSsPasswordL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetCallForwardingStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchSendUssdMessageL(const CMmDataPackage* aDataPackage);
	TInt DispatchSendUssdMessageNoFdnCheckL(const CMmDataPackage* aDataPackage);
	TInt DispatchSendUssdReleaseL();
	TInt DispatchClearBlacklistL();
	
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackNotifyCallBarringStatusChange(TInt aError, RMobilePhone::TMobilePhoneCBCondition aCondition);
	void CallbackReceiveUssdMessage(TInt aError, const TDesC8& aMessage, TUint8 aDcs, 
			RMobileUssdMessaging::TMobileUssdMessageType aType, 
			RMobileUssdMessaging::TMobileUssdDataFormat aFormat);
	void CallbackNotifyCallWaitingStatusChange(TInt aError, RMobilePhone::TMobileService aServiceGroup, RMobilePhone::TMobilePhoneCWStatus  aStatus);
	void CallbackNotifyCallForwardingStatusChange(TInt aError, RMobilePhone::TMobilePhoneCFCondition aCallForwardingCondition);
	void CallbackNotifyCallForwardingActive(TInt aError, RMobilePhone::TMobileService aServiceGroup, RMobilePhone::TMobilePhoneCFActive aActiveType);
	void CallbackSendNetworkServiceRequestNoFdnCheck(TInt aError);
	void CallbackGetCallWaitingStatus(TInt aError,  const CMobilePhoneCWList& aCallWaitingList);
	void CallbackSendNetworkServiceRequest(TInt aError);
	void CallbackGetCallBarringStatus(TInt aError, const CMobilePhoneCBList& aCallBarringList);
	void CallbackSetCallBarringStatus(TInt aError);
	void CallbackSetCallForwardingStatus(TInt aError);
	void CallbackSetCallWaitingStatus(TInt aError);
	void CallbackSetSsPassword(TInt aError);
	void CallbackGetCallForwardingStatus(TInt aError, const CMobilePhoneCFList& aCallForwardingList);
	void CallbackSendUssdMessage(TInt aError);
	void CallbackSendUssdMessageNoFdnCheck(TInt aError);
	void CallbackSendUssdRelease(TInt aError, TUint8 aOpCode, const TDesC& aAdditionalInfo);
	void CallbackClearBlacklist(TInt aError);
	void CallbackGetDiagnosticOctets(TInt aError, TInt aCallId, TUint8 aOctet);
	void CallbackNotifyAdditionalInfo(TInt aError, TUint8 aOperationCode, const TDesC8& aAdditionalInfo);
	void CallbackNotifyRequestComplete(TInt aError, TInt aStatus);
	void CallbackNotifyNetworkEventForwardMode(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsForwMode aForwardMode);
	void CallbackNotifyNetworkEventCallWaiting(TInt aError, RMmCustomAPI::TSsMode aMode, TBool aCallIsWaiting);
	void CallbackNotifyNetworkEventHoldMode(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsHoldMode aHoldMode);
	void CallbackNotifyNetworkEventConfrence(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, TBool aConferenceIndicator);
	void CallbackNotifyNetworkEventCug(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, TUint16 aCugIndex);
	void CallbackNotifyNetworkEventClirSuppression(TInt aError, RMmCustomAPI::TSsMode aMode, TBool aClirSuppressionRejected);
	void CallbackNotifyNetworkEventEctCallState(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsEctState aEctCallState, RMmCustomAPI::TSsChoice aEctChoice, const TDesC& aRemotePartyNumber);
	void CallbackNotifyNetworkEvent(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode);
	void CallbackNotifySendNetworkServiceRequest(TInt aError, RMobilePhone::TMobilePhoneNotifySendSSOperation aOperationCode, const TDesC& aAdditionalInfo);

	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);
	
private:

	CSupplementaryServicesDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();
	
private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck* iLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck;
    MLtsyDispatchSupplementaryServicesGetCallWaitingStatus* iLtsyDispatchSupplementaryServicesGetCallWaitingStatus;
    MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest* iLtsyDispatchSupplementaryServicesSendNetworkServiceRequest;
    MLtsyDispatchSupplementaryServicesGetCallBarringStatus* iLtsyDispatchSupplementaryServicesGetCallBarringStatus;
    MLtsyDispatchSupplementaryServicesSetCallBarringStatus* iLtsyDispatchSupplementaryServicesSetCallBarringStatus;
    MLtsyDispatchSupplementaryServicesSetCallForwardingStatus* iLtsyDispatchSupplementaryServicesSetCallForwardingStatus;
    MLtsyDispatchSupplementaryServicesSetCallWaitingStatus* iLtsyDispatchSupplementaryServicesSetCallWaitingStatus;
    MLtsyDispatchSupplementaryServicesSetSsPassword* iLtsyDispatchSupplementaryServicesSetSsPassword;
    MLtsyDispatchSupplementaryServicesGetCallForwardingStatus* iLtsyDispatchSupplementaryServicesGetCallForwardingStatus;
    MLtsyDispatchSupplementaryServicesSendUssdMessage* iLtsyDispatchSupplementaryServicesSendUssdMessage;
    MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck* iLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck;
    MLtsyDispatchSupplementaryServicesSendUssdRelease* iLtsyDispatchSupplementaryServicesSendUssdRelease;
    MLtsyDispatchSupplementaryServicesClearBlacklist* iLtsyDispatchSupplementaryServicesClearBlacklist;
   
    
	}; // class CSupplementaryServicesDispatcher

#endif // __CSUPPLEMENTARYSERVICESDISPATCHER_H_
	
