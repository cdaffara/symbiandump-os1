// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CHAR_A3F_DEVSOUND_COMPLIANCESUITECONSTANTS_H
#define CHAR_A3F_DEVSOUND_COMPLIANCESUITECONSTANTS_H


// constant declarations
const TInt KMaxStringLength = 255;	// Used to define DTMF length
const TInt KSizeBuf = 256;
const TInt KMicrosecsInTenSec = 10000000;
const TInt KMicroSecsInOneSec = 1000000;
const TInt KMicroSecsTwoSec = 2000000;
const TInt KMinimumPriority = -100;
const TInt KMaximumPriority = 100;
const TInt KFourCCMaxBitDisplacement = 24;
const TInt K8Bits = 8;
const TInt KTFourCC = 4;
const TInt KMaxSampleRateStringLength = 21;
const TInt KMaxChannelsStringLength = 10;
const TInt KExtraVolume = 50;
// To compute elapsed time for some timed test cases
const TInt64 KPlayVarianceTime = 1500000; // 1.5 seconds
const TInt KMaxSpeakerBalanceValue = 100;
const TInt KMinSpeakerBalanceValue = 0;
const TInt KMaxRecordBalanceValue = 100;


_LIT(KMsgDeleteDevsound, "---- Deleting DevSound object ----");
_LIT(KMsgErrorGetParameter,"Error in getting parameter %S from INI file");
_LIT(KMsgErrorDevSoundCallback,"DevSound called %S() callback with error = %d");
_LIT(KMsgErrorFourccLength,"Error in getting parameter length from INI file %d");

_LIT(KInitializeCompleteText, "InitializeComplete");
_LIT(KPlayErrorText, "PlayError");
_LIT(KRecordErrorText, "RecordError");
_LIT(KToneFinishedText, "ToneFinished");

// Section error name keys
_LIT(KExpectedError, "ExpectedError");

_LIT(KEMMFSampleRate8000Hz,"EMMFSampleRate8000Hz");
_LIT(KEMMFSampleRate11025Hz,"EMMFSampleRate11025Hz");
_LIT(KEMMFSampleRate16000Hz,"EMMFSampleRate16000Hz");
_LIT(KEMMFSampleRate22050Hz,"EMMFSampleRate22050Hz");
_LIT(KEMMFSampleRate32000Hz,"EMMFSampleRate32000Hz");
_LIT(KEMMFSampleRate44100Hz,"EMMFSampleRate44100Hz");
_LIT(KEMMFSampleRate48000Hz,"EMMFSampleRate48000Hz");
_LIT(KEMMFSampleRate88200Hz,"EMMFSampleRate88200Hz");
_LIT(KEMMFSampleRate96000Hz,"EMMFSampleRate96000Hz");
_LIT(KEMMFSampleRate12000Hz,"EMMFSampleRate12000Hz");
_LIT(KEMMFSampleRate24000Hz,"EMMFSampleRate24000Hz");
_LIT(KEMMFSampleRate64000Hz,"EMMFSampleRate64000Hz");

_LIT(KEMMFMono,"EMMFMono");
_LIT(KEMMFStereo,"EMMFStereo");

// Section name keys
_LIT(KSampleRate, "SampleRate");
_LIT(KChannel, "Channel");
_LIT(KPriority, "Priority");
_LIT(KVolume, "Volume");
_LIT(KExVolume, "ExVolume");
_LIT(KGain, "Gain");
_LIT(KExGain, "ExGain");
_LIT(KVolumeRamp, "VolumeRamp");
_LIT(KRepeatCount, "RepeatCount");
_LIT(KRepeatTrailingSilence, "RepeatTrailingSilence");
_LIT(KToneOnLength, "ToneOnLength");
_LIT(KToneOffLength, "ToneOffLength");
_LIT(KPauseLength, "PauseLength");
_LIT(KFilename, "Filename");
_LIT(KOtherFilename, "OtherFilename");
_LIT(KDuration, "Duration");
_LIT(KFrequencyTone1, "FrequencyTone1");
_LIT(KFrequencyTone2, "FrequencyTone2");
_LIT(KDTMFString, "DTMFString");
_LIT(KInvalidToneSequence,"InvalidToneSequence");
_LIT(KLSpeakerBalance, "LSpeakerBalance");
_LIT(KRSpeakerBalance, "RSpeakerBalance");
_LIT(KLRecordBalance, "LRecordBalance");
_LIT(KRRecordBalance, "RRecordBalance");
_LIT(KExLSpeakerBalance, "ExLSpeakerBalance");
_LIT(KExRSpeakerBalance, "ExRSpeakerBalance");
_LIT(KExLRecordBalance, "ExLRecordBalance");
_LIT(KExRRecordBalance, "ExRRecordBalance");
_LIT(KFourccCode, "FourccCode");
_LIT(KExSamplesPlayed, "ExSamplesPlayed");
_LIT(KExSamplesRecorded, "ExSamplesRecorded");
_LIT(KExCapChannels, "ExCapChannels");
_LIT(KExCapRate, "ExCapRate");


enum TPanicCodes
	{
	EFsmIncorrectErrorPassed = 0,
	EInvalidCallbackCall
	};

// DevSound events
enum TMmfDevSoundEvent
	{
	EEventInitialize = 0,
	EEventInitComplete,
	EEventBTBF,
	EEventBTBE,
	EResourceAvailable,
	EEventTimerComplete
	};

// DevSound states
enum TMmfDevSoundState
	{
	EStateInitial = 0,
	EStateCreated,
	EStateInitializing,
	EStatePlaying,
	EStateRecording,
	EStatePause,
	EStateResumePlaying
	};

// User defined "devSound client" error codes
enum TMmfDevSoundClientErrorCodes
	{
	EInvalidClientFSMEvent = 1,
	EInvalidClientFSMState,
	EReadFileErrorInClient,
	EWriteFileErrorInClient
	};

// Panic function
static void Panic(const TDesC &aCategory, TInt aReason)
	{
	User::Panic(aCategory, aReason);
	}

#ifdef MM_TB101_L
		// Valid data for tone sequence. This code should be used for non-ref tests. This is a valid tone sequence for HW
		const TUint8 KFixedSequenceTestSequenceDataX[]=
		{
		0x02, 0x4A, 0x3A, 0x69, 0x39, 0xBD, 0xAD, 0xA5,
		0x84, 0x81, 0xD1, 0xD5, 0xB9, 0x94, 0x04, 0x00,
		0x27, 0x34, 0x41, 0x0B, 0x2A, 0x82, 0x68, 0x51,
		0x72, 0x19, 0x22, 0xC9, 0x21, 0x47, 0x10, 0x4C,
		0x85, 0x48, 0x71, 0x06, 0x90, 0x48, 0x85, 0x48,
		0x68, 0x20, 0x00
		};
#else
	// Valid data for tone sequence for reference platform. Below tone data may not work for HW devices
	const TUint8 KFixedSequenceTestSequenceDataX[]=
		{
		0x53, 0x51, 0x4E, 0x43, // HEADER
		0xFE, 0xFF, // -2 STARTLOOP INDICATOR
		0x0A, 0x00,  // Number of times that the sequence will be played
		/* Tone Data constitutes the following:
		  ToneDur,    Freq1,      Vol1,       Freq2       Vol2
		The sequence is arranged in blocks of 8 bits to represent 16 bit values,
		which allows more dynamic range, frequency and time values.
		The tag Low and High mean low and high byte block.
		Low    Hi    Low   Hi   Low   Hi    Low   Hi    Low   Hi
		*/
		0x64, 0x00, 0xB8, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0xD2, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0xEE, 0x01, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0x0B, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0x2A, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0x4B, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0x6E, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0x93, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0xBA, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0xE4, 0x02, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0x3F, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0x70, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0xA4, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0xDC, 0x03, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		0x64, 0x00, 0x17, 0x04, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
		// Silence
		// ToneDur,    Freq1,      Vol1,       Freq2       Vol2
		0x70, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFD, 0xFF, // -3 ENDOFLOOP INDICATOR
		// Another silence
		// ToneDur,    Freq1,      Vol1,       Freq2       Vol2
		0x70, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF, 0xFF // -1 SEQ_CMD_RET INDICATOR
		};


#endif//#if defined(MM_TB101_L)


#endif	// CHAR_A3F_DEVSOUND_COMPLIANCESUITECONSTANTS_H

