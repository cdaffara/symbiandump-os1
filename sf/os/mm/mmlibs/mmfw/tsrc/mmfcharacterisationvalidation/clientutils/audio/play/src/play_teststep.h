// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// audioplaytest.h
// 
//

#ifndef AUDIOPLAYTEST_H
#define AUDIOPLAYTEST_H

#include "char_audioplay_common.h"


/******************************************************************************************************
Open an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0001, MM-MMF-APLCLNT-CHRTZ-OGG-I-0001
*******************************************************************************************************/

class RTestStepAPOpenFile : public RTestBaseAudioPlay
	{
public:
	static RTestStepAPOpenFile* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPOpenFile(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Open a corrupt audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0002, MM-MMF-APLCLNT-CHRTZ-OGG-I-0002
*******************************************************************************************************/

class RTestStepAPOpenCorruptFile : public RTestBaseAudioPlay
	{
public:
	static RTestStepAPOpenCorruptFile* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);

	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPOpenCorruptFile(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Open an audio file with no controller to support the audio file format.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0003
*******************************************************************************************************/

class RTestStepAPOpenFileNoController : public RTestBaseAudioPlay
	{
public:
	static RTestStepAPOpenFileNoController* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPOpenFileNoController(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Open an audio file providing the incorrect path i.e. no such file exists in that path.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0004
*******************************************************************************************************/

class RTestStepAPOpenNoFile: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPOpenNoFile* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPOpenNoFile(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Open an audio clip from a descriptor.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0006, MM-MMF-APLCLNT-CHRTZ-OGG-I-0006
*******************************************************************************************************/

class RTestStepAPOpenDes: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPOpenDes* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPOpenDes(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	HBufC8* iAudio;						// Buffer for holding audio descriptor data
	TPtr8 iData;						// Pointer to point to iAudio
	};

/******************************************************************************************************
Open an audio clip from a descriptor with no controller to support the audio file format.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0007, MM-MMF-APLCLNT-CHRTZ-OGG-I-0007
*******************************************************************************************************/

class RTestStepAPOpenDesNoController: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPOpenDesNoController* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPOpenDesNoController(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	HBufC8* iAudio;						// Buffer for holding audio descriptor data
	};

/******************************************************************************************************
Open an audio clip file from a URL.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0009
*******************************************************************************************************/

class RTestStepAPOpenURL: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPOpenURL* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPOpenURL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Play an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0011, MM-MMF-APLCLNT-CHRTZ-OGG-I-0011
*******************************************************************************************************/

class RTestStepAPPlayAudioFile: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPPlayAudioFile* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPPlayAudioFile(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Play an audio file while in the EPlaying state.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0012, MM-MMF-APLCLNT-CHRTZ-OGG-I-0012
*******************************************************************************************************/

class RTestStepAPPlayAudioFileWhilePlaying: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPPlayAudioFileWhilePlaying* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
		
private:
	RTestStepAPPlayAudioFileWhilePlaying(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Play an audio file before opening the clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0013, MM-MMF-APLCLNT-CHRTZ-OGG-I-0013
*******************************************************************************************************/

class RTestStepAPPlayAudioFileBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPPlayAudioFileBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPPlayAudioFileBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Stop while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0014, MM-MMF-APLCLNT-CHRTZ-OGG-I-0014
*******************************************************************************************************/

class RTestStepAPStopWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPStopWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPStopWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Pause while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0015, MM-MMF-APLCLNT-CHRTZ-OGG-I-0015
*******************************************************************************************************/

class RTestStepAPPauseWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPPauseWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPPauseWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Close while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0016, MM-MMF-APLCLNT-CHRTZ-OGG-I-0016
*******************************************************************************************************/

class RTestStepAPCloseWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPCloseWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPCloseWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	
	// Flag for identifying the second OpenFileL() to verfiy that Close() actually closed the file
	TBool iSecondOpen;
	};
	
/******************************************************************************************************
Get the max volume.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0017, MM-MMF-APLCLNT-CHRTZ-OGG-I-0017
*******************************************************************************************************/

class RTestStepAPGetMaxVolume: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetMaxVolume* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetMaxVolume(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Get the max volume while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0018, MM-MMF-APLCLNT-CHRTZ-OGG-I-0018
*******************************************************************************************************/

class RTestStepAPGetMaxVolumeWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetMaxVolumeWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetMaxVolumeWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

	
/******************************************************************************************************
Get the max volume before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0019
*******************************************************************************************************/

class RTestStepAPGetMaxVolumeBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetMaxVolumeBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetMaxVolumeBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Get the current playback volume.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0020, MM-MMF-APLCLNT-CHRTZ-OGG-I-0020
*******************************************************************************************************/

class RTestStepAPGetPlaybackVolume: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetPlaybackVolume* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetPlaybackVolume(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Get the current playback volume while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0021, MM-MMF-APLCLNT-CHRTZ-OGG-I-0021
*******************************************************************************************************/

class RTestStepAPGetPlaybackVolumeWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetPlaybackVolumeWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetPlaybackVolumeWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Get the current playback volume before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0022
*******************************************************************************************************/

class RTestStepAPGetPlaybackVolumeBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetPlaybackVolumeBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetPlaybackVolumeBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the current playback volume to zero.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0023, MM-MMF-APLCLNT-CHRTZ-OGG-I-0023
*******************************************************************************************************/

class RTestStepAPSetPlaybackVolumeToZero: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlaybackVolumeToZero* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlaybackVolumeToZero(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the current playback volume to the maximum value.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0024, MM-MMF-APLCLNT-CHRTZ-OGG-I-0024
*******************************************************************************************************/

class RTestStepAPSetPlaybackVolumeToMax: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlaybackVolumeToMax* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlaybackVolumeToMax(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the current playback volume to zero while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0025, MM-MMF-APLCLNT-CHRTZ-OGG-I-0025
*******************************************************************************************************/

class RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlaybackVolumeToZeroWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the current playback volume to the maximum value while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0026, MM-MMF-APLCLNT-CHRTZ-OGG-I-0026
*******************************************************************************************************/

class RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlaybackVolumeToMaxWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
	
/******************************************************************************************************
Set the current playback volume to zero before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0027
*******************************************************************************************************/

class RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlaybackVolumeToZeroBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the current playback volume to the maximum before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0028
*******************************************************************************************************/

class RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlaybackVolumeToMaxBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the volume ramp.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0029, MM-MMF-APLCLNT-CHRTZ-OGG-I-0029
*******************************************************************************************************/

class RTestStepAPSetVolumeRamp: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetVolumeRamp* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetVolumeRamp(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the volume ramp while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0030, MM-MMF-APLCLNT-CHRTZ-OGG-I-0030
*******************************************************************************************************/

class RTestStepAPSetVolumeRampWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetVolumeRampWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetVolumeRampWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the number of times the audio clip is to be repeated to zero.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0032, MM-MMF-APLCLNT-CHRTZ-OGG-I-0032
*******************************************************************************************************/

class RTestStepAPSetRepeatsToZero: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetRepeatsToZero* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetRepeatsToZero(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the number of times the audio clip is to be repeated to KMdaRepeatForever.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0033, MM-MMF-APLCLNT-CHRTZ-OGG-I-0033
*******************************************************************************************************/

class RTestStepAPSetRepeatsToForever: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetRepeatsToForever* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetRepeatsToForever(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Set the number of times the audio clip is to be repeated to zero while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0034, MM-MMF-APLCLNT-CHRTZ-OGG-I-0034
*******************************************************************************************************/

class RTestStepAPSetRepeatsToZeroWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetRepeatsToZeroWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetRepeatsToZeroWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the number of times the audio clip is to be repeated to KMdaRepeatForever while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0035, MM-MMF-APLCLNT-CHRTZ-OGG-I-0035
*******************************************************************************************************/

class RTestStepAPSetRepeatsToForeverWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetRepeatsToForeverWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetRepeatsToForeverWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the number of times the audio clip is to be repeated to zero before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0036
*******************************************************************************************************/

class RTestStepAPSetRepeatsToZeroBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetRepeatsToZeroBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetRepeatsToZeroBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the number of times the audio clip is to be repeated to KMdaRepeatForever before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0037
*******************************************************************************************************/

class RTestStepAPSetRepeatsToForeverBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetRepeatsToForeverBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetRepeatsToForeverBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Set the priority for playback.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0038, MM-MMF-APLCLNT-CHRTZ-OGG-I-0038
*******************************************************************************************************/

class RTestStepAPSetPriority: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPriority* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPriority(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the priority for playback while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0039, MM-MMF-APLCLNT-CHRTZ-OGG-I-0039
*******************************************************************************************************/

class RTestStepAPSetPriorityWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPriorityWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPriorityWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the priority for playback before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0040
*******************************************************************************************************/

class RTestStepAPSetPriorityBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPriorityBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPriorityBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Get the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0041, MM-MMF-APLCLNT-CHRTZ-OGG-I-0041
*******************************************************************************************************/

class RTestStepAPGetPosition: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetPosition* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetPosition(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Get the current head position while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0042, MM-MMF-APLCLNT-CHRTZ-OGG-I-0042
*******************************************************************************************************/

class RTestStepAPGetPositionWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetPositionWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetPositionWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0043, MM-MMF-APLCLNT-CHRTZ-OGG-I-0043
*******************************************************************************************************/

class RTestStepAPSetPosition: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPosition* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPosition(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the current head position to a negative value.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0044, MM-MMF-APLCLNT-CHRTZ-OGG-I-0044
*******************************************************************************************************/

class RTestStepAPSetPositionNegative: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPositionNegative* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPositionNegative(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Set the current head position to a value greater than the duration.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0045, MM-MMF-APLCLNT-CHRTZ-OGG-I-0045
*******************************************************************************************************/

class RTestStepAPSetPositionPastDuration: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPositionPastDuration* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPositionPastDuration(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Set the current head position while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0046, MM-MMF-APLCLNT-CHRTZ-OGG-I-0046
*******************************************************************************************************/

class RTestStepAPSetPositionWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPositionWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPositionWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current head position while playing is in progress to a negative value.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0047, MM-MMF-APLCLNT-CHRTZ-OGG-I-0047
*******************************************************************************************************/

class RTestStepAPSetPositionNegativeWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPositionNegativeWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPositionNegativeWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current head position while playing is in progress to a value greater than the duration.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0048, MM-MMF-APLCLNT-CHRTZ-OGG-I-0048
*******************************************************************************************************/

class RTestStepAPSetPositionPastDurationWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPositionPastDurationWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPositionPastDurationWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current head position while playing has been paused.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0049, MM-MMF-APLCLNT-CHRTZ-OGG-I-0049
*******************************************************************************************************/

class RTestStepAPSetPositionWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPositionWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPositionWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the current head position while playing has been paused to a negative value.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0050, MM-MMF-APLCLNT-CHRTZ-OGG-I-0050
*******************************************************************************************************/

class RTestStepAPSetPositionNegativeWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPositionNegativeWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPositionNegativeWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current head position while playing has been paused to a value greater than the duration.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0051, MM-MMF-APLCLNT-CHRTZ-OGG-I-0051
*******************************************************************************************************/

class RTestStepAPSetPositionPastDurationWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPositionPastDurationWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPositionPastDurationWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current playback window.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0052, MM-MMF-APLCLNT-CHRTZ-OGG-I-0052
*******************************************************************************************************/

class RTestStepAPSetPlayWindow: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlayWindow* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlayWindow(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	TTime iStart;
	TTime iStop;
	};

/******************************************************************************************************
Set the current playback window while playing is in progress and the start is after the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0053, MM-MMF-APLCLNT-CHRTZ-OGG-I-0053
*******************************************************************************************************/

class RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlayWindowStartAfterHeadWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	
	// To hold the start and end time of the play window
	TTimeIntervalMicroSeconds iStart;
	TTimeIntervalMicroSeconds iEnd;
	};

/******************************************************************************************************
Set the current playback window while playing is in progress and the end is before the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0054, MM-MMF-APLCLNT-CHRTZ-OGG-I-0054
*******************************************************************************************************/

class RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlayWindowEndBeforeHeadWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current playback window while playing is in progress and the start is greater than the end.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0055, MM-MMF-APLCLNT-CHRTZ-OGG-I-0055
*******************************************************************************************************/

class RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlayWindowStartAfterEndWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current playback window while playing is paused and the start is after the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0056, MM-MMF-APLCLNT-CHRTZ-OGG-I-0056
*******************************************************************************************************/

class RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlayWindowStartAfterHeadWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current playback window while playing is paused and the end is before the current head position.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0057, MM-MMF-APLCLNT-CHRTZ-OGG-I-0057
*******************************************************************************************************/

class RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlayWindowEndBeforeHeadWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current playback window while playing is paused and the start is greater than the end.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0058, MM-MMF-APLCLNT-CHRTZ-OGG-I-0058
*******************************************************************************************************/

class RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetPlayWindowStartAfterEndWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Clear the current playback window.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0059, MM-MMF-APLCLNT-CHRTZ-OGG-I-0059
*******************************************************************************************************/

class RTestStepAPClearPlayWindow: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPClearPlayWindow* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPClearPlayWindow(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	TTime iStart;
	TTime iStop;
	};

/******************************************************************************************************
Clear the current playback window while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0060, MM-MMF-APLCLNT-CHRTZ-OGG-I-0060
*******************************************************************************************************/

class RTestStepAPClearPlayWindowWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPClearPlayWindowWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPClearPlayWindowWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	
	TTime iStart;
	TTime iStop;
	};

/******************************************************************************************************
Clear the current playback window before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0061
*******************************************************************************************************/

class RTestStepAPClearPlayWindowBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPClearPlayWindowBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPClearPlayWindowBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	TTime iStart;
	TTime iStop;
	};

/******************************************************************************************************
Set the current playback balance.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0062, MM-MMF-APLCLNT-CHRTZ-OGG-I-0062
*******************************************************************************************************/

class RTestStepAPSetBalance: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalance* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalance(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Set the current playback balance to less than KMMFBalanceMaxLeft.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0063, MM-MMF-APLCLNT-CHRTZ-OGG-I-0063
*******************************************************************************************************/

class RTestStepAPSetBalanceLessMaxLeft: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceLessMaxLeft* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceLessMaxLeft(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Set the current playback balance to greater than KMMFBalanceMaxRight.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0064, MM-MMF-APLCLNT-CHRTZ-OGG-I-0064
*******************************************************************************************************/

class RTestStepAPSetBalanceGreaterMaxRight: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceGreaterMaxRight* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceGreaterMaxRight(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Set the current playback balance while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0065, MM-MMF-APLCLNT-CHRTZ-OGG-I-0065
*******************************************************************************************************/

class RTestStepAPSetBalanceWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the current playback balance to less than KMMFBalanceMaxLeft while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0066, MM-MMF-APLCLNT-CHRTZ-OGG-I-0066
*******************************************************************************************************/

class RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceLessMaxLeftWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current playback balance to greater than KMMFBalanceMaxRight while playing is in progress.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0067, MM-MMF-APLCLNT-CHRTZ-OGG-I-0067
*******************************************************************************************************/

class RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceGreaterMaxRightWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the current playback balance while playing is paused.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0068, MM-MMF-APLCLNT-CHRTZ-OGG-I-0068
*******************************************************************************************************/

class RTestStepAPSetBalanceWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the current playback balance to less than KMMFBalanceMaxLeft while playing is paused.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0069, MM-MMF-APLCLNT-CHRTZ-OGG-I-0069
*******************************************************************************************************/

class RTestStepAPSetBalanceLessMaxLeftWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceLessMaxLeftWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceLessMaxLeftWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Set the current playback balance to greater than KMMFBalanceMaxRight while playing is paused.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0070, MM-MMF-APLCLNT-CHRTZ-OGG-I-0070
*******************************************************************************************************/

class RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceGreaterMaxRightWhileClipPaused(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Set the current playback balance before opening the clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0071
*******************************************************************************************************/

class RTestStepAPSetBalanceBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPSetBalanceBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPSetBalanceBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Get the current playback balance.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0072, MM-MMF-APLCLNT-CHRTZ-OGG-I-0072
*******************************************************************************************************/

class RTestStepAPGetBalance: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetBalance* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetBalance(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Get the current playback balance while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0073, MM-MMF-APLCLNT-CHRTZ-OGG-I-0073
*******************************************************************************************************/

class RTestStepAPGetBalanceWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetBalanceWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetBalanceWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Get the current playback balance before opening the clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0074
*******************************************************************************************************/

class RTestStepAPGetBalanceBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetBalanceBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetBalanceBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Get the bit rate of the audio clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0075, MM-MMF-APLCLNT-CHRTZ-OGG-I-0075
*******************************************************************************************************/

class RTestStepAPGetBitrate: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetBitrate* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetBitrate(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Get the bit rate of the audio clip while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0076, MM-MMF-APLCLNT-CHRTZ-OGG-I-0076
*******************************************************************************************************/

class RTestStepAPGetBitrateWhilePlayingClip: public RTestBaseAudioPlay

	{
public:
	static RTestStepAPGetBitrateWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetBitrateWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Get the bit rate of the audio clip before opening the clip.

MM-MMF-APLCLNT-CHRTZ-NONE-I-0077
*******************************************************************************************************/

class RTestStepAPGetBitrateBeforeOpenClip: public RTestBaseAudioPlay

	{
public:
	static RTestStepAPGetBitrateBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetBitrateBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Register the event for notification.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0078, MM-MMF-APLCLNT-CHRTZ-OGG-I-0078
*******************************************************************************************************/
class RTestStepAPRegisterEventNotification: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPRegisterEventNotification* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPRegisterEventNotification(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Register the event for notification while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0079, MM-MMF-APLCLNT-CHRTZ-OGG-I-0079
*******************************************************************************************************/
class RTestStepAPRegisterEventNotificationWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPRegisterEventNotificationWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPRegisterEventNotificationWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
	
/******************************************************************************************************
Register the event for notification before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0080, MM-MMF-APLCLNT-CHRTZ-OGG-I-0080
*******************************************************************************************************/
class RTestStepAPRegisterEventNotificationBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPRegisterEventNotificationBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPRegisterEventNotificationBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Register the event for notification with an event Uid that is not supported.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0082, MM-MMF-APLCLNT-CHRTZ-OGG-I-0082
*******************************************************************************************************/

class RTestStepAPRegisterEventNotificationUidNotSupported: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPRegisterEventNotificationUidNotSupported* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPRegisterEventNotificationUidNotSupported(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Register the event for notification with an event Uid that is already registered.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0083, MM-MMF-APLCLNT-CHRTZ-OGG-I-0083
*******************************************************************************************************/

class RTestStepAPRegisterEventNotificationUidAlreadyRegistered: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPRegisterEventNotificationUidAlreadyRegistered* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPRegisterEventNotificationUidAlreadyRegistered(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Cancel the registered notification event.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0084, MM-MMF-APLCLNT-CHRTZ-OGG-I-0084
*******************************************************************************************************/

class RTestStepAPCancelRegisteredNotificationEvent: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPCancelRegisteredNotificationEvent* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPCancelRegisteredNotificationEvent(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
	
/******************************************************************************************************
Cancel the registered notification event while playing an audio file.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0085, MM-MMF-APLCLNT-CHRTZ-OGG-I-0085
*******************************************************************************************************/

class RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPCancelRegisteredNotificationEventWhilePlayingClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};

/******************************************************************************************************
Cancel the registered notification event with an event Uid that is not matched.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0086, MM-MMF-APLCLNT-CHRTZ-OGG-I-0086
*******************************************************************************************************/

class RTestStepAPCancelRegisteredNotificationEventUidNotMatched: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPCancelRegisteredNotificationEventUidNotMatched* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPCancelRegisteredNotificationEventUidNotMatched(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Cancel the registered notification event with an event Uid of NULL.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0087, MM-MMF-APLCLNT-CHRTZ-OGG-I-0087
*******************************************************************************************************/

class RTestStepAPCancelRegisteredNotificationEventUidNULL: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPCancelRegisteredNotificationEventUidNULL* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPCancelRegisteredNotificationEventUidNULL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};
		
/******************************************************************************************************
Try to get the number of meta data entries.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0089, MM-MMF-APLCLNT-CHRTZ-OGG-I-0089
*******************************************************************************************************/

class RTestStepAPGetNumberOfMetaDataEntries: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetNumberOfMetaDataEntries* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetNumberOfMetaDataEntries(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError);
	
	// Not all controllers support metadata so iTestExpError is used to determine expected behaviour.
	// This should be initialised in the constructor.
	TInt iTestExpError;
	};	

/******************************************************************************************************
Try to get a meta data entry.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0090, MM-MMF-APLCLNT-CHRTZ-OGG-I-0090
*******************************************************************************************************/

class RTestStepAPGetMetaDataEntry: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetMetaDataEntry* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetMetaDataEntry(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError);
	
	// Not all controllers support metadata so iTestExpError is used to determine expected behaviour.
	// This should be initialised in the constructor.
	TInt iTestExpError;
	};
	
/******************************************************************************************************
Try to get the number of meta data entries while in the EPlaying state.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0091, MM-MMF-APLCLNT-CHRTZ-OGG-I-0091
*******************************************************************************************************/

class RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetNumberOfMetaDataEntriesWhilePlaying(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	
	// Not all controllers support metadata so iTestExpError is used to determine expected behaviour.
	// This should be initialised in the constructor.
	TInt iTestExpError;
	};

/******************************************************************************************************
Try to get a meta data entry while in the EPlaying state.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0092, MM-MMF-APLCLNT-CHRTZ-OGG-I-0092
*******************************************************************************************************/

class RTestStepAPGetMetaDataEntryWhilePlaying: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetMetaDataEntryWhilePlaying* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetMetaDataEntryWhilePlaying(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName, TInt aTestExpError);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	
	// Not all controllers support metadata so iTestExpError is used to determine expected behaviour.
	// This should be initialised in the constructor.
	TInt iTestExpError;
	};
	
/******************************************************************************************************
Get the the number of meta data entries before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0093, MM-MMF-APLCLNT-CHRTZ-OGG-I-0093
*******************************************************************************************************/

class RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetNumberOfMetaDataEntriesBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};	

/******************************************************************************************************
Get the meta data entries before opening the audio clip.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0094, MM-MMF-APLCLNT-CHRTZ-OGG-I-0094
*******************************************************************************************************/

class RTestStepAPGetMetaDataEntryBeforeOpenClip: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPGetMetaDataEntryBeforeOpenClip* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPGetMetaDataEntryBeforeOpenClip(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	};

/******************************************************************************************************
Test added as part of CR1566 (TruePause)
Test should work in both A3F and non-A3F even when the DevSound implementation doesn't support true pause
Play-Pause-Play.

MM-MMF-APLCLNT-CHRTZ-WAV-I-0095, MM-MMF-APLCLNT-CHRTZ-OGG-I-0095
MM-MMF-APLCLNT-CHRTZ-AMR-I-0095, MM-MMF-APLCLNT-CHRTZ-MP3-I-0095, MM-MMF-APLCLNT-CHRTZ-AAC-I-0095
*******************************************************************************************************/

class RTestStepAPPlayPausePlay: public RTestBaseAudioPlay
	{
public:
	static RTestStepAPPlayPausePlay* NewL(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
 
	// Override from RAsyncTestStep
	 void DoKickoffTestL();
	
	// State Machine Function
	void Fsm();
			
private:
	RTestStepAPPlayPausePlay(const TDesC& aStepName, const TDesC& aSectName, const TDesC& aKeyName);
	
	// Callback handler for Timed tests
	void DoTimerCallback();
	};
		
#endif // AUDIOPLAYTEST_H
