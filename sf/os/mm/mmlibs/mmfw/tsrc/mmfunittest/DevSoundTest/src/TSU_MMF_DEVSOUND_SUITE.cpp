// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This main DLL entry point for the TSU_MMF_DEVSOUND_SUITE.dll
// 
//

// System includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_DEVSOUND_SUITE.h"
#include "TSU_MMF_DEVSOUND_STEP.h"


/** 
 *
 * NewTestSuiteL
 *	NewTestSuite is exported at ordinal 1
 *	this provides the interface to allow schedule test
 *	to create instances of this test suite
 * @result CTestSuiteDevSound*
 *
 */
EXPORT_C CTestSuiteDevSound* NewTestSuiteL() 
	{ 
	CTestSuiteDevSound* result = new (ELeave) CTestSuiteDevSound;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
	}

/**
 *
 * ~CTestSuiteDevSound
 *
 */
CTestSuiteDevSound::~CTestSuiteDevSound() 
	{}

/**
 *
 * GetVersion
 *	Get Test Suite version
 * @result TPtrC
 *
 */
TPtrC CTestSuiteDevSound::GetVersion( void ) const
	{
	_LIT(KTxtVersion,"1.08");
	return KTxtVersion();
	}


/**
 *
 * AddTestStepL
 *	Add a test step into the suite
 * @param aPtrTestStep
 *	Test step pointer
 *
 */
void CTestSuiteDevSound::AddTestStepL( CTestStepDevSound* aPtrTestStep )
	{
	// test steps contain a pointer back to the suite which owns them
	aPtrTestStep->SetTestSuite(this);

	// add the step using the base class method
	CTestSuite::AddTestStepL(aPtrTestStep);
	}

/**
 *
 * InitialiseL
 *	Constructor for test suite
 *	this creates all the test steps and 
 *	stores them inside CTestSuiteDevSound
 *
 */
void CTestSuiteDevSound::InitialiseL(void)
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_DEVSOUND_SUITE");

	//Initialize default pcm16 play
	AddTestStepL(new(ELeave) CTestStepDevSoundInitializePlay());
	//Initialize with HwDevice uid play
	AddTestStepL(new(ELeave) CTestStepDevSoundInitializeHwDeviceUidPlay());
	//Initialize with fourCC play
	AddTestStepL(new(ELeave) CTestStepDevSoundInitializeFourCCPlay());
	//Initialize default pcm16 record 
	AddTestStepL(new(ELeave) CTestStepDevSoundInitializeRecord());
	//Initialize with HwDevice uid record
	AddTestStepL(new(ELeave) CTestStepDevSoundInitializeHwDeviceUidRecord());
	//Initialize with fourCC record
	AddTestStepL(new(ELeave) CTestStepDevSoundInitializeFourCCRecord());
	//Initialize with invalid fourCC code
	AddTestStepL(new(ELeave) CTestStepDevSoundInitializeInvalidFourCC());
	//Check Caps
	AddTestStepL(new(ELeave) CTestStepDevSoundCheckCaps());
	//Play Simple Tone
	AddTestStepL(new(ELeave) CTestStepDevSoundSimpleTone());
	//Play DTMF String
	AddTestStepL(new(ELeave) CTestStepDevSoundDTMFTones());
	//Play Three Simple Tones with 0, MaxVolume/2, and MaxVolume
	AddTestStepL(new(ELeave) CTestStepDevSoundSimpleToneChangeVolume());
	//Play DTMF String, Invalid Characters
	AddTestStepL(new(ELeave) CTestStepDevSoundDTMFTonesInvalidStrings());
	//Play Simple Tone, invalid Frequency
	AddTestStepL(new(ELeave) CTestStepDevSoundSimpleToneInvalidFreq());
	//Play Simple Tone, invalid Duration
	AddTestStepL(new(ELeave) CTestStepDevSoundSimpleToneInvalidDuration());
	//PlayInit
	AddTestStepL(new(ELeave) CTestStepDevSoundPlayInit());
	//Play 1 buffer of PCM, 16Bit, 8kHz, Mono, Raw, data
	AddTestStepL(new(ELeave) CTestStepDevSoundPlay1BufferPCM16());
	//Play 5 buffers of PCM, 16Bit, 8kHz, Mono, Raw, data
	AddTestStepL(new(ELeave) CTestStepDevSoundPlay5BuffersPCM16());
	//Play PCM, 16Bit, 8kHz, Mono, Raw, data until EOF
	AddTestStepL(new(ELeave) CTestStepDevSoundPlayEOFPCM16());
	//Play 1 buffer of PCM, 8Bit, 8kHz, Mono, Raw, data
	AddTestStepL(new(ELeave) CTestStepDevSoundPlay1BufferPCM8());
	//Play 5 buffers of PCM, 16Bit, 8kHz, Mono, Raw, data
	AddTestStepL(new(ELeave) CTestStepDevSoundPlay5BuffersPCM8());
	//Play PCM, 16Bit, 8kHz, Mono, Raw, data until EOF
	AddTestStepL(new(ELeave) CTestStepDevSoundPlayEOFPCM8());
	//Play Garbage until EOF
	AddTestStepL(new(ELeave) CTestStepDevSoundPlayGarbageEOF());
	//MaxGain test
	AddTestStepL(new(ELeave) CTestStepDevSoundMaxGain());
	//MaxVolume test
	AddTestStepL(new(ELeave) CTestStepDevSoundMaxVolume());
	//SetGain test
	AddTestStepL(new(ELeave) CTestStepDevSoundSetGain());
	//SetVolume test
	AddTestStepL(new(ELeave) CTestStepDevSoundSetVolume());
	//Record 1 buffer of PCM, 16Bit, 8kHz, Mono, Raw, data
	AddTestStepL(new(ELeave) CTestStepDevSoundRecord1BufferPCM16());
	//Record 10 buffers of PCM, 16Bit, 8kHz, Mono, Raw, data
	AddTestStepL(new(ELeave) CTestStepDevSoundRecord10BuffersPCM16());
	//Record 1 buffer of PCM, 8Bit, 8kHz, Mono, Raw, data
	AddTestStepL(new(ELeave) CTestStepDevSoundRecord1BufferPCM8());
	//Record 10 buffers of PCM, 8Bit, 8kHz, Mono, Raw, data
	AddTestStepL(new(ELeave) CTestStepDevSoundRecord10BuffersPCM8());
	//SetConfigL stereo
	AddTestStepL(new(ELeave) CTestStepDevSoundSetConfigStereo());
	//SetConfigL sample rate
	AddTestStepL(new(ELeave) CTestStepDevSoundSetConfigSampleRate());
	//GetSupportedInputDataTypesL test
	AddTestStepL(new(ELeave) CTestStepDevSoundGetSupportedInputDataTypes());
	//GetSupportedOutputDataTypesL test
	AddTestStepL(new(ELeave) CTestStepDevSoundGetSupportedOutputDataTypes());
	//GetSupportedInputDataTypesL OOM test
	AddTestStepL(new(ELeave) CTestStepDevSoundGetSupportedInputDataTypesOOM());
	//GetSupportedOutputDataTypesL OOM test
	AddTestStepL(new(ELeave) CTestStepDevSoundGetSupportedOutputDataTypesOOM());
	//RecordInit test with MultimediaDD capability
	AddTestStepL(new(ELeave) CTestStepDevSoundRecordInitNoUECap(ETrue));
	//RecordInit test with no capabilities
	AddTestStepL(new(ELeave) CTestStepDevSoundRecordInitNoUECap(EFalse));
	//RecordInit test with All capabilities
	AddTestStepL(new(ELeave) CTestStepDevSoundRecordInitWithUECap(ETrue));
	//RecordInit test with UserEnvironment capability
	AddTestStepL(new(ELeave) CTestStepDevSoundRecordInitWithUECap(EFalse));
	//RecordData test with All capabilities
	AddTestStepL(new(ELeave) CTestStepDevSoundRecordDataWithUECap(ETrue));
	//RecordData test with UserEnvironment capability
	AddTestStepL(new(ELeave) CTestStepDevSoundRecordDataWithUECap(EFalse));
	//Test added as a part of DEF054803 - To check whether the sample rate is set to 8000Hz for tone play
	AddTestStepL(new(ELeave) CTestStepDevSoundCheckSampleRateforTone());
	//Tests added as a part of INC102615 - To check policy update after pause
	CTestSuite::AddTestStepL(CTestStepDevSoundPlayPauseTone::NewL(this));
	CTestSuite::AddTestStepL(CTestStepDevSoundPlayPauseTone2::NewL(this));
	CTestSuite::AddTestStepL(CTestStepDevSoundPlayPauseTone3::NewL(this));
	//Test added as a part of PDEF108583 - To check tone playing pre-emption
	CTestSuite::AddTestStepL(RTestStepDevSoundToneTone::NewL(this));
	//Test added as a part of PDEF108583 - To check recording pre-emption
	CTestSuite::AddTestStepL(RTestStepDevSoundRecordRecord::NewL(this));
	//Tests added as a part of INC111338 - To check continuing operation after loss and regain of policy
	CTestSuite::AddTestStepL(RTestStepDevSoundToneReplay::NewL(this));
	CTestSuite::AddTestStepL(RTestStepDevSoundRecordRerecord::NewL(this));
	CTestSuite::AddTestStepL(RTestStepDevSoundPlayReplay::NewL(this));

	//CustomInterface framework test
	CTestSuite::AddTestStepL(RTestStepBasicCustomInterface::NewL());
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	//PREQ1540 DevSound Server-Side alloc stress test via custom interface
	CTestSuite::AddTestStepL(new(ELeave) CTestStepDevSoundServerSideAlloc());
#endif
	//CustomInterface framework test alloc
	CTestSuite::AddTestStepL(RTestStepBasicCustomInterfaceAlloc::NewL());
	//DevSound EmptyBuffers custom interface positive test
	CTestSuite::AddTestStepL(new(ELeave) CTestStepDevSoundEmptyBuffersPos());
	//DevSound EmptyBuffers custom interface negative test while recording
	CTestSuite::AddTestStepL(new(ELeave) CTestStepDevSoundEmptyBuffersNeg1());
	//DevSound EmptyBuffers custom interface negative test without starting playback
	CTestSuite::AddTestStepL(new(ELeave) CTestStepDevSoundEmptyBuffersNeg2());
	//DEF086171  - Possible memory leak within DevSound CI Framework 
	CTestSuite::AddTestStepL(new(ELeave) CTestStepDevSoundCITest());
	//PDEF121315:Cannot update the observer specified with InitializeL 
	CTestSuite::AddTestStepL(RDevSoundMultiInitTest::NewL(_L("MM-MMF-DEVSOUND-U-0073-HP")));
	//test case for PDEF117811 :Ref DevSound policy updating on error condition is inconsistent 
	CTestSuite::AddTestStepL(RDevSoundPolicyUpdateTest::NewL(_L("MM-MMF-DEVSOUND-U-0074-HP")));

	
		

	// New volume ramp tests
	CTestSuite::AddTestStepL(RDevSoundDualToneVolumeRampTest::NewL(_L("MM-MMF-DEVSOUND-U-0075-HP")));
	CTestSuite::AddTestStepL(RDevSoundDTMFStringVolumeRampTest::NewL(_L("MM-MMF-DEVSOUND-U-0076-HP")));
	CTestSuite::AddTestStepL(RDevSoundToneSequenceVolumeRampTest::NewL(_L("MM-MMF-DEVSOUND-U-0077-HP")));
	CTestSuite::AddTestStepL(RDevSoundDualToneSetVolumeRampWhilePlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0078-HP")));
	CTestSuite::AddTestStepL(RDevSoundDTMFStringSetVolumeRampWhilePlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0079-HP")));
	CTestSuite::AddTestStepL(RDevSoundToneSequenceSetVolumeRampWhilePlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0080-HP")));
	
	// MM-MMF-DEVSOUND-U-0104-HP
	CTestSuite::AddTestStepL(RTestStepRecordOverflow::NewL(_L("MM-MMF-DEVSOUND-U-0104-HP")));
	
	// TruePause is only available for A3F
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	CTestSuite::AddTestStepL(RA3FDevSoundPauseAndResumePlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0081-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPlayRejectionTestWhilePausing::NewL(_L("MM-MMF-DEVSOUND-U-0082-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPlayPreemptionTestWhilePausing::NewL(_L("MM-MMF-DEVSOUND-U-0083-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPlayPauseAndStopPlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0084-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPauseSetVolumeBalanceAndResumePlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0085-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPauseSetConfigAndResumePlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0086-HP")));

	CTestSuite::AddTestStepL(RA3FDevSoundPauseAndResumeRecordingPCMTest::NewL(_L("MM-MMF-DEVSOUND-U-0088-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPauseAndResumeRecordingPCMDoNotWaitForLastBufferTest::NewL(_L("MM-MMF-DEVSOUND-U-0089-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPauseAndResumeRecordingNonPCMTest::NewL(_L("MM-MMF-DEVSOUND-U-0090-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPauseSetGainBalanceAndResumeRecordingPCMTest::NewL(_L("MM-MMF-DEVSOUND-U-0091-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPauseSetConfigAndResumeRecordingPCMTest::NewL(_L("MM-MMF-DEVSOUND-U-0092-HP")));
		
	CTestSuite::AddTestStepL(RA3FDevSoundTonePlayTonePauseAndResume::NewL(_L("MM-MMF-DEVSOUND-U-0093-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundTonePlayDualTonePauseAndResume::NewL(_L("MM-MMF-DEVSOUND-U-0094-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundTonePlayDTMFStringPauseAndResume::NewL(_L("MM-MMF-DEVSOUND-U-0095-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundTonePlayToneSequencePauseAndResume::NewL(_L("MM-MMF-DEVSOUND-U-0096-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundTonePlayToneSetVolumeBalancePauseAndResume::NewL(_L("MM-MMF-DEVSOUND-U-0097-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundTonePlayToneSequenceSetVolumeBalancePauseAndResume::NewL(_L("MM-MMF-DEVSOUND-U-0098-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPlayPauseFlushResumeTest::NewL(_L("MM-MMF-DEVSOUND-U-0099-HP")));
	
	CTestSuite::AddTestStepL(RA3FDevSoundPauseAndInitRecordingPCMTest::NewL(_L("MM-MMF-DEVSOUND-U-0105-HP")));
	CTestSuite::AddTestStepL(RA3FDevSoundPauseAndResumeAndInitPlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0199-HP")));
	//SMP: MultCustomInterface framework test
	CTestSuite::AddTestStepL(RTestStepMultipleCustomInterface::NewL());	
	
#else
	CTestSuite::AddTestStepL(RNonA3FDevSoundPauseAndResumePlayingTest::NewL(_L("MM-MMF-DEVSOUND-U-0087-HP")));
#endif
	}

