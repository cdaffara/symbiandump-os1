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

#ifndef C3GPLIBRARY_H
#define C3GPLIBRARY_H

#include <f32file.h>
#include <f32file64.h>
#include <caf/content.h>
#include <e32base.h>
#include <3gplibrary/mp4lib.h>

/**
Error code for C3GPParse API

3GP Parser requires more data before the requested information can be retrieved
*/
const TInt KErr3gpLibMoreDataRequired = (-12300);

class M3GPParseCallback;

/** 
File format to use when composing a file.
@publishedPartner
@released
*/
enum T3GPFileFormatType
	{
	/** 3GP file format. */
	E3GP3GP,
	/** MPEG-4 file format. */
	E3GPMP4,
	/** 3GPP2 file format. */
	E3GP3G2
	};

/** 
Optional flags to be used when composing a file.
@publishedPartner
@released
*/
enum T3GPComposeFlag
	{
	/** Default value. No special setup. */
	E3GPNoFlag = 0x0000,
	/** Metadata is placed at end of file. */
	E3GPMetaDataLast = 0x0001,
	/** Collect metadata in files instead of storing it in memory. */
	E3GPLongClip = 0x0002, 
	};

/** 
Type of stream contained in the file.
@publishedPartner
@released
*/
enum T3GPFrameType
	{
	/** Frame contains audio data. */
	E3GPAudio,
	/** Frame contains video data. */
	E3GPVideo
	};

/** 
Type of video stream contained in the file. 
@publishedPartner
@released
*/
enum T3GPVideoType
	{
	/** No Video */
	E3GPNoVideo,
	/** MPEG-4 video (MPEG-4 Part 2) - Simple and Advance Simple Profiles */
	E3GPMpeg4Video, 
	/** H263 Profile 0  */
	E3GPH263Profile0, 
	/** H263 Profile 3 */
	E3GPH263Profile3,
	/** AVC (also known as H.264) Baseline Profile */               
	E3GPAvcProfileBaseline,
	/** AVC (also known as H.264) Main Profile */               
    E3GPAvcProfileMain,
    /** AVC (also known as H.264) Extended Profile */               
    E3GPAvcProfileExtended,
    /** AVC (also known as H.264) High Profile */               
    E3GPAvcProfileHigh
	};


/** 
Type of audio stream contained in the file.
@publishedPartner
@released
*/
enum T3GPAudioType
	{
	/** No audio */
	E3GPNoAudio,
	/** MPEG-4 audio (MPEG-4 Part 3) */
	E3GPMpeg4Audio,
	/** AMR NB */
	E3GPAmrNB,   
	/** AMR WB */   
	E3GPAmrWB,
	/** QCELP 13K */
	E3GPQcelp13K
	};


/** 
Location of the user data in the file.
@publishedPartner
@released
*/
enum T3GPUdtaLocation
	{
	/** User data resides within the Movie Box. */
	E3GPUdtaMoov,
	/** User data resides within the Track Box for video. */
	E3GPUdtaVideoTrak,
	/** User data resides within the Track Box for audio. */
	E3GPUdtaAudioTrak
	};	

/** 
Storage method of QCELP audio stream. 
@publishedPartner
@released
*/
enum T3GPQcelpStorageMode
	{
	/** QCELP Sample Entry ('sqcp') box is used for the storage of QCELP media data. */
	E3GPQcelpSampleEntryBox,
	/** MP4 Audio Sample Entry ('esds') box is used for the storage of QCELP media data. */
	E3GPMP4AudioDescriptionBox	
	};	

/** 
Define the dependency information between video frames.
@publishedPartner
@released
*/
enum T3GPVideoFrameDependency
	{
	/** The dependency of this sample is unknown or the dependency of other 
	samples on this sample is unknown. */
	E3GPDependencyUnknown = 0,
	/** This sample does depend on others or other samples depend on this sample. */
	E3GPDependencyExists,
	/** This sample does not depend on others or no other sample depends on this sample. */
	E3GPDependencyNone,
	};	

/** 
Define the existence of redundant coding in a frame.
@publishedPartner
@released
*/
enum T3GPVideoFrameRedundancy
	{
	/** It is unknown whether there is redundant coding in this sample. */
	E3GPRedundancyUnknown = 0,
	/** There is redundant coding in this sample. */
	E3GPRedundancyExists,
	/** There is no redundant coding in this sample. */
	E3GPRedundancyNone,
	};		

/** 
Structure containing a frame's dependency information.
@publishedPartner
@released
*/
NONSHARABLE_STRUCT(T3GPFrameDependencies)
	{
	/** Defines the current frame’s dependency.  It can be defined as an I-frame 
	(E3GPDependencyNone), not an I-frame (E3GPDependencyExists), or dependency 
	unknown (E3GPDependencyUnknown).  See T3GPVideoFrameDependency.
	*/
	T3GPVideoFrameDependency iDependsOn;
	/** Defines the dependency of other frames on this frame. It can be defined as a disposable 
	frame (E3GPDependencyNone), non-disposable (E3GPDependencyExists), or dependency unknown 
	(E3GPDependencyUnknown).  See T3GPVideoFrameDependency.
	*/
	T3GPVideoFrameDependency iIsDependedOn;
	/** Defines the existence of redundant coding in this frame.  See T3GPVideoFrameRedundancy. */
	T3GPVideoFrameRedundancy iHasRedundancy;
	};

/** 
Structure containing video frame properties.
@publishedPartner
@released
*/
NONSHARABLE_STRUCT(T3GPFrameInfoParameters)
	{
	/** Frame size. */
	TUint32 iSize;
	/** Frame start time. */
	TUint32 iStartTime;
	/** ETrue if frame is an intra or random access point. */
	TBool iIsRandomAccessPoint;
	};

/**
This class should not be used directly.  It stores common properties amongst all supported 
types of video data. 

Note: iTimescale should be chosen so that the duration of each sample is an integer.  It should also 
be chosen so that the duration of the presentation fits into 32 bits.

@publishedPartner
@released
*/	
NONSHARABLE_CLASS(T3GPVideoPropertiesBase)
	{	
protected:
	/**
 	@internalTechnology
 	*/
	T3GPVideoPropertiesBase(T3GPVideoType aType, TUint aTimescale, const TSize& aSize);
public:
	/** Type of video stream contained in the file. */
	T3GPVideoType iType;
	/** Timescale of the video data.  This is the number of time units that pass in one second. */
	TUint iTimescale;
	/** Video width and height in pixels */
	TSize iSize;	
	};

/** 
This structure stores the common and MPEG-4 video specific properties of video data
@publishedPartner
@released
*/
NONSHARABLE_CLASS(T3GPVideoPropertiesMpeg4Video) : public T3GPVideoPropertiesBase
	{
public:
	IMPORT_C T3GPVideoPropertiesMpeg4Video(TUint aTimescale, const TSize& aSize, TUint aMaxBitRate, TUint aAvgBitRate, const TDesC8& aDecoderSpecificInfo);
public:
	/** Maximum video bit rate */
	TUint iMaxBitRate;
	/** Average video bit rate */
	TUint iAvgBitRate;	
	/** MPEG-4 video decoder specific info data stored in an ESDS atom */
	const TDesC8& iDecoderSpecificInfo;
	};

/** 
This structure stores the common and H.263 specific properties of video data 
@publishedPartner
@released
*/
NONSHARABLE_CLASS(T3GPVideoPropertiesH263) : public T3GPVideoPropertiesBase
	{
public:
	enum TProfile { EProfile0, EProfile3 };

public:
	IMPORT_C T3GPVideoPropertiesH263(TUint aTimescale, const TSize& aSize, TInt aVideoLevel, TProfile aProfile);	
public:
	/** Indicates the H263 video level. */
	TInt iVideoLevel;
	};

/** 
This structure stores the common and AVC-specific properties of video data. 
@publishedPartner
@released
*/
NONSHARABLE_CLASS(T3GPVideoPropertiesAvc) : public T3GPVideoPropertiesBase
	{
public:
	IMPORT_C T3GPVideoPropertiesAvc(TUint aTimescale, const TSize& aSize, const TDesC8& aDecoderSpecificInfo);	
public:
	/** AVCDecoderConfigurationRecord data that will be stored in the avcC atom. */
	const TDesC8& iDecoderSpecificInfo;
	};

/** 
This class should not be used directly.  This structure stores common properties amongst all supported types of audio data 
@publishedPartner
@released
*/
NONSHARABLE_CLASS(T3GPAudioPropertiesBase)
	{
protected:
	/**
 	@internalTechnology
 	*/
	T3GPAudioPropertiesBase(T3GPAudioType aType, TUint aTimescale, TInt aFramesPerSample);
public:
	/** Type of audio stream contained in the file */
	T3GPAudioType iType;
	/** Timescale of the audio data.  This is the number of time units that pass in one second.  It 
	 must be smaller than 65536. */
	TUint iTimescale;
	/** Frames per sample.  It must be smaller than 512. MPEG-4 audio has a fixed value of 1. */
	TInt iFramesPerSample;
	};

/** 
This structure stores the common and MPEG-4 audio specific properties of audio data 
@publishedPartner
@released
*/
NONSHARABLE_CLASS(T3GPAudioPropertiesMpeg4Audio) : public T3GPAudioPropertiesBase
	{
public:
	IMPORT_C T3GPAudioPropertiesMpeg4Audio(TUint aTimescale, const TDesC8& aDecoderSpecificInfo);
public:
	/** MPEG-4 audio decoder specific info data stored in an ESDS atom */
	const TDesC8& iDecoderSpecificInfo;	
	};

/** 
This structure stores the common and AMR specific properties of audio data 
@publishedPartner
@released
*/
NONSHARABLE_CLASS(T3GPAudioPropertiesAmr) : public T3GPAudioPropertiesBase
	{
public:
	enum TSpeechCodec{ EAmrWB, EAmrNB };

public:
	IMPORT_C T3GPAudioPropertiesAmr(TUint aTimescale, TInt aFramesPerSample, TInt aModeSet, TSpeechCodec aCodec);
public:	
	/** AMR mode set. */
	TInt iModeSet;
	};

/** 
This structure stores the common and Qcelp specific properties of audio data 
@publishedPartner
@released
*/
NONSHARABLE_CLASS(T3GPAudioPropertiesQcelp) : public T3GPAudioPropertiesBase
	{
public:
	IMPORT_C T3GPAudioPropertiesQcelp(TUint aTimescale, TInt aFramesPerSample, const TDesC8& aDecoderSpecificInfo);
	IMPORT_C T3GPAudioPropertiesQcelp(TUint aTimescale, TInt aFramesPerSample);
public:	
	/** Specifies the mode to store QCELP audio data.
	In 3G2 files, QCELP can be registered to be stored in two ways:
	- Use QCELP Sample Entry (‘sqcp’) Box.  This is the default mode.
	- Use MPEG4 Audio Sample Description (‘esds’) Box.  If this mode is selected, decoder specific info should be supplied.
	See T3GPQcelpStorageMode. */
	T3GPQcelpStorageMode iMode;	
	/** MPEG-4 audio decoder specific info data stored in an ESDS atom.
	If this is provided as an input parameter, the storage mode is automatically set to MPEG4 Audio 
	Sample Description Box mode	 */
	const TDesC8& iDecoderSpecificInfo;
	};

/** 
@publishedPartner
@released

The 3GP composer accepts H.263 /H.264 / MPEG-4 / AVC video, and AMR / MPEG-4 AAC / QCELP 
audio as input to create a 3GP / 3G2 / MP4 file. 
*/
NONSHARABLE_CLASS(C3GPCompose) : public CBase
	{
public:
	IMPORT_C static C3GPCompose* NewL();
	IMPORT_C static C3GPCompose* NewL(TInt aMediaWriteBufferSize, TInt aWriteBufferMaxCount);	
	IMPORT_C ~C3GPCompose();
	
	IMPORT_C TInt Open(T3GPFileFormatType aFileFormat, const T3GPVideoPropertiesBase* aVideo, const T3GPAudioPropertiesBase* aAudio, const TDesC& aFilename, TUint aFlags = E3GPNoFlag);	
	IMPORT_C TInt Open(T3GPFileFormatType aFileFormat, const T3GPVideoPropertiesBase* aVideo, const T3GPAudioPropertiesBase* aAudio, RFile& aFile, TUint aFlags = E3GPNoFlag);
	IMPORT_C TInt Open(T3GPFileFormatType aFileFormat, const T3GPVideoPropertiesBase* aVideo, const T3GPAudioPropertiesBase* aAudio, RFile64& aFile, TUint aFlags = E3GPNoFlag);
	IMPORT_C TInt WriteVideoFrame(const TDesC8& aBuffer, TUint aDuration, TBool aKeyFrame);	
	IMPORT_C TInt WriteVideoFrame(const TDesC8& aBuffer, TUint aDuration, TBool aKeyFrame, const T3GPFrameDependencies& aDependencies);
	IMPORT_C TInt WriteAudioFrames(const TDesC8& aBuffer, TUint aDuration);
	IMPORT_C TInt SetUserData(T3GPUdtaLocation aLocation, const TDesC8& aBuffer);     	
	IMPORT_C TInt Complete();
private:
	C3GPCompose(TInt aMediaWriteBufferSize = 0, TInt aWriteBufferMaxCount = 0);
	
	TInt SymbianOSError(MP4Err aError);
	mp4_u32 Mp4Type(const T3GPVideoPropertiesBase* aVideo, const T3GPAudioPropertiesBase* aAudio);
	TInt SetComposeProperties(const T3GPVideoPropertiesBase* aVideo, const T3GPAudioPropertiesBase* aAudio, T3GPFileFormatType aFileFormat, TUint aFlag);
	TInt SetMPeg4VideoProperties(const T3GPVideoPropertiesBase* aVideo);
	TInt SetAvcVideoProperties(const T3GPVideoPropertiesBase* aVideo);
	TInt SetH263VideoProperties(const T3GPVideoPropertiesBase* aVideo);
	TInt SetMpeg4AudioProperties(const T3GPAudioPropertiesBase* aAudio);
	TInt SetAmrAudioProperties(const T3GPAudioPropertiesBase* aAudio);
	TInt SetQcelpAudioProperties(const T3GPAudioPropertiesBase* aAudio);
	TInt SetComposeFlag(T3GPFileFormatType aFileFormat, TUint aFlag);
	void Reset();
	static void Panic(TInt aPanic);
private:
	MP4Handle iHandler;
	TInt iMediaWriteBufferSize;
	TInt iWriteBufferMaxCount;
	TBool iHasVideo;
	TBool iHasAudio;
	TBool iDuplicateFileHandleCreated;
	RFile64 iFile;
	};


/**
The Parser provides APIs to parse 3GP, 3G2 and MP4 contents (from a file, a CAF object, or given 
through memory buffer), containing H.263/MPEG-4/H.264 video and AMR/MPEG-4/QCELP audio.
@publishedPartner
@released
*/
NONSHARABLE_CLASS(C3GPParse) : public CBase, public M3GPMP4LibAsyncObserver
	{
public:
	IMPORT_C static C3GPParse* NewL();
	IMPORT_C static C3GPParse* NewL(TInt aReadBufferSize);
	IMPORT_C ~C3GPParse();
	
	IMPORT_C TInt Open();
	IMPORT_C TInt Open(const TDesC& aFilename);
	IMPORT_C TInt Open(const RFile& aFile);
	IMPORT_C TInt Open(const RFile64& aFile);
	IMPORT_C TInt Open(const ContentAccess::CData& aData);
	IMPORT_C TInt InsertData(const TDesC8& aBuffer);
	IMPORT_C TInt GetVideoProperties(T3GPVideoType& aType, TUint& aLength, TReal& aFrameRate, TUint& aAvgBitRate, TSize& aSize, TUint& aTimeScale) const;
	IMPORT_C TInt GetAudioProperties(T3GPAudioType& aType, TUint& aLength, TInt& aFramesPerSample, TUint& aAvgBitRate, TUint& aTimeScale) const;
	IMPORT_C TInt GetContainerProperties(TUint& aSize, TUint& aAvgBitRate) const;
	IMPORT_C TInt GetNumBufferedBytes(TInt& aNum) const;
	IMPORT_C TInt GetFrameType(T3GPFrameType& aType) const;
	IMPORT_C TInt GetVideoFrameSize(TUint& aSize) const;
	IMPORT_C TInt GetAudioFramesSize(TUint& aSize) const;	
	IMPORT_C TInt GetVideoDecoderSpecificInfoSize(TInt& aSize) const;
	IMPORT_C TInt GetVideoDecoderSpecificInfo(TDes8& aInfo) const;
	IMPORT_C TInt GetAudioDecoderSpecificInfoSize(TInt& aSize) const;
	IMPORT_C TInt GetAudioDecoderSpecificInfo(TDes8& aBuffer) const;
	IMPORT_C TInt GetVideoTimestamp(TUint& aTimeStampInMs, TUint& aTimeStampInTimescale) const;
	IMPORT_C TInt GetStreamable(TBool& aStreamable) const;
	IMPORT_C TInt Seek(TUint aPosition, TBool aKeyFrame, TUint& aAudioPosition, TUint& aVideoPosition) const;	
	IMPORT_C TInt GetFrameAvailability(T3GPFrameType aType, TBool& aAvailable) const;	
	IMPORT_C TInt GetNumberOfVideoFrames(TUint& aNum) const;
	IMPORT_C TInt GetVideoSampleEntryIndex(TUint& aIndex) const;
	IMPORT_C TInt GetVideoFrameSize(TUint aIndex, TUint& aSize) const;
	IMPORT_C TInt GetVideoFrameStartTime(TUint aIndex, TUint& aTimeStampInMs, TUint& aTimeStampInTimescale) const;	
	IMPORT_C TInt GetVideoFrameKeyType(TUint aIndex, TBool& aKeyFrame) const;
	IMPORT_C TInt GetAudioSampleEntryIndex(TUint& aIndex) const;	
	IMPORT_C TInt GetQcelpStorageMode(T3GPQcelpStorageMode& aMode) const;
	IMPORT_C TInt GetH263VideoLevel(TInt& aLevel) const;
	IMPORT_C TInt GetUserDataAtomSize(TUint32 aType, T3GPUdtaLocation aLocation, TUint& aAtomIndex, TInt& aSize) const;
	IMPORT_C TInt GetUserDataAtom(TUint32 aType, T3GPUdtaLocation aLocation, TDes8& aBuffer, TUint& aAtomIndex) const;
	IMPORT_C TInt GetVideoFrameDependencies(T3GPFrameDependencies& aDependencies) const;
	IMPORT_C TInt GetVideoFrameProperties(TUint aStartIndex, TUint aNumberOfFrames, RArray<T3GPFrameInfoParameters>& aArray) const;	
	IMPORT_C TInt ReadVideoFrame(TDes8& aBuffer, TBool& aKeyFrame, TUint& aTimeStampInMs, TUint& aTimeStampInTimescale) const;	
	IMPORT_C void ReadVideoFrame(M3GPParseCallback& aCallback, TDes8& aBuffer);
	IMPORT_C TInt ReadAudioFrames(TDes8& aBuffer, TInt& aReturnedFrames, TUint& aTimeStampInMs, TUint& aTimeStampInTimescale) const;
	IMPORT_C void ReadAudioFrames(M3GPParseCallback& aCallback, TDes8& aBuffer);
	IMPORT_C void CancelReadFrame();
	IMPORT_C TInt Complete();
private:
	C3GPParse(TInt aReadBufferSize = 0);
		
	TInt SymbianOSError(MP4Err aError) const;
	TInt DoGetVideoProperties()  const; 
	TInt DoGetAudioProperties() const;
	TInt DoGetStreamProperties() const;
	T3GPVideoType WrapperVideoType(TUint aType) const;
	T3GPAudioType WrapperAudioType(TUint aType) const;
	mp4_u8 UdtaLocation(T3GPUdtaLocation aLocation) const;
	void Reset();
	static void Panic(TInt aPanic);
	
	// From M3GPMP4LibAsyncObserver
	void M3GPMP4LibVideoFrameAvailable(MP4Err aError, mp4_u32 aFrameSize, mp4_u32 aTimeStamp, mp4_bool aKeyFrame, mp4_u32 aTimestamp2); 
	void M3GPMP4LibAudioFramesAvailable(MP4Err aError, mp4_u32 aAudioSize, mp4_u32 aTimeStamp, mp4_u32 aReturnedFrames, mp4_u32 aTimestamp2);
	
private:
	MP4Handle iHandler;
	TInt iReadBufferSize;
	
	// Async read
	M3GPParseCallback* iCallback;
	TDes8* iAsyncReadBuffer;
	

	TBool iDuplicateFileHandleCreated;
	RFile64 iFile;
	// Video properties
	mutable T3GPVideoType iVideoType;
	mutable TUint iVideoLength;
	mutable TUint iVideoMp4Type;
	mutable TReal iVideoFrameRate; 
	mutable TSize iVideoSize; 
	mutable TUint iVideoTimeScale;
	mutable TBool iVideoPropertiesSaved;
	mutable TInt iVideoError;
	
	// Audio Properties
	mutable T3GPAudioType iAudioType;
	mutable TUint iAudioLength;
	mutable TUint iAudioMp4Type;
	mutable TReal iAudioFramesPerSample; 
	mutable TSize iAudioSize; 
	mutable TUint iAudioTimeScale;
	mutable TUint iAudioAvgBitRate;
	mutable TBool iAudioPropertiesSaved;
	mutable TInt iAudioError;
	
	// Stream properties
	mutable TUint iStreamAvgBitRate;
	mutable TUint iStreamSize;
	mutable TBool iStreamPropertiesSaved;
	mutable TInt iStreamError;
    };

/**
A mixin class to be implemented by the client in order to use the asynchronous frame retrieval 
APIs provided by the C3GPParse class.  The derived class methods are called by C3GPParse when 
the audio/video frames retrieval completes, successful or not.
@publishedPartner
@released
*/
class M3GPParseCallback
	{
public:
	/**
	This is called upon the completion of an asynchronous request to retrieve audio 
	frames by the API C3GPParse::ReadAudioFramesAsync.
	
	@see C3GPParse::ReadAudioFramesAsync
	
	@param	aError			Returns KErrNone on success.  Returns KErrAccessDenied if the 3GP Parser is in buffer mode. 
							Otherwise, returns one of the system wide error codes. See synchronous version of 
							C3GPParse::ReadAudioFrames.
	@param	aReturnedFrames	Number of the returned frames or 0 if not known.
	@param	aTimeStampInMs	Audio frame presentation time in milliseconds from the beginning of the audio sequence.	
	@param	aTimeStampInTimescale	Audio frame presentation time in timescale from the beginning of the audio sequence.
	*/
	virtual void AudioFramesAvailable(TInt aError, TUint aReturnedFrames, TUint aTimeStampInMs, TUint aTimeStampInTimescale) = 0;
	
	/**
	VideoFrameAvailable is called upon completion of an asynchronous request to retrieve 
	video frames by the API C3GPParse::ReadVideoFrameAsync.
	
	@see C3GPParse::ReadVideoFrameAsync
	
	@param	aError		Returns KErrNone on success.  Returns KErrAccessDenied if the 3GP Parser is in buffer mode.  
						Otherwise, returns one of the system wide error codes. See synchronous version of 
						C3GPParse::ReadVideoFrame.
	@param	aKeyFrame	Returns ETrue if the current frame is a key frame (intra), otherwise the value is EFalse.
	@param	aTimeStampInMs	Video frame presentation time in milliseconds from the beginning of the video sequence.	
	@param	aTimeStampInTimescale	Video frame presentation time in timescale from the beginning of the video sequence.
	*/
	virtual void VideoFrameAvailable(TInt aError, TBool aKeyFrame, TUint aTimeStampInMs, TUint aTimeStampInTimescale) = 0;
	};

#endif // C3GPLIBRARY_H
