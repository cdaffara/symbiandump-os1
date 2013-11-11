/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

  

#ifndef MMTSY_TIMEOUTDEFS_H
#define MMTSY_TIMEOUTDEFS_H

//INCLUDES
#include <e32def.h>

// CONSTANTS
//Disable if automatic response is not required
#define REQHANDLE_TIMER

    #ifdef REQHANDLE_TIMER
    //lenght of timer entry table
    const TUint8 KMultimodeTsyTimeStoreLength = 25;

    #ifdef __WINS__
    //value for increasing the timouts defined below for WINS debugging
    const TUint8 KMultimodeTsyWinsTimeoutIncreasing = 0;
    #endif

    //CMmPhoneTsy
    //DTMF
    const TUint8 KMmPhoneSendDTMFTonesTimeOut = 125;
    const TUint8 KMmPhoneStartDTMFToneTimeOut = 5;
    const TUint8 KMmPhoneStopDTMFToneTimeOut = 5;

    // All Calls Termination
    const TUint8 KMmPhoneTerminateAllCallsTimeOut = 30;
    // Active Calls Termination
    const TUint8 KMmPhoneTerminateActiveCallsTimeOut = 30;
    
    //NET
    const TUint8 KMmPhoneSelectNetworkTimeOut = 165;
    const TUint8 KMmPhoneSendNetworkServiceRequestTimeOut = 30;
    const TUint8 KMmPhoneSendNetworkServiceRequestNoFdnCheckTimeOut = 30;
    const TUint8 KMmPhoneGetDetectedNetworksPhase1TimeOut = 255;
    const TUint8 KMmPhoneSetNWSelectionSettingTimeOut = 120;
    const TUint8 KMmCustomGetNetworkOperatorNameTimeout = 20;
    const TUint8 KMmPhoneIMSAuthenticateTimeOut = 240;
    const TUint8 KMmPhoneGetCurrentActiveUSimApplicationTimeOut = 60;
    const TUint8 KMmPhoneGetAuthorizationInfoPhase1TimeOut = 30;

    //SECURITY
    const TUint8 KMmPhoneSetLockSettingTimeOut = 60;
    const TUint8 KMmPhoneGetLockInfoTimeOut = 60;
    const TUint8 KMmPhoneAbortSecurityCodeTimeOut = 60;
    const TUint8 KMmPhoneVerifySecurityCodeTimeOut = 60;
    const TUint8 KMmPhoneChangeSecurityCodeTimeOut = 60;
    const TUint8 KMmPhoneGetSecurityCodeInfoTimeOut = 60;

    //SS
    //These features that do not require PIN verification
    const TUint8 KMmPhoneSetCallForwardingStatusTimeOut = 60;
    const TUint8 KMmPhoneSetCallBarringStatusTimeOut = 60;
    const TUint8 KMmPhoneGetCallForwardingStatusPhase1TimeOut = 60;
    const TUint8 KMmPhoneGetCallBarringStatusPhase1TimeOut = 60;
    const TUint8 KMmPhoneGetCallWaitingStatusPhase1TimeOut = 60;
    const TUint8 KMmPhoneSetSSPasswordTimeOut = 60;
    const TUint8 KMmPhoneSetCallWaitingStatusTimeOut = 60;
    const TUint8 KMmPhoneGetIdentityServiceStatusTimeOut = 60;
    const TUint8 KMmPhoneGetAlsLineTimeOut = 60;
    const TUint8 KMmPhoneSetAlsLineTimeOut = 60;
    const TUint8 KMmPhoneGetIccMessageWaitingIndicators = 60;
    const TUint8 KMmPhoneSetIccMessageWaitingIndicators = 60;

    //These require verifying of securiry code -> may take a long time.
    const TUint8 KMmPhoneSetFdnSettingTimeOut = 120;
    const TUint8 KMmPhoneGetServiceTableTimeOut = 120;
    const TUint8 KMmPhoneGetCustomerServiceProfileTimeOut = 120;
    const TUint8 KMmPhoneGetPhoneStoreInfoTimeOut = 120;
    const TUint8 KMmPhoneGetFdnStatusTimeOut = 120;

    //SIM
    const TUint8 KMmPhoneGetServiceProvicedNameTimeOut = 30;
    const TUint8 KMmPhoneGetMailboxNumbersTimeOut = 20;
    const TUint8 KMmPhoneEnumerateAPNEntriesTimeOut = 30;
    const TUint8 KMmPhoneGetAPNnameTimeOut = 30;
    const TUint8 KMmPhoneAppendAPNNameTimeOut = 30;
    const TUint8 KMmPhoneDeleteAPNNameTimeOut = 30;
    const TUint8 KMmPhoneSetAPNControlListServiceStatusTimeOut = 30;
    const TUint8 KMmPhoneGetAPNControlListServiceStatusTimeOut = 30;

    //CMmLineTsy
    //None


    //CMmCallTsy
    const TUint8 KMmCallDialTimeOut = 50;
    const TUint8 KMmCallDialSignalingTimeOut = 50;
    const TUint8 KMmCallAnswerTimeOut = 50;
    const TUint8 KMmCallAnswerCallBackTimeOut = 180;
    const TUint8 KMmCallHangUpTimeOut = 60;
    const TUint8 KMmCallHoldTimeOut = 30;
    const TUint8 KMmCallResumeTimeOut = 3;
    const TUint8 KMmCallSwapTimeOut = 3;
    const TUint8 KMmCallDeflectCallTimeOut = 10;
    const TUint8 KMmCallDialEmergencyCallTimeOut = 75;
    const TUint8 KMmCallTransferTimeOut = 10;
    const TUint8 KMmCallGoOneToOneTimeOut = 5;
    const TUint8 KMmCallSetDynamicHscsdParamsTimeOut = 20;


    //CMmConferenceCallTsy
    const TUint8 KMmConferenceCallCreateConferenceTimeOut = 10;
    const TUint8 KMmConferenceCallAddCallTimeOut = 10;
    const TUint8 KMmConferenceCallSwapTimeOut = 10;
    const TUint8 KMmConferenceCallHangUpTimeOut = 60;

    //CMmSmsTsy
    const TUint8 KMmSmsReceiveMessageCancel = 10;
    const TUint8 KMmSmsAckNackMessage = 15;
    const TUint8 KMmSmsSendMessage = 55;
    const TUint8 KMmSmsSendMessageNoFdnCheckTimeOut = 55;
    const TUint8 KMmSmsSendSatMessage = 65;
    const TUint8 KMmSmsSetReceiveMode = 10;
    const TUint8 KMmSmsResumeSmsReception = 10;
    const TUint8 KMmSmsGetMessageStoreInfo = 120;
    const TUint8 KMmSmsReadAndStoreSmspList = 30;

    //SMS memory specific times
    const TUint8 KMmSimSmsGetInfo = 50;
    const TUint8 KMmSimSmsWriteSms = 10;
    const TUint8 KMmSimSmsReadDeleteSms = 15;
    const TUint8 KMmSimSmsDeleteAllSms = 30;
    const TUint8 KMmSimSmsReadAllSms = 120;

    //CMmBroadCastTsy
    const TUint8 KMmBroadcastReceiveMessageCancel = 10;
    const TUint8 KMmBroadcastSetFilter = 5;

    //CMmUssdTsy
    const TUint8 KMmUssdSendMessageTimeOut = 50;
    const TUint8 KMmUssdSendReleaseTimeOut = 50;
    const TUint8 KMmUssdSendMessageNoFdnCheckTimeOut = 50;

    //CMmStorageTsy
    // PBStore
    const TUint8 KMmPBStoreReadTimeOut = 10;
    const TUint8 KMmPBStoreWriteTimeOut = 10;
    const TUint8 KMmPBDeleteTimeOut = 10;
    const TUint8 KMmPBDeleteAllTimeOut = 75;
    const TUint8 KMmPBGetInfoTimeOut = 120;
    const TUint8 KMmPBStoreWriteToFirstEmptyLocTimeOut = 120;

    // ONStore
    const TUint8 KMmONStoreReadTimeOut = 10;
    const TUint8 KMmONStoreWriteTimeOut = 50;
    const TUint8 KMmONDeleteTimeOut = 20;
    const TUint8 KMmONDeleteAllTimeOut = 75;
    const TUint8 KMmONGetInfoTimeOut = 40;
    const TUint8 KMmONStoreAllTimeOut = 40;
    const TUint8 KMmONStoreWriteToFirstEmptyLocTimeOut = 40;

    // ENStore
    const TUint8 KMmENStoreReadTimeOut = 20;
    const TUint8 KMmENGetInfoTimeOut = 40;

    //CMmSIMTsy

    // Changed HomeZone parameter reading timeout
    // from 20 seconds to four minutes. Workaround for strange behaviour after
    // a full SIM refresh.
    const TUint8 KMmViagHomeZoneReadParamsTimeOut = 240;
    const TUint8 KMmViagHomeZoneReadCacheTimeOut = 20;
    const TUint8 KMmViagHomeZoneWriteCacheTimeOut = 20;
    const TUint8 KMmStartSimCbTopicBrowsingTimeOut = 3;
    const TUint8 KMmViagHomeZoneWriteSettingsTimeOut = 10;
    const TUint8 KMmDeleteSimCbTopicTimeOut = 10;

    //CMmWIMTsy

    const TUint8 KMmSendAPDUTimeOut = 20;
    const TUint8 KMmSimWarmResetTimeOut = 10;
    const TUint8 KMmGetATRTimeOut = 10;
    const TUint8 KMmGetSimCardReaderStatusTimeOut = 10;
    const TUint8 KMmSendAPDUV2TimeOut = 10;
    const TUint8 KMmPowerSimOnTimeOut = 10;
    const TUint8 KMmPowerSimOffTimeOut = 10;

    //CMmSimLockTsy

    const TUint8 KMmActivateSimLockTimeOut = 20;
    const TUint8 KMmDeactivateSimLockTimeOut = 20;

    //Packet Data session
    //Packet Data Related timeout values can be find from standard document:
    //    3GPP TS 24.008 V3.12.0 (2002-06)
    //
    //CMmPacketServiceTsy
    // Attach timer T3310, value 15s, Retransmission 5 times
    const TUint8 KMmPacketServiceAttachTimeOut = 85;
    // Detach timer T3321, value 15s, Retransmission 5 times
    const TUint8 KMmPacketServiceDetachTimeOut = 85;
    const TUint8 KMmPacketServiceGetNtwkRegStatusTimeOut = 10;
    const TUint8 KMmPacketServiceSetPreferredBearerTimeOut = 10;
    const TUint8 KMmPacketServiceRejectActivationRequest = 10;
    const TUint8 KMmPacketServiceSetAttachModeTimeOut = 10;
    const TUint8 KMmPacketServiceGetAttachModeTimeOut = 10;
    const TUint8 KMmPacketSetDefaultContextParamsTimeOut = 10;
    const TUint8 KMmPacketServiceGetMbmsNetworkServiceStatusTimeOut = 10;
    const TUint8 KMmPacketServiceUpdateMbmsMonitorServiceListTimeOut = 10;

    //CMmPacketContextTsy
    const TUint8 KMmPacketContextInitialiseContextTimeOut = 10;
    const TUint8 KMmPacketContextActivateTimeOut = 10;
    // Deactivate timer T3390, value 8s, Retransmission 5 times
    const TUint8 KMmPacketContextDeactivateTimeOut = 30;
    const TUint8 KMmPacketContextDeleteTimeOut = 10;
    const TUint8 KMmPacketContextSetConfigTimeOut = 10;
    const TUint8 KMmPacketContextGetDataVolumeTransferredTimeOut = 10;
    const TUint8 KMmPacketContextModifyActiveContext = 10;
    const TUint8 KMmPacketContextAddMediaAuthorization = 10;

    //CMmPacketQoSTsy
    const TUint8 KMmPacketQoSSetProfileParametersTimeOut = 10;

    //CMmPacketTsy
    const TUint8 KMmPacketContextSetAuthenticationInfoTimeOut = 5;

    //CMmCustomTsy
    const TUint8 KMmCustomTerminateCallTimeOut = 5;
    const TUint8 KMmCustomGetAlsBlockedTimeOut = 10;
    const TUint8 KMmCustomSetAlsBlockedTimeOut = 10;
    const TUint8 KMmCustomGetCipheringInfoTimeOut = 10;
    const TUint8 KMmCustomCallGsmBlackListClearTimeOut = 10;
    const TUint8 KMmCustomCheckEmergencyNumberTimeout = 10;
    const TUint8 KMmCustomGetOperatorNameTimeout = 20;
    const TUint8 KMmCustomGetProgrammableOperatorLogoTimeout = 20;
    const TUint8 KMmCustomGetOperatorName = 10;
    const TUint8 KMmCustomCheckTwoDigitDialTimeout = 10;
    const TUint8 KMmCustomResetNetServer = 20;
    const TUint8 KMmCustomReadSimFile = 10;
    const TUint8 KMmCustomGetLifeTimer = 10;
    const TUint8 KMmCustomIMSAuthenticate = 20;
    const TUint8 KMmCustomGetSimAuthenticationData = 10;
    const TUint8 KMmCustomSetDriveModeTimeOut = 5;
    const TUint8 KMmCustomTsyReadHSxPAStatusTimeOut = 10;
    const TUint8 KMmCustomTsyWriteHSxPAStatusTimeOut = 10;
    const TUint8 KMmCustomTsyGetIccCallForwardingStatusTimeOut = 10;
    const TUint8 KMmCustomTsyGetCellInfoTimeOut = 10;
    const TUint8 KMmCustomGetUSIMServiceSupportTimeOut = 10;
    const TUint8 KMmCustomTsyGetSystemNetworkBandTimeOut = 11;
   

#endif      //  REQHANDLE_TIMER

#endif      //  MMTSY_TIMEOUTDEFS_H

// End of File
