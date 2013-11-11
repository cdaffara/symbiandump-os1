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

#ifndef CHAR_A3F_DEVSOUND_TONETEST_H
#define CHAR_A3F_DEVSOUND_TONETEST_H

// system includes
#include "char_a3f_devsound_testbase.h"
#include "char_a3f_devsound_toneclient.h"
#include "char_a3f_devsound_playrecordclients.h"

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0001
 */
class RA3FDevSoundToneSetConfigTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetConfigTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneSetConfigTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iSampleRate;
	TInt iChannels;	
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0002
 */
class RA3FDevSoundToneSetPrioritySettingsTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetPrioritySettingsTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneSetPrioritySettingsTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iPriority;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0003
 */
class RA3FDevSoundToneSetVolumeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetVolumeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneSetVolumeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iVolume;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0004
 */
class RA3FDevSoundToneVolumeRampTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneVolumeRampTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneVolumeRampTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
private:
	TInt iVolumeRamp;
	TInt iDuration; 
	TInt iFrequencyTone1;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0005
 */
class RA3FDevSoundToneSetToneRepeatsTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetToneRepeatsTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetToneRepeatsTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
private:
	TInt iDuration; 
	TInt iFrequencyTone1;
	TInt iRepeatTrailingSilence;
	TInt iRepeatCount;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0006
 */
class RA3FDevSoundToneSetDTMFLengthsTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetDTMFLengthsTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetDTMFLengthsTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	
private:
	TBool iFirstToneFinishedCall;
	TInt iToneOnLength;
	TInt iToneOffLength;
	TInt iPauseLength;
	TBuf16<KMaxStringLength> iDTMFString;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TInt iDTMFPauses;
	TInt iDTMFTones;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0007
 */
class RA3FDevSoundToneSetVolumeWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetVolumeWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetVolumeWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
private:
	TInt iDuration; 
	TInt iVolume;
	TInt iFrequencyTone1;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0008
 */
class RA3FDevSoundToneSetPlayBalanceWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetPlayBalanceWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetPlayBalanceWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration; 
	TInt iVolume;
	TInt iFrequencyTone1;
	TInt  iLSpeakerBalance;
	TInt  iRSpeakerBalance;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0009
 */
class RA3FDevSoundToneSetVolumeWhilePausePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetVolumeWhilePausePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetVolumeWhilePausePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
private:
	TInt  iDuration; 
	TInt  iVolume;
	TInt  iFrequencyTone1;
	TBool iTimeToEnterPauseElapsed;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0010
 */
class RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetVolumeThenInitializeToPlayAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iVolume;
	TBool iReinitialized;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0011
 */
class RA3FDevSoundToneSetVolumeRampWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetVolumeRampWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetVolumeRampWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);

private:
	TInt iDuration; 
	TInt iVolume;
	TInt iFrequencyTone1;
	TInt iVolumeRamp;
	TBool iTonePlayedTwice;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TInt iTimesToneFinishedCalled;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0012
 */
class RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetToneRepetitionsWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration; 
	TInt iFrequencyTone1;
	TInt iRepeatTrailingSilence;
	TInt iRepeatCount;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0013
 */
class RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetDTMFLengthsWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);

private:
	TBool iFirstToneFinishedCall;
	TInt iToneOnLength;
	TInt iToneOffLength;
	TInt iPauseLength;
	TBuf16<KMaxStringLength> iDTMFString;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TInt iDTMFPauses;
	TInt iDTMFTones;
	TInt iTotalExpectedPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0014
 */
class RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetToneRepetitionsNotPlayingTonesModeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration; 
	TInt iFrequencyTone1;
	TInt iRepeatTrailingSilence;
	TInt iRepeatCount;	
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TBool iInitializedToPlayTones;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0015
 */
class RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneSetDTMFLengthsNotPlayingTonesModeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	
private:
	TBool iInitializedToPlayTones;
	TBuf16<KMaxStringLength> iDTMFString;
	TInt iToneOnLength;
	TInt iToneOffLength;
	TInt iPauseLength;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TInt iDTMFPauses;
	TInt iDTMFTones;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0016
 */
class RA3FDevSoundToneSetConfigWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetConfigWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetConfigWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TInt iVolume;
	TInt iFrequencyTone1;
	TInt iSampleRate;
	TInt iChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0017
 */
class RA3FDevSoundToneSetPriorityWhilePlayingTest : public RA3FDevSoundTestBase, public MA3FDevsoundToneClientObserver
	{
public:
	static RA3FDevSoundToneSetPriorityWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetPriorityWhilePlayingTest(const TDesC& aTestName);
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientToneFinishedCallback(TInt aError);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
private:
	
	TInt iDuration;
	TInt iFrequencyTone1;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0018
 */
class RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneSetConfigTestToInvalidSampleRateValue(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iSampleRate;
	TInt iChannels;	
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0019
 */
class RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneSetConfigTestToInvalidChannelValueTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iSampleRate;
	TInt iChannels;	
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0020
 */
class RA3FDevSoundToneSetDTMFLengthsToNegativeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetDTMFLengthsToNegativeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetDTMFLengthsToNegativeTest(const TDesC& aTestName);
	void ToneFinished(TInt aError);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iToneOnLength;
	TInt iToneOffLength;
	TInt iPauseLength;
	TBuf16<KMaxStringLength> iDTMFString;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0021
 */
class RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetDTMFToneOnLengthToMinusOneTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TBool iFirstToneFinishedCall;
	TInt iToneOnLength;
	TInt iToneOffLength;
	TInt iPauseLength;
	TBuf16<KMaxStringLength> iDTMFString;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0022
 */
class RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetToneRepetitionsRepeatTrailingSilenceToNegativeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration; 
	TInt iFrequencyTone1;
	TInt iRepeatTrailingSilence;
	TInt iRepeatCount;	
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TBool iInitializedToPlayTones;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0023
 */
class RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneSetToneRepetitionsRepeatCountToKMdaRepeatForeverTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	void DoTimerCallback();
	
private:
	TInt iDuration; 
	TInt iFrequencyTone1;
	TInt iRepeatTrailingSilence;
	TInt iRepeatCount;	
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TBool iInitializedToPlayTones;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0024
 */
class RA3FDevSoundToneCapsTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneCapsTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneCapsTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iExCapRate;
	TInt iExCapChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0025
 */
class RA3FDevSoundToneConfigDefaultTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneConfigDefaultTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneConfigDefaultTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TMMFSampleRate iExSampleRate;
	TInt iExCapRate;
	TInt iExCapChannels;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0026
 */
class RA3FDevSoundToneMaxVolumeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneMaxVolumeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneMaxVolumeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iExVolume;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0027
 */
class RA3FDevSoundToneVolumeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneVolumeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneVolumeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iExVolume;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0028
 */
class RA3FDevSoundToneQueryCapsWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneQueryCapsWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneQueryCapsWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iExCapRate;
	TInt iExCapChannels;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0029
 */
class RA3FDevSoundToneConfigWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneConfigWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneConfigWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iExCapRate;
	TInt iExCapChannels;
	TMMFSampleRate iExSampleRate;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0030
 */
class RA3FDevSoundToneMaxVolumeWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneMaxVolumeWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneMaxVolumeWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iExVolume;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0031
 */
class RA3FDevSoundToneVolumeWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneVolumeWhilePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneVolumeWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);

private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iExVolume;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0032
 */
class RA3FDevSoundToneQueryCapsWhilePausePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneQueryCapsWhilePausePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneQueryCapsWhilePausePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iExCapRate;
	TInt iExCapChannels;
	TBool iTimeToEnterPauseElapsed;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0033
 */
class RA3FDevSoundToneConfigWhilePausePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneConfigWhilePausePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneConfigWhilePausePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TBool iTimeToEnterPauseElapsed;
	TInt iExCapRate;
	TInt iExCapChannels;
	TMMFSampleRate iExSampleRate;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0034
 */
class RA3FDevSoundToneMaxVolumeWhilePausePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneMaxVolumeWhilePausePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneMaxVolumeWhilePausePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iExVolume;
	TBool iTimeToEnterPauseElapsed;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0035
 */
class RA3FDevSoundToneVolumeWhilePausePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneVolumeWhilePausePlayingTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundToneVolumeWhilePausePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);

private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iExVolume;
	TBool iTimeToEnterPauseElapsed;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0036
 */
class RA3FDevSoundTonePlayToneOnceTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneOnceTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneOnceTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0037
 */
class RA3FDevSoundTonePlayDualToneTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDualToneTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDualToneTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0038
 */
class RA3FDevSoundTonePlayDTMFToneTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDTMFToneTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDTMFToneTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
private:
	TBuf16<KMaxStringLength> iDTMFString;
	TBool iToneStopped;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	TInt iDTMFPauses;
	TInt iDTMFTones;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0039
 */
class RA3FDevSoundTonePlayToneSequenceTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneSequenceTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequenceTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0040
 */
class RA3FDevSoundToneNewToneRequestWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundToneNewToneRequestWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneNewToneRequestWhilePlayingTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TTime iStartTime;
	TTime iEndTime;
	TTimeIntervalMicroSeconds iPlayToneTime;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0041
 */
class RA3FDevSoundTonePreemptionTest : public RA3FDevSoundTestBase, public MA3FDevsoundToneClientObserver
	{
public:
	static RA3FDevSoundTonePreemptionTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundTonePreemptionTest(const TDesC& aTestName);
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientToneFinishedCallback(TInt aError);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0042
 */
class RA3FDevSoundToneRejectionTest : public RA3FDevSoundTestBase, public MA3FDevsoundToneClientObserver
	{
public:
	static RA3FDevSoundToneRejectionTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundToneRejectionTest(const TDesC& aTestName);
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientToneFinishedCallback(TInt aError);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0043
 */
class RA3FDevSoundTonePlayToneOnceAndStopTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneOnceAndStopTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneOnceAndStopTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TBool iToneStopped;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0044
 */
class RA3FDevSoundTonePlayDualToneOnceAndStopTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDualToneOnceAndStopTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDualToneOnceAndStopTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	TBool iToneStopped;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0045
 */
class RA3FDevSoundTonePlayDTMFStringOnceAndStopTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDTMFStringOnceAndStopTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDTMFStringOnceAndStopTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TBuf16<KMaxStringLength> iDTMFString;
	TBool iToneStopped;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0046
 */
class RA3FDevSoundTonePlayToneSequenceOnceAndStopTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneSequenceOnceAndStopTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequenceOnceAndStopTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
private:
	TBool iToneStopped;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0047
 */
class RA3FDevSoundTonePlayToneOncePauseTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneOncePauseTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneOncePauseTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TBool iPaused;
	TInt iSamplesPlayedPaused;
	
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0048
 */
class RA3FDevSoundTonePlayDualToneOncePauseTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDualToneOncePauseTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDualToneOncePauseTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	TBool iPaused;
	TInt iSamplesPlayedPaused;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0049
 */
class RA3FDevSoundTonePlayDTMFStringOncePauseTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDTMFStringOncePauseTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDTMFStringOncePauseTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);

private:
	TBuf16<KMaxStringLength> iDTMFString;
	TBool iPaused;
	TInt iSamplesPlayedPaused;
	TInt iDTMFPauses;
	TInt iDTMFTones;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0050
 */
class RA3FDevSoundTonePlayToneSequenceOncePauseTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneSequenceOncePauseTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequenceOncePauseTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();
	void ToneFinished(TInt aError);
private:
	TBool iPaused;
	TInt iSamplesPlayedPaused;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0051
 */
class RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneOnceInitializedToPlayAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0052
 */
class RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDualToneOnceInitializedToPlayAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
		
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0053
 */
class RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDTMFStringOnceInitializedToPlayAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TBuf16<KMaxStringLength> iDTMFString;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0054
 */
class RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequenceOnceInitializedToPlayAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0055
 */
class RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneOnceInitializedToRecordAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0056
 */
class RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDualToneOnceInitializedToRecordAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
		
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0057
 */
class RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDTMFStringOnceInitializedToRecordAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TBuf16<KMaxStringLength> iDTMFString;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0058
 */
class RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequenceOnceInitializedToRecordAudioTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0059
 */
class RA3FDevSoundTonePlayToneWithoutCallingInitializeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneWithoutCallingInitializeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneWithoutCallingInitializeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();

private:
	TInt iDuration;
	TInt iFrequencyTone1;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0060
 */
class RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDualToneWithoutCallingInitializeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TInt iDuration;
	TInt iFrequencyTone1;
	TInt iFrequencyTone2;
	TBuf16<KMaxStringLength> iDTMFString;
	};
	
/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0061
 */
class RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayDTMFStringWithoutCallingInitializeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	
private:
	TBuf16<KMaxStringLength> iDTMFString;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0062
 */
class RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequenceWithoutCallingInitializeTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-TONE-0063
 */
class RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundTonePlayToneSequenceUsingInvalidDescriptorTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void ToneFinished(TInt aError);
	
private:
	TBuf8<KMaxStringLength> iInvalidToneSequence;
	};


#endif //CHAR_A3F_DEVSOUND_TONETEST_H


