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



#ifndef _MMTSY_NUMBEROFSLOTS_H_
#define _MMTSY_NUMBEROFSLOTS_H_

// CMmPhoneTsy 
const TUint8 KMmPhoneModemDetectedSlots = 2;
const TUint8 KMmPhoneCapsChangeNotificationSlots = 2;
const TUint8 KMmPhoneDTMFCapsChangeSlots = 2;
const TUint8 KMmPhoneStopInDTMFStringSlots = 2;
const TUint8 KMmPhoneNetworkRegistrationStatusChangeSlots = 4;
const TUint8 KMmPhoneModeChangeSlots = 2;
const TUint8 KMmPhoneCurrentNetworkChangeSlots = 2;
const TUint8 KMmPhoneNetworkSelectionSettingChangeSlots = 2;
const TUint8 KMmPhoneBatteryInfoChangeSlots = 2;
const TUint8 KMmPhoneSignalStrengthChangeSlots = 2;
const TUint8 KMmPhoneCallForwardingStatusChangeSlots = 2;
const TUint8 KMmPhoneCallBarringStatusChangeSlots = 2;
const TUint8 KMmPhoneCallWaitingStatusChangeSlots = 2;
const TUint8 KMmPhoneCCBSStatusChangeSlots = 2;
const TUint8 KMmPhoneAlternatingCallCapsChangeSlots = 2;
const TUint8 KMmPhoneAlternatingCallModeChangeSlots = 2;
const TUint8 KMmPhoneALSLineChangeSlots = 2;
const TUint8 KMmPhoneCostCapsChangeSlots = 2;
const TUint8 KMmPhoneCostInfoChangeSlots = 2;
const TUint8 KMmPhoneSecurityCapsChangeSlots = 2;
const TUint8 KMmPhoneLockInfoChangeSlots = 2;
const TUint8 KMmPhoneSecurityEventSlots = 2;
const TUint8 KMmPhoneMessageWaitingSlots = 2;
const TUint8 KMmPhoneFdnStatusChangeSlots = 2;
const TUint8 KMmPhoneIccAccessCapsChangeSlots = 2;
const TUint8 KMmPhoneNITZInfoChangeSlots = 2;
const TUint8 KMmPhoneDefaultPrivacyChangeSlots = 2;
const TUint8 KMmPhoneCallServiceCapsChangeSlots = 2;
const TUint8 KMmPhoneCallForwardingActiveSlots = 2;
const TUint8 KMmPhoneMulticallParamsChangeSlots = 1;
const TUint8 KMmPhoneIncomingCallTypeChangeSlots = 2;
const TUint8 KMmPhoneUUSSettingChangeSlots = 1;
const TUint8 KMmPhoneIccMessageWaitingIndicatorsChangeSlots = 2;
const TUint8 KMmPhoneIndicatorChangeSlots = 1;
const TUint8 KMmPhoneNetworkSecurityLevelChangeSlots = 1;
const TUint8 KMmPhoneSimRefreshNowSlots = 2;
const TUint8 KMmPhoneSecurityCodeInfoChangeSlots = 2;
const TUint8 KMmPhoneAPNListChangedSlots = 2;
const TUint8 KMmPhoneAPNControlListServiceStatusChangeSlots = 2;
const TUint8 KMmPhoneNotifySendNetworkServiceRequestSlots = 2;
const TUint8 KMmPhoneNotifyAllSendNetworkServiceRequestSlots = 2;
const TUint8 KMmPhoneCellInfoChangeSlots = 2;

//CMmLineTsy
const TUint8 KMmLineStatusChangeSlots = 2;
const TUint8 KMmLineCallAddedSlots = 2;
const TUint8 KMmLineCapsChangeSlots = 2;
const TUint8 KMmLineHookChangeSlots = 1;
const TUint8 KMmLineIncomingCallSlots = 2;
const TUint8 KMmLineMobileLineStatusChangeSlots = 2;

//CMmCallTsy
const TUint8 KMmCallHookChangeSlots = 1;
const TUint8 KMmCallStatusChangeSlots = 7;
const TUint8 KMmCallDurationChangeSlots = 10; 
const TUint8 KMmCallCapsChangeSlots = 7;
const TUint8 KMmCallCallEventSlots = 7;
const TUint8 KMmCallMobileCallStatusChangeSlots = 7;
const TUint8 KMmCallRemotePartyInfoChangeSlots = 7;
const TUint8 KMmCallPrivacyConfirmationSlots = 2;
const TUint8 KMmCallTrafficChannelConfirmationSlots = 2;
const TUint8 KMmCallHscsdInfoChangeSlots = 2;
const TUint8 KMmCallMobileDataCallCapsChangeSlots = 2;
const TUint8 KMmCallAlternatingCallSwitchSlots = 2;
const TUint8 KMmCallMobileCallCapsChangeSlots = 7;
const TUint8 KMmCallVoiceFallbackSlots = 1;
const TUint8 KMmCallUUSCapsChangeSlots = 1;
const TUint8 KMmCallReceiveUUISlots = 2;

//CMmConferenceCallTsy
const TUint8 KMmConferenceCallCapsChangeSlots = 2;
const TUint8 KMmConferenceCallStatusChangeSlots = 5;
const TUint8 KMmConferenceCallEventSlots = 3;

//CMmBroadCastTsy
const TUint8 KMmBroadcastMessagingReceiveMessageSlots = 10;
const TUint8 KMmBroadcastMessagingNotifyFilterSettingChangeSlots = 3;

//CMmStorageTsy 
const TUint8 KMmPhoneStoreNotifyStoreEventSlots = 3; 
const TUint8 KMmONStoreNotifyStoreEventSlots = 3; 
const TUint8 KMmENStoreNotifyStoreEventSlots = 3; 

//CMmSmsTsy
const TUint8 KMmSmsMessagingReceiveMessageSlots = 3;
const TUint8 KMmSmsMessagingNotifyReceiveModeChangeSlots = 3;
const TUint8 KMmSmsMessagingNotifyMoSmsBearerChangeSlots = 3;
const TUint8 KMmSmsMessagingNotifySmspListChangeSlots = 3;

//CMmCustomTsy
const TUint8 KMmCustomDefaultSlots = 2;
const TUint8 KMmCustomSimLockSlots = 1;
const TUint8 KMmCustomNotifyDtmfEventSlots = 5;

//CMmSsTsy
const TUint8 KMmCustomSsNotifySlots = 1;

//CMmUssdTsy
const TUint8 KMmUssdMessagingReceiveMessageSlots = 5;
const TUint8 KMmUssdMessagingNotifyNetworkReleaseSlots = 5;

//CMmSecurityTsy
const TUint8 KMmCustomIsBlockedSlots = 3;
const TUint8 KMmCustomCheckSecurityCodeSlots = 2;
//Packet Data session
//CMmPacketServiceTsy
const TUint8 KMmPacketServiceNotifyStatusChangeSlots = 5;
const TUint8 KMmPacketServiceNotifyDynamicCapsChangeSlots = 4;
const TUint8 KMmPacketServiceNotifyContextAddedSlots = 4;
const TUint8 KMmPacketServiceNotifyContextActivationRequestedSlots = 3;
const TUint8 KMmPacketServiceNotifyChangeOfNtwkRegStatusSlots = 5;
const TUint8 KMmPacketServiceNotifyMSClassChangeSlots = 3;
const TUint8 KMmPacketServiceNotifyMBMSServiceStatusSlots = 2;  

//CMmPacketContextTsy
const TUint8 KMmPacketContextNotifyConfigChangedSlots = 2;
const TUint8 KMmPacketContextNotifyConnectionSpeedChangeSlots = 2;
const TUint8 KMmPacketContextNotifyStatusChangeSlots = 5;
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
const TUint8 KMmPacketContextNotifyConnectionInfoChangeSlots = 3;
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

//CMmPacketQoSTsy
const TUint8 KMmPacketQoSNotifyParameterChangedSlots = 2;

#endif      //  _MMTSY_NUMBEROFSLOTS_H_
            
// End of File
