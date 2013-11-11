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
// the Licensee LTSY relating to Phone related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHPHONEINTERFACE_H_
#define MLTSYDISPATCHPHONEINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>
#include <ctsy/rmmcustomapi.h>

class MLtsyDispatchPhoneBootNotifyModemStatusReady : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId = KDispatchPhoneFuncUnitId+1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyBootNotifyModemStatusReadyIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneBootNotifyModemStatusReadyComp()
	 *
	 * Implementation of this interface should indicate when the
	 * Modem is ready for requests made from the CTSY.
	 *
	 * @return KErrNone on success otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleBootNotifyModemStatusReadyReqL() = 0;

	}; // class MLtsyDispatchPhoneBootNotifyModemStatusReady


class MLtsyDispatchPhoneBootNotifySimStatusReady : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneBootNotifySimStatusReadyApiId = KDispatchPhoneFuncUnitId+2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsyBootNotifySimStatusReadyIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneBootNotifySimStatusReadyComp()
	 *
	 * Implementation of this interface should indicate when the SIM is ready
	 * for requests made from the CTSY.
	 * For example, any caching of data has been completed.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleBootNotifySimStatusReadyReqL() = 0;

	}; // class MLtsyDispatchPhoneBootNotifySimStatusReady


class MLtsyDispatchPhoneGetFdnStatus : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetFdnStatusApiId = KDispatchPhoneFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetFdnStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetFdnStatusComp()
	 *
	 * Implementation of this interface should get the current FDN status.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetFdnStatusReqL() = 0;

	}; // class MLtsyDispatchPhoneGetFdnStatus


class MLtsyDispatchPhoneGetHomeNetwork : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetHomeNetworkApiId = KDispatchPhoneFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetHomeNetwork
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetHomeNetworkComp()
	 *
	 * Implementation of this interface should get the current home network.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetHomeNetworkReqL() = 0;

	}; // class MLtsyDispatchPhoneGetHomeNetwork


class MLtsyDispatchPhoneGetNetworkRegistrationStatus : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId = KDispatchPhoneFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetNetworkRegistrationStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetNetworkRegistrationStatusComp()
	 *
	 * Implementation of this interface should get the current network registration status.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetNetworkRegistrationStatusReqL() = 0;

	}; // class MLtsyDispatchPhoneGetNetworkRegistrationStatus


class MLtsyDispatchPhoneSimRefreshRegister : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneSimRefreshRegisterApiId = KDispatchPhoneFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsySimRefreshRegisterIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneSimRefreshRegisterComp()
	 *
	 * Implementation of this interface should return whether registering for refresh indications on a list of SIM files was successful.
	 * Note: When the LTSY needs to notify the CTSY of a refresh CCtsyDispatcherCallback::CallbackPhoneSimRefreshNowInd() should be invoked.
	 *
	 * @param aFilesToRegister  Bitmask containing list of files to register for refresh indications.
	 *                           The list of possible files is specified in TCacheFileTypes.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSimRefreshRegisterReqL(TUint16 aFilesToRegister) = 0;

	}; // class MLtsyDispatchPhoneSimRefreshRegister



class MLtsyDispatchPhoneGetServiceProviderName : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhoneGetServiceProviderNameApiId = KDispatchPhoneFuncUnitId + 7;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetServiceProviderName
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneGetServiceProviderNameComp()
	 *
	 * The service provider information to be returned is in the form RMobilePhone::TMobilePhoneServiceProviderNameV2.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetServiceProviderNameReqL() = 0;

	}; // class MLtsyDispatchPhoneGetServiceProviderName



class MLtsyDispatchPhoneGetPhoneId : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhoneGetPhoneIdApiId = KDispatchPhoneFuncUnitId + 8;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetPhoneId
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneGetPhoneIdComp()
	 *
	 * Implementation of this interface should get the phone's identity as described by the
	 * fields in RMobilePhone::TMobilePhoneIdentityV1.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetPhoneIdReqL() = 0;

	}; // class MLtsyDispatchPhoneGetPhoneId



class MLtsyDispatchPhoneGetDetectedNetworks : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhoneGetDetectedNetworksApiId = KDispatchPhoneFuncUnitId + 9;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetDetectedNetworksV2Phase1
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksComp()
	 *
	 * Implementation of this interface should get the current detected networks.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetDetectedNetworksReqL() = 0;

	}; // class MLtsyDispatchPhoneGetDetectedNetworks



class MLtsyDispatchPhoneGetDetectedNetworksCancel : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhoneGetDetectedNetworksCancelApiId = KDispatchPhoneFuncUnitId + 10;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetDetectedNetworksCancel
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneGetDetectedNetworksCancelComp()
	 *
	 * Implementation of this interface should cancel a pending retrieval of the detected networks.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetDetectedNetworksCancelReqL() = 0;

	}; // class MLtsyDispatchPhoneGetDetectedNetworksCancel



class MLtsyDispatchPhoneSelectNetwork : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhoneSelectNetworkApiId = KDispatchPhoneFuncUnitId + 11;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSelectNetwork
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneSelectNetworkComp()
	 *
	 * Implementation of this interface should attempt a network selection.
	 *
	 * @param aIsManual Specifies whether phone should use a manual or automatic network selection method.
	 * @param aNetworkManualSelection If aIsManual==ETrue, then this parameter contain the user's manually selected network.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSelectNetworkReqL(TBool aIsManual, const RMobilePhone::TMobilePhoneNetworkManualSelection& aNetworkManualSelection) = 0;

	}; // class MLtsyDispatchPhoneSelectNetwork



class MLtsyDispatchPhoneSelectNetworkCancel : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhoneSelectNetworkCancelApiId = KDispatchPhoneFuncUnitId + 12;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSelectNetworkCancel
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneSelectNetworkCancelComp()
	 *
	 * Implementation of this interface should cancel a pending network selection.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSelectNetworkCancelReqL() = 0;

	}; // class MLtsyDispatchPhoneSelectNetworkCancel



class MLtsyDispatchPhoneSetNetworkSelectionSetting : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchPhoneSetNetworkSelectionSettingApiId = KDispatchPhoneFuncUnitId + 13;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetNetworkSelectionSetting
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneSetNetworkSelectionSettingComp()
	 *
	 * Implementation of this interface should set the new network selection.
	 * Note: 	The completion of this will also invoke the network selection change
	 * 			notifier in the CTSY (EMobilePhoneNotifyNetworkSelectionSettingChange).
	 *
	 * @param aPhoneNetworkSelection The new network selection setting.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetNetworkSelectionSettingReqL(const RMobilePhone::TMobilePhoneNetworkSelectionV1& aPhoneNetworkSelection) = 0;

	}; // class MLtsyDispatchPhoneSetNetworkSelectionSetting


class MLtsyDispatchPhoneGetCurrentNetworkInfo : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetCurrentNetworkInfoApiId = KDispatchPhoneFuncUnitId + 14;

	/**
	 * The CTSY Dispatcher shall invoke this function after a NotifyModemReadyReceived event occurs,
	 * normally during phone boot-up. It is an internal call.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetCurrentNetworkInfoComp()
	 *
	 * Implementation of this interface should get the current network information.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCurrentNetworkInfoReqL() = 0;

	}; // class MLtsyDispatchPhoneGetCurrentNetworkInfo


class MLtsyDispatchPhoneGetNetworkMode : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetNetworkModeApiId = KDispatchPhoneFuncUnitId + 15;

	/**
	 * The CTSY Dispatcher shall invoke this function after a NotifyModemReadyReceived event occurs,
	 * normally during phone boot-up. It is an internal call.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetNetworkModeComp()
	 *
	 * Implementation of this interface should get the current network information.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetNetworkModeReqL() = 0;

	}; // class MLtsyDispatchPhoneGetNetworkMode


class MLtsyDispatchPhoneGetNitzInfo : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetNitzInfoApiId = KDispatchPhoneFuncUnitId + 16;

	/**
	 * The CTSY Dispatcher shall invoke this function after a NotifyModemReadyReceived event occurs,
	 * normally during phone boot-up. It is an internal call.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetNitzInfoComp()
	 *
	 * Implementation of this interface should get the current network NITZ information.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetNitzInfoReqL() = 0;

	}; // class MLtsyDispatchPhoneGetNitzInfo

class MLtsyDispatchPhoneGetSignalStrength : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetSignalStrengthApiId = KDispatchPhoneFuncUnitId + 17;

	/**
	 * The CTSY Dispatcher shall invoke this function after a NotifyModemReadyReceived event occurs,
	 * normally during phone boot-up. It is an internal call.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetSignalStrengthComp()
	 *
	 * Implementation of this interface should get the current network signal strength information.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetSignalStrengthReqL() = 0;

	}; // class MLtsyDispatchPhoneGetSignalStrength

class MLtsyDispatchPhoneGetBatteryInfo : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetBatteryInfoApiId = KDispatchPhoneFuncUnitId + 18;

	/**
	 * The CTSY Dispatcher shall invoke this function after a NotifyModemReadyReceived event occurs,
	 * normally during phone boot-up. It is an internal call.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetBatteryInfoComp()
	 *
	 * Implementation of this interface should get the current battery information.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetBatteryInfoReqL() = 0;

	}; // class MLtsyDispatchPhoneGetBatteryInfo

class MLtsyDispatchPhoneNspsWakeup : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneNspsWakeupApiId = KDispatchPhoneFuncUnitId + 19;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomNetWakeupIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneNspsWakeupComp().
	 *
	 * Implementation of this interface should wake the phone from a NSPS (No Service Power Save) state.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleNspsWakeupReqL() = 0;

	}; // class MLtsyDispatchPhoneNspsWakeup

class MLtsyDispatchPhoneSetSystemNetworkMode : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneSetSystemNetworkModeApiId = KDispatchPhoneFuncUnitId + 20;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSetSystemNetworkModeIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkModeComp().
	 *
	 * Implementation of this interface should set the system network mode.
	 *
	 * @param aMode The new mode to set the system network to.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetSystemNetworkModeReqL(RMmCustomAPI::TNetworkModeCaps aMode) = 0;

	}; // class MLtsyDispatchPhoneSetSystemNetworkMode

class MLtsyDispatchPhoneGetCurrentSystemNetworkModes : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId = KDispatchPhoneFuncUnitId + 21;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetCurrentSystemNetworkModesIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneGetCurrentSystemNetworkModesComp().
	 *
	 * Implementation of this interface should return the current system network mode.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCurrentSystemNetworkModesReqL() = 0;

	}; // class MLtsyDispatchPhoneGetCurrentSystemNetworkModes

class MLtsyDispatchPhoneResetNetServer : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneResetNetServerApiId = KDispatchPhoneFuncUnitId + 22;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomResetNetServerIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneResetNetServerComp().
	 *
	 * Implementation of this interface should reset the net server.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleResetNetServerReqL() = 0;

	}; // class MLtsyDispatchPhoneResetNetServer

class MLtsyDispatchPhoneSetAlwaysOnMode : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneSetAlwaysOnModeApiId = KDispatchPhoneFuncUnitId + 23;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSetAlwaysOnMode
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneSetAlwaysOnModeComp().
	 *
	 * Implementation of this interface should set the "always on" mode.
	 *
	 * @param aMode The new "always on" mode to set, can be
     *               RMmCustomAPI::EAlwaysModeVPLMN for VPLMN (Visited Public Land Mobile Network) always on,
     *               RMmCustomAPI::EAlwaysModeHPLMN for HPLMN (Home Public Land Mobile Network) always on,
     *               RMmCustomAPI::EAlwaysModeBoth for VPLMN and HPLMN always on,
     *               RMmCustomAPI::EAlwaysModeNeither for neither VPLMN nor HPLMN always on.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetAlwaysOnModeReqL(RMmCustomAPI::TSetAlwaysOnMode aMode) = 0;

	}; // class MLtsyDispatchPhoneSetAlwaysOnMode

class MLtsyDispatchPhoneSetDriveMode : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneSetDriveModeApiId = KDispatchPhoneFuncUnitId + 24;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSetDriveModeIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneSetDriveModeComp().
	 *
	 * Implementation of this interface should set the drive mode status.
	 *
	 * @param aMode The new mode to set (RMmCustomAPI::EDeactivateDriveMode for deactivate drive mode
     *               or RMmCustomAPI::EActivateDriveMode to activate drive mode).
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetDriveModeReqL(RMmCustomAPI::TSetDriveMode aMode) = 0;

	}; // class MLtsyDispatchPhoneSetDriveMode

class MLtsyDispatchPhoneGetHspaStatus : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetHspaStatusApiId = KDispatchPhoneFuncUnitId + 25;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomReadHSxPAStatusIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneGetHspaStatusComp().
	 *
	 * Implementation of this interface should return the phone high speed channel status.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetHspaStatusReqL() = 0;

	}; // class MLtsyDispatchPhoneGetHspaStatus

class MLtsyDispatchPhoneSetHspaStatus : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneSetHspaStatusApiId = KDispatchPhoneFuncUnitId + 26;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomWriteHSxPAStatusIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneSetHspaStatusComp().
	 *
	 * Implementation of this interface should set the phone high speed channel status.
	 *
	 * @param aStatus The new HSxPA status (enabled or disabled).
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetHspaStatusReqL(RMmCustomAPI::THSxPAStatus aStatus) = 0;

	}; // class MLtsyDispatchPhoneSetHspaStatus

class MLtsyDispatchPhoneGetNetworkProviderName : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetNetworkProviderNameApiId = KDispatchPhoneFuncUnitId + 27;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetNetworkProviderNameIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneGetNetworkProviderNameComp().
	 *
	 * Implementation of this interface should return the name of the current network provider.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetNetworkProviderNameReqL() = 0;

	}; // class MLtsyDispatchPhoneGetNetworkProviderName

class MLtsyDispatchPhoneGetOperatorName : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetOperatorNameApiId = KDispatchPhoneFuncUnitId + 28;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetOperatorNameIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneGetOperatorNameComp().
	 *
	 * Implementation of this interface should return the name of the current operator.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetOperatorNameReqL() = 0;

	}; // class MLtsyDispatchPhoneGetOperatorName

class MLtsyDispatchPhoneGetCellInfo : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetCellInfoApiId = KDispatchPhoneFuncUnitId + 29;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetCellInfoIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking one of the
	 * CCtsyDispatcherCallback::CallbackPhoneGetCellInfoComp().
	 *
	 * Implementation of this interface should return the cell information.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCellInfoReqL() = 0;

	}; // class MLtsyDispatchPhoneGetCellInfo

class MLtsyDispatchPhoneRegisterCellInfoChangeNotification : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId = KDispatchPhoneFuncUnitId + 30;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECtsyPhoneCellInfoIndReq
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneRegisterCellInfoChangeNotificationComp()
	 *
	 * Implementation of this interface should inform the LTSY that the CTSY wishes to register for
	 * notifications when the Cell Information has changed.
	 * 
	 * These notifications can be provided via the LTSY by invoking
	 * CCtsyDispatcherCallback::CallbackPhoneNotifyCellInfoChangeInd()
	 * 
	 * This call from the CTSY only occurs once on the first client side call of RMobilePhone::NotifyCellInfoChange().
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhone::NotifyCellInfoChange()
	 */
	virtual TInt HandleRegisterCellInfoChangeNotificationReqL() = 0;

	}; // class MLtsyDispatchPhoneRegisterCellInfoChangeNotification

class MLtsyDispatchPhoneGetPhoneCellInfo : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetPhoneCellInfoApiId = KDispatchPhoneFuncUnitId + 31;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECtsyPhoneCellInfoReq
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetPhoneCellInfoComp()
	 *
	 * Implementation of this interface should retrieve the current Cell Information.
	 * 
	 * Cell information is cached within the CTSY thus this handler is only invoked if the
	 * CTSY does not currently hold valid cell information. E.g. the CTSY's cache has never
	 * been populated as GetPhoneCellInfo Request-Complete has never been exercised, or never
	 * been populated via CallbackPhoneNotifyCellInfoChangeInd(), or CallbackPhoneNotifyCellInfoChangeInd()
	 * is completed with an error thus marking the cache as dirty.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobilePhone::GetCellInfo()
	 */
	virtual TInt HandleGetPhoneCellInfoReqL() = 0;

	}; // class MLtsyDispatchPhoneGetPhoneCellInfo

class MLtsyDispatchPhoneGetUsimServiceSupport : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetUsimServiceSupportApiId = KDispatchPhoneFuncUnitId + 32;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetServiceTableSupportbyApplicationIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetUsimServiceSupportComp()
	 *
	 * Implementation of this interface should retrieve if the application is supported.
	 * 
	 * @param aApplicationNumber The application number to check support for in the USIM.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 *  @see RMmCustomAPI::GetUSIMServiceSupport()
	 */
	virtual TInt HandleGetGetUsimServiceSupportReqL(TInt aApplicationNumber) = 0;

	}; // class MLtsyDispatchPhoneGetUsimServiceSupport

class MLtsyDispatchPhoneGetCurrentActiveUsimApplication : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId = KDispatchPhoneFuncUnitId + 33;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetCurrentActiveUSimApplication
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetCurrentActiveUsimApplicationComp()
	 *
	 * Implementation of this interface should retrieve the AID of the current active USIM application.
	 * (There is only ever at most one USIM application active at a time)
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 *  @see RMmCustomAPI::GetCurrentActiveUSimApplication()
	 */
	virtual TInt HandleGetCurrentActiveUsimApplicationReqL() = 0;

	}; // class MLtsyDispatchPhoneGetCurrentActiveUsimApplication

class MLtsyDispatchPhoneTerminateAllCalls : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneTerminateAllCallsApiId = KDispatchPhoneFuncUnitId + 34;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECtsyPhoneTerminateAllCallsReq
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneTerminateAllCallsComp()
	 *
	 * Implementation of this interface should terminate all active calls.
	 * 
	 * @param aCallId if a call is in the connecting state then this parameter will contain the id of that call, else 0.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 *  @see RMobilePhone::TerminateAllCalls()
	 */
	virtual TInt HandleTerminateAllCallsReqL(TInt aCallId) = 0;

	}; // class MLtsyDispatchPhoneTerminateAllCalls

class MLtsyDispatchPhoneGetSystemNetworkBand : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneGetSystemNetworkBandApiId = KDispatchPhoneFuncUnitId + 35;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetBandSelectionIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneGetSystemNetworkBandComp()
	 *
	 * Implementation of this interface should return the current band and network mode.	 
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 *  @see RMmCustomApi::GetSystemNetworkBand()
	 */
	virtual TInt HandleGetSystemNetworkBandReqL() = 0;

	}; // class MLtsyDispatchPhoneGetSystemNetworkBand

class MLtsyDispatchPhoneSetSystemNetworkBand : public MLtsyDispatchInterface
	{
public:

    static const TInt KLtsyDispatchPhoneSetSystemNetworkBandApiId = KDispatchPhoneFuncUnitId + 36;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSetBandSelectionIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackPhoneSetSystemNetworkBandComp()
	 *
	 * Implementation of this interface should set the current band and network mode.	 
	 * 
	 * @param aBand the desired network band.
	 * @param aMode the desired network mode.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 *  @see RMmCustomApi::SetSystemNetworkBand()
	 */
	virtual TInt HandleSetSystemNetworkBandReqL(RMmCustomAPI::TBandSelection aBand, RMmCustomAPI::TNetworkModeCaps aMode) = 0;

	}; // class MLtsyDispatchPhoneSetSystemNetworkBand

#endif /*MLTSYDISPATCHPHONEINTERFACE_H_*/
