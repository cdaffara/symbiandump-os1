/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of integration test constants
*
*/


#ifndef __TFDOSSERVERTYPES_H__
#define __TFDOSSERVERTYPES_H__

#include <e32base.h>

enum TTFDosFunction
    {
    ETFDosFunction_None                             = 0,
    
#ifndef __ACCESSORY_FW    
    ETFDosAudio_Open                                = 1,
    ETFDosAudio_SetHandsfreeMode                    = 4,
    ETFDosAudio_GetHandsfreeMode                    = 5,
    ETFDosAudio_GetOutputVolume                     = 19,
    ETFDosAudio_SetOutputVolume                     = 22,
    ETFDosAudio_Close                               = 28,
    ETFDosAccessory_Open                            = 30,
    ETFDosAccessory_SetLoopSetModeState             = 31,
    ETFDosAccessory_GetLoopSetModeState             = 32,
    ETFDosAccessory_GetAccessoryMode 		   		= 33,
    ETFDosAccessory_Close                           = 40,
#endif // __ACCESSORY_FW    
    
#ifndef RD_STARTUP_CHANGE
    ETFDosSysUtils_Open                             = 41,
    ETFDosSysUtils_GetSimLanguage                   = 42,
    ETFDosSysUtils_SetDosAlarm                      = 43,
    ETFDosSysUtils_PerformDosRfs                    = 44,
    ETFDosSysUtils_Close                            = 45,
    ETFDosHelper_Open                               = 50,
    ETFDosHelper_GetStartupReason                   = 51,
    ETFDosHelper_GetSWStartupReason                 = 52,
    ETFDosHelper_SetSWStartupReason                 = 53,
    ETFDosHelper_HiddenReset                        = 54,
    ETFDosHelper_GetRTCStatus                       = 55,
    ETFDosHelper_GenerateGripEvent                  = 56,
    ETFDosHelper_Close                              = 57,
    ETFDosMtc_Open                                  = 60,
    ETFDosMtc_PowerOn                               = 61,
    ETFDosMtc_PowerOff                              = 62,
    ETFDosMtc_DosSync                               = 63,
    ETFDosMtc_ResetGenerate                         = 64,
    ETFDosMtc_SetState                              = 65,
    ETFDosMtc_SetStateFlag                          = 66,
    ETFDosMtc_GetStateFlag                          = 67,
    ETFDosMtc_DosShutdownSync                       = 68,
    ETFDosMtc_Close                                 = 69,
    ETFDosSelfTest_Open                             = 70,
    ETFDosSelfTest_PerformSelfTest                  = 71,
    ETFDosSelfTest_Close                            = 72,
#endif // RD_STARTUP_CHANGE

    ETFDosSae_Open                                  = 90,
    ETFDosSae_StartSae                              = 91,
    ETFDosSae_Close                                 = 92,
    
#ifndef __ACCESSORY_FW 
    ETFDosBTAudio_Open                              = 100,
    ETFDosBTAudio_AttachAudioAccessoryReq           = 101,
    ETFDosBTAudio_CancelAttachReq                   = 102,
    ETFDosBTAudio_DetachAudioAccessoryReq           = 103,
    ETFDosBTAudio_CancelDetachReq                   = 104,
    ETFDosBTAudio_AudioLinkOpenResp                 = 105,
    ETFDosBTAudio_AudioLinkCloseResp                = 106,
    ETFDosBTAudio_ActivateVoiceRecognitionResp      = 107,
    ETFDosBTAudio_AbortVoiceRecognitionResp         = 108,
    ETFDosBTAudio_VoiceRecognitionStatusResp        = 109,
    ETFDosBTAudio_QuerySpeedDialNumberResp          = 110,
    ETFDosBTAudio_QueryLastDialledVoiceNumberResp   = 111,
    ETFDosBTAudio_AudioLinkOpenInd                  = 112,
    ETFDosBTAudio_AudioLinkCloseInd                 = 113,
    ETFDosBTAudio_VoiceRecognitionStatusInd         = 114,
    ETFDosBTAudio_Close                             = 115,
#endif // __ACCESSORY_FW   
 
    ETFDosExtension_Open                            = 120,
    ETFDosExtension_CallFunction                    = 121,
    ETFDosExtension_CallFunctionAndComplete         = 122,
    ETFDosExtension_Close                           = 123,
    };


enum TTFDosEvent
    {
    ETFDosEvent_None                                = 0,

#ifndef RD_STARTUP_CHANGE
    ETFDosEvent_NotifySelfTestStatus                = 1,
    ETFDosEvent_DoMtcInitiatedShutdown              = 2,
    ETFDosEvent_PhonePowerOn                        = 3,
#endif //RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
    ETFDosEvent_SimReady                            = 8,
    ETFDosEvent_SimChanged                          = 9,
    ETFDosEvent_SimLockStatus                       = 10,
    ETFDosEvent_SimState                            = 11,
    ETFDosEvent_CallsForwardingStatus               = 12,
    ETFDosEvent_VoiceMailStatus                     = 13,
    ETFDosEvent_FaxMessage                          = 14,
    ETFDosEvent_EmailMessage                        = 15,
#endif //RD_STARTUP_CHANGE

 //   ETFDosEvent_NetworkBars                         = 16,

#ifndef RD_STARTUP_CHANGE
    ETFDosEvent_SecurityCode                        = 17,
#endif //RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
    ETFDosEvent_CurrentSimOwnedStatus               = 19,
#endif //RD_STARTUP_CHANGE

#ifndef __ACCESSORY_FW
    ETFDosEvent_AudioLinkOpenReq                    = 20,
    ETFDosEvent_AudioLinkCloseReq                   = 21,
    ETFDosEvent_ActivateVoiceRecognitionReq         = 22,
    ETFDosEvent_AbortVoiceRecognitionReq            = 23,
    ETFDosEvent_VoiceRecognitionStatusReq           = 24,
    ETFDosEvent_QuerySpeedDialNumberReq             = 25,
    ETFDosEvent_QueryLastDialledVoiceNumberReq      = 26,
    ETFDosEvent_AccessoryModeChangedInd             = 27,
    ETFDosEvent_BTAccessoryModeChangedInd           = 28,
    ETFDosEvent_AccessorySpeakerVolumeSetInd        = 29,
    ETFDosEvent_ServiceLevelConnEstablishedInd      = 30,
    ETFDosEvent_AccessoryModeChanged                = 31,
    ETFDosEvent_HeadsetButtonChanged                = 32,
    ETFDosEvent_HandsfreeModeChanged                = 33,
    ETFDosEvent_MicrophoneMuteStateChanged          = 34,
    ETFDosEvent_OutputVolumeLevelChanged            = 35,
#endif

#ifndef RD_STARTUP_CHANGE
    ETFDosEvent_SmsStorageStatusChanged             = 36,
#endif // RD_STARTUP_CHANGE

    ETFDosEvent_General                             = 39,

#ifndef RD_STARTUP_CHANGE
    ETFDosEvent_SimPresentStateChanged              = 40,
#endif //RD_STARTUP_CHANGE

    ETFDosEvent_Synchronized                        = 10000
    };


enum TTFDosFlags
    {
    ETFDosFlags_None                                = 0x00,
    ETFDosFlags_IgnoreParameters                    = 0x01,
    ETFDosFlags_IgnoreEventParameters               = 0x02,
    ETFDosFlags_IgnoreResult                        = 0x04,
    ETFDosFlags_IgnoreUnexpectedEvents              = 0x08,
    ETFDosFlags_StoreParameters                     = 0x10,
    ETFDosFlags_UseParameters                       = 0x20,
    };

    
struct TTFDosServerControlTestCaseState
    {
    TTFDosFunction          iDosFunction;
    TInt                    iArg1;
    TInt                    iArg2;
    TInt                    iExpectedResult;
    TTFDosEvent             iCompletionEvent;
    TInt                    iStateFlags;
    };    

const TInt KTFErrDosServiceMissing             = -4001;
const TInt KTFErrDosNoSyncEnd                  = -4002;
const TInt KTFErrDosUnexpectedEvent            = -5000;
const TInt KTFErrDosUnexpectedEventParameter   = -6000;
const TInt KTFErrDosUnexpectedArg1             = -7000;
const TInt KTFErrDosUnexpectedArg2             = -8000;
const TInt KTFErrDosUnexpectedResult           = -9000;

#endif
