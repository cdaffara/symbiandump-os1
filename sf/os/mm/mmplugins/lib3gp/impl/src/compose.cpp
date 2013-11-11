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
#include "mp4endian.h"
#include "mp4compose.h"
#include "mp4currenttime.h"
#include "mp4utils.h"

#define MP4_INT_MAX		KMaxTInt32	
#define MDAT_HEADER_SIZE 16

// MACROS
// Debug print macro
#ifdef _DEBUG
#include <e32svr.h>
#define PRINT(x)
#else
#define PRINT(x)
#endif


inline void updateChunkOffset(sampleTable *st, mp4_i32 index, mp4_i64 value) 
{
  if (value > MP4_INT_MAX) 
    st->stcoNeed64Bits = ETrue; 
  
  st->stcoChunkOffset[index] = value;
}

/* must be called after determineAudioTrakMetaDataSize and determineVideoTrakMetaDataSize */
size_t mvhdAtomSize(MP4HandleImp handle)
{
  if (handle->videoDuration > MP4_INT_MAX || handle->audioDuration > MP4_INT_MAX)
  {
    return 120;
  }
  else
  {
    return 108;
  }
}



/* helper functions */
mp4_i32 formatMdatHeader(mp4_u8 *buffer, mp4_i64 size);

/*
 * Function:
 *
 *   mp4_i32 updateVideoMetaData(MP4HandleImp handle,
 *                               mp4_u32 size,
 *                               mp4_u32 duration)
 *
 * Description:
 *
 *   This function updates sample table atom data.
 *
 *   One call of this function will generate one chunk in the MP4 file.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   size      Size of video frame to insert
 *   duration  Duration of the video frame (in media timescale)
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 updateVideoMetaData(MP4HandleImp handle, mp4_u32 size, mp4_u32 duration, mp4_bool keyframe)
{
  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (handle->metaDataBlocks == BLOCK_LIMIT)
    {
      /* Write metadata to temporary files */

      if (writeMetaDataTmp(handle) < 0)
        return -1;
    }

    handle->metaDataBlocks++;
  }

  handle->videoSampleTable->currentChunk++;

  if (updateDecodingTimeToSample(handle, handle->videoSampleTable, duration) < 0)
    return -1;

  if (updateSampleSize(handle, handle->videoSampleTable, size) < 0)
    return -1;

  if (updateSampleToChunk(handle->videoSampleTable) < 0)
    return -1;

  if (updateChunkOffset(handle, handle->videoSampleTable) < 0)
    return -1;

  if (keyframe)
    if (updateSyncSample(handle, handle->videoSampleTable) < 0)
      return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 updateAudioMetaData(MP4HandleImp handle,
 *                               mp4_u32 size,
 *                               mp4_u32 numberofframes)
 *
 * Description:
 *
 *   This function updates sample table atom data.
 *
 *   One call of this function will generate one chunk in the MP4 file.
 *
 * Parameters:
 *
 *   handle          MP4 library handle
 *   size            Size of video frame to insert
 *   duration        Duration of audio frames (in timescale,
 *                   see MP4ComposeAddAudioDescription)
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 updateAudioMetaData(MP4HandleImp handle, mp4_u32 size, mp4_u32 duration)
{
  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (handle->metaDataBlocks == BLOCK_LIMIT)
    {
      /* Write metadata to temporary files */

      if (writeMetaDataTmp(handle) < 0)
        return -1;
    }

    handle->metaDataBlocks++;
  }

  handle->audioSampleTable->currentChunk++;

  if (updateDecodingTimeToSample(handle, handle->audioSampleTable, duration) < 0)
    return -1;

  if (updateSampleSize(handle, handle->audioSampleTable, size) < 0)
    return -1;

  if (updateSampleToChunk(handle->audioSampleTable) < 0)
    return -1;

  if (updateChunkOffset(handle, handle->audioSampleTable) < 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeFTYPAndMDATToFile(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function writes FTYP box to a file. In addition, it writes MDAT box
 *   size and type to a file. The function is used when meta data is put to
 *   the end of file.
 *
 * Parameters:
 *
 *   handle     MP4 library handle
 *
 * Return value:
 *
 *   0          Success
 *   -1         Error
 *
 */
mp4_i32 writeFTYPAndMDATToFile(MP4HandleImp handle)
{
  mp4_u8  buf[32];
  mp4_u32 i = 0;


  if (writeFTYP(handle) < 0)
    return -1;

  handle->ftypWritten = MP4TRUE;


  i = formatMdatHeader(buf, (mp4_u32)0);
  if (writeFile(handle, buf, i) < 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeDataToFile(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function writes meta and media data to a file.
 *
 * Parameters:
 *
 *   handle     MP4 library handle
 *
 * Return value:
 *
 *   0          Success
 *   -1         Error
 *
 */
mp4_i32 writeDataToFile(MP4HandleImp handle)
{
  PRINT((_L("e_writedatatofile 1")));
  mp4_u32   metaDataSize = 0;
  trakSize  *audioTrackSize;
  trakSize  *videoTrackSize;
  mp4_bool  haveAudio = MP4FALSE;
  mp4_bool  haveVideo = MP4FALSE;
  mp4_u8 ftypdelta = 0;


  if ((handle->type & MP4_TYPE_AMR_NB) ||
      (handle->type & MP4_TYPE_AMR_WB) ||
       (handle->type & MP4_TYPE_QCELP_13K) ||     
      (handle->type & MP4_TYPE_MPEG4_AUDIO))
    haveAudio = MP4TRUE;

  if ((handle->type & MP4_TYPE_H263_PROFILE_0) ||
      (handle->type & MP4_TYPE_H263_PROFILE_3) ||
      (handle->type & MP4_TYPE_MPEG4_VIDEO) ||
	  containsAvcVideo( handle->type ) )
    haveVideo = MP4TRUE;

  if ((handle->generate3G2 && !(handle->type &  MP4_TYPE_QCELP_13K)) ||
       (!handle->generate3G2 && !(handle->type &  MP4_TYPE_AMR_WB)))
     ftypdelta = 4; /* one more additional compatible brand */
  else
     ftypdelta = 0;
  
  if( containsAvcVideo( handle->type ) )
  {
    ftypdelta += 4;
  }  

  PRINT((_L("e_writedatatofile_alloc_audiotrk 1")));
  audioTrackSize = (trakSize *)mp4malloc(sizeof(trakSize));
  if (audioTrackSize == NULL)
    return -1;
  PRINT((_L("e_writedatatofile_alloc_audiotrk 0")));    
	
  PRINT((_L("e_writedatatofile_alloc_videotrk 1")));  
  videoTrackSize = (trakSize *)mp4malloc(sizeof(trakSize));
  if (videoTrackSize == NULL)
  {
    mp4free(audioTrackSize);

    return -1;
  }
  PRINT((_L("e_writedatatofile_alloc_videotrk 0")));  

  if (haveAudio)
  {
	PRINT((_L("e_writedatatofile_deter_audiotrk_metadatasize 1")));  
    if (determineAudioTrakMetaDataSize(handle, handle->audioSampleTable, audioTrackSize) < 0)
        {
        mp4free(audioTrackSize);
        mp4free(videoTrackSize);
        return -1;
        }
	PRINT((_L("e_writedatatofile_deter_audiotrk_metadatasize 0")));          
  }

  if (haveVideo)
  {
	PRINT((_L("e_writedatatofile_deter_videotrk_metadatasize 1")));
    if (determineVideoTrakMetaDataSize(handle, handle->videoSampleTable, videoTrackSize) < 0)
        {
        mp4free(audioTrackSize);
        mp4free(videoTrackSize);
        return -1;
        }
PRINT((_L("e_writedatatofile_deter_videotrk_metadatasize 0")));        
  }

  if (handle->flags & MP4_FLAG_METADATALAST)
  {
    metaDataSize += (FTYP_SIZE + ftypdelta);             /* ftyp */
    handle->metaDataSize = metaDataSize;
  }
  else
  {
    metaDataSize += (FTYP_SIZE + ftypdelta);             /* ftyp */
    metaDataSize += 8;                     /* moov atomheader */
    metaDataSize += mvhdAtomSize(handle);                   /* mvhd */
    if (handle->moovUDTA)
        {
        metaDataSize += 8 + (mp4_u32)handle->moovUDTA->atomcontentsize;
        }
    metaDataSize += audioTrackSize->trak;  /* Audio trak */
    metaDataSize += videoTrackSize->trak;  /* Video trak */

    handle->metaDataSize = metaDataSize;
  }


  if (!(handle->flags & MP4_FLAG_LONGCLIP))
  {
    /* Update metadata pointers only if metadata is in memory */

    if (haveAudio)
    {
	PRINT((_L("e_writedatatofile_reupdata_audiometadata 1")));     
      if (reUpdateAudioMetaData(handle->audioSampleTable, metaDataSize) < 0)
      {
        mp4free(audioTrackSize);
        mp4free(videoTrackSize);

        return -1;
      }
	PRINT((_L("e_writedatatofile_reupdata_audiometadata 0")));           
    }

    if (haveVideo)
    {
	PRINT((_L("e_writedatatofile_reupdata_videometadata 1")));               
      if (reUpdateVideoMetaData(handle->videoSampleTable, metaDataSize) < 0)
      {
        mp4free(audioTrackSize);
        mp4free(videoTrackSize);

        return -1;
      }
	PRINT((_L("e_writedatatofile_reupdata_videometadata 0")));      
    }
  }
  else
  {
    /* Write the rest of metadata to temporary files */
	PRINT((_L("e_writedatatofile_write_metadatablocks 1")));
    if (handle->metaDataBlocks)
      if (writeMetaDataTmp(handle) < 0)
          {
          mp4free(audioTrackSize);
          mp4free(videoTrackSize);
          return -1;
          }
	PRINT((_L("e_writedatatofile_write_metadatablocks 0")));          
  }


  if (handle->flags & MP4_FLAG_METADATALAST)
  {
    mp4_u8  buf[16];
    mp4_u32 moovSize = 0;

    moovSize += 8;                     /* moov atomheader */
    moovSize += mvhdAtomSize(handle);                   /* mvhd */
    moovSize += audioTrackSize->trak;  /* Audio trak */
    moovSize += videoTrackSize->trak;  /* Video trak */
    if (handle->moovUDTA)
        {
        moovSize += 8 + handle->moovUDTA->atomcontentsize;
        }    
        
	PRINT((_L("e_writedatatofile_write_moov 1")));
    if (writeMOOV(handle, moovSize, haveAudio, haveVideo, audioTrackSize, videoTrackSize) < 0)
    {
      mp4free(audioTrackSize);
      mp4free(videoTrackSize);

      return -1;
    }
    PRINT((_L("e_writedatatofile_write_moov 0")));

    /* Overwrite media data size */
	PRINT((_L("e_writedatatofile_update_moov_media_size 1")));
	if(!handle->bufferWrite)
	{
		if (seekFileAbsWrite(handle, (FTYP_SIZE + ftypdelta)) != 0)
			{
			mp4free(audioTrackSize);
			mp4free(videoTrackSize);
			return -1;
			}
	}
	
	//make sure the buf is large enough to hold the mdat header
	TInt i;
	i = formatMdatHeader(buf, handle->mediaDataBytes);
    if (writeFileUnbuffered(handle, buf, i) < 0)
        {
        mp4free(audioTrackSize);
        mp4free(videoTrackSize);
        return -1;
        }
	PRINT((_L("e_writedatatofile_update_moov_media_size 0")));        
  }
  else
  {
	PRINT((_L("e_writedatatofile_write_ftyp 1")));  
    if (writeFTYP(handle) < 0)
    {
      mp4free(audioTrackSize);
      mp4free(videoTrackSize);

      return -1;
    }
    PRINT((_L("e_writedatatofile_write_ftyp 0")));  

	PRINT((_L("e_writedatatofile_write_new_moov 1")));  
    if (writeMOOV(handle, metaDataSize - (FTYP_SIZE + ftypdelta), haveAudio, haveVideo, audioTrackSize, videoTrackSize) < 0)
    {
      mp4free(audioTrackSize);
      mp4free(videoTrackSize);

      return -1;
    }
    PRINT((_L("e_writedatatofile_write_new_moov 0")));

	PRINT((_L("e_writedatatofile_write_new_mdia 1")));
    if (writeMediaData(handle) < 0)
    {
      mp4free(audioTrackSize);
      mp4free(videoTrackSize);

      return -1;
    }
    PRINT((_L("e_writedatatofile_write_new_mdia 0")));
  }

PRINT((_L("e_writedatatofile_free_audioandvideotrks 1")));
  mp4free(audioTrackSize);
  mp4free(videoTrackSize);
PRINT((_L("e_writedatatofile_free_audioandvideotrks 0")));
PRINT((_L("e_writedatatofile 0")));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 updateDecodingTimeToSample(MP4HandleImp handle,
 *                                      sampleTable *st,
 *                                      mp4_u32 duration)
 *
 * Description:
 *
 *   This function updates stts atom data.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   st        sampleTable
 *   duration  Duration of sample to insert (in media timescale)
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 updateDecodingTimeToSample(MP4HandleImp handle, sampleTable *st, mp4_u32 duration)
{
  if (!handle)
    return -1;

  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (st->sttsCurrentEntryCount == 0)
    {
      st->sttsSampleCount[st->sttsCurrentEntryCount] = 1;
      st->sttsSampleDelta[st->sttsCurrentEntryCount] = duration;

      st->sttsCurrentEntryCount++;
      st->sttsEntryCount++;

      return 0;
    }

    if (st->sttsCurrentEntryCount == st->sttsMaxEntryCount)
    {
      void *p;

      p = mp4realloc(st->sttsSampleCount,
                     2 * sizeof(mp4_u32) * st->sttsMaxEntryCount,
                     sizeof(mp4_u32) * st->sttsMaxEntryCount);
      if (p == NULL)
        return -1;

      st->sttsSampleCount = (mp4_u32 *)p;

      p = mp4realloc(st->sttsSampleDelta,
                     2 * sizeof(mp4_u32) * st->sttsMaxEntryCount,
                     sizeof(mp4_u32) * st->sttsMaxEntryCount);
      if (p == NULL)
        return -1;

      st->sttsSampleDelta = (mp4_u32 *)p;

      st->sttsMaxEntryCount *= 2;
    }

    if (st->sttsSampleDelta[st->sttsCurrentEntryCount - 1] == duration)
    {
      st->sttsSampleCount[st->sttsCurrentEntryCount - 1]++;
    }
    else
    {
      st->sttsSampleCount[st->sttsCurrentEntryCount] = 1;
      st->sttsSampleDelta[st->sttsCurrentEntryCount] = duration;

      st->sttsCurrentEntryCount++;
      st->sttsEntryCount++;
    }
  }
  else
  {
    if (st->sttsEntryCount == 0)
    {
      st->sttsSampleCount[st->sttsEntryCount]++;
      st->sttsSampleDelta[st->sttsEntryCount] = duration;

      st->sttsEntryCount++;

      return 0;
    }

    if (st->sttsEntryCount == st->sttsMaxEntryCount)
    {
      void *p;

      p = mp4realloc(st->sttsSampleCount,
                     2 * sizeof(mp4_u32) * st->sttsMaxEntryCount,
                     sizeof(mp4_u32) * st->sttsMaxEntryCount);
      if (p == NULL)
        return -1;

      st->sttsSampleCount = (mp4_u32 *)p;

      p = mp4realloc(st->sttsSampleDelta,
                     2 * sizeof(mp4_u32) * st->sttsMaxEntryCount,
                     sizeof(mp4_u32) * st->sttsMaxEntryCount);
      if (p == NULL)
        return -1;

      st->sttsSampleDelta = (mp4_u32 *)p;

      st->sttsMaxEntryCount *= 2;
    }

    if (st->sttsSampleDelta[st->sttsEntryCount - 1] == duration)
    {
      st->sttsSampleCount[st->sttsEntryCount - 1]++;
    }
    else
    {
      st->sttsSampleCount[st->sttsEntryCount] = 1;
      st->sttsSampleDelta[st->sttsEntryCount] = duration;

      st->sttsEntryCount++;
    }
  }


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 updateSampleSize(MP4HandleImp handle,
 *                            sampleTable *st,
 *                            mp4_u32 size)
 *
 * Description:
 *
 *   This function updates stsz atom data.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   st        sampleTable
 *   size      Size of sample in bytes
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 updateSampleSize(MP4HandleImp handle, sampleTable *st, mp4_u32 size)
{
  if (!handle)
    return -1;

  if (size == 0)
    return -1;

  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (st->stszCurrentSampleCount == st->stszMaxSampleCount)
    {
      void *p;

      p = mp4realloc(st->stszEntrySize,
                     2 * sizeof(mp4_u32) * st->stszMaxSampleCount,
                     sizeof(mp4_u32) * st->stszMaxSampleCount);
      if (p == NULL)
        return -1;

      st->stszEntrySize = (mp4_u32 *)p;

      st->stszMaxSampleCount *= 2;
    }

    st->stszEntrySize[st->stszCurrentSampleCount] = size;

    st->stszCurrentSampleCount++;
    st->stszSampleCount++;
  }
  else
  {
    if (st->stszSampleCount == st->stszMaxSampleCount)
    {
      void *p;

      p = mp4realloc(st->stszEntrySize,
                     2 * sizeof(mp4_u32) * st->stszMaxSampleCount,
                     sizeof(mp4_u32) * st->stszMaxSampleCount);
      if (p == NULL)
        return -1;

      st->stszEntrySize = (mp4_u32 *)p;

      st->stszMaxSampleCount *= 2;
    }

    st->stszEntrySize[st->stszSampleCount] = size;

    st->stszSampleCount++;
  }


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 updateSampleToChunk(sampleTable *st)
 *
 * Description:
 *
 *   This function updates stsc atom data.
 *
 * Parameters:
 *
 *   st        sampleTable
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 updateSampleToChunk(sampleTable *st)
{
  if (st->stscEntryCount != 0)
    return 0;


  st->stscFirstChunk[st->stscEntryCount] = st->currentChunk;
  st->stscSamplesPerChunk[st->stscEntryCount] = 1;
  st->stscSampleDescriptionIndex[st->stscEntryCount] = 1;  /* Note: Need to update here for multiple sample entry support */
  st->stscEntryCount++;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 updateChunkOffset(MP4HandleImp handle,
 *                             sampleTable *st)
 *
 * Description:
 *
 *   This function updates stco atom data.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   st        sampleTable
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 updateChunkOffset(MP4HandleImp handle, sampleTable *st)
{
  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (st->stcoCurrentEntryCount == st->stcoMaxEntryCount)
    {
      void *p;

      p = mp4realloc(st->stcoChunkOffset,
                     2 * sizeof(mp4_u64) * st->stcoMaxEntryCount,
                     sizeof(mp4_u64) * st->stcoMaxEntryCount);
      if (p == NULL)
        return -1;

      st->stcoChunkOffset = (mp4_u64*)p;

      st->stcoMaxEntryCount *= 2;
    }
    
    if (handle->flags & MP4_FLAG_METADATALAST)
      updateChunkOffset(st, st->stcoCurrentEntryCount, handle->mediaDataBytes);
    else
      updateChunkOffset(st, st->stcoCurrentEntryCount, handle->bytesInTmpFile);

    st->stcoCurrentEntryCount++;
    st->stcoEntryCount++;
  }
  else
  {
    if (st->stcoEntryCount == st->stcoMaxEntryCount)
    {
      void *p;

      p = mp4realloc(st->stcoChunkOffset,
                     2 * sizeof(mp4_u64) * st->stcoMaxEntryCount,
                     sizeof(mp4_u64) * st->stcoMaxEntryCount);
      if (p == NULL)
        return -1;

      st->stcoChunkOffset = (mp4_u64 *)p;

      st->stcoMaxEntryCount *= 2;
    }

    if (handle->flags & MP4_FLAG_METADATALAST)
      updateChunkOffset(st, st->stcoEntryCount, handle->mediaDataBytes);
    else
      updateChunkOffset(st, st->stcoEntryCount, handle->bytesInTmpFile);

    st->stcoEntryCount++;
  }


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 updateSyncSample(MP4HandleImp handle,
 *                            sampleTable *st)
 *
 * Description:
 *
 *   This function updates stss atom data.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   st        sampleTable
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 updateSyncSample(MP4HandleImp handle, sampleTable *st)
{
  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (st->stssCurrentEntryCount == st->stssMaxEntryCount)
    {
      void *p;

      p = mp4realloc(st->stssSampleNumber,
                     2 * sizeof(mp4_u32) * st->stssMaxEntryCount,
                     sizeof(mp4_u32) * st->stssMaxEntryCount);
      if (p == NULL)
        return -1;

      st->stssSampleNumber = (mp4_u32 *)p;

      st->stssMaxEntryCount *= 2;
    }

    st->stssSampleNumber[st->stssCurrentEntryCount] = handle->videoSampleNum;
    st->stssCurrentEntryCount++;
    st->stssEntryCount++;
  }
  else
  {
    if (st->stssEntryCount == st->stssMaxEntryCount)
    {
      void *p;

      p = mp4realloc(st->stssSampleNumber,
                     2 * sizeof(mp4_u32) * st->stssMaxEntryCount,
                     sizeof(mp4_u32) * st->stssMaxEntryCount);
      if (p == NULL)
        return -1;

      st->stssSampleNumber = (mp4_u32 *)p;

      st->stssMaxEntryCount *= 2;
    }

    st->stssSampleNumber[st->stssEntryCount] = handle->videoSampleNum;
    st->stssEntryCount++;
  }


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineAudioTrakMetaDataSize(MP4HandleImp handle,
 *                                          sampleTable *st,
 *                                          trakSize *ts)
 *
 * Description:
 *
 *   This function calculates the audio track meta data size.
 *
 * Parameters:
 *
 *   handle      MP4 library handle
 *   st          Sample table data
 *   ts          Atom sizes are returned here
 *
 * Return value:
 *
 *   0           Success
 *
 */
mp4_i32 determineAudioTrakMetaDataSize(MP4HandleImp handle, sampleTable *st, trakSize *ts)
{
  if (handle->type & MP4_TYPE_AMR_NB) /* AMR-NB */
  {
    ts->damr = 17;
    ts->samr = 36 + ts->damr;
    ts->stsd = 16 + ts->samr;
  }
  else if (handle->type & MP4_TYPE_AMR_WB) /* AMR-WB */
  {
    ts->damr = 17;
    ts->sawb = 36 + ts->damr;
    ts->stsd = 16 + ts->sawb;
  }
  else if ((handle->type & MP4_TYPE_QCELP_13K) && (!handle->qcelpStoredAsMPEGAudio)) /* QCELP 13K stored in QCELPSampleEntry */
  {
    ts->dqcp = 14;
    ts->sqcp = 36 + ts->dqcp;
    ts->stsd = 16 + ts->sqcp;
  } 
  else if ((handle->type & MP4_TYPE_QCELP_13K) && (handle->qcelpStoredAsMPEGAudio)) /* QCELP 13K stored in MP4AudioDescription */
  {
    calculateES_DescriptorSize(handle, MP4_TYPE_QCELP_13K);
    ts->esds = 12 + handle->ES_DescriptorSize; /*37 + handle->audioDecSpecificInfoSize;*/
    ts->mp4a = 36 + ts->esds;
    ts->stsd = 16 + ts->mp4a;
  }
  else /* MPEG audio */
  {
    calculateES_DescriptorSize(handle, MP4_TYPE_MPEG4_AUDIO);
    ts->esds = 12 + handle->ES_DescriptorSize; /*37 + handle->audioDecSpecificInfoSize;*/
    ts->mp4a = 36 + ts->esds;
    ts->stsd = 16 + ts->mp4a;
  }
  ts->stts = 16 + st->sttsEntryCount * 8;
  ts->stsc = 16 + st->stscEntryCount * 12;
  if (st->stszSampleSize != 0)
    ts->stsz = 20;
  else
    ts->stsz = 20 + st->stszSampleCount * 4;
  ts->stco = 16 + st->stcoEntryCount * (st->stcoNeed64Bits ? 8 : 4);
  ts->stbl = 8 + ts->stsd + ts->stts + ts->stsc + ts->stsz + ts->stco;
  ts->dref = 28;
  ts->dinf = 8 + ts->dref;
  ts->smhd = 16;
  ts->minf = 8 + ts->smhd + ts->dinf + ts->stbl;
  ts->hdlr = 33;

  if (handle->audioDuration > MP4_INT_MAX)
  {
    ts->mdhd = 44;
    ts->tkhd = 104;
  }
  else
  {
    ts->mdhd = 32;
    ts->tkhd = 92;
  }

  ts->mdia = 8 + ts->mdhd + ts->hdlr + ts->minf;
  if ( handle->audioUDTA )
      {
      ts->udta = 8 + handle->audioUDTA->atomcontentsize;
      }
  ts->trak = 8 + ts->tkhd + ts->mdia + ts->udta;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineVideoTrakMetaDataSize(MP4HandleImp handle,
 *                                          sampleTable *st,
 *                                          trakSize *ts)
 *
 * Description:
 *
 *   This function calculates the video track meta data size.
 *
 * Parameters:
 *
 *   handle      MP4 library handle
 *   st          Sample table data
 *   ts          Atom sizes are returned here
 *
 * Return value:
 *
 *   0           Success
 *
 */
mp4_i32 determineVideoTrakMetaDataSize(MP4HandleImp handle, sampleTable *st, trakSize *ts)
{
  /* Note: This functions assumes single sample entry per media track. 
	   If necessary, modify to support multiple sample entries in the future. */
  if ((handle->type & MP4_TYPE_H263_PROFILE_0) || (handle->type & MP4_TYPE_H263_PROFILE_3))
  {
    ts->d263 = 15;
    ts->s263 = 86 + ts->d263;
    ts->stsd = 16 + ts->s263;
  }
  else /* MPEG-4 */
   if ((handle->type & MP4_TYPE_MPEG4_VIDEO))		
  {
    ts->esds = 37 + handle->videoDecSpecificInfoSize;
    ts->mp4v = 86 + ts->esds;
    ts->stsd = 16 + ts->mp4v;
  }
  else  /* AVC */
  if ( containsAvcVideo( handle->type ) )
  {
    /* Note: If necessary, add btrt and m4ds boxes here in the future. */
   ts->avcc = 8 + handle->videoDecSpecificInfoSize; /*handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->avcc->avcConfigSize;*/
   ts->avc1 = 86 + ts->avcc;
   ts->stsd = 16 + ts->avc1;
  }
  else
  {
  }

  ts->stts = 16 + st->sttsEntryCount * 8;
  ts->stsc = 16 + st->stscEntryCount * 12;
  if (st->stszSampleSize != 0)
    ts->stsz = 20;
  else
    ts->stsz = 20 + st->stszSampleCount * 4;
  ts->stco = 16 + st->stcoEntryCount * (st->stcoNeed64Bits ? 8 : 4);
  ts->stss = 16 + st->stssEntryCount * 4;
  if( handle->videoSampleTable->sdtpEntryCount )
  	ts->sdtp = 4 + 4 + 1 + 3 + handle->videoSampleTable->sdtpEntryCount; //size + 'SDTP' + ver + flags + dependencies
  else
    ts->sdtp = 0;

  ts->stbl = 8 + ts->stsd + ts->stts + ts->stsc + ts->stsz + ts->stco + ts->stss + ts->sdtp;
  ts->dref = 28;
  ts->dinf = 8 + ts->dref;
  ts->vmhd = 20;
  ts->minf = 8 + ts->vmhd + ts->dinf + ts->stbl;
  ts->hdlr = 33;
  
  if (handle->videoDuration > MP4_INT_MAX)
  {
    ts->mdhd = 44;
    ts->tkhd = 104;
  }
  else
  {
    ts->mdhd = 32;
    ts->tkhd = 92;
  }

  ts->mdia = 8 + ts->mdhd + ts->hdlr + ts->minf;
  if ( handle->videoUDTA )
    {
    ts->udta = 8 + handle->videoUDTA->atomcontentsize;  
    }
  ts->trak = 8 + ts->tkhd + ts->mdia + ts->udta;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 reUpdateAudioMetaData(sampleTable *st,
 *                                 mp4_u32 metaDataSize)
 *
 * Description:
 *
 *   This function updates the chunk offsets in the meta data to point to
 *   correct places.
 *
 * Parameters:
 *
 *   st            Sample table data
 *   metaDataSize  Meta data size
 *
 * Return value:
 *
 *   0             Success
 *
 */
mp4_i32 reUpdateAudioMetaData(sampleTable *st, mp4_u32 metaDataSize)
{
  mp4_u32 i;


  for (i = 0; i < st->stcoEntryCount; i++)
    updateChunkOffset(st, i, st->stcoChunkOffset[i] + metaDataSize + MDAT_HEADER_SIZE);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 reUpdateVideoMetaData(sampleTable *st,
 *                                 mp4_u32 metaDataSize)
 *
 * Description:
 *
 *   This function updates the chunk offsets in the meta data to point to
 *   correct places.
 *
 * Parameters:
 *
 *   st            Sample table data
 *   metaDataSize  Meta data size
 *
 * Return value:
 *
 *   0             Success
 *
 */
mp4_i32 reUpdateVideoMetaData(sampleTable *st, mp4_u32 metaDataSize)
{
  mp4_u32 i;


  for (i = 0; i < st->stcoEntryCount; i++)
    updateChunkOffset(st, i, st->stcoChunkOffset[i] + metaDataSize + MDAT_HEADER_SIZE);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeFTYP(MP4HandleImp handle)
 *
 * Description:
 *
 *   Write FTYP atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeFTYP(MP4HandleImp handle)
{
  mp4_u8  *buf;
  mp4_u32 i = 0;
  mp4_u8 ftypdelta = 0;

   if ((handle->generate3G2 && !(handle->type &  MP4_TYPE_QCELP_13K)) ||
       (!handle->generate3G2 && !(handle->type &  MP4_TYPE_AMR_WB)))
     ftypdelta = 4; /* one more additional compatible brand */
   else
     ftypdelta = 0;

   if( containsAvcVideo( handle->type ) )
   {
     ftypdelta += 4;
   }
   if(handle->bufferWrite)
		handle->ftypdelta=ftypdelta;

  buf = (mp4_u8 *)mp4malloc(FTYP_SIZE + ftypdelta);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)(FTYP_SIZE + ftypdelta));
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_FTYP);
  i += 4;

  if ( containsAvcVideo( handle->type ) )
  {
      if(handle->generateMP4)
      {
	      /* MPEG-4 Major brand */
	      buf[i++] = 'm';
	      buf[i++] = 'p';
	      buf[i++] = '4';
	      buf[i++] = '2';
      }

	  else
	  {
	    /* AVC is included for 3GPP Release 6 and beyond */
	    /* Major brand */
	    buf[i++] = '3';
	    buf[i++] = 'g';
	    buf[i++] = 'p';
	    buf[i++] = '6';
	  }
  }
  else
  {
    if(handle->generateMP4)
    {
	    /* MPEG-4 Major brand */
	    buf[i++] = 'm';
	    buf[i++] = 'p';
	    buf[i++] = '4';
	    buf[i++] = '2';
    }
    else if(handle->generate3G2)
    {
	    /* 3GPP2 Major brand */
	    buf[i++] = '3';
	    buf[i++] = 'g';
	    buf[i++] = '2';
	    buf[i++] = 'a';
    }
    else
    {
	    /* 3GPP Major brand */
	    buf[i++] = '3';
	    buf[i++] = 'g';
	    buf[i++] = 'p';
	    buf[i++] = '4';
    }
  }

  /* Minor version */
  if(handle->generateMP4)
  {  /* MPEG-4 Minor Version */
      insertu32(buf+i, (mp4_u32)(0)); /* 0 */
      i += 4;
  }
  else if(handle->generate3G2)
  {  /* 3GPP2 Minor Version */
      if( containsAvcVideo( handle->type ) )
      {
      	insertu32(buf+i, (mp4_u32)(2*256*256)); /* VB.0.0 */
      	i += 4;
      }
      else
      {
      	insertu32(buf+i, (mp4_u32)(1*256*256)); /* VA.0.0 */
      	i += 4;
      }
  }
  else  
  { /* 3GPP Minor Version */
      if( containsAvcVideo( handle->type ) )
      {
      	  insertu32(buf+i, (mp4_u32)2*256); /* V6.3.0 */
      	  i += 4;
      }
      else
      {
      	  insertu32(buf+i, (mp4_u32)4*256); /* V4.4.0 */
          i += 4;
      }
  }

  /* Compatible brands */
  if(handle->generateMP4)
  {/* MPEG-4 Compatible Brands */
	  buf[i++] = 'm';
	  buf[i++] = 'p';
	  buf[i++] = '4';
	  buf[i++] = '2';

      buf[i++] = '3';
      buf[i++] = 'g';
      buf[i++] = 'p';
      buf[i++] = '4';

      buf[i++] = 'i';
      buf[i++] = 's';
      buf[i++] = 'o';
      buf[i++] = 'm';
	  if ( containsAvcVideo( handle->type ) )
	  {
		  /* AVC is included for 3GPP Release 6 and beyond */
		  buf[i++] = 'a';
		  buf[i++] = 'v';
		  buf[i++] = 'c';
		  buf[i++] = '1';
	  }
  }
  else if(handle->generate3G2)
  {/* 3GPP2 Compatible Brands */
      if( containsAvcVideo( handle->type ) )
      {
      	  buf[i++] = '3';
	      buf[i++] = 'g';
	      buf[i++] = '2';
	      buf[i++] = 'b';
	      if (!(handle->type &  MP4_TYPE_QCELP_13K))
          { // If 3GPP codecs are used, then put 3GP6 in compatible brands 
              buf[i++] = '3';
              buf[i++] = 'g';
              buf[i++] = 'p';
              buf[i++] = '6';
          }
      }
      else
      {
      	  buf[i++] = '3';
      	  buf[i++] = 'g';
      	  buf[i++] = '2';
      	  buf[i++] = 'a';
          if (!(handle->type &  MP4_TYPE_QCELP_13K))
          { // If 3GPP codecs are used, then put 3GP4 in compatible brands 
              buf[i++] = '3';
              buf[i++] = 'g';
              buf[i++] = 'p';
              buf[i++] = '4';
          }
      }
	  if ( containsAvcVideo( handle->type ) )
	  {
		  /* AVC is included for 3GPP Release 6 and beyond */
		  buf[i++] = 'a';
		  buf[i++] = 'v';
		  buf[i++] = 'c';
		  buf[i++] = '1';
	  }

      buf[i++] = 'i';
      buf[i++] = 's';
      buf[i++] = 'o';
      buf[i++] = 'm';
  }
  else
  {/* 3GPP Compatible Brands */
      if ( containsAvcVideo( handle->type ) )
	  {
	      buf[i++] = '3';
	      buf[i++] = 'g';
	      buf[i++] = 'p';
	      buf[i++] = '6';
	  }
	  else
	  {
	      buf[i++] = '3';
	      buf[i++] = 'g';
	      buf[i++] = 'p';
	      buf[i++] = '4';
	  }
	  
      if (!(handle->type &  MP4_TYPE_AMR_WB))
      { // If 3GPP2 codecs are used, then put 3G2A in compatible brands 
          buf[i++] = '3';
          buf[i++] = 'g';
          buf[i++] = '2';
          buf[i++] = 'a';
      }

      buf[i++] = 'i';
      buf[i++] = 's';
      buf[i++] = 'o';
      buf[i++] = 'm';

	  if ( containsAvcVideo( handle->type ) )
	  {
		  /* AVC is included for 3GPP Release 6 and beyond */
		  buf[i++] = 'a';
		  buf[i++] = 'v';
		  buf[i++] = 'c';
		  buf[i++] = '1';
	  }
      
  }
  if (writeFile(handle, buf, (FTYP_SIZE + ftypdelta)) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeMOOV(MP4HandleImp handle,
 *                     mp4_u32 moovSize,
 *                     mp4_bool haveAudio,
 *                     mp4_bool haveVideo,
 *                     trakSize *audioTrackSize,
 *                     trakSize *videoTrackSize)
 *
 * Description:
 *
 *   Write MOOV atom.
 *
 * Parameters:
 *
 *   handle          MP4 library handle
 *   moovSize        Size of MOOV atom in bytes
 *   haveAudio       Flag to indicate whether audio exists or not
 *   haveVideo       Flag to indicate whether video exists or not
 *   audioTrackSize  Size of audio track in bytes
 *   videoTrackSize  Size of video track in bytes
 *
 * Return value:
 *
 *   0               Success
 *   -1              Error
 *
 */
mp4_i32 writeMOOV(MP4HandleImp handle, mp4_u32 moovSize, mp4_bool haveAudio, mp4_bool haveVideo, trakSize *audioTrackSize, trakSize *videoTrackSize)
{
  PRINT((_L("e_writemoov 1")));
  mp4_u8  buf[8];
  mp4_u32 i = 0;


  /* Size */
  insertu32(buf+i, moovSize);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MOOV);
  i += 4;

  PRINT((_L("e_writemoov_header 1")));
  if (writeFile(handle, buf, 8) < 0)
    return -1;
  PRINT((_L("e_writemoov_header 0")));  

  PRINT((_L("e_writemoov_mvhd 1")));	
  if (writeMVHD(handle) < 0)
    return -1;
  PRINT((_L("e_writemoov_mvhd 0")));

  PRINT((_L("e_writemoov_video 1")));
  if (haveVideo)
    if (writeVideoTrak(handle, videoTrackSize) < 0)
      return -1;
  PRINT((_L("e_writemoov_video 0")));    

  PRINT((_L("e_writemoov_audio 1")));
  if (haveAudio)
    if (writeAudioTrak(handle, audioTrackSize) < 0)
      return -1;
  PRINT((_L("e_writemoov_audio 0")));    
    
  PRINT((_L("e_writemoov_udta 1")));    
  if (handle->moovUDTA)
    {
    if (writeUDTA(handle, handle->moovUDTA) < 0)
        {
        return -1;
        }
    }
  PRINT((_L("e_writemoov_udta 0")));
  PRINT((_L("e_writemoov 0")));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeMVHD(MP4HandleImp handle)
 *
 * Description:
 *
 *   Write MVHD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeMVHD(MP4HandleImp handle)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  u32;

  size_t mvhdSize = mvhdAtomSize(handle);
      
  buf = (mp4_u8 *)mp4malloc(mvhdSize);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)mvhdSize);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MVHD);
  i += 4;


  if (handle->videoDuration > MP4_INT_MAX || handle->audioDuration > MP4_INT_MAX)
  {
      /* Version and flags */
      insertu32(buf+i, (mp4_u32)0x01000000);  //its going to be a version 1 atom
      i += 4;
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Timescale */
      insertu32(buf+i, (mp4_u32)MVHD_TIMESCALE);
      i += 4;
    
      /* Duration */
      {
        mp4_u64  u64;
        mp4_u64  videoDuration = 0;
        mp4_u64  audioDuration = 0;
    
    
        if (handle->videoTimeScale)
          videoDuration = (mp4_u64)((mp4_double)MVHD_TIMESCALE *
                                    (mp4_double)handle->videoDuration /
                                    (mp4_double)handle->videoTimeScale +
                                    (mp4_double)0.5);
        if (handle->audioTimeScale)
          audioDuration = (mp4_u64)((mp4_double)MVHD_TIMESCALE *
                                    (mp4_double)handle->audioDuration /
                                    (mp4_double)handle->audioTimeScale +
                                    (mp4_double)0.5);
    
        if (audioDuration > videoDuration)
          u64 = audioDuration;
        else
          u64 = videoDuration;
    
        insertu64(buf+i, u64);
        i += 8;
      }
 
  }
  else
  {
      /* Version and flags */
      insertu32(buf+i, (mp4_u32)0);
      i += 4;

      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Timescale */
      insertu32(buf+i, (mp4_u32)MVHD_TIMESCALE);
      i += 4;
    
      /* Duration */
      {
        mp4_u32  videoDuration = 0;
        mp4_u32  audioDuration = 0;
    
    
        if (handle->videoTimeScale)
          videoDuration = (mp4_u32)((mp4_double)MVHD_TIMESCALE *
                                    (mp4_double)handle->videoDuration /
                                    (mp4_double)handle->videoTimeScale +
                                    (mp4_double)0.5);
        if (handle->audioTimeScale)
          audioDuration = (mp4_u32)((mp4_double)MVHD_TIMESCALE *
                                    (mp4_double)handle->audioDuration /
                                    (mp4_double)handle->audioTimeScale +
                                    (mp4_double)0.5);
    
        if (audioDuration > videoDuration)
          u32 = audioDuration;
        else
          u32 = videoDuration;
    
        insertu32(buf+i, u32);
        i += 4;
      }
  }
  /* Reserved */
  insertu32(buf+i, (mp4_u32)0x00010000);
  i += 4;

  insertu16(buf+i, (mp4_u16)0x0100);
  i += 2;

  insertu16(buf+i, (mp4_u16)0x0000);
  i += 2;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00010000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00010000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x40000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00010000);
  i += 4;

  if (writeFile(handle, buf, mvhdSize) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoTrak(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video track atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoTrak(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32 i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->trak);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_TRAK);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeVideoTKHD(handle, ts) < 0)
    return -1;

  if (writeVideoMDIA(handle, ts) < 0)
    return -1;
  
  if (handle->videoUDTA)
    {
    if (writeUDTA(handle, handle->videoUDTA) < 0)
        return -1;
    }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoTKHD(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write TKHD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoTKHD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8      *buf;
  mp4_u32     i = 0;
  mp4_u32     u32;
  mp4_double  ud;


  buf = (mp4_u8 *)mp4malloc(ts->tkhd);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->tkhd);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_TKHD);
  i += 4;

  
  if (handle->videoDuration > MP4_INT_MAX)
  {
      mp4_u64     u64;
      /* Version and flags */
      buf[i++] = 1;     //make this a version 1 atom
      buf[i++] = 0;
      buf[i++] = 0;
      buf[i++] = 7;  /* Track enabled, used in movie and preview */
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Track ID */
      insertu32(buf+i, (mp4_u32)1);
      i += 4;
    
      /* Reserved */
      insertu32(buf+i, (mp4_u32)0);
      i += 4;
    
      /* Duration */
      if ( (handle->videoDuration == 0) || (handle->videoTimeScale == 0) )
          {
          ud = 0;
          }
      else
          {
          ud = (mp4_double)MVHD_TIMESCALE * (mp4_double)handle->videoDuration / (mp4_double)handle->videoTimeScale + (mp4_double)0.5;
          }
    
      u64 = (mp4_u64)ud;
      insertu64(buf+i, u64);
      i += 8;
  }
  else
  {
      /* Version and flags */
      buf[i++] = 0;
      buf[i++] = 0;
      buf[i++] = 0;
      buf[i++] = 7;  /* Track enabled, used in movie and preview */
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Track ID */
      insertu32(buf+i, (mp4_u32)1);
      i += 4;
    
      /* Reserved */
      insertu32(buf+i, (mp4_u32)0);
      i += 4;
    
      /* Duration */
      if ( (handle->videoDuration == 0) || (handle->videoTimeScale == 0) )
          {
          ud = 0;
          }
      else
          {
          ud = (mp4_double)MVHD_TIMESCALE * (mp4_double)handle->videoDuration / (mp4_double)handle->videoTimeScale + (mp4_double)0.5;
          }
    
      u32 = (mp4_u32)ud;
      insertu32(buf+i, u32);
      i += 4;
  }
  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)0);  /* Visual track */
  i += 2;

  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  insertu32(buf+i, (mp4_u32)0x00010000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00010000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x40000000);
  i += 4;

  insertu16(buf+i, (mp4_u16)handle->videoWidth);  /* Width */
  i += 2;

  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  insertu16(buf+i, (mp4_u16)handle->videoHeight);  /* Height */
  i += 2;

  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  if (writeFile(handle, buf, ts->tkhd) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoMDIA(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video MDIA atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoMDIA(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->mdia);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MDIA);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeVideoMDHD(handle, ts) < 0)
    return -1;

  if (writeVideoHDLR(handle, ts) < 0)
    return -1;

  if (writeVideoMINF(handle, ts) < 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoMDHD(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video MDHD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoMDHD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  u32;


  buf = (mp4_u8 *)mp4malloc(ts->mdhd);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->mdhd);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MDHD);
  i += 4;

  if (handle->videoDuration > MP4_INT_MAX)
  {
      /* Version and flags */
      insertu32(buf+i, (mp4_u32)0x01000000); //version 1 atom
      i += 4;
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Timescale */
      insertu32(buf+i, (mp4_u32)handle->videoTimeScale);
      i += 4;
    
      /* Duration */
      insertu64(buf+i, handle->videoDuration);
      i += 8;
  }
  else
  {
      /* Version and flags */
      insertu32(buf+i, (mp4_u32)0);
      i += 4;
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Timescale */
      insertu32(buf+i, (mp4_u32)handle->videoTimeScale);
      i += 4;
    
      /* Duration */
      insertu32(buf+i, (mp4_u32)handle->videoDuration);
      i += 4;
  }
  
  /* Language */
  insertu16(buf+i, (mp4_u16)0x55c4); /* 'und' */
  i += 2;

  /* Reserved */
  insertu16(buf+i, (mp4_u16)0x0000);
  i += 2;

  if (writeFile(handle, buf, ts->mdhd) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoHDLR(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video HDLR atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoHDLR(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(ts->hdlr);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->hdlr);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_HDLR);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Handler type */
  buf[i++] = 'v';
  buf[i++] = 'i';
  buf[i++] = 'd';
  buf[i++] = 'e';

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Empty string */
  buf[i++] = 0;

  if (writeFile(handle, buf, ts->hdlr) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoMINF(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video MINF atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoMINF(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->minf);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MINF);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeVMHD(handle, ts) < 0)
    return -1;

  if (writeDINF(handle, ts) < 0)
    return -1;

  if (writeVideoSTBL(handle, ts) < 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32  writeVMHD(MP4HandleImp handle,
 *                      trakSize *ts)
 *
 * Description:
 *
 *   Write VMHD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32  writeVMHD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(ts->vmhd);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->vmhd);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_VMHD);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0x00000001);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  if (writeFile(handle, buf, ts->vmhd) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeDINF(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write DINF atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeDINF(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->dinf);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_DINF);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeDREF(handle, ts) < 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32  writeDREF(MP4HandleImp handle,
 *                      trakSize *ts)
 *
 * Description:
 *
 *   Write DREF atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32  writeDREF(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(ts->dref);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->dref);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_DREF);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)1);
  i += 4;

  /* URL atom */

  /* Size */
  insertu32(buf+i, (mp4_u32)12);
  i += 4;

  /* Type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_URL);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0x00000001);
  i += 4;

  if (writeFile(handle, buf, ts->dref) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTBL(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video STBL atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTBL(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stbl);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STBL);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeVideoSTSD(handle, ts) < 0)
    return -1;

  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (writeVideoSTTSLongClip(handle, ts) < 0)
      return -1;
  }
  else
  {
    if (writeVideoSTTS(handle, ts) < 0)
      return -1;
  }

  if (writeVideoSTSC(handle, ts) < 0)
    return -1;

  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (writeVideoSTSZLongClip(handle, ts) < 0)
      return -1;
	
	if (handle->videoSampleTable->stcoNeed64Bits)
	{
      if (writeVideoCO64LongClip(handle, ts) < 0)
        return -1;
	}
	else
	{
      if (writeVideoSTCOLongClip(handle, ts) < 0)
        return -1;
	}

    if (writeVideoSTSSLongClip(handle, ts) < 0)
      return -1;
    
    if(ts->sdtp && writeVideoSDTPLongClip(handle, ts) < 0)
      return -1;
  }
  else
  {
    if (writeVideoSTSZ(handle, ts) < 0)
      return -1;

	if (handle->videoSampleTable->stcoNeed64Bits)
	{
      if (writeVideoCO64(handle, ts) < 0)
        return -1;
	}
	else
	{
      if (writeVideoSTCO(handle, ts) < 0)
        return -1;
	}

    if (writeVideoSTSS(handle, ts) < 0)
      return -1;
    
    if(ts->sdtp && writeVideoSDTP(handle, ts) < 0)
      return -1;
  }
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTSD(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video STSD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTSD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[16];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stsd);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSD);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)1);
  i += 4;

  if (writeFile(handle, buf, 16) < 0)
    return -1;

  if (handle->type & MP4_TYPE_MPEG4_VIDEO)
  {
    if (writeMP4V(handle, ts) < 0)
      return -1;
  }
  else if ((handle->type & MP4_TYPE_H263_PROFILE_0) ||
           (handle->type & MP4_TYPE_H263_PROFILE_3))
  {
    if (writeS263(handle, ts) < 0)
      return -1;
  }
  else if ( containsAvcVideo( handle->type ) )
  {
	  if (writeAVC1(handle, ts) < 0)
		  return -1;
  }
  else
  {
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeMP4V(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write MP4V atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeMP4V(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(86);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->mp4v);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MP4V);
  i += 4;

  /* Reserved */
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;

  /* Data reference index */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Width */
  insertu16(buf+i, (mp4_u16)handle->videoWidth);
  i += 2;

  /* Height */
  insertu16(buf+i, (mp4_u16)handle->videoHeight);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0x00480000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00480000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)24);
  i += 2;

  insertu16(buf+i, (mp4_u16)0xffff);
  i += 2;

  if (writeFile(handle, buf, 86) < 0)
  {
    mp4free(buf);

    return -1;
  }

  if (writeVideoESD(handle, ts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoESD(MP4HandleImp handle,
 *                         trakSize *ts)
 *
 * Description:
 *
 *   Write video ESD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoESD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(ts->esds);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->esds);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_ESD);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* ES_DescrTag */
  buf[i++] = 0x03;

  /* Size */
  buf[i++] = (mp4_u8)(23 + handle->videoDecSpecificInfoSize);

  /* ES_ID */
  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  /* Flags */
  buf[i++] = 0;

  /* DecoderConfigDescrTag */
  buf[i++] = 0x04;

  /* Size */
  buf[i++] = (mp4_u8)(15 + handle->videoDecSpecificInfoSize);

  /* ObjectTypeIndication */
  buf[i++] = 0x20;

  /* Flags */
  buf[i++] = 0x11;

  /* BufferSizeDB */
  buf[i++] = 0x00;
  buf[i++] = 0x50;
  buf[i++] = 0x00;

  /* MaxBitrate */
  insertu32(buf+i, (mp4_u32)handle->videoMaxBitrate);
  i += 4;

  /* AvgBitrate */
  insertu32(buf+i, (mp4_u32)handle->videoAvgBitrate);
  i += 4;

  /* DecSpecificInfoTag */
  buf[i++] = 0x05;

  /* Size */
  buf[i++] = (mp4_u8)handle->videoDecSpecificInfoSize;

  /* DecoderSpecificInfo */
  mp4memcpy(buf+i, handle->videoDecSpecificInfo, handle->videoDecSpecificInfoSize);
  i += handle->videoDecSpecificInfoSize;

  /* SLConfigDescrTag */
  buf[i++] = 0x06;

  /* Size */
  buf[i++] = 1;

  /* */
  buf[i++] = 2;

  if (writeFile(handle, buf, ts->esds) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeS263(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write S263 atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeS263(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(86);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->s263);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_S263);
  i += 4;

  /* Reserved */
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;

  /* Data reference index */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Width */
  insertu16(buf+i, (mp4_u16)handle->videoWidth);
  i += 2;

  /* Height */
  insertu16(buf+i, (mp4_u16)handle->videoHeight);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0x00480000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00480000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)24);
  i += 2;

  insertu16(buf+i, (mp4_u16)0xffff);
  i += 2;

  if (writeFile(handle, buf, 86) < 0)
  {
    mp4free(buf);

    return -1;
  }

  if (writeD263(handle, ts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeD263(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write D263 atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeD263(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[15];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->d263);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_D263);
  i += 4;

  /* Vendor */
  buf[i++] = 'S';
  buf[i++] = '6';
  buf[i++] = '0';
  buf[i++] = ' ';

  /* Decoder version */
  buf[i++] = 0;

  /* H263_Level */
  buf[i++] = handle->videoLevel;

  /* H263_Profile */
  if (handle->type & MP4_TYPE_H263_PROFILE_0)
    buf[i++] = 0;
  else if (handle->type & MP4_TYPE_H263_PROFILE_3)
    buf[i++] = 3;
  else
    return -1;

  if (writeFile(handle, buf, 15) < 0)
    return -1;

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeAVC1(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write AVC1 atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAVC1(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(86);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->avc1);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_AVC1);
  i += 4;

  /* Reserved */
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;

  /* Data reference index */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Width */
  insertu16(buf+i, (mp4_u16)handle->videoWidth);
  i += 2;

  /* Height */
  insertu16(buf+i, (mp4_u16)handle->videoHeight);
  i += 2;

  /* H-res (default is 72dpi = 0x00480000) */
  insertu32(buf+i, (mp4_u32)0x00480000);
  i += 4;

  /* V-res (default is 72dpi = 0x00480000) */
  insertu32(buf+i, (mp4_u32)0x00480000);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;
  
  /* Frame count (default is 1) */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Compressor name (32 byte string) */
  // The spec *recommends* inserting "\012AVC Coding" here
  // but we just have a string of nulls.
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Depth (default is 0x0018 which indicates colour images with no alpha) */
  insertu16(buf+i, (mp4_u16)24);
  i += 2;

  /* Pre-defined (-1) */
  insertu16(buf+i, (mp4_u16)0xffff);
  i += 2;

  if (writeFile(handle, buf, 86) < 0)
  {
    mp4free(buf);

    return -1;
  }

  if (writeAVCC(handle, ts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  /* Note: If necessary, include writing of btrt and m4ds atoms. */

  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeAVCC(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write AVCC atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAVCC(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(ts->avcc);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->avcc);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_AVCC);
  i += 4;

  /*mp4memcpy(buf+i, handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->avcc->avcConfig, 
									 handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->avcc->avcConfigSize);*/
  
  mp4memcpy(buf+i, handle->videoDecSpecificInfo, handle->videoDecSpecificInfoSize);

  /*i += handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->avcc->avcConfigSize;*/

  i += handle->videoDecSpecificInfoSize;

  if (writeFile(handle, buf, ts->avcc) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTTS(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video STTS atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTTS(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stts);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stts);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STTS);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->sttsEntryCount);
  i += 4;

  /* Sample count and sample delta */
  for (j = 0; j < handle->videoSampleTable->sttsEntryCount; j++)
  {
    insertu32(buf+i, (mp4_u32)handle->videoSampleTable->sttsSampleCount[j]);
    i += 4;
    insertu32(buf+i, (mp4_u32)handle->videoSampleTable->sttsSampleDelta[j]);
    i += 4;
  }

  if (writeFile(handle, buf, ts->stts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTTSLongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write video STTS atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTTSLongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u8   *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;


  buf = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE / 2);
  if (buf2 == NULL)
    {
    mp4free(buf);
    return -1;
    }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stts);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STTS);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->sttsEntryCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Sample count and delta */

  /* Seek to the beginning of temporary files */

  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_VIDEO_STTS_SAMPLE_COUNT) < 0)
      {
      mp4free(buf);
      mp4free(buf2);
      return -1;
      }
  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_VIDEO_STTS_SAMPLE_DELTA) < 0)
      {
      mp4free(buf);
      mp4free(buf2);
      return -1;
      }

  left = handle->videoSampleTable->sttsEntryCount * 4; /* Bytes left in each file */

  while (left)
  {
    if (left > METADATA_COPY_BUFFERSIZE / 2)
      bytestoread = METADATA_COPY_BUFFERSIZE / 2;
    else
      bytestoread = left;

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_VIDEO_STTS_SAMPLE_COUNT) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 4)
    {
      insertu32(buf + 2*j, ((mp4_u32 *)buf2)[j / 4]);
    }

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_VIDEO_STTS_SAMPLE_DELTA) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 4)
    {
      insertu32(buf + 2*j + 4, ((mp4_u32 *)buf2)[j / 4]);
    }

    if (writeFile(handle, buf, 2 * bytestoread) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTSC(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video STSC atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTSC(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stsc);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stsc);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSC);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stscEntryCount);
  i += 4;

  /* First chunk, samples per chunk and sample description index */
  for (j = 0; j < handle->videoSampleTable->stscEntryCount; j++)
  {
    insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stscFirstChunk[j]);
    i += 4;
    insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stscSamplesPerChunk[j]);
    i += 4;
    insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stscSampleDescriptionIndex[j]);
    i += 4;
  }

  if (writeFile(handle, buf, ts->stsc) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTSZ(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video STSZ atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTSZ(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stsz);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stsz);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSZ);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Sample size */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stszSampleSize);
  i += 4;

  /* Sample count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stszSampleCount);
  i += 4;

  /* Entry sizes */
  if (handle->videoSampleTable->stszSampleSize == 0)
  {
    for (j = 0; j < handle->videoSampleTable->stszSampleCount; j++)
    {
      insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stszEntrySize[j]);
      i += 4;
    }
  }

  if (writeFile(handle, buf, ts->stsz) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTSZLongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write video STSZ atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTSZLongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u8  *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;


  buf = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf2 == NULL)
    {
    mp4free(buf);
    return -1;
    }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stsz);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSZ);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Sample size */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stszSampleSize);
  i += 4;

  /* Sample count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stszSampleCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Entry sizes */

  if (handle->videoSampleTable->stszSampleSize == 0)
  {
    /* Seek to the beginning of temporary file */

    if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_VIDEO_STSZ_ENTRY_SIZE) < 0)
        {
        mp4free(buf);
        mp4free(buf2);
        return -1;
        }

    left = handle->videoSampleTable->stszSampleCount * 4; /* Bytes left in the file */

    while (left)
    {
      if (left > METADATA_COPY_BUFFERSIZE)
        bytestoread = METADATA_COPY_BUFFERSIZE;
      else
        bytestoread = left;

      if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_VIDEO_STSZ_ENTRY_SIZE) < 0)
      {
        mp4free(buf);
        mp4free(buf2);

        return -1;
      }

      for (j = 0; j < bytestoread; j += 4)
      {
        insertu32(buf + j, ((mp4_u32 *)buf2)[j / 4]);
      }

      if (writeFile(handle, buf, bytestoread) < 0)
      {
        mp4free(buf);
        mp4free(buf2);

        return -1;
      }

      left -= bytestoread;
    }
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTCO(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video STCO atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTCO(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stco);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stco);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STCO);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stcoEntryCount);
  i += 4;

  /* Chunk offsets */
  for (j = 0; j < handle->videoSampleTable->stcoEntryCount; j++)
  {
    insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stcoChunkOffset[j]);
    i += 4;
  }

  if (writeFile(handle, buf, ts->stco) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeVideoCO64(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video CO64 atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoCO64(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stco);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stco);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_CO64);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stcoEntryCount);
  i += 4;

  /* Chunk offsets */
  for (j = 0; j < handle->videoSampleTable->stcoEntryCount; j++)
  {
    insertu64(buf+i, (mp4_u64)handle->videoSampleTable->stcoChunkOffset[j]);
    i += 8;
  }

  if (writeFile(handle, buf, ts->stco) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeVideoSTCOLongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write video STCO atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTCOLongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u8   *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;
  mp4_u32  bufferSize = (METADATA_COPY_BUFFERSIZE/sizeof(mp4_u64)) * sizeof(mp4_u64); //must be a multiple of u64
  																					  //METADATA_COPY_BUFFERSIZE is only guaranteed to be a multiple of 4
	
  buf = (mp4_u8 *)mp4malloc(bufferSize);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(bufferSize);
  if (buf2 == NULL)
    {
    mp4free(buf);
    return -1;
    }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stco);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STCO);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stcoEntryCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Chunk offsets */

  /* Seek to the beginning of temporary file */

  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_VIDEO_STCO_CHUNK_OFFSET) < 0)
    {
    mp4free(buf);
    mp4free(buf2);
    return -1;
    }

  left = handle->videoSampleTable->stcoEntryCount * 8; /* Bytes left in the file */

  while (left)
  {
    if (left > bufferSize)
      bytestoread = bufferSize;
    else
      bytestoread = left;

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_VIDEO_STCO_CHUNK_OFFSET) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 8)
    {
      ((mp4_u64 *)buf2)[j / 8] += (handle->metaDataSize + MDAT_HEADER_SIZE); /* Update chunk offsets */
      insertu32(buf + j/2, ((mp4_u64 *)buf2)[j / 8]);
    }

    if (writeFile(handle, buf, bytestoread/2) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeVideoCO64LongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write video CO64 atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoCO64LongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u8   *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;
  mp4_u32  bufferSize = (METADATA_COPY_BUFFERSIZE/sizeof(mp4_u64)) * sizeof(mp4_u64); //must be a multiple of u64
  																					  //METADATA_COPY_BUFFERSIZE is only guaranteed to be a multiple of 4
	
  buf = (mp4_u8 *)mp4malloc(bufferSize);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(bufferSize);
  if (buf2 == NULL)
    {
    mp4free(buf);
    return -1;
    }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stco);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_CO64);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stcoEntryCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Chunk offsets */

  /* Seek to the beginning of temporary file */

  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_VIDEO_STCO_CHUNK_OFFSET) < 0)
    {
    mp4free(buf);
    mp4free(buf2);
    return -1;
    }

  left = handle->videoSampleTable->stcoEntryCount * 8; /* Bytes left in the file */

  while (left)
  {
    if (left > bufferSize)
      bytestoread = bufferSize;
    else
      bytestoread = left;

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_VIDEO_STCO_CHUNK_OFFSET) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 8)
    {
      ((mp4_u64 *)buf2)[j / 8] += (handle->metaDataSize + MDAT_HEADER_SIZE); /* Update chunk offsets */
      insertu64(buf + j, ((mp4_u64 *)buf2)[j / 8]);
    }

    if (writeFile(handle, buf, bytestoread) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeVideoSTSS(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video STSS atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTSS(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stss);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stss);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSS);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stssEntryCount);
  i += 4;

  /* Sample numbers */
  for (j = 0; j < handle->videoSampleTable->stssEntryCount; j++)
  {
    insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stssSampleNumber[j]);
    i += 4;
  }

  if (writeFile(handle, buf, ts->stss) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeVideoSTSSLongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write video STSS atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSTSSLongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u8   *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;


  buf = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf2 == NULL)
      {
      mp4free(buf);
      return -1;
      }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stss);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSS);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->videoSampleTable->stssEntryCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Sample numbers */

  /* Seek to the beginning of temporary file */

  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_VIDEO_STSS_SAMPLE_NUMBER) < 0)
    {
    mp4free(buf);
    mp4free(buf2);
    return -1;
    }

  left = handle->videoSampleTable->stssEntryCount * 4; /* Bytes left in the file */

  while (left)
  {
    if (left > METADATA_COPY_BUFFERSIZE)
      bytestoread = METADATA_COPY_BUFFERSIZE;
    else
      bytestoread = left;

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_VIDEO_STSS_SAMPLE_NUMBER) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 4)
    {
      insertu32(buf + j, ((mp4_u32 *)buf2)[j / 4]);
    }

    if (writeFile(handle, buf, bytestoread) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioTrak(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio track atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioTrak(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->trak);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_TRAK);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeAudioTKHD(handle, ts) < 0)
    return -1;

  if (writeAudioMDIA(handle, ts) < 0)
    return -1;
  
  if (handle->audioUDTA)
    {
    if (writeUDTA(handle, handle->audioUDTA) < 0)
        return -1;
    }  

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioTKHD(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio TKHD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioTKHD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8      *buf;
  mp4_u32     i = 0;
  mp4_u32     u32;
  mp4_double  ud;


  buf = (mp4_u8 *)mp4malloc(ts->tkhd);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->tkhd);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_TKHD);
  i += 4;

  
  if (handle->audioDuration > MP4_INT_MAX)
  {
      mp4_u64     u64;
      /* Version and flags */
      buf[i++] = 1;
      buf[i++] = 0;
      buf[i++] = 0;
      buf[i++] = 7;  /* Track enabled, used in movie and preview */
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Track ID */
      insertu32(buf+i, (mp4_u32)2);
      i += 4;
    
      /* Reserved */
      insertu32(buf+i, (mp4_u32)0);
      i += 4;
    
      /* Duration */
      if ( ( handle->audioTimeScale == 0 ) || ( handle->audioDuration == 0 ) )
          {
          ud = 0;
          }
      else
          {
          ud = (mp4_double)MVHD_TIMESCALE * (mp4_double)handle->audioDuration / (mp4_double)handle->audioTimeScale + (mp4_double)0.5;
          }
      u64 = (mp4_u64)ud;
      insertu64(buf+i, u64);
      i += 8;

  }
  else
  {
      /* Version and flags */
      buf[i++] = 0;
      buf[i++] = 0;
      buf[i++] = 0;
      buf[i++] = 7;  /* Track enabled, used in movie and preview */
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Track ID */
      insertu32(buf+i, (mp4_u32)2);
      i += 4;
    
      /* Reserved */
      insertu32(buf+i, (mp4_u32)0);
      i += 4;
    
      /* Duration */
      if ( ( handle->audioTimeScale == 0 ) || ( handle->audioDuration == 0 ) )
          {
          ud = 0;
          }
      else
          {
          ud = (mp4_double)MVHD_TIMESCALE * (mp4_double)handle->audioDuration / (mp4_double)handle->audioTimeScale + (mp4_double)0.5;
          }
      u32 = (mp4_u32)ud;
      insertu32(buf+i, u32);
      i += 4;
  }
  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)0x0100);  /* Audio track */
  i += 2;

  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  insertu32(buf+i, (mp4_u32)0x00010000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00010000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x40000000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);  /* Audio track */
  i += 4;

  insertu32(buf+i, (mp4_u32)0);  /* Audio track */
  i += 4;

  if (writeFile(handle, buf, ts->tkhd) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioMDIA(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio MDIA atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioMDIA(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->mdia);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MDIA);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeAudioMDHD(handle, ts) < 0)
    return -1;

  if (writeAudioHDLR(handle, ts) < 0)
    return -1;

  if (writeAudioMINF(handle, ts) < 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioMDHD(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio MDHD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioMDHD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  u32;


  buf = (mp4_u8 *)mp4malloc(ts->mdhd);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->mdhd);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MDHD);
  i += 4;

  if (handle->audioDuration > MP4_INT_MAX)
  {
      /* Version and flags */
      insertu32(buf+i, (mp4_u32)0x1000000);     //version 1 atom
      i += 4;
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu64(buf+i, (mp4_u64)u32);
      i += 8;
    
      /* Timescale */
      insertu32(buf+i, (mp4_u32)handle->audioTimeScale);
      i += 4;
    
      /* Duration */
      insertu64(buf+i, handle->audioDuration);
      i += 8;
      
  }
  else
  {
      /* Version and flags */
      insertu32(buf+i, (mp4_u32)0);
      i += 4;
    
      /* Creation time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Modification time */
      if (getCurrentTime(&u32) < 0)
        u32 = 0;
      insertu32(buf+i, (mp4_u32)u32);
      i += 4;
    
      /* Timescale */
      insertu32(buf+i, (mp4_u32)handle->audioTimeScale);
      i += 4;
    
      /* Duration */
      insertu32(buf+i, (mp4_u32)handle->audioDuration);
      i += 4;
  }

  /* Language */
  insertu16(buf+i, (mp4_u16)0x55c4); /* 'und' */
  i += 2;

  /* Reserved */
  insertu16(buf+i, (mp4_u16)0x0000);
  i += 2;

  if (writeFile(handle, buf, ts->mdhd) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioHDLR(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio HDLR atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioHDLR(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(ts->hdlr);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->hdlr);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_HDLR);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Handler type */
  buf[i++] = 's';
  buf[i++] = 'o';
  buf[i++] = 'u';
  buf[i++] = 'n';

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Empty string */
  buf[i++] = 0;

  if (writeFile(handle, buf, ts->hdlr) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioMINF(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio MINF atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioMINF(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->minf);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MINF);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeSMHD(handle, ts) < 0)
    return -1;

  if (writeDINF(handle, ts) < 0)
    return -1;

  if (writeAudioSTBL(handle, ts) < 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32  writeSMHD(MP4HandleImp handle,
 *                      trakSize *ts)
 *
 * Description:
 *
 *   Write SMHD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32  writeSMHD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(ts->smhd);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->smhd);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_SMHD);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  if (writeFile(handle, buf, ts->smhd) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioSTBL(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio STBL atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTBL(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[8];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stbl);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STBL);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeAudioSTSD(handle, ts) < 0)
    return -1;

  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (writeAudioSTTSLongClip(handle, ts) < 0)
      return -1;
  }
  else
  {
    if (writeAudioSTTS(handle, ts) < 0)
      return -1;
  }

  if (writeAudioSTSC(handle, ts) < 0)
    return -1;

  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (writeAudioSTSZLongClip(handle, ts) < 0)
      return -1;
    
	if (handle->audioSampleTable->stcoNeed64Bits)
	{
	  if (writeAudioCO64LongClip(handle, ts) < 0)
      	return -1;
	}
	else
	{
	  if (writeAudioSTCOLongClip(handle, ts) < 0)
        return -1;
	}
  }
  else
  {
    if (writeAudioSTSZ(handle, ts) < 0)
      return -1;

	if (handle->audioSampleTable->stcoNeed64Bits)
	{
      if (writeAudioCO64(handle, ts) < 0)
        return -1;
	}
	else
	{
      if (writeAudioSTCO(handle, ts) < 0)
        return -1;
	}
  }


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32  writeAudioSTSD(MP4HandleImp handle,
 *                           trakSize *ts)
 *
 * Description:
 *
 *   Write audio STSD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTSD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[16];
  mp4_u32 i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stsd);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSD);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)1);
  i += 4;

  if (writeFile(handle, buf, 16) < 0)
    return -1;

  if (handle->type & MP4_TYPE_MPEG4_AUDIO)
  {
    if (writeMP4A(handle, ts) < 0)
      return -1;
  }
  else if (handle->type & MP4_TYPE_AMR_NB)
  {
    if (writeSAMR(handle, ts) < 0)
      return -1;
  }
  else if (handle->type & MP4_TYPE_AMR_WB)
  {
    if (writeSAWB(handle, ts) < 0)
      return -1;
  }
  else if ((handle->type & MP4_TYPE_QCELP_13K) && (!handle->qcelpStoredAsMPEGAudio))
  {
    if (writeSQCP(handle, ts) < 0)
      return -1;
  }
  else if ((handle->type & MP4_TYPE_QCELP_13K) && (handle->qcelpStoredAsMPEGAudio))
  {
    if (writeMP4A(handle, ts) < 0)
      return -1;
  }
  else
  {
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeMP4A(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write MP4A atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeMP4A(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(36);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->mp4a);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_MP4A);
  i += 4;

  /* Reserved */
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;

  /* Data reference index */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)2);
  i += 2;

  insertu16(buf+i, (mp4_u16)16);
  i += 2;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Timescale */
  insertu16(buf+i, (mp4_u16)handle->audioTimeScale);
  i += 2;

  /* Reserved */
  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  if (writeFile(handle, buf, 36) < 0)
  {
    mp4free(buf);

    return -1;
  }

  if (writeAudioESD(handle, ts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioESD(MP4HandleImp handle,
 *                         trakSize *ts)
 *
 * Description:
 *
 *   Write audio ESD atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioESD(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  bitrate = 0;
  mp4_u32 num_of_bytes = 0;
  mp4_u32 size = 0;
  mp4_u32 index = 0;
  mp4_u32 tempnum = 0;

  mp4_u32 size1, size2;
  mp4_u32 numofsizebytes1, numofsizebytes2;

  buf = (mp4_u8 *)mp4malloc(ts->esds);
  if (buf == NULL)
    return -1;

  /* Calculate the necessary size information */
  size1 = handle->audioDecSpecificInfoSize;
  if (size1 < 128)
    numofsizebytes1 = 1;
  else
  {
      numofsizebytes1 = 1;
      while ( size1 >= 128 ) 
      {
        size1 = size1 >> 7;
        numofsizebytes1++;
      }  
  }

  size2 = 14 + numofsizebytes1 + handle->audioDecSpecificInfoSize;
   if (size2 < 128)
      numofsizebytes2 = 1;
    else
    {
        numofsizebytes2 = 1;
        while ( size2 >= 128 ) 
        {
          size2 = size2 >> 7;
          numofsizebytes2++;
        }  
    }
    /* End of size calculations */

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->esds);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_ESD);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* ES_DescrTag */
  buf[i++] = 0x03;

  /* Size */
  size = 21 + numofsizebytes1 + numofsizebytes2 + handle->audioDecSpecificInfoSize; 
  if (size < 128)
    buf[i++] = (mp4_u8)(size);
  else
  {
      num_of_bytes = 0;
      while ( size >= 128 ) 
      {
        size = size >> 7;
        num_of_bytes++;
      }  
      size = 21 + numofsizebytes1 + numofsizebytes2 + handle->audioDecSpecificInfoSize; 
      for(index = num_of_bytes; index > 0; index--)  
      {
        tempnum = size >> (7 * index); 
        buf[i++] = (mp4_u8)(0x80 | (mp4_u8) tempnum);
        size -= (tempnum << (7 * index));
      }
      buf[i++] = (mp4_u8)size;
  }

  /* ES_ID */
  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  /* Flags */
  buf[i++] = 0;

  /* DecoderConfigDescrTag */
  buf[i++] = 0x04;

  /* Size */
  size =  14 + numofsizebytes1 + handle->audioDecSpecificInfoSize; 
  if (size < 128)
    buf[i++] = (mp4_u8)(size);
  else
  {
      num_of_bytes = 0;
      while ( size >= 128 ) 
      {
        size = size >> 7;
        num_of_bytes++;
      }  
      size =  14 + numofsizebytes1 + handle->audioDecSpecificInfoSize; 
      for(index = num_of_bytes; index > 0; index--)  
      {
        tempnum = size >> (7 * index); 
        buf[i++] = (mp4_u8)(0x80 | (mp4_u8) tempnum);
        size -= (tempnum << (7 * index));
      }
      buf[i++] = (mp4_u8)size;
  }

  /* ObjectTypeIndication */
  if (handle->type & MP4_TYPE_MPEG4_AUDIO)
    buf[i++] = 0x40;
  else if ((handle->type & MP4_TYPE_QCELP_13K) && (handle->qcelpStoredAsMPEGAudio))
    buf[i++] = 0xE1;
  else
  {
  }

  /* Flags */
  buf[i++] = 0x15;

  /* BufferSizeDB */
  if (handle->type & MP4_TYPE_MPEG4_AUDIO) 
  {
      buf[i++] = 0x00;
      buf[i++] = 0x00;
      buf[i++] = 0x00;
  }
  else if ((handle->type & MP4_TYPE_QCELP_13K) && (handle->qcelpStoredAsMPEGAudio))
  { /* 4096 for QCELP 13K */
      buf[i++] = 0x00;
      buf[i++] = 0x10;
      buf[i++] = 0x00;
  }
  else
  {
  }


  if ((handle->audioDuration != 0) && (handle->audioTimeScale != 0))
    bitrate = (mp4_u32)((mp4_double)8 *
                              (mp4_double)handle->audioMediaDataSize /
                              ((mp4_double)handle->audioDuration /
                               (mp4_double)handle->audioTimeScale));
  else
    bitrate = 0;

  /* MaxBitrate */
  insertu32(buf+i, (mp4_u32)bitrate); /*0x00010000*/
  i += 4;

  /* AvgBitrate */
  insertu32(buf+i, (mp4_u32)bitrate);/*0x00008000*/
  i += 4;

  /* DecSpecificInfoTag */
  buf[i++] = 0x05;

  /* Size */
  size = handle->audioDecSpecificInfoSize;
  if (size < 128)
    buf[i++] = (mp4_u8)(size);
  else
  {
      num_of_bytes = 0;
      while ( size >= 128 ) 
      {
        size = size >> 7;
        num_of_bytes++;
      }  
      size = handle->audioDecSpecificInfoSize;
      for(index = num_of_bytes; index > 0; index--)  
      {
        tempnum = size >> (7 * index); 
        buf[i++] = (mp4_u8)(0x80 | (mp4_u8) tempnum);
        size -= (tempnum << (7 * index));
      }
      buf[i++] = (mp4_u8)size;
  }

  /* DecoderSpecificInfo */
  mp4memcpy(buf+i, handle->audioDecSpecificInfo, handle->audioDecSpecificInfoSize);
  i += handle->audioDecSpecificInfoSize;

  /* SLConfigDescrTag */
  buf[i++] = 0x06;

  /* Size */
  buf[i++] = 1;

  /* */
  buf[i++] = 2;

  if (writeFile(handle, buf, ts->esds) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeSAMR(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write SAMR atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32  writeSAMR(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(36);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->samr);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_SAMR);
  i += 4;

  /* Reserved */
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;

  /* Data reference index */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)2);
  i += 2;

  insertu16(buf+i, (mp4_u16)16);
  i += 2;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Timescale */
  insertu16(buf+i, (mp4_u16)handle->audioTimeScale);
  i += 2;

  /* Reserved */
  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  if (writeFile(handle, buf, 36) < 0)
  {
    mp4free(buf);

    return -1;
  }

  if (writeDAMR(handle, ts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeSAWB(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write SAWB atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeSAWB(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32 i = 0;


  buf = (mp4_u8 *)mp4malloc(36);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->sawb);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_SAWB);
  i += 4;

  /* Reserved */
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;

  /* Data reference index */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)2);
  i += 2;

  insertu16(buf+i, (mp4_u16)16);
  i += 2;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Timescale */
  insertu16(buf+i, (mp4_u16)handle->audioTimeScale);
  i += 2;

  /* Reserved */
  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  if (writeFile(handle, buf, 36) < 0)
  {
    mp4free(buf);

    return -1;
  }

  if (writeDAMR(handle, ts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeDAMR(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write DAMR atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeDAMR(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[17];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->damr);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_DAMR);
  i += 4;

  /* Vendor */
  buf[i++] = 'S';
  buf[i++] = '6';
  buf[i++] = '0';
  buf[i++] = ' ';

  /* Decoder version */
  buf[i++] = 0;

  /* Mode set */
  insertu16(buf+i, (mp4_u16)handle->audioModeSet);
  i += 2;

  /* Mode change period */
  buf[i++] = 0;

  /* Frames per sample */
  buf[i++] = handle->audioFramesPerSample;

  if (writeFile(handle, buf, 17) < 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioSTTS(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio STTS atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTTS(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stts);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stts);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STTS);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->sttsEntryCount);
  i += 4;

  /* Sample count and sample delta */
  for (j = 0; j < handle->audioSampleTable->sttsEntryCount; j++)
  {
    insertu32(buf+i, (mp4_u32)handle->audioSampleTable->sttsSampleCount[j]);
    i += 4;
    insertu32(buf+i, (mp4_u32)handle->audioSampleTable->sttsSampleDelta[j]);
    i += 4;
  }

  if (writeFile(handle, buf, ts->stts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioSTTSLongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write audio STTS atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTTSLongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u8   *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;


  buf = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE / 2);
  if (buf2 == NULL)
      {
      mp4free(buf);
      return -1;
      }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stts);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STTS);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->sttsEntryCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Sample count and delta */

  /* Seek to the beginning of temporary files */

  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_AUDIO_STTS_SAMPLE_COUNT) < 0)
      {
      mp4free(buf);
      mp4free(buf2);
      return -1;
      }
  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_AUDIO_STTS_SAMPLE_DELTA) < 0)
      {
      mp4free(buf);
      mp4free(buf2);
      return -1;
      }

  left = handle->audioSampleTable->sttsEntryCount * 4; /* Bytes left in each file */

  while (left)
  {
    if (left > METADATA_COPY_BUFFERSIZE / 2)
      bytestoread = METADATA_COPY_BUFFERSIZE / 2;
    else
      bytestoread = left;

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_AUDIO_STTS_SAMPLE_COUNT) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 4)
    {
      insertu32(buf + 2*j, ((mp4_u32 *)buf2)[j / 4]);
    }

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_AUDIO_STTS_SAMPLE_DELTA) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 4)
    {
      insertu32(buf + 2*j + 4, ((mp4_u32 *)buf2)[j / 4]);
    }

    if (writeFile(handle, buf, 2 * bytestoread) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioSTSC(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio STSC atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTSC(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stsc);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stsc);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSC);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stscEntryCount);
  i += 4;

  /* First chunk, samples per chunk and sample description index */
  for (j = 0; j < handle->audioSampleTable->stscEntryCount; j++)
  {
    insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stscFirstChunk[j]);
    i += 4;
    insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stscSamplesPerChunk[j]);
    i += 4;
    insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stscSampleDescriptionIndex[j]);
    i += 4;
  }

  if (writeFile(handle, buf, ts->stsc) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioSTSZ(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio STSZ atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTSZ(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stsz);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stsz);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSZ);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Sample size */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stszSampleSize);
  i += 4;

  /* Sample count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stszSampleCount);
  i += 4;

  /* Entry sizes */
  if (handle->audioSampleTable->stszSampleSize == 0)
  {
    for (j = 0; j < handle->audioSampleTable->stszSampleCount; j++)
    {
      insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stszEntrySize[j]);
      i += 4;
    }
  }

  if (writeFile(handle, buf, ts->stsz) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioSTSZLongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write audio STSZ atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTSZLongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u8  *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;


  buf = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf2 == NULL)
      {
      mp4free(buf);
      return -1;
      }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stsz);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STSZ);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Sample size */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stszSampleSize);
  i += 4;

  /* Sample count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stszSampleCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Entry sizes */

  if (handle->audioSampleTable->stszSampleSize == 0)
  {
    /* Seek to the beginning of temporary file */

    if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_AUDIO_STSZ_ENTRY_SIZE) < 0)
        {
        mp4free(buf);
        mp4free(buf2);
        return -1;
        }

    left = handle->audioSampleTable->stszSampleCount * 4; /* Bytes left in the file */

    while (left)
    {
      if (left > METADATA_COPY_BUFFERSIZE)
        bytestoread = METADATA_COPY_BUFFERSIZE;
      else
        bytestoread = left;

      if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_AUDIO_STSZ_ENTRY_SIZE) < 0)
      {
        mp4free(buf);
        mp4free(buf2);

        return -1;
      }

      for (j = 0; j < bytestoread; j += 4)
      {
        insertu32(buf + j, ((mp4_u32 *)buf2)[j / 4]);
      }

      if (writeFile(handle, buf, bytestoread) < 0)
      {
        mp4free(buf);
        mp4free(buf2);

        return -1;
      }

      left -= bytestoread;
    }
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeAudioSTCO(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio STCO atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTCO(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stco);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stco);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STCO);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stcoEntryCount);
  i += 4;

  /* Chunk offsets */
  for (j = 0; j < handle->audioSampleTable->stcoEntryCount; j++)
  {
    insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stcoChunkOffset[j]);
    i += 4;
  }

  if (writeFile(handle, buf, ts->stco) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeAudioCO64(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write audio CO64 atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioCO64(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;


  buf = (mp4_u8 *)mp4malloc(ts->stco);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stco);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_CO64);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stcoEntryCount);
  i += 4;

  /* Chunk offsets */
  for (j = 0; j < handle->audioSampleTable->stcoEntryCount; j++)
  {
    insertu64(buf+j, (mp4_u64)handle->audioSampleTable->stcoChunkOffset[j]);
  }

  if (writeFile(handle, buf, ts->stco) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeAudioSTCOLongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write audio STCO atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioSTCOLongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u8   *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;
  mp4_u32  bufferSize = (METADATA_COPY_BUFFERSIZE/sizeof(mp4_u64)) * sizeof(mp4_u64); //must be a multiple of u64
  																					  //METADATA_COPY_BUFFERSIZE is only guaranteed to be a multiple of 4


  buf = (mp4_u8 *)mp4malloc(bufferSize);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(bufferSize);
  if (buf2 == NULL)
      {
      mp4free(buf);
      return -1;
      }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stco);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_STCO);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stcoEntryCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Chunk offsets */

  /* Seek to the beginning of temporary file */

  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_AUDIO_STCO_CHUNK_OFFSET) < 0)
  {
    mp4free(buf);
    mp4free(buf2);
    return -1;
  }

  left = handle->audioSampleTable->stcoEntryCount * 8; /* Bytes left in the file */

  while (left)
  {
    if (left > bufferSize)
      bytestoread = bufferSize;
    else
      bytestoread = left;

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_AUDIO_STCO_CHUNK_OFFSET) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 8)
    {
      ((mp4_u64 *)buf2)[j / 8] += (handle->metaDataSize + MDAT_HEADER_SIZE); /* Update chunk offsets */
      insertu32(buf + j/2, ((mp4_u64 *)buf2)[j / 8]);
    }

    if (writeFile(handle, buf, bytestoread/2) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeAudioCO64LongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write audio CO64 atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAudioCO64LongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u8   *buf2;
  mp4_u32  i = 0;
  mp4_u32  j;
  mp4_u32  left;
  mp4_u32  bytestoread;
  mp4_u32  bufferSize = (METADATA_COPY_BUFFERSIZE/sizeof(mp4_u64)) * sizeof(mp4_u64); //must be a multiple of u64
  																					  //METADATA_COPY_BUFFERSIZE is only guaranteed to be a multiple of 4


  buf = (mp4_u8 *)mp4malloc(bufferSize);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(bufferSize);
  if (buf2 == NULL)
      {
      mp4free(buf);
      return -1;
      }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->stco);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_CO64);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Entry count */
  insertu32(buf+i, (mp4_u32)handle->audioSampleTable->stcoEntryCount);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Chunk offsets */

  /* Seek to the beginning of temporary file */

  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_AUDIO_STCO_CHUNK_OFFSET) < 0)
  {
    mp4free(buf);
    mp4free(buf2);
    return -1;
  }

  left = handle->audioSampleTable->stcoEntryCount * 8; /* Bytes left in the file */

  while (left)
  {
    if (left > bufferSize)
      bytestoread = bufferSize;
    else
      bytestoread = left;

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_AUDIO_STCO_CHUNK_OFFSET) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    for (j = 0; j < bytestoread; j += 8)
    {
      ((mp4_u64 *)buf2)[j / 8] += (handle->metaDataSize + MDAT_HEADER_SIZE); /* Update chunk offsets */
      insertu64(buf + j, ((mp4_u64 *)buf2)[j / 8]);
    }

    if (writeFile(handle, buf, bytestoread) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeMediaData(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function writes media data to the output file.
 *
 *   Before writing media data to the output file, meta data has
 *   been written to the file. Media data that is in tmpfile is
 *   read from there and written to the end of the output file.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 writeMediaData(MP4HandleImp handle)
{
  mp4_u8  *buf;
  mp4_u32 i = 0;
  mp4_u64 left;
  mp4_u32 bytestoread;


  buf = (mp4_u8 *)mp4malloc(1024);
  if (buf == NULL)
    return -1;


  i = formatMdatHeader(buf, handle->bytesInTmpFile);
  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);

    return -1;
  }

  /* Seek to the beginning of tmpfile */
  if (seekTmpFileAbs(handle, 0) < 0)
      {
      mp4free(buf);
      return -1;
      }

  left = handle->bytesInTmpFile;

  while (left)
  {
    if (left > 1024)
      bytestoread = 1024;
    else
      bytestoread = left;

    if (readTmpFile(handle, buf, bytestoread) < 0)
    {
      mp4free(buf);

      return -1;
    }

    if (writeFile(handle, buf, bytestoread) < 0)
    {
      mp4free(buf);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 insertu64(mp4_u8 *buf,
 *                     mp4_u64 value)
 *
 * Description:
 *
 *   This function writes value into buf taking into account the endianism
 *   of the current computer.
 *
 *   It is assumed that the caller of the function has allocated enough
 *   space for buf.
 *
 * Parameters:
 *
 *   buf      Buffer to write to
 *   value    Value to write to buf
 *
 * Return value:
 *
 *   0        Success
 *
 */
mp4_i32 insertu64(mp4_u8 *buf, mp4_u64 value)
{
  mp4_u64 u64;


  u64 = u64endian(value);
  mp4memcpy(buf, &u64, sizeof(mp4_u64));

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 insertu32(mp4_u8 *buf,
 *                     mp4_u32 value)
 *
 * Description:
 *
 *   This function writes value into buf taking into account the endianism
 *   of the current computer.
 *
 *   It is assumed that the caller of the function has allocated enough
 *   space for buf.
 *
 * Parameters:
 *
 *   buf      Buffer to write to
 *   value    Value to write to buf
 *
 * Return value:
 *
 *   0        Success
 *
 */
mp4_i32 insertu32(mp4_u8 *buf, mp4_u32 value)
{
  mp4_u32 u32;


  u32 = u32endian(value);
  mp4memcpy(buf, &u32, 4);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 insertu16(mp4_u8 *buf,
 *                     mp4_u16 value)
 *
 * Description:
 *
 *   This function writes value into buf taking into account the endianism
 *   of the current computer.
 *
 *   It is assumed that the caller of the function has allocated enough
 *   space for buf.
 *
 * Parameters:
 *
 *   buf      Buffer to write to
 *   value    Value to write to buf
 *
 * Return value:
 *
 *   0        Success
 *
 */
mp4_i32 insertu16(mp4_u8 *buf, mp4_u16 value)
{
  mp4_u16 u16;


  u16 = u16endian(value);
  mp4memcpy(buf, &u16, 2);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 writeMetaDataTmp(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function writes metadata accumulated in memory to the disk.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *
 * Return value:
 *
 *   0         Success
 *   -1        Error
 *
 */
mp4_i32 writeMetaDataTmp(MP4HandleImp handle)
{
  PRINT((_L("e_writemetadatatmp 1")));
  if (handle->videoSampleTable)
  {
  	PRINT((_L("e_writemetadatatmp_writemetadata_video 1")));
    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->videoSampleTable->sttsSampleCount,
                             handle->videoSampleTable->sttsCurrentEntryCount * sizeof(mp4_u32),
                             METADATAFILE_VIDEO_STTS_SAMPLE_COUNT) < 0)
      return -1;

    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->videoSampleTable->sttsSampleDelta,
                             handle->videoSampleTable->sttsCurrentEntryCount * sizeof(mp4_u32),
                             METADATAFILE_VIDEO_STTS_SAMPLE_DELTA) < 0)
      return -1;

    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->videoSampleTable->stszEntrySize,
                             handle->videoSampleTable->stszCurrentSampleCount * sizeof(mp4_u32),
                             METADATAFILE_VIDEO_STSZ_ENTRY_SIZE) < 0)
      return -1;

    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->videoSampleTable->stcoChunkOffset,
                             handle->videoSampleTable->stcoCurrentEntryCount * sizeof(mp4_u64),
                             METADATAFILE_VIDEO_STCO_CHUNK_OFFSET) < 0)
      return -1;

    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->videoSampleTable->stssSampleNumber,
                             handle->videoSampleTable->stssCurrentEntryCount * sizeof(mp4_u32),
                             METADATAFILE_VIDEO_STSS_SAMPLE_NUMBER) < 0)
      return -1;

    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->videoSampleTable->sdtpSampleDependency,
                             handle->videoSampleTable->sdtpCurrentEntryCount * sizeof(mp4_u8),
                             METADATAFILE_VIDEO_SDTP_SAMPLE_DEPENDENCY) < 0)
      return -1;
  	PRINT((_L("e_writemetadatatmp_writemetadata_video 0")));
  }

  if (handle->audioSampleTable)
  {
  	PRINT((_L("e_writemetadatatmp_writemetadata_audio 1")));
    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->audioSampleTable->sttsSampleCount,
                             handle->audioSampleTable->sttsCurrentEntryCount * sizeof(mp4_u32),
                             METADATAFILE_AUDIO_STTS_SAMPLE_COUNT) < 0)
      return -1;

    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->audioSampleTable->sttsSampleDelta,
                             handle->audioSampleTable->sttsCurrentEntryCount * sizeof(mp4_u32),
                             METADATAFILE_AUDIO_STTS_SAMPLE_DELTA) < 0)
      return -1;

    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->audioSampleTable->stszEntrySize,
                             handle->audioSampleTable->stszCurrentSampleCount * sizeof(mp4_u32),
                             METADATAFILE_AUDIO_STSZ_ENTRY_SIZE) < 0)
      return -1;

    if (writeMetaDataFileNum(handle,
                             (mp4_u8 *)handle->audioSampleTable->stcoChunkOffset,
                             handle->audioSampleTable->stcoCurrentEntryCount * sizeof(mp4_u64),
                             METADATAFILE_AUDIO_STCO_CHUNK_OFFSET) < 0)
      return -1;
  	PRINT((_L("e_writemetadatatmp_writemetadata_audio 0")));      
  }

	if (handle->videoSampleTable)
		{
  		handle->videoSampleTable->sttsCurrentEntryCount  = 0;
  		handle->videoSampleTable->stszCurrentSampleCount = 0;
  		handle->videoSampleTable->stcoCurrentEntryCount  = 0;
  		handle->videoSampleTable->stssCurrentEntryCount  = 0;
  		handle->videoSampleTable->sdtpCurrentEntryCount  = 0;  
		}

	if (handle->audioSampleTable)
		{		
  		handle->audioSampleTable->sttsCurrentEntryCount  = 0;
  		handle->audioSampleTable->stszCurrentSampleCount = 0;
  		handle->audioSampleTable->stcoCurrentEntryCount  = 0;		
		}

  handle->metaDataBlocks = 0;
  handle->metaDataOnDisk = MP4TRUE;

  PRINT((_L("e_writemetadatatmp 0")));
  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeSQCP(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write SQCP atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32  writeSQCP(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(36);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->sqcp);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_SQCP);
  i += 4;

  /* Reserved */
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;

  /* Data reference index */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)2);
  i += 2;

  insertu16(buf+i, (mp4_u16)16);
  i += 2;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Timescale */
  insertu16(buf+i, (mp4_u16)handle->audioTimeScale);
  i += 2;

  /* Reserved */
  insertu16(buf+i, (mp4_u16)0);
  i += 2;

  if (writeFile(handle, buf, 36) < 0)
  {
    mp4free(buf);

    return -1;
  }

  if (writeDQCP(handle, ts) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeDQCP(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write DQCP atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeDQCP(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  buf[14];
  mp4_u32  i = 0;


  /* Size */
  insertu32(buf+i, (mp4_u32)ts->dqcp);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_DQCP);
  i += 4;

  /* Vendor */
  buf[i++] = 'n';
  buf[i++] = 'o';
  buf[i++] = 'k';
  buf[i++] = 'i';

  /* Decoder version */
  buf[i++] = 0;

  /* Mode set */
//  insertu16(buf+i, (mp4_u16)handle->audioModeSet);
//  i += 2;

  /* Mode change period */
//  buf[i++] = 0;

  /* Frames per sample */
  buf[i++] = handle->audioFramesPerSample;

  if (writeFile(handle, buf, 14) < 0)
    return -1;

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 calculateES_DescriptorSize(mp4_u32 type)
 *
 * Description:
 *
 *   Calculated the ES_Descriptor size inside the ESDS box.
 *    Updates handle->ES_DescriptorSize;
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *  type        the media type:
 *                  MP4_TYPE_QCELP_13K
 *                  MP4_TYPE_MPEG_AUDIO
 *                  MP4_TYPE_MPEG4_VIDEO 
 *
 * Return value:
 *
 *   0        Success
 *
 */
mp4_i32 calculateES_DescriptorSize(MP4HandleImp handle, mp4_u32 type)
{
  mp4_u32 size1, size2, size3;
  mp4_u32 numofsizebytes1, numofsizebytes2, numofsizebytes3;
  mp4_u32 decspecinfosize = 0;

  if (((type & MP4_TYPE_QCELP_13K) && (handle->qcelpStoredAsMPEGAudio)) ||
        (type & MP4_TYPE_MPEG4_AUDIO))
    decspecinfosize = handle->audioDecSpecificInfoSize;
  else /* MPEG video case */
    decspecinfosize = handle->videoDecSpecificInfoSize;

  size1 = decspecinfosize;
  if (size1 < 128)
    numofsizebytes1 = 1;
  else
  {
      numofsizebytes1 = 1;
      while ( size1 >= 128 ) 
      {
        size1 = size1 >> 7;
        numofsizebytes1++;
      }  
  }

  size2 = 14 + numofsizebytes1 + decspecinfosize;
   if (size2 < 128)
      numofsizebytes2 = 1;
    else
    {
        numofsizebytes2 = 1;
        while ( size2 >= 128 ) 
        {
          size2 = size2 >> 7;
          numofsizebytes2++;
        }  
    }

  size3 = 21 + numofsizebytes1 + numofsizebytes2 + decspecinfosize;
   if (size3 < 128)
      numofsizebytes3 = 1;
    else
    {
        numofsizebytes3 = 1;
        while ( size3 >= 128 ) 
        {
          size3 = size3 >> 7;
          numofsizebytes3++;
        }  
    }

    /* ES_DescriptorSize contains the size of the ES_Descriptor Field, including the 0x03 (ES_ID Tag) */
    handle->ES_DescriptorSize =  21 + numofsizebytes1 + numofsizebytes2 + decspecinfosize + numofsizebytes3 + 1;
    return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeUDTA(MP4HandleImp handle,
 *                     userDataAtom *udta)
 *
 * Description:
 *
 *   Write UDTA atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32  writeUDTA(MP4HandleImp handle, userDataAtom *udta)
{
  mp4_u8  buf[8];
  mp4_u32 i = 0;


  /* Size */
  insertu32(buf+i, 8 + (mp4_u32)udta->atomcontentsize);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_UDTA);
  i += 4;

  if (writeFile(handle, buf, 8) < 0)
    return -1;

  if (writeFile(handle, udta->contentdata, udta->atomcontentsize) < 0)
    return -1;

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 updateVideoDependencyMetaData(MP4HandleImp handle,
 *                                         mp4_u8 aDependsOn,
 *                                         mp4_u8 aIsDependentOn,
 *                                         mp4_u8 aHasRedundancy)
 *
 * Description:
 *
 *   Updates SDTP video dependency data
 *
 * Parameters:
 *
 *   handle          MP4 library handle
 *   aDependsOn      This frame's dependency on other frames
 *   aIsDependentOn  Other frames's dependency on this frame
 *   aHasRedundancy  Whether there is redundant coding in this frame
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 updateVideoDependencyMetaData(MP4HandleImp handle, mp4_u8 aDependsOn, mp4_u8 aIsDependentOn, mp4_u8 aHasRedundancy)
{
  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    if (handle->videoSampleTable->sdtpCurrentEntryCount == handle->videoSampleTable->sdtpMaxEntryCount)
    {
      void *p;

      p = mp4realloc(handle->videoSampleTable->sdtpSampleDependency,
                     2 * sizeof(mp4_u8) * handle->videoSampleTable->sdtpMaxEntryCount,
                     sizeof(mp4_u8) * handle->videoSampleTable->stssMaxEntryCount);
      if (p == NULL)
        return -1;

      handle->videoSampleTable->sdtpSampleDependency = (mp4_u8 *)p;

      handle->videoSampleTable->sdtpMaxEntryCount *= 2;
    }

    handle->videoSampleTable->sdtpSampleDependency[handle->videoSampleTable->sdtpCurrentEntryCount] = (aDependsOn << 4) | (aIsDependentOn << 2) | (aHasRedundancy);
    handle->videoSampleTable->sdtpCurrentEntryCount++;
    handle->videoSampleTable->sdtpEntryCount++;
  }
  else
  {
    if (handle->videoSampleTable->sdtpEntryCount == handle->videoSampleTable->sdtpMaxEntryCount)
    {
      void *p;

      p = mp4realloc(handle->videoSampleTable->sdtpSampleDependency,
                     2 * sizeof(mp4_u8) * handle->videoSampleTable->sdtpMaxEntryCount,
                     sizeof(mp4_u8) * handle->videoSampleTable->sdtpMaxEntryCount);
      if (p == NULL)
        return -1;

      handle->videoSampleTable->sdtpSampleDependency = (mp4_u8 *)p;

      handle->videoSampleTable->sdtpMaxEntryCount *= 2;
    }

    handle->videoSampleTable->sdtpSampleDependency[handle->videoSampleTable->sdtpEntryCount] = (aDependsOn << 4) | (aIsDependentOn << 2) | (aHasRedundancy);
    handle->videoSampleTable->sdtpEntryCount++;
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeVideoSDTP(MP4HandleImp handle,
 *                          trakSize *ts)
 *
 * Description:
 *
 *   Write video SDTP atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSDTP(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;
  mp4_u32  j;

  buf = (mp4_u8 *)mp4malloc(ts->sdtp);
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->sdtp);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_SDTP);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Sample dependencies */
  for (j = 0; j < handle->videoSampleTable->sdtpEntryCount; j++)
  {
    buf[i++] = (mp4_u8)handle->videoSampleTable->sdtpSampleDependency[j];
  }

  if (writeFile(handle, buf, ts->sdtp) < 0)
  {
    mp4free(buf);

    return -1;
  }

  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeVideoSDTPLongClip(MP4HandleImp handle,
 *                                  trakSize *ts)
 *
 * Description:
 *
 *   Write video SDTP atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeVideoSDTPLongClip(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8   *buf;
  mp4_u8   *buf2;
  mp4_u32  i = 0;
  mp4_u32  left;
  mp4_u32  bytestoread;


  buf = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE);
  if (buf == NULL)
    return -1;

  buf2 = (mp4_u8 *)mp4malloc(METADATA_COPY_BUFFERSIZE / 2);
  if (buf2 == NULL)
    {
    mp4free(buf);
    return -1;
    }

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->sdtp);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_SDTP);
  i += 4;

  /* Version and flags */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  if (writeFile(handle, buf, i) < 0)
  {
    mp4free(buf);
    mp4free(buf2);

    return -1;
  }

  /* Sample count and delta */

  /* Seek to the beginning of temporary files */

  if (seekMetaDataFileNumAbs(handle, 0, METADATAFILE_VIDEO_SDTP_SAMPLE_DEPENDENCY) < 0)
      {
      mp4free(buf);
      mp4free(buf2);
      return -1;
      }

  left = handle->videoSampleTable->sdtpEntryCount; /* Bytes left in each file */

  while (left)
  {
    if (left > METADATA_COPY_BUFFERSIZE / 2)
      bytestoread = METADATA_COPY_BUFFERSIZE / 2;
    else
      bytestoread = left;

    if (readMetaDataFileNum(handle, buf2, bytestoread, METADATAFILE_VIDEO_SDTP_SAMPLE_DEPENDENCY) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    if (writeFile(handle, buf2, bytestoread) < 0)
    {
      mp4free(buf);
      mp4free(buf2);

      return -1;
    }

    left -= bytestoread;
  }

  mp4free(buf);
  mp4free(buf2);


  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 writeAVCP(MP4HandleImp handle,
 *                     trakSize *ts)
 *
 * Description:
 *
 *   Write AVCP atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *   ts       Atom sizes
 *
 * Return value:
 *
 *   0        Success
 *   -1       Error
 *
 */
mp4_i32 writeAVCP(MP4HandleImp handle, trakSize *ts)
{
  mp4_u8  *buf;
  mp4_u32  i = 0;


  buf = (mp4_u8 *)mp4malloc(86+7); // 86 = size of VisualSampleEntry, 7 = size of AVCDecoderConfigurationRecord
  								 // with PPS and SPS sizes set to 0
  if (buf == NULL)
    return -1;

  /* Size */
  insertu32(buf+i, (mp4_u32)ts->avcp);
  i += 4;

  /* Atom type */
  insertu32(buf+i, (mp4_u32)ATOMTYPE_AVCP);
  i += 4;

  /* Reserved */
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;
  buf[i++] = 0;

  /* Data reference index */
  insertu16(buf+i, (mp4_u16)1);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  /* Width */
  insertu16(buf+i, (mp4_u16)handle->videoWidth);
  i += 2;

  /* Height */
  insertu16(buf+i, (mp4_u16)handle->videoHeight);
  i += 2;

  /* Reserved */
  insertu32(buf+i, (mp4_u32)0x00480000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0x00480000);
  i += 4;

  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)1);
  i += 2;

// Compressorname
  buf[i++] = 14;
  insertu32(buf+i, (mp4_u32)0x41564320); // "AVC "
  i += 4;
  insertu32(buf+i, (mp4_u32)0x70617261); // "para"
  i += 4;
  insertu32(buf+i, (mp4_u32)0x6d657465); // "mete"
  i += 4;
  insertu16(buf+i, (mp4_u16)0x7273); // "rs"
  i += 2;
  buf[i++] = 0;
  insertu32(buf+i, (mp4_u32)0);
  i += 4;
  insertu32(buf+i, (mp4_u32)0);
  i += 4;
  insertu32(buf+i, (mp4_u32)0);
  i += 4;
  insertu32(buf+i, (mp4_u32)0);
  i += 4;

  insertu16(buf+i, (mp4_u16)24);
  i += 2;

  insertu16(buf+i, (mp4_u16)0xffff);
  i += 2;

  // AVCC without picparams & seqparams
  mp4memcpy(buf+i, handle->videoDecSpecificInfo, 5); // 5 = configurationVersion + AVCProfileIndication +
													 //     profile_compatibility + AVCLevelIndication +
													 //     '111111' + lengthSizeMinusOne (2 bits)
  i += 5;
  buf[i++] = 0xE0 | 0; // '111' + numOfSequenceParameterSets (5 bits)
  buf[i++] = 0;        // numOfPictureParameterSets
  
  if (writeFile(handle, buf, 86+7) < 0)
  {
    mp4free(buf);

    return -1;
  }
  
  mp4free(buf);

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 formatMdatHeader(mp4_u8 *buffer, 
 *                            mp4_u64 size)
 *
 * Description:
 *
 *   Formats the MDAT header into buffer
 *
 * Parameters:
 *
 *   buffer   buffer to write the MDAT header into
 *   size     Size of the media data (excluding the MDAT header)
 *
 * Return value:
 *
 *   size of header
 *
 */
mp4_i32 formatMdatHeader(mp4_u8 *buf, mp4_u64 size)
{
  TInt i = 0;
  if (size < MP4_INT_MAX)
  {
  	//insert free box
    insertu32(buf, (mp4_u32)8);
    i += sizeof(mp4_u32);
    insertu32(buf + i, (mp4_u32)ATOMTYPE_FREE);
    i += sizeof(mp4_u32);
    
    //insert mdat box
    insertu32(buf + i, (mp4_u32)size + 8);
    i += sizeof(mp4_u32);
    insertu32(buf + i, (mp4_u32)ATOMTYPE_MDAT);
    i += sizeof(mp4_u32);
  }
  else
  {
    //insert mdat box
    insertu32(buf + i, (mp4_u32)1);
    i += sizeof(mp4_u32);
    insertu32(buf + i, (mp4_u32)ATOMTYPE_MDAT);
    i += sizeof(mp4_u32);
	insertu64(buf + i, (mp4_u64)size + 16);        
	i += sizeof(mp4_u64);
  }

  ASSERT(MDAT_HEADER_SIZE == i);  
  return i;
}
	
// End of File
