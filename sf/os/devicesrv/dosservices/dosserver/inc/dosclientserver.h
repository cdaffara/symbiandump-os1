/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*    This file contains definitions that are necessary for the Client-Server 
*    communication.
*
*/


#ifndef __DOSCLIENTSERVER_H__
#define __DOSCLIENTSERVER_H__

#include <e32std.h>

// Constants
_LIT(KDosServerName,"DosServer");
_LIT(KDosServerImg,"DosServer");         // DLL/EXE name

const TInt KSDReserveFileMaxSize = 0x40000; //256*1024

/**
* This enumeration contains the DosServer client-server messages.
*/
enum TDosMessages
    {
    //SubSession management
    EDosCreateSysUtilsSubSession=100,
    EDosCreateHelperSubSession,
    EDosCreateMtcSubSession,
    EDosCreateSelfTestSubSession,
    EDosCreateSaeSubSession,
    EDosCreateAccessorySubSession,
    EDosCreateAudioSubSession,
    EDosCreateExtensionSubSession,
    EDosCreateEventRcvSubSession,
    EDosCreateEventSndSubSession,
    EDosCreateBTAudioSubSession,
    EDosCloseSubSession,
    EDosCreateSharedDataSubSession,


    //Services from SysUtils
    EGetSimLanguage=200,
    EPerformDosRfs,
    ESetDosAlarm,

    //Services from Helper
    EGetStartupReason=300,
    EGetSWStartupReason,
    ESetSWStartupReason,
    EHelperHiddenReset,
    EGetRTCStatus,
    EGenerateGripEvent,

    //Services from MTC
    EPowerOn=400,
    EPowerOff,
    EResetGenerate,
    ESetState,
    EDosSync,
    ESetStateFlag,
    EGetStateFlag,
    EDosShutdownSync,

    //Services from SelfTest
    EPerformSelfTest=500,

    //Services from Sae
    EStartSae=700,

    //Services from Extension
    ECallFunction=800,

    //Service for Firing events
    ERegisterEvent=900,
    EUnRegisterEvent,
    EWaitEvent,
    EDosEventFiring,
    ECancelWaitEvent,

    EServerShutdown=1000,

    // Service from Accessory
    ESetAccessoryMode=1100,
    ESetLoopSetModeState,
    EGetLoopSetModeState,
    EGetAccessoryModeAsync,
    ECancelGetAccessoryModeAsync,
    EGetAccessoryModeNew,

    // Service from Audio
    ESetVolumeMuteState=1200,
    EGetVolumeMuteState,
    ESetHandsfreeMode,
    EGetHandsfreeMode,
    EGetOutputVolume,
    ESetOutputVolume,
    EEnumerateAudioVolumeLevels,
    EEnumerateAudioVolumeLevelsAsync,
    ECancelEnumerateAudioVolumeLevelsAsync,
    EGetOutputVolumeAsync,
    ECancelGetOutputVolumeAsync,
    ESetOutputVolumeAsync,
    ECancelSetOutputVolumeAsync,
    ESetAccessoryModeAsync,
    ECancelSetAccessoryModeAsync,
    ESetVolumeMuteStateAsync,
    ECancelSetVolumeMuteStateAsync,
    EGetVolumeMuteStateAsync,
    ECancelGetVolumeMuteStateAsync,

    //Services from BTAudio        
    EAttachAudioAccessoryReq=1300, // requests
    ECancelAttachAudioAccessoryReq,
    EDetachAudioAccessoryReq,
    ECancelDetachAudioAccessoryReq,
    EAudioLinkOpenResp, // responses
    EAudioLinkCloseResp,
    EActivateVoiceRecognitionResp,
    EAbortVoiceRecognitionResp,
    EVoiceRecognitionStatusResp,
    EQuerySpeedDialNumberResp,
    EQueryLastDialledVoiceNumberResp,   
    EAudioLinkOpenInd,  // indications
    EAudioLinkCloseInd,
    EVoiceRecognitionStatusInd,
    // Service from SharedData
    ERequestFreeDiskSpace=1400,
    ERequestFreeDiskSpaceCancel
    };

//FLAGS
const TUint KNoAutoComplete=        0x0;
const TUint KAutoComplete=          0x1;
const TUint KUseDefaultIntensity=   0x2;

/**
* The following enumaration defines the DosServer panic codes.
*/
enum TDosPanic
{
    // Illegal function called via client-server interface
    EPanicIllegalFunction=0,
    EPanicBadHandle,
    EPanicBadDescriptor,
};


#endif // __DOSCLIENTSERVER_H__
