/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DRM Audio player
*
*/



#ifndef DRMAUDIOSAMPLEPLAYER_H
#define DRMAUDIOSAMPLEPLAYER_H



#include <e32base.h>
#include <mdaaudiosampleplayer.h>

// FORWARD DECLARATIONS
class CDrmAudioPlayerAdaptation;

// CLASS DECLARATION

/**
*  Interface class for DRM Player callbacks
*
*
*  @lib DRMAudioPlayer.lib
*  @since Series 60 3.0
*/

class MDrmAudioPlayerCallback
	{
public:
        /**
        * Called by CDrmPlayerUtility when initialization is complete.
        * @since Series 60 3.0
		* @param aError The status of the audio sample after initialisation.
	        The following values have the same specific meaning
	        across all EPOC platforms: KErrNone the
	        sample is ready to play. KErrNotSupported
	        the audio format or particular encoding type is not
	        recognised or not supported. KErrNotFound
	        the audio sample cannot be found.
	        KErrNoMemory there is insufficient memory
	        to play this audio sample. Other values are possible
	        indicating a problem opening the audio sample. These
	        values are dependent on the EPOC platform.
        * @param aDuration The duration of the audio sample.
        */

	virtual void MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration) = 0;

        /**
        * Called by CDrmPlayerUtility when playing is complete.
        * @since Series 60 3.0
        * @param aError
		     The status of playback. The following values have the
	         same specific meaning across all EPOC platforms:
	         KErrNone playing of the audio sample is
	         complete. KErrCorrupt the sample data is
	         corrupt. KErrInUse the sound device is in
	         use by another higher priority client. This can happen
	         during playback. KErrNoMemory there is
	         insufficient memory to play this audio sample Other
	         values are possible indicating a problem opening the
	         audio sample. These values are dependent on the EPOC
	         platform.
        */
	virtual void MdapcPlayComplete(TInt aError) = 0;
	};



/**
*  Used by third party developers to play sampled audio data.
The class offers a simple interface to open, play and obtain
information from, sampled audio data. The audio data can be supplied
either in a file (file-based), as a descriptor
(descriptor-based) or as a URL reference.
*
*  @lib DRMAudioPlayer.lib
*  @since Series 60 3.0
*/
class CDrmPlayerUtility : public CBase
	{
	public:

        /**
        * Two-phased constructor.
        */
	IMPORT_C static  CDrmPlayerUtility* NewL(MDrmAudioPlayerCallback& aCallback,TInt aPriority,
															  TMdaPriorityPreference aPref);

        /**
        * Two-phased constructor.
        */
	IMPORT_C static  CDrmPlayerUtility* NewFilePlayerL(const TDesC& aFileName,MDrmAudioPlayerCallback& aCallback,
																		TInt aPriority,
																		TMdaPriorityPreference aPref);

        /**
        * Two-phased constructor.
        */
	IMPORT_C static  CDrmPlayerUtility* NewDesPlayerL(const TDesC8& aData,
													MDrmAudioPlayerCallback& aCallback,
													TInt aPriority,
													TMdaPriorityPreference aPref
													);

        /**
        * Two-phased constructor.
        */
	IMPORT_C static  CDrmPlayerUtility* NewDesPlayerReadOnlyL(const TDesC8& aData,
													MDrmAudioPlayerCallback& aCallback,
													TInt aPriority,
													TMdaPriorityPreference aPref
													);

        /**
        * Destructor.
        */
	IMPORT_C ~CDrmPlayerUtility();

        /**
		* Begins playback of the initialised audio sample at the current volume and priority levels.
        *
        * @since Series 60 3.0
        */
	IMPORT_C void Play();

        /**
		* Stops playback of the audio sample as soon as is possible.
		*
        * @since Series 60 3.0
        */
	IMPORT_C void Stop();

        /**
		* Pauses playback of the audio clip
		*
        * @since Series 60 3.0
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt Pause();

        /**
		* Changes the current playback volume to a specified value.
		*
        * @since Series 60 3.0
        * @param aVolume Any value between 0 (mute) and the maximum volume
        */
	IMPORT_C void SetVolume(TInt aVolume
							);

        /**
		* Sets the number of times the audio sample should be repeated during the playback operation
		*
        * @since Series 60 3.0

        * @param aRepeatNumberOfTimes
		The number of times to repeat the sample. Use 0 for no repeat,
		KMdaRepeatForever for continuous, any other value for number of times.
        * @param aTrailingSilence
		 The duration of silence after the sample has played in microseconds.
        */
	IMPORT_C void SetRepeats(TInt aRepeatNumberOfTimes,
							const TTimeIntervalMicroSeconds& aTrailingSilence
							);

        /**
		* Changes the current playback volume to a specified value.
		*
        * @since Series 60 3.0
        * @param aVolume Any value between 0 (mute) and the maximum volume returned by
        */
	IMPORT_C void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);


        /**
		* Returns the duration of the audio sample in microseconds.
		*
        * @since Series 60 3.0
		* @return The duration of the sample in microseconds
        */
	IMPORT_C const TTimeIntervalMicroSeconds& Duration();

        /**
		* Returns an integer representing the maximum volume that the audio clip can support.
		*
        * @since Series 60 3.0
		* @return The maximum permissible volume setting
        */
	IMPORT_C TInt MaxVolume();

        /**
		* Opens an audio clip from a file.
		*
        * @since Series 60 3.0
		* @param aFileName The file containing the audio clip
        */
	IMPORT_C void OpenFileL(const TDesC& aFileName);

        /**
		* Opens an audio clip from a file.
		*
        * @since Series 60 3.0
		* @param aFile The file handle of the file containing the clip
        */
	IMPORT_C void OpenFileL(const RFile& aFile);

        /**
		* Opens an audio clip from a file.
		*
        * @since Series 60 3.0
		* @param aSource The source of the file containing the clip
        */
	IMPORT_C void OpenFileL(const TMMSource& aSource);

        /**
		* Opens an audio clip from a descriptor.
		*
        * @since Series 60 3.0
		* @param aDescriptor The descriptor containing the audio clip
        */
	IMPORT_C void OpenDesL(const TDesC8& aDescriptor);

        /**
		* Opens an audio clip from a URL.
		*
        * @since Series 60 3.0
		* @param aUrl The URL of the audio clip.
		* @param aMimeType The mime type associated with the specified URL/audio clip
        */
	IMPORT_C void OpenUrlL(const TDesC& aUrl, TInt aIapId = KUseDefaultIap, const TDesC8& aMimeType=KNullDesC8);

        /**
		* Closes the current audio clip.
		*
        * @since Series 60 3.0
        */
	IMPORT_C void Close();

        /**
		* Returns the current playback position in microseconds from the start of the clip.
		*
        * @since Series 60 3.0
		* @param aPosition The number of microseconds from the start of the clip to the current play position
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt GetPosition(TTimeIntervalMicroSeconds& aPosition);

        /**
		* Returns the current playback position in microseconds from the start of the clip.
		*
        * @since Series 60 3.0
		* @param aPosition The number of microseconds from the start of the clip to the current play position
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C void SetPosition(const TTimeIntervalMicroSeconds& aPosition);

        /**
		* Set the priority for playback.
		*
        * @since Series 60 3.0
		* @param aPriority
			The priority level to apply, EMdaPriorityMin client can be interrupted by any other client,
			EMdaPriorityNormal client can only be interrupted by a client with a higher priority or
			EMdaPriorityMax client cannot be interrupted by other clients.
		* @param aPref The time and quality preferences to apply.
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt SetPriority(TInt aPriority, TMdaPriorityPreference aPref);

        /**
		* Returns the current playback volume.
		*
        * @since Series 60 3.0
		* @param aVolume A value between 0 (mute) and the maximum volume setting
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt GetVolume(TInt& aVolume);

        /**
		* Gets the number of meta data entries in the current audio clip.
		*
        * @since Series 60 3.0
		* @param aNumEntries On return, contains the number of meta data entries
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);

       /**
		* Returns a specified meta data entry from the audio clip.
		*
        * @since Series 60 3.0
		* @param aMetaDataIndex The index of the meta data entry to return
		* @return The returned meta data entry
        */
	IMPORT_C CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);

       /**
		* Defines the size of the current playback window.
		*
        * @since Series 60 3.0
		* @param aStart
			The position defining the start of the window, measured in microseconds.
			If this value is less than zero, it is set to zero. If this value is
			greater than aEnd, then it is swapped with aEnd.
		* @param aEnd
			The position defining the end of the window, measured in microseconds.
			If this value is greater than the value returned by Duration(), it is
			set to the value of Duration(). If this value is less than aStart, then
			it is swapped with aStart.
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
								const TTimeIntervalMicroSeconds& aEnd);

       /**
		* Clears the current playback window.
		*
        * @since Series 60 3.0
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt ClearPlayWindow();

       /**
		* Sets the current playback balance.
		*
        * @since Series 60 3.0
        * @param aBalance The balance value to set
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt SetBalance(TInt aBalance = KMMFBalanceCenter);

       /**
		* Returns the current playback balance.
		*
        * @since Series 60 3.0
        * @param aBalance On return, contains the current balance setting
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt GetBalance(TInt& aBalance);

       /**
		* Returns the current bit rate
		*
        * @since Series 60 3.0
        * @param aBitRate On return, contains the current bit rate
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt GetBitRate(TUint& aBitRate);

       /**
		* Allows user to register for audio loading callback
		*
        * @since Series 60 3.0
        * @param aCallback Reference to the client to be notified
        */
	IMPORT_C void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback);

       /**
		* Gets the progress of audio loading
		*
        * @since Series 60 3.0
        * @param aPercentageProgress Upon return, contains the percentage progress
        */
	IMPORT_C void GetAudioLoadingProgressL(TInt& aPercentageProgress);

       /**
		* Returns controller implementation parameters
		*
        * @since Series 60 3.0
        * @return The controller information
        */
	IMPORT_C const CMMFControllerImplementationInformation& ControllerImplementationInformationL();

       /**
		* Sends a custom command synchronously to the controller plugin
		*
        * @since Series 60 3.0
        * @param aDestination
			The destination of the custom command, consisting of the unique ID of the
			interface of this custom command and a special handle id KMMFObjectHandleController
			to indicate that the custom command is to be handled by the controller plugin
        * @param aFunction The function number to indicate which function is to be called
			on the controller's custom command interface.
        * @param aDataTo1 A reference to data to be copied to the controller plugin. The exact
			contents of the data are dependent on the custom command interface of the controller. Can be NULL.
        * @param aDataTo2
			A reference to data to be copied to the controller plugin. The exact contents of the data
			are dependent on the custom command interface of the controller. Can be NULL
        * @param aDataFrom
			A reference to an area of memory to which the controller plugin will write any data
			to be passed back to the client. Cannot be NULL
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);

      /**
		* Sends a custom command synchronously to the controller plugin
		*
        * @since Series 60 3.0
        * @param aDestination
			The destination of the custom command, consisting of the unique ID of the
			interface of this custom command and a special handle id KMMFObjectHandleController
			to indicate that the custom command is to be handled by the controller plugin
        * @param aFunction The function number to indicate which function is to be called
			on the controller's custom command interface.
        * @param aDataTo1 A reference to data to be copied to the controller plugin. The exact
			contents of the data are dependent on the custom command interface of the controller. Can be NULL.
        * @param aDataTo2
			A reference to data to be copied to the controller plugin. The exact contents of the data
			are dependent on the custom command interface of the controller. Can be NULL
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);

      /**
		* Sends a custom command asynchronously to the controller plugin
		*
        * @since Series 60 3.0
        * @param aDestination
			The destination of the custom command, consisting of the unique ID of the
			interface of this custom command and a special handle id KMMFObjectHandleController
			to indicate that the custom command is to be handled by the controller plugin
        * @param aFunction The function number to indicate which function is to be called
			on the controller's custom command interface.
        * @param aDataTo1 A reference to data to be copied to the controller plugin. The exact
			contents of the data are dependent on the custom command interface of the controller. Can be NULL.
        * @param aDataTo2
			A reference to data to be copied to the controller plugin. The exact contents of the data
			are dependent on the custom command interface of the controller. Can be NULL
        * @param aDataFrom
			A reference to an area of memory to which the controller plugin will write any data
			to be passed back to the client. Cannot be NULL
        * @param aDataFrom
			The TRequestStatus of an active object. This will contain the result of the custom
			command on completion. The exact range of result values is dependent on the custom
			command interface
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);

      /**
		* Sends a custom command asynchronously to the controller plugin
		*
        * @since Series 60 3.0
        * @param aDestination
			The destination of the custom command, consisting of the unique ID of the
			interface of this custom command and a special handle id KMMFObjectHandleController
			to indicate that the custom command is to be handled by the controller plugin
        * @param aFunction The function number to indicate which function is to be called
			on the controller's custom command interface.
        * @param aDataTo1 A reference to data to be copied to the controller plugin. The exact
			contents of the data are dependent on the custom command interface of the controller. Can be NULL.
        * @param aDataTo2
			A reference to data to be copied to the controller plugin. The exact contents of the data
			are dependent on the custom command interface of the controller. Can be NULL
        * @param aDataFrom
			A reference to an area of memory to which the controller plugin will write any data
			to be passed back to the client. Cannot be NULL
		* @return KErrNone on success, otherwise another of the system-wide error codes
        */
	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);

protected:
	/**
	This member is internal and not intended for use.
	*/
	CDrmAudioPlayerAdaptation* iProperties;

	};

#endif
