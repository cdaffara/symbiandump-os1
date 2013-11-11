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

#include <e32std.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/common/mmfpaniccodes.h>
#include <mmfformatimplementationuids.hrh>

#include "mmfclientaudioconverter.h"

// declared in the recorder module
void Panic(TInt aPanicCode);

//CMdaAudioConvertUtility


/**
Returns the current utility state.

@return The state of the audio sample data. See CMdaAudioClipUtility::TState.

@since  5.0
*/
CMdaAudioClipUtility::TState CMdaAudioConvertUtility::State()
	{
	ASSERT(iProperties);
	return iProperties->State();
	}

/**
Closes the current audio clip.

@since  5.0
*/
void CMdaAudioConvertUtility::Close()
	{
	ASSERT(iProperties);
	iProperties->Close();
	}

/**
This function is mapped to ConvertL(). ConvertL() should be used instead.

@see ConvertL()

@since  5.0
*/
void CMdaAudioConvertUtility::PlayL()
	{
	ASSERT(iProperties);
	iProperties->PlayL();
	}

/**
This function is mapped to ConvertL. ConvertL() should be used instead.

@since  5.0
*/
void CMdaAudioConvertUtility::RecordL()
	{
	ASSERT(iProperties);
	iProperties->RecordL();
	}

/**
Stops the current operation (playback/recording/conversion).

When conversion has been stopped, successfully or otherwise, the client is notified by 
MMdaObjectStateChangeObserver::MoscoStateChangeEvent(). The callback is initiated by this function 
providing it with state change information and error codes.

@since  5.0
*/
void CMdaAudioConvertUtility::Stop()
	{
	ASSERT(iProperties);
	iProperties->Stop();
	}

/**
Crops the current clip from the current position. The remainder of the clip is discarded.

The effects of the function cannot be undone. The function is synchronous and can leave if there is 
a problem. The leave code depends on the configuration.

@since  5.0
*/
void CMdaAudioConvertUtility::CropL()
	{
	ASSERT(iProperties);
	iProperties->CropL(ETrue);
	}

/**
Sets the current position in the audio clip.

A subsequent call to ConvertL() starts conversion from this new position.

@param  aPosition
        The position in the audio clip, in microseconds.

@since  5.0
*/
void CMdaAudioConvertUtility::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	ASSERT(iProperties);
	iProperties->SetPosition(aPosition);
	}

/**
Returns the current position in the audio clip. The position is defined in terms of a time interval
measured from the beginning of the audio sample data.

@return The current position in the audio clip, in microseconds.

@since  5.0
*/
const TTimeIntervalMicroSeconds& CMdaAudioConvertUtility::Position()
	{
	ASSERT(iProperties);
	return iProperties->Position();
	}

/**
Returns the amount of recording time available to the current clip.

@return The recording time available measured in microseconds.

@since  5.0
*/
const TTimeIntervalMicroSeconds& CMdaAudioConvertUtility::RecordTimeAvailable()
	{
	ASSERT(iProperties);
	return iProperties->RecordTimeAvailable();
	}

/**
Returns the duration of the audio clip.

@return The duration in microseconds.

@since  5.0
*/
const TTimeIntervalMicroSeconds& CMdaAudioConvertUtility::Duration()
	{
	ASSERT(iProperties);
	return iProperties->Duration();
	}

/**
Sets a window for playback.

The window is defined in terms of a start and end time. A subsequent call to ConvertL() results 
in just the data within the window being converted.

@param  aStart
        The position in the clip to start playback, in microseconds. This must be any value from zero
        to aEnd. If this value is less than zero or greater than aEnd, this function raises 
        EMMFMediaClientPanicServerCommunicationProblem panic in debug version.
@param  aEnd
        The position in the clip to end playback, in microseconds. This must be any value from aStart 
        to the value returned by Duration(). If this value is greater than the value returned by 
        Duration() or less than aStart, this function raises EMMFMediaClientPanicServerCommunicationProblem panic in debug version.

@since  5.0
*/
void CMdaAudioConvertUtility::SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd)
	{
	ASSERT(iProperties);
	iProperties->SetPlayWindow(aStart, aEnd);
	}

/**
Clears the playback window. 

Playback returns to playing the entire clip. A subsequent call to ConvertL() results in the entire 
source audio being converted.

@since  5.0
*/
void CMdaAudioConvertUtility::ClearPlayWindow()
	{
	ASSERT(iProperties);
	iProperties->ClearPlayWindow();
	}

/**
Sets the number of repetitions for playback. This is unsupported for CMdaConvertUtility as there is
no playback facility. It is provided only for binary compatibility.

@param  aRepeatNumberOfTimes
        The number of times to repeat the audio clip, after the first playback. If this is set to 
        KMdaRepeatForever, then the audio sample, together with the trailing silence, is repeated 
        indefinitely. If this is set to zero, then the audio sample is not repeated.
@param  aTrailingSilence
        A delay to wait before each repetition.

@since  5.0
*/
void CMdaAudioConvertUtility::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	ASSERT(iProperties);
	iProperties->SetRepeats(aRepeatNumberOfTimes, aTrailingSilence);
	}

/**
Sets the maximum size of an audio clip.

This function is provided so that applications such as recorders and converters can limit the amount 
of file storage/memory that should be allocated.

@param  aMaxWriteLength
        The maximum size of the audio clip, in bytes. If the default value is used, there is no 
        maximum file size.

@since 7.0
*/
void CMdaAudioConvertUtility::SetMaxWriteLength(TInt aMaxWriteLength /* = KMdaClipLocationMaxWriteLengthNone*/)
	{
	ASSERT(iProperties);
	iProperties->SetMaxWriteLength(aMaxWriteLength);
	}

/**
Crops the audio clip from the start of the file to the current position. The audio data
prior to the current position is discarded.

The effects of the function cannot be undone.

The function is synchronous and can leave if there is a problem. The leave code depends on the
configuration.

@since 7.0s
*/
void CMdaAudioConvertUtility::CropFromBeginningL()
	{
	ASSERT(iProperties);
	iProperties->CropL(EFalse);
	}

/**
Constructs and initialises a new instance of the audio converter for converting audio sample data 
from one format to another.

The function leaves if the audio converter object cannot be created.

@param  aObserver
        The class to receive state change events from converter.
@param  aServer
        No longer used, should be NULL.
@param  aPriority
        The Priority Value - this client's relative priority. This is a value between EMdaPriorityMin and 
        EMdaPriorityMax and represents a relative priority. A higher value indicates a more important request.
@param  aPref
        The Priority Preference - an additional audio policy parameter. The suggested default is 
        EMdaPriorityPreferenceNone. Further values are given by TMdaPriorityPreference, and additional 
        values may be supported by given phones and/or platforms, but should not be depended upon by 
        portable code.

@return A pointer to a new instance of CMdaAudioConvertUtility.

@since  5.0

Note: The Priority Value and Priority Preference are used primarily when deciding what to do when
several audio clients attempt to play or record simultaneously. In addition to the Priority Value and Preference, 
the adaptation may consider other parameters such as the SecureId and Capabilities of the client process. 
Whatever, the decision  as to what to do in such situations is up to the audio adaptation, and may
vary between different phones. Portable applications are advised not to assume any specific behaviour. 
*/
EXPORT_C CMdaAudioConvertUtility* CMdaAudioConvertUtility::NewL(MMdaObjectStateChangeObserver& aObserver,
																CMdaServer* /*aServer*/,
																TInt aPriority,
																TInt aPref)
	{
	CMdaAudioConvertUtility* self = new(ELeave) CMdaAudioConvertUtility();
	CleanupStack::PushL(self);
	self->iProperties = new(ELeave) CMMFMdaAudioConvertUtility(self, aObserver);
	self->iProperties->ConstructL(aPriority, aPref);
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor. 

Closes the audio clip and frees resources.

@since  5.0
*/
CMdaAudioConvertUtility::~CMdaAudioConvertUtility()
	{
	delete iProperties;
	}

/**
Ensures that any subsequent calls to OpenXYZ() will create controllers that
share a heap.

The default behaviour is that for each converter utility a controller with its own heap
is created. Each heap uses a chunk, so using this function avoids situations where 
the number of chunks per process is limited.
The default behaviour is generally to be preferred, and should give lower overall
memory usage. However, if many controllers are to be created for a particular thread,
then this function should be used to prevent running out of heaps or chunks.

@since	9.2
*/
EXPORT_C void CMdaAudioConvertUtility::UseSharedHeap()
	{
	ASSERT(iProperties);
	iProperties->UseSharedHeap();
	}
	
/**
Opens source and target files (both of which must already exist) so that audio sample data can be 
extracted from the source file, converted and appended to the target file.

When opening is complete, successfully or otherwise, the client is notified by 
MMdaObjectStateChangeObserver::MoscoStateChangeEvent(). The callback is initiated by this function 
providing it with state change information and error codes.

@param  aPrimaryFile
        The full path and filename of a file containing audio sample data to be converted.
@param  aSecondaryFile
        The full path and filename of a file to which converted data is appended.

@since  5.0
*/
void CMdaAudioConvertUtility::OpenL(const TDesC& aPrimaryFile, const TDesC& aSecondaryFile)
	{
	ASSERT(iProperties);
	iProperties->OpenL(aPrimaryFile, aSecondaryFile);
	}

/**
Opens an existing audio file so that audio sample data can be extracted from it, converted and 
placed into the target audio object.

When opening is complete, successfully or otherwise, the client is notified by 
MMdaObjectStateChangeObserver::MoscoStateChangeEvent(). The callback is initiated by this function 
providing it with state change information and error codes.

@param  aPrimaryFile
        The full path and filename of a file containing audio sample data to be converted.
@param  aLocation
        The target location for the audio data. This is normally a file (TMdaFileClipLocation) or a 
        descriptor (TMdaDesClipLocation).
@param  aFormat
        The audio format for the target audio object.
@param  aArg1
        The codec to use for the target audio object.
@param  aArg2
        The audio settings for the target audio object.

@since  5.0
*/
void CMdaAudioConvertUtility::OpenL(const TDesC& aPrimaryFile, TMdaClipLocation* aLocation, TMdaClipFormat* aFormat,
									TMdaPackage* aArg1 /*= NULL*/, TMdaPackage* aArg2 /*= NULL*/)
	{
	ASSERT(iProperties);
	iProperties->OpenL(aPrimaryFile, aLocation, aFormat, aArg1, aArg2);
	}

/**
Opens an audio clip for conversion.

@param  aPriLocation
        The source location for audio data. This is normally a file (TMdaFileClipLocation) or a 
        descriptor (TMdaDesClipLocation).
@param  aSecLocation
        The target location for audio data. This is normally a file (TMdaFileClipLocation) or a 
        descriptor (TMdaDesClipLocation).
@param  aPriFormat
        The audio format of the source audio object.
@param  aSecFormat
        The audio format for the target audio object.
@param  aPriArg1
        The codec used by the source audio object.
@param  aPriArg2
        The audio settings of the source audio object.
@param  aSecArg1
        The codec to be used for the target audio object.
@param  aSecArg2
        The audio settings for the target audio object.

@since  5.0
*/
void CMdaAudioConvertUtility::OpenL(TMdaClipLocation* aPriLocation, TMdaClipLocation* aSecLocation,
									TMdaClipFormat* aPriFormat,  TMdaClipFormat* aSecFormat,
									TMdaPackage* aPriArg1 /*= NULL*/, TMdaPackage* aPriArg2 /*= NULL*/,
									TMdaPackage* aSecArg1 /*= NULL*/, TMdaPackage* aSecArg2 /*= NULL*/)
	{
	ASSERT(iProperties);
	iProperties->OpenL(aPriLocation, aSecLocation, aPriFormat, aSecFormat, aPriArg1, aPriArg2, aSecArg1, aSecArg2);
	}

// New functions for 7.0s
/**
Opens source and target audio objects and specifies controller and audio format and codec 
information.

When opening is complete, successfully or otherwise, the client is notified by 
MMdaObjectStateChangeObserver::MoscoStateChangeEvent(). The callback is initiated by this function
providing it with state change information and error codes.

@param  aPriLocation
        The source location for audio data. This is normally a file (TMdaFileClipLocation) or a 
        descriptor (TMdaDesClipLocation).
@param  aSecLocation
        The destination location for the converted audio data. This is normally a file 
        (TMdaFileClipLocation) or a descriptor (TMdaDesClipLocation).
@param  aControllerUid
        The UID of the controller to used for conversion.
@param  aDestFormatUid
        The UID of the destination format.
@param  aDestDataType
        The audio codec to use for the destination data sink.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::OpenL(TMdaClipLocation* aPriLocation, TMdaClipLocation* aSecLocation, TUid aControllerUid,
											  TUid aDestFormatUid, TFourCC aDestDataType)
	{
	ASSERT(iProperties);
	iProperties->OpenL(aPriLocation, aSecLocation, aControllerUid, aDestFormatUid, aDestDataType);
	}

/**
Returns a list of the supported data types for the conversion destination.

@param  aSupportedDestinationDataTypes
        A list of four character codes, representing the supported data
        encodings for the conversion destination.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDestinationDataTypes)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedDestinationDataTypesL(aSupportedDestinationDataTypes);
	}

/**
Sets the data type of the destination audio clip.

@param  aCodec
        The four character code, representing the encoding of the destination audio clip

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::SetDestinationDataTypeL(TFourCC aCodec)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationDataTypeL(aCodec);
	}

/**
Returns the data type of the destination audio clip.

@return The four character code, representing the encoding of the destination audio clip.

@since  7.0s
*/
EXPORT_C TFourCC CMdaAudioConvertUtility::DestinationDataTypeL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationDataTypeL();
	}

/**
Returns the data type of the source audio clip.

@return The four character code, representing the encoding of the source audio clip.

@since  7.0s
*/
EXPORT_C TFourCC CMdaAudioConvertUtility::SourceDataTypeL()
	{
	ASSERT(iProperties);
	return iProperties->SourceDataTypeL();
	}

/**
Sets the bit rate of the destination audio clip.

The bit rate must be one of the supported bit rates of the audio target. Use
GetSupportedConversionBitRatesL() to retrieve a list of supported bit rates.

@param  aBitRate
        The destination bit rate in bits/second.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::SetDestinationBitRateL(TUint aBitRate)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationBitRateL(aBitRate);
	}

/**
Returns the bit rate of the destination audio clip.

@return The destination bit rate in bits/second.

@since  7.0s
*/
EXPORT_C TUint CMdaAudioConvertUtility::DestinationBitRateL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationBitRateL();
	}

/**
Gets a list of the supported bit rates for the conversion destination.

@param  aSupportedBitRates
        The list of bit rates supported for the conversion destination.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::GetSupportedConversionBitRatesL(RArray<TUint>& aSupportedBitRates)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedConversionBitRatesL(aSupportedBitRates);
	}

/**
Returns the bit rate of the source audio clip.

@return The source bit rate in bits/second.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioConvertUtility::SourceBitRateL()
	{
	ASSERT(iProperties);
	return iProperties->SourceBitRateL();
	}

/**
Sets the sample rate for the conversion destination.

The sample rate must be one of the supported sample rates of the audio target. Use 
GetSupportedConversionSampleRatesL() to retrieve a list of supported sample rates.
This function should not be used if the audio clip already exists; that is, in the
"Open and Append" scenario, when the function's behaviour is undefined.

@param  aSampleRate
        The sample rate of the conversion destination in samples per second.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::SetDestinationSampleRateL(TUint aSampleRate)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationSampleRateL(aSampleRate);
	}

/**
Returns the sample rate of the conversion destination.

@return The sample rate of the conversion destination in samples per second.

@since  7.0s
*/
EXPORT_C TUint CMdaAudioConvertUtility::DestinationSampleRateL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationSampleRateL();
	}

/**
Gets a list of supported conversion sample rates. This is a list of the sample rates that
the conversion destination can take.

@param  aSupportedSampleRates
        A list of the sample rates that are supported for the conversion.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::GetSupportedConversionSampleRatesL(RArray<TUint>& aSupportedSampleRates)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedConversionSampleRatesL(aSupportedSampleRates);
	}

/**
Returns the sample rate of the source audio clip.

@return The source sample rate in samples/second.

@since  7.0s
*/
EXPORT_C TUint CMdaAudioConvertUtility::SourceSampleRateL()
	{
	ASSERT(iProperties);
	return iProperties->SourceSampleRateL();
	}

/**
Sets the format of the destination audio clip.

The UID (aFormatUid) corresponds to the UID of the destination format to use.

@param  aFormatUid
        The UID of the destination format.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::SetDestinationFormatL(TUid aFormatUid)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationFormatL(aFormatUid);
	}

/**
Returns the format of the destination audio clip.

@return The UID of the destination format.

@since  7.0s
*/
EXPORT_C TUid CMdaAudioConvertUtility::DestinationFormatL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationFormatL();
	}

/**
Returns the format of the source audio clip.

@return The UID of the source format.

@since  7.0s
*/
EXPORT_C TUid CMdaAudioConvertUtility::SourceFormatL()
	{
	ASSERT(iProperties);
	return iProperties->SourceFormatL();
	}

/**
Sets the number of channels the destination audio clip contains.

The number of channels must be one of the values returned by 
GetSupportedConversionNumberOfChannelsL().

@param  aNumberOfChannels
        The number of channels.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::SetDestinationNumberOfChannelsL(TUint aNumberOfChannels)
	{
	ASSERT(iProperties);
	iProperties->SetDestinationNumberOfChannelsL(aNumberOfChannels);
	}

/**
Returns the number of channels the destination audio clip contains.

@return The number of channels.

@since  7.0s
*/
EXPORT_C TUint CMdaAudioConvertUtility::DestinationNumberOfChannelsL()
	{
	ASSERT(iProperties);
	return iProperties->DestinationNumberOfChannelsL();
	}

/**
Returns a list of the supported number of channels for conversion.

For example, 1 for mono, 2 for stereo and so on.

@param  aSupportedNumChannels
        A list of the number of channels supported for the conversion destination.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::GetSupportedConversionNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels)
	{
	ASSERT(iProperties);
	iProperties->GetSupportedConversionNumberOfChannelsL(aSupportedNumChannels);
	}

/**
Returns the number of channels used by the conversion source.

@return The number of channels.

@since  7.0s
*/
EXPORT_C TUint CMdaAudioConvertUtility::SourceNumberOfChannelsL()
	{
	ASSERT(iProperties);
	return iProperties->SourceNumberOfChannelsL();
	}
/**
Performs the conversion from the source audio format to the destination.

When conversion is complete, successfully or otherwise, the client is notified by 
MMdaObjectStateChangeObserver::MoscoStateChangeEvent(). The callback is initiated by this function 
providing it with state change information and error codes.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::ConvertL()
	{
	ASSERT(iProperties);
	iProperties->ConvertL();
	}

/**
Returns the controller implementation information associated with the current controller.

@return The controller implementation structure associated with the controller

@since  7.0s
*/
EXPORT_C const CMMFControllerImplementationInformation& CMdaAudioConvertUtility::ControllerImplementationInformationL()
	{
	ASSERT(iProperties);
	return iProperties->ControllerImplementationInformationL();
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
@param  aDataFrom
        A reference to an area of memory to which the controller framework will
        write any data to be passed back to the client. Can't be KNullDesC8.

@return The result of the request. Exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioConvertUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	ASSERT(iProperties);
	return iProperties->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}

/**
Sends a synchronous custom command to the controller.

@param  aDestination
        The destination of the message, consisting of the UID of the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called on the interface defined in 
        the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controllert framework. The exact
        contents of the data are dependent on the interface being called. Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller framework. The exact
        contents of the data are dependent on the interface being called. Can be KNullDesC8.

@return The result of the request. The exact range of values is dependent on the interface.

@since  7.0s
*/
EXPORT_C TInt CMdaAudioConvertUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	ASSERT(iProperties);
	return iProperties->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}

/**
Send a asynchronous custom command to the controller.

Note: This method will return immediately.  The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the controller framework.

@param  aDestination
        The destination of the message, consisting of the UID of the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called on the interface defined in
        the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller framework. The exact
        contents of the data are dependent on the interface being called. Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller framework. The exact
        contents of the data are dependent on the interface being called. Can be KNullDesC8.
@param  aDataFrom
        A reference to an area of memory to which the controller framework will write any data to be
        passed back to the client. Can't be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object.  This will contain the result of the request on
        completion. The exact range of result values is dependent on the interface.
@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	ASSERT(iProperties);
	iProperties->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}

/**
Send a asynchronous custom command to the controller.

Note: This method will return immediately.  The RunL of the active object owning the
aStatus parameter will be called when the command is completed by the controller framework.

@param  aDestination
        The destination of the message, consisting of the UID of the interface of this message.
@param  aFunction
        The function number to indicate which function is to be called on the interface defined in 
        the aDestination parameter.
@param  aDataTo1
        A reference to the first chunk of data to be copied to the controller framework. The exact 
        contents of the data are dependent on the interface being called. Can be KNullDesC8.
@param  aDataTo2
        A reference to the second chunk of data to be copied to the controller framework. The exact 
        contents of the data are dependent on the interface being called. Can be KNullDesC8.
@param  aStatus
        The TRequestStatus of an active object. This will contain the result of the request on
        completion. The exact range of result values is dependent on the interface.

@since  7.0s
*/
EXPORT_C void CMdaAudioConvertUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	ASSERT(iProperties);
	iProperties->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
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
EXPORT_C TInt CMdaAudioConvertUtility::SetThreadPriority(const TThreadPriority& aThreadPriority) const
	{
	ASSERT(iProperties);
	return iProperties->SetThreadPriority(aThreadPriority);
	}


/******************************************************************************/


CMMFMdaAudioConvertUtility::CMMFMdaAudioConvertUtility(CMdaAudioConvertUtility* aParent,
													   MMdaObjectStateChangeObserver& aCallback) :
	iCallback(aCallback),
	iAudioControllerCustomCommands(iController),
	iAudioPlayControllerCustomCommands(iController),
	iAudioRecordControllerCustomCommands(iController),
	iParent(aParent),
	iHasCropped(EFalse),
	iCroppedDuration(0)
	{
	iConvertStart = TTimeIntervalMicroSeconds(0);
	iConvertEnd = TTimeIntervalMicroSeconds(0);
	iConvertWindowSet = EFalse;
	}

void CMMFMdaAudioConvertUtility::ConstructL(TInt /*aPriority*/, TInt /*aPref*/)
	{
	iControllerEventMonitor = CMMFControllerEventMonitor::NewL(*this, iController);
	iAsyncCallback = CMMFMdaObjectStateChangeObserverCallback::NewL(iCallback);
	User::LeaveIfError(iMediaIds.Append(KUidMediaTypeAudio));
	iFindAndOpenController = CMMFFindAndOpenController::NewL(*this);
	iFindAndOpenController->Configure(iMediaIds[0], iPrioritySettings);
	iFindAndOpenController->ConfigureController(iController, *iControllerEventMonitor, CMMFFindAndOpenController::EConvert);
	}

CMMFMdaAudioConvertUtility::~CMMFMdaAudioConvertUtility()
	{
	delete iControllerImplementationInformation;
	delete iAsyncCallback;
	delete iFindAndOpenController;
	delete iControllerEventMonitor;
	iMediaIds.Close();
	iController.Close();
	}

void CMMFMdaAudioConvertUtility::UseSharedHeap()
	{
	iFindAndOpenController->UseSharedHeap();
	}
	
void CMMFMdaAudioConvertUtility::MfaocComplete(		
		TInt& aError, 
		RMMFController* /*aController*/,
		TUid aControllerUid, 
		TMMFMessageDestination* /*aSourceHandle*/, 
		TMMFMessageDestination* /*aSinkHandle*/)
	{
	TInt oldState = iState;

	if (aError == KErrNone)
		{
		iControllerUid = aControllerUid;
		aError = ControllerOpen();
		if (iFindAndOpenController)	
			{
			iFindAndOpenController->Close();
			}
		}

	iAsyncCallback->CallBack(iParent, oldState, iState, aError);
	}

void CMMFMdaAudioConvertUtility::OpenL(TMdaClipLocation* aPriLocation, 
									   TMdaClipLocation* aSecLocation, 
									   TMdaClipFormat* aPriFormat, 
									   TMdaClipFormat* aSecFormat, 
									   TMdaPackage* aPriArg1 /*= NULL*/, 
									   TMdaPackage* aPriArg2 /*= NULL*/, 
									   TMdaPackage* aSecArg1 /*= NULL*/, 
									   TMdaPackage* aSecArg2 /*= NULL*/)
	{
	__ASSERT_ALWAYS((aPriLocation && aSecLocation && aPriFormat && aSecFormat), User::Leave(KErrArgument));

	__ASSERT_ALWAYS((((aPriLocation->Uid() == KUidMdaFileResLoc) || (aPriLocation->Uid() == KUidMdaDesResLoc) || (aPriLocation->Uid() == KUidMdaUrlResLoc)) &&
	   ((aSecLocation->Uid() == KUidMdaFileResLoc) || (aSecLocation->Uid() == KUidMdaDesResLoc) || (aSecLocation->Uid() == KUidMdaUrlResLoc))),
		User::Leave(KErrNotSupported));

	__ASSERT_ALWAYS(((aPriFormat->Uid() != KNullUid) && (aSecFormat->Uid() != KNullUid)), User::Leave(KErrNotSupported));

	Reset();
	
	//Do aPriArg2 & aSecArg2 contain the correct package type
	if(aPriArg2)
		{
		TMdaPackage* pckg = aPriArg2;
		if(pckg->Uid() != KUidMdaMediaTypeAudio)
			User::Leave(KErrNotSupported);

		TMdaAudioDataSettings audioSettings = *(TMdaAudioDataSettings*)aPriArg2;
		iSourceSampleRate = audioSettings.iSampleRate;
		iSourceChannels = audioSettings.iChannels;
		}

	if(aSecArg2)
		{
		TMdaPackage* pckg = aSecArg2;
		if(pckg->Uid() != KUidMdaMediaTypeAudio)
			User::Leave(KErrNotSupported);
		TMdaAudioDataSettings audioSettings = *(TMdaAudioDataSettings*)aSecArg2;
		iSinkSampleRate = audioSettings.iSampleRate;
		iSinkChannels = audioSettings.iChannels;
		}


	//Do aPriArg1 & aSecArg1 contain supported FourCC types
	iSourceDataType = KMMFFourCCCodeNULL;
	if (aPriArg1)
		{
		iSourceDataType = CMMFClientUtility::ConvertMdaCodecToFourCC(*aPriArg1);
		if(iSourceDataType == KMMFFourCCCodeNULL)
			User::Leave(KErrNotSupported);
		}

	iSinkDataType = KMMFFourCCCodeNULL;
	if (aSecArg1)
		{
		iSinkDataType = CMMFClientUtility::ConvertMdaCodecToFourCC(*aSecArg1);
		if(iSinkDataType == KMMFFourCCCodeNULL)
			User::Leave(KErrNotSupported);
		}



	TInt err = KErrNone;

	iSourceFormatUid = CMMFClientUtility::ConvertMdaFormatUidToECOMRead(aPriFormat->Uid());
	//If a new formatter plugin has been added, use the supplied read format implementation ID
	if (iSourceFormatUid == KNullUid)
		iSourceFormatUid = aPriFormat->Uid();

	iSinkFormatUid = CMMFClientUtility::ConvertMdaFormatUidToECOMWrite(aSecFormat->Uid());
	//If a new formatter plugin has been added, use the supplied write format implementation ID
	if (iSinkFormatUid == KNullUid)
		iSinkFormatUid = aSecFormat->Uid();

	TRAP(err, ConfigureSourceSinkL(aPriLocation, aSecLocation));

	if (!err)
		iFindAndOpenController->OpenByFormatUid(iSourceFormatUid, iSinkFormatUid);

	if (err)
		{
		TInt oldState = State();
		iAsyncCallback->CallBack(iParent, oldState, oldState, err);
		return;
		}
	}




void CMMFMdaAudioConvertUtility::OpenL(const TDesC& aPrimaryFile, const TDesC& aSecondaryFile)
	{
	Reset();
	
	iFindAndOpenController->ConfigureSourceSink(
		CMMFFindAndOpenController::TSourceSink(KUidMmfFileSource, CMMFFindAndOpenController::GetConfigFile(aPrimaryFile)), 
		CMMFFindAndOpenController::TSourceSink(KUidMmfFileSink, CMMFFindAndOpenController::GetConfigFile(aSecondaryFile)));

	TMMFileSource tfs(aPrimaryFile);
	iFindAndOpenController->OpenByFileSource(tfs, aSecondaryFile);
	}

void CMMFMdaAudioConvertUtility::OpenL(const TDesC& aPrimaryFile,
						TMdaClipLocation* aLocation,	// Normally file or descriptor
						TMdaClipFormat* aFormat,		// Data format
						TMdaPackage* aArg1,		// Normally codec to use
						TMdaPackage* aArg2)		// Normally audio settings
	{
	__ASSERT_ALWAYS(aLocation && aFormat, User::Leave(KErrArgument)); 

	Reset();

	// convert from the old parameters
	if (aFormat)
		{
		iSinkFormatUid = CMMFClientUtility::ConvertMdaFormatUidToECOMWrite(aFormat->Uid());
	
		//If a new formatter plugin has been added, use the supplied format ID
		if (iSinkFormatUid == KNullUid)
			iSinkFormatUid = aFormat->Uid();
		}

	if (aArg1)
		iSinkDataType = CMMFClientUtility::ConvertMdaCodecToFourCC(*aArg1);

	if (aArg2)
		{//have audio settings
		TMdaAudioDataSettings audioSettings = *(TMdaAudioDataSettings*)aArg2; //shoud check arg2 are data settings
		iSinkSampleRate = audioSettings.iSampleRate;
		iSinkChannels = audioSettings.iChannels;
		}


	
	TMMFFileConfig sourceCfg;
	sourceCfg().iPath = aPrimaryFile;

	TMMFDescriptorConfig dstDesCfg;
	TMMFFileConfig dstFileCfg;
	CBufFlat* dstCfgBuffer = NULL;
	CMMFUrlParams* dstURLCfg = NULL;
	TUid dstUid = KNullUid;
	TPtrC8 dstCfg(NULL, 0);

	TInt err = KErrNone;

	if (aLocation->Uid() == KUidMdaFileResLoc)
		{//sink clip location is a file
		TDesC& fileName = ((TMdaFileClipLocation*)aLocation)->iName;
		dstFileCfg().iPath = fileName;
		dstCfg.Set(dstFileCfg);
		dstUid = KUidMmfFileSink;
		}
	else if (aLocation->Uid() == KUidMdaDesResLoc)
		{//sink clip is a descriptor - pass down descriptor & thread id
		TMdaDesClipLocation* desLoc = (TMdaDesClipLocation*)aLocation;
		dstDesCfg().iDes = desLoc->iDes;
		dstDesCfg().iDesThreadId = desLoc->iThreadId;
		dstCfg.Set(dstDesCfg);
		dstUid = KUidMmfDescriptorSink;
		}
	else if (aLocation->Uid() == KUidMdaUrlResLoc)
		{
		TMdaUrlClipLocation* desLoc = (TMdaUrlClipLocation*)aLocation;
		dstURLCfg = CMMFUrlParams::NewLC(desLoc->iUrl, desLoc->iIapId);
		dstCfgBuffer = dstURLCfg->ExternalizeToCBufFlatLC();
		dstCfg.Set(dstCfgBuffer->Ptr(0));
		dstUid = KUidMmfUrlSink;
		}
	else
		err = KErrNotSupported;

	if (!err)
		{
		iFindAndOpenController->ConfigureSourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfFileSource,	sourceCfg), 
			CMMFFindAndOpenController::TSourceSink(dstUid, dstCfg));
			
		TMMFileSource tfs(aPrimaryFile);
		iFindAndOpenController->OpenByFileSource(tfs);
		}



	if (dstCfgBuffer)
		CleanupStack::PopAndDestroy(dstCfgBuffer);
	if (dstURLCfg)
		CleanupStack::PopAndDestroy(dstURLCfg);


	if (err)
		{	
		TInt oldState = State();
		iAsyncCallback->CallBack(iParent, oldState, oldState, err);
		return;
		}
	}

void CMMFMdaAudioConvertUtility::OpenL(TMdaClipLocation* aPriLocation, 
						TMdaClipLocation* aSecLocation, 
						TUid aControllerUid, // the controller to use
						TUid aDestFormatUid,
						TFourCC aDestDataType)
	{
	__ASSERT_ALWAYS(aPriLocation && aSecLocation, User::Leave(KErrArgument)); 

	Reset();


	// Configure the destination format and data type
	if (aDestFormatUid != KNullUid)
		iSinkFormatUid = aDestFormatUid;

	if (aDestDataType != KMMFFourCCCodeNULL)
		iSinkDataType = aDestDataType;

	TRAPD(err, ConfigureSourceSinkL(aPriLocation, aSecLocation));
	if (!err)
		iFindAndOpenController->OpenByControllerUid(aControllerUid);


	if (err)
		{
		TInt oldState = State();
		iAsyncCallback->CallBack(iParent, oldState, oldState, err);
		return;
		}
	}


void CMMFMdaAudioConvertUtility::HandleEvent(const TMMFEvent& aEvent)
	{
	//When converting, state is EPlaying, ERecording is never used
	if (aEvent.iEventType==KMMFEventCategoryPlaybackComplete || aEvent.iEventType==KMMFErrorCategoryControllerGeneralError)
		{
		TInt oldState = iState;
		iState = CMdaAudioClipUtility::EOpen;

		//if we weren't converting, don't advise Client.
		if(oldState == CMdaAudioClipUtility::EPlaying)
			iCallback.MoscoStateChangeEvent(iParent, CMdaAudioClipUtility::EPlaying, iState, aEvent.iErrorCode);
		}
	}

void CMMFMdaAudioConvertUtility::PlayL()
	{
	// N.B. ConvertL should be used in preference to PlayL
	ConvertL();
	}

void CMMFMdaAudioConvertUtility::RecordL()
	{
	// N.B. ConvertL should be used in preference to RecordL
	ConvertL();
	}

void CMMFMdaAudioConvertUtility::ConvertL()
	{
	if (iState == CMdaAudioClipUtility::EOpen) 
		{
		TInt err;
		err = iController.Prime();
		if (err==KErrNone)
			{
			err=iController.SetPosition(iPosition);
			if (!err && iConvertWindowSet)
				err = iAudioPlayControllerCustomCommands.SetPlaybackWindow(iConvertStart, iConvertEnd);
			if (err==KErrNone)
				err = iController.Play();
			}

			TInt oldState = iState;
			if (!err)
				iState = CMdaAudioClipUtility::EPlaying;

			iAsyncCallback->CallBack(iParent, oldState, iState, err);
		}
	else
		iAsyncCallback->CallBack(iParent, iState, iState, KErrNotReady);
	}

void CMMFMdaAudioConvertUtility::Stop()
	{ 
	TInt err = iController.Pause();
	TInt oldState = iState;
	if (!err)
		iState = CMdaAudioClipUtility::EOpen;
	iAsyncCallback->CallBack(iParent, oldState, iState, err);	
	}

void CMMFMdaAudioConvertUtility::CropL(TBool aCropToEnd)
	{
	// if we are busy converting, or we have not opened the file, return KErrNotReady
	if (iState!=CMdaAudioClipUtility::EOpen)
		User::Leave(KErrNotReady);

	// check that cropping position is valid if clip has been cropped before
	if (iHasCropped && iPosition > iCroppedDuration)
		{
		User::Leave(KErrArgument);
		}

	TInt err = iController.Prime();
	if (!err)
		{

		if (!err)
			err = iController.SetPosition(iPosition);
		
		err = iAudioRecordControllerCustomCommands.Crop(aCropToEnd);
		// try to stop controller regardless of whether any of the above commands failed
		iController.Stop();
		// save the duration of the cropped clip because
		// Duration() returns length of the original clip only
		// this is used to prevent a subsequent crop, beyond the end of the
		// already cropped clip.
		if (err == KErrNone)
			{
			iHasCropped = ETrue;
			if (aCropToEnd)
				iCroppedDuration = iPosition;
			else
				iCroppedDuration = TTimeIntervalMicroSeconds(iDuration.Int64() - iPosition.Int64());
			}
		}
	User::LeaveIfError(err);
	}

void CMMFMdaAudioConvertUtility::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	iPosition = aPosition;

	// Clip the position if aPosition is greater than the duration, or less then 0
	const TTimeIntervalMicroSeconds duration = Duration();
	if (iPosition > duration)
		iPosition = duration;
	else if (iPosition < TTimeIntervalMicroSeconds(0))
		iPosition = 0;

	if (iState==CMdaAudioClipUtility::EPlaying)
		iController.SetPosition(iPosition);
	}

const TTimeIntervalMicroSeconds& CMMFMdaAudioConvertUtility::Position()
	{
	if (iState==CMdaAudioClipUtility::EPlaying)
		{
		TInt err = iController.GetPosition(iPositionTemp);
		if (err==KErrNone)
			return iPositionTemp;
		}
	return iPosition;
	}

const TTimeIntervalMicroSeconds& CMMFMdaAudioConvertUtility::RecordTimeAvailable()
	{
#ifdef _DEBUG
	TInt error = 
#endif
		iAudioRecordControllerCustomCommands.GetRecordTimeAvailable(iRecordTimeAvailable);
	__ASSERT_DEBUG(error==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem)); 
	return iRecordTimeAvailable;
	}

const TTimeIntervalMicroSeconds& CMMFMdaAudioConvertUtility::Duration()
	{
	TInt err = iController.GetDuration(iDuration);
	if (err)
		iDuration = 0;
	return iDuration;
	}

void CMMFMdaAudioConvertUtility::SetMaxWriteLength(TInt aMaxWriteLength)
	{
	iAudioRecordControllerCustomCommands.SetMaxFileSize(aMaxWriteLength);
	}


void CMMFMdaAudioConvertUtility::SetPlayWindow(const TTimeIntervalMicroSeconds& aStart, const TTimeIntervalMicroSeconds& aEnd)
	{
	if (aStart >= TTimeIntervalMicroSeconds(0) &&
		aStart < iDuration &&
			aStart <= aEnd &&
			aEnd <= iDuration )
		{
		iConvertStart = aStart;
		iConvertEnd = aEnd;
		iConvertWindowSet = ETrue;

		if (iState==CMdaAudioClipUtility::EPlaying)
			{
#ifdef _DEBUG
			TInt error = 
#endif
				iAudioPlayControllerCustomCommands.SetPlaybackWindow(aStart, aEnd);
			__ASSERT_DEBUG(error==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem)); 
			}
		}
	else
		{
		__ASSERT_DEBUG(EFalse, Panic(EMMFMediaClientPanicServerCommunicationProblem)); 
		}
	}

void CMMFMdaAudioConvertUtility::ClearPlayWindow()
	{
	iConvertWindowSet = EFalse;
#ifdef _DEBUG
	TInt err = 
#endif //_DEBUG
	iAudioPlayControllerCustomCommands.DeletePlaybackWindow();
	__ASSERT_DEBUG(err==KErrNone, Panic(EMMFMediaClientPanicServerCommunicationProblem)); 
	}

void CMMFMdaAudioConvertUtility::SetRepeats(TInt /*aRepeatNumberOfTimes*/, const TTimeIntervalMicroSeconds& /*aTrailingSilence*/)
	{
	// This doesn't really make sense for the converter.
	}

void CMMFMdaAudioConvertUtility::SetPriority(TInt aPriority, TInt aPref)
	{
	iPrioritySettings.iPref = aPref;
	iPrioritySettings.iPriority = aPriority;
	iFindAndOpenController->Configure(iMediaIds[0], iPrioritySettings);
	iController.SetPrioritySettings(iPrioritySettings);
	}


void CMMFMdaAudioConvertUtility::Close()
	{
	iControllerEventMonitor->Cancel();
	iController.Close();

	if (iFindAndOpenController)
		iFindAndOpenController->Close();

	iHasCropped = EFalse;
	iCroppedDuration = 0;
	iState = CMdaAudioClipUtility::ENotReady;
	if(iControllerImplementationInformation)
		{
		delete iControllerImplementationInformation;
		iControllerImplementationInformation = NULL;
		}
	iControllerUid = KNullUid;
	}

void CMMFMdaAudioConvertUtility::CropL(const TTimeIntervalMicroSeconds &aCropBegin,
									   const TTimeIntervalMicroSeconds &aCropEnd)
	{
	// if we are busy converting, or we have not opened the file, return KErrNotReady
	if (iState!=CMdaAudioClipUtility::EOpen)
		User::Leave(KErrNotReady);

	// check that cropping positions are valid if clip has been cropped before
	if (iHasCropped && aCropEnd > iCroppedDuration)
		{
		User::Leave(KErrArgument);
		}

	TInt err = iController.Prime();
	if (!err)
		{
		err =iController.SetPosition(aCropEnd);
		if (!err)
			err = iAudioRecordControllerCustomCommands.Crop(ETrue);
		if (!err)
			{
			//remember the cropping, store the cropped length
			iHasCropped = ETrue;
			iCroppedDuration = aCropEnd;
			err = iController.SetPosition(aCropBegin);
			}
		if (!err)
			err = iAudioRecordControllerCustomCommands.Crop(EFalse);
		if (!err) // store the new cropped length
			iCroppedDuration = TTimeIntervalMicroSeconds(aCropEnd.Int64() - aCropBegin.Int64());
			
		// try to stop controller regardless of whether any of the above commands failed
		iController.Stop();
		}
	User::LeaveIfError(err);
	}

CMdaAudioClipUtility::TState CMMFMdaAudioConvertUtility::State()
	{
	return iState;
	}

void CMMFMdaAudioConvertUtility::GetSupportedDestinationDataTypesL(RArray<TFourCC>& aSupportedDataTypes)
	{
	iAudioControllerCustomCommands.GetSupportedSinkDataTypesL(aSupportedDataTypes);
	}

void CMMFMdaAudioConvertUtility::SetSourceDataTypeL(TFourCC aDataType)
	{
	User::LeaveIfError(iAudioControllerCustomCommands.SetSourceDataType(aDataType));
	}

void CMMFMdaAudioConvertUtility::SetDestinationDataTypeL(TFourCC aDataType)
	{
	User::LeaveIfError(iAudioControllerCustomCommands.SetSinkDataType(aDataType));
	}

TFourCC CMMFMdaAudioConvertUtility::DestinationDataTypeL()
	{
	TFourCC dataType;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSinkDataType(dataType));
	return dataType;
	}

TFourCC CMMFMdaAudioConvertUtility::SourceDataTypeL()
	{
	TFourCC dataType;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSourceDataType(dataType));
	return dataType;
	}


void CMMFMdaAudioConvertUtility::SetDestinationBitRateL(TUint aBitRate)
	{
	User::LeaveIfError(iAudioControllerCustomCommands.SetSinkBitRate(aBitRate));
	}

void CMMFMdaAudioConvertUtility::SetSourceSampleRateL(TUint aSampleRate)
	{
	User::LeaveIfError(iController.Stop());
	User::LeaveIfError(iAudioControllerCustomCommands.SetSourceSampleRate(aSampleRate));
	}

void CMMFMdaAudioConvertUtility::SetDestinationSampleRateL(TUint aSampleRate)
	{
	User::LeaveIfError(iController.Stop());
	User::LeaveIfError(iAudioControllerCustomCommands.SetSinkSampleRate(aSampleRate));
	}

void CMMFMdaAudioConvertUtility::SetSourceFormatL(TUid aRecordFormat)
	{
	User::LeaveIfError(iAudioControllerCustomCommands.SetSourceFormat(aRecordFormat));
	}

void CMMFMdaAudioConvertUtility::SetDestinationFormatL(TUid aRecordFormat)
	{
	User::LeaveIfError(iAudioControllerCustomCommands.SetSinkFormat(aRecordFormat));
	}

void CMMFMdaAudioConvertUtility::SetSourceNumberOfChannelsL(TUint aNumberOfChannels)
	{
	User::LeaveIfError(iAudioControllerCustomCommands.SetSourceNumChannels(aNumberOfChannels));
	}

void CMMFMdaAudioConvertUtility::SetDestinationNumberOfChannelsL(TUint aNumberOfChannels)
	{
	User::LeaveIfError(iAudioControllerCustomCommands.SetSinkNumChannels(aNumberOfChannels));
	}

TUint CMMFMdaAudioConvertUtility::DestinationBitRateL()
	{
	TUint bitRate;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSinkBitRate(bitRate));
	return bitRate;
	}

TUint CMMFMdaAudioConvertUtility::DestinationSampleRateL()
	{
	TUint sampleRate;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSinkSampleRate(sampleRate));
	return sampleRate;
	}

TUid CMMFMdaAudioConvertUtility::DestinationFormatL()
	{
	TUid format;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSinkFormat(format));
	return format;
	}

TUint CMMFMdaAudioConvertUtility::DestinationNumberOfChannelsL()
	{
	TUint numChannels;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSinkNumChannels(numChannels));
	return numChannels;
	}

TUint CMMFMdaAudioConvertUtility::SourceBitRateL()
	{
	TUint bitRate;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSourceBitRate(bitRate));
	return bitRate;
	}

TUint CMMFMdaAudioConvertUtility::SourceSampleRateL()
	{
	TUint sampleRate;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSourceSampleRate(sampleRate));
	return sampleRate;
	}

TUid CMMFMdaAudioConvertUtility::SourceFormatL()
	{
	TUid format;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSourceFormat(format));
	return format;
	}

TUint CMMFMdaAudioConvertUtility::SourceNumberOfChannelsL()
	{
	TUint numChannels;
	User::LeaveIfError(iAudioControllerCustomCommands.GetSourceNumChannels(numChannels));
	return numChannels;
	}


void CMMFMdaAudioConvertUtility::GetSupportedConversionBitRatesL(RArray<TUint>& aSupportedBitRates)
	{
	iAudioControllerCustomCommands.GetSupportedSinkBitRatesL(aSupportedBitRates);
	}

void CMMFMdaAudioConvertUtility::GetSupportedConversionSampleRatesL(RArray<TUint>& aSupportedSampleRates)
	{
	User::LeaveIfError(iController.Prime());
	iAudioControllerCustomCommands.GetSupportedSinkSampleRatesL(aSupportedSampleRates);
	User::LeaveIfError(iController.Stop());
	}

void CMMFMdaAudioConvertUtility::GetSupportedConversionNumberOfChannelsL(RArray<TUint>& aSupportedNumChannels)
	{
	iAudioControllerCustomCommands.GetSupportedSinkNumChannelsL(aSupportedNumChannels);
	}

CMdaAudioConvertUtility* CMMFMdaAudioConvertUtility::Parent() const
	{
	ASSERT(iParent);
	return static_cast<CMdaAudioConvertUtility*>(iParent);
	}

TInt CMMFMdaAudioConvertUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
	
TInt CMMFMdaAudioConvertUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
	
void CMMFMdaAudioConvertUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
	
void CMMFMdaAudioConvertUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

const CMMFControllerImplementationInformation& CMMFMdaAudioConvertUtility::ControllerImplementationInformationL()
	{
	if (!iControllerImplementationInformation)
		{
		if (iControllerUid==KNullUid)
			User::Leave(KErrNotReady);
		iControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(iControllerUid);
		}
	return *iControllerImplementationInformation;
	}


void CMMFMdaAudioConvertUtility::Reset()
	{
	// Make sure any existing controller is closed.
	Close();

	iSourceFormatUid = KNullUid;
	iSinkFormatUid = KNullUid;
	iSourceDataType = KMMFFourCCCodeNULL;
	iSinkDataType = KMMFFourCCCodeNULL;
	iSourceSampleRate = 0;
	iSinkSampleRate = 0;
	iSourceChannels = 0;
	iSinkChannels = 0;
	}


TInt CMMFMdaAudioConvertUtility::ControllerOpen()
	{
	TInt err = KErrNone;

	if(iSourceFormatUid != KNullUid)
		TRAP(err, SetSourceFormatL(iSourceFormatUid));

	if(!err && iSinkFormatUid != KNullUid)
		TRAP(err, SetDestinationFormatL(iSinkFormatUid));

	if(!err && iSourceDataType != KMMFFourCCCodeNULL)
		{
		TRAP(err, SetSourceDataTypeL(iSourceDataType));
		}

	if(!err && iSinkDataType != KMMFFourCCCodeNULL)
		{
		TRAP(err, SetDestinationDataTypeL(iSinkDataType));
		}

	// set the audio data settings ie sample rate & channels
	if (!err && iSourceSampleRate != 0)
		{
		TRAP(err, SetSourceSampleRateL(iSourceSampleRate));
		}

	if (!err && iSourceChannels != 0)
		{
		TRAP(err, SetSourceNumberOfChannelsL(iSourceChannels));
		}

	if (!err && iSinkSampleRate != 0)
		{
		TRAP(err, SetDestinationSampleRateL(iSinkSampleRate));
		}

	if (!err && iSinkChannels != 0)
		{
		TRAP(err, SetDestinationNumberOfChannelsL(iSinkChannels));
		}

	//get the clip duration
	if (!err)
		{
		iDuration = TTimeIntervalMicroSeconds(0);
		err = iController.GetDuration(iDuration);
		}

	if (err)
		Close();
	else
		iState = CMdaAudioClipUtility::EOpen;
	
	return err;
	}


void CMMFMdaAudioConvertUtility::ConfigureSourceSinkL(TMdaClipLocation* aPriLocation, TMdaClipLocation* aSecLocation)
	{
	TMMFDescriptorConfig srcDesCfg;
	TMMFFileConfig srcFileCfg;
	CBufFlat* srcCfgBuffer = NULL;
	CMMFUrlParams* srcURLCfg = NULL;
	TPtrC8 sourceCfg;
	TUid sourceUid = KNullUid;

	TMMFDescriptorConfig dstDesCfg;
	TMMFFileConfig dstFileCfg;
	CBufFlat* dstCfgBuffer = NULL;
	CMMFUrlParams* dstURLCfg = NULL;
	TPtrC8 dstCfg;
	TUid dstUid = KNullUid;

	TInt err = KErrNone;

	// setup the source config info
	if (aPriLocation->Uid() == KUidMdaFileResLoc)
		{//sink clip location is a file
		TDesC& fileName = ((TMdaFileClipLocation*)aPriLocation)->iName;		
		srcFileCfg().iPath = fileName;
		sourceCfg.Set(srcFileCfg);
		sourceUid = KUidMmfFileSource;
		}
	else if (aPriLocation->Uid() == KUidMdaDesResLoc)
		{//sink clip is a descriptor - pass down descriptor & thread id
		TMdaDesClipLocation* srcLoc = (TMdaDesClipLocation*)aPriLocation;
		srcDesCfg().iDes = srcLoc->iDes;
		srcDesCfg().iDesThreadId = srcLoc->iThreadId;
		sourceCfg.Set(srcDesCfg);
		sourceUid = KUidMmfDescriptorSource;
		}
	else if (aPriLocation->Uid() == KUidMdaUrlResLoc)
		{
		TMdaUrlClipLocation* srcLoc = (TMdaUrlClipLocation*)aPriLocation;
		srcURLCfg = CMMFUrlParams::NewLC(srcLoc->iUrl, srcLoc->iIapId);
		srcCfgBuffer = srcURLCfg->ExternalizeToCBufFlatLC();
		sourceCfg.Set(srcCfgBuffer->Ptr(0));
		sourceUid = KUidMmfUrlSource;
		}
	else
		{
		err = KErrNotSupported;
		}

	if (!err)
		{
		if (aSecLocation->Uid() == KUidMdaFileResLoc)
			{//sink clip location is a file
			TDesC& fileName = ((TMdaFileClipLocation*)aSecLocation)->iName;
			dstFileCfg().iPath = fileName;
			dstCfg.Set(dstFileCfg);
			dstUid = KUidMmfFileSink;
			}
		else if (aSecLocation->Uid() == KUidMdaDesResLoc)
			{//sink clip is a descriptor - pass down descriptor & thread id
			TMdaDesClipLocation* desLoc = (TMdaDesClipLocation*)aSecLocation;
			dstDesCfg().iDes = desLoc->iDes;
			dstDesCfg().iDesThreadId = desLoc->iThreadId;
			dstCfg.Set(dstDesCfg);
			dstUid = KUidMmfDescriptorSink;
			}
		else if (aSecLocation->Uid() == KUidMdaUrlResLoc)
			{
			TMdaUrlClipLocation* desLoc = (TMdaUrlClipLocation*)aSecLocation;
			dstURLCfg = CMMFUrlParams::NewLC(desLoc->iUrl, desLoc->iIapId);
			dstCfgBuffer = dstURLCfg->ExternalizeToCBufFlatLC();
			dstCfg.Set(dstCfgBuffer->Ptr(0));
			dstUid = KUidMmfUrlSink;
			}
		else
			{
			err = KErrNotSupported;
			}
		}

	if (!err)
		{
		iFindAndOpenController->ConfigureSourceSink(
			CMMFFindAndOpenController::TSourceSink(sourceUid, sourceCfg), 
			CMMFFindAndOpenController::TSourceSink(dstUid, dstCfg));
		}

	if (dstCfgBuffer)
		CleanupStack::PopAndDestroy(2);	//dstCfgBuffer, dstURLCfg
	if (srcCfgBuffer)
		CleanupStack::PopAndDestroy(2); //srcCfgBuffer, srcURLCfg

	User::LeaveIfError(err);
	}


TInt CMMFMdaAudioConvertUtility::SetThreadPriority(const TThreadPriority& aThreadPriority) const
	{
	return iController.SetThreadPriority(aThreadPriority);
	}





