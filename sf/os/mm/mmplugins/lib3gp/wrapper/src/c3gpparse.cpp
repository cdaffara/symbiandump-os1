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

#include <c3gplibrary.h>

/**
Creates an instance of a 3GP Parser for reading 3GP/3G2/MP4 data using default buffer size.

The default value for read buffer size is 8k. 

@return A pointer to the newly created 3gp parse object. 

@leave	KErrNoMemory	Out of memory.
*/
EXPORT_C C3GPParse* C3GPParse::NewL()
	{
	C3GPParse* self = new (ELeave) C3GPParse();
	return self;
	}

/**
Creates an instance of a 3GP Parser for reading 3GP/3G2/MP4 data, and sets the 
internal file read buffer size.  

@param	aReadBufferSize	Size of file read buffer.

@return A pointer to the newly created 3gp parse object. 

@leave	KErrNoMemory	Out of memory.
@leave	KErrGeneral		General error.

@panic	C3GPParse	KErrArgument	if size of file read buffer is not greater than 0.
*/
EXPORT_C C3GPParse* C3GPParse::NewL(TInt aReadBufferSize)
	{
	__ASSERT_ALWAYS((aReadBufferSize > 0), Panic(KErrArgument));
	C3GPParse* self = new (ELeave) C3GPParse(aReadBufferSize);
	return self;
	}

// First phase constructor
C3GPParse::C3GPParse(TInt aReadBufferSize) : 
		iReadBufferSize(aReadBufferSize),
		iDuplicateFileHandleCreated(EFalse),
		iVideoType(E3GPNoVideo),
		iAudioType(E3GPNoAudio)
	{
	}

/**
This function initialises the 3GP Parser for reading 3GP/3G2/MP4 data from a buffer and expects data 
to be inserted through subsequent calls to C3GPParse::InsertData.  

@see C3GPParse::InsertData

@return	KErrNone 		if successful. Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNoMemory	if an attempt to allocate memory has failed;
		KErrInUse		if the parser is currently engaged; C3GPParse::Complete must be called to 
						finish the parsing activities before the parser can be re-initialised again.
*/	
EXPORT_C TInt C3GPParse::Open()
	{
	if (iHandler)
		{
		return KErrInUse;
		}
	
	return SymbianOSError(MP4ParseOpen(&iHandler, NULL));
	}

/**
This function initialises the 3GP Parser for reading 3GP/3G2/MP4 data from a file.

@param	aFilename	A full path name of the file containing the data. 

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNoMemory	if an attempt to allocate memory has failed;
		KErrAccessDenied	if opening file has failed;
		KErrUnderflow	if the file name length is not greater than 0;
		KErrInUse		if the parser is currently engaged;  C3GPParse::Complete must be called to 
						finish the parsing activities before the parser can be re-initialised again.
*/	
EXPORT_C TInt C3GPParse::Open(const TDesC& aFilename)
	{
	if (iHandler)
		{
		return KErrInUse;
		}
	if (aFilename.Length() <= 0)
		{
		return KErrUnderflow;
		}
	
	// Create a zero terminated version of the file name
	RBuf fileName;	
	TInt err = fileName.Create(aFilename.Length() + 1);
	if (err == KErrNone)
		{
		fileName.Copy(aFilename);
		mp4_u16* mp4FileName = const_cast<mp4_u16*>(fileName.PtrZ());
		MP4Err mp4Err = MP4ParseOpen(&iHandler, reinterpret_cast<MP4FileName>(mp4FileName));
		
		if (mp4Err == MP4_OK)
			{
			// Set composing related values to 0.
			mp4Err = MP4SetCustomFileBufferSizes(iHandler, 0, 0, iReadBufferSize);
			if (mp4Err != MP4_OK)
				{
				// Ignore the error
				Complete();
				}
			}
		err = SymbianOSError(mp4Err);
		}
	fileName.Close();
	return err;
	}

/**
This function initialises the 3GP Parser for reading 3GP/3G2/MP4 data from a file.

@param  aFile   File handle of the file containing the data.  It is expected to be a valid file handle, 
                opened and will be closed outside of the library.

@return KErrNone        if successful.  Otherwise, returns one of the system wide error codes.
        KErrNoMemory    if an attempt to allocate memory has failed;
        KErrInUse       if the parser is currently engaged;  C3GPParse::Complete must be called to 
                        finish the parsing activities before the parser can be re-initialised again.
*/
EXPORT_C TInt C3GPParse::Open(const RFile& aFile)
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

	return Open(iFile);
	}
/**
This function initialises the 3GP Parser for reading 3GP/3G2/MP4 data from a file.

@param	aFile	File handle of the file containing the data.  It is expected to be a valid file handle, 
				opened and will be closed outside of the library.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrNoMemory	if an attempt to allocate memory has failed;
		KErrInUse		if the parser is currently engaged;  C3GPParse::Complete must be called to 
						finish the parsing activities before the parser can be re-initialised again.
*/
EXPORT_C TInt C3GPParse::Open(const RFile64& aFile)
	{
	if (iHandler)
		{
		return KErrInUse;
		}
	
	const RFile64* file = &aFile;
	MP4Err mp4Err = MP4ParseOpenFileHandle64(&iHandler, (const_cast<RFile64*>(file)));
	if (mp4Err == MP4_OK)
		{
		// Set composing related values to 0.
		mp4Err = MP4SetCustomFileBufferSizes(iHandler, 0, 0, iReadBufferSize);
		if (mp4Err != MP4_OK)
			{
			// Ignore the error
			Complete();
			}
		}

	return SymbianOSError(mp4Err);
	}

/**
This function initialises the 3GP Parser for reading 3GP/3G2/MP4 data from a CAF object.

@param	aData	A CData object pointing to a CAF object.  It is expected to be opened and will be 
				closed outside of the library.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrNoMemory	if an attempt to allocate memory has failed;
		KErrInUse		if the parser is currently engaged;  C3GPParse::Complete must be called to 
						finish the parsing activities before the parser can be re-initialised again.
*/
EXPORT_C TInt C3GPParse::Open(const ContentAccess::CData& aData)
	{
	if (iHandler)
		{
		return KErrInUse;
		}
	
	MP4Err mp4Err = MP4ParseOpenCAF(&iHandler, const_cast<ContentAccess::CData*>(&aData));

	if (mp4Err  == MP4_OK)
		{
		// Set composing related values to 0.
		mp4Err = MP4SetCustomFileBufferSizes(iHandler, 0, 0, iReadBufferSize);
		if (mp4Err != MP4_OK)
			{
			// Ignore the error
			Complete();
			}
		}
	
	return SymbianOSError(mp4Err);
	}

/**
Destructor. Deletes all objects.
*/
EXPORT_C C3GPParse::~C3GPParse()
	{
	Complete(); // Ignore the error
	}

/**
This function completes the parsing operation.

If C3GPParse::Complete is called before the parse is initialised, it will be ignored and KErrNone is 
returned.

The parser can be reused again after this call, following another call to C3GPParse::Open to 
re-initialise the parser. 

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
*/
EXPORT_C TInt C3GPParse::Complete()
	{
	MP4Err mp4Err = MP4_OK;
	if (iHandler)
		{
		mp4Err = MP4ParseClose(iHandler);
		}
		
	if (iAsyncReadBuffer)
		{
		CancelReadFrame();
		}
	
	// Always reset the class member data even MP4ParseClose returns error
	Reset();
	return SymbianOSError(mp4Err);
	}

// Helper function to reset class member data.
void C3GPParse::Reset()
	{
	iVideoType = E3GPNoVideo;
	iAudioType = E3GPNoAudio;
	iAsyncReadBuffer = NULL;
	iCallback = NULL; // Parse doesn't own the callback. Set it to NULL.
	iAudioAvgBitRate = 0;
	iStreamAvgBitRate = 0;
	iVideoPropertiesSaved = EFalse;
	iAudioPropertiesSaved = EFalse;
	iStreamPropertiesSaved = EFalse;
	iVideoError = KErrNone;
	iAudioError = KErrNone;
	iStreamError = KErrNone;
	iHandler = NULL;
	iDuplicateFileHandleCreated = EFalse;
	iFile.Close();
	}

/**
This function inserts MP4/3GP/3G2 data into the parser.

It is only necessary to call this function if no parameter has been supplied when 
C3GPParse::Open is called.  Several functions can return KErr3gpLibMoreDataRequired if the library 
does not have enough data to return the information that the caller requests. In this case, more data 
needs to be inserted to the library before calling those functions again.

This function makes a copy of the data inserted into the library so the caller can use the input buffer 
for other purposes. If the function returns KErrNoMemory, the buffer contents have not been copied into 
the library and the caller needs to reduce the buffer content before calling again.

If an empty string is supplied for the argument aBuffer, it indicates that there is be no more data 
to feed through this function.  Such a function call must be done to indicate that all 3GP/MP4/3G2 
data has been written to the library's internal memory.

@param	aBuffer	The descriptor containing the data to be inserted.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNoMemory	if parser cannot allocate enough memory for the data;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::InsertData(const TDesC8& aBuffer)
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}

	return SymbianOSError(MP4ParseWriteData(iHandler, const_cast<mp4_u8*>(aBuffer.Ptr()), aBuffer.Length()));
	}

/**
This function returns the parameters describing the video stream. 

If no file or CAF object is supplied during parser initialisation, this should be called after 
enough data has been inserted into the library buffers so that the 3GP headers containing the 
information can be read.

The aFrameRate parameter refers to the frame rate of the original video material.

@param	aType			The type of video stream.  Refer to T3GPVideoType for supported video type.
@param	aLength			Duration of video in milliseconds.
@param	aFrameRate		Average frame rate of video (in Hz).
@param	aAvgBitRate		Average bit rate of video.
@param	aSize			Width and height of video image measured in pixels.
@param	aTimeScale		Timescale of video track.  

@return	KErrNone 			if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral			if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video;
		KErrCorrupt			if 3GP stream is invalid;
		KErrNotReady		if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if more data is required before the requested information can be 
							retrieved;  See C3GPParse::InsertData.
*/
EXPORT_C TInt C3GPParse::GetVideoProperties(T3GPVideoType& aType, TUint& aLength, TReal& aFrameRate, 
		TUint& aAvgBitRate, TSize& aSize, TUint& aTimeScale) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	TInt err = DoGetVideoProperties();
	
	if (err == KErrNone)
		{
		aLength = iVideoLength;
		aType = iVideoType;
		aFrameRate = iVideoFrameRate;
		aSize.iWidth = iVideoSize.iWidth;
		aSize.iHeight = iVideoSize.iHeight;
		aTimeScale = iVideoTimeScale;
		}
	else 
		{
		return err;
		}

	// Get average bit rate of the stream in bps
	err = DoGetStreamProperties();
	if (err != KErrNone)
		{
		return err;
		}
	
	// Video average bit rate is calculated from GetStreamDescription’s aAvgBitRate substructs Audio’s aAvgBitRate
	// GetAudioProperties has not been called
	err = DoGetAudioProperties();
	if (err == KErrNone)
		{
		aAvgBitRate = iStreamAvgBitRate - iAudioAvgBitRate;
		}
	else
		{
		// if the audio stream is not usable, ignore the error since the stream
		// in concern is video stream.  The audio stream error can be dealt with by
		// users when it asks for audio properties. 
		aAvgBitRate = iStreamAvgBitRate;
		}
	
	return KErrNone;
	}

/**
This function returns the parameters describing the audio stream. 
 
If no file or CAF object is supplied during parser initialisation, this should be called after 
enough data has been inserted into the library so that the 3GP headers containing the information 
can be read.

@param	aType		The type of audio stream.  Refer to T3GPFrameType for supported audio type values.
@param	aLength		Duration of audio in milliseconds.
@param	aFramesPerSample	Number of audio frames in each sample.
@param	aAvgBitRate	Average bit rate of audio.
@param	aTimeScale	Timescale of audio track. 

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if input does not contain audio;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if more data is required before the requested information can be 
						retrieved.  See C3GPParse::InsertData.
*/
EXPORT_C TInt C3GPParse::GetAudioProperties(T3GPAudioType& aType, TUint& aLength, TInt& aFramesPerSample, 
		TUint& aAvgBitRate, TUint& aTimeScale) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	TInt err = DoGetAudioProperties();
	
	if (err == KErrNone)
		{
		aLength = iAudioLength;
		aType = iAudioType;
		aFramesPerSample = iAudioFramesPerSample;
		aAvgBitRate = iAudioAvgBitRate;
		aTimeScale = iAudioTimeScale;
		iAudioPropertiesSaved = ETrue;
		}

	return err;
	}

/**
This function returns the parameters that describe the contents of the input file or buffer. 

If no file or CAF object is supplied during parser initialisation, this should be called after 
enough data has been inserted into the library so that the headers containing the information can 
be read.

@param	aSize		Length of the stream in bytes.
@param	aAvgBitRate	Average bit rate of the stream in bps.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if more data is required before the requested information can be 
						retrieved.  See C3GPParse::InsertData.
 */
EXPORT_C TInt C3GPParse::GetContainerProperties(TUint& aSize, TUint& aAvgBitRate) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	TInt err = DoGetStreamProperties();
	if (err == KErrNone)
		{
		aSize = iStreamSize;
		aAvgBitRate = iStreamAvgBitRate;
		}

	return err;
	}

/**
This function returns the number of bytes that the library instance has in its allocated buffers.

The function is only valid when the parser is initialized without any parameters. Zero is 
returned when in file mode, that is, the parser has been initialized with a valid filename, file handle 
or a CAF object.

@see C3GPParse::InsertData.

@param	aNum	Number of allocated bytes in the library.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the parser has been initialised with a file name or file handle;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetNumBufferedBytes(TInt& aNum) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
		
	mp4_u32 numBytes = 0;
	MP4Err mp4Err = MP4ParseGetBufferedBytes(iHandler, &numBytes);
	aNum = numBytes;
	
	return SymbianOSError(mp4Err);
	}

/**
This function returns the type of the next audio/video frame in the stream.

This function has no effect on the parser’s current position.

@param	aType	Type of the next frame.  Refer to the definition of T3GPFrameType for all possible values.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNotFound	if frame does not exist (the previous frame was the last one);
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErrCorrupt		if 3GP stream is invalid;
		KErr3gpLibMoreDataRequired		if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetFrameType(T3GPFrameType& aType) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	mp4_u32 frameType = 0;
	MP4Err mp4Err = MP4ParseNextFrameType(iHandler, &frameType);
	if (mp4Err == MP4_OK)
		{
		if (
		        frameType == MP4_TYPE_MPEG4_VIDEO ||
		        frameType == MP4_TYPE_H263_PROFILE_0 || 
		        frameType == MP4_TYPE_H263_PROFILE_3 ||
		        frameType == MP4_TYPE_AVC_PROFILE_BASELINE ||
                frameType == MP4_TYPE_AVC_PROFILE_MAIN ||
                frameType == MP4_TYPE_AVC_PROFILE_EXTENDED ||
                frameType == MP4_TYPE_AVC_PROFILE_HIGH
		   )
			{
			aType = E3GPVideo;
			}
		else if (frameType == MP4_TYPE_MPEG4_AUDIO || frameType == MP4_TYPE_AMR_NB || 
			frameType == MP4_TYPE_AMR_WB || frameType == MP4_TYPE_QCELP_13K)
			{
			aType = E3GPAudio;
			}
		else
			{
			// This should not happen
			Panic(KErrCorrupt);
			}
		}
	return SymbianOSError(mp4Err);
	}

/**
This function returns the size of the immediate next video frame based on the current 
position of the parser.

Calling this function does not change the current position of the parser.

@param	aSize	Size of the next video frame in bytes.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video;		
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetVideoFrameSize(TUint& aSize) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties(); 
	if (err != KErrNone)
		{
		return err;
		}
	
	// video type from 3GP file header has been saved
	mp4_u32 frameSize = 0;	
	MP4Err mp4Err = MP4ParseNextFrameSize(iHandler, iVideoMp4Type, &frameSize);
	if (mp4Err == MP4_OK)
		{		
		aSize = frameSize;
		}
	
	return SymbianOSError(mp4Err);	
	}

/**
This function returns the total size of the audio frames in the immediate audio sample based on the 
current position of the parser.

This function has no effect on the parser’s current position.

@param	aSize	Size of the next audio sample in bytes.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain audio;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetAudioFramesSize(TUint& aSize) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains audio stream
	TInt err = DoGetAudioProperties(); 
	if (err != KErrNone)
		{
		return err;
		}
	
	// audio type from 3GP file header has been saved. Directely use iAudioMp4Type as input
	mp4_u32 frameSize = 0;
	
	MP4Err mp4Err = MP4ParseNextFrameSize(iHandler, iAudioMp4Type, &frameSize);
	if (mp4Err == MP4_OK)
		{
		aSize = frameSize;
		}
	
	return SymbianOSError(mp4Err);	
	}

/**
This function reads the next video frame from the 3GP file/stream and returns it to the caller.  
The current position of video stream will be moved forward.

The next frame depends on the position in the input 3GP file.  C3GPParse::Seek can be used to change 
the current position in the 3GP file.

If the function returns KErr3gpLibMoreDataRequired, the caller needs to call C3GPParse::InsertData 
to insert more data before calling the function again.

Since there are separate cursors for storing current positions of video and audio streams, calling this 
function does not affect the position of the next audio stream.

@param	aBuffer		Video frame is returned here.
@param	aKeyFrame	Returns ETrue if the current frame is a key frame (intra), otherwise the value is EFalse.
@param	aTimeStampInMs	Video frame presentation time in milliseconds from the beginning of the video sequence.
@param	aTimeStampInTimescale	Video frame presentation time in timescale from the beginning of the video sequence.  

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the 3GP input data contains no video stream;
		KErrNotFound	if frame does not exist (the previous frame was the last one);
		KErrOverflow	if requested frame does not fit into the given buffer; Caller can use 
						C3GPParse::GetVideoFrameSize to retrieve the minimum buffer size needed 
						to fit this video frame;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::ReadVideoFrame(TDes8& aBuffer, TBool& aKeyFrame, TUint& aTimeStampInMs, 
										TUint& aTimeStampInTimescale) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aBuffer.MaxLength() <= 0)
		{
		return KErrOverflow;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties();	
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 frameSize = 0;
	mp4_u32 timeStamp = 0;
	mp4_bool keyFrame = EFalse; 
	mp4_u32 timestamp2 = 0;
	
	MP4Err mp4Err = MP4ParseReadVideoFrame(iHandler, const_cast<mp4_u8*>(aBuffer.Ptr()), aBuffer.MaxLength(), 
			&frameSize, &timeStamp, &keyFrame, &timestamp2);
	
	if (mp4Err == MP4_OK)
		{
		aBuffer.SetLength(frameSize);
		aKeyFrame = keyFrame;
		aTimeStampInMs = timeStamp;
		aTimeStampInTimescale = timestamp2;
		}
	return SymbianOSError(mp4Err);
	}

/**
Return size of video DecoderSpecificInfo.
For files with MPEG-4 video this data is read from the esds atom. For files with AVC video
this data is read from the avcC atom.

Note: Only MPEG-4 video and H.264/AVC video streams contain DecoderSpecificInfo.

@param	aSize	Size of video DecoderSpecificInfo.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the 3GP input data contains no MPEG-4 / AVC video stream;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetVideoDecoderSpecificInfoSize(TInt& aSize) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}

	// Check if 3GP input data contains video stream and the video type is either
	// MPEG-4 or H.264/AVC
	TInt err = DoGetVideoProperties();	
	if (err == KErrNone)
		{
		if (!(
		        iVideoType == E3GPMpeg4Video ||
		        iVideoType == E3GPAvcProfileBaseline ||
				iVideoType == E3GPAvcProfileMain ||
				iVideoType == E3GPAvcProfileExtended ||
                iVideoType == E3GPAvcProfileHigh
        ))
			{
			return KErrNotSupported;
			}
		}
	else
		{
		return err;
		}
	
	mp4_u32 decspecinfosize = 0;
	MP4Err mp4Err = MP4ParseReadVideoDecoderSpecificInfo(iHandler, NULL, 0, &decspecinfosize);
	
	if ( mp4Err == MP4_OK || mp4Err == MP4_BUFFER_TOO_SMALL)
		{
		aSize = decspecinfosize;
		mp4Err = MP4_OK;
		}

	return SymbianOSError(mp4Err);
	}

/**
This function reads DecoderSpecificInfo data from 3GP metadata and returns it to the caller.
For files with MPEG-4 video this data is read from the esds atom. For files with AVC video
this data is read from the avcC atom.

Note: Only MPEG-4 video and H.264/AVC video streams contain DecoderSpecificInfo.

@see C3GPParse::GetVideoDecoderSpecificInfoSize

@param	aInfo			The descriptor to store the video decoder information.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the 3GP input data contains no MPEG-4 / AVC video stream;
		KErrOverflow	if requested frame does not fit into the given buffer;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetVideoDecoderSpecificInfo(TDes8& aInfo) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aInfo.MaxLength() <= 0)
		{
		return KErrOverflow;
		}
	
	// Check if 3GP input data contains video stream and the video type is either
	// MPEG-4 or H.264/AVC
	TInt err = DoGetVideoProperties();	
	if (err == KErrNone)
		{
		if (!(
		        iVideoType == E3GPMpeg4Video ||
		        iVideoType == E3GPAvcProfileBaseline ||
		        iVideoType == E3GPAvcProfileMain ||
		        iVideoType == E3GPAvcProfileExtended ||
                iVideoType == E3GPAvcProfileHigh
		))
			{
			return KErrNotSupported;
			}
		}
	else
		{
		return err;
		}
	
	mp4_u32 decspecinfosize = 0;
	MP4Err mp4Err = MP4ParseReadVideoDecoderSpecificInfo(iHandler, const_cast<mp4_u8*>(aInfo.Ptr()), 
			aInfo.MaxLength(),&decspecinfosize);
	if (mp4Err == MP4_OK)
		{
		aInfo.SetLength(decspecinfosize);
		}	
	return SymbianOSError(mp4Err);
	}

/**
This function reads the audio frames that are stored in the current audio sample from the 
3GP file/stream and returns them to the caller.  The current position of audio stream will be moved forward. 

Note: The next frame depends on the position in the input 3GP file.  C3GPParse::Seek can 
be used to change the current position in the 3GP file.

If the function returns KErr3gpLibMoreDataRequired, the caller needs to call C3GPParse::InsertData 
to insert more data before calling the function again.

Note: aReturnedFrames may differ from the correct value when accessing the last audio sample.

Note: Since there are separate cursors for storing current positions for video and audio streams, calling 
this function does not change current position of the parser for video stream.

@param	aBuffer			Audio frames are returned here.
@param	aReturnedFrames	Number of the returned frames or 0 if not known.
@param	aTimeStampInMs	Audio frame presentation time in milliseconds from the beginning of the 
						audio sequence.
@param	aTimeStampInTimescale	Audio frame presentation time in timescale from the beginning 
						of the audio sequence.  
					
@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the 3GP input data contains no audio stream; 
		KErrOverflow	if requested frame does not fit into the given buffer; Caller can use 
						C3GPParse::GetAudioFrameSize to retrieve the minimum size needed to 
						fit this audio frame;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotFound	if no more frames available;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::ReadAudioFrames(TDes8& aBuffer, TInt& aReturnedFrames, TUint& aTimeStampInMs, 
		TUint& aTimeStampInTimescale) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aBuffer.MaxLength() <= 0)
		{
		return KErrOverflow;
		}
	
	// Check if 3GP input data contains audio stream
	TInt err = DoGetAudioProperties();	
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 numOfFrames = 0;
	mp4_u32 timeStampInMs = 0;
	mp4_u32 timeStampInTimescale = 0;
	mp4_u32 audioSize = 0;
	MP4Err mp4Err = MP4ParseReadAudioFrames(iHandler, const_cast<mp4_u8*>(aBuffer.Ptr()), aBuffer.MaxLength(), 
			&audioSize, &timeStampInMs, &numOfFrames, &timeStampInTimescale);
	if (mp4Err == MP4_OK)
		{
		aBuffer.SetLength(audioSize);
		aReturnedFrames = numOfFrames;
		aTimeStampInMs = timeStampInMs;
		aTimeStampInTimescale = timeStampInTimescale;	
		}
	return SymbianOSError(mp4Err);
	}

/**
Returns size of audio DecoderSpecificInfo data from 3GP metadata.

@param	aSize	Size of DecoderSpecificInfo to be returned (in bytes).

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the stream does not contain audio stream stored in MPEG-4 audio sample boxes.
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetAudioDecoderSpecificInfoSize(TInt& aSize) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}

	// Check if 3GP input data contains audio stream
	TInt err = DoGetAudioProperties();	
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 decspecinfosize = 0;
	MP4Err mp4Err = MP4ParseReadAudioDecoderSpecificInfo(iHandler, NULL, 0, &decspecinfosize);	
	if ( mp4Err == MP4_OK  || mp4Err == MP4_BUFFER_TOO_SMALL)
		{
		aSize = decspecinfosize;
		mp4Err = MP4_OK;
		}	
	return SymbianOSError(mp4Err);
	}

/**
This function reads DecoderSpecificInfo data from 3GP metadata and returns it to the caller.

Note: AMR DecoderSpecificInfo data structure is returned in runtime in an architecture-specific 
Endian format, that is, no Endian conversion is necessary for the data.

@see C3GPParse::GetAudioDecoderSpecificInfoSize

@param	aBuffer				DecoderSpecificInfo is returned here.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the stream does not contain audio stream stored in MPEG-4 audio sample boxes.		
		KErrOverflow	if requested frame does not fit into the given buffer;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetAudioDecoderSpecificInfo(TDes8& aBuffer) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aBuffer.MaxLength() <= 0)
		{
		return KErrOverflow;
		}
	
	// Check if 3GP input data contains audio stream
	TInt err = DoGetAudioProperties();	
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 decspecinfosize = 0;
	MP4Err mp4Err = MP4ParseReadAudioDecoderSpecificInfo(iHandler, const_cast<mp4_u8*>(aBuffer.Ptr()), 
			aBuffer.MaxLength(),&decspecinfosize);
	if (mp4Err == MP4_OK)
		{
		aBuffer.SetLength(decspecinfosize);
		}
	return SymbianOSError(mp4Err);
	}

/**
Returns the timestamp of the next video frame.  The current position of the video stream will be moved forward.

The function can be used to find out which frames have been coded to optimize the input frame selection 
if video frame rate needs to be modified.

When this function call returns KErrEof, there are no more video frames left in the 3GP file and 
the timestamp returned with the previous call was the timestamp of the last video frame.

Note:  C3GPParse::Seek can be used to change the current position in the 3GP file. 

@param	aTimeStampInMs			Timestamp in milliseconds is returned here.
@param	aTimeStampInTimescale	Timestamp in timescale is returned here.  

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video;		
		KErrEof			if no more video frames left;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetVideoTimestamp(TUint& aTimeStampInMs, TUint& aTimeStampInTimescale) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties();	
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 timeStampInMs = 0;
	mp4_u32 timeStampInTimescale = 0;
	MP4Err mp4Err = MP4ParseGetNextVideoTimestamp(iHandler, &timeStampInMs, &timeStampInTimescale);
	if (mp4Err == MP4_OK)
		{
		aTimeStampInMs = timeStampInMs;
		aTimeStampInTimescale = timeStampInTimescale;
		}
	return SymbianOSError(mp4Err);
	}

/**
This function determines whether the input 3GP stream is streamable, that is, whether the media data is arranged 
in such a manner that playback can be started without downloading the entire stream.

@param	aStreamable	Returns ETrue if the file is streamable.  Otherwise, returns EFalse.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetStreamable(TBool& aStreamable) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	MP4Err mp4Err = MP4ParseIsStreamable(iHandler);
	if (mp4Err == MP4_OK)
		{
		aStreamable = ETrue;
		}
	else
		{
		aStreamable = EFalse;
		}
	return SymbianOSError(mp4Err);
	}

/**
This function seeks the position specified by the aPosition parameter in the input 3GP file/stream. 

The position is considered to start from the beginning of the presentation time line in the 3GP file. 
Thus audio and video positions cannot be given separately.

The function will set the current audio and video positions in the following manner:

	If there is only audio data in the file, the current position is set to the audio frame at or just 
	before the given position.

	If there is only video in the file and the key frame is EFalse, the current position is set to the 
	video frame at or just before the given position. If the key frame is set to ETrue, the current 
	position is set to the first key frame at or before the current position.

	If there are both audio and video in the file, video is first positioned as explained above and then 
	audio is sought to the closest position in relation to video.	
	
If the position to seek is greater than the duration of video or audio, the video or audio position is 
set to the position of the last video or audio frame as explained above.

If the function returns KErr3gpLibMoreDataRequired, the caller needs to call C3GPParse::InsertData 
to insert more data before calling the function again.

@param	aPosition		Position to seek in milliseconds in the 3GP presentation time line.
@param	aKeyFrame		If set to ETrue, the first video key frame before a given point is sought.  If 
						set to EFalse, the first video frame before a given point is sought.
@param	aAudioPosition	Position of audio after seeking (in milliseconds).
@param	aVideoPosition	Position of video after seeking (in milliseconds).

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotFound	if cannot seek the requested position;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
*/
EXPORT_C TInt C3GPParse::Seek(TUint aPosition, TBool aKeyFrame, TUint& aAudioPosition, TUint& aVideoPosition) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	mp4_u32 audioPosition = 0;
	mp4_u32 videoPosition = 0;
	mp4_bool keyFrame = aKeyFrame;
	MP4Err mp4Err = MP4ParseSeek(iHandler, aPosition, &audioPosition, &videoPosition, keyFrame);
	if (mp4Err == MP4_OK)
		{
		aAudioPosition = audioPosition;
		aVideoPosition = videoPosition;
		}
	return SymbianOSError(mp4Err);
	}

/**
This function determines whether the next frame of the type aType is available.

This function has no effect on the parser’s current position.

@param	aType		The type of frame to check for.  Refer to T3GPFrameType for supported types.
@param	aAvailable	Return ETrue if the type of frame specified by aType is available.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotFound	if frame of the requested type is not available;
		KErrCorrupt		if 3GP stream is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetFrameAvailability(T3GPFrameType aType, TBool& aAvailable) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	aAvailable = EFalse;
	TInt err = KErrNone;
	MP4Err mp4Err = MP4_OK;
	if (aType == E3GPAudio)
		{
		// Check if 3GP input data contains audio stream
		err = DoGetAudioProperties();	
		if (err == KErrNotSupported)
			{
			// If there is no requested type, err will be KErrNotSupported. But in here, KErrNotFound is 
			// more proper.
			err = KErrNotFound;
			}
		else if (err == KErrNone)
			{
			mp4Err = MP4ParseIsFrameAvailable(iHandler, iAudioMp4Type);
			err = SymbianOSError(mp4Err);
			}
		}
	else if (aType == E3GPVideo) 
		{
		// Check if 3GP input data contains video stream
		err = DoGetVideoProperties();	
		if (err == KErrNotSupported)
			{
			// If there is no requested type, err will be KErrNotSupported. But in here, KErrNotFound is 
			// more proper.
			err = KErrNotFound;
			}
		else if (err == KErrNone)
			{
			mp4Err = MP4ParseIsFrameAvailable(iHandler, iVideoMp4Type);
			err = SymbianOSError(mp4Err);
			}
		}
	else
		{
		Panic(KErrArgument); // This should not happen.
		}
	
	if (err == KErrNone)
		{
		aAvailable = ETrue;
		}
	return err;
	}

/**
Returns the number of video frames.

@param	aNum	Number of video frames.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video;		
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetNumberOfVideoFrames(TUint& aNum) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties(); 
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 numVideoFrame = 0;
	MP4Err mp4Err = MP4ParseGetNumberOfVideoFrames(iHandler, &numVideoFrame);
	if (mp4Err == MP4_OK)
		{
		aNum = numVideoFrame;
		}
	return SymbianOSError(mp4Err);
	}

/**
This function gives the video sample entry index of the next video frame to be read.

The smallest index value is 1.

@param	aIndex	Returns the Visual Sample Entry index of the next video frame to be read.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video;		
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetVideoSampleEntryIndex(TUint& aIndex) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties(); 
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 index = 0;
	MP4Err mp4Err = MP4ParseGetVideoSampleEntryIndex(iHandler, &index);
	if (mp4Err == MP4_OK)
		{
		aIndex = index;
		}
	return SymbianOSError(mp4Err);
	}

/**
Returns video frame size.

@param	aIndex	Index of video frame.
@param	aSize	Return the size of the video frame.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video;		
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetVideoFrameSize(TUint aIndex, TUint& aSize) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties(); 
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 videoFrameSize = 0;
	MP4Err mp4Err = MP4ParseGetVideoFrameSize(iHandler, aIndex, &videoFrameSize);
	if (mp4Err == MP4_OK)
		{
		aSize = videoFrameSize;
		}
	return SymbianOSError(mp4Err);
	}

/**
Returns video frame start time.

@param	aIndex					Index of video frame.
@param	aTimeStampInMs			Result in milliseconds.
@param	aTimeStampInTimescale	Result is returned here.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video;		
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetVideoFrameStartTime(TUint aIndex, TUint& aTimeStampInMs, TUint& aTimeStampInTimescale) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties(); 
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 timeStampInMs = 0;
	mp4_u32 timeStampInTimescale = 0;
	MP4Err mp4Err = MP4ParseGetVideoFrameStartTime(iHandler, aIndex, &timeStampInTimescale, &timeStampInMs);
	if (mp4Err == MP4_OK)
		{
		aTimeStampInMs = timeStampInMs;
		aTimeStampInTimescale = timeStampInTimescale;
		}
	return SymbianOSError(mp4Err);
	}

/**
Checks if a video frame of with index aIndex exists.

@param	aIndex		Index of video frame.
@param	aKeyFrame	Return ETrue if the video frame is a key frame.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video;		
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetVideoFrameKeyType(TUint aIndex, TBool& aKeyFrame) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties(); 
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_bool keyFrame = EFalse;
	MP4Err mp4Err = MP4ParseGetVideoFrameType(iHandler, aIndex, &keyFrame);
	if (mp4Err == MP4_OK)
		{
		aKeyFrame = keyFrame;
		}
	return SymbianOSError(mp4Err);
	}

/**
This function gives the audio sample entry index of the next audio frame to be read.

The smallest index value is 1. 

@param	aIndex	Returns the Audio Sample Entry index of the next video frame to be read.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain audio;		
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetAudioSampleEntryIndex(TUint& aIndex) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains audio stream
	TInt err = DoGetAudioProperties(); 
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u32 audioSampleEntryIndex = 0;
	MP4Err mp4Err = MP4ParseGetAudioSampleEntryIndex(iHandler, &audioSampleEntryIndex);
	if (mp4Err == MP4_OK)
		{
		aIndex = audioSampleEntryIndex;
		}
	return SymbianOSError(mp4Err);
	}

/**
This function provides the storage mode of 13K QCELP in 3G2 file. 

In 3G2 files, QCELP can be registered to be stored in two ways:
	using the QCELP Sample Entry ('sqcp') Box or
	using the MPEG4 Audio Sample Description ('esds') Box.

@param	aMode	Returns the QCELP storage mode.  See T3GPQcelpStorageMode.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the 3GP input data does not contain any QCELP audio stream;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetQcelpStorageMode(T3GPQcelpStorageMode& aMode) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains any QCELP audio stream
	TInt err = DoGetAudioProperties();	
	if (err == KErrNone)
		{
		if (iAudioType != E3GPQcelp13K)
			{
			return KErrNotSupported;
			}
		}
	else
		{
		return err;
		}

	mp4_u8 audioQcelpStorageMode = 0;
	MP4Err mp4Err = MP4ParseGetQCELPStorageMode(iHandler, &audioQcelpStorageMode);
	if (mp4Err == MP4_OK)
		{
		if (audioQcelpStorageMode == 1)
			{
			aMode = E3GPMP4AudioDescriptionBox;
			}
		else
			{
			aMode = E3GPQcelpSampleEntryBox;
			}
		}
	return SymbianOSError(mp4Err);
	}

/**
This function provides the video level of the H263 video stream contained in the 3GP data. 

@param	aLevel	Returns the H263 video level.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the 3GP input data does not contain any H263 video stream; 
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse::GetH263VideoLevel(TInt& aLevel) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains any H263 video stream
	TInt err = DoGetVideoProperties();	
	if (err == KErrNone)
		{
		if (!(iVideoType == E3GPH263Profile0 || iVideoType == E3GPH263Profile3))
			{
			return KErrNotSupported;
			}
		}
	else
		{
		return err;
		}
	
	TVideoClipProperties videoClipProperties;
	MP4Err mp4Err = MP4ParseGetVideoClipProperties(iHandler, videoClipProperties);
	if (mp4Err == MP4_OK)
		{
		aLevel = videoClipProperties.iH263Level;
		}
	return SymbianOSError(mp4Err);
	}

/**
Returns the needed size for memory buffer to store an atom of given type from user data atom (UDTA).

@param	aType		Type of atom to be read from UDTA. Hex value of 4 chars representing atom type 
					defined in standard. For example, 0x7469746c is 'titl', title for the media atom.
@param	aLocation	Specifies the location of user information to be retrieved.  Refer to 
					T3GPUdtaLocation for supported values.
@param	aAtomIndex	Specifies the index of atom if UDTA contains multiple sub-atoms of the same 
					aUdtaAtomType to retrieve when supplied as input parameter.  Returns the 
					highest index of the matching sub-atom found in the specified location as 
					output parameter.
@param	aSize		This contains the needed size for memory buffer.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotFound	if UDTA or wanted sub-atom is not available in asked location;
		KErrAccessDenied	if cannot seek to UDTA atom location;
		KErrArgument	if asked aLocation is invalid;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.

@panic C3GPParse	KErrArgument	if the location of user information is not in the range of T3GPUdtaLocation
*/
EXPORT_C TInt C3GPParse::GetUserDataAtomSize(TUint32 aType, T3GPUdtaLocation aLocation, 
		TUint& aAtomIndex, TInt& aSize) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	mp4_u32 userDataAtomSize = 0;
	mp4_u32 atomIndex = aAtomIndex;
	mp4_u32 type = aType;
	mp4_u8 location = UdtaLocation(aLocation);
	MP4Err mp4Err = MP4ParseGetUserDataAtom(iHandler, location, type, 
			NULL, userDataAtomSize, atomIndex);
	if ( mp4Err == MP4_OK  || mp4Err == MP4_OUTPUT_BUFFER_TOO_SMALL)
		{
		aSize = userDataAtomSize;
		aAtomIndex = atomIndex;
		mp4Err = MP4_OK;
		}
	return SymbianOSError(mp4Err);
	}

/**
Retrieves an atom of given type from user data atom (UDTA) to the given buffer.

The buffer returned stores an atom of structure that conforms to the definition of 
a "full box" as specified in ISO/IEC 14496-12:2003: "Information technology – Coding 
of audio-visual objects – Part 12: ISO base media file format."					

For more information on user data atoms, see Section 8 – Asset Information of "3GPP 
TS 26.244 version 6.1.0 – 3GP file format (Rel 6)."

@param	aType		Type of atom to be read from UDTA. Hex value of 4 chars representing atom type 
					defined in standard. For example, 0x7469746c is 'titl', title for the media atom.
@param	aLocation	Specifies the location of user information to be retrieved.  Refer to 
					T3GPUdtaLocation for supported values.
@param	aBuffer		The descriptor to store the requested user data atom.  
@param	aAtomIndex	Specifies the index of atom if UDTA contains multiple sub-atoms of the same 
					aUdtaAtomType to retrieve when supplied as input parameter.  Returns the 
					highest index of the matching sub-atom found in the specified location as 
					output parameter.  Only matching sub-atoms are counted when calculating the highest
					index.  

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotFound	if UDTA or wanted sub-atom is not available in asked location;
		KErrAccessDenied	if cannot seek to UDTA atom location;
		KErrArgument	if asked aUdtaLocation is invalid;
		KErrOverflow	if the buffer to write atom to is too small. Use C3GPParse::GetUserDataAtomSize 
						to retrieve the proper size.
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if if 3GP library needs more data before the requested 
						information can be returned.

@panic C3GPParse	KErrArgument	if the location of user information is not in the range of T3GPUdtaLocation
*/
EXPORT_C TInt C3GPParse::GetUserDataAtom(TUint32 aType, T3GPUdtaLocation aLocation, TDes8& aBuffer, TUint& aAtomIndex) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	if (aBuffer.MaxLength() <= 0)
		{
		return KErrOverflow;
		}
	
	mp4_u32 userDataAtomSize = aBuffer.MaxLength();
	mp4_u32 atomIndex = aAtomIndex;
	mp4_u32 type = aType;
	mp4_u8 location = UdtaLocation(aLocation);
	MP4Err mp4Err = MP4ParseGetUserDataAtom(iHandler, location, type, 
			const_cast<mp4_u8*>(aBuffer.Ptr()), userDataAtomSize, atomIndex);	
	if ( mp4Err == MP4_OK )
		{
		aType = type;
		aBuffer.SetLength(userDataAtomSize);
		aAtomIndex = atomIndex;
		}
	return SymbianOSError(mp4Err);
	}

/**
This function gets the next frame's dependency information from SDTP box.

@param	aDependencies	Returns the next frame's dependency information. See T3GPFrameDependencies.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes. 
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video stream;
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open;
		KErr3gpLibMoreDataRequired	if if 3GP library needs more data before the requested 
						information can be returned.
*/
EXPORT_C TInt C3GPParse::GetVideoFrameDependencies(T3GPFrameDependencies& aDependencies) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties();	
	if (err != KErrNone)
		{
		return err;
		}
	
	mp4_u8 dependsOn;
	mp4_u8 isDependedOn;
	mp4_u8 hasRedundancy;	
	MP4Err mp4Err = MP4ParseNextVideoFrameDependencies(iHandler, &dependsOn, &isDependedOn, &hasRedundancy);
	if (mp4Err == MP4_OK)
		{
		switch (dependsOn)
			{
			case 1:
				aDependencies.iDependsOn = E3GPDependencyExists;
			break;
			case 2:
				aDependencies.iDependsOn = E3GPDependencyNone;
			break;
			case 0:
				aDependencies.iDependsOn = E3GPDependencyUnknown;
			break;
			default:
				Panic(KErrCorrupt);
			}
		
		switch (isDependedOn)
			{
			case 1:
				aDependencies.iIsDependedOn = E3GPDependencyExists;
			break;
			case 2:
				aDependencies.iIsDependedOn = E3GPDependencyNone;
			break;
			case 0:
				aDependencies.iIsDependedOn = E3GPDependencyUnknown;
			break;
			default:
				Panic(KErrCorrupt);
			}
		
		switch (hasRedundancy)
			{
			case 1:
				aDependencies.iHasRedundancy = E3GPRedundancyExists;
			break;
			case 2:
				aDependencies.iHasRedundancy = E3GPRedundancyNone;
			break;
			case 0:
				aDependencies.iHasRedundancy = E3GPRedundancyUnknown;
			break;
			default:
				Panic(KErrCorrupt);
			}
		}
	return SymbianOSError(mp4Err);
	}

/**
This function gets frame properties, from aStartIndex for a count of aNumberOfFrames frames.

Properties obtained are start time, frame type, and frame size, stored in the T3GPFrameInfoParameters 
struct.

@param	aStartIndex		Index to start getting info for the array.
@param	aNumberOfFrames	Number of frames to retrieve frame info.
@param	aArray			An array of T3GPFrameInfoParameters struct to store video frame properties.  
						The input array will be flushed of all existing elements before use.

@return	KErrNone 		if successful.  Otherwise, returns one of the system wide error codes.
		KErrGeneral		if an error has no specific categorisation;
		KErrNotSupported	if the input does not contain video stream;		
		KErrNoMemory	if an attempt to allocate memory has failed;
		KErrAccessDenied	if opening file has failed;		
		KErrNotReady	if the parser has not yet been initialised; See C3GPParse::Open.
*/
EXPORT_C TInt C3GPParse:: GetVideoFrameProperties(TUint aStartIndex, TUint aNumberOfFrames, 
		RArray<T3GPFrameInfoParameters>& aArray) const
	{
	if (!iHandler)
		{
		return KErrNotReady;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties();	
	if (err != KErrNone)
		{
		return err;
		}
	
	// Check if the input parameters are valid before retrieving video frame properties.
	TUint totalNumOfVideoFrames;
	err = GetNumberOfVideoFrames(totalNumOfVideoFrames);
	if (err != KErrNone)
		{
		return err;
		}
	
	if (aStartIndex >= totalNumOfVideoFrames || aNumberOfFrames > totalNumOfVideoFrames)	
		{
		return KErrGeneral;
		}
	
	TFrameInfoParameters* infoArray = new TFrameInfoParameters[aNumberOfFrames];
	
	if (!infoArray)
		{
		return KErrNoMemory;
		}

	mp4_u32 startIndex = aStartIndex;
	mp4_u32 numberOfFrames = aNumberOfFrames;
	MP4Err mp4Err = MP4GetVideoFrameProperties(iHandler, startIndex, numberOfFrames, infoArray);

	if ( mp4Err == MP4_OK )
		{
		aArray.Reset();
		T3GPFrameInfoParameters infoParams; 
		
		for (TInt i=0; i<aNumberOfFrames; ++i)
			{
			infoParams.iSize = infoArray[i].iSize;
			infoParams.iStartTime = infoArray[i].iStartTime;
			infoParams.iIsRandomAccessPoint = infoArray[i].iType;
			err = aArray.Append(infoParams);
			if (err != KErrNone)
				{
				delete [] infoArray;
				infoArray = NULL;
				return err;
				}
			}		
		}
	else
		{
		err = SymbianOSError(mp4Err);
		}
	delete [] infoArray;
	infoArray = NULL;
	return err;
	}

/**
This function reads the current video frame from an input file and returns it to the caller asynchronously.  
The current position of video stream will be moved forward.

This function is not supported when the parser is in buffer mode.

C3GPParse::CancelReadFrame can be used to cancel an outstanding asynchronous frame read request.

Note:  Only one asynchronous parse audio or video frame(s) operation can be ongoing at any given time.

Upon completion, successfully or otherwise, the callback function M3GPParseCallback::VideoFrameAvailable is called.

@param	aCallback	Reference to class derived from M3GPAsyncObserver designed to receive notification of 
					asynchronous event completion.
@param	aBuffer		The descriptor to store the video frames.
*/
EXPORT_C void C3GPParse::ReadVideoFrame(M3GPParseCallback& aCallback, TDes8& aBuffer)
	{
	if (!iHandler)
		{
		aCallback.VideoFrameAvailable(KErrNotReady, EFalse, 0, 0);
		return;
		}
	if (aBuffer.MaxLength() <= 0)
		{
		aCallback.VideoFrameAvailable(KErrOverflow, EFalse, 0, 0);
		return;
		}
	if (iAsyncReadBuffer)
		{
		aCallback.VideoFrameAvailable(KErrInUse, EFalse, 0, 0);
		return;
		}
	
	// Check if 3GP input data contains video stream
	TInt err = DoGetVideoProperties();	
	if (err != KErrNone)
		{
		aCallback.VideoFrameAvailable(err, EFalse, 0, 0);
		return;
		}
	
	iCallback = &aCallback;
	mp4_u32 bufferSize = aBuffer.MaxLength();
	MP4Err mp4Err = MP4ParseReadVideoFrameAsync(iHandler, this, 
			const_cast<mp4_u8*>(aBuffer.Ptr()), &bufferSize);	
	if (mp4Err != MP4_OK)
		{
		aCallback.VideoFrameAvailable(SymbianOSError(mp4Err), EFalse, 0, 0);
		}
	else
		{
		// MP4ParseReadVideoFrameAsync success. Store aBuffer since its length has
		// not been set.
		iAsyncReadBuffer = &aBuffer;
		}
	}

/**
This function reads the audio frames that are stored in the current audio sample from the input 
file and returns them to the caller asynchronously.  The current position of audio stream 
will be moved forward. 

This function is not supported when the parser is in buffer mode.

C3GPParse::CancelReadFrame can be used to cancel an outstanding asynchronous frame read request.

Note:  Only one asynchronous parse audio or video frame(s) operation can be ongoing at any given time.

Upon completion, successfully or otherwise, the callback function M3GPParseCallback::AudioFramesAvailable is called.

@param	aCallback	Reference to class derived from M3GPAsyncObserver designed to receive notification of 
					asynchronous event completion.
@param	aBuffer		The descriptor to store the audio frames.
*/
EXPORT_C void C3GPParse::ReadAudioFrames(M3GPParseCallback& aCallback, TDes8& aBuffer)
	{
	if (!iHandler)
		{
		aCallback.AudioFramesAvailable(KErrNotReady, 0, 0, 0);
		return;
		}
	if (aBuffer.MaxLength() <= 0)
		{
		aCallback.AudioFramesAvailable(KErrOverflow, 0, 0, 0);
		return;
		}
	if (iAsyncReadBuffer)
		{
		aCallback.AudioFramesAvailable(KErrInUse, 0, 0, 0);
		return;
		}
	
	// Check if 3GP input data contains audio stream
	TInt err = DoGetAudioProperties();	
	if (err != KErrNone)
		{
		aCallback.AudioFramesAvailable(err, 0, 0, 0);
		return;
		}
	
	iCallback = &aCallback;
	mp4_u32 bufferSize = aBuffer.MaxLength(); 
	MP4Err mp4Err = MP4ParseReadAudioFramesAsync(iHandler, this, 
			const_cast<mp4_u8*>(aBuffer.Ptr()), &bufferSize);	
	if (mp4Err != MP4_OK)
		{
		aCallback.AudioFramesAvailable(SymbianOSError(mp4Err), 0, 0, 0);
		}
	else
		{
		// MP4ParseReadAudioFramesAsync success. Store aBuffer since its length has
		// not been set.
		iAsyncReadBuffer = &aBuffer;
		}
	}

/**
This function cancels the outstanding asynchronous read audio/video frame request. 

No callback function will be called.

Note:  As only one asynchronous parse audio or video frame(s) operation can be ongoing at any given time, 
this function can be used to cancel audio or video read request.
*/
EXPORT_C void C3GPParse::CancelReadFrame()
	{
	if (!iHandler)
		{
		return;
		}
	if (iAsyncReadBuffer)
		{
		MP4CancelReadFrame(iHandler);
		iAsyncReadBuffer = NULL;
		iCallback = NULL;
		}	
	}

// Receive asynchronous parse video frames operation completion notification.
void C3GPParse::M3GPMP4LibVideoFrameAvailable(MP4Err aError, mp4_u32 aFrameSize, mp4_u32 aTimeStamp, 
		mp4_bool aKeyFrame, mp4_u32 aTimestamp2)
	{	
	// Check if there is an outstanding asynchronous request
	if (iAsyncReadBuffer)
		{
		if (aError == MP4_OK)
			{
			// Set the buffer length for the asynchronous read video frame
			iAsyncReadBuffer->SetLength(aFrameSize);
			}
		iAsyncReadBuffer = NULL;
		// Parser can send out the callback to the client
		TBool keyFrame = aKeyFrame;
		TUint timeStampInMs = aTimeStamp;
		TUint timeStampInTimescale = aTimestamp2;	
		iCallback->VideoFrameAvailable(SymbianOSError(aError), keyFrame, timeStampInMs, timeStampInTimescale);
		}

	iCallback = NULL;
	}

// Receive asyncronous parse audio frames operation completion notification.
void C3GPParse::M3GPMP4LibAudioFramesAvailable(MP4Err aError, mp4_u32 aAudioSize, mp4_u32 aTimeStamp, 
		mp4_u32 aReturnedFrames, mp4_u32 aTimestamp2)
	{	
	// Check if there is an outstanding asynchronous request	
	if (iAsyncReadBuffer)
		{
		if (aError == MP4_OK)
			{
			// Set the buffer length for the asynchronous read audio frame
			iAsyncReadBuffer->SetLength(aAudioSize);
			}
		iAsyncReadBuffer = NULL;
		// Parser can send out the callback to the client
		TUint returnedFrames = aReturnedFrames;
		TUint timeStampInMs = aTimeStamp;
		TUint timeStampInTimescale = aTimestamp2;
		iCallback->AudioFramesAvailable(SymbianOSError(aError), returnedFrames, timeStampInMs, timeStampInTimescale);	
		}

	iCallback = NULL;
	}

// Helper function to convert c style error code to Symbian OS standard error code
TInt C3GPParse::SymbianOSError(MP4Err aError) const
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
		case (MP4_NOT_AVAILABLE):
			error = KErr3gpLibMoreDataRequired;
		break;
		case (MP4_FILE_MODE):
		case (MP4_NOT_STREAMABLE):
		case (MP4_NO_VIDEO):
		case (MP4_NO_AUDIO):
			error = KErrNotSupported;
		break;
		case (MP4_BUFFER_TOO_SMALL):
		case (MP4_OUTPUT_BUFFER_TOO_SMALL):
			error = KErrOverflow;
		break;
		case (MP4_END_OF_VIDEO):
			error = KErrEof;
		break;
		case (MP4_CANT_SEEK):
		case (MP4_NO_FRAME):
		case (MP4_NO_REQUESTED_FRAME):
		case (MP4_UDTA_NOT_FOUND):
			error = KErrNotFound;
		break;
		case (MP4_FILE_ERROR):
			error = KErrAccessDenied;
		break;
		case (MP4_INVALID_INPUT_STREAM):
			error = KErrCorrupt;
		break;
		case (MP4_INVALID_TYPE):
			error = KErrArgument;
		break;
		default:
		// Mapped all possible errors returned by the MP4_library. Anything else should NOT happen
			Panic(KErrArgument);
		}
	return error;
	}

// Help function to get video properties
TInt C3GPParse::DoGetVideoProperties() const
	{
	if (!iVideoPropertiesSaved || iVideoError == KErr3gpLibMoreDataRequired)
		{
		MP4Err mp4Err = MP4_OK;
		mp4_u32 length = 0;
		mp4_u32 type = 0;
		mp4_u32 width = 0;
		mp4_u32 height = 0;
		mp4_u32 timeScale = 0;
		mp4_double frameRate = 0;

		mp4Err = MP4ParseRequestVideoDescription(iHandler, &length, &frameRate, &type, &width, &height, &timeScale);
		iVideoPropertiesSaved = ETrue;
		if (mp4Err == MP4_OK)
			{
			iVideoType = WrapperVideoType(type);
			if (iVideoType == E3GPNoVideo)
				{
				iVideoError = KErrNotSupported;
				return KErrNotSupported;
				}
			iVideoLength = length;
			iVideoMp4Type = type; // Type of video stream			
			iVideoSize.iWidth = width;
			iVideoSize.iHeight = height;
			iVideoTimeScale = timeScale;
			iVideoFrameRate = frameRate;	
			}
		iVideoError = SymbianOSError(mp4Err);
		}
	return iVideoError;
	}

// Help function to get audio properties
TInt C3GPParse::DoGetAudioProperties() const
	{
	if (!iAudioPropertiesSaved || iAudioError == KErr3gpLibMoreDataRequired)
		{
		MP4Err mp4Err = MP4_OK;
		mp4_u32 audioLength = 0;
		mp4_u32 audioType = 0;
		mp4_u32 averateBitRate = 0;
		mp4_u32 timeScale = 0;
		mp4_u8 framesPerSample = 0;
		
		mp4Err = MP4ParseRequestAudioDescription(iHandler, &audioLength, &audioType, 
				&framesPerSample, &timeScale, &averateBitRate);
		iAudioPropertiesSaved = ETrue;
		if (mp4Err == MP4_OK)
			{
			iAudioLength = audioLength;
			iAudioMp4Type = audioType; // Type of audio stream
			iAudioType = WrapperAudioType(audioType);
			iAudioFramesPerSample = framesPerSample;
			iAudioAvgBitRate = averateBitRate;
			iAudioTimeScale = timeScale;
			}
		iAudioError = SymbianOSError(mp4Err);
		}
	return iAudioError;
	}

// Help function to get stream properties
TInt C3GPParse::DoGetStreamProperties() const
	{
	if (!iStreamPropertiesSaved || iStreamError == KErr3gpLibMoreDataRequired)
		{
		MP4Err mp4Err = MP4_OK;
		mp4_u32 streamSize = 0;
		mp4_u32 streamAvgBitRate;
		mp4Err = MP4ParseRequestStreamDescription(iHandler,&streamSize, &streamAvgBitRate);
		iStreamPropertiesSaved = ETrue;
		if (mp4Err == MP4_OK)
			{
			iStreamSize = streamSize;
			iStreamAvgBitRate = streamAvgBitRate;
			}
		iStreamError = SymbianOSError(mp4Err);	
		}
	return iStreamError;
	}

// Helper function to map Mp4 enum type to T3GPVideoType for video
T3GPVideoType C3GPParse::WrapperVideoType(TUint aType) const
	{
	T3GPVideoType videoType = E3GPNoVideo;
	switch (aType)
		{
		case (MP4_TYPE_MPEG4_VIDEO):
			videoType = E3GPMpeg4Video;
		break;
		case (MP4_TYPE_H263_PROFILE_0):
			videoType = E3GPH263Profile0;
		break;
		case (MP4_TYPE_H263_PROFILE_3):
			videoType = E3GPH263Profile3;
		break;
		case (MP4_TYPE_AVC_PROFILE_BASELINE):
			videoType = E3GPAvcProfileBaseline;
		break;
		case (MP4_TYPE_AVC_PROFILE_MAIN):
			videoType = E3GPAvcProfileMain;
		break;
		case (MP4_TYPE_AVC_PROFILE_EXTENDED):
			videoType = E3GPAvcProfileExtended;
		break;
		
		case (MP4_TYPE_AVC_PROFILE_HIGH):
			videoType = E3GPAvcProfileHigh;
		break;
		case MP4_TYPE_NONE:
		break;
		default:
			Panic(KErrCorrupt); // This should not happen.
		break;
		}
	return videoType;
	}

// Helper function to map Mp4 enum type to T3GPAudioType for audio
T3GPAudioType C3GPParse::WrapperAudioType(TUint aType) const
	{
	T3GPAudioType audioType = E3GPNoAudio;
	switch (aType)
		{
		case (MP4_TYPE_MPEG4_AUDIO):
			audioType = E3GPMpeg4Audio;
		break;
		case (MP4_TYPE_AMR_NB):
			audioType = E3GPAmrNB;
		break;
		case (MP4_TYPE_AMR_WB):
			audioType = E3GPAmrWB;
		break;
		case (MP4_TYPE_QCELP_13K):
			audioType = E3GPQcelp13K;
		break;
		default:
			Panic(KErrCorrupt); // This should not happen.
		}
	return audioType;
	}

// Helper function to map enum type of the location of the user data in the file
mp4_u8 C3GPParse::UdtaLocation(T3GPUdtaLocation aLocation) const
	{
	mp4_u8 location  = MP4_UDTA_NONE;
	
	switch (aLocation )
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
		}
	return location;
	}


void C3GPParse::Panic(TInt aPanic)
	// Panic client
	{
	_LIT(K3GPParsePanicName, "C3GPParse");
	User::Panic(K3GPParsePanicName, aPanic);
	}

	
