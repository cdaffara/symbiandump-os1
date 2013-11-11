// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @publishedPartner
 @deprecated
*/

#ifndef _MP4LIB_H
#define _MP4LIB_H

#define MP4Handle mp4_i32*

#include <f32file.h>
#include <caf/caf.h>
#include <3gplibrary/mp4config.h>

/**
Provides async parse read observer.
*/
class M3GPMP4LibAsyncObserver
{
   public:

        /**
        Informs the client about asyncronous parse audio frames operation completion.
        
        @param aError     		Error code.
        @param aAudioSize		Size of the returned audio frames (in bytes).
        @param aTimeStamp 		Audio frame presentation time in milliseconds from the
                   				beginning of the audio sequence.
        @param aReturnedFrames	Number of frames returned, or 0 if not known.
        @param aTimestamp2		Audio frame presentation time in timescale from the
                   				beginning of the audio sequence
        
        */
        virtual void M3GPMP4LibAudioFramesAvailable(MP4Err aError,
        											mp4_u32 aAudioSize, 
        											mp4_u32 aTimeStamp, 
        											mp4_u32 aReturnedFrames,
 					                              	mp4_u32 aTimestamp2) = 0;    

        /**
        Informs the client about asyncronous parse video frame operation completion.

        @param aError     	Error code.
        @param aFrameSize   Size of the returned frame (in bytes).
        @param aTimeStamp   Video frame presentation time in milliseconds from the 
        					beginning of the video sequence
        @param aKeyFrame	Returns MP4TRUE if the current frame is a key frame (intra), 
        					otherwise the value is MP4FALSE
        @param aTimestamp2	Video frame presentation time in timescale from the beginning 
        					of the video sequence
        
        */
        virtual void M3GPMP4LibVideoFrameAvailable(MP4Err aError,
        										   mp4_u32 aFrameSize, 
        										   mp4_u32 aTimeStamp, 
        										   mp4_bool aKeyFrame, 
        										   mp4_u32 aTimestamp2) = 0;    
};

/**  
This class provides async temporary file deleting observer.
*/
class M3GPMP4LibAsyncTempFileRemoverObserver
{
   public:

        /**
        Informs the client about unused temporary file that can be deleted.
        @param tempFileName		Name of the temporary file that can be deleted.
                     			Ownership of the file name memory buffer is transferred.
        */
        virtual void M3GPMP4LibDeleteTempFileName( MP4FileName tempFileName ) = 0;    

};
#ifdef __cplusplus
extern "C" {
#endif


/**
Error codes returned by the library
*/
enum
	{
	/** Success */
  	MP4_OK,   
  	/** Generic error */                 
	MP4_ERROR,   
	/** Library can't allocate memory */              
	MP4_OUT_OF_MEMORY,   
	/** MP4 library needs more data before the
       requested information can be returned */      
	MP4_NOT_AVAILABLE,  
	/** The library has been opened with the input in a file */       
	MP4_FILE_MODE, 
	/** The input is not streamable */            
	MP4_NOT_STREAMABLE,
	/** Frame of requested type is not available */        
	MP4_NO_REQUESTED_FRAME, 
	/** Requested frame doesn't fit into the given buffer */   
	MP4_BUFFER_TOO_SMALL,
	/** No more video frames left */      
	MP4_END_OF_VIDEO,
	/** Can't seek to requested position */          
	MP4_CANT_SEEK,  
	/** Error in file handling */           
	MP4_FILE_ERROR,        
	/** Input stream is invalid */    
	MP4_INVALID_INPUT_STREAM, 
	/** Frame doesn't exist (previous frame was last) */ 
	MP4_NO_FRAME,   
	/** Illegal type */           
	MP4_INVALID_TYPE, 
	/** Timescale has not been set */         
	MP4_TIMESCALE_NOT_SET,
	/** Error writing metadata */     
	MP4_METADATA_ERROR,
	/** No video in input stream */        
	MP4_NO_VIDEO, 
	/** No audio in input stream */             
	MP4_NO_AUDIO,  
	/** Output buffer has not been initialized for buffer writing */             
	MP4_NO_OUTPUT_BUFFER, 
	/** Not enough space in output buffer to write to */      
	MP4_OUTPUT_BUFFER_TOO_SMALL,
	/** No UDTA data found */	
	MP4_UDTA_NOT_FOUND		 
	};


/** 
Possible data source types
*/
enum
	{
	/** Data is read from an RFile64 handle */
	MP4_SOURCE_RFILE,   
	/** Data is read via the CAF */        
	MP4_SOURCE_CAF,             
	};

/** 
Possible input/output values for audio/video type
*/

/** No audio or video */
#define MP4_TYPE_NONE              0x0000
/** Video Type. MPEG-4 video (MPEG-4 Part 2) - Simple and Advance Simple Profiles */
#define MP4_TYPE_MPEG4_VIDEO       0x0001
/** Audio Type.  MPEG-4 audio (MPEG-4 Part 3) */
#define MP4_TYPE_MPEG4_AUDIO       0x0002
/** Video Type. H263 Profile 0  */
#define MP4_TYPE_H263_PROFILE_0    0x0004
/** Video Type. H263 Profile 3  */
#define MP4_TYPE_H263_PROFILE_3    0x0008
/** Audio Type. AMR NB */
#define MP4_TYPE_AMR_NB            0x0010
/** Audio Type. AMR WB */
#define MP4_TYPE_AMR_WB            0x0020
/** Type not supported */
#define MP4_TYPE_UNSUPPORTED       0x0040
/** AVC baseline (MPEG-4 Part 10) */
#define MP4_TYPE_AVC_PROFILE_BASELINE      0x0080
/** AVC main (MPEG-4 Part 10) */
#define MP4_TYPE_AVC_PROFILE_MAIN          0x0100
/** AVC extended (MPEG-4 Part 10) */
#define MP4_TYPE_AVC_PROFILE_EXTENDED      0x0200
/** QCELP 13K additions */
#define MP4_TYPE_QCELP_13K         0x0400
/** AVC high (MPEG-4 Part 10) */
#define MP4_TYPE_AVC_PROFILE_HIGH      0x0800

/**
Flags used by composer
*/
/** No special setup */
#define MP4_FLAG_NONE              0x0000
/** Meta data is placed at end of file */
#define MP4_FLAG_METADATALAST      0x0001
/** Collect metadata in files instead of storing it in memory */
#define MP4_FLAG_LONGCLIP          0x0002
/** Generate a 3G2 File instead of a 3GP File */
#define MP4_FLAG_GENERATE_3G2      0x0004
/** Generate a MP4 File instead of a 3GP File */
#define MP4_FLAG_GENERATE_MP4      0x0008
/** Use large output buffers for writing */
#define MP4_FLAG_LARGEFILEBUFFER   0x0010


/** Size of buf in MP4HandleStruct must be at least 128 bytes */
#define TMPBUFSIZE 128  
/** Duration of one AMR frame is 20 ms */         
#define AUDIO_FRAME_DURATION 20  

#define MP4FALSE (mp4_bool)0
#define MP4TRUE  (mp4_bool)1

/** 
Possible locations for user data atom (UDTA)
*/
/** No user data */
#define MP4_UDTA_NONE         0x0000
/** User data resides within the Movie Box */
#define MP4_UDTA_MOOV         0x0001
/** User data resides within the Track Box for video */
#define MP4_UDTA_VIDEOTRAK    0x0002
/** User data resides within the Track Box for audio */
#define MP4_UDTA_AUDIOTRAK    0x0004

/**
Initializes the MP4 composing operation. The instance handle of the library is 
set by the function.
 
The possible values of type parameter are

     MP4_TYPE_H263_PROFILE_0
     MP4_TYPE_H263_PROFILE_3
     MP4_TYPE_MPEG4_VIDEO
     MP4_TYPE_AMR_NB
     MP4_TYPE_AMR_WB
     MP4_TYPE_MPEG4_AUDIO
     MP4_TYPE_NONE
     MP4_TYPE_AVC_PROFILE_BASELINE 
     MP4_TYPE_AVC_PROFILE_MAIN
     MP4_TYPE_AVC_PROFILE_EXTENDED
	 MP4_TYPE_AVC_PROFILE_HIGH
     MP4_TYPE_QCELP_13K

In addition, any combination of one video and one audio type are
possible. For example

     MP4_TYPE_H263_PROFILE_0 | MP4_TYPE_AMR_NB

The output file is placed in the current working directory if no path is
specified. The type of filename is dependent on the architecture. When
compiling in Symbian OS, this is a unicode string.

 
@param handle		MP4 library handle
@param filename	Name of the output MP4 file
@param type		Type of the output file
 
@return	MP4_OK             Success
 			MP4_ERROR          Generic error
 			MP4_FILE_ERROR     Error opening file
 			MP4_OUT_OF_MEMORY  Out of memory
*/
extern IMPORT_C MP4Err MP4ComposeOpen(MP4Handle *handle, MP4FileName filename, mp4_u32 type);

/**
This function initializes the MP4 composing operation. The instance
handle of the library is set by the function.
 
The possible values of type parameter are
 
 	 MP4_TYPE_H263_PROFILE_0
     MP4_TYPE_H263_PROFILE_3
     MP4_TYPE_MPEG4_VIDEO
     MP4_TYPE_AMR_NB
     MP4_TYPE_AMR_WB
     MP4_TYPE_MPEG4_AUDIO
     MP4_TYPE_NONE
     MP4_TYPE_AVC_PROFILE_BASELINE 
     MP4_TYPE_AVC_PROFILE_MAIN
     MP4_TYPE_AVC_PROFILE_EXTENDED
	 MP4_TYPE_AVC_PROFILE_HIGH
     MP4_TYPE_QCELP_13K
     
In addition, any combination of one video and one audio type are possible. For example
 
     MP4_TYPE_H263_PROFILE_0 | MP4_TYPE_AMR_NB
 
The output is placed in the buffer given as argument. 
Size of composed data is in composedSize variable.
 
@param handle	       MP4 library handle
@param type           Type of the output file
@param composeBuffer  Buffer to write to.
@param composedSize   Size of composed data.
 
@return 
   MP4_OK                 Success
   MP4_ERROR              Generic error
   MP4_FILE_ERROR         Error opening file
   MP4_OUT_OF_MEMORY      Out of memory
   MP4_NO_OUTPUT_BUFFER   No composed buffer 
*/
extern IMPORT_C MP4Err MP4ComposeOpenToBuffer(MP4Handle *handle, mp4_u32 type, mp4_u8* composeBuffer, mp4_u32 *composedSize);

/** 
This function initializes the MP4 composing operation to existing file handle. The instance
handle of the library is set by the function.
 
NOTE: MP4_FLAG_METADATALAST must be defined in MP4ComposeSetFlags if MP4ComposeOpenFileHandle is used!
 
The possible values of type parameter are
 
     MP4_TYPE_H263_PROFILE_0
     MP4_TYPE_H263_PROFILE_3
     MP4_TYPE_MPEG4_VIDEO
     MP4_TYPE_AMR_NB
     MP4_TYPE_AMR_WB
     MP4_TYPE_MPEG4_AUDIO
     MP4_TYPE_NONE
     MP4_TYPE_AVC_PROFILE_BASELINE 
     MP4_TYPE_AVC_PROFILE_MAIN
     MP4_TYPE_AVC_PROFILE_EXTENDED
	 MP4_TYPE_AVC_PROFILE_HIGH
     MP4_TYPE_QCELP_13K
 
In addition, any combination of one video and one audio type are possible. For example
 
      MP4_TYPE_H263_PROFILE_0 | MP4_TYPE_AMR_NB
 
The output file is placed in the current working directory if no path is
specified. The type of filename is dependent on the architecture. When
compiling in Symbian OS, this is a unicode string.
 
@param handle             MP4 library handle
@param composedfile       File handle to output file
@param metadataTempDrive  If MP4ComposeSetFlags() is configured to use temporary files for metadata
                          (MP4_FLAG_LONGCLIP) this is way to tell library which drive to use for 
                          temporary files.
@param type               Type of the output file
 
@return     MP4_OK             Success
            MP4_ERROR          Generic error
            MP4_FILE_ERROR     Error opening file
            MP4_OUT_OF_MEMORY  Out of memory
*/
extern IMPORT_C MP4Err MP4ComposeOpenFileHandle(MP4Handle *handle, RFile *composedfile, TDriveNumber metadataTempDrive, mp4_u32 type);

/** 
This function initializes the MP4 composing operation to existing file handle. The instance
handle of the library is set by the function.
 
NOTE: MP4_FLAG_METADATALAST must be defined in MP4ComposeSetFlags if MP4ComposeOpenFileHandle is used!
 
The possible values of type parameter are
 
  	 MP4_TYPE_H263_PROFILE_0
     MP4_TYPE_H263_PROFILE_3
     MP4_TYPE_MPEG4_VIDEO
     MP4_TYPE_AMR_NB
     MP4_TYPE_AMR_WB
     MP4_TYPE_MPEG4_AUDIO
     MP4_TYPE_NONE
     MP4_TYPE_AVC_PROFILE_BASELINE 
     MP4_TYPE_AVC_PROFILE_MAIN
     MP4_TYPE_AVC_PROFILE_EXTENDED
	 MP4_TYPE_AVC_PROFILE_HIGH
     MP4_TYPE_QCELP_13K
 
In addition, any combination of one video and one audio type are possible. For example
 
      MP4_TYPE_H263_PROFILE_0 | MP4_TYPE_AMR_NB
 
The output file is placed in the current working directory if no path is
specified. The type of filename is dependent on the architecture. When
compiling in Symbian OS, this is a unicode string.
 
@param handle             MP4 library handle
@param composedfile       File handle to output file
@param metadataTempDrive  If MP4ComposeSetFlags() is configured to use temporary files for metadata
						  (MP4_FLAG_LONGCLIP) this is way to tell library which drive to use for 
  						  temporary files.
@param type               Type of the output file
 
@return		MP4_OK             Success
 			MP4_ERROR          Generic error
 			MP4_FILE_ERROR     Error opening file
 			MP4_OUT_OF_MEMORY  Out of memory
*/
extern IMPORT_C MP4Err MP4ComposeOpenFileHandle64(MP4Handle *handle, RFile64 *composedfile, TDriveNumber metadataTempDrive, mp4_u32 type);

/**
This function closes the MP4 composing operation. It frees the memory
allocated by the library instance and closes the output MP4 file.
 
It is necessary to call this function before the output file is
guaranteed to be a valid MP4 file even though the file may exist prior to
the call.
 
The library handle is not valid after this call.
 
@param handle   MP4 library handle
 
@return		MP4_OK              Success
 			MP4_ERROR           Generic error
 			MP4_METADATA_ERROR  Metadata could not be written
*/
extern IMPORT_C MP4Err MP4ComposeClose(MP4Handle handle);

/**
This function is used to inform the MP4 library about the video data. The
data given to the library is inserted into the MP4 file headers.
 
The parameter timescale indicates the timescale of the video data
to be inserted. Timescale tells how many units of time pass in one second.
 
Note: Timescale should be chosen so that the duration of each sample is
an integer. Timescale should also be chosen so that the duration
of the presentation fits into 32 bits.
 
@param handle   	MP4 library handle
@param timescale    Timescale of video
@param width        Video width
@param height       Video height
@param maxbitrate   Maximum bitrate of video
@param avgbitrate   Average bitrate of video
 
@return		MP4_OK       Success
 			MP4_ERROR    Generic error
*/
extern IMPORT_C MP4Err MP4ComposeAddVideoDescription(MP4Handle handle, mp4_u32 timescale, mp4_u16 width, mp4_u16 height, mp4_u32 maxbitrate, mp4_u32 avgbitrate);

/**
This function is used to inform the MP4 library about the audio data. The
data given to the library is inserted into the MP4 file headers.
 
The parameter timescale indicates the timescale of the audio data
to be inserted. Timescale tells how many units of time pass in one second.
 
Note: timescale must be smaller than 65536.
 
Note: audioFramesPerSample and modeSet are needed only for AMR audio.
    
@param handle                MP4 library handle
@param timescale             Timescale of audio
@param audioFramesPerSample  AMR frames per sample
@param modeSet               AMR mode set
 
@return		MP4_OK     Success
 			MP4_ERROR  Generic error
*/
extern IMPORT_C MP4Err MP4ComposeAddAudioDescription(MP4Handle handle, mp4_u32 timescale, mp4_u8 audioFramesPerSample, mp4_u16 modeSet);

/**
This function writes one video frame to the MP4 composer library.
 
The frames must be inserted in causal order. Because the library doesn't
analyze the video bitstream, the frames can't be retrieved from the
resulting MP4 file correctly if they are not in proper order.
 
The data is available in the MP4 output file only after calling
MP4ComposeClose. MP4ComposeClose is called exactly once when all audio
and video data has been inserted into the library.
 
@param handle       MP4 library handle
@param buffer       Video frame data
@param framesize    Size of the video frame in bytes
@param duration     Duration of video frame (in timescale,
                 	see MP4ComposeAddVideoDescription)
@param keyframe     Flag to indicate whether this frame is a keyframe
 
@return		MP4_OK					Success
 			MP4_ERROR				Generic error
 			MP4_TIMESCALE_NOT_SET	Timescale has not been set
*/
extern IMPORT_C MP4Err MP4ComposeWriteVideoFrame(MP4Handle handle, mp4_u8 *buffer, mp4_u32 framesize, mp4_u32 duration, mp4_bool keyframe);

/**
This function writes audio frames to the MP4 composer library. The data is 
available in the MP4 output file only after calling
MP4ComposeClose. MP4ComposeClose is called exactly once when all audio
and video data has been insrted into the library.
 
For MPEG audio:
 
This function writes one MPEG audio frame to the MP4 composer library.
 
Note: numberofframes is meaningless with MPEG audio since only one frame
can be added with one call.
 
For other audio types:
 
This function writes a number of audio frames to the MP4 composer library. 
All audio frames inserted with one function call will be placed inside
one audio sample in the resulting file.
 
Note: Once this function is called with a certain numberofframes
parameter, it can't be changed. This is a limitation of the MP4 file
format. Only the last call can have a different number of frames if
their number is less than numberofframes.
 
@param handle          MP4 library handle
@param buffer          Audio data
@param bytestowrite    Number of bytes of audio data in buffer
@param numberofframes  Number of frames to write
@param duration        Duration of audio frames (in timescale,
	                   see MP4ComposeAddAudioDescription)
 
@return		MP4_OK             		Success
 			MP4_ERROR				Generic error
 			MP4_TIMESCALE_NOT_SET	Timescale has not been set
*/
extern IMPORT_C MP4Err MP4ComposeWriteAudioFrames(MP4Handle handle, mp4_u8 *buffer, mp4_u32 bytestowrite, mp4_u32 numberofframes, mp4_u32 duration);

/**
This function writes MPEG-4 or AVC video DecoderSpecificInfo data into the appropriate atom.

If the video type is MPEG-4 the DecoderSpecificInfo will be written into the esds atom. If the
video type is AVC (any profile) the DecoderSpecificInfo will be written into the avcC atom.

Note: This function needs to be called only when MPEG-4 or AVC video is used.
 
@param handle         MP4 library handle
@param info           Buffer containing DecoderSpecificInfo
@param infosize       Size of info in bytes
 
@return		MP4_OK		Success
 			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ComposeWriteVideoDecoderSpecificInfo(MP4Handle handle, mp4_u8 *info, mp4_u32 infosize);

/**
This function writes MPEG audio DecoderSpecificInfo data into esds atom.
 
Note: This function needs to be called only when MPEG audio is used.
 
@param handle         MP4 library handle
@param info           Buffer containing DecoderSpecificInfo
@param infosize       Size of info in bytes
 
@return		MP4_OK     Success
 			MP4_ERROR  Generic error
*/
extern IMPORT_C MP4Err MP4ComposeWriteAudioDecoderSpecificInfo(MP4Handle handle, mp4_u8 *info, mp4_u32 infosize);

/**
This function allows settings flags that modify the behaviour
of the library. Use binary OR to specify several flags.
 
The following flags can be used:
 
MP4_FLAG_NONE            No special setup.
 
MP4_FLAG_METADATALAST    Write metadata last in the file without tmpfile.
                         Default is to write the metadata in the
                         beginning of the file.
                          
MP4_FLAG_LONGCLIP        Collect metadata in files instead of collecting
                         it in memory if the amount is large. Only usable
                         in Symbian OS.
 
MP4_FLAG_GENERATE_3G2    Generate a 3G2 File instead of a 3GP File.
                         If not set, then 3GP file is generated.
                         Generation of 3G2 file may be automatically set 
                         during composition, based on the selection 
                         of 3GPP2 specific codecs.
 
MP4_FLAG_GENERATE_MP4    Generate a MP4 File instead of a 3GP File.
                         If not set, then 3GP file is generated.
                         Generation may be automatically set to 3GP
                         if codecs selected do not support MP4.
 
MP4_FLAG_LARGEFILEBUFFER Use larger output buffers in file writing.
                         Non MP4_ARCH_WIN32 compilations only.
 
Note: This function should be called after MP4ComposeOpen before
      any other MP4Compose functions.
 
Note: If MP4ComposeOpenFileHandle is used instead of MP4ComposeOpen the MP4_FLAG_METADATALAST must be defined.
 
@param handle         MP4 library handle
@param flags          Flags to enable
 
@return		MP4_OK              Success
 			MP4_ERROR  			Generic error
 			MP4_OUT_OF_MEMORY	Out of Memory
*/
extern IMPORT_C MP4Err MP4ComposeSetFlags(MP4Handle handle, mp4_u32 flags);

/**
This function initializes the MP4 parsing operation. The instance handle
of the library is set by the function.

The filename parameter is used to specify the type of the input. If the
parameter is non-NULL, it is assumed to be the filename of the input MP4
file. If the parameter is NULL, the library expects the input MP4 data to
be inserted by subsequent calls of MP4ParseWriteData.
 
@param handle     MP4 library handle
@param filename   Name of the input MP4 file or NULL if the data is coming from
                  stream
 
@return		MP4_OK             Success
			MP4_ERROR          Generic error
			MP4_OUT_OF_MEMORY  Out of memory
			MP4_FILE_ERROR     Error opening file
*/
extern IMPORT_C MP4Err MP4ParseOpen(MP4Handle *handle, MP4FileName filename);

/**
This function initializes the MP4 parsing operation. The instance handle
of the library is set by the function.
 
The RFile *inputfile parameter is pointer to valid file handle.
Library expects that file has been opened and will be closed outside of library.
 
@param handle     MP4 library handle
@param inputfile  File handle to be parsed.
           
@return     MP4_OK             Success
            MP4_ERROR          Generic error
            MP4_OUT_OF_MEMORY  Out of memory
            MP4_FILE_ERROR;    Error accessing file
*/
extern IMPORT_C MP4Err MP4ParseOpenFileHandle(MP4Handle *handle, RFile *inputfile );

/**
This function initializes the MP4 parsing operation. The instance handle
of the library is set by the function.
 
The RFile64 *inputfile parameter is pointer to valid file handle.
Library expects that file has been opened and will be closed outside of library.
 
@param handle     MP4 library handle
@param inputfile  File handle to be parsed.
           
@return		MP4_OK             Success
			MP4_ERROR          Generic error
			MP4_OUT_OF_MEMORY  Out of memory
			MP4_FILE_ERROR;    Error accessing file
*/
extern IMPORT_C MP4Err MP4ParseOpenFileHandle64(MP4Handle *handle, RFile64 *inputfile );

/**
This function initializes the MP4 parsing operation. The instance handle
of the library is set by the function.
 
The ContentAccess::CData *inputfile parameter is pointer to valid CAF object.
Library expects that file has been opened and will be closed outside of library.
 
@param handle     MP4 library handle
@param inputfile  CAF data source handle to be parsed.
           
@return		MP4_OK             Success
			MP4_ERROR          Generic error
			MP4_OUT_OF_MEMORY  Out of memory
			MP4_FILE_ERROR;    Error accessing file
*/
extern IMPORT_C MP4Err MP4ParseOpenCAF(MP4Handle *handle, ContentAccess::CData *inputfile );

/**
This function closes the MP4 parsing operation. It frees the resources
allocated by the library instance.

The library handle is not valid after this call.
 
@param handle     MP4 library handle
 
@return		MP4_OK		Success
			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ParseClose(MP4Handle handle);

/**
This function returns parameters describing the video stream. It can be
called after enough data has been inserted into the library so that MP4
headers containing the information can be read. If there is not enough
data, those functions return MP4_NOT_AVAILABLE and more data needs to be
inserted into the library by MP4ParseWriteData.
 
The framerate parameter refers to the frame rate of the original video
material.
 
The videotype parameter can be one of the following:
  MP4_TYPE_MPEG4_VIDEO
  MP4_TYPE_H263_PROFILE_0
  MP4_TYPE_H263_PROFILE_3
  MP4_TYPE_AVC_PROFILE_BASELINE 
  MP4_TYPE_AVC_PROFILE_MAIN
  MP4_TYPE_AVC_PROFILE_EXTENDED
  MP4_TYPE_AVC_PROFILE_HIGH
 
Note: For timescale parameter, NULL can be specified if the caller
      doesn't need the value.
 
@param handle       MP4 library handle
@param videolength  Duration of video in milliseconds
@param framerate    Frame rate of video (in Hz)
@param videotype    Type of video stream
@param videowidth   Width of video image measured in pixels
@param videoheight  Height of video image measured in pixels
@param timescale    Timescale of video track

@return		MP4_OK             		  Success
			MP4_ERROR                 Generic error
			MP4_NOT_AVAILABLE         MP4 library needs more data before the
    			                      requested information can be returned
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
			MP4_NO_VIDEO              Input doesn't contain video track
*/
extern IMPORT_C MP4Err MP4ParseRequestVideoDescription(MP4Handle handle, mp4_u32 *videolength, mp4_double *framerate, mp4_u32 *videotype, mp4_u32 *videowidth, mp4_u32 *videoheight, mp4_u32 *timescale);

/**
This function returns parameters describing the audio stream. It can be
called after enough data has been inserted into the library so that MP4
headers containing the information can be read.
 
Note: framespersample parameter is set only when AMR type audio is used.
 
Note: For timescale parameter, NULL can be specified if the caller
      doesn't need the value.
 
@param handle                    MP4 library handle
@param audiolength               Duration of audio in milliseconds
@param audiotype                 Type of audio
@param framespersample           Number of audio frames in each sample
@param timescale                 Timescale of audio track
@param averagebitrate            Average bitrate of audio
 
@return		MP4_OK            		  Success
			MP4_ERROR                 Generic error
			MP4_NOT_AVAILABLE         MP4 library needs more data before the
                        			  requested information can be returned
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
			MP4_NO_AUDIO              Input doesn't contain audio track
*/
extern IMPORT_C MP4Err MP4ParseRequestAudioDescription(MP4Handle handle, mp4_u32 *audiolength, mp4_u32 *audiotype, mp4_u8 *framespersample, mp4_u32 *timescale, mp4_u32 *averagebitrate);

/**
This function returns parameters describing the contents of the MP4
file. It can be called after enough data has been inserted into the
library so that MP4 headers containing the information can be read.
 
@param handle                 MP4 library handle
@param streamsize             Length of the stream in bytes
@param streamaveragebitrate   Average bitrate of the stream in bps

@return		MP4_OK					  Success
			MP4_ERROR                 Generic error
			MP4_NOT_AVAILABLE         MP4 library needs more data before the
    			                      requested information can be returned
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
*/
extern IMPORT_C MP4Err MP4ParseRequestStreamDescription(MP4Handle handle, mp4_u32 *streamsize, mp4_u32 *streamaveragebitrate);

/**
This function inserts MP4 data to the MP4 library.
 
It is necessary to call this function only if the input filename has not
been given with MP4ParseOpen. Several functions can return
MP4_NOT_AVAILABLE if the library doesn't have enough data to return the
information that the caller requests. In that case, more data needs to be
inserted to the library before calling those functions again.
 
This function makes a copy of the data inserted into the library so the
caller can use buffer for other purposes. If the function returns
MP4_OUT_OF_MEMORY, the buffer contents have not been copied into the
library and the caller needs to reduce the buffer size before calling
again.
 
If bytestowrite argument is zero, it indicates that there would be no more 
data to feed via this function. Such a function call MUST be done if a 
complete 3GP/MP4 file is to be written to the library's internal memory. The
buffer argument may be inputted as NULL in such a case.
 
@param handle         MP4 library handle
@param buffer         Data to be written
@param bytestowrite   Size of buffer in bytes
 
@return		MP4_OK             Success
			MP4_ERROR          Generic error
			MP4_OUT_OF_MEMORY  Library can't allocate enough memory for the data
*/
extern IMPORT_C MP4Err MP4ParseWriteData(MP4Handle handle, mp4_u8 *buffer, mp4_u32 bytestowrite);

/**
This function returns the number of bytes that the library instance has
in its allocated buffers.
 
The function is only valid when the filename is not provided when opening
the library. Zero is returned in bytes when in file mode.
 
@see MP4ParseWriteData.
 
@param handle   MP4 library handle
@param bytes    Number of allocated bytes in the library
 
@return		MP4_OK         Success
			MP4_ERROR      Generic error
			MP4_FILE_MODE  The library has been opened with the input in a file
*/
extern IMPORT_C MP4Err MP4ParseGetBufferedBytes(MP4Handle handle, mp4_u32 *bytes);

/**
This function returns the type of the next audio/video frame in the
stream. The type can be one of the following:
 
MP4_TYPE_MPEG4_VIDEO
MP4_TYPE_MPEG4_AUDIO
MP4_TYPE_H263_PROFILE_0
MP4_TYPE_H263_PROFILE_3
MP4_TYPE_AMR_NB
MP4_TYPE_AMR_WB
MP4_TYPE_AVC_PROFILE_BASELINE 
MP4_TYPE_AVC_PROFILE_MAIN
MP4_TYPE_AVC_PROFILE_EXTENDED
MP4_TYPE_AVC_PROFILE_HIGH
MP4_TYPE_QCELP_13K
 
The function is only valid when the input is streamable.
 
When reading audio/video from an MP4 container in streaming mode, the
samples must be read from the container in the same order as they were
inserted in it. This function determines the type of the next sample and
based on this information the user can call either MP4ParseReadVideoFrame
or MP4ParseReadAudioFrames to fetch more data.
 
@param handle   MP4 library handle
@param type     Type of the next frame

@return		MP4_OK             		  Success
			MP4_ERROR                 Generic error
			MP4_NOT_STREAMABLE        The input is not streamable
			MP4_NOT_AVAILABLE         MP4 library needs more data before the
			                          requested information can be returned
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
			MP4_NO_FRAME              Frame doesn't exist (previous frame was last)
*/
extern IMPORT_C MP4Err MP4ParseNextFrameType(MP4Handle handle, mp4_u32 *type);

/**
This function returns the size of the next frame of type type in bytes.
The frame type can be one of the following:
 
MP4_TYPE_MPEG4_VIDEO
MP4_TYPE_MPEG4_AUDIO
MP4_TYPE_H263_PROFILE_0
MP4_TYPE_H263_PROFILE_3
MP4_TYPE_AMR_NB
MP4_TYPE_AMR_WB
MP4_TYPE_AVC_PROFILE_BASELINE 
MP4_TYPE_AVC_PROFILE_MAIN
MP4_TYPE_AVC_PROFILE_EXTENDED
MP4_TYPE_AVC_PROFILE_HIGH
MP4_TYPE_QCELP_13K
 
@param handle     MP4 library handle
@param type       Type of the frame
@param framesize  Size of the requested frame in bytes
 
@return		MP4_OK		              Success
			MP4_ERROR                 Generic error
			MP4_NOT_AVAILABLE         MP4 library needs more data before the requested
           				              frames can be returned
			MP4_NO_REQUESTED_FRAME    Frame of requested type is not available
			MP4_INVALID_TYPE          Illegal type
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
 
*/
extern IMPORT_C MP4Err MP4ParseNextFrameSize(MP4Handle handle, mp4_u32 type, mp4_u32 *framesize);

/**
This function reads the next video frame from the MP4 file/stream and
returns it to the caller.
 
Note: Next frame depends on the position in the input MP4 file.
MP4ParseSeek can be used to change the current position in the MP4 file.
 
If the function returns MP4_NOT_AVAILABLE, the caller needs to call
MP4ParseWriteData to insert more data before calling again.
 
framesize is returned even if buffer was too small to contain the data.
 
Note: Specify NULL as timestamp2 if not interested in this parameter.
 
@param handle       MP4 library handle
@param buffer       Video frame is returned here
@param buffersize   Size of the user allocated buffer (in bytes)
@param framesize    Size of the frame that was returned (in bytes)
@param timestamp    Video frame presentation time in milliseconds from the
             		beginning of the video sequence
@param keyframe     Has a value of MP4TRUE if current frame is a keyframe
             		(intra) or MP4FALSE otherwise
@param timestamp2   Video frame presentation time in timescale from the
             		beginning of the video sequence
 
@return		MP4_OK             		  Success
			MP4_NOT_AVAILABLE         MP4 library needs more data before the requested
     			                      frame can be returned
			MP4_BUFFER_TOO_SMALL      Requested frame doesn't fit into the given buffer
			MP4_NO_FRAME              Frame doesn't exist (previous frame was last)
			MP4_ERROR                 Generic error
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
*/
extern IMPORT_C MP4Err MP4ParseReadVideoFrame(MP4Handle handle, mp4_u8 *buffer, mp4_u32 buffersize, mp4_u32 *framesize, mp4_u32 *timestamp, mp4_bool *keyframe, mp4_u32 *timestamp2);

/**
This function reads DecoderSpecificInfo data from MPEG-4 or AVC metadata and
returns it to the caller.

If the video type is MPEG-4 the DecoderSpecificInfo from the esds atom will be
returned. If the video type is AVC (any profile) the DecoderSpecificInfo from the
avcC atom will be returned.

decspecinfosize is set even if buffer is too small to contain the data.
The function can be called with buffersize 0 to query the size of
DecoderSpecificInfo.
 
Note: DecoderSpecificInfo is specific to MPEG-4 and AVC video. Therefore,
	  only MPEG-4 or AVC video streams contain this information.
 
@param handle           MP4 library handle
@param buffer           DecoderSpecificInfo is returned here
@param buffersize       Size of the user allocated buffer (in bytes)
@param decspecinfosize  Size of DecoderSpecificInfo is returned here
 
@return		MP4_OK             		  Success
			MP4_NOT_AVAILABLE         MP4 library needs more data before the requested
        			                  frames can be returned
			MP4_BUFFER_TOO_SMALL      Requested data doesn't fit into the given buffer
			MP4_ERROR                 Generic error
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
*/
extern IMPORT_C MP4Err MP4ParseReadVideoDecoderSpecificInfo(MP4Handle handle, mp4_u8 *buffer, mp4_u32 buffersize, mp4_u32 *decspecinfosize);

/**
This function reads the audio frames that are stored in the current
audio sample from the MP4 file/stream and returns them to the caller.
 
Note: Next frame depends on the position in the input MP4 file.
      MP4ParseSeek can be used to change the current position in the
      MP4 file.
 
If the function returns MP4_NOT_AVAILABLE, the caller needs to call
MP4ParseWriteData to insert more data before calling again.
 
audiosize is returned even if buffer was too small to contain the data.
 
Note: returnedframes may differ from the correct value when accessing
      the last audio sample.
 
Note: Specify NULL as timestamp2 if not interested in this parameter.
 
@param handle           MP4 library handle
@param buffer           Audio frames are returned here
@param buffersize       Size of the user allocated buffer (in bytes)
@param audiosize        Size of the returned audio frames (in bytes)
@param timestamp        Audio frame presentation time in milliseconds from the
 		                beginning of the audio sequence
@param returnedframes   Number of frames returned, or 0 if not known
@param timestamp2       Audio frame presentation time in timescale from the
 		                beginning of the audio sequence
 
@return		MP4_OK					  Success
			MP4_NOT_AVAILABLE         MP4 library needs more data before the requested
           				              frames can be returned
			MP4_BUFFER_TOO_SMALL      Requested frames don't fit into the given buffer
			MP4_ERROR                 Generic error
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
			MP4_NO_FRAME              No more frames available
*/
extern IMPORT_C MP4Err MP4ParseReadAudioFrames(MP4Handle handle, mp4_u8 *buffer, mp4_u32 buffersize, mp4_u32 *audiosize, mp4_u32 *timestamp, mp4_u32 *returnedframes, mp4_u32 *timestamp2);

/**
This function reads DecoderSpecificInfo data from MP4 metadata and
returns it to the caller.
 
decspecinfosize is set even if buffer is too small to contain the data.
The function can be called with buffersize 0 to query the size of
DecoderSpecificInfo.
 
Note: This function can be used for both MPEG-4 audio and AMR.
 
@param handle           MP4 library handle
@param buffer           DecoderSpecificInfo is returned here
@param buffersize       Size of the user allocated buffer (in bytes)
@param decspecinfosize  Size of DecoderSpecificInfo is returned here
 
@return		MP4_OK             Success
			MP4_NOT_AVAILABLE  MP4 library needs more data before the requested
                          		frames can be returned
			MP4_BUFFER_TOO_SMALL  Requested data doesn't fit into the given buffer
			MP4_ERROR          Generic error
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
*/
extern IMPORT_C MP4Err MP4ParseReadAudioDecoderSpecificInfo(MP4Handle handle, mp4_u8 *buffer, mp4_u32 buffersize, mp4_u32 *decspecinfosize);

/**
Return the timestamp of the next video frame. This only works if the
input MP4 data is in a file.
 
The function can be used to find out which frames have been coded to
optimize the input frame selection if video frame rate needs to be
modified.
 
When this function call returns MP4_END_OF_VIDEO, there are no more video
frames left in the MP4 file and the timestamp returned with the previous
call was the timestamp of the last video frame.
 
Note: User must call MP4ParseSeek() to seek to the correct position
      after calls of this function. Otherwise the current position is
      undetermined.
 
Note: Specify NULL as timestamp2 if this value is not needed.
 
@param handle      MP4 library handle
@param timestamp   Timestamp in milliseconds is returned here
@param timestamp2  Timestamp in timescale is returned here
 
@return		MP4_OK 		              Success
			MP4_END_OF_VIDEO          No more video frames left
			MP4_ERROR                 Generic error
			MP4_NOT_AVAILABLE         MP4 library needs more data before the
          				                requested information can be returned
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
*/
extern IMPORT_C MP4Err MP4ParseGetNextVideoTimestamp(MP4Handle handle, mp4_u32 *timestamp, mp4_u32 *timestamp2);

/**
This function determines whether the input MP4 stream is streamable,
i.e. the media data is arranged in such a manner that playback can be
started without downloading the entire stream.
 
@param handle   MP4 library handle
 
@return		MP4_OK                    The file is streamable
			MP4_NOT_STREAMABLE        The file is not streamable
			MP4_NOT_AVAILABLE         MP4 library needs more data before the requested
        			                  information can be returned
			MP4_ERROR                 Generic error
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
*/
extern IMPORT_C MP4Err MP4ParseIsStreamable(MP4Handle handle);

/**
This function seeks to the position specified by position parameter in
the input MP4 file. The function can be called only if the input MP4 data
is in a file and the file name has been specified with MP4ParseOpen
function call.
 
The position is considered to be from the start of the presentation time
line in the MP4 file. Thus audio and video positions can't be given
separately.
 
The function will set current audio and video positions in the following
manner:
 
If there is only audio in the file, the current position will be set to
the audio frame at or just before the given position.
 
If there is only video in the file and keyframe is MP4FALSE, the current
position will be set to the video frame at or just before the given
position. If keyframe is set to MP4TRUE, the current position will be set
to the first keyframe at or before current position.
 
If there are both audio and video in the file, video is first positioned
as explained above and then audio is seeked to the closest position in
relation to video.
 
@param handle          MP4 library handle
@param position        Position to seek to in milliseconds in the MP4
                	   presentation time line
@param audioPosition   Position of audio after seek (in milliseconds)
@param videoPosition   Position of video after seek (in milliseconds)
@param keyframe        If set to MP4TRUE, the 1st video keyframe before given
                	   point will be seeked to. If set to MP4FALSE, the first
                	   video frame before given point will be seeked to.
 
@return		MP4_OK         Success
			MP4_ERROR      Generic error
			MP4_CANT_SEEK  Can't seek to requested position
 
*/
extern IMPORT_C MP4Err MP4ParseSeek(MP4Handle handle, mp4_u32 position, mp4_u32 *audioPosition, mp4_u32 *videoPosition, mp4_bool keyframe);

/**
This function determines whether the next frame of type type is
available.
 
The frame type can be one of the following:
 
MP4_TYPE_MPEG4_VIDEO
MP4_TYPE_MPEG4_AUDIO
MP4_TYPE_H263_PROFILE_0
MP4_TYPE_H263_PROFILE_3
MP4_TYPE_AMR_NB
MP4_TYPE_AMR_WB
MP4_TYPE_AVC_PROFILE_BASELINE 
MP4_TYPE_AVC_PROFILE_MAIN
MP4_TYPE_AVC_PROFILE_EXTENDED
MP4_TYPE_AVC_PROFILE_HIGH
MP4_TYPE_QCELP_13K
 
@param handle     MP4 library handle
@param type       Type of the frame
 
@return		MP4_OK                    Frame is available
			MP4_ERROR                 Generic error
			MP4_NOT_AVAILABLE         MP4 library needs more data before the requested
    			                      frames can be returned
			MP4_NO_REQUESTED_FRAME    Frame of requested type is not available
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
*/
extern IMPORT_C MP4Err MP4ParseIsFrameAvailable(MP4Handle handle, mp4_u32 type);

/**
Return the largest position in a file that has been accessed or the latest accessed 
absolute byte location of the stream.
 
Note: The function returns a valid position after one audio or video
      sample has been read.
 
@param handle     MP4 library handle
@param position   Result is returned here
 
@return		MP4_OK		Success 
*/
extern IMPORT_C MP4Err MP4ParseGetLastPosition(MP4Handle handle, mp4_u32 *position);

/**
Return the largest position in a file that has been accessed or the latest accessed 
absolute byte location of the stream.
 
Note: The function returns a valid position after one audio or video
      sample has been read.
 
@param handle     MP4 library handle
@param position   Result is returned here
 
@return		MP4_OK		Success 
*/
extern IMPORT_C MP4Err MP4ParseGetLastPosition64(MP4Handle handle, mp4_u64 *position);

/**
Return the number of video frames.
 
@param handle			MP4 library handle
@param numberOfFrames   Result is returned here
 
@return		MP4_OK		Success
			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ParseGetNumberOfVideoFrames(MP4Handle handle, mp4_u32 *numberOfFrames);

/**
Return video frame size.
 
@param handle           MP4 library handle
@param index            Index of video frame
@param frameSize        Result is returned here
 
@return		MP4_OK		Success
			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ParseGetVideoFrameSize(MP4Handle handle, mp4_u32 index, mp4_u32 *frameSize);

/**
Return video frame start time.
 
@param handle           MP4 library handle
@param index            Index of video frame
@param timestamp        Result is returned here
@param timestampms      Result in milliseconds
 
@return		MP4_OK		Success
			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ParseGetVideoFrameStartTime(MP4Handle handle, mp4_u32 index, mp4_u32 *timestamp, mp4_u32 *timestampms);

/**
Return video frame type.
 
@param handle           MP4 library handle
@param index            Index of video frame
@param frametype        Result is returned here
 
@return		MP4_OK 		Success
			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ParseGetVideoFrameType(MP4Handle handle, mp4_u32 index, mp4_bool *frametype);

/**
Return the video sample entry index of the next video frame to be read.
 
The smallest index value is 1.
 
@param handle     MP4 library handle
@param videosampleentryindex   Visual SampleEntry index of the next video frame to be read.
 
@return		MP4_OK		Success
			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ParseGetVideoSampleEntryIndex(MP4Handle handle, mp4_u32 *videosampleentryindex);

/**
Return the audio sample entry index of the next audio frame to be read.
 
The smallest index value is 1.
 
@param handle     MP4 library handle
@param audiosampleentryindex   Visual SampleEntry index of the next audio frame to be read.
 
@return		MP4_OK		Success
			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ParseGetAudioSampleEntryIndex(MP4Handle handle, mp4_u32 *audiosampleentryindex);

/**
 This function sets the storage mode of QCELP 13K
 qcelpStorageMode = 0  ...QCELP will be registered using QCELPSampleEntry Box (Default)
 qcelpStorageMode = 1  ...QCELP will be registered using MP4AudioDescription Box 
 
 Note: This function needs to be called only when QCLEP 13K is used and when it is going to 
       be stored registered using the MP4AudioDescription Box. 
 
 @param handle         	   MP4 library handle
 @param qcelpStorageMode   Decoder configuration registration mode of QCELP 13K Codec
 
 @return	MP4_OK		Success
 			MP4_ERROR	Generic error
*/
extern IMPORT_C MP4Err MP4ComposeSetQCELPStorageMode(MP4Handle handle, mp4_u8 qcelpStorageMode);

/**
Return the decoder configuration registration mode of QCELP 13K Codec.
 
 qcelpStorageMode = 0        QCELP is registered using QCELPSampleEntry Box (Default)
 qcelpStorageMode = 1        QCELP is registered using MP4AudioDescription Box
 
@param handle     MP4 library handle
@param qcelpStorageMode   Decoder configuration registration mode of QCELP 13K Codec
 
@return		MP4_OK		Success
			MP4_ERROR  	Generic error 
*/
extern IMPORT_C MP4Err MP4ParseGetQCELPStorageMode(MP4Handle handle, mp4_u8 *qcelpStorageMode);

/**
This function gets the frame infoparameters from star index to number of elements in array from that index,
properties obtained are startTime, frameType and frameSize.  
 
Important Note:--- The number of elements allocated in the array should be equal to the sizeofarray
 
 
@param apihandle     MP4 library handle
@param startindex    Index from where to start getting information for the array
@param sizeofarray   number of elements that exist in the allocated frame array
@param aFrameInfoArray Array of Structures of frame parameters 
 

@return		MP4_OK             Success
			MP4_ERROR          Generic error
			MP4_NO_OUTPUT_BUFFER Output buffer pointer uninitialized
*/
extern IMPORT_C MP4Err MP4GetVideoFrameProperties(MP4Handle apihandle,mp4_u32 startindex,mp4_u32 sizeofarray,TFrameInfoParameters* aFrameInfoArray);

/**
 This function sets properties of composed video clip.
 	 
 Setup details in TVideoClipProperties struct:
 
       mp4_u32 iH263Level        level of H.263
 
 @param apihandle              MP4 library handle
 @param aVideoClipProperties   Setup info struct.
 
 @return	MP4_OK             Success
 			MP4_ERROR          Generic error
*/
extern IMPORT_C MP4Err MP4ComposeSetVideoClipProperties(MP4Handle apihandle, const TVideoClipProperties& aVideoClipProperties);

/**
This function gets properties of parsed video clip.

Returned details in TVideoClipProperties struct:
 
    mp4_u32 iH263Level        level of H.263
 
@param apihandle     		MP4 library handle
@param aVideoClipProperties Filled info struct.
 
@return		MP4_OK             Success
			MP4_ERROR          Generic error
*/
extern IMPORT_C MP4Err MP4ParseGetVideoClipProperties(MP4Handle apihandle, TVideoClipProperties& aVideoClipProperties);

/**
Retrieve an atom of given type from user data atom (UDTA) to the given buffer.
 
Note: on return parameters contain following information:
	udtaLocation   Hash of all locations in file that contain UDTA atom.
                   MP4_UDTA_NONE | MP4_UDTA_MOOV | MP4_UDTA_VIDEOTRAK | MP4_UDTA_AUDIOTRAK
	buffer         Buffer containing asked atom.
	bufferSize     Size of atom written in buffer. If buffer is too small for atom variable contains needed size.
	atomIndex      Count of same subatom type in this UDTA. Starts from 0 (zero) index.

@param handle         MP4 library handle
@param udtaLocation   Location of UDTA atom inside file.
@param udtaAtomType   Type of atom to be read from UDTA.
   		                Hex value of 4 chars representing atom type defined in standard.
       		            For example: 0x7469746c //'titl' - title for the media atom
                                0x61757468 //'auth' - author of the media atom
                                0x676e7265 //'gnre' - genre (category and style) of the media atom  
@param buffer         Buffer to write atom to.
@param bufferSize     Size of given buffer.
@param atomIndex      Index of atom if UDTA contains multiple subatoms of same asked udtaAtomType.
 
@return		MP4_OK             			   Success
			MP4_ERROR                      Generic error 
			MP4_OUTPUT_BUFFER_TOO_SMALL    Buffer to write atom to is too small, required size in buffersize argument.
			MP4_INVALID_TYPE               Asked udtaLocation is invalid.
			MP4_NOT_AVAILABLE              UDTA or wanted subatom is not available in asked location.
			MP4_CANT_SEEK                  Can't seek to UDTA atom location.
*/
extern IMPORT_C MP4Err MP4ParseGetUserDataAtom(MP4Handle handle, 
                                                            mp4_u8& udtaLocation,
                                                            mp4_u32 udtaAtomType,
                                                            mp4_u8* buffer,
                                                            mp4_u32& bufferSize,
                                                            mp4_u32& atomIndex );

/**
Writes a buffer containing whole atom to inside of user data atom (UDTA) defined in udtaLocation.
 
@param apihandle      MP4 library handle
@param udtaLocation   Location of UDTA atom inside file.
                       MP4_UDTA_MOOV | MP4_UDTA_VIDEOTRAK | MP4_UDTA_AUDIOTRAK 
@param buffer         Buffer to write atom from.
@param bufferSize     Size of buffer.
 
@return		MP4_OK             			   Success
 			MP4_ERROR                      Generic error 
 			MP4_OUT_OF_MEMORY              Out of memory
 			MP4_INVALID_TYPE               Asked udtaLocation is invalid.
*/
extern IMPORT_C MP4Err MP4ComposeSetUserDataAtom(MP4Handle apihandle, 
                                                            mp4_u8& udtaLocation,
                                                            mp4_u8* buffer,
                                                            mp4_u32& bufferSize );     

/**
Sets internal buffer sizes to 3GPMP4Lib. Usage optional.
Must be used after MP4ComposeSetFlags when composing.
 
Defaults for composing (with MP4_FLAG_LARGEFILEBUFFER): 
 
	mediaWriteBufferSize:       2048, (2*65536)
	writeBufferMaxCount:        15
 
Default for parsing:
	readBufferSize:             8k             
                 
@param apihandle			MP4 library handle
@param mediaWriteBufferSize	Size of media data file output buffers (bytes).
 							0 to use default internal value.
@param writeBufferMaxCount	Max amount of buffers (both media and meta) before file output changes
							to synchronous (by default file writing is asynchronous operation).
	                        0 to use default internal value.
	                        Min value 6.
@param readBufferSize		Size of file read buffer. 0 to use default internal value.
 
@return		MP4_OK             Success
			MP4_ERROR          Generic error
 			MP4_OUT_OF_MEMORY  Out of Memory error 
*/
extern IMPORT_C MP4Err MP4SetCustomFileBufferSizes( MP4Handle apihandle, 
                                                               mp4_u32 mediaWriteBufferSize,
                                                               mp4_u32 writeBufferMaxCount,
                                                               mp4_u32 readBufferSize );

/**
This function gets the next frame's dependency information from SDTP box.
 
Return values of each parameter can be the following:
  aDependsOn:
    0: the dependency of this sample is unknown;
    1: this sample does depend on others (not an I picture);
    2: this sample does not depend on others (I picture);
 
  aIsDependedOn:
    0: the dependency of other samples on this sample is unknown;
    1: other samples depend on this one (not disposable);
    2: no other sample depends on this one (disposable);
 
  aHasRedundancy:
    0: it is unknown whether there is redundant coding in this sample;
    1: there is redundant coding in this sample;
    2: there is no redundant coding in this sample;
 
@param apihandle       MP4 library handle
@param aDependsOn      Defines whether current frame is I-frame (2), isn't I-frame (1) or if it only
 		               can be found out by using MP4ParseReadVideoFrame()-method (0).
@param aIsDependentOn  Defined the dependency of other frames on this one
@param aHasRedundancy  Defined the existence of redundant coding in this frame

@return		MP4_OK             Success
			MP4_ERROR          Generic error
			MP4_NOT_AVAILABLE  Metadata not available
			MP4_INVALID_INPUT_STREAM Error in reading metadata
			MP4_NO_OUTPUT_BUFFER Output buffer uninitialized	
*/
extern IMPORT_C MP4Err MP4ParseNextVideoFrameDependencies(MP4Handle apihandle, mp4_u8* aDependsOn, mp4_u8* aIsDependentOn, mp4_u8* aHasRedundancy);

/**
This function sets the next frame's dependency information to SDTP box.
 
NOTE: This function should be called before MP4ComposeWriteVideoFrame for each frame. Otherwise
      default values are inserted for the missing frames (0 = unknown).
 
Possible values for each parameter are the following:
    aDependsOn:
       0: the dependency of this sample is unknown;
       1: this sample does depend on others (not an I picture);
       2: this sample does not depend on others (I picture);
       3: reserved, DO NOT USE

    aIsDependedOn:
       0: the dependency of other samples on this sample is unknown;
       1: other samples depend on this one (not disposable);
       2: no other sample depends on this one (disposable);
       3: reserved, DO NOT USE

    aHasRedundancy:
       0: it is unknown whether there is redundant coding in this sample;
       1: there is redundant coding in this sample;
       2: there is no redundant coding in this sample;
       3: reserved, DO NOT USE

@param apihandle		MP4 library handle
@param aDependsOn		Defines whether current frame is I-frame (2), isn't I-frame (1) or if it only
						can be found out by using MP4ParseReadVideoFrame()-method (0).
@param aIsDependentOn	Defines the dependency of other frames on this one
@param aHasRedundancy	Defines the existence of redundant coding in this frame
 
@return		MP4_OK             Success
			MP4_ERROR          Generic error
*/
extern IMPORT_C MP4Err MP4ComposeWriteNextVideoFrameDependencies(MP4Handle apihandle, mp4_u8 aDependsOn, mp4_u8 aIsDependentOn, mp4_u8 aHasRedundancy);

/**
This function reads the audio frames asynchronously that are stored in the current
audio sample from the MP4 file and returns them to the caller.
 
Note: Only one async parse audio or video frame(s) operation can be ongoing at any given time.
 
Note: Observer notification only comes if called function returns MP4_OK.
If given buffer is too small MP4_BUFFER_TOO_SMALL is returned and buffersize contains needed
size for audio frame.
 
When feeding in parsed clip as memory buffers if the function returns MP4_NOT_AVAILABLE, 
the client needs to call MP4ParseWriteData to insert more data before calling this method again.
 
@param handle       MP4 library handle
@param aObserver	Observer designed to receive notification of Asynchronous event completion
@param buffer       Buffer that audio frames are written to.
@param buffersize   Size of the user allocated buffer (in bytes). In MP4_BUFFER_TOO_SMALL error case contains
	 				needed size for memory buffer.
 
@return		MP4_OK            		  Success
			MP4_NOT_AVAILABLE         MP4 library needs more data before the requested
									  frames can be returned
			MP4_BUFFER_TOO_SMALL      Requested frames don't fit into the given buffer
			MP4_ERROR                 Generic error
			MP4_FILE_ERROR			  Async operation is not for buffer mode
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
			MP4_NO_FRAME              No more frames available
			MP4_OUT_OF_MEMORY		  Out of Memory.
*/ 
extern IMPORT_C MP4Err MP4ParseReadAudioFramesAsync(MP4Handle handle, M3GPMP4LibAsyncObserver* aObserver, mp4_u8 *buffer, mp4_u32* buffersize );

/**
This function reads the next video frame from the MP4 file and returns it to the caller.
 
Note: Only one async parse audio or video frame(s) operation can be ongoing at any given time.
 
Note: Observer notification only comes if called function returns MP4_OK.
If given buffer is too small MP4_BUFFER_TOO_SMALL is returned and buffersize contains needed
size for video frame.
 
When feeding in parsed clip as memory buffers if the function returns MP4_NOT_AVAILABLE, 
the client needs to call MP4ParseWriteData to insert more data before calling this method again.
 
@param handle       MP4 library handle
@param aObserver	Observer designed to receive notification of Asynchronous event completion
@param buffer       Video frame is returned here
@param buffersize   Size of the user allocated buffer (in bytes). In MP4_BUFFER_TOO_SMALL error case contains
					needed size for memory buffer.
 
@return		MP4_OK             		  Success
			MP4_NOT_AVAILABLE         MP4 library needs more data before the requested
          				              frame can be returned
			MP4_BUFFER_TOO_SMALL      Requested frame doesn't fit into the given buffer
			MP4_NO_FRAME              Frame doesn't exist (previous frame was last)
			MP4_ERROR                 Generic error
			MP4_OUT_OF_MEMORY		  Out of Memory
			MP4_INVALID_INPUT_STREAM  MP4 stream is invalid
			MP4_FILE_ERROR			  Async operation is not for buffer mode			
*/
extern IMPORT_C MP4Err MP4ParseReadVideoFrameAsync(MP4Handle handle, M3GPMP4LibAsyncObserver* aObserver, mp4_u8* buffer, mp4_u32* buffersize );

/**
This function sets async temporary file deleting observer. Meta data temporary file removing can be done 
on background to improve video recording stopping.

@param handle       MP4 library handle
@param aObserver    Temorary file deleting observer
 
@return		MP4_OK       Success
			MP4_ERROR    Generic error
*/

extern IMPORT_C MP4Err MP4ComposeSetTempFileRemoverObserver(MP4Handle *handle, M3GPMP4LibAsyncTempFileRemoverObserver *aObserver);
  
/**
This function cancels the outstanding asynchronous read audio/video frame request. 

No callback function will be called.

Note:  As only one asynchronous parse audio or video frame(s) operation can be ongoing at any given time, 
this function can be used to cancel audio or video read request. If this is called without any outstanding 
request, nothing happens.

@param handle       MP4 library handle

@see MP4ParseReadVideoFrameAsync 
@see MP4ParseReadAudioFramesAsync
*/
extern IMPORT_C void MP4CancelReadFrame(MP4Handle handle);

/**
Fetches ID32 data location.
 *
 * Parameters:
 *
@param handle         MP4 library handle
@param location       Returns ID32 data location

@return	MP4_OK                         Success
		MP4_ERROR                      Generic error 
		MP4_INVALID_INPUT_STREAM       MP4 stream is invalid
		MP4_NOT_AVAILABLE              Metadata, META atom or ID32 atom is not available
 */
extern IMPORT_C MP4Err MP4ParseGetID32Location(MP4Handle apihandle, mp4_u32& location);

#ifdef __cplusplus
}
#endif


#endif
// End of File
