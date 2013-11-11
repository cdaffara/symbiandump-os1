// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the test suite class implementation for A3F DevSound Integration tests.
// 
//

#include "tsi_a3f_devsound_testsuite.h"
#include "tsi_a3f_devsound_record.h"
#include "tsi_a3f_devsound_play.h"
#include "tsi_a3f_devsound_tone.h"

/*
 *
 * NewTestSuiteL is exported at ordinal 1
 * - this provides the interface to allow schedule test to create instances of this test suite
 *
 */
// Entry function - create a test suite object
EXPORT_C CTestSuiteA3FDevSound* CTestSuiteA3FDevSound::NewTestSuiteL()
	{
	return CTestSuiteA3FDevSound::NewL();

	}

/*
 *
 * NewL
 *
 */
CTestSuiteA3FDevSound* CTestSuiteA3FDevSound::NewL()
	{
	CTestSuiteA3FDevSound* result = new (ELeave) CTestSuiteA3FDevSound();
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(result);
	return result;

	}

/*
 *
 * CTestSuiteA3FDevSound - Constructor
 *
 */
CTestSuiteA3FDevSound::CTestSuiteA3FDevSound()
	{
	iSuiteName = _L("TSI_A3F_DEVSOUND");

	}

/*
 *
 * ~CTestSuiteA3FDevSound - Destructor
 *
 */
CTestSuiteA3FDevSound::~CTestSuiteA3FDevSound()
	{
	}

/*
 * GetVersion
 * - Get test suite version
 *
 */
TPtrC CTestSuiteA3FDevSound::GetVersion() const
	{
	_LIT(KTxtVersion, "1.0");
	return KTxtVersion();

	}

/*
 *
 * InitialiseL - Constructor for test suite
 * - creates all the test steps and stores them inside CTestSuiteA3FDevSound
 *
 */
void CTestSuiteA3FDevSound::InitialiseL()
	{
	const TUint32 KMMFFourCCCodeInvalid = 0x414d5121;
	TFourCC fourCCCodePCM8(KMMFFourCCCodePCM8);
	TFourCC fourCCCodePCMU8(KMMFFourCCCodePCMU8);
	TFourCC fourCCCodePCM16(KMMFFourCCCodePCM16);
	TFourCC fourCCCodeMuLAW(KMMFFourCCCodeMuLAW);
	TFourCC fourCCCodeInvalid(KMMFFourCCCodeInvalid);

	// to play AAC audio without OpenMAX IL HwDevice, use this FourCC code
	TFourCC fourCCCodeAAC(KMMFFourCCCodeAAC);

	// to play AAC audio with OpenMAX IL HwDevice, use this FourCC code
	// note: IL HwDevice has been classified as AAC2 which is a test FourCC code and not a real one
	const TUint32 KMMFFourCCCodeAAC2 = 0x32434141; //('A','A','C','2')
	TFourCC fourCCCodeAAC2(KMMFFourCCCodeAAC2);

	// DevSound PCM - Play test cases

	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0001"), _L("SectionPCM-0001"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0002"), _L("SectionPCM-0002"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0003"), _L("SectionPCM-0003"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0004"), _L("SectionPCM-0004"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0005"), _L("SectionPCM-0005"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0006"), _L("SectionPCM-0006"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0007"), _L("SectionPCM-0007"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0008"), _L("SectionPCM-0008"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0009"), _L("SectionPCM-0009"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0010"), _L("SectionPCM-0010"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0011"), _L("SectionPCM-0011"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0012"), _L("SectionPCM-0012"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0013"), _L("SectionPCM-0013"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0014"), _L("SectionPCM-0014"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlayEmpty::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0015"), _L("SectionPCM-0015"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0016"), _L("SectionPCM-0016"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0017"), _L("SectionPCM-0017"), fourCCCodeMuLAW));
	AddTestStepL(RStepA3FDevSoundPlayInvalidConfig::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0018"), _L("SectionPCM-0018"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlayInvalidConfig::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0019"), _L("SectionPCM-0019"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlayInvalidFourCC::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0020"), fourCCCodeInvalid));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0021"), _L("SectionPCM-0021"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlayEOFPlayMultipleTimes::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0022"), _L("SectionPCM-0022")));
	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0023"), _L("SectionPCM-0023")));
	AddTestStepL(RStepA3FDevSoundPlayStopMultipleTimes::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0024"), _L("SectionPCM-0024"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundReadConfigDuringPlay::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0025"), _L("SectionPCM-0025"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundSetVolPlayStopGetVol::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0026"), _L("SectionPCM-0026"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundFlushBuffersPaused::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0027"), _L("SectionPCM-0027"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundInvalidStatePlay::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0028"), _L("SectionPCM-0028"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundFlushBuffersPlaying::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0029"), _L("SectionPCM-0029"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundInitializeWhilePlaying::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0030"), _L("SectionPCM-0030"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundInitializeWithHwDeviceUID::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0031")));
	AddTestStepL(RStepA3FDevSoundSetBalanceDuringPlay::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0032"), _L("SectionPCM-0032"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundGetSampleNumsAfterStop::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0033"), _L("SectionPCM-0033"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundInititalizeDuringInitialize::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0034"), _L("SectionPCM-0034"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundPlayInitDuringPlayInit::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0035"), _L("SectionPCM-0035"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundVolumeCrop::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0036"), _L("SectionPCM-0036"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundInitializeForConverting::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0037"), _L("SectionPCM-0037"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundCancelInitializePlay::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0038"), _L("SectionPCM-0038"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundNegCancelInitializePlay::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0039"), _L("SectionPCM-0039"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundEmptyBuffersInCreatedState::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0040")));
	AddTestStepL(RStepA3FDevSoundEmptyBuffersInInitializedState::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0041"), _L("SectionPCM-0041"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundEmptyBuffersInInitializingState::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-I-0042"), _L("SectionPCM-0042"), fourCCCodePCM16));

	// DevSound PCM - Long Play test cases

	AddTestStepL(RStepA3FDevSoundPlaySimple::NewL(_L("MM-A3F-DEVSOUND-PLAY-PCM-LONG-I-0001"), _L("SectionPCM-Long-0001"), fourCCCodePCM16));

	// DevSound PCM - Record test cases

	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0001"), _L("SectionPCM-0001"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordMultipleTimes::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0002"), _L("SectionPCM-0002"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0003"), _L("SectionPCM-0003"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0004"), _L("SectionPCM-0004"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0005"), _L("SectionPCM-0005"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0006"), _L("SectionPCM-0006"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0007"), _L("SectionPCM-0007"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0008"), _L("SectionPCM-0008"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0009"), _L("SectionPCM-0009"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0010"), _L("SectionPCM-0010"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordSimple::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0011"), _L("SectionPCM-0011"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundRecordAndPlay::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0012"), _L("SectionPCM-0012"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundSetGainDuringRecord::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0013"), _L("SectionPCM-0013"), fourCCCodePCM8));
	AddTestStepL(RStepA3FDevSoundSetGainDuringRecord::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0014"), _L("SectionPCM-0014"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundInvalidModeRecord::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0015"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundInitializeWhileRecording::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0016"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundCancelInitializeRecord::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0017"), _L("SectionPCM-0017"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundEmptyBuffersInitializedToRecord::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0018"), _L("SectionPCM-0018"), fourCCCodePCM16));
	AddTestStepL(RStepA3FDevSoundEmptyBuffersWhileRecording::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-I-0019"), fourCCCodePCM16));

	// DevSound PCM - Long Record test cases

	AddTestStepL(RStepA3FDevSoundRecordLong::NewL(_L("MM-A3F-DEVSOUND-REC-PCM-LONG-I-0001"), _L("SectionPCM-Long-0001"), fourCCCodePCM16));

	// DevSound - Tone test cases

	AddTestStepL(RStepA3FDevSoundDTMFTonePlay::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0001")));
	AddTestStepL(RStepA3FDevSoundNormalAndDualTonePlay::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0002"), _L("SectionTONE-0002")));
	AddTestStepL(RStepA3FDevSoundNormalAndDualTonePlay::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0003"), _L("SectionTONE-0003")));
	AddTestStepL(RStepA3FDevSoundInitializeWhileTonePlaying::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0004")));
	AddTestStepL(RStepA3FDevSoundNotSupportedPlayFixedSeq::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0005")));
	AddTestStepL(RStepA3FDevSoundInvalidInitializeForTone::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0006")));
	AddTestStepL(RStepA3FDevSoundCancelInitializeTone::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0007"), _L("SectionTONE-0007")));
	AddTestStepL(RStepA3FDevSoundEmptyBuffersInitializedForTone::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0008"), _L("SectionTONE-0008")));
	AddTestStepL(RStepA3FDevSoundEmptyBuffersWhileTonePlaying::NewL(_L("MM-A3F-DEVSOUND-TONE-I-0009")));

	}

// End of File
