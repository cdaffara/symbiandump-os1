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

#ifndef MDAAUDIOSAMPLEEDITOR_H
#define MDAAUDIOSAMPLEEDITOR_H



#include <e32std.h>
#include <f32file.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfaudio.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfdrmcustomcommands.h>

#include <mda/client/resource.h>
#include <mda/client/utility.h>
#include <mda/common/audio.h>
#include <mmfclntutility.h>
#include <mmf/common/mmfdurationinfocustomcommands.h>

class CMdaServer;

/**
 * @publishedAll
 * @deprecated
 *
 * Dummy class - not supported in 7.0s
 */
class CMdaAudioType : public CBase
	{
public:
	/**
    Destructor
	*/
	IMPORT_C ~CMdaAudioType();
	};

/**
@publishedAll
@released

This class is used by both the audio recorder and audio converter. This class should not be used,
other than the TState enum.

Prior to 7.0s the pure virtual functions in this class were implemented virtual functions. In 7.0s 
this changed, so now any classes derived from this one must implement the functions themselves.

@see CMdaAudioRecorderUtility
@see CMdaAudioConvertUtility
*/
class CMdaAudioClipUtility : public CBase, 
							 public MMMFClientUtility
	{
public:
	/**
	Defines the possible state of the audio data sample.
	*/
	enum TState
		{
		/** The audio clip recorder or audio converter has been constructed but no file or descriptor 
		has been opened.
		*/
		ENotReady = 0,
		/** Files or descriptors are open but no playing or recording operation is in progress.
		*/
		EOpen,
		/** Audio sample data is playing.
		*/
		EPlaying,
		/** New audio sample data is being recorded.
		*/
		ERecording
		};

public:
	/**
	@internalTechnology

	Returns the current state of the audio sample data.

	@return The state of the audio sample data.

	@since 5.0
	*/
	virtual TState State()=0;

	/**
	@internalTechnology

	Releases any currently open audio sample data.

	@since 5.0
	*/
	virtual void Close()=0;

	/**
	@internalTechnology

	Begins playback of audio sample data at the current playback position using the current volume,
	gain and priority settings.

	@since 5.0
	*/
	virtual void PlayL()=0;

	/**
	@internalTechnology

	Starts appending new audio sample data.

	The new audio sample data is appended to the existing audio sample data in the same format as
	the existing audio sample data. If existing data is to be overwritten, then it should be cropped
	to the appropriate length before appending the new data.
	
	Note, before starting to record, make sure that the gain is set by calling
	CMdaAudioRecorderUtility::SetGain(), as the initial gain is undefined. The gain might also
	have been modified by another client application.
	
	
	@since 5.0
	@see CMdaAudioRecorderUtility 
	@see CMdaAudioRecorderUtility::SetGain()
	*/
	virtual void RecordL()=0;

	/**
	@internalTechnology
	
	Stops the playback or recording operation.
	
	The operation is stopped at the current location. For a playback operation, the head is positioned at the last played data.
	
	@since 5.0
	*/
	virtual void Stop()=0;

	/**
	@internalTechnology

	Deletes all audio sample data after the current head position. The effects of the function cannot be undone.

	The function is synchronous and can leave if there is a problem. The leave code depends on the configuration.

	@since 5.0
	*/
	virtual void CropL()=0;

	/**
	@internalTechnology
	
	Sets the head position.
	
	The playback head is moved to a position which is defined in terms of a time interval measured
	from the beginning of the audio sample data. A subsequent call to PlayL() starts from
	this new position.
	
	
	@since 5.0
	*/
	virtual void SetPosition(const TTimeIntervalMicroSeconds& aPosition)=0;

	/**
	@internalTechnology
	
	Returns the current head position.
	
	The head position is defined in terms of a time interval measured from the beginning of the audio sample data.
	
	@since 5.0
	*/
	virtual const TTimeIntervalMicroSeconds& Position()=0;

	/**
	@internalTechnology

	Returns the recording time available for the selected file or descriptor and encoding format.
	
	@since 5.0
	*/
	virtual const TTimeIntervalMicroSeconds& RecordTimeAvailable()=0;

	/**
	@internalTechnology

	Returns the duration of the audio sample data.
	
	@since 5.0
	*/
	virtual const TTimeIntervalMicroSeconds& Duration()=0;

	/**
	@internalTechnology
	
	Defines a window on the audio sample data.

	The window is defined in terms of a start and end position.
	When the current playback position reaches the window end position, or Stop() is called, the
	current playback position is set to the window start position and playback stops.

	The current playback position is not affected by a call to SetPlayWindow() unless it is outside
	the new playback window, in which case it is set to the window start or end position depending
	on which one is closer.

	The window persists until ClearPlayWindow() is called.
	Loading new audio sample data without adjusting or clearing the window will result in
	playback errors if the window is outside the new data.
	
	@param  aStart
	        The position defining the start of the window, measured in microseconds. If this
	        value is less than zero, it is set to zero. If this value is greater than aEnd,
	        then it is swapped with aEnd.
	@param  aEnd
	        The position defining the end of the window, measured in microseconds. If this value
	        is greater than the value returned by Duration(), it is set to the value of Duration().
	        If this value is less than aStart, then it is swapped with aStart.
	
	@since 5.0
	*/
	virtual void SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd)=0;

	/**
	@internalTechnology
	
	Removes any window defined on the audio sample data.
	
	All subsequent playing, position setting and position querying requests are mapped to the whole of the audio sample data.
	
	@since 5.0
	*/
	virtual void ClearPlayWindow()=0;

	/**
	@internalTechnology
	
	Sets the number of times the audio sample is to be repeated during the PlayL() operation.
	
	A period of silence can follow each playing of the audio sample. The audio sample can be repeated
	indefinitely or until stopped.

	@param  aRepeatNumberOfTimes  
            The number of times the audio sample, together with the trailing silence, is to 
            be repeated. If this is set to KMdaRepeatForever, then the audio sample, together with 
            the trailing silence, is repeated indefinitely. If this is set to zero, then the audio 
            sample is not repeated.
	@param  aTrailingSilence  
            The length of the trailing silence in microseconds.

	@since 5.0
	*/
	virtual void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)=0;

	/**
	@internalTechnology
	
	This function is no longer supported. It is provided for binary compatibility with previous
	releases and always leaves with KErrNotSupported.

	@return Always returns NULL.

	@since 5.0
	*/
 	virtual CMdaAudioType* TypeL();

	/**
	@internalTechnology
	
	Sets the maximum size for a file that is being recorded.
	
	When this limit is reached, MMF stops recording and notifies the client application. Notification is caused
	by MMdaObjectStateChangeObserver::MoscoStateChangeEvent() with the error code KErrEof.
	
	This function is provided so that applications such as recorders can limit the amount of file storage/memory
	that should be allocated.
	
	@param aMaxWriteLength
	       The maximum file size in kilobytes. If the default value is used, there is no maximum file size.

	@since 5.0
	*/
	virtual void SetMaxWriteLength(TInt aMaxWriteLength = KMdaClipLocationMaxWriteLengthNone)=0;

	/**
	@internalTechnology
	
	Deletes all audio sample data from the beginning of the audio clip. The effects of the function cannot be undone.
	
	The function is synchronous and can leave if there is a problem. The leave code depends on the configuration.
	
	@since 5.0
	*/
	virtual void CropFromBeginningL()=0;
public:
	// Reserved functions - do not call - to allow virtual additional functions to be added to class

	/**
	@internalTechnology

	Reserved function
    */
	virtual void ClipUtil_Reserved3();

	/**
	@internalTechnology
	
	Reserved function
	*/
	virtual void ClipUtil_Reserved4();
	};


class CMMFMdaAudioRecorderUtility;

/**
@publishedAll
@released

Plays back, records and edits audio sample data.

The class offers an interface to play, record and edit audio sample data. This data can be supplied 
either in a file or as a descriptor. The functions that start and stop playback and recording are 
defined in the base class CMdaAudioClipUtility.

While this class is abstract, the static NewL() function constructs, initialises and returns a 
pointer to an instance of a concrete class derived from this abstract class. This concrete class 
is part of the MMF implementation and is private.

*/
class CMdaAudioRecorderUtility : public CMdaAudioClipUtility
	{
public:
	
	/**
	Device mode. Used by SetAudioDeviceMode method which is not used from 7.0s
	@since 5.0
	*/
	enum TDeviceMode
		{
		/**
		The default behaviour.

		When recording:
		If a telephony call is in progress, an attempt is made to connect to the telephony downlink
		plus the device microphone.	If there is no telephony call in progress or a connection to the
		telephony downlink fails, an attempt is made to connect to the device microphone only.

		When playing:
		If a telephony call is in progress, an attempt is made to connect to the telephony uplink.
		If there is no telephony call in progress or a connection to the telephony uplink fails, an
		attempt is made to connect to the device speaker.
        */
		EDefault = 0,
		/** See EDefault.
		*/
		ETelephonyOrLocal = EDefault,
		/** 
		When recording:
		If a telephony call is in progress the audio source is the telephony downlink mixed with
		the device microphone. If there is no telephony call in progress or a connection to the telephony
		downlink fails, no attempt is made to connect to the microphone.

		When playing:
		If a telephony call is in progress the audio sample data is played to the telephony uplink and
		the device speaker. If there is no telephony call in progress or a connection to the telephony
		uplink fails, no attempt is made to connect to the device speaker.
		*/
		ETelephonyMixed = 1,
		/**
		When recording, the audio source is the telephony downlink and is not mixed with the device microphone.

		When playing, the audio sample data is played to the telephony uplink and is not mixed with the 
		device speaker.
		*/
		ETelephonyNonMixed = 2,
		/**
		When recording, the audio source is the device microphone.

		When playing, the audio sample data is played to the device speaker.
		*/
		ELocal = 3
		};

	virtual ~CMdaAudioRecorderUtility();

	IMPORT_C static CMdaAudioRecorderUtility* NewL(
		MMdaObjectStateChangeObserver& aObserver, 
		CMdaServer* aServer = NULL, 
		TInt aPriority = EMdaPriorityNormal, 
		TInt aPref = EMdaPriorityPreferenceTimeAndQuality);

	virtual void OpenFileL(const TDesC& aFileName);
	IMPORT_C void OpenFileL(const RFile& aFile);

	IMPORT_C void OpenFileL(const TMMSource& aSource);

	virtual void OpenDesL(const TDesC8& aDescriptor);

	IMPORT_C void OpenDesL(TDes8& aDescriptor);

	virtual void OpenL(TMdaClipLocation* aLocation,
						TMdaClipFormat* aFormat,		
						TMdaPackage* aArg1 = NULL,	
						TMdaPackage* aArg2 = NULL);

	virtual void SetAudioDeviceMode(TDeviceMode aMode);

	virtual TInt MaxVolume();

	virtual TInt MaxGain();

	virtual TInt SetVolume(TInt aVolume);

	virtual void SetGain(TInt aGain);

 	virtual void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);

	IMPORT_C void OpenFileL(const TDesC& aFileName,
						TUid aRecordControllerUid,				// controller to use
						TUid aPlaybackControllerUid=KNullUid,	// playback controller (if different)
						TUid aDestinationFormatUid=KNullUid,	// Data format
						TFourCC aDestinationDataType = KFourCCNULL);	// data type for recording

	IMPORT_C void OpenFileL(const RFile& aFile,
						TUid aRecordControllerUid,				// controller to use
						TUid aPlaybackControllerUid=KNullUid,	// playback controller (if different)
						TUid aDestinationFormatUid=KNullUid,	// Data format
						TFourCC aDestinationDataType = KFourCCNULL);	// data type for recording

	IMPORT_C void OpenFileL(const TMMSource& aSource,
						TUid aRecordControllerUid,				// controller to use
						TUid aPlaybackControllerUid=KNullUid,	// playback controller (if different)
						TUid aDestinationFormatUid=KNullUid,	// Data format
						TFourCC aDestinationDataType = KFourCCNULL);	// data type for recording

	IMPORT_C void OpenDesL(TDes8& aDescriptor,
						TUid aRecordControllerUid,				// controller to use
						TUid aPlaybackControllerUid=KNullUid,	// playback controller (if different)
						TUid aDestinationFormatUid=KNullUid,	// Data format
						TFourCC aDestinationDataType = KFourCCNULL);	// data type for recording

	IMPORT_C void OpenUrlL(const TDesC& aUrl,
						TInt aIapId,						// internet access point
						TUid aRecordControllerUid,				// controller to use
						TUid aPlaybackControllerUid=KNullUid,	// playback controller (if different)
						TUid aDestinationFormatUid=KNullUid,	// Data format
						TFourCC aDestinationDataType = KFourCCNULL);	// data type for recording

	IMPORT_C void OpenUrlL(const TDesC& aUrl,
							TInt aIapId =KUseDefaultIap, 
							const TDesC8& aMimeType = KNullDesC8);

	IMPORT_C TInt GetGain(TInt& aGain);

	IMPORT_C TInt GetVolume(TInt& aVolume);

	IMPORT_C TInt SetPlaybackBalance(TInt aBalance = KMMFBalanceCenter);

	IMPORT_C TInt GetPlaybackBalance(TInt& aBalance);

	IMPORT_C TInt SetRecordBalance(TInt aBalance = KMMFBalanceCenter);

	IMPORT_C TInt GetRecordBalance(TInt& aBalance);

	IMPORT_C TInt GetNumberOfMetaDataEntries(TInt& aNumEntries);

	IMPORT_C CMMFMetaDataEntry* GetMetaDataEntryL(TInt aMetaDataIndex);

	IMPORT_C void AddMetaDataEntryL(CMMFMetaDataEntry& aMetaDataEntry);

	IMPORT_C TInt RemoveMetaDataEntry(TInt aMetaDataIndex);

	IMPORT_C void ReplaceMetaDataEntryL(TInt aMetaDataIndex, CMMFMetaDataEntry& aMetaDataEntry);

	IMPORT_C void SetPriority(TInt aPriority, TInt aPref);

	//DataType support

	IMPORT_C void GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDataTypes);

	IMPORT_C void SetDestinationDataTypeL(TFourCC aDataType);

	IMPORT_C TFourCC DestinationDataTypeL();

	// Bit rate support

	IMPORT_C void SetDestinationBitRateL(TUint aBitRate);

	IMPORT_C TUint DestinationBitRateL();

	IMPORT_C TUint SourceBitRateL();

	IMPORT_C void GetSupportedBitRatesL(RArray<TUint>& aSupportedBitRates);
	
	// Sample rate support

	IMPORT_C void SetDestinationSampleRateL(TUint aSampleRate);

	IMPORT_C TUint DestinationSampleRateL();

	IMPORT_C void GetSupportedSampleRatesL(RArray<TUint>& aSupportedSampleRates);
	// Format support

	IMPORT_C void SetDestinationFormatL(TUid aFormatUid);
	
	IMPORT_C TUid DestinationFormatL();

	// Number of channels
	IMPORT_C void SetDestinationNumberOfChannelsL(TUint aNumberOfChannels);

	IMPORT_C TUint DestinationNumberOfChannelsL();

	IMPORT_C void GetSupportedNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels);

	virtual TState State();

	virtual void Close();

	virtual void PlayL();

	virtual void RecordL();

	virtual void Stop();

	virtual void CropL();

	virtual void SetPosition(const TTimeIntervalMicroSeconds& aPosition);

	virtual const TTimeIntervalMicroSeconds& Position();

	virtual const TTimeIntervalMicroSeconds& RecordTimeAvailable();

	virtual const TTimeIntervalMicroSeconds& Duration();
	
	IMPORT_C TMMFDurationInfo Duration(TTimeIntervalMicroSeconds& aDuration);

	virtual void SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);

	virtual void ClearPlayWindow();

	virtual void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);

	virtual void SetMaxWriteLength(TInt aMaxWriteLength = KMdaClipLocationMaxWriteLengthNone);

	virtual void CropFromBeginningL();

	IMPORT_C void RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback);

	IMPORT_C void GetAudioLoadingProgressL(TInt& aPercentageComplete);

	IMPORT_C const CMMFControllerImplementationInformation& AudioPlayControllerImplementationInformationL();

	IMPORT_C const CMMFControllerImplementationInformation& AudioRecorderControllerImplementationInformationL();

	IMPORT_C TInt RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);

	IMPORT_C TInt RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);

	IMPORT_C void RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);

	IMPORT_C void RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);

	IMPORT_C TInt PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);
	
	IMPORT_C TInt PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);

	IMPORT_C void PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);

	IMPORT_C void PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);

	IMPORT_C MMMFDRMCustomCommand* GetDRMCustomCommand();

	IMPORT_C TInt RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData = KNullDesC8);

	IMPORT_C TInt CancelRegisterAudioResourceNotification(TUid aNotificationEventId);
    
 	IMPORT_C TInt WillResumePlay();
 	
 	IMPORT_C TInt SetThreadPriorityPlayback(const TThreadPriority& aThreadPriority) const;
 	IMPORT_C TInt SetThreadPriorityRecord(const TThreadPriority& aThreadPriority) const;
	
	IMPORT_C void UseSharedHeap();
private:
	CMMFMdaAudioRecorderUtility* iProperties;
	};

class CMMFMdaAudioConvertUtility;

/**
@publishedAll
@released

Converts the type, sample rate and format of audio data.

The audio sample data can be supplied either in a file or as a descriptor.
*/
class CMdaAudioConvertUtility : public CMdaAudioClipUtility
	{
public:

	IMPORT_C static CMdaAudioConvertUtility* NewL(
		MMdaObjectStateChangeObserver& aObserver, 
		CMdaServer* aServer = NULL, 
		TInt aPriority = EMdaPriorityNormal, 
		TInt aPref = EMdaPriorityPreferenceTimeAndQuality);

	virtual ~CMdaAudioConvertUtility();

	virtual void OpenL(const TDesC& aPrimaryFile, const TDesC& aSecondaryFile);

	virtual void OpenL(const TDesC& aPrimaryFile,
						TMdaClipLocation* aLocation,	// Normally file or descriptor
						TMdaClipFormat* aFormat,		// Data format
						TMdaPackage* aArg1 = NULL,		// Normally codec to use
						TMdaPackage* aArg2 = NULL);		// Normally audio settings

	virtual void OpenL(TMdaClipLocation* aPriLocation, 
						TMdaClipLocation* aSecLocation, 
						TMdaClipFormat* aPriFormat, 
						TMdaClipFormat* aSecFormat, 
						TMdaPackage* aPriArg1 = NULL, 
						TMdaPackage* aPriArg2 = NULL, 
						TMdaPackage* aSecArg1 = NULL, 
						TMdaPackage* aSecArg2 = NULL);

	// new controller based open
	// N.B. this override is new and must NOT be made virtual. This is because
	// it will break BC on the WINS build (due to the MS compiler grouping and
	// reordering the overrides together in the vtable and _not_ using the 
	// declaration order!)

	IMPORT_C void OpenL(TMdaClipLocation* aPriLocation, 
						TMdaClipLocation* aSecLocation, 
						TUid aControllerUid, 
						TUid aDestFormatUid,
						TFourCC aDestDataType);

	//Data type support

	IMPORT_C void GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDestinationDataTypes);

	IMPORT_C void SetDestinationDataTypeL(TFourCC aDataType);

	IMPORT_C TFourCC DestinationDataTypeL();

	IMPORT_C TFourCC SourceDataTypeL();
	// Bit rate support	

	IMPORT_C void SetDestinationBitRateL(TUint aBitRate);

	IMPORT_C TUint DestinationBitRateL();

	IMPORT_C void GetSupportedConversionBitRatesL(RArray<TUint>& aSupportedBitRates);

	IMPORT_C TInt SourceBitRateL();

	// Sample rate support
	IMPORT_C void SetDestinationSampleRateL(TUint aSampleRate);

	IMPORT_C TUint DestinationSampleRateL();

	IMPORT_C void GetSupportedConversionSampleRatesL(RArray<TUint>& aSupportedSampleRates);

	IMPORT_C TUint SourceSampleRateL();

	// Format support
	IMPORT_C void SetDestinationFormatL(TUid aFormatUid);

	IMPORT_C TUid DestinationFormatL();

	IMPORT_C TUid SourceFormatL();

	// Number of channels
	IMPORT_C void SetDestinationNumberOfChannelsL(TUint aNumberOfChannels);

	IMPORT_C TUint DestinationNumberOfChannelsL();

	IMPORT_C void GetSupportedConversionNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels);

	IMPORT_C TUint SourceNumberOfChannelsL();

	// New function ConvertL (PlayL and RecordL now deprecated for CMdaAudioConvertUtility)
	IMPORT_C void ConvertL();

	virtual TState State();

	virtual void Close();

	virtual void PlayL();

	virtual void RecordL();

	virtual void Stop();

	virtual void CropL();

	virtual void SetPosition(const TTimeIntervalMicroSeconds& aPosition);

	virtual const TTimeIntervalMicroSeconds& Position();

	virtual const TTimeIntervalMicroSeconds& RecordTimeAvailable();

	virtual const TTimeIntervalMicroSeconds& Duration();

	virtual void SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd);

	virtual void ClearPlayWindow();

	virtual void SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence);

	virtual void SetMaxWriteLength(TInt aMaxWriteLength = KMdaClipLocationMaxWriteLengthNone);

	virtual void CropFromBeginningL();

	IMPORT_C const CMMFControllerImplementationInformation& ControllerImplementationInformationL();

	IMPORT_C TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom);

	IMPORT_C TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2);

	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus);

	IMPORT_C void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus);
	
 	IMPORT_C TInt SetThreadPriority(const TThreadPriority& aThreadPriority) const;
	
	IMPORT_C void UseSharedHeap(); 		
private:
	CMMFMdaAudioConvertUtility* iProperties;
	
	};

#endif
