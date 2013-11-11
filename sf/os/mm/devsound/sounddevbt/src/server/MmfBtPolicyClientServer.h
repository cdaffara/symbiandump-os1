// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFBTPOLICYCLIENTSERVER_H
#define MMFBTPOLICYCLIENTSERVER_H

#include <mmf/common/mmfbase.h>

// opcodes used in message passing between client and server
enum TMMFPolicyServerRequest
	{
	EMMFPolicySetDevSoundInfo,
	EMMFPolicyMakeRequest,
	EMMFPolicyCancelMakeRequest,
	EMMFPolicyUpdateState,
	EMMFPolicyReceiveEvents,
	EMMFPolicyCancelReceiveEvents,
	EMMFPolicyGetPlayFormatsSupported,
	EMMFPolicyGetRecordFormatsSupported,
	EMMFPolicyGetPlayFormat,
	EMMFPolicyGetRecordFormat,
	EMMFPolicyLaunchRequests,
	EMMFPolicyRequestResourceNotification,
	EMMFPolicyCancelRequestResourceNotification,
	EMMFPolicyStopNotification,
	EMMFPolicyGetResourceNotificationEvent
	};

enum TMMFAudioPolicyState
	{
	EMMFStatePlayData,
	EMMFStateRecordData,
	EMMFStatePlayTone,
	EMMFStatePlayDTMFString,
	EMMFStatePlayToneSequence,
	EMMFStateStopped,
	EMMFStateClosed,
	EMMFStatePaused,
	EMMFStatePlayDualTone,
	EMMFStateNotified,
	EMMFStateCompleted
	};


class TMMFAudioPolicyPrioritySettings
/**
*@internalTechnology
*/
	{
public:
	TInt iPriority;
	TMdaPriorityPreference iPref;
	TMMFAudioPolicyState iState; 
	TBool iCapabilities;
	};
	
class TMMFResourceNotificationSettings
/**
*@internalTechnology
*/
	{
public:
	TUid iNotificationUid;
	TBufC8<256> iNotificationDelay;
	};

typedef TPckgBuf<TMMFAudioPolicyPrioritySettings> TMMFAudioPolicyPrioritySettingsPckg;
typedef TPckgBuf<TMMFResourceNotificationSettings> TMMFAudioPolicyResourceNotificationSettingsPckg;

#endif
