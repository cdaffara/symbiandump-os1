
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef DEVSOUNDPLUGIN_H
#define DEVSOUNDPLUGIN_H

_LIT8(KDevSoundPluginMatchString, "*"); // ECom insists on something

/**
Interface class used in the plugin implementation of DevSound.
The CMMFDevSound implementation loads a plugin based on this interface class. 
Once this has been constructed, calls to method functions of CMMFDevSound are passed
verbatim to this interface. For further description of required functionality, 
see CMMFDevSound.

@see CMMFDevSound
*/

class MMMFDevSoundPlugin
	{
public:
	/**	
	This must provide an implementation as defined by CMMFDevSound::~CMMFDevSound()
	
	@see CMMFDevSound::~CMMFDevSound()
	*/
	virtual ~MMMFDevSoundPlugin() {}

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode)
	
	@param  aDevSoundObserver
	        A reference to DevSound Observer instance.
	@param  aMode
	        The mode for which this object will be used.
	        
	@see CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode)
	*/
    virtual void InitializeL(MDevSoundObserver& aDevSoundObserver, TMMFState aMode)=0;


	/**
	This must provide an implementation as defined by 
	CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode)
	
	@param  aDevSoundObserver
	        A reference to DevSound Observer instance.
	@param  aHWDev
	        The CMMFHwDevice implementation identifier.
	@param  aMode
	        The mode for which this object will be used.
	        
	@see CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode)
	*/
	virtual void InitializeL(MDevSoundObserver& aDevSoundObserver, TUid aHWDev, TMMFState aMode)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode)

	@param  aDevSoundObserver
	        A reference to DevSound Observer instance.
	@param  aDesiredFourCC
	        The CMMFHwDevice implementation FourCC code.
	@param  aMode
	        The mode for which this object will be used.
	        
	@see CMMFDevSound::InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode)
	*/
	virtual void InitializeL(MDevSoundObserver& aDevSoundObserver, TFourCC aDesiredFourCC, TMMFState aMode)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::Capabilities()
	
	@return	The device settings.
	
	@see CMMFDevSound::Capabilities()
	*/
	virtual TMMFCapabilities Capabilities()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::Config()
	
	@return	The device settings.
	
	@see CMMFDevSound::Config()
	*/
	virtual TMMFCapabilities Config() const=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::SetConfigL(const TMMFCapabilities& aCaps)

	@param  aCaps The attribute values to which CMMFDevSound object will be configured to.
	        
	@see CMMFDevSound::SetConfigL(const TMMFCapabilities& aCaps)
	*/
	virtual void SetConfigL(const TMMFCapabilities& aCaps)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::MaxVolume()
	
	@return	The maximum volume. This value is platform dependent but is always greater than or equal
	to one.
	
	@see CMMFDevSound::MaxVolume()
	*/
	virtual TInt MaxVolume()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::Volume()
	
	@return The current volume level.
		
	@see CMMFDevSound::Volume()
	*/
	virtual TInt Volume()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::SetVolume()
	
	@param  aVolume
	        The volume setting. This can be any value from 0 to the value
	        returned by a call to CMMFDevSound::MaxVolume(). If the
	        volume is not within this range, the volume is automatically set to
	        minimum or maximum value based on the value that is being passed.
	        Setting a zero value mutes the sound. Setting the maximum value
	        results in the loudest possible sound.
        
	@see CMMFDevSound::SetVolume()
	*/
	virtual void SetVolume(TInt aVolume)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::MaxGain()
	
	@return The maximum gain. This value is platform dependent but is always greater than or equal
	to one.
	
	@see CMMFDevSound::MaxGain()
	*/
	virtual TInt MaxGain()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::Gain()
	
	@return The current gain level.
	
	@see CMMFDevSound::Gain()
	*/
	virtual TInt Gain()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::SetGain()
	
	@param  aGain
		    The gain setting. This can be any value from zero to the value
		    returned by a call to CMMFDevSound::MaxGain(). If the
		    volume is not within this range, the gain is automatically set to
		    minimum or maximum value based on the value that is being passed.
		    Setting a zero value mutes the sound. Setting the maximum value
		    results in the loudest possible sound.
	@see CMMFDevSound::SetGain()
	*/
	virtual void SetGain(TInt aGain)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	
	@param  aLeftPercentage
	        On return contains the left speaker volume percentage.
	@param  aRightPercentage
	        On return contains the right speaker volume percentage.
	        
	@see CMMFDevSound::GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	*/
	virtual void GetPlayBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	
	@param  aLeftPercentage
	        On return contains left speaker volume perecentage. This can be any
	        value from zero to 100. Setting a zero value mutes the sound on left
	        speaker.
	@param  aRightPercentage
	        On return contains right speaker volume perecentage. This can be any
	        value from zero to 100. Setting a zero value mutes the sound on
	        right speaker.
	        
	@see CMMFDevSound::SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	*/
	virtual void SetPlayBalanceL(TInt aLeftPercentage, TInt aRightPercentage)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	
	@param  aLeftPercentage
	        On return contains the left microphone gain percentage.
	@param  aRightPercentage
	        On return contains the right microphone gain percentage.
	        
	@see CMMFDevSound::GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)
	*/
	virtual void GetRecordBalanceL(TInt& aLeftPercentage, TInt& aRightPercentage)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::GetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	
	@param  aLeftPercentage
	        The left microphone gain precentage. This can be any value from zero to
	        100. Setting a zero value mutes the gain on left microphone.
	@param  aRightPercentage
	        The right microphone gain precentage. This can be any value from zero to
	        100. Setting a zero value mutes the gain on right microphone.
	        
	@see CMMFDevSound::GetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage)
	*/
	virtual void SetRecordBalanceL(TInt aLeftPercentage, TInt aRightPercentage)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::PlayInitL()
	
	@see CMMFDevSound::PlayInitL()
	*/
	virtual void PlayInitL()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::RecordInitL()

	@see CMMFDevSound::RecordInitL()
	*/
	virtual void RecordInitL()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::PlayData()
	
	@see CMMFDevSound::PlayData()
	*/
	virtual void PlayData()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::RecordData()
	
	@see CMMFDevSound::RecordData()
	*/
	virtual void RecordData()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::Stop()
	
	@see CMMFDevSound::Stop()
	*/
	virtual void Stop()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::Pause()
	
	@see CMMFDevSound::Pause()
	*/
	virtual void Pause()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::SamplesRecorded()
	
	@return The samples recorded.
	
	@see CMMFDevSound::SamplesRecorded()
	*/
	virtual TInt SamplesRecorded()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::SamplesPlayed()
	
	@return The samples played.
	
	@see CMMFDevSound::SamplesPlayed()
	*/
	virtual TInt SamplesPlayed()=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	
	@param  aFrequency
	        The frequency at which the tone will be played.
	@param  aDuration
	        The period over which the tone will be played. A zero value causes
	        the no tone to be played.
	        
	@see CMMFDevSound::PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)
	*/
	virtual void PlayToneL(TInt aFrequency, const TTimeIntervalMicroSeconds& aDuration)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	
	@param  aFrequencyOne
	        The first frequency of dual tone.
	@param  aFrequencyTwo
	        The second frequency of dual tone
	@param  aDuration
	        The period over which the tone will be played. A zero value causes
	        the no tone to be played (Verify this with test app).
	        
	@see CMMFDevSound::PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)
	*/
	virtual void PlayDualToneL(TInt aFrequencyOne, TInt aFrequencyTwo, const TTimeIntervalMicroSeconds& aDuration)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::PlayDTMFStringL(const TDesC& aDTMFString)
	
	@param  aDTMFString The DTMF sequence in a descriptor.
	
	@see CMMFDevSound::PlayDTMFStringL(const TDesC& aDTMFString)
	*/
	virtual void PlayDTMFStringL(const TDesC& aDTMFString)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::PlayToneSequenceL(const TDesC8& aData)

	@param  aData The tone sequence in a descriptor.
	
	@see CMMFDevSound::PlayToneSequenceL(const TDesC8& aData)
	*/
	virtual void PlayToneSequenceL(const TDesC8& aData)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::PlayFixedSequenceL(TInt aSequenceNumber)
	
	@param  aSequenceNumber
	        The index identifying the specific pre-defined tone sequence. Index
	        values are relative to zero.
	        This can be any value from zero to the value returned by a call to
	        FixedSequenceCount() - 1.
	        The function raises a panic if the sequence number is not within this
	        range.
	        
	@see CMMFDevSound::PlayFixedSequenceL(TInt aSequenceNumber)
	@see FixedSequenceCount()
	*/
	virtual void PlayFixedSequenceL(TInt aSequenceNumber)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::SetToneRepeats(TInt aRepeatCount,
					const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	
	@param  aRepeatCount
	        The number of times the tone, together with the trailing silence,
	        is to be repeated. If this is set to KMdaRepeatForever, then the
	        tone, together with the trailing silence, is repeated indefinitely
	        or until Stop() is called. If this is set to zero, then the tone is
	        not repeated.
	@param  aRepeatTrailingSilence
	        An interval of silence which will be played after each tone.
	        Supported only during tone playing.
	        
	@see CMMFDevSound::SetToneRepeats(TInt aRepeatCount,
					const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)
	*/
	virtual void SetToneRepeats(TInt aRepeatCount,
					const TTimeIntervalMicroSeconds& aRepeatTrailingSilence)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::(TTimeIntervalMicroSeconds32& aToneOnLength,
									TTimeIntervalMicroSeconds32& aToneOffLength,
									TTimeIntervalMicroSeconds32& aPauseLength)

	@param  aToneOnLength
	        The period over which the tone will be played. If this is set to
	        zero, then the tone is not played.
	@param  aToneOffLength
	        The period over which the no tone will be played.
	@param  aPauseLength
	        The period over which the tone playing will be paused.
	        
	@see CMMFDevSound::(TTimeIntervalMicroSeconds32& aToneOnLength,
									TTimeIntervalMicroSeconds32& aToneOffLength,
									TTimeIntervalMicroSeconds32& aPauseLength)
	*/
	virtual void SetDTMFLengths(TTimeIntervalMicroSeconds32& aToneOnLength,
									TTimeIntervalMicroSeconds32& aToneOffLength,
									TTimeIntervalMicroSeconds32& aPauseLength)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	
	@param  aRampDuration
	        The period over which the volume is to rise. A zero value causes
	        the tone sample to be played at the normal level for the full
	        duration of the playback. A value, which is longer than the duration
	        of the tone sample means that the sample never reaches its normal
	        volume level.
	        
	@see CMMFDevSound::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	*/
	virtual void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)

	@param	aPrioritySettings 
			A class type representing the client's priority, priority preference and state.
			
	@see CMMFDevSound::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)
	*/
	virtual void SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::CustomInterface(TUid aInterfaceId)
	
	@param  aInterfaceId
	        The interface UID, defined with the custom interface.
	
	@return A pointer to the interface implementation, or NULL if the device does not
	        implement the interface requested. The return value must be cast to the
	        correct type by the user.The user should be careful while caching the custom interface pointer,
	        as in some situations the lower level custom interface pointer may become NULL
	        
	@see CMMFDevSound::CustomInterface(TUid aInterfaceId)
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId)=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::FixedSequenceCount()
	
	@return The fixed sequence count. This value is implementation dependent but is always greater 
	        than or equal to zero.
	        
	@see CMMFDevSound::FixedSequenceCount()
	*/
	virtual TInt FixedSequenceCount()=0;

	/**
	This must provide an implementation as defined by CMMFDevSound::FixedSequenceName(TInt aSequenceNumber)
	
	@param  aSequenceNumber
	        The index identifying the specific pre-defined tone sequence. Index values are relative 
	        to zero. This can be any value from zero to the value returned by a call to
	        FixedSequenceCount() - 1.
	        The function raises a panic if sequence number is not within this
	        range.

	@return A reference to a Descriptor containing the fixed sequence name indexed by
	        aSequenceNumber.
	        
	@see CMMFDevSound::FixedSequenceName(TInt aSequenceNumber)
	@see FixedSequenceCount()
	*/
	virtual const TDesC& FixedSequenceName(TInt aSequenceNumber)=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes,
													const TMMFPrioritySettings& aPrioritySettings) const
	
	@param  aSupportedDataTypes
	        The array of supported data types that will be filled in by this function.
	        The supported data types of the DevSound are in the form of an array
	        of TFourCC codes. Any existing entries in the array will be overwritten on 
	        calling this function. If no supported data types are found given the priority 
	        settings, then the aSupportedDatatypes array will have zero entries.
	@param  aPrioritySettings
	        The priority settings used to determine the supported datatypes.  Note this
	        does not set the priority settings. For input datatypes the iState member
	        of the priority settings would be expected to be either
	        EMMFStatePlaying or EMMFStatePlayingRecording. The priority settings may
	        effect the supported datatypes depending on the audio routing.
	        
	@see CMMFDevSound::GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes,
													const TMMFPrioritySettings& aPrioritySettings) const
	*/
	virtual void GetSupportedInputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const=0;

	/**
	This must provide an implementation as defined by 
	CMMFDevSound::GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const
	
	@param  aSupportedDataTypes
	        The array of supported data types that will be filled in by this function.
	        The supported datatypes of the DevSound are in the form of an array
	        of TFourCC codes.
	        Any existing entries in the array will be overwritten on calling this function.
	        If no supported datatypes are found given the priority settings, then
	        the aSupportedDatatypes array will have zero entries.
	@param  aPrioritySettings
	        The priority settings used to determine the supported data types.  Note this
	        does not set the priority settings. For output data types the iState member
	        of the priority settings would expected to be either
	        EMMFStateRecording or EMMFStatePlayingRecording. The priority settings may
	        effect the supported datatypes depending on the audio routing.
	        
	@see CMMFDevSound::GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const
	*/
	virtual void GetSupportedOutputDataTypesL(RArray<TFourCC>& aSupportedDataTypes, const TMMFPrioritySettings& aPrioritySettings) const=0;
	
	};

#endif // DEVSOUNDPLUGIN_H
