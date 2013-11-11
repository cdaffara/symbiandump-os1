// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Multimode ETel API v2.0
// Header file for Multimode API IPC request definitions
// This version of the file contains the reordered version of the IPC table for the purpose
// of having a more efficient policy table as part of the platform security framework.
// Previously IPCs were ordered by functional area and now they are ordered by the capability
// associated with them.
// 
//
   
/**
 @file
*/

#ifndef __ETELMMCS_H__
#define __ETELMMCS_H__

#include <et_clsvr.h>

/**
Multimode ETel - Client-Server IPC request definitions
@publishedPartner
@released
*/

#define IPC_MULTIMODE_EXT     (20000)
#define EEtelMMIpcOffset0     (IPC_MULTIMODE_EXT)
#define EEtelMMIpcOffset1     (IPC_MULTIMODE_EXT + 1000)
#define EEtelMMIpcOffset2     (IPC_MULTIMODE_EXT + 2000)
#define EEtelMMIpcOffset3     (IPC_MULTIMODE_EXT + 3000)
#define EEtelMMIpcOffset4     (IPC_MULTIMODE_EXT + 4000)
#define EEtelMMIpcOffset5     (IPC_MULTIMODE_EXT + 5000)
#define EEtelMMIpcOffset6     (IPC_MULTIMODE_EXT + 6000)
#define EEtelMMIpcOffset7     (IPC_MULTIMODE_EXT + 7000)
#define EEtelMMIpcOffset8     (IPC_MULTIMODE_EXT + 8000)
#define EEtelMMIpcOffset9     (IPC_MULTIMODE_EXT + 9000)
#define EEtelMMIpcOffset10    (IPC_MULTIMODE_EXT + 10000)
#define EEtelMMIpcOffset11    (IPC_MULTIMODE_EXT + 11000)
#define EEtelMMIpcOffset12    (IPC_MULTIMODE_EXT + 12000)
#define EEtelMMIpcOffset13    (IPC_MULTIMODE_EXT + 13000)
#define EEtelMMIpcOffset14    (IPC_MULTIMODE_EXT + 14000)
#define EEtelMMIpcOffset15    (IPC_MULTIMODE_EXT + 2012)
#define EEtelMMIpcOffset16    (IPC_MULTIMODE_EXT + 2013)
#define EEtelMMIpcOffset17    (IPC_MULTIMODE_EXT + 2004)
#define EEtelMMIpcOffset18    (IPC_MULTIMODE_EXT + 2005)
#define EEtelMMIpcOffset19    (IPC_MULTIMODE_EXT + 2009)
#define EEtelMMIpcOffset20    (IPC_MULTIMODE_EXT + 2010)
#define EEtelMMIpcOffset21    (IPC_MULTIMODE_EXT + 2016)
#define EEtelMMIpcOffset22    (IPC_MULTIMODE_EXT + 2017)
#define EEtelMMIpcOffset23    (IPC_MULTIMODE_EXT + 4011)
#define EEtelMMIpcOffset24    (IPC_MULTIMODE_EXT + 4012)
#define EEtelMMIpcOffset25    (IPC_MULTIMODE_EXT + 4015)
#define EEtelMMIpcOffset26    (IPC_MULTIMODE_EXT + 4016)
#define EEtelMMIpcOffset27    (IPC_MULTIMODE_EXT + 15000)
#define EEtelMMIpcOffset28    (IPC_MULTIMODE_EXT + 16000)
#define EEtelMMIpcOffset29    (IPC_MULTIMODE_EXT + 17000)
#define EEtelMMIpcOffset30    (IPC_MULTIMODE_EXT + 18000)



//
// None
//
enum
	{
	EMobileBroadcastMessagingGetCaps = EEtelMMIpcOffset0,                 // 20000
	EMobileBroadcastMessagingNotifyIdListChange,                          // 20001
	EMobileBroadcastMessagingReceiveMessage,                              // 20002
	EMobileCallGetCurrentHscsdInfo,                                       // 20003
	EMobileCallGetMobileCallCaps,                                         // 20004
	EMobileCallGetMobileCallStatus,                                       // 20005
	EMobileCallGetMobileDataCallCaps,                                     // 20006
	EMobileCallGetUUSCaps,                                                // 20007
	EMobileCallNotifyAlternatingCallSwitch,                               // 20008
	EMobileCallNotifyAudioToneEvent,                                      // 20009
	EMobileCallNotifyCallEvent,                                           // 20010
	EMobileCallNotifyHscsdInfoChange,                                     // 20011
	EMobileCallNotifyMobileCallCapsChange,                                // 20012
	EMobileCallNotifyMobileCallStatusChange,                              // 20013
	EMobileCallNotifyMobileDataCallCapsChange,                            // 20014
	EMobileCallNotifyUUSCapsChange,                                       // 20015
	EMobileCallNotifyVoiceFallback,                                       // 20016
	EMobileConferenceCallEnumerateCalls,                                  // 20017
	EMobileConferenceCallGetCaps,                                         // 20018
	EMobileConferenceCallGetConferenceStatus,                             // 20019
	EMobileConferenceCallNotifyCapsChange,                                // 20020
	EMobileConferenceCallNotifyConferenceEvent,                           // 20021
	EMobileConferenceCallNotifyConferenceStatusChange,                    // 20022
	EMobileLineGetMobileLineStatus,                                       // 20023
	EMobileLineNotifyMobileLineStatusChange,                              // 20024
	EMobilePhoneEnumerateUSimApplications,                                // 20025
	EMobilePhoneGetALSLine,                                               // 20026
	EMobilePhoneGetAlternatingCallCaps,                                   // 20027
	EMobilePhoneGetAlternatingCallMode,                                   // 20028
	EMobilePhoneGetBatteryCaps,                                           // 20029
	EMobilePhoneGetBatteryInfo,                                           // 20030
	EMobilePhoneGetCCBSStatus,                                            // 20031
	EMobilePhoneGetCallForwardingIndicator,                               // 20032
	EMobilePhoneGetCallServiceCaps,                                       // 20033
	EMobilePhoneGetCipheringIndicatorStatus,                              // 20034
	EMobilePhoneGetCompMethodName,                                        // 20035
	EMobilePhoneGetCostCaps,                                              // 20036
	EMobilePhoneGetCurrentMode,                                           // 20037
	EMobilePhoneGetCustomerServiceProfile,                                // 20038
	EMobilePhoneGetDTMFCaps,                                              // 20039
	EMobilePhoneGetFdnStatus,                                             // 20040
	EMobilePhoneGetHomeNetworkSearchPeriod,                               // 20041
	EMobilePhoneGetIccAccessCaps,                                         // 20042
	EMobilePhoneGetIdentityCaps,                                          // 20043
	EMobilePhoneGetIdentityServiceStatus,                                 // 20044
	EMobilePhoneGetIncomingCallType,                                      // 20045
	EMobilePhoneGetIndicator,                                             // 20046
	EMobilePhoneGetIndicatorCaps,                                         // 20047
	EMobilePhoneGetLastUsedAccessTechnology,                              // 20048
	EMobilePhoneGetMulticallParams,                                       // 20049
	EMobilePhoneGetMultimediaCallPreference,                              // 20050
	EMobilePhoneGetMultimodeCaps,                                         // 20051
	EMobilePhoneGetNetworkCaps,                                           // 20052
	EMobilePhoneGetNetworkInvScanSetting,                                 // 20053
	EMobilePhoneGetNetworkRegistrationStatus,                             // 20054
	EMobilePhoneGetNetworkSecurityLevel,                                  // 20055
	EMobilePhoneGetPersonalisationCaps,                                   // 20056
	EMobilePhoneGetSecurityCaps,                                          // 20057
	EMobilePhoneGetServiceTable,                                          // 20058
	EMobilePhoneGetSignalCaps,                                            // 20059
	EMobilePhoneGetSignalStrength,                                        // 20060
	EMobilePhoneGetUSimApplicationsInfo,                                  // 20061
	EMobilePhoneGetUSimAppsSelectionMode,                                 // 20062
	EMobilePhoneGetUUSSetting,                                            // 20063
	EMobilePhoneNotifyALSLineChange,                                      // 20064
	EMobilePhoneNotifyAlternatingCallCapsChange,                          // 20065
	EMobilePhoneNotifyAlternatingCallModeChange,                          // 20066
	EMobilePhoneNotifyBatteryInfoChange,                                  // 20067
	EMobilePhoneNotifyCCBSRecall,                                         // 20068
	EMobilePhoneNotifyCCBSStatusChange,                                   // 20069
	EMobilePhoneNotifyCallBarringStatusChange,                            // 20070
	EMobilePhoneNotifyCallForwardingActive,                               // 20071
	EMobilePhoneNotifyCallForwardingStatusChange,                         // 20072
	EMobilePhoneNotifyCallServiceCapsChange,                              // 20073
	EMobilePhoneNotifyCallWaitingStatusChange,                            // 20074
	EMobilePhoneNotifyCipheringIndicatorStatus,                           // 20075
	EMobilePhoneNotifyCostCapsChange,                                     // 20076
	EMobilePhoneNotifyDTMFCapsChange,                                     // 20077
	EMobilePhoneNotifyDTMFEvent,                                          // 20078
	EMobilePhoneNotifyFdnStatusChange,                                    // 20079
	EMobilePhoneNotifyIccAccessCapsChange,                                // 20080
	EMobilePhoneNotifyIdentityServiceStatus,                              // 20081
	EMobilePhoneNotifyIdentitySuppressionRejected,                        // 20082
	EMobilePhoneNotifyIncomingCallTypeChange,                             // 20083
	EMobilePhoneNotifyIndicatorChange,                                    // 20084
	EMobilePhoneNotifyMessageWaiting,                                     // 20085
	EMobilePhoneNotifyModeChange,                                         // 20086
	EMobilePhoneNotifyMulticallIndicatorChange,                           // 20087
	EMobilePhoneNotifyMulticallParamsChange,                              // 20088
	EMobilePhoneNotifyMultimediaCallPreferenceChange,                     // 20089
	EMobilePhoneNotifyNetworkInvScanChange,                               // 20090
	EMobilePhoneNotifyNetworkInvScanEvent,                                // 20091
	EMobilePhoneNotifyNetworkRegistrationStatusChange,                    // 20092
	EMobilePhoneNotifyNetworkSecurityLevelChange,                         // 20093
	EMobilePhoneNotifySecurityCapsChange,                                 // 20094
	EMobilePhoneNotifySecurityEvent,                                      // 20095
	EMobilePhoneNotifySendNetworkServiceRequest,                          // 20096
	EMobilePhoneNotifySignalStrengthChange,                               // 20097
	EMobilePhoneNotifyStorePreferredNetworksListChange,                   // 20098
	EMobilePhoneNotifyUSimApplicationsInfoChange,                         // 20099
	EMobilePhoneNotifyUSimAppsSelectionModeChange,                        // 20100
	EMobilePhoneNotifyUUSSettingChange,                                   // 20101
	EMobileSmsMessagingEnumerateMessageStores,                            // 20102
	EMobileSmsMessagingGetCaps,                                           // 20103
	EMobileSmsMessagingGetMessageStoreInfo,                               // 20104
	EMobileSmsMessagingGetMoSmsBearer,                                    // 20105
	EMobileSmsMessagingGetReceiveMode,                                    // 20106
	EMobileSmsMessagingNotifyMoSmsBearerChange,                           // 20107
	EMobileSmsMessagingNotifyReceiveModeChange,                           // 20108
	EMobileSmsMessagingNotifySmspListChange,                              // 20109
	EMobileUssdMessagingGetCaps,                                          // 20110
	EMobileUssdMessagingNotifyNetworkRelease,                             // 20111
	EMobilePhoneNotifyImsAuthorizationInfoChanged,						  // 20112
	EMobilePhoneEnumerateSmartCardApplications,			      			  // 20113
	EMobilePhoneGetSmartCardApplicationInfo,                              // 20114
	EMobilePhoneNotifySmartCardApplicationInfoChange,                     // 20115
	EMobilePhoneGetCurrentActiveUSimApplication,                          // 20116
	EMobilePhoneNotifyAllSendNetworkServiceRequest,	                      // 20117
	EMobilePhoneNotifyPreferredWlanSIDListChange					      // 20118
	};

enum
	{
	EMobileBroadcastMessagingNotifyIdListChangeCancel = EMobileBroadcastMessagingNotifyIdListChange + EMobileCancelOffset,                     // 20501
	EMobileBroadcastMessagingReceiveMessageCancel = EMobileBroadcastMessagingReceiveMessage + EMobileCancelOffset,                             // 20502
	EMobileCallNotifyAlternatingCallSwitchCancel = EMobileCallNotifyAlternatingCallSwitch + EMobileCancelOffset,                               // 20508
	EMobileCallNotifyAudioToneEventCancel = EMobileCallNotifyAudioToneEvent + EMobileCancelOffset,                                             // 20509
	EMobileCallNotifyCallEventCancel = EMobileCallNotifyCallEvent + EMobileCancelOffset,                                                       // 20510
	EMobileCallNotifyHscsdInfoChangeCancel = EMobileCallNotifyHscsdInfoChange + EMobileCancelOffset,                                           // 20511
	EMobileCallNotifyMobileCallCapsChangeCancel = EMobileCallNotifyMobileCallCapsChange + EMobileCancelOffset,                                 // 20512
	EMobileCallNotifyMobileCallStatusChangeCancel = EMobileCallNotifyMobileCallStatusChange + EMobileCancelOffset,                             // 20513
	EMobileCallNotifyMobileDataCallCapsChangeCancel = EMobileCallNotifyMobileDataCallCapsChange + EMobileCancelOffset,                         // 20514
	EMobileCallNotifyUUSCapsChangeCancel = EMobileCallNotifyUUSCapsChange + EMobileCancelOffset,                                               // 20515
	EMobileCallNotifyVoiceFallbackCancel = EMobileCallNotifyVoiceFallback + EMobileCancelOffset,                                               // 20516
	EMobileConferenceCallNotifyCapsChangeCancel = EMobileConferenceCallNotifyCapsChange + EMobileCancelOffset,                                 // 20520
	EMobileConferenceCallNotifyConferenceEventCancel = EMobileConferenceCallNotifyConferenceEvent + EMobileCancelOffset,                       // 20521
	EMobileConferenceCallNotifyConferenceStatusChangeCancel = EMobileConferenceCallNotifyConferenceStatusChange + EMobileCancelOffset,         // 20522
	EMobileLineNotifyMobileLineStatusChangeCancel = EMobileLineNotifyMobileLineStatusChange + EMobileCancelOffset,                             // 20524
	EMobilePhoneEnumerateUSimApplicationsCancel = EMobilePhoneEnumerateUSimApplications + EMobileCancelOffset,                                 // 20525
	EMobilePhoneGetBatteryInfoCancel = EMobilePhoneGetBatteryInfo + EMobileCancelOffset,                                                       // 20530
	EMobilePhoneGetCCBSStatusCancel = EMobilePhoneGetCCBSStatus + EMobileCancelOffset,                                                         // 20531
	EMobilePhoneGetCallForwardingIndicatorCancel = EMobilePhoneGetCallForwardingIndicator + EMobileCancelOffset,                               // 20532
	EMobilePhoneGetCipheringIndicatorStatusCancel = EMobilePhoneGetCipheringIndicatorStatus + EMobileCancelOffset,                             // 20534
	EMobilePhoneGetCompMethodNameCancel = EMobilePhoneGetCompMethodName + EMobileCancelOffset,                                                 // 20535
	EMobilePhoneGetCustomerServiceProfileCancel = EMobilePhoneGetCustomerServiceProfile + EMobileCancelOffset,                                 // 20538
	EMobilePhoneGetFdnStatusCancel = EMobilePhoneGetFdnStatus + EMobileCancelOffset,                                                           // 20540
	EMobilePhoneGetHomeNetworkSearchPeriodCancel = EMobilePhoneGetHomeNetworkSearchPeriod + EMobileCancelOffset,                               // 20541
	EMobilePhoneGetIdentityServiceStatusCancel = EMobilePhoneGetIdentityServiceStatus + EMobileCancelOffset,                                   // 20544
	EMobilePhoneGetIncomingCallTypeCancel = EMobilePhoneGetIncomingCallType + EMobileCancelOffset,                                             // 20545
	EMobilePhoneGetIndicatorCancel = EMobilePhoneGetIndicator + EMobileCancelOffset,                                                           // 20546
	EMobilePhoneGetLastUsedAccessTechnologyCancel = EMobilePhoneGetLastUsedAccessTechnology + EMobileCancelOffset,                             // 20548
	EMobilePhoneGetMulticallParamsCancel = EMobilePhoneGetMulticallParams + EMobileCancelOffset,                                               // 20549
	EMobilePhoneGetMultimediaCallPreferenceCancel = EMobilePhoneGetMultimediaCallPreference + EMobileCancelOffset,                             // 20550
	EMobilePhoneGetNetworkRegistrationStatusCancel = EMobilePhoneGetNetworkRegistrationStatus + EMobileCancelOffset,                           // 20554
	EMobilePhoneGetNetworkSecurityLevelCancel = EMobilePhoneGetNetworkSecurityLevel + EMobileCancelOffset,                                     // 20555
	EMobilePhoneGetPersonalisationCapsCancel = EMobilePhoneGetPersonalisationCaps + EMobileCancelOffset,                                       // 20556
	EMobilePhoneGetServiceTableCancel = EMobilePhoneGetServiceTable + EMobileCancelOffset,                                                     // 20558
	EMobilePhoneGetSignalStrengthCancel = EMobilePhoneGetSignalStrength + EMobileCancelOffset,                                                 // 20560
	EMobilePhoneGetUSimApplicationsInfoCancel = EMobilePhoneGetUSimApplicationsInfo + EMobileCancelOffset,                                     // 20561
	EMobilePhoneGetUSimAppsSelectionModeCancel = EMobilePhoneGetUSimAppsSelectionMode + EMobileCancelOffset,                                   // 20562
	EMobilePhoneGetUUSSettingCancel = EMobilePhoneGetUUSSetting + EMobileCancelOffset,                                                         // 20563
	EMobilePhoneNotifyALSLineChangeCancel = EMobilePhoneNotifyALSLineChange + EMobileCancelOffset,                                             // 20564
	EMobilePhoneNotifyAlternatingCallCapsChangeCancel = EMobilePhoneNotifyAlternatingCallCapsChange + EMobileCancelOffset,                     // 20565
	EMobilePhoneNotifyAlternatingCallModeChangeCancel = EMobilePhoneNotifyAlternatingCallModeChange + EMobileCancelOffset,                     // 20566
	EMobilePhoneNotifyBatteryInfoChangeCancel = EMobilePhoneNotifyBatteryInfoChange + EMobileCancelOffset,                                     // 20567
	EMobilePhoneNotifyCCBSRecallCancel = EMobilePhoneNotifyCCBSRecall + EMobileCancelOffset,                                                   // 20568
	EMobilePhoneNotifyCCBSStatusChangeCancel = EMobilePhoneNotifyCCBSStatusChange + EMobileCancelOffset,                                       // 20569
	EMobilePhoneNotifyCallBarringStatusChangeCancel = EMobilePhoneNotifyCallBarringStatusChange + EMobileCancelOffset,                         // 20570
	EMobilePhoneNotifyCallForwardingActiveCancel = EMobilePhoneNotifyCallForwardingActive + EMobileCancelOffset,                               // 20571
	EMobilePhoneNotifyCallForwardingStatusChangeCancel = EMobilePhoneNotifyCallForwardingStatusChange + EMobileCancelOffset,                   // 20572
	EMobilePhoneNotifyCallServiceCapsChangeCancel = EMobilePhoneNotifyCallServiceCapsChange + EMobileCancelOffset,                             // 20573
	EMobilePhoneNotifyCallWaitingStatusChangeCancel = EMobilePhoneNotifyCallWaitingStatusChange + EMobileCancelOffset,                         // 20574
	EMobilePhoneNotifyCipheringIndicatorStatusCancel = EMobilePhoneNotifyCipheringIndicatorStatus + EMobileCancelOffset,                       // 20575
	EMobilePhoneNotifyCostCapsChangeCancel = EMobilePhoneNotifyCostCapsChange + EMobileCancelOffset,                                           // 20576
	EMobilePhoneNotifyDTMFCapsChangeCancel = EMobilePhoneNotifyDTMFCapsChange + EMobileCancelOffset,                                           // 20577
	EMobilePhoneNotifyDTMFEventCancel = EMobilePhoneNotifyDTMFEvent + EMobileCancelOffset,                                                     // 20578
	EMobilePhoneNotifyFdnStatusChangeCancel = EMobilePhoneNotifyFdnStatusChange + EMobileCancelOffset,                                         // 20579
	EMobilePhoneNotifyIccAccessCapsChangeCancel = EMobilePhoneNotifyIccAccessCapsChange + EMobileCancelOffset,                                 // 20580
	EMobilePhoneNotifyIdentityServiceStatusCancel = EMobilePhoneNotifyIdentityServiceStatus + EMobileCancelOffset,                             // 20581
	EMobilePhoneNotifyIdentitySuppressionRejectedCancel = EMobilePhoneNotifyIdentitySuppressionRejected + EMobileCancelOffset,                 // 20582
	EMobilePhoneNotifyIncomingCallTypeChangeCancel = EMobilePhoneNotifyIncomingCallTypeChange + EMobileCancelOffset,                           // 20583
	EMobilePhoneNotifyIndicatorChangeCancel = EMobilePhoneNotifyIndicatorChange + EMobileCancelOffset,                                         // 20584
	EMobilePhoneNotifyMessageWaitingCancel = EMobilePhoneNotifyMessageWaiting + EMobileCancelOffset,                                           // 20585
	EMobilePhoneNotifyModeChangeCancel = EMobilePhoneNotifyModeChange + EMobileCancelOffset,                                                   // 20586
	EMobilePhoneNotifyMulticallIndicatorChangeCancel = EMobilePhoneNotifyMulticallIndicatorChange + EMobileCancelOffset,                       // 20587
	EMobilePhoneNotifyMulticallParamsChangeCancel = EMobilePhoneNotifyMulticallParamsChange + EMobileCancelOffset,                             // 20588
	EMobilePhoneNotifyMultimediaCallPreferenceChangeCancel = EMobilePhoneNotifyMultimediaCallPreferenceChange + EMobileCancelOffset,           // 20589
	EMobilePhoneNotifyNetworkInvScanChangeCancel = EMobilePhoneNotifyNetworkInvScanChange + EMobileCancelOffset,                               // 20590
	EMobilePhoneNotifyNetworkInvScanEventCancel = EMobilePhoneNotifyNetworkInvScanEvent + EMobileCancelOffset,                                 // 20591
	EMobilePhoneNotifyNetworkRegistrationStatusChangeCancel = EMobilePhoneNotifyNetworkRegistrationStatusChange + EMobileCancelOffset,         // 20592
	EMobilePhoneNotifyNetworkSecurityLevelChangeCancel = EMobilePhoneNotifyNetworkSecurityLevelChange + EMobileCancelOffset,                   // 20593
	EMobilePhoneNotifySecurityCapsChangeCancel = EMobilePhoneNotifySecurityCapsChange + EMobileCancelOffset,                                   // 20594
	EMobilePhoneNotifySecurityEventCancel = EMobilePhoneNotifySecurityEvent + EMobileCancelOffset,                                             // 20595
	EMobilePhoneNotifySendNetworkServiceRequestCancel = EMobilePhoneNotifySendNetworkServiceRequest + EMobileCancelOffset,                     // 20596
	EMobilePhoneNotifySignalStrengthChangeCancel = EMobilePhoneNotifySignalStrengthChange + EMobileCancelOffset,                               // 20597
	EMobilePhoneNotifyStorePreferredNetworksListChangeCancel = EMobilePhoneNotifyStorePreferredNetworksListChange + EMobileCancelOffset,       // 20598
	EMobilePhoneNotifyUSimApplicationsInfoChangeCancel = EMobilePhoneNotifyUSimApplicationsInfoChange + EMobileCancelOffset,                   // 20599
	EMobilePhoneNotifyUSimAppsSelectionModeChangeCancel = EMobilePhoneNotifyUSimAppsSelectionModeChange + EMobileCancelOffset,                 // 20600
	EMobilePhoneNotifyUUSSettingChangeCancel = EMobilePhoneNotifyUUSSettingChange + EMobileCancelOffset,                                       // 20601
	EMobileSmsMessagingGetMessageStoreInfoCancel = EMobileSmsMessagingGetMessageStoreInfo + EMobileCancelOffset,                               // 20604
	EMobileSmsMessagingNotifyMoSmsBearerChangeCancel = EMobileSmsMessagingNotifyMoSmsBearerChange + EMobileCancelOffset,                       // 20607
	EMobileSmsMessagingNotifyReceiveModeChangeCancel = EMobileSmsMessagingNotifyReceiveModeChange + EMobileCancelOffset,                       // 20608
	EMobileSmsMessagingNotifySmspListChangeCancel = EMobileSmsMessagingNotifySmspListChange + EMobileCancelOffset,                             // 20609
	EMobileUssdMessagingNotifyNetworkReleaseCancel = EMobileUssdMessagingNotifyNetworkRelease + EMobileCancelOffset,                           // 20611
	EMobilePhoneNotifyImsAuthorizationInfoChangedCancel = EMobilePhoneNotifyImsAuthorizationInfoChanged + EMobileCancelOffset,                 // 20612
	EMobilePhoneEnumerateSmartCardApplicationsCancel = EMobilePhoneEnumerateSmartCardApplications + EMobileCancelOffset,                       // 20613
	EMobilePhoneGetSmartCardApplicationInfoCancel = EMobilePhoneGetSmartCardApplicationInfo + EMobileCancelOffset,                             // 20614
	EMobilePhoneNotifySmartCardApplicationInfoChangeCancel = EMobilePhoneNotifySmartCardApplicationInfoChange + EMobileCancelOffset,           // 20615
	EMobilePhoneGetCurrentActiveUSimApplicationCancel = EMobilePhoneGetCurrentActiveUSimApplication + EMobileCancelOffset,                     // 20616
	EMobilePhoneNotifyAllSendNetworkServiceRequestCancel = EMobilePhoneNotifyAllSendNetworkServiceRequest + EMobileCancelOffset,               // 20617
	EMobilePhoneNotifyPreferredWlanSIDListChangeCancel = EMobilePhoneNotifyPreferredWlanSIDListChange + EMobileCancelOffset					   // 20618
	};

//
// NetworkServices
//
enum
	{
	EMobileCallActivateCCBS = EEtelMMIpcOffset1,                          // 21000
	EMobileCallAnswerISV,                                                 // 21001
	EMobileCallAnswerMultimediaCallAsVoice,                               // 21002
	EMobileCallDeflect,                                                   // 21003
	EMobileCallDialISV,                                                   // 21004
	EMobileCallGoOneToOne,                                                // 21005
	EMobileCallHold,                                                      // 21006
	EMobileCallNotifyPrivacyConfirmation,                                 // 21007
	EMobileCallNotifyTrafficChannelConfirmation,                          // 21008
	EMobileCallRejectCCBS,                                                // 21009
	EMobileCallResume,                                                    // 21010
	EMobileCallSetDynamicHscsdParams,                                     // 21011
	EMobileCallSetPrivacy,                                                // 21012
	EMobileCallSetTrafficChannel,                                         // 21013
	EMobileCallSwap,                                                      // 21014
	EMobileCallSwitchAlternatingCall,                                     // 21015
	EMobileCallTransfer,                                                  // 21016
	EMobileConferenceCallAddCall,                                         // 21017
	EMobileConferenceCallCreateConference,                                // 21018
	EMobileConferenceCallHangUp,                                          // 21019
	EMobileConferenceCallSwap,                                            // 21020
	EMobilePhoneAcceptCCBSRecall,                                         // 21021
	EMobilePhoneContinueDTMFStringSending,                                // 21022
	EMobilePhoneNotifyStopInDTMFString,                                   // 21023
	EMobilePhoneReadDTMFTones,                                            // 21024
	EMobilePhoneRefuseCCBSRecall,                                         // 21025
	EMobilePhoneSendDTMFTones,                                            // 21026
	EMobilePhoneStartDTMFTone,                                            // 21027
	EMobilePhoneStopDTMFTone,                                             // 21028
	EMobilePhoneTerminateAllCalls,                                        // 21029
	EMobilePhoneTerminateActiveCalls                                      // 21030
	};

enum
	{
	EMobileCallActivateCCBSCancel = EMobileCallActivateCCBS + EMobileCancelOffset,                                                             // 21500
	EMobileCallAnswerISVCancel = EMobileCallAnswerISV + EMobileCancelOffset,                                                                   // 21501
	EMobileCallAnswerMultimediaCallAsVoiceCancel = EMobileCallAnswerMultimediaCallAsVoice + EMobileCancelOffset,                               // 21502
	EMobileCallDeflectCancel = EMobileCallDeflect + EMobileCancelOffset,                                                                       // 21503
	EMobileCallDialISVCancel = EMobileCallDialISV + EMobileCancelOffset,                                                                       // 21504
	EMobileCallGoOneToOneCancel = EMobileCallGoOneToOne + EMobileCancelOffset,                                                                 // 21505
	EMobileCallHoldCancel = EMobileCallHold + EMobileCancelOffset,                                                                             // 21506
	EMobileCallNotifyPrivacyConfirmationCancel = EMobileCallNotifyPrivacyConfirmation + EMobileCancelOffset,                                   // 21507
	EMobileCallNotifyTrafficChannelConfirmationCancel = EMobileCallNotifyTrafficChannelConfirmation + EMobileCancelOffset,                     // 21508
	EMobileCallResumeCancel = EMobileCallResume + EMobileCancelOffset,                                                                         // 21510
	EMobileCallSetDynamicHscsdParamsCancel = EMobileCallSetDynamicHscsdParams + EMobileCancelOffset,                                           // 21511
	EMobileCallSwapCancel = EMobileCallSwap + EMobileCancelOffset,                                                                             // 21514
	EMobileCallSwitchAlternatingCallCancel = EMobileCallSwitchAlternatingCall + EMobileCancelOffset,                                           // 21515
	EMobileCallTransferCancel = EMobileCallTransfer + EMobileCancelOffset,                                                                     // 21516
	EMobileConferenceCallAddCallCancel = EMobileConferenceCallAddCall + EMobileCancelOffset,                                                   // 21517
	EMobileConferenceCallCreateConferenceCancel = EMobileConferenceCallCreateConference + EMobileCancelOffset,                                 // 21518
	EMobileConferenceCallHangUpCancel = EMobileConferenceCallHangUp + EMobileCancelOffset,                                                     // 21519
	EMobileConferenceCallSwapCancel = EMobileConferenceCallSwap + EMobileCancelOffset,                                                         // 21520
	EMobilePhoneAcceptCCBSRecallCancel = EMobilePhoneAcceptCCBSRecall + EMobileCancelOffset,                                                   // 21521
	EMobilePhoneNotifyStopInDTMFStringCancel = EMobilePhoneNotifyStopInDTMFString + EMobileCancelOffset,                                       // 21523
	EMobilePhoneReadDTMFTonesCancel = EMobilePhoneReadDTMFTones + EMobileCancelOffset,                                                         // 21524
	EMobilePhoneSendDTMFTonesCancel = EMobilePhoneSendDTMFTones + EMobileCancelOffset,                                                         // 21526
	EMobilePhoneTerminateAllCallsCancel = EMobilePhoneTerminateAllCalls + EMobileCancelOffset,                                                 // 21529
	EMobilePhoneTerminateActiveCallsCancel = EMobilePhoneTerminateActiveCalls + EMobileCancelOffset                                            // 21530
	};

//
// ReadDeviceData
//
enum
	{
	EMobileConferenceCallGetMobileCallInfo = EEtelMMIpcOffset2,           // 22000
	EMobilePhoneEnumerateAPNEntries,                                      // 22001
	EMobilePhoneGetAPNname,                                               // 22002
	EMobilePhoneGetFeatureCode                                            // 22003
	};

//
// None
//
enum
	{
	EMobilePhoneGetHomeNetwork = EEtelMMIpcOffset17                       // 22004
	};

//
// ReadDeviceData
//
enum
	{
	EMobilePhoneGetIccMessageWaitingIndicators = EEtelMMIpcOffset18,      // 22005
	EMobilePhoneGetLockInfo,                                              // 22006
	EMobilePhoneGetMmsConfig,                                             // 22007
	EMobilePhoneGetNITZInfo                                               // 22008
	};

//
// None
//
enum
	{
	EMobilePhoneGetNetworkName = EEtelMMIpcOffset19                       // 22009
	};

//
// ReadDeviceData
//
enum
	{
	EMobilePhoneGetNetworkSelectionSetting = EEtelMMIpcOffset20,          // 22010
	EMobilePhoneGetPersonalisationStatus                                  // 22011
	};

//
// None
//
enum
	{
	EMobilePhoneGetPhoneId = EEtelMMIpcOffset15                           // 22012
	};

//
// ReadDeviceData
//
enum
	{
	EMobilePhoneGetPhoneStoreInfo = EEtelMMIpcOffset16,                   // 22013
	EMobilePhoneGetPreferredNetworksPhase1,                               // 22014
	EMobilePhoneGetPreferredNetworksPhase2                                // 22015
	};

//
// None
//
enum
	{
	EMobilePhoneGetServiceProviderName = EEtelMMIpcOffset21               // 22016
	};

//
// ReadDeviceData
//
enum
	{
	EMobilePhoneGetSubscriberId = EEtelMMIpcOffset22,                     // 22017
	EMobilePhoneNotifyIccMessageWaitingIndicatorsChange,                  // 22018
	EMobilePhoneNotifyLockInfoChange,                                     // 22019
	EMobilePhoneNotifyMmsConfig,                                          // 22020
	EMobilePhoneNotifyMmsUpdate,                                          // 22021
	EMobilePhoneNotifyNITZInfoChange,                                     // 22022
	EMobilePhoneNotifyNetworkSelectionSettingChange,                      // 22023
	EMobilePhoneStoreReadAllPhase1,                                       // 22024
	EMobilePhoneStoreReadAllPhase2,                                       // 22025
	EMobilePhoneStoreReadAllV4Phase1,                                     // 22026
	EMobilePhoneStoreReadAllV4Phase2,                                     // 22027
	EMobileSmsMessagingGetSmspListPhase1,                                 // 22028
	EMobileSmsMessagingGetSmspListPhase2,                                 // 22029
	EMobilePhoneAuthorizationInfoPhase1,                                  // 22030
	EMobilePhoneAuthorizationInfoPhase2,                                  // 22031
	EMobilePhoneIMSAuthenticate,                                          // 22032
	EMobilePhoneGetScFileInfo,                                            // 22033
	EMobilePhoneReadScFile,                                               // 22034
	EMobilePhoneGetIccIdentity,                                           // 22035
	EMobilePhoneGetSecurityCodeInfo,                                      // 22036
	EMobilePhoneNotifySecurityCodeInfoChange,							  // 22037
	EMobilePhoneSmartCardAuthenticate,                                    // 22038
	EMobileSmartCardEapGetAuthenticationStatus,                           // 22039
	EMobileSmartCardEapGetUserIdentity,                                   // 22040
	EMobileSmartCardEapGetEapKey,                                         // 22041
	EMobileSmartCardEapNotifyEapMethodAccessStatusChange,                 // 22042
	EMobileSmartCardEapAuthenticationPhase1,                              // 22043
	EMobileSmartCardEapAuthenticationPhase2,                              // 22044
	EMobileSmartCardEapReleaseEapMethod,                                  // 22045
	EMobileSmartCardEapGetEapMethodAccessStatus,                          // 22046
	EMobilePhoneNotifyAPNListChanged,									  // 22047
	EMobilePhoneGetAPNControlListServiceStatus,							  // 22048
	EMobilePhoneNotifyAPNControlListServiceStatusChange,				  // 22049
	EMobilePhoneGetAuthenticationParams,								  // 22050
	EMobilePhoneNotifyAuthenticateDataChange,							  // 22051
	EMobilePhoneAuthenticationListPhase1,							      // 22052
	EMobilePhoneAuthenticationListPhase2,								  // 22053
	EMobilePhoneNotifyWlanDataChange,									  // 22054
	EMobilePhoneGetWlanData,											  // 22055
	EMobilePhoneGetPreferredWlanSIDsPhase1,								  // 22056
	EMobilePhoneGetPreferredWlanSIDsPhase2								  // 22057
	};

enum
	{
	EMobilePhoneEnumerateAPNEntriesCancel = EMobilePhoneEnumerateAPNEntries + EMobileCancelOffset,                                             // 22501
	EMobilePhoneGetAPNnameCancel = EMobilePhoneGetAPNname + EMobileCancelOffset,                                                               // 22502
	EMobilePhoneGetFeatureCodeCancel = EMobilePhoneGetFeatureCode + EMobileCancelOffset,                                                       // 22503
	EMobilePhoneGetHomeNetworkCancel = EMobilePhoneGetHomeNetwork + EMobileCancelOffset,                                                       // 22504
	EMobilePhoneGetIccMessageWaitingIndicatorsCancel = EMobilePhoneGetIccMessageWaitingIndicators + EMobileCancelOffset,                       // 22505
	EMobilePhoneGetLockInfoCancel = EMobilePhoneGetLockInfo + EMobileCancelOffset,                                                             // 22506
	EMobilePhoneGetMmsConfigCancel = EMobilePhoneGetMmsConfig + EMobileCancelOffset,                                                           // 22507
	EMobilePhoneGetNetworkNameCancel = EMobilePhoneGetNetworkName + EMobileCancelOffset,                                                       // 22509
	EMobilePhoneGetPersonalisationStatusCancel = EMobilePhoneGetPersonalisationStatus + EMobileCancelOffset,                                   // 22511
	EMobilePhoneGetPhoneIdCancel = EMobilePhoneGetPhoneId + EMobileCancelOffset,                                                               // 22512
	EMobilePhoneGetPhoneStoreInfoCancel = EMobilePhoneGetPhoneStoreInfo + EMobileCancelOffset,                                                 // 22513
	EMobilePhoneGetPreferredNetworksCancel = EMobilePhoneGetPreferredNetworksPhase1 + EMobileCancelOffset,                                     // 22514
	EMobilePhoneGetServiceProviderNameCancel = EMobilePhoneGetServiceProviderName + EMobileCancelOffset,                                       // 22516
	EMobilePhoneGetSubscriberIdCancel = EMobilePhoneGetSubscriberId + EMobileCancelOffset,                                                     // 22517
	EMobilePhoneNotifyIccMessageWaitingIndicatorsChangeCancel = EMobilePhoneNotifyIccMessageWaitingIndicatorsChange + EMobileCancelOffset,     // 22518
	EMobilePhoneNotifyLockInfoChangeCancel = EMobilePhoneNotifyLockInfoChange + EMobileCancelOffset,                                           // 22519
	EMobilePhoneNotifyMmsConfigCancel = EMobilePhoneNotifyMmsConfig + EMobileCancelOffset,                                                     // 22520
	EMobilePhoneNotifyMmsUpdateCancel = EMobilePhoneNotifyMmsUpdate + EMobileCancelOffset,                                                     // 22521
	EMobilePhoneNotifyNITZInfoChangeCancel = EMobilePhoneNotifyNITZInfoChange + EMobileCancelOffset,                                           // 22522
	EMobilePhoneNotifyNetworkSelectionSettingChangeCancel = EMobilePhoneNotifyNetworkSelectionSettingChange + EMobileCancelOffset,             // 22523
	EMobilePhoneStoreReadAllCancel = EMobilePhoneStoreReadAllPhase1 + EMobileCancelOffset,                                                     // 22524
	EMobilePhoneStoreReadAllV4Cancel = EMobilePhoneStoreReadAllV4Phase1 + EMobileCancelOffset,                                                 // 22526
	EMobileSmsMessagingGetSmspListCancel = EMobileSmsMessagingGetSmspListPhase1 + EMobileCancelOffset,                                         // 22528
	EMobilePhoneAuthorizationInfoCancel = EMobilePhoneAuthorizationInfoPhase1 + EMobileCancelOffset,                                           // 22530
	EMobilePhoneIMSAuthenticateCancel = EMobilePhoneIMSAuthenticate + EMobileCancelOffset,                                                     // 22532
	EMobilePhoneGetScFileInfoCancel = EMobilePhoneGetScFileInfo + EMobileCancelOffset,                                                         // 22533
	EMobilePhoneReadScFileCancel = EMobilePhoneReadScFile + EMobileCancelOffset,                                                               // 22534
	EMobilePhoneGetIccIdentityCancel = EMobilePhoneGetIccIdentity + EMobileCancelOffset,                                                       // 22535
	EMobilePhoneGetSecurityCodeInfoCancel = EMobilePhoneGetSecurityCodeInfo + EMobileCancelOffset,                                             // 22536
	EMobilePhoneNotifySecurityCodeInfoChangeCancel = EMobilePhoneNotifySecurityCodeInfoChange + EMobileCancelOffset,                           // 22537
	EMobilePhoneSmartCardAuthenticateCancel = EMobilePhoneSmartCardAuthenticate + EMobileCancelOffset,                                         // 22538
	EMobileSmartCardEapGetAuthenticationStatusCancel = EMobileSmartCardEapGetAuthenticationStatus + EMobileCancelOffset,                       // 22539
  	EMobileSmartCardEapGetUserIdentityCancel = EMobileSmartCardEapGetUserIdentity + EMobileCancelOffset,                                       // 22540
	EMobileSmartCardEapGetEapKeyCancel = EMobileSmartCardEapGetEapKey + EMobileCancelOffset,                                                   // 22541
	EMobileSmartCardEapNotifyEapMethodAccessStatusChangeCancel = EMobileSmartCardEapNotifyEapMethodAccessStatusChange + EMobileCancelOffset,   // 22542
	EMobileSmartCardEapAuthenticationCancel = EMobileSmartCardEapAuthenticationPhase1 + EMobileCancelOffset,                                   // 22543
	EMobilePhoneNotifyAPNListChangedCancel = EMobilePhoneNotifyAPNListChanged + EMobileCancelOffset,										   // 22547
	EMobilePhoneGetAPNControlListServiceStatusCancel = EMobilePhoneGetAPNControlListServiceStatus + EMobileCancelOffset,					   // 22548
	EMobilePhoneNotifyAPNControlListServiceStatusChangeCancel = EMobilePhoneNotifyAPNControlListServiceStatusChange  + EMobileCancelOffset,	   // 22549
	EMobilePhoneGetAuthenticationParamsCancel = EMobilePhoneGetAuthenticationParams + EMobileCancelOffset,									   // 22550
	EMobilePhoneNotifyAuthenticateDataChangeCancel = EMobilePhoneNotifyAuthenticateDataChange + EMobileCancelOffset,						   // 22551
	EMobilePhoneAuthenticationListCancel = EMobilePhoneAuthenticationListPhase1 + EMobileCancelOffset,										   // 22552
	EMobilePhoneNotifyWlanDataChangeCancel = EMobilePhoneNotifyWlanDataChange + EMobileCancelOffset, 										   // 22554
	EMobilePhoneGetWlanDataCancel = EMobilePhoneGetWlanData + EMobileCancelOffset,															   // 22555
	EMobilePhoneGetPreferredWlanSIDsCancel = EMobilePhoneGetPreferredWlanSIDsPhase1	+ EMobileCancelOffset									   // 22556													   // 22554
	};

//
// WriteDeviceData
//
enum
	{
	EMobileNamStoreSetActiveNam = EEtelMMIpcOffset3,                      // 23000
	EMobileNamStoreStoreAll,                                              // 23001
	EMobileNamStoreStoreAllV4,                                            // 23002
	EMobilePhoneAbortSecurityCode,                                        // 23003
	EMobilePhoneChangeSecurityCode,                                       // 23004
	EMobilePhoneClearBlacklist,                                           // 23005
	EMobilePhoneClearCostMeter,                                           // 23006
	EMobilePhoneProgramFeatureCode,                                       // 23007
	EMobilePhoneSetALSLine,                                               // 23008
	EMobilePhoneSetAlternatingCallMode,                                   // 23009
	EMobilePhoneSetFdnSetting,                                            // 23010
	EMobilePhoneSetIccMessageWaitingIndicators,                           // 23011
	EMobilePhoneSetIdentityServiceStatus,                                 // 23012
	EMobilePhoneSetIncomingCallType,                                      // 23013
	EMobilePhoneSetLockSetting,                                           // 23014
	EMobilePhoneSetMaxCostMeter,                                          // 23015
	EMobilePhoneSetMmsUserConnParams,                                     // 23016
	EMobilePhoneSetMmsUserPreferences,                                    // 23017
	EMobilePhoneSetMulticallParams,                                       // 23018
	EMobilePhoneSetMultimediaCallPreference,                              // 23019
	EMobilePhoneSetNetworkSelectionSetting,                               // 23020
	EMobilePhoneSetPuct,                                                  // 23021
	EMobilePhoneSetUSimApplicationStatus,                                 // 23022
	EMobilePhoneSetUSimAppsSelectionMode,                                 // 23023
	EMobilePhoneStorePreferredNetworksList,                               // 23024
	EMobilePhoneVerifySecurityCode,                                       // 23025
	EMobileSmsMessagingSetMoSmsBearer,                                    // 23026
	EMobileSmsMessagingSetReceiveMode,                                    // 23027
	EMobileSmsMessagingStoreSmspList,                                     // 23028
	EMobilePhoneSetSmartCardApplicationStatus,                            // 23029
	EMobilePhoneUpdateScFile,                                             // 23030
	EMobilePhoneAppendAPNName,											  // 23031
	EMobilePhoneDeleteAPNName,											  // 23032
	EMobilePhoneSetGbaBootstrapParams,									  // 23033
	EMobilePhoneSetWlanData,											  // 23034
	EMobilePhoneStorePreferredWlanSIDList								  // 23035
	};

enum
	{
	EMobileNamStoreSetActiveNamCancel = EMobileNamStoreSetActiveNam + EMobileCancelOffset,                                                     // 23500
	EMobileNamStoreStoreAllCancel = EMobileNamStoreStoreAll + EMobileCancelOffset,                                                             // 23501
	EMobileNamStoreStoreAllV4Cancel = EMobileNamStoreStoreAllV4 + EMobileCancelOffset,                                                         // 23502
	EMobilePhoneChangeSecurityCodeCancel = EMobilePhoneChangeSecurityCode + EMobileCancelOffset,                                               // 23504
	EMobilePhoneClearBlacklistCancel = EMobilePhoneClearBlacklist + EMobileCancelOffset,                                                       // 23505
	EMobilePhoneClearCostMeterCancel = EMobilePhoneClearCostMeter + EMobileCancelOffset,                                                       // 23506
	EMobilePhoneProgramFeatureCodeCancel = EMobilePhoneProgramFeatureCode + EMobileCancelOffset,                                               // 23507
	EMobilePhoneSetALSLineCancel = EMobilePhoneSetALSLine + EMobileCancelOffset,                                                               // 23508
	EMobilePhoneSetAlternatingCallModeCancel = EMobilePhoneSetAlternatingCallMode + EMobileCancelOffset,                                       // 23509
	EMobilePhoneSetFdnSettingCancel = EMobilePhoneSetFdnSetting + EMobileCancelOffset,                                                         // 23510
	EMobilePhoneSetIccMessageWaitingIndicatorsCancel = EMobilePhoneSetIccMessageWaitingIndicators + EMobileCancelOffset,                       // 23511
	EMobilePhoneSetIdentityServiceStatusCancel = EMobilePhoneSetIdentityServiceStatus + EMobileCancelOffset,                                   // 23512
	EMobilePhoneSetIncomingCallTypeCancel = EMobilePhoneSetIncomingCallType + EMobileCancelOffset,                                             // 23513
	EMobilePhoneSetLockSettingCancel = EMobilePhoneSetLockSetting + EMobileCancelOffset,                                                       // 23514
	EMobilePhoneSetMaxCostMeterCancel = EMobilePhoneSetMaxCostMeter + EMobileCancelOffset,                                                     // 23515
	EMobilePhoneSetMmsUserConnParamsCancel = EMobilePhoneSetMmsUserConnParams + EMobileCancelOffset,                                           // 23516
	EMobilePhoneSetMmsUserPreferencesCancel = EMobilePhoneSetMmsUserPreferences + EMobileCancelOffset,                                         // 23517
	EMobilePhoneSetMulticallParamsCancel = EMobilePhoneSetMulticallParams + EMobileCancelOffset,                                               // 23518
	EMobilePhoneSetMultimediaCallPreferenceCancel = EMobilePhoneSetMultimediaCallPreference + EMobileCancelOffset,                             // 23519
	EMobilePhoneSetNetworkSelectionSettingCancel = EMobilePhoneSetNetworkSelectionSetting + EMobileCancelOffset,                               // 23520
	EMobilePhoneSetPuctCancel = EMobilePhoneSetPuct + EMobileCancelOffset,                                                                     // 23521
	EMobilePhoneSetUSimApplicationStatusCancel = EMobilePhoneSetUSimApplicationStatus + EMobileCancelOffset,                                   // 23522
	EMobilePhoneSetUSimAppsSelectionModeCancel = EMobilePhoneSetUSimAppsSelectionMode + EMobileCancelOffset,                                   // 23523
	EMobilePhoneStorePreferredNetworksListCancel = EMobilePhoneStorePreferredNetworksList + EMobileCancelOffset,                               // 23524
	EMobilePhoneVerifySecurityCodeCancel = EMobilePhoneVerifySecurityCode + EMobileCancelOffset,                                               // 23525
	EMobileSmsMessagingSetMoSmsBearerCancel = EMobileSmsMessagingSetMoSmsBearer + EMobileCancelOffset,                                         // 23526
	EMobileSmsMessagingSetReceiveModeCancel = EMobileSmsMessagingSetReceiveMode + EMobileCancelOffset,                                         // 23527
	EMobileSmsMessagingStoreSmspListCancel = EMobileSmsMessagingStoreSmspList + EMobileCancelOffset,                                           // 23528
	EMobilePhoneSetSmartCardApplicationStatusCancel = EMobilePhoneSetSmartCardApplicationStatus + EMobileCancelOffset,                         // 23529
	EMobilePhoneUpdateScFileCancel = EMobilePhoneUpdateScFile + EMobileCancelOffset,                                                           // 23530
	EMobilePhoneAppendAPNNameCancel = EMobilePhoneAppendAPNName + EMobileCancelOffset,														   // 23531
	EMobilePhoneDeleteAPNNameCancel = EMobilePhoneDeleteAPNName + EMobileCancelOffset,														   // 23532
	EMobilePhoneSetGbaBootstrapParamsCancel = EMobilePhoneSetGbaBootstrapParams + EMobileCancelOffset,										   // 23533
	EMobilePhoneSetWlanDataCancel = EMobilePhoneSetWlanData	+ EMobileCancelOffset,													           // 23534
	EMobilePhoneStorePreferredWlanSIDListCancel = EMobilePhoneStorePreferredWlanSIDList + EMobileCancelOffset								   // 23535
	};

//
// ReadUserData
//
enum
	{
	EMobileBroadcastMessagingGetFilterSetting = EEtelMMIpcOffset4,        // 24000
	EMobileBroadcastMessagingGetIdListPhase1,                             // 24001
	EMobileBroadcastMessagingGetIdListPhase2,                             // 24002
	EMobileBroadcastMessagingGetLanguageFilter,                           // 24003
	EMobileBroadcastMessagingNotifyFilterSettingChange,                   // 24004
	EMobileBroadcastMessagingNotifyLanguageFilterChange,                  // 24005
	EMobileCallGetMobileCallInfo,                                         // 24006
	EMobileCallNotifyRemotePartyInfoChange,                               // 24007
	EMobilePhoneBookStoreRead,                                            // 24008
	EMobilePhoneGetAirTimeDuration,                                       // 24009
	EMobilePhoneGetCostInfo                                               // 24010
	};

//
// None
//
enum
	{
	EMobilePhoneGetCurrentNetworkNoLocation = EEtelMMIpcOffset23          // 24011
	};

//
// ReadUserData
//
enum
	{
	EMobilePhoneGetMailboxNumbers = EEtelMMIpcOffset24,                   // 24012
	EMobilePhoneNotifyAirTimeDurationChange,                              // 24013
	EMobilePhoneNotifyCostInfoChange                                      // 24014
	};

//
// None
//
enum
	{
	EMobilePhoneNotifyCurrentNetworkNoLocationChange = EEtelMMIpcOffset25 // 24015
	};

//
// ReadUserData
//
enum
	{
	EMobilePhoneNotifyMailboxNumbersChange = EEtelMMIpcOffset26,          // 24016
	EMobilePhoneStoreGetInfo,                                             // 24017
	EMobilePhoneStoreNotifyStoreEvent,                                    // 24018
	EMobilePhoneStoreRead                                                 // 24019
	};

enum
	{
	EMobileBroadcastMessagingGetIdListCancel = EMobileBroadcastMessagingGetIdListPhase1 + EMobileCancelOffset,                                 // 24501
	EMobileBroadcastMessagingGetLanguageFilterCancel = EMobileBroadcastMessagingGetLanguageFilter + EMobileCancelOffset,                       // 24503
	EMobileBroadcastMessagingNotifyFilterSettingChangeCancel = EMobileBroadcastMessagingNotifyFilterSettingChange + EMobileCancelOffset,       // 24504
	EMobileBroadcastMessagingNotifyLanguageFilterChangeCancel = EMobileBroadcastMessagingNotifyLanguageFilterChange + EMobileCancelOffset,     // 24505
	EMobileCallGetMobileCallInfoCancel = EMobileCallGetMobileCallInfo + EMobileCancelOffset,                                                   // 24506
	EMobileCallNotifyRemotePartyInfoChangeCancel = EMobileCallNotifyRemotePartyInfoChange + EMobileCancelOffset,                               // 24507
	EMobilePhoneBookStoreReadCancel = EMobilePhoneBookStoreRead + EMobileCancelOffset,                                                         // 24508
	EMobilePhoneGetCostInfoCancel = EMobilePhoneGetCostInfo + EMobileCancelOffset,                                                             // 24510
	EMobilePhoneGetCurrentNetworkNoLocationCancel = EMobilePhoneGetCurrentNetworkNoLocation + EMobileCancelOffset,                             // 24511
	EMobilePhoneGetMailboxNumbersCancel = EMobilePhoneGetMailboxNumbers + EMobileCancelOffset,                                                 // 24512
	EMobilePhoneNotifyAirTimeDurationChangeCancel = EMobilePhoneNotifyAirTimeDurationChange + EMobileCancelOffset,                             // 24513
	EMobilePhoneNotifyCostInfoChangeCancel = EMobilePhoneNotifyCostInfoChange + EMobileCancelOffset,                                           // 24514
	EMobilePhoneNotifyCurrentNetworkNoLocationChangeCancel = EMobilePhoneNotifyCurrentNetworkNoLocationChange + EMobileCancelOffset,           // 24515
	EMobilePhoneNotifyMailboxNumbersChangeCancel = EMobilePhoneNotifyMailboxNumbersChange + EMobileCancelOffset,                               // 24516
	EMobilePhoneStoreGetInfoCancel = EMobilePhoneStoreGetInfo + EMobileCancelOffset,                                                           // 24517
	EMobilePhoneStoreNotifyStoreEventCancel = EMobilePhoneStoreNotifyStoreEvent + EMobileCancelOffset,                                         // 24518
	EMobilePhoneStoreReadCancel = EMobilePhoneStoreRead + EMobileCancelOffset                                                                  // 24519
	};

//
// WriteUserData
//
enum
	{
	EMobileBroadcastMessagingSetFilterSetting = EEtelMMIpcOffset5,        // 25000
	EMobileBroadcastMessagingSetLanguageFilter,                           // 25001
	EMobileBroadcastMessagingStoreIdList,                                 // 25002
	EMobileONStoreStoreAll,                                               // 25003
	EMobilePhoneBookStoreWrite,                                           // 25004
	EMobilePhoneStoreDelete,                                              // 25005
	EMobilePhoneStoreDeleteAll,                                           // 25006
	EMobilePhoneStoreWrite                                                // 25007
	};

enum
	{
	EMobileBroadcastMessagingSetFilterSettingCancel = EMobileBroadcastMessagingSetFilterSetting + EMobileCancelOffset,                         // 25500
	EMobileBroadcastMessagingSetLanguageFilterCancel = EMobileBroadcastMessagingSetLanguageFilter + EMobileCancelOffset,                       // 25501
	EMobileBroadcastMessagingStoreIdListCancel = EMobileBroadcastMessagingStoreIdList + EMobileCancelOffset,                                   // 25502
	EMobileONStoreStoreAllCancel = EMobileONStoreStoreAll + EMobileCancelOffset,                                                               // 25503
	EMobilePhoneBookStoreWriteCancel = EMobilePhoneBookStoreWrite + EMobileCancelOffset,                                                       // 25504
	EMobilePhoneStoreDeleteAllCancel = EMobilePhoneStoreDeleteAll + EMobileCancelOffset,                                                       // 25506
	EMobilePhoneStoreDeleteCancel = EMobilePhoneStoreDelete + EMobileCancelOffset,                                                             // 25505
	EMobilePhoneStoreWriteCancel = EMobilePhoneStoreWrite + EMobileCancelOffset                                                                // 25507
	};

//
// ReadDeviceData
//
enum
	{
	EMobilePhoneGetCurrentNetwork = EEtelMMIpcOffset6,                    // 26000
	EMobilePhoneNotifyCurrentNetworkChange,	                              // 26001
	EMobilePhoneGetCellInfo,											  // 26002
	EMobilePhoneNotifyCellInfoChange									  // 26003
	};

enum
	{
	EMobilePhoneGetCurrentNetworkCancel = EMobilePhoneGetCurrentNetwork + EMobileCancelOffset,                                                 // 26500
	EMobilePhoneNotifyCurrentNetworkChangeCancel = EMobilePhoneNotifyCurrentNetworkChange + EMobileCancelOffset,                               // 26501
	EMobilePhoneGetCellInfoChangeCancel = EMobilePhoneGetCellInfo + EMobileCancelOffset,                                						   // 26502
	EMobilePhoneNotifyCellInfoChangeCancel = EMobilePhoneNotifyCellInfoChange + EMobileCancelOffset                                			   // 26503
	};

//
// NetworkControl NetworkServices
//
enum
	{
	EMobileCallDialEmergencyCall = EEtelMMIpcOffset7,                     // 27000
	EMobileCallDialNoFdnCheck,                                            // 27001
	EMobileSmsMessagingAckSmsStored,                                      // 27002
	EMobileSmsMessagingNackSmsStored,                                     // 27003
	EMobileSmsMessagingResumeSmsReception,                                // 27004
	EMobileSmsMessagingSendMessageNoFdnCheck,                             // 27005
	EMobileUssdMessagingSendRelease                                       // 27006
	};

enum
	{
	EMobileCallDialEmergencyCallCancel = EMobileCallDialEmergencyCall + EMobileCancelOffset,                                                   // 27500
	EMobileCallDialNoFdnCheckCancel = EMobileCallDialNoFdnCheck + EMobileCancelOffset,                                                         // 27501
	EMobileSmsMessagingAckSmsStoredCancel = EMobileSmsMessagingAckSmsStored + EMobileCancelOffset,                                             // 27502
	EMobileSmsMessagingNackSmsStoredCancel = EMobileSmsMessagingNackSmsStored + EMobileCancelOffset,                                           // 27503
	EMobileSmsMessagingResumeSmsReceptionCancel = EMobileSmsMessagingResumeSmsReception + EMobileCancelOffset,                                 // 27504
	EMobileSmsMessagingSendMessageNoFdnCheckCancel = EMobileSmsMessagingSendMessageNoFdnCheck + EMobileCancelOffset,                           // 27505
	EMobileUssdMessagingSendReleaseCancel = EMobileUssdMessagingSendRelease + EMobileCancelOffset                                              // 27506
	};

//
// NetworkControl NetworkServices WriteDeviceData
//
enum
	{
	EMobilePhoneSendNetworkServiceRequestNoFdnCheck = EEtelMMIpcOffset8,  // 28000
	EMobileUssdMessagingSendMessage,                                      // 28001
	EMobileUssdMessagingSendMessageNoFdnCheck                             // 28002
	};

enum
	{
	EMobilePhoneSendNetworkServiceRequestNoFdnCheckCancel = EMobilePhoneSendNetworkServiceRequestNoFdnCheck + EMobileCancelOffset,             // 28500
	EMobileUssdMessagingSendMessageCancel = EMobileUssdMessagingSendMessage + EMobileCancelOffset,                                             // 28501
	EMobileUssdMessagingSendMessageNoFdnCheckCancel = EMobileUssdMessagingSendMessageNoFdnCheck + EMobileCancelOffset                          // 28502
	};

//
// NetworkControl ReadDeviceData
//
enum
	{
	EMobileCallGetMobileDataCallRLPRange = EEtelMMIpcOffset9,             // 29000
	EMobilePhoneGetDefaultPrivacy,                                        // 29001
	EMobilePhoneNotifyDefaultPrivacyChange,                               // 29002
	EMobileUssdMessagingReceiveMessage,                                   // 29003
	EMobileSmartCardEapInitialiseEapMethod                                // 29004
	};

enum
	{
	EMobileCallGetMobileDataCallRLPRangeCancel = EMobileCallGetMobileDataCallRLPRange + EMobileCancelOffset,                                   // 29500
	EMobilePhoneNotifyDefaultPrivacyChangeCancel = EMobilePhoneNotifyDefaultPrivacyChange + EMobileCancelOffset,                               // 29502
	EMobileUssdMessagingReceiveMessageCancel = EMobileUssdMessagingReceiveMessage + EMobileCancelOffset,                                       // 29503
	EMobileSmartCardEapInitialiseEapMethodCancel = EMobileSmartCardEapInitialiseEapMethod + EMobileCancelOffset                                // 29504
	};

//
// NetworkControl WriteDeviceData
//
enum
	{
	EMobilePhoneMMInitialise = EEtelMMIpcOffset10,                        // 30000
	EMobilePhoneSetDefaultPrivacy,                                        // 30001
	EMobilePhoneSetPersonalisationStatus                                  // 30002
	};

enum
	{
	EMobilePhoneMMInitialiseCancel = EMobilePhoneMMInitialise + EMobileCancelOffset,                                                           // 30500
	EMobilePhoneSetDefaultPrivacyCancel = EMobilePhoneSetDefaultPrivacy + EMobileCancelOffset,                                                 // 30501
	EMobilePhoneSetPersonalisationStatusCancel = EMobilePhoneSetPersonalisationStatus + EMobileCancelOffset                                    // 30502
	};

//
// NetworkServices ReadDeviceData
//
enum
	{
	EMobilePhoneGetBarringStatusPhase1 = EEtelMMIpcOffset11,              // 31000
	EMobilePhoneGetBarringStatusPhase2,                                   // 31001
	EMobilePhoneGetCallForwardingStatusPhase1,                            // 31002
	EMobilePhoneGetCallForwardingStatusPhase2,                            // 31003
	EMobilePhoneGetCompletionRequestsPhase1,                              // 31004
	EMobilePhoneGetCompletionRequestsPhase2,                              // 31005
	EMobilePhoneGetDetectedNetworksPhase1,                                // 31006
	EMobilePhoneGetDetectedNetworksPhase2,                                // 31007
	EMobilePhoneGetDetectedNetworksV2Phase1,                              // 31008
	EMobilePhoneGetDetectedNetworksV2Phase2,                              // 31009
	EMobilePhoneGetWaitingStatusPhase1,                                   // 31010
	EMobilePhoneGetWaitingStatusPhase2,                                   // 31011
	EMobilePhoneGetDetectedNetworksV5Phase1,                              // 31012
	EMobilePhoneGetDetectedNetworksV5Phase2,                              // 31013
	EMobilePhoneGetDetectedNetworksV8Phase1,                              // 31014
	EMobilePhoneGetDetectedNetworksV8Phase2,                              // 31015					
	};

enum
	{
	EMobilePhoneGetBarringStatusCancel = EMobilePhoneGetBarringStatusPhase1 + EMobileCancelOffset,                                             // 31500
	EMobilePhoneGetCallForwardingStatusCancel = EMobilePhoneGetCallForwardingStatusPhase1 + EMobileCancelOffset,                               // 31502
	EMobilePhoneGetCompletionRequestsCancel = EMobilePhoneGetCompletionRequestsPhase1 + EMobileCancelOffset,                                   // 31504
	EMobilePhoneGetDetectedNetworksCancel = EMobilePhoneGetDetectedNetworksPhase1 + EMobileCancelOffset,                                       // 31506
	EMobilePhoneGetDetectedNetworksV2Cancel = EMobilePhoneGetDetectedNetworksV2Phase1 + EMobileCancelOffset,                                   // 31508
	EMobilePhoneGetWaitingStatusCancel = EMobilePhoneGetWaitingStatusPhase1 + EMobileCancelOffset,                                             // 31510
	EMobilePhoneGetDetectedNetworksV5Cancel = EMobilePhoneGetDetectedNetworksV5Phase1 + EMobileCancelOffset,                                   // 31511
	EMobilePhoneGetDetectedNetworksV8Cancel = EMobilePhoneGetDetectedNetworksV8Phase1 + EMobileCancelOffset,                                   // 31514
	};

//
// NetworkServices ReadUserData
//
enum
	{
	EMobileCallReceiveUUI = EEtelMMIpcOffset12,                           // 32000
	EMobileSmsMessagingReceiveMessage                                     // 32001
	};

enum
	{
	EMobileCallReceiveUUICancel = EMobileCallReceiveUUI + EMobileCancelOffset,                                                                 // 32500
	EMobileSmsMessagingReceiveMessageCancel = EMobileSmsMessagingReceiveMessage + EMobileCancelOffset                                          // 32501
	};

//
// NetworkServices WriteDeviceData
//
enum
	{
	EMobileCallActivateUUS = EEtelMMIpcOffset13,                          // 33000
	EMobilePhoneDeactivateCCBS,                                           // 33001
	EMobilePhoneSelectNetwork,                                            // 33002
	EMobilePhoneSendNetworkServiceRequest,                                // 33003
	EMobilePhoneSetCallBarringPassword,                                   // 33004
	EMobilePhoneSetCallBarringStatus,                                     // 33005
	EMobilePhoneSetCallForwardingStatus,                                  // 33006
	EMobilePhoneSetCallWaitingStatus,                                     // 33007
	EMobilePhoneSetSSPassword,                                            // 33008
	EMobilePhoneSetUUSSetting,                                            // 33009
	EMobilePhoneSetAPNControlListServiceStatus							  // 33010
	};

enum
	{
	EMobileCallActivateUUSCancel = EMobileCallActivateUUS + EMobileCancelOffset,                                                               // 33500
	EMobilePhoneDeactivateCCBSCancel = EMobilePhoneDeactivateCCBS + EMobileCancelOffset,                                                       // 33501
	EMobilePhoneSelectNetworkCancel = EMobilePhoneSelectNetwork + EMobileCancelOffset,                                                         // 33502
	EMobilePhoneSendNetworkServiceRequestCancel = EMobilePhoneSendNetworkServiceRequest + EMobileCancelOffset,                                 // 33503
	EMobilePhoneSetCallBarringPasswordCancel = EMobilePhoneSetCallBarringPassword + EMobileCancelOffset,                                       // 33504
	EMobilePhoneSetCallBarringStatusCancel = EMobilePhoneSetCallBarringStatus + EMobileCancelOffset,                                           // 33505
	EMobilePhoneSetCallForwardingStatusCancel = EMobilePhoneSetCallForwardingStatus + EMobileCancelOffset,                                     // 33506
	EMobilePhoneSetCallWaitingStatusCancel = EMobilePhoneSetCallWaitingStatus + EMobileCancelOffset,                                           // 33507
	EMobilePhoneSetSSPasswordCancel = EMobilePhoneSetSSPassword + EMobileCancelOffset,                                                         // 33508
	EMobilePhoneSetUUSSettingCancel = EMobilePhoneSetUUSSetting + EMobileCancelOffset,                                                         // 33509
	EMobilePhoneSetAPNControlListServiceStatusCancel = EMobilePhoneSetAPNControlListServiceStatus + EMobileCancelOffset						   // 33510
	};

//
// NetworkServices WriteUserData
//
enum
	{
	EMobileCallAnswerWithUUI = EEtelMMIpcOffset14,                        // 34000
	EMobileCallHangupWithUUI,                                             // 34001
	EMobileCallSendUUI,                                                   // 34002
	EMobileSmsMessagingSendMessage                                        // 34003
	};

enum
	{
	EMobileCallAnswerWithUUICancel = EMobileCallAnswerWithUUI + EMobileCancelOffset,                                                           // 34500
	EMobileCallHangupWithUUICancel = EMobileCallHangupWithUUI + EMobileCancelOffset,                                                           // 34501
	EMobileCallSendUUICancel = EMobileCallSendUUI + EMobileCancelOffset,                                                                       // 34502
	EMobileSmsMessagingSendMessageCancel = EMobileSmsMessagingSendMessage + EMobileCancelOffset                                                // 34503
	};

//
// None
//
enum
	{
 	EMobileLocationServicesNotifyMtlr = EEtelMMIpcOffset27,            	//35000
	EMobileLocationServicesNotifyMeasurementControl,			//35001
 	EMobileLocationServicesSendMolr,					//35002
	EMobileLocationServicesSendMtlrResponse,				//35003
	EMobileLocationServicesSendMtlrError,					//35004
	EMobileLocationServicesRejectMtlr,					//35005
	EMobileLocationServicesSendMeasurementReport,				//35006
	EMobileLocationServicesSendMeasurementControlFailure,			//35007
	EMobileLocationServicesSetLcsDomain,					//35008
 	EMobileLocationServicesNotifyResetUePositioningInformation //35009
	};

enum
	{
	EMobileLocationServicesNotifyMtlrCancel = EMobileLocationServicesNotifyMtlr + EMobileCancelOffset,							//35500
	EMobileLocationServicesNotifyMeasurementControlCancel = EMobileLocationServicesNotifyMeasurementControl + EMobileCancelOffset,				//35501
	EMobileLocationServicesSendMolrCancel= EMobileLocationServicesSendMolr + EMobileCancelOffset,								//35502
	EMobileLocationServicesSendMtlrResponseCancel= EMobileLocationServicesSendMtlrResponse + EMobileCancelOffset,						//35503									
	EMobileLocationServicesSendMtlrErrorCancel= EMobileLocationServicesSendMtlrError + EMobileCancelOffset,							//35504								
	EMobileLocationServicesRejectMtlrCancel= EMobileLocationServicesRejectMtlr + EMobileCancelOffset,							//35505			
	EMobileLocationServicesSendMeasurementReportCancel = EMobileLocationServicesSendMeasurementReport + EMobileCancelOffset,				//35506			
	EMobileLocationServicesSendMeasurementControlFailureCancel = EMobileLocationServicesSendMeasurementControlFailure + EMobileCancelOffset,		//35507			
 	EMobileLocationServicesNotifyResetUePositioningInformationCancel = EMobileLocationServicesNotifyResetUePositioningInformation + EMobileCancelOffset	//35509
	};

#endif // __ETELMMCS_H__
