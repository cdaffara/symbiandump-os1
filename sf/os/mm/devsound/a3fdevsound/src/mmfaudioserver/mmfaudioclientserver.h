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



#ifndef MMFAUDIOCLIENTSERVER_H
#define MMFAUDIOCLIENTSERVER_H

//  INCLUDES
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfbase.h>
#include <mmf/server/sounddevice.h>

// DATA TYPES
// opcodes used in message passing between client and server
enum TMMFAudioServerRequest
	{
	EMMFAudioLaunchRequests,
	EMMFDevSoundSessionHandle
	};

// opcodes used in message passing between client and server for DevSound
// operations.
enum TMMFDevSoundProxyRequest
	{
	EMMFDevSoundProxyPostOpen,
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
	EMMFDevSoundProxyICEvent,
	EMMFDevSoundProxyBTBFEvent,
	EMMFDevSoundProxyBTBEEvent,
	EMMFDevSoundProxyPEEvent,
	EMMFDevSoundProxyREEvent,
	EMMFDevSoundProxyTFEvent,
	EMMFDevSoundProxySETCEvent,
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
	EMMFDevSoundProxySamplesRecorded,
	EMMFDevSoundProxySamplesPlayed,
	EMMFDevSoundProxySetToneRepeats,
	EMMFDevSoundProxySetPrioritySettings,
	EMMFDevSoundProxyFixedSequenceName,
	EMMFDevSoundProxyCustomInterface,
	EMMFDevSoundProxyFixedSequenceCount,
	EMMFDevSoundProxyRequestResourceNotification,
	EMMFDevSoundProxyCancelRequestResourceNotification,
	EMMFDevSoundProxyGetResourceNotificationData,	
	EMMFDevSoundProxyWillResumePlay,
	EMMFDevSoundProxyEmptyBuffers,
	EMMFDevSoundProxyCancelInitialize,
	EMMFDevSoundProxyBTBFData,
	EMMFDevSoundProxyBTBEData,
	EMMFDevSoundProxySyncCustomCommand,
	EMMFDevSoundProxySyncCustomCommandResult,
	EMMFDevSoundProxyAsyncCustomCommand,
	EMMFDevSoundProxyAsyncCustomCommandResult,
	EMMFDevSoundCustomCommandCloseMuxDemuxPair,
	EMMFDevSoundProxyClose,
	EMMFDevSoundProxyPausedRecordCompleteEvent,
	EMMFDevSoundProxyGetTimePlayed,
	EMMFDevSoundProxySetClientThreadInfo,
	EMMFDevSoundProxyIsResumeSupported,
	EMMFDevSoundProxyResume,
	EMMFDevSoundProxyMarkEvent
	};

// Signals that the message is destined for the DevSound
const TUid KUidInterfaceMMFDevSound = {0x10207ABA};
const TInt KMMFObjectHandleDevSound = -3;

// Addresses the chunk states
enum TChunkOp
	{
	ENull,
	EOpen
	};

/**
*  A class representing CMMFBuffer information.
*  Buffers are used for both playback and recording.
*
*  @lib MmfDevSoundAdaptation.lib
*/
class TMMFDevSoundProxyHwBuf
	{
public:
	TUid iBufferType;
	TInt iRequestSize;
	TBool iLastBuffer;
	TUint iBufferSize;
	TChunkOp iChunkOp;
	};

typedef TPckgBuf<TMMFDevSoundProxyHwBuf> TMMFDevSoundProxyHwBufPckg;

/**
*  A class representing settings information used to send configuration
*  information to DevSound server.
*  Buffers are used for both playback and recording digital audio.
*
*  @lib MmfDevSoundAdaptation.lib
*/
class TMMFDevSoundProxySettings
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
	TUid iInterface;
	};

// Packages a DevSound object information into a modifiable buffer descriptor. 
// Provides a type safe way of transferring an object or data structure which is contained within a modifiable buffer descriptor. 
typedef TPckgBuf<TMMFDevSoundProxySettings> TMMFDevSoundProxySettingsPckg;

// Tone Fixed Sequence Max Length
const TInt KMaxFixedSequenceNameLength = 1024;

/**
* TMMFDevSoundQueueItem is used to pass asynch messages between the devsound
* proxy and server for the observer's BufferToBeFilled, BufferToBeEmptied,
* PlayError, RecordError, ToneFinished, InitializeComplete and
* SendEventToClient events.
*/
class TMMFDevSoundQueueItem
	{
public:
	TMMFDevSoundProxyRequest    iRequest;
	TMMFEventPckg               iEventPckg; // SendEventToClient message payload
	TInt                        iErrorCode; // InitComplete, ToneFinished, PlayError, RecordError message payload
	};

#endif	//MMFAUDIOCLIENTSERVER_H

// End of File
