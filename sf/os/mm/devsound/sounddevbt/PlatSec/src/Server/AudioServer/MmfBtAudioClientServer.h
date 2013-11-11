// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFAUDIOCLIENTSERVER_H
#define MMFAUDIOCLIENTSERVER_H

#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfbase.h>
#include <mmf/server/sounddevice.h>

// opcodes used in message passing between client and server
enum TMMFAudioServerRequest
	{
	EMMFAudioLaunchRequests,
	EMMFDevSoundServerNameLength,
	EMMFDevSoundServerName
	};

enum TMMFDevSoundProxyRequest
	{
	EMMFDevSoundProxyInitialize1,
	EMMFDevSoundProxyInitialize2,
	EMMFDevSoundProxyInitialize3,
	EMMFDevSoundProxyInitialize4,
	EMMFDevSoundProxyCapabilities,
	EMMFDevSoundProxyConfig,
	EMMFDevSoundProxySetConfig,
	EMMFDevSoundProxyMaxVolume,
	EMMFDevSoundProxyVolume,
	EMMFDevSoundProxySetVolume,
	EMMFDevSoundProxyMaxGain,
	EMMFDevSoundProxyGain,
	EMMFDevSoundProxySetGain,
	EMMFDevSoundProxyPlayBalance,
	EMMFDevSoundProxySetPlayBalance,
	EMMFDevSoundProxyRecordBalance,
	EMMFDevSoundProxySetRecordBalance,
	EMMFDevSoundProxyReceiveICEvent,
	EMMFDevSoundProxyCancelReceiveICEvent,
	EMMFDevSoundProxyReceiveBTBFEvent,
	EMMFDevSoundProxyCancelReceiveBTBFEvent,
	EMMFDevSoundProxyReceiveBTBEEvent,
	EMMFDevSoundProxyCancelReceiveBTBEEvent,
	EMMFDevSoundProxyReceivePEEvent,
	EMMFDevSoundProxyCancelReceivePEEvent,
	EMMFDevSoundProxyReceiveREEvent,
	EMMFDevSoundProxyCancelReceiveREEvent,
	EMMFDevSoundProxyReceiveTFEvent,
	EMMFDevSoundProxyCancelReceiveTFEvent,
	EMMFDevSoundProxyReceiveSETCEvent,
	EMMFDevSoundProxyCancelReceiveSETCEvent,
	EMMFDevSoundProxyPlayInit,
	EMMFDevSoundProxyRecordInit,
	EMMFDevSoundProxyPlayData,
	EMMFDevSoundProxyRecordData,
	EMMFDevSoundProxyStop,
	EMMFDevSoundProxyPause,
	EMMFDevSoundProxyPlayTone,
	EMMFDevSoundProxyPlayDualTone,
	EMMFDevSoundProxyPlayDTMFString,
	EMMFDevSoundProxyPlayDTMFStringLength,
	EMMFDevSoundProxyPlayToneSequence,
	EMMFDevSoundProxyPlayFixedSequence,
	EMMFDevSoundProxySetDTMFLengths,
	EMMFDevSoundProxySetVolumeRamp,
	EMMFDevSoundProxyGetSupportedInputDataTypes,
	EMMFDevSoundProxyGetSupportedOutputDataTypes,
	EMMFDevSoundProxyCopyFourCCArrayData,
	EMMFDevSoundProxyGetRecordedBuffer,
	EMMFDevSoundProxySamplesRecorded,
	EMMFDevSoundProxySamplesPlayed,
	EMMFDevSoundProxySetToneRepeats,
	EMMFDevSoundProxySetPrioritySettings,
	EMMFDevSoundProxyFixedSequenceName,
	EMMFDevSoundProxyConvertInit,
	EMMFDevSoundProxyConvertData,
	EMMFDevSoundProxyCustomInterface,
	EMMFDevSoundProxyFixedSequenceCount,
	EMMFDevSoundProxyRequestResourceNotification,
	EMMFDevSoundProxyCancelRequestResourceNotification,
	EMMFDevSoundProxyGetResourceNotificationData,	
	EMMFDevSoundProxyWillResumePlay,
	EMMFDevSoundProxySetClientThreadInfo
	};

class TMMFDevSoundProxyHwBuf
/**
*@internalTechnology
*/
	{
public:
	TUid iBufferType;
	TInt iRequestSize;
	TBool iLastBuffer;
	TUint iBufferSize;
	TBufC8<1024> iData;
	};

typedef TPckgBuf<TMMFDevSoundProxyHwBuf> TMMFDevSoundProxyHwBufPckg;


class TMMFDevSoundProxySettings
/**
*@internalTechnology
*/
	{
public:
	TMMFState iMode;
	TUid iHWDev;
	TFourCC iDesiredFourCC;
	TMMFCapabilities iCaps;
	TMMFCapabilities iConfig;
	TInt iMaxVolume;
	TInt iVolume;
	TInt iMaxGain;
	TInt iGain;
	TInt iLeftPercentage;
	TInt iRightPercentage;
	TInt iFrequencyOne;
	TInt iFrequencyTwo;
	TTimeIntervalMicroSeconds iDuration;
	TTimeIntervalMicroSeconds32 iToneOnLength;
	TTimeIntervalMicroSeconds32 iToneOffLength;
	TTimeIntervalMicroSeconds32 iPauseLength;
	TUid iNotificationEventUid;
	};


typedef TPckgBuf<TMMFDevSoundProxySettings> TMMFDevSoundProxySettingsPckg;
const TInt KMaxFixedSequenceNameLength = 1024;

#endif
