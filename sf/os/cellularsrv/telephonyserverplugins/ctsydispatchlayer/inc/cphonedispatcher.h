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

#ifndef __CPHONEDISPATCHER_H_
#define __CPHONEDISPATCHER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <etelmm.h>

#include <ctsy/ltsy/mltsydispatchfactory.h>


#include "requestqueueoneshot.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "mdispatchercallback.h"
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/rmmcustomapi.h>

// FORWARD DECLARATIONS
class CMmDataPackage;
class MmMessageManagerCallback;
class CMobilePhoneNetworkListV2;
class TDispatcherHolder;

class MLtsyDispatchPhoneGetFdnStatus;
class MLtsyDispatchPhoneGetNetworkRegistrationStatus;
class MLtsyDispatchPhoneGetHomeNetwork;
class MLtsyDispatchPhoneBootNotifyModemStatusReady;
class MLtsyDispatchPhoneBootNotifySimStatusReady;
class MLtsyDispatchPhoneSimRefreshRegister;

class MLtsyDispatchPhoneGetServiceProviderName;
class MLtsyDispatchPhoneGetPhoneId;
class MLtsyDispatchPhoneGetDetectedNetworks;
class MLtsyDispatchPhoneGetDetectedNetworksCancel;
class MLtsyDispatchPhoneSelectNetwork;
class MLtsyDispatchPhoneSelectNetworkCancel;
class MLtsyDispatchPhoneSetNetworkSelectionSetting;
class MBootSequenceCallbacks;
class MLtsyDispatchPhoneGetCurrentNetworkInfo;
class MLtsyDispatchPhoneGetNetworkMode;
class MLtsyDispatchPhoneGetNitzInfo;
class MLtsyDispatchPhoneGetSignalStrength;
class MLtsyDispatchPhoneGetBatteryInfo;
class MLtsyDispatchPhoneNspsWakeup;
class MLtsyDispatchPhoneSetSystemNetworkMode;
class MLtsyDispatchPhoneGetCurrentSystemNetworkModes;
class MLtsyDispatchPhoneResetNetServer;
class MLtsyDispatchPhoneSetAlwaysOnMode;
class MLtsyDispatchPhoneSetDriveMode;
class MLtsyDispatchPhoneGetHspaStatus;
class MLtsyDispatchPhoneSetHspaStatus;
class MLtsyDispatchPhoneGetNetworkProviderName;
class MLtsyDispatchPhoneGetOperatorName;
class MLtsyDispatchPhoneGetCellInfo;
class MLtsyDispatchPhoneRegisterCellInfoChangeNotification;
class MLtsyDispatchPhoneGetPhoneCellInfo;
class MLtsyDispatchPhoneGetUsimServiceSupport;
class MLtsyDispatchPhoneGetCurrentActiveUsimApplication;
class MLtsyDispatchPhoneTerminateAllCalls;
class MLtsyDispatchPhoneGetSystemNetworkBand;
class MLtsyDispatchPhoneSetSystemNetworkBand;

const TUint KPhoneIdReqOriginPhoneFu = 0x1;
const TUint KPhoneIdReqOriginSatFu = 0x2;


// CLASS DECLARATION

/**
 * This class is responsible for packing and unpacking data belonging
 * to Phone related requests to the Licensee LTSY.
 */
class CPhoneDispatcher : public CBase, public MDispatcherCallback
	{
public:

	virtual ~CPhoneDispatcher();
	
	static CPhoneDispatcher* NewL(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			MmMessageManagerCallback& aSatMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot,
			MBootSequenceCallbacks& aBootSequenceObserver);
	
	static CPhoneDispatcher* NewLC(
			MLtsyDispatchFactoryV1& aLtsyFactory,
			MmMessageManagerCallback& aMessageManagerCallback,
			MmMessageManagerCallback& aSatMessageManagerCallback,
			CRequestQueueOneShot& aRequestAsyncOneShot,
			MBootSequenceCallbacks& aBootSequenceObserver);
	
	// Dispatcher functions for dispatching requests DOWN to the Licensee LTSY
	
	TInt DispatchGetFdnStatusL();
	TInt DispatchGetNetworkRegistrationStatusL();
	TInt DispatchGetHomeNetworkL();
	TInt DispatchBootNotifyModemStatusReadyL();
	TInt DispatchBootNotifySimStatusReadyL();
	TInt DispatchSimRefreshRegisterL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetServiceProviderNameL();
	TInt DispatchGetPhoneIdL(TUint aRequestOrigin);
	TInt DispatchGetDetectedNetworksL();
	TInt DispatchGetDetectedNetworksCancelL();
	TInt DispatchSelectNetworkL(const CMmDataPackage* aDataPackage);
	TInt DispatchSelectNetworkCancelL();
	TInt DispatchSetNetworkSelectionSettingL(const CMmDataPackage* aDataPackage);
	TInt DispatchNspsWakeupL();
	TInt DispatchSetSystemNetworkModeL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetCurrentSystemNetworkModesL();
	TInt DispatchResetNetServerL();
	TInt DispatchSetAlwaysOnModeL(const CMmDataPackage* aDataPackage);
	TInt DispatchSetDriveModeL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetHspaStatusL();
	TInt DispatchSetHspaStatusL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetNetworkProviderNameL();
	TInt DispatchGetOperatorNameL();
	TInt DispatchGetCellInfoL();
	TInt DispatchRegisterCellInfoChangeNotificationL();
	TInt DispatchGetPhoneCellInfoL();
	TInt DispatchGetUsimServiceSupportL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetCurrentActiveUsimApplicationL();
	TInt DispatchTerminateAllCallsL(const CMmDataPackage* aDataPackage);
	TInt DispatchGetSystemNetworkBandL();
	TInt DispatchSetSystemNetworkBandL(const CMmDataPackage* aDataPackage);
	
	
	// These getters are internal to CtsyDispatcher
	TInt DispatchGetCurrentNetworkInfoL();
	TInt DispatchGetNetworkModeL();
	TInt DispatchGetNitzInfoL();
	TInt DispatchGetSignalStrengthL();
	TInt DispatchGetBatteryInfoL();
	
	
	// Complete functions for receiving completions UP from the Licensee LTSY
	// via the CCtsyDispatcherCallback object.
	
	void CallbackBootNotifyModemStatusReady(TInt aError, TRfStateInfo aRfStatus);
	void CallbackBootNotifySimStatusReady(TInt aError);
	void CallbackSimRefreshRegister(TInt aError);
	void CallbackGetFdnStatus(TInt aError, RMobilePhone::TMobilePhoneFdnStatus aFdnStatus);
	void CallbackGetNetworkRegistrationStatus(TInt aError, RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus);
	void CallbackGetHomeNetwork(TInt aError,const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo);
	void CallbackNotifyNetworkRegistrationStatusChange(TInt aError, RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus);
	
	void CallbackNotifyNetworkModeChange(TInt aError, RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);
	void CallbackNotifyEgprsInfoChange(TInt aError, TBool aEdgeSupported);
	void CallbackNotifySignalStrengthChange(TInt aError, TInt32 aSignalStrength, TInt8 aBar);
	void CallbackNotifyNitzInfoChange(TInt aError, const RMobilePhone::TMobilePhoneNITZ& aNitzInfo);
	void CallbackNotifyBatteryInfoChange(TInt aError, const RMobilePhone::TMobilePhoneBatteryInfoV1& aBatteryInfo);
	void CallbackNotifyCurrentNetworkChange(TInt aError, const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo, const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	void CallbackNotifyRfStatusChange(TInt aError, TRfStateInfo aRfStatus);
	void CallbackGetServiceProviderName(TInt aError, const RMobilePhone::TMobilePhoneServiceProviderNameV2& aServiceProvider);
	void CallbackGetPhoneId(TInt aError, const RMobilePhone::TMobilePhoneIdentityV1& aPhoneId);
	void CallbackGetDetectedNetworks(TInt aError, const CMobilePhoneNetworkListV2& aPhoneNetworkList);
	void CallbackGetDetectedNetworksCancel(TInt aError);
	void CallbackSelectNetwork(TInt aError);
	void CallbackSelectNetworkCancel(TInt aError);
	void CallbackSetNetworkSelectionSetting(TInt aError, RMobilePhone::TMobilePhoneSelectionMethod aSelectionMethod);
	void CallbackGetCurrentNetworkInfo(TInt aError, const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo, const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	void CallbackGetNetworkMode(TInt aError, RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);
	void CallbackGetNitzInfo(TInt aError, const RMobilePhone::TMobilePhoneNITZ& aNitzInfo);
	void CallbackGetSignalStrength(TInt aError, TInt32 aSignalStrength, TInt8 aBar);
	void CallbackGetBatteryInfo(TInt aError, const RMobilePhone::TMobilePhoneBatteryInfoV1& aBatteryInfo);
	void CallbackNotifyNetworkSelectionSettingChange(TInt aError, RMobilePhone::TMobilePhoneSelectionMethod aSelectionMethod);
	void CallbackNotifyNspsStatusChange(TInt aError, TBool aEnabled);
	void CallbackNspsWakeup(TInt aError);
	void CallbackSetSystemNetworkMode(TInt aError);
	void CallbackGetCurrentSystemNetworkModes(TInt aError, TUint32 aCurrentSystemNetworkModes);
	void CallbackNotifyRauEvent(TInt aError, RMmCustomAPI::TRauEventStatus aRauEvent);
	void CallbackNotifyNetworkConnectionFailure(TInt aError);
 	void CallbackNotifyCellInfoChanged(TInt aError);
	void CallbackNotifyDtmfEvent(TInt aError, RMmCustomAPI::TDtmfEventType aType,
                                    RMmCustomAPI::TDtmfEvent aEvent, TChar aTone);
	void CallbackNotifyDtmfEvent(TInt aError, RMobilePhone::TMobilePhoneDTMFEvent aEventType);
	void CallbackNotifyCellInfoChanged(TInt aError, const RMmCustomAPI::TMmCellInfo::TGSMCellInfo& aInfo);
	void CallbackNotifyCellInfoChanged(TInt aError, const RMmCustomAPI::TMmCellInfo::TWCDMACellInfo& aInfo);
	void CallbackResetNetServer(TInt aError);
	void CallbackSetAlwaysOnMode(TInt aError);
	void CallbackSetDriveMode(TInt aError);
	void CallbackGetHspaStatus(TInt aError, RMmCustomAPI::THSxPAStatus aStatus);
	void CallbackSetHspaStatus(TInt aError);
	void CallbackNotifyHspaStatusChanged(TInt aError, RMmCustomAPI::THSxPAStatus aStatus);
	void CallbackGetNetworkProviderName(TInt aError, const TDesC& aName);
	void CallbackGetOperatorName(TInt aError, RMmCustomAPI::TOperatorNameType aType, const TDesC& aName);
	void CallbackGetCellInfo(TInt aError);
	void CallbackGetCellInfo(TInt aError,const RMmCustomAPI::TMmCellInfo::TGSMCellInfo &aInfo);
	void CallbackGetCellInfo(TInt aError,const RMmCustomAPI::TMmCellInfo::TWCDMACellInfo &aInfo);
	void CallbackRegisterCellInfoChangeNotification(TInt aError);
	void CallbackGetPhoneCellInfo(TInt aError, const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo);
	void CallbackGetUsimServiceSupport(TInt aError, TInt aApplicationNumber, TBool aSupported);
	void CallbackGetCurrentActiveUsimApplication(TInt aError, const TDesC8& aAid);
	void CallbackTerminateAllCalls(TInt aError);
	void CallbackGetSystemNetworkBand(TInt aError, RMmCustomAPI::TBandSelection aBand, RMmCustomAPI::TNetworkModeCaps aMode);
	void CallbackSetSystemNetworkBand(TInt aError);
	
	// From MDispatcherCallback
	void CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage);
	
	// Other public functions
	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);
	
private:

	CPhoneDispatcher(MLtsyDispatchFactoryV1& aLtsyFactory,
	   			  	 MmMessageManagerCallback& aMessageManagerCallback,
	   			  	 MmMessageManagerCallback& aSatMessageManagerCallback,
	   			  	 CRequestQueueOneShot& aRequestAsyncOneShot, 
	   			  	 MBootSequenceCallbacks& aBootSequenceObserver);		
	
	void ConstructL();	
private:	// Not owned

	MLtsyDispatchFactoryV1& iLtsyFactoryV1;
	MmMessageManagerCallback& iMessageManagerCallback;
	MmMessageManagerCallback& iSatMessageManagerCallback;
	CRequestQueueOneShot& iRequestAsyncOneShot;
	MBootSequenceCallbacks& iBootSequenceObserver;
	TDispatcherHolder* iDispatcherHolder;
	
	// Interfaces in the Licensee LTSY, not owned by this object
	
	MLtsyDispatchPhoneGetFdnStatus* iLtsyDispatchPhoneGetFdnStatus;
	MLtsyDispatchPhoneGetNetworkRegistrationStatus* iLtsyDispatchPhoneGetNetworkRegistrationStatus;
	MLtsyDispatchPhoneGetHomeNetwork* iLtsyDispatchPhoneGetHomeNetwork;
	MLtsyDispatchPhoneBootNotifyModemStatusReady* iLtsyDispatchPhoneBootNotifyModemStatusReady;
	MLtsyDispatchPhoneBootNotifySimStatusReady* iLtsyDispatchPhoneBootNotifySimStatusReady;
	MLtsyDispatchPhoneSimRefreshRegister* iLtsyDispatchPhoneSimRefreshRegister;
	
    MLtsyDispatchPhoneGetServiceProviderName* iLtsyDispatchPhoneGetServiceProviderName;
    MLtsyDispatchPhoneGetPhoneId* iLtsyDispatchPhoneGetPhoneId;
    MLtsyDispatchPhoneGetDetectedNetworks* iLtsyDispatchPhoneGetDetectedNetworks;
    MLtsyDispatchPhoneGetDetectedNetworksCancel* iLtsyDispatchPhoneGetDetectedNetworksCancel;
    MLtsyDispatchPhoneSelectNetwork* iLtsyDispatchPhoneSelectNetwork;
    MLtsyDispatchPhoneSelectNetworkCancel* iLtsyDispatchPhoneSelectNetworkCancel;
    MLtsyDispatchPhoneSetNetworkSelectionSetting* iLtsyDispatchPhoneSetNetworkSelectionSetting;
    MLtsyDispatchPhoneGetCurrentNetworkInfo* iLtsyDispatchPhoneGetCurrentNetworkInfo;
    MLtsyDispatchPhoneGetNetworkMode* iLtsyDispatchPhoneGetNetworkMode;
    MLtsyDispatchPhoneGetNitzInfo* iLtsyDispatchPhoneGetNitzInfo;
    MLtsyDispatchPhoneGetSignalStrength* iLtsyDispatchPhoneGetSignalStrength;
    MLtsyDispatchPhoneGetBatteryInfo* iLtsyDispatchPhoneGetBatteryInfo;
    MLtsyDispatchPhoneNspsWakeup* iLtsyDispatchPhoneNspsWakeup;
    MLtsyDispatchPhoneSetSystemNetworkMode* iLtsyDispatchPhoneSetSystemNetworkMode;
    MLtsyDispatchPhoneGetCurrentSystemNetworkModes* iLtsyDispatchPhoneGetCurrentSystemNetworkModes;
    MLtsyDispatchPhoneResetNetServer* iLtsyDispatchPhoneResetNetServer;
    MLtsyDispatchPhoneSetAlwaysOnMode* iLtsyDispatchPhoneSetAlwaysOnMode;
    MLtsyDispatchPhoneSetDriveMode* iLtsyDispatchPhoneSetDriveMode;
    MLtsyDispatchPhoneGetHspaStatus* iLtsyDispatchPhoneGetHspaStatus;
    MLtsyDispatchPhoneSetHspaStatus* iLtsyDispatchPhoneSetHspaStatus;
    MLtsyDispatchPhoneGetNetworkProviderName* iLtsyDispatchPhoneGetNetworkProviderName;
    MLtsyDispatchPhoneGetOperatorName* iLtsyDispatchPhoneGetOperatorName;
    MLtsyDispatchPhoneGetCellInfo* iLtsyDispatchPhoneGetCellInfo;
    MLtsyDispatchPhoneRegisterCellInfoChangeNotification* iLtsyDispatchPhoneRegisterCellInfoChangeNotification;
    MLtsyDispatchPhoneGetPhoneCellInfo* iLtsyDispatchPhoneGetPhoneCellInfo;
    MLtsyDispatchPhoneGetUsimServiceSupport* iLtsyDispatchPhoneGetUsimServiceSupport;
    MLtsyDispatchPhoneGetCurrentActiveUsimApplication* iLtsyDispatchPhoneGetCurrentActiveUsimApplication;
	MLtsyDispatchPhoneTerminateAllCalls* iLtsyDispatchPhoneTerminateAllCalls;
	MLtsyDispatchPhoneGetSystemNetworkBand* iLtsyDispatchPhoneGetSystemNetworkBand;
	MLtsyDispatchPhoneSetSystemNetworkBand* iLtsyDispatchPhoneSetSystemNetworkBand; 

private:
	TUint iPhoneIdReqOrigin; //to prevent two requests being fired off if sat and phone both 
									//request id while the other is in progress. Also to only complete
									//the appropriate request.
	
	RMmCustomAPI::TMmCellInfo *iCellInfo; // Since this object is very big we will create it once in the heap.
	
	}; // class CPhoneDispatcher

#endif // __CPHONEDISPATCHER_H_
	
