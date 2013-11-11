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

#ifndef CCTSYDISPATCHERCALLBACK_H
#define CCTSYDISPATCHERCALLBACK_H

// INCLUDES
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <e32base.h>
#include <e32std.h>
#include <etelmm.h>
#include <etelsat.h>
#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>
#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/mltsydispatchsmsinterface.h>
#include <ctsy/ltsy/mltsydispatchcellbroadcastinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>

#include <mmlist.h>
#include <ctsy/rmmcustomapi.h>



// FORWARD DECLARATIONS
class TDispatcherHolder;

/**
 * These are the IDs used to identify the callback indicators APIs
 * offered through CCtsyDispatcherCallback. There is one for each callback indicator API.
 *
 * The indicators are used as part of a MLtsyDispatchFactoryV1::IsCallbackIndicatorSupported
 * implementation to indicate whether the baseband supports indication.
 *
 * Each Id is identified by its funcitonal unit and group.  All Ids are contained within an enum whose name ends
 * with a group number e.g.  TLtsyDispatchCallControlCallbackIndicatorIds_Group1.
 *
 * When there is a need to identify >32 IDs for a functional unit a new group enum
 * must be created e.g.  TLtsyDispatchCallControlCallbackIndicatorIds_Group2
 *
 * @see MLtsyDispatchFactoryV1::IsCallbackIndicatorSupported()
 */

/**
 * Call Control callback indicator IDs
 */
enum TLtsyDispatchCallControlCallbackIndicatorIds_Group1
  {
  KLtsyDispatchCallControlNotifyCallStatusChangeIndId = 1,
  KLtsyDispatchCallControlReceiveUuiIndId = 2,
  KLtsyDispatchCallControlNotifyDataCallCapsChangeIndId = 4,
  KLtsyDispatchCallControlNotifyIncomingCallIndId = 8,
  KLtsyDispatchCallControlNotifyHscsdInfoChangeIndId = 16,
  KLtsyDispatchCallControlNotifyCallEventIndId = 32,
  KLtsyDispatchCallControlGetCallInfoIndId = 64,
  KLtsyDispatchCallControlNotifyIccCallForwardingStatusChangeIndId = 128
  										// = 256
  };

/**
 * Call Control Multiparty callback indicator IDs
 */
enum TLtsyDispatchCallControlMultipartyCallbackIndicatorIds_Group1
  {
  // = 1 Put multiparty indicator IDs here if some are added. Follow naming convention.
  // = 2
  // = 4
  };

/**
 * Cell Broadcast callback indicator IDs
 */
enum TLtsyDispatchCellBroadcastCallbackIndicatorIds_Group1
  {
  KLtsyDispatchCellBroadcastGsmBroadcastNotifyMessageReceivedIndId = 1,
  KLtsyDispatchCellBroadcastWcdmaBroadcastMessageReceivedIndId = 2,
  };

/**
 * Packet Services callback indicator IDs
 */
enum TLtsyDispatchPacketServicesCallbackIndicatorIds_Group1
  {
  KLtsyDispatchPacketServicesNotifyPacketAttachModeChangeIndId = 1,
  KLtsyDispatchPacketServicesNotifyQosProfileChangedIndId = 2,
  KLtsyDispatchPacketServicesNotifyRatTransferCapsChangeIndId = 4,
  KLtsyDispatchPacketServicesNotifyPacketMsClassChangeIndId = 8,
  KLtsyDispatchPacketServicesNotifyPacketDynamicCapsChangeIndId = 16,
  KLtsyDispatchPacketServicesNotifyPdpContextAddedIndId = 32,
  KLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId = 64,
  KLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId = 128,
  KLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId = 256,
  KLtsyDispatchPacketServicesNotifyPacketNetworkRegistrationStatusIndId = 512,
  KLtsyDispatchPacketServicesNotifyMbmsContextConfigChangedIndId = 1024,
  KLtsyDispatchPacketServicesNotifyMbmsNetworkServiceStatusChangeIndId = 2048,
  KLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId = 4096
  };


/**
 * Phonebook callback indicator IDs
 */
enum TLtsyDispatchPhonebookCallbackIndicatorIds_Group1
  {
  KLtsyDispatchPhonebookStoreResetCacheIndId = 1,
  KLtsyDispatchPhonebookStoreGetFdnInfoIndId = 2
  };

/**
 * Phonebook ON callback indicator IDs
 */
enum TLtsyDispatchPhonebookONCallbackIndicatorIds_Group1
  {
  KLtsyDispatchPhonebookOnStoreReadAllIndId = 1,
  KLtsyDispatchPhonebookOnStoreWriteAllIndId = 2
  };

/**
 * Phone callback indicator IDs
 */
enum TLtsyDispatchPhoneCallbackIndicatorIds_Group1
  {
  KLtsyDispatchPhoneNotifyNetworkModeChangeIndId = 1,
  KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId = 2,
  KLtsyDispatchPhoneNotifySignalStrengthChangeIndId = 4,
  KLtsyDispatchPhoneNotifyNetworkRegistrationStatusChangeIndId = 8,
  KLtsyDispatchPhoneNotifyNitzInfoChangeIndId = 16,
  KLtsyDispatchPhoneNotifyBatteryInfoChangeIndId = 32,
  KLtsyDispatchPhoneNotifyCurrentNetworkChangeIndId = 64,
  KLtsyDispatchPhoneNotifyRfStatusChangeIndId = 128,
  KLtsyDispatchPhoneNotifyCellInfoChangeIndId = 256
  };

/**
 * SAT callback indicator IDs
 */
enum TLtsyDispatchSatCallbackIndicatorIds_Group1
  {
  KLtsyDispatchSatUssdStatusIndId = 1,
  KLtsyDispatchSatNotifyCallControlRequestIndId = 2,
  KLtsyDispatchSatPcmdIndId = 4,
  KLtsyDispatchSatNotifyProactiveSimSessionEndIndId = 8,
  KLtsyDispatchSatNotifyLocalInfoIndId = 16,
  KLtsyDispatchSatTimeZoneIndId = 32,
  KLtsyDispatchSatAccessTechnologyChangeIndId = 64,
  KLtsyDispatchSatSetProactiveCommandOnGoingStatusIndId = 128,
  KLtsyDispatchSatCallConnectedIndId = 256,
  KLtsyDispatchSatSsStatusIndId = 512,
  KLtsyDispatchSatCallDisconnectedIndId = 1024,
  KLtsyDispatchSatMtCallIndId = 2048,
  KLtsyDispatchSatCheckAtkDataForAlphaIdIndId = 4096,
  KLtsyDispatchSatCreateEnvelopeIndId = 8192,
  KLtsyDispatchSatSmControlEnvelopeIndId = 16384,
  KLtsyDispatchSatImsiChangedIndId = 32768,
  KLtsyDispatchSatLocationStatusIndId = 65536,
  KLtsyDispatchSatRemoveEventListIndId = 131072,
  KLtsyDispatchSatRefreshIndId = 262144,
  KLtsyDispatchSatSmControlResponseIndId = 524288,
  };

/**
 * Security callback indicator IDs
 */
enum TLtsyDispatchSecurityCallbackIndicatorIds_Group1
  {
  KLtsyDispatchSecurityBootNotifySecurityReadyIndId = 1,
  KLtsyDispatchSecurityNotifySecurityEventIndId = 2,
  KLtsyDispatchSecurityNotifyLockInfoChangeIndId = 4
  };

/**
 * SIM callback indicator IDs
 */
enum TLtsyDispatchSimCallbackIndicatorIds_Group1
  {
  KLtsyDispatchSimRefreshSimFilesIndId = 1,
  KLtsyDispatchSimNotifyIccMessageWaitingIndicatorsChangeIndId = 2,
  KLtsyDispatchSimNotifyApnListChangeIndId = 4,
  KLtsyDispatchSimNotifyApnControlListServiceStatusChangeIndId = 8,
  KLtsyDispatchSimGetActiveIccApplicationTypeIndId = 16,
  };

/**
 * Sms callback indicator IDs
 */
enum TLtsyDispatchSmsCallbackIndicatorIds_Group1
  {
  KLtsyDispatchSmsNotifyReceiveSmsMessageIndId = 1
  };

/**
 * Supplementary Services callback indicator IDs
 */
enum TLtsyDispatchSupplementaryServicesCallbackIndicatorIds_Group1
  {
  KLtsyDispatchSupplementaryServicesNotifyCallBarringStatusChangeIndId = 1,
  KLtsyDispatchSupplementaryServicesReceiveUssdMessageIndId = 2,
  KLtsyDispatchSupplementaryServicesNotifyCallWaitingStatusChangeIndId = 4,
  KLtsyDispatchSupplementaryServicesNotifyCallForwardingStatusChangeIndId = 8,
  KLtsyDispatchSupplementaryServicesNotifyCallForwardingActiveIndId = 16,
  KLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId = 32
  };

// CLASS DECLARATION

/**
 * CCtsyDispatcherCallback
 *
 * This class is responsible for receiving completions from the Licensee LTSY and
 * forwarding them onto the relevant dispatcher object in the CTSY Dispatcher which
 * will handle the completion.
 */
class CCtsyDispatcherCallback : public CBase
	{
public:

	virtual ~CCtsyDispatcherCallback();
	static CCtsyDispatcherCallback* NewL();
	static CCtsyDispatcherCallback* NewLC();

	// Callback functions called by the Licensee LTSY to complete a request Callback<something>Comp()
	// or to send an unsolicited completion Callback<something>Ind().

	// CallControl related callbacks
	IMPORT_C void CallbackCallControlNotifyCallStatusChangeInd(TInt aError, TInt aCallId, RMobileCall::TMobileCallStatus aMobileCallStatus);
	IMPORT_C void CallbackCallControlReceiveUuiInd(TInt aError, TInt aCallId, const TDesC& aCallUui);
	IMPORT_C void CallbackCallControlNotifyDataCallCapsChangeInd(TInt aError, TInt aCallId, const RMobileCall::TMobileCallDataCapsV1& aMobileCallDataCaps);
	IMPORT_C void CallbackCallControlNotifyIncomingCallInd(TInt aError, const RMobileCall::TMobileCallInfoV1& aCallInfo);
	IMPORT_C void CallbackCallControlNotifyHscsdInfoChangeInd(TInt aError, TInt aCallId, const RMobileCall::TMobileCallHscsdInfoV8& aCallParams, const RMobileCall::TMobileCallDataCapsV1& aCallCaps);
	IMPORT_C void CallbackCallControlNotifyCallEventInd(TInt aError, TInt aCallId, RMobileCall::TMobileCallEvent aCallEvent);
	IMPORT_C void CallbackCallControlNotifyCallInfoChangeInd(TInt aError, const RMobileCall::TMobileCallInfoV1& aMobileCallInfo);
    IMPORT_C void CallbackCallControlRemoteAlertingToneChangeInd(TInt aError, RMmCustomAPI::TRemoteAlertingToneStatus aRemoteAlertingTone);
	IMPORT_C void CallbackCallControlAnswerComp(TInt aError, TInt aCallId);
	IMPORT_C void CallbackCallControlHoldComp(TInt aError, TInt aCallId);
	IMPORT_C void CallbackCallControlDialEmergencyComp(TInt aError, TInt aCallId);
	IMPORT_C void CallbackCallControlStopDtmfToneComp(TInt aError);
	IMPORT_C void CallbackCallControlSetActiveAlsLineComp(TInt aError);
	IMPORT_C void CallbackCallControlSendDtmfTonesCancelComp(TInt aError);
	IMPORT_C void CallbackCallControlHangUpComp(TInt aError, TInt aCallId);
	IMPORT_C void CallbackCallControlResumeComp(TInt aError, TInt aCallId);
	IMPORT_C void CallbackCallControlSetDynamicHscsdParamsComp(TInt aError, TInt aCallId);
	IMPORT_C void CallbackCallControlDialVoiceComp(TInt aError, TInt aCallId);
	IMPORT_C void CallbackCallControlTransferComp(TInt aError);
	IMPORT_C void CallbackCallControlSendDtmfTonesComp(TInt aError);
	IMPORT_C void CallbackCallControlGetIdentityServiceStatusComp(TInt aError, RMobilePhone::TMobilePhoneIdServiceStatus aIdentityServiceStatus);
	IMPORT_C void CallbackCallControlSwapComp(TInt aError);
	IMPORT_C void CallbackCallControlStartDtmfToneComp(TInt aError);
	IMPORT_C void CallbackCallControlGetActiveAlsLineComp(TInt aError, RMobilePhone::TMobilePhoneALSLine aAlsLine);
	IMPORT_C void CallbackCallControlDialDataComp(TInt aError, TInt aCallId);
	IMPORT_C void CallbackCallControlUpdateLifeTimerComp(TInt aError);
	IMPORT_C void CallbackCallControlNotifyIccCallForwardingStatusChangeInd(TInt aError, const RMobilePhone::TMobileAddress& aCallForwardingNo,
				 		RMobilePhone::TCFUIndicatorStatusFlags aCallForwardingStatusFlags,
				 		RMmCustomAPI::TMultipleSubscriberProfileID aMultipleSubscriberProfileId);
    IMPORT_C void CallbackCallControlGetAlsPpSupportComp(TInt aError, RMmCustomAPI::TAlsSupport aAlsSupport);
    IMPORT_C void CallbackCallControlGetAlsBlockedStatusComp(TInt aError, RMmCustomAPI::TGetAlsBlockStatus aAlsStatus);
    IMPORT_C void CallbackCallControlSetAlsBlockedComp(TInt aError);
    IMPORT_C void CallbackCallControlGetLifeTimeComp(TInt aError);
    IMPORT_C void CallbackCallControlGetLifeTimeComp(TInt aError, TUint32 aHours, TUint8 aMinutes);
    IMPORT_C void CallbackCallControlGetLifeTimeComp(TInt aError, const TDateTime &aManufacturingDate);
    IMPORT_C void CallbackCallControlGetLifeTimeComp(TInt aError, const TDateTime &aManufacturingDate, TUint32 aHours, TUint8 aMinutes);
    IMPORT_C void CallbackCallControlTerminateErrorCallComp(TInt aError);
    IMPORT_C void CallbackCallControlTerminateAllCallsComp(TInt aError);
    IMPORT_C void CallbackCallControlGetCallForwardingIndicatorComp(TInt aError, RMobilePhone::TMobileTON aTypeOfNumber,
                                                                    RMobilePhone::TMobileNPI aMobilePlan,
                                                                    const TDesC &aNumber,
                                                                    RMobilePhone::TCFUIndicatorStatusFlags aCFUIndicatorStatusFlags,
                                                                    RMobilePhone::TMultipleSubscriberProfileID aMultipleSubscriberProfileId);

	// Phone related callbacks
	IMPORT_C void CallbackPhoneGetFdnStatusComp(TInt aError, RMobilePhone::TMobilePhoneFdnStatus aFdnStatus);
	IMPORT_C void CallbackPhoneGetNetworkRegistrationStatusComp(TInt aError, RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus);
	IMPORT_C void CallbackPhoneBootNotifyModemStatusReadyComp(TInt aError, TRfStateInfo aRfStatus);
	IMPORT_C void CallbackPhoneBootNotifySimStatusReadyComp(TInt aError);
	IMPORT_C void CallbackPhoneGetHomeNetworkComp(TInt aError, const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo);
	IMPORT_C void CallbackPhoneNotifyNetworkModeChangeInd(TInt aError, RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);
	IMPORT_C void CallbackPhoneNotifyEgprsInfoChangeInd(TInt aError, TBool aEdgeSupported);
	IMPORT_C void CallbackPhoneNotifySignalStrengthChangeInd(TInt aError, TInt32 aSignalStrength, TInt8 aBar);
	IMPORT_C void CallbackPhoneNotifyNetworkRegistrationStatusChangeInd(TInt aError, RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus);
	IMPORT_C void CallbackPhoneNotifyNitzInfoChangeInd(TInt aError, const RMobilePhone::TMobilePhoneNITZ& aNitzInfo);
	IMPORT_C void CallbackPhoneNotifyBatteryInfoChangeInd(TInt aError, const RMobilePhone::TMobilePhoneBatteryInfoV1& aBatteryInfo);
	IMPORT_C void CallbackPhoneNotifyCurrentNetworkChangeInd(TInt aError, const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo, const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	IMPORT_C void CallbackPhoneGetServiceProviderNameComp(TInt aError, const RMobilePhone::TMobilePhoneServiceProviderNameV2& aServiceProvider);
	IMPORT_C void CallbackPhoneGetPhoneIdComp(TInt aError, const RMobilePhone::TMobilePhoneIdentityV1& aPhoneId);
	IMPORT_C void CallbackPhoneGetDetectedNetworksComp(TInt aError, const CMobilePhoneNetworkListV2& aPhoneNetworkList);
	IMPORT_C void CallbackPhoneGetDetectedNetworksCancelComp(TInt aError);
	IMPORT_C void CallbackPhoneSelectNetworkComp(TInt aError);
	IMPORT_C void CallbackPhoneSelectNetworkCancelComp(TInt aError);
	IMPORT_C void CallbackPhoneSetNetworkSelectionSettingComp(TInt aError, RMobilePhone::TMobilePhoneSelectionMethod aSelectionMethod);
	IMPORT_C void CallbackPhoneNotifyRfStatusChangeInd(TInt aError, TRfStateInfo aRfStatus);
	IMPORT_C void CallbackPhoneGetCurrentNetworkInfoComp(TInt aError, const RMobilePhone::TMobilePhoneNetworkInfoV5& aNetworkInfo, const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);
	IMPORT_C void CallbackPhoneGetNetworkModeComp(TInt aError, RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);
	IMPORT_C void CallbackPhoneGetNitzInfoComp(TInt aError, const RMobilePhone::TMobilePhoneNITZ& aNitzInfo);
	IMPORT_C void CallbackPhoneGetSignalStrengthComp(TInt aError, TInt32 aSignalStrength, TInt8 aBar);
	IMPORT_C void CallbackPhoneGetBatteryInfoComp(TInt aError, const RMobilePhone::TMobilePhoneBatteryInfoV1& aBatteryInfo);
	IMPORT_C void CallbackPhoneSimRefreshRegisterComp(TInt aError);
	IMPORT_C void CallbackPhoneNotifyNspsStatusChangeInd(TInt aError, TBool aEnabled);
	IMPORT_C void CallbackPhoneNotifyNetworkSelectionSettingChangeInd(TInt aError, RMobilePhone::TMobilePhoneSelectionMethod aSelectionMethod);
	IMPORT_C void CallbackPhoneGetCellInfoComp(TInt aError);
	IMPORT_C void CallbackPhoneGetCellInfoComp(TInt aError, const RMmCustomAPI::TMmCellInfo::TGSMCellInfo &aInfo);
    IMPORT_C void CallbackPhoneGetCellInfoComp(TInt aError,
                                               const RMmCustomAPI::TMmCellInfo::TWCDMACellInfo &aInfo);
	IMPORT_C void CallbackPhoneNotifyCellInfoChangedInd(TInt aError);
	IMPORT_C void CallbackPhoneNotifyCellInfoChangedInd(TInt aError, const RMmCustomAPI::TMmCellInfo::TGSMCellInfo &aInfo);
	IMPORT_C void CallbackPhoneNotifyCellInfoChangedInd(TInt aError, const RMmCustomAPI::TMmCellInfo::TWCDMACellInfo &aInfo);
    IMPORT_C void CallbackPhoneNspsWakeupComp(TInt aError);
    IMPORT_C void CallbackPhoneSetSystemNetworkModeComp(TInt aError);
    IMPORT_C void CallbackPhoneGetCurrentSystemNetworkModesComp(TInt aError, TUint32 aCurrentSystemNetworkModes);
	IMPORT_C void CallbackPhoneNotifyRauEventInd(TInt aError, RMmCustomAPI::TRauEventStatus aRauEvent);
	IMPORT_C void CallbackPhoneNotifyNetworkConnectionFailureInd(TInt aError);
	IMPORT_C void CallbackPhoneNotifyDtmfEventInd(TInt aError, RMmCustomAPI::TDtmfEventType aType,
                                                  RMmCustomAPI::TDtmfEvent aEvent, TChar aTone);
	IMPORT_C void CallbackPhoneNotifyDtmfEventInd(TInt aError, RMobilePhone::TMobilePhoneDTMFEvent aEventType);
    IMPORT_C void CallbackPhoneResetNetServerComp(TInt aError);
    IMPORT_C void CallbackPhoneSetAlwaysOnModeComp(TInt aError);
    IMPORT_C void CallbackPhoneSetDriveModeComp(TInt aError);
    IMPORT_C void CallbackPhoneGetHspaStatusComp(TInt aError, RMmCustomAPI::THSxPAStatus aStatus);
    IMPORT_C void CallbackPhoneSetHspaStatusComp(TInt aError);
	IMPORT_C void CallbackPhoneNotifyHspaStatusChangedInd(TInt aError, RMmCustomAPI::THSxPAStatus aStatus);
    IMPORT_C void CallbackPhoneGetNetworkProviderNameComp(TInt aError, const TDesC& aName);
    IMPORT_C void CallbackPhoneGetOperatorNameComp(TInt aError, RMmCustomAPI::TOperatorNameType aType, const TDesC& aName);
    IMPORT_C void CallbackPhoneRegisterCellInfoChangeNotificationComp(TInt aError);
	IMPORT_C void CallbackPhoneGetPhoneCellInfoComp(TInt aError, const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo);
	IMPORT_C void CallbackPhoneNotifyCellInfoChangeInd(TInt aError, const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo);
	IMPORT_C void CallbackPhoneGetUsimServiceSupportComp(TInt aError, TInt aApplicationNumber, TBool aSupported);
	IMPORT_C void CallbackPhoneGetCurrentActiveUsimApplicationComp(TInt aError, const TDesC8& aAid);
	IMPORT_C void CallbackPhoneTerminateAllCallsComp(TInt aError);
	IMPORT_C void CallbackPhoneGetSystemNetworkBandComp(TInt aError, RMmCustomAPI::TBandSelection aBand, RMmCustomAPI::TNetworkModeCaps aMode);
	IMPORT_C void CallbackPhoneSetSystemNetworkBandComp(TInt aError);

	// Security related callbacks
	IMPORT_C void CallbackSecurityBootNotifySecurityReadyInd(TInt aError);
	IMPORT_C void CallbackSecurityNotifySecurityEventInd(TInt aError, RMobilePhone::TMobilePhoneSecurityEvent aSecEvent);
	IMPORT_C void CallbackSecurityNotifyLockInfoChangeInd(TInt aError, RMobilePhone::TMobilePhoneLockStatus aStatus, RMobilePhone::TMobilePhoneLockSetting aSetting, RMobilePhone::TMobilePhoneLock aLock);
	IMPORT_C void CallbackSecurityGetSecurityCodeInfoComp(TInt aError, RMobilePhone::TMobilePhoneSecurityCode aSecCode, 
	                      TInt aRemainingEntryAttempts);
	IMPORT_C void CallbackSecurityGetLockInfoComp(TInt aError, RMobilePhone::TMobilePhoneLockStatus aLockStatus, RMobilePhone::TMobilePhoneLockSetting aLockSetting);
	IMPORT_C void CallbackSecurityAbortSecurityCodeComp(TInt aError);
	IMPORT_C void CallbackSecurityGetCurrentActivePinComp(TInt aError, RMobilePhone::TMobilePhoneSecurityCode aSecCode);
	IMPORT_C void CallbackSecuritySetLockSettingComp(TInt aError);	                     
	IMPORT_C void CallbackSecurityVerifySecurityCodeComp(TInt aError);
	IMPORT_C void CallbackSecurityGetPin1DisableSupportedComp(TInt aError, TBool aIsSupported);
	IMPORT_C void CallbackSecurityNotifyCipheringInfoChangeInd(TInt aError, const RMmCustomAPI::TCipheringInfo& aInfo);
	IMPORT_C void CallbackSecurityCheckSecurityCodeComp(TInt aError);
	IMPORT_C void CallbackSecurityDisablePhoneLockComp(TInt aError);
	IMPORT_C void CallbackSecurityGetCipheringInfoComp(TInt aError, TBool aIndStatus);
	IMPORT_C void CallbackSecurityIsSecurityCodeBlockedComp(TInt aError, TBool aIsBlocked);	
	IMPORT_C void CallbackSecurityCheckSecurityCodeCancelComp(TInt aError);

	// PhonebookEn related callbacks
	IMPORT_C void CallbackPhonebookEnStoreGetInfoComp(TInt aError, TInt aUsedEntries);
	IMPORT_C void CallbackPhonebookEnStoreReadAllComp(TInt aError, TInt aIndex, const TDesC& aNumber, TBool aMoreToCome);
	IMPORT_C void CallbackPhonebookEnStoreReadEntryComp(TInt aError, TInt aIndex, const TDesC& aNumber);

	// CellBroadcast related callbacks
	IMPORT_C void CallbackCellBroadcastGsmBroadcastNotifyMessageReceivedInd(TInt aError, const TDesC8& aCbsMsg);
	IMPORT_C void CallbackCellBroadcastWcdmaBroadcastMessageReceivedInd(TInt aError, const TDesC8& aWcdmaCbsData, const DispatcherCellBroadcast::TWcdmaCbsMsgBase& aWcdmaCbsMsgBase, TBool aMoreToCome);
	IMPORT_C void CallbackCellBroadcastSetBroadcastFilterSettingComp(TInt aError);
	IMPORT_C void CallbackCellBroadcastActivateBroadcastReceiveMessageComp(TInt aError);
	IMPORT_C void CallbackCellBroadcastReceiveMessageCancelComp(TInt aError);
	IMPORT_C void CallbackCellBroadcastStartSimCbTopicBrowsingComp(TInt aError, const CArrayFixFlat< RMmCustomAPI::TSimCbTopic >& aSimTopicArray );
	IMPORT_C void CallbackCellBroadcastDeleteSimCbTopicComp(TInt aError);

	// PhonebookOn related callbacks
	IMPORT_C void CallbackPhonebookOnStoreReadAllInd(TInt aError);
	IMPORT_C void CallbackPhonebookOnStoreReadComp(TInt aError, TInt aIndex, const TDesC& aName, const TDesC& aNumber);
	IMPORT_C void CallbackPhonebookOnStoreDeleteAllComp(TInt aError);
	IMPORT_C void CallbackPhonebookOnStoreReadEntryComp(TInt aError, TInt aIndex, const TDesC& aName, const TDesC& aNumber);
	IMPORT_C void CallbackPhonebookOnStoreGetInfoComp(TInt aError, TInt aNumOfEntries, TInt aUsedEntries, TInt aNameLen, TInt aNumLen);
	IMPORT_C void CallbackPhonebookOnStoreGetReadStoreSizeComp(TInt aError, TInt aSize);
	IMPORT_C void CallbackPhonebookOnStoreDeleteEntryComp(TInt aError);
	IMPORT_C void CallbackPhonebookOnStoreWriteEntryComp(TInt aError);
	IMPORT_C void CallbackPhonebookOnStoreWriteComp(TInt aError, TInt aLocation);
	IMPORT_C void CallbackPhonebookOnStoreGetStoreSizeComp(TInt aError, TInt aSize);

	// Phonebook related callbacks
	IMPORT_C void CallbackPhonebookStoreResetCacheInd(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook);
	IMPORT_C void CallbackPhonebookStoreSetFdnPhonebookInfoInd(TInt aError, TInt aTotalEntries , TInt aMaxTextLength , TInt aMaxNumberLength);
	IMPORT_C void CallbackPhonebookStoreReadEntryComp(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aPhonebookEntries);
	IMPORT_C void CallbackPhonebookStoreDeleteEntryComp(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aMaxNumberLength = -1);
	IMPORT_C void CallbackPhonebookStoreCacheComp(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, const TDesC8& aPhonebookEntries);
	IMPORT_C void CallbackPhonebookStoreGetPhonebookInfoComp(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aUsedEntries);
	IMPORT_C void CallbackPhonebookStoreInitialiseComp(TInt aError, const DispatcherPhonebook::TPhonebookStoreInfoV1& aPhonebookStoreInfo);
	IMPORT_C void CallbackPhonebookStoreDeleteAllComp(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TInt aMaxNumberLength = -1);
	IMPORT_C void CallbackPhonebookStoreWriteEntryComp(TInt aError, DispatcherPhonebook::TPhonebook aPhonebook, TUint16 aIndex, TInt aMaxNumberLength = -1);
	IMPORT_C void CallbackPhonebookGetPhoneStoreInfoComp(TInt aError, const RMobilePhoneStore::TMobilePhoneStoreInfoV1& aPhoneStoreInfo);
	IMPORT_C void CallbackPhonebookSmsStoreReadAllComp(TInt aError, const DispatcherPhonebook::TSmsData& aSmsData, TInt aIndex, TBool aMoreToCome, TBool aReceivedClass2ToBeResent);
	IMPORT_C void CallbackPhonebookSmsStoreDeleteEntryComp(TInt aError);
	IMPORT_C void CallbackPhonebookSmsStoreDeleteAllComp(TInt aError);
	IMPORT_C void CallbackPhonebookSmsStoreGetInfoComp(TInt aError, TInt aTotalEntries, TInt aUsedEntries);
	IMPORT_C void CallbackPhonebookSmsStoreReadEntryComp(TInt aError, const DispatcherPhonebook::TSmsData& aSmsData);
	IMPORT_C void CallbackPhonebookSmsStoreWriteEntryComp(TInt aError, TInt aLocation, TBool aReceivedClass2ToBeResent);

	// Sim related callbacks
	IMPORT_C void CallbackSimRefreshSimFilesInd(TInt aError, TUint16 aRefreshFileList);
	IMPORT_C void CallbackSimNotifyIccMessageWaitingIndicatorsChangeInd(TInt aError, const RMobilePhone::TMobilePhoneMessageWaitingV1& aIndicators);
	IMPORT_C void CallbackSimNotifyApnListChangeInd(TInt aError);
	IMPORT_C void CallbackSimNotifyApnControlListServiceStatusChangeInd(TInt aError, RMobilePhone::TAPNControlListServiceStatus aStatus);
	IMPORT_C void CallbackSimGetApnControlListServiceStatusComp(TInt aError, RMobilePhone::TAPNControlListServiceStatus aStatus);
	IMPORT_C void CallbackSimDeleteApnNameComp(TInt aError);
	IMPORT_C void CallbackSimEnumerateApnEntriesComp(TInt aError, TUint32 aNumEntries);
	IMPORT_C void CallbackSimChangeSecurityCodeComp(TInt aError);
	IMPORT_C void CallbackSimSetFdnSettingComp(TInt aError);
	IMPORT_C void CallbackSimGetCustomerServiceProfileComp(TInt aError, const RMobilePhone::TMobilePhoneCspFileV1& aCsp);
	IMPORT_C void CallbackSimGetSubscriberIdComp(TInt aError, const TDesC8& aId);
	IMPORT_C void CallbackSimAppendApnNameComp(TInt aError);
	IMPORT_C void CallbackSimGetActiveIccApplicationTypeComp(TInt aError, MLtsyDispatchSimGetActiveIccApplicationType::TIccType aIccType);
	IMPORT_C void CallbackSimSetIccMessageWaitingIndicatorsComp(TInt aError);
	IMPORT_C void CallbackSimSetApnControlListServiceStatusComp(TInt aError);
	IMPORT_C void CallbackSimGetApnNameComp(TInt aError, const RMobilePhone::TAPNEntryV3& aEntry);
	IMPORT_C void CallbackSimGetServiceTableComp(TInt aError, const RMobilePhone::TMobilePhoneServiceTableV1& aServiceTable);
	IMPORT_C void CallbackSimGetIccMessageWaitingIndicatorsComp(TInt aError, const RMobilePhone::TMobilePhoneMessageWaitingV1& aMessageWaiting);
	IMPORT_C void CallbackSimNotifySimCardStatusInd(TInt aError, RMmCustomAPI::TSIMCardStatus aSimCardStatus);
	IMPORT_C void CallbackSimSimLockActivateComp(TInt aError);
	IMPORT_C void CallbackSimSimLockDeActivateComp(TInt aError);
	IMPORT_C void CallbackSimGetAnswerToResetComp(TInt aError, const TDesC8& aAnswerToReset);
	IMPORT_C void CallbackSimGetSimCardReaderStatusComp(TInt aError, TUint8 aSimCardReaderStatus);
	IMPORT_C void CallbackSimGetSimAuthenticationDataComp(TInt aError, const TDesC8& aSignedResponse, const TDesC8& aCipheringKey);
	IMPORT_C void CallbackSimGetSimAuthenticationDataComp(TInt aError, const TDesC8& aResponse, const TDesC8& aCipheringKey, const TDesC8& aIntegrityKey, const TDesC8& aAUTS);
	IMPORT_C void CallbackSimPowerSimOffComp(TInt aError);
	IMPORT_C void CallbackSimPowerSimOnComp(TInt aError);
	IMPORT_C void CallbackSimReadSimFileComp(TInt aError, const TDesC8& aResponseBytes);
	IMPORT_C void CallbackSimSendApduRequestComp(TInt aError, TUint8 aServiceType, TUint8 aCardReaderNumber, TUint8 aApplicationType, const TDesC8& aResponseData);
	IMPORT_C void CallbackSimSendApduRequestV2Comp(TInt aError, const TDesC8& aResponseData);
	IMPORT_C void CallbackSimSimWarmResetComp(TInt aError);
	IMPORT_C void CallbackSimSetSimMessageStatusReadComp(TInt aError);

	// Sms related callbacks
	IMPORT_C void CallbackSmsNotifyReceiveSmsMessageInd(TInt aError, TBool aInd, const TSmsMsg& aSmsMessage);
	IMPORT_C void CallbackSmsSendSatSmsComp(TInt aError);
	IMPORT_C void CallbackSmsGetSmsStoreInfoComp(TInt aError, TInt aTotalEntries, TInt aUsedEntries);
	IMPORT_C void CallbackSmsGetSmspListComp(TInt aError, const TDesC& aServiceCenterAddress, const TDesC& aDestinationAddress,
				  const TDesC& aAlphaTagData, const DispatcherSim::TSmsParameters& aSmsParameters, TBool aMoreToCome);
	IMPORT_C void CallbackSmsNackSmsStoredComp(TInt aError);
	IMPORT_C void CallbackSmsAckSmsStoredComp(TInt aError);
	IMPORT_C void CallbackSmsResumeSmsReceptionComp(TInt aError);
	IMPORT_C void CallbackSmsSendSmsMessageComp(TInt aError, TInt aMsgRef, const TDesC8& aSmsSubmitReport);
	IMPORT_C void CallbackSmsSendSmsMessageNoFdnCheckComp(TInt aError, TInt aMsgRef, const TDesC8& aSmsSubmitReport);
	IMPORT_C void CallbackSmsSetMoSmsBearerComp(TInt aError);
	IMPORT_C void CallbackSmsStoreSmspListEntryComp(TInt aError);

	// CallControlMultiparty related callbacks
	IMPORT_C void CallbackCallControlMultipartyConferenceHangUpComp(TInt aError);
	IMPORT_C void CallbackCallControlMultipartyConferenceAddCallComp(TInt aError);
	IMPORT_C void CallbackCallControlMultipartyCreateConferenceComp(TInt aError);
	IMPORT_C void CallbackCallControlMultipartyConferenceSwapComp(TInt aError);
	IMPORT_C void CallbackCallControlMultipartyConferenceGoOneToOneComp(TInt aError, TInt aCallId);

	// SupplementaryServices related callbacks
	IMPORT_C void CallbackSupplementaryServicesNotifyCallBarringStatusChangeInd(TInt aError, RMobilePhone::TMobilePhoneCBCondition aCondition);
	IMPORT_C void CallbackSupplementaryServicesReceiveUssdMessageInd(TInt aError, const TDesC8& aMessage, TUint8 aDcs, RMobileUssdMessaging::TMobileUssdMessageType aType, RMobileUssdMessaging::TMobileUssdDataFormat aFormat);
	IMPORT_C void CallbackSupplementaryServicesNotifyCallWaitingStatusChangeInd(TInt aError, RMobilePhone::TMobileService aServiceGrouop, RMobilePhone::TMobilePhoneCWStatus aStatus);
	IMPORT_C void CallbackSupplementaryServicesNotifyCallForwardingStatusChangeInd(TInt aError, RMobilePhone::TMobilePhoneCFCondition aCallForwardingCondition);
	IMPORT_C void CallbackSupplementaryServicesNotifyCallForwardingActiveInd(TInt aError, RMobilePhone::TMobileService aServiceGroup, RMobilePhone::TMobilePhoneCFActive aActiveType);
	IMPORT_C void CallbackSupplementaryServicesGetDiagnosticOctetsInd(TInt aError, TInt aCallId, TUint8 aOctet);
	IMPORT_C void CallbackSupplementaryServicesNotifyAdditionalInfoInd(TInt aError, TUint8 aOperationCode, const TDesC8& aAdditionalInfo);
	IMPORT_C void CallbackSupplementaryServicesNotifyRequestCompleteInd(TInt aError, TInt aStatus);
	IMPORT_C void CallbackSupplementaryServicesNotifySendNetworkServiceRequestInd(TInt aError, RMobilePhone::TMobilePhoneNotifySendSSOperation aOperationCode, const TDesC& aAdditionalInfo);
	// NotifySS options
	IMPORT_C void CallbackSupplementaryServicesNotifyNetworkEventForwardModeInd(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsForwMode aForwardMode);
	IMPORT_C void CallbackSupplementaryServicesNotifyNetworkEventCallWaitingInd(TInt aError, RMmCustomAPI::TSsMode aMode, TBool aCallIsWaiting);
	IMPORT_C void CallbackSupplementaryServicesNotifyNetworkEventHoldModeInd(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsHoldMode aHoldMode);
	IMPORT_C void CallbackSupplementaryServicesNotifyNetworkEventConfrenceInd(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, TBool aConferenceIndicator);
	IMPORT_C void CallbackSupplementaryServicesNotifyNetworkEventCugInd(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, TUint16 aCugIndex);
	IMPORT_C void CallbackSupplementaryServicesNotifyNetworkEventClirSuppressionInd(TInt aError, RMmCustomAPI::TSsMode aMode, TBool aClirSuppressionRejected);
	IMPORT_C void CallbackSupplementaryServicesNotifyNetworkEventEctCallStateInd(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode, RMmCustomAPI::TSsEctState aEctCallState, RMmCustomAPI::TSsChoice aEctChoice, const TDesC& aRemotePartyNumber);
	IMPORT_C void CallbackSupplementaryServicesNotifyNetworkEventInd(TInt aError, RMmCustomAPI::TSsType aType, RMmCustomAPI::TSsMode aMode);
	
	IMPORT_C void CallbackSupplementaryServicesSendNetworkServiceRequestNoFdnCheckComp(TInt aError);
	IMPORT_C void CallbackSupplementaryServicesGetCallWaitingStatusComp(TInt aError, const CMobilePhoneCWList& aCallWaitingList);
	IMPORT_C void CallbackSupplementaryServicesSendNetworkServiceRequestComp(TInt aError);
	IMPORT_C void CallbackSupplementaryServicesGetCallBarringStatusComp(TInt aError, const CMobilePhoneCBList& aCallBarringList);
	IMPORT_C void CallbackSupplementaryServicesSetCallBarringStatusComp(TInt aError);
	IMPORT_C void CallbackSupplementaryServicesSetCallForwardingStatusComp(TInt aError);
	IMPORT_C void CallbackSupplementaryServicesSetCallWaitingStatusComp(TInt aError);
	IMPORT_C void CallbackSupplementaryServicesSetSsPasswordComp(TInt aError);
	IMPORT_C void CallbackSupplementaryServicesGetCallForwardingStatusComp(TInt aError, const CMobilePhoneCFList& aCallForwardingList);
	IMPORT_C void CallbackSupplementaryServicesSendUssdMessageComp(TInt aError);
	IMPORT_C void CallbackSupplementaryServicesSendUssdMessageNoFdnCheckComp(TInt aError);
	IMPORT_C void CallbackSupplementaryServicesSendUssdReleaseComp(TInt aError, TUint8 aOpCode, const TDesC& aAdditionalInfo);
	IMPORT_C void CallbackSupplementaryServicesClearBlacklistComp(TInt aError);

	// PacketServices related callbacks
	IMPORT_C void CallbackPacketServicesNotifyPacketAttachModeChangeInd(TInt aError, RPacketService::TPreferredBearer aBearer);
	IMPORT_C void CallbackPacketServicesNotifyQosProfileChangedGPRSInd(TInt aError, const TDesC& aContextName, const RPacketQoS::TQoSGPRSNegotiated&  aQoSGPRSNegotiated);
	IMPORT_C void CallbackPacketServicesNotifyQosProfileChangedR99Ind(TInt aError, const TDesC& aContextName, const RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated);
	IMPORT_C void CallbackPacketServicesNotifyQosProfileChangedR5Ind(TInt aError, const TDesC& aContextName, const RPacketQoS::TQoSR5Negotiated& aQoSR5Negotiated);
	IMPORT_C void CallbackPacketServicesNotifyRatTransferCapsChangeInd(TInt aError, TDynamicTransferCapsFlags aCaps);
	IMPORT_C void CallbackPacketServicesNotifyPacketMsClassChangeInd(TInt aError, RPacketService::TMSClass aMSClass);
	IMPORT_C void CallbackPacketServicesNotifyPacketDynamicCapsChangeInd(TInt aError, TBool aGprsIsSupportedOnCell, TBool aCsServicesIsSupportedOnCell);
	IMPORT_C void CallbackPacketServicesNotifyPdpContextAddedInd(TInt aError, const TDesC& aContextName);
	IMPORT_C void CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(TInt aError, const RPacketContext::TContextConfigGPRS& aContextConfigGPRS);
	IMPORT_C void CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(TInt aError, const RPacketContext::TContextConfigR99_R4& aContextConfigR99_R4);
	IMPORT_C void CallbackPacketServicesNotifyNetworkInitiatedContextActivationRequestInd(TInt aError, const RPacketContext::TContextConfig_R5& aContextConfig_R5);				
	IMPORT_C void CallbackPacketServicesNotifyPdpContextConfigChangedInd(TInt aError, const TDesC& aContextName, const TDesC8& aAccessPointName, const TDesC8& aPdpAddress, const TUint aPdpCompression, const TDesC8& aPcoBuffer);
	IMPORT_C void CallbackPacketServicesNotifyPdpContextStatusChangeInd(TInt aError, const TDesC& aContextName, const TContextMisc& aContextMisc);
	IMPORT_C void CallbackPacketServicesNotifyPdpContextConnectionSpeedChangeInd(TInt aError, const TDesC& aContextName, TUint aConnectionSpeed);
	IMPORT_C void CallbackPacketServicesNotifyPacketNetworkRegistrationStatusInd(TInt aError, RPacketService::TRegistrationStatus aRegistrationStatus);
	IMPORT_C void CallbackPacketServicesNotifyMbmsContextConfigChangedInd(TInt aError, const TDesC& aContextName, const RPacketMbmsContext::TContextConfigMbmsV1& aContextConfigMbms);
	IMPORT_C void CallbackPacketServicesNotifyMbmsNetworkServiceStatusChangeInd(TInt aError, TMbmsNetworkServiceStatus aMbmsNetworkServiceStatus);
	IMPORT_C void CallbackPacketServicesNotifyMbmsServiceAvailabilityChangeInd(TInt aError, const RArray<TUint>& aAvailableServiceIds);
	IMPORT_C void CallbackPacketServicesNotifyConnectionInfoChangeInd(TInt aError, const TDesC& aContextName, const RPacketContext::TConnectionInfoV1& aConnectionInfo);
	IMPORT_C void CallbackPacketServicesPacketAttachComp(TInt aError);
	IMPORT_C void CallbackPacketServicesGetPacketAttachModeComp(TInt aError, RPacketService::TAttachMode aAttachMode);
	IMPORT_C void CallbackPacketServicesGetPacketNetworkRegistrationStatusComp(TInt aError, RPacketService::TRegistrationStatus aRegistrationStatus);
	IMPORT_C void CallbackPacketServicesPacketDetachComp(TInt aError);
	IMPORT_C void CallbackPacketServicesSetPdpContextConfigComp(TInt aError, const TDesC& aContextName);
	IMPORT_C void CallbackPacketServicesModifyActivePdpContextComp(TInt aError, const TDesC& aContextName, TInt8 aRejectionCode);
	IMPORT_C void CallbackPacketServicesInitialisePdpContextComp(TInt aError, const TDesC& aContextName, const TDesC& aChannelId);
	IMPORT_C void CallbackPacketServicesDeletePdpContextComp(TInt aError, const TDesC& aContextName);
	IMPORT_C void CallbackPacketServicesSetPacketAttachModeComp(TInt aError);
	IMPORT_C void CallbackPacketServicesSetDefaultPdpContextParamsComp(TInt aError);
	IMPORT_C void CallbackPacketServicesActivatePdpContextComp(TInt aError, const TDesC& aContextName, const TDesC8& aAccessPointName, RPacketContext::TProtocolType aPdpType);
	IMPORT_C void CallbackPacketServicesSetPdpContextQosComp(TInt aError, const TDesC& aContextName);
	IMPORT_C void CallbackPacketServicesRejectNetworkInitiatedContextActivationRequestComp(TInt aError);
	IMPORT_C void CallbackPacketServicesDeactivatePdpContextComp(TInt aError, const TDesC& aContextName);
	IMPORT_C void CallbackPacketServicesGetStatusComp(TInt aError, RPacketService::TStatus aPacketStatus, TBool aIsResumed);
	IMPORT_C void CallbackPacketServicesGetStaticCapabilitiesComp(TInt aError, TUint aStaticCapabilities);
	IMPORT_C void CallbackPacketServicesGetMaxNoMonitoredServiceListsComp(TInt aError, TInt aMaxNoMonitoredServiceLists);
	IMPORT_C void CallbackPacketServicesGetMaxNoActiveServicesComp(TInt aError, TInt aMaxNoActiveServices);
	IMPORT_C void CallbackPacketServicesInitialiseMbmsContextComp(TInt aError, const TDesC& aContextName, const TDesC& aChannelId);
	IMPORT_C void CallbackPacketServicesGetMbmsNetworkServiceStatusComp(TInt aError, TMbmsNetworkServiceStatus aMbmsNetworkServiceStatus);
	IMPORT_C void CallbackPacketServicesUpdateMbmsMonitorServiceListComp(TInt aError);
	IMPORT_C void CallbackPacketServicesUpdateMbmsMonitorServiceListComp(TInt aError, const RArray<TUint>& aFailedServiceIds);
	IMPORT_C void CallbackPacketServicesUpdateMbmsSessionListComp(TInt aError, const TDesC& aContextName);
	
	// Sat related callbacks
	IMPORT_C void CallbackSatUssdControlSupportedChangeInd(TInt aError, TBool aSupported);
	IMPORT_C void CallbackSatPcmdInd(TInt aError, const TDesC8& aData);
	IMPORT_C void CallbackSatNotifyProactiveSimSessionEndInd(TInt aError, TUint8 aStatusWord1, TUint8 aStatusWord2);
	IMPORT_C void CallbackSatTimeZoneChangeInd(TInt aError, TUint8 aTimeZone);
	IMPORT_C void CallbackSatAccessTechnologyChangeInd(TInt aError, TUint8 aCurrentAccessTechnology);
	IMPORT_C void CallbackSatCallConnectedInd(TInt aError, TUint8 aTransactionId, TBool aNearEnd);
	IMPORT_C void CallbackSatSsChangeInd(TInt aError, DispatcherSat::TSsStatus aStatus);
	IMPORT_C void CallbackSatCallDisconnectedInd(TInt aError, TUint8 aTransactionId, TBool aNearEnd, const TDesC8& aCause);
	IMPORT_C void CallbackSatMtCallInd(TInt aError, TUint8 aTransactionId, const TDesC8& aAddress, const TDesC8& aSubAddress);
	IMPORT_C void CallbackSatCallControlEnvelopeResponseInd(TInt aError, const TDesC8& aResponseTlv);
	IMPORT_C void CallbackSatCreateCallControlEnvelopeInd(TInt aError, TUint8 aTag, const TDesC8& aBcc1, const TDesC8& aBcc2, const TDesC8& aAddress, TUint8 aTonNpi, TUint8 aDcs);
	IMPORT_C void CallbackSatCreateSmControlEnvelopeInd(TInt aError, const TDesC8& aRpDestinationAddress, const TDesC8& aTpDestinationAddress);
	IMPORT_C void CallbackSatImsiChangedInd(TInt aError);
	IMPORT_C void CallbackSatLocationStatusInd(TInt aError, TUint8 aLcnStatus, const TDesC8& aCountryCode,
			TUint16 aLcnAreaCode, TUint16 aCellId);
	IMPORT_C void CallbackSatRemoveEventListInd(TInt aError);
	IMPORT_C void CallbackSatCreateRefreshTerminalRspInd(TInt aError, RSat::TPCmdResult aGeneralResult, const TDesC& aAdditionalInfo);
	IMPORT_C void CallbackSatSmControlEnvelopeResponseInd(TInt aError, const TDesC8& aResponseTlv);
	IMPORT_C void CallbackSatCreateSmsPpDownloadEnvelopeInd(TInt aError, const TDesC8& aRpOriginatingAddress, const TDesC8& aSmsTpdu);
	IMPORT_C void CallbackSatCreateSmsDeliverReportInd(TInt aError);
	IMPORT_C void CallbackSatCreateCellBroadcastEnvelopeInd(TInt aError, const TDesC8& aCellBroadcastPage);
	IMPORT_C void CallbackSatGetImageInstanceComp(TInt aError, const TDesC8& aImageInstanceData, const TDesC8& aImageInstanceBody);
	IMPORT_C void CallbackSatGetIconDataComp(TInt aError, const TDesC8& aIconEfImgRecord);
	IMPORT_C void CallbackSatGetClutComp(TInt aError, const TDesC8& aClut);
	IMPORT_C void CallbackSatGetDefaultBearerCapabilityComp(TInt aError, const TDesC8& aCapability);
	IMPORT_C void CallbackSatGetSmsPpDownloadSupportedComp(TInt aError, TBool aSupported);
	IMPORT_C void CallbackSatGetSmsControlActivatedComp(TInt aError, TBool aActivated);
	IMPORT_C void CallbackSatGetAccessTechnologyComp(TInt aError, TUint8 aAccessTechnology);
	IMPORT_C void CallbackSatTimingAdvanceComp(TInt aError, TUint8 aTimingAdvance, TUint8 aMeStatus);
	IMPORT_C void CallbackSatProvideLocationInfoComp(TInt aError, TUint16 aLocationAreaCode, TUint16 aCellId,
			const TDesC8& aOperatorCode);
	IMPORT_C void CallbackSatSetPollingIntervalComp(TInt aError, TUint8 aPollingInterval);
	IMPORT_C void CallbackSatLocalInformationNmrComp(TInt aError, const TDesC8& aNmr, const TDesC& aBcchList);
	IMPORT_C void CallbackSatGetUssdControlSupportedComp(TInt aError, TBool aSupported);
	IMPORT_C void CallbackSatTerminalRspComp(TInt aError);
	IMPORT_C void CallbackSatRefreshAllowedComp(TInt aError);
	IMPORT_C void CallbackSatReadyComp(TInt aError);
	IMPORT_C void CallbackSatUssdControlEnvelopeErrorComp(TInt aError);
	
protected:


private:

	// So that the CMessageRouter can call SetDispatcherHolder (which we do not want
	// to export) on it to give it the dispatcher object references.
	friend class CMessageRouter;

	void SetDispatcherHolder(TDispatcherHolder& aDispatcherHolder);

	CCtsyDispatcherCallback();
	void ConstructL();

private:

	// Not Owned

	/**
	 * Holds pointers to dispatcher objects.  Completions sent by the Licensee LTSY
	 * are forwarded by this class to the relevant dispatcher stored in the
	 * dispatcher holder.
	 */
	TDispatcherHolder* iDispatcherHolder;

	}; // class CCtsyDispatcherCallback

#endif // CCTSYDISPATCHERCALLBACK_H
