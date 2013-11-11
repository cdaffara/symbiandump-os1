// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32debug.h>
#include <c3gplibrary.h>

#if defined (COMPOSE_DEBUG)
#define DEBUG_PRINT RDebug::Print
#else
#define DEBUG_PRINT
#endif

const TInt KMinWriteBufferMaxCount = 6;
_LIT(K3GPComposePanicName, "C3GPCompose");

// This is video base class containing common video properties.
T3GPVideoPropertiesBase::T3GPVideoPropertiesBase(T3GPVideoType aType,
		TUint aTimescale, const TSize& aSize) :
		iType(aType), iTimescale(aTimescale), iSize(aSize)
	{
	}

/**
This structure stores the common and MPEG-4 video specific properties of video data.
 
@param	aTimescale				Timescale of the video data.  This is the number of time units that 
 								pass in one second.
@param	aSize					Video width and height in pixels.
@param	aMaxBitRate 			Maximum video bit rate.
@param	aAvgBitRate				Average video bit rate.
@param	aDecoderSpecificInfo	MPEG-4 video DecoderSpecificInfo data stored in an ESDS atom.
*/
EXPORT_C T3GPVideoPropertiesMpeg4Video::T3GPVideoPropertiesMpeg4Video(TUint aTimescale,
		const TSize& aSize, TUint aMaxBitRate, TUint aAvgBitRate, const TDesC8& aDecoderSpecificInfo) :
		T3GPVideoPropertiesBase(E3GPMpeg4Video, aTimescale, aSize),
		iMaxBitRate(aMaxBitRate),
		iAvgBitRate(aAvgBitRate),
		iDecoderSpecificInfo(aDecoderSpecificInfo)
	{
	}

/**
This structure stores the common and H.263 specific properties of video data.
 
@param	aTimescale	Timescale of the video data.  This is the number of time units that 
					pass in one second.
@param	aSize		Video width and height in pixels.
@param	aVideoLevel	Indicates the H263 video level.
@param	aProfile	Indicates the H263 profile.
*/
EXPORT_C T3GPVideoPropertiesH263::T3GPVideoPropertiesH263(TUint aTimescale, const TSize& aSize,
		TInt aVideoLevel, TProfile aProfile) :
T3GPVideoPropertiesBase((aProfile == EProfile0) ? E3GPH263Profile0 : E3GPH263Profile3,
		aTimescale, aSize), iVideoLevel(aVideoLevel)
	{
	}

/**
This structure stores the common and AVC specific properties of video data.
 
@param	aTimescale				Timescale of the video data.  This is the number of time units that 
								pass in one second.
@param	aSize					Video width and height in pixels.
@param	aDecoderSpecificInfo	AVCDecoderConfigurationRecord data that will be stored in the avcC atom.
*/
EXPORT_C T3GPVideoPropertiesAvc::T3GPVideoPropertiesAvc(TUint aTimescale, const TSize& aSize,
		const TDesC8& aDecoderSpecificInfo) :
T3GPVideoPropertiesBase(E3GPAvcProfileBaseline, aTimescale, aSize),
iDecoderSpecificInfo(aDecoderSpecificInfo)
	{
	/*
		NOTE: Although Baseline profile is being set here, it's just used to indicate
		the fact that we have AVC data. The underlying 3GP lib does not differentiate
		between profiles when composing a file. It simply writes the contents of
		iDecoderSpecificInfo (which contains the profile amongst other things)
		verbatim into the "avcC" box.
	*/
	}

// This is audio base class containing common audio properties.
T3GPAudioPropertiesBase::T3GPAudioPropertiesBase(T3GPAudioType aType,
		TUint aTimescale, TInt aFramesPerSample) :
		iType(aType), iTimescale(aTimescale), iFramesPerSample(aFramesPerSample)
	{
	__ASSERT_ALWAYS((aTimescale > 0) && (aTimescale <= KMaxTUint16), User::Panic(K3GPComposePanicName, KErrOverflow));		
	}

/**
This structure stores the common and MPEG-4 audio-specific properties of audio data.
 
@param	aTimescale			Timescale of the audio data.  This is the number of time units that pass in one 
 							second. It must be smaller than 65536.
@param	aDecoderSpecificInfo MPEG-4 audio DecoderSpecificInfo data stored in an ESDS atom.
*/
EXPORT_C T3GPAudioPropertiesMpeg4Audio::T3GPAudioPropertiesMpeg4Audio(TUint aTimescale,
		const TDesC8& aDecoderSpecificInfo) :
		T3GPAudioPropertiesBase(E3GPMpeg4Audio, aTimescale, 0), 
		iDecoderSpecificInfo(aDecoderSpecificInfo)
	{
	}

/**
This structure stores the common and AMR-specific properties of audio data. 
 
@param	aTimescale			Timescale of the audio data.  This is the number of time units that pass in one 
 							second. It must be smaller than 65536.
@param	aFramesPerSample	Frames per sample.  It must be smaller than 256.
 							MPEG-4 audio has a fixed value of 1.
@param	aModeSet			AMR mode set.
@param	aCodec				AMR Speech Codec.
*/
EXPORT_C T3GPAudioPropertiesAmr::T3GPAudioPropertiesAmr(TUint aTimescale, TInt aFramesPerSample,
		TInt aModeSet, TSpeechCodec aCodec) :
		T3GPAudioPropertiesBase((aCodec == EAmrNB) ? E3GPAmrNB : E3GPAmrWB, 
		aTimescale, aFramesPerSample), iModeSet(aModeSet)
	{	
	__ASSERT_ALWAYS((aFramesPerSample > 0) && (aFramesPerSample <= KMaxTUint8), 
					User::Panic(K3GPComposePanicName, KErrOverflow));	
	}

/**
This structure stores the common and QCELP-specific properties of MPEG4 audio data. The storage mode is 
automatically set to MPEG4 Audio Sample Description Box mode.
 
@param	aTimescale			Timescale of the audio data.  This is the number of time units that pass in one 
 							second. It must be smaller than 65536.
@param	aFramesPerSample	Frames per sample.  It must be smaller than 512.
 							MPEG-4 audio has a fixed value of 1.
@param	aDecoderSpecificInfo MPEG-4 audio decoder specific information data stored in an ESDS atom.
*/
EXPORT_C T3GPAudioPropertiesQcelp::T3GPAudioPropertiesQcelp(TUint aTimescale, TInt aFramesPerSample,
		const TDesC8& aDecoderSpecificInfo) :
		T3GPAudioPropertiesBase(E3GPQcelp13K, aTimescale, aFramesPerSample),
		iMode(E3GPMP4AudioDescriptionBox),
		iDecoderSpecificInfo(aDecoderSpecificInfo)
	{
	__ASSERT_ALWAYS((aFramesPerSample > 0) && (aFramesPerSample <= KMaxTUint8), 
					User::Panic(K3GPComposePanicName, KErrOverflow));		
	}

/**
This structure stores the common and QCELP-specific properties of audio data.  
 
@param	aTimescale			Timescale of the audio data.  This is the number of time units that pass in one 
 							second. It must be smaller than 65536.
@param	aFramesPerSample	Frames per sample.  It must be smaller than 512.
 							MPEG-4 audio has a fixed value of 1.
*/

EXPORT_C T3GPAudioPropertiesQcelp::T3GPAudioPropertiesQcelp(TUint aTimescale, TInt aFramesPerSample) :
		T3GPAudioPropertiesBase(E3GPQcelp13K, aTimescale, aFramesPerSample),
		iMode(E3GPQcelpSampleEntryBox),
		iDecoderSpecificInfo(KNullDesC8)
	{
	__ASSERT_ALWAYS((aFramesPerSample > 0) && (aFramesPerSample <= KMaxTUint8), 
					User::Panic(K3GPComposePanicName, KErrOverflow));		
	}

/** 
Create an instance of 3GP composer using default buffer count and size.

The default values for buffer count and size are as follow:
Write Buffer Size is 2048 
Write Buffer Max Count is 15

@return A pointer to the newly created 3gp compose object.

@leave	KErrGeneral		General error.
@leave	KErrNoMemory	Out of memory.

@panic	C3GPCompose	KErrAbort	if clients do not a CActiveScheduler installed already.
*/
EXPORT_C C3GPCompose* C3GPCompose::NewL()
	{
	// Leave if no scheduler exists
	__ASSERT_ALWAYS ((CActiveScheduler::Current() != NULL), Panic(KErrAbort));
	C3GPCompose* self = new (ELeave) C3GPCompose(); 
	return self;
	}

/** 
Create an instance of 3GP composer, and let the user set a count limit and size of 
internal buffer for composition.

The default values for buffer count and size are as follow:
 Write Buffer Size is 2048 
 Write Buffer Max Count is 15

An increase of the buffer count and size will lead to a decrease of file I/O activities, thereby, 
improves the performance of the 3GP Composer at the expense of higher memory usage.

@param	aMediaWriteBufferSize Size of media data file output buffer (in bytes).
@param	aWriteBufferMaxCount	Maximum number of buffers (both media and meta) allowed before file 
 								output changes to synchronous (by default file writing is asynchronous 
 								operation).  A minimum value of 6 is enforced.

@return A pointer to the newly created 3gp compose object.

@leave	KErrGeneral		General error.
@leave	KErrNoMemory	Out of memory.

@panic	C3GPCompose	KErrAbort	if clients do not a CActiveScheduler installed already.
@panic	C3GPCompose	KErrArgument	if Write Buffer Size is less or equal to 0 or Write Buffer Max Count is
									less than 6.
 */
EXPORT_C C3GPCompose* C3GPCompose::NewL(TInt aMediaWriteBufferSize, TInt aWriteBufferMaxCount)
	{
	__ASSERT_ALWAYS ((aMediaWriteBufferSize > 0 && aWriteBufferMaxCount >= KMinWriteBufferMaxCount), 
					Panic(KErrArgument));
	// Leave if no scheduler exists
	__ASSERT_ALWAYS ((CActiveScheduler::Current() != NULL), Panic(KErrAbort));
	
	C3GPCompose* self = new (ELeave) C3GPCompose(aMediaWriteBufferSize, aWriteBufferMaxCount);
	return self;
	}

// First phase constructor
C3GPCompose::C3GPCompose(TInt aMediaWriteBufferSize, TInt aWriteBufferMaxCount) :
		iMediaWriteBufferSize(aMediaWriteBufferSize), 
		iWriteBufferMaxCount(aWriteBufferMaxCount),
		iDuplicateFileHandleCreated(EFalse)
	{
	}

/** 
This function initialises the 3GP composer for writing 3GP/3G2/MP4 data into a file.
Any combination of one video and one audio type is acceptable.  

Note: Ownership of aVideo and aAudio remains with the caller.  Both aVideo and aAudio are ready for 
deletion after C3GPCompose::Open returns.

@param	aFileFormat	Specifies the file format in which the data will be created.  Refer to 
 					T3GPFileFormatType for supported file format types.
@param 	aVideo 		Specifies the video stream to be used for video data.  The input data given will 
 					be inserted into 3GP file headers and is ready to be disposed when 
 					C3GPCompose::Open returns. See Video Properties Classes.
 					If aVideo is NULL, audio-only file will be composed.
@param 	aAudio 		Specifies the audio stream to be used for audio data.  The input data given will 
 					be inserted into 3GP file headers and is ready to be disposed when 
 					C3GPCompose::Open returns. See Audio Properties Classes.
 					If aAudio is NULL, video-only file will be composed.
@param	aFilename  	A full path name of the file to save the data to.  An empty path is not allowed.
@param	aFlags		Optional flags for composing preferences.  Refer to T3GPComposeFlag for supported flags.
 					The combined use of flags is acceptable.  For example:
 					E3GPLongClip | E3GPMetaDataLast 

@return KErrNone		if successful. Otherwise, returns one of the system wide error codes.
 		KErrGeneral		if an error has no specific categorisation;
  		KErrNoMemory	if an attempt to allocate memory has failed;
  		KErrArgument	if neither video nor audio stream is specified;  
  		KErrAccessDenied	if opening file has failed;
  		KErrUnderflow	if the file name length is not greater than 0;
  		KErrInUse		if the composer is currently engaged; C3GPCompose::Complete must be called to 
  		 		 		finish the current composition before the composer can be re-initialised again.
*/
EXPORT_C TInt C3GPCompose::Open(T3GPFileFormatType aFileFormat,
		const T3GPVideoPropertiesBase* aVideo,
		const T3GPAudioPropertiesBase* aAudio,
		const TDesC& aFilename,
		TUint aFlags)
	{
	if (iHandler)
		{
		return KErrInUse;
		}
	if (aFilename.Length() <= 0)
		{
		return KErrUnderflow;
		}
	if (!aVideo && !aAudio)
		{
		// if neither video nor audio is supplied
		return KErrArgument;
		}
	
	// Create a zero terminated version of the file name
	RBuf fileName;	
	TInt err = fileName.Create(aFilename.Length() + 1);
	if (err == KErrNone)
		{
		fileName.Copy(aFilename);
		mp4_u16* mp4FileName = const_cast<mp4_u16*>(fileName.PtrZ());
		MP4Err mp4Err = MP4ComposeOpen(&iHandler, reinterpret_cast<MP4FileName>(mp4FileName), Mp4Type(aVideo, aAudio));
		
		if (mp4Err == MP4_OK)
			{
			// Write audio and video properties to the 3GP file 
			err = SetComposeProperties(aVideo, aAudio, aFileFormat, aFlags);
			if (err != KErrNone)
				{
				Complete(); // Ingore the error
				}
			}
		else
			{
			err = SymbianOSError(mp4Err);
			}
		}
	fileName.Close();
	return err;
	}

/**
This function initialises the 3GP composer for writing 3GP/3G2/MP4 data into a file.
Any combination of one video and one audio type is acceptable.  

Note: E3GPMetaDataLast will be defaulted in aFlags if file handle is used for the initialisation 
of the 3GP composer.

Note: Ownership of aVideo and aAudio remains with the caller.  Both aVideo and aAudio are ready for 
deletion after C3GPCompose::Open returns.

@param  aFileFormat Specifies the file format in which the data will be created.  Refer to 
                    T3GPFileFormatType for supported file format types.
@param  aVideo      Specifies the video stream to be used for video data.  The input data given will 
                    be inserted into 3GP file headers and is ready to be disposed when 
                    C3GPCompose::Open returns. See Video Properties Classes.
                    If aVideo is NULL, audio-only file will be composed.
@param  aAudio      Specifies the audio stream to be used for audio data.  The input data given will 
                    be inserted into 3GP file headers and is ready to be disposed when 
                    C3GPCompose::Open returns. See Audio Properties Classes.
                    If aAudio is NULL, video-only file will be composed.
@param  aFile       File handle of the file to save the data to.  E3GPMetaDataLast needs to be set for 
                    aFlags when this is used.
@param  aFlags      Optional flags for composing preferences.  Refer to T3GPComposeFlag for supported flags.
                    The combined use of flags is acceptable.  For example:
                    E3GPLongClip | E3GPMetaDataLast 

@return KErrNone        if successful. Otherwise, returns one of the system wide error codes.
        KErrGeneral     if an error has no specific categorisation;
        KErrNoMemory    if an attempt to allocate memory has failed;
        KErrArgument    if neither video nor audio stream is specified;         
        KErrAccessDenied    if opening file has failed;
        KErrInUse   if the composer is currently engaged; C3GPCompose::Complete must be called to 
                        finish the current composition before the composer can be re-initialised again.
*/

EXPORT_C TInt C3GPCompose::Open(T3GPFileFormatType aFileFormat,
		const T3GPVideoPropertiesBase* aVideo,
		const T3GPAudioPropertiesBase* aAudio,
		RFile& aFile,
		TUint aFlags)
	{
	TInt err = KErrNone;
	if (!iDuplicateFileHandleCreated)
		{
		iDuplicateFileHandleCreated = ETrue;
		iFile.Close();
		err = iFile.Duplicate(aFile);
		if (err != KErrNone)
			{
			return err;
			}
		}

	return Open(aFileFormat, aVideo, aAudio, iFile, aFlags);
	}

/**
This function initialises the 3GP composer for writing 3GP/3G2/MP4 data into a file.
Any combination of one video and one audio type is acceptable.  

Note: E3GPMetaDataLast will be defaulted in aFlags if file handle is used for the initialisation 
of the 3GP composer.

Note: Ownership of aVideo and aAudio remains with the caller.  Both aVideo and aAudio are ready for 
deletion after C3GPCompose::Open returns.

@param	aFileFormat Specifies the file format in which the data will be created.  Refer to 
 					T3GPFileFormatType for supported file format types.
@param 	aVideo 		Specifies the video stream to be used for video data.  The input data given will 
 					be inserted into 3GP file headers and is ready to be disposed when 
 					C3GPCompose::Open returns. See Video Properties Classes.
 					If aVideo is NULL, audio-only file will be composed.
@param 	aAudio 		Specifies the audio stream to be used for audio data.  The input data given will 
 					be inserted into 3GP file headers and is ready to be disposed when 
 					C3GPCompose::Open returns. See Audio Properties Classes.
 					If aAudio is NULL, video-only file will be composed.
@param	aFile  		File handle of the file to save the data to.  E3GPMetaDataLast needs to be set for 
 					aFlags when this is used.
@param	aFlags  	Optional flags for composing preferences.  Refer to T3GPComposeFlag for supported flags.
 					The combined use of flags is acceptable.  For example:
 					E3GPLongClip | E3GPMetaDataLast 

@return KErrNone		if successful. Otherwise, returns one of the system wide error codes.
 		KErrGeneral		if an error has no specific categorisation;
  		KErrNoMemory	if an attempt to allocate memory has failed;
  		KErrArgument	if neither video nor audio stream is specified;    		
  		KErrAccessDenied	if opening file has failed;
  		KErrInUse	if the composer is currently engaged; C3GPCompose::Complete must be called to 
  		 		 		finish the current composition before the composer can be re-initialised again.
*/
EXPORT_C TInt C3GPCompose::Open(T3GPFileFormatType aFileFormat,
		const T3GPVideoPropertiesBase* aVideo,
		const T3GPAudioPropertiesBase* aAudio,
		RFile64& aFile,
		TUint aFlags)
	{
	if (iHandler)
		{
		return KErrInUse;
		}
	if (!aVideo && !aAudio)
		{
		// if neither video nor audio is supplied
		return KErrArgument;
		}
	
	TInt driveNumber = EDriveA;
	TDriveInfo driveInfo;
	TInt err = aFile.Drive(driveNumber, driveInfo);
	if (err == KErrNone)
		{
		MP4Err mp4Err = MP4ComposeOpenFileHandle64(&iHandler, &aFile, static_cast<TDriveNumber>(driveNumber), Mp4Type(aVideo, aAudio));
		if (mp4Err == MP4_OK)
			{			
			// Write audio and video properties to the 3GP file 
			err = SetComposeProperties(aVideo, aAudio, aFileFormat, aFlags);
			if (err != KErrNone)
				{
				Complete(); // Ingore the error
				}
			}
		else
			{
			err = SymbianOSError(mp4Err);
			}
		}

	return err;
	}

/**
Destructor
Deletes all objects and releases all resource owned by this instance.
*/
EXPORT_C C3GPCompose::~C3GPCompose()
	{
	Complete(); // Ignore the error
	}

/**
This function completes the composing operation. It frees the memory allocated by the library instance 
and closes the output file.

It is necessary to call this function before the output file is guaranteed to be a valid output file 
even though the file may exist prior to the call.

The composer can be reused again after this call, following another call to C3GPCompose::Open to 
re-initialise the composer. 

If C3GPCompose::Complete is called before the composer is initialised, it will be ignored and KErrNone 
is returned.

Although during destruction of C3GPCompose, this function will be automatically called, and no error 
code will be returned.  Therefore, when destroying the Composer object that you have used to compose a 
file, you should ensure that data is committed to the file by invoking C3GPCompose::Complete before 
destroying the Composer object.

@return KErrNone		if successful. Otherwise, returns one of the system wide error codes.
 		KErrGeneral		if an error has no specific categorisation;
 		KErrWrite		if metadata could not be written.
*/
EXPORT_C TInt C3GPCompose::Complete()
	{
	MP4Err mp4Err = MP4_OK;
	if (iHandler)
		{
		mp4Err = MP4ComposeClose(iHandler);
		}
	// Always reset the class member data even this function returns error
	Reset();
	return SymbianOSError(mp4Err);
	}

// Helper function to reset class member data.
void C3GPCompose::Reset()
	{
	iHasVideo = EFalse;
	iHasAudio = EFalse;
	iHandler = NULL;
	iFile.Close();
	}

/**
This function writes one video frame to the output file or buffer.

The frames must be inserted according to their causal sequence.  Because the library doesn't analyze 
the video bit stream, frames are inserted into the 3GP file in the same order as they are entered 
with this function.  Therefore, the frames will not be retrieved from the resulting 3GP file or 
buffer correctly if they are not in proper order.

A frame inserted with this function call will result in one 3GP sample and one 3GP chunk.

The current frame's dependency information which is using default values (E3GPDependencyUnknown & 
E3GPRedundancyUnknown) is inserted.

The data is available in the output file only after calling C3GPCompose::Complete. C3GPCompose::Complete 
should be called exactly once when all audio and video data has been inserted into the library.

@param	aBuffer		The descriptor containing the video frame data to be written.
@param	aDuration	Duration of video frame in timescale, see T3GPVideoPropertiesBase.
@param	aKeyFrame	ETrue to indicate whether this frame is a key frame.  EFalse otherwise.

@return	KErrNone		if successful. Otherwise, returns one of the system wide error codes.
 		KErrGeneral		if an error has no specific categorisation;
 		KErrNotSupported if the composer is setup for an audio-only file;
 		KErrUnderflow	if the supplied video frame buffer data is empty;
 		KErrNotReady	if the composer has not yet been initialised;  See C3GPCompose::Open.
*/
EXPORT_C TInt C3GPCompose::WriteVideoFrame(const TDesC8& aBuffer, TUint aDuration, TBool aKeyFrame)
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aBuffer.Length() <= 0)
		{
		return KErrUnderflow;
		}
	if (!iHasVideo)
		{
		return KErrNotSupported;
		}

	// Insert video frame data
	mp4_bool keyFrame = aKeyFrame;
	mp4_u32 duration = aDuration;
	MP4Err mp4Err = MP4ComposeWriteVideoFrame(iHandler, const_cast<mp4_u8*>(aBuffer.Ptr()), aBuffer.Length(), duration, keyFrame);
	
	return SymbianOSError(mp4Err);
	}

/**
This function sets the current frame's dependency information to SDTP box and writes one video frame 
to the output file.

The frames must be inserted according to their causal sequence.  Because the library doesn't analyze 
the video bit stream, frames are inserted into the 3GP file in the same order as they are entered 
with this function.  Therefore, the frames will not be retrieved from the resulting 3GP file or 
buffer correctly if they are not in proper order.

A frame inserted with this function call will result in one 3GP sample and one 3GP chunk.

The data is available in the output file only after calling C3GPCompose::Complete. C3GPCompose::Complete 
should be called exactly once when all audio and video data has been inserted into the library.

@param	aBuffer		The descriptor containing the video frame data to be written.
@param	aDuration	Duration of video frame in timescale, see T3GPVideoPropertiesBase.
@param	aKeyFrame	ETrue to indicate whether this frame is a key frame.  EFalse otherwise.
@param	aDependencies   This specifies the current frame's dependency information.  
					The information will be supplied into the SDTP box.
 					See T3GPFrameDependencies.

@return	KErrNone		if successful. Otherwise, returns one of the system wide error codes.
 		KErrGeneral		if an error has no specific categorisation;
 		KErrNotSupported if the composer is setup for an audio-only file;
 		KErrUnderflow	if the supplied video frame buffer data is empty;
 		KErrNotReady	if the composer has not yet been initialised;  See C3GPCompose::Open.
*/
EXPORT_C TInt C3GPCompose::WriteVideoFrame(const TDesC8& aBuffer, TUint aDuration, TBool aKeyFrame,
		const T3GPFrameDependencies& aDependencies)
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aBuffer.Length() <= 0)
		{
		return KErrUnderflow;
		}
	if (!iHasVideo)
		{
		return KErrNotSupported;
		}

	// Insert the current frame's dependency information
	MP4Err mp4Err = MP4ComposeWriteNextVideoFrameDependencies(iHandler, aDependencies.iDependsOn,
			aDependencies.iIsDependedOn, aDependencies.iHasRedundancy);

	if (mp4Err == MP4_OK)
		{
		// Insert video frame data
		mp4_bool keyFrame = aKeyFrame;
		mp4_u32 duration = aDuration;
		mp4Err = MP4ComposeWriteVideoFrame(iHandler, const_cast<mp4_u8*>(aBuffer.Ptr()), 
				aBuffer.Length(), duration, keyFrame);
		}

	return SymbianOSError(mp4Err);
	}

/**
This function writes audio frames into the output file or buffer.  The data is available in the 
3GP output file only after calling C3GPCompose::Complete.  C3GPCompose::Complete should be called exactly 
once when all audio and video data has been inserted into the library.

For MPEG-4 audio:
This function writes one MPEG audio frame to the 3GP file. 

For other audio types:
This function writes a number of audio frames to the 3GP file specified during composer setup 
in the input parameter aAudio when calling C3GPCompose::Open.  All audio frames inserted with 
one function call will be placed inside one sample in the resulting file.

Note: Only the last call can have a different number of frames if the number is less than 
the number of frames per sample specified during composer setup.

@see T3GPAudioPropertiesAmr
@see T3GPAudioPropertiesQcelp  		

@param	aBuffer			The descriptor containing the audio data to be written.
@param	aDuration		Duration of audio frames in timescale, see T3GPAudioPropertiesBase

@return	KErrNone		if successful.  Otherwise, returns one of the system wide error codes.
 		KErrGeneral		if an error has no specific categorisation;
 		KErrNotSupported if the composer is setup for a video-only file;
 		KErrUnderflow	if the supplied audio frames buffer data is empty;
 		KErrNotReady	if the composer has not yet been initialised;  See C3GPCompose::Open.
*/
EXPORT_C TInt C3GPCompose::WriteAudioFrames(const TDesC8& aBuffer, TUint aDuration)
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aBuffer.Length() <= 0)
		{
		return KErrUnderflow;
		}
	if (!iHasAudio)
		{
		return KErrNotSupported;
		}

	mp4_u32 duration = aDuration;
    // use 0 for the number of frames since it is being ignored within the mp4 library implementation
	MP4Err mp4Err = MP4ComposeWriteAudioFrames(iHandler, const_cast<mp4_u8*>(aBuffer.Ptr()), 
			aBuffer.Length(), 0, duration);
	
	return SymbianOSError(mp4Err);
	}

/**
Writes a buffer containing whole atom to inside of user data atom (UDTA) defined in aLocation.

The buffer should contain an atom of structure that conforms to the definition of a "full box" 
as specified in ISO/IEC 14496-12:2003: "Information technology – Coding of audio-visual objects
 – Part 12: ISO base media file format."
 
For more information on user data atoms, see Section 8 – Asset Information of "3GPP TS 26.244 
version 6.1.0 – 3GP file format (Rel 6)."

@param	aLocation	Specifies the location of user information to be written.  Refer to 
 					T3GPUdtaLocation for possible values.
@param	aBuffer		The descriptor containing the user information to write into file.

@return	KErrNone		if successful.  Otherwise, returns one of the system wide error codes.
 		KErrGeneral		if an error has no specific categorisation;
 		KErrArgument	if asked aLocation is invalid;
 		KErrUnderflow	if the supplied buffer data is empty;
 		KErrNotSupported	if specify video track user data but no video type is specified, 
 						or specify audio track user data but no audio type is specified
 		KErrNoMemory	if an attempt to allocate memory has failed;
 		KErrNotReady	if the composer has not yet been initialised;  See C3GPCompose::Open.
 		
@panic	C3GPCompose	KErrArgument if the location of user information is not of T3GPUdtaLocation. 
*/
EXPORT_C TInt C3GPCompose::SetUserData(T3GPUdtaLocation aLocation, const TDesC8& aBuffer)
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aBuffer.Length() <= 0)
		{
		return KErrUnderflow;
		}
	if ((!iHasAudio && aLocation == E3GPUdtaAudioTrak) || (!iHasVideo && aLocation == E3GPUdtaVideoTrak))
		{
		return KErrNotSupported;
		}
	
	mp4_u8 location;
	switch (aLocation)
		{
		case (E3GPUdtaMoov):
			location = MP4_UDTA_MOOV;
		break;
		case (E3GPUdtaVideoTrak):
			location = MP4_UDTA_VIDEOTRAK;
		break;
		case (E3GPUdtaAudioTrak):
			location = MP4_UDTA_AUDIOTRAK;
		break;
		default:
			Panic(KErrArgument);
		break;
		}

	mp4_u32 bufferSize = aBuffer.Length();
	MP4Err mp4Err = MP4ComposeSetUserDataAtom(iHandler, location, const_cast<mp4_u8*>(aBuffer.Ptr()), bufferSize);
	return SymbianOSError(mp4Err);
	}

// Helper function to convert 3GP/MP4 library specific error codes to system wide error codes
TInt C3GPCompose::SymbianOSError(MP4Err aError)
	{
	TInt error = KErrNone;

	switch (aError)
		{
		case (MP4_OK):
		break;
		case (MP4_ERROR):
			error = KErrGeneral;
		break;
		case (MP4_OUT_OF_MEMORY):
			error = KErrNoMemory;
		break;
		case (MP4_FILE_ERROR):
			error = KErrAccessDenied;
		break;
		case (MP4_INVALID_TYPE):
			error = KErrArgument;
		break;
		case (MP4_METADATA_ERROR):
			error = KErrWrite;
		break;
		default:
			Panic(KErrArgument);
		}
	return error;
	}

// Helper function to map 3GP enum type to MP4 audio and video type
mp4_u32 C3GPCompose::Mp4Type(const T3GPVideoPropertiesBase* aVideo, const T3GPAudioPropertiesBase* aAudio)
	{
	mp4_u32 videoType = MP4_TYPE_NONE;
	mp4_u32 audioType = MP4_TYPE_NONE;
	
	if (aVideo)
		{
		iHasVideo = ETrue;
		switch (aVideo->iType)
			{
			case (E3GPMpeg4Video):
				videoType = MP4_TYPE_MPEG4_VIDEO;
			break;
			case (E3GPH263Profile0):
				videoType = MP4_TYPE_H263_PROFILE_0;
			break;
			case (E3GPH263Profile3):
				videoType = MP4_TYPE_H263_PROFILE_3;
			break;
			
			/*
			 * NOTE: The underlying 3GP library does
			 * not differentiate between the various AVC
			 * profiles when composing.
			 * 
			 * In all cases it will simply copy the data
			 * from the iDecoderSpecificInfo member of
			 * T3GPVideoPropertiesAvc into the 'avcC' atom.
			 * It does not do any checking of that data, so
			 * it is the API user's responsibility to ensure
			 * that it contains a valid AVCDecoderConfigurationRecord
			 * with the correct AVC profile and level.
			 * 
			 * An interesting side-effect of this is that you can
			 * compose AVC data with arbitrary profiles even if they
			 * are not "supported" by this API. For example, as long
			 * as the AVCDecoderConfigurationRecord says there is
			 * High 10 profile data and the AVC data is of that profile
			 * then you will still end up with a valid file.
			 */
			case (E3GPAvcProfileBaseline):
				videoType = MP4_TYPE_AVC_PROFILE_BASELINE;
			break;
            case (E3GPAvcProfileMain):
                videoType = MP4_TYPE_AVC_PROFILE_MAIN;
            break;
            case (E3GPAvcProfileExtended):
                videoType = MP4_TYPE_AVC_PROFILE_EXTENDED;
            break;
            case (E3GPAvcProfileHigh):
                videoType = MP4_TYPE_AVC_PROFILE_HIGH;
            break;
			default:
				Panic(KErrArgument);
			}
		}

	if(aAudio)
		{
		iHasAudio = ETrue;
		switch (aAudio->iType)
			{
			case (E3GPMpeg4Audio):
				audioType = MP4_TYPE_MPEG4_AUDIO;
			break;
			case (E3GPAmrNB):
				audioType = MP4_TYPE_AMR_NB;
			break;
			case (E3GPAmrWB):
				audioType = MP4_TYPE_AMR_WB;
			break;
			case (E3GPQcelp13K):
				audioType = MP4_TYPE_QCELP_13K;
			break;
			default:
				Panic(KErrArgument);
			}
		}
	return (videoType | audioType);
	}
	
// Helper function to set compose properties
TInt C3GPCompose::SetComposeProperties(const T3GPVideoPropertiesBase* aVideo,
		const T3GPAudioPropertiesBase* aAudio, T3GPFileFormatType aFileFormat, TUint aFlag)
	{
	mp4_u32 writeBufferSize = iMediaWriteBufferSize;
	mp4_u32 writeBufferMaxCount = iWriteBufferMaxCount;
	MP4Err mp4Err = MP4SetCustomFileBufferSizes(iHandler, writeBufferSize, writeBufferMaxCount, 0);
	if ( mp4Err != MP4_OK)
		{
		return SymbianOSError(mp4Err);
		}
	
	// Set compose flag before other MP4Compose functions
	TInt err = SetComposeFlag(aFileFormat, aFlag);
	if (err != KErrNone)
		{
		return err;
		}
	
	if (aVideo)
		{
		switch (aVideo->iType)
			{
			case (E3GPMpeg4Video):
				err = SetMPeg4VideoProperties(aVideo);
			break;
			case (E3GPAvcProfileBaseline):
            case (E3GPAvcProfileMain):
            case (E3GPAvcProfileExtended):
            case (E3GPAvcProfileHigh):
				err = SetAvcVideoProperties(aVideo);
			break;
			case (E3GPH263Profile0):
			case (E3GPH263Profile3):
				err = SetH263VideoProperties(aVideo);
			break;
			default:
				Panic(KErrArgument);
			}
		}
	if (err != KErrNone)
		{
		return err;
		}

	if (aAudio)
		{
		switch (aAudio->iType)
			{
			case (E3GPMpeg4Audio):
				err = SetMpeg4AudioProperties(aAudio);
			break;
			case (E3GPQcelp13K):
				err = SetQcelpAudioProperties(aAudio);
			break;
			case (E3GPAmrNB):
			case (E3GPAmrWB):
				err = SetAmrAudioProperties(aAudio);
			break;
			default:
				Panic(KErrArgument);
			}
		}
	return err;
	}

// Inform the 3GP library about the MPeg4 video data
TInt C3GPCompose::SetMPeg4VideoProperties(const T3GPVideoPropertiesBase* aVideo)
	{
	const T3GPVideoPropertiesMpeg4Video* mpeg4Video = static_cast<const T3GPVideoPropertiesMpeg4Video*>(aVideo);

	MP4Err err = MP4ComposeWriteVideoDecoderSpecificInfo(iHandler, const_cast<mp4_u8*>(mpeg4Video->iDecoderSpecificInfo.Ptr()), 
			mpeg4Video->iDecoderSpecificInfo.Length());

	if ( err == MP4_OK)
		{
		err = MP4ComposeAddVideoDescription(iHandler, mpeg4Video->iTimescale,
				mpeg4Video->iSize.iWidth, mpeg4Video->iSize.iHeight,
				mpeg4Video->iMaxBitRate, mpeg4Video->iAvgBitRate);
		}

	return SymbianOSError(err);
	}

// Inform the 3GP library about the AVC video data
TInt C3GPCompose::SetAvcVideoProperties(const T3GPVideoPropertiesBase* aVideo)
	{
	const T3GPVideoPropertiesAvc* avcVideo = static_cast<const T3GPVideoPropertiesAvc*>(aVideo);

	MP4Err err = MP4ComposeWriteVideoDecoderSpecificInfo(iHandler, const_cast<mp4_u8*>(avcVideo->iDecoderSpecificInfo.Ptr()), 
			avcVideo->iDecoderSpecificInfo.Length ());

	if ( err == MP4_OK)
		{
		// aMaxBitRate and aAvgBitRate are MPEG-4 video specific values.  Set 0 for them
		err = MP4ComposeAddVideoDescription(iHandler, avcVideo->iTimescale,
				avcVideo->iSize.iWidth, avcVideo->iSize.iHeight, 0, 0);
		}

	return SymbianOSError(err);
	}

// Inform the 3GP library about the H263 video data
TInt C3GPCompose::SetH263VideoProperties(const T3GPVideoPropertiesBase* aVideo)
	{
	// aMaxBitRate and aAvgBitRate are MPEG-4 video specific values.  Set 0 for H263 video
	MP4Err err = MP4ComposeAddVideoDescription(iHandler, aVideo->iTimescale,
			aVideo->iSize.iWidth, aVideo->iSize.iHeight, 0, 0);

	if ( err == MP4_OK)
		{
		const T3GPVideoPropertiesH263* h263Video = static_cast<const T3GPVideoPropertiesH263*>(aVideo);
		TVideoClipProperties properties;
		properties.iH263Level = h263Video->iVideoLevel;
		err = MP4ComposeSetVideoClipProperties(iHandler, properties);
		}

	return SymbianOSError(err);
	}

// Inform the 3GP library about the MPeg4 audio data
TInt C3GPCompose::SetMpeg4AudioProperties(const T3GPAudioPropertiesBase* aAudio)
	{
	const T3GPAudioPropertiesMpeg4Audio* mpeg4Audio = static_cast<const T3GPAudioPropertiesMpeg4Audio*>(aAudio);

	MP4Err err = MP4ComposeWriteAudioDecoderSpecificInfo(iHandler, const_cast<mp4_u8*>(mpeg4Audio->iDecoderSpecificInfo.Ptr()), 
			mpeg4Audio->iDecoderSpecificInfo.Length ());

	if ( err == MP4_OK)
		{
		//modeSet is needed only for AMR audio. Set it to 0 for Mpeg4 audio.
		err = MP4ComposeAddAudioDescription(iHandler, mpeg4Audio->iTimescale, mpeg4Audio->iFramesPerSample, 0);
		}

	return SymbianOSError(err);
	}

// Inform the 3GP library about the Amr audio data
TInt C3GPCompose::SetAmrAudioProperties(const T3GPAudioPropertiesBase* aAudio)
	{
	const T3GPAudioPropertiesAmr* amrAudio = static_cast<const T3GPAudioPropertiesAmr*>(aAudio);
	//modeSet is needed only for AMR audio. 
	MP4Err err = MP4ComposeAddAudioDescription(iHandler, amrAudio->iTimescale,
			amrAudio->iFramesPerSample, amrAudio->iModeSet);
	return SymbianOSError(err);
	}

// Sets the storage mode of storing 13K QCELP data in a 3G2 file
TInt C3GPCompose::SetQcelpAudioProperties(const T3GPAudioPropertiesBase* aAudio)
	{
	const T3GPAudioPropertiesQcelp* qcelpAudio = static_cast<const T3GPAudioPropertiesQcelp*>(aAudio);
	MP4Err err = MP4ComposeSetQCELPStorageMode(iHandler, qcelpAudio->iMode);
	if ( err == MP4_OK)
		{
		if ( qcelpAudio->iMode == E3GPMP4AudioDescriptionBox)
			{
			err = MP4ComposeWriteAudioDecoderSpecificInfo(iHandler, const_cast<mp4_u8*>(qcelpAudio->iDecoderSpecificInfo.Ptr()), qcelpAudio->iDecoderSpecificInfo.Length ());
			}
		if ( err == MP4_OK)
			{
			//modeSet is needed only for AMR audio. Set it to 0 for Qcelp audio.
			err = MP4ComposeAddAudioDescription(iHandler, qcelpAudio->iTimescale, qcelpAudio->iFramesPerSample, 0);
			}
		}

	return SymbianOSError(err);
	}

// Set compose flag
TInt C3GPCompose::SetComposeFlag(T3GPFileFormatType aFileFormat, TUint aFlag)
	{
	mp4_u32 fileFormat = 0;
	switch (aFileFormat)
		{
		case (E3GPMP4):
			fileFormat = MP4_FLAG_GENERATE_MP4;
		break;
		case (E3GP3G2):
			fileFormat = MP4_FLAG_GENERATE_3G2;
		break;
		case (E3GP3GP):
			fileFormat = MP4_FLAG_NONE;
		break;
		default:
			Panic(KErrArgument);
		}

	MP4Err err = MP4ComposeSetFlags(iHandler, aFlag | fileFormat);
	return SymbianOSError(err);
	}


void C3GPCompose::Panic(TInt aPanic)
// Panic client
	{
	User::Panic(K3GPComposePanicName, aPanic);
	}
