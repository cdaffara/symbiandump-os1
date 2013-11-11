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
#include "cphonedispatcherTraces.h"
#endif

#include "cphonedispatcher.h"

#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include <ctsy/rmmcustomapi.h>

#include <ctsy/ltsy/ltsylogger.h>
#include "ctsydispatcherpanic.h"
#include "cmessagerouter.h"
#include "tdispatcherholder.h"
#include <ctsy/ltsy/sat_defs.h>


const TInt KImeiBufLength=16; //the structure defined in CSatNotifyLocalInfo was 16 bytes however ETSI TS 123 003 V7.7.0 specifies 15 bytes for IMEIs.

CPhoneDispatcher::CPhoneDispatcher(
		MLtsyDispatchFactoryV1& aLtsyFactory,
		MmMessageManagerCallback& aMessageManagerCallback,
		MmMessageManagerCallback& aSatMessageManagerCallback,
		CRequestQueueOneShot& aRequestAsyncOneShot,
		MBootSequenceCallbacks& aBootSequenceObserver)
	: 	iLtsyFactoryV1(aLtsyFactory),
		iMessageManagerCallback(aMessageManagerCallback),
		iSatMessageManagerCallback(aSatMessageManagerCallback),
		iRequestAsyncOneShot(aRequestAsyncOneShot),
		iBootSequenceObserver(aBootSequenceObserver)
	{
	} // CPhoneDispatcher::CPhoneDispatcher

	  
CPhoneDispatcher::~CPhoneDispatcher()
	{
	delete iCellInfo;
	} // CPhoneDispatcher::~CPhoneDispatcher


CPhoneDispatcher* CPhoneDispatcher::NewLC(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	MmMessageManagerCallback& aSatMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot,
	MBootSequenceCallbacks& aBootSequenceObserver)
	{
	TSYLOGENTRYEXIT;
	CPhoneDispatcher* self =
		new (ELeave) CPhoneDispatcher(aLtsyFactory, aMessageManagerCallback, aSatMessageManagerCallback, aRequestAsyncOneShot, aBootSequenceObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CPhoneDispatcher::NewLC


CPhoneDispatcher* CPhoneDispatcher::NewL(
	MLtsyDispatchFactoryV1& aLtsyFactory,
	MmMessageManagerCallback& aMessageManagerCallback,
	MmMessageManagerCallback& aSatMessageManagerCallback,
	CRequestQueueOneShot& aRequestAsyncOneShot,
	MBootSequenceCallbacks& aBootSequenceObserver)
	{
	TSYLOGENTRYEXIT;
	CPhoneDispatcher* self =
		CPhoneDispatcher::NewLC(aLtsyFactory, aMessageManagerCallback, aSatMessageManagerCallback, aRequestAsyncOneShot, aBootSequenceObserver);
	CleanupStack::Pop (self);
	return self;
	} // CPhoneDispatcher::NewL


void CPhoneDispatcher::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	// Get the Licensee LTSY interfaces related to Phone functionality
	// from the factory
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId))
		{
		TAny* bootNotifyModemStatusReadyInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
			MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId,
			bootNotifyModemStatusReadyInterface);
		iLtsyDispatchPhoneBootNotifyModemStatusReady =
				static_cast<MLtsyDispatchPhoneBootNotifyModemStatusReady*>(bootNotifyModemStatusReadyInterface);
		__ASSERT_DEBUG(iLtsyDispatchPhoneBootNotifyModemStatusReady, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId))
		{
	    TAny* getBootNotifySimStatusReadyInterface = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId,
	    		getBootNotifySimStatusReadyInterface);
		iLtsyDispatchPhoneBootNotifySimStatusReady =
			static_cast<MLtsyDispatchPhoneBootNotifySimStatusReady*>(getBootNotifySimStatusReadyInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneBootNotifySimStatusReady, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId))
		{
	    TAny* getFdnStatusInterface = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId,
	    		getFdnStatusInterface);
		iLtsyDispatchPhoneGetFdnStatus =
			static_cast<MLtsyDispatchPhoneGetFdnStatus*>(getFdnStatusInterface);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetFdnStatus, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId))
		{
	    TAny* getRegistrationStatusInterface = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId,
	    		getRegistrationStatusInterface);
		iLtsyDispatchPhoneGetNetworkRegistrationStatus =
			static_cast<MLtsyDispatchPhoneGetNetworkRegistrationStatus*>(getRegistrationStatusInterface);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetNetworkRegistrationStatus, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId))
		{
	    TAny* getHomeNetworkInterface = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId,
	    		getHomeNetworkInterface);
		iLtsyDispatchPhoneGetHomeNetwork =
			static_cast<MLtsyDispatchPhoneGetHomeNetwork*>(getHomeNetworkInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneGetHomeNetwork, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId))
		{
		TAny* simRefreshRegisterInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId,
		       	simRefreshRegisterInterface);
		iLtsyDispatchPhoneSimRefreshRegister =
				static_cast<MLtsyDispatchPhoneSimRefreshRegister*>(simRefreshRegisterInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneSimRefreshRegister, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId))
		{
		TAny* getServiceProviderNameInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId,
		       	getServiceProviderNameInterface);
		iLtsyDispatchPhoneGetServiceProviderName =
				static_cast<MLtsyDispatchPhoneGetServiceProviderName*>(getServiceProviderNameInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneGetServiceProviderName, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId))
		{
		TAny* getPhoneIdInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId,
		       	getPhoneIdInterface);
		iLtsyDispatchPhoneGetPhoneId =
				static_cast<MLtsyDispatchPhoneGetPhoneId*>(getPhoneIdInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneGetPhoneId, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId))
		{
		TAny* getDetectedNetworksInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId,
		       	getDetectedNetworksInterface);
		iLtsyDispatchPhoneGetDetectedNetworks =
				static_cast<MLtsyDispatchPhoneGetDetectedNetworks*>(getDetectedNetworksInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneGetDetectedNetworks, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId))
		{
		TAny* getDetectedNetworksCancelInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId,
		       	getDetectedNetworksCancelInterface);
		iLtsyDispatchPhoneGetDetectedNetworksCancel =
				static_cast<MLtsyDispatchPhoneGetDetectedNetworksCancel*>(getDetectedNetworksCancelInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneGetDetectedNetworksCancel, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId))
		{
		TAny* selectNetworkInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId,
		       	selectNetworkInterface);
		iLtsyDispatchPhoneSelectNetwork =
				static_cast<MLtsyDispatchPhoneSelectNetwork*>(selectNetworkInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneSelectNetwork, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId))
		{
		TAny* selectNetworkCancelInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId,
		       	selectNetworkCancelInterface);
		iLtsyDispatchPhoneSelectNetworkCancel =
				static_cast<MLtsyDispatchPhoneSelectNetworkCancel*>(selectNetworkCancelInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneSelectNetworkCancel, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId))
		{
		TAny* setNetworkSelectionSettingInterface = NULL;
		iLtsyFactoryV1.GetDispatchHandler(
		       	MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId,
		       	setNetworkSelectionSettingInterface);
		iLtsyDispatchPhoneSetNetworkSelectionSetting =
				static_cast<MLtsyDispatchPhoneSetNetworkSelectionSetting*>(setNetworkSelectionSettingInterface);
        __ASSERT_DEBUG(iLtsyDispatchPhoneSetNetworkSelectionSetting, CtsyDispatcherPanic(EInvalidNullPtr));
  		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId))
		{
	    TAny* getCurrentNetworkInfo = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId,
	    		getCurrentNetworkInfo);
	    iLtsyDispatchPhoneGetCurrentNetworkInfo = 
				static_cast<MLtsyDispatchPhoneGetCurrentNetworkInfo*>(getCurrentNetworkInfo);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetCurrentNetworkInfo, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId))
		{
	    TAny* getNetworkMode = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId,
	    		getNetworkMode);
	    iLtsyDispatchPhoneGetNetworkMode = 
				static_cast<MLtsyDispatchPhoneGetNetworkMode*>(getNetworkMode);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetNetworkMode, CtsyDispatcherPanic(EInvalidNullPtr));
		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId))
		{
	    TAny* getNitzInfo = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId,
	    		getNitzInfo);
	    iLtsyDispatchPhoneGetNitzInfo = 
				static_cast<MLtsyDispatchPhoneGetNitzInfo*>(getNitzInfo);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetNitzInfo, CtsyDispatcherPanic(EInvalidNullPtr));
		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId))
		{
	    TAny* getSignalStrength = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId,
	    		getSignalStrength);
	    iLtsyDispatchPhoneGetSignalStrength =
				static_cast<MLtsyDispatchPhoneGetSignalStrength*>(getSignalStrength);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetSignalStrength, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId))
		{
	    TAny* getBatteryInfo = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId,
	    		getBatteryInfo);
	    iLtsyDispatchPhoneGetBatteryInfo = 
				static_cast<MLtsyDispatchPhoneGetBatteryInfo*>(getBatteryInfo);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetBatteryInfo, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId))
		{
	    TAny* nspsWakeup = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId,
	    		nspsWakeup);
	    iLtsyDispatchPhoneNspsWakeup = 
				static_cast<MLtsyDispatchPhoneNspsWakeup*>(nspsWakeup);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneNspsWakeup, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId))
		{
	    TAny* setSystemNetworkMode = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId,
	    		setSystemNetworkMode);
	    iLtsyDispatchPhoneSetSystemNetworkMode = 
				static_cast<MLtsyDispatchPhoneSetSystemNetworkMode*>(setSystemNetworkMode);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneSetSystemNetworkMode, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId))
		{
	    TAny* getCurrentSystemNetworkModes = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId,
	    		getCurrentSystemNetworkModes);
	    iLtsyDispatchPhoneGetCurrentSystemNetworkModes = 
				static_cast<MLtsyDispatchPhoneGetCurrentSystemNetworkModes*>(getCurrentSystemNetworkModes);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetCurrentSystemNetworkModes, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId))
		{
	    TAny* resetNetServer = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId,
	    		resetNetServer);
	    iLtsyDispatchPhoneResetNetServer = 
				static_cast<MLtsyDispatchPhoneResetNetServer*>(resetNetServer);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneResetNetServer, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId))
		{
	    TAny* setAlwaysOnMode = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId,
	    		setAlwaysOnMode);
	    iLtsyDispatchPhoneSetAlwaysOnMode = 
				static_cast<MLtsyDispatchPhoneSetAlwaysOnMode*>(setAlwaysOnMode);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneSetAlwaysOnMode, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId))
		{
	    TAny* setDriveMode = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId,
	    		setDriveMode);
	    iLtsyDispatchPhoneSetDriveMode = 
				static_cast<MLtsyDispatchPhoneSetDriveMode*>(setDriveMode);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneSetDriveMode, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId))
		{
	    TAny* getHspaStatus = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId,
	    		getHspaStatus);
	    iLtsyDispatchPhoneGetHspaStatus = 
				static_cast<MLtsyDispatchPhoneGetHspaStatus*>(getHspaStatus);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetHspaStatus, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId))
		{
	    TAny* setHspaStatus = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId,
	    		setHspaStatus);
	    iLtsyDispatchPhoneSetHspaStatus = 
				static_cast<MLtsyDispatchPhoneSetHspaStatus*>(setHspaStatus);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneSetHspaStatus, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId))
		{
	    TAny* getNetworkProviderName = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId,
	    		getNetworkProviderName);
	    iLtsyDispatchPhoneGetNetworkProviderName = 
				static_cast<MLtsyDispatchPhoneGetNetworkProviderName*>(getNetworkProviderName);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetNetworkProviderName, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId))
		{
	    TAny* getOperatorName = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId,
	    		getOperatorName);
	    iLtsyDispatchPhoneGetOperatorName = 
				static_cast<MLtsyDispatchPhoneGetOperatorName*>(getOperatorName);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetOperatorName, CtsyDispatcherPanic(EInvalidNullPtr));
		}

	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId))
		{
	    TAny* getCellInfo = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId,
	    		getCellInfo);
	    iLtsyDispatchPhoneGetCellInfo = 
				static_cast<MLtsyDispatchPhoneGetCellInfo*>(getCellInfo);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetCellInfo, CtsyDispatcherPanic(EInvalidNullPtr));
		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId))
		{
		TAny* registerCellInfoChangeNotification = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId,
	    		registerCellInfoChangeNotification);
	    iLtsyDispatchPhoneRegisterCellInfoChangeNotification = 
				static_cast<MLtsyDispatchPhoneRegisterCellInfoChangeNotification*>(registerCellInfoChangeNotification);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneRegisterCellInfoChangeNotification, CtsyDispatcherPanic(EInvalidNullPtr));
		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId))
		{
	    TAny* getPhoneCellInfo = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId,
	    		getPhoneCellInfo);
	    iLtsyDispatchPhoneGetPhoneCellInfo = 
				static_cast<MLtsyDispatchPhoneGetPhoneCellInfo*>(getPhoneCellInfo);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetPhoneCellInfo, CtsyDispatcherPanic(EInvalidNullPtr));
		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId))
		{
	    TAny* getUsimServiceSupport = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId,
	    		getUsimServiceSupport);
	    iLtsyDispatchPhoneGetUsimServiceSupport = 
				static_cast<MLtsyDispatchPhoneGetUsimServiceSupport*>(getUsimServiceSupport);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetUsimServiceSupport, CtsyDispatcherPanic(EInvalidNullPtr));
		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId))
		{
	    TAny* terminateAllCalls = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId,
	    		terminateAllCalls);
	    iLtsyDispatchPhoneTerminateAllCalls= 
				static_cast<MLtsyDispatchPhoneTerminateAllCalls*>(terminateAllCalls);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneTerminateAllCalls, CtsyDispatcherPanic(EInvalidNullPtr));
		}


	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId))
		{
	    TAny* getCurrentActiveUsimApplication = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId,
	    		getCurrentActiveUsimApplication);
	    iLtsyDispatchPhoneGetCurrentActiveUsimApplication = 
				static_cast<MLtsyDispatchPhoneGetCurrentActiveUsimApplication*>(getCurrentActiveUsimApplication);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetCurrentActiveUsimApplication, CtsyDispatcherPanic(EInvalidNullPtr));
		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId))
		{
	    TAny* getSystemNetworkBand = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId,
	    		getSystemNetworkBand);
	    iLtsyDispatchPhoneGetSystemNetworkBand = 
				static_cast<MLtsyDispatchPhoneGetSystemNetworkBand*>(getSystemNetworkBand);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneGetSystemNetworkBand, CtsyDispatcherPanic(EInvalidNullPtr));
		}
	
	if(iLtsyFactoryV1.IsDispatchInterfaceSupported(KDispatchPhoneFuncUnitId, MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId))
		{
	    TAny* setSystemNetworkBand = NULL;
	    iLtsyFactoryV1.GetDispatchHandler(
	    		MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId,
	    		setSystemNetworkBand);
	    iLtsyDispatchPhoneSetSystemNetworkBand = 
				static_cast<MLtsyDispatchPhoneSetSystemNetworkBand*>(setSystemNetworkBand);
  		__ASSERT_DEBUG(iLtsyDispatchPhoneSetSystemNetworkBand, CtsyDispatcherPanic(EInvalidNullPtr));
		}


	
	iCellInfo = new(ELeave) RMmCustomAPI::TMmCellInfo; 
	} // CPhoneDispatcher::ConstructL

void CPhoneDispatcher::SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder)
/**
 * Set the dispatcher holder.  
 * 
 * @param aDispatcherHolder Reference to dispatcher holder.
 */
	{
	TSYLOGENTRYEXIT;
	
	iDispatcherHolder = &aDispatcherHolder;
	} // CPhoneDispatcher::SetDispatcherHolder
	
TInt CPhoneDispatcher::DispatchGetFdnStatusL()
/**
 * Pass an EMobilePhoneGetFdnStatus request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	if (iLtsyDispatchPhoneGetFdnStatus)
		{
		ret = iLtsyDispatchPhoneGetFdnStatus->HandleGetFdnStatusReqL();
		}
	
	return TSYLOGSETEXITERR(ret); 
	} // CPhoneDispatcher::DispatchGetFdnStatusL


TInt CPhoneDispatcher::DispatchGetNetworkRegistrationStatusL()
/**
 * Pass an EMobilePhoneGetNetworkRegistrationStatus request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetNetworkRegistrationStatus)
		{
		ret = iLtsyDispatchPhoneGetNetworkRegistrationStatus->HandleGetNetworkRegistrationStatusReqL();
		}
	
	return TSYLOGSETEXITERR(ret); 	
	} // CPhoneDispatcher::DispatchGetNetworkRegistrationStatusL


TInt CPhoneDispatcher::DispatchGetHomeNetworkL()
/**
 * Pass an EMobilePhoneGetHomeNetwork request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetHomeNetwork)
		{
		ret = iLtsyDispatchPhoneGetHomeNetwork->HandleGetHomeNetworkReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetHomeNetworkL


TInt CPhoneDispatcher::DispatchBootNotifyModemStatusReadyL()
/**
 * Pass an EMmTsyBootNotifyModemStatusReadyIPC request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneBootNotifyModemStatusReady)
		{
		ret = iLtsyDispatchPhoneBootNotifyModemStatusReady->HandleBootNotifyModemStatusReadyReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchBootNotifyModemStatusReadyL


TInt CPhoneDispatcher::DispatchBootNotifySimStatusReadyL()
/**
 * Pass an EMmTsyBootNotifySimStatusReadyIPC request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneBootNotifySimStatusReady)
		{
		ret = iLtsyDispatchPhoneBootNotifySimStatusReady->HandleBootNotifySimStatusReadyReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchBootNotifySimStatusReadyL


TInt CPhoneDispatcher::DispatchSimRefreshRegisterL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMmTsySimRefreshRegisterIPC
 * and pass request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPhoneSimRefreshRegister)
		{
		TUint16 refreshFileList = 0;
		aDataPackage->UnPackData(refreshFileList); 
			
		ret = iLtsyDispatchPhoneSimRefreshRegister->HandleSimRefreshRegisterReqL(refreshFileList); 
		}  
	
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchSimRefreshRegisterL


TInt CPhoneDispatcher::DispatchGetServiceProviderNameL()
/**
 * Pass an EMobilePhoneGetServiceProviderName request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	//no data to unpack

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetServiceProviderName)
		{
		ret = iLtsyDispatchPhoneGetServiceProviderName->HandleGetServiceProviderNameReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhoneDispatcher::DispatchGetServiceProviderNameL

TInt CPhoneDispatcher::DispatchGetPhoneIdL(TUint aRequestOrigin)
/**
 * Unpack data related to EMobilePhoneGetPhoneId
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetPhoneId && 0==iPhoneIdReqOrigin) 
		{
		ret = iLtsyDispatchPhoneGetPhoneId->HandleGetPhoneIdReqL();
		}
	iPhoneIdReqOrigin |= aRequestOrigin;

	return TSYLOGSETEXITERR(ret);
	} // CPhoneDispatcher::DispatchGetPhoneIdL

TInt CPhoneDispatcher::DispatchGetDetectedNetworksL()
/**
 * Pass an EMobilePhoneGetDetectedNetworksV2Phase1 request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetDetectedNetworks)
		{
		ret = iLtsyDispatchPhoneGetDetectedNetworks->HandleGetDetectedNetworksReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhoneDispatcher::DispatchGetDetectedNetworksL

TInt CPhoneDispatcher::DispatchGetDetectedNetworksCancelL()
/**
 * Pass an EMobilePhoneGetDetectedNetworksCancel request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetDetectedNetworksCancel)
		{
		ret = iLtsyDispatchPhoneGetDetectedNetworksCancel->HandleGetDetectedNetworksCancelReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhoneDispatcher::DispatchGetDetectedNetworksCancelL

TInt CPhoneDispatcher::DispatchSelectNetworkL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSelectNetwork
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
	if (iLtsyDispatchPhoneSelectNetwork)
		{
		TBool* isManual = NULL;
		RMobilePhone::TMobilePhoneNetworkManualSelection* networkManualSelection = NULL;

		aDataPackage->UnPackData(&isManual, &networkManualSelection);
		
		__ASSERT_DEBUG(isManual, CtsyDispatcherPanic(EInvalidNullPtr));
		__ASSERT_DEBUG(networkManualSelection, CtsyDispatcherPanic(EInvalidNullPtr));		
			
		ret = iLtsyDispatchPhoneSelectNetwork->HandleSelectNetworkReqL(*isManual, *networkManualSelection);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhoneDispatcher::DispatchSelectNetworkL

TInt CPhoneDispatcher::DispatchSelectNetworkCancelL()
/**
 * Pass an EMobilePhoneSelectNetworkCancel request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneSelectNetworkCancel)
		{
		ret = iLtsyDispatchPhoneSelectNetworkCancel->HandleSelectNetworkCancelReqL();
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhoneDispatcher::DispatchSelectNetworkCancelL

TInt CPhoneDispatcher::DispatchSetNetworkSelectionSettingL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to EMobilePhoneSetNetworkSelectionSetting
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
	if (iLtsyDispatchPhoneSetNetworkSelectionSetting)
		{
		RMobilePhone::TMobilePhoneNetworkSelectionV1* networkSetting = NULL;
		aDataPackage->UnPackData(&networkSetting);
		
		__ASSERT_DEBUG(networkSetting, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchPhoneSetNetworkSelectionSetting->HandleSetNetworkSelectionSettingReqL(*networkSetting);
		}

	return TSYLOGSETEXITERR(ret);
	} // CPhoneDispatcher::DispatchSetNetworkSelectionSettingL

TInt CPhoneDispatcher::DispatchGetCurrentNetworkInfoL()
/**
 * This method fetches the current network info from the LTSY.
 * It is not an exposed api to the CTSY and therefore is not the handler for the RMobilePhone::GetCurrentNetwork api.
 * It's purpose is to fetch the info and store it in the Ctsy's cache.  
 * Pass the request on to LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetCurrentNetworkInfo)
		{
		ret = iLtsyDispatchPhoneGetCurrentNetworkInfo->HandleGetCurrentNetworkInfoReqL(); 
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetCurrentNetworkInfoL
	
TInt CPhoneDispatcher::DispatchGetNetworkModeL()
/**
 * This method fetches the current network info from the LTSY.
 * It is not an exposed api to the CTSY.
 * It's purpose is to fetch the mode and store it in the Ctsy's cache.  
 * Pass the request on to LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetNetworkMode)
		{
		ret = iLtsyDispatchPhoneGetNetworkMode->HandleGetNetworkModeReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetNetworkModeL

TInt CPhoneDispatcher::DispatchGetNitzInfoL()
/**
 * This method fetches the NITZ info from the LTSY.
 * It is not an exposed api to the CTSY.
 * It's purpose is to fetch the mode and store it in the Ctsy's cache.  
 * Pass the request on to LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetNitzInfo)
		{
		ret = iLtsyDispatchPhoneGetNitzInfo->HandleGetNitzInfoReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetNitzInfoL

TInt CPhoneDispatcher::DispatchGetSignalStrengthL()
/**
 * This method fetches the signal strength info from the LTSY.
 * It is not an exposed api to the CTSY.
 * It's purpose is to fetch the signal strength and store it in the Ctsy's cache.  
 * Pass the request on to LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetSignalStrength)
		{
		ret = iLtsyDispatchPhoneGetSignalStrength->HandleGetSignalStrengthReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetSignalStrengthL

TInt CPhoneDispatcher::DispatchGetBatteryInfoL()
/**
 * This method fetches the current battery info from the LTSY.
 * It is not an exposed api to the CTSY.
 * It's purpose is to fetch the battery info and store it in the Ctsy's cache.  
 * Pass the request on to LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetBatteryInfo)
		{
		ret = iLtsyDispatchPhoneGetBatteryInfo->HandleGetBatteryInfoReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetBatteryInfoL

TInt CPhoneDispatcher::DispatchNspsWakeupL()
/**
 * Pass an ECustomNetWakeupIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneNspsWakeup)
		{
		ret = iLtsyDispatchPhoneNspsWakeup->HandleNspsWakeupReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchNspsWakeupL

TInt CPhoneDispatcher::DispatchSetSystemNetworkModeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSetSystemNetworkModeIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPhoneSetSystemNetworkMode)
		{
		RMmCustomAPI::TNetworkModeCaps networkMode;
		aDataPackage->UnPackData(networkMode); 
			
		ret = iLtsyDispatchPhoneSetSystemNetworkMode->HandleSetSystemNetworkModeReqL(networkMode); 
		}  
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchSetSystemNetworkModeL

TInt CPhoneDispatcher::DispatchGetCurrentSystemNetworkModesL()
/**
 * Pass an ECustomGetCurrentSystemNetworkModesIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetCurrentSystemNetworkModes)
		{
		ret = iLtsyDispatchPhoneGetCurrentSystemNetworkModes->HandleGetCurrentSystemNetworkModesReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetCurrentSystemNetworkModesL

TInt CPhoneDispatcher::DispatchResetNetServerL()
/**
 * Pass an ECustomResetNetServerIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	if (iLtsyDispatchPhoneResetNetServer)
		{
		ret = iLtsyDispatchPhoneResetNetServer->HandleResetNetServerReqL(); 
		}  
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchResetNetServerL

TInt CPhoneDispatcher::DispatchSetAlwaysOnModeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSetAlwaysOnMode
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPhoneSetAlwaysOnMode)
		{
		RMmCustomAPI::TSetAlwaysOnMode alwaysOnModeMode;
		aDataPackage->UnPackData(alwaysOnModeMode); 
			
		ret = iLtsyDispatchPhoneSetAlwaysOnMode->HandleSetAlwaysOnModeReqL(alwaysOnModeMode); 
		}  
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchSetAlwaysOnModeL

TInt CPhoneDispatcher::DispatchSetDriveModeL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomSetDriveModeIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPhoneSetDriveMode)
		{
		RMmCustomAPI::TSetDriveMode driveMode;
		aDataPackage->UnPackData(driveMode); 
			
		ret = iLtsyDispatchPhoneSetDriveMode->HandleSetDriveModeReqL(driveMode); 
		}  
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchSetDriveModeL

TInt CPhoneDispatcher::DispatchGetHspaStatusL()
/**
 * Pass an ECustomReadHSxPAStatusIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetHspaStatus)
		{
		ret = iLtsyDispatchPhoneGetHspaStatus->HandleGetHspaStatusReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetHspaStatusL

TInt CPhoneDispatcher::DispatchSetHspaStatusL(const CMmDataPackage* aDataPackage)
/**
 * Unpack data related to ECustomWriteHSxPAStatusIPC
 * and pass request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));

	if (iLtsyDispatchPhoneSetHspaStatus)
		{
		RMmCustomAPI::THSxPAStatus status;
		aDataPackage->UnPackData(status); 
			
		ret = iLtsyDispatchPhoneSetHspaStatus->HandleSetHspaStatusReqL(status); 
		}  
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchSetHspaStatusL

TInt CPhoneDispatcher::DispatchGetNetworkProviderNameL()
/**
 * Pass an ECustomGetNetworkProviderNameIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetNetworkProviderName)
		{
		ret = iLtsyDispatchPhoneGetNetworkProviderName->HandleGetNetworkProviderNameReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetNetworkProviderNameL

TInt CPhoneDispatcher::DispatchGetOperatorNameL()
/**
 * Pass an ECustomGetOperatorNameIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetOperatorName)
		{
		ret = iLtsyDispatchPhoneGetOperatorName->HandleGetOperatorNameReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetOperatorNameL

TInt CPhoneDispatcher::DispatchGetCellInfoL()
/**
 * Pass an ECustomGetCellInfoIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetCellInfo)
		{
		ret = iLtsyDispatchPhoneGetCellInfo->HandleGetCellInfoReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetCellInfoL

TInt CPhoneDispatcher::DispatchRegisterCellInfoChangeNotificationL()
/**
 * Pass an ECtsyPhoneCellInfoIndReq request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneRegisterCellInfoChangeNotification)
		{
		ret = iLtsyDispatchPhoneRegisterCellInfoChangeNotification->HandleRegisterCellInfoChangeNotificationReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchRegisterCellInfoChangeNotificationL

TInt CPhoneDispatcher::DispatchGetPhoneCellInfoL()
/**
 * Pass an ECtsyPhoneCellInfoReq request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetPhoneCellInfo)
		{
		ret = iLtsyDispatchPhoneGetPhoneCellInfo->HandleGetPhoneCellInfoReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetPhoneCellInfoL

TInt CPhoneDispatcher::DispatchGetUsimServiceSupportL(const CMmDataPackage* aDataPackage)
/**
 * Pass an ECustomGetServiceTableSupportbyApplicationIPC request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetUsimServiceSupport)
		{
		RMmCustomAPI::TAppSupport* appSupport = NULL;
		
		aDataPackage->UnPackData(&appSupport);
		__ASSERT_DEBUG(appSupport, CtsyDispatcherPanic(EInvalidNullPtr));
		
		ret = iLtsyDispatchPhoneGetUsimServiceSupport->HandleGetGetUsimServiceSupportReqL(appSupport->iAppNum);
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetUsimServiceSupportL

TInt CPhoneDispatcher::DispatchGetCurrentActiveUsimApplicationL()
/**
 * Pass an EMobilePhoneGetCurrentActiveUSimApplication request on to Licensee LTSY.
 * 
 * @return Returns error code returned by the LTSY or KErrNotSupported if
 * the LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;

	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetCurrentActiveUsimApplication)
		{
		ret = iLtsyDispatchPhoneGetCurrentActiveUsimApplication->HandleGetCurrentActiveUsimApplicationReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetCurrentActiveUsimApplicationL

TInt CPhoneDispatcher::DispatchTerminateAllCallsL(const CMmDataPackage* aDataPackage)
/**
 * Pass an ECtsyPhoneTerminateAllCallsReq request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;	
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneTerminateAllCalls)
		{
		TInt callId=0;
		aDataPackage->UnPackData(callId);
		ret = iLtsyDispatchPhoneTerminateAllCalls->HandleTerminateAllCallsReqL(callId);
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchTerminateAllCallsL

TInt CPhoneDispatcher::DispatchGetSystemNetworkBandL()
/**
 * Pass an ECustomGetBandSelectionIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;	
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneGetSystemNetworkBand)
		{
		ret = iLtsyDispatchPhoneGetSystemNetworkBand->HandleGetSystemNetworkBandReqL();
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchGetSystemNetworkBandL

TInt CPhoneDispatcher::DispatchSetSystemNetworkBandL(const CMmDataPackage* aDataPackage)
/**
 * Pass an ECustomSetBandSelectionIPC request on to Licensee LTSY.
 *
 * @return Returns error code returned by the Licensee LTSY or KErrNotSupported if
 * the Licensee LTSY does not support this request.
 */
	{
	TSYLOGENTRYEXIT;
	TInt ret = KErrNotSupported;	
	
	__ASSERT_DEBUG(aDataPackage, CtsyDispatcherPanic(EInvalidNullPtr));
	
	// Call Handle... method in Licensee LTSY
	if (iLtsyDispatchPhoneSetSystemNetworkBand)
		{
		RMmCustomAPI::TBandSelection band;
		RMmCustomAPI::TNetworkModeCaps mode;
		aDataPackage->UnPackData(band, mode);
		ret = iLtsyDispatchPhoneSetSystemNetworkBand->HandleSetSystemNetworkBandReqL(band, mode);
		}
		
	return TSYLOGSETEXITERR(ret);	
	} // CPhoneDispatcher::DispatchSetSystemNetworkBandL

//
// Callback handlers follow
//


void CPhoneDispatcher::CallbackGetFdnStatus(TInt aError, RMobilePhone::TMobilePhoneFdnStatus aFdnStatus)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * RMobilePhone::GetFdnStatus request back to the Common TSY.
 * 
 * @param aError The error code to complete back.
 * @param aFdnStatus The FDN status to complete back. 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aFdnStatus=%d"), aError, aFdnStatus);
	
	// Package return type to return to Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aFdnStatus);
	
	iMessageManagerCallback.Complete(EMobilePhoneGetFdnStatus, &dataPackage, aError);

	} // CPhoneDispatcher::CallbackGetFdnStatus


void CPhoneDispatcher::CallbackBootNotifyModemStatusReady(TInt aError, TRfStateInfo aRfStatus)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * EMmTsyBootNotifyModemStatusReadyIPC request back to the Common TSY.
 * 
 * @param aError The error code to complete back.
 * @param aRfStatus RF status. 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMmTsyBootNotifyModemStatusReadyIPC, aError);
	
	CMmDataPackage data;
	data.PackData(&aRfStatus);
	//also complete the RF status indicator
	iMessageManagerCallback.Complete(EMmTsyBootGetRFStatusIPC, &data, aError);
		
	// The receipt of this callback is part of the boot sequence so signal back
	TRAP_IGNORE(iBootSequenceObserver.HandleNotifyModemReadyReceivedL());
	
	} // CPhoneDispatcher::CallbackBootNotifyModemStatusReady


void CPhoneDispatcher::CallbackBootNotifySimStatusReady(TInt aError)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * EMmTsyBootNotifySimStatusReadyIPC request back to the Common TSY.
 * 
 * @param aError The error code to complete back. 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMmTsyBootNotifySimStatusReadyIPC, aError);
	} // CPhoneDispatcher::CallbackBootNotifySimStatusReady


void CPhoneDispatcher::CallbackSimRefreshRegister(TInt aError)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * EMmTsySimRefreshRegisterIPC request back to the Common TSY.
 * 
 * @param aError The error code to complete back. 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMmTsySimRefreshRegisterIPC, aError);
	} // CPhoneDispatcher::CallbackSimRefreshRegister


void CPhoneDispatcher::CallbackGetNetworkRegistrationStatus(
	TInt aError, RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * RMobilePhone::GetNetworkRegistrationStatus back to the Common TSY.
 * 
 * @param aError The error code to complete back. 
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// The Common TSY expects a notify registration status request to complete
	// when the get request completes. Hide this behaviour from the Licensee LTSY
	// in this CTSY Dispatcher.
	CallbackNotifyNetworkRegistrationStatusChange(aError, aRegStatus);	

	// Now complete the actual get request
	iMessageManagerCallback.Complete(EMobilePhoneGetNetworkRegistrationStatus, aError);
	
	} // CPhoneDispatcher::CallbackGetNetworkRegistrationStatus


void CPhoneDispatcher::CallbackNotifyNetworkRegistrationStatusChange(
		TInt aError, RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus)
/**
 * Packages the data returned by the Licensee LTSY and completes the 
 * RMobilePhone::NotifyNetworkRegistrationStatusChange back to the Common TSY.
 * 
 * @param aError The error code to complete back.
 * @param aRegStatus The phone registration status to complete back.
 */
  {
  TSYLOGENTRYEXITARGS(_L8("aError=%d, aRegStatus=%d"), aError, aRegStatus);
  
  // CTSY expects a network status parameter. But it never makes use of it. Pass a dummy value. 
  RMobilePhone::TMobilePhoneNetworkStatus dummyNetStatus = RMobilePhone::ENetworkStatusUnknown;
  
  CMmDataPackage dataPackage;
  dataPackage.PackData(
		  static_cast<RMobilePhone::TMobilePhoneRegistrationStatus*>(&aRegStatus), 
		  static_cast<RMobilePhone::TMobilePhoneNetworkStatus*>(&dummyNetStatus));
  
  iMessageManagerCallback.Complete(EMobilePhoneNotifyNetworkRegistrationStatusChange, 
		  &dataPackage, aError);
  } // CPhoneDispatcher::CallbackNotifyNetworkRegistrationStatusChange


void CPhoneDispatcher::CallbackGetHomeNetwork(TInt aError, 
					const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo)
/**
 * Callback function to be used by the request.
 * 
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aNetworkInfo Network information returned by the LTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneNetworkInfoV5*>(&aNetworkInfo));
	
	// Now complete the actual get request
	iMessageManagerCallback.Complete(EMobilePhoneGetHomeNetwork, &dataPackage, aError);
	} // CPhoneDispatcher::CallbackGetHomeNetwork


void CPhoneDispatcher::CallbackNotifyNetworkModeChange(TInt aError,
		RMobilePhone::TMobilePhoneNetworkMode aNetworkMode)
/**
 * Callback function to be used by the request to complete a NotifyModeChange.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aNetworkMode The network mode to be sent to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(static_cast<RMobilePhone::TMobilePhoneNetworkMode*>(&aNetworkMode));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyModeChange, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyNetworkModeChange

void CPhoneDispatcher::CallbackNotifyEgprsInfoChange(TInt aError, TBool aEdgeSupported)
/**
 * Callback function to be used by the request to complete a NotifyEgprsInfoChange.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aEdgeSupported If EGPRS is supported on the current cell.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aEdgeSupported=%d"), aError, aEdgeSupported);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aEdgeSupported);
	
	// note aError isn't used in the CTSY
	iMessageManagerCallback.Complete(EMmTsyNotifyEGprsInfoChangeIPC, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyEgprsInfoChange

void CPhoneDispatcher::CallbackNotifySignalStrengthChange(TInt aError,
		TInt32 aSignalStrength,
		TInt8 aBar)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifySignalStrengthChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSignalStrength The signal strength to be sent to the CTSY Dispatcher
 * @param aBar The number of bars to display.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	// Note: Ctsy expects the data to be packed in the order <bar, signalStrength>,
	// so swap the order here.
	dataPackage.PackData(static_cast<TInt8*>(&aBar), static_cast<TInt32*>(&aSignalStrength));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifySignalStrengthChange, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifySignalStrengthChange

void CPhoneDispatcher::CallbackNotifyNitzInfoChange(TInt aError,
				const RMobilePhone::TMobilePhoneNITZ& aNitzInfo)
/**
 * Callback function to be used by the request to complete a NotifyNitzInfoChange.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aNitzInfo The current NITZ information to be sent to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneNITZ*>(&aNitzInfo));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyNITZInfoChange, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyNitzInfoChange

void CPhoneDispatcher::CallbackNotifyBatteryInfoChange(TInt aError,
				const RMobilePhone::TMobilePhoneBatteryInfoV1& aBatteryInfo)
/**
 * Callback function to be used by the request to complete a NotifyBatteryInfoChange.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aBatteryInfo The battery information to be sent to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneBatteryInfoV1*>(&aBatteryInfo));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyBatteryInfoChange, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyBatteryInfoChange

void CPhoneDispatcher::CallbackNotifyCurrentNetworkChange(TInt aError,
			const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo,
			const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
/**
 * Callback function to complete an RMobilePhone::NotifyCurrentNetworkChange request
 * to the Common TSY.
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aNetworkInfo The network information to be sent to the CTSY Dispatcher.
 * 		  Note the actual data type contained could be of type TMobilePhoneNetworkInfoV2 or TMobilePhoneNetworkInfoV1.
 * @param aLocationArea The mobile phone location area.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneNetworkInfoV5*>(&aNetworkInfo),
						 const_cast<RMobilePhone::TMobilePhoneLocationAreaV1*>(&aLocationArea));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyCurrentNetworkChange, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyCurrentNetworkChange

void CPhoneDispatcher::CallbackNotifyRfStatusChange(TInt aError, TRfStateInfo aRfStatus)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyRfStatusChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aRfStatus new RF status.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	data.PackData(&aRfStatus);
	//also complete the RF status indicator
	iMessageManagerCallback.Complete(EMmTsyBootGetRFStatusIPC, &data, aError);
	
	} // CPhoneDispatcher::CallbackNotifyRfStatusChange

void CPhoneDispatcher::CallbackGetServiceProviderName(TInt aError,
		const RMobilePhone::TMobilePhoneServiceProviderNameV2& aServiceProvider)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneGetServiceProviderNameComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aServiceProvider The service provider information.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneServiceProviderNameV2*>(&aServiceProvider));
	
	iMessageManagerCallback.Complete(EMobilePhoneGetServiceProviderName, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackGetServiceProviderName

void CPhoneDispatcher::CallbackGetPhoneId(TInt aError,
				const RMobilePhone::TMobilePhoneIdentityV1& aPhoneId)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneGetPhoneIdComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aPhoneId The mobile phone identity returned by the LTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	//The GetImei request from SAT has been rolled into this one. An ESatTsyGetIMEI request now
	//triggers DispatchGetPhoneIdL. 
	
	CMmDataPackage dataPackage;
	
	if (iPhoneIdReqOrigin & KPhoneIdReqOriginPhoneFu)
		{
		// Pack the data to return to the Common TSY
		dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneIdentityV1*>(&aPhoneId));
		
		iMessageManagerCallback.Complete(ECtsyGetPhoneIdV2Comp, &dataPackage, aError);
		}
	
	if (iPhoneIdReqOrigin & KPhoneIdReqOriginSatFu)
		{
		__ASSERT_DEBUG(aPhoneId.iSerialNumber.Length() <= KImeiBufLength, CtsyDispatcherPanic(EBadLength));
		// Pack the data to return to the Common TSY
		TBuf8<KImeiBufLength> imeiBuf;
		imeiBuf.Copy(aPhoneId.iSerialNumber.Left(KImeiBufLength));
		
		dataPackage.PackData(&imeiBuf);

		iSatMessageManagerCallback.Complete(ESatTsyGetIMEI, &dataPackage, aError);
		}
	
	iPhoneIdReqOrigin = 0;
	
	} // CPhoneDispatcher::CallbackGetPhoneId

void CPhoneDispatcher::CallbackGetDetectedNetworks(TInt aError,
				const CMobilePhoneNetworkListV2& aPhoneNetworkList)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aPhoneNetworkList The list of detected networks
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	
	const CMobilePhoneNetworkListV2* phoneNetworkList = &aPhoneNetworkList;

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&phoneNetworkList);

	iMessageManagerCallback.Complete(EMobilePhoneGetDetectedNetworksV2Phase1, &dataPackage, aError);
	} // CPhoneDispatcher::CallbackGetDetectedNetworks

void CPhoneDispatcher::CallbackGetDetectedNetworksCancel(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksCancelComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneGetDetectedNetworksCancel, aError);
	
	} // CPhoneDispatcher::CallbackGetDetectedNetworksCancel

void CPhoneDispatcher::CallbackSelectNetwork(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneSelectNetworkComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(EMobilePhoneSelectNetwork, aError);
	} // CPhoneDispatcher::CallbackSelectNetwork

void CPhoneDispatcher::CallbackSelectNetworkCancel(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneSelectNetworkCancelComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	iMessageManagerCallback.Complete(EMobilePhoneSelectNetworkCancel, aError);
	
	} // CPhoneDispatcher::CallbackSelectNetworkCancel

void CPhoneDispatcher::CallbackSetNetworkSelectionSetting(TInt aError, 
		RMobilePhone::TMobilePhoneSelectionMethod aSelectionMethod)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneSetNetworkSelectionSettingComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSelectionMethod The new network selection method.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSelectionMethod=%d"), aError, aSelectionMethod);

	iMessageManagerCallback.Complete(EMobilePhoneSetNetworkSelectionSetting, aError);
	
	if(aError == KErrNone)
		{
		RMobilePhone::TMobilePhoneNetworkSelectionV1 phoneNetworkSelection;
		phoneNetworkSelection.iMethod = aSelectionMethod;
		
		// Pack the data to return to the Common TSY
		CMmDataPackage data;
		data.PackData(&phoneNetworkSelection);

		iMessageManagerCallback.Complete(EMobilePhoneNotifyNetworkSelectionSettingChange, &data, aError);
		}
	} // CPhoneDispatcher::CallbackSetNetworkSelectionSetting


void CPhoneDispatcher::CallbackGetCurrentNetworkInfo(TInt aError,
				const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo,
				const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetCurrentNetworkInfoL()
 * 
 * @param aError The error code to complete back.
 * @param aNetworkInfo The network information to be sent to the CTSY Dispatcher.
 * 		  Note the actual data type contained could be of type TMobilePhoneNetworkInfoV2 or TMobilePhoneNetworkInfoV1.
 * @param aLocationArea The mobile phone location area.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// This is the callback for the internal getter for current network info.
	// In order to update CTSY's network info cache, we can make use of the completion as used by
	// the NotifyCurrentNetworkChange.
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneNetworkInfoV5*>(&aNetworkInfo),
						 const_cast<RMobilePhone::TMobilePhoneLocationAreaV1*>(&aLocationArea));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyCurrentNetworkChange, &dataPackage, aError);

	} // CPhoneDispatcher::CallbackGetCurrentNetworkChange

void CPhoneDispatcher::CallbackGetNetworkMode(TInt aError,
				RMobilePhone::TMobilePhoneNetworkMode aNetworkMode) 
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetNetworkModeL()
 * 
 * @param aError The error code to complete back.
 * @param aNetworkMode The network mode to be sent to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// This is the callback for the internal getter for the network mode.
	// In order to update CTSY's network mode cache, we can make use of the completion as used by
	// the NotifyNetworkModeChange.
	CMmDataPackage dataPackage;
	dataPackage.PackData(static_cast<RMobilePhone::TMobilePhoneNetworkMode*>(&aNetworkMode));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyModeChange, &dataPackage, aError);

	} // CPhoneDispatcher::CallbackGetNetworkMode

void CPhoneDispatcher::CallbackGetNitzInfo(TInt aError,
				const RMobilePhone::TMobilePhoneNITZ& aNitzInfo)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetNitzInfoL()
 * 
 * @param aError The error code to complete back.
 * @param aNitzInfo The NITZ information to be sent to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// This is the callback for the internal getter for the NITZ info.
	// In order to update CTSY's NITZ cache, we can make use of the completion as used by
	// the NotifyNITZInfoChange.
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneNITZ*>(&aNitzInfo));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyNITZInfoChange, &dataPackage, aError);

	} // CPhoneDispatcher::CallbackGetNitzInfo

void CPhoneDispatcher::CallbackGetSignalStrength(TInt aError,
				TInt32 aSignalStrength,
				TInt8 aBar) 
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetSignalStrengthL()
 * 
 * @param aError The error code to complete back.
 * @param aSignalStrength The signal strength value to be sent to the CTSY Dispatcher.
 * @param aBar The bar display to be sent to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// This is the callback for the internal getter for the NITZ info.
	// In order to update CTSY's NITZ cache, we can make use of the completion as used by
	// the NotifySignalStrengthChange.
	CMmDataPackage dataPackage;
	// Note: Ctsy expects the data to be packed in the order <bar, signalStrength>,
	// so swap the order here.
	dataPackage.PackData(static_cast<TInt8*>(&aBar),
						 static_cast<TInt32*>(&aSignalStrength));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifySignalStrengthChange, &dataPackage, aError);

	} // CPhoneDispatcher::CallbackGetSignalStrength

void CPhoneDispatcher::CallbackGetBatteryInfo(TInt aError,
				const RMobilePhone::TMobilePhoneBatteryInfoV1& aBatteryInfo)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetBatteryInfoL()
 * 
 * @param aError The error code to complete back.
 * @param aBatteryInfo The battery information to be sent to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// This is the callback for the internal getter for the battery information.
	// In order to update CTSY's battery information cache, we can make use of the completion as used by
	// the NotifyBatteryInfoChange.
	CMmDataPackage dataPackage;
	dataPackage.PackData(const_cast<RMobilePhone::TMobilePhoneBatteryInfoV1*>(&aBatteryInfo));
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyBatteryInfoChange, &dataPackage, aError);

	} // CPhoneDispatcher::CallbackGetBatteryInfo

void CPhoneDispatcher::CallbackNspsWakeup(TInt aError)
/**
 * Completes CPhoneDispatcher::DispatchNspsWakeupL()
 * 
 * @param aError The error code to complete back.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECustomNetWakeupIPC, aError);
	} // CPhoneDispatcher::CallbackNspsWakeup

void CPhoneDispatcher::CallbackSetSystemNetworkMode(TInt aError)
/**
 * Completes CPhoneDispatcher::DispatchSetSystemNetworkModeL()
 * 
 * @param aError The error code to complete back.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECustomSetSystemNetworkModeIPC, aError);
	} // CPhoneDispatcher::CallbackSetSystemNetworkMode

void CPhoneDispatcher::CallbackGetCurrentSystemNetworkModes(TInt aError, TUint32 aCurrentSystemNetworkModes)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetCurrentSystemNetworkModesL()
 * 
 * @param aError The error code to complete back.
 * @param aCurrentSystemNetworkModes A bit mask that represents the current
 *         network modes with values from RMmCustomAPI::TNetworkModeCaps.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aCurrentSystemNetworkModes=%d"), aError,aCurrentSystemNetworkModes);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	data.PackData(&aCurrentSystemNetworkModes);
	
	iMessageManagerCallback.Complete(ECustomGetCurrentSystemNetworkModesIPC, &data, aError);
	} // CPhoneDispatcher::CallbackGetCurrentSystemNetworkModes

void CPhoneDispatcher::CallbackResetNetServer(TInt aError)
/**
 * Completes CPhoneDispatcher::DispatchResetNetServerL()
 * 
 * @param aError The error code to complete back.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECustomResetNetServerIPC, aError);
	} // CPhoneDispatcher::CallbackResetNetServer

void CPhoneDispatcher::CallbackSetAlwaysOnMode(TInt aError)
/**
 * Completes CPhoneDispatcher::DispatchSetAlwaysOnModeL()
 * 
 * @param aError The error code to complete back.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECustomSetAlwaysOnMode, aError);
	} // CPhoneDispatcher::CallbackSetAlwaysOnMode

void CPhoneDispatcher::CallbackSetDriveMode(TInt aError)
/**
 * Completes CPhoneDispatcher::DispatchSetDriveModeL()
 * 
 * @param aError The error code to complete back.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECustomSetDriveModeIPC, aError);
	} // CPhoneDispatcher::CallbackSetDriveMode

void CPhoneDispatcher::CallbackNotifyNspsStatusChange(TInt aError, TBool aEnabled)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyNspsStatusChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aEnabled whether No Signal Power Save is enabled.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aEnabled=%d"), aError, aEnabled);

	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	data.PackData(&aEnabled);
	
	iMessageManagerCallback.Complete(ECustomNotifyNSPSStatusIPC, &data, aError);
	
	} // CPhoneDispatcher::CallbackNotifyNspsStatusChange

void CPhoneDispatcher::CallbackNotifyNetworkSelectionSettingChange(TInt aError, RMobilePhone::TMobilePhoneSelectionMethod aSelectionMethod)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkSelectionSettingChangeInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aSelectionMethod The new network selection method.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d,aSelectionMethod=%d"), aError, aSelectionMethod);
	
	RMobilePhone::TMobilePhoneNetworkSelectionV1 phoneNetworkSelection;
	phoneNetworkSelection.iMethod = aSelectionMethod;
	
	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	data.PackData(&phoneNetworkSelection);

	iMessageManagerCallback.Complete(EMobilePhoneNotifyNetworkSelectionSettingChange, &data, aError);
	} // CPhoneDispatcher::CallbackNotifyNetworkSelectionSettingChange

void CPhoneDispatcher::CallbackNotifyRauEvent(TInt aError, RMmCustomAPI::TRauEventStatus aRauEvent)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyRauEventInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aRauEvent The RAU (Routing Area Update) event to be notified (Active or Resumed).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aRauEvent=%d"), aError, aRauEvent);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aRauEvent);
	
	iMessageManagerCallback.Complete(ECustomNotifyRauEventIPC, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyRauEvent

void CPhoneDispatcher::CallbackNotifyNetworkConnectionFailure(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyNetworkConnectionFailureInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	
	iMessageManagerCallback.Complete(ECustomNotifyNetworkConnectionFailureIPC, aError);
	
	} // CPhoneDispatcher::CallbackNotifyNetworkConnectionFailure

void CPhoneDispatcher::CallbackNotifyDtmfEvent(TInt aError, RMmCustomAPI::TDtmfEventType aType,
                                               RMmCustomAPI::TDtmfEvent aEvent, TChar aTone)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyDtmfEventInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aType The DTMF event type.
 * @param aEvent The DTMF event to be notified.
 * @param aTone The event tone.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d, aEvent=%d, aTone=%d"), aError, aType, aEvent, (TInt)aTone);

	// Pack the data to return to the Common TSY
    RMmCustomAPI::TDtmfInfo info;
    info.iType = aType;
    info.iEvent = aEvent;
    info.iTone = aTone;
	CMmDataPackage dataPackage;
	dataPackage.PackData(&info);
	
	iMessageManagerCallback.Complete(ECustomNotifyDtmfEventIPC, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyDtmfEvent

void CPhoneDispatcher::CallbackNotifyDtmfEvent(TInt aError, RMobilePhone::TMobilePhoneDTMFEvent aEventType)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyDtmfEventInd()
 *
 * @param aError KErrNone on success, or another error code indicating the error otherwise.
 * @param aEventType The DTMF event type. Either EStartDtmfTone or EStopDtmfTone
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aEventType=%d"), aEventType);

	// Pack the data to return to the Common TSY
    RMobilePhone::TMobilePhoneDTMFEvent info;
    info = aEventType;
 	CMmDataPackage dataPackage;
	dataPackage.PackData(&info);
	
	iMessageManagerCallback.Complete(EMobilePhoneNotifyDTMFEvent, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyDtmfEvent	
	
void CPhoneDispatcher::CallbackGetHspaStatus(TInt aError, RMmCustomAPI::THSxPAStatus aStatus)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetHspaStatusL()
 * 
 * @param aError The error code to complete back.
 * @param aStatus The phone HSxPA status (enable or disable).
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d"), aError, aStatus);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	data.PackData(&aStatus);
	
	iMessageManagerCallback.Complete(ECustomReadHSxPAStatusIPC, &data, aError);
	} // CPhoneDispatcher::CallbackGetHspaStatus

void CPhoneDispatcher::CallbackSetHspaStatus(TInt aError)
/**
 * Completes CPhoneDispatcher::DispatchSetHspaStatusL()
 * 
 * @param aError The error code to complete back.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECustomWriteHSxPAStatusIPC, aError);
	} // CPhoneDispatcher::CallbackSetHspaStatus

void CPhoneDispatcher::CallbackNotifyHspaStatusChanged(TInt aError, RMmCustomAPI::THSxPAStatus aStatus)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyHspaStatusChangedInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aStatus The new HSxPA status (enable or disable).
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aStatus=%d"), aError, aStatus);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	dataPackage.PackData(&aStatus);
	
	iMessageManagerCallback.Complete(ECustomNotifyHSxPAStatusIPC, &dataPackage, aError);
	
	} // CPhoneDispatcher::CallbackNotifyHspaStatusChanged

void CPhoneDispatcher::CallbackGetNetworkProviderName(TInt aError, const TDesC& aName)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetNetworkProviderNameL()
 * 
 * @param aError The error code to complete back.
 * @param aName The name of the current network provider.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	TDesC* name = const_cast<TDesC*>(&aName);
	data.PackData(&name);
	
	iMessageManagerCallback.Complete(ECustomGetNetworkProviderNameIPC, &data, aError);
	} // CPhoneDispatcher::CallbackGetNetworkProviderName

void CPhoneDispatcher::CallbackGetOperatorName(TInt aError, RMmCustomAPI::TOperatorNameType aType, const TDesC& aName)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetOperatorNameL()
 * 
 * @param aError The error code to complete back.
 * @param aType The type of the operator name.
 * @param aName The name of the current operator.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aType=%d"), aError,aType);
	
	// Pack the data to return to the Common TSY
    RMmCustomAPI::TOperatorNameInfo nameInfo;
    nameInfo.iType = aType;
    nameInfo.iName.Copy(aName);
	CMmDataPackage data;
	data.PackData(&nameInfo);
	
	iMessageManagerCallback.Complete(ECustomGetOperatorNameIPC, &data, aError);
	} // CPhoneDispatcher::CallbackGetOperatorName

void CPhoneDispatcher::CallbackGetCellInfo(TInt aError)
/**
 * Completes CPhoneDispatcher::DispatchGetCellInfoL()
 * 
 * @param aError The error code to complete back.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Pack the data to return to the Common TSY
	iCellInfo->iMode = RMmCustomAPI::TMmCellInfo::EUnknown;
	CMmDataPackage data;
	data.PackData(iCellInfo);
	
	iMessageManagerCallback.Complete(ECustomGetCellInfoIPC, &data, aError);
	} // CPhoneDispatcher::CallbackGetCellInfo

void CPhoneDispatcher::CallbackGetCellInfo(TInt aError, const RMmCustomAPI::TMmCellInfo::TGSMCellInfo& aInfo)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetCellInfoL()
 * 
 * @param aError The error code to complete back.
 * @param aInfo The GSM cell information.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Pack the data to return to the Common TSY
	iCellInfo->iMode = RMmCustomAPI::TMmCellInfo::EGSM;
	iCellInfo->iGsmCellInfo = aInfo;
	CMmDataPackage data;
	data.PackData(iCellInfo);
	
	iMessageManagerCallback.Complete(ECustomGetCellInfoIPC, &data, aError);
	} // CPhoneDispatcher::CallbackGetCellInfo

void CPhoneDispatcher::CallbackGetCellInfo(TInt aError, const RMmCustomAPI::TMmCellInfo::TWCDMACellInfo& aInfo)
/**
 * Packages the data returned by the Licensee LTSY and completes CPhoneDispatcher::DispatchGetCellInfoL()
 * 
 * @param aError The error code to complete back.
 * @param aInfo The WCDMA cell information.
 */
    {
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Pack the data to return to the Common TSY
	iCellInfo->iMode = RMmCustomAPI::TMmCellInfo::EWCDMA;
	iCellInfo->iWcdmaCellInfo = aInfo;
	CMmDataPackage data;
	data.PackData(iCellInfo);
	
	iMessageManagerCallback.Complete(ECustomGetCellInfoIPC, &data, aError);
	} // CPhoneDispatcher::CallbackGetCellInfo

void CPhoneDispatcher::CallbackNotifyCellInfoChanged(TInt aError)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	iCellInfo->iMode = RMmCustomAPI::TMmCellInfo::EUnknown;
	CMmDataPackage data;
	data.PackData(iCellInfo);
	
	iMessageManagerCallback.Complete(ECustomNotifyCellInfoChangeIPC, &data, aError);
	
	} // CPhoneDispatcher::CallbackNotifyCellInfoChanged

void CPhoneDispatcher::CallbackNotifyCellInfoChanged(TInt aError, const RMmCustomAPI::TMmCellInfo::TGSMCellInfo& aInfo)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aInfo The new GSM cell information.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	iCellInfo->iMode = RMmCustomAPI::TMmCellInfo::EGSM;
	iCellInfo->iGsmCellInfo = aInfo;
	CMmDataPackage data;
	data.PackData(iCellInfo);
	
	iMessageManagerCallback.Complete(ECustomNotifyCellInfoChangeIPC, &data, aError);
	
	} // CPhoneDispatcher::CallbackNotifyCellInfoChanged

void CPhoneDispatcher::CallbackNotifyCellInfoChanged(TInt aError, const RMmCustomAPI::TMmCellInfo::TWCDMACellInfo& aInfo)
/**
 * Callback function to be used by the request to complete 
 * CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangedInd()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aInfo The new WCDMA cell information.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);

	// Pack the data to return to the Common TSY
	CMmDataPackage dataPackage;
	iCellInfo->iMode = RMmCustomAPI::TMmCellInfo::EWCDMA;
	iCellInfo->iWcdmaCellInfo = aInfo;
	CMmDataPackage data;
	data.PackData(iCellInfo);
	
	iMessageManagerCallback.Complete(ECustomNotifyCellInfoChangeIPC, &data, aError);
	
	} // CPhoneDispatcher::CallbackNotifyCellInfoChanged

void CPhoneDispatcher::CallbackRegisterCellInfoChangeNotification(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhoneRegisterCellInfoChangeNotificationComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECtsyPhoneCellInfoIndComp, aError);
	} // CPhoneDispatcher::CallbackRegisterCellInfoChangeNotification

void CPhoneDispatcher::CallbackGetPhoneCellInfo(TInt aError, const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhoneGetPhoneCellInfoComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aCellInfo The cell information to be cached in the CTSY and returned to the client.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	// Pack the data to return to the Common TSY
	CMmDataPackage data;
	data.PackData(&const_cast<RMobilePhone::TMobilePhoneCellInfoV9&>(aCellInfo));
	
	iMessageManagerCallback.Complete(ECtsyPhoneCellInfoComp, &data, aError);
	} // CPhoneDispatcher::CallbackGetPhoneCellInfo

void CPhoneDispatcher::CallbackGetUsimServiceSupport(TInt aError, TInt aApplicationNumber, TBool aSupported)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhoneGetUsimServiceSupportComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 * @param aApplicationNumber The application number to which the support status is related to.
 * @param aSupported The service supported state (True / False)
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aApplicationNumber=%d, aSupported=%d"), aError, aApplicationNumber, aSupported);
	
	// Pack the data to return to the Common TSY
	
	RMmCustomAPI::TAppSupport appSupport;
	appSupport.iAppNum = aApplicationNumber;
	appSupport.iSupported = aSupported;
	
	CMmDataPackage data;
	data.PackData(&appSupport);
	
	iMessageManagerCallback.Complete(ECustomGetServiceTableSupportbyApplicationIPC, &data, aError);
	} //CPhoneDispatcher::CallbackGetUsimServiceSupport

void CPhoneDispatcher::CallbackGetCurrentActiveUsimApplication(TInt aError, const TDesC8& aAid)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentActiveUsimApplicationComp()
 *
* @param aError The error code to be returned to the CTSY Dispatcher.
* @param aAid The AID of the currently active USIM Application, this should be no longer than RMobilePhone::KAIDSize.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aAid=%S"), aError, &aAid);
	
	__ASSERT_DEBUG(aAid.Length() <= RMobilePhone::KAIDSize, CtsyDispatcherPanic(EBadLength));
	
	//CTSY unpacks as a TAID* so we have to copy the TDesC& here to be safe.
	RMobilePhone::TAID aid(aAid.Left(RMobilePhone::KAIDSize));
	
	CMmDataPackage data;
	data.PackData(&aid);
	
	iMessageManagerCallback.Complete(EMobilePhoneGetCurrentActiveUSimApplication, &data, aError);
	} //CPhoneDispatcher::CallbackGetCurrentActiveUsimApplication

void CPhoneDispatcher::CallbackTerminateAllCalls(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhoneTerminateAllCallsComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECtsyPhoneTerminateAllCallsComp, aError);
	} //CPhoneDispatcher::CallbackTerminateAllCalls

void CPhoneDispatcher::CallbackGetSystemNetworkBand(TInt aError, RMmCustomAPI::TBandSelection aBand, RMmCustomAPI::TNetworkModeCaps aMode)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentActiveUsimApplicationComp()
 *
* @param aError The error code to be returned to the CTSY Dispatcher.
* @param aBand the current network band
* @param aMode the current nework mode
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d, aBand=%d, aMode=%d"), aError, aBand, aMode);
	
	CMmDataPackage data;
	data.PackData(&aBand, &aMode);
	
	iMessageManagerCallback.Complete(ECustomGetBandSelectionIPC, &data, aError);
	} //CPhoneDispatcher::CallbackGetSystemNetworkBand

void CPhoneDispatcher::CallbackSetSystemNetworkBand(TInt aError)
/**
 * Callback function to be used by the request to complete
 * CCtsyDispatcherCallback::CallbackSetSystemNetworkBandComp()
 *
 * @param aError The error code to be returned to the CTSY Dispatcher.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aError=%d"), aError);
	
	iMessageManagerCallback.Complete(ECustomSetBandSelectionIPC, aError);
	} //CPhoneDispatcher::CallbackSetSystemNetworkBand

void CPhoneDispatcher::CallbackSync(CRequestQueueOneShot::TIpcDataPackage& aIpcDataPackage)
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
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEDISPATCHER_CALLBACKSYNC_1, "WARNING: CPhoneDispatcher::CallbackSync unhandled IPC=%d", aIpcDataPackage.iIpc);
		__ASSERT_DEBUG(NULL, CtsyDispatcherPanic(EUnhandledCtsyIpc));
		break;		
		} // switch (aIpcDataPackage.iIpc)
	} // CPhoneDispatcher::CallbackSync
	
