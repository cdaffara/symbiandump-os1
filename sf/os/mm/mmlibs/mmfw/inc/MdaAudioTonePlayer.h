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
//

#ifndef __MDAAUDIOTONEPLAYER_H__
#define __MDAAUDIOTONEPLAYER_H__


#include <e32std.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmfclntutility.h>

class CMdaServer;


/**
@publishedAll
@released

The state of the audio tone utility.
*/
enum TMdaAudioToneUtilityState
	{
	/** The audio tone utility has not been prepared to play a tone.
	*/
	EMdaAudioToneUtilityNotReady=0,
	/** The audio tone utility has been prepared to play a tone and is ready.
	*/
	EMdaAudioToneUtilityPrepared,
	/** The audio tone utility is currently playing a tone.
	*/
	EMdaAudioToneUtilityPlaying,
	/** The audio tone utility is currently paused.
	This state is only reached if underlying DevSound implementations supports resume.
	*/
	EMdaAudioToneUtilityPaused
	};

const TInt KMdaInfiniteDurationDTMFToneOnLength = -1; 
const TInt KMdaAudioToneRepeatForever = -2;

/**
@publishedAll
@released

An interface to a set of audio tone player callback functions.

The class is a mixin and is intended to be inherited by the client
class which is observing the audio tone playing operation. The
functions encapsulated by this class are called when specific events
occur in the process of preparing to play a tone and during the
playing of a tone. A reference to this object is passed as a
parameter when constructing an audio tone player utility, using the
CMdaAudioToneUtility interface.

@since 5.0
*/
class MMdaAudioToneObserver
	{
public:

	/**
	Defines required client behaviour when an attempt to configure the
	audio tone player utility has completed, successfully or otherwise.

	@param     aError
	           KErrNone if successful. One of the system
	           wide error codes if the attempt failed. The possible
	           values depend on the EPOC platform.
	*/
	virtual void MatoPrepareComplete(TInt aError) = 0;

	/**
	Defines required client behaviour when the tone playing operation has
	completed, successfully or otherwise.

	@param     aError
	           KErrNone if successful. One of the system
	           wide error codes if the attempt failed. The possible
	           values depend on the EPOC platform.
	*/
	virtual void MatoPlayComplete(TInt aError) = 0;
	};


class MMdaAudioTonePlayStartObserver
	{
public:
	/**
	Defines required client behaviour when the tone playing operation has
	started, successfully or otherwise.

	@param     aError
	           KErrNone if successful. One of the system
	           wide error codes if the attempt failed. The possible
	           values depend on the EPOC platform.
	*/
	virtual void MatoPlayStarted(TInt aError) = 0;
	};


class CMMFMdaAudioToneUtility;

/**
@publishedAll
@released

Generates tones on an audio capable EPOC device.

The class offers an interface for generating tones on all audio
capable EPOC devices.

To use the tone utility:

1. Create an instance by calling NewL().

2. Call the appropriate PrepareToPlay variant for the required tone type
and wait for the callback indicating success.

3. Call Play and either wait for the callback to indicate completion,
or call CancelPlay to end playback early.

4. Delete the instance.

It is possible to call Play before calling any PrepareToPlay variant. This
will result in a default fixed sequence tone being played.

@since 5.0
*/
class CMdaAudioToneUtility : public CBase,
							 public MMMFClientUtility
	{
public:

	IMPORT_C static CMdaAudioToneUtility* NewL(MMdaAudioToneObserver& aObserver, CMdaServer* aServer = NULL);

	IMPORT_C static CMdaAudioToneUtility* NewL(MMdaAudioToneObserver& aObserver, CMdaServer* aServer,
											   TInt aPriority, 
											   TInt aPref = EMdaPriorityPreferenceTimeAndQuality);

	~CMdaAudioToneUtility();

	virtual TMdaAudioToneUtilityState State();

	virtual TInt MaxVolume();

	virtual TInt Volume();

	virtual void SetVolume(TInt aVolume);

	virtual void SetPriority(TInt aPriority, TInt aPref);

	virtual void SetDTMFLengths(TTimeIntervalMicroSeconds32 aToneLength,
								TTimeIntervalMicroSeconds32 aToneOffLength,
								TTimeIntervalMicroSeconds32 aPauseLength);

	virtual void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);

	virtual void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

	virtual TInt FixedSequenceCount();

	virtual const TDesC& FixedSequenceName(TInt aSequenceNumber);

	virtual void PrepareToPlayTone(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration);

	virtual void PrepareToPlayDTMFString(const TDesC& aDTMF);

	virtual void PrepareToPlayDesSequence(const TDesC8& aSequence);

	virtual void PrepareToPlayFileSequence(const TDesC& aFileName);

	virtual void PrepareToPlayFixedSequence(TInt aSequenceNumber);

	virtual void CancelPrepare();

	virtual void Play();

	virtual void CancelPlay();

	IMPORT_C TInt Pause();

	IMPORT_C TInt Resume();
	
	IMPORT_C void SetBalanceL(TInt aBalance=KMMFBalanceCenter);

	IMPORT_C TInt GetBalanceL();

	IMPORT_C void PrepareToPlayDualTone(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration);
	
	IMPORT_C void PrepareToPlayFileSequence(RFile& aFile);
	
	IMPORT_C TAny* CustomInterface(TUid aInterfaceId);

	IMPORT_C void RegisterPlayStartCallback(MMdaAudioTonePlayStartObserver& aObserver);

protected:
	/**
	This member is internal and not intended for use.
	*/
	CMMFMdaAudioToneUtility* iProperties;
	};


#endif
