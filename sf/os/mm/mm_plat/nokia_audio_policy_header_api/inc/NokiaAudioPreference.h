/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This file contains priority and preference codes for Nokia
*                specific audio clients of the MMF DevSound. These priority and
*                preference settings allow the Audio Policy Server making
*                decisions regarding play requests received from the clients.
*
*                Symbian allows S60 applications to use the upper 16 bits of
*                the Priority Preference value. Unique assignments of these
*                upper bits allow the Audio Policy Server to make decisions
*                in 'special' cases, in which the result may be a refusal,
*                modification or substituing of the request based on the
*                profile settings, call status and other audio activities.
*
*/


#ifndef NOKIAAUDIOPREFERENCE_H
#define NOKIAAUDIOPREFERENCE_H

#define AUDIO_PREFERENCE_SPLIT
#include <AudioPreference.h>

// The following constants defines the range of preference values that can be
// allocated for NSS applications.
// Preference values can range from 0x06000001 - 0x06001000.
//
// Note: If an NSS application becomes part of S60 delivery, the corresponding
// preference/priority values will be moved to AudioPreference.h. When this
// happens, AudioPreference.h will contain preference value(s) that lies within
// KNokiaAudioPrefMinValue and KNokiaAudioPrefMaxValue range.
// So before allocating a new preference value, check AudioPreference.h to make
// sure the value is not already defined.

const TUint KNokiaAudioPrefMinValue                                 = 0x06000001;
const TUint KNokiaAudioPrefMaxValue                                 = 0x06001000;

// Preference values definition
const TUint KAudioPrefNotification                                  = 0x06000001;
const TUint KAudioPrefDeviceModeChange                              = 0x06000002;
const TUint KAudioPrefConnectivityEvent                             = 0x06000003;
const TUint KAudioPrefAccessoryConnectionChange                     = 0x06000004;
const TUint KAudioPrefApplicationGridBGM                            = 0x06000005;
const TUint KAudioPrefNavigation                                    = 0x06000006;   // Cap: MMDD
const TUint KAudioPrefVoiceActivation                               = 0x06000007;   // Cap:?  Hands Free Voice UI

// Legacy definition. Do not modify these values - Begins
const TUint KAudioPrefUnknownIncomingCall                           = 0x05110009;
const TUint KAudioPrefRadioUiEvent                                  = 0x03010001;
const TUint KAudioPrefMenuPopup                                     = 0x03200001;
const TUint KAudioPrefMenuCommand                                   = 0x03210001;
// Legacy definition. Do not modify these values - Ends

// Priority values definition
const TUint KAudioPriorityNotificationTone                          = 55;
const TUint KAudioPriorityDeviceModeChange                          = 48;
const TUint KAudioPriorityConnectivityEvent                         = 48;
const TUint KAudioPriorityAccessoryConnectionChange                 = 48;
const TUint KAudioPriorityApplicationGridBGM                        = 47;
const TUint KAudioPriorityNavigation                                = 85;   // Cap: MMDD
const TUint KAudioPriorityVoiceActivation                           = 60;   // Cap: ? Hands Free Voice UI

// Legacy definition. Do not modify these values - Begins
const TUint KAudioPriorityUnknownIncomingCall                       = 95;
const TUint KAudioPriorityRadioUiEvent                              = 79;
const TUint KAudioPriorityMenuPopup                                 = 47;
const TUint KAudioPriorityMenuCommand                               = 47;
// Legacy definition. Do not modify these values - Ends

// Constants
const TUint8   KMaxToneVol = 9;
const TUint8   KMinToneVol = 1;

//** Priorities
const TUint KAudioPriorityPhoneCallLine2                        = 100;
const TUint KAudioPriorityIncomingDataCall                      = 100;
const TUint KAudioPriorityAutoRedialCompleted                   = 100;
const TUint KAudioPriorityIntegratedTelephonyUplink             = 100;
const TUint KAudioPriorityIntegratedTelephonyDownlink           = 100;
const TUint KAudioPriorityIntegratedTelephonyUplinkNonSignal    = 100;
const TUint KAudioPriorityIntegratedIncomingCall                = 100;
const TUint KAudioPriorityVoipAudioUplinkNonSignal              = 100;
const TUint KAudioPriorityVideoCall                             = 99;
const TUint KAudioPriorityExternalTelephonyUplink               = 95;
const TUint KAudioPriorityExternalTelephonyDownlink             = 95;
const TUint KAudioPriorityExternalTelephonyUplinkNonSignal      = 95;
const TUint KAudioPriorityExternalIncomingCall                  = 95;
const TUint KAudioPriorityCsPoc                                 = 95;
const TUint KAudioPriorityCalendarAlarm                         = 93;
const TUint KAudioPriorityReceivedSpecialMessage                = 90;
const TUint KAudioPriorityBatt                                  = 85;
const TUint KAudioPriorityRechargeBattery                       = 85;
const TUint KAudioPriorityWrongCharger                          = 85;
const TUint KAudioPriorityAudioRecording                        = 80;
const TUint KAudioPriorityAudioMsgEditor                        = 80;
const TUint KAudioPriorityFMRadioRecording                      = 80;
const TUint KAudioPriorityRecordingEvent                        = 80;
const TUint KAudioPriorityVoiceRecorderStopTone                 = 80;
const TUint KAudioPriorityAudioPlaybackStreaming                = 80;
const TUint KAudioPriorityVideoPlayback                         = 80;
const TUint KAudioPriorityMobileTV                              = 80;
const TUint KAudioPriorityVoiceRecorderRecordingTone            = 80;
const TUint KAudioPriorityRedialTone                            = 75;
const TUint KAudioPriorityAutoRedial                            = 75;
const TUint KAudioPriorityCallWaitingTone                       = 75;
const TUint KAudioPriorityReorderTone                           = 75;
const TUint KAudioPriorityCongestionTone                        = 75;
const TUint KAudioPriorityRadioPathNotAvailableTone             = 75;
const TUint KAudioPriorityRadioPathAcknowledgementTone          = 75;
const TUint KAudioPriorityDialTone                              = 75;
const TUint KAudioPriorityVideoCallUplink                       = 74;
const TUint KAudioPriorityVoiceTagRecognitionEvent              = 73;
const TUint KAudioPriorityVoiceTagPlayback                      = 73;
const TUint KAudioPriorityVoiceStartingTone                     = 73;
const TUint KAudioPriorityVoiceErrorTone                        = 73;
const TUint KAudioPriorityVoiceAbortTone                        = 73;
const TUint KAudioPriorityHighLevel                             = 70;
const TUint KAudioPriorityConn                                  = 65;
const TUint KAudioPriorityConnectionStopTone                    = 65;
const TUint KAudioPriorityOperationReadyTone                    = 65;
const TUint KAudioPriorityStereoBT                              = 65;
const TUint KAudioPriorityVolAdj                                = 60;
const TUint KAudioPriorityAudioVolumeMax                        = 60;
const TUint KAudioPriorityIhfActive                             = 60;
const TUint KAudioPriorityRingingTonePreviewWithVibra           = 60;
const TUint KAudioPriorityFilePreview                           = 60;
const TUint KAudioPriorityCharging                              = 55;
const TUint KAudioPriorityBatteryFull                           = 55;
const TUint KAudioPriorityConfirmationTone                      = 55;
const TUint KAudioPriorityDefaultBeep                           = 55;
const TUint KAudioPriorityPowerOff                              = 50;
const TUint KAudioPriorityDtmfKeyRelease                        = 45;
const TUint KAudioPriorityVoiceAid                              = 40;

//** Preferences
const TUint KAudioPrefStereoBT                              = 0x05340001;
const TUint KAudioPrefVideoCallUplink                       = 0x05320001;
const TUint KAudioPrefVideoCallDownlink                     = 0x05310001;
const TUint KAudioPrefCsPoc                                 = 0x05110010;
const TUint KAudioPrefExternalIncomingCall                  = 0x05110009;
const TUint KAudioPrefExternalTelephonyUplinkNonSignal      = 0x05110008;
const TUint KAudioPrefExternalTelephonyDownlink             = 0x05110007;
const TUint KAudioPrefExternalTelephonyUplink               = 0x05110006;
const TUint KAudioPrefIntegratedIncomingCall                = 0x05110005;
const TUint KAudioPrefIntegratedTelephonyUplinkNonSignal    = 0x05110004;
const TUint KAudioPrefIntegratedTelephonyDownlink           = 0x05110003;
const TUint KAudioPrefIntegratedTelephonyUplink             = 0x05110002;
const TUint KAudioPrefMobileTVEvent                         = 0x03100001;
const TUint KAudioPrefAutoRedial                            = 0x01870001;
const TUint KAudioPrefRedialTone                            = 0x01860001;
const TUint KAudioPrefVideoPlayback                         = 0x01430001;
const TUint KAudioPrefAMREncode                             = 0x01360001;
const TUint KAudioPrefAMRDecode                             = 0x01350001;
const TUint KAudioPrefIncomingCallLine2                     = 0x01330001;
const TUint KAudioPrefAutoRedialCompleted                   = 0x01310001;
const TUint KAudioPrefBatteryLow                            = 0x01030001;
const TUint KAudioPrefRechargeBattery                       = 0x01020001;
const TUint KAudioPrefWrongCharger                          = 0x01010001;
const TUint KAudioPrefAudioMsgEditor                        = 0x00970001;
const TUint KAudioPrefFMRadioRecording                      = 0x00960001;
const TUint KAudioPrefAudioRecording                        = 0x00940001;
const TUint KAudioPrefVoiceRecRecording                     = 0x00920001;
const TUint KAudioPrefRadioPathAck                          = 0x00890001;
const TUint KAudioPrefDial                                  = 0x00880001;
const TUint KAudioPrefError                                 = 0x00720001;
const TUint KAudioPrefVoiceError                            = 0x00620001;
const TUint KAudioPrefVoiceAbort                            = 0x00610001;
const TUint KAudioPrefConnectionStart                       = 0x00530001;
const TUint KAudioPrefConnectionStop                        = 0x00520001;
const TUint KAudioPrefOperationReady                        = 0x00510001;
const TUint KAudioPrefFilePreview                           = 0x00440001;
const TUint KAudioPrefAudioVolumeMin                        = 0x00430001;
const TUint KAudioPrefAudioVolumeMax                        = 0x00420001;
const TUint KAudioPrefIHFActive                             = 0x00410001;
const TUint KAudioPrefLowLevel                              = 0x00350001;
const TUint KAudioPrefCharging                              = 0x00330001;
const TUint KAudioPrefBatteryFull                           = 0x00340001;
const TUint KAudioPrefPowerOn                               = 0x00220001;
const TUint KAudioPrefPowerOff                              = 0x00210001;
const TUint KAudioPrefKeyUpDTMF                             = 0x00120001;
const TUint KAudioPrefComposer                              = 0x00050001;
const TUint KAudioPrefSRSPlayback                           = 0x00030001;
const TUint KAudioPrefSRSTrain                              = 0x00020001;
const TUint KAudioPrefSRSRecog                              = 0x00010001;

#endif // NOKIAAUDIOPREFERENCE_H
