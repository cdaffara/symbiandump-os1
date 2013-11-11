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

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>

#include "cltsyfactoryv1.h"
#include "cltsycallcontrolhandler.h"
#include "cltsyphonehandler.h"
#include "cltsysecurityhandler.h"
#include "cltsyphonebookenhandler.h"
#include "cltsycellbroadcasthandler.h"
#include "cltsyphonebookonhandler.h"
#include "cltsyphonebookhandler.h"
#include "cltsysimhandler.h"
#include "cltsysmshandler.h"
#include "cltsycallcontrolmultipartyhandler.h"
#include "cltsysupplementaryserviceshandler.h"
#include "cltsypacketserviceshandler.h"
#include "cltsysathandler.h"
#include "creceivethreadmanager.h"
#include "cltsyreceivethreadeventstatehandler.h"


// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

CLtsyFactoryV1::CLtsyFactoryV1()
	{
	}

CLtsyFactoryV1::~CLtsyFactoryV1()
	{
	TSYLOGENTRYEXIT;

	delete iReceiveThreadManager;
	delete iReceiveThreadEventStateHandler;

	// Delete dispatch handlers
	delete iLtsyDispatchCallControlHandler;
	iLtsyDispatchCallControlHandler = NULL;

	delete iLtsyDispatchPhoneHandler;
	iLtsyDispatchPhoneHandler = NULL;

	delete iLtsyDispatchSecurityHandler;
	iLtsyDispatchSecurityHandler = NULL;

	delete iLtsyDispatchPhonebookEnHandler;
	iLtsyDispatchPhonebookEnHandler = NULL;

	delete iLtsyDispatchCellBroadcastHandler;
	iLtsyDispatchCellBroadcastHandler = NULL;

	delete iLtsyDispatchPhonebookOnHandler;
	iLtsyDispatchPhonebookOnHandler = NULL;

	delete iLtsyDispatchPhonebookHandler;
	iLtsyDispatchPhonebookHandler = NULL;

	delete iLtsyDispatchSimHandler;
	iLtsyDispatchSimHandler = NULL;

	delete iLtsyDispatchSmsHandler;
	iLtsyDispatchSmsHandler = NULL;

	delete iLtsyDispatchCallControlMultipartyHandler;
	iLtsyDispatchCallControlMultipartyHandler = NULL;

	delete iLtsyDispatchSupplementaryServicesHandler;
	iLtsyDispatchSupplementaryServicesHandler = NULL;

	delete iLtsyDispatchPacketServicesHandler;
	iLtsyDispatchPacketServicesHandler = NULL;

	delete iLtsyDispatchSatHandler;
	iLtsyDispatchSatHandler = NULL;


	MTEST_DELETE_MOCKLTSYENGINE;

	}

MLtsyFactoryBase::TCtsyInterfaceVersion CLtsyFactoryV1::Version()
	{
	TSYLOGENTRYEXIT;
	return EIfVersion1;
	}

void CLtsyFactoryV1::ConstructL(CCtsyDispatcherCallback& aCallback)
/**
 * Second phase constructor. This function must be called explicitly
 * by the client to complete construction before any other functions in this
 * class are called.
 */
	{
	TSYLOGENTRYEXIT;

	// Create the LTSY dispatch handlers
	iLtsyDispatchCallControlHandler = CLtsyCallControlHandler::NewL(aCallback);
	iLtsyDispatchPhoneHandler = CLtsyPhoneHandler::NewL(aCallback);
	iLtsyDispatchSecurityHandler = CLtsySecurityHandler::NewL(aCallback);
	iLtsyDispatchPhonebookEnHandler = CLtsyPhonebookEnHandler::NewL(aCallback);
	iLtsyDispatchCellBroadcastHandler = CLtsyCellBroadcastHandler::NewL(aCallback);
	iLtsyDispatchPhonebookOnHandler = CLtsyPhonebookOnHandler::NewL(aCallback);
	iLtsyDispatchPhonebookHandler = CLtsyPhonebookHandler::NewL(aCallback);
	iLtsyDispatchSimHandler = CLtsySimHandler::NewL(aCallback);
	iLtsyDispatchSmsHandler = CLtsySmsHandler::NewL(aCallback);
	iLtsyDispatchCallControlMultipartyHandler = CLtsyCallControlMultipartyHandler::NewL(aCallback);
	iLtsyDispatchSupplementaryServicesHandler = CLtsySupplementaryServicesHandler::NewL(aCallback);
	iLtsyDispatchPacketServicesHandler = CLtsyPacketServicesHandler::NewL(aCallback);
	iLtsyDispatchSatHandler = CLtsySatHandler::NewL(aCallback);

	/*
	Sometimes baseband interfaces are such that the API to receive signal callbacks
	from baseband is synchronous and is essentially a forever wait loop. In this situation
	it is necessary to spawn a separate thread in which the wait loop will execute.
	But on receiving a callback signal it is necessary to transfer the thread of execution
	back the main ETEL thread that runs the LTSY/CTSY and ETEL.

	This function will start a new thread to initiate the wait loop.

	NOTE: If interface with baseband is not as described above this function can be completely removed.
	*/
	iReceiveThreadEventStateHandler = CLtsyReceiveThreadEventStateHandler::NewL();
	iReceiveThreadManager = CReceiveThreadManager::NewL(*iReceiveThreadEventStateHandler, *iReceiveThreadEventStateHandler);
	iReceiveThreadManager->StartReceiveThreadL();

	// Can be removed in real LTSY implementation //
	MTEST_CREATE_MOCKLTSYENGINE(aCallback);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchCallControlHandler); // Can be removed in real LTSY implementation
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchPhoneHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchSecurityHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchPhonebookEnHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchCellBroadcastHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchPhonebookOnHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchPhonebookHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchSimHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchSmsHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchCallControlMultipartyHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchSupplementaryServicesHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchPacketServicesHandler);
	MTEST_SET_MOCKLTSYENGINE(iLtsyDispatchSatHandler);

	//

	} // CLtsyFactoryV1::ConstructL


TInt CLtsyFactoryV1::GetDispatchHandler(
			TLtsyDispatchInterfaceApiId aDispatchInterfaceId,
			TAny*& aDispatchInterface)
/**
 * Part of the MLtsyFactoryV1 interface.  Returns the requested dispatch handler.
 * For more details see MLtsyFactoryV1::GetDispatchHandler
 *
 * @param aInterfaceId The dispatch interface identifier of the requested
 * interface.
 *
 * @param aDispatchInterface If this function completes with KErrNone, will contain
 * a reference to a pointer to the dispatch handler interface in the LTSY.
 * aDispatchInterface may be NULL. Otherwise the parameter will remain unchanged.
 *
 * @return KErrNone if the requested interface was returned in the
 * aDispatchInterface parameter; KErrNotFound if the requested interface
 * was not found in the Licensee LTSY.
 */
	{
	TSYLOGENTRYEXITARGS(_L8("aDispatchInterfaceId=%d"), aDispatchInterfaceId);

	switch (aDispatchInterfaceId)
		{

		// Handlers for IPCs used in boot sequence

	case MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneBootNotifyModemStatusReady*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneBootNotifySimStatusReady*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetNetworkRegistrationStatus*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSimRefreshRegister*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetFdnStatus*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetHomeNetwork*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneNspsWakeup*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSetSystemNetworkMode*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetCurrentSystemNetworkModes*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneResetNetServer*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSetAlwaysOnMode*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSetDriveMode*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetHspaStatus*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSetHspaStatus*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetNetworkProviderName*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetOperatorName*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetCellInfo*>
												(iLtsyDispatchPhoneHandler);
		break;

		// CallControl Functional Unit interface handlers

	case MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlAnswer*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlHold*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlDialEmergency*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlStopDtmfTone*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlSetActiveAlsLine*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlSendDtmfTonesCancel*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlHangUp*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlResume*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlSetDynamicHscsdParams*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlDialVoice*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlTransfer*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlSendDtmfTones*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlGetIdentityServiceStatus*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlSwap*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlSwap*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlLoanDataPort*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlRecoverDataPort*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlStartDtmfTone*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlGetActiveAlsLine*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlDialData*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlQueryIsEmergencyNumber*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlGetAlsPpSupport*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlGetAlsBlockedStatus*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlSetAlsBlocked*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlGetLifeTime*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlTerminateErrorCall*>
												(iLtsyDispatchCallControlHandler);
        break;
	case MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlTerminateAllCalls*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlGetCallForwardingIndicator*>
												(iLtsyDispatchCallControlHandler);
		break;
	case MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlUpdateLifeTimer*>
												(iLtsyDispatchCallControlHandler);
		break;

		// Phone Functional Unit interface handlers

	case MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetServiceProviderName*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetPhoneId*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetDetectedNetworks*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetDetectedNetworksCancel*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSelectNetwork*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSelectNetworkCancel*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSetNetworkSelectionSetting*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetCurrentNetworkInfo*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetNetworkMode*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetNitzInfo*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetSignalStrength*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetBatteryInfo*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneRegisterCellInfoChangeNotification*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetPhoneCellInfo*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetUsimServiceSupport*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetCurrentActiveUsimApplication*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneTerminateAllCalls*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneGetSystemNetworkBand*>
												(iLtsyDispatchPhoneHandler);
		break;
	case MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhoneSetSystemNetworkBand*>
												(iLtsyDispatchPhoneHandler);
		break;		
		
		// Security Functional Unit interface handlers

	case MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityGetSecurityCodeInfo*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityGetLockInfo*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityAbortSecurityCode*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityGetCurrentActivePin*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecuritySetLockSetting*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityVerifySecurityCode*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityGetPin1DisableSupported*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityCheckSecurityCode*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityDisablePhoneLock*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityGetCipheringInfo*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityIsSecurityCodeBlocked*>
												(iLtsyDispatchSecurityHandler);
		break;
	case MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSecurityCheckSecurityCodeCancel*>
												(iLtsyDispatchSecurityHandler);
		break;												
				
		// PhonebookEn Functional Unit interface handlers

	case MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookEnStoreGetInfo*>
												(iLtsyDispatchPhonebookEnHandler);
		break;
	case MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookEnStoreReadAll*>
												(iLtsyDispatchPhonebookEnHandler);
		break;
	case MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookEnStoreReadEntry*>
												(iLtsyDispatchPhonebookEnHandler);
		break;

		// CellBroadcast Functional Unit interface handlers

	case MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCellBroadcastSetBroadcastFilterSetting*>
												(iLtsyDispatchCellBroadcastHandler);
		break;
	case MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage*>
												(iLtsyDispatchCellBroadcastHandler);
		break;
	case MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCellBroadcastReceiveMessageCancel*>
												(iLtsyDispatchCellBroadcastHandler);
		break;
	case MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing*>
												(iLtsyDispatchCellBroadcastHandler);
		break;		
	case MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCellBroadcastDeleteSimCbTopic*>
												(iLtsyDispatchCellBroadcastHandler);
		break;		

		// PhonebookOn Functional Unit interface handlers

	case MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreRead*>
												(iLtsyDispatchPhonebookOnHandler);
		break;
	case MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreDeleteAll*>
												(iLtsyDispatchPhonebookOnHandler);
		break;
	case MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreReadEntry*>
												(iLtsyDispatchPhonebookOnHandler);
		break;
	case MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreGetInfo*>
												(iLtsyDispatchPhonebookOnHandler);
		break;
	case MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreGetReadStoreSize*>
												(iLtsyDispatchPhonebookOnHandler);
		break;
	case MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreDeleteEntry*>
												(iLtsyDispatchPhonebookOnHandler);
		break;
	case MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreWriteEntry*>
												(iLtsyDispatchPhonebookOnHandler);
		break;
	case MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreWrite*>
												(iLtsyDispatchPhonebookOnHandler);
		break;
	case MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookOnStoreGetStoreSize*>
												(iLtsyDispatchPhonebookOnHandler);
		break;

		// Phonebook Functional Unit interface handlers

	case MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookStoreReadEntry*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookStoreDeleteEntry*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookStoreCache*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookStoreGetPhonebookInfo*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookStoreInitialise*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookStoreDeleteAll*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookSmsStoreReadAll*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookStoreWriteEntry*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookGetPhoneStoreInfo*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookSmsStoreDeleteEntry*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookSmsStoreDeleteAll*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookSmsStoreGetInfo*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookSmsStoreReadEntry*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookSmsStoreWriteEntry*>
												(iLtsyDispatchPhonebookHandler);
		break;
	case MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPhonebookStoreCacheCancel*>
												(iLtsyDispatchPhonebookHandler);
		break;

		// Sim Functional Unit interface handlers

	case MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetApnControlListServiceStatus*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimDeleteApnName*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimEnumerateApnEntries*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimChangeSecurityCode*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimSetFdnSetting*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetCustomerServiceProfile*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetSubscriberId*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimAppendApnName*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetActiveIccApplicationType*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimSetIccMessageWaitingIndicators*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimSetApnControlListServiceStatus*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetApnName*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimSimRefreshDone*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetServiceTable*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetIccMessageWaitingIndicators*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId:
			aDispatchInterface = static_cast<MLtsyDispatchSimSimLockActivate*>
													(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimSimLockDeActivate*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetAnswerToReset*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetSimCardReaderStatus*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetSimAuthenticationEapSimData*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimGetSimAuthenticationEapAkaData*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimPowerSimOff*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimPowerSimOn*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimReadSimFile*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId:
			aDispatchInterface = static_cast<MLtsyDispatchSimSendApduRequest*>
													(iLtsyDispatchSimHandler);
			break;
	case MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimSendApduRequestV2*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimSimWarmReset*>
												(iLtsyDispatchSimHandler);
		break;
	case MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSimSetSimMessageStatusRead*>
												(iLtsyDispatchSimHandler);
		break;

		// Sms Functional Unit interface handlers

	case MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsSendSatSms*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsGetSmsStoreInfo*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsGetSmspList*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsNackSmsStored*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsAckSmsStored*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsResumeSmsReception*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsSendSmsMessage*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsSendSmsMessageNoFdnCheck*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsSetMoSmsBearer*>
												(iLtsyDispatchSmsHandler);
		break;
	case MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSmsStoreSmspListEntry*>
												(iLtsyDispatchSmsHandler);
		break;

		// CallControlMultiparty Functional Unit interface handlers

	case MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlMultipartyConferenceHangUp*>
												(iLtsyDispatchCallControlMultipartyHandler);
		break;
	case MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlMultipartyConferenceAddCall*>
												(iLtsyDispatchCallControlMultipartyHandler);
		break;
	case MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlMultipartyCreateConference*>
												(iLtsyDispatchCallControlMultipartyHandler);
		break;
	case MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlMultipartyConferenceSwap*>
												(iLtsyDispatchCallControlMultipartyHandler);
		break;
	case MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId:
		aDispatchInterface = static_cast<MLtsyDispatchCallControlMultipartyConferenceGoOneToOne*>
												(iLtsyDispatchCallControlMultipartyHandler);
		break;

		// SupplementaryServices Functional Unit interface handlers

	case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesGetCallWaitingStatus*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesGetCallBarringStatus*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSetCallBarringStatus*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSetCallForwardingStatus*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSetCallWaitingStatus*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSetSsPassword*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesGetCallForwardingStatus*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSendUssdMessage*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;		
	case MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesSendUssdRelease*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;
	case MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSupplementaryServicesClearBlacklist*>
												(iLtsyDispatchSupplementaryServicesHandler);
		break;

		// PacketServices Functional Unit interface handlers

	case MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesPacketAttach*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesGetPacketAttachMode*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesPacketDetach*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesSetPdpContextConfig*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesModifyActivePdpContext*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesInitialisePdpContext*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesDeletePdpContext*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesSetPacketAttachMode*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesNotifyPacketStatusChange::KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesNotifyPacketStatusChange*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params*>
												(iLtsyDispatchPacketServicesHandler);
		break;		
	case MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesActivatePdpContext*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesSetPdpContextQosGprs*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId:
			aDispatchInterface = static_cast<MLtsyDispatchPacketServicesSetPdpContextQosR99R4*>
													(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId:
				aDispatchInterface = static_cast<MLtsyDispatchPacketServicesSetPdpContextQosR5*>
														(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesDeactivatePdpContext*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesAddPacketFilter*>
												(iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesGetStatus*>
						   				 (iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesGetStaticCapabilities*>
						   				 (iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists*>
						   				 (iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesGetMaxNoActiveServices*>
						   				 (iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesInitialiseMbmsContext*>
						   				 (iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus*>
						   				 (iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList*>
						   				 (iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesUpdateMbmsSessionList*>
						   				 (iLtsyDispatchPacketServicesHandler);
		break;
	case MLtsyDispatchPacketServicesRemovePacketFilter::KLtsyDispatchPacketServicesRemovePacketFilterApiId:
		aDispatchInterface = static_cast<MLtsyDispatchPacketServicesRemovePacketFilter*>
										(iLtsyDispatchPacketServicesHandler);
		break;

		// Sat Functional Unit interface handlers

	case MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatSmsDeliverReport*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatGetImageInstance*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatGetIconData*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatGetClut*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatGetDefaultBearerCapability*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatGetSmsPpDownloadSupported*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatGetSmsControlActivated*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatCellBroadcastEnvelope::KLtsyDispatchSatCellBroadcastEnvelopeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatCellBroadcastEnvelope*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatGetAccessTechnology*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatTimingAdvance*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatSmControlResponseData*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatProvideLocationInfo*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatSetPollingInterval*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatSmsPpDownloadEnvelope::KLtsyDispatchSatSmsPpDownloadEnvelopeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatSmsPpDownloadEnvelope*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatLocalInformationNmr*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatCallAndSmsControlEnvelope*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatRefreshAllowed*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatReady*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatPCmdNotification*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatUssdControlEnvelopeError*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatTimerExpirationEnvelope::KLtsyDispatchSatTimerExpirationEnvelopeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatTimerExpirationEnvelope*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatTerminalRsp*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatMenuSelectionEnvelope*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatEventDownloadEnvelope*>
												(iLtsyDispatchSatHandler);
		break;
	case MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId:
		aDispatchInterface = static_cast<MLtsyDispatchSatGetUssdControlSupported*>
												(iLtsyDispatchSatHandler);
		break;
	default:
		return TSYLOGSETEXITERR(KErrNotFound);
		} // switch (aDispatchInterfaceId)

	return TSYLOGSETEXITERR(KErrNone);
	} // CLtsyFactoryV1::GetDispatchHandler

TBool CLtsyFactoryV1::IsDispatchInterfaceSupported(TInt aFuncUnitId, TLtsyDispatchInterfaceApiId aDispatchApiId)
	{
	switch(aFuncUnitId)
	    {

	  case KDispatchCallControlFuncUnitId:
	    return iLtsyDispatchCallControlHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchPhoneFuncUnitId:
	    return iLtsyDispatchPhoneHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchSecurityFuncUnitId:
	    return iLtsyDispatchSecurityHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchPhonebookEnFuncUnitId:
	    return iLtsyDispatchPhonebookEnHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchCellBroadcastFuncUnitId:
	    return iLtsyDispatchCellBroadcastHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchPhonebookOnFuncUnitId:
	    return iLtsyDispatchPhonebookOnHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchPhonebookFuncUnitId:
	    return iLtsyDispatchPhonebookHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchSimFuncUnitId:
	    return iLtsyDispatchSimHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchSmsFuncUnitId:
	    return iLtsyDispatchSmsHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchCallControlMultipartyFuncUnitId:
	    return iLtsyDispatchCallControlMultipartyHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchSupplementaryServicesFuncUnitId:
	    return iLtsyDispatchSupplementaryServicesHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchPacketServicesFuncUnitId:
	    return iLtsyDispatchPacketServicesHandler->IsInterfaceSupported(aDispatchApiId);
	  case KDispatchSatFuncUnitId:
	    return iLtsyDispatchSatHandler->IsInterfaceSupported(aDispatchApiId);
	  default:
	    return EFalse;
	    }
	}

void CLtsyFactoryV1::IsCallbackIndicatorSupported(TInt aFuncUnitId, TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
	{
	switch(aFuncUnitId)
	    {
	  case KDispatchCallControlFuncUnitId:
	    iLtsyDispatchCallControlHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchPhoneFuncUnitId:
	    iLtsyDispatchPhoneHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchSecurityFuncUnitId:
	    iLtsyDispatchSecurityHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchPhonebookEnFuncUnitId:
	    iLtsyDispatchPhonebookEnHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchCellBroadcastFuncUnitId:
	    iLtsyDispatchCellBroadcastHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchPhonebookOnFuncUnitId:
	    iLtsyDispatchPhonebookOnHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchPhonebookFuncUnitId:
	    iLtsyDispatchPhonebookHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchSimFuncUnitId:
	    iLtsyDispatchSimHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchSmsFuncUnitId:
	    iLtsyDispatchSmsHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchCallControlMultipartyFuncUnitId:
	    iLtsyDispatchCallControlMultipartyHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchSupplementaryServicesFuncUnitId:
	    iLtsyDispatchSupplementaryServicesHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchPacketServicesFuncUnitId:
	    iLtsyDispatchPacketServicesHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  case KDispatchSatFuncUnitId:
	    iLtsyDispatchSatHandler->IsCallbackIndicatorSupported(aIdGroup, aIndIdBitMask);
	    break;
	  default:
		  aIndIdBitMask = 0;//no support
		 break;
	    }
	}

CTsySatMessagingBase* CLtsyFactoryV1::GetSatMessagingService(MCtsySatService& /*aSatMessagingService*/)
	{
	return NULL;
	}

void CLtsyFactoryV1::Release()
/**
 * Called by the CTSY Dispatcher to inform this object that it can clean itself up.
 */
	{
	TSYLOGENTRYEXIT;
	delete this;
	} // CLtsyFactoryV1::Release





EXPORT_C MLtsyFactoryBase* LTsyFactoryL()
/**
 * Creates an instance of an object implementing the MLtsyFactoryBase interface.
 * ConstructL() must be called on the object returned before it is used.
 */
	{
	TSYLOGENTRYEXIT;
	CLtsyFactoryV1* factory = new (ELeave) CLtsyFactoryV1;
	return factory;
	} // LTsyFactoryL

