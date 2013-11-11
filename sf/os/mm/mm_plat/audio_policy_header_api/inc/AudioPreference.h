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
* Description:  This file contains priority and preference codes for
*                audio clients of the MMF DevSound. These priority and
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


#ifndef AUDIOPREFERENCE_H
#define AUDIOPREFERENCE_H

#include <e32base.h>

//** Priorities
const TUint KAudioPriorityCSEmergencyCallDownlink           = 100; // CAP : NONE (Valid only for phone App SID: 0x100058B3)
const TUint KAudioPriorityCSEmergencyCallUplink             = 100; // CAP : NONE (Valid only for phone App SID: 0x100058B3)
const TUint KAudioPriorityPhoneCall                         = 100;
const TUint KAudioPriorityCSCallUplink                      = 100;
const TUint KAudioPriorityCSCallDownlink                    = 100;
const TUint KAudioPriorityTextToSpeechCall                  = 100;
const TUint KAudioPriorityVoiceDial                         = 99;
const TUint KAudioPriorityAlarm                             = 95;
const TUint KAudioPriorityPocAudioUplink                    = 95;
const TUint KAudioPriorityPocAudioDownlink                  = 95;
const TUint KAudioPriorityIncomingPocCall                   = 95;
const TUint KAudioPriorityVideoRecording                    = 93;
const TUint KAudioPriorityRecvMsg                           = 90;
const TUint KAudioPriorityRealOnePlayer                     = 80;
const TUint KAudioPriorityRecording                         = 80;
const TUint KAudioPrioritySmilPlayer                        = 80;
const TUint KAudioPriorityMmsViewer                         = 80;
const TUint KAudioPriorityUPnPRemotePlayback                = 80;  // CAP: None
const TUint KAudioPriorityFMRadio                           = 79;
const TUint KAudioPrioritySwisPlayback                      = 74;
const TUint KAudioPriorityPocStartNotification              = 70;
const TUint KAudioPriorityPocEndNotification                = 70;
const TUint KAudioPriorityPocQueuedNotification             = 70;
const TUint KAudioPriorityWarningTone                       = 70;
const TUint KAudioPriorityRingingTonePreview                = 60;
const TUint KAudioPriorityPreview                           = 60;
const TUint KAudioPriorityPhonePower                        = 50;
const TUint KAudioPriorityLowLevel                          = 55;
const TUint KAudioPriorityKeyPress                          = 45;
const TUint KAudioPriorityDTMFString                        = 45;
const TUint KAudioPriorityDtmfKeyPress                      = 45;
const TUint KAudioPriorityUnknownDTMFString                 = 45;  // CAP: WDD
const TUint KAudioPriorityKeyPressNonDTMFWithFeedback		= 45;
const TUint KAudioPriorityVoipAudioUplink                   = 100;
const TUint KAudioPriorityVoipAudioDownlink                 = 100;
const TUint KAudioPriorityNetMsg                            = 75;
const TUint KAudioPrioritySpecialInformationTone            = 75;
const TUint KAudioPriorityRingingTone                       = 75;
const TUint KAudioPriorityCameraTone                        = 45;
const TUint KAudioPriorityVisualization                     = 40;

// Legacy definition. Do not modify these values - Begins
const TUint KAudioPriorityUnknownVoipAudioUplink            = 80;   // CAP: WDD
const TUint KAudioPriorityUnknownVoipAudioUplinkNonSignal   = 80;   // CAP: WDD
const TUint KAudioPriorityUnknownVoipAudioDownlink          = 80;   // CAP: WDD
// Legacy definition. Do not modify these values - Ends


//** Preferences
// The following constant defines the maximum preference value defined by the
// S60 platform. Values ranging from 0x00000000 - 0x06000000 are reserved for
// preference values used by S60 applications.
const TUint KAudioPrefMaxValue                              = 0x06000000;

const TUint KAudioPrefSwisPlayback                          = 0x05330001;
const TUint KAudioPrefVoipAudioUplinkNonSignal              = 0x05230001;
const TUint KAudioPrefVoipAudioUplink                       = 0x05220001;
const TUint KAudioPrefVoipAudioDownlink                     = 0x05210001;
const TUint KAudioPrefCSEmergencyCallDownlink               = 0x05150001; // CAP : NONE (Valid only for phone App SID: 0x100058B3)
const TUint KAudioPrefCSEmergencyCallUplink                 = 0x05140001; // CAP : NONE (Valid only for phone App SID: 0x100058B3)
const TUint KAudioPrefVoipEmergencyCallDownlink             = 0x05140002; // CAP : None (Valid only for MCE Server SID: 0x1020E4C7)
const TUint KAudioPrefVoipEmergencyCallUplink               = 0x05140003; // CAP : None (Valid only for MCE Server SID: 0x1020E4C7)
const TUint KAudioPrefVoipEmergencyCallUplinkNonSignal      = 0x05140004; // CAP : None (Valid only for MCE Server SID: 0x1020E4C7)
const TUint KAudioPrefPocAudioUplink                        = 0x05130001;
const TUint KAudioPrefPocAudioDownlink                      = 0x05120001;
const TUint KAudioPrefIncomingPocCall                       = 0x05110001;
const TUint KAudioPrefRadioAudioEvent                       = 0x03000001;
const TUint KAudioPrefUPnPPlayback                          = 0x01610001;  // CAP: NONE
const TUint KAudioPrefSmilPlayer                            = 0x01450001;
const TUint KAudioPrefMmsViewer                             = 0x01440001;
const TUint KAudioPrefRealOneLocalPlayback                  = 0x01420001;
const TUint KAudioPrefRealOneStreaming                      = 0x01410001;
const TUint KAudioPrefTextToSpeechCall                      = 0x01370001;
const TUint KAudioPrefCSCallDownlink                        = 0x01360001;
const TUint KAudioPrefCSCallUplink                          = 0x01350001;
const TUint KAudioPrefIncomingCall                          = 0x01340001;
const TUint KAudioPrefIncomingDataCall                      = 0x01320001;
const TUint KAudioPrefAlarmClock                            = 0x01220001;
const TUint KAudioPrefCalendarAlarm                         = 0x01210001;
const TUint KAudioPrefNewSMS                                = 0x01120001;
const TUint KAudioPrefNewSpecialMessage                     = 0x01110001;
const TUint KAudioPrefVideoRecording                        = 0x00950001;
const TUint KAudioPrefVoiceRecStart                         = 0x00930001;
const TUint KAudioPrefVoiceRecStop                          = 0x00910001;
const TUint KAudioPrefPocQueuedNotification                 = 0x00750001;
const TUint KAudioPrefPocEndNotification                    = 0x00740001;
const TUint KAudioPrefPocStartNotification                  = 0x00730001;
const TUint KAudioPrefWarning                               = 0x00710001;
const TUint KAudioPrefVoiceStarting                         = 0x00630001;
const TUint KAudioPrefConfirmation                          = 0x00320001;
const TUint KAudioPrefDefaultTone                           = 0x00310001;
const TUint KAudioDTMFString                                = 0x00150001;
const TUint KAudioPrefUnknownDTMFString                     = 0x00150002; // CAP: WDD
const TUint KAudioPrefKeyPressNonDTMF                       = 0x00140001;
const TUint KAudioPrefKeyPressNonDTMFWithFeedback           = 0x00140002; // CAP: WDD
const TUint KAudioPrefKeyDownDTMF                           = 0x00130001;
const TUint KAudioPrefCamera                                = 0x00110001;
const TUint KAudioPrefKeyPressPreview                       = 0x00080001;
const TUint KAudioPrefRingFilePreviewVibra                  = 0x00070001;
const TUint KAudioPrefVoiceRec                              = 0x00060001;
const TUint KAudioPrefRingFilePreview                       = 0x00040001;
const TUint KAudioPrefVocosPlayback                         = 0x00030001;
const TUint KAudioPrefVocosTrain                            = 0x00020001;
const TUint KAudioPrefVocosRecog                            = 0x00010001;

// Legacy definition. Do not modify these values - Begins
const TUint KAudioPrefUnknownVoipAudioUplink                = 0x05110006; // CAP: WDD
const TUint KAudioPrefUnknownVoipAudioUplinkNonSignal       = 0x05110008; // CAP: WDD
const TUint KAudioPrefUnknownVoipAudioDownlink              = 0x05110007; // CAP: WDD
// Legacy definition. Do not modify these values - Ends

const TUint KAudioPrefVisualization                         = 0x05350001;
const TUint KAudioPrefRingGoing                             = 0x00870001;
const TUint KAudioPrefBusy                                  = 0x00860001;
const TUint KAudioPrefCallWaiting                           = 0x00850001;
const TUint KAudioPrefReorder                               = 0x00840001;
const TUint KAudioPrefCongestion                            = 0x00830001;
const TUint KAudioPrefSpecialInformation                    = 0x00820001;
const TUint KAudioPrefRadioNotAvailable                     = 0x00810001;
const TUint KAudioPrefVoiceAid                              = 0x00411441;

#endif // AUDIOPREFERENCE_H
