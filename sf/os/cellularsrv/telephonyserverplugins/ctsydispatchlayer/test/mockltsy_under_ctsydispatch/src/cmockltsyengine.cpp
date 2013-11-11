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

#include <ctsy/ltsy/mltsydispatchcallcontrolinterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include <ctsy/ltsy/mltsydispatchphonebookeninterface.h>
#include <ctsy/ltsy/mltsydispatchcellbroadcastinterface.h>
#include <ctsy/ltsy/mltsydispatchphonebookoninterface.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchsmsinterface.h>
#include <ctsy/ltsy/mltsydispatchcallcontrolmultipartyinterface.h>
#include <ctsy/ltsy/mltsydispatchsupplementaryservicesinterface.h>
#include <ctsy/ltsy/mltsydispatchpacketservicesinterface.h>
#include <ctsy/ltsy/mltsydispatchsatinterface.h>

#include "cmockltsyengine.h"
#include "cmocksyserver.h"
#include <test/rmockltsy.h>
#include <test/tmockltsydata.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>

#include "cmockcallcontrolmesshandler.h"
#include "cmockphonemesshandler.h"
#include "cmocksecuritymesshandler.h"
#include "cmockphonebookenmesshandler.h"
#include "cmockcellbroadcastmesshandler.h"
#include "cmockphonebookonmesshandler.h"
#include "cmockphonebookmesshandler.h"
#include "cmocksimmesshandler.h"
#include "cmocksmsmesshandler.h"
#include "cmockcallcontrolmultipartymesshandler.h"
#include "cmocksupplementaryservicesmesshandler.h"
#include "cmockpacketservicesmesshandler.h"
#include "cmocksatmesshandler.h"
#include "mockltsyindicatorids.h"

_LIT(KCallControlNotifyCallStatusChangeApiName, "CallControl NotifyCallStatusChange");
_LIT(KCallControlReceiveUuiApiName, "CallControl ReceiveUui");
_LIT(KCallControlNotifyDataCallCapsChangeApiName, "CallControl NotifyDataCallCapsChange");
_LIT(KCallControlNotifyIncomingCallApiName, "CallControl NotifyIncomingCall");
_LIT(KCallControlNotifyHscsdInfoChangeApiName, "CallControl NotifyHscsdInfoChange");
_LIT(KCallControlNotifyCallEventApiName, "CallControl NotifyCallEvent");
_LIT(KCallControlNotifyCallInfoChangeApiName, "CallControl NotifyCallInfoChange");
_LIT(KCallControlNotifyIccCallForwardingStatusChangeApiName, "CallControl NotifyIccCallForwardingStatusChange");
_LIT(KCallControlRemoteAlertingToneChangeApiName, "CallControl RemoteAlertingToneChange");
_LIT(KCallControlAnswerApiName, "CallControl Answer");
_LIT(KCallControlHoldApiName, "CallControl Hold");
_LIT(KCallControlDialEmergencyApiName, "CallControl DialEmergency");
_LIT(KCallControlStopDtmfToneApiName, "CallControl StopDtmfTone");
_LIT(KCallControlSetActiveAlsLineApiName, "CallControl SetActiveAlsLine");
_LIT(KCallControlSendDtmfTonesCancelApiName, "CallControl SendDtmfTonesCancel");
_LIT(KCallControlHangUpApiName, "CallControl HangUp");
_LIT(KCallControlResumeApiName, "CallControl Resume");
_LIT(KCallControlSetDynamicHscsdParamsApiName, "CallControl SetDynamicHscsdParams");
_LIT(KCallControlDialVoiceApiName, "CallControl DialVoice");
_LIT(KCallControlTransferApiName, "CallControl Transfer");
_LIT(KCallControlSendDtmfTonesApiName, "CallControl SendDtmfTones");
_LIT(KCallControlGetIdentityServiceStatusApiName, "CallControl GetIdentityServiceStatus");
_LIT(KCallControlSwapApiName, "CallControl Swap");
_LIT(KCallControlSingleSwapApiName, "CallControl Single Swap");
_LIT(KCallControlLoanDataPortApiName, "CallControl LoanDataPort");
_LIT(KCallControlRecoverDataPortApiName, "CallControl RecoverDataPort");
_LIT(KCallControlStartDtmfToneApiName, "CallControl StartDtmfTone");
_LIT(KCallControlGetActiveAlsLineApiName, "CallControl GetActiveAlsLine");
_LIT(KCallControlDialDataApiName, "CallControl DialData");
_LIT(KCallControlQueryIsEmergencyNumberApiName, "CallControl QueryIsEmergencyNumber");
_LIT(KCallControlGetAlsPpSupportApiName, "CallControl GetAlsPpSupport");
_LIT(KCallControlGetAlsBlockedStatusApiName, "CallControl GetAlsBlockedStatus");
_LIT(KCallControlSetAlsBlockedApiName, "CallControl SetAlsBlocked");
_LIT(KCallControlGetLifeTimeApiName, "CallControl GetLifeTime");
_LIT(KCallControlTerminateErrorCallApiName, "CallControl TerminateErrorCall");
_LIT(KCallControlTerminateAllCallsApiName, "CallControl TerminateAllCalls");
_LIT(KCallControlGetCallForwardingIndicatorApiName, "CallControl GetCallForwardingIndicator");
_LIT(KCallControlUpdateLifeTimerApiName, "CallControl UpdateLifeTimer");
_LIT(KCallControlMultipartyConferenceHangUpApiName, "CallControlMultiparty ConferenceHangUp");
_LIT(KCallControlMultipartyConferenceAddCallApiName, "CallControlMultiparty ConferenceAddCall");
_LIT(KCallControlMultipartyCreateConferenceApiName, "CallControlMultiparty CreateConference");
_LIT(KCallControlMultipartyConferenceSwapApiName, "CallControlMultiparty ConferenceSwap");
_LIT(KCallControlMultipartyConferenceGoOneToOneApiName, "CallControlMultiparty ConferenceGoOneToOne");
_LIT(KCellBroadcastGsmBroadcastNotifyMessageReceivedApiName, "CellBroadcast GsmBroadcastNotifyMessageReceived");
_LIT(KCellBroadcastWcdmaBroadcastMessageReceivedApiName, "CellBroadcast WcdmaBroadcastMessageReceived");
_LIT(KCellBroadcastSetBroadcastFilterSettingApiName, "CellBroadcast SetBroadcastFilterSetting");
_LIT(KCellBroadcastActivateBroadcastReceiveMessageApiName, "CellBroadcast ActivateBroadcastReceiveMessage");
_LIT(KCellBroadcastReceiveMessageCancelApiName, "CellBroadcast ReceiveMessageCancel");
_LIT(KCellBroadcastStartSimCbTopicBrowsingApiName, "CellBroadcast StartSimCbTopicBrowsing");
_LIT(KCellBroadcastDeleteSimCbTopicApiName, "CellBroadcast DeleteSimCbTopic");
_LIT(KPacketServicesNotifyPacketAttachModeChangeApiName, "PacketServices NotifyPacketAttachModeChange");
_LIT(KPacketServicesNotifyQosProfileChangedApiName, "PacketServices NotifyQosProfileChanged");
_LIT(KPacketServicesNotifyRatTransferCapsChangeApiName, "PacketServices NotifyRatTransferCapsChange");
_LIT(KPacketServicesNotifyPacketMsClassChangeApiName, "PacketServices NotifyPacketMsClassChange");
_LIT(KPacketServicesNotifyPacketDynamicCapsChangeApiName, "PacketServices NotifyPacketDynamicCapsChange");
_LIT(KPacketServicesNotifyPdpContextAddedApiName, "PacketServices NotifyPdpContextAdded");
_LIT(KPacketServicesNotifyNetworkInitiatedContextActivationRequestApiName, "PacketServices NotifyNetworkInitiatedContextActivationRequest");
_LIT(KPacketServicesNotifyPdpContextConfigChangedApiName, "PacketServices NotifyPdpContextConfigChanged");
_LIT(KPacketServicesNotifyPdpContextStatusChangeApiName, "PacketServices NotifyPdpContextStatusChange");
_LIT(KPacketServicesNotifyPacketNetworkRegistrationStatusApiName, "PacketServices NotifyPacketNetworkRegistrationStatus");
_LIT(KPacketServicesNotifyPdpContextConnectionSpeedChangeApiName, "PacketServices NotifyPdpContextConnectionSpeedChange");
_LIT(KPacketServicesNotifyMbmsContextConfigChangedApiName, "PacketServices NotifyMbmsContextConfigChanged");
_LIT(KPacketServicesNotifyMbmsNetworkServiceStatusChangeApiName, "PacketServices NotifyMbmsNetworkServiceStatusChange");
_LIT(KPacketServicesNotifyMbmsServiceAvailabilityChangeApiName, "PacketServices NotifyMbmsServiceAvailabilityChange");
_LIT(KPacketServicesNotifyConnectionInfoChange, "PacketServices NotifyConnectionInfoChange");
_LIT(KPacketServicesPacketAttachApiName, "PacketServices PacketAttach");
_LIT(KPacketServicesGetPacketAttachModeApiName, "PacketServices GetPacketAttachMode");
_LIT(KPacketServicesGetPacketNetworkRegistrationStatusApiName, "PacketServices GetPacketNetworkRegistrationStatus");
_LIT(KPacketServicesPacketDetachApiName, "PacketServices PacketDetach");
_LIT(KPacketServicesSetPdpContextConfigApiName, "PacketServices SetPdpContextConfig");
_LIT(KPacketServicesModifyActivePdpContextApiName, "PacketServices ModifyActivePdpContext");
_LIT(KPacketServicesInitialisePdpContextApiName, "PacketServices InitialisePdpContext");
_LIT(KPacketServicesDeletePdpContextApiName, "PacketServices DeletePdpContext");
_LIT(KPacketServicesSetPacketAttachModeApiName, "PacketServices SetPacketAttachMode");
_LIT(KPacketServicesNotifyPacketStatusChangeApiName, "PacketServices NotifyPacketStatusChange");
_LIT(KPacketServicesSetDefaultPdpContextGprsParamsApiName, "PacketServices SetDefaultPdpContextGprsParams");
_LIT(KPacketServicesSetDefaultPdpContextR99R4ParamsApiName, "PacketServices SetDefaultPdpContextR99R4Params");
_LIT(KPacketServicesActivatePdpContextApiName, "PacketServices ActivatePdpContext");
_LIT(KPacketServicesSetPdpContextQosGprsApiName, "PacketServices SetPdpContextQosGprs");
_LIT(KPacketServicesSetPdpContextQosR99R4ApiName, "PacketServices SetPdpContextQosR99R4");
_LIT(KPacketServicesSetPdpContextQosR5ApiName, "PacketServices SetPdpContextQosR5");
_LIT(KPacketServicesRejectNetworkInitiatedContextActivationRequestApiName, "PacketServices RejectNetworkInitiatedContextActivationRequest");
_LIT(KPacketServicesDeactivatePdpContextApiName, "PacketServices DeactivatePdpContext");
_LIT(KPacketServicesAddPacketFilterApiName, "PacketServices AddPacketFilter");
_LIT(KPacketServicesGetStatusApiName, "PacketServices GetStatus");
_LIT(KPacketServicesGetStaticCapabilitiesApiName, "PacketServices GetStaticCapabilities");
_LIT(KPacketServicesGetMaxNoMonitoredServiceListsApiName, "PacketServices GetMaxNoMonitoredServiceLists");
_LIT(KPacketServicesGetMaxNoActiveServicesApiName, "PacketServices GetMaxNoActiveServices");
_LIT(KPacketServicesInitialiseMbmsContextApiName, "PacketServices InitialiseMbmsContext");
_LIT(KPacketServicesGetMbmsNetworkServiceStatusApiName, "PacketServices GetMbmsNetworkServiceStatus");
_LIT(KPacketServicesUpdateMbmsMonitorServiceListApiName, "PacketServices UpdateMbmsMonitorServiceList");
_LIT(KPacketServicesUpdateMbmsSessionListApiName, "PacketServices UpdateMbmsSessionList");
_LIT(KPacketServicesRemovePacketFilterApiName, "PacketServices RemovePacketFilter");
_LIT(KPhoneNotifyNetworkModeChangeApiName, "Phone NotifyNetworkModeChange");
_LIT(KPhoneNotifyEgprsInfoChangeApiName, "Phone NotifyEgprsInfoChange");
_LIT(KPhoneNotifySignalStrengthChangeApiName, "Phone NotifySignalStrengthChange");
_LIT(KPhoneNotifyNetworkRegistrationStatusChangeApiName, "Phone NotifyNetworkRegistrationStatusChange");
_LIT(KPhoneNotifyNitzInfoChangeApiName, "Phone NotifyNitzInfoChange");
_LIT(KPhoneNotifyBatteryInfoChangeApiName, "Phone NotifyBatteryInfoChange");
_LIT(KPhoneNotifyCurrentNetworkChangeApiName, "Phone NotifyCurrentNetworkChange");
_LIT(KPhoneNotifyRfStatusChangeApiName, "Phone NotifyRfStatusChange");
_LIT(KPhoneNotifyNspsStatusChangeApiName, "Phone NotifyNspsStatusChange");
_LIT(KPhoneNotifyNetworkSelectionSettingChangeApiName, "Phone NotifyNetworkSelectionSettingChange");
_LIT(KPhoneNotifyRauEventApiName, "Phone NotifyRauEvent");
_LIT(KPhoneNotifyNetworkConnectionFailureApiName, "Phone NotifyNetworkConnectionFailure");
_LIT(KCustomPhoneNotifyDtmfEventApiName, "Custom Phone NotifyDtmfEvent");
_LIT(KPhoneNotifyDtmfEventApiName, "Phone NotifyDtmfEvent");
_LIT(KPhoneNotifyHspaStatusChangedApiName, "Phone NotifyHspaStatusChanged");
_LIT(KPhoneNotifyCellInfoChangedApiName, "Phone NotifyCellInfoChanged");
_LIT(KPhoneNotifyCellInfoChangeApiName, "Phone NotifyCellInfoChange");
_LIT(KPhoneBootNotifyModemStatusReadyApiName, "Phone BootNotifyModemStatusReady");
_LIT(KPhoneBootNotifySimStatusReadyApiName, "Phone BootNotifySimStatusReady");
_LIT(KPhoneGetFdnStatusApiName, "Phone GetFdnStatus");
_LIT(KPhoneGetHomeNetworkApiName, "Phone GetHomeNetwork");
_LIT(KPhoneGetNetworkRegistrationStatusApiName, "Phone GetNetworkRegistrationStatus");
_LIT(KPhoneSimRefreshRegisterApiName, "Phone SimRefreshRegister");
_LIT(KPhoneGetServiceProviderNameApiName, "Phone GetServiceProviderName");
_LIT(KPhoneGetPhoneIdApiName, "Phone GetPhoneId");
_LIT(KPhoneGetDetectedNetworksApiName, "Phone GetDetectedNetworks");
_LIT(KPhoneGetDetectedNetworksCancelApiName, "Phone GetDetectedNetworksCancel");
_LIT(KPhoneSelectNetworkApiName, "Phone SelectNetwork");
_LIT(KPhoneSelectNetworkCancelApiName, "Phone SelectNetworkCancel");
_LIT(KPhoneSetNetworkSelectionSettingApiName, "Phone SetNetworkSelectionSetting");
_LIT(KPhoneGetCurrentNetworkInfoApiName, "Phone GetCurrentNetworkInfo");
_LIT(KPhoneGetNetworkModeApiName, "Phone GetNetworkMode");
_LIT(KPhoneGetNitzInfoApiName, "Phone GetNitzInfo");
_LIT(KPhoneGetSignalStrengthApiName, "Phone GetSignalStrength");
_LIT(KPhoneGetBatteryInfoApiName, "Phone GetBatteryInfo");
_LIT(KPhoneNspsWakeupApiName, "Phone NspsWakeup");
_LIT(KPhoneSetSystemNetworkModeApiName, "Phone SetSystemNetworkMode");
_LIT(KPhoneGetCurrentSystemNetworkModesApiName, "Phone GetCurrentSystemNetworkModes");
_LIT(KPhoneResetNetServerApiName, "Phone ResetNetServer");
_LIT(KPhoneSetAlwaysOnModeApiName, "Phone SetAlwaysOnMode");
_LIT(KPhoneSetDriveModeApiName, "Phone SetDriveMode");
_LIT(KPhoneGetHspaStatusApiName, "Phone GetHspaStatus");
_LIT(KPhoneSetHspaStatusApiName, "Phone SetHspaStatus");
_LIT(KPhoneGetNetworkProviderNameApiName, "Phone GetNetworkProviderName");
_LIT(KPhoneGetOperatorNameApiName, "Phone GetOperatorName");
_LIT(KPhoneGetCellInfoApiName, "Phone GetCellInfo");
_LIT(KPhoneRegisterCellInfoChangeNotificationApiName, "Phone RegisterCellInfoChangeNotification");
_LIT(KPhoneGetPhoneCellInfoApiName, "Phone GetPhoneCellInfo");
_LIT(KPhoneGetUsimServiceSupportApiName, "Phone GetUsimServiceSupport");
_LIT(KPhoneGetCurrentActiveUsimApplicationApiName, "Phone GetCurrentActiveUsimApplication");
_LIT(KPhoneTerminateAllCallsApiName, "Phone TerminateAllCalls");
_LIT(KPhoneGetSystemNetworkBandApiName, "Phone GetSystemNetworkBand");
_LIT(KPhoneSetSystemNetworkBandApiName, "Phone SetSystemNetworkBand");
_LIT(KPhonebookStoreResetCacheApiName, "Phonebook StoreResetCache");
_LIT(KPhonebookStoreSetFdnPhonebookInfoApiName, "Phonebook StoreSetFdnPhonebookInfo");
_LIT(KPhonebookStoreReadEntryApiName, "Phonebook StoreReadEntry");
_LIT(KPhonebookStoreDeleteEntryApiName, "Phonebook StoreDeleteEntry");
_LIT(KPhonebookStoreCacheApiName, "Phonebook StoreCache");
_LIT(KPhonebookStoreCacheCancelApiName, "Phonebook StoreCacheCancel");
_LIT(KPhonebookStoreGetPhonebookInfoApiName, "Phonebook StoreGetPhonebookInfo");
_LIT(KPhonebookStoreInitialiseApiName, "Phonebook StoreInitialise");
_LIT(KPhonebookStoreDeleteAllApiName, "Phonebook StoreDeleteAll");
_LIT(KPhonebookStoreWriteEntryApiName, "Phonebook StoreWriteEntry");
_LIT(KPhonebookGetPhoneStoreInfoApiName, "Phonebook GetPhoneStoreInfo");
_LIT(KPhonebookSmsStoreReadAllApiName, "Phonebook SmsStoreReadAll");
_LIT(KPhonebookSmsStoreDeleteEntryApiName, "Phonebook SmsStoreDeleteEntry");
_LIT(KPhonebookSmsStoreDeleteAllApiName, "Phonebook SmsStoreDeleteAll");
_LIT(KPhonebookSmsStoreGetInfoApiName, "Phonebook SmsStoreGetInfo");
_LIT(KPhonebookSmsStoreReadEntryApiName, "Phonebook SmsStoreReadEntry");
_LIT(KPhonebookSmsStoreWriteEntryApiName, "Phonebook SmsStoreWriteEntry");
_LIT(KPhonebookEnStoreGetInfoApiName, "PhonebookEn StoreGetInfo");
_LIT(KPhonebookEnStoreReadAllApiName, "PhonebookEn StoreReadAll");
_LIT(KPhonebookEnStoreReadEntryApiName, "PhonebookEn StoreReadEntry");
_LIT(KPhonebookOnStoreReadApiName, "PhonebookOn StoreRead");
_LIT(KPhonebookOnStoreDeleteAllApiName, "PhonebookOn StoreDeleteAll");
_LIT(KPhonebookOnStoreReadEntryApiName, "PhonebookOn StoreReadEntry");
_LIT(KPhonebookOnStoreGetInfoApiName, "PhonebookOn StoreGetInfo");
_LIT(KPhonebookOnStoreGetReadStoreSizeApiName, "PhonebookOn StoreGetReadStoreSize");
_LIT(KPhonebookOnStoreDeleteEntryApiName, "PhonebookOn StoreDeleteEntry");
_LIT(KPhonebookOnStoreWriteEntryApiName, "PhonebookOn StoreWriteEntry");
_LIT(KPhonebookOnStoreWriteApiName, "PhonebookOn StoreWrite");
_LIT(KPhonebookOnStoreGetStoreSizeApiName, "PhonebookOn StoreGetStoreSize");
_LIT(KSatUssdControlSupportedChangeApiName, "Sat UssdControlSupportedChange");
_LIT(KSatPcmdApiName, "Sat Pcmd");
_LIT(KSatNotifyProactiveSimSessionEndApiName, "Sat NotifyProactiveSimSessionEnd");
_LIT(KSatNotifyLocalInfoApiName, "Sat NotifyLocalInfo");
_LIT(KSatTimeZoneChangeApiName, "Sat TimeZoneChange");
_LIT(KSatAccessTechnologyChangeApiName, "Sat AccessTechnologyChange");
_LIT(KSatCallConnectedApiName, "Sat CallConnected");
_LIT(KSatSsChangeApiName, "Sat SsChange");
_LIT(KSatCallDisconnectedApiName, "Sat CallDisconnected");
_LIT(KSatMtCallApiName, "Sat MtCall");
_LIT(KKMockLtsyDispatchSatCallControlEnvelopeResponseApiName, "Sat CallControlEnvelopeResponse");
_LIT(KSatCreateCallControlEnvelopeApiName, "Sat CreateCallControlEnvelope");
_LIT(KSatCreateMoSmControlEnvelopeApiName, "Sat CreateMoSmControlEnvelope");
_LIT(KSatImsiChangedApiName, "Sat ImsiChanged");
_LIT(KSatLocationStatusApiName, "Sat LocationStatus");
_LIT(KSatRemoveEventListApiName, "Sat RemoveEventList");
_LIT(KSatCreateRefreshTerminalRspApiName, "Sat CreateRefreshTerminalRsp");
_LIT(KSatMoSmControlEnvelopeResponseApiName, "Sat MoSmControlEnvelopeResponse");
_LIT(KSatCreateSmsPpDownloadEnvelopeApiName, "Sat CreateSmsPpDownloadEnvelope");
_LIT(KSatCreateSmsDeliveryReportApiName, "Sat CreateSmsDeliveryReport");
_LIT(KSatCreateCellBroadcastEnvelopeApiName, "Sat CreateCellBroadcastEnvelope");
_LIT(KSatSmsDeliverReportApiName, "Sat SmsDeliverReport");
_LIT(KSatGetImageInstanceApiName, "Sat GetImageInstance");
_LIT(KSatGetIconDataApiName, "Sat GetIconData");
_LIT(KSatGetClutApiName, "Sat GetClut");
_LIT(KSatGetDefaultBearerCapabilityApiName, "Sat GetDefaultBearerCapability");
_LIT(KSatGetSmsPpDownloadSupportedApiName, "Sat GetSmsPpDownloadSupported");
_LIT(KSatGetSmsControlActivatedApiName, "Sat GetSmsControlActivated");
_LIT(KSatGetAccessTechnologyApiName, "Sat GetAccessTechnology");
_LIT(KSatCellBroadcastEnvelopeApiName, "Sat CellBroadcastEnvelope");
_LIT(KSatTimingAdvanceApiName, "Sat TimingAdvance");
_LIT(KSatSmControlResponseDataApiName, "Sat SmControlResponseData");
_LIT(KSatProvideLocationInfoApiName, "Sat ProvideLocationInfo");
_LIT(KSatSetPollingIntervalApiName, "Sat SetPollingInterval");
_LIT(KSatSmsPpDownloadEnvelopeApiName, "Sat SmsPpDownloadEnvelope");
_LIT(KSatLocalInformationNmrApiName, "Sat LocalInformationNmr");
_LIT(KSatCallAndSmsControlEnvelopeApiName, "Sat CallAndSmsControlEnvelope");
_LIT(KSatRefreshAllowedApiName, "Sat RefreshAllowed");
_LIT(KSatReadyApiName, "Sat Ready");
_LIT(KSatPCmdNotificationApiName, "Sat PCmdNotification");
_LIT(KSatUssdControlEnvelopeErrorApiName, "Sat UssdControlEnvelopeError");
_LIT(KSatTimerExpirationEnvelopeApiName, "Sat TimerExpirationEnvelope");
_LIT(KSatTerminalRspApiName, "Sat TerminalRsp");
_LIT(KSatMenuSelectionEnvelopeApiName, "Sat MenuSelectionEnvelope");
_LIT(KSatEventDownloadEnvelopeApiName, "Sat EventDownloadEnvelope");
_LIT(KSatGetUssdControlSupportedApiName, "Sat GetUssdControlSupported");
_LIT(KSecurityBootNotifySecurityReadyApiName, "Security BootNotifySecurityReady");
_LIT(KSecurityNotifySecurityEventApiName, "Security NotifySecurityEvent");
_LIT(KSecurityNotifyLockInfoChangeApiName, "Security NotifyLockInfoChange");
_LIT(KSecurityGetSecurityCodeInfoApiName, "Security GetSecurityCodeInfo");
_LIT(KSecurityGetLockInfoApiName, "Security GetLockInfo");
_LIT(KSecurityAbortSecurityCodeApiName, "Security AbortSecurityCode");
_LIT(KSecurityGetCurrentActivePinApiName, "Security GetCurrentActivePin");
_LIT(KSecuritySetLockSettingApiName, "Security SetLockSetting");
_LIT(KSecurityVerifySecurityCodeApiName, "Security VerifySecurityCode");
_LIT(KSecurityGetPin1DisableSupportedApiName, "Security GetPin1DisableSupported");
_LIT(KSecurityCheckSecurityCodeApiName, "Security CheckSecurityCode");
_LIT(KSecurityDisablePhoneLockApiName, "Security DisablePhoneLock");
_LIT(KSecurityGetCipheringInfoApiName, "Security GetCipheringInfo");
_LIT(KSecurityIsSecurityCodeBlockedApiName, "Security IsSecurityCodeBlocked");
_LIT(KSecurityCheckSecurityCodeCancelApiName, "Security CheckSecurityCodeCancel");
_LIT(KSimRefreshSimFilesApiName, "Sim RefreshSimFiles");
_LIT(KSimNotifyIccMessageWaitingIndicatorsChangeApiName, "Sim NotifyIccMessageWaitingIndicatorsChange");
_LIT(KSimNotifyApnListChangeApiName, "Sim NotifyApnListChange");
_LIT(KSimNotifyApnControlListServiceStatusChangeApiName, "Sim NotifyApnControlListServiceStatusChange");
_LIT(KSimGetApnControlListServiceStatusApiName, "Sim GetApnControlListServiceStatus");
_LIT(KSimDeleteApnNameApiName, "Sim DeleteApnName");
_LIT(KSimEnumerateApnEntriesApiName, "Sim EnumerateApnEntries");
_LIT(KSimChangeSecurityCodeApiName, "Sim ChangeSecurityCode");
_LIT(KSimSetFdnSettingApiName, "Sim SetFdnSetting");
_LIT(KSimGetCustomerServiceProfileApiName, "Sim GetCustomerServiceProfile");
_LIT(KSimGetSubscriberIdApiName, "Sim GetSubscriberId");
_LIT(KSimAppendApnNameApiName, "Sim AppendApnName");
_LIT(KSimGetActiveIccApplicationTypeApiName, "Sim GetActiveIccApplicationType");
_LIT(KSimSetIccMessageWaitingIndicatorsApiName, "Sim SetIccMessageWaitingIndicators");
_LIT(KSimSetApnControlListServiceStatusApiName, "Sim SetApnControlListServiceStatus");
_LIT(KSimGetApnNameApiName, "Sim GetApnName");
_LIT(KSimSimRefreshDoneApiName, "Sim SimRefreshDone");
_LIT(KSimGetServiceTableApiName, "Sim GetServiceTable");
_LIT(KSimGetIccMessageWaitingIndicatorsApiName, "Sim GetIccMessageWaitingIndicators");
_LIT(KSimSimLockActivateApiName, "Sim SimLockActivate");
_LIT(KSimSimLockDeActivateApiName, "Sim SimLockDeActivate");
_LIT(KSimGetAnswerToResetApiName, "Sim GetAnswerToReset");
_LIT(KSimGetSimCardReaderStatusApiName, "Sim GetSimCardReaderStatus");
_LIT(KSimGetSimAuthenticationEapSimDataApiName, "Sim GetSimAuthenticationEapSimData");
_LIT(KSimGetSimAuthenticationEapAkaDataApiName, "Sim GetSimAuthenticationEapAkaData");
_LIT(KSimPowerSimOffApiName, "Sim PowerSimOff");
_LIT(KSimPowerSimOnApiName, "Sim PowerSimOn");
_LIT(KSimReadSimFileApiName, "Sim ReadSimFile");
_LIT(KSimSendApduRequestApiName, "Sim SendApduRequest");
_LIT(KSimSendApduRequestV2ApiName, "Sim SendApduRequestV2");
_LIT(KSimSimWarmResetApiName, "Sim SimWarmReset");
_LIT(KSimSetSimMessageStatusReadApiName, "Sim SetSimMessageStatusRead");
_LIT(KSmsNotifyReceiveSmsMessageApiName, "Sms NotifyReceiveSmsMessage");
_LIT(KSmsSendSatSmsApiName, "Sms SendSatSms");
_LIT(KSmsGetSmsStoreInfoApiName, "Sms GetSmsStoreInfo");
_LIT(KSmsGetSmspListApiName, "Sms GetSmspList");
_LIT(KSmsNackSmsStoredApiName, "Sms NackSmsStored");
_LIT(KSmsAckSmsStoredApiName, "Sms AckSmsStored");
_LIT(KSmsResumeSmsReceptionApiName, "Sms ResumeSmsReception");
_LIT(KSmsSendSmsMessageApiName, "Sms SendSmsMessage");
_LIT(KSmsSendSmsMessageNoFdnCheckApiName, "Sms SendSmsMessageNoFdnCheck");
_LIT(KSmsSetMoSmsBearerApiName, "Sms SetMoSmsBearer");
_LIT(KSmsStoreSmspListEntryApiName, "Sms StoreSmspListEntry");
_LIT(KSupplementaryServicesNotifyCallBarringStatusChangeApiName, "SupplementaryServices NotifyCallBarringStatusChange");
_LIT(KSupplementaryServicesReceiveUssdMessageApiName, "SupplementaryServices ReceiveUssdMessage");
_LIT(KSupplementaryServicesNotifyCallWaitingStatusChangeApiName, "SupplementaryServices NotifyCallWaitingStatusChange");
_LIT(KSupplementaryServicesNotifyCallForwardingStatusChangeApiName, "SupplementaryServices NotifyCallForwardingStatusChange");
_LIT(KSupplementaryServicesNotifyCallForwardingActiveApiName, "SupplementaryServices NotifyCallForwardingActive");
_LIT(KSupplementaryServicesGetDiagnosticOctetsApiName, "SupplementaryServices GetDiagnosticOctets");
_LIT(KSupplementaryServicesNotifyAdditionalInfoApiName, "SupplementaryServices NotifyAdditionalInfo");
_LIT(KSupplementaryServicesNotifyRequestCompleteApiName, "SupplementaryServices NotifyRequestComplete");
_LIT(KSupplementaryServicesNotifyNetworkEventForwardModeApiName, "SupplementaryServices NotifyNetworkEventForwardMode");
_LIT(KSupplementaryServicesNotifyNetworkEventCallWaitingApiName, "SupplementaryServices NotifyNetworkEventCallWaiting");
_LIT(KSupplementaryServicesNotifyNetworkEventHoldModeApiName, "SupplementaryServices NotifyNetworkEventHoldMode");
_LIT(KSupplementaryServicesNotifyNetworkEventConfrenceApiName, "SupplementaryServices NotifyNetworkEventConfrence");
_LIT(KSupplementaryServicesNotifyNetworkEventCugApiName, "SupplementaryServices NotifyNetworkEventCug");
_LIT(KSupplementaryServicesNotifyNetworkEventClirSuppressionApiName, "SupplementaryServices NotifyNetworkEventClirSuppression");
_LIT(KSupplementaryServicesNotifyNetworkEventEctCallStateApiName, "SupplementaryServices NotifyNetworkEventEctCallState");
_LIT(KSupplementaryServicesNotifyNetworkEventApiName, "SupplementaryServices NotifyNetworkEvent");
_LIT(KSupplementaryServicesNotifySendNetworkServiceRequestApiName, "SupplementaryServices NotifySendNetworkServiceRequest");
_LIT(KSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiName, "SupplementaryServices SendNetworkServiceRequestNoFdnCheck");
_LIT(KSupplementaryServicesGetCallWaitingStatusApiName, "SupplementaryServices GetCallWaitingStatus");
_LIT(KSupplementaryServicesSendNetworkServiceRequestApiName, "SupplementaryServices SendNetworkServiceRequest");
_LIT(KSupplementaryServicesGetCallBarringStatusApiName, "SupplementaryServices GetCallBarringStatus");
_LIT(KSupplementaryServicesSetCallBarringStatusApiName, "SupplementaryServices SetCallBarringStatus");
_LIT(KSupplementaryServicesSetCallForwardingStatusApiName, "SupplementaryServices SetCallForwardingStatus");
_LIT(KSupplementaryServicesSetCallWaitingStatusApiName, "SupplementaryServices SetCallWaitingStatus");
_LIT(KSupplementaryServicesSetSsPasswordApiName, "SupplementaryServices SetSsPassword");
_LIT(KSupplementaryServicesGetCallForwardingStatusApiName, "SupplementaryServices GetCallForwardingStatus");
_LIT(KSupplementaryServicesSendUssdMessageApiName, "SupplementaryServices SendUssdMessage");
_LIT(KSupplementaryServicesSendUssdMessageNoFdnCheckApiName, "SupplementaryServices SendUssdMessageNoFdnCheck");
_LIT(KSupplementaryServicesSendUssdReleaseApiName, "SupplementaryServices SendUssdRelease");
_LIT(KSupplementaryServicesClearBlacklistApiName, "SupplementaryServices ClearBlacklist");


const TDesC& ApiIdToName(TInt aIpc);

/**
 Factory function
 */
CMockLtsyEngine* CMockLtsyEngine::NewL(CCtsyDispatcherCallback& aCallback)
	{
	CMockLtsyEngine* self = new (ELeave)CMockLtsyEngine(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
 Constructor
 */
CMockLtsyEngine::CMockLtsyEngine(CCtsyDispatcherCallback& aCallback) :
	iCompletionCallback(aCallback)
	{
	} // CMockLtsyEngine::CMockLtsyEngine


CMockLtsyEngine::~CMockLtsyEngine()
	{
	delete iMockCallControlMessHandler;
	delete iMockPhoneMessHandler;
	delete iMockSecurityMessHandler;
	delete iMockPhonebookEnMessHandler;
	delete iMockCellBroadcastMessHandler;
	delete iMockPhonebookOnMessHandler;
	delete iMockPhonebookMessHandler;
	delete iMockSimMessHandler;
	delete iMockSmsMessHandler;
	delete iMockCallControlMultipartyMessHandler;
	delete iMockSupplementaryServicesMessHandler;
	delete iMockPacketServicesMessHandler;
	delete iMockSatMessHandler;
	delete iMockServer;
	} // CMockLtsyEngine::~CMockLtsyEngine


void CMockLtsyEngine::ConstructL()
	{
	CMockSYEngine::ConstructL();

	// Start MockSY server
	iMockServer = CMockSYServer::NewL(KMockLtsyServerName,
			CActive::EPriorityHigh, *this);

	// Create message handlers
	iMockCallControlMessHandler = CMockCallControlMessHandler::NewL(*this,
			iCompletionCallback);
	iMockPhoneMessHandler = CMockPhoneMessHandler::NewL(*this,
			iCompletionCallback);
	iMockSecurityMessHandler = CMockSecurityMessHandler::NewL(*this,
			iCompletionCallback);
	iMockPhonebookEnMessHandler = CMockPhonebookEnMessHandler::NewL(*this,
			iCompletionCallback);
	iMockCellBroadcastMessHandler = CMockCellBroadcastMessHandler::NewL(*this,
			iCompletionCallback);
	iMockPhonebookOnMessHandler = CMockPhonebookOnMessHandler::NewL(*this,
			iCompletionCallback);
	iMockPhonebookMessHandler = CMockPhonebookMessHandler::NewL(*this,
			iCompletionCallback);
	iMockSimMessHandler = CMockSimMessHandler::NewL(*this, iCompletionCallback);
	iMockSmsMessHandler = CMockSmsMessHandler::NewL(*this, iCompletionCallback);
	iMockCallControlMultipartyMessHandler
			= CMockCallControlMultipartyMessHandler::NewL(*this,
					iCompletionCallback);
	iMockSupplementaryServicesMessHandler
			= CMockSupplementaryServicesMessHandler::NewL(*this,
					iCompletionCallback);
	iMockPacketServicesMessHandler = CMockPacketServicesMessHandler::NewL(
			*this, iCompletionCallback);
	iMockSatMessHandler = CMockSatMessHandler::NewL(*this, iCompletionCallback);

	} // CMockLtsyEngine::ConstructL


TInt CMockLtsyEngine::ExtFuncL(TInt aFuncUnitId, TInt aInterfaceId, ...)
	{
	TInt ret = KErrNotSupported;
	MMmMessHandlerBase* messageHandler = RouteRequestUsingFUId(aFuncUnitId);
	if (messageHandler)
		{
		VA_LIST list;
		VA_START(list,aInterfaceId);
		ret = messageHandler->ExtFuncL(aInterfaceId, list);
		}

	return ret;
	} // CMockLtsyEngine::ExtFuncL


TBool CMockLtsyEngine::IsInterfaceSupported(const TInt aFuncUnitId,
		TLtsyDispatchInterfaceApiId aDispatchApiId)
	{
	switch (aFuncUnitId)
			{
			case KDispatchCallControlFuncUnitId:
				return iMockCallControlMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchPhoneFuncUnitId:
				return iMockPhoneMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchSecurityFuncUnitId:
				return iMockSecurityMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchPhonebookEnFuncUnitId:
				return iMockPhonebookEnMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchCellBroadcastFuncUnitId:
				return iMockCellBroadcastMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchPhonebookOnFuncUnitId:
				return iMockPhonebookOnMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchPhonebookFuncUnitId:
				return iMockPhonebookMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchSimFuncUnitId:
				return iMockSimMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchSmsFuncUnitId:
				return iMockSmsMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchCallControlMultipartyFuncUnitId:
				return iMockCallControlMultipartyMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchSupplementaryServicesFuncUnitId:
				return iMockSupplementaryServicesMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchPacketServicesFuncUnitId:
				return iMockPacketServicesMessHandler->IsHandlerForApi(aDispatchApiId);
			case KDispatchSatFuncUnitId:
				return iMockSatMessHandler->IsHandlerForApi(aDispatchApiId);
			default:
				return NULL;
			}
	} // CMockLtsyEngine::IsInterfaceSupported

void CMockLtsyEngine::IsCallbackIndSupported(const TInt /*aFuncUnitId*/,
		TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& aIndIdBitMask)
	{
	aIndIdBitMask = 0xFFFF; //simulate support for all indicators from MockLTSY.
	} // CMockLtsyEngine::IsCallbackIndSupported

MMockMessHandlerBase* CMockLtsyEngine::RouteRequestUsingFUId(TInt aFuncUnitId)
	{
	switch (aFuncUnitId)
		{
		case KDispatchCallControlFuncUnitId:
			return iMockCallControlMessHandler;
		case KDispatchPhoneFuncUnitId:
			return iMockPhoneMessHandler;
		case KDispatchSecurityFuncUnitId:
			return iMockSecurityMessHandler;
		case KDispatchPhonebookEnFuncUnitId:
			return iMockPhonebookEnMessHandler;
		case KDispatchCellBroadcastFuncUnitId:
			return iMockCellBroadcastMessHandler;
		case KDispatchPhonebookOnFuncUnitId:
			return iMockPhonebookOnMessHandler;
		case KDispatchPhonebookFuncUnitId:
			return iMockPhonebookMessHandler;
		case KDispatchSimFuncUnitId:
			return iMockSimMessHandler;
		case KDispatchSmsFuncUnitId:
			return iMockSmsMessHandler;
		case KDispatchCallControlMultipartyFuncUnitId:
			return iMockCallControlMultipartyMessHandler;
		case KDispatchSupplementaryServicesFuncUnitId:
			return iMockSupplementaryServicesMessHandler;
		case KDispatchPacketServicesFuncUnitId:
			return iMockPacketServicesMessHandler;
		case KDispatchSatFuncUnitId:
			return iMockSatMessHandler;
		default:
			return NULL;
		}
	} // CMockLtsyEngine::RouteRequestUsingFUId


MMockMessHandlerBase* CMockLtsyEngine::RouteRequestUsingApiId(TInt aApiId)
  {

  if (iMockCallControlMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockCallControlMessHandler;
	  }
  else if (iMockPhoneMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockPhoneMessHandler;
	  }
  else if (iMockSecurityMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockSecurityMessHandler;
	  }
  else if (iMockPhonebookEnMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockPhonebookEnMessHandler;
	  }
  else if (iMockCellBroadcastMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockCellBroadcastMessHandler;
	  }
  else if (iMockPhonebookOnMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockPhonebookOnMessHandler;
	  }
  else if (iMockPhonebookMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockPhonebookMessHandler;
	  }
  else if (iMockSimMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockSimMessHandler;
	  }
  else if (iMockSmsMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockSmsMessHandler;
	  }
  else if (iMockCallControlMultipartyMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockCallControlMultipartyMessHandler;
	  }
  else if (iMockSupplementaryServicesMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockSupplementaryServicesMessHandler;
	  }
  else if (iMockPacketServicesMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockPacketServicesMessHandler;
	  }
  else if (iMockSatMessHandler->IsHandlerForApi(aApiId))
	  {
	  return iMockSatMessHandler;
	  }
  //add others here
  else
	  {
	  return NULL;
	  }
  } // CMockLtsyEngine::RouteRequestUsingApiId


/**
 Generate a completion
 */
void CMockLtsyEngine::DoCompleteEventL(const TMockSYEvent& aEvent)
	{
	MMockMessHandlerBase* handler = RouteRequestUsingApiId(aEvent.iCmdId);
	ASSERT(handler!=NULL);
	handler->CompleteL(aEvent.iCmdId, aEvent.iData, aEvent.iResultCode);
	} // CMockLtsyEngine::DoCompleteEvent


TBool CMockLtsyEngine::CanIgnoreUnexpectedIpc(TInt aCmdId)
	{
	// CTSY sends ECtsyUpdateLifeTimeReq request to LTSY every 10sec. If a test case making call lasts
	// more than 10 sec, Mockltsy should be informed that ECtsyUpdateLifeTimeReq will be received at some point.
	// However, some test cases take more than 10 sec on some hardwares and less than 10sec on the other hardwares.
	// In this case, it is hardly possible to keep Mockltsy posted. Therefore, it is assumed that ECtsyUpdateLifeTimeReq
	// may received by Mockltsy at unexpected times and this doesn't indicate any error.
	TBool canIgnore = EFalse;
	if(MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId == aCmdId)
		{
		canIgnore = ETrue;
		}
	return canIgnore;
	}

void CMockLtsyEngine::LogData(const TDesC& aDesc)
	{
	Log(aDesc);
	}

void CMockLtsyEngine::LogRequest(TInt aCmdId,
		const MMockSyEngineData& /*aData*/, TInt aResultCode)
	{

	TBuf<KMaxLogLineSize> buffer;
	const TDesC &ipcName = ApiIdToName(aCmdId);
	if (ipcName != KNullDesC)
		buffer.Format(_L(">>> ipc=%S Err=%d"),&ipcName, aResultCode);
	else
		buffer.Format(_L(">>> ipc=%d (unmapped ipc) Err=%d"),aCmdId, aResultCode);
	Log(buffer);

	} // CMockLtsyEngine::LogRequest


void CMockLtsyEngine::LogCompletion(TInt aCmdId, const TDesC8& /*aData*/,
		TInt aResultCode)
	{
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& ipcName = ApiIdToName(aCmdId);
	if (ipcName != KNullDesC)
		buffer.Format(_L("<<< ipc=%S Err=%d"), &ipcName, aResultCode);
	else
		buffer.Format(_L("<<< ipc=%d (unmapped ipc) Err=%d"), aCmdId, aResultCode);
	Log(buffer);

	} // CMockLtsyEngine::LogCompletion

void CMockLtsyEngine::LogExpectError(TInt aCmdId,
		const MMockSyEngineData& aData, TInt aExpectedCmd,
		const TDesC8& aExpectedData, TBool aIsErrorIgnored)
	{
	TBuf<KMaxLogLineSize> buffer;
	const TDesC& ipcName = ApiIdToName(aCmdId);
	
	if(!aIsErrorIgnored)
		buffer.Copy(_L("ERROR: "));
	else
		buffer.Copy(_L("IGNORED: "));
	
	if (aExpectedCmd != 0)
		{
		const TDesC& expectedIpcName = ApiIdToName(aExpectedCmd);
		if(ipcName != KNullDesC)
			{
			if (expectedIpcName != KNullDesC)
				buffer.AppendFormat(_L("Expected ipc=%S Received ipc=%S"),&expectedIpcName, &ipcName);
			else
				buffer.AppendFormat(_L("Expected ipc=%d (unmapped ipc) Received ipc=%S"), aExpectedCmd, &ipcName);
			}
		else
			{
			if (expectedIpcName != KNullDesC)
				buffer.AppendFormat(_L("Expected ipc=%S Received ipc=%d (unmapped ipc)"),&expectedIpcName,aCmdId);
			else
				buffer.AppendFormat(_L("Expected ipc=%d (unmapped ipc) Received ipc=%d (unmapped ipc)"),aExpectedCmd,aCmdId);
			}
		Log(buffer);
		if (aExpectedCmd == aCmdId)
			{
			MMockMessHandlerBase* handler = RouteRequestUsingApiId(aCmdId);
			ASSERT(handler!=NULL);
			handler->LogErrorData(aExpectedCmd, aData, aExpectedData);
			}
		}
	else
		{
		if (ipcName != KNullDesC)
			buffer.AppendFormat(_L("Expected ipc=NONE Received ipc=%S"), &ipcName);
		else
			buffer.AppendFormat(_L("Expected ipc=NONE Received ipc=%d (unmapped ipc)"), aCmdId);
		Log(buffer);
		}
	} // CMockLtsyEngine::LogExpectError


const TDesC& ApiIdToName(TInt aIpc)
	{
	switch (aIpc)
		{
		case KMockLtsyDispatchCallControlNotifyCallStatusChangeIndId:
		    return KCallControlNotifyCallStatusChangeApiName;
		case KMockLtsyDispatchCallControlReceiveUuiIndId:
		    return KCallControlReceiveUuiApiName;
		case KMockLtsyDispatchCallControlNotifyDataCallCapsChangeIndId:
		    return KCallControlNotifyDataCallCapsChangeApiName;
		case KMockLtsyDispatchCallControlNotifyIncomingCallIndId:
		    return KCallControlNotifyIncomingCallApiName;
		case KMockLtsyDispatchCallControlNotifyHscsdInfoChangeIndId:
		    return KCallControlNotifyHscsdInfoChangeApiName;
		case KMockLtsyDispatchCallControlNotifyCallEventIndId:
		    return KCallControlNotifyCallEventApiName;
		case KMockLtsyDispatchCallControlNotifyCallInfoChangeIndId:
		    return KCallControlNotifyCallInfoChangeApiName;
		case KMockLtsyDispatchCallControlNotifyIccCallForwardingStatusChangeIndId:
		    return KCallControlNotifyIccCallForwardingStatusChangeApiName;
		case KMockLtsyDispatchCallControlRemoteAlertingToneChangeIndId:
		    return KCallControlRemoteAlertingToneChangeApiName;
		case MLtsyDispatchCallControlAnswer::KLtsyDispatchCallControlAnswerApiId:
		    return KCallControlAnswerApiName;
		case MLtsyDispatchCallControlHold::KLtsyDispatchCallControlHoldApiId:
		    return KCallControlHoldApiName;
		case MLtsyDispatchCallControlDialEmergency::KLtsyDispatchCallControlDialEmergencyApiId:
		    return KCallControlDialEmergencyApiName;
		case MLtsyDispatchCallControlStopDtmfTone::KLtsyDispatchCallControlStopDtmfToneApiId:
		    return KCallControlStopDtmfToneApiName;
		case MLtsyDispatchCallControlSetActiveAlsLine::KLtsyDispatchCallControlSetActiveAlsLineApiId:
		    return KCallControlSetActiveAlsLineApiName;
		case MLtsyDispatchCallControlSendDtmfTonesCancel::KLtsyDispatchCallControlSendDtmfTonesCancelApiId:
		    return KCallControlSendDtmfTonesCancelApiName;
		case MLtsyDispatchCallControlHangUp::KLtsyDispatchCallControlHangUpApiId:
		    return KCallControlHangUpApiName;
		case MLtsyDispatchCallControlResume::KLtsyDispatchCallControlResumeApiId:
		    return KCallControlResumeApiName;
		case MLtsyDispatchCallControlSetDynamicHscsdParams::KLtsyDispatchCallControlSetDynamicHscsdParamsApiId:
		    return KCallControlSetDynamicHscsdParamsApiName;
		case MLtsyDispatchCallControlDialVoice::KLtsyDispatchCallControlDialVoiceApiId:
		    return KCallControlDialVoiceApiName;
		case MLtsyDispatchCallControlTransfer::KLtsyDispatchCallControlTransferApiId:
		    return KCallControlTransferApiName;
		case MLtsyDispatchCallControlSendDtmfTones::KLtsyDispatchCallControlSendDtmfTonesApiId:
		    return KCallControlSendDtmfTonesApiName;
		case MLtsyDispatchCallControlGetIdentityServiceStatus::KLtsyDispatchCallControlGetIdentityServiceStatusApiId:
		    return KCallControlGetIdentityServiceStatusApiName;
		case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSwapApiId:
		    return KCallControlSwapApiName;
		case MLtsyDispatchCallControlSwap::KLtsyDispatchCallControlSingleSwapApiId:
		    return KCallControlSingleSwapApiName;
		case MLtsyDispatchCallControlLoanDataPort::KLtsyDispatchCallControlLoanDataPortApiId:
		    return KCallControlLoanDataPortApiName;
		case MLtsyDispatchCallControlRecoverDataPort::KLtsyDispatchCallControlRecoverDataPortApiId:
		    return KCallControlRecoverDataPortApiName;
		case MLtsyDispatchCallControlStartDtmfTone::KLtsyDispatchCallControlStartDtmfToneApiId:
		    return KCallControlStartDtmfToneApiName;
		case MLtsyDispatchCallControlGetActiveAlsLine::KLtsyDispatchCallControlGetActiveAlsLineApiId:
		    return KCallControlGetActiveAlsLineApiName;
		case MLtsyDispatchCallControlDialData::KLtsyDispatchCallControlDialDataApiId:
		    return KCallControlDialDataApiName;
		case MLtsyDispatchCallControlQueryIsEmergencyNumber::KLtsyDispatchCallControlQueryIsEmergencyNumberApiId:
		    return KCallControlQueryIsEmergencyNumberApiName;
		case MLtsyDispatchCallControlGetAlsPpSupport::KLtsyDispatchCallControlGetAlsPpSupportApiId:
		    return KCallControlGetAlsPpSupportApiName;
		case MLtsyDispatchCallControlGetAlsBlockedStatus::KLtsyDispatchCallControlGetAlsBlockedStatusApiId:
		    return KCallControlGetAlsBlockedStatusApiName;
		case MLtsyDispatchCallControlSetAlsBlocked::KLtsyDispatchCallControlSetAlsBlockedApiId:
		    return KCallControlSetAlsBlockedApiName;
		case MLtsyDispatchCallControlGetLifeTime::KLtsyDispatchCallControlGetLifeTimeApiId:
		    return KCallControlGetLifeTimeApiName;
		case MLtsyDispatchCallControlTerminateErrorCall::KLtsyDispatchCallControlTerminateErrorCallApiId:
		    return KCallControlTerminateErrorCallApiName;
		case MLtsyDispatchCallControlTerminateAllCalls::KLtsyDispatchCallControlTerminateAllCallsApiId:
		    return KCallControlTerminateAllCallsApiName;
		case MLtsyDispatchCallControlGetCallForwardingIndicator::KLtsyDispatchCallControlGetCallForwardingIndicatorApiId:
		    return KCallControlGetCallForwardingIndicatorApiName;
		case MLtsyDispatchCallControlUpdateLifeTimer::KLtsyDispatchCallControlUpdateLifeTimerApiId:
		    return KCallControlUpdateLifeTimerApiName;
		case MLtsyDispatchCallControlMultipartyConferenceHangUp::KLtsyDispatchCallControlMultipartyConferenceHangUpApiId:
		    return KCallControlMultipartyConferenceHangUpApiName;
		case MLtsyDispatchCallControlMultipartyConferenceAddCall::KLtsyDispatchCallControlMultipartyConferenceAddCallApiId:
		    return KCallControlMultipartyConferenceAddCallApiName;
		case MLtsyDispatchCallControlMultipartyCreateConference::KLtsyDispatchCallControlMultipartyCreateConferenceApiId:
		    return KCallControlMultipartyCreateConferenceApiName;
		case MLtsyDispatchCallControlMultipartyConferenceSwap::KLtsyDispatchCallControlMultipartyConferenceSwapApiId:
		    return KCallControlMultipartyConferenceSwapApiName;
		case MLtsyDispatchCallControlMultipartyConferenceGoOneToOne::KLtsyDispatchCallControlMultipartyConferenceGoOneToOneApiId:
		    return KCallControlMultipartyConferenceGoOneToOneApiName;
		case KMockLtsyDispatchCellBroadcastGsmBroadcastNotifyMessageReceivedIndId:
		    return KCellBroadcastGsmBroadcastNotifyMessageReceivedApiName;
		case KMockLtsyDispatchCellBroadcastWcdmaBroadcastMessageReceivedIndId:
		    return KCellBroadcastWcdmaBroadcastMessageReceivedApiName;
		case MLtsyDispatchCellBroadcastSetBroadcastFilterSetting::KLtsyDispatchCellBroadcastSetBroadcastFilterSettingApiId:
		    return KCellBroadcastSetBroadcastFilterSettingApiName;
		case MLtsyDispatchCellBroadcastActivateBroadcastReceiveMessage::KLtsyDispatchCellBroadcastActivateBroadcastReceiveMessageApiId:
		    return KCellBroadcastActivateBroadcastReceiveMessageApiName;
		case MLtsyDispatchCellBroadcastReceiveMessageCancel::KLtsyDispatchCellBroadcastReceiveMessageCancelApiId:
		    return KCellBroadcastReceiveMessageCancelApiName;
		case MLtsyDispatchCellBroadcastStartSimCbTopicBrowsing::KLtsyDispatchCellBroadcastStartSimCbTopicBrowsingApiId:
		    return KCellBroadcastStartSimCbTopicBrowsingApiName;
		case MLtsyDispatchCellBroadcastDeleteSimCbTopic::KLtsyDispatchCellBroadcastDeleteSimCbTopicApiId:
		    return KCellBroadcastDeleteSimCbTopicApiName;
		case KMockLtsyDispatchPacketServicesNotifyPacketAttachModeChangeIndId:
		    return KPacketServicesNotifyPacketAttachModeChangeApiName;
		case KMockLtsyDispatchPacketServicesNotifyQosProfileChangedIndId:
		    return KPacketServicesNotifyQosProfileChangedApiName;
		case KMockLtsyDispatchPacketServicesNotifyRatTransferCapsChangeIndId:
		    return KPacketServicesNotifyRatTransferCapsChangeApiName;
		case KMockLtsyDispatchPacketServicesNotifyPacketMsClassChangeIndId:
		    return KPacketServicesNotifyPacketMsClassChangeApiName;
		case KMockLtsyDispatchPacketServicesNotifyPacketDynamicCapsChangeIndId:
		    return KPacketServicesNotifyPacketDynamicCapsChangeApiName;
		case KMockLtsyDispatchPacketServicesNotifyPdpContextAddedIndId:
		    return KPacketServicesNotifyPdpContextAddedApiName;
		case KMockLtsyDispatchPacketServicesNotifyNetworkInitiatedContextActivationRequestIndId:
		    return KPacketServicesNotifyNetworkInitiatedContextActivationRequestApiName;
		case KMockLtsyDispatchPacketServicesNotifyPdpContextConfigChangedIndId:
		    return KPacketServicesNotifyPdpContextConfigChangedApiName;
		case KMockLtsyDispatchPacketServicesNotifyPdpContextStatusChangeIndId:
		    return KPacketServicesNotifyPdpContextStatusChangeApiName;
		case KMockLtsyDispatchPacketServicesNotifyPacketNetworkRegistrationStatusIndId:
		    return KPacketServicesNotifyPacketNetworkRegistrationStatusApiName;
		case KMockLtsyDispatchPacketServicesNotifyPdpContextConnectionSpeedChangeIndId:
		    return KPacketServicesNotifyPdpContextConnectionSpeedChangeApiName;
		case KMockLtsyDispatchPacketServicesNotifyMbmsContextConfigChangedIndId:
		    return KPacketServicesNotifyMbmsContextConfigChangedApiName;
		case KMockLtsyDispatchPacketServicesNotifyMbmsNetworkServiceStatusChangeIndId:
		    return KPacketServicesNotifyMbmsNetworkServiceStatusChangeApiName;
		case KMockLtsyDispatchPacketServicesNotifyMbmsServiceAvailabilityChangeIndId:
		    return KPacketServicesNotifyMbmsServiceAvailabilityChangeApiName;
		case KMockLtsyDispatchPacketServicesNotifyConnectionInfoChangeIndId:
			return KPacketServicesNotifyConnectionInfoChange;		   
		case MLtsyDispatchPacketServicesPacketAttach::KLtsyDispatchPacketServicesPacketAttachApiId:
		    return KPacketServicesPacketAttachApiName;
		case MLtsyDispatchPacketServicesGetPacketAttachMode::KLtsyDispatchPacketServicesGetPacketAttachModeApiId:
		    return KPacketServicesGetPacketAttachModeApiName;
		case MLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatus::KLtsyDispatchPacketServicesGetPacketNetworkRegistrationStatusApiId:
		    return KPacketServicesGetPacketNetworkRegistrationStatusApiName;
		case MLtsyDispatchPacketServicesPacketDetach::KLtsyDispatchPacketServicesPacketDetachApiId:
		    return KPacketServicesPacketDetachApiName;
		case MLtsyDispatchPacketServicesSetPdpContextConfig::KLtsyDispatchPacketServicesSetPdpContextConfigApiId:
		    return KPacketServicesSetPdpContextConfigApiName;
		case MLtsyDispatchPacketServicesModifyActivePdpContext::KLtsyDispatchPacketServicesModifyActivePdpContextApiId:
		    return KPacketServicesModifyActivePdpContextApiName;
		case MLtsyDispatchPacketServicesInitialisePdpContext::KLtsyDispatchPacketServicesInitialisePdpContextApiId:
		    return KPacketServicesInitialisePdpContextApiName;
		case MLtsyDispatchPacketServicesDeletePdpContext::KLtsyDispatchPacketServicesDeletePdpContextApiId:
		    return KPacketServicesDeletePdpContextApiName;
		case MLtsyDispatchPacketServicesSetPacketAttachMode::KLtsyDispatchPacketServicesSetPacketAttachModeApiId:
		    return KPacketServicesSetPacketAttachModeApiName;
		case MLtsyDispatchPacketServicesNotifyPacketStatusChange::KLtsyDispatchPacketServicesNotifyPacketStatusChangeApiId:
		    return KPacketServicesNotifyPacketStatusChangeApiName;
		case MLtsyDispatchPacketServicesSetDefaultPdpContextGprsParams::KLtsyDispatchPacketServicesSetDefaultPdpContextGprsParamsApiId:
		    return KPacketServicesSetDefaultPdpContextGprsParamsApiName;
		case MLtsyDispatchPacketServicesSetDefaultPdpContextR99R4Params::KLtsyDispatchPacketServicesSetDefaultPdpContextR99R4ParamsApiId:
		    return KPacketServicesSetDefaultPdpContextR99R4ParamsApiName;
		case MLtsyDispatchPacketServicesActivatePdpContext::KLtsyDispatchPacketServicesActivatePdpContextApiId:
		    return KPacketServicesActivatePdpContextApiName;
		case MLtsyDispatchPacketServicesSetPdpContextQosGprs::KLtsyDispatchPacketServicesSetPdpContextQosGprsApiId:
		    return KPacketServicesSetPdpContextQosGprsApiName;
		case MLtsyDispatchPacketServicesSetPdpContextQosR99R4::KLtsyDispatchPacketServicesSetPdpContextQosR99R4ApiId:
		    return KPacketServicesSetPdpContextQosR99R4ApiName;
		case MLtsyDispatchPacketServicesSetPdpContextQosR5::KLtsyDispatchPacketServicesSetPdpContextQosR5ApiId:
		    return KPacketServicesSetPdpContextQosR5ApiName;
		case MLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequest::KLtsyDispatchPacketServicesRejectNetworkInitiatedContextActivationRequestApiId:
		    return KPacketServicesRejectNetworkInitiatedContextActivationRequestApiName;
		case MLtsyDispatchPacketServicesDeactivatePdpContext::KLtsyDispatchPacketServicesDeactivatePdpContextApiId:
		    return KPacketServicesDeactivatePdpContextApiName;
		case MLtsyDispatchPacketServicesAddPacketFilter::KLtsyDispatchPacketServicesAddPacketFilterApiId:
		    return KPacketServicesAddPacketFilterApiName;		    
		case MLtsyDispatchPacketServicesGetStatus::KLtsyDispatchPacketServicesGetStatusApiId:
		    return KPacketServicesGetStatusApiName;
		case MLtsyDispatchPacketServicesGetStaticCapabilities::KLtsyDispatchPacketServicesGetStaticCapabilitiesApiId:
		    return KPacketServicesGetStaticCapabilitiesApiName;
		case MLtsyDispatchPacketServicesGetMaxNoMonitoredServiceLists::KLtsyDispatchPacketServicesGetMaxNoMonitoredServiceListsApiId:
		    return KPacketServicesGetMaxNoMonitoredServiceListsApiName;
		case MLtsyDispatchPacketServicesGetMaxNoActiveServices::KLtsyDispatchPacketServicesGetMaxNoActiveServicesApiId:
		    return KPacketServicesGetMaxNoActiveServicesApiName;
		case MLtsyDispatchPacketServicesInitialiseMbmsContext::KLtsyDispatchPacketServicesInitialiseMbmsContextApiId:
		    return KPacketServicesInitialiseMbmsContextApiName;
		case MLtsyDispatchPacketServicesGetMbmsNetworkServiceStatus::KLtsyDispatchPacketServicesGetMbmsNetworkServiceStatusApiId:
		    return KPacketServicesGetMbmsNetworkServiceStatusApiName;
		case MLtsyDispatchPacketServicesUpdateMbmsMonitorServiceList::KLtsyDispatchPacketServicesUpdateMbmsMonitorServiceListApiId:
		    return KPacketServicesUpdateMbmsMonitorServiceListApiName;
		case MLtsyDispatchPacketServicesUpdateMbmsSessionList::KLtsyDispatchPacketServicesUpdateMbmsSessionListApiId:
		    return KPacketServicesUpdateMbmsSessionListApiName;
		case MLtsyDispatchPacketServicesRemovePacketFilter::KLtsyDispatchPacketServicesRemovePacketFilterApiId:
			return KPacketServicesRemovePacketFilterApiName;
		case KMockLtsyDispatchPhoneNotifyNetworkModeChangeIndId:
		    return KPhoneNotifyNetworkModeChangeApiName;
		case KMockLtsyDispatchPhoneNotifyEgprsInfoChangeIndId:
		    return KPhoneNotifyEgprsInfoChangeApiName;
		case KMockLtsyDispatchPhoneNotifySignalStrengthChangeIndId:
		    return KPhoneNotifySignalStrengthChangeApiName;
		case KMockLtsyDispatchPhoneNotifyNetworkRegistrationStatusChangeIndId:
		    return KPhoneNotifyNetworkRegistrationStatusChangeApiName;
		case KMockLtsyDispatchPhoneNotifyNitzInfoChangeIndId:
		    return KPhoneNotifyNitzInfoChangeApiName;
		case KMockLtsyDispatchPhoneNotifyBatteryInfoChangeIndId:
		    return KPhoneNotifyBatteryInfoChangeApiName;
		case KMockLtsyDispatchPhoneNotifyCurrentNetworkChangeIndId:
		    return KPhoneNotifyCurrentNetworkChangeApiName;
		case KMockLtsyDispatchPhoneNotifyRfStatusChangeIndId:
		    return KPhoneNotifyRfStatusChangeApiName;
		case KMockLtsyDispatchPhoneNotifyNspsStatusChangeIndId:
		    return KPhoneNotifyNspsStatusChangeApiName;
		case KMockLtsyDispatchPhoneNotifyNetworkSelectionSettingChangeIndId:
		    return KPhoneNotifyNetworkSelectionSettingChangeApiName;
		case KMockLtsyDispatchPhoneNotifyRauEventIndId:
		    return KPhoneNotifyRauEventApiName;
		case KMockLtsyDispatchPhoneNotifyNetworkConnectionFailureIndId:
		    return KPhoneNotifyNetworkConnectionFailureApiName;
		case KMockLtsyDispatchCustomPhoneNotifyDtmfEventIndId:
		    return KCustomPhoneNotifyDtmfEventApiName;
		case KMockLtsyDispatchPhoneNotifyDtmfEventIndId:
		    return KPhoneNotifyDtmfEventApiName;
		case KMockLtsyDispatchPhoneNotifyHspaStatusChangedIndId:
		    return KPhoneNotifyHspaStatusChangedApiName;
		case KMockLtsyDispatchPhoneNotifyCellInfoChangedIndId:
		    return KPhoneNotifyCellInfoChangedApiName;
		case KMockLtsyDispatchPhoneNotifyCellInfoChangeIndId:
		    return KPhoneNotifyCellInfoChangeApiName;
		case MLtsyDispatchPhoneBootNotifyModemStatusReady::KLtsyDispatchPhoneBootNotifyModemStatusReadyApiId:
		    return KPhoneBootNotifyModemStatusReadyApiName;
		case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId:
		    return KPhoneBootNotifySimStatusReadyApiName;
		case MLtsyDispatchPhoneGetFdnStatus::KLtsyDispatchPhoneGetFdnStatusApiId:
		    return KPhoneGetFdnStatusApiName;
		case MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId:
		    return KPhoneGetHomeNetworkApiName;
		case MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId:
		    return KPhoneGetNetworkRegistrationStatusApiName;
		case MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId:
		    return KPhoneSimRefreshRegisterApiName;
		case MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId:
		    return KPhoneGetServiceProviderNameApiName;
		case MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId:
		    return KPhoneGetPhoneIdApiName;
		case MLtsyDispatchPhoneGetDetectedNetworks::KLtsyDispatchPhoneGetDetectedNetworksApiId:
		    return KPhoneGetDetectedNetworksApiName;
		case MLtsyDispatchPhoneGetDetectedNetworksCancel::KLtsyDispatchPhoneGetDetectedNetworksCancelApiId:
		    return KPhoneGetDetectedNetworksCancelApiName;
		case MLtsyDispatchPhoneSelectNetwork::KLtsyDispatchPhoneSelectNetworkApiId:
		    return KPhoneSelectNetworkApiName;
		case MLtsyDispatchPhoneSelectNetworkCancel::KLtsyDispatchPhoneSelectNetworkCancelApiId:
		    return KPhoneSelectNetworkCancelApiName;
		case MLtsyDispatchPhoneSetNetworkSelectionSetting::KLtsyDispatchPhoneSetNetworkSelectionSettingApiId:
		    return KPhoneSetNetworkSelectionSettingApiName;
		case MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId:
		    return KPhoneGetCurrentNetworkInfoApiName;
		case MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId:
		    return KPhoneGetNetworkModeApiName;
		case MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId:
		    return KPhoneGetNitzInfoApiName;
		case MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId:
		    return KPhoneGetSignalStrengthApiName;
		case MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId:
		    return KPhoneGetBatteryInfoApiName;
		case MLtsyDispatchPhoneNspsWakeup::KLtsyDispatchPhoneNspsWakeupApiId:
		    return KPhoneNspsWakeupApiName;
		case MLtsyDispatchPhoneSetSystemNetworkMode::KLtsyDispatchPhoneSetSystemNetworkModeApiId:
		    return KPhoneSetSystemNetworkModeApiName;
		case MLtsyDispatchPhoneGetCurrentSystemNetworkModes::KLtsyDispatchPhoneGetCurrentSystemNetworkModesApiId:
		    return KPhoneGetCurrentSystemNetworkModesApiName;
		case MLtsyDispatchPhoneResetNetServer::KLtsyDispatchPhoneResetNetServerApiId:
		    return KPhoneResetNetServerApiName;
		case MLtsyDispatchPhoneSetAlwaysOnMode::KLtsyDispatchPhoneSetAlwaysOnModeApiId:
		    return KPhoneSetAlwaysOnModeApiName;
		case MLtsyDispatchPhoneSetDriveMode::KLtsyDispatchPhoneSetDriveModeApiId:
		    return KPhoneSetDriveModeApiName;
		case MLtsyDispatchPhoneGetHspaStatus::KLtsyDispatchPhoneGetHspaStatusApiId:
		    return KPhoneGetHspaStatusApiName;
		case MLtsyDispatchPhoneSetHspaStatus::KLtsyDispatchPhoneSetHspaStatusApiId:
		    return KPhoneSetHspaStatusApiName;
		case MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId:
		    return KPhoneGetNetworkProviderNameApiName;
		case MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId:
		    return KPhoneGetOperatorNameApiName;
		case MLtsyDispatchPhoneGetCellInfo::KLtsyDispatchPhoneGetCellInfoApiId:
		    return KPhoneGetCellInfoApiName;
		case MLtsyDispatchPhoneRegisterCellInfoChangeNotification::KLtsyDispatchPhoneRegisterCellInfoChangeNotificationApiId:
		    return KPhoneRegisterCellInfoChangeNotificationApiName;
		case MLtsyDispatchPhoneGetPhoneCellInfo::KLtsyDispatchPhoneGetPhoneCellInfoApiId:
		    return KPhoneGetPhoneCellInfoApiName;
		case MLtsyDispatchPhoneGetUsimServiceSupport::KLtsyDispatchPhoneGetUsimServiceSupportApiId:
		    return KPhoneGetUsimServiceSupportApiName;
		case MLtsyDispatchPhoneGetCurrentActiveUsimApplication::KLtsyDispatchPhoneGetCurrentActiveUsimApplicationApiId:
		    return KPhoneGetCurrentActiveUsimApplicationApiName;
		case MLtsyDispatchPhoneTerminateAllCalls::KLtsyDispatchPhoneTerminateAllCallsApiId:
		    return KPhoneTerminateAllCallsApiName;
		case MLtsyDispatchPhoneGetSystemNetworkBand::KLtsyDispatchPhoneGetSystemNetworkBandApiId:
		    return KPhoneGetSystemNetworkBandApiName;
		case MLtsyDispatchPhoneSetSystemNetworkBand::KLtsyDispatchPhoneSetSystemNetworkBandApiId:
		    return KPhoneSetSystemNetworkBandApiName;
		case KMockLtsyDispatchPhonebookStoreResetCacheIndId:
		    return KPhonebookStoreResetCacheApiName;
		case KMockLtsyDispatchPhonebookStoreSetFdnPhonebookInfoIndId:
		    return KPhonebookStoreSetFdnPhonebookInfoApiName;
		case MLtsyDispatchPhonebookStoreReadEntry::KLtsyDispatchPhonebookStoreReadEntryApiId:
		    return KPhonebookStoreReadEntryApiName;
		case MLtsyDispatchPhonebookStoreDeleteEntry::KLtsyDispatchPhonebookStoreDeleteEntryApiId:
		    return KPhonebookStoreDeleteEntryApiName;
		case MLtsyDispatchPhonebookStoreCache::KLtsyDispatchPhonebookStoreCacheApiId:
		    return KPhonebookStoreCacheApiName;
		case MLtsyDispatchPhonebookStoreCacheCancel::KLtsyDispatchPhonebookStoreCacheCancelApiId:
		    return KPhonebookStoreCacheCancelApiName;
		case MLtsyDispatchPhonebookStoreGetPhonebookInfo::KLtsyDispatchPhonebookStoreGetPhonebookInfoApiId:
		    return KPhonebookStoreGetPhonebookInfoApiName;
		case MLtsyDispatchPhonebookStoreInitialise::KLtsyDispatchPhonebookStoreInitialiseApiId:
		    return KPhonebookStoreInitialiseApiName;
		case MLtsyDispatchPhonebookStoreDeleteAll::KLtsyDispatchPhonebookStoreDeleteAllApiId:
		    return KPhonebookStoreDeleteAllApiName;
		case MLtsyDispatchPhonebookStoreWriteEntry::KLtsyDispatchPhonebookStoreWriteEntryApiId:
		    return KPhonebookStoreWriteEntryApiName;
		case MLtsyDispatchPhonebookGetPhoneStoreInfo::KLtsyDispatchPhonebookGetPhoneStoreInfoApiId:
		    return KPhonebookGetPhoneStoreInfoApiName;
		case MLtsyDispatchPhonebookSmsStoreReadAll::KLtsyDispatchPhonebookSmsStoreReadAllApiId:
		    return KPhonebookSmsStoreReadAllApiName;
		case MLtsyDispatchPhonebookSmsStoreDeleteEntry::KLtsyDispatchPhonebookSmsStoreDeleteEntryApiId:
		    return KPhonebookSmsStoreDeleteEntryApiName;
		case MLtsyDispatchPhonebookSmsStoreDeleteAll::KLtsyDispatchPhonebookSmsStoreDeleteAllApiId:
		    return KPhonebookSmsStoreDeleteAllApiName;
		case MLtsyDispatchPhonebookSmsStoreGetInfo::KLtsyDispatchPhonebookSmsStoreGetInfoApiId:
		    return KPhonebookSmsStoreGetInfoApiName;
		case MLtsyDispatchPhonebookSmsStoreReadEntry::KLtsyDispatchPhonebookSmsStoreReadEntryApiId:
		    return KPhonebookSmsStoreReadEntryApiName;
		case MLtsyDispatchPhonebookSmsStoreWriteEntry::KLtsyDispatchPhonebookSmsStoreWriteEntryApiId:
		    return KPhonebookSmsStoreWriteEntryApiName;
		case MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId:
		    return KPhonebookEnStoreGetInfoApiName;
		case MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId:
		    return KPhonebookEnStoreReadAllApiName;
		case MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId:
		    return KPhonebookEnStoreReadEntryApiName;
		case MLtsyDispatchPhonebookOnStoreRead::KLtsyDispatchPhonebookOnStoreReadApiId:
		    return KPhonebookOnStoreReadApiName;
		case MLtsyDispatchPhonebookOnStoreDeleteAll::KLtsyDispatchPhonebookOnStoreDeleteAllApiId:
		    return KPhonebookOnStoreDeleteAllApiName;
		case MLtsyDispatchPhonebookOnStoreReadEntry::KLtsyDispatchPhonebookOnStoreReadEntryApiId:
		    return KPhonebookOnStoreReadEntryApiName;
		case MLtsyDispatchPhonebookOnStoreGetInfo::KLtsyDispatchPhonebookOnStoreGetInfoApiId:
		    return KPhonebookOnStoreGetInfoApiName;
		case MLtsyDispatchPhonebookOnStoreGetReadStoreSize::KLtsyDispatchPhonebookOnStoreGetReadStoreSizeApiId:
		    return KPhonebookOnStoreGetReadStoreSizeApiName;
		case MLtsyDispatchPhonebookOnStoreDeleteEntry::KLtsyDispatchPhonebookOnStoreDeleteEntryApiId:
		    return KPhonebookOnStoreDeleteEntryApiName;
		case MLtsyDispatchPhonebookOnStoreWriteEntry::KLtsyDispatchPhonebookOnStoreWriteEntryApiId:
		    return KPhonebookOnStoreWriteEntryApiName;
		case MLtsyDispatchPhonebookOnStoreWrite::KLtsyDispatchPhonebookOnStoreWriteApiId:
		    return KPhonebookOnStoreWriteApiName;
		case MLtsyDispatchPhonebookOnStoreGetStoreSize::KLtsyDispatchPhonebookOnStoreGetStoreSizeApiId:
		    return KPhonebookOnStoreGetStoreSizeApiName;
		case KMockLtsyDispatchSatUssdControlSupportedChangeIndId:
		    return KSatUssdControlSupportedChangeApiName;
		case KMockLtsyDispatchSatPcmdIndId:
		    return KSatPcmdApiName;
		case KMockLtsyDispatchSatNotifyProactiveSimSessionEndIndId:
		    return KSatNotifyProactiveSimSessionEndApiName;
		case KMockLtsyDispatchSatNotifyLocalInfoIndId:
		    return KSatNotifyLocalInfoApiName;
		case KMockLtsyDispatchSatTimeZoneChangeIndId:
		    return KSatTimeZoneChangeApiName;
		case KMockLtsyDispatchSatAccessTechnologyChangeIndId:
		    return KSatAccessTechnologyChangeApiName;
		case KMockLtsyDispatchSatCallConnectedIndId:
		    return KSatCallConnectedApiName;
		case KMockLtsyDispatchSatSsChangeIndId:
		    return KSatSsChangeApiName;
		case KMockLtsyDispatchSatCallDisconnectedIndId:
		    return KSatCallDisconnectedApiName;
		case KMockLtsyDispatchSatMtCallIndId:
		    return KSatMtCallApiName;
		case KMockLtsyDispatchSatCallControlEnvelopeResponseIndId:
		    return KKMockLtsyDispatchSatCallControlEnvelopeResponseApiName;
		case KMockLtsyDispatchSatCreateCallControlEnvelopeIndId:
		    return KSatCreateCallControlEnvelopeApiName;
		case KMockLtsyDispatchSatCreateMoSmControlEnvelopeIndId:
		    return KSatCreateMoSmControlEnvelopeApiName;
		case KMockLtsyDispatchSatImsiChangedIndId:
		    return KSatImsiChangedApiName;
		case KMockLtsyDispatchSatLocationStatusIndId:
		    return KSatLocationStatusApiName;
		case KMockLtsyDispatchSatRemoveEventListIndId:
		    return KSatRemoveEventListApiName;
		case KMockLtsyDispatchSatCreateRefreshTerminalRspIndId:
		    return KSatCreateRefreshTerminalRspApiName;
		case KMockLtsyDispatchSatMoSmControlEnvelopeResponseIndId:
		    return KSatMoSmControlEnvelopeResponseApiName;
		case KMockLtsyDispatchSatCreateSmsPpDownloadEnvelopeIndId:
		    return KSatCreateSmsPpDownloadEnvelopeApiName;
		case KMockLtsyDispatchSatCreateSmsDeliveryReportIndId:
		    return KSatCreateSmsDeliveryReportApiName;
		case KMockLtsyDispatchSatCreateCellBroadcastEnvelopeIndId:
		    return KSatCreateCellBroadcastEnvelopeApiName;
		case MLtsyDispatchSatSmsDeliverReport::KLtsyDispatchSatSmsDeliverReportApiId:
		    return KSatSmsDeliverReportApiName;
		case MLtsyDispatchSatGetImageInstance::KLtsyDispatchSatGetImageInstanceApiId:
		    return KSatGetImageInstanceApiName;
		case MLtsyDispatchSatGetIconData::KLtsyDispatchSatGetIconDataApiId:
		    return KSatGetIconDataApiName;
		case MLtsyDispatchSatGetClut::KLtsyDispatchSatGetClutApiId:
		    return KSatGetClutApiName;
		case MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId:
		    return KSatGetDefaultBearerCapabilityApiName;
		case MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId:
		    return KSatGetSmsPpDownloadSupportedApiName;
		case MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId:
		    return KSatGetSmsControlActivatedApiName;
		case MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId:
		    return KSatGetAccessTechnologyApiName;
		case MLtsyDispatchSatCellBroadcastEnvelope::KLtsyDispatchSatCellBroadcastEnvelopeApiId:
		    return KSatCellBroadcastEnvelopeApiName;
		case MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId:
		    return KSatTimingAdvanceApiName;
		case MLtsyDispatchSatSmControlResponseData::KLtsyDispatchSatSmControlResponseDataApiId:
		    return KSatSmControlResponseDataApiName;
		case MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId:
		    return KSatProvideLocationInfoApiName;
		case MLtsyDispatchSatSetPollingInterval::KLtsyDispatchSatSetPollingIntervalApiId:
		    return KSatSetPollingIntervalApiName;
		case MLtsyDispatchSatSmsPpDownloadEnvelope::KLtsyDispatchSatSmsPpDownloadEnvelopeApiId:
		    return KSatSmsPpDownloadEnvelopeApiName;
		case MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId:
		    return KSatLocalInformationNmrApiName;
		case MLtsyDispatchSatCallAndSmsControlEnvelope::KLtsyDispatchSatCallAndSmsControlEnvelopeApiId:
		    return KSatCallAndSmsControlEnvelopeApiName;
		case MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId:
		    return KSatRefreshAllowedApiName;
		case MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId:
		    return KSatReadyApiName;
		case MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId:
		    return KSatPCmdNotificationApiName;
		case MLtsyDispatchSatUssdControlEnvelopeError::KLtsyDispatchSatUssdControlEnvelopeErrorApiId:
		    return KSatUssdControlEnvelopeErrorApiName;
		case MLtsyDispatchSatTimerExpirationEnvelope::KLtsyDispatchSatTimerExpirationEnvelopeApiId:
		    return KSatTimerExpirationEnvelopeApiName;
		case MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId:
		    return KSatTerminalRspApiName;
		case MLtsyDispatchSatMenuSelectionEnvelope::KLtsyDispatchSatMenuSelectionEnvelopeApiId:
		    return KSatMenuSelectionEnvelopeApiName;
		case MLtsyDispatchSatEventDownloadEnvelope::KLtsyDispatchSatEventDownloadEnvelopeApiId:
		    return KSatEventDownloadEnvelopeApiName;
		case MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId:
		    return KSatGetUssdControlSupportedApiName;
		case KMockLtsyDispatchSecurityBootNotifySecurityReadyIndId:
		    return KSecurityBootNotifySecurityReadyApiName;
		case KMockLtsyDispatchSecurityNotifySecurityEventIndId:
		    return KSecurityNotifySecurityEventApiName;
		case KMockLtsyDispatchSecurityNotifyLockInfoChangeIndId:
		    return KSecurityNotifyLockInfoChangeApiName;
		case MLtsyDispatchSecurityGetSecurityCodeInfo::KLtsyDispatchSecurityGetSecurityCodeInfoApiId:
		    return KSecurityGetSecurityCodeInfoApiName;
		case MLtsyDispatchSecurityGetLockInfo::KLtsyDispatchSecurityGetLockInfoApiId:
		    return KSecurityGetLockInfoApiName;
		case MLtsyDispatchSecurityAbortSecurityCode::KLtsyDispatchSecurityAbortSecurityCodeApiId:
		    return KSecurityAbortSecurityCodeApiName;
		case MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId:
		    return KSecurityGetCurrentActivePinApiName;
		case MLtsyDispatchSecuritySetLockSetting::KLtsyDispatchSecuritySetLockSettingApiId:
		    return KSecuritySetLockSettingApiName;
		case MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId:
		    return KSecurityVerifySecurityCodeApiName;
		case MLtsyDispatchSecurityGetPin1DisableSupported::KLtsyDispatchSecurityGetPin1DisableSupportedApiId:
		    return KSecurityGetPin1DisableSupportedApiName;
		case MLtsyDispatchSecurityCheckSecurityCode::KLtsyDispatchSecurityCheckSecurityCodeApiId:
		    return KSecurityCheckSecurityCodeApiName;
		case MLtsyDispatchSecurityDisablePhoneLock::KLtsyDispatchSecurityDisablePhoneLockApiId:
		    return KSecurityDisablePhoneLockApiName;
		case MLtsyDispatchSecurityGetCipheringInfo::KLtsyDispatchSecurityGetCipheringInfoApiId:
		    return KSecurityGetCipheringInfoApiName;
		case MLtsyDispatchSecurityIsSecurityCodeBlocked::KLtsyDispatchSecurityIsSecurityCodeBlockedApiId:
		    return KSecurityIsSecurityCodeBlockedApiName;
		case MLtsyDispatchSecurityCheckSecurityCodeCancel::KLtsyDispatchSecurityCheckSecurityCodeCancelApiId:
		    return KSecurityCheckSecurityCodeCancelApiName;
		case KMockLtsyDispatchSimRefreshSimFilesIndId:
		    return KSimRefreshSimFilesApiName;
		case KMockLtsyDispatchSimNotifyIccMessageWaitingIndicatorsChangeIndId:
		    return KSimNotifyIccMessageWaitingIndicatorsChangeApiName;
		case KMockLtsyDispatchSimNotifyApnListChangeIndId:
		    return KSimNotifyApnListChangeApiName;
		case KMockLtsyDispatchSimNotifyApnControlListServiceStatusChangeIndId:
		    return KSimNotifyApnControlListServiceStatusChangeApiName;
		case MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId:
		    return KSimGetApnControlListServiceStatusApiName;
		case MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId:
		    return KSimDeleteApnNameApiName;
		case MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId:
		    return KSimEnumerateApnEntriesApiName;
		case MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId:
		    return KSimChangeSecurityCodeApiName;
		case MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId:
		    return KSimSetFdnSettingApiName;
		case MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId:
		    return KSimGetCustomerServiceProfileApiName;
		case MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId:
		    return KSimGetSubscriberIdApiName;
		case MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId:
		    return KSimAppendApnNameApiName;
		case MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId:
		    return KSimGetActiveIccApplicationTypeApiName;
		case MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId:
		    return KSimSetIccMessageWaitingIndicatorsApiName;
		case MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId:
		    return KSimSetApnControlListServiceStatusApiName;
		case MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId:
		    return KSimGetApnNameApiName;
		case MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId:
		    return KSimSimRefreshDoneApiName;
		case MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId:
		    return KSimGetServiceTableApiName;
		case MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId:
		    return KSimGetIccMessageWaitingIndicatorsApiName;
		case MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId:
		    return KSimSimLockActivateApiName;
		case MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId:
		    return KSimSimLockDeActivateApiName;
		case MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId:
		    return KSimGetAnswerToResetApiName;
		case MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId:
		    return KSimGetSimCardReaderStatusApiName;
		case MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId:
		    return KSimGetSimAuthenticationEapSimDataApiName;
		case MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId:
		    return KSimGetSimAuthenticationEapAkaDataApiName;
		case MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId:
		    return KSimPowerSimOffApiName;
		case MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId:
		    return KSimPowerSimOnApiName;
		case MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId:
		    return KSimReadSimFileApiName;
		case MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId:
		    return KSimSendApduRequestApiName;
		case MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId:
		    return KSimSendApduRequestV2ApiName;
		case MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId:
		    return KSimSimWarmResetApiName;
		case MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId:
		    return KSimSetSimMessageStatusReadApiName;
		case KMockLtsyDispatchSmsNotifyReceiveSmsMessageIndId:
		    return KSmsNotifyReceiveSmsMessageApiName;
		case MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId:
		    return KSmsSendSatSmsApiName;
		case MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId:
		    return KSmsGetSmsStoreInfoApiName;
		case MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId:
		    return KSmsGetSmspListApiName;
		case MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId:
		    return KSmsNackSmsStoredApiName;
		case MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId:
		    return KSmsAckSmsStoredApiName;
		case MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId:
		    return KSmsResumeSmsReceptionApiName;
		case MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId:
		    return KSmsSendSmsMessageApiName;
		case MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId:
		    return KSmsSendSmsMessageNoFdnCheckApiName;
		case MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId:
		    return KSmsSetMoSmsBearerApiName;
		case MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId:
		    return KSmsStoreSmspListEntryApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyCallBarringStatusChangeIndId:
		    return KSupplementaryServicesNotifyCallBarringStatusChangeApiName;
		case KMockLtsyDispatchSupplementaryServicesReceiveUssdMessageIndId:
		    return KSupplementaryServicesReceiveUssdMessageApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyCallWaitingStatusChangeIndId:
		    return KSupplementaryServicesNotifyCallWaitingStatusChangeApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingStatusChangeIndId:
		    return KSupplementaryServicesNotifyCallForwardingStatusChangeApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyCallForwardingActiveIndId:
		    return KSupplementaryServicesNotifyCallForwardingActiveApiName;
		case KMockLtsyDispatchSupplementaryServicesGetDiagnosticOctetsIndId:
		    return KSupplementaryServicesGetDiagnosticOctetsApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyAdditionalInfoIndId:
		    return KSupplementaryServicesNotifyAdditionalInfoApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyRequestCompleteIndId:
		    return KSupplementaryServicesNotifyRequestCompleteApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventForwardModeIndId:
		    return KSupplementaryServicesNotifyNetworkEventForwardModeApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCallWaitingIndId:
		    return KSupplementaryServicesNotifyNetworkEventCallWaitingApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventHoldModeIndId:
		    return KSupplementaryServicesNotifyNetworkEventHoldModeApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventConfrenceIndId:
		    return KSupplementaryServicesNotifyNetworkEventConfrenceApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventCugIndId:
		    return KSupplementaryServicesNotifyNetworkEventCugApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventClirSuppressionIndId:
		    return KSupplementaryServicesNotifyNetworkEventClirSuppressionApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventEctCallStateIndId:
		    return KSupplementaryServicesNotifyNetworkEventEctCallStateApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifyNetworkEventIndId:
		    return KSupplementaryServicesNotifyNetworkEventApiName;
		case KMockLtsyDispatchSupplementaryServicesNotifySendNetworkServiceRequestIndId:
		    return KSupplementaryServicesNotifySendNetworkServiceRequestApiName;
		case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId:
		    return KSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiName;
		case MLtsyDispatchSupplementaryServicesGetCallWaitingStatus::KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId:
		    return KSupplementaryServicesGetCallWaitingStatusApiName;
		case MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest::KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId:
		    return KSupplementaryServicesSendNetworkServiceRequestApiName;
		case MLtsyDispatchSupplementaryServicesGetCallBarringStatus::KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId:
		    return KSupplementaryServicesGetCallBarringStatusApiName;
		case MLtsyDispatchSupplementaryServicesSetCallBarringStatus::KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId:
		    return KSupplementaryServicesSetCallBarringStatusApiName;
		case MLtsyDispatchSupplementaryServicesSetCallForwardingStatus::KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId:
		    return KSupplementaryServicesSetCallForwardingStatusApiName;
		case MLtsyDispatchSupplementaryServicesSetCallWaitingStatus::KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId:
		    return KSupplementaryServicesSetCallWaitingStatusApiName;
		case MLtsyDispatchSupplementaryServicesSetSsPassword::KLtsyDispatchSupplementaryServicesSetSsPasswordApiId:
		    return KSupplementaryServicesSetSsPasswordApiName;
		case MLtsyDispatchSupplementaryServicesGetCallForwardingStatus::KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId:
		    return KSupplementaryServicesGetCallForwardingStatusApiName;
		case MLtsyDispatchSupplementaryServicesSendUssdMessage::KLtsyDispatchSupplementaryServicesSendUssdMessageApiId:
		    return KSupplementaryServicesSendUssdMessageApiName;
		case MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck::KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId:
		    return KSupplementaryServicesSendUssdMessageNoFdnCheckApiName;
		case MLtsyDispatchSupplementaryServicesSendUssdRelease::KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId:
		    return KSupplementaryServicesSendUssdReleaseApiName;
		case MLtsyDispatchSupplementaryServicesClearBlacklist::KLtsyDispatchSupplementaryServicesClearBlacklistApiId:
		    return KSupplementaryServicesClearBlacklistApiName;

		default:
			return KNullDesC;
		} // switch (aIpc)
	} // ApiIdToName

