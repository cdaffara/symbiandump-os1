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

#ifndef CHAR_A3F_DEVSOUND_PLAYTEST_H
#define CHAR_A3F_DEVSOUND_PLAYTEST_H

// User includes
#include "char_a3f_devsound_testbase.h"
#include "char_a3f_devsound_playrecordclients.h"
#include "char_a3f_devsound_toneclient.h"

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0001
 */
class RA3FDevSoundPlaySetConfigTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetConfigTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetConfigTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TMMFSampleRate iESampleRate;
	TInt iSampleRate;
	TInt iChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0002
 */
class RA3FDevSoundPlaySetVolumeWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetVolumeWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetVolumeWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
private:
	TInt iVolume;
	TFileName iFilename;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0003
 */
class RA3FDevSoundPlaySetVolumeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetVolumeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetVolumeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TInt iVolume;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0004
 */
class RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetVolumeWhilePausedPlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();

private:
	TFileName iFilename;
	TInt iVolume;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0005
 */
class RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest: public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetVolumeInitializeToPlayAudioThenTonesTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TBool iReinitialized;
	TInt iVolume;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0006
 */
class RA3FDevSoundPlaySetVolumeRampWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetVolumeRampWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetVolumeRampWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	void PlayError(TInt aError);

private:
	TFileName iFilename;
	TInt iVolumeRamp;
	TBool iPlayInitTwice;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0007
 */
class RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetVolumeBeyondMaxVolumeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0008
 */
class RA3FDevSoundPlaySetVolumeToNegativeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetVolumeToNegativeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetVolumeToNegativeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	
private:
	TInt iVolume;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0009
 */
class RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetPlayBalanceBeyondOneHundredTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	
private:
	TInt iLSpeakerBalance;
	TInt iRSpeakerBalance;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0010
 */
class RA3FDevSoundPlaySetPlayBalanceToNegativeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetPlayBalanceToNegativeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetPlayBalanceToNegativeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	
private:
	TInt iLSpeakerBalance;
	TInt iRSpeakerBalance;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0011
 */
class RA3FDevSoundPlaySetPrioritySettingsTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySetPrioritySettingsTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySetPrioritySettingsTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TInt iPriority;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0012
 */
class RA3FDevSoundPlayCapabilitiesTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayCapabilitiesTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayCapabilitiesTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	
private:
	TInt iExCapChannels;
	TInt iExCapRate;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0013
 */
class RA3FDevSoundPlayConfigTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayConfigTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayConfigTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TMMFSampleRate iESampleRate;
	TMMFSampleRate iExSampleRate;
	TInt iExCapRate;
	TInt iExCapChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0014
 */
class RA3FDevSoundPlayGetPlayBalanceTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayGetPlayBalanceTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayGetPlayBalanceTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	
private:
	TInt iExLSpeakerBalance;
	TInt iExRSpeakerBalance;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0015
 */
class RA3FDevSoundPlayGetSupportedInputDataTypesLTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayGetSupportedInputDataTypesLTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayGetSupportedInputDataTypesLTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	RArray<TFourCC> iSupportedDataTypes;
	TInt iCurrentTypeInTest;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0016
 */
class RA3FDevSoundPlayMaxVolumeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayMaxVolumeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayMaxVolumeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TInt iExVolume;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0017
 */
class RA3FDevSoundPlaySamplesPlayedTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySamplesPlayedTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySamplesPlayedTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	
private:
	TInt iExSamplesPlayed;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0018
 */
class RA3FDevSoundPlayVolumeTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayVolumeTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayVolumeTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	
private:
	TInt iExVolume;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0019
 */
class RA3FDevSoundPlayCapabilitiesWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayCapabilitiesWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayCapabilitiesWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();

private:
	TFileName iFilename;
	TInt iExCapChannels;
	TInt iExCapRate;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0020
 */
class RA3FDevSoundPlayConfigWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayConfigWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayConfigWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();

private:
	TFileName iFilename;
	TMMFSampleRate iExSampleRate;
	TInt iExCapRate;
	TInt iExCapChannels;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0021
 */
class RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayGetPlayBalanceWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();

private:
	TFileName iFilename;
	TInt iExLSpeakerBalance;
	TInt iExRSpeakerBalance;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0022
 */
class RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayGetSupportedInputDataTypesWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
private:
	TFileName iFilename;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0023
 */
class RA3FDevSoundPlayMaxVolumeWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayMaxVolumeWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayMaxVolumeWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();

private:
	TFileName iFilename;
	TInt iExVolume;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0024
 */
class RA3FDevSoundPlaySamplesPlayedWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlaySamplesPlayedWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlaySamplesPlayedWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	
private:
	TFileName iFilename;
	TInt iExSamplesPlayed;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0025
 */
class RA3FDevSoundPlayVolumeWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayVolumeWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayVolumeWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	
private:
	TFileName iFilename;
	TInt iExVolume;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0026
 */
class RA3FDevSoundPlayTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);

private:
	TFileName iFilename;
	};	

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0027
 */
class RA3FDevSoundPlayEmptyBufferWhilePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayEmptyBufferWhilePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayEmptyBufferWhilePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();

private:
	TFileName iFilename;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0028
 */
class RA3FDevSoundPlayEmptyBufferBeforePlayingTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayEmptyBufferBeforePlayingTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayEmptyBufferBeforePlayingTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0029
 */
class RA3FDevSoundPlayPauseTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayPauseTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayPauseTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();

private:
	TFileName iFilename;
	TBool iPaused;
	TInt iSamplesPlayedPaused;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0030
 */
class RA3FDevSoundPlayStopTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayStopTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayStopTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();

private:
	TFileName iFilename;
	TBool iStopped;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0031
 */
class RA3FDevSoundPlayPreemptionTest : public RA3FDevSoundTestBase, public MA3FDevsoundPlayAudioClientObserver
	{
public:
	static RA3FDevSoundPlayPreemptionTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayPreemptionTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void PlayError(TInt aError);
	void DoTimerCallback();
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeFilledCallback(TInt aError);
	void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer);
	void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);

private:
	TFileName iFilename;
	TFileName iOtherFilename;
	};

/*
 *========================================================================================================
 * 3F-DEVSOUND-CHRTZ-PLAY-RAW-0032
 */
class RA3FDevSoundPlayPreemptedByToneTest : public RA3FDevSoundTestBase, public MA3FDevsoundToneClientObserver
	{
public:
	static RA3FDevSoundPlayPreemptedByToneTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayPreemptedByToneTest(const TDesC& aTestName);
	void PlayError(TInt aError);
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientToneFinishedCallback(TInt aError);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();	
	
private:
	TFileName iFilename;
	TInt iFrequencyTone1;
	TInt iDuration;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0033
 */
class RA3FDevSoundPlayPreemptedByRecordTest : public RA3FDevSoundTestBase, public MA3FDevsoundRecordAudioClientObserver
	{
public:
	static RA3FDevSoundPlayPreemptedByRecordTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayPreemptedByRecordTest(const TDesC& aTestName);
	void PlayError(TInt aError);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();	
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeEmptiedCallback(TInt aError);
	void ClientRecordErrorCallback(TInt aError);
	void ClientRecordHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);

private:
	TInt iDuration;
	TBool iOtherClientStarted;
	TFileName iFilename;
	TFileName iOtherFilename;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0034
 */
class RA3FDevSoundPlayRejectionTest : public RA3FDevSoundTestBase, public MA3FDevsoundPlayAudioClientObserver
	{
public:
	static RA3FDevSoundPlayRejectionTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayRejectionTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeFilledCallback(TInt aError);
	void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer);
	void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);
	
private:
	TFileName iFilename;
	TFileName iOtherFilename;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0035
 */
class RA3FDevSoundPlayRejectedByToneTest : public RA3FDevSoundTestBase, public MA3FDevsoundPlayAudioClientObserver
	{
public:
	static RA3FDevSoundPlayRejectedByToneTest* NewL(const TDesC& aTestName);
private:
	RA3FDevSoundPlayRejectedByToneTest(const TDesC& aTestName);
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoKickoffTestL();
	void DoTimerCallback();	
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeFilledCallback(TInt aError);
	void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer);
	void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);

private:
	TFileName iFilename;
	TInt iFrequencyTone1;
	TInt iDuration;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0036
 */
class RA3FDevSoundPlayRejectedByRecordTest : public RA3FDevSoundTestBase, public MA3FDevsoundPlayAudioClientObserver
	{
public:
	static RA3FDevSoundPlayRejectedByRecordTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayRejectedByRecordTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void DoTimerCallback();
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeFilledCallback(TInt aError);
	void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer);
	void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);
	
private:
	TFileName iFilename;
	TFileName iOtherFilename;
	TBool iOtherClientStarted;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0037
 */
class RA3FDevSoundPlayDataBeforePlayInitTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayDataBeforePlayInitTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayDataBeforePlayInitTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0038
 */
class RA3FDevSoundPlayInitTwiceTest : public RA3FDevSoundTestBase
	{
public:
	static RA3FDevSoundPlayInitTwiceTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayInitTwiceTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void BufferToBeFilled(CMMFBuffer* /*aBuffer*/);

private:
	TInt iFirstPlayInitErr;
	TInt iErr;
	};

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0039
 */
class RA3FDevSoundPlayARNTest : public RA3FDevSoundTestBase, public MA3FDevsoundPlayAudioClientObserver
	{
public:
	static RA3FDevSoundPlayARNTest* NewL(const TDesC& aTestName);

private:
	RA3FDevSoundPlayARNTest(const TDesC& aTestName);
	void DoKickoffTestL();
	void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
	void PlayError(TInt aError);
	void DoTimerCallback();
	void ClientInitializeCompleteCallback(TInt aError);
	void ClientBufferToBeFilledCallback(TInt aError);
	void ClientPlayErrorCallback(TInt aError, TBool aLastBuffer);
	void ClientPlayHandleOpenFileErrorInValidEventOrStateCallBack(TMmfDevSoundClientErrorCodes aError);

private:
	TFileName iFilename;
	TFileName iOtherFilename;
	TBool iRepeat;
	};

/*
*========================================================================================================
* MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0040
*/
class RA3FDevSoundImmediatePauseAfterCreation : public RA3FDevSoundTestBase
   {
public:
   static RA3FDevSoundImmediatePauseAfterCreation* NewL(const TDesC& aTestName);

private:
    RA3FDevSoundImmediatePauseAfterCreation(const TDesC& aTestName);
   void DoKickoffTestL();
   void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
   void DoTimerCallback();
private:
   TFileName iFilename;
   TFileName iOtherFilename;
   TBool iRepeat;
   };

/*
 *========================================================================================================
 * MM-A3F-DEVSOUND-CHRTZ-PLAY-RAW-0041
 */
class RA3FDevSoundPlayEmptyBuffersWhilePlayingTest : public RA3FDevSoundTestBase
    {
public:
    static RA3FDevSoundPlayEmptyBuffersWhilePlayingTest* NewL(const TDesC& aTestName);

private:
    RA3FDevSoundPlayEmptyBuffersWhilePlayingTest(const TDesC& aTestName);
    void DoKickoffTestL();
    void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
private:
    TInt iVolume;
    TInt iBtbfCount;
    TFileName iFilename;
    };

/*
 *========================================================================================================
 * 3F-DEVSOUND-CHRTZ-PLAY-RAW-0042
 */
class RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest : public RA3FDevSoundTestBase, public MA3FDevsoundToneClientObserver
    {
public:
    static RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest* NewL(const TDesC& aTestName);
private:
    RA3FDevSoundPlayPreemptedByToneAfterSetVolumeTest(const TDesC& aTestName);
    void PlayError(TInt aError);
    void ClientInitializeCompleteCallback(TInt aError);
    void ClientToneFinishedCallback(TInt aError);
    void Fsm(TMmfDevSoundEvent aDevSoundEvent, TInt aError);
    void DoKickoffTestL();
    void DoTimerCallback(); 
    
private:
    TFileName   iFilename;
    TInt        iExVolume;
    TInt        iFrequencyTone1;
    TInt        iDuration;
    TBool       iSetVolume;
    TBool       iResume;
    };

#endif // CHAR_A3F_DEVSOUND_PLAYTEST_H

