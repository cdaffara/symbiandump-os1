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

#include <3gplibrary/mp4config.h>
#include <3gplibrary/mp4lib.h>
#include "mp4atom.h"
#include "mp4memwrap.h"
#include "mp4file.h"
#include "mp4buffer.h"
#include "mp4endian.h"
#include "mp4utils.h"

using namespace ContentAccess;

extern EXPORT_C MP4Err MP4ParseOpen(MP4Handle *apihandle,
                                                 MP4FileName filename)
{
  MP4HandleImp *handle = (MP4HandleStruct **)apihandle;
  *handle = (MP4HandleImp)mp4malloc(sizeof(MP4HandleStruct));
  if (*handle == NULL)
    return MP4_OUT_OF_MEMORY;

  (*handle)->file32Duplicate = NULL;
  (*handle)->FileHandleFromOutside = EFalse;
  (*handle)->sourceType = MP4_SOURCE_RFILE;

  if (filename)
    if (initFileRead(filename, *handle) == -1)
    {
      closeFile(*handle);
      mp4free(*handle);
      *handle = NULL;
      return MP4_FILE_ERROR;
    }

  (*handle)->mem = listCreate();
  if ((*handle)->mem == NULL)
  {
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

  if (((*handle)->buf = (mp4_u8 *)mp4malloc(TMPBUFSIZE)) == NULL)
  {
    listDestroyList((*handle)->mem);
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

  if (((*handle)->diskReadBuf = (mp4_u8 *)mp4malloc(READBUFSIZE)) == NULL)
  {
    mp4free((*handle)->buf);
    listDestroyList((*handle)->mem);
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

  // register for stblib use 
  if (openStdlib() !=  MP4_OK)
	  {
	  mp4free((*handle)->diskReadBuf);	  
	  mp4free((*handle)->buf);
	  listDestroyList((*handle)->mem);
	  closeFile(*handle);
	  mp4free(*handle);
	  *handle = NULL;
	  
	  return MP4_ERROR;
	  }
  
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseOpenFileHandle(MP4Handle *apihandle,
                                                 RFile *inputfile)
{
  MP4Err err;
  RFile64 *f64 = new RFile64;
  if (f64 == NULL)
  {
    return MP4_OUT_OF_MEMORY;
  }
  if  (f64->Duplicate(*inputfile) != KErrNone)
  {
    delete f64;
    return MP4_ERROR;
  }
  err = MP4ParseOpenFileHandle64(apihandle, f64);
  if (err == MP4_OK)
  {    
    MP4HandleImp* handle = (MP4HandleStruct **)apihandle;
    (*handle)->file32Duplicate = (void*)(f64);
  }
  return err;
}

extern EXPORT_C MP4Err MP4ParseOpenFileHandle64(MP4Handle *apihandle,
                                                 RFile64 *inputfile)
{
  MP4HandleImp *handle = (MP4HandleStruct **)apihandle;
  *handle = (MP4HandleImp)mp4malloc(sizeof(MP4HandleStruct));
  if (*handle == NULL)
    return MP4_OUT_OF_MEMORY;

  (*handle)->file32Duplicate = NULL;
  (*handle)->rfile = (void *)inputfile;
  (*handle)->file = (*handle)->rfile;
  (*handle)->FileHandleFromOutside = ETrue;
  (*handle)->sourceType = MP4_SOURCE_RFILE;
  if (inputfile == NULL)
      {
      closeFile(*handle);
      mp4free(*handle);
      *handle = NULL;
      return MP4_FILE_ERROR;
      }

  (*handle)->mem = listCreate();
  if ((*handle)->mem == NULL)
  {
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

  if (((*handle)->buf = (mp4_u8 *)mp4malloc(TMPBUFSIZE)) == NULL)
  {
    listDestroyList((*handle)->mem);
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

  if (((*handle)->diskReadBuf = (mp4_u8 *)mp4malloc(READBUFSIZE)) == NULL)
  {
    mp4free((*handle)->buf);
    listDestroyList((*handle)->mem);
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

  	// register for stblib use 
  	if (openStdlib() !=  MP4_OK)
  		{
  		// clean up
		mp4free((*handle)->diskReadBuf);
		mp4free((*handle)->buf);
		listDestroyList((*handle)->mem);
		closeFile(*handle);
		mp4free(*handle);
		*handle = NULL;
  
		return MP4_ERROR;
  		} 
  
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseOpenCAF(MP4Handle *apihandle,
                                                    ContentAccess::CData *inputfile)
{
  MP4HandleImp *handle = (MP4HandleStruct **)apihandle;
  *handle = (MP4HandleImp)mp4malloc(sizeof(MP4HandleStruct));
  if (*handle == NULL)
    return MP4_OUT_OF_MEMORY;

  (*handle)->file32Duplicate = NULL;
  (*handle)->cfile = inputfile;
  (*handle)->file = (*handle)->cfile;
  (*handle)->FileHandleFromOutside = ETrue;
  (*handle)->sourceType = MP4_SOURCE_CAF;
  
  if (inputfile == NULL)
      {
      closeFile(*handle);
      mp4free(*handle);
      *handle = NULL;
      return MP4_FILE_ERROR;
      }

  (*handle)->mem = listCreate();
  if ((*handle)->mem == NULL)
  {
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

  if (((*handle)->buf = (mp4_u8 *)mp4malloc(TMPBUFSIZE)) == NULL)
  {
    listDestroyList((*handle)->mem);
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

  if (((*handle)->diskReadBuf = (mp4_u8 *)mp4malloc(READBUFSIZE)) == NULL)
  {
    mp4free((*handle)->buf);
    listDestroyList((*handle)->mem);
    closeFile(*handle);
    mp4free(*handle);
    *handle = NULL;
    return MP4_OUT_OF_MEMORY;
  }

	// register for stblib use 
	if (openStdlib() !=  MP4_OK)
		{
		// clean up
		mp4free((*handle)->diskReadBuf);
		mp4free((*handle)->buf);
		listDestroyList((*handle)->mem);
		closeFile(*handle);
		mp4free(*handle);
		*handle = NULL;

		return MP4_ERROR;
		} 
  
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseClose(MP4Handle apihandle)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (!handle)
    return MP4_ERROR;

  if (handle->file)
    closeFile(handle);

  closeStdlib(); /* Free memory allocated by stdlib wrapper functions */

  listDestroyList(handle->mem);

  if (handle->diskReadBuf)
    mp4free(handle->diskReadBuf);

  if (handle->buf)
    mp4free(handle->buf);
  
  if (freeMOOV(handle->moov) < 0)
   	return MP4_ERROR;

  if (freeFTYP(handle->ftyp) < 0)
   	return MP4_ERROR;

  if (handle->file32Duplicate)
      ((RFile64*)handle->file32Duplicate)->Close();
  
  if (handle)
    mp4free(handle);

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseRequestVideoDescription(MP4Handle apihandle,
                                                                    mp4_u32 *videolength,
                                                                    mp4_double *framerate,
                                                                    mp4_u32 *videotype,
                                                                    mp4_u32 *videowidth,
                                                                    mp4_u32 *videoheight,
                                                                    mp4_u32 *timescale)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  if (!handle)
	  {
	  return MP4_ERROR;
	  }

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:
      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }
      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      
      
    case -1:
    default:
      return MP4_ERROR;
    }
  }

  if (!handle->moov)
    return MP4_ERROR;
  if (!handle->moov->trakVideo)
    return MP4_NO_VIDEO;
  
  mp4_i32 videoError = 0;
  videoError = determineVideoType(handle, videotype);
  if ( videoError == -2 )
  {
    return MP4_NO_VIDEO;
  }
  else if ( videoError < 0 )
  {
  	return MP4_ERROR;
  }

  if (determineVideoLength(handle, videolength) < 0)
    return MP4_ERROR;

  if (determineFrameRate(handle, framerate) < 0)
    return MP4_ERROR;

  if (determineVideoResolution(handle, videowidth, videoheight) < 0)
    return MP4_ERROR;

  if (determineVideoTimeScale(handle, timescale) < 0)
    return MP4_ERROR;


  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseRequestAudioDescription(MP4Handle apihandle,
                                                                    mp4_u32 *audiolength,
                                                                    mp4_u32 *audiotype,
                                                                    mp4_u8 *framespersample,
                                                                    mp4_u32 *timescale,
                                                                    mp4_u32 *averagebitrate)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  if (!handle)
	  {
	  return MP4_ERROR;
	  }

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:
      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }

      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      

    case -1:
    default:
      return MP4_ERROR;
    }
  }

  if (!handle->moov)
    return MP4_ERROR;
  if (!handle->moov->trakAudio)
    return MP4_NO_AUDIO;

  if (determineAudioLength(handle, audiolength) < 0)
    return MP4_ERROR;
  
  mp4_i32 audioError = 0;
  audioError = determineAudioType(handle, audiotype);
  if ( audioError == -2 )
  {
    return MP4_NO_AUDIO;
  }
  else if ( audioError < 0 )
  {
  	return MP4_ERROR;
  }

  if (determineAudioFramesPerSample(handle, framespersample) < 0)
    return MP4_ERROR;

  if (determineAudioTimeScale(handle, timescale) < 0)
    return MP4_ERROR;

  if (averagebitrate != NULL)
    if (determineAudioAverageBitRate(handle, averagebitrate) < 0)
      return MP4_ERROR;


  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseRequestStreamDescription(MP4Handle apihandle,
                                                                     mp4_u32 *streamsize,
                                                                     mp4_u32 *streamaveragebitrate)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:

      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }
      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      
      
    case -1:
    default:
      return MP4_ERROR;
    }
  }

  if (determineStreamSize(handle, streamsize) < 0)
    return MP4_ERROR;

  if (determineStreamAverageBitRate(handle, streamaveragebitrate, *streamsize) < 0)
    return MP4_ERROR;


  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseWriteData(MP4Handle apihandle,
                                                      mp4_u8 *buffer,
                                                      mp4_u32 bytestowrite)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (handle->file)
    return MP4_ERROR;

  if ((buffer == NULL) && (bytestowrite != 0))
      return MP4_ERROR;

  if (handle->LastWriteDataCalled == MP4TRUE)
      return MP4_ERROR;  // WriteData() call has been made before with bytestowrite=0, can not call again.

  if (bytestowrite != 0)
  {
      if (addData(handle, buffer, bytestowrite) == -1)
        return MP4_OUT_OF_MEMORY;
  }
  else  
      handle->LastWriteDataCalled = MP4TRUE;


  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseGetBufferedBytes(MP4Handle apihandle,
                                                             mp4_u32 *bytes)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  *bytes = getBufferedBytes(handle);

  if (handle->file)
    return MP4_FILE_MODE;

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseNextFrameType(MP4Handle apihandle,
                                                          mp4_u32 *type)
	{
	MP4HandleImp handle = (MP4HandleImp)apihandle;

	if (!handle->metaDataComplete)
		{
		switch (metaDataAvailable(handle))
			{
			case 0:
				return MP4_NOT_AVAILABLE;

			case 1:
				handle->cafError = KErrNone;
				if (readMetaData(handle) < 0)
					{
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
					
					if ( handle->cafError != KErrNone )
						{
						// if CAF/DRM caused the error return it instead of generic errorcode.
						return handle->cafError;
						}
					else
						{
						return MP4_INVALID_INPUT_STREAM;
						}
					}
				handle->metaDataComplete = MP4TRUE;
				break;

		    case -2:
		      // Reading of FTYP meta data failed, so free up any allocated memory
		      freeFTYP(handle->ftyp);
		      handle->ftyp = NULL;
		      return MP4_ERROR;      
				
			case -1:
			default:
				return MP4_ERROR;
			}
		}

	/* No audio nor video */
	if ((!handle->moov->trakAudio) && (!handle->moov->trakVideo))
		return MP4_ERROR;
  
	mp4_u32 audiotype = 0;
	mp4_i32 audioerror = 0; 
	if (handle->moov->trakAudio)
		{
		audioerror = determineAudioType(handle, &audiotype);
		}
  
	mp4_u32 videotype = 0;
	mp4_i32 videoerror = 0;
	if (handle->moov->trakVideo)
		{
		videoerror = determineVideoType(handle, &videotype);
		}

	/* Audio track only */
	if (handle->moov->trakAudio && !handle->moov->trakVideo)
		{
		if (audioerror == 0)
			{
			// if audio is of supported type, check if the last audio frame has been 
			// retrieved
			if (handle->audioLast)
				{
				return MP4_NO_FRAME;
				}
			else
				{
				*type = audiotype;
				return MP4_OK;
				}
			}
		else
			{
			return MP4_ERROR;
			}
		}

	/* Video track only */
	if (handle->moov->trakVideo && !handle->moov->trakAudio)
		{
		if (videoerror == 0)
			{
			// if video is of supported type, check if the last video frame has been 
			// retrieved			
			if (handle->videoLast)
				{
				return MP4_NO_FRAME;
				}
			else
				{
				*type = videotype;
				return MP4_OK;
				}
			}
		else
			{
			return MP4_ERROR;
			}
		}

	/* All audio has been read, but there is video left */
	if (handle->audioLast && !handle->videoLast)
		{
		if (videoerror == 0)
			{
			*type = videotype;
			return MP4_OK;
			}
		else
			{
			return MP4_ERROR;
			}
		}

	/* All video has been read, but there is audio left */
	if (handle->videoLast && !handle->audioLast)
		{
		if (audioerror == 0)
			{
			*type = audiotype;
			return MP4_OK;
			}
		else
			{
			return MP4_ERROR;
			}
		}

	/* All data has been read */
	if (handle->audioLast && handle->videoLast)
		{
		return MP4_NO_FRAME;
		}

	if (handle->audioSampleOffset < handle->videoFrameOffset)
		{
		/* Next frame is audio */
		if (audioerror == 0)
			{
			*type = audiotype;
			return MP4_OK;
			}
		else if ( (audioerror == -2) && (videoerror == 0) )
			{
			*type = videotype;
			return MP4_OK;		
			}
		else
			{
			return MP4_ERROR;
			}
		}
	else
		{
		/* Next frame is video */
		if (videoerror == 0)
			{
			*type = videotype;
			return MP4_OK;
			}
		else if ( (videoerror == -2) && (audioerror == 0) )
			{
			*type = audiotype;
			return MP4_OK;		
			}
		else
			{
			return MP4_ERROR;
			}
		}
	}

extern EXPORT_C MP4Err MP4ParseNextFrameSize(MP4Handle apihandle,
                                                          mp4_u32 type,
                                                          mp4_u32 *framesize)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:

      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }

      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;            
      
    case -1:
    default:
      return MP4_ERROR;
    }
  }

  switch (type & handle->type)
  {
  case MP4_TYPE_MPEG4_VIDEO:
  case MP4_TYPE_H263_PROFILE_0:
  case MP4_TYPE_H263_PROFILE_3:
  case MP4_TYPE_AVC_PROFILE_BASELINE:
  case MP4_TYPE_AVC_PROFILE_MAIN:
  case MP4_TYPE_AVC_PROFILE_EXTENDED:
  case MP4_TYPE_AVC_PROFILE_HIGH:
  
    /* There is a next video frame */

    if (!handle->videoLast)
      *framesize = handle->videoFrameSize;
    else
    {
      *framesize = 0;

      return MP4_NO_REQUESTED_FRAME;
    }

    break;

  case MP4_TYPE_MPEG4_AUDIO:
  case MP4_TYPE_AMR_NB:
  case MP4_TYPE_AMR_WB:
  case MP4_TYPE_QCELP_13K:

    /* There is a next audio frame */

    if (!handle->audioLast)
      *framesize = handle->audioSampleSize;
    else
    {
      *framesize = 0;

      return MP4_NO_REQUESTED_FRAME;
    }

    break;

  case MP4_TYPE_NONE:
  default:

    if (type == MP4_TYPE_MPEG4_VIDEO ||
        type == MP4_TYPE_H263_PROFILE_0 ||
        type == MP4_TYPE_H263_PROFILE_3 ||
        type == MP4_TYPE_MPEG4_AUDIO ||
        type == MP4_TYPE_AMR_NB ||
        type == MP4_TYPE_AMR_WB ||
		isAvcVideo(type) ||
        type == MP4_TYPE_QCELP_13K
        )
      return MP4_NO_REQUESTED_FRAME;

    return MP4_INVALID_TYPE;
  }

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseReadVideoFrame(MP4Handle apihandle,
                                                           mp4_u8 *buffer,
                                                           mp4_u32 buffersize,
                                                           mp4_u32 *framesize,
                                                           mp4_u32 *timestamp,
                                                           mp4_bool *keyframe,
                                                           mp4_u32 *timestamp2)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:

      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }
      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      
      
    case -1:
    default:
      return MP4_ERROR;
    }
  }

  /* Is video available? */

  if (!((handle->type & MP4_TYPE_MPEG4_VIDEO) ||
        (handle->type & MP4_TYPE_H263_PROFILE_0) ||
        (handle->type & MP4_TYPE_H263_PROFILE_3) ||
		containsAvcVideo( handle->type ) ))
    return MP4_ERROR;

  /* Are there samples left? */

  if (handle->videoLast)
    return MP4_NO_FRAME;

  if (!handle->moov)
    return MP4_ERROR;
  if (!handle->moov->trakVideo)
    return MP4_ERROR;

  switch (fetchVideoFrame(handle, handle->moov->trakVideo, buffer, buffersize, framesize, timestamp, keyframe, timestamp2))
  {
  case -1:
    return MP4_ERROR;
  case -2:
    return MP4_BUFFER_TOO_SMALL;
  case -3:
    return MP4_NOT_AVAILABLE;
  case -4:
    return MP4_INVALID_INPUT_STREAM;
  default:
    break;
  }

  switch (advanceVideoFrame(handle, handle->moov->trakVideo))
  {
  case -1:
    return MP4_ERROR;
  case -2:
    handle->videoLast = MP4TRUE;
    break;
  default:
    break;
  }

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseReadVideoDecoderSpecificInfo(MP4Handle apihandle,
                                                                         mp4_u8 *buffer,
                                                                         mp4_u32 buffersize,
                                                                         mp4_u32 *decspecinfosize)
	{
	MP4HandleImp handle = (MP4HandleImp)apihandle;
	if (!handle->metaDataComplete)
		{
		switch (metaDataAvailable(handle))
			{
			case 0:
				return MP4_NOT_AVAILABLE;

			case 1:
				handle->cafError = KErrNone;
				if (readMetaData(handle) < 0)
					{
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
					
					if ( handle->cafError != KErrNone )
						{
						// if CAF/DRM caused the error return it instead of generic errorcode.
						return handle->cafError;
						}
					else
						{
						return MP4_INVALID_INPUT_STREAM;
						}
					}
				handle->metaDataComplete = MP4TRUE;
				break;

		    case -2:
		      // Reading of FTYP meta data failed, so free up any allocated memory
		      freeFTYP(handle->ftyp);
		      handle->ftyp = NULL;
		      return MP4_ERROR;      

			case -1:
			default:
				return MP4_ERROR;
			}
		}

	/* Is video type MPEG or AVC? */
	if (!(handle->type & MP4_TYPE_MPEG4_VIDEO) && 
	   !(containsAvcVideo( handle->type )) )
		{
		return MP4_ERROR;
		}

	if (!handle->moov)
		{
		return MP4_ERROR;
		}
	if (!handle->moov->trakVideo)
		{
		return MP4_ERROR;
		}
	if (!handle->moov->trakVideo->mdia)
		{
		return MP4_ERROR;
		}
	if (!handle->moov->trakVideo->mdia->minf)
		{
		return MP4_ERROR;
		}
	if (!handle->moov->trakVideo->mdia->minf->stbl)
		{
		return MP4_ERROR;
		}
	if (!handle->moov->trakVideo->mdia->minf->stbl->stsd)
		{
		return MP4_ERROR;
		}
	if (handle->videoSampleEntryIndex > handle->moov->trakVideo->mdia->minf->stbl->stsd->entryCount)
		{
		return MP4_ERROR;
		}			

	TInt index = handle->videoSampleEntryIndex - 1;		  
	if (handle->videoSampleEntryIndex == 0)			  
		{
		// even though the video sample contains no actual data, if the video sample exist
		// and contains decoder specific info, return it anyway
		index = 0;
		}
	
	if (handle->type & MP4_TYPE_MPEG4_VIDEO)
		{
		/* Copy DecoderSpecificInfo into buffer */
		if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[index])
			{
			return MP4_ERROR;
			}
		if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[index]->esd)
			{
			return MP4_ERROR;
			}

		*decspecinfosize = handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[index]->esd->decSpecificInfoSize;
		if (buffersize < handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[index]->esd->decSpecificInfoSize)
			{
			return MP4_BUFFER_TOO_SMALL;
			}

		mp4memcpy(buffer, 
				  handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[index]->esd->decSpecificInfo,
				  handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[index]->esd->decSpecificInfoSize);
		}
  	else if ( containsAvcVideo( handle->type ) )
  		{
  		/* Copy the AVCDecoderConfigurationRecord into buffer */
  		if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[index])
  			{
  			return MP4_ERROR;
  			}
  		if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[index]->avcc)
  			{
  			return MP4_ERROR;
  			}

  		*decspecinfosize = handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[index]->avcc->avcConfigSize;
  		if (buffersize < handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[index]->avcc->avcConfigSize)
  			{
  			return MP4_BUFFER_TOO_SMALL;
  			}

  		mp4memcpy(buffer,
  				  handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[index]->avcc->avcConfig,
				  handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[index]->avcc->avcConfigSize);
  		}

	return MP4_OK;
	}

extern EXPORT_C MP4Err MP4ParseReadAudioFrames(MP4Handle apihandle,
                                                            mp4_u8 *buffer,
                                                            mp4_u32 buffersize,
                                                            mp4_u32 *audiosize,
                                                            mp4_u32 *timestamp,
                                                            mp4_u32 *returnedframes,
                                                            mp4_u32 *timestamp2)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:
      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }
      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      
      
    case -1:
    default:
      return MP4_ERROR;
    }
  }

  /* Is audio available? */

  if (!((handle->type & MP4_TYPE_MPEG4_AUDIO) ||
        (handle->type & MP4_TYPE_AMR_NB) ||
        (handle->type & MP4_TYPE_AMR_WB) ||
        (handle->type & MP4_TYPE_QCELP_13K)))
    return MP4_ERROR;

  /* Are there samples left? */

  if (handle->audioLast)
    return MP4_NO_FRAME;

  if (!handle->moov)
    return MP4_ERROR;
  if (!handle->moov->trakAudio)
    return MP4_ERROR;

  switch (fetchAudioSample(handle, handle->moov->trakAudio, buffer, buffersize, audiosize, timestamp, returnedframes, timestamp2))
  {
  case -1:
    return MP4_ERROR;
  case -2:
    return MP4_BUFFER_TOO_SMALL;
  case -3:
    return MP4_NOT_AVAILABLE;
  case -4:
    return MP4_INVALID_INPUT_STREAM;
  default:
    break;
  }

  switch (advanceAudioSample(handle, handle->moov->trakAudio))
  {
  case -1:
    return MP4_ERROR;
  case -2:
    handle->audioLast = MP4TRUE;
    break;
  default:
    break;
  }

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseReadAudioDecoderSpecificInfo(MP4Handle apihandle,
                                                                         mp4_u8 *buffer,
                                                                         mp4_u32 buffersize,
                                                                         mp4_u32 *decspecinfosize)
	{
	MP4HandleImp handle = (MP4HandleImp)apihandle;

	if (!handle->metaDataComplete)
		{
		switch (metaDataAvailable(handle))
			{
			case 0:
				return MP4_NOT_AVAILABLE;

			case 1:
				handle->cafError = KErrNone;
				if (readMetaData(handle) < 0)
					{
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
					
					if ( handle->cafError != KErrNone )
						{
						// if CAF/DRM caused the error return it instead of generic errorcode.
						return handle->cafError;
						}
					else
						{
						return MP4_INVALID_INPUT_STREAM;
						}
					}
				handle->metaDataComplete = MP4TRUE;
				break;

		    case -2:
		      // Reading of FTYP meta data failed, so free up any allocated memory
		      freeFTYP(handle->ftyp);
		      handle->ftyp = NULL;
		      return MP4_ERROR;      
				
			case -1:
			default:
				return MP4_ERROR;
			}
		}

    if (!handle->moov)
    	{
    	return MP4_ERROR;
    	}
    if (!handle->moov->trakAudio)
    	{
    	return MP4_ERROR;
    	}
    if (!handle->moov->trakAudio->mdia)
    	{
    	return MP4_ERROR;
    	}
    if (!handle->moov->trakAudio->mdia->minf)
    	{
    	return MP4_ERROR;
    	}
    if (!handle->moov->trakAudio->mdia->minf->stbl)
    	{
    	return MP4_ERROR;
    	}
    if (!handle->moov->trakAudio->mdia->minf->stbl->stsd)
    	{
    	return MP4_ERROR;
    	}
	
    
	TInt index = handle->audioSampleEntryIndex - 1;		  
	if (handle->audioSampleEntryIndex == 0)			  
		{
		// even though the audio sample contains no actual data, if the audio sample exist
		// and contains decoder specific info, return it anyway
		index = 0;
		}
    
	/* Audio type */
	if ((handle->type & MP4_TYPE_MPEG4_AUDIO) || 
		((handle->type & MP4_TYPE_QCELP_13K) && (handle->qcelpStoredAsMPEGAudio)))
		{
		/* Copy DecoderSpecificInfo into buffer */
		if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[index])
			{
			return MP4_ERROR;
			}
		if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[index]->esd)
			{
			return MP4_ERROR;
			}

		*decspecinfosize = handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[index]->esd->decSpecificInfoSize;
		if (buffersize < handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[index]->esd->decSpecificInfoSize)
			{
			return MP4_BUFFER_TOO_SMALL;
			}

		mp4memcpy(buffer,
				  handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[index]->esd->decSpecificInfo,
				  handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[index]->esd->decSpecificInfoSize);
		}
	else if ((handle->type & MP4_TYPE_AMR_NB) || 
			 (handle->type & MP4_TYPE_AMR_WB) || 
			 ((handle->type & MP4_TYPE_QCELP_13K) && (!handle->qcelpStoredAsMPEGAudio)))
		{
		/* Copy DecoderSpecificInfo into buffer */
		if (handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[index] == NULL &&
			handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[index] == NULL &&
            handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[index] == NULL)
			{
			return MP4_ERROR;
			}

		if (handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[index])
			{
			if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[index]->damr)
				{
				return MP4_ERROR;
				}

			*decspecinfosize = 9;
			if (buffersize < *decspecinfosize)
				{
				return MP4_BUFFER_TOO_SMALL;
				}

			mp4memcpy(buffer, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[index]->damr->vendor), 4);
			mp4memcpy(buffer+4, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[index]->damr->decoderVersion), 1);
			mp4memcpy(buffer+5, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[index]->damr->modeSet), 2);
			mp4memcpy(buffer+7, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[index]->damr->modeChangePeriod), 1);
			mp4memcpy(buffer+8, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[index]->damr->framesPerSample), 1);
			}

		if (handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[index])
			{
			if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[index]->damr)
				{
				return MP4_ERROR;
				}

			*decspecinfosize = 9;
			if (buffersize < *decspecinfosize)
				{
				return MP4_BUFFER_TOO_SMALL;
				}

			mp4memcpy(buffer, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[index]->damr->vendor), 4);
			mp4memcpy(buffer+4, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[index]->damr->decoderVersion), 1);
			mp4memcpy(buffer+5, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[index]->damr->modeSet), 2);
			mp4memcpy(buffer+7, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[index]->damr->modeChangePeriod), 1);
			mp4memcpy(buffer+8, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[index]->damr->framesPerSample), 1);
			}

		if (handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[index])
			{
			if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[index]->dqcp)
				{
				return MP4_ERROR;
				}

			*decspecinfosize = 6;
			if (buffersize < *decspecinfosize)
				{
				return MP4_BUFFER_TOO_SMALL;
				}

			mp4memcpy(buffer, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[index]->dqcp->vendor), 4);
			mp4memcpy(buffer+4, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[index]->dqcp->decoderVersion), 1);
			mp4memcpy(buffer+5, &(handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[index]->dqcp->framesPerSample), 1);
			}
		}
	else
		{
		return MP4_ERROR;
		}
	
	return MP4_OK;
	}

extern EXPORT_C MP4Err MP4ParseGetNextVideoTimestamp(MP4Handle apihandle,
                                                                  mp4_u32 *timestamp,
                                                                  mp4_u32 *timestamp2)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:

      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }
      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      
      
    case -1:
    default:
      return MP4_ERROR;
    }
  }

  if (!handle->moov)
    return MP4_ERROR;

  if (!handle->moov->trakVideo)
    return MP4_ERROR;

  if (!handle->moov->trakVideo->mdia)
    return MP4_ERROR;

  if (handle->videoLast)
    return MP4_END_OF_VIDEO;

  if (convertVideoSampleToTime(handle, handle->moov->trakVideo->mdia, timestamp, timestamp2) < 0)
    return MP4_ERROR;

  switch (advanceVideoFrame(handle, handle->moov->trakVideo))
  {
  case -1:
    return MP4_ERROR;
  case -2:
    handle->videoLast = MP4TRUE;
    break;
  default:
    break;
  }

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseIsStreamable(MP4Handle apihandle)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:

      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }
      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      

    case -1:
    default:
      return MP4_ERROR;
    }
  }

  /* There is no audio nor video */

  if (!handle->audioSampleNum && !handle->videoSampleNum)
    return MP4_ERROR;

  /* There is audio, but no video */

  if (handle->audioSampleNum && !handle->videoSampleNum)
  {
    if (!handle->moov)
      return MP4_ERROR;

    if (!handle->moov->trakAudio)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia->minf)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia->minf->stbl)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia->minf->stbl->stco)
      return MP4_ERROR;

    if (handle->moov->trakAudio->mdia->minf->stbl->stco->entryCount < 2)
      return MP4_OK;
    
    if (getChunkOffset(handle->moov->trakAudio->mdia->minf->stbl, 0) <
        getChunkOffset(handle->moov->trakAudio->mdia->minf->stbl, 1))
      return MP4_OK;

    return MP4_NOT_STREAMABLE;
  }

  /* There is video, but no audio */

  if (handle->videoSampleNum && !handle->audioSampleNum)
  {
    if (!handle->moov)
      return MP4_ERROR;

    if (!handle->moov->trakVideo)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl->stco)
      return MP4_ERROR;

    if (handle->moov->trakVideo->mdia->minf->stbl->stco->entryCount < 2)
      return MP4_OK;

    if (getChunkOffset(handle->moov->trakVideo->mdia->minf->stbl, 0) <
        getChunkOffset(handle->moov->trakVideo->mdia->minf->stbl, 1))
      return MP4_OK;

    return MP4_NOT_STREAMABLE;
  }

  /* There are both audio and video */

  if (handle->videoSampleNum && handle->audioSampleNum)
  {
    mp4_i64 diff;


    if (!handle->moov)
      return MP4_ERROR;

    if (!handle->moov->trakAudio)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia->minf)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia->minf->stbl)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia->minf->stbl->stco)
      return MP4_ERROR;

    if (!handle->moov->trakAudio->mdia->minf->stbl->stco->entryCount)
      return MP4_ERROR;

    if (!handle->moov->trakVideo)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl->stco)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl->stco->entryCount)
      return MP4_ERROR;

    diff = getChunkOffset(handle->moov->trakAudio->mdia->minf->stbl, 0) -
           getChunkOffset(handle->moov->trakVideo->mdia->minf->stbl, 0);

    /* If the distance between 1st audio and video chunk offsets is larger
       than 50000, MP4 is not streamable. */

    if (diff < -50000 || diff > 50000)
      return MP4_NOT_STREAMABLE;

    return MP4_OK;
  }

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseSeek(MP4Handle apihandle,
                                                 mp4_u32 position,
                                                 mp4_u32 *audioPosition,
                                                 mp4_u32 *videoPosition,
                                                 mp4_bool keyframe)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  if (!handle->metaDataComplete)
      {
      switch (metaDataAvailable(handle))
          {
          case 0:
              return MP4_NOT_AVAILABLE;
          case 1:
              handle->cafError = KErrNone;
              if (readMetaData(handle) < 0)
                  {
			      // Reading of meta data failed, so free up any allocated memory
		  		  freeFTYP(handle->ftyp);
		  		  handle->ftyp = NULL;
				  freeMOOV(handle->moov);
				  handle->moov = NULL;
                  
                  if ( handle->cafError != KErrNone )
                      {// if CAF/DRM caused the error return it instead of generic errorcode.
                      return handle->cafError;
                      }
                  else
                      {
                      return MP4_INVALID_INPUT_STREAM;
                      }
                  }
              handle->metaDataComplete = MP4TRUE;
              break;

          case -2:
            // Reading of FTYP meta data failed, so free up any allocated memory
            freeFTYP(handle->ftyp);
            handle->ftyp = NULL;
            return MP4_ERROR;      

          case -1:
          default:
              return MP4_ERROR;
          }
      }

  if (!handle->moov)
    return MP4_ERROR;

  handle->audioLast = MP4FALSE;
  handle->videoLast = MP4FALSE;

  /* There is no audio nor video */

  if (!handle->audioSampleNum && !handle->videoSampleNum)
    return MP4_ERROR;

  /* There is only audio */

  if (handle->audioSampleNum && !handle->videoSampleNum)
  {
    mp4_u32 audioSample;

    if (!handle->moov->trakAudio)
      return MP4_ERROR;
    if (!handle->moov->trakAudio->mdia)
      return MP4_ERROR;

    if (convertTimeToSample(handle, handle->moov->trakAudio, position, &audioSample) < 0)
      return MP4_CANT_SEEK;

    if (goToAudioSample(handle, handle->moov->trakAudio, audioSample) < 0)
      return MP4_ERROR;

    if (convertAudioSampleToTime(handle, handle->moov->trakAudio->mdia, audioPosition, NULL) < 0)
      return MP4_ERROR;
    
    if (handle->file)
    	{
    	handle->lastAccessedPosInFile = handle->audioSampleOffset + handle->audioSampleSize - 1;
    	}
    else
    	{
    	// check if there is enough data in the buffers
    	if (handle->audioSampleOffset + handle->audioSampleSize - 1 > getCumulativeBufferedBytes(handle))
	      	{
	      	if (handle->LastWriteDataCalled)
	      		{
	      		// user has indicated that no more data will be available
	      		return MP4_CANT_SEEK;	
	      		}
	      	else
		      	{
		      	// signal to user that more data needed
				return MP4_NOT_AVAILABLE;	
		      	}			
			}
    	else if (handle->audioSampleOffset < handle->absPosition)
    		{
    		handle->absPosition = handle->audioSampleOffset;
    		}    	
    	}

    return MP4_OK;
  	}

  	/* There is only video */
  	if (handle->videoSampleNum && !handle->audioSampleNum)
  		{
  		mp4_u32 videoSample;
  		mp4_u32 newVideoSample;

  		if (!handle->moov->trakVideo)
  			return MP4_ERROR;
  		if (!handle->moov->trakVideo->mdia)
  			return MP4_ERROR;

  		if (convertTimeToSample(handle, handle->moov->trakVideo, position, &videoSample) < 0)
  			return MP4_CANT_SEEK;

  		if (keyframe)
  			{
  			if (findVideoKeyFrame(handle, handle->moov->trakVideo, videoSample, &newVideoSample) < 0)
  				{
  				return MP4_CANT_SEEK;
  				}
  			videoSample = newVideoSample;
  			}

  		if (goToVideoSample(handle, handle->moov->trakVideo, videoSample) < 0)
  			{
  			return MP4_ERROR;
  			}

  		if (convertVideoSampleToTime(handle, handle->moov->trakVideo->mdia, videoPosition, NULL) < 0)
  			{
  			return MP4_ERROR;
  			}

  		if (handle->file)
  			{
  			handle->lastAccessedPosInFile = handle->videoFrameOffset + handle->videoFrameSize - 1;
  			}
  		else  // input is a stream
  			{ 
  			// check if there is enough data in the buffers
  			if (handle->videoFrameOffset + handle->videoFrameSize > getCumulativeBufferedBytes(handle))
  				{
  		      	if (handle->LastWriteDataCalled)
  		      		{
  		      		// user has indicated that no more data will be available
  		      		return MP4_CANT_SEEK;	
  		      		}
  		      	else
  			      	{
  			      	// signal to user that more data needed
  					return MP4_NOT_AVAILABLE;	
  			      	}			
  				}
  			else
  				{
  				handle->absPosition = handle->videoFrameOffset;
  				}
  			}

  		return MP4_OK;
  		}

  /* There is audio and video */

  if (handle->videoSampleNum && handle->audioSampleNum)
  {
    mp4_u32 audioSample;
    mp4_u32 videoSample;
    mp4_u32 newVideoSample;


    if (!handle->moov->trakAudio)
      return MP4_ERROR;
    if (!handle->moov->trakAudio->mdia)
      return MP4_ERROR;
    if (!handle->moov->trakVideo)
      return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia)
      return MP4_ERROR;

    if (convertTimeToSample(handle, handle->moov->trakVideo, position, &videoSample) < 0)
      return MP4_CANT_SEEK;

    if (keyframe)
    {
      if (findVideoKeyFrame(handle, handle->moov->trakVideo, videoSample, &newVideoSample) < 0)
        return MP4_CANT_SEEK;

      videoSample = newVideoSample;
    }

    if (goToVideoSample(handle, handle->moov->trakVideo, videoSample) < 0)
      return MP4_ERROR;

    if (convertVideoSampleToTime(handle, handle->moov->trakVideo->mdia, videoPosition, NULL) < 0)
      return MP4_ERROR;

    if (handle->file)
    	{
    	handle->lastAccessedPosInFile = handle->videoFrameOffset + handle->videoFrameSize - 1;	
    	}      
    else  // input is a stream
    	{	
    	// check if there is enough data in the buffers
		if (handle->videoFrameOffset + handle->videoFrameSize > getCumulativeBufferedBytes(handle))
	      	{
	      	if (handle->LastWriteDataCalled)
	      		{
	      		// user has indicated that no more data will be available
	      		return MP4_CANT_SEEK;	
	      		}
	      	else
		      	{
		      	// signal to user that more data needed
				return MP4_NOT_AVAILABLE;	
		      	}			
	      	}        	
		else
	      	{
	        handle->absPosition = handle->videoFrameOffset;
	      	}        	
    }

    /* Audio */

    if (convertTimeToSample(handle, handle->moov->trakAudio, *videoPosition, &audioSample) < 0)
      return MP4_CANT_SEEK;

    if (goToAudioSample(handle, handle->moov->trakAudio, audioSample) < 0)
      return MP4_ERROR;

    if (convertAudioSampleToTime(handle, handle->moov->trakAudio->mdia, audioPosition, NULL) < 0)
      return MP4_ERROR;

    if (handle->file)  
    	{
	      if (handle->audioSampleOffset + handle->audioSampleSize - 1 > handle->lastAccessedPosInFile)
	      	{
			handle->lastAccessedPosInFile = handle->audioSampleOffset + handle->audioSampleSize - 1;      	
	      	}        
    	}
    else   // input is a stream
    	{
    	// check if there is enough data in the buffers
		if (handle->audioSampleOffset + handle->audioSampleSize - 1 > getCumulativeBufferedBytes(handle))
			{
	      	if (handle->LastWriteDataCalled)
	      		{
	      		// user has indicated that no more data will be available
	      		return MP4_CANT_SEEK;	
	      		}
	      	else
		      	{
		      	// signal to user that more data needed
				return MP4_NOT_AVAILABLE;	
		      	}
			}
		else if (handle->audioSampleOffset < handle->absPosition)
			{
			handle->absPosition = handle->audioSampleOffset;
			}
		}

    return MP4_OK;
  }

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseIsFrameAvailable(MP4Handle apihandle,
                                                             mp4_u32 type)
{
    MP4HandleImp handle = (MP4HandleImp)apihandle;
    if (!handle->metaDataComplete)
        {
        switch (metaDataAvailable(handle))
            {
            case 0:
                return MP4_NOT_AVAILABLE;
            case 1:
                handle->cafError = KErrNone;
                if (readMetaData(handle) < 0)
                    {
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
                    
                  	if ( handle->cafError != KErrNone )
                      	{
						// if CAF/DRM caused the error return it instead of generic errorcode.

                      	return handle->cafError;
                     	}
                  	else
                      	{
                        return MP4_INVALID_INPUT_STREAM;
                   	    }
                    }
                handle->metaDataComplete = MP4TRUE;
                break;

            case -2:
              // Reading of FTYP meta data failed, so free up any allocated memory
              freeFTYP(handle->ftyp);
              handle->ftyp = NULL;
              return MP4_ERROR;      

            case -1:
            default:
                return MP4_ERROR;
            }
        }

  switch (type & handle->type)
  {
  case MP4_TYPE_MPEG4_VIDEO:
  case MP4_TYPE_H263_PROFILE_0:
  case MP4_TYPE_H263_PROFILE_3:
  case MP4_TYPE_AVC_PROFILE_BASELINE:
  case MP4_TYPE_AVC_PROFILE_MAIN:
  case MP4_TYPE_AVC_PROFILE_EXTENDED:
  case MP4_TYPE_AVC_PROFILE_HIGH:

    /* There is no frame available if last sample has been reached */

    if (handle->videoLast)
      return MP4_NO_REQUESTED_FRAME;

    /* Input in a file => it is available */

    if (handle->file)
      return MP4_OK;

    /* If frame has been buffered, it is available */

    if (handle->videoFrameOffset + handle->videoFrameSize <=
        getCumulativeBufferedBytes(handle))
      return MP4_OK;
    else
      return MP4_NOT_AVAILABLE;

  case MP4_TYPE_MPEG4_AUDIO:
  case MP4_TYPE_AMR_NB:
  case MP4_TYPE_AMR_WB:
  case MP4_TYPE_QCELP_13K:

    /* There is no frame available if last sample has been reached */

    if (handle->audioLast)
      return MP4_NO_REQUESTED_FRAME;

    /* Input in a file => it is available */

    if (handle->file)
      return MP4_OK;

    /* If frame has been buffered, it is available */

    if (handle->audioSampleOffset + handle->audioSampleSize <=
        getCumulativeBufferedBytes(handle))
      return MP4_OK;
    else
      return MP4_NOT_AVAILABLE;

  case MP4_TYPE_NONE:
  default:

    return MP4_NO_REQUESTED_FRAME;
  }
}

extern EXPORT_C MP4Err MP4ParseGetLastPosition(MP4Handle apihandle,
                                                            mp4_u32 *position)
{
  return MP4ParseGetLastPosition64(apihandle, (mp4_u64 *)position);
}

extern EXPORT_C MP4Err MP4ParseGetLastPosition64(MP4Handle apihandle,
                                                            mp4_u64 *position)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  if (!handle->file)
      {
      *position = handle->absPosition; // return the latest accessed absolute byte location of the stream.
      return MP4_OK;
      }
  *position = handle->lastAccessedPosInFile;

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseGetNumberOfVideoFrames(MP4Handle apihandle, 
                                                                   mp4_u32 *numberOfFrames)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  if (!handle->metaDataComplete)
      {
      switch (metaDataAvailable(handle))
          {
          case 0:
              return MP4_NOT_AVAILABLE;
          case 1:
              handle->cafError = KErrNone;
              if (readMetaData(handle) < 0)
                  {
			      // Reading of meta data failed, so free up any allocated memory
		  		  freeFTYP(handle->ftyp);
		  		  handle->ftyp = NULL;
				  freeMOOV(handle->moov);
			 	  handle->moov = NULL;
                  
                  if ( handle->cafError != KErrNone )
                      {// if CAF/DRM caused the error return it instead of generic errorcode.
                      return handle->cafError;
                      }
                  else
                      {
                      return MP4_INVALID_INPUT_STREAM;
                      }
                  }
              handle->metaDataComplete = MP4TRUE;
              break;

          case -2:
            // Reading of FTYP meta data failed, so free up any allocated memory
            freeFTYP(handle->ftyp);
            handle->ftyp = NULL;
            return MP4_ERROR;      

          case -1:
          default:
              return MP4_ERROR;
          }
      }
  
  if (!handle->moov)
    return MP4_ERROR;

  if (handle->moov->trakVideo)
  {
    if (!handle->moov->trakVideo->mdia)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl->stsz)
      return MP4_ERROR;
   }
   else
   {
      return MP4_ERROR;
   }

   *numberOfFrames = handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleCount;
   return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseGetVideoFrameSize(MP4Handle apihandle, 
                                                              mp4_u32 index, 
                                                              mp4_u32 *frameSize)
	{
	MP4HandleImp handle = (MP4HandleImp)apihandle;
	if (!handle->metaDataComplete)
		{
		switch (metaDataAvailable(handle))
			{
			case 0:
				return MP4_NOT_AVAILABLE;
			case 1:
				handle->cafError = KErrNone;
				if (readMetaData(handle) < 0)
					{
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;

					if ( handle->cafError != KErrNone )
						{// if CAF/DRM caused the error return it instead of generic errorcode.
						return handle->cafError;
						}
					else
						{
						return MP4_INVALID_INPUT_STREAM;
						}
					}
				handle->metaDataComplete = MP4TRUE;
				break;

		    case -2:
		      // Reading of FTYP meta data failed, so free up any allocated memory
		      freeFTYP(handle->ftyp);
		      handle->ftyp = NULL;
		      return MP4_ERROR;      

			case -1:
			default:
				return MP4_ERROR;
			}
		}

	if (!handle->moov)
		return MP4_ERROR;

	if (handle->moov->trakVideo)
		{
	    if (!handle->moov->trakVideo->mdia)
	    	return MP4_ERROR;
	
	    if (!handle->moov->trakVideo->mdia->minf)
	    	return MP4_ERROR;
	
	    if (!handle->moov->trakVideo->mdia->minf->stbl)
	    	return MP4_ERROR;
	
	    if (!handle->moov->trakVideo->mdia->minf->stbl->stsz)
	    	return MP4_ERROR;
	    
	    // ensure the index entered is within bound
	    if (index >= handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleCount)
	    	return MP4_ERROR;     	
		}
	else
		{
		return MP4_ERROR;
		}

	*frameSize = handle->moov->trakVideo->mdia->minf->stbl->stsz->entrySize[index];
	return MP4_OK;
	}

extern EXPORT_C MP4Err MP4ParseGetVideoFrameStartTime(MP4Handle apihandle, 
                                                                   mp4_u32 index, 
                                                                   mp4_u32 *timestamp,
                                                                   mp4_u32 *timestampms)
{
  mp4_u32      tmptime=0;
  mp4_u32      sample=0;
  mp4_u32      entry=0;
  mp4_u32      videoSampleNumber = index+1;

  MP4HandleImp handle = (MP4HandleImp)apihandle;
  if (!handle->metaDataComplete)
      {
      switch (metaDataAvailable(handle))
          {
          case 0:
              return MP4_NOT_AVAILABLE;
          case 1:
              handle->cafError = KErrNone;
              if (readMetaData(handle) < 0)
                  {
		      	  // Reading of meta data failed, so free up any allocated memory
		  		  freeFTYP(handle->ftyp);
		  		  handle->ftyp = NULL;
				  freeMOOV(handle->moov);
				  handle->moov = NULL;
                  
                  if ( handle->cafError != KErrNone )
                      {// if CAF/DRM caused the error return it instead of generic errorcode.
                      return handle->cafError;
                      }
                  else
                      {
                      return MP4_INVALID_INPUT_STREAM;
                      }
                  }
              handle->metaDataComplete = MP4TRUE;
              break;

          case -2:
            // Reading of FTYP meta data failed, so free up any allocated memory
            freeFTYP(handle->ftyp);
            handle->ftyp = NULL;
            return MP4_ERROR;      

          case -1:
          default:
              return MP4_ERROR;
          }
      }  

  if (!handle->moov)
  {
    return MP4_ERROR;
  }

  if (handle->moov->trakVideo)
  {
    if (!handle->moov->trakVideo->mdia)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl)
      return MP4_ERROR;

    if (!handle->moov->trakVideo->mdia->minf->stbl->stts)
      return MP4_ERROR;
  }
  else
  {
    return MP4_ERROR;
  }

  for (;;)
  {
    if (sample + handle->moov->trakVideo->mdia->minf->stbl->stts->sampleCount[entry] < videoSampleNumber)
    {
      sample += handle->moov->trakVideo->mdia->minf->stbl->stts->sampleCount[entry];
      tmptime += (handle->moov->trakVideo->mdia->minf->stbl->stts->sampleCount[entry] * 
				handle->moov->trakVideo->mdia->minf->stbl->stts->sampleDelta[entry]);
      entry++;
      if (entry == handle->moov->trakVideo->mdia->minf->stbl->stts->entryCount)
        return MP4_ERROR;
    }
    else
    {
      tmptime += ((videoSampleNumber - sample - 1) * handle->moov->trakVideo->mdia->minf->stbl->stts->sampleDelta[entry]);
      break;
    }
  }
  if (handle->moov->trakVideo->mdia->mdhd->timeScale == 0)
    {
    return MP4_ERROR;
    }
  
  *timestamp = tmptime;

  *timestampms =(mp4_u32)((mp4_double)tmptime * (mp4_double)1000 / 
                               (mp4_double)handle->moov->trakVideo->mdia->mdhd->timeScale + (mp4_double)0.5);

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseGetVideoFrameType(MP4Handle apihandle, 
                                                              mp4_u32 index, 
                                                              mp4_bool *frametype)
	{
	mp4_u32  i;
	mp4_u32  videoSampleNumber = index+1;
	mp4_bool keyFrame = MP4FALSE;

  MP4HandleImp handle = (MP4HandleImp)apihandle;
  if (!handle->metaDataComplete)
      {
      switch (metaDataAvailable(handle))
          {
          case 0:
              return MP4_NOT_AVAILABLE;
          case 1:
              handle->cafError = KErrNone;
              if (readMetaData(handle) < 0)
                  {
		      	  // Reading of meta data failed, so free up any allocated memory
		  		  freeFTYP(handle->ftyp);
		  		  handle->ftyp = NULL;
				  freeMOOV(handle->moov);
				  handle->moov = NULL;
                  
                  if ( handle->cafError != KErrNone )
                      {// if CAF/DRM caused the error return it instead of generic errorcode.
                      return handle->cafError;
                      }
                  else
                      {
                      return MP4_INVALID_INPUT_STREAM;
                      }
                  }
              handle->metaDataComplete = MP4TRUE;
              break;

          case -2:
            // Reading of FTYP meta data failed, so free up any allocated memory
            freeFTYP(handle->ftyp);
            handle->ftyp = NULL;
            return MP4_ERROR;      

          case -1:
          default:
              return MP4_ERROR;
          }
      }  

	if (!handle->moov)
		return MP4_ERROR;

	if (handle->moov->trakVideo)
		{
		if (!handle->moov->trakVideo->mdia)
			return MP4_ERROR;

		if (!handle->moov->trakVideo->mdia->minf)
			return MP4_ERROR;
		
		if (!handle->moov->trakVideo->mdia->minf->stbl)
			return MP4_ERROR;
		
		if (!handle->moov->trakVideo->mdia->minf->stbl->stss)
			return MP4_ERROR;

		// if the video frame index is out of bounds
		if (!handle->moov->trakVideo->mdia->minf->stbl->stsz ||
			videoSampleNumber > handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleCount)
			return MP4_ERROR;
		}
  	else
  		{
  		return MP4_ERROR;
  		}

	for (i=0; i < handle->moov->trakVideo->mdia->minf->stbl->stss->entryCount; i++)
		{
    	if (handle->moov->trakVideo->mdia->minf->stbl->stss->sampleNumber[i] == videoSampleNumber)
			{
			keyFrame = MP4TRUE;
			break;
	    	}
  		}
  
	*frametype = keyFrame;
  	return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseGetVideoSampleEntryIndex(MP4Handle apihandle, mp4_u32 *videosampleentryindex)
{
    MP4HandleImp handle = (MP4HandleImp)apihandle;
    if (!handle->metaDataComplete)
        {
        switch (metaDataAvailable(handle))
            {
            case 0:
                return MP4_NOT_AVAILABLE;
            case 1:
                handle->cafError = KErrNone;
                if (readMetaData(handle) < 0)
                    {
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
                    
                    if ( handle->cafError != KErrNone )
                        {// if CAF/DRM caused the error return it instead of generic errorcode.
                        return handle->cafError;
                        }
                    else
                        {
                        return MP4_INVALID_INPUT_STREAM;
                        }
                    }
                handle->metaDataComplete = MP4TRUE;
                break;

            case -2:
              // Reading of FTYP meta data failed, so free up any allocated memory
              freeFTYP(handle->ftyp);
              handle->ftyp = NULL;
              return MP4_ERROR;      

            case -1:
            default:
                return MP4_ERROR;
            }
        }    

    if (!handle->moov->trakVideo)
      return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia)
      return MP4_ERROR;

	if (handle->videoSampleEntryIndex == 0) /* can't be zero. Sample Entry index must be at least 1*/
		return MP4_ERROR;
	else
		*videosampleentryindex = handle->videoSampleEntryIndex;

	return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseGetAudioSampleEntryIndex(MP4Handle apihandle, mp4_u32 *audiosampleentryindex)
{
    MP4HandleImp handle = (MP4HandleImp)apihandle;
    if (!handle->metaDataComplete)
        {
        switch (metaDataAvailable(handle))
            {
            case 0:
                return MP4_NOT_AVAILABLE;
            case 1:
                handle->cafError = KErrNone;
                if (readMetaData(handle) < 0)
                    {
      				// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
                    
                    if ( handle->cafError != KErrNone )
                        {// if CAF/DRM caused the error return it instead of generic errorcode.
                        return handle->cafError;
                        }
                    else
                        {
                        return MP4_INVALID_INPUT_STREAM;
                        }
                    }
                handle->metaDataComplete = MP4TRUE;
                break;

            case -2:
              // Reading of FTYP meta data failed, so free up any allocated memory
              freeFTYP(handle->ftyp);
              handle->ftyp = NULL;
              return MP4_ERROR;      

            case -1:
            default:
                return MP4_ERROR;
            }
        }    

    if (!handle->moov->trakAudio)
      return MP4_ERROR;
    if (!handle->moov->trakAudio->mdia)
      return MP4_ERROR;

	if (handle->audioSampleEntryIndex == 0) /* can't be zero. Sample Entry index must be at least 1*/
		return MP4_ERROR;
	else
		*audiosampleentryindex = handle->audioSampleEntryIndex;

	return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseGetQCELPStorageMode(MP4Handle apihandle, mp4_u8 *qcelpStorageMode)
{
    MP4HandleImp handle = (MP4HandleImp)apihandle;
    if (!handle->metaDataComplete)
        {
        switch (metaDataAvailable(handle))
            {
            case 0:
                return MP4_NOT_AVAILABLE;
            case 1:
                handle->cafError = KErrNone;
                if (readMetaData(handle) < 0)
                    {
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
                    
                    if ( handle->cafError != KErrNone )
                        {// if CAF/DRM caused the error return it instead of generic errorcode.
                        return handle->cafError;
                        }
                    else
                        {
                        return MP4_INVALID_INPUT_STREAM;
                        }
                    }
                handle->metaDataComplete = MP4TRUE;
                break;

            case -2:
              // Reading of FTYP meta data failed, so free up any allocated memory
              freeFTYP(handle->ftyp);
              handle->ftyp = NULL;
              return MP4_ERROR;      

            case -1:
            default:
                return MP4_ERROR;
            }
        }    

    if (!handle->moov->trakAudio)
      return MP4_ERROR;
    if (!handle->moov->trakAudio->mdia)
      return MP4_ERROR;
    if (!(handle->type & MP4_TYPE_QCELP_13K))
      return MP4_ERROR;

    if(handle->qcelpStoredAsMPEGAudio)
       *qcelpStorageMode = 1;
    else
      *qcelpStorageMode = 0;

    return MP4_OK;
}

extern EXPORT_C MP4Err MP4GetVideoFrameProperties(MP4Handle apihandle,mp4_u32 startindex,mp4_u32 sizeofarray,TFrameInfoParameters* aFrameInfoArray)
{

	mp4_u32      tmptime=0;
	mp4_u32      sample=0;
	mp4_u32      entry=0;
	mp4_u32      videoSampleNumber = (startindex+sizeofarray)+1;
	mp4_u32  i;
	mp4_u32  j;
	mp4_u32  index;
	mp4_u32  actualIndex;

	MP4HandleImp handle = (MP4HandleImp)apihandle;
	
	TBool sampleSyncTableExists = ETrue;
	  if (!handle->metaDataComplete)
	      {
	      switch (metaDataAvailable(handle))
	          {
	          case 0:
	              return MP4_NOT_AVAILABLE;
	          case 1:
	              handle->cafError = KErrNone;
	              if (readMetaData(handle) < 0)
	                  {
			      	  // Reading of meta data failed, so free up any allocated memory
			  		  freeFTYP(handle->ftyp);
			  		  handle->ftyp = NULL;
					  freeMOOV(handle->moov);
					  handle->moov = NULL;
	                  
	                  if ( handle->cafError != KErrNone )
	                      {// if CAF/DRM caused the error return it instead of generic errorcode.
	                      return handle->cafError;
	                      }
	                  else
	                      {
	                      return MP4_INVALID_INPUT_STREAM;
	                      }
	                  }
	              handle->metaDataComplete = MP4TRUE;
	              break;

	          case -2:
	            // Reading of FTYP meta data failed, so free up any allocated memory
	            freeFTYP(handle->ftyp);
	            handle->ftyp = NULL;
	            return MP4_ERROR;      

	          case -1:
	          default:
	              return MP4_ERROR;
	          }
	      }	


	if (!handle->moov)
		return MP4_ERROR;

  	if (handle->moov->trakVideo)
	  	{
		if (!handle->moov->trakVideo->mdia)
		  	return MP4_ERROR;

		if (!handle->moov->trakVideo->mdia->minf)
		  	return MP4_ERROR;

		if (!handle->moov->trakVideo->mdia->minf->stbl)
		  	return MP4_ERROR;

		if (!handle->moov->trakVideo->mdia->minf->stbl->stsz)	//for size
		  	return MP4_ERROR;
		  
		if (!handle->moov->trakVideo->mdia->minf->stbl->stss)	//for type
			{
			// If sample sync table doesn't exist mark all frames as intra / random access point			
			sampleSyncTableExists = EFalse;
			}
		
		if (!handle->moov->trakVideo->mdia->minf->stbl->stts)	//For timeStamp
			return MP4_ERROR;
	  	}
	  else
		{
		return MP4_ERROR;
		}


	if (handle->moov->trakVideo->mdia->mdhd->timeScale == 0)	//For timeStamp
    {
		 return MP4_ERROR;
    }

	if((startindex+sizeofarray) > handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleCount)	//more than number of frames
	{
		 return MP4_ERROR;	
	}

	if(aFrameInfoArray == NULL)
	{
		return MP4_NO_OUTPUT_BUFFER;
	}

	if ( sampleSyncTableExists )
		{
		for (i=0; i < handle->moov->trakVideo->mdia->minf->stbl->stss->entryCount; i++)		//set all types to true
			{
			//because counting is stored from 1 but index from 0
			mp4_u32 currFrame = handle->moov->trakVideo->mdia->minf->stbl->stss->sampleNumber[i] - 1;	
			
			if ((currFrame >= startindex) && (currFrame < (startindex + sizeofarray)))
				{
				aFrameInfoArray[currFrame - startindex].iType = MP4TRUE;							
				}
			}
	  	}

    index=0;	//initialize to beginning
	actualIndex=0;//array indexer

	for(;;)
		{
		if(index< videoSampleNumber)
			{
			for(j=0;j<handle->moov->trakVideo->mdia->minf->stbl->stts->sampleCount[entry];j++)
				{
				if(index >=startindex)
					{
					//first initialize flag to false if not previously set to true.
					if(aFrameInfoArray[actualIndex].iType != MP4TRUE)
						{
						aFrameInfoArray[actualIndex].iType = MP4FALSE;
						}
					//aFrameInfoArray[index].iStartTime = tmptime + handle->moov->trakVideo->mdia->minf->stbl->stts->sampleDelta[entry];
					if(index==0)		//so first frame of entire clip
						{
						aFrameInfoArray[actualIndex].iStartTime =0;
						}
					else
						{
					  	aFrameInfoArray[actualIndex].iStartTime = (mp4_u32)((mp4_double)tmptime * (mp4_double)1000 / 
	                           (mp4_double)handle->moov->trakVideo->mdia->mdhd->timeScale + (mp4_double)0.5);
						}
					aFrameInfoArray[actualIndex].iSize = handle->moov->trakVideo->mdia->minf->stbl->stsz->entrySize[index];
					// If sample sync table doesn't exist mark all frames as intra / random access point
					if (!sampleSyncTableExists)
						{
						aFrameInfoArray[actualIndex].iType = MP4TRUE;	
						}
					actualIndex++; //point to next index in array
					}
				tmptime += handle->moov->trakVideo->mdia->minf->stbl->stts->sampleDelta[entry];
				//Now insert size before incrementing index
				if(index == videoSampleNumber-2)
				break;
				index++;
				}
			if(index==videoSampleNumber-2)
			break;
			sample += handle->moov->trakVideo->mdia->minf->stbl->stts->sampleCount[entry];
			entry++;
			}
		else
			{
			break;
			}
		}
	return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseGetVideoClipProperties(MP4Handle apihandle, TVideoClipProperties& aVideoClipProperties)
{
    MP4HandleImp handle = (MP4HandleImp)apihandle;
    if (!handle)
    	{
        return MP4_ERROR;
    	}
    
    if (!handle->metaDataComplete)
        {
        switch (metaDataAvailable(handle))
            {
            case 0:
                return MP4_NOT_AVAILABLE;
            case 1:
                handle->cafError = KErrNone;
                if (readMetaData(handle) < 0)
                    {
      				// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
                    
                    if ( handle->cafError != KErrNone )
                        {// if CAF/DRM caused the error return it instead of generic errorcode.
                        return handle->cafError;
                        }
                    else
                        {
                        return MP4_INVALID_INPUT_STREAM;
                        }
                    }
                handle->metaDataComplete = MP4TRUE;
                break;

            case -2:
              // Reading of FTYP meta data failed, so free up any allocated memory
              freeFTYP(handle->ftyp);
              handle->ftyp = NULL;
              return MP4_ERROR;      

            case -1:
            default:
                return MP4_ERROR;
            }
        }    

    if (!handle->moov)
        return MP4_ERROR;
    if (!handle->moov->trakVideo)
        return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia)
        return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia->minf)
        return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia->minf->stbl)
        return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia->minf->stbl->stsd)
        return MP4_ERROR;

    /* Assume that the video type is the same for all sample entries. Just get the video type from the first one */
    if (handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0])
    {
        if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0]->d263)
            return MP4_ERROR;

        if (handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0]->d263->h263Level)
        {
            aVideoClipProperties.iH263Level = handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0]->d263->h263Level;
            return MP4_OK;
        }
        else
        {
            return MP4_ERROR;
        }
    }
    else
    {
        return MP4_ERROR;        
    }
}

extern EXPORT_C MP4Err MP4ParseGetUserDataAtom(MP4Handle apihandle, 
                                                            mp4_u8& udtaLocation,
                                                            mp4_u32 udtaAtomType,
                                                            mp4_u8* buffer,
                                                            mp4_u32& bufferSize,
                                                            mp4_u32& atomIndex )
    {
    mp4_u32 size = 0;
    mp4_u64 largesize = 0;
    mp4_u32 type = 0;
    mp4_i32 bytesread = 0;
    mp4_i32 totalbytesread = 0;
    MP4Err retError = MP4_OK;
    userDataAtom* udta = NULL;
    mp4_bool found = MP4FALSE;
    mp4_u32 foundIndex = 0;

    MP4HandleImp handle = (MP4HandleImp)apihandle;
    
    if (!handle)
    	{
        return MP4_ERROR;
    	}
    
    if (!handle->metaDataComplete)
        {
        switch (metaDataAvailable(handle))
            {
            case 0:
                return MP4_NOT_AVAILABLE;
            case 1:
                handle->cafError = KErrNone;
                if (readMetaData(handle) < 0)
                    {
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
                    
                    if ( handle->cafError != KErrNone )
                        {// if CAF/DRM caused the error return it instead of generic errorcode.
                        return handle->cafError;
                        }
                    else
                        {
                        return MP4_INVALID_INPUT_STREAM;
                        }
                    }
                handle->metaDataComplete = MP4TRUE;
                break;

            case -2:
              // Reading of FTYP meta data failed, so free up any allocated memory
              freeFTYP(handle->ftyp);
              handle->ftyp = NULL;
              return MP4_ERROR;      

            case -1:
            default:
                return MP4_ERROR;
            }
        }    

    if (!handle->moov)
        return MP4_ERROR;

    // Check where to read udta from.
    switch ( udtaLocation )
        {
        case MP4_UDTA_NONE:
            {
            retError = MP4_UDTA_NOT_FOUND;
			break;
            }
        case MP4_UDTA_MOOV:
            {
            if ( !handle->moov->udta )
                {
                retError = MP4_UDTA_NOT_FOUND;
                break;
                }
            else
                {
                udta = handle->moov->udta;
                }
            break;
            }
        case MP4_UDTA_VIDEOTRAK:
            {
            if ( !handle->moov->trakVideo )
                {
                retError = MP4_UDTA_NOT_FOUND;
                break;
                }
            if ( !handle->moov->trakVideo->udta )
                {
                retError = MP4_UDTA_NOT_FOUND;
                }
            else
                {
                udta = handle->moov->trakVideo->udta;
                }
            break;
            }
        case MP4_UDTA_AUDIOTRAK:
            {
            if (!handle->moov->trakAudio)
                {
                retError = MP4_UDTA_NOT_FOUND;
                break;
                }
            if ( !handle->moov->trakAudio->udta )
                {
                retError = MP4_UDTA_NOT_FOUND;
                }
            else
                {
                udta = handle->moov->trakAudio->udta;
                }
            break;
            }
       default:
            {
            retError = MP4_INVALID_TYPE;
            }
        }

    if ( retError == MP4_OK )   // valid UDTA found.
        {
        if ( !udta->atomhdr )
            {
            retError = MP4_UDTA_NOT_FOUND;
            }
        if ( !udta->atomcontentloc || (udta->atomcontentsize == 0) )
            {
            retError = MP4_UDTA_NOT_FOUND;
            }

        // seek to UDTA atom in memory structure or file.
        if (handle->file) /* Input is in a file */
            {
            if (seekFileAbs(handle, udta->atomcontentloc) != 0)
                return MP4_CANT_SEEK;
            }
        else
            {
            handle->absPosition = udta->atomcontentloc;
            }
        
        // search for wanted atom from UDTA and read it to buffer
        while ( totalbytesread < (mp4_i32)udta->atomcontentsize )
            {
            if (peekData(handle, handle->buf, 16) < 0)
                return MP4_ERROR;

            size = u32endian(*((mp4_u32 *)handle->buf));
            type = u32endian(*((mp4_u32 *)(handle->buf+4)));
        
            if ( type == udtaAtomType )
                {
                if ( atomIndex == foundIndex )
                    {
                    if ( size == 1 )
                        {
                        largesize = u64endian(*((mp4_u64*)(handle->buf+8)));
                        size = (mp4_u32)I64INT(largesize);
                        }
                    if ( size > bufferSize )
                        {
                        // Although the output buffer supplied by the caller is 
                        // not large enough to store the sub user atom's content, keep 
                        // parsing thru the whole user data to retrieve the highest index
                        // to be returned thru the atomIndex parameter
                        bufferSize = size;
                        retError = MP4_OUTPUT_BUFFER_TOO_SMALL;
                        bytesread = readUnknown(handle);
	                	if (bytesread < 0)
	                		return MP4_ERROR;
						}
                    else
                    	{
	                    bytesread = readData(handle, buffer, size);
	                    if (bytesread < 0)
	                        return MP4_ERROR;
	                    bufferSize = bytesread;                    		
                    	}
                    	
                    totalbytesread += bytesread;
                    foundIndex += 1;
                    found = MP4TRUE;
                    }
                else
                    {
                	bytesread = readUnknown(handle);
                	if (bytesread < 0)
                		return MP4_ERROR;
                	totalbytesread += bytesread;
                	foundIndex += 1;
                    }
                }
            else
                {
                bytesread = readUnknown(handle);
                if (bytesread < 0)
                    return MP4_ERROR;
                totalbytesread += bytesread;
                }
            }
        }

    if ( ( atomIndex > foundIndex ) || !found )
        {
        retError = MP4_UDTA_NOT_FOUND;
        }

    // fill how many wanted type atom there is in asked UDTA.
    if ( found )
        {
        atomIndex = foundIndex - 1;
        }
    else
        {
        atomIndex = 0;
        }

    // fill udtaLocation
    udtaLocation = MP4_UDTA_NONE;

    if ( handle->moov->udta )
        {
        udtaLocation |= MP4_UDTA_MOOV;
        }
    if ( handle->moov->trakVideo )
        {
        if ( handle->moov->trakVideo->udta )
            {
            udtaLocation |= MP4_UDTA_VIDEOTRAK;
            }
        }
    if ( handle->moov->trakAudio )
        {
        if ( handle->moov->trakAudio->udta )
            {
            udtaLocation |= MP4_UDTA_AUDIOTRAK;
            }
        }

    return retError;
    }

extern EXPORT_C MP4Err MP4ParseNextVideoFrameDependencies(MP4Handle apihandle, mp4_u8* aDependsOn, mp4_u8* aIsDependentOn, mp4_u8* aHasRedundancy)
{
	MP4HandleImp handle = (MP4HandleImp)apihandle;
	mp4_u32 type = 0;
	
	if (!handle)
	    {
		return MP4_ERROR;
	    }
	if(!aDependsOn || !aIsDependentOn || !aHasRedundancy)
	    {
		return MP4_NO_OUTPUT_BUFFER;
	    }
	if (!handle->metaDataComplete)
	{
		switch (metaDataAvailable(handle))
		{
		case 0:
			return MP4_NOT_AVAILABLE;

		case 1:
			if (readMetaData(handle) < 0)
				{
		      	// Reading of meta data failed, so free up any allocated memory
		  		freeFTYP(handle->ftyp);
		  		handle->ftyp = NULL;
				freeMOOV(handle->moov);
				handle->moov = NULL;
				
				return MP4_INVALID_INPUT_STREAM;
				}

			handle->metaDataComplete = MP4TRUE;
			break;

	    case -2:
	      // Reading of FTYP meta data failed, so free up any allocated memory
	      freeFTYP(handle->ftyp);
	      handle->ftyp = NULL;
	      return MP4_ERROR;      

		case -1:
		default:
			return MP4_ERROR;
		}
	}
    
    if (determineVideoType(handle, &type) < 0)
        {
        return MP4_ERROR;
        }
    if( !isAvcVideo(type) )
        {
        *aDependsOn = 0; // Unknown
        *aIsDependentOn = 0; // Unknown
        *aHasRedundancy = 0; // Unknown
        return MP4_OK;
        }	

    if (!handle->moov)
        return MP4_ERROR;
    if (!handle->moov->trakVideo)
        return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia)
        return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia->minf)
        return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia->minf->stbl)
        return MP4_ERROR;
    if (!handle->moov->trakVideo->mdia->minf->stbl->sdtp)
        {
        *aDependsOn = 0; // Unknown
        *aIsDependentOn = 0; // Unknown
        *aHasRedundancy = 0; // Unknown
        return MP4_OK;
        }
    if (!handle->moov->trakVideo->mdia->minf->stbl->sdtp->dep)
        {
        return MP4_ERROR;
        }
        
    if (handle->videoSampleNum <= 0)    
    	{
    	return MP4_ERROR;	
    	}
        
    if(handle->moov->trakVideo->mdia->minf->stbl->sdtp->dep[handle->videoSampleNum - 1].sDependsOn > 2 ||
       handle->moov->trakVideo->mdia->minf->stbl->sdtp->dep[handle->videoSampleNum - 1].sIsDependentOn > 2 ||
       handle->moov->trakVideo->mdia->minf->stbl->sdtp->dep[handle->videoSampleNum - 1].sHasRedundancy > 2)
        {
    	return MP4_ERROR;
        }
    *aDependsOn = handle->moov->trakVideo->mdia->minf->stbl->sdtp->dep[handle->videoSampleNum - 1].sDependsOn;
    *aIsDependentOn = handle->moov->trakVideo->mdia->minf->stbl->sdtp->dep[handle->videoSampleNum - 1].sIsDependentOn;
    *aHasRedundancy = handle->moov->trakVideo->mdia->minf->stbl->sdtp->dep[handle->videoSampleNum - 1].sHasRedundancy;
    return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseReadAudioFramesAsync(MP4Handle apihandle, M3GPMP4LibAsyncObserver* aObserver, mp4_u8 *buffer, mp4_u32* buffersize)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  
	if (!handle->file)
		{
		// async operation is only supported for files.
		return MP4_FILE_ERROR;
		}	

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:
      return MP4_NOT_AVAILABLE;
    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }
      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      

    case -1:
    default:
      return MP4_ERROR;
    }
  }
  
  handle->asyncObserver = aObserver;

  /* Is audio available? */
  if (!((handle->type & MP4_TYPE_MPEG4_AUDIO) ||
        (handle->type & MP4_TYPE_AMR_NB) ||
        (handle->type & MP4_TYPE_AMR_WB) ||
        (handle->type & MP4_TYPE_QCELP_13K)))
    return MP4_ERROR;

  /* Are there samples left? */
  if (handle->audioLast)
    return MP4_NO_FRAME;

  if (!handle->moov)
    return MP4_ERROR;
  if (!handle->moov->trakAudio)
    return MP4_ERROR;
  
  switch (fetchAudioSampleAsync(handle, handle->moov->trakAudio, buffer, buffersize))
  {
  case -1:
    return MP4_ERROR;
  case -2:
    return MP4_BUFFER_TOO_SMALL;
  case -4:
    return MP4_INVALID_INPUT_STREAM;
  case MP4_OUT_OF_MEMORY:
    return MP4_OUT_OF_MEMORY;
  default:
    break;
  }  
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ParseReadVideoFrameAsync(MP4Handle apihandle,
 																M3GPMP4LibAsyncObserver* aObserver, 		
                            							       	mp4_u8* buffer,
                                   								mp4_u32* buffersize)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  
	if (!handle->file)
		{
		// async operation is only supported for files.
		return MP4_FILE_ERROR;
		}

  if (!handle->metaDataComplete)
  {
    switch (metaDataAvailable(handle))
    {
    case 0:

      return MP4_NOT_AVAILABLE;

    case 1:
      handle->cafError = KErrNone;
      if (readMetaData(handle) < 0)
      {
      	// Reading of meta data failed, so free up any allocated memory
  		freeFTYP(handle->ftyp);
  		handle->ftyp = NULL;
		freeMOOV(handle->moov);
		handle->moov = NULL;
      
      	if ( handle->cafError != KErrNone )
      	{
      	  // if CAF/DRM caused the error return it instead of generic errorcode.
      	  return handle->cafError;
      	}
      	else
      	{
	      return MP4_INVALID_INPUT_STREAM;
      	}
      }
      handle->metaDataComplete = MP4TRUE;
      break;

    case -2:
      // Reading of FTYP meta data failed, so free up any allocated memory
      freeFTYP(handle->ftyp);
      handle->ftyp = NULL;
      return MP4_ERROR;      

    case -1:
    default:
      return MP4_ERROR;
    }
  }

  /* Is video available? */
  if (!((handle->type & MP4_TYPE_MPEG4_VIDEO) ||
        (handle->type & MP4_TYPE_H263_PROFILE_0) ||
        (handle->type & MP4_TYPE_H263_PROFILE_3) ||
		containsAvcVideo( handle->type ) ))
    return MP4_ERROR;

  /* Are there samples left? */
  if (handle->videoLast)
    return MP4_NO_FRAME;
  
  handle->asyncObserver = aObserver;  

  if (!handle->moov)
    return MP4_ERROR;
  if (!handle->moov->trakVideo)
    return MP4_ERROR;

  switch (fetchVideoFrameAsync(handle, handle->moov->trakVideo, buffer, buffersize ))
  {
  case -1:
    return MP4_ERROR;
  case -2:
    return MP4_BUFFER_TOO_SMALL;
  case -3:
    return MP4_NOT_AVAILABLE;
  case -4:
    return MP4_INVALID_INPUT_STREAM;
  case MP4_OUT_OF_MEMORY:
    return MP4_OUT_OF_MEMORY;
  default:
    break;
  }

  return MP4_OK;
}

extern EXPORT_C void MP4CancelReadFrame(MP4Handle ahandle)
	{
	MP4HandleImp handle = (MP4HandleImp)ahandle;
	
	if (handle->asyncReader)
		{
		handle->asyncReader->Cancel();
		}
	}

extern EXPORT_C MP4Err MP4ParseGetID32Location(MP4Handle apihandle, mp4_u32& location)
    { 
    MP4HandleImp handle = (MP4HandleImp)apihandle;

    if (!handle)
        {
        return MP4_ERROR;
        }

    if (!handle->metaDataComplete)
        {
        switch (metaDataAvailable(handle))
            {
            case 0:
                return MP4_NOT_AVAILABLE;
            case 1:
                handle->cafError = KErrNone;
                if (readMetaData(handle) < 0)
                    {
			      	// Reading of meta data failed, so free up any allocated memory
			  		freeFTYP(handle->ftyp);
			  		handle->ftyp = NULL;
					freeMOOV(handle->moov);
					handle->moov = NULL;
                    if (handle->cafError != KErrNone)
                        {// if CAF/DRM caused the error return it instead of generic errorcode.
                        return handle->cafError;
                        }
                    else
                        {
                        return MP4_INVALID_INPUT_STREAM;
                        }
                    }
                handle->metaDataComplete = MP4TRUE;
                break;

		    case -2:
                // Reading of FTYP meta data failed, so free up any allocated memory
                freeFTYP(handle->ftyp);
                handle->ftyp = NULL;
                return MP4_ERROR;
		      
            case -1:
            default:
                return MP4_ERROR;
            }
        }    

    if (!handle->moov)
        return MP4_ERROR;

    metaAtom* meta = handle->moov->meta;
    if (!meta)
        return MP4_NOT_AVAILABLE;

    if (!meta->atomhdr)
        return MP4_NOT_AVAILABLE;
    
    if(meta->ID32)
        {
        location = meta->ID32->atomcontentloc;     
        }
    else
        {
        return MP4_NOT_AVAILABLE;
        }    

    return MP4_OK;
    }
// End of File
