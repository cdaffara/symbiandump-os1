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

#ifndef __CSIMDISPATCHER_H_
#define __CSIMDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/rmmcustomapi.h>

#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class TDispatcherHolder;

class MLtsyDispatchSimGetApnControlListServiceStatus;
class MLtsyDispatchSimDeleteApnName;
class MLtsyDispatchSimEnumerateApnEntries;
class MLtsyDispatchSimChangeSecurityCode;
class MLtsyDispatchSimSetFdnSetting;
class MLtsyDispatchSimGetCustomerServiceProfile;
class MLtsyDispatchSimGetSubscriberId;
class MLtsyDispatchSimAppendApnName;
class MLtsyDispatchSimGetActiveIccApplicationType;
class MLtsyDispatchSimSetIccMessageWaitingIndicators;
class MLtsyDispatchSimSetApnControlListServiceStatus;
class MLtsyDispatchSimGetApnName;
class MLtsyDispatchSimSimRefreshDone;
class MLtsyDispatchSimGetServiceTable;
class MLtsyDispatchSimGetIccMessageWaitingIndicators;
class MLtsyDispatchSimSimLockActivate;
class MLtsyDispatchSimSimLockDeActivate;
class MLtsyDispatchSimGetAnswerToReset;
class MLtsyDispatchSimGetSimCardReaderStatus;
class MLtsyDispatchSimGetSimAuthenticationEapSimData;
class MLtsyDispatchSimGetSimAuthenticationEapAkaData;
class MLtsyDispatchSimPowerSimOff;
class MLtsyDispatchSimPowerSimOn;
class MLtsyDispatchSimReadSimFile;
class MLtsyDispatchSimSendApduRequest;
class MLtsyDispatchSimSendApduRequestV2;
class MLtsyDispatchSimSimWarmReset;
class MLtsyDispatchSimSetSimMessageStatusRead;



// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to Sim related requests to the Licensee LTSY.
 */
class CSimDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CSimDispatcher();
	
	static CSimDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	static CSimDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchGetApnControlListServiceStatusL();
	TInt DispatchDeleteApnNameL(const CMmDataPackage* aDataPackage);
	TInt DispatchEnumerateApnEntriesL();
	TInt DispatchChangeSecurityCodeL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetFdnSettingL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetCustomerServiceProfileL();
	TInt DispatchGetSubscriberIdL();
	TInt DispatchAppendApnNameL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetIccTypeL();
	TInt DispatchSetIccMessageWaitingIndicatorsL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetApnControlListServiceStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetApnNameL(const CMmDataPackage* aDataPackage);
	TInt DispatchSimRefreshDoneL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetServiceTableL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetIccMessageWaitingIndicatorsL();
	TInt DispatchSimLockActivateL(const CMmDataPackage* aDataPackage);
	TInt DispatchSimLockDeActivateL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetAnswerToResetL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetSimCardReaderStatusL();
	TInt DispatchGetSimAuthenticationDataL(const CMmDataPackage* aDataPackage);
	TInt DispatchPowerSimOffL();
	TInt DispatchPowerSimOnL();
	TInt DispatchReadSimFileL(const CMmDataPackage* aDataPackage);
	TInt DispatchSendApduRequestL(const CMmDataPackage* aDataPackage);
	TInt DispatchSendApduRequestV2L(const CMmDataPackage* aDataPackage);
	TInt DispatchSimWarmResetL();
	TInt DispatchSetSimMessageStatusReadL(const CMmDataPackage* aDataPackage);
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackRefreshSimFiles(TInt aError, TUint16 aRefreshFileList);
	void CallbackNotifyIccMessageWaitingIndicatorsChange(TInt aError, const RMobilePhone::TMobilePhoneMessageWaitingV1& aIndicators);
	void CallbackNotifyApnListChange(TInt aError);
	void CallbackNotifyApnControlListServiceStatusChange(TInt aError, RMobilePhone::TAPNControlListServiceStatus aStatus);
	void CallbackGetApnControlListServiceStatus(TInt aError, RMobilePhone::TAPNControlListServiceStatus aStatus);
	void CallbackDeleteApnName(TInt aError);
	void CallbackEnumerateApnEntries(TInt aError, TUint32 aNumEntries);
	void CallbackChangeSecurityCode(TInt aError);
	void CallbackSetFdnSetting(TInt aError);
	void CallbackGetCustomerServiceProfile(TInt aError, const RMobilePhone::TMobilePhoneCspFileV1& aCsp);
	void CallbackGetSubscriberId(TInt aError, const TDesC8& aId);
	void CallbackAppendApnName(TInt aError);
	void CallbackGetActiveIccApplicationType(TInt aError, MLtsyDispatchSimGetActiveIccApplicationType::TIccType aIccType);
	void CallbackSetIccMessageWaitingIndicators(TInt aError);
	void CallbackSetApnControlListServiceStatus(TInt aError); 
	void CallbackGetApnName(TInt aError, const RMobilePhone::TAPNEntryV3& aEntry);
	void CallbackGetServiceTable(TInt aError, const RMobilePhone::TMobilePhoneServiceTableV1& aServiceTable);
	void CallbackGetIccMessageWaitingIndicators(TInt aError, const RMobilePhone::TMobilePhoneMessageWaitingV1& aMessageWaitingIndicator);
	void CallbackNotifySimCardStatus(TInt aError, RMmCustomAPI::TSIMCardStatus aSimCardStatus);
	void CallbackSimLockActivate(TInt aError);
	void CallbackSimLockDeActivate(TInt aError);
	void CallbackGetAnswerToReset(TInt aError, const TDesC8& aAnswerToReset);
	void CallbackGetSimCardReaderStatus(TInt aError, TUint8 aSimCardReaderStatus);
	void CallbackGetSimAuthenticationData(TInt aError, const TDesC8& aSignedResponse, const TDesC8& aCipheringKey);
	void CallbackGetSimAuthenticationData(TInt aError, const TDesC8& aResponse, const TDesC8& aCipheringKey, const TDesC8& aIntegrityKey, const TDesC8& aAUTS);		
	void CallbackPowerSimOff(TInt aError);
	void CallbackPowerSimOn(TInt aError);
	void CallbackReadSimFile(TInt aError, const TDesC8& aResponseBytes);
	void CallbackSendApduRequest(TInt aError, TUint8 aServiceType, TUint8 aCardReaderNumber, TUint8 aApplicationType, const TDesC8& aResponseData);
	void CallbackSendApduRequestV2(TInt aError, const TDesC8& aResponseData);
	void CallbackSimWarmReset(TInt aError);
	void CallbackSetSimMessageStatusRead(TInt aError);
	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);

	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);
	
private:

	CSimDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			   MmMessageManagerCallback& aMessageManagerCallback,
	   			   CRequestQueueOneShot& aRequestAsyncOneShot);		
	
	void ConstructL();
	
private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
    MLtsyDispatchSimGetApnControlListServiceStatus* iLtsyDispatchSimGetApnControlListServiceStatus;
    MLtsyDispatchSimDeleteApnName* iLtsyDispatchSimDeleteApnName;
    MLtsyDispatchSimEnumerateApnEntries* iLtsyDispatchSimEnumerateApnEntries;
    MLtsyDispatchSimChangeSecurityCode* iLtsyDispatchSimChangeSecurityCode;
    MLtsyDispatchSimSetFdnSetting* iLtsyDispatchSimSetFdnSetting;
    MLtsyDispatchSimGetCustomerServiceProfile* iLtsyDispatchSimGetCustomerServiceProfile;
    MLtsyDispatchSimGetSubscriberId* iLtsyDispatchSimGetSubscriberId;
    MLtsyDispatchSimAppendApnName* iLtsyDispatchSimAppendApnName;
    MLtsyDispatchSimGetActiveIccApplicationType* iLtsyDispatchSimGetIccType;
    MLtsyDispatchSimSetIccMessageWaitingIndicators* iLtsyDispatchSimSetIccMessageWaitingIndicators;
    MLtsyDispatchSimSetApnControlListServiceStatus* iLtsyDispatchSimSetApnControlListServiceStatus;
    MLtsyDispatchSimGetApnName* iLtsyDispatchSimGetApnName;
    MLtsyDispatchSimSimRefreshDone* iLtsyDispatchSimSimRefreshDone;
    MLtsyDispatchSimGetServiceTable* iLtsyDispatchSimGetServiceTable;
    MLtsyDispatchSimGetIccMessageWaitingIndicators* iLtsyDispatchSimGetIccMessageWaitingIndicators;
    MLtsyDispatchSimSimLockActivate* iLtsyDispatchSimSimLockActivate;
    MLtsyDispatchSimSimLockDeActivate* iLtsyDispatchSimSimLockDeActivate;
    MLtsyDispatchSimGetAnswerToReset* iLtsyDispatchSimGetAnswerToReset;
    MLtsyDispatchSimGetSimCardReaderStatus* iLtsyDispatchSimGetSimCardReaderStatus;
    MLtsyDispatchSimGetSimAuthenticationEapSimData* iLtsyDispatchSimGetSimAuthenticationEapSimData;
    MLtsyDispatchSimGetSimAuthenticationEapAkaData* iLtsyDispatchSimGetSimAuthenticationEapAkaData;
    MLtsyDispatchSimPowerSimOff* iLtsyDispatchSimPowerSimOff;
    MLtsyDispatchSimPowerSimOn* iLtsyDispatchSimPowerSimOn;
    MLtsyDispatchSimReadSimFile* iLtsyDispatchSimReadSimFile;
    MLtsyDispatchSimSendApduRequest* iLtsyDispatchSimSendApduRequest;
    MLtsyDispatchSimSendApduRequestV2* iLtsyDispatchSimSendApduRequestV2;
    MLtsyDispatchSimSimWarmReset* iLtsyDispatchSimSimWarmReset;
    MLtsyDispatchSimSetSimMessageStatusRead* iLtsyDispatchSimSetSimMessageStatusRead;
    
	}; // class CSimDispatcher

#endif // __CSIMDISPATCHER_H_
	
