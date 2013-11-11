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

#include "char_a3f_devsound_compliancesuite.h"
#include "char_a3f_devsound_tonetest.h"
#include "char_a3f_devsound_recordtest.h"
#include "char_a3f_devsound_initializationtest.h"
#include "char_a3f_devsound_playtest.h"


/*
 Entry point - Create a test suite object
 */
EXPORT_C CA3FDevSoundComplianceSuite* NewTestSuiteL() 
	{ 
	return CA3FDevSoundComplianceSuite::NewL();
	}

CA3FDevSoundComplianceSuite* CA3FDevSoundComplianceSuite::NewL() 
	{ 
	CA3FDevSoundComplianceSuite* self = new (ELeave) CA3FDevSoundComplianceSuite;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CA3FDevSoundComplianceSuite::CA3FDevSoundComplianceSuite()
	{
	iSuiteName = KTestSuiteName;
	}

CA3FDevSoundComplianceSuite::~CA3FDevSoundComplianceSuite()
	{
	}

void CA3FDevSoundComplianceSuite::InitialiseL()
	{
	// - Initialisation Test Cases ---------------------------------------------------------
	AddTestStepL(RA3FDevSoundToneInitializationTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0001")));
	AddTestStepL(RA3FDevSoundToneCancelInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0002")));
	AddTestStepL(RA3FDevSoundToneInitializeToPlayTonesTwiceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0003")));
	AddTestStepL(RA3FDevSoundToneInitializeToPlayTonesThenAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0004")));
	AddTestStepL(RA3FDevSoundToneInitializeToPlayTonesThenRecordAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0005")));
	AddTestStepL(RA3FDevSoundToneInitializeTFourCCTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0006")));
	AddTestStepL(RA3FDevSoundInitializeInvalidModeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0007")));
	AddTestStepL(RA3FDevSoundPlayInitializationTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0008")));
	AddTestStepL(RA3FDevSoundPlayCancelInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0009")));
	AddTestStepL(RA3FDevSoundPlayInitializeToPlayAudioThenTonesTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0010")));
	AddTestStepL(RA3FDevSoundPlayInitializeToPlayAudioTwiceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0011")));
	AddTestStepL(RA3FDevSoundPlayInitializeToPlayAudioThenRecordTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0012")));	
	AddTestStepL(RA3FDevSoundPlayInitializeInvalidTFourCCTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0013")));
	AddTestStepL(RA3FDevSoundRecordInitializationTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0014")));
	AddTestStepL(RA3FDevSoundRecordCancelInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0015")));
	AddTestStepL(RA3FDevSoundRecordInitializeThenPlayToneInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0016")));
	AddTestStepL(RA3FDevSoundRecordInitializeThenPlayAudioInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0017")));
	AddTestStepL(RA3FDevSoundRecordInitializeToRecordAudioTwiceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0018")));
	AddTestStepL(RA3FDevSoundRecordInitializeNoFormatSpecificationPresentedTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0019")));
	AddTestStepL(RA3FDevSoundPlayDataWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0020")));
	AddTestStepL(RA3FDevSoundPlayDTMFStringWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0021")));
	AddTestStepL(RA3FDevSoundPlayDualToneWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0022")));
	AddTestStepL(RA3FDevSoundPlayFixedSequenceWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0023")));
	AddTestStepL(RA3FDevSoundPlayInitWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0024")));
	AddTestStepL(RA3FDevSoundPlayToneWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0025")));
	AddTestStepL(RA3FDevSoundPlayToneSequenceWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0026")));
	AddTestStepL(RA3FDevSoundRecordDataWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0027")));
	AddTestStepL(RA3FDevSoundRecordInitWithoutInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0028")));
	AddTestStepL(RA3FDevSoundToneInitializationMoreThanOnceBeforeInitCompleteTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0029")));
	AddTestStepL(RA3FDevSoundPlayInitializationMoreThanOnceBeforeInitCompleteTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0030")));
	AddTestStepL(RA3FDevSoundRecordInitializationMoreThanOnceBeforeInitCompleteTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0031")));
	AddTestStepL(RA3FDevSoundToneFirstInitializationLeavesThenSecondSucceeds::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0032")));
	AddTestStepL(RA3FDevSoundPlayFirstInitializationLeavesThenSecondSucceeds::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0033")));
	AddTestStepL(RA3FDevSoundRecordFirstInitializationLeavesThenSecondSucceeds::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-INIT-0034")));

	// - Play Audio Test Cases ---------------------------------------------------------
	AddTestStepL(RA3FDevSoundPlaySetConfigTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0001")));
	AddTestStepL(RA3FDevSoundPlaySetVolumeWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0002")));
	AddTestStepL(RA3FDevSoundPlaySetVolumeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0003")));
	AddTestStepL(RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0004")));
	AddTestStepL(RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0005")));
	AddTestStepL(RA3FDevSoundPlaySetVolumeRampWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0006")));
	AddTestStepL(RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0007")));
	AddTestStepL(RA3FDevSoundPlaySetVolumeToNegativeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0008")));
	AddTestStepL(RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0009")));
	AddTestStepL(RA3FDevSoundPlaySetPlayBalanceToNegativeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0010")));
	AddTestStepL(RA3FDevSoundPlaySetPrioritySettingsTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0011")));
	AddTestStepL(RA3FDevSoundPlayCapabilitiesTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0012")));
	AddTestStepL(RA3FDevSoundPlayConfigTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0013")));
	AddTestStepL(RA3FDevSoundPlayGetPlayBalanceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0014")));
	AddTestStepL(RA3FDevSoundPlayGetSupportedInputDataTypesLTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0015")));
	AddTestStepL(RA3FDevSoundPlayMaxVolumeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0016")));
	AddTestStepL(RA3FDevSoundPlaySamplesPlayedTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0017")));
	AddTestStepL(RA3FDevSoundPlayVolumeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0018")));
	AddTestStepL(RA3FDevSoundPlayCapabilitiesWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0019")));
	AddTestStepL(RA3FDevSoundPlayConfigWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0020")));
	AddTestStepL(RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0021")));
	AddTestStepL(RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0022")));
	AddTestStepL(RA3FDevSoundPlayMaxVolumeWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0023")));
	AddTestStepL(RA3FDevSoundPlaySamplesPlayedWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0024")));
	AddTestStepL(RA3FDevSoundPlayVolumeWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0025")));
	AddTestStepL(RA3FDevSoundPlayTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0026")));
	AddTestStepL(RA3FDevSoundPlayEmptyBufferWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0027")));
	AddTestStepL(RA3FDevSoundPlayEmptyBufferBeforePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0028")));
	AddTestStepL(RA3FDevSoundPlayPauseTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0029")));
	AddTestStepL(RA3FDevSoundPlayStopTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0030")));
	AddTestStepL(RA3FDevSoundPlayPreemptionTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0031")));
	AddTestStepL(RA3FDevSoundPlayPreemptedByToneTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0032")));
	AddTestStepL(RA3FDevSoundPlayPreemptedByRecordTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0033")));
	AddTestStepL(RA3FDevSoundPlayRejectionTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0034")));
	AddTestStepL(RA3FDevSoundPlayRejectedByToneTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0035")));
	AddTestStepL(RA3FDevSoundPlayRejectedByRecordTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0036")));
	AddTestStepL(RA3FDevSoundPlayDataBeforePlayInitTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0037")));
	AddTestStepL(RA3FDevSoundPlayInitTwiceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0038")));
	AddTestStepL(RA3FDevSoundPlayARNTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0039")));
	AddTestStepL(RA3FDevSoundImmediatePauseAfterCreation::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0040")));
	AddTestStepL(RA3FDevSoundPlayEmptyBuffersWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0041")));
    AddTestStepL(RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0042")));
	
	// - Play Tones Test Cases ---------------------------------------------------------
	AddTestStepL(RA3FDevSoundToneSetConfigTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0001")));
	AddTestStepL(RA3FDevSoundToneSetPrioritySettingsTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0002")));
	AddTestStepL(RA3FDevSoundToneSetVolumeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0003")));
	AddTestStepL(RA3FDevSoundToneVolumeRampTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0004")));
	AddTestStepL(RA3FDevSoundToneSetToneRepeatsTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0005")));
	AddTestStepL(RA3FDevSoundToneSetDTMFLengthsTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0006")));
	AddTestStepL(RA3FDevSoundToneSetVolumeWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0007")));
	AddTestStepL(RA3FDevSoundToneSetPlayBalanceWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0008")));
	AddTestStepL(RA3FDevSoundToneSetVolumeWhilePausePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0009")));
	AddTestStepL(RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0010")));
	AddTestStepL(RA3FDevSoundToneSetVolumeRampWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0011")));
	AddTestStepL(RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0012")));
	AddTestStepL(RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0013")));
	AddTestStepL(RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0014")));
	AddTestStepL(RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0015")));
	AddTestStepL(RA3FDevSoundToneSetConfigWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0016")));
	AddTestStepL(RA3FDevSoundToneSetPriorityWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0017")));
	AddTestStepL(RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0018")));
	AddTestStepL(RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0019")));
	AddTestStepL(RA3FDevSoundToneSetDTMFLengthsToNegativeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0020")));
	AddTestStepL(RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0021")));
	AddTestStepL(RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0022")));
	AddTestStepL(RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0023")));
	AddTestStepL(RA3FDevSoundToneCapsTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0024")));
	AddTestStepL(RA3FDevSoundToneConfigDefaultTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0025")));
	AddTestStepL(RA3FDevSoundToneMaxVolumeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0026")));
	AddTestStepL(RA3FDevSoundToneVolumeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0027")));
	AddTestStepL(RA3FDevSoundToneQueryCapsWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0028")));
	AddTestStepL(RA3FDevSoundToneConfigWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0029")));
	AddTestStepL(RA3FDevSoundToneMaxVolumeWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0030")));
	AddTestStepL(RA3FDevSoundToneVolumeWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0031")));
	AddTestStepL(RA3FDevSoundToneQueryCapsWhilePausePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0032")));
	AddTestStepL(RA3FDevSoundToneConfigWhilePausePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0033")));
	AddTestStepL(RA3FDevSoundToneMaxVolumeWhilePausePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0034")));
	AddTestStepL(RA3FDevSoundToneVolumeWhilePausePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0035")));
	AddTestStepL(RA3FDevSoundTonePlayToneOnceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0036")));
	AddTestStepL(RA3FDevSoundTonePlayDualToneTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0037")));
	AddTestStepL(RA3FDevSoundTonePlayDTMFToneTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0038")));
	AddTestStepL(RA3FDevSoundTonePlayToneSequenceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0039")));
	AddTestStepL(RA3FDevSoundToneNewToneRequestWhilePlayingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0040")));
	AddTestStepL(RA3FDevSoundTonePreemptionTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0041")));
	AddTestStepL(RA3FDevSoundToneRejectionTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0042")));
	AddTestStepL(RA3FDevSoundTonePlayToneOnceAndStopTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0043")));
	AddTestStepL(RA3FDevSoundTonePlayDualToneOnceAndStopTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0044")));
	AddTestStepL(RA3FDevSoundTonePlayDTMFStringOnceAndStopTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0045")));
	AddTestStepL(RA3FDevSoundTonePlayToneSequenceOnceAndStopTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0046")));
	AddTestStepL(RA3FDevSoundTonePlayToneOncePauseTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0047")));
	AddTestStepL(RA3FDevSoundTonePlayDualToneOncePauseTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0048")));
	AddTestStepL(RA3FDevSoundTonePlayDTMFStringOncePauseTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0049")));
	AddTestStepL(RA3FDevSoundTonePlayToneSequenceOncePauseTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0050")));
	AddTestStepL(RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0051")));
	AddTestStepL(RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0052")));
	AddTestStepL(RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0053")));
	AddTestStepL(RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0054")));
	AddTestStepL(RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0055")));
	AddTestStepL(RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0056")));
	AddTestStepL(RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0057")));
	AddTestStepL(RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0058")));
	AddTestStepL(RA3FDevSoundTonePlayToneWithoutCallingInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0059")));
	AddTestStepL(RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0060")));
	AddTestStepL(RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0061")));
	AddTestStepL(RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0062")));
	AddTestStepL(RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-TONE-0063")));

	//- Record Audio Test Cases ---------------------------------------------------------
	AddTestStepL(RA3FDevSoundRecordConfigurationSetConfigTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0001")));
	AddTestStepL(RA3FDevSoundRecordConfigurationSetPriorityTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0002")));
	AddTestStepL(RA3FDevSoundRecordConfigurationSetGainTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0003")));
	AddTestStepL(RA3FDevSoundRecordConfigurationSetRecordBalanceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0004")));
	AddTestStepL(RA3FDevSoundRecordSetGainWhileRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0005")));
	AddTestStepL(RA3FDevSoundSetRecordGainBeyondMaxLimitConfigurationTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0006")));
	AddTestStepL(RA3FDevSoundSetRecordGainToNegativeValueConfigurationTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0007")));
	AddTestStepL(RA3FDevSoundSetRecordBalanceToValuesBeyondUpperLimitConfigurationTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0008")));
	AddTestStepL(RA3FDevSoundSetRecordBalanceToNegativeValuesConfigurationTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0009")));
	AddTestStepL(RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0010")));
	AddTestStepL(RA3FDevSoundRecordConfigurationSetGainWhenInitToPlayTonesTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0011")));
	AddTestStepL(RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0012")));
	AddTestStepL(RA3FDevSoundRecordConfigurationSetRecordBalanceWhenInitToPlayTonesTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0013")));
	AddTestStepL(RA3FDevSoundCallSetConfigLWhileRecordingConfigurationTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0014")));
	AddTestStepL(RA3FDevSoundQueryRecordCapabilitiesTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0015")));
	AddTestStepL(RA3FDevSoundQueryRecordConfigTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0016")));
	AddTestStepL(RA3FDevSoundQueryRecordBalanceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0017")));
	AddTestStepL(RA3FDevSoundQueryRecordSupportedOutputDataTypesTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0018")));
	AddTestStepL(RA3FDevSoundQueryRecordMaxGainTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0019")));
	AddTestStepL(RA3FDevSoundQueryRecordSamplesRecordedTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0020")));
	AddTestStepL(RA3FDevSoundQueryRecordGainTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0021")));
	AddTestStepL(RA3FDevSoundQueryRecordCapabilitiesWhileRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0022")));
	AddTestStepL(RA3FDevSoundQueryRecordConfigWhileRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0023")));
	AddTestStepL(RA3FDevSoundQueryRecordBalanceWhileRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0024")));
	AddTestStepL(RA3FDevSoundQueryRecordSupportedOutputDataTypesWhileRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0025")));
	AddTestStepL(RA3FDevSoundQueryRecordMaxGainWhileRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0026")));
	AddTestStepL(RA3FDevSoundQueryRecordSamplesRecordedWhileRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0027")));
	AddTestStepL(RA3FDevSoundQueryRecordGainWhileRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0028")));
	AddTestStepL(RA3FDevSoundRecordingTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0029")));
	AddTestStepL(RA3FDevSoundLeavePauseStateToInitializedToRecordAudioTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0030")));
	AddTestStepL(RA3FDevSoundRecordPreemptionTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0031")));
	AddTestStepL(RA3FDevSoundRecordRejectionTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0032")));
	AddTestStepL(RA3FDevSoundRecordDataWithoutRecordInitTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0033")));
	AddTestStepL(RA3FDevSoundRecordCallingRecordInitLMoreThanOnceTest::NewL(_L("MM-A3F-DEVSOUND-CHRTZ-REC-RAW-0034")));
	}
