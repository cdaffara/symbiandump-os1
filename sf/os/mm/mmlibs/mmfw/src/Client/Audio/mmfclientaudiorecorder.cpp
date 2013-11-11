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

#include <badesca.h>
#include <utf.h>

#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfpaniccodes.h>
#include <mmfformatimplementationuids.hrh>

#include "mmfclientutility.h"
#include "mmfclientaudiorecorder.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfdurationinfocustomcommandsimpl.h>
#include <mmf/common/mmfdurationinfocustomcommandsenums.h>
#endif

void Panic(TInt aPanicCode)
	{
	_LIT(KMMFMediaClientAudioPanicCategory, "MMFAudioClient");
	User::Panic(KMMFMediaClientAudioPanicCategory, aPanicCode);
	}

EXPORT_C CMdaAudioType::~CMdaAudioType()
	{
	}

/**
Destructor. Closes the audio clip and frees resources.

@since 5.0
*/
CMdaAudioRecorderUtility::~CMdaAudioRecorderUtility()
	{	  
	delete iProperties;
	}

/**
Ensures that any subsequent calls to OpenXYZ() will create controllers that
share a heap.

The default behaviour is that for each recorder utility a controller with its own heap
is created. Each heap uses a chunk, so using this function avoids situations where 
the number of chunks per process is limited.
The default behaviour is generally to be preferred, and should give lower overall
memory usage. However, if many controllers are to be created for a particular thread,
then this function should be used to prevent running out of heaps or chunks.

@since	9.2
*/
EXPORT_C void CMdaAudioRecorderUtility::UseSharedHeap()
	{
	ASSERT(iProperties);
	iProperties->UseSharedHeap();
	}

/**
Constructs and initialises a new instance of the audio recorder for playing, recording and editing audio sample data.

The function leaves if the audio recorder object cannot be created.

@param  aObserver
        Class to receive state change events from recorder
@param  aServer
        No longer used, should be NULL
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.

@return A pointer to the new audio recorder object
@since 5.0

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMdaAudioRecorderUtility* CMdaAudioRecorderUtility::NewL(MMdaObjectStateChangeObserver& aObserver,
																  CMdaServer* /*aServer*/,
																  TInt aPriority,
																  TInt aPref)
	{
	CMdaAudioRecorderUtility* self = new(ELeave) CMdaAudioRecorderUtility();
	CleanupStack::PushL(self);
	self->iProperties = new(ELeave) CMMFMdaAudioRecorderUtility(self, aObserver);
	self->iProperties->ConstructL(aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}

/**
Opens a file to play back or record audio sample data.

@param  aFileName
        The full path and filename of the file which is a source of existing audio sample data.

@since 5.0
*/
void CMdaAudioRecorderUtility::OpenFileL(const TDesC& aFileName)
	{
	ASSERT(iProperties);
	iProperties->OpenFileL(aFileName);
	}
	
/**
Opens a file to play back or record audio sample data.

@param  aFile
        The full path and filename of the file which is a source of existing audio sample data.

*/
EXPORT_C void CMdaAudioRecorderUtility::OpenFileL(const RFile& aFile)
	{
	ASSERT(iProperties);
	RFile& file = const_cast<RFile&>(aFile);
	iProperties->OpenFileL(file);
	}

/**
Opens a file to play or record audio sample data.

@param  aFileSource
        The full path and filename of the file or an open file handle
        which is a source of existing audio sample data.
*/
EXPORT_C void CMdaAudioRecorderUtility::OpenFileL(const TMMSource& aSource)
	{
	ASSERT(iProperties);
	iProperties->OpenFileL(aSource);
	}

/**
Opens a descriptor to play back or record audio sample data.

@param  aDescriptor
        The descriptor which is a source of existing audio sample data. This descriptor must
        remain in existence for the lifetime of this audio clip recorder object.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::OpenDesL(TDes8& aDescriptor)
	{
	ASSERT(iProperties);
	iProperties->OpenDesL(aDescriptor);
	}

/**
Opens a descriptor to play back or record audio sample data (const version).

@param  aDescriptor
        The descriptor which is a source of existing audio sample data. This descriptor must
        remain in existence for the lifetime of this audio clip recorder object.

@since 5.0
*/
void CMdaAudioRecorderUtility::OpenDesL( const TDesC8& aDescriptor)
	{
	ASSERT(iProperties);
	iProperties->OpenDesL( aDescriptor);
	}

/**
Opens a TMdaClipLocation package of audio data that can be played back or recorded to.

@param  aLocation
        The location of a source of existing audio sample data or a target for new audio sample data.
        This is normally a file name (a TMdaFileClipLocation) or a descriptor (a TMdaDesClipLocation).
@param  aFormat
        The format of the audio clip.
@param  aArg1
        The codec/data type to use.
@param  aArg2
        The audio settings

@since  5.0
*/
void CMdaAudioRecorderUtility::OpenL(TMdaClipLocation* aLocation,		// Normally file or descriptor
									 TMdaClipFormat* aFormat,			// Data format
									 TMdaPackage* aArg1 /*= NULL*/,		// Normally codec to use
									 TMdaPackage* aArg2 /*= NULL*/)		// Normally audio settings
	{
	ASSERT(iProperties);
	iProperties->OpenL(aLocation, aFormat, aArg1, aArg2);
	}

/**
Sets the audio device mode. This function is provided for binary compatibility, but
does not have any effect from 7.0s.

@param  aMode
        One of the TDeviceMode settings

@since 5.0
*/
void CMdaAudioRecorderUtility::SetAudioDeviceMode(TDeviceMode aMode)
	{
	ASSERT(iProperties);
	iProperties->SetAudioDeviceMode(aMode);
	}

/**
Returns the maximum volume setting for the audio device.

@return The maximum volume. This value is platform dependent but is always greater than or equal to one.

@since 5.0
*/
TInt CMdaAudioRecorderUtility::MaxVolume()
	{
	ASSERT(iProperties);
	return iProperties->MaxVolume();
	}

/**
Return the maximum value for the gain

@return The maximum gain. For devices where automatic gain control is used, this value may be zero.

@since 5.0
*/
TInt CMdaAudioRecorderUtility::MaxGain()
	{
	ASSERT(iProperties);
	return iProperties->MaxGain();
	}

/**
Sets the playback volume for the audio device to a specified value.

@param  aVolume
        The volume setting. This can be any value from zero to the value returned by a 
        call to MaxVolume(). A value that is less than zero is set to zero. A value that 
        is greater than MaxVolume() is set to MaxVolume(). Setting a zero value mutes the 
        sound. Setting the maximum value results in the loudest possible sound. Values less 
        than zero would be set to zero and the values greater than the maximum permitted 
        volume would be set to the maximum volume.
        
@return An error code indicating if the call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.  KErrNotReady indicates that the clip has not 
        been sucessfully opened for play.
        
@since 5.0
*/
TInt CMdaAudioRecorderUtility::SetVolume(TInt aVolume)
	{
	ASSERT(iProperties);
	return iProperties->SetVolume(aVolume);
	}

/**
Sets the gain for the audio device to a specified value.

@param  aGain
        The gain setting. This can be any value from zero to the value returned by a call to
        MaxGain(). A value which is less than zero is set to zero. A value which is greater
        than MaxGain() is set to MaxGain().

@since 5.0
*/
void CMdaAudioRecorderUtility::SetGain(TInt aGain)
	{
	ASSERT(iProperties);
	iProperties->SetGain(aGain);
	}

/**
Sets the volume increase gradient.

This defines the period over which the volume level of a played audio sample is to rise smoothly 
from nothing to the normal volume level.

@param  aRampDuration
        The period over which the volume is to rise. A zero value causes the audio sample to be
        played at the normal level for the full duration of the playback. A value which is longer 
        than the duration of the audio sample means that the sample never reaches its normal 
        volume level.

@since  5.0
*/
void CMdaAudioRecorderUtility::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	ASSERT(iProperties);
	iProperties->SetVolumeRamp(aRampDuration);
	}

/**
Register to receive notifications of audio loading/rebuffering

@param  aCallback
        The object to receive audio loading notifications.

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback)
	{
	ASSERT(iProperties);
	iProperties->RegisterForAudioLoadingNotification(aCallback);
	}
	
/**
Returns the current progress of audio loading.

@param aPercentageProgress
       The percentage of the audio clip loaded.

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
	ASSERT(iProperties);
	iProperties->GetAudioLoadingProgressL(aPercentageProgress);
	}


/**
Opens an audio clip from a file to play back existing audio data or to record new audio data.

@param  aFile
        The full path and filename of the file that contains or will contain the audio data.
@param  aRecordControllerUid
        The UID of the controller to use for recording.
@param  aPlaybackControllerUid
        The UID of the controller to use for playback. If KNullUid is used the
        same controller is used for playback and recording.
@param  aFormatUid
        The UID of record audio format.
@param  aCodec
        Data type for recording

*/
EXPORT_C void CMdaAudioRecorderUtility::OpenFileL(const RFile& aFile,
												  TUid aRecordControllerUid,					// controller to use
												  TUid aPlaybackControllerUid /*=KNullUid*/,	// playback controller (if different)
												  TUid aFormatUid /*=KNullUid*/,				// Data format
												  TFourCC aCodec /*= NULL*/)					// codec to use	
	{
	ASSERT(iProperties);
	RFile& file = const_cast<RFile&>(aFile);
	iProperties->OpenFileL(file, aRecordControllerUid, aPlaybackControllerUid, aFormatUid, aCodec);
	}
	
/**
Opens an audio clip from a file to play back existing audio data or to record new audio data.

@param  aFileName
        The full path and filename of the file that contains or will contain the audio data.
@param  aRecordControllerUid
        The UID of the controller to use for recording.
@param  aPlaybackControllerUid
        The UID of the controller to use for playback. If KNullUid is used the
        same controller is used for playback and recording.
@param  aFormatUid
        The UID of record audio format.
@param  aCodec
        Data type for recording

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::OpenFileL(const TDesC& aFileName,
												  TUid aRecordControllerUid,					// controller to use
												  TUid aPlaybackControllerUid /*=KNullUid*/,	// playback controller (if different)
												  TUid aFormatUid /*=KNullUid*/,				// Data format
												  TFourCC aCodec /*= NULL*/)					// codec to use	
	{
	ASSERT(iProperties);
	iProperties->OpenFileL(aFileName, aRecordControllerUid, aPlaybackControllerUid, aFormatUid, aCodec);
	}

/**
Opens an audio clip from a filename or an open file handle
to play back existing audio data or to record new audio data.

@param  aFileSource
        The full path and filename of the file or a fiule handle that contains or will contain the audio data.
@param  aRecordControllerUid
        The UID of the controller to use for recording.
@param  aPlaybackControllerUid
        The UID of the controller to use for playback. If KNullUid is used the
        same controller is used for playback and recording.
@param  aFormatUid
        The UID of record audio format.
@param  aCodec
        Data type for recording
*/
EXPORT_C void CMdaAudioRecorderUtility::OpenFileL(const TMMSource& aFileSource,
												  TUid aRecordControllerUid,					// controller to use
												  TUid aPlaybackControllerUid /*=KNullUid*/,	// playback controller (if different)
												  TUid aFormatUid /*=KNullUid*/,				// Data format
												  TFourCC aCodec /*= NULL*/)					// codec to use	
	{
	ASSERT(iProperties);
	iProperties->OpenFileL(aFileSource, aRecordControllerUid, aPlaybackControllerUid, aFormatUid, aCodec);
	}

/**
Opens an audio clip from a descriptor to play back existing audio data from or to record new audio data to.

@param  aDescriptor
        The descriptor that contains or will contain the audio data.
@param  aControllerUid
        The UID of the controller to use for recording.
@param  aPlaybackControllerUid
        The UID of the controller to use for playback. If KNullUid is used then the
        same controller is used for playback and recording.
@param  aFormatUid
        The UID of the audio record format
@param  aCodec
        Data type for recording.

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::OpenDesL(TDes8& aDescriptor,
												 TUid aControllerUid,							// controller to use
												 TUid aPlaybackControllerUid /*=KNullUid*/,		// playback controller (if different)
												 TUid aFormatUid /*=KNullUid*/,					// Data format
												 TFourCC aCodec /*= NULL*/)						// codec to use	
	{
	ASSERT(iProperties);
	iProperties->OpenDesL(aDescriptor, aControllerUid, aPlaybackControllerUid, aFormatUid, aCodec);
	}

/**
Opens an audio clip from a URL to play back existing audio data from or to record new audio data to.

@param  aUrl
        The URL that contains or will contain the audio data.
@param  aIapId
        The ID of the internet access point to use. KUseDefaultIap selects the default Internet access point.
@param  aControllerUid
        The UID of the controller to use for recording.
@param  aPlaybackControllerUid
        The UID of the controller to use for playback. If KNullUid is used then the same controller
        is used for playback and recording.
@param  aFormatUid
        The UID of the audio record format.
@param  aCodec
        Data type for recording.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::OpenUrlL(const TDesC& aUrl,
												 TInt aIapId,
												 TUid aControllerUid,							// controller to use
												 TUid aPlaybackControllerUid /*=KNullUid*/,		// playback controller (if different)
												 TUid aFormatUid /*=KNullUid*/,					// Data format
												 TFourCC aCodec /*= NULL*/)						// codec to use
	{
	ASSERT(iProperties);
	iProperties->OpenUrlL(aUrl, aIapId, aControllerUid, aPlaybackControllerUid, aFormatUid, aCodec);
	}

/**
Opens an audio clip from a URL to play back or record audio sample data.

@param  aUrl
        The URL that contains or will contain the audio data.
@param  aIapId
        The ID of the internet access point to use. KUseDefaultIap selects the default Internet access point.
@param  aMimeType
        The MIME type of the data pointed to by aUrl.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::OpenUrlL(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType)
	{
	ASSERT(iProperties);
	iProperties->OpenUrlL(aUrl, aIapId, aMimeType);
	}

/**
Returns the current gain setting of the audio device.

@param  aGain
        On return contains the current gain setting for the audio device. Can be
        any value between zero and MaxGain().

@return An error code indicating if the call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::GetGain(TInt& aGain)
	{
	ASSERT(iProperties);
	return iProperties->GetGain(aGain);
	}

/**
Returns the current volume setting for the audio device.

@param  aVolume
        On return contains the current volume setting of the audio device. Can be any value between
        zero and MaxVolume().

@return An error code indicating if the call was successful. KErrNone on success, otherwise another
        of the system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::GetVolume(TInt& aVolume)
	{
	ASSERT(iProperties);
	return iProperties->GetVolume(aVolume);
	}

/**
Sets the current playback balance.

The balance can be any value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight, the default value being KMMFBalanceCenter.

@param  aBalance
        The playback balance to set.

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the
        system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::SetPlaybackBalance(TInt aBalance /*= KMMFBalanceCenter*/)
	{
	ASSERT(iProperties);
	return iProperties->SetPlaybackBalance(aBalance);
	}

/**
 *	Returns The current balance setting for the audio device. This function may not return the same value 
 *			as passed to SetPlaybackBalance depending on the internal implementation in 
 *			the underlying components.
 *
 *	@param  aBalance
 *       	The playback balance, between KMMFBalanceMaxLeft and KMMFBalanceMaxRight
 *
 *	@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the 
 *       	system-wide error codes.
 *
 *	@since 	7.0s
 */
EXPORT_C TInt CMdaAudioRecorderUtility::GetPlaybackBalance(TInt& aBalance)
	{
	ASSERT(iProperties);
	return iProperties->GetPlaybackBalance(aBalance);
	}

/**
Sets the current recording balance setting between the left and right stereo microphones

The microphone balance can be changed before or during recording and is effective immediately.

@param  aBalance
        The balance value to set. Any value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight, the default 
        value being KMMFBalanceCenter

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the
        system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::SetRecordBalance(TInt aBalance /*= KMMFBalanceCenter*/)
	{
	ASSERT(iProperties);
	return iProperties->SetRecordBalance(aBalance);
	}

/**
Returns the current recording balance setting between the left and right stereo microphones.

@param  aBalance
        On return, contains the current recording balance between KMMFBalanceMaxLeft and KMMFBalanceMaxRight

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the
        system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::GetRecordBalance(TInt& aBalance)
	{
	ASSERT(iProperties);
	return iProperties->GetRecordBalance(aBalance);
	}

/**
Returns the number of meta data entries associated with this clip.

@param  aNumEntries
        On return contains the number of meta data entries.

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the
        system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::GetNumberOfMetaDataEntries(TInt& aNumEntries)
	{
	ASSERT(iProperties);
	return iProperties->GetNumberOfMetaDataEntries(aNumEntries);
	}

/**
Returns the specified meta data entry from the current audio clip.

@param  aMetaDataIndex
        The index of the entry to retrieve. The index is between 0 and GetNumberOfMetaDataEntries()-1

@return The meta data entry requested

@since 7.0s
*/
EXPORT_C CMMFMetaDataEntry* CMdaAudioRecorderUtility::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
	ASSERT(iProperties);
	return iProperties->GetMetaDataEntryL(aMetaDataIndex);
	}

/**
Adds a meta data entry to the audio clip.

@param  aMetaDataEntry
        The new meta data entry to add to the clip.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::AddMetaDataEntryL(CMMFMetaDataEntry& aMetaDataEntry)
	{
	ASSERT(iProperties);
	iProperties->AddMetaDataEntryL(aMetaDataEntry);
	}

/**
Removes a specified meta data entry from the audio clip.

@param  aMetaDataIndex
        The index of the meta data entry to remove.

@return An error code indicating if the call was successful. KErrNone on success, otherwise another of the 
        system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::RemoveMetaDataEntry(TInt aMetaDataIndex)
	{
	ASSERT(iProperties);
	return iProperties->RemoveMetaDataEntry(aMetaDataIndex);
	}

/**
Replaces the specified meta data entry with a new entry.

@param  aMetaDataIndex
        The index of the meta data entry to replace.
@param  aMetaDataEntry
        The new meta data entry to replace the one pointed to by aMetaDataIndex.

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::ReplaceMetaDataEntryL(TInt aMetaDataIndex, CMMFMetaDataEntry& aMetaDataEntry)
	{
	ASSERT(iProperties);
	iProperties->ReplaceMetaDataEntryL(aMetaDataIndex, aMetaDataEntry);
	}

/**
Sets the recording priority. 

The priority is used to arbitrate between multiple objects simultaneously trying to access the sound hardware.

@param  aPriority
        The Priority Value.
@param  aPref
        The Priority Preference.

@since 7.0s

@see CMdaAudioRecorderUtility::NewL()

*/
EXPORT_C void CMdaAudioRecorderUtility::SetPriority(TInt aPriority, TInt aPref)
	{
	ASSERT(iProperties);
	iProperties->SetPriority(aPriority, aPref);
	}

/**
Returns a list of the supported codecs for the conversion destination.

@param  aSupportedDataTypes
        List of four character codes, representing supported data codecs for the conversion destination.

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDataTypes)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedDestinationDataTypesL(aSupportedDataTypes);
	}

/**
Sets the codec to use with the record controller.

@param  aCodec
        Four character code, representing the codec of the destination audio clip

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::SetDestinationDataTypeL(TFourCC aCodec)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationDataTypeL(aCodec);
	}

/**
Returns the codec that is currently set for the destination audio clip.

@return Four character code, representing the codec of the destination audio clip.

@since 7.0s
*/
EXPORT_C TFourCC CMdaAudioRecorderUtility::DestinationDataTypeL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationDataTypeL();
	}

/**
Sets the bit rate of the data sink in bits per second.

The bit rate must be one of the supported bit rates of the data sink. Use GetSupportedBitRatesL()
to retrieve a list of supported bit rates for the data sink.

@param  aBitRate
        Destination bit rate in bits/second.

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::SetDestinationBitRateL(TUint aBitRate)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationBitRateL(aBitRate);
	}

/**
Returns the bit rate in bits per second that the data sink is currently set to.

@return Bit rate of data sink in bits/second.

@since 7.0s
*/
EXPORT_C TUint CMdaAudioRecorderUtility::DestinationBitRateL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationBitRateL();
	}

/**
Returns a list of the supported bit rates for recording to the data sink.

@param  aSupportedBitRates
        List of bit rates supported for the data sink destination.

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::GetSupportedBitRatesL(RArray<TUint>& aSupportedBitRates)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedBitRatesL(aSupportedBitRates);
	}

/**
Sets the sample rate for the conversion destination data sink.

The sample rate must be one of the supported sample rates of the data sink. Use GetSupportedSampleRatesL()
to retrieve a list of supported sample rates for the data sink. This function should not be used if the
audio clip already exists; that is, in the "Open and Append" scenario, when the function's behaviour is
undefined.

@param  aSampleRate
        The sample rate of the conversion destination.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::SetDestinationSampleRateL(TUint aSampleRate)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationSampleRateL(aSampleRate);
	}

/**
Returns the sample rate in samples/second that the destination data sink is currently set to.

@return The sample rate of the conversion destination

@since 7.0s
*/
EXPORT_C TUint CMdaAudioRecorderUtility::DestinationSampleRateL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationSampleRateL();
	}

/**
If a destination sample rate has not been set then GetSupportedSampleRatesL() returns all valid sample rates

If a destination sample rate has been set then GetSupportedSampleRatesL() returns all valid sample rates
less than or equal to the destination sample rate

@param  aSupportedSampleRates
        On return, contains a list of the sample rates that are supported for recording.

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::GetSupportedSampleRatesL(RArray<TUint>& aSupportedSampleRates)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedSampleRatesL(aSupportedSampleRates);
	}

/**
Sets the format of the audio clip. This can only be done if the audio clip does not exist.

@param  aFormatUid
        UID of the audio clip format

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::SetDestinationFormatL(TUid aFormatUid)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationFormatL(aFormatUid);
	}

/**
Returns the format(UID) of the destination audio clip.

@return UID of the audio clip format.

@since 7.0s
*/
EXPORT_C TUid CMdaAudioRecorderUtility::DestinationFormatL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationFormatL();
	}

/**
Sets the number of channels for the recorded audio clip.

The number of channels must be one of the values returned by GetSupportedNumberOfChannelsL().

@param  aNumberOfChannels
        The number of channels to record

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::SetDestinationNumberOfChannelsL(TUint aNumberOfChannels)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationNumberOfChannelsL(aNumberOfChannels);
	}

/**
Returns the number of channels that the destination audio clip is currently configured to support.

@return The number of channels supported by audio clip

@since 7.0s
*/
EXPORT_C TUint CMdaAudioRecorderUtility::DestinationNumberOfChannelsL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationNumberOfChannelsL();
	}


/**
Returns a list of the supported number of channels for recording. For example 1 for mono, 2 for stereo and so on.

@param  aSupportedNumChannels
        On return, contains a list of the number of channels supported for recording

@since 7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::GetSupportedNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedNumberOfChannelsL(aSupportedNumChannels);
	}

/**
Gets a controller's DRM custom command implementation.

@return A pointer to a controller's DRM custom command implementation, or NULL if
the controller does not support it.
*/
EXPORT_C MMMFDRMCustomCommand* CMdaAudioRecorderUtility::GetDRMCustomCommand()
	{
	ASSERT(iProperties);
	return iProperties->GetDRMCustomCommand();
	}

/**
Returns the controller implementation information associated with the playback controller

@return The controller implementation structure associated with the controller

@since 7.0s
*/
EXPORT_C const CMMFControllerImplementationInformation& CMdaAudioRecorderUtility::AudioPlayControllerImplementationInformationL()
	{
	ASSERT(iProperties);
	return iProperties->AudioPlayControllerImplementationInformationL();
	}
	
/**
Returns the controller implementation information associated with the recording controller.

@return The controller implementation structure associated with the controller

@since 7.0s
*/
EXPORT_C const CMMFControllerImplementationInformation& CMdaAudioRecorderUtility::AudioRecorderControllerImplementationInformationL()
	{
	ASSERT(iProperties);
	return iProperties->AudioRecorderControllerImplementationInformationL();
	}

/**
Sends a synchronous custom command to the recording controller.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client.  Can't be KNullDesC8.

@return The result of the request. Exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	ASSERT(iProperties);
	return iProperties->RecordControllerCustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}

/**
Sends a synchronous custom command to the recording controller.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.

@return The result of the request.  Exact range of values is dependent on the interface.

@since  7.0s

*/
EXPORT_C TInt CMdaAudioRecorderUtility::RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	ASSERT(iProperties);
	return iProperties->RecordControllerCustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}

/**
Send a asynchronous custom command to the recording controller.

Note: This method will return immediately.  The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the
controller framework.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client.  Can't be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object.  This will contain the
        result of the request on completion.  The exact range of
        result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	ASSERT(iProperties);
	iProperties->RecordControllerCustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}

/**
Send a asynchronous custom command to the recording controller

Note: 
This method will return immediately.  The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the
controller framework.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object.  This will contain the
        result of the request on completion.  The exact range of
        result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	ASSERT(iProperties);
	iProperties->RecordControllerCustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

/**
Send a synchronous custom command to the playback controller

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client.  Can't be KNullDesC8.

@return The result of the request.  Exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	ASSERT(iProperties);
	return iProperties->PlayControllerCustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}

/**
Send a synchronous custom command to the playback controller.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.

@return The result of the request.  Exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioRecorderUtility::PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	ASSERT(iProperties);
	return iProperties->PlayControllerCustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}

/**
Send a asynchronous custom command to the recording controller.

Note: 
This method will return immediately.  The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the
controller framework.
@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client.  Can't be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object.  This will contain the
        result of the request on completion.  The exact range of
        result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	ASSERT(iProperties);
	iProperties->PlayControllerCustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
	
/**
Send a asynchronous custom command to the recording controller.

Note: 
This method will return immediately.  The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the
controller framework.

@param  aDestination
        The destination of the message, consisting of the uid of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called.  Can be KNullDesC8.
@param   aStatus
        The TRequestStatus of an active object.  This will contain the
        result of the request on completion.  The exact range of
        result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CMdaAudioRecorderUtility::PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	ASSERT(iProperties);
	iProperties->PlayControllerCustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

/**
Returns the bit rate of the audio clip.

@return The bit rate of the audio clip

@leave  KErrNotSupported
		Leaves with KErrNotSupported error, if no controllers are found

@since  7.0s
*/
EXPORT_C TUint CMdaAudioRecorderUtility::SourceBitRateL()
	{
	ASSERT(iProperties);
	return iProperties->SourceBitRateL();	
	}
	
/**
Registers the Event for Notification when resource is avaliable.

@param	aCallback
      	The audio outputstream observer interface..
      	
@param 	aNotificationEventUid
 	The Event for which the client is registered.
 	
@param 	aNotificationRegistrationData
	Notification registration specific data.
	
@return An error code indicating if the registration was successful. KErrNone on success, 
	otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CMdaAudioRecorderUtility::RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData)
	{
	ASSERT(iProperties);
	return iProperties->RegisterAudioResourceNotification(aCallback,aNotificationEventUid,aNotificationRegistrationData);
	}

/**
Cancels the registered notification event.

@param  aNotificationEventUid
	The Event to notify the client.
	
@return An error code indicating if the registration was successful. KErrNone on success, 
	otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CMdaAudioRecorderUtility::CancelRegisterAudioResourceNotification(TUid aNotificationEventUid)
	{
	ASSERT(iProperties);
	return iProperties->CancelRegisterAudioResourceNotification(aNotificationEventUid);
	}

/**
Waits for the client to resume the play even after the default timer expires.

@return An error code indicating if the registration was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CMdaAudioRecorderUtility::WillResumePlay()
	{
	ASSERT(iProperties);
	return iProperties->WillResumePlay();
	}

/**
Set the priority of the playback controller's sub thread.

This can be used to increase the responsiveness of the audio plugin to minimise
any lag in processing. This function should be used with care as it may have knock-on
effects elsewhere in the system.

@param	aPriority
		The TThreadPriority that the thread should run under.  The default is EPriorityNormal.
@return	TInt
		A standard error code: KErrNone if successful, KErrNotReady if the thread does not have a
		valid handle.
*/
EXPORT_C TInt CMdaAudioRecorderUtility::SetThreadPriorityPlayback(const TThreadPriority& aPriority) const
	{
	ASSERT(iProperties);
	return iProperties->SetThreadPriorityPlayback(aPriority);
	}

/**
Set the priority of the record controller's sub thread.

This can be used to increase the responsiveness of the audio plugin to minimise
any lag in processing. This function should be used with care as it may have knock-on
effects elsewhere in the system.

@param	aPriority
		The TThreadPriority that the thread should run under.  The default is EPriorityNormal.
@return	TInt
		A standard error code: KErrNone if successful, KErrNotReady if the thread does not have a
		valid handle.
*/
EXPORT_C TInt CMdaAudioRecorderUtility::SetThreadPriorityRecord(const TThreadPriority& aPriority) const
	{
	ASSERT(iProperties);
	return iProperties->SetThreadPriorityRecord(aPriority);
	}

CMMFMdaAudioRecorderUtility* CMMFMdaAudioRecorderUtility::NewL(MMdaObjectStateChangeObserver& aObserver, 
																  CMdaServer* /*aServer*/, 
																  TInt aPriority, 
																  TInt aPref)
	{
	CMMFMdaAudioRecorderUtility* self = new(ELeave) CMMFMdaAudioRecorderUtility(NULL, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}


// CMMFMdaAudioRecorderUtility
CMMFMdaAudioRecorderUtility::CMMFMdaAudioRecorderUtility(CMdaAudioRecorderUtility* aParent,
														 MMdaObjectStateChangeObserver& aCallback) :
	iCallback(aCallback),
	iRecordControllerAvailable(EFalse),
	iPlaybackControllerAvailable(EFalse),
	iAudioPlayDeviceCustomCommands(iPlaybackController),
	iAudioRecordDeviceCustomCommands(iRecordController),
	iAudioPlayControllerCustomCommands(iPlaybackController),
	iAudioRecordControllerCustomCommands(iRecordController),
	iAudioControllerRecCustomCommands(iRecordController),
	iAudioControllerPlyCustomCommands(iPlaybackController),
	iNotificationRegistrationCommands(iPlaybackController),
	iDRMCustomCommands(iPlaybackController),
	iAudioPlayControllerSetRepeatsCommands(iPlaybackController),
	iParent(aParent),
	iCachedDescriptor( NULL, 0 )
	{
	iPlayDataPathInitialised = EFalse;
	iPlayWindowSet = ENone;
	iPlayFormatUid = KNullUid;
	iPlayDataType = KMMFFourCCCodeNULL;
	iPlayChannels = 0;
	iPlaySampleRate = 0;
	}

void CMMFMdaAudioRecorderUtility::ConstructL(TInt aPriority, TInt aPref)
	{
	iRecordEventMonitor = CMMFControllerEventMonitor::NewL(*this,iRecordController);
	iPlaybackEventMonitor = CMMFControllerEventMonitor::NewL(*this,iPlaybackController);

	iRepeatTrailingSilenceTimer = CRepeatTrailingSilenceTimer::NewL(*this);
	iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;
	User::LeaveIfError(iMediaIds.Append(KUidMediaTypeAudio));
	iAsyncCallback = CMMFMdaObjectStateChangeObserverCallback::NewL(iCallback);
	iFindAndOpenController = CMMFFindAndOpenController::NewL(*this);
	iFindAndOpenController->Configure(iMediaIds[0], iPrioritySettings);
	iFindAndOpenController->ConfigureSecondaryController(iRecordController, *iRecordEventMonitor, CMMFFindAndOpenController::ERecord);
	iFindAndOpenController->ConfigureController(iPlaybackController, *iPlaybackEventMonitor, CMMFFindAndOpenController::EPlayback);
	}

CMMFMdaAudioRecorderUtility::~CMMFMdaAudioRecorderUtility()
	{
	Close(); // make sure the file is closed, and the event monitors are cancelled
	iMediaIds.Close();
	delete iFindAndOpenController;
	delete iPlaybackEventMonitor;
	delete iRecordEventMonitor;
	delete iAsyncCallback;
	delete iRepeatTrailingSilenceTimer;
	delete iPlaybackSourceData;
	delete iPlayControllerImplementationInformation;
	delete iRecordControllerImplementationInformation;
	}

void CMMFMdaAudioRecorderUtility::UseSharedHeap()
	{
	iFindAndOpenController->UseSharedHeap();
	}

void CMMFMdaAudioRecorderUtility::MfaocComplete(		
		TInt& aError, 
		RMMFController* aController, 
		TUid aControllerUid, 
		TMMFMessageDestination* /*aSourceHandle*/, 
		TMMFMessageDestination* /*aSinkHandle*/)
	{
	if (aController == &iRecordController)
		{
		if (aError == KErrNone)
			{
			iRecordControllerAvailable = ETrue;
			iRecordControllerUid = aControllerUid;
			}
		else if (aError == KErrNoMemory || aError == KErrCancel ||				
				 aError == KErrCANoRights || aError == KErrCANoPermission)
			// if error is no rights, or permission denied, we send this to the 
			// client and do not fallback on the playbackcontroller (as this would
			// produce the same error).
			{			
			iAsyncCallback->CallBack(iParent, iState, iState, aError);
			}
		else
			{
			// if we don't find a record controller,
			// swallow the error & indicate to CMMFFindAndOpenController
			// that we want to open the primary (playback) controller anyway
			aError = KErrNone;	
			}	
		}
	else		// aController == &iPlaybackController
		{
		if (aError == KErrNone)
			{
			iPlaybackControllerAvailable = ETrue;
			iPlayControllerUid = aControllerUid;
			}
			
		//If the Client is using a Source/Sink file that does not have the right 
		//permissions (i.e. Intent - EUnknown, ilegal Unique Id) 
		//an Error = KErrCANoPermission, KErrCANoRights, KErrNotFound is sent to him.
		else if (aError == KErrCANoPermission || aError == KErrCANoRights ||
				 aError == KErrNotFound)
			{
			iAsyncCallback->CallBack(iParent, iState, iState, aError);
			}

		// If the record controller UID was supplied but not the playback 
		// controller's, then we assumed that they wanted to use the record 
		// controller uid for  playback. However a failure to open (KErrNotFound) 
		// the play controller under this assumption is not reported back.
		if (iPlaybackControllerUidNotSupplied &&
			iRecordControllerUid != KNullUid &&
			iPlayControllerUid == KNullUid &&	// not supplied
			aError == KErrNotFound)
			{
			aError = KErrNone;
			}

		if (aError == KErrNone)
			aError = ControllerOpen();


		// we're either opening the controller for the first time
		// or re-opening the controller prior to playing
		if (aError == KErrNone && !iClientCallbackPending)		// re-opening ?
			return;		

		if (aError == KErrNone)
			aError = OpenComplete();  
		
		// this will report error if error occured during adding of source /sink.
		// Error get really detected when Duration of clip is retrieved.
		// So,if error occurs, try for next controllers if they are present in the selection list.
		if (aError != KErrNone)
			{
			if(iFindAndOpenController-> ControllerIndex() < (iFindAndOpenController->ControllerCount())-1)
				{
				if(!iFindAndOpenController->StopTryLoadController())
					{
					return;	
					}
				}
			}

		TInt oldState = iState;
		if (aError == KErrNone)
			iState = CMdaAudioClipUtility::EOpen;

		iAsyncCallback->CallBack(iParent, oldState, iState, aError);
		}
	}
	
	
void CMMFMdaAudioRecorderUtility::OpenFileL(const TDesC& aFileName)
	{
	TMMFileSource filesource(aFileName, KDefaultContentObject, EPlay);
	OpenFileL(filesource);
	}

void CMMFMdaAudioRecorderUtility::OpenFileL(const TMMSource& aSource)
	{
	// Make sure we are closed
	Reset();

	if (aSource.SourceType()== KUidMMFileHandleSource)
		{
		const RFile& file = static_cast<const TMMFileHandleSource&>(aSource).Handle();
		iFindAndOpenController->ConfigureSourceSink(
			aSource,
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
		iFindAndOpenController->ConfigureSecondarySourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput),
			CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, file)); 
				
		}
	if (aSource.SourceType()==KUidMMFileSource)
		{
		const TDesC& fileName = static_cast<const TMMFileSource&>(aSource).Name();
		iFindAndOpenController->ConfigureSourceSink(
			aSource,
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
		iFindAndOpenController->ConfigureSecondarySourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput),
			CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, CMMFFindAndOpenController::GetConfigFile(fileName))); 
		}
	iFindAndOpenController->OpenByFileSource(aSource);
	}


void CMMFMdaAudioRecorderUtility::OpenDesL(TDes8& aDescriptor)
	{
	// Make sure we are closed
	Reset();
	
	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSource, CMMFFindAndOpenController::GetConfigDescriptor(aDescriptor)), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
	iFindAndOpenController->ConfigureSecondarySourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput),
		CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSink, CMMFFindAndOpenController::GetConfigDescriptor(aDescriptor))); 
	
	iFindAndOpenController->OpenByDescriptor(aDescriptor);
	}

/**************************************************
*
* ConvertDescriptor
* aParam aDescriptor
* This method converts a TDesC8 to a TDes8
* and is here to fix an interface error
*
**/
void CMMFMdaAudioRecorderUtility::ConvertDescriptor(const TDesC8& aDescriptor )
	{
	//[ casting away const ptr to ptr ]
	TUint8* desBufferPtr = const_cast<TUint8*> (aDescriptor.Ptr());
	iCachedDescriptor.Set( desBufferPtr,aDescriptor.Length(),aDescriptor.Length());
	}
void CMMFMdaAudioRecorderUtility::OpenDesL(const TDesC8& aDescriptor)
	{
	// [ cache and forward the descriptor ]
	 ConvertDescriptor( aDescriptor );
	 OpenDesL( iCachedDescriptor );
	}

void CMMFMdaAudioRecorderUtility::OpenUrlL(const TDesC& aUrl, TInt aIapId, const TDesC8& aMimeType)
	{
	// Make sure we are closed
	Reset();

	CBufFlat* urlCfgBuffer = NULL;
	CMMFFindAndOpenController::GetConfigUrlL(urlCfgBuffer, aUrl, aIapId);
	
	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
	iFindAndOpenController->ConfigureSecondarySourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));

	iFindAndOpenController->OpenByUrl(aUrl, aIapId, aMimeType);
	delete urlCfgBuffer;
	}

TInt CMMFMdaAudioRecorderUtility::RecordOrPlaybackFormat(const RMMFControllerImplInfoArray& aControllers, TUid aSuppliedFormatUid, TUid& aRecordFormatUid, TUid& aPlayFormatUid)
	{
	for (TInt controllerIndex=0; controllerIndex < aControllers.Count(); controllerIndex++)
		{
		const RMMFFormatImplInfoArray& recordFormatInfo = aControllers[controllerIndex]->RecordFormats();
		for(TInt recordFormatIndex =0; recordFormatIndex < recordFormatInfo.Count(); recordFormatIndex++)
			{
			if(recordFormatInfo[recordFormatIndex]->Uid() == aSuppliedFormatUid)
				{
				aRecordFormatUid = aSuppliedFormatUid;
				return KErrNone;
				}
			}

		const RMMFFormatImplInfoArray& playFormatInfo = aControllers[controllerIndex]->PlayFormats();
		for(TInt playFormatIndex =0; playFormatIndex < playFormatInfo.Count(); playFormatIndex++)
			{
			if(playFormatInfo[playFormatIndex]->Uid() == aSuppliedFormatUid)
				{
				aPlayFormatUid = aSuppliedFormatUid;
				return KErrNone;
				}
			}
		}

	return KErrNotSupported;
	}

const RMMFFormatImplInfoArray& CMMFMdaAudioRecorderUtility::ControllersFormatInfo(const CMMFControllerImplementationInformation* aController, TControllerType aControllerType)
	{
	if(aControllerType == ERecordController)
		return aController->RecordFormats();
	else return aController->PlayFormats();
	}

//Search for controllers that support the required record format
TUid CMMFMdaAudioRecorderUtility::FindControllerL(const RMMFControllerImplInfoArray& aControllers, TUid aFormatUid, TControllerType aControllerType, const CDesC8Array*& selectedFormatMimeTypes)
	{
	for (TInt controllerIndex=0; controllerIndex < aControllers.Count(); controllerIndex++)
		{
		const RMMFFormatImplInfoArray& formatInfo = ControllersFormatInfo(aControllers[controllerIndex], aControllerType);

		for(TInt formatIndex =0; formatIndex < formatInfo.Count(); formatIndex++)
			{
			if(formatInfo[formatIndex]->Uid() == aFormatUid)
				{
				selectedFormatMimeTypes = &formatInfo[formatIndex]->SupportedMimeTypes();
				return aControllers[controllerIndex]->Uid();
				}
			}
		}
	return KNullUid;
	}

//Search for controllers that support the required mime type
TUid CMMFMdaAudioRecorderUtility::FindControllerL(const RMMFControllerImplInfoArray& aControllers, const CDesC8Array*& aMimeTypes, TControllerType aControllerType, TUid& aFormat)
	{
	if(!aMimeTypes || (aMimeTypes->MdcaCount() ==0))
		return KNullUid;

	for (TInt controllerIndex=0; controllerIndex < aControllers.Count(); controllerIndex++)
		{
		const RMMFFormatImplInfoArray& formatInfo = ControllersFormatInfo(aControllers[controllerIndex], aControllerType);

		for(TInt formatIndex =0; formatIndex < formatInfo.Count(); formatIndex++)
			{
			const CDesC8Array& ControllersMimeTypes = formatInfo[formatIndex]->SupportedMimeTypes();
			for(TInt mimeType =0; mimeType < ControllersMimeTypes.Count(); mimeType++)
				{
				if(ControllersMimeTypes[mimeType] == (*aMimeTypes)[0]) //only check frst mime type
					{
					aFormat = formatInfo[formatIndex]->Uid();
					return aControllers[controllerIndex]->Uid();
					}
				}
			}
		}

	return KNullUid;
	}

//Search for controller that supports the file extensions
TUid CMMFMdaAudioRecorderUtility::FindControllerL(const RMMFControllerImplInfoArray& aControllers, const TMdaClipLocation* aLocation, TControllerType aControllerType, TUid& aFormat)
	{
	if(aLocation->Uid() != KUidMdaFileResLoc)
		return KNullUid;

	const CDesC8Array* aFileExtensions = NULL;

	//Use ability of CMMFFormatSelectionParameters to extract extension from filename
	TDesC& fileName = ((TMdaFileClipLocation*)aLocation)->iName;
	CMMFFormatSelectionParameters* fileNameSelector = CMMFFormatSelectionParameters::NewLC();
	fileNameSelector->SetMatchToFileNameL(fileName);
	const TDesC8& fileExtension = fileNameSelector->MatchData();

	for (TInt controllerIndex=0; controllerIndex < aControllers.Count(); controllerIndex++)
		{
		const RMMFFormatImplInfoArray& formatInfo = ControllersFormatInfo(aControllers[controllerIndex], aControllerType);

		for(TInt formatIndex =0; formatIndex < formatInfo.Count(); formatIndex++)
			{
			aFileExtensions = &formatInfo[formatIndex]->SupportedFileExtensions();
			if(aFileExtensions)
				{
				for(TInt fileExtensionIndex =0; fileExtensionIndex < aFileExtensions->MdcaCount(); fileExtensionIndex++)
					{
					if(fileExtension == (*aFileExtensions)[fileExtensionIndex])
						{
						aFormat = formatInfo[formatIndex]->Uid();
						CleanupStack::PopAndDestroy(fileNameSelector);
						return aControllers[controllerIndex]->Uid();
						}
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(fileNameSelector);
	return KNullUid;
	}

void CMMFMdaAudioRecorderUtility::OpenL(TMdaClipLocation* aLocation,	// Normally file or descriptor
										TMdaClipFormat* aFormat,		// Data format
										TMdaPackage* aArg1,				// Normally codec to use
										TMdaPackage* aArg2)				// Normally audio settings
	{
	__ASSERT_ALWAYS(aLocation && aFormat, User::Leave(KErrArgument)); 
	__ASSERT_ALWAYS(aFormat->Uid() != KNullUid, User::Leave(KErrNotSupported)); 

	TInt err = KErrNone;
	TUid playController = KNullUid;
	TUid recordController = KNullUid;

	// Make sure we are closed
	Reset();

	TUid recordFormatUid = KNullUid;
	TUid playFormatUid = KNullUid;

	//If a new format plugin is requested, aFormat will not be one of the supported MdaFormats
	//and CMMFClientUtility::ConvertMdaFormatUidToECOM... will return NULL
	recordFormatUid = CMMFClientUtility::ConvertMdaFormatUidToECOMWrite(aFormat->Uid());
	playFormatUid = CMMFClientUtility::ConvertMdaFormatUidToECOMRead(aFormat->Uid());


	if (aArg1) 
		iPlayDataType = CMMFClientUtility::ConvertMdaCodecToFourCC(*aArg1);

	RMMFControllerImplInfoArray controllers;
	CleanupResetAndDestroyPushL(controllers);

	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();

	// Select the media IDs to allow
	cSelect->SetMediaIdsL(iMediaIds,CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);

	CMMFFormatSelectionParameters* fSelectPlay = CMMFFormatSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelectRecord = CMMFFormatSelectionParameters::NewLC();
	cSelect->SetRequiredRecordFormatSupportL(*fSelectRecord);
	cSelect->SetRequiredPlayFormatSupportL(*fSelectPlay);

	//Obtain a list of the controllers
	cSelect->ListImplementationsL(controllers);

	if (controllers.Count()==0)
		err = KErrNotSupported;

	//If a new format plugin has been added, determine whether aFormat is for record or playback
	if(!err && (recordFormatUid == KNullUid))
		{
		err = RecordOrPlaybackFormat(controllers, aFormat->Uid(), recordFormatUid, playFormatUid);
		}

	const CDesC8Array* selectedFormatMimeTypes = NULL;

	if(!err && (recordFormatUid != KNullUid))
		{
		TRAP(err, recordController = FindControllerL(controllers, recordFormatUid, ERecordController, selectedFormatMimeTypes));
		if((recordController == KNullUid) && !err)
			err = KErrNotSupported;
		}
	if(!err && (playFormatUid != KNullUid))
		{
		TRAP(err, playController = FindControllerL(controllers, playFormatUid, EPlayController, selectedFormatMimeTypes));
		if((playController == KNullUid) && !err)
			err = KErrNotSupported;
		}


	//At this stage we must have found at least one controller, record or playback
	//If we have found only one controller, use mime type info from it to identify the other
	if(!err && (recordController == KNullUid))
		{
		if(selectedFormatMimeTypes)
			TRAP(err, recordController = FindControllerL(controllers, selectedFormatMimeTypes, ERecordController, recordFormatUid));
		
		//if searching on mime type hasn't worked, as a last resort try file extension.
		if((recordController == KNullUid) && !err)
			TRAP(err, recordController = FindControllerL(controllers, aLocation, ERecordController, recordFormatUid));
		}
	else if(!err && (playController == KNullUid))
		{
		if(selectedFormatMimeTypes)
			TRAP(err, playController = FindControllerL(controllers, selectedFormatMimeTypes, EPlayController, playFormatUid));
		
		//if searching on mime type hasn't worked, as a last resort try file extension.
		if((playController == KNullUid) && !err)
			TRAP(err, playController = FindControllerL(controllers, aLocation, EPlayController, playFormatUid));
		}

	
	//Did we find the play controller that supports the required format
	if((recordController == KNullUid) && (playController == KNullUid) && (err == KErrNone))
		err = KErrNotSupported;

		
	CleanupStack::PopAndDestroy(4);//controllers, fSelectRecord, fSelectPlay, cSelect

	// get the play sample rate & channels if defined
	iPlaySampleRate = iPlayChannels = 0;
	if (!err && aArg2)
		{//have audio settings
		TMdaAudioDataSettings audioSettings = *(TMdaAudioDataSettings*)aArg2; //shoud check arg2 are data settings
		if (audioSettings.iSampleRate)
			{
			iPlaySampleRate = audioSettings.iSampleRate;
			}
		if (!err && audioSettings.iChannels)
			{
			iPlayChannels = audioSettings.iChannels;
			}
		}

	// define what needs to be done once controller(s) have been opened
	if (err == KErrNone)
		{
		iRecordFormatUid = recordFormatUid;
		iPlayFormatUid = playFormatUid;
		iRecordSinkDataType = iPlayDataType;
		}

	iPlayControllerUid = playController;
	iRecordControllerUid = recordController;
	
	if (!err && aLocation->Uid() == KUidMdaFileResLoc)
		{//sink clip location is a file
		TDesC& fileName = ((TMdaFileClipLocation*)aLocation)->iName;
		iFindAndOpenController->ConfigureSourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfFileSource, CMMFFindAndOpenController::GetConfigFile(fileName)), 
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
		iFindAndOpenController->ConfigureSecondarySourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput), 
			CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, CMMFFindAndOpenController::GetConfigFile(fileName)));
			
		iFindAndOpenController->OpenByControllerUid(iPlayControllerUid, iRecordControllerUid);
		}
	else if (!err && aLocation->Uid() == KUidMdaDesResLoc)
		{//sink clip is a descriptor - pass down descriptor & thread id
		TMdaDesClipLocation* desLoc = (TMdaDesClipLocation*)aLocation;
		iFindAndOpenController->ConfigureSourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSource, CMMFFindAndOpenController::GetConfigDescriptor(*desLoc->iDes)), 
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput, KNullDesC8));
		iFindAndOpenController->ConfigureSecondarySourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput, KNullDesC8), 
			CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSink, CMMFFindAndOpenController::GetConfigDescriptor(*desLoc->iDes)));

		iFindAndOpenController->OpenByControllerUid(iPlayControllerUid, iRecordControllerUid);
		}
	else if(!err)
		{
		// we do not want to allow new functionality for the old API. So we
		// don't support URL for the old functions.
		err = KErrNotSupported;
		}

	if (err)
		iAsyncCallback->CallBack(iParent, iState, iState, err);
	}

TInt CMMFMdaAudioRecorderUtility::ControllerOpen()
	{
	TInt err = KErrNone;

	// we're either opening the controller for the first time 
	// (iClientCallbackPending = ETrue) or re-opening the controller 
	// prior to playing...
	if (iClientCallbackPending)
		{
		if (iRecordFormatUid != KNullUid)
			err = iAudioControllerRecCustomCommands.SetSinkFormat(iRecordFormatUid);

		if (!err && iRecordSinkDataType != KMMFFourCCCodeNULL) 
			{
			err = iAudioControllerRecCustomCommands.SetCodec(iRecordSourceDataType, iRecordSinkDataType);
			}
		
		// set the audio data settings ie sample rate & channels
		if (!err && iPlaySampleRate)
			{
			TRAP(err, SetDestinationSampleRateL(iPlaySampleRate));
			}
		if (!err && iPlayChannels)
			{
			TRAP(err, SetDestinationNumberOfChannelsL(iPlayChannels));
			}
		}
	else
		{
		SetupPlaybackDataPathComplete();
		}

	return err;
	}
	
void CMMFMdaAudioRecorderUtility::OpenFileL(const TDesC& aFileName,	
						TUid aControllerUid,
						TUid aPlaybackControllerUid,
						TUid aFormatUid,		
						TFourCC aDataType)
	{
	TMMFileSource filesource(aFileName, KDefaultContentObject, EPlay);
	OpenFileL(filesource, aControllerUid, aPlaybackControllerUid, aFormatUid, aDataType);	
	}
	
	

void CMMFMdaAudioRecorderUtility::OpenFileL(const TMMSource& aSource,	
						TUid aControllerUid,
						TUid aPlaybackControllerUid,
						TUid aFormatUid,		
						TFourCC aDataType)
						
	{
	Reset();

	iRecordFormatUid = aFormatUid;
	iPlayFormatUid = aFormatUid;
	iRecordSinkDataType = aDataType;
	TInt err = KErrNotSupported;
	if (aSource.SourceType()==KUidMMFileSource)
		{
		const TMMFileSource& fileSource = static_cast<const TMMFileSource&>(aSource);
		err = DoOpenFileSource(fileSource, aControllerUid, aPlaybackControllerUid);
		}
	else if (aSource.SourceType()==KUidMMFileHandleSource)
		{
		const TMMFileHandleSource& fileSource = static_cast<const TMMFileHandleSource&>(aSource);
		err = DoOpenFileHandleSource(fileSource, aControllerUid, aPlaybackControllerUid);
		}
	
	if (err != KErrNone)
		iAsyncCallback->CallBack(iParent, iState, iState, err);
	}
	
void CMMFMdaAudioRecorderUtility::OpenDesL(TDes8& aDescriptor,	
						TUid aControllerUid,
						TUid aPlaybackControllerUid,
						TUid aFormatUid,		
						TFourCC aDataType)
						
	{
	Reset();

	// define what needs to be done once controller(s) have been opened
	iRecordFormatUid = aFormatUid;
	iPlayFormatUid = aFormatUid;
	iRecordSinkDataType = aDataType;

	TInt err = DoOpenDes(aDescriptor, aControllerUid, aPlaybackControllerUid);
	if (err != KErrNone)
		iAsyncCallback->CallBack(iParent, iState, iState, err);
	}

void CMMFMdaAudioRecorderUtility::OpenUrlL(const TDesC& aUrl,
						TInt aIapId,
						TUid aControllerUid,
						TUid aPlaybackControllerUid,
						TUid aFormatUid,		
						TFourCC aDataType)
						
	{
	Reset();

	iRecordFormatUid = aFormatUid;
	iPlayFormatUid = aFormatUid;
	iRecordSinkDataType = aDataType;

	TInt err = DoOpenUrlL(aUrl, aIapId, aControllerUid, aPlaybackControllerUid);
	if (err != KErrNone)
		iAsyncCallback->CallBack(iParent, iState, iState, err);
	}


TInt CMMFMdaAudioRecorderUtility::DoOpenFileHandleSource(const TMMFileHandleSource& aSource,	
						TUid aControllerUid,
						TUid aPlaybackControllerUid)
	{
	if((aControllerUid == KNullUid) && (aPlaybackControllerUid == KNullUid))
		return KErrNotSupported;

	TInt err = KErrNone;


	//If aPlayController is KNullUid, then assume that they want to use the
	//record controller for playback. However a failure to open (KErrNotFound) the 
	//play controller under this assumption is not reported back.

	if(aPlaybackControllerUid == KNullUid)
		{
		iPlaybackControllerUidNotSupplied = ETrue;
		aPlaybackControllerUid = aControllerUid;
		}

	if(aControllerUid == KNullUid)	// playback only ?
		{
		iFindAndOpenController->ConfigureSourceSink(
			aSource,
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
		}
	else
		{
		iFindAndOpenController->ConfigureSourceSink(
			aSource,
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
		iFindAndOpenController->ConfigureSecondarySourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput), 
			CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, aSource.Handle()));
		}
	iFindAndOpenController->OpenByControllerUid(aPlaybackControllerUid, aControllerUid);

	return err;
	}
	
TInt CMMFMdaAudioRecorderUtility::DoOpenFileSource(const TMMFileSource& aSource,	
						TUid aControllerUid,
						TUid aPlaybackControllerUid)
	{
	if((aControllerUid == KNullUid) && (aPlaybackControllerUid == KNullUid))
		return KErrNotSupported;

	TInt err = KErrNone;


	//If aPlayController is KNullUid, then assume that they want to use the
	//record controller for playback. However a failure to open (KErrNotFound) the 
	//play controller under this assumption is not reported back.

	if(aPlaybackControllerUid == KNullUid)
		{
		iPlaybackControllerUidNotSupplied = ETrue;
		aPlaybackControllerUid = aControllerUid;
		}

	if(aControllerUid == KNullUid)	// playback only ?
		{
		iFindAndOpenController->ConfigureSourceSink(
			aSource,
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput, KNullDesC8));
		}
	else
		{
		iFindAndOpenController->ConfigureSourceSink(
			aSource,
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput, KNullDesC8));
		iFindAndOpenController->ConfigureSecondarySourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput, KNullDesC8), 
			CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, CMMFFindAndOpenController::GetConfigFile(aSource.Name())));
		}
	iFindAndOpenController->OpenByControllerUid(aPlaybackControllerUid, aControllerUid);

	return err;
	}
	


TInt CMMFMdaAudioRecorderUtility::DoOpenDes(TDesC8& aDescriptor,	
						TUid aControllerUid,
						TUid aPlaybackControllerUid)
						
	{
	if((aControllerUid == KNullUid) && (aPlaybackControllerUid == KNullUid))
		{
		return KErrNotSupported;
		}
	//If aPlayController is KNullUid, then assume that they want to use the
	//record controller for playback. However a failure to open (KErrNotFound) the 
	//play controller under this assumption is not reported back.

	if(aPlaybackControllerUid == KNullUid)
		{
		iPlaybackControllerUidNotSupplied = ETrue;
		aPlaybackControllerUid = aControllerUid;
		}	

	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSource, CMMFFindAndOpenController::GetConfigDescriptor(aDescriptor)), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput, KNullDesC8));
	iFindAndOpenController->ConfigureSecondarySourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput, KNullDesC8), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSink, CMMFFindAndOpenController::GetConfigDescriptor(aDescriptor)));
	iFindAndOpenController->OpenByControllerUid(aPlaybackControllerUid, aControllerUid);

	return KErrNone;
	}

TInt CMMFMdaAudioRecorderUtility::DoOpenUrlL(const TDesC& aUrl,
						TInt aIapId,
						TUid aControllerUid,
						TUid aPlaybackControllerUid)
						
	{
	if((aControllerUid == KNullUid) && (aPlaybackControllerUid == KNullUid))
		return KErrNotSupported;

	iRecordControllerUid = aControllerUid;
	iPlayControllerUid = aPlaybackControllerUid;
	
	CBufFlat* urlCfgBuffer = NULL;
	CMMFFindAndOpenController::GetConfigUrlL(urlCfgBuffer, aUrl, aIapId);
	
	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput, KNullDesC8));
	iFindAndOpenController->ConfigureSecondarySourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput, KNullDesC8), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfUrlSink, urlCfgBuffer->Ptr(0)));
	iFindAndOpenController->OpenByControllerUid(aPlaybackControllerUid, aControllerUid);

	delete urlCfgBuffer;

	return KErrNone;
	}


TInt CMMFMdaAudioRecorderUtility::OpenComplete() 
	{
	// Get the clip duration
	Duration();
	if(iDurationError == KErrNone && iFindAndOpenController)
		{
		iFindAndOpenController->CloseConfig();
		}
	return iDurationError;
	}

void CMMFMdaAudioRecorderUtility::ClosePlaybackController()
	{
	if (iPlaybackEventMonitor)
		iPlaybackEventMonitor->Cancel();
	iPlaybackControllerAvailable = EFalse;
	if(iPlayControllerImplementationInformation)
		{
		delete iPlayControllerImplementationInformation;
		iPlayControllerImplementationInformation = NULL;
		}
	iPlaybackController.Close();
	iPlayControllerUid = KNullUid;
	}

void CMMFMdaAudioRecorderUtility::CloseRecordController()
	{
	if (iRecordEventMonitor)
		iRecordEventMonitor->Cancel();
	iRecordControllerAvailable = EFalse;
	if(iRecordControllerImplementationInformation)
		{
		delete iRecordControllerImplementationInformation;
		iRecordControllerImplementationInformation = NULL;
		}
	iRecordController.Close();
	iRecordControllerUid = KNullUid;
	}

void CMMFMdaAudioRecorderUtility::CloseControllers()
	{
	CloseRecordController();
	ClosePlaybackController();
	}

void CMMFMdaAudioRecorderUtility::SetupPlaybackDataPath() 
	{
	if (!iPlaybackControllerAvailable)
		{
		iAsyncCallback->CallBack(iParent, iState, iState, KErrNotSupported);	
		return;
		}

	// Ensure the playback  controller is made aware that the clip has changed
	if (iClipChanged)
		{
		// save the playback settings so we can restore them later
		TInt value;
		if (KErrNone == GetVolume(value))
			iPlayBackSettings.iVolume = value;
		if (KErrNone == GetPlaybackBalance(value))
			iPlayBackSettings.iBalance = value;
		
		ClosePlaybackController();

		//OpenControllerPostProcessingConfigure(EFalse);	// no callback to client
		iClientCallbackPending = EFalse;	// no callback to client
		iClipChanged = EFalse;
		
		iFindAndOpenController->ReOpen();
		}
	else
		{
		TInt bal = 0 ;
		if (KErrNone == GetPlaybackBalance(bal))
             {
		  	 iPlayBackSettings.iBalance = bal;
		  	 }
		SetupPlaybackDataPathComplete();
        }
	}


void CMMFMdaAudioRecorderUtility::SetupPlaybackDataPathComplete()
	{
	TInt error = KErrNone;

	if (iPlayFormatUid != KNullUid && iPlaybackControllerAvailable)
		error = iAudioControllerPlyCustomCommands.SetSourceFormat(iPlayFormatUid);

	if (!error && (iPlayDataType != KMMFFourCCCodeNULL)  && iPlaybackControllerAvailable) 
		error = iAudioControllerPlyCustomCommands.SetCodec(iPlayDataType, KMMFFourCCCodeNULL);

	if (!error && iPlaySampleRate  && iPlaybackControllerAvailable)
		error = iAudioControllerPlyCustomCommands.SetSourceSampleRate(iPlaySampleRate);
			
	if (!error && iPlayChannels  && iPlaybackControllerAvailable)
		error = iAudioControllerPlyCustomCommands.SetSourceNumChannels(iPlayChannels);

	if (!error)
		iPlayDataPathInitialised = ETrue;

	if (!error)
		{
		if ( iPlayBackSettings.iVolume > -1)
			{
			error = SetVolume(iPlayBackSettings.iVolume);
			}
		if (error == KErrNone)
			{
			error=SetPlaybackBalance(iPlayBackSettings.iBalance);
			}
		if ( error == KErrNone)
			{
			SetVolumeRamp(iPlayBackSettings.iVolumeRampInterval);
			iPlayBackSettings.iVolumeRampInterval = 0;
			DoPlayContinue();
			}
		}

	if (error)
		{
		iAsyncCallback->CallBack(iParent, iState, iState, error);	
		}
	}


void CMMFMdaAudioRecorderUtility::SetAudioDeviceMode(CMdaAudioRecorderUtility::TDeviceMode /*aMode*/)
	{
	// this functionality is no longer provided
	}

TInt CMMFMdaAudioRecorderUtility::MaxVolume()
	{
	if(!iPlaybackControllerAvailable)
		return 0;

	TInt vol = 0;
	TInt error = iAudioPlayDeviceCustomCommands.GetMaxVolume(vol);
	__ASSERT_DEBUG(error==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem)); 
	if (error!=KErrNone)
		vol = 0;
	return vol;
	}

TInt CMMFMdaAudioRecorderUtility::MaxGain()
	{
	if(!iRecordControllerAvailable)
		return 0;

	TInt gain = 0;
	TInt error = iAudioRecordDeviceCustomCommands.GetMaxGain(gain);
	__ASSERT_DEBUG(error==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem));
	if (error!=KErrNone)
		gain = 0;
	return gain;
	}

TInt CMMFMdaAudioRecorderUtility::SetVolume(TInt aVolume)
	{
	if(!iPlaybackControllerAvailable)
		{
		return KErrNotReady;
		}
	
	TInt err = iAudioPlayDeviceCustomCommands.SetVolume(aVolume);
	if (err == KErrArgument)
		{
		TInt maxVolume = MaxVolume();
		if (aVolume < 0)
			{
			aVolume = 0;
			}
		else if (aVolume > maxVolume)
			{
			aVolume = maxVolume;
			}
		err = iAudioPlayDeviceCustomCommands.SetVolume(aVolume);			
		}

	return err; 
	}

/**
 *
 * Get the current volume
 *
 * @param	"TInt& aVolume"
 *          The current volume
 *
 * @return	"TInt" One of the system wide error codes
 * @since	7.0s
 */		
TInt CMMFMdaAudioRecorderUtility::GetVolume(TInt& aVolume)
	{
	if(!iPlaybackControllerAvailable)
		return KErrNotSupported;

	TInt err = iAudioPlayDeviceCustomCommands.GetVolume(aVolume);
	return err;
	}



void CMMFMdaAudioRecorderUtility::SetGain(TInt aGain)
	{
	if(!iRecordControllerAvailable)
		return;

	iAudioRecordDeviceCustomCommands.SetGain(aGain);
	}

void CMMFMdaAudioRecorderUtility::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	if(!iPlaybackControllerAvailable)
		return;

	iAudioPlayDeviceCustomCommands.SetVolumeRamp(aRampDuration);
	iPlayBackSettings.iVolumeRampInterval = aRampDuration;
	}



/**
Handles an event from a controller.

@param  aEvent
        The received event. If the error code contained in aEvent is one of
        KErrInUse, KErrDied or KErrAccessDenied this indicates that the sound device is in use
        by another higher priority client.
*/
void CMMFMdaAudioRecorderUtility::HandleEvent(const TMMFEvent& aEvent)
	{
		// handle loading started/complete messages first, as the later code does not explictly check the event type
	if (aEvent.iEventType == KMMFEventCategoryAudioLoadingStarted)
		{
		if (iLoadingObserver)
			{
			iLoadingObserver->MaloLoadingStarted();
			}
		}
	else if (aEvent.iEventType == KMMFEventCategoryAudioLoadingComplete)
		{
		if (iLoadingObserver)
			{
			iLoadingObserver->MaloLoadingComplete();
			}
		}
	else if (aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable)
		{
		if (iAudioResourceNotificationCallBack != NULL)
			{
			TBuf8<TMMFAudioConfig::KNotificationDataBufferSize> notificationData;
			if (KErrNone != iNotificationRegistrationCommands.GetResourceNotificationData(aEvent.iEventType,notificationData))
				{
				notificationData.SetLength(0);
				}
			iAudioResourceNotificationCallBack->MarncResourceAvailable(aEvent.iEventType,notificationData);
			}
		}	
	else if (aEvent.iEventType == KMMFEventCategoryPlaybackComplete)
		{
		if (iState == CMdaAudioClipUtility::ERecording)
			{
			// reset the position for subsequent records
			iPosition = iPlayStart;	
			iState = CMdaAudioClipUtility::EOpen;

			iCallback.MoscoStateChangeEvent(iParent, CMdaAudioClipUtility::ERecording, iState, aEvent.iErrorCode);
			}
		else if (iState == CMdaAudioClipUtility::EPlaying)
			{
			// reset the position for subsequent plays
			iPosition = iPlayStart;	

			iNumberOfTimesPlayed++;

			//If playback completed with an error or we have played enough times.
			if ((aEvent.iErrorCode != KErrNone) ||
				((iNumberOfTimesPlayed > iNumberOfTimesToRepeat) && (iNumberOfTimesToRepeat != KMdaRepeatForever)))
				{
				//we've repeated enough times now
				iNumberOfTimesPlayed = 0;
				iState = CMdaAudioClipUtility::EOpen;
				iCallback.MoscoStateChangeEvent(iParent, CMdaAudioClipUtility::EPlaying, iState, aEvent.iErrorCode);
				}
			else
				{
				// We need to play silence, then repeat the clip
				iTrailingSilenceLeftToPlay = iTrailingSilence;
				PlaySilence();
				}
			}
		else //iState != EPlaying or ERecording
			{ //received KMMFEventCategoryPlaybackComplete event when not expecting it

			iNumberOfTimesPlayed = 0;

			// reset the position for subsequent plays
			iPosition = iPlayStart;	

			iState = CMdaAudioClipUtility::EOpen;
			}
		}
	else if (aEvent.iEventType==KMMFErrorCategoryControllerGeneralError)
			{
			TInt oldState = iState;
			iPosition = iPlayStart;
			iState = CMdaAudioClipUtility::EOpen;

			if(oldState==CMdaAudioClipUtility::EPlaying || oldState==CMdaAudioClipUtility::ERecording)
				{
				iCallback.MoscoStateChangeEvent(iParent, oldState, iState, aEvent.iErrorCode);
				}
			}
	// else we have an unexpected event that cannot be dealt with by the client.
	// We will simply ignore this.
	}

void CMMFMdaAudioRecorderUtility::PlaySilence()
	{
	// iRepeatTrailingSilenceTimer->After() takes a TTimeIntervalMicroSeconds32
	// so for longer periods of silence call it repeatedly with KMaxTInt lengths
	TTimeIntervalMicroSeconds32 silence;
	if (iTrailingSilenceLeftToPlay.Int64() > KMaxTInt)
		{
		silence = KMaxTInt;
		iTrailingSilenceLeftToPlay = iTrailingSilenceLeftToPlay.Int64() - KMaxTInt;
		}
	else
		{
		silence = I64INT(iTrailingSilenceLeftToPlay.Int64());
		iTrailingSilenceLeftToPlay = 0;
		}
	iRepeatTrailingSilenceTimer->After(silence);
	}

void CMMFMdaAudioRecorderUtility::RepeatTrailingSilenceTimerComplete()
	{
	if (iTrailingSilenceLeftToPlay.Int64() > 0)
		{
		PlaySilence();
		}
	else
		{
		// reset the position for subsequent plays
		iPosition = iPlayStart;
		DoPlay();
		}
	}


CMdaAudioClipUtility::TState CMMFMdaAudioRecorderUtility::State()
	{
	return iState;
	}

void CMMFMdaAudioRecorderUtility::Close()
	{
	CloseControllers();

	if (iFindAndOpenController)	
		iFindAndOpenController->Close();

	iState = CMdaAudioClipUtility::ENotReady;
	}

void CMMFMdaAudioRecorderUtility::DoPlay()
	{
	if (iPlayDataPathInitialised)
		{
		DoPlayContinue();
		}
	else
		{
		SetupPlaybackDataPath();
		}

	if(iEventHolder != KNullUid)
		{
		TInt err = iNotificationRegistrationCommands.RegisterAsClient(iEventHolder, iNotificationDataHolder);			
		iEventHolder = KNullUid;
		iNotificationDataHolder = KNullDesC8;
		if(err != KErrNone)
			{
			iAsyncCallback->CallBack(iParent, iState, iState, err);
			}
		}
	}

void CMMFMdaAudioRecorderUtility::DoPlayContinue()
	{
	__ASSERT_DEBUG(iPlayDataPathInitialised, Panic(EMMFMediaClientBadState));

	TInt err;
	if (iState == CMdaAudioClipUtility::EOpen || iState == CMdaAudioClipUtility::EPlaying)
		{
		err = iPlaybackController.Prime();
		if (err==KErrNone)
			{
			// make sure we don't set the position outside the play window -
			// but allow it to remain unchanged if it's within the window
			if (iPlayWindowSet == ESet && 
				(iPosition < iPlayStart || iPosition >= iPlayEnd))
				iPosition = iPlayStart;
			if( iPosition < 0)
				iPosition = 0;
			err=iPlaybackController.SetPosition(iPosition);
			if (err==KErrNone)
				{
				if (iPlayWindowSet == ESet)
					err=iAudioPlayControllerCustomCommands.SetPlaybackWindow(iPlayStart,iPlayEnd);
				else if (iPlayWindowSet == EClear)
					{
					err=iAudioPlayControllerCustomCommands.DeletePlaybackWindow();
					iPlayWindowSet = ENone;		// assume window will stay cleared
					}

				if (err==KErrNone)
					{
					err = iPlaybackController.Play();
					if (err==KErrNone)
						{
						if (iState != CMdaAudioClipUtility::EPlaying)
							{
							// There has been a change in state - callback
							TInt oldState = iState;
							iState = CMdaAudioClipUtility::EPlaying;
							iAsyncCallback->CallBack(iParent, oldState, iState, KErrNone);
							}
						return; // success
						}
					}
				}
			}
		}
	else
		err = KErrNotReady;

	// There has been an error
	iAsyncCallback->CallBack(iParent, iState, iState, err);
	}

void CMMFMdaAudioRecorderUtility::PlayL()
	{
	if(!iPlaybackControllerAvailable)
		User::Leave(KErrNotSupported);

	// if we're already playing, call the client's callback with KErrNotReady.
	// This is what the controller would do if we allowed the Play()
	// to propagate down. Need to do it here too (for consistency)
	// in case we're in a trailing silence period.
    if (iState == CMdaAudioClipUtility::EPlaying)
		{
		iAsyncCallback->CallBack(iParent, iState, iState, KErrNotReady);
		return;
		}

	// cancel the repeat timer in case the client has called Play()
	// without waiting for the previous play to complete
	iRepeatTrailingSilenceTimer->Cancel();	
	// Reset played count 
	iNumberOfTimesPlayed = 0;	
	if(iNumberOfTimesToRepeat>0 || iNumberOfTimesToRepeat == KMdaRepeatForever)
		{
		TInt err = iAudioPlayControllerSetRepeatsCommands.SetRepeats(iNumberOfTimesToRepeat, iTrailingSilence);
		if(err==KErrNone)
			{
			iNumberOfTimesToRepeat = 0;
			iTrailingSilence = 0;
			}
		//Controller not supporting setrepeats custom command is not a real error
		//we revert back to playerutility's loop play implementation in that case
		}
	DoPlay();
	}


void CMMFMdaAudioRecorderUtility::RecordL()
	{
	if(!iRecordControllerAvailable)
		User::Leave(KErrNotSupported);

	TInt err;
	if (iState == CMdaAudioClipUtility::EOpen)
		{
		err = iRecordController.Prime();
		if (err==KErrNone)
			{
			// add other settings
			// when recording, always append to an existing clip
			err=iRecordController.SetPosition(Duration());
			if (err==KErrNone)
				{
				err = iRecordController.Play();
				if (err==KErrNone)
					{
					// Success
					iState = CMdaAudioClipUtility::ERecording;
					iClipChanged = ETrue;
					iAsyncCallback->CallBack(iParent, CMdaAudioClipUtility::EOpen, iState, KErrNone);
					return;
					}
				else 
					{
					iState = CMdaAudioClipUtility::ERecording;
					iAsyncCallback->CallBack(iParent, CMdaAudioClipUtility::EOpen, iState, err);
					}
				}
			}
		}
	else
		err = KErrNotReady;

	// There has been an error
	iAsyncCallback->CallBack(iParent, iState, iState, err);
	}

void CMMFMdaAudioRecorderUtility::Stop()
	{
	// Stop the controller
	// NB : RMMFController::Stop() should never return an error, 
	// but theoretically there may be third-party controllers which do -
	// for whatever reason. To avoid code bloat, rather than 
	// returning an error for all subsequent calls to PlayL() etc
	// we're going to ignore the error here & allow the controller 
	// to report it's own errors. 
	if (iState == CMdaAudioClipUtility::EOpen)
		{
		// reset the position for subsequent plays
		iPosition = iPlayStart;	
		}
	if (iState == CMdaAudioClipUtility::ERecording)
		{
		iRecordController.Stop();
		// reset the position for subsequent plays
		iPosition = iPlayStart;	
		iState = CMdaAudioClipUtility::EOpen;
		}
	else if (iState == CMdaAudioClipUtility::EPlaying)
		{
		// cancel the repeat timer in case the client has called Stop()
		// during the trailing silence period
		iRepeatTrailingSilenceTimer->Cancel();	

		iPlaybackController.Stop();
		// reset the position for subsequent plays
		iPosition = iPlayStart;	
		iState = CMdaAudioClipUtility::EOpen;
		}
	}

void CMMFMdaAudioRecorderUtility::CropL(TBool aCropToEnd)
	{
	if(!iRecordControllerAvailable)
		User::Leave(KErrNotSupported);

	User::LeaveIfError(iRecordController.Prime());
	User::LeaveIfError(iRecordController.SetPosition(iPosition));
	User::LeaveIfError(iAudioRecordControllerCustomCommands.Crop(aCropToEnd));
	iRecordController.Stop();
	}

void CMMFMdaAudioRecorderUtility::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	// we need to remember the position 
	iPosition = aPosition;
	if (iState==CMdaAudioRecorderUtility::EPlaying)
		{	
		iPlaybackController.SetPosition(iPosition);
		}
	if (iState==CMdaAudioRecorderUtility::ERecording)
		{
		iRecordController.SetPosition(iPosition);
		}
	}

const TTimeIntervalMicroSeconds& CMMFMdaAudioRecorderUtility::Position()
	{
	// need to handle differently if playing or recording, versus stopped
	TInt error = KErrNone;
	TBool getPositionCalled = EFalse;
	TTimeIntervalMicroSeconds position = TTimeIntervalMicroSeconds(0);
	if (iState==CMdaAudioRecorderUtility::EPlaying)
		{
		error = iPlaybackController.GetPosition(position);
		getPositionCalled = ETrue;
		}
	else if (iState==CMdaAudioRecorderUtility::ERecording)
		{
		error = iRecordController.GetPosition(position);
		getPositionCalled = ETrue;
		}
	if (error==KErrNone && getPositionCalled)
		iPosition = position;
	return iPosition;
	}

const TTimeIntervalMicroSeconds& CMMFMdaAudioRecorderUtility::RecordTimeAvailable()
	{
	if(!iRecordControllerAvailable)
		{//Will still be initial value of be 0Usec
		return iRecordTimeAvailable;
		}

#ifdef _DEBUG
	TInt err = 
#endif // _DEBUG
	iAudioRecordControllerCustomCommands.GetRecordTimeAvailable(iRecordTimeAvailable);

	__ASSERT_DEBUG(err==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem)); 

	return iRecordTimeAvailable;
	}

const TTimeIntervalMicroSeconds& CMMFMdaAudioRecorderUtility::Duration()
	{
	TInt err = KErrNone;
	iDuration = 0;
	iDurationError = KErrNone;
	
	//Prefer duration from record controller
	if(iRecordControllerAvailable)
		{
		err = iRecordController.GetDuration(iDuration);
		}
	// Getting the duration from the record controller might
	// legitimately fail if the record controller and format we selected 
	// (based on matching the header data with the format's resource file)
	// turns out not to really support the clip once the format is instantiated.
	// E.g. opening a wav/mp3 file will result in selecting the standard WAV format 
	// plugin, but when it is instantiated it will not recognize the format.
	// If this happens, try using the playback controller
	if ((!iRecordControllerAvailable || err != KErrNone ) && iPlaybackControllerAvailable)
		{
		err = iPlaybackController.GetDuration(iDuration);
		}
		
	iDurationError = err;
	
	return iDuration;
	}

/**
Returns the duration of the audio sample in microseconds, and the duration state.

The function raises a CMdaAudioPlayerUtility 1 panic if the audio
player utility is not initialised.

@param aDuration
	   The duration of the sample in microseconds.
@return The duration state

@since	9.1
*/	
TMMFDurationInfo CMMFMdaAudioRecorderUtility::Duration(TTimeIntervalMicroSeconds& aDuration)
	{
	TPckgBuf<TMMFDurationInfo> pckg;
	TMMFDurationInfo result = EMMFDurationInfoValid;
	TInt err = KErrNone;
	
	TMMFMessageDestinationPckg iDestinationPckg(TMMFMessageDestination(KUidInterfaceMMFDurationInfoControl, KMMFObjectHandleController));
	
		
	err = iRecordController.CustomCommandSync(iDestinationPckg,
										 	 EMMFGetDurationInfo,
										 	 KNullDesC8,
										 	 KNullDesC8,
										     pckg );
										     
	// Getting the duration information from the record controller might
	// legitimately fail if the record controller and format we selected 
	// (based on matching the header data with the format's resource file)
	// turns out not to really support the clip once the format is instantiated.
	// E.g. opening a wav/mp3 file will result in selecting the standard WAV format 
	// plugin, but when it is instantiated it will not recognize the format.
	// If this happens, try using the playback controller
	if ((!iRecordControllerAvailable || err != KErrNone ) && iPlaybackControllerAvailable && (err != KErrNotSupported) )
		{
		err = iPlaybackController.CustomCommandSync(iDestinationPckg,
											 	 	EMMFGetDurationInfo,
													KNullDesC8,
													KNullDesC8,
													pckg );
		}
																 
	switch ( err )
		{
		case KErrNone:
			result = pckg();
			break;
		
		case KErrNotSupported:
			// Custom command not implemented return EMMFDurationInfoValid as the default value
			break;
		
		default:
			// Unknown error
			result = EMMFDurationInfoUnknown;
			break;
		}

	// Get the duration information to return in aDuration
	aDuration = Duration();
	return result;
	}

void CMMFMdaAudioRecorderUtility::SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd)
	{
	if(!iPlaybackControllerAvailable)
		{
		return;
		}

	iPlayStart = aStart;
	iPlayEnd = aEnd;
	iPlayWindowSet = ESet;
	if (iState==CMdaAudioRecorderUtility::EPlaying) 
		{
#ifdef _DEBUG
		TInt error = 
#endif
			iAudioPlayControllerCustomCommands.SetPlaybackWindow(aStart, aEnd);
		__ASSERT_DEBUG(error==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem)); 
		}
	}

void CMMFMdaAudioRecorderUtility::ClearPlayWindow()
	{
	if(!iPlaybackControllerAvailable)
		{
		return;
		}

	// clear play window start - very important because this is assigned 
	// to iPosition when we stop & is used to set the position on the next PlayL()
	iPosition = iPlayStart = iPlayEnd = TTimeIntervalMicroSeconds(0);

	iPlayWindowSet = EClear;
	if (iState==CMdaAudioRecorderUtility::EPlaying)
		{
#ifdef _DEBUG
		TInt error = 
#endif
			iAudioPlayControllerCustomCommands.DeletePlaybackWindow();
		__ASSERT_DEBUG(error==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem)); 
		}
	}

void CMMFMdaAudioRecorderUtility::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	TInt err = iAudioPlayControllerSetRepeatsCommands.SetRepeats(aRepeatNumberOfTimes, aTrailingSilence);
	if(err!=KErrNone)
		{
		iNumberOfTimesToRepeat = aRepeatNumberOfTimes;
		iTrailingSilence = aTrailingSilence;
		}
	}

void CMMFMdaAudioRecorderUtility::SetMaxWriteLength(TInt aMaxWriteLength)
	{
	if(!iRecordControllerAvailable)
		{
		return;
		}

#ifdef _DEBUG
	TInt err = 
#endif // _DEBUG
	iAudioRecordControllerCustomCommands.SetMaxFileSize(aMaxWriteLength);

	ASSERT(err == KErrNone);	// error calling SetMaxFileSize
	}

void CMMFMdaAudioRecorderUtility::SetPriority(TInt aPriority, TInt aPref)
	{
	iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;
	iFindAndOpenController->Configure(iMediaIds[0], iPrioritySettings);
	
	if(iPlaybackControllerAvailable)
		{
		iPlaybackController.SetPrioritySettings(iPrioritySettings);
		}
	if(iRecordControllerAvailable)
		{
		iRecordController.SetPrioritySettings(iPrioritySettings);
		}
	}

void CMMFMdaAudioRecorderUtility::CropL(const TTimeIntervalMicroSeconds& aStart,
											  const TTimeIntervalMicroSeconds& aEnd)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	User::LeaveIfError(iRecordController.Prime());
	User::LeaveIfError(iRecordController.SetPosition(aEnd));
	User::LeaveIfError(iAudioRecordControllerCustomCommands.Crop(ETrue));
	User::LeaveIfError(iRecordController.SetPosition(aStart));
	User::LeaveIfError(iAudioRecordControllerCustomCommands.Crop(EFalse));
	iRecordController.Stop();
	}

TInt CMMFMdaAudioRecorderUtility::GetGain(TInt& aGain) 
	{
	if(!iRecordControllerAvailable)
		{
		return KErrNotSupported;
		}

	TInt error;
	error = iAudioRecordDeviceCustomCommands.GetGain(aGain);
	return error;
	}

TInt CMMFMdaAudioRecorderUtility::GetNumberOfMetaDataEntries(TInt& aNumEntries) 
	{
	if(!iRecordControllerAvailable && !iPlaybackControllerAvailable)
		{
		return KErrNotSupported;
		}
	if(iRecordControllerAvailable)
		return iRecordController.GetNumberOfMetaDataEntries(aNumEntries);
	else
		return iPlaybackController.GetNumberOfMetaDataEntries(aNumEntries);

	}

CMMFMetaDataEntry* CMMFMdaAudioRecorderUtility::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
	if(!iRecordControllerAvailable && !iPlaybackControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	if(iRecordControllerAvailable)
		return iRecordController.GetMetaDataEntryL(aMetaDataIndex);
	else
		return iPlaybackController.GetMetaDataEntryL(aMetaDataIndex);

	}

void CMMFMdaAudioRecorderUtility::AddMetaDataEntryL(CMMFMetaDataEntry& aMetaDataEntry)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	iAudioRecordControllerCustomCommands.AddMetaDataEntryL(aMetaDataEntry);
	}

TInt CMMFMdaAudioRecorderUtility::RemoveMetaDataEntry(TInt aMetaDataIndex)
	{
	if(!iRecordControllerAvailable)
		{
		return KErrNotSupported;
		}
	return iAudioRecordControllerCustomCommands.RemoveMetaDataEntry(aMetaDataIndex);
	}

void CMMFMdaAudioRecorderUtility::ReplaceMetaDataEntryL(TInt aMetaDataIndex,
														CMMFMetaDataEntry& aMetaDataEntry)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	iAudioRecordControllerCustomCommands.ReplaceMetaDataEntryL(aMetaDataIndex, 
																	aMetaDataEntry);
	}

TInt CMMFMdaAudioRecorderUtility::SetPlaybackBalance(TInt aBalance)
	{
	if(!iPlaybackControllerAvailable)
		{
		return KErrNotSupported;
		}
	return iAudioPlayDeviceCustomCommands.SetBalance(aBalance);
	
	}

TInt CMMFMdaAudioRecorderUtility::GetPlaybackBalance(TInt& aBalance)
	{
	if(!iPlaybackControllerAvailable)
		{
		return KErrNotSupported;
		}
	return iAudioPlayDeviceCustomCommands.GetBalance(aBalance);		
	}

TInt CMMFMdaAudioRecorderUtility::SetRecordBalance(TInt aBalance)
	{
	if(!iRecordControllerAvailable)
		{
		return KErrNotSupported;
		}
	return iAudioRecordDeviceCustomCommands.SetBalance(aBalance);
	}

TInt CMMFMdaAudioRecorderUtility::GetRecordBalance(TInt& aBalance)
	{
	if(!iRecordControllerAvailable)
		{
		return KErrNotSupported;
		}
	return iAudioRecordDeviceCustomCommands.GetBalance(aBalance);
	}


void CMMFMdaAudioRecorderUtility::GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDataTypes)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	iAudioControllerRecCustomCommands.GetSupportedSinkDataTypesL(aSupportedDataTypes);
	}

void CMMFMdaAudioRecorderUtility::SetDestinationDataTypeL(TFourCC aDataType)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	User::LeaveIfError(iAudioControllerRecCustomCommands.SetSinkDataType(aDataType));
	}

TFourCC CMMFMdaAudioRecorderUtility::DestinationDataTypeL()
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	TFourCC dataType;
	User::LeaveIfError(iAudioControllerRecCustomCommands.GetSinkDataType(dataType));
	return dataType;
	}

void CMMFMdaAudioRecorderUtility::SetDestinationBitRateL(TUint aBitRate)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	User::LeaveIfError(iAudioControllerRecCustomCommands.SetSinkBitRate(aBitRate));
	}

void CMMFMdaAudioRecorderUtility::SetDestinationSampleRateL(TUint aSampleRate)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	User::LeaveIfError(iAudioControllerRecCustomCommands.SetSinkSampleRate(aSampleRate));
	}

void CMMFMdaAudioRecorderUtility::SetDestinationFormatL(TUid aRecordFormat)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	User::LeaveIfError(iAudioControllerRecCustomCommands.SetSinkFormat(aRecordFormat));
	}

void CMMFMdaAudioRecorderUtility::SetDestinationNumberOfChannelsL(TUint aNumberOfChannels)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	User::LeaveIfError(iAudioControllerRecCustomCommands.SetSinkNumChannels(aNumberOfChannels));
	}

TUint CMMFMdaAudioRecorderUtility::DestinationBitRateL()
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	TUint bitRate;
	User::LeaveIfError(iAudioControllerRecCustomCommands.GetSinkBitRate(bitRate));
	return bitRate;
	}

TUint CMMFMdaAudioRecorderUtility::DestinationSampleRateL()
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	TUint sampleRate;
	User::LeaveIfError(iAudioControllerRecCustomCommands.GetSinkSampleRate(sampleRate));
	return sampleRate;
	}

TUid CMMFMdaAudioRecorderUtility::DestinationFormatL()
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	TUid format;
	User::LeaveIfError(iAudioControllerRecCustomCommands.GetSinkFormat(format));
	return format;
	}

TUint CMMFMdaAudioRecorderUtility::DestinationNumberOfChannelsL()
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	TUint numChannels;
	User::LeaveIfError(iAudioControllerRecCustomCommands.GetSinkNumChannels(numChannels));
	return numChannels;
	}

void CMMFMdaAudioRecorderUtility::GetSupportedBitRatesL(RArray<TUint>& aSupportedBitRates)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	iAudioControllerRecCustomCommands.GetSupportedSinkBitRatesL(aSupportedBitRates);
	}

void CMMFMdaAudioRecorderUtility::GetSupportedSampleRatesL(RArray<TUint>& aSupportedSampleRates)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	iAudioControllerRecCustomCommands.GetSupportedSinkSampleRatesL(aSupportedSampleRates);
	}

void CMMFMdaAudioRecorderUtility::GetSupportedNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels)
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	iAudioControllerRecCustomCommands.GetSupportedSinkNumChannelsL(aSupportedNumChannels);
	}

/**
Returns source bit rate.

@return	Bit rate of data source in bits/second

@leave  KErrNotSupported
		Leaves with KErrNotSupported error, if no controllers are found

@since	7.0s
*/		
TUint CMMFMdaAudioRecorderUtility::SourceBitRateL()
	{
	if(!iPlaybackControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	TUint bitRate;
	User::LeaveIfError(iAudioControllerPlyCustomCommands.GetSourceBitRate(bitRate));
	return bitRate;
	}

void CMMFMdaAudioRecorderUtility::Reset() 
	{
	Close();
	// reset all state variables
	iPlayWindowSet = ENone;
	iPlayFormatUid = KNullUid;
	iPlayDataPathInitialised = EFalse;
	iPlayDataType = KMMFFourCCCodeNULL;
	iPlayStart = TTimeIntervalMicroSeconds(0);
	iPlayEnd = TTimeIntervalMicroSeconds(0);
	iPosition = TTimeIntervalMicroSeconds(0);
	iDuration = TTimeIntervalMicroSeconds(0);
	iRecordTimeAvailable = TTimeIntervalMicroSeconds(0);
	iNumberOfTimesToRepeat = 0;
	iTrailingSilence = TTimeIntervalMicroSeconds(0);
	iClientCallbackPending = ETrue;
	iRecordFormatUid = KNullUid;
	iPlayFormatUid = KNullUid;
	iRecordSourceDataType = KMMFFourCCCodeNULL;
	iRecordSinkDataType = KMMFFourCCCodeNULL;
	iPlaySampleRate = 0;
	iPlayChannels = 0;
	iPlaybackControllerUidNotSupplied = EFalse;
	}

TInt CMMFMdaAudioRecorderUtility::RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData)
	{
	iAudioResourceNotificationCallBack = &aCallback;
	TInt err = iNotificationRegistrationCommands.RegisterAsClient(aNotificationEventUid,aNotificationRegistrationData);
	if(err == KErrNotReady)
		{
		iEventHolder = 	aNotificationEventUid;
		iNotificationDataHolder = aNotificationRegistrationData;
		return KErrNone;
		}
	iEventHolder = KNullUid;
	iNotificationDataHolder = KNullDesC8;
	return err;
	}

TInt CMMFMdaAudioRecorderUtility::CancelRegisterAudioResourceNotification(TUid aNotificationEventId)
	{
	TInt err = iNotificationRegistrationCommands.CancelRegisterAsClient(aNotificationEventId);
	if(err == KErrNotReady)
		{
		if(aNotificationEventId != KMMFEventCategoryAudioResourceAvailable)	
			{
			return KErrNotSupported;
			}
		if(iEventHolder == KNullUid)	
			{
			return KErrCancel;
			}	
		iEventHolder = KNullUid;
		iNotificationDataHolder = KNullDesC8;
		return KErrNone;
		}
	return err;
	}

TInt CMMFMdaAudioRecorderUtility::WillResumePlay()
	{
	return iNotificationRegistrationCommands.WillResumePlay();
	}

// CMdaAudioRecorderUtility

/**
Returns the current state of the audio sample data.

@return The state of the audio sample data. 

@since 5.0
*/
CMdaAudioRecorderUtility::TState CMdaAudioRecorderUtility::State()
	{
	ASSERT(iProperties);
	return iProperties->State();
	}

/**
Close the current audio clip.

@since 5.0
*/
void CMdaAudioRecorderUtility::Close()
	{
	ASSERT(iProperties);
	iProperties->Close();
	}

/**
Begins playback of audio sample data at the current playback position using the current volume,
gain and priority settings.

@since 5.0
*/
void CMdaAudioRecorderUtility::PlayL()
	{
	ASSERT(iProperties);
	iProperties->PlayL();
	}

/**
Starts appending new audio sample data.

The new audio sample data is appended to the existing audio sample data in the same format 
as the existing audio sample data. If existing data is to be overwritten, then it should 
be cropped to the appropriate length before appending the new data.

Note:
Before starting to record, make sure that the gain is set by calling SetGain(), as the initial
gain is undefined. The gain might also have been modified by another client application.

@since 5.0

@capability	UserEnvironment
			For recording - the requesting client process must have the 
			UserEnvironment capability.
*/
void CMdaAudioRecorderUtility::RecordL()
	{
	ASSERT(iProperties);
	iProperties->RecordL();
	}

/**
Stops the current operation (playback/recording/conversion).

The operation is stopped at the current location. For a playback operation, the head is positioned 
at the last played data.

@since 5.0
*/
void CMdaAudioRecorderUtility::Stop()
	{
	ASSERT(iProperties);
	iProperties->Stop();
	}

/**
Deletes all audio sample data after the current head position. The effects of the function cannot be undone.

The function is synchronous and can leave if there is a problem. The leave code depends on the configuration.

@since 5.0
*/
void CMdaAudioRecorderUtility::CropL()
	{
	ASSERT(iProperties);
	iProperties->CropL(ETrue);
	}

/**
Sets the head position.

The playback head is moved to a position which is defined in terms of a time interval measured from the beginning of the audio sample data. A subsequent call to PlayL() starts from this new position.
This method's behaviour is undefined and dependent on the controller in use if it is called in the playing state and when a play window is set.
@param  aPosition
        The new head position measured in microseconds. 

@since 5.0
*/
void CMdaAudioRecorderUtility::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	ASSERT(iProperties);
	iProperties->SetPosition(aPosition);
	}

/**
Returns the current head position.

The head position is defined in terms of a time interval measured from the beginning of the audio sample data.

@return The current position in the audio clip, in microseconds.

@since 5.0
*/
const TTimeIntervalMicroSeconds& CMdaAudioRecorderUtility::Position()
	{
	ASSERT(iProperties);
	return iProperties->Position();
	}

/**
Returns the recording time available for the selected file or descriptor and encoding format.

@return The recording time available measured in microseconds. 

@since 5.0
*/
const TTimeIntervalMicroSeconds& CMdaAudioRecorderUtility::RecordTimeAvailable()
	{
	ASSERT(iProperties);
	return iProperties->RecordTimeAvailable();
	}

/**
Returns the duration of the audio clip.

@return The duration in microseconds.

@since 5.0
*/
const TTimeIntervalMicroSeconds& CMdaAudioRecorderUtility::Duration()
	{
	ASSERT(iProperties);
	return iProperties->Duration();
	}

/**
Returns the duration of the audio sample in microseconds, and the duration state.

The function raises a CMdaAudioPlayerUtility 1 panic if the audio
player utility is not initialised.

@param aDuration
	   The duration of the sample in microseconds.
@return The duration state

@since	9.1
*/	
EXPORT_C TMMFDurationInfo CMdaAudioRecorderUtility::Duration(TTimeIntervalMicroSeconds& aDuration)
	{
	ASSERT(iProperties);
	return iProperties->Duration(aDuration);
	}

/**
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
        The position defining the end of the window, measured in microseconds. If this 
        value is greater than the value returned by Duration(), it is set to the value of 
        Duration(). If this value is less than aStart, then it is swapped with aStart.

@since 5.0
*/
void CMdaAudioRecorderUtility::SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd)
	{
	ASSERT(iProperties);
	iProperties->SetPlayWindow(aStart, aEnd);
	}

/**
Removes any window defined on the audio sample data.

All subsequent playing, position setting and position querying requests are mapped to the whole of the
audio sample data.

@since 5.0
*/
void CMdaAudioRecorderUtility::ClearPlayWindow()
	{
	ASSERT(iProperties);
	iProperties->ClearPlayWindow();
	}

/**
Sets the number of times the audio sample is to be repeated during the PlayL() operation.

A period of silence can follow each playing of the audio sample. The audio sample can 
be repeated indefinitely or until stopped.

@param  aRepeatNumberOfTimes
        The number of times the audio sample, together with the trailing silence, is to be
        repeated. If this is set to KMdaRepeatForever, then the audio sample, together with 
        the trailing silence, is repeated indefinitely. If this is set to zero, then the audio 
        sample is not repeated. The behaviour is undefined for negative values (other than
		KMdaRepeatForever).
@param  aTrailingSilence
        The length of the trailing silence in microseconds. 
		Negative values will produce a panic USER 87.

@since 5.0
*/
void CMdaAudioRecorderUtility::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	ASSERT(iProperties);
	iProperties->SetRepeats(aRepeatNumberOfTimes, aTrailingSilence);
	}

/**
Sets the maximum size for a file that is being recorded.

When this limit is reached, MMF stops recording and notifies the client application. Notification is 
caused by MMdaObjectStateChangeObserver::MoscoStateChangeEvent() with the error code KErrEof.

This function is provided so that applications such as recorders can limit the amount of file 
storage/memory that should be allocated.

@param  aMaxWriteLength
        The maximum file size in bytes. If the default value is used, there is no maximum file size.

@since 7.0
*/
void CMdaAudioRecorderUtility::SetMaxWriteLength(TInt aMaxWriteLength /* = KMdaClipLocationMaxWriteLengthNone*/)
	{
	ASSERT(iProperties);
	iProperties->SetMaxWriteLength(aMaxWriteLength);
	}

/**
Deletes all audio sample data from the beginning of the audio clip to the current position. 
The effects of the function cannot be undone.

The function is synchronous and can leave if there is a problem. The leave code depends on 
the configuration.

@since 7.0s
*/
void CMdaAudioRecorderUtility::CropFromBeginningL()
	{
	ASSERT(iProperties);
	iProperties->CropL(EFalse);
	}
	
const CMMFControllerImplementationInformation& CMMFMdaAudioRecorderUtility::AudioPlayControllerImplementationInformationL()
	{
	if(!iPlaybackControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	if (!iPlayControllerImplementationInformation)
		{
		if (iPlayControllerUid==KNullUid)
			User::Leave(KErrNotReady);
		iPlayControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(iPlayControllerUid);
		}
	return *iPlayControllerImplementationInformation;
	}

const CMMFControllerImplementationInformation& CMMFMdaAudioRecorderUtility::AudioRecorderControllerImplementationInformationL()
	{
	if(!iRecordControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}

	if (!iRecordControllerImplementationInformation)
		{
		if (iRecordControllerUid==KNullUid)
			User::Leave(KErrNotReady);
		iRecordControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(iRecordControllerUid);
		}
	return *iRecordControllerImplementationInformation;
	}
	
void CMMFMdaAudioRecorderUtility::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aLoadingObserver)
	{
	iLoadingObserver = &aLoadingObserver;
	}
	
void CMMFMdaAudioRecorderUtility::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
	if(!iPlaybackControllerAvailable)
		{
		User::Leave(KErrNotSupported);
		}
	User::LeaveIfError(iAudioPlayControllerCustomCommands.GetLoadingProgress(aPercentageProgress));
	}


TInt CMMFMdaAudioRecorderUtility::RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	if(!iRecordControllerAvailable)
		{
		return KErrNotSupported;
		}
	
	return iRecordController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
	
TInt CMMFMdaAudioRecorderUtility::RecordControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	if(!iRecordControllerAvailable)
		{
		return KErrNotSupported;
		}

	return iRecordController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
	
void CMMFMdaAudioRecorderUtility::RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	if(!iRecordControllerAvailable)
		{
		TRequestStatus* s = &aStatus;
		User::RequestComplete(s, KErrNotSupported);
		return;
		}
	iRecordController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
	
void CMMFMdaAudioRecorderUtility::RecordControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	if(!iRecordControllerAvailable)
		{
		TRequestStatus* s = &aStatus;
		User::RequestComplete(s, KErrNotSupported);
		return;
		}
	iRecordController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}
	
TInt CMMFMdaAudioRecorderUtility::PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	if(!iPlaybackControllerAvailable)
		{
		return KErrNotSupported;
		}

	return iPlaybackController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
	
TInt CMMFMdaAudioRecorderUtility::PlayControllerCustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	if(!iPlaybackControllerAvailable)
		{
		return KErrNotSupported;
		}
	return iPlaybackController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
	
void CMMFMdaAudioRecorderUtility::PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	if(!iPlaybackControllerAvailable)
		{
		TRequestStatus* s = &aStatus;
		User::RequestComplete(s, KErrNotSupported);
		return;
		}
	iPlaybackController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
	
void CMMFMdaAudioRecorderUtility::PlayControllerCustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	if(!iPlaybackControllerAvailable)
		{
		TRequestStatus* s = &aStatus;
		User::RequestComplete(s, KErrNotSupported);
		return;
		}
	iPlaybackController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

void CMMFMdaAudioRecorderUtility::OpenFileL(const RFile& aFile)
	{
	// Make sure we are closed
	Reset();

	RFile& file = const_cast<RFile&>(aFile);
	TMMFileHandleSource source(file, KDefaultContentObject, EPlay);
	iFindAndOpenController->ConfigureSourceSink(
		source,
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
	iFindAndOpenController->ConfigureSecondarySourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput),
		CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, aFile)); 
		
	iFindAndOpenController->OpenByFileSource(source);
	}

void CMMFMdaAudioRecorderUtility::OpenFileL(const RFile& aFile,	
						TUid aControllerUid,
						TUid aPlaybackControllerUid,
						TUid aFormatUid,		
						TFourCC aDataType)
						
	{
	Reset();

	iRecordFormatUid = aFormatUid;
	iPlayFormatUid = aFormatUid;
	iRecordSinkDataType = aDataType;

	TInt err = KErrNone;
	if((aControllerUid == KNullUid) && (aPlaybackControllerUid == KNullUid))
		{
		err = KErrNotSupported;
		}
	else
		{
		//If aPlayController is KNullUid, then assume that they want to use the
		//record controller for playback. However a failure to open (KErrNotFound) the 
		//play controller under this assumption is not reported back.

		if(aPlaybackControllerUid == KNullUid)
			{
			iPlaybackControllerUidNotSupplied = ETrue;
			aPlaybackControllerUid = aControllerUid;
			}

		RFile& file = const_cast<RFile&>(aFile);
		TMMFileHandleSource source(file, KDefaultContentObject, EPlay);

		iFindAndOpenController->ConfigureSourceSink(
			source,
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
		
		if (aControllerUid != KNullUid)
			{
			iFindAndOpenController->ConfigureSecondarySourceSink(
				CMMFFindAndOpenController::TSourceSink(KUidMmfAudioInput), 
				CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, aFile));
			}
		
		iFindAndOpenController->OpenByControllerUid(aPlaybackControllerUid, aControllerUid);
		}
	
	if (err != KErrNone)
		{
		iAsyncCallback->CallBack(iParent, iState, iState, err);
		}
	}

MMMFDRMCustomCommand* CMMFMdaAudioRecorderUtility::GetDRMCustomCommand()
	{
	if (iDRMCustomCommands.IsSupported())
		{
		return static_cast<MMMFDRMCustomCommand*>(&iDRMCustomCommands);
		}
		
	return NULL;
	}
	
TInt CMMFMdaAudioRecorderUtility::SetThreadPriorityPlayback(const TThreadPriority& aThreadPriority) const
	{
	return iPlaybackController.SetThreadPriority(aThreadPriority);
	}

TInt CMMFMdaAudioRecorderUtility::SetThreadPriorityRecord(const TThreadPriority& aThreadPriority) const
	{
	return iRecordController.SetThreadPriority(aThreadPriority);
	}
