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


#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>
#include <videorecorder.h>
#include "VideoRecorderBody.h"
#include "mmfvideocallback.h"
#include "mmfvideopaniccodes.h"
/**
Constructs and initialises a new instance of the video recorder utility.

The function leaves if the video recorder utility object cannot be created.

@param  aObserver
        An object to receive video recorder notifications.
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.

@return A pointer to the new video recorder utility object.

@since 7.0s

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CVideoRecorderUtility* CVideoRecorderUtility::NewL(MVideoRecorderUtilityObserver& aObserver,
															TInt aPriority,
															TInt aPref)
	{
	CVideoRecorderUtility* s = new(ELeave) CVideoRecorderUtility();
	CleanupStack::PushL(s);
	s->iBody = CVideoRecorderUtility::CBody::NewL(s, aObserver, aPriority, aPref);
	CleanupStack::Pop();
	return s;
	}
/**
Destructor. Closes the current clip, if one is open, and frees resources.

@since 7.0s
*/
CVideoRecorderUtility::~CVideoRecorderUtility()
	{
	delete iBody;
	}

/**
Opens a file that can be used to store a recorded video clip.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aFileName
        A full path name of the file to save the video clip to.
@param  aCameraHandle
        A handle to the camera to use for recording.
@param  aControllerUid
        The UID of the controller to use for recording.
@param  aVideoFormatUid
        The UID of the video format to record to.
@param  aVideoCodec
        The descriptor containing the video MIME type.
@param  aAudioCodec
        A four character code representing the encoding to be used for audio data.

@since 7.0s
*/


EXPORT_C void CVideoRecorderUtility::OpenFileL(const TDesC& aFileName,
											   TInt aCameraHandle,
											   TUid aControllerUid,
											   TUid aVideoFormatUid,
											   const TDesC8& aVideoCodec,
											   TFourCC aAudioCodec)
	{
	iBody->OpenFileL(aFileName, aCameraHandle, aControllerUid, aVideoFormatUid, aVideoCodec, aAudioCodec);
	}
	

//
// This method launches and initializes plugin controller based on the stream 
// source header passed in the descriptor buffer.
//

/**
Specifies a descriptor that can be used to store a recorded video clip.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aDescriptor
        The descriptor used to store the recorded video clip.
@param  aCameraHandle
        The handle of the camera to record from.
@param  aControllerUid
        The UID of the controller to use for recording.
@param  aVideoFormatUid
        The UID of the video format to record to.
@param  aVideoCodec
        The descriptor containing the video MIME type.
@param  aAudioCodec
        A four character code representing the encoding to be used for audio data.

@since 7.0s
*/

EXPORT_C void CVideoRecorderUtility::OpenDesL(TDes8& aDescriptor,
										TInt aCameraHandle,
										TUid aControllerUid,
										TUid aVideoFormatUid,
										const TDesC8& aVideoCodec,
										TFourCC aAudioCodec)
	{
	iBody->OpenDesL(aDescriptor, aCameraHandle, aControllerUid, aVideoFormatUid, aVideoCodec, aAudioCodec);
	}
/**
Specifies a URL that can be used to store a recorded video clip.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aUrl
        The URL used to store the recorded video clip.
@param  aIapId
        The Id of the internet access point to use.
@param  aCameraHandle
        The handle of the camera to record from.
@param  aControllerUid
        The UID of the controller to use for recording.
@param  aVideoFormatUid
        The UID of the video format to record to.
@param  aVideoCodec
        The descriptor containing the video MIME type.
@param  aAudioCodec
        A four character code representing the encoding to be used for audio data.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::OpenUrlL(const TDesC& aUrl,
											  TInt aIapId,
											  TInt aCameraHandle,
											  TUid aControllerUid,
											  TUid aVideoFormatUid,
											  const TDesC8& aVideoCodec,
											  TFourCC aAudioCodec)
	{
	iBody->OpenUrlL(aUrl, aIapId, aCameraHandle, aControllerUid, aVideoFormatUid, aVideoCodec, aAudioCodec);
	}
/**
Starts the recording of video data to the specified file, descriptor or URL.

If the video format also supports the use of an audio track, recording of the audio track can
be enabled using SetAudioEnabledL().

The client will receive a MvruoRecordComplete on completion of recording,
unless it explicitly ends recording with Stop. Prepare() should be called before the first
call to Record().

This function must not be called until a MVideoRecorderUtilityObserver::MvruoPrepareComplete() 
has been received with an aError equal to KErrNone.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::Record()
	{
	iBody->Record();
	}
/**
Stops the recording of video data to the specified file, descriptor or URL.

If the video format supports audio tracks and audio is enabled, this function also stops the
recording of the audio data.

Recording is stopped without sending the MvruoRecordComplete message to the client.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@since 7.0s
*/
EXPORT_C TInt CVideoRecorderUtility::Stop()
	{
	return iBody->Stop();
	}
/**
Prepares the recorder for recording. 

This should be called before the first call to record. The client will receive a 
MvruoPrepareComplete when the prepare operation is complete.

@see MVideoRecorderUtilityObserver::MvruoPrepareComplete()

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::Prepare()
	{
	iBody->Prepare();
	}
/**
Closes a video clip.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::Close()
	{
	iBody->Close();
	}
/**
Pauses the recording of video data to the specified file, descriptor or URL.

If the video format supports audio tracks and audio is enabled, this function also pauses the
recording of the audio data.

If an error occurs, this function leaves with one of the system wide error codes.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::PauseL()
	{
	iBody->PauseL();
	}
/**
Sets the video frame rate.

@param  aFramesPerSecond
        The video frame rate, in frames/second.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetVideoFrameRateL(TReal32 aFramesPerSecond)
	{
	iBody->SetVideoFrameRateL(aFramesPerSecond);
	}
/**
Returns the video frame rate.

If an error occurs, this function leaves with one of the system wide error codes.

@return The frame rate in frames/second.

@since 7.0s
*/
EXPORT_C TReal32 CVideoRecorderUtility::VideoFrameRateL() const
	{
	return iBody->VideoFrameRateL();
	}
/**
Gets the video frame size.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aSize
        The video frame size, in pixels.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::GetVideoFrameSizeL(TSize& aSize) const
	{
	iBody->GetVideoFrameSizeL(aSize);
	}
/**
Returns the number of meta data entries associated with this video clip.

If an error occurs, this function leaves with one of the system wide error codes.

@return The number of meta data entries

@since 7.0s
*/
EXPORT_C TInt CVideoRecorderUtility::NumberOfMetaDataEntriesL() const
	{
	return iBody->NumberOfMetaDataEntriesL();
	}
/**
Returns a meta data entry from the current clip.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aMetaDataIndex
        The index of the entry to retrieve. The index is between 0 and 
        NumberOfMetaDataEntriesL()-1.

@return The meta data entry requested.

@leave KErrArgument 
       If the index is out of range, or with one of the other system-wide error codes.

@since 7.0s
*/
EXPORT_C CMMFMetaDataEntry* CVideoRecorderUtility::MetaDataEntryL(TInt aMetaDataIndex) const
	{
	return iBody->MetaDataEntryL(aMetaDataIndex);
	}
/**
Adds a new meta data entry to the video clip.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aNewEntry
        The meta data entry to add.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::AddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry)
	{
	iBody->AddMetaDataEntryL(aNewEntry);
	}
/**
Removes a meta data entry.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aIndex
        The index of the meta data entry to remove.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::RemoveMetaDataEntryL(TInt aIndex)
	{
	iBody->RemoveMetaDataEntryL(aIndex);
	}
/**
Replaces a meta data entry.        

The meta data entry specified by aIndex is replaced by the new data specified in aNewEntry. The 
index must be a value between 0 and NumberOfMetaDataEntriesL()-1.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aIndex
        The index of the meta data entry to replace.
@param  aNewEntry
        The meta data entry to replace the specified entry.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::ReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry)
	{
	iBody->ReplaceMetaDataEntryL(aIndex, aNewEntry);
	}
/**
Sets the recording priority. This is used to arbitrate between simultaneous accesses of
the sound hardware.

@param  aPriority
        The Priority Value.
@param  aPref
        The Priority Preference.

@see CVideoRecorderUtility::NewL()

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetPriorityL(TInt aPriority, TInt aPref)
	{
	iBody->SetPriorityL(aPriority, aPref);
	}
/**
Returns the length of the video clip.

@return The length of the video clip in microseconds.

@since 7.0s
*/
EXPORT_C TTimeIntervalMicroSeconds CVideoRecorderUtility::DurationL() const
	{
	return iBody->DurationL();
	}
/**
Returns the video bit rate.

@return The video bit rate in bits/second or KMMFVariableVideoBitRate.

@since 7.0s
*/
EXPORT_C TInt CVideoRecorderUtility::VideoBitRateL()
	{
	return iBody->VideoBitRateL();
	}
/**
Sets the video bit rate.

@param  aVideoBitRate
        The video bit rate in bits/second.
        This can be called with the parameter KMMFVariableVideoBitRate
        which specifies a variable bit rate.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetVideoBitRateL(TInt aVideoBitRate)
	{
	iBody->SetVideoBitRateL(aVideoBitRate);
	}
/**
Sets the audio bit rate.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aAudioBitRate
        The audio bit rate in bits/second.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetAudioBitRateL(TInt aAudioBitRate)
	{
	iBody->SetAudioBitRateL(aAudioBitRate);
	}
/**
Gets the audio bit rate.

If an error occurs, this function leaves with one of the system wide error codes.

@return The audio bit rate in bits/second.

@since 7.0s
*/
EXPORT_C TInt CVideoRecorderUtility::AudioBitRateL() const
	{
	return iBody->AudioBitRateL();
	}
/**
Returns the status of audio support for the video clip currently being recorded.

@return A boolean indicating if an audio stream is present. ETrue if it is, otherwise EFalse.

@since 7.0s
*/
EXPORT_C TBool CVideoRecorderUtility::AudioEnabledL() const
	{
	return iBody->AudioEnabledL();
	}
/**
Sets whether the current clip has an audio stream.

@param  aEnabled
        A boolean indicating if the audio stream is enabled. ETrue to enable audio, EFalse to 
        disable it.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetAudioEnabledL(TBool aEnabled)
	{
	iBody->SetAudioEnabledL(aEnabled);
	}
/**
Sets the audio recording gain.

If you want to disable audio, rather than setting the recording gain to 0, use SetAudioEnabledL().

If an error occurs, this function leaves with one of the system wide error codes

@param  aGain
        The recording gain, between 0 and MaxGain().

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetGainL(TInt aGain)
	{
	iBody->SetGainL(aGain);
	}
/**
Returns the current recording gain.

If an error occurs, this function leaves with one of the system wide error codes.

@return The current recording gain, between 0 and MaxGain().

@since 7.0s
*/
EXPORT_C TInt CVideoRecorderUtility::GainL() const
	{
	return iBody->GainL();
	}
/**
Returns the maximum possible setting for the recording gain.

@return An integer value specifying the maximum recording gain.

@since 7.0s
*/
EXPORT_C TInt CVideoRecorderUtility::MaxGainL() const
	{
	return iBody->MaxGainL();
	}
/**
Sets the maximum recording size for a video clip in bytes.
															   
If an error occurs, this function leaves with one of the system wide error codes.

@param  aClipSizeInBytes
        The maximum file size of a video clip.
        This can be called with the parameter KMMFNoMaxClipSize
        which signifies no maximum clip size.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetMaxClipSizeL(TInt aClipSizeInBytes) 
	{
	iBody->SetMaxClipSizeL(aClipSizeInBytes);
	}
/**
Sets the video frame size in pixels.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aSize
        The video frame size, in pixels

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetVideoFrameSizeL(const TSize& aSize) 
	{
	iBody->SetVideoFrameSizeL(aSize);
	}
/**
Gets the current recording priority. This is used to arbitrate between simultaneous accesses of
the sound hardware.

@param  aPriority
        A priority between EMdaPriorityMin and EMdaPriorityMax. EMdaPriorityMin means the client
        can be interrupted by any other client, EMdaPriorityNormal means the client can only
        be interrupted by a client with a higher priority or EMdaPriorityMax means the client
        cannot be interrupted by other clients.
@param  aPref
        Priority preference

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::GetPriorityL(TInt& aPriority, TMdaPriorityPreference& aPref) const 
	{
	iBody->GetPriorityL(aPriority, aPref);
	}
/**
Sets the audio type. This represents the encoding of the audio
data within the current format.

To retrieve a list of available audio codecs, use GetSupportedAudioTypesL().

If an error occurs, this function leaves with one of the system wide error codes.

@param  aDataType
        The Four Character Code representing the Audio Type.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetAudioTypeL(TFourCC aDataType)
	{
	iBody->SetAudioTypeL(aDataType);
	}
/**
Sets the codec to use for the video component of the video clip.

This represents the encoding of the video data within the current format. To retrieve a list of 
available video codecs, use GetSupportedVideoTypesL().

If an error occurs, this function leaves with one of the system wide error codes.

@param  aDataType
        The descriptor containing the video MIME type.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::SetVideoTypeL(const TDesC8& aDataType)
	{
	iBody->SetVideoTypeL(aDataType);
	}

EXPORT_C void Reserved1()
	{
	// reserved function replacing removed VideoTypeL() method
	// this function should never be called hence generate a
	// panic with code ENoVideoTypeL
	Panic( ENoVideoTypeL );
	}
/**
Returns the audio type. This represents the encoding of audio data within the current format.

If an error occurs, this function leaves with one of the system wide error codes.

@return A Four Character Code representing the AudioType.

@since 7.0s
*/
EXPORT_C TFourCC CVideoRecorderUtility::AudioTypeL() const
	{
	return iBody->AudioTypeL();
	}
/**
Gets a list of the audio types that are supported for recording to.

@param  aAudioTypes
        An array of Four Character Codes representing the audio types supported.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::GetSupportedAudioTypesL(RArray<TFourCC>& aAudioTypes) const
	{
	iBody->GetSupportedAudioTypesL(aAudioTypes);
	}
/**
Gets a list of the video types that are supported for recording to.

@param  aVideoTypes
        An array of Four Character Codes representing the video types supported.

@since 7.0s
*/
EXPORT_C void CVideoRecorderUtility::GetSupportedVideoTypesL(CDesC8Array& aVideoTypes) const
	{
	iBody->GetSupportedVideoTypesL(aVideoTypes);
	}
/**
Returns the (possibly estimated) record time left in the clip.

@return The record time available, in microseconds.

@since 7.0s
*/
EXPORT_C TTimeIntervalMicroSeconds CVideoRecorderUtility::RecordTimeAvailable() const
	{
	return iBody->RecordTimeAvailable();
	}
/**
Sends a synchronous custom command to the controller.

@param  aDestination
        The destination of the message, consisting of the UID of the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called on the interface defined 
        in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller framework. The
        exact contents of the data are dependent on the interface being called. Can be 
        KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called. Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client. Can't be KNullDesC8.

@return The result of the request. Exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CVideoRecorderUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	return iBody->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
/**
Sends a synchronous custom command to the controller.

@param  aDestination
        The destination of the message, consisting of the UID of the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called. Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called. Can be KNullDesC8.

@return The result of the request. The exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CVideoRecorderUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	return iBody->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
/**
Sends an asynchronous custom command to the controller.

Note: 
This method will return immediately. The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the controller framework.

@param  aDestination  
        The destination of the message, consisting of the UID of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called. Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called. Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client. Can't be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object.  This will contain the
        result of the request on completion.  The exact range of
        result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CVideoRecorderUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iBody->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
/**
Sends an asynchronous custom command to the controller.

Note: 
This method will return immediately. The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the
controller framework.

@param  aDestination
        The destination of the message, consisting of the UID of
        the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called
        on the interface defined in the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called. Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller
        framework. The exact contents of the data are dependent on the
        interface being called. Can be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object. This will contain the
        result of the request on completion. The exact range of
        result values are dependent on the interface.

@since  7.0s
*/	
EXPORT_C void CVideoRecorderUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iBody->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}
/**
Returns the controller implementation information associated with the current controller.

@return The controller implementation structure associated with the controller.

@since 7.0s
*/	
EXPORT_C const CMMFControllerImplementationInformation& CVideoRecorderUtility::ControllerImplementationInformationL()
	{
	return iBody->ControllerImplementationInformationL();
	}
/**
Returns the video format's MIME type.

@return The video clip's MIME type.

@since 7.0s
*/
EXPORT_C const TDesC8& CVideoRecorderUtility::VideoFormatMimeType() const
	{
	return iBody->VideoFormatMimeType();
	}
	
/**
Opens a file that can be used to store a recorded video clip.

If an error occurs, this function leaves with one of the system wide error codes.

@param  aFile
        File handle of file to use for video clip
@param  aCameraHandle
        A handle to the camera to use for recording.
@param  aControllerUid
        The UID of the controller to use for recording.
@param  aVideoFormatUid
        The UID of the video format to record to.
@param  aVideoCodec
        The descriptor containing the video MIME type.
@param  aAudioCodec
        A four character code representing the encoding to be used for audio data.

@since 7.0s
*/

EXPORT_C void CVideoRecorderUtility::OpenFileL(const RFile& aFile,
											   TInt aCameraHandle,
											   TUid aControllerUid,
											   TUid aVideoFormatUid,
											   const TDesC8& aVideoCodec,
											   TFourCC aAudioCodec)
	{
	iBody->OpenFileL(aFile, aCameraHandle, aControllerUid, aVideoFormatUid, aVideoCodec, aAudioCodec);
	}

/**
Sets recorded video pixel aspect ratio. The client must select the
video codec and set the picture size (SetVideoFrameSizeL()) before
calling this method, since the supported aspect ratios may depend on
the codec and picture size. If the codec or picture size is not set,
the controller may accept or reject pixel aspect ratios incorrectly.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.
This method can be called after the destination has been opened, 
but only before calling Prepare().

@pre 	The video codec has been selected.

@param 	aAspectRatio 
		Video pixel aspect ratio. The default aspect ratio 
		is 1:1 (square pixels).
@leave 	KErrNotSupported
		The specified pixel aspect ratio is not supported or the 
		controller does not support the pixel aspect ratio extension.
@leave 	KErrNotReady 
		Invalid state: This method can be called after the destination 
		has been opened but only before calling Prepare().
*/
EXPORT_C void CVideoRecorderUtility::SetPixelAspectRatioL(const TVideoAspectRatio& aAspectRatio)
	{
	iBody->SetPixelAspectRatioL(aAspectRatio);
	}
	
/** 
Gets the current pixel aspect ratio.

If an error occurs, this function leaves with one of the system
wide error codes. This method can be called after the destination 
has been opened.

@pre 	The destination file, descriptor, or URL has been opened.

@param 	aAspectRatio
		The current video pixel aspect ratio.
@leave 	KErrNotSupported
		The controller does not support the pixel aspect ratio extension.
@leave 	KErrNotReady
		Invalid state: This method can be called after the destination 
		has been opened.
*/
EXPORT_C void CVideoRecorderUtility::GetPixelAspectRatioL(TVideoAspectRatio& aAspectRatio) const
	{
	iBody->GetPixelAspectRatioL(aAspectRatio);
	}
	
/** 
Gets a list of supported pixel aspect ratios. The client must select the
video codec and set the picture size (SetVideoFrameSizeL()) before
calling this method, since the supported aspect ratios may depend on
the codec and picture size. If the codec or picture size is not set,
the returned list may not be correct.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@pre 	The video codec has been selected.

@param 	aAspectRatios 
		On return contains an array of supported pixel aspect ratios.
@leave 	KErrNotSupported 
		The controller does not support the pixel aspect ratio extension.
@leave 	KErrNotReady
		Invalid state: This method can be called after the destination 
		has been opened and the video codec selected.
*/
EXPORT_C void CVideoRecorderUtility::GetSupportedPixelAspectRatiosL(RArray<TVideoAspectRatio>& aAspectRatios) const
	{
	iBody->GetSupportedPixelAspectRatiosL(aAspectRatios);
	}
	
/** 
Sets the number of audio channels to record (1 for mono, 2 for stereo).
The client must select the audio codec before calling this method, since
not all codecs support stereo or multi-channel recording. If the codec
has not been set, this method may accept or reject configurations
incorrectly.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.
This method can be called after the
destination has been opened, but only before calling Prepare().

@pre 	The audio codec has been selected.

@param 	aNumChannels 
		Number of audio channels to use -- 1 for mono, 2 for stereo.
@leave 	KErrNotSupported 
		The specified number of channels is not supported or the controller 
		does not support controlling the number of audio channels.
@leave 	KErrNotReady
		Invalid state: This method can be called after the destination 
		has been opened, but only before calling Prepare().
*/
EXPORT_C void CVideoRecorderUtility::SetAudioChannelsL(const TUint aNumChannels)
	{
	iBody->SetAudioChannelsL(aNumChannels);
	}
	
/** 
Returns the current number of audio channels to record (1 for mono, 2 for stereo).

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.
This method can be called after the destination has been opened.

@pre 	The destination file, descriptor, or URL has been opened.

@leave 	KErrNotSupported
		The controller does not support controlling the number of audio 
		channels.
@leave 	KErrNotReady
		Invalid state: This method can be called after the destination 
		has been opened.
@return The number of audio channels to be recorded.
*/
EXPORT_C TUint CVideoRecorderUtility::AudioChannelsL() const
	{
	return(iBody->AudioChannelsL());
	}
	
/** 
Gets the list of the supported numbers of audio channels. One channel
represents mono, two channels stereo and so on. This method can only be called
after the audio codec has been selected, otherwise the list may be
incorrect.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@pre 	The audio codec has been selected.

@param 	aChannels
		On return contains an array of supported numbers of audio channels.
@leave 	KErrNotSupported
		The controller does not support controlling the number of audio 
		channels.
@leave 	KErrNotReady
		Invalid state: This method can be called after the audio codec has been selected.
*/  
EXPORT_C void CVideoRecorderUtility::GetSupportedAudioChannelsL(RArray<TUint>& aChannels) const
	{
	iBody->GetSupportedAudioChannelsL(aChannels);
	}

/** 
Sets the sample rate for audio recording.
The client must select the audio codec before calling this method, since
the supported sample rates depend on the codec. If the codec
has not been set, this method may accept or reject configurations
incorrectly.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@pre 	The audio codec has been selected.

@param 	aSampleRate
		Sample rate for audio recording.
@leave 	KErrNotSupported
		The specified sample rate is not supported or the controller does 
		not support controlling the audio sample rate.
@leave 	KErrNotReady
		Invalid state: This method can be called after the destination 
		has been opened and the audio codec selected, but only before calling Prepare().
*/
EXPORT_C void CVideoRecorderUtility::SetAudioSampleRateL(const TUint aSampleRate)
	{
	iBody->SetAudioSampleRateL(aSampleRate);
	}

/** 
Returns the current audio sample rate.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.
This method can be called after the
destination has been opened.

@pre 	The destination file, descriptor, or URL has been opened.

@leave 	KErrNotSupported
		The controller does not support controlling the audio sample rate.
@leave 	KErrNotReady
		Invalid state: This method can be called after the destination 
		has been opened.
@return The current audio sample rate.
*/
EXPORT_C TUint CVideoRecorderUtility::AudioSampleRateL() const
	{
	return(iBody->AudioSampleRateL());
	}
	
/** 
Gets the list of the supported audio sample rates.
The client must select the audio codec before calling this method, since
the supported sample rates depend on the codec. If the codec
has not been set, the returned list may not be correct.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@pre 	The audio codec has been selected.

@param 	aSampleRates
		On return contains an array of supported sample rates.
@leave 	KErrNotSupported
		The controller does not support controlling the audio sample rate.
@leave 	KErrNotReady
		Invalid state: This method can be called after the audio codec has been selected.
*/  
EXPORT_C void CVideoRecorderUtility::GetSupportedAudioSampleRatesL(RArray<TUint>& aSampleRates) const
	{
	iBody->GetSupportedAudioSampleRatesL(aSampleRates);
	}

/** 
Enables or disables video recording without affecting audio recording.
This API can be used to record audio only clips. Video recording is enabled by
default. This can only be called before starting to record.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@param aEnabled ETrue to enable video recording, EFalse to disable it

@pre Recorder is created opened, prepare is not yet called.
	 This API can not be called after prepare.

@leave KErrNotReady Open is not yet called or This API is being called after Prepare().
@leave KErrNotSupported This method is not supported -- video recording
         is always enabled

@publishedPartner
*/
EXPORT_C void CVideoRecorderUtility::SetVideoEnabledL(TBool aEnabled)
	{
	iBody->SetVideoEnabledL(aEnabled);
	}

/** 
Queries whether video recording is currently enabled or not.
If video recording is disabled it will record audio only clip.
Video recording is enabled by default.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@return ETrue if video recording is enabled, EFalse if not

@see CVideoRecorderUtility::SetVideoEnabledL()

@publishedPartner
*/
EXPORT_C TBool CVideoRecorderUtility::VideoEnabledL() const
	{
	return iBody->VideoEnabledL();
	}

/** 
Controls video recording quality. Setting the video quality using this
method will override other bitrate setting, but will not affect picture
size, and framerate if VideoFrameRateFixed is set.
Changing the bitrate or framerate would override the quality settings.
The return value from VideoQualityL() will not reflect the quality 
accurately after this.

@param aQuality Video quality, in the range of 0 to 100, inclusive. 0
                refers to lowest, 25 refers to low, 50 refers to normal,
			   75 refers to high, and 100 refers to lossless quality
			   as is supported by the codec. Standard set of values 
			   defined by enum TVideoQuality. 

@pre Recorder is created opened, prepare is not yet called.
	 This API can not be called after Prepare().

@leave KErrNotReady Open is not yet called or This API is being called after Prepare().
@leave KErrNotSupported This method is not supported.
@see TVideoQuality

@publishedPartner
*/
EXPORT_C void CVideoRecorderUtility::SetVideoQualityL(TInt aQuality)
	{
	iBody->SetVideoQualityL(aQuality);
	}

/** 
Queries the current video quality setting. The return value may not
reflect bitrate and framerate settings made through SetVideoBitRateL()
and SetVideoFrameRateL(). Value returned indicate just the quality 
level set using SetVideoQualityL().
Default value for Quality is EVideoQualityNormal (value 50).

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@return Video quality, in the range of 0 to 100, inclusive.

@leave KErrNotReady Open is not yet called.
@see CVideoRecorderUtility::SetVideoQualityL()

@publishedPartner
*/
EXPORT_C TInt CVideoRecorderUtility::VideoQualityL() const
	{
	return iBody->VideoQualityL();
	}

/**
Controls whether video recording framerate set using the API the
SetVideoFrameRateL should be fixed or variable.
When fixed framerate is requested, the video encoder will try to
maintain a constant framerate, even at the expense of picture quality.
This will result in a reduced picture quality at lower bitrates, but
a smoother framerate.

The fixed framerate setting is not an absolute requirement -- encoders
may still skip frames even though a fixed framerate is required, but
they should do it less often. Additionally, many encoders will operate
at a fixed framerate at higher bitrates regardless of this setting.

By default the framerate is not fixed.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@param aFixedFrameRate ETrue to request a fixed framerate, EFalse to
                      request a variable framerate.
@pre Recorder is created opened, prepare is not yet called.
	 This API can not be called after prepare.

@leave KErrNotReady Open is not yet called or This API is being called after Prepare().
@leave KErrNotSupported This method is not supported.

@publishedPartner
*/
EXPORT_C void CVideoRecorderUtility::SetVideoFrameRateFixedL(TBool aFixedFrameRate)
	{
	iBody->SetVideoFrameRateFixedL(aFixedFrameRate);
	}

/**
Queries the current fixed/variable framerate setting.

If an error occurs, this function leaves with any of the system
wide error codes. Common error codes are listed below.

@return ETrue if the framerate is requested to be fixed, EFalse if
        variable

@leave KErrNotReady Open is not yet called or This API is being called after Prepare().
@see CVideoRecorderUtility::SetVideoFrameRateFixedL()

@publishedPartner
*/
EXPORT_C TBool CVideoRecorderUtility::VideoFrameRateFixedL() const
	{
	return iBody->VideoFrameRateFixedL();
	}
