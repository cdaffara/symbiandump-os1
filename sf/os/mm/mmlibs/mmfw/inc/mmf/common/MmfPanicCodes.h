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

#ifndef __MMF_COMMON_PANICCODES_H__
#define __MMF_COMMON_PANICCODES_H__

//Multimedia Framework Panic Codes

/**
@publishedAll
@released

Panic codes for CMMFAudioInput.
*/
enum TMMFAudioInputPanicCode
	{
	/**
	Panic in MMMFAudioInput::NewAudioInputL() if ImplementationUid!=KUidMmfAudioInput.
	*/
	EMMFAudioInputPanicBadUID = 1,
	/**
	MDevSoundObserver::ToneFinished() was called but not implemented.
	*/
	EMMFAudioInputPanicToneFinishedNotSupported,
	/**
	MDevSoundObserver::BufferToBeFilled() was called but not implemented.
	*/
	EMMFAudioInputPanicPlayerDataUsedNotSupported,
	/**
	MDevSoundObserver::PlayError() was called but not implemented.
	*/
	EMMFAudioInputPanicPlayErrorNotSupported,
	/**
	Panic if CMMFAudioInput::DevSound not loaded.
	*/
	EMMFAudioInputDevSoundNotLoaded
	};

/**
@publishedAll
@released

Panic codes for CMMFAudioOutput
*/
enum TMMFAudioOutputPanicCode
	{
	/**	Panic in MMMFAudioOutput::NewAudioInputL() if ImplementationUid!=KUidMmfAudioInput
	*/
	EMMFAudioOutputPanicBadUID = 1,

	/**	MMMFAudioOutput::BufferFilledL() was called but not implemented
	*/
	EMMFAudioOutputPanicBufferFilledLNotSupported,

	/**	MMMFAudioOutput::ToneFinished() was called but not implemented
	*/
	EMMFAudioOutputPanicToneFinishedNotSupported,

	/** Unimplemented panic code.
	*/
	EMMFAudioOutputPanicRecordDataFilledNotSupported,

	/**	MMMFAudioOutput::RecordError() was called but not implemented
	*/
	EMMFAudioOutputPanicRecordErrorNotSupported,

	/**	Panic if CMMFAudioOutput::iMMFDevSound is NULL
	*/
	EMMFAudioOutputDevSoundNotLoaded, 

	/**	Panic if trying to resume when resume is not supported by DevSound
	*/
	EMMFAudioOutputDevSoundResumingWhenNotSupported
	};

/**
@publishedAll
@released

Panic codes for CMMFDescriptor
*/
enum TMMFDescriptorPanicCode
	{
	/** MDataSource::BufferEmptiedL() was called but not implemented
	*/
	EMMFDescriptorPanicBufferEmptiedLNotSupported = 1,
	/** MDataSink::BufferFilledL() was called but not implemented
	*/
	EMMFDescriptorPanicBufferFilledLNotSupported
	};

/**
@publishedAll
@released

Panic codes for CMMFFile
*/
enum TMMFFilePanicCode
	{
	/** MDataSource::BufferEmptiedL() called but not implemented
	*/
	EMMFFilePanicBufferEmptiedLNotSupported = 1,
	/** MDataSink::BufferFilledL() called but not implemented
	*/
	EMMFFilePanicBufferFilledLNotSupported,
	/** Panic code raised if the file is already open.
	*/
	EMMFFileAlreadyOpen,
    /** Panic code raised if the file Handle is NULL.
    */  
  EMMFFileHandleNULL
	};

/** 
@publishedAll
@deprecated

Panic codes for CMMFDataPath
*/
enum TMMFDataPathPanicCode
	{
	/**
	The iState value is not the expected value (normally EPlaying). For example, when it is asked to initialise
	the sink or source whilst it is already playing some audio data.
	*/
	EMMFDataPathPanicBadState = 1000000,

	/**
	The code has detected a condition that cannot occur - must be programming error
	*/
	EMMFDataPathPanicProgrammingError = 2000000,

	/**
	A panic in CMMFDataPath::EmptySinkBufferL() if sink status
	is not EFull or EBeingFilled.
	*/
	EMMFDataPathPanicSinkError = 3000000
	};

/**
@publishedAll
@released

Panic codes for RMMFSubThread.
*/
enum TMMFSubThreadPanicCode
	{
	/** An attempt to close the subthread using RMMFSubThreadBase::Shutdown() has failed. The shutdown command
	waits for both the death of the thread and the time-out of a timer to minimise the risk of deadlock.
	If the timer times-out and the sub-thread is not dead then this thread is panicked in debug mode or,
	in release mode the sub-thread is killed.
	*/
	EMMFSubThreadPanicTimedOut = 1
	};

/**
@publishedAll
@released

Panic codes for MMFCodecBaseDefinitions
*/
enum TMMFCodecBaseDefinitionsPanicCode
	{
	/**
	Panic code raised when the codec TMMFTableAudioCodec::ConvertFast() method is not supported for this codec.
	*/
	EMMFCodecBaseDefinitionsPanicConvertFastNotSupported = 1	
	};

/**
@publishedAll
@released

Panic codes for CMMFAudioController.
*/
enum TMMFAudioControllerPanicCode
	{

	/** Panic code for a bad Unique Identifier. This is reserved.
	*/
	EMMFAudioControllerPanicBadUID = 1,

	/** AddDataSourceL() failed to add a data source. This could be caused by an 
	audio source that has been loaded but now does not exist.
	*/
	EMMFAudioControllerPanicDataSourceDoesNotExist,

	/** AddDataSinkL() failed to add a sink. This could be caused by an audio sink 
	that has been loaded but now does not exist.
	*/
	EMMFAudioControllerPanicDataSinkDoesNotExist
	};

/**
@publishedAll
@deprecated

Panic codes for MMFDisplay
*/
enum TMMFDisplayPanicCode
	{
  	/** Panic code raised when this CMMFDisplay is in the wrong thread.
	*/
	EMMFDisplayPanicWrongThread = 1,
  	/** Panic code raised when the CMMFDisplay has been asked to do something before it is ready.
	*/
	EMMFDisplayPanicNotReady,
  	/** Panic code raised when the CMMFDisplay::CreateSinkBufferL() method is not supported.
	*/
	EMMFDisplayPanicCreateSinkBufferLNotSupported,
  	/** Panic code raised when the CMMFDisplay::BufferFilledL() method is not supported.
	*/
	EMMFDisplayPanicBufferFilledLNotSupported,
  	/** Panic code raised when the calling supplier does not exist.
	*/
	EMMFDisplayPanicCallingSupplierDoesNotExist,
  	/** Panic code raised when the CMMFDisplay method has an internal error.
	*/
	EMMFDisplayPanicRequestStatusError
	};

/**
@publishedAll
@deprecated

Panic codes for MMFToneController.
*/
enum TMMFToneControllerPanicCode 
	{
	/** Panic code raised when the tone controller encounters a bad Unique Identifier.
	*/
	EMMFToneControllerPanicBadUID = 1,
	/** Panic code raised when the mixin class API encounters an unexpected call, for example
	when the hardware has full buffer or when playback is stopped due to error or EOF.
	*/
	EMMFToneControllerPanicUnexpectedMixinApiCall,
	/** Panic code raised when there is an error with the cancel command. Not used at present.
	*/
	EMMFToneControllerPanicBadCancelCustomCommand
	};


/**
@publishedAll
@released

Panic codes for the audio media client
*/
enum TMMFMediaClientPanicCode
	{
  	/** The specified audio device mode is not supported.
	*/
	EMMFMediaClientPanicSetAudioDeviceModeNotSupported = 1,

	/** An error occurred communicating with the audio device.
	*/
	EMMFMediaClientPanicServerCommunicationProblem,

	/** The functions CMMFMdaAudioConvertUtility::OpenL() or CMMFMdaAudioRecorderUtility::DoPlay()
	detected an invalid internal state
	*/
	EMMFMediaClientBadState,

	/** The audio media client received a bad argument.
	*/
	EMMFMediaClientBadArgument,

	/** Reserved function should not be called
	*/
	EMMFReservedCall
	};

/**
@publishedAll

Panic codes for the sw codec wrapper
*/
enum TMMFSwCodecWrapperPanicCode
	{
	/** Codec wrapper did not create an internal datapath
	*/
	EMMFSwCodecWrapperNoDataPath,

	/** Codec wrapper does not have a device handle
	*/
	EMMFSwCodecWrapperNoDevice,

	/** Codec wrapper codec returns non existant process result
	*/
	EMMFSwCodecWrapperBadCodec,

	/** Sound driver returns unexpected buffer
	*/
	EMMFSwCodecWrapperBadBuffer
	};


/**
@publishedAll
@released

Panic codes for the media client utility
*/
enum TMMFMediaClientUtilityPanicCode 
	{
	/** Client utility is in a bad state
	*/
	EMMFMediaClientUtilityBadState = 1,
	
	/** Client utility is passed a bad argument
	*/
	EMMFMediaClientUtilityBadArgument
	};

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
/**
@publishedAll
@released

Panic codes for the video player utility
*/
enum TMMFVideoPlayerUtilityPanicCode 
	{
	/** File not opened
	*/
	EMMFVideoPlayerUtilityFileNotOpened = 1,

	/** Subtitle not supported
	*/
	EMMFVideoPlayerUtilitySubtitleNotSupported = 2,

	/** No display window available
	*/
	EMMFVideoPlayerUtilityNoWindowAdded = 3,

	/** Subtitle not enabled
	*/
	EMMFVideoPlayerUtilitySubtitleNotEnabled = 4,

	/** Subtitle language not supported
	*/
	EMMFVideoPlayerUtilitySubtitleLanguageNotSupported = 5
	};
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

#endif
