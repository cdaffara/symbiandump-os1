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




/**
 @file
 @publishedPartner
 @released
*/

#ifndef MAUDIOGAINCONTROL_H
#define MAUDIOGAINCONTROL_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/a3fbase.h>
#include <a3f/maudioprocessingunit.h>

class MAudioGainControlObserver;


/**
 * Interface to gain control processing unit.
 *
 * Gain control is used for controlling volume and gain related settings of an audio stream.
 */
class MAudioGainControl
	{
	public:

	/** 
	* Returns the maximum gain the device supports.
	*
	* The maximum gain value depends on the setup of the stream.
	* 
	* this is the maximum gain value which can be passed to SetGain().
	* @param aMaxGain On return, contains the maximum gain value, provided that the returned error code is KErrNone.
	* @return An error code. KErrNone if the maximum gain value is available, otherwise one of the system wide error codes.
	*/
	virtual TInt GetMaxGain(TInt& aMaxGain) const= 0 ;

	/**
	* Returns the maximum ramp time that is supported for the associated stream.
	*
	* The maximum ramp time may be different in playback and recording modes,
	* so the value should be checked before setting the ramptime. this function
	* also does not take into account any possible ongoing ramp.
	* 
	* this is the maximum ramp time value which can be passed to SetGain().
	* @param aMaxRampTime On return contains the maximum ramp time.
	* The value is platform and stream setup dependent but is always greater than or equal to zero.
	* @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	*/
	virtual TInt GetMaxRampTime(TTimeIntervalMicroSeconds& aMaxRampTime) const= 0;

	/**
	* Requests changes to the audio gain on a channel-by-channel basis. 

	* Note that the values for each channel are independent of each other. 
	* The call is asynchronous in nature, resulting in a GainChanged() 
	* callback, and the request is only applied following a successful 
	* Commit() cycle. 
	* The callback signals the receipt of the SetGain() call from the 
	* underlying adaptation, and not (say) the completion of a ramp operation. 
	* The actual gains will only change once the associated Audio Stream becomes EActive. 
	* If the associated Audio Stream is already EActive 
	* then the change will start immediately following the next successful Commit() cycle. 
	* MMRC may choose to modify or ignore the requested gains. 

	* @param aChannels	 Contains the channels for which the gain should be set and the gain value for those channels.
	* @param aRampOperation  How aRampDuration is to be interpreted.(see below)
	* @param aRampDuration Duration in microseconds.
	* @return KErrNone if initially successful.
	*         KErrNotSupported if the SetGain() call referred to non-existent channels.
	*         KErrArgument if aGain of less than 0 or larger than max gain, the same channel is specified multiple times, 
	*           or aRampDuration is less than 0 or greater than max ramp duration.
	*         KErrA3fUnsupportedRamp  if the ramp operation requested is not supported, or the Uid value supplies is unknown.
	* Other system-wide error codes are also possible.
	* These errors could instead by returned as part of the subsequent GainChanged() callback – depending on the a3f adaptation.
	* Also note: the requested parameters are specific to the associated stream. 
	* If more than one stream is concurrently active to the same device, 
	* it is up to the adaptation to arrange both the device gains the mixer and splitter gains to achieve a similar effect to the several streams being played through separate devices and mixed post-amplification.
	*/
	virtual TInt SetGain(RArray<TAudioChannelGain>& aChannels, TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration)=0;

	/**
	* Sets the audio channel gain.
	* 
	* By default all channels are set to use same gain value. Note that the
	* values for each channel are independent of each other, meaning that
	* they only control the gain level on which the particular channel is
	* being used and there is no limitation requiring the sum of all channels to be 100.
	* 
	* The gain values will be applied once the associated stream is active.
	* if the stream is active while this function is called, then the values
	* are applied immediately.
	*
	* @param aChannels Contains the channels for which the gain should be set and the gain value for those channels.
	* @return An error code. KErrNone on success.
	*                        KErrNotSupported if gain for unsupported channels is tried to be set.
	*                        KErrArgument if a value of less than 0 or larger than max gain is tried to be set as gain.
	*                        Other system wide error codes are also possible.
	*/
	virtual TInt SetGain(RArray<TAudioChannelGain>& aChannels)=0;

	/**
	* Returns channel specific gain settings.
	*
	* @param  aChannels On return contains current gain settings for each audio channel.
	* @return An error code. KErrNone on success.
	*                        KErrArgument if the array passed in as a parameter is not empty.
	*                        KErrOutOfMemory in case of memory exhaustion.
	*                        Other system wide error codes are also possible.
	*/
	virtual TInt GetGain(RArray<TAudioChannelGain>& aChannels) const= 0;

	/**
	* Registers an observer for receiving callbacks.
	*
	* @param aObserver a reference to the observer to register.
	* @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	*/
	virtual TInt RegisterAudioGainControlObserver(MAudioGainControlObserver& aObserver)=0;

	/**
	* Unregisters an observer.
	*
	* @param aObserver a reference to observer to unregister.
	*/
	virtual void UnregisterAudioGainControlObserver(MAudioGainControlObserver& aObserver)=0;

	};

#endif // MAUDIOGAINCONTROL_H
