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

#include <bautils.h>
#include <utf.h>
#include <mmf/common/mmfpaniccodes.h>
#include "mmfclientaudioplayer.h"
#include "mmfclientutility.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfdurationinfocustomcommandsimpl.h>
#include <mmf/common/mmfdurationinfocustomcommandsenums.h>
#endif

using namespace ContentAccess;

// declared in the recorder module
void Panic(TInt aPanicCode);

/**
Constructs and initialises a new instance of the audio player utility.

The function leaves if the audio player utility object cannot be created.

No callback notification is made upon completion of NewL().

@param  aCallback
        The audio player observer interface.
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.

@return A pointer to the new audio player utility object.

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMdaAudioPlayerUtility* CMdaAudioPlayerUtility::NewL(MMdaAudioPlayerCallback& aCallback,
															  TInt aPriority,
															  TInt aPref)
	{
	CMdaAudioPlayerUtility* self = new(ELeave) CMdaAudioPlayerUtility();
	CleanupStack::PushL(self);
	self->iProperties = CMMFMdaAudioPlayerUtility::NewL(aCallback, aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructs and initialises a new instance of the audio player utility for playing sampled audio data 
from a file. The audio data must be in a supported format (e.g. WAV and AU).

The function leaves if the audio player utility object cannot be created.

When initialisation of the audio player utility is complete, successfully or otherwise, the callback 
function MMdaAudioPlayerCallback::MapcInitComplete() is called.

@param  aFileName 
        The full path name of the file containing the audio data.
@param  aCallback 
        The audio player observer interface.
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.
@param  aServer
        Not used in 7.0s. This parameter is provided for binary compatibility with previous versions.

@return A pointer to the new audio player utility object.

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMdaAudioPlayerUtility* CMdaAudioPlayerUtility::NewFilePlayerL(const TDesC& aFileName,
																		MMdaAudioPlayerCallback& aCallback,
																		TInt aPriority,
																		TInt aPref,
																		CMdaServer* /*aServer*/)
	{
	CMdaAudioPlayerUtility* self = new(ELeave) CMdaAudioPlayerUtility();
	CleanupStack::PushL(self);
	self->iProperties = CMMFMdaAudioPlayerUtility::NewFilePlayerL(aFileName, aCallback, aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructs and initialises a new instance of the audio player utility for playing sampled audio data 
from a descriptor.

The audio data must be in a supported format (e.g. WAV and AU).

The function leaves if the audio player utility object cannot be created. When initialisation of the 
audio player utility is complete, successfully or otherwise, the callback function 
MMdaAudioPlayerCallback::MapcInitComplete() is called.

@param  aData 
        A descriptor containing the audio data. This descriptor must remain in existence for the 
        lifetime of this audio player utility object.
@param  aCallback 
        The audio player observer interface.
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.
@param  aServer
        Not used in 7.0s. This parameter is provided for binary compatibility with previous versions.

@return A pointer to the new audio player utility object.

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMdaAudioPlayerUtility* CMdaAudioPlayerUtility::NewDesPlayerL(const TDesC8& aData, MMdaAudioPlayerCallback& aCallback, TInt aPriority, TInt aPref, CMdaServer* /*aServer*/)
	{
	CMdaAudioPlayerUtility* self = new(ELeave) CMdaAudioPlayerUtility();
	CleanupStack::PushL(self);
	self->iProperties = CMMFMdaAudioPlayerUtility::NewDesPlayerL(aData, aCallback, aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructs and initialises a new instance of the audio player utility for playing sampled audio data 
from a read only descriptor.

The audio data must be in a supported format (e.g. WAV and AU).

The function leaves if the audio player utility object cannot be created. When initialisation of 
the audio player utility is complete, successfully or otherwise, the callback function 
MMdaAudioPlayerCallback::MapcInitComplete() is called.

@param  aData 
        A read only descriptor containing the audio data. This descriptor must remain in existence 
        for the lifetime of this audio player utility object.
@param  aCallback 
        The audio player observer interface.
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.
@param  aServer
        Not used in 7.0s. This parameter is provided for binary compatibility with previous versions.

@return A pointer to a new audio player utility.

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMdaAudioPlayerUtility* CMdaAudioPlayerUtility::NewDesPlayerReadOnlyL(const TDesC8& aData, MMdaAudioPlayerCallback& aCallback, TInt aPriority, TInt aPref, CMdaServer* /*aServer*/)
	{
	CMdaAudioPlayerUtility* self = new(ELeave) CMdaAudioPlayerUtility();
	CleanupStack::PushL(self);
	self->iProperties = CMMFMdaAudioPlayerUtility::NewDesPlayerReadOnlyL(aData, aCallback, aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}

CMdaAudioPlayerUtility::CMdaAudioPlayerUtility()
	{
	}

/**
Destructor.

Frees all resources owned by the object prior to its destruction.
*/
CMdaAudioPlayerUtility::~CMdaAudioPlayerUtility()
	{
	delete iProperties;
	}

/**
Ensures that any subsequent calls to OpenXYZ() will create controllers that
share a heap.

The default behaviour is that for each player utility a controller with its own heap
is created. Each heap uses a chunk, so using this function avoids situations where 
the number of chunks per process is limited.
The default behaviour is generally to be preferred, and should give lower overall
memory usage. However, if many controllers are to be created for a particular thread,
then this function should be used to prevent running out of heaps or chunks.

@since	9.1
*/
EXPORT_C void CMdaAudioPlayerUtility::UseSharedHeap()
	{
	ASSERT(iProperties);
	iProperties->UseSharedHeap();
	}

// 5.0 functions

/**
Begins playback of audio sample data at the current playback position using the current volume,
gain and priority settings.

When playing of the audio sample is complete, successfully or
otherwise, the callback function
MMdaAudioPlayerCallback::MapcPlayComplete() is
called.

If this function is called whilst already playing then 
MMdaAudioPlayerCallback::MapcPlayComplete will return with the
error code KErrNotReady.

@since	5.0
*/
void CMdaAudioPlayerUtility::Play()
	{
	ASSERT(iProperties);
	iProperties->Play();
	}

/**
Stops playback of the audio sample as soon as possible.

If the audio sample is playing, playback is stopped as soon as
possible. If playback is already complete, nothing further happens as
a result of calling this function. The callback function
MMdaAudioPlayerCallback::MapcPlayComplete() is not
called.

@since	5.0
*/
void CMdaAudioPlayerUtility::Stop()
	{
	ASSERT(iProperties);
	iProperties->Stop();
	}


/**
Changes the current playback volume to a specified value.

The volume can be changed before or during playback and is effective
immediately. The volume can be set to any value between zero (mute) and 
the maximum permissible volume (determined using MaxVolume()).

@param  aVolume
        The volume setting. This can be any value from zero to
        the value returned by a call to
        CMdaAudioPlayerUtility::MaxVolume().
        Setting a zero value mutes the sound. Setting the maximum
        value results in the loudest possible sound. Values less 
        than zero would be set to zero and the values greater than 
        the maximum permitted volume would be set to the maximum volume.
@return An error code indicating if the function call was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
@panic  EMMFMediaClientBadArgument is raised when the audio player utility is not initialised.

@since	5.0
*/
TInt CMdaAudioPlayerUtility::SetVolume(TInt aVolume)
	{
	ASSERT(iProperties);
	return iProperties->SetVolume(aVolume);
	}

/**
Sets the number of times the audio sample is to be repeated during the
playback operation.

A period of silence can follow each playing of the sample. The audio
sample can be repeated indefinitely.

@param   aRepeatNumberOfTimes
         The number of times the audio sample, together with
         the trailing silence, is to be repeated. If this is
         set to KMdaRepeatForever, then the audio
         sample, together with the trailing silence, is
         repeated indefinitely or until Stop() is
         called. If this is set to zero, then the audio sample
         is not repeated.
@param   aTrailingSilence
         The time interval of the trailing silence in microseconds.

@since	5.0
*/
void CMdaAudioPlayerUtility::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	ASSERT(iProperties);
	iProperties->SetRepeats(aRepeatNumberOfTimes, aTrailingSilence);
	}

/**
Defines the period over which the volume level is to rise smoothly
from nothing to the normal volume level.

@param  aRampDuration
        The period over which the volume is to rise. A zero
        value causes the audio sample to be played at the
        normal level for the full duration of the playback. A
        value which is longer than the duration of the audio
        sample means that the sample never reaches its normal
        volume level.

@since	5.0
*/
void CMdaAudioPlayerUtility::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	ASSERT(iProperties);
	iProperties->SetVolumeRamp(aRampDuration);
	}

/**
Returns the duration of the audio sample in microseconds.

@return The duration of the sample in microseconds.

@since	5.0
*/
const TTimeIntervalMicroSeconds& CMdaAudioPlayerUtility::Duration()
	{
	ASSERT(iProperties);
	return iProperties->Duration();
	}

/**
Returns the duration of the audio sample in microseconds, and the duration state.

@param aDuration
	   The duration of the sample in microseconds.
@return The duration state

@since	9.1
*/	
EXPORT_C TMMFDurationInfo CMdaAudioPlayerUtility::Duration(TTimeIntervalMicroSeconds& aDuration)
{
	ASSERT(iProperties);
	return iProperties->Duration(aDuration);
}

/**
Returns an integer representing the maximum volume.

This is the maximum value which can be passed to
CMdaAudioPlayerUtility::SetVolume(). This value is platform 
independent, but is always greater than or equal to one.

@return The maximum volume setting.
@panic  EMMFMediaClientPanicServerCommunicationProblem is raised when the audio player utility is not initialised. 

@since	5.0
*/
TInt CMdaAudioPlayerUtility::MaxVolume()
	{
	ASSERT(iProperties);
	return iProperties->MaxVolume();
	}

// 7.0s functions

/**
Opens an audio clip from a file.

The audio data must be in a supported format (for example, WAV or AU).

This function leaves with KErrNotReady if there is a previous open statement awaiting notification of completion.

@param  aFileName
        The file to open.
@leave  KErrNotReady
        If a previous open statement is awaiting notification of completion.
		opening the file
@since	7.0s
*/
EXPORT_C void CMdaAudioPlayerUtility::OpenFileL(const TDesC& aFileName)
	{
	ASSERT(iProperties);
	iProperties->OpenFileL(aFileName);
	}
	
/**
Opens an audio clip from a file.

The audio data must be in a supported format (for example, WAV or AU).

This function leaves with KErrNotReady if there is a previous open statement awaiting notification of completion.

Note: it is generally advisable that the RFile is shared through the call RFs::ShareProtected().
This allows the adaptation to pass it to another process, if that is required. This is particularly
true of playing DRM encrypted files.

@param  aFile
        The open shared session file handle to use
@leave 	KErrBadHandle
		If the file handle is not shared through the call RFs::ShareProtected(), and the adaptation needs it to be.
@leave  KErrNotReady
        If a previous open statement is awaiting notification of completion.
		opening the file
*/
EXPORT_C void CMdaAudioPlayerUtility::OpenFileL(const RFile& aFile)
	{
	ASSERT(iProperties);
	RFile& file = const_cast<RFile&>(aFile);
	TMMFileHandleSource tfs(file, KDefaultContentObject, EPlay);
	iProperties->OpenFileL(tfs);
	}

/**
Opens an audio clip from a file.

The audio data must be in a supported format (for example, WAV or AU).

This function leaves with KErrNotReady if there is a previous open statement awaiting notification of completion.

@param  aSource
        The file to open or an open file handle to use
@leave  KErrNotReady
        If a previous open statement is awaiting notification of completion.
		opening the file
*/
EXPORT_C void CMdaAudioPlayerUtility::OpenFileL(const TMMSource& aSource)
	{
	ASSERT(iProperties);
	iProperties->OpenFileL(aSource);
	}
	
/**
Opens an audio clip from a descriptor.

The audio data must be in a supported format (for example, WAV or AU).

@param  aDescriptor
        A descriptor containing the audio clip.
@leave  KErrInUse
        If a previous open statement is awaiting notification of completion.

@since	7.0s
*/
EXPORT_C void CMdaAudioPlayerUtility::OpenDesL(const TDesC8& aDescriptor)
	{
	ASSERT(iProperties);
	iProperties->OpenDesL(aDescriptor);
	}

/**
Opens an audio clip from a URL.

The audio data must be in a supported format (for example, WAV or AU).

@param	aUrl
		The URL to open.
@param 	aIapId
		Internet access point(IAP) ID to use. KUseDefaultIap selects the default IAP.
@param  aMimeType
		MIME type of the URL source.

@leave  KErrInUse 
        If a previous open statement is awaiting notification of completion.

@since  7.0s
*/
EXPORT_C void CMdaAudioPlayerUtility::OpenUrlL(const TDesC& aUrl, const TInt aIapId /*=KUseDefaultIap*/, const TDesC8& aMimeType /*=KNullDesC8*/)
	{
	ASSERT(iProperties);
	iProperties->OpenUrlL(aUrl, aIapId, aMimeType);
	}

/**
Pauses the playback of the audio clip.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since	7.0s
*/
EXPORT_C TInt CMdaAudioPlayerUtility::Pause()
	{
	ASSERT(iProperties);
	return iProperties->Pause();
	}

/**
Closes the current audio clip (allowing another clip to be opened).

@since	7.0s
*/
EXPORT_C void CMdaAudioPlayerUtility::Close()
	{
	ASSERT(iProperties);
	iProperties->Close();
	}

/**
Returns the current playback position in microseconds from the start of the clip.

@param   aPosition
         The current time position in microseconds from the start of the clip to the current
         play position.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since	7.0s
*/
EXPORT_C TInt CMdaAudioPlayerUtility::GetPosition(TTimeIntervalMicroSeconds& aPosition)
	{
	ASSERT(iProperties);
	return iProperties->GetPosition(aPosition);
	}

/**
Sets the current playback position in microseconds from the start of the clip.

@param  aPosition
        The position to move to in microseconds from the start of the clip.

@since	7.0s
*/
EXPORT_C void CMdaAudioPlayerUtility::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	ASSERT(iProperties);
	iProperties->SetPosition(aPosition);
	}

/**
Sets the priority for playback. This is used to arbitrate between multiple
objects trying to access a single sound device.

@param  aPriority
        The Priority Value.
@param  aPref
        The Priority Preference.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s

@see CMdaAudioPlayerUtility::NewL()

*/
EXPORT_C TInt CMdaAudioPlayerUtility::SetPriority(TInt aPriority, TInt aPref)
	{
	ASSERT(iProperties);
	return iProperties->SetPriority(aPriority, aPref);
	}

/**
Returns the current playback volume.

@param  aVolume
        A value between 0 and the maximum volume settings returned by MaxVolume().

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since	7.0s
*/
EXPORT_C TInt CMdaAudioPlayerUtility::GetVolume(TInt& aVolume)
	{
	ASSERT(iProperties);
	return iProperties->GetVolume(aVolume);
	}

/**
Returns the number of meta data entries in the current audio clip.

@param  aNumEntries
        The number of meta data entries in the header of the current clip.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since	7.0s
*/
EXPORT_C TInt CMdaAudioPlayerUtility::GetNumberOfMetaDataEntries(TInt& aNumEntries)
	{
	ASSERT(iProperties);
	return iProperties->GetNumberOfMetaDataEntries(aNumEntries);
	}

/**
Returns the requested meta data entry.

@param  aMetaDataIndex
        The index number of the meta data to retrieve.

@return The requested meta data entry.
@leave  KErrNotFound
        The meta data entry does not exist.
@leave  KErrNotImplemented
        The controller does not support meta data information for this format.

@since	7.0s
*/
EXPORT_C CMMFMetaDataEntry* CMdaAudioPlayerUtility::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
	ASSERT(iProperties);
	return iProperties->GetMetaDataEntryL(aMetaDataIndex);
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
        The position defining the start of the window, measured in microseconds. If this value is less
        than zero, it is set to zero. If this value is greater than aEnd, then it is swapped with aEnd.
@param  aEnd
        The position defining the end of the window, measured in microseconds. If this value is greater
        than the value returned by Duration(), it is set to the value of Duration(). If this value is
        less than aStart, then it is swapped with aStart.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since	7.0s
*/
EXPORT_C TInt CMdaAudioPlayerUtility::SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
													const TTimeIntervalMicroSeconds& aEnd)
	{
	ASSERT(iProperties);
	return iProperties->SetPlayWindow(aStart, aEnd);
	}

/**
Clears the current playback window.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since	7.0s
*/
EXPORT_C TInt CMdaAudioPlayerUtility::ClearPlayWindow()
	{
	ASSERT(iProperties);
	return iProperties->ClearPlayWindow();
	}

/**
Sets the current playback balance.

@param  aBalance
        A value between KMMFBalanceMaxLeft
        and KMMFBalanceMaxRight. The default value is
        KMMFBalanceCenter.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioPlayerUtility::SetBalance(TInt aBalance /*= KMMFBalanceCenter*/)
	{
	ASSERT(iProperties);
	return iProperties->SetBalance(aBalance);
	}

/**
 *	Returns The current playback balance. This function may not return the same value 	
 *			as passed to SetBalance depending on the internal implementation in 
 *			the underlying components.
 *
 *	@param  aBalance
 *        	A value between KMMFBalanceMaxLeft
 *       	and KMMFBalanceMaxRight.
 *
 *	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
 *        	another of the system-wide error codes.
 *
 *	@since	7.0s
 */
EXPORT_C TInt CMdaAudioPlayerUtility::GetBalance(TInt& aBalance)
	{
	ASSERT(iProperties);
	return iProperties->GetBalance(aBalance);
	}

/**
Returns the controller implementation information associated with the current controller.

@return The controller implementation structure

@since 7.0s
*/
EXPORT_C const CMMFControllerImplementationInformation& CMdaAudioPlayerUtility::ControllerImplementationInformationL()
	{
	ASSERT(iProperties);
	return iProperties->ControllerImplementationInformationL();
	}

/**
Registers callback object to receive notifications of audio loading/rebuffering.

@param  aCallback
        The object to receive audio loading notifications.

@since  7.0s
*/
EXPORT_C void CMdaAudioPlayerUtility::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback)
	{
	ASSERT(iProperties);
	iProperties->RegisterForAudioLoadingNotification(aCallback);
	}

/**
Returns the current progress of audio loading.

@param  aPercentageProgress
        The percentage of the audio clip loaded.

@since  7.0s
*/
EXPORT_C void CMdaAudioPlayerUtility::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
	ASSERT(iProperties);
	iProperties->GetAudioLoadingProgressL(aPercentageProgress);
	}

/**
Sends a synchronous custom command to the controller.

@param  aDestination
        The destination of the message, consisting of the UID of
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
EXPORT_C TInt CMdaAudioPlayerUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	ASSERT(iProperties);
	return iProperties->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}

/**
Sends a synchronous custom command to the controller.

@param  aDestination
        The destination of the message, consisting of the UID of
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
EXPORT_C TInt CMdaAudioPlayerUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	ASSERT(iProperties);
	return iProperties->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}

/**
Sends an asynchronous custom command to the controller.

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
        write any data to be passed back to the client.  Can't be KNullDesC8."
@param  aStatus
        The TRequestStatus of an active object.  This will contain the
        result of the request on completion.  The exact range of
        result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CMdaAudioPlayerUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	ASSERT(iProperties);
	iProperties->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}

/**
Sends an asynchronous custom command to the controller.

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
EXPORT_C void CMdaAudioPlayerUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	ASSERT(iProperties);
	iProperties->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

/**
Returns the bit rate of the audio clip.

@param  aBitRate
		The bit rate of the audio clip

@return An error code indicating if the function call was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioPlayerUtility::GetBitRate(TUint& aBitRate)
	{
	ASSERT(iProperties);
	return iProperties->GetBitRate(aBitRate);	
	}

/**
Gets a controller's DRM custom command implementation.

@return A pointer to a controller's DRM custom command implementation, or NULL if the
controller does not support it.
*/
EXPORT_C MMMFDRMCustomCommand* CMdaAudioPlayerUtility::GetDRMCustomCommand()
	{
	ASSERT(iProperties);
	return iProperties->GetDRMCustomCommand();
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
EXPORT_C TInt CMdaAudioPlayerUtility::RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,TUid aNotificationEventUid,const TDesC8& aNotificationRegistrationData)
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
EXPORT_C TInt CMdaAudioPlayerUtility::CancelRegisterAudioResourceNotification(TUid aNotificationEventUid)
	{
	ASSERT(iProperties);
	return iProperties->CancelRegisterAudioResourceNotification(aNotificationEventUid);
	}

/**
Waits for the client to resume the play even after the default timer expires.

@return An error code indicating if the registration was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
*/
EXPORT_C TInt CMdaAudioPlayerUtility::WillResumePlay()
	{
	ASSERT(iProperties);
	return iProperties->WillResumePlay();
	}


/**
Set the priority of the controller's sub thread.

This can be used to increase the responsiveness of the audio plugin to minimise
any lag in processing. This function should be used with care as it may have knock-on
effects elsewhere in the system.

@param	aPriority
		The TThreadPriority that the thread should run under.  The default is EPriorityNormal.
@return	TInt
		A standard error code: KErrNone if successful, KErrNotReady if the thread does not have a
		valid handle.
*/
EXPORT_C TInt CMdaAudioPlayerUtility::SetThreadPriority(const TThreadPriority& aPriority) const
	{
	ASSERT(iProperties);
	return iProperties->SetThreadPriority(aPriority);
	}




CMMFMdaAudioPlayerUtility* CMMFMdaAudioPlayerUtility::NewL(MMdaAudioPlayerCallback& aCallback,
															  TInt aPriority,
															  TInt aPref)
	{
	CMMFMdaAudioPlayerUtility* self = new(ELeave) CMMFMdaAudioPlayerUtility(aCallback, aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMFMdaAudioPlayerUtility* CMMFMdaAudioPlayerUtility::NewFilePlayerL(const TDesC& aFileName,
																		MMdaAudioPlayerCallback& aCallback,
																		TInt aPriority,
																		TInt aPref,
																		CMdaServer* /*aServer*/)
	{
	CMMFMdaAudioPlayerUtility* self = new(ELeave) CMMFMdaAudioPlayerUtility(aCallback, aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL();
	TMMFileSource filesource(aFileName, KDefaultContentObject, EPlay);
	self->OpenFileL(filesource);
	CleanupStack::Pop(self);
	return self;
	}

CMMFMdaAudioPlayerUtility* CMMFMdaAudioPlayerUtility::NewDesPlayerL(const TDesC8& aData, MMdaAudioPlayerCallback& aCallback, TInt aPriority, TInt aPref, CMdaServer* /*aServer*/)
	{
	CMMFMdaAudioPlayerUtility* self = new(ELeave) CMMFMdaAudioPlayerUtility(aCallback, aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->OpenDesL(aData);
	CleanupStack::Pop(self);
	return self;
	}

CMMFMdaAudioPlayerUtility* CMMFMdaAudioPlayerUtility::NewDesPlayerReadOnlyL(const TDesC8& aData, MMdaAudioPlayerCallback& aCallback, TInt aPriority, TInt aPref, CMdaServer* /*aServer*/)
	{
	CMMFMdaAudioPlayerUtility* self = new(ELeave) CMMFMdaAudioPlayerUtility(aCallback, aPriority, aPref);
	CleanupStack::PushL(self);
	self->ConstructL();
	self->OpenDesL(aData);
	CleanupStack::Pop(self);
	return self;
	}

void CMMFMdaAudioPlayerUtility::UseSharedHeap()
	{
	iFindAndOpenController->UseSharedHeap();
	}

// CMMFMdaAudioPlayerUtility
CMMFMdaAudioPlayerUtility::~CMMFMdaAudioPlayerUtility()
	{
	
	delete iControllerImplementationInformation;
	delete iAsyncCallBack;
	delete iRepeatTrailingSilenceTimer;
	delete iFindAndOpenController;
	delete iControllerEventMonitor;
	iMediaIds.Close();
	iController.Close();
	}

CMMFMdaAudioPlayerUtility::CMMFMdaAudioPlayerUtility(MMdaAudioPlayerCallback& aCallback, TInt aPriority, TInt aPref) :
	iCallback(aCallback),
	iAudioPlayDeviceCommands(iController),
	iAudioPlayControllerCommands(iController),
	iNotificationRegistrationCommands(iController),
	iDRMCustomCommands(iController),
	iAudioPlayControllerSetRepeatsCommands(iController)
	{
	iState = EStopped;
	iPrioritySettings.iPriority = aPriority;
	iPrioritySettings.iPref = aPref;
	iPlayStart = TTimeIntervalMicroSeconds(0);
	iPlayEnd = TTimeIntervalMicroSeconds(0);
	iPlayWindowSet = ENone;
	iEventHolder = KNullUid;
	}

void CMMFMdaAudioPlayerUtility::ConstructL()
	{
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	iRepeatTrailingSilenceTimer = CRepeatTrailingSilenceTimer::NewL(*this);
	iAsyncCallBack = CMMFMdaAudioPlayerCallBack::NewL(iCallback);
	User::LeaveIfError(iMediaIds.Append(KUidMediaTypeAudio));
	iFindAndOpenController = CMMFFindAndOpenController::NewL(*this);
	iFindAndOpenController->Configure(iMediaIds[0], iPrioritySettings);
	iFindAndOpenController->ConfigureController(iController, *iControllerEventMonitor, CMMFFindAndOpenController::EPlayback);
	}

void CMMFMdaAudioPlayerUtility::MfaocComplete(		
		TInt& aError, 
		RMMFController* /*aController*/,
		TUid aControllerUid, 
		TMMFMessageDestination* /*aSourceHandle*/, 
		TMMFMessageDestination* /*aSinkHandle*/)
	{
	if (aError == KErrNone)
		{
		iControllerUid = aControllerUid;

		// Get the clip duration
		iDuration = TTimeIntervalMicroSeconds(0);
		aError = iController.GetDuration(iDuration);
				
		// If an error occurred during GetDuration, may try for next controller, if present.
		if (aError != KErrNone)
			{
			iControllerEventMonitor->Cancel();
			
			if (iFindAndOpenController)	
				{
				if(iFindAndOpenController-> ControllerIndex() < (iFindAndOpenController->ControllerCount())-1)
					{
					return;   //actually tries to load next controllers, if there are other controllers selected in the controller list
					}
				}
			
			iController.Close(); // otherwise close the controller
			}
	
		if (iFindAndOpenController)	
			{
			iFindAndOpenController->Close();
			}
		}
	
	iAsyncCallBack->InitComplete(aError, iDuration);
	}

/**
	Open an audio clip from a file
	@param "const TFileSource& aFileSource" "the file to open"
	@leave "" "Leaves on an error opening the file
	@since version 5.0
*/
void CMMFMdaAudioPlayerUtility::OpenFileL(const TDesC& aFileName)
	{
	TMMFileSource filesource(aFileName, KDefaultContentObject, EPlay);
	OpenFileL(filesource);
	}
	
/**
	Open an audio clip from a file
	@param "const RFile& aFile" "the shared session file handle to open"
	@leave "" "KErrBadHandle if the file handle is not shared through the call RFs::ShareProtected(), and the underlying CAF layer needs it to be.
	@leave "" "Leaves on an error opening the file
	@since version 5.0
*/
void CMMFMdaAudioPlayerUtility::OpenFileL(const RFile& aFile)
	{
	RFile& file = const_cast<RFile&>(aFile);
	TMMFileHandleSource filesource(file, KDefaultContentObject, EPlay);
	OpenFileL(filesource);
	}

void CMMFMdaAudioPlayerUtility::OpenFileL(const TMMSource& aSource)
	{
	// If iAsyncCallBack is already active, we're still in the process of notifying the client
	// that a previous request to Open...(...) has completed.
	if (iAsyncCallBack->IsActive())
		User::Leave(KErrNotReady);
	
	if (aSource.SourceType()==KUidMMFileHandleSource)
		{
		RFile& fileHandle = static_cast<const TMMFileHandleSource&>(aSource).Handle();
		iFindAndOpenController->ConfigureSourceSink(
			TMMFileHandleSource(fileHandle, aSource.UniqueId(), aSource.Intent(), aSource.IsUIEnabled()),
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));

		}
	if (aSource.SourceType()==KUidMMFileSource)
		{
		const TDesC& fileName = static_cast<const TMMFileSource&>(aSource).Name();
		iFindAndOpenController->ConfigureSourceSink(
			TMMFileSource(fileName, aSource.UniqueId(), aSource.Intent(), aSource.IsUIEnabled()),
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
		}

	iFindAndOpenController->OpenByFileSource(aSource);
	}

/**
	Open an audio clip from a descriptor
	@param "const TDesC8& aDescriptor" "the descriptor containing the clip"
	@leave "" "Leaves on an error opening the descriptor"
	@since version 5.0
*/
void CMMFMdaAudioPlayerUtility::OpenDesL(const TDesC8& aDescriptor)
	{
	// If iAsyncCallBack is already active, we're still in the process of notifying the client
	// that a previous request to Open...(...) has completed.
	if (iAsyncCallBack->IsActive())
		User::Leave(KErrInUse);

	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfDescriptorSource,
													CMMFFindAndOpenController::GetConfigDescriptor(aDescriptor)),
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
	iFindAndOpenController->OpenByDescriptor(aDescriptor);
	}

/**
	Open an audio clip from a Url
	@param "const TDesC& aUrl" "the url reference to the clip"
	@leave "" "Leaves on an error opening the url"
	@since version 7.0s
*/
void CMMFMdaAudioPlayerUtility::OpenUrlL(const TDesC& aUrl, const TInt aIapId, const TDesC8& aMimeType)
	{
	// If iAsyncCallBack is already active, we're still in the process of notifying the client
	// that a previous request to Open...(...) has completed.
	if (iAsyncCallBack->IsActive())
		User::Leave(KErrInUse);

	CBufFlat* urlCfgBuffer = NULL;
	CMMFFindAndOpenController::GetConfigUrlL(urlCfgBuffer, aUrl, aIapId);
	
	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfUrlSource, urlCfgBuffer->Ptr(0)), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
	iFindAndOpenController->OpenByUrl(aUrl, aIapId, aMimeType);
	delete urlCfgBuffer;
	}

/**
Begins playback of the initialised audio sample at the current volume
and priority levels.

When playing of the audio sample is complete, successfully or
otherwise, the callback function
MMdaAudioPlayerCallback::MapcPlayComplete() is
called.

If this function is called whilst already playing then 
MMdaAudioPlayerCallback::MapcPlayComplete will return with the
error code KErrNotReady.

@since	5.0
*/
void CMMFMdaAudioPlayerUtility::Play()
	{
	// if we're already playing, call the client's callback with KErrNotReady.
	// This is what the controller would do if we allowed the Play()
	// to propagate down. Need to do it here too (for consistency)
	// in case we're in a trailing silence period.
    if (iState == EPlaying)
		{
		iAsyncCallBack->PlayComplete(KErrNotReady);
		return;
		}

	// cancel the repeat timer in case the client has called Play()
	// without waiting for the previous play to complete
	iRepeatTrailingSilenceTimer->Cancel();	
	// Reset played count
	if(iState != EPaused)
		{
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
		}

	DoPlay();
	}

void CMMFMdaAudioPlayerUtility::DoPlay()
	{
#if defined(__AUDIO_PROFILING)
	RDebug::ProfileStart(4);
#endif  // defined(__AUDIO_PROFILING)
    TInt err = KErrNone;
    if (iState != EPaused || iRepeatCancelled)
        {
		err = iController.Prime();
		iRepeatCancelled = EFalse;

#if defined(__AUDIO_PROFILING)
	RDebug::ProfileEnd(4);
#endif  // defined(__AUDIO_PROFILING)

		// make sure we don't set the position outside the play window -
		// but allow it to remain unchanged if it's within the window
		if (iPlayWindowSet == ESet &&
			(iPosition < iPlayStart || iPosition >= iPlayEnd))
			iPosition = iPlayStart;

		if (err==KErrNone)
			err = iController.SetPosition(iPosition);
        }

	if (err==KErrNone)
		{
		if (iPlayWindowSet == ESet)
			err = iAudioPlayControllerCommands.SetPlaybackWindow(iPlayStart, iPlayEnd);
		else if (iPlayWindowSet == EClear)
			{
			err = iAudioPlayControllerCommands.DeletePlaybackWindow();
			iPlayWindowSet = ENone;	// assume window will stay cleared
			}
		}

	if (err==KErrNone)
		{
#if defined(__AUDIO_PROFILING)
		RDebug::ProfileStart(5);
#endif  // defined(__AUDIO_PROFILING)
		
		err = iController.Play();
	
#if defined(__AUDIO_PROFILING)
		RDebug::ProfileEnd(5);
#endif  // defined(__AUDIO_PROFILING)
		}

	if (err!=KErrNone)
		iAsyncCallBack->PlayComplete(err);
	else
		iState = EPlaying;
	
	if(iEventHolder != KNullUid)
		{
		err = iNotificationRegistrationCommands.RegisterAsClient(iEventHolder,iNotificationDataHolder);			
		iEventHolder = KNullUid;
		iNotificationDataHolder = KNullDesC8;
		if(err == KErrNotSupported)
			{
			return;
			}
		if(err != KErrNone)
			{
			iController.Stop();
			iAsyncCallBack->PlayComplete(err);
			}
		}
	}

/**
Stops playback of the audio sample as soon as possible.

If the audio sample is playing, playback is stopped as soon as
possible. If playback is already complete, nothing further happens as
a result of calling this function. The callback function
MMdaAudioPlayerCallback::MapcPlayComplete() is not
called.

@since	5.0
*/
void CMMFMdaAudioPlayerUtility::Stop()
	{
	
	if (iState==EStopped)
		{
		// resetting the position to the start.
		//if any position change in stoped state
		iPosition = iPlayStart;	
		return;
		}
	
	if (iState==EPlaying || iState==EPaused)
		{
		// cancel the repeat timer in case the client has called Stop()
		// during the trailing silence period
		iRepeatTrailingSilenceTimer->Cancel();	

		iController.Stop();
		iPosition = iPlayStart;	
		iState = EStopped;	
		}

	}

/**
 *
 * Pauses playback of the audio clip
 * @return One of the system-wide error codes
 * @since	7.0s
 */
TInt CMMFMdaAudioPlayerUtility::Pause()
	{
	TInt err = KErrNone;
	if(iRepeatTrailingSilenceTimer->IsActive())
		{
		iRepeatTrailingSilenceTimer->Cancel();
		iRepeatCancelled = ETrue;
		iState = EPaused;	
		return KErrNone;
		}
	if (iState==EPlaying)
		{
		err = iController.Pause();
		if (!err || err==KErrNotReady)
			err = iController.GetPosition(iPosition);
		iState = EPaused;
		}
	return err;
	}

/**
 *
 * Closes the current audio clip (allowing another clip to be opened)
 *
 * @since	7.0s
 */
void CMMFMdaAudioPlayerUtility::Close()
	{
	// Reset the audio player state.
	Stop();
	iControllerEventMonitor->Cancel();
	iController.Close();
	if (iFindAndOpenController)	
		iFindAndOpenController->Close();
	if(iControllerImplementationInformation)
		{
		delete iControllerImplementationInformation;
		iControllerImplementationInformation = NULL;
		}
	iControllerUid = KNullUid;
	}


/**
Changes the current playback volume to a specified value.

The volume can be changed before or during playback and is effective
immediately.

@param  aVolume
        The volume setting. This can be any value from zero to
        the value returned by a call to
        CMdaAudioPlayerUtility::MaxVolume().
        Setting a zero value mutes the sound. Setting the
        maximum value results in the loudest possible sound.
@return An error code indicating if the function call was successful. KErrNone on success, 
		otherwise another of the system-wide error codes.
@panic  EMMFMediaClientBadArgument is raised when the audio player utility is not initialised.

@since  5.0
*/
TInt CMMFMdaAudioPlayerUtility::SetVolume(TInt aVolume)
	{
	TInt err = iAudioPlayDeviceCommands.SetVolume(aVolume);
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
		err = iAudioPlayDeviceCommands.SetVolume(aVolume);			
		}

	return err;
	}

/**
Sets the number of times the audio sample is to be repeated during the
playback operation.

A period of silence can follow each playing of the sample. The audio
sample can be repeated indefinitely.

@param  aRepeatNumberOfTimes
        The number of times the audio sample, together with
        the trailing silence, is to be repeated. If this is
        set to KMdaRepeatForever, then the audio
        sample, together with the trailing silence, is
        repeated indefinitely or until Stop() is
        called. If this is set to zero, then the audio sample
        is not repeated. The behaviour is undefined for
		negative values (other than KMdaRepeatForever).
@param  aTrailingSilence
        The time interval of the training silence.
		Negative values will produce a panic USER 87.
@since	5.0
*/
void CMMFMdaAudioPlayerUtility::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	TInt err = iAudioPlayControllerSetRepeatsCommands.SetRepeats(aRepeatNumberOfTimes, aTrailingSilence);
	
	if(err!=KErrNone)
		{
		iNumberOfTimesToRepeat = aRepeatNumberOfTimes;
		iTrailingSilence = aTrailingSilence;
		}
	}

/**
Defines the period over which the volume level is to rise smoothly
from nothing to the normal volume level.

@param  aRampDuration
        The period over which the volume is to rise. A zero
        value causes the audio sample to be played at the
        normal level for the full duration of the playback. A
        value which is longer than the duration of the audio
        sample means that the sample never reaches its normal
        volume level.

@since  5.0
*/
void CMMFMdaAudioPlayerUtility::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	iAudioPlayDeviceCommands.SetVolumeRamp(aRampDuration);
	}

TInt CMMFMdaAudioPlayerUtility::SetPriority(TInt aPriority, TInt aPref)
	{
	iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;
	iFindAndOpenController->Configure(iMediaIds[0], iPrioritySettings);

	return iController.SetPrioritySettings(iPrioritySettings);
	}

/**
Returns the duration of the audio sample.

@return The duration in microseconds.

@since  5.0
*/
const TTimeIntervalMicroSeconds& CMMFMdaAudioPlayerUtility::Duration()
	{
	TInt err = iController.GetDuration(iDuration);
	if (err)
		{
		iDuration = 0;
		}
	return iDuration;
	}
	
/**
Returns the duration of the audio sample in microseconds, and the duration state.

@param aDuration
	   The duration of the sample in microseconds.
@return The duration state

@since	9.1
*/	
TMMFDurationInfo CMMFMdaAudioPlayerUtility::Duration(TTimeIntervalMicroSeconds& aDuration)
{	
	TPckgBuf<TMMFDurationInfo> pckg;
	TMMFDurationInfo result = EMMFDurationInfoValid;
	
	TMMFMessageDestinationPckg iDestinationPckg(TMMFMessageDestination(KUidInterfaceMMFDurationInfoControl, KMMFObjectHandleController));
	
	TInt err = iController.CustomCommandSync(iDestinationPckg,
										 	 EMMFGetDurationInfo,
										 	 KNullDesC8,
										 	 KNullDesC8,
										     pckg );
																 
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
	// This is the intended behaviour regardless of what value err has
	aDuration = Duration();
	return result;
}	
	
/**
Returns an integer representing the maximum volume.

This is the maximum value which can be passed to
CMdaAudioPlayerUtility::SetVolume().

@return The maximum volume. This value is platform dependent but is always greater than or equal 
        to one.
@panic  EMMFMediaClientPanicServerCommunicationProblem is raised when the audio player utility is not initialised.

@since  5.0
*/
TInt CMMFMdaAudioPlayerUtility::MaxVolume()
	{
	TInt maxVolume = 0;
#ifdef _DEBUG
	TInt error = 
#endif
		iAudioPlayDeviceCommands.GetMaxVolume(maxVolume);
	__ASSERT_DEBUG(error==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem));
	return maxVolume;
	}

void CMMFMdaAudioPlayerUtility::HandleEvent(const TMMFEvent& aEvent)
	{
	// handle loading started/complete messages first, as the later code does not explicitly check the event type
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
			if (KErrNone != iNotificationRegistrationCommands.GetResourceNotificationData(aEvent.iEventType, notificationData))
				{
				notificationData.SetLength(0);
				}
			iAudioResourceNotificationCallBack->MarncResourceAvailable(aEvent.iEventType, notificationData);
			}
		}
	else if (aEvent.iEventType == KMMFEventCategoryPlaybackComplete)
		{
		TInt oldState = iState;
		//DevCR KEVN-7T5EHA: In case of pre-emption, we need to get the position from Controller, if that fails we reset the position to keep the original behaviour.
		if(aEvent.iErrorCode == KErrInUse ||aEvent.iErrorCode == KErrDied ||aEvent.iErrorCode == KErrAccessDenied )
		    {
		    TInt err= iController.GetPosition(iPosition);
		    if(err != KErrNone)
		        {
		        iPosition = iPlayStart;
		        }
		    }
		else
		    {
		    iPosition = iPlayStart;
		    }
		if (aEvent.iErrorCode == KErrNone)
			{
			//If we weren't playing, ignore the event.
			if (oldState == EPlaying)
				{
				//we finished playing the clip so repeat if required
				iNumberOfTimesPlayed++;
				if ((iNumberOfTimesPlayed > iNumberOfTimesToRepeat) && (iNumberOfTimesToRepeat != KMdaRepeatForever))
					{
					//we've repeated enough times now
					iNumberOfTimesPlayed = 0;
					iState = EStopped;
					iCallback.MapcPlayComplete(KErrNone);
					}
				else
					{
					// We need to play silence, then repeat the clip
					iTrailingSilenceLeftToPlay = iTrailingSilence;
					PlaySilence();
					}
				}
			}
		else
			{ //aEvent.iErrorCode != KErrNone
			//if we weren't playing, don't advise Client.
			iState = EStopped;
			if (oldState == EPlaying)
				{
				iCallback.MapcPlayComplete(aEvent.iErrorCode);
				}
			}
		}
	else if(aEvent.iEventType == KMMFErrorCategoryControllerGeneralError)
		{
		TInt oldState = iState;
		iPosition = iPlayStart;
		iState = EStopped;
		if (oldState == EPlaying)
			{
			iCallback.MapcPlayComplete(aEvent.iErrorCode);
			}
		}
	// else we have an unexpected event that cannot be dealt with by the client.
	// We will simply ignore this.
	}

void CMMFMdaAudioPlayerUtility::PlaySilence()
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

void CMMFMdaAudioPlayerUtility::RepeatTrailingSilenceTimerComplete()
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

/**
 *
 * Returns the current playback position in microseconds
 *
 * @param "TTimeIntervalMicroSeconds& aPosition"
 *          The current time position in microseconds from the start of the file
 * @return "TInt" One of the global error codes
 *
 * @since	7.0s
 */
TInt CMMFMdaAudioPlayerUtility::GetPosition(TTimeIntervalMicroSeconds& aPosition)
	{
	TInt error = KErrNone;
	if (iState==EPlaying)
		error = iController.GetPosition(iPosition);
	aPosition = iPosition;
	return error;
	}

/**
 *
 * Set the current playback position in microseconds from the start of the file
 *
 * @param "TTimeIntervalMicroSeconds& aPosition"
 *         The position to move to in microseconds from the start of the file.
 *         If aPosition is negative, the position is set to the start of the file.
 *         If aPosition is greater than the file duration, the position is set to the
 *         end of the file.
 *
 * @since	7.0s
 */
void CMMFMdaAudioPlayerUtility::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	// Clip the position if aPosition is greater than the duration
	// or if aPosition is negative.
	const TTimeIntervalMicroSeconds maxPosition(Duration());
	const TTimeIntervalMicroSeconds minPosition(0);

	if (aPosition > maxPosition)
		iPosition = maxPosition;
	else if (aPosition < minPosition)
		iPosition = minPosition;
	else
		iPosition = aPosition;

    if (iState==EPlaying || iState==EPaused)
		{
		iController.SetPosition(iPosition);
		}
//	else if (iState == EPaused)
//		{
//		Stop();	// We call stop so that DevSound's internal buffers are reset
//		}
	}

/**
Returns the current playback volume

@param aVolume
       A volume value between 0 and the value returned by MaxVolume().

@return One of the global error codes.

@since  7.0s
*/
TInt CMMFMdaAudioPlayerUtility::GetVolume(TInt& aVolume)
	{
	TInt error = iAudioPlayDeviceCommands.GetVolume(aVolume);
	return error;
	}

/**
 *
 * Returns the number of meta data entries in the current clip
 *
 * @param "TInt& aNumEntries"
 *          The number of meta data entries in the header of the current clip
 * @return "TInt" One of the global error codes
 *
 * @since	7.0s
 */
TInt CMMFMdaAudioPlayerUtility::GetNumberOfMetaDataEntries(TInt& aNumEntries) 
	{
	TInt error = iController.GetNumberOfMetaDataEntries(aNumEntries);
	return error;
	}

/**
 *
 * Returns the requested meta data entry
 *
 * @param "TInt aMetaDataIndex"
 *          The index number of the meta data to retrieve
 * @return "CMMFMetaDataEntry*" The meta data entry to return
 * @leave	Leaves with KErrNotFound if the meta data entry does not exist or
 *			KErrNotImplemented if the controller does not support meta data 
 *			information for this format. Other errors indicate more general system
 *			failure.
 *
 * @since	7.0s
 */
CMMFMetaDataEntry* CMMFMdaAudioPlayerUtility::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
	return iController.GetMetaDataEntryL(aMetaDataIndex);
	}

/**
 *
 * Set the current playback window
 *
 * @param	"const TTimeIntervalMicroSeconds& aStart"
 *			Start time of playback window relative to start of file
 * @param	"const TTimeIntervalMicroSeconds& aEnd"
 *			End time of playback window relative to start of file
 *
 * @return "TInt" One of the global error codes
 *
 * @since	7.0s
 */
TInt CMMFMdaAudioPlayerUtility::SetPlayWindow(const TTimeIntervalMicroSeconds& aPlayStart,
											  const TTimeIntervalMicroSeconds& aPlayEnd)
	{
	TInt error = KErrNone;

	if (aPlayStart >= TTimeIntervalMicroSeconds(0) &&
		aPlayStart < iDuration &&
			aPlayStart < aPlayEnd &&
			aPlayEnd <= iDuration )
		{
		iPlayStart = aPlayStart;
		iPlayEnd = aPlayEnd;
		iPlayWindowSet = ESet;

		if (iState==EPlaying)
			error = iAudioPlayControllerCommands.SetPlaybackWindow(aPlayStart, aPlayEnd);
		}
	else
		error = KErrArgument;

	return error;
	}
	
/**
 *
 * Clear the current playback window
 *
 * @return "TInt" One of the global error codes
 *
 * @since	7.0s
 */
TInt CMMFMdaAudioPlayerUtility::ClearPlayWindow()
	{
	// clear play window start - very important because this is assigned 
	// to iPosition when we stop & is used to set the position on the next Play()
	iPosition = iPlayStart = iPlayEnd = TTimeIntervalMicroSeconds(0);

	iPlayWindowSet = EClear;
	TInt err = KErrNone;
	if (iState==EPlaying)
		err = iAudioPlayControllerCommands.DeletePlaybackWindow();
	return err;
	}

/**
Sets the current playback balance

@param  aBalance
        A value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight. The default value is
        KMMFBalanceCenter.

@return One of the global error codes.

@since  7.0s
*/
TInt CMMFMdaAudioPlayerUtility::SetBalance(TInt aBalance)
	{
	TInt err = iAudioPlayDeviceCommands.SetBalance(aBalance);
	return err;
	}

/**
Returns the bit rate of the audio clip.

@param  aBitRate
        Bit rate of the audio clip.

@return One of the global error codes.

@since  7.0s
*/
TInt CMMFMdaAudioPlayerUtility::GetBitRate(TUint& aBitRate)
	{
	RMMFAudioControllerCustomCommands controller(iController);
	TInt err = controller.GetSourceBitRate(aBitRate);
	return err;	
	}

const CMMFControllerImplementationInformation& CMMFMdaAudioPlayerUtility::ControllerImplementationInformationL()
	{
	if (!iControllerImplementationInformation)
		{
		if (iControllerUid==KNullUid)
			User::Leave(KErrNotReady);
		iControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(iControllerUid);
		}
	return *iControllerImplementationInformation;
	}
	
void CMMFMdaAudioPlayerUtility::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
	User::LeaveIfError(iAudioPlayControllerCommands.GetLoadingProgress(aPercentageProgress));
	}
	
TInt CMMFMdaAudioPlayerUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
	
TInt CMMFMdaAudioPlayerUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
	
void CMMFMdaAudioPlayerUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
	
void CMMFMdaAudioPlayerUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

/**
Returns the current playback balance

@param  aBalance
        A value between KMMFBalanceMaxLeft and KMMFBalanceMaxRight

@return One of the global error codes.

@since  7.0s
*/
TInt CMMFMdaAudioPlayerUtility::GetBalance(TInt& aBalance)
	{
	TInt err = iAudioPlayDeviceCommands.GetBalance(aBalance);
	return err;
	}
	
MMMFDRMCustomCommand* CMMFMdaAudioPlayerUtility::GetDRMCustomCommand()
	{
	// XXX: check controller supports MMMFDRMCustomCommandImplementor
	if (iDRMCustomCommands.IsSupported())
		{
		return static_cast<MMMFDRMCustomCommand*>(&iDRMCustomCommands);
		}
	else
		{
		return NULL;
		}
	}
	
void CMMFMdaAudioPlayerUtility::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aLoadingObserver)
	{
	iLoadingObserver = &aLoadingObserver;
	}

TInt CMMFMdaAudioPlayerUtility::RegisterAudioResourceNotification(MMMFAudioResourceNotificationCallback& aCallback,
																	TUid aNotificationEventUid,
																	const TDesC8& aNotificationRegistrationData)
	{
	iAudioResourceNotificationCallBack = &aCallback;
	TInt err = iNotificationRegistrationCommands.RegisterAsClient(aNotificationEventUid, aNotificationRegistrationData);
	if(err == KErrNotReady)
		{
		iEventHolder = 	aNotificationEventUid;
		iNotificationDataHolder = aNotificationRegistrationData;
		return KErrNone;
		}
	iNotificationDataHolder = KNullDesC8;
	iEventHolder = KNullUid;
	return err;
	}

TInt CMMFMdaAudioPlayerUtility::CancelRegisterAudioResourceNotification(TUid aNotificationEventId)
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
	
TInt CMMFMdaAudioPlayerUtility::WillResumePlay()
	{
	return iNotificationRegistrationCommands.WillResumePlay();
	}
	
TInt CMMFMdaAudioPlayerUtility::SetThreadPriority(const TThreadPriority& aThreadPriority) const
	{
	return iController.SetThreadPriority(aThreadPriority);
	}
	
CRepeatTrailingSilenceTimer* CRepeatTrailingSilenceTimer::NewL(MRepeatTrailingSilenceTimerObs& aObs)
	{
	CRepeatTrailingSilenceTimer* s = new(ELeave) CRepeatTrailingSilenceTimer(aObs);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

void CRepeatTrailingSilenceTimer::RunL()
	{
	iObs.RepeatTrailingSilenceTimerComplete();
	}

CRepeatTrailingSilenceTimer::CRepeatTrailingSilenceTimer(MRepeatTrailingSilenceTimerObs& aObs) :
	CTimer(EPriorityHigh),
	iObs(aObs)
	{
	CActiveScheduler::Add(this);
	}



CMMFMdaAudioPlayerCallBack* CMMFMdaAudioPlayerCallBack::NewL(MMdaAudioPlayerCallback& aCallback)
	{
	return new(ELeave) CMMFMdaAudioPlayerCallBack(aCallback);
	}

CMMFMdaAudioPlayerCallBack::CMMFMdaAudioPlayerCallBack(MMdaAudioPlayerCallback& aCallback) :
	CActive(CActive::EPriorityHigh), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

CMMFMdaAudioPlayerCallBack::~CMMFMdaAudioPlayerCallBack()
	{
	Cancel();
	}

void CMMFMdaAudioPlayerCallBack::InitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	iState = ECallbackInitComplete;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}

void CMMFMdaAudioPlayerCallBack::PlayComplete(TInt aError)
	{
	iError = aError;
	iState = ECallbackPlayComplete;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}

void CMMFMdaAudioPlayerCallBack::RunL()
	{
	switch (iState)
		{
		case ECallbackInitComplete: 
			iCallback.MapcInitComplete(iError, iDuration);
			break;
		case ECallbackPlayComplete:
			iCallback.MapcPlayComplete(iError);
			break;
		}
	}

void CMMFMdaAudioPlayerCallBack::DoCancel()
	{
	// Nothing to cancel
	}
