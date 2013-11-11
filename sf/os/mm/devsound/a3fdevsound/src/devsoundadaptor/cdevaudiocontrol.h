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




#ifndef CDEVAUDIOCONTROL_H
#define CDEVAUDIOCONTROL_H

#include <e32base.h>

#include "cdevaudio.h"

#include <a3f/maudiostreamobserver.h>
#include <a3f/maudiocodecobserver.h>
#include <a3f/maudiogaincontrolobserver.h>
#include <a3f/msynccisupport.h>
#include <a3f/a3f_trace_utils.h>


class MAudioCodec;
class MAudioGainControl;

/**
*  Panic category and codes that DevSoundAdaptor raises on the client
*/
enum TDevSoundAdaptorPanicCode
	{
	ECommitFailedDuringStop = 1, 
	ECommitFailedDuringPause,
	EStreamBeingDemotedToEIdle,
	EAudioCodecIsNull,
	EStreamMismatch,
	EBufferMismatch,
	EInvalidStateDuringPreemptionCycle
	};


/**
 * CDevAudioControl.
 *
 * ?description
 *
 * @lib mmfdevsoundadaptation.lib
 * @since
 */
NONSHARABLE_CLASS(CDevAudioControl) :	public CBase,
										public MAudioStreamObserver,
										public MAudioGainControlObserver,
										public MAudioContextObserver,
										public MCustomInterfaceSupportObserver,
										public MAudioCodecObserver
	{

public:

	enum TStereoCh
		{
		ELeftCh=0,
		ERightCh=1
		};

	~CDevAudioControl();

	/**
	* Initialize wanted state control components
	* @since
	* @return error code
	*/
	virtual TInt Initialize(TUid aFormat);

	/**
	* Uninitialize wanted state control components
	* @since
	* @return error code
	*/
	virtual TInt Uninitialize();

	/**
	* Remove any processing unit
	* @since
	* @return error code
	*/
	virtual TInt RemoveProcessingUnits();

	/**
	* Returns the supported Audio settings ie. encoding, sample rates,
	* mono/stereo operation, buffer size etc..
	* @since
	* @param aCaps on return, contains supported capabilities
	* @return error code
	*/
	TInt GetCapabilities(TMMFCapabilities& aCap);

	/**
	* Returns the current device configuration.
	* @since
	* @param aConfig on return, contains device settings.
	* @return error code
	*/
	TInt GetConfig(TMMFCapabilities& aConfig);

	/**
	* Configure CMMFDevSound object with the settings in aConfig. Use this
	* to set sampling rate, encoding and mono/stereo.
	* Leaves on failure.
	* @since
	* @param const TMMFCapabilities& aConfig The attribute values to which
	*        CMMFDevSound object will be configured to.
	* @return error code
	*/
	TInt SetConfig(const TMMFCapabilities& aCaps);

	/**
	* Initializes and starts the wanted operation (Play, Record, TonePlay).
	* @since
	* @return error code
	*/
	virtual TInt ProcessInit();

	/**
	* Processes the data (PlayData, RecordData).
	* @since
	* @return void
	*/
	virtual void ProcessData();

	/**
	* Stops the ongoing operation (Play, Record, TonePlay).
	* @since
	* @return void
	*/
	virtual TInt Stop();

	/**
	* Temporarily Stops the ongoing operation (Play, Record, TonePlay).
	* @since
	* @return void
	*/
	virtual TInt Pause();

	/**
	* Returns the samples played/recorded so far
	* @since
	* @return TInt Returns the samples played/recorded.
	*/
	virtual TInt GetSamples();

	/**
	* Retrieves a custom interface to the device.
	* @since
	* @param TUid aInterfaceId The interface UID, defined with the custom
	*        interface.
	* @return TAny* A pointer to the interface implementation, or NULL if
	*        the device does not implement the interface requested. The
	*        return value must be cast to the correct type by the user.
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId);

	/**
	* Apply gains, balance. Scale gain to underlying (a3f) max gain.
	* Assumed to be in active state. When called for SetVolume() etc call on higher-level
	* aCommit should be ETrue, and will call Commit() if derived result is KErrNone.
	* @param aDevSoundGain the DevSound gain to be applied
	* @param aDevSoundMaxGain the cached A3F Max Gain to be used as boundary
	* @param aBalance the balance value to be applied
	* @param const TTimeIntervalMicroSeconds &aRampDuration The period over
	*        which the volume is to rise. A zero value causes the
	*        sample to be played at the normal level for the full duration
	*        of the playback. A value, which is longer than the duration of
	*        the overall clip means that the sample never reaches its normal
	*        volume level.
	* @param aBecomingActive indicates if DevSoundAdaptor is becoming active
	*        which is needed to avoid if a commit here could clash with the one that is going to activate the stream

	* @return error code
	*/
	TInt SetGains(TInt aDevSoundGain, TInt aDevSoundMaxGain, TInt aBalance[2], const TTimeIntervalMicroSeconds& aRampDuration, TBool aBecomingActive);

	/**
	* Maps "legacy" volume/gain values to CAP channel array
	* @since
	*/
	void MapGains();

	/**
	* Destroy logical chain
	* @since
	* @
	*/
	TBool DestroyChain();

	// From base class MAudioStreamObserver

	/**
	* Handles audio stream state change event.
	*/
	virtual void StateEvent(MAudioStream& aStream, TInt aReason, TAudioState aNewState);

	/**
	* Notifies that adding of processing unit to the stream has been completed
	* successfully or otherwise.
	*/
	virtual void AddProcessingUnitComplete(MAudioStream& aStream, MAudioProcessingUnit* aInstance, TInt aError);

	/**
	* Notifies that removing of processing unit from the stream has been completed
	* successfully or otherwise.
	*/
	virtual void RemoveProcessingUnitComplete(MAudioStream& aStream, MAudioProcessingUnit* aInstance, TInt aError);

	/*
	Call-back indicating that is the last buffer has been processed by the sink.
	*/
	virtual void ProcessingFinished (MAudioStream & aStream);

	/**
	* Signals completion of a Flush() operation.
	*/
	virtual void FlushComplete (MAudioStream& aStream, TInt aError);

	// From base class MAudioGainControlObserver

	/**
	* Notifies the observer that the max ramp time supported by the stream, has changed.
	*
	* @since
	* @param aStream a reference to the stream whose max ramp time changed.
	*/
	virtual void MaxRampTimeChanged(MAudioGainControl& aGain);

	/**
	* Notifies the observer that the maximum gain value supported by the stream has changed.
	*
	* @since
	* @param aStream a reference to the stream whose supported con
	*/
	virtual void MaxGainChanged(MAudioGainControl& aGain);

	/**
	* Notifies the observer that the stream gain has changes, due to request from the client
	* or otherwise.
	*
	* If gain change by the client cannot be fulfilled, a gain change with an error code
	* other than KErrNone will be issued.
	*
	* @since
	* @param aStream a reference to the stream whose gain has changed.
	* @param aStream an error code. KErrNone if the gain change was requested by the client
	*        and was completed successfully.
	*/
	virtual void GainChanged(MAudioGainControl& aGain, TInt aError);

	/**
	* Saves tonedata for later use
	* This includes information about tone type, data, length, etc.
	*
	* @since
	* @param aToneData
	* @return error code
	*/
	virtual TInt SetToneData(TToneData& aToneData);

	// From base class MAudioContextObserver
	/**
	* Callback to context observer to show progression through Commit() and pre-emption cycles
	*
	* @param aEvent  a Uid giving the specific event.
	* @param aError  an error code. KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual void ContextEvent(TUid aEvent, TInt aError);

	// from MCustomInterfaceSupportObserver
	virtual void CustomInterfaceRemoval(TUid, TAny* aPtr);

	//From MAudioCodecObserver

	virtual void SampleRateSet(TInt aError);

	virtual void ModeSet(TInt aError);

	virtual void GetSupportedSampleRatesComplete (TInt aError);

	virtual void GetSupportedModesComplete (TInt aError);

	/**
	* Called when a ProcessingFinished callback is received
	*
	* @since
	* @param TBool& aAyncCompletion
	* @return an error code KErrNone if successful
	*/
	virtual TInt ProcessingFinishedReceived(TBool& aAyncCompletion);

	virtual TInt ProcessingError(TBool& aAyncCompletion);
	
	/**
	* EmptyBuffers using A3F::Flush
	*
	* @since
	* @return an error code. KErrNone if successful
	*						KErrNotReady if not playing or paused
	*						KErrNotSupported if the operation is not supported
	*/
	virtual TInt RequestEmptyBuffers();

	/**
	* Gets the current play time from the audio renderer
	* @since 
	* @param TTimeIntervalMicroSeconds& aTime On return contains the current play time
	* @return an error code KErrNone if successful
	*/
	virtual TInt GetTimePlayed(TTimeIntervalMicroSeconds& aTime);

	/**
	* Resume the operation (Play, Record, TonePlay) temporarily paused .
	* @since
	* @return TInt KErrNone if succesful
	*              KErrNotSupported if the operation is not supported by this implementation
	*/
	virtual TInt Resume();

	/*
	Used to send a stop call when there is a error in the buffer
	*/
	virtual void BufferErrorEvent();
	
protected:

	CDevAudioControl();

	void ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver);

	// Ensure iAudioCodecIf is setup properly
	TInt CacheAudioCodecIf();

	/**
	* Returns to initialized state wanted control components
	* @since 
	* @return error code
	*/
	TInt Unload();


	/**
	* Panic the thread
	* @since 
	* @param panic code
	*/
	void Panic(TDevSoundAdaptorPanicCode aCode);
	
private:
	TInt ResolveMode(TUint aMode, TUid& aModeValue);
	TInt ResolveSampleRate(TInt aSampleRate, TInt& aSampleRateValue);
	TUint GetModes(const RArray<TUid>& aMode);
	TUint GetMode(TUid aMode);
	TUint GetSampleRates(const RArray<TInt>& aSampleRates);
	TUint GetSampleRate(TInt aSampleRates);
	void CompleteMessageCap(TInt aError);

/**
 * Member data is protected for subclass access
 */
protected: // data

	// Not own
	CDevAudio *iDevAudio;

	/**
	* Pointer to audio codec
	* Not own.
	*/
	MAudioCodec* iAudioCodecIf;

	/**
	* Pointer to audio gain control
	* Not own.
	*/
	MAudioGainControl* iGainControl;

	/**
	* Observer for callbacks to DevSound Framework
	* Not own.
	*/
	MDevSoundAdaptationObserver *iAdaptationObserver;


	/*
	* TODO: Review if this for buffer exchange
	*/
	CMMFBuffer* iBuffer;

	/**
	* Local cache of volume stuff
	*/
	TInt iLegacyGain;
	TInt iLegacyLeft;
	TInt iLegacyRight;

	TInt 	iError;
	TBool 	iGainUpdateNeeded;
	TBool	iStateEventReceived;
	TInt 	iStateEventError;
	TInt	iCallbackFromAdaptor;
	TInt 	iProcessingUnitError;

	TInt 	iErrorCondition;
	
	TBool	iObserverRegistered;
	
	//Indicates whether the AsyncOperationComplete callback needs to be made during Preemption
	TBool iIgnoreAsyncOpComplete;

	TBool	iPauseResumeSequenceDueToEmptyBuffers;

private:

	RArray<TAudioChannelGain>	iChannelGains;

	RArray<TUid>				iSupportedModes;
	RArray<TInt>				iSupportedRates;
	TInt						iDesiredSampleRate;
	TUid						iDesiredMode;
	TInt						iCurrentSampleRate;
	TUid						iCurrentMode;
	TInt						iOutstandingCallbacks;

	};

#endif // CDEVAUDIOCONTROL_H
