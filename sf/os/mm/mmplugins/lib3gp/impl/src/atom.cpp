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
#include "mp4buffer.h"
#include "mp4endian.h"
#include "mp4file.h"
#include "mp4utils.h"

// MACROS
// Debug print macro
#ifdef _DEBUG
#include <e32svr.h>
//#define PRINT(x) RDebug::Print x
#define PRINT(x)
#else
#define PRINT(x)
#endif

mp4_i32 readBoxHeader(MP4HandleImp handle, mp4_i64* size, mp4_u32* type);
 
/*
 * Function:
 *
 *   mp4_i32 metaDataAvailable(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function determines whether meta data is available for reading
 *   or not.
 *
 *   Meta data is available if the input is in a file.
 *
 *   When reading from a stream, meta data is considered available if
 *   it is in the beginning of the stream and the entire Moov atom has
 *   been received. FTYP atom is allowed before MOOV atom.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0                Meta data is not available because enough data has not
 *                    been inserted into the library
 *   1                Meta data is available
 *   Negative value   Meta data is not available because of fatal error
 *
 */
mp4_i32 metaDataAvailable(MP4HandleImp handle)
{
  mp4_u32 size;
  mp4_u32 type;


  /* Meta data is available if the input is in a file or if a complete file has been inputted as a stream*/

  if (handle->file)
    return 1;

  /* When reading from a stream, meta data is considered available if
     it is in the beginning of the stream and the entire MOOV atom has
     been received. FTYP atom is allowed before MOOV atom. */

  if (!handle->ftypRead)
  {
    if (peekData(handle, handle->buf, 8) < 0) /* 8 bytes are not available */
        return 0;

    size = u32endian(*((mp4_u32 *)handle->buf));
    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    if (type == ATOMTYPE_FTYP)
    {
      if (getBufferedBytes(handle) - handle->position < size) /* FTYP is not available */
        return 0;
      if ((handle->ftyp = (fileTypeAtom *)mp4malloc(sizeof(fileTypeAtom))) == NULL)
        return -100;
      if (readFTYP(handle, handle->ftyp) < 0)
        return -2;
    }
  }

  // Now the ftyp is read. No need to chedk MOOV presence for full files in the memory.
  if (handle->LastWriteDataCalled == MP4TRUE)
      return 1;

  if (handle->LastWriteDataCalled == MP4FALSE)
  {//Whole stream is not fed to the internal memory yet. 
   for (;;)
    {
      if (peekData(handle, handle->buf, 8) < 0)
        return 0;

      size = u32endian(*((mp4_u32 *)handle->buf));
      type = u32endian(*((mp4_u32 *)(handle->buf+4)));

      if (type == ATOMTYPE_MOOV)
        {
        if (getBufferedBytes(handle) - handle->absPosition >= size) /* Entire Moov is available */
            {
            return 1;        
            }
        else
            {
            return 0;
            }
        }

      if ((mp4_i32)size <= 0)
        return -1;

      handle->absPosition+=size;
    }
  }
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 readFTYP(MP4HandleImp handle,
 *                    fileTypeAtom *ftyp)
 *
 * Description:
 *
 *   This function parses one FTYP atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   ftyp               FTYP pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readFTYP(MP4HandleImp handle, fileTypeAtom *ftyp)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u32 n = 0;
  mp4_i32 compatiblebrandsize = 0;


  if ((ftyp->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, ftyp->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (ftyp->atomhdr->type != ATOMTYPE_FTYP)
    return -1;

  if (ftyp->atomhdr->size < 16) // 8(header)+8bytes needed for major and minor brand
    return -1;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  ftyp->majorBrand = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  ftyp->minorVersion = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  if (ftyp->atomhdr->size == (mp4_u32)totalbytesread)
    return totalbytesread;

  compatiblebrandsize = (mp4_i32)ftyp->atomhdr->size - totalbytesread;
  if ( compatiblebrandsize < 4 )   // at this point we must have at least 1 compatible brand
    return -1;
  if ( compatiblebrandsize > 20*4) // maximum of 20 compatible brands 4byte entries
    return -1;
  if ( compatiblebrandsize % 4 ) // must be able to divide by 4
    return -1;

  ftyp->compatibleBrands = (mp4_u32 *)mp4malloc( compatiblebrandsize );
  if (ftyp->compatibleBrands == NULL)
    return -1;

  while ((mp4_u32)totalbytesread < ftyp->atomhdr->size)
  {
    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;

    ftyp->compatibleBrands[n] = u32endian(*((mp4_u32 *)handle->buf));

    n++;
    totalbytesread += bytesread;
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMetaData(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function reads the meta data from the file/stream and stores
 *   the information in the atom structures available via handle.
 *
 * Parameters:
 *
 *   handle  MP4 library handle
 *
 * Return value:
 *
 *   Negative value   Error
 *   >= 0             Success. Value tells the number of bytes read.
 *
 */
mp4_i32 readMetaData(MP4HandleImp handle)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if (handle->file)
  {
    mp4_u64 size;
    mp4_u32 type;


    if (seekFileAbs(handle, 0) < 0)
      return -1;

    /* Seek to the start of FTYP atom */

    for (;;)
    {
      if (readBoxHeader(handle, &size, &type) <0)
        return -1;

      if (type == ATOMTYPE_FTYP)
      {
        /* Read FTYP */

        if ((handle->ftyp = (fileTypeAtom *)mp4malloc(sizeof(fileTypeAtom))) == NULL)
          return -100;
        bytesread = readFTYP(handle, handle->ftyp);
        if (bytesread < 0)
          return -3;
        totalbytesread += bytesread;

        break;
      }

      if (size <= 0)
        return -1;

      if (seekFile(handle, size) != 0)
        return -1;
    }

    if (seekFileAbs(handle, 0) < 0)
      return -1;

    /* Seek to the start of MOOV atom */
    for (;;)
    {
      if (readBoxHeader(handle, &size, &type) <0)
        return -1;

      if (type == ATOMTYPE_MOOV)
        break;

      if (size <= 0)
        return -1;

      if (seekFile(handle, size) != 0)
        return -1;
    }
  }

  // If all data of a file is in the memory and the file does not have MOOV box right after FTYP, 
  // then we need to seek for the location of the MOOV first

  if (handle->LastWriteDataCalled == MP4TRUE)
  { 
    mp4_u32 size;
    mp4_u32 type;
    // Seek until the beginning of MOOV box.
    for(;;)
    {
        if (peekData(handle, handle->buf, 8) < 0)
            return -1;

        size = u32endian(*((mp4_u32 *)handle->buf));
        type = u32endian(*((mp4_u32 *)(handle->buf+4)));
    
        if (type == ATOMTYPE_MOOV)
          break;

        if ((mp4_i32)size <= 0)
          return -1;

        if (discardData(handle, size) < 0)
          return -1;
    }
  }

  if ((handle->moov = (movieAtom *)mp4malloc(sizeof(movieAtom))) == NULL)
    return -100;

  bytesread = readMoov(handle, handle->moov);
  if (bytesread < 0)
    return -3;
  totalbytesread += bytesread;

  if (handle->moov->trakAudio)
  {
    mp4_u32 audiotype;
	mp4_i32 errorAudio = 0;

	errorAudio = determineAudioType(handle, &audiotype);
    if ( errorAudio == 0 )
    {
	   	handle->type |= audiotype;    	
    
	    /* Move to the beginning of the 1st audio frame */
	    switch (advanceAudioSample(handle, handle->moov->trakAudio))
	    {
	    case -1:
	      return -1;
	    case -2:
	      handle->audioLast = MP4TRUE;
	      break;
	    default:
	      break;
	    }
	}
	else if (errorAudio == -2)
	{
		handle->type |= audiotype;	
	}
	else
	{
		return -1;
	}
  }

  if (handle->moov->trakVideo)
  {
    mp4_u32 videotype;
    mp4_i32 errorVideo = 0;

	errorVideo = determineVideoType(handle, &videotype);
    if ( errorVideo == 0 )
    {
	   	handle->type |= videotype;    	
    
    	/* Move to the beginning of the 1st video frame */
	    switch (advanceVideoFrame(handle, handle->moov->trakVideo))
	    {
	    case -1:
	      return -1;
	    case -2:
	       handle->videoLast = MP4TRUE;
	      break;
	    default:
	      break;
	    }
	}
	else if (errorVideo == -2)
	{
	   	handle->type |= videotype;	
	}
	else
	{
		return -1;
	}
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMoov(MP4HandleImp handle,
 *                    movieAtom *moov)
 *
 * Description:
 *
 *   This function parses one MOOV atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   moov               MOOV pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMoov(MP4HandleImp handle, movieAtom *moov)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((moov->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, moov->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (moov->atomhdr->type != ATOMTYPE_MOOV)
    return -1;

  while ((mp4_u32)totalbytesread < handle->moov->atomhdr->size)
  {
    mp4_u32 type;


    if (peekData(handle, handle->buf, 8) < 0)
      return -1;

    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    switch (type)
    {
    case ATOMTYPE_MVHD:

      if (moov->mvhd) /* MVHD has already been read, more than one is not allowed */
        return -1;

      if ((moov->mvhd = (movieHeaderAtom *)mp4malloc(sizeof(movieHeaderAtom))) == NULL)
        return -100;

      bytesread = readMVHD(handle, moov->mvhd);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_IODS:

      if (moov->iods) /* IODS has already been read, more than one is not allowed */
        return -1;

      if ((moov->iods = (objectDescriptorAtom *)mp4malloc(sizeof(objectDescriptorAtom))) == NULL)
        return -100;

      bytesread = readIODS(handle, moov->iods);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_TRAK:

      {
        trackAtom *ta;

        if ((ta = (trackAtom *)mp4malloc(sizeof(trackAtom))) == NULL)
          return -100;

        bytesread = readTRAK(handle, ta);
        if (bytesread < 0)
        {
          if (freeTRAK(ta) < 0)
            return -1;
          return -1;
        }
        totalbytesread += bytesread;

        if (!ta->mdia)
        {
          if (freeTRAK(ta) < 0)
            return -1;
          return -1;
        }
        if (!ta->mdia->hdlr)
        {
          if (freeTRAK(ta) < 0)
            return -1;
          return -1;
        }

        if (ta->mdia->hdlr->handlerType != HANDLERTYPE_VIDE &&
            ta->mdia->hdlr->handlerType != HANDLERTYPE_SOUN) /* Track is neither video nor audio */
        {
          /* Do nothing with the unknown track */
          if (freeTRAK(ta) < 0)
            return -1;
          break;
        }

        if (ta->mdia->hdlr->handlerType == HANDLERTYPE_VIDE)
        {
	        if (moov->trakVideo) /* Video Track already read */
	        {
	          if (freeTRAK(ta) < 0)
	            return -1;
	        }
	        else
	        {
	          moov->trakVideo = ta;
	        }
        }
		else if (ta->mdia->hdlr->handlerType == HANDLERTYPE_SOUN)
        {
	        if (moov->trakAudio) /* Audio Track already read */
	        {
	          if (freeTRAK(ta) < 0)
	            return -1;
	        }
	        else
	        {
	          moov->trakAudio = ta;
	        }
        }
        else
        {
        }
        break;
      }

    case ATOMTYPE_UDTA:

      {
      if (moov->udta) /* UDTA has already been read */
          {
          bytesread = readUnknown(handle);
          if (bytesread < 0)
            return -1;
          totalbytesread += bytesread;
          break;
          }


      if ((moov->udta = (userDataAtom *)mp4malloc(sizeof(userDataAtom))) == NULL)
        return -100;

      bytesread = readUDTA(handle, moov->udta);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;
      }
      
    case ATOMTYPE_META:
        
        {
        if (moov->meta) /* META has already been read, more than one is not allowed */
          return -1;
           
        if ((moov->meta = (metaAtom *)mp4malloc(sizeof(metaAtom))) == NULL)
            return -100;

        bytesread = readMeta(handle, moov->meta);
        if (bytesread < 0)
          return -1;
        totalbytesread += bytesread;
        
        break;
        }
        
    default:

      bytesread = readUnknown(handle);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;
    }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readAtomheader(MP4HandleImp handle,
 *                          atomHeader *ah)
 *
 * Description:
 *
 *   This function reads an atom header and stores the information
 *   in ah.
 *
 * Parameters:
 *
 *   handle           MP4 library handle
 *   ah               atomHeader structure that is used to store the
 *                    information
 *
 * Return value:
 *
 *   Negative value   Error
 *   >= 0             Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readAtomHeader(MP4HandleImp handle, atomHeader *ah)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;

  //PRINT((_L("readAtomHeader")));
  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  ah->size = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  ah->type = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  if (ah->size == 1)
  {
    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    ah->largeSize = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;
  }

  if (ah->type == ATOMTYPE_UUID)
  {
    bytesread = readData(handle, handle->buf, 16);
    if (bytesread < 0)
      return -1;
    mp4memcpy(ah->extendedType, handle->buf, 16);
    totalbytesread += bytesread;
  }

  //PRINT((_L("   size %u, size %Lu, type %c%c%c%c"), ah->size, ah->largeSize,((unsigned char *)(&ah->type))[3], ((unsigned char *)(&ah->type))[2], ((unsigned char *)(&ah->type))[1], ((unsigned char *)(&ah->type))[0]));
  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readFullAtomHeader(MP4HandleImp handle,
 *                              atomHeader *ah)
 *
 * Description:
 *
 *   This function reads a full atom header and stores the information
 *   in ah.
 *
 * Parameters:
 *
 *   handle           MP4 library handle
 *   ah               atomHeader structure that is used to store the
 *                    information
 *
 * Return value:
 *
 *   Negative value   Error
 *   >= 0             Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readFullAtomHeader(MP4HandleImp handle, atomHeader *ah)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;

  //PRINT((_L("readFullAtomHeader")));

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  ah->size = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  ah->type = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  ah->version = *(handle->buf);
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 3);
  if (bytesread < 0)
    return -1;
  mp4memcpy(ah->flags, handle->buf, 3);
  totalbytesread += bytesread;

  //PRINT((_L("   size %u, size %Lu, type %c%c%c%c"), ah->size, ah->largeSize,((unsigned char *)(&ah->type))[3], ((unsigned char *)(&ah->type))[2], ((unsigned char *)(&ah->type))[1], ((unsigned char *)(&ah->type))[0]));
  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMVHD(MP4HandleImp handle,
 *                    movieHeaderAtom *mvhd)
 *
 * Description:
 *
 *   This function parses one MVHD atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   mvhd               MVHD pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMVHD(MP4HandleImp handle, movieHeaderAtom *mvhd)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((mvhd->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, mvhd->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (mvhd->atomhdr->type != ATOMTYPE_MVHD)
    return -1;

  if (mvhd->atomhdr->version == 1) /* 64 bit */
  {
    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    mvhd->creationTime64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    mvhd->modificationTime64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mvhd->timeScale = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    mvhd->duration64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;
  }
  else /* 32 bit */
  {
    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mvhd->creationTime = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mvhd->modificationTime = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mvhd->timeScale = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mvhd->duration = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;
  }

  bytesread = discardData(handle, 76);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  mvhd->nextTrackID = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readIODS(MP4HandleImp handle,
 *                    objectDescriptorAtom *iods)
 *
 * Description:
 *
 *   This function parses one IODS atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   iods               IODS pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readIODS(MP4HandleImp handle, objectDescriptorAtom *iods)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((iods->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, iods->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (iods->atomhdr->type != ATOMTYPE_IODS)
    return -1;

  bytesread = discardData(handle, iods->atomhdr->size - totalbytesread);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readTRAK(MP4HandleImp handle,
 *                    trackAtom *trak)
 *
 * Description:
 *
 *   This function parses one TRAK atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   trak               TRAK pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readTRAK(MP4HandleImp handle, trackAtom *trak)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((trak->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, trak->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (trak->atomhdr->type != ATOMTYPE_TRAK)
    return -1;


  while ((mp4_u32)totalbytesread < trak->atomhdr->size)
  {
    mp4_u32 type;


    if (peekData(handle, handle->buf, 8) < 0)
      return -1;

    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    switch (type)
    {
    case ATOMTYPE_TKHD:

      if (trak->tkhd) /* MVHD has already been read, more than one is not allowed */
        return -1;

      if ((trak->tkhd = (trackHeaderAtom *)mp4malloc(sizeof(trackHeaderAtom))) == NULL)
        return -100;

      bytesread = readTKHD(handle, trak->tkhd);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_TREF:

      if (trak->tref) /* TREF has already been read, more than one is not allowed */
        return -1;

      if ((trak->tref = (trackReferenceAtom *)mp4malloc(sizeof(trackReferenceAtom))) == NULL)
        return -100;

      bytesread = readTREF(handle, trak->tref);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_EDTS:

      if (trak->edts) /* EDTS has already been read, more than one is not allowed */
        return -1;

      if ((trak->edts = (editListContainerAtom *)mp4malloc(sizeof(editListContainerAtom))) == NULL)
        return -100;

      bytesread = readEDTS(handle, trak->edts);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_MDIA:

      if (trak->mdia) /* MDIA has already been read, more than one is not allowed */
        return -1;

      if ((trak->mdia = (mediaAtom *)mp4malloc(sizeof(mediaAtom))) == NULL)
        return -100;

      bytesread = readMDIA(handle, trak->mdia);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_UDTA:
      {
      if (trak->udta) /* UDTA has already been read */
          {
          bytesread = readUnknown(handle);
          if (bytesread < 0)
            return -1;
          totalbytesread += bytesread;
          break;
          }

      if ((trak->udta = (userDataAtom *)mp4malloc(sizeof(userDataAtom))) == NULL)
        return -100;

      bytesread = readUDTA(handle, trak->udta);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;
      }

    default:

      bytesread = readUnknown(handle);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;
    }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readUnknown(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function reads one atom of unknown type. Atom contents are
 *   discarded.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readUnknown(MP4HandleImp handle)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u32 size;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  size = u32endian(*((mp4_u32 *)handle->buf));
  if ( size < 4 )
  {
  	return -1;
  }

  if ( handle->file )
  {
  	if ( seekFile(handle, size - totalbytesread) < 0 )
  	{
  	   return -1;	
  	}
  	else
  	{
  	   return size;	
  	}
  }
  else
  {
  	bytesread = discardData(handle, size - totalbytesread);
	  if (bytesread < 0)
    	return -1;
  	totalbytesread += bytesread;
  }
  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readTKHD(MP4HandleImp handle,
 *                    trackHeaderAtom *tkhd)
 *
 * Description:
 *
 *   This function parses one TKHD atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   tkhd               TKHD pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readTKHD(MP4HandleImp handle, trackHeaderAtom *tkhd)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((tkhd->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, tkhd->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (tkhd->atomhdr->type != ATOMTYPE_TKHD)
    return -1;

  if (tkhd->atomhdr->version == 1) /* 64 bit */
  {
    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    tkhd->creationTime64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    tkhd->modificationTime64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    tkhd->trackID = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    tkhd->reserved = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    tkhd->duration64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;
  }
  else /* 32 bit */
  {
    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    tkhd->creationTime = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    tkhd->modificationTime = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    tkhd->trackID = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    tkhd->reserved = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    tkhd->duration = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;
  }

  bytesread = discardData(handle, 52);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  tkhd->width = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  tkhd->height = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readTREF(MP4HandleImp handle,
 *                    trackReferenceAtom *tref)
 *
 * Description:
 *
 *   This function parses one TREF atom and discards the contents.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   tref               TREF pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readTREF(MP4HandleImp handle, trackReferenceAtom *tref)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((tref->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, tref->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (tref->atomhdr->type != ATOMTYPE_TREF)
    return -1;

  bytesread = discardData(handle, tref->atomhdr->size - totalbytesread);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readEDTS(MP4HandleImp handle,
 *                    editListContainerAtom *edts)
 *
 * Description:
 *
 *   This function parses one EDTS atom and discards the contents.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   edts               EDTS pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readEDTS(MP4HandleImp handle, editListContainerAtom *edts)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((edts->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, edts->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (edts->atomhdr->type != ATOMTYPE_EDTS)
    return -1;

  bytesread = discardData(handle, edts->atomhdr->size - totalbytesread);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMDIA(MP4HandleImp handle,
 *                    mediaAtom *mdia)
 *
 * Description:
 *
 *   This function parses one MDIA atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   mdia               MDIA pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMDIA(MP4HandleImp handle, mediaAtom *mdia)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((mdia->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, mdia->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (mdia->atomhdr->type != ATOMTYPE_MDIA)
    return -1;


  while ((mp4_u32)totalbytesread < mdia->atomhdr->size)
  {
    mp4_u32 type;


    if (peekData(handle, handle->buf, 8) < 0)
      return -1;

    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    switch (type)
    {
    case ATOMTYPE_MDHD:

      if (mdia->mdhd) /* MDHD has already been read, more than one is not allowed */
        return -1;

      if ((mdia->mdhd = (mediaHeaderAtom *)mp4malloc(sizeof(mediaHeaderAtom))) == NULL)
        return -100;

      bytesread = readMDHD(handle, mdia->mdhd);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_HDLR:

      if (mdia->hdlr) /* HDLR has already been read, more than one is not allowed */
        return -1;

      if ((mdia->hdlr = (handlerAtom *)mp4malloc(sizeof(handlerAtom))) == NULL)
        return -100;

      bytesread = readHDLR(handle, mdia->hdlr);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_MINF:

      if (mdia->minf) /* MINF has already been read, more than one is not allowed */
        return -1;

      if ((mdia->minf = (mediaInformationAtom *)mp4malloc(sizeof(mediaInformationAtom))) == NULL)
        return -100;

      bytesread = readMINF(handle, mdia->minf);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    default:

      bytesread = readUnknown(handle);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;
    }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMDHD(MP4HandleImp handle,
 *                    mediaHeaderAtom *mdhd)
 *
 * Description:
 *
 *   This function parses one MDHD atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   mdhd               MDHD pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMDHD(MP4HandleImp handle, mediaHeaderAtom *mdhd)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((mdhd->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, mdhd->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (mdhd->atomhdr->type != ATOMTYPE_MDHD)
    return -1;


  if (mdhd->atomhdr->version == 1) /* 64 bit */
  {
    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    mdhd->creationTime64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    mdhd->modificationTime64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mdhd->timeScale = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 8);
    if (bytesread < 0)
      return -1;
    mdhd->duration64 = u64endian(*((mp4_u64 *)handle->buf));
    totalbytesread += bytesread;
  }
  else /* 32 bit */
  {
    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mdhd->creationTime = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mdhd->modificationTime = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mdhd->timeScale = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;

    bytesread = readData(handle, handle->buf, 4);
    if (bytesread < 0)
      return -1;
    mdhd->duration = u32endian(*((mp4_u32 *)handle->buf));
    totalbytesread += bytesread;
  }

  bytesread = discardData(handle, 4);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readHDLR(MP4HandleImp handle,
 *                    handlerAtom *hdlr)
 *
 * Description:
 *
 *   This function parses one HDLR atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   hdlr               HDLR pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readHDLR(MP4HandleImp handle, handlerAtom *hdlr)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((hdlr->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, hdlr->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (hdlr->atomhdr->type != ATOMTYPE_HDLR)
    return -1;


  bytesread = discardData(handle, 4);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  hdlr->handlerType = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, hdlr->atomhdr->size - totalbytesread);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMINF(MP4HandleImp handle,
 *                    mediaInformationAtom *minf)
 *
 * Description:
 *
 *   This function parses one MINF atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   minf               MINF pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMINF(MP4HandleImp handle, mediaInformationAtom *minf)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((minf->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, minf->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (minf->atomhdr->type != ATOMTYPE_MINF)
    return -1;


  while ((mp4_u32)totalbytesread < minf->atomhdr->size)
  {
    mp4_u32 type;


    if (peekData(handle, handle->buf, 8) < 0)
      return -1;

    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    switch (type)
    {
    case ATOMTYPE_VMHD:

      if (minf->vmhd || minf->smhd) /* VMHD or SMHD has already been read, more than one is not allowed */
        return -1;

      if ((minf->vmhd = (videoMediaHeaderAtom *)mp4malloc(sizeof(videoMediaHeaderAtom))) == NULL)
        return -100;

      bytesread = readVMHD(handle, minf->vmhd);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_SMHD:

      if (minf->smhd || minf->vmhd) /* SMHD or VMHD has already been read, more than one is not allowed */
        return -1;

      if ((minf->smhd = (soundMediaHeaderAtom *)mp4malloc(sizeof(soundMediaHeaderAtom))) == NULL)
        return -100;

      bytesread = readSMHD(handle, minf->smhd);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_DINF:

      if (minf->dinf) /* DINF has already been read, more than one is not allowed */
        return -1;

      if ((minf->dinf = (dataInformationAtom *)mp4malloc(sizeof(dataInformationAtom))) == NULL)
        return -100;

      bytesread = readDINF(handle, minf->dinf);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_STBL:

      if (minf->stbl) /* STBL has already been read, more than one is not allowed */
        return -1;

      if ((minf->stbl = (sampleTableAtom *)mp4malloc(sizeof(sampleTableAtom))) == NULL)
        return -100;

      bytesread = readSTBL(handle, minf->stbl);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    default:

      bytesread = readUnknown(handle);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;
    }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readVMHD(MP4HandleImp handle,
 *                    videoMediaHeaderAtom *vmhd)
 *
 * Description:
 *
 *   This function parses one VMHD atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   vmhd               VMHD pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readVMHD(MP4HandleImp handle, videoMediaHeaderAtom *vmhd)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((vmhd->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, vmhd->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (vmhd->atomhdr->type != ATOMTYPE_VMHD)
    return -1;


  bytesread = discardData(handle, vmhd->atomhdr->size - totalbytesread);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSMHD(MP4HandleImp handle,
 *                    soundMediaHeaderAtom *smhd)
 *
 * Description:
 *
 *   This function parses one SMHD atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   smhd               SMHD pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSMHD(MP4HandleImp handle, soundMediaHeaderAtom *smhd)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((smhd->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, smhd->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (smhd->atomhdr->type != ATOMTYPE_SMHD)
    return -1;


  bytesread = discardData(handle, smhd->atomhdr->size - totalbytesread);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readDINF(MP4HandleImp handle,
 *                    dataInformationAtom *dinf)
 *
 * Description:
 *
 *   This function parses one DINF atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   dinf               DINF pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readDINF(MP4HandleImp handle, dataInformationAtom *dinf)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((dinf->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, dinf->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (dinf->atomhdr->type != ATOMTYPE_DINF)
    return -1;


  while ((mp4_u32)totalbytesread < dinf->atomhdr->size)
  {
    mp4_u32 type;


    if (peekData(handle, handle->buf, 8) < 0)
      return -1;

    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    switch (type)
    {
    case ATOMTYPE_DREF:

      if (dinf->dref) /* DINF has already been read, more than one is not allowed */
        return -1;

      if ((dinf->dref = (dataReferenceAtom *)mp4malloc(sizeof(dataReferenceAtom))) == NULL)
        return -100;

      bytesread = readDREF(handle, dinf->dref);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    default:

      return -1;
    }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readDREF(MP4HandleImp handle,
 *                    dataReferenceAtom *dref)
 *
 * Description:
 *
 *   This function parses one DREF atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   dref               DREF pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readDREF(MP4HandleImp handle, dataReferenceAtom *dref)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((dref->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, dref->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (dref->atomhdr->type != ATOMTYPE_DREF)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  dref->entryCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  if (dref->entryCount != 1)
    return -1;

  while ((mp4_u32)totalbytesread < dref->atomhdr->size)
  {
    bytesread = readUnknown(handle);
    if (bytesread < 0)
      return -1;
    totalbytesread += bytesread;
  }
  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readURL(MP4HandleImp handle,
 *                   dataEntryURLAtom *url)
 *
 * Description:
 *
 *   This function parses one URL atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   url                URL pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readURL(MP4HandleImp handle, dataEntryURLAtom *url)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((url->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, url->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (url->atomhdr->type != ATOMTYPE_URL)
    return -1;


  if (!(url->atomhdr->flags[0] == 0x00 &&
        url->atomhdr->flags[1] == 0x00 &&
        url->atomhdr->flags[2] == 0x01))
    return -1;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readURN(MP4HandleImp handle,
 *                   dataEntryURNAtom *urn)
 *
 * Description:
 *
 *   This function parses one URN atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   urn                URN pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readURN(MP4HandleImp handle, dataEntryURNAtom *urn)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((urn->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, urn->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (urn->atomhdr->type != ATOMTYPE_URN)
    return -1;


  if (!(urn->atomhdr->flags[0] == 0x00 &&
        urn->atomhdr->flags[1] == 0x00 &&
        urn->atomhdr->flags[2] == 0x01))
    return -1;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSTBL(MP4HandleImp handle,
 *                    sampleTableAtom *stbl)
 *
 * Description:
 *
 *   This function parses one STBL atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stbl               STBL pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSTBL(MP4HandleImp handle, sampleTableAtom *stbl)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((stbl->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, stbl->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (stbl->atomhdr->type != ATOMTYPE_STBL)
    return -1;


  while ((mp4_u32)totalbytesread < stbl->atomhdr->size)
  {
    mp4_u32 type;


    if (peekData(handle, handle->buf, 8) < 0)
      return -1;

    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    switch (type)
    {
    case ATOMTYPE_STTS:

      if (stbl->stts) /* STTS has already been read, more than one is not allowed */
        return -1;

      if ((stbl->stts = (timeToSampleAtom *)mp4malloc(sizeof(timeToSampleAtom))) == NULL)
        return -100;

      bytesread = readSTTS(handle, stbl->stts);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_CTTS:

      if (stbl->ctts) /* CTTS has already been read, more than one is not allowed */
        return -1;

      if ((stbl->ctts = (compositionTimeToSampleAtom *)mp4malloc(sizeof(compositionTimeToSampleAtom))) == NULL)
        return -100;

      bytesread = readCTTS(handle, stbl->ctts);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_STSS:

      if (stbl->stss) /* STSS has already been read, more than one is not allowed */
        return -1;

      if ((stbl->stss = (syncSampleAtom *)mp4malloc(sizeof(syncSampleAtom))) == NULL)
        return -100;

      bytesread = readSTSS(handle, stbl->stss);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_STSD:

      if (stbl->stsd) /* STSD has already been read, more than one is not allowed */
        return -1;

      if ((stbl->stsd = (sampleDescriptionAtom *)mp4malloc(sizeof(sampleDescriptionAtom))) == NULL)
        return -100;

      bytesread = readSTSD(handle, stbl->stsd);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_STSZ:

      if (stbl->stsz) /* STSZ or STZ2 has already been read, more than one is not allowed */
        return -1;

      if ((stbl->stsz = (sampleSizeAtom *)mp4malloc(sizeof(sampleSizeAtom))) == NULL)
        return -100;

      bytesread = readSTSZ(handle, stbl->stsz);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_STZ2:

      if (stbl->stsz) /* STSZ or STZ2 has already been read, more than one is not allowed */
        return -1;

      if ((stbl->stsz = (sampleSizeAtom *)mp4malloc(sizeof(sampleSizeAtom))) == NULL)
        return -100;

      bytesread = readSTZ2(handle, stbl->stsz);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_STSC:

      if (stbl->stsc) /* STSC has already been read, more than one is not allowed */
        return -1;

      if ((stbl->stsc = (sampleToChunkAtom *)mp4malloc(sizeof(sampleToChunkAtom))) == NULL)
        return -100;

      bytesread = readSTSC(handle, stbl->stsc);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_STCO:

      if (stbl->stco) /* STCO or CO64 has already been read, more than one is not allowed */
        return -1;

      if ((stbl->stco = (chunkOffsetAtom *)mp4malloc(sizeof(chunkOffsetAtom))) == NULL)
        return -100;
      
      stbl->is32BitOffsets = ETrue;
      bytesread = readSTCO(handle, stbl->stco);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_CO64:

      if (stbl->stco64) /* STCO or CO64 has already been read, more than one is not allowed */
        return -1;

      if ((stbl->stco64 = (chunkOffset64Atom *)mp4malloc(sizeof(chunkOffset64Atom))) == NULL)
        return -100;
      
      stbl->is32BitOffsets = EFalse;
      bytesread = readCO64(handle, stbl->stco64);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    case ATOMTYPE_SDTP:
      if (stbl->sdtp) /* SDTP has already been read, more than one is not allowed */
        return -1;
      
      if ((stbl->sdtp = (sampleDependencyAtom *)mp4malloc(sizeof(sampleDependencyAtom))) == NULL)
        return -100;

      if (!stbl->stsz)
    	  {
    	  return -1;
    	  }

	  // sample_count of SDTP is taken from the sample_count in the Sample Size Box ('stsz') or
	  // Compact Sample Size Box (‘stz2’). 
	  bytesread = readSDTP(handle, stbl->sdtp, stbl->stsz->sampleCount);
    	  
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;
      break;

    default: /* Other atoms are not needed */

      bytesread = readUnknown(handle);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;
    }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSTTS(MP4HandleImp handle,
 *                    timeToSampleAtom *stts)
 *
 * Description:
 *
 *   This function parses one STTS atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stts               STTS pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSTTS(MP4HandleImp handle, timeToSampleAtom *stts)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u32 i;


  if ((stts->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, stts->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (stts->atomhdr->type != ATOMTYPE_STTS)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  stts->entryCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;
  
  if ( stts->entryCount )
  {
	  stts->sampleCount = (mp4_u32 *)mp4malloc(stts->entryCount * sizeof(mp4_u32));
	  if (stts->sampleCount == NULL)
	    return -1;
	  stts->sampleDelta = (mp4_i32 *)mp4malloc(stts->entryCount * sizeof(mp4_i32));
	  if (stts->sampleDelta == NULL)
	    return -1;

	  for (i = 0; i < stts->entryCount; i++)
	  {
	    bytesread = readData(handle, handle->buf, 8);
	    if (bytesread < 0)
	      return -1;

	    stts->sampleCount[i] = u32endian(*((mp4_u32 *)handle->buf));
	    stts->sampleDelta[i] = i32endian(*((mp4_i32 *)(handle->buf+4)));

	    totalbytesread += bytesread;
	  }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readCTTS(MP4HandleImp handle,
 *                    compositionTimeToSampleAtom *ctts)
 *
 * Description:
 *
 *   This function parses one CTTS atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   ctts               CTTS pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readCTTS(MP4HandleImp handle, compositionTimeToSampleAtom *ctts)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u32 i;


  if ((ctts->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, ctts->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (ctts->atomhdr->type != ATOMTYPE_CTTS)
    return -1;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  ctts->entryCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  if ( ctts->entryCount )
  {
	  ctts->sampleCount = (mp4_u32 *)mp4malloc(ctts->entryCount * sizeof(mp4_u32));
	  if (ctts->sampleCount == NULL)
	    return -1;
	  ctts->sampleOffset = (mp4_u32 *)mp4malloc(ctts->entryCount * sizeof(mp4_u32));
	  if (ctts->sampleOffset == NULL)
	    return -1;

	  for (i = 0; i < ctts->entryCount; i++)
	  {
	    bytesread = readData(handle, handle->buf, 8);
	    if (bytesread < 0)
	      return -1;

	    ctts->sampleCount[i] = u32endian(*((mp4_u32 *)handle->buf));
	    ctts->sampleOffset[i] = u32endian(*((mp4_u32 *)(handle->buf+4)));

	    totalbytesread += bytesread;
	  }
  }
  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSTSS(MP4HandleImp handle,
 *                    syncSampleAtom *stss)
 *
 * Description:
 *
 *   This function parses one STSS atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stss               STSS pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSTSS(MP4HandleImp handle, syncSampleAtom *stss)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u32 i;


  if ((stss->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, stss->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (stss->atomhdr->type != ATOMTYPE_STSS)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  stss->entryCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  if ( stss->entryCount )
  {
	  stss->sampleNumber = (mp4_u32 *)mp4malloc(stss->entryCount * sizeof(mp4_u32));
	  if (stss->sampleNumber == NULL)
	    return -1;

	  for (i = 0; i < stss->entryCount; i++)
	  {
	    bytesread = readData(handle, handle->buf, 4);
	    if (bytesread < 0)
	      return -1;

	    stss->sampleNumber[i] = u32endian(*((mp4_u32 *)handle->buf));

	    totalbytesread += bytesread;
	  }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSTSD(MP4HandleImp handle,
 *                    sampleDescriptionAtom *stsd)
 *
 * Description:
 *
 *   This function parses one STSD atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stsd               STSD pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSTSD(MP4HandleImp handle, sampleDescriptionAtom *stsd)
	{
	mp4_i32 bytesread;
	mp4_i32 totalbytesread = 0;
	mp4_u32 totalsampleentriesread = 0;
	mp4_u32 unknownsampleentriesread = 0;
	mp4_bool skipentries = 0;

	if ((stsd->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
        {
        // for memory cleanup set entrycount to allocated num of entries.
        stsd->entryCount = totalsampleentriesread;
	    return -100;
        }

	bytesread = readFullAtomHeader(handle, stsd->atomhdr);
	if (bytesread < 0)
	    {
	    // for memory cleanup set entrycount to allocated num of entries.
	    stsd->entryCount = totalsampleentriesread;
	    return -1;
	    }
	totalbytesread += bytesread;

	if (stsd->atomhdr->type != ATOMTYPE_STSD)
	    {
	    // for memory cleanup set entrycount to allocated num of entries.
	    stsd->entryCount = totalsampleentriesread;
	    return -1;
	    }

	bytesread = readData(handle, handle->buf, 4);
	if (bytesread < 0)
	    {
	    // for memory cleanup set entrycount to allocated num of entries.
	    stsd->entryCount = totalsampleentriesread;
	    return -1;
	    }
	stsd->entryCount = u32endian(*((mp4_u32 *)handle->buf));
	totalbytesread += bytesread;

	mp4_u32 type;
	while ((mp4_u32)totalbytesread < stsd->atomhdr->size)
		{	
		// if the number of entries read already surpasses the number of entries specified 
		// within the STSD atom, the file is corrupted.
		if ((totalsampleentriesread + unknownsampleentriesread) >= stsd->entryCount)
			{
			// for memory cleanup set entrycount to allocated num of entries.
			stsd->entryCount = totalsampleentriesread;
  	  		return -1;    	    	
			}

		// read the next sample type
		if (peekData(handle, handle->buf, 8) < 0)
	        {
	        // for memory cleanup set entrycount to allocated num of entries.
	        stsd->entryCount = totalsampleentriesread;
		    return -1;
	        }
		type = u32endian(*((mp4_u32 *)(handle->buf+4)));
    
		// if the max sample entiries supported by the library has been reached 
		if ((stsd->entryCount > STSDMAXSAMPLEENTRYCOUNT) && (totalsampleentriesread == STSDMAXSAMPLEENTRYCOUNT))
			{
			// skip reading the rest of the entries to make sure no more than max count of sample entries 
			// will be processed, so that cleanup will always work. 
			type = 0;
			skipentries = 1;
			}

		switch (type)
			{
			case ATOMTYPE_MP4V:
				{
				if (stsd->mp4v[totalsampleentriesread]) /* MP4V[totalsampleentriesread] has already been read, more than one is not allowed */ 
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
		
				if ((stsd->mp4v[totalsampleentriesread] = (visualSampleEntry *)mp4malloc(sizeof(visualSampleEntry))) == NULL)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -100;
			        }
		
				bytesread = readMP4V(handle, stsd->mp4v[totalsampleentriesread]);
				totalsampleentriesread++;
				if (bytesread < 0)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				totalbytesread += bytesread;
				break;
				}

			case ATOMTYPE_MP4A:
				{	
				if (stsd->mp4a[totalsampleentriesread]) /* MP4A[totalsampleentriesread] has already been read, more than one is not allowed */
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }

				if ((stsd->mp4a[totalsampleentriesread] = (audioSampleEntry *)mp4malloc(sizeof(audioSampleEntry))) == NULL)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -100;
			        }

				bytesread = readMP4A(handle, stsd->mp4a[totalsampleentriesread]);
				totalsampleentriesread++;
				if (bytesread < 0)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				totalbytesread += bytesread;
				break;
				}

			case ATOMTYPE_MP4S:
				{
				if (stsd->mp4s[totalsampleentriesread]) /* MP4S has already been read, more than one is not allowed */
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }

				if ((stsd->mp4s[totalsampleentriesread] = (mpegSampleEntry *)mp4malloc(sizeof(mpegSampleEntry))) == NULL)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -100;
			        }

				bytesread = readMP4S(handle, stsd->mp4s[totalsampleentriesread]);
				totalsampleentriesread++;
				if (bytesread < 0)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				totalbytesread += bytesread;
				}
				break;

			case ATOMTYPE_S263:
				{
				if (stsd->s263[totalsampleentriesread]) /* MP4S has already been read, more than one is not allowed */
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }

				if ((stsd->s263[totalsampleentriesread] = (h263SampleEntry *)mp4malloc(sizeof(h263SampleEntry))) == NULL)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -100;
			        }

				bytesread = readS263(handle, stsd->s263[totalsampleentriesread]);
				totalsampleentriesread++;
				if (bytesread < 0)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				totalbytesread += bytesread;
				}
				break;

			case ATOMTYPE_SAMR:
				{
				if (stsd->samr[totalsampleentriesread]) /* SAMR has already been read, more than one is not allowed */
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }

				if ((stsd->samr[totalsampleentriesread] = (amrSampleEntry *)mp4malloc(sizeof(amrSampleEntry))) == NULL)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -100;
			        }

				bytesread = readSAMR(handle, stsd->samr[totalsampleentriesread]);
				totalsampleentriesread++;
				if (bytesread < 0)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				totalbytesread += bytesread;
				}
				break;

			case ATOMTYPE_SAWB:
				{
				if (stsd->sawb[totalsampleentriesread]) /* SAWB has already been read, more than one is not allowed */
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }

				if ((stsd->sawb[totalsampleentriesread] = (amrSampleEntry *)mp4malloc(sizeof(amrSampleEntry))) == NULL)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -100;
			        }

				bytesread = readSAWB(handle, stsd->sawb[totalsampleentriesread]);
				totalsampleentriesread++;
				if (bytesread < 0)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				totalbytesread += bytesread;
				}
				break;

			case ATOMTYPE_AVC1:
				{
				if (stsd->avc1[totalsampleentriesread]) /* AVC1 has already been read, more than one is not allowed */
					{
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
					}
				if ((stsd->avc1[totalsampleentriesread] = (avcSampleEntry *)mp4malloc(sizeof(avcSampleEntry))) == NULL)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -100;
			        }

				bytesread = readAVC1(handle, stsd->avc1[totalsampleentriesread]);
				totalsampleentriesread++;
				if (bytesread < 0)
					{
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				totalbytesread += bytesread;
				}
				break;

			case ATOMTYPE_SQCP:
				{
				if (stsd->sqcp[totalsampleentriesread]) /* SQCP has already been read, more than one is not allowed */
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				if ((stsd->sqcp[totalsampleentriesread] = (qcelpSampleEntry *)mp4malloc(sizeof(qcelpSampleEntry))) == NULL)
			        {
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -100;
			        }

				bytesread = readSQCP(handle, stsd->sqcp[totalsampleentriesread]);
				totalsampleentriesread++;
				if (bytesread < 0)
					{
			        // for memory cleanup set entrycount to allocated num of entries.
			        stsd->entryCount = totalsampleentriesread;
				    return -1;
			        }
				totalbytesread += bytesread;
				}
				break;

			default: /* Other atoms are not needed */
				// no need to increment totalsampleentriesread as no memory is allocated for unsupported 
				// or unrecognized sample types.  Alternatively, increment the count of unknown samples.   
				// This is for ensure if a non-audio/video track can properly be parsed without being 
				// recongized as an invalid format file.
				unknownsampleentriesread++;
				bytesread = readUnknown(handle);
				if (bytesread < 0)
		    		{
		    		// for memory cleanup set entrycount to allocated num of entries.
		    		stsd->entryCount = totalsampleentriesread;
		    		return -1;
		    		}
				totalbytesread += bytesread;
				break;
			}
		}

	// if the STSD atom's entry count is NOT the same as the number (supported & unsupported) entries parsed,
	// the atom is likely a corrupted one. 
	if ((totalsampleentriesread + unknownsampleentriesread) != stsd->entryCount)
		{
		// for memory cleanup set entrycount to allocated num of entries.
		stsd->entryCount = totalsampleentriesread;
		return -1;
		}
	else 
		{
		// if the STSD atom's entry count is the same as the number of (supported & unsupported) entries 
		// parsed, check if some entries are skipped because the max sample entry count has been reached  
		if (skipentries) 
			{
			// if STSDMAXSAMPLEENTRYCOUNT was reached edit entrycount to make sure cleanup works.
			stsd->entryCount = STSDMAXSAMPLEENTRYCOUNT;
			}
		else if (unknownsampleentriesread > 0)
			{
			// unknown (unsupported) sample entries present, set the STSD entry count to the actual 
			// number of supported sample entries detected
			stsd->entryCount = totalsampleentriesread;
			}
		}
  
	return totalbytesread;
	}


/*
 * Function:
 *
 *   mp4_i32 readSTSZ(MP4HandleImp handle,
 *                    sampleSizeAtom *stsz)
 *
 * Description:
 *
 *   This function parses one STSZ atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stsz               STSZ pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSTSZ(MP4HandleImp handle, sampleSizeAtom *stsz)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((stsz->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, stsz->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (stsz->atomhdr->type != ATOMTYPE_STSZ)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  stsz->sampleSize = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  stsz->sampleCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  // zero size samplesize means samples have different sizes, and those sizes are stored in sampleSizeEntries.
  if ((stsz->sampleCount) && (stsz->sampleSize == 0))
  {
    mp4_u32 i;

    // check validity of stsz->sampleCount before allocating entrysize table.
    if ( handle->moov->mvhd )
        {
        if ( handle->moov->mvhd->timeScale > 0 )
            {
            TUint magicNumberMaxSamplesInSec = MAXSAMPLESPERSECOND;
            TUint maxSampleCount;

            if ( handle->moov->mvhd->atomhdr->version == 1 ) // 64bit duration
                {
                maxSampleCount = I64INT(handle->moov->mvhd->duration64 / TUint(handle->moov->mvhd->timeScale) + 1) * magicNumberMaxSamplesInSec;
                }
            else    // 32bit duration
                {
                maxSampleCount = TUint((TUint( handle->moov->mvhd->duration  ) / TUint(handle->moov->mvhd->timeScale) + 1)) * magicNumberMaxSamplesInSec;
                }

            if ( maxSampleCount < stsz->sampleCount )
                {
                // corrupted 
                return -1;
                }
            }
        }

    // allocate stsz->entrySize table
    stsz->entrySize = (mp4_u32 *)mp4malloc(stsz->sampleCount * sizeof(mp4_u32));
    if (stsz->entrySize == NULL)
      return -1;

    for (i = 0; i < stsz->sampleCount; i++)
    {
      bytesread = readData(handle, handle->buf, 4);
      if (bytesread < 0)
        return -1;

      stsz->entrySize[i] = u32endian(*((mp4_u32 *)handle->buf));

      totalbytesread += bytesread;
    }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSTZ2(MP4HandleImp handle,
 *                    sampleSizeAtom *stsz)
 *
 * Description:
 *
 *   This function parses one STZ2 atom.
 *
 *   The result is placed in STSZ structure.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stsz               STSZ pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSTZ2(MP4HandleImp handle, sampleSizeAtom *stsz)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u8  fieldsize;


  if ((stsz->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, stsz->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (stsz->atomhdr->type != ATOMTYPE_STZ2)
    return -1;


  bytesread = discardData(handle, 3);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  fieldsize = handle->buf[0];
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  stsz->sampleCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  switch (fieldsize)
  {
  case 4: /* Two entries in each byte */

    {
      mp4_u32 i;

      stsz->entrySize = (mp4_u32 *)mp4malloc(stsz->sampleCount * sizeof(mp4_u32));
      if (stsz->entrySize == NULL)
        return -1;

      for (i = 0; i < (stsz->sampleCount + 1) / 2; i++)
      {
        bytesread = readData(handle, handle->buf, 1);
        if (bytesread < 0)
          return -1;

        totalbytesread += bytesread;

        stsz->entrySize[i * 2] = (mp4_u32)(handle->buf[0] >> 4);

        if (stsz->sampleCount % 2 == 0) /* Even number of samples */
        {
          stsz->entrySize[i * 2 + 1] = (mp4_u32)(handle->buf[0] & 0x0f);
          continue;
        }

        /* This condition is needed to avoid writing after the table */

        if (i == (stsz->sampleCount + 1) / 2 - 1) /* Last sample */
        {
        }
        else
          stsz->entrySize[i * 2 + 1] = (mp4_u32)(handle->buf[0] & 0x0f);
      }
    }

    break;

  case 8: /* One entry for each byte */

    {
      mp4_u32 i;

      stsz->entrySize = (mp4_u32 *)mp4malloc(stsz->sampleCount * sizeof(mp4_u32));
      if (stsz->entrySize == NULL)
        return -1;

      for (i = 0; i < stsz->sampleCount; i++)
      {
        bytesread = readData(handle, handle->buf, 1);
        if (bytesread < 0)
          return -1;

        stsz->entrySize[i] = (mp4_u32)handle->buf[0];

        totalbytesread += bytesread;
      }
    }

    break;

  case 16: /* Each entry in 2 bytes */

    {
      mp4_u32 i;

      stsz->entrySize = (mp4_u32 *)mp4malloc(stsz->sampleCount * sizeof(mp4_u32));
      if (stsz->entrySize == NULL)
        return -1;

      for (i = 0; i < stsz->sampleCount; i++)
      {
        bytesread = readData(handle, handle->buf, 2);
        if (bytesread < 0)
          return -1;

        stsz->entrySize[i] = (mp4_u32)u16endian(*((mp4_u16 *)handle->buf));

        totalbytesread += bytesread;
      }
    }

    break;

  default: /* Illegal fieldsize */

    return -1;
  }


  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSTSC(MP4HandleImp handle,
 *                    sampleToChunkAtom *stsc)
 *
 * Description:
 *
 *   This function parses one STSC atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stsc               STSC pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSTSC(MP4HandleImp handle, sampleToChunkAtom *stsc)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u32 i;


  if ((stsc->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, stsc->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (stsc->atomhdr->type != ATOMTYPE_STSC)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  stsc->entryCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  if (stsc->entryCount)
  {
	  stsc->firstChunk = (mp4_u32 *)mp4malloc(stsc->entryCount * sizeof(mp4_u32));
	  if (stsc->firstChunk == NULL)
	    return -1;
	  stsc->samplesPerChunk = (mp4_u32 *)mp4malloc(stsc->entryCount * sizeof(mp4_u32));
	  if (stsc->samplesPerChunk == NULL)
	    return -1;
	  stsc->sampleDescriptionIndex = (mp4_u32 *)mp4malloc(stsc->entryCount * sizeof(mp4_u32));
	  if (stsc->sampleDescriptionIndex == NULL)
	  {
		return -1;	  	
	  }

	  for (i = 0; i < stsc->entryCount; i++)
	  {
	    bytesread = readData(handle, handle->buf, 12);
	    if (bytesread < 0)
	      return -1;

	    stsc->firstChunk[i] = u32endian(*((mp4_u32 *)handle->buf));
	    stsc->samplesPerChunk[i] = u32endian(*((mp4_u32 *)(handle->buf+4)));
	    stsc->sampleDescriptionIndex[i] = u32endian(*((mp4_u32 *)(handle->buf+8)));
	    if ( stsc->sampleDescriptionIndex[i] > stsc->entryCount)
	    {
	    	return -1;
	    }

	    totalbytesread += bytesread;
	  }
  }
  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSTCO(MP4HandleImp handle,
 *                    chunkOffsetAtom *stco)
 *
 * Description:
 *
 *   This function parses one STCO atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stco               STCO pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSTCO(MP4HandleImp handle, chunkOffsetAtom *stco)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u32  i;


  if ((stco->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, stco->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (stco->atomhdr->type != ATOMTYPE_STCO)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  stco->entryCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  if (stco->entryCount)
  {
	  // validate stco->entryCount before allocating chunkOffsetTable
	  if ( handle->moov->mvhd )
	    {
	    if ( handle->moov->mvhd->timeScale > 0 )
	        {
	        TUint magicNumberMaxSamplesInSec = MAXSAMPLESPERSECOND;
	        TUint maxSampleCount;

	        if ( handle->moov->mvhd->atomhdr->version == 1 ) // 64bit duration
	            {
	            maxSampleCount = I64INT(handle->moov->mvhd->duration64 / TUint(handle->moov->mvhd->timeScale) + 1) * magicNumberMaxSamplesInSec;
	            }
	        else    // 32bit duration
	            {
	            maxSampleCount = TUint((TUint( handle->moov->mvhd->duration  ) / TUint(handle->moov->mvhd->timeScale) + 1)) * magicNumberMaxSamplesInSec;
	            }

	        if ( maxSampleCount < stco->entryCount )
	            {
	            // corrupted 
	            return -1;
	            }
	        }
	    }

	  stco->chunkOffset = (mp4_u32 *)mp4malloc(stco->entryCount * sizeof(mp4_u32));
	  if (stco->chunkOffset == NULL)
	    return -1;

	  for (i = 0; i < stco->entryCount; i++)
	  {
	    bytesread = readData(handle, handle->buf, 4);
	    if (bytesread < 0)
	      return -1;

	    stco->chunkOffset[i] = u32endian(*((mp4_u32 *)handle->buf));

	    totalbytesread += bytesread;
	  }
  }
  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readCO64(MP4HandleImp handle,
 *                    chunkOffset64Atom *stco64)
 *
 * Description:
 *
 *   This function parses one CO64 atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   stco64               CO64 pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readCO64(MP4HandleImp handle, chunkOffset64Atom *stco64)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_u32 i;


  if ((stco64->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, stco64->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (stco64->atomhdr->type != ATOMTYPE_CO64)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  stco64->entryCount = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  if ( stco64->entryCount )
  {
	  stco64->chunkOffset = (mp4_u64 *)mp4malloc(stco64->entryCount * sizeof(mp4_u64));
	  if (stco64->chunkOffset == NULL)
	    return -1;

	  for (i = 0; i < stco64->entryCount; i++)
	  {
	    bytesread = readData(handle, handle->buf, 8);
	    if (bytesread < 0)
	      return -1;
	    
	    stco64->chunkOffset[i] = u64endian(*((mp4_u64 *)(handle->buf)));

	    totalbytesread += bytesread;
	  }
  }
  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMP4V(MP4HandleImp handle,
 *                    visualSampleEntry *mp4v)
 *
 * Description:
 *
 *   This function parses one MP4V atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   mp4v               MP4V pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMP4V(MP4HandleImp handle, visualSampleEntry *mp4v)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((mp4v->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, mp4v->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (mp4v->atomhdr->type != ATOMTYPE_MP4V)
    return -1;


  bytesread = discardData(handle, 6);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  mp4v->dataReferenceIndex = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 16);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  mp4v->width = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  mp4v->height = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 50);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if ((mp4v->esd = (ESDAtom *)mp4malloc(sizeof(ESDAtom))) == NULL)
    return -100;

  bytesread = readESD(handle, mp4v->esd);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;
  
  if ( totalbytesread < mp4v->atomhdr->size )
  	{
    bytesread = discardData(handle, mp4v->atomhdr->size - totalbytesread );
  	if (bytesread < 0)
  		{
    	return -1;	  		
  		}
  	totalbytesread += bytesread;
  	}  

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMP4A(MP4HandleImp handle,
 *                    audioSampleEntry *mp4a)
 *
 * Description:
 *
 *   This function parses one MP4A atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   mp4a               MP4A pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMP4A(MP4HandleImp handle, audioSampleEntry *mp4a)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((mp4a->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, mp4a->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (mp4a->atomhdr->type != ATOMTYPE_MP4A)
    return -1;


  bytesread = discardData(handle, 6);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  mp4a->dataReferenceIndex = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 16);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  mp4a->timeScale = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 2);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if ((mp4a->esd = (ESDAtom *)mp4malloc(sizeof(ESDAtom))) == NULL)
    return -100;

  bytesread = readESD(handle, mp4a->esd);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;
  
  if ( totalbytesread < mp4a->atomhdr->size )
  	{
    bytesread = discardData(handle, mp4a->atomhdr->size - totalbytesread );
  	if (bytesread < 0)
  		{
    	return -1;	  		
  		}
  	totalbytesread += bytesread;
  	}

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readMP4S(MP4HandleImp handle,
 *                    mpegSampleEntry *mp4s)
 *
 * Description:
 *
 *   This function parses one MP4S atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   mp4s               MP4S pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMP4S(MP4HandleImp handle, mpegSampleEntry *mp4s)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((mp4s->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, mp4s->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (mp4s->atomhdr->type != ATOMTYPE_MP4S)
    return -1;


  bytesread = discardData(handle, 6);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  mp4s->dataReferenceIndex = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  if ((mp4s->esd = (ESDAtom *)mp4malloc(sizeof(ESDAtom))) == NULL)
    return -100;

  bytesread = readESD(handle, mp4s->esd);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;
  
  if ( totalbytesread < mp4s->atomhdr->size )
  	{
    bytesread = discardData(handle, mp4s->atomhdr->size - totalbytesread );
  	if (bytesread < 0)
  		{
    	return -1;	  		
  		}
  	totalbytesread += bytesread;
  	}    

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readS263(MP4HandleImp handle,
 *                    h263SampleEntry *s263)
 *
 * Description:
 *
 *   This function parses one S263 atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   s263               S263 pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readS263(MP4HandleImp handle, h263SampleEntry *s263)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((s263->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, s263->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (s263->atomhdr->type != ATOMTYPE_S263)
    return -1;


  bytesread = discardData(handle, 6);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  s263->dataReferenceIndex = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 16);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  s263->width = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  s263->height = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 50);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if ((s263->d263 = (h263SpecificAtom *)mp4malloc(sizeof(h263SpecificAtom))) == NULL)
    return -100;

  bytesread = readD263(handle, s263->d263);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;
  
  if ( totalbytesread < s263->atomhdr->size )
  	{
    bytesread = discardData(handle, s263->atomhdr->size - totalbytesread );
  	if (bytesread < 0)
  		{
    	return -1;	  		
  		}
  	totalbytesread += bytesread;
  	}      

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSAMR(MP4HandleImp handle,
 *                    amrSampleEntry *samr)
 *
 * Description:
 *
 *   This function parses one SAMR atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   samr               SAMR pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSAMR(MP4HandleImp handle, amrSampleEntry *samr)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((samr->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, samr->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (samr->atomhdr->type != ATOMTYPE_SAMR)
    return -1;


  bytesread = discardData(handle, 6);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  samr->dataReferenceIndex = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 16);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  samr->timeScale = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 2);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if ((samr->damr = (amrDecSpecStruc *)mp4malloc(sizeof(amrDecSpecStruc))) == NULL)
    return -100;

  bytesread = readDAMR(handle, samr->damr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;
  
  if ( totalbytesread < samr->atomhdr->size )
  	{
    bytesread = discardData(handle, samr->atomhdr->size - totalbytesread );
  	if (bytesread < 0)
  		{
    	return -1;	  		
  		}
  	totalbytesread += bytesread;
  	}      

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readSAWB(MP4HandleImp handle,
 *                    amrSampleEntry *sawb)
 *
 * Description:
 *
 *   This function parses one SAWB atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   sawb               SAWB pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSAWB(MP4HandleImp handle, amrSampleEntry *sawb)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((sawb->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, sawb->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (sawb->atomhdr->type != ATOMTYPE_SAWB)
    return -1;


  bytesread = discardData(handle, 6);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  sawb->dataReferenceIndex = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 16);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  sawb->timeScale = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 2);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if ((sawb->damr = (amrDecSpecStruc *)mp4malloc(sizeof(amrDecSpecStruc))) == NULL)
    return -100;

  bytesread = readDAMR(handle, sawb->damr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;
  
  if ( totalbytesread < sawb->atomhdr->size )
  	{
    bytesread = discardData(handle, sawb->atomhdr->size - totalbytesread );
  	if (bytesread < 0)
  		{
    	return -1;	  		
  		}
  	totalbytesread += bytesread;
  	}      

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readESD(MP4HandleImp handle,
 *                   ESDAtom *esd)
 *
 * Description:
 *
 *   This function parses one ESD atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   esd                ESD pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readESD(MP4HandleImp handle, ESDAtom *esd)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;
  mp4_i32 decConfDescrBytesRead = 0;


  if ((esd->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readFullAtomHeader(handle, esd->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (esd->atomhdr->type != ATOMTYPE_ESD)
    return -1;


  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  esd->esDescrTag = handle->buf[0];
  totalbytesread += bytesread;
  if (esd->esDescrTag != 3) /* ES_DescrTag == 3 */
    return -1;

  esd->size = 0;
  do
  {
    mp4_u8 c;

    bytesread = readData(handle, handle->buf, 1);
    if (bytesread < 0)
      return -1;
    c = (mp4_u8)(handle->buf[0] & 0x7f);
    esd->size = (esd->size << 7) | c;
    totalbytesread += bytesread;
  }
  while (handle->buf[0] & 0x80);

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  esd->ESID = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  esd->flags = handle->buf[0];
  totalbytesread += bytesread;

  if (esd->flags & 0x80) /* Stream Dependence flag has been set */
  {
    bytesread = readData(handle, handle->buf, 2);
    if (bytesread < 0)
      return -1;
    esd->dependsOnESID = u16endian(*((mp4_u16 *)handle->buf));
    totalbytesread += bytesread;
  }

  if (esd->flags & 0x40) /* URL flag has been set */
  {
    bytesread = readData(handle, handle->buf, 1);
    if (bytesread < 0)
      return -1;
    esd->URLLength = handle->buf[0];
    totalbytesread += bytesread;

    bytesread = discardData(handle, esd->URLLength);
    if (bytesread < 0)
      return -1;
    totalbytesread += bytesread;
  }

  if (esd->flags & 0x20) /* OCR stream flag has been set */
  {
    bytesread = readData(handle, handle->buf, 2);
    if (bytesread < 0)
      return -1;
    esd->OCRESID = u16endian(*((mp4_u16 *)handle->buf));
    totalbytesread += bytesread;
  }

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  esd->decConfDescrTag = handle->buf[0];
  totalbytesread += bytesread;
  if (esd->decConfDescrTag != 4) /* DecoderConfigDescrTag == 4 */
    return -1;

  esd->decConfDescrSize = 0;
  do
  {
    mp4_u8 c;

    bytesread = readData(handle, handle->buf, 1);
    if (bytesread < 0)
      return -1;
    c = (mp4_u8)(handle->buf[0] & 0x7f);
    esd->decConfDescrSize = (esd->decConfDescrSize << 7) | c;
    totalbytesread += bytesread;
  }
  while (handle->buf[0] & 0x80);

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  esd->objectTypeIndication = handle->buf[0];
  totalbytesread += bytesread;
  decConfDescrBytesRead += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  esd->stream = handle->buf[0];
  totalbytesread += bytesread;
  decConfDescrBytesRead += bytesread;

  bytesread = readData(handle, handle->buf, 3);
  if (bytesread < 0)
    return -1;
  esd->bufferSizeDB = ((mp4_u32)handle->buf[0]) << 16 |
                      ((mp4_u32)handle->buf[1]) << 8 |
                      ((mp4_u32)handle->buf[2]);
  totalbytesread += bytesread;
  decConfDescrBytesRead += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  esd->maxBitrate = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;
  decConfDescrBytesRead += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  esd->avgBitrate = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;
  decConfDescrBytesRead += bytesread;

  if ((mp4_u32)decConfDescrBytesRead < esd->decConfDescrSize)
  {
    bytesread = readData(handle, handle->buf, 1);
    if (bytesread < 0)
      return -1;
    esd->decSpecificInfoTag = handle->buf[0];
    totalbytesread += bytesread;
    decConfDescrBytesRead += bytesread;
    if (esd->decSpecificInfoTag != 5) /* DecSpecificInfoTag == 5 */
    {
      bytesread = discardData(handle, esd->decConfDescrSize - decConfDescrBytesRead);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;
    }
    else
    {
      esd->decSpecificInfoSize = 0;
      do
      {
        mp4_u8 c;

        bytesread = readData(handle, handle->buf, 1);
        if (bytesread < 0)
          return -1;
        c = (mp4_u8)(handle->buf[0] & 0x7f);
        esd->decSpecificInfoSize = (esd->decSpecificInfoSize << 7) | c;
        totalbytesread += bytesread;
      }
      while (handle->buf[0] & 0x80);

      if(esd->decSpecificInfoSize)
      {
        if ((esd->decSpecificInfo = (mp4_u8 *)mp4malloc(esd->decSpecificInfoSize)) == NULL)
          return -100;

        bytesread = readData(handle, esd->decSpecificInfo, esd->decSpecificInfoSize);
        if (bytesread < 0)
          return -1;
        totalbytesread += bytesread;
      }
    }
  }

  bytesread = discardData(handle, esd->atomhdr->size - totalbytesread);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readD263(MP4HandleImp handle,
 *                    h263SpecificAtom *d263)
 *
 * Description:
 *
 *   This function parses one D263 atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   d263               D263 pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readD263(MP4HandleImp handle, h263SpecificAtom *d263)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((d263->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, d263->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (d263->atomhdr->type != ATOMTYPE_D263)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  d263->vendor = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  d263->decoderVersion = handle->buf[0];
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  d263->h263Level = handle->buf[0];
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  d263->h263Profile = handle->buf[0];
  totalbytesread += bytesread;

  /* Check for the bitrate atom */

  while ((mp4_u32)totalbytesread < d263->atomhdr->size)
  {
    mp4_u32 type;


    if (peekData(handle, handle->buf, 8) < 0)
      return -1;

    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    switch (type)
    {
    case ATOMTYPE_BITR:

      if (d263->bitr) /* BITR has already been read, more than one is not allowed */
        return -1;

      if ((d263->bitr = (bitrateAtom *)mp4malloc(sizeof(bitrateAtom))) == NULL)
        return -100;

      bytesread = readBITR(handle, d263->bitr);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;

    default: /* Other atoms are not needed */

      bytesread = readUnknown(handle);
      if (bytesread < 0)
        return -1;
      totalbytesread += bytesread;

      break;
    }
  }

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readBITR(MP4HandleImp handle,
 *                    bitrateAtom *d263)
 *
 * Description:
 *
 *   This function parses one BITR atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   bitr               BITR pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readBITR(MP4HandleImp handle, bitrateAtom *bitr)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((bitr->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, bitr->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (bitr->atomhdr->type != ATOMTYPE_BITR)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  bitr->avgBitrate = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  bitr->maxBitrate = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readDAMR(MP4HandleImp handle,
 *                    amrDecSpecStruc *damr)
 *
 * Description:
 *
 *   This function parses one DAMR atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   damr               DAMR pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readDAMR(MP4HandleImp handle, amrDecSpecStruc *damr)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((damr->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, damr->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (damr->atomhdr->type != ATOMTYPE_DAMR)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  damr->vendor = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  damr->decoderVersion = handle->buf[0];
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  damr->modeSet = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  damr->modeChangePeriod = handle->buf[0];
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  damr->framesPerSample = handle->buf[0];
  totalbytesread += bytesread;

  return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 freeFTYP(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function frees memory for FTYP atom.
 *
 * Parameters:
 *
 *   ftyp       FTYP atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeFTYP(fileTypeAtom *ftyp)
{
  if (ftyp)
  {
    if (freeAtomHeader(ftyp->atomhdr) < 0)
      return -1;
    if (ftyp->compatibleBrands)
      mp4free(ftyp->compatibleBrands);

    mp4free(ftyp);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeMOOV(movieAtom *moov)
 *
 * Description:
 *
 *   This function frees memory for MOOV atom.
 *
 * Parameters:
 *
 *   moov       MOOV atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMOOV(movieAtom *moov)
{
  if (moov)
  {
    if (freeAtomHeader(moov->atomhdr) < 0)
      return -1;
    if (freeMVHD(moov->mvhd) < 0)
      return -1;
    if (freeTRAK(moov->trakAudio) < 0)
      return -1;
    if (freeTRAK(moov->trakVideo) < 0)
      return -1;
    if (freeIODS(moov->iods) < 0)
      return -1;
    if (freeUDTA(moov->udta) < 0)
      return -1;
    if (freeMETA(moov->meta) < 0)
      return -1;

    mp4free(moov);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeAtomHeader(atomHeader *atomhdr)
 *
 * Description:
 *
 *   This function frees memory for atom header.
 *
 * Parameters:
 *
 *   atomhdr    atom header pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeAtomHeader(atomHeader *atomhdr)
{
  if (atomhdr)
    mp4free(atomhdr);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeMVHD(movieHeaderAtom *mvhd)
 *
 * Description:
 *
 *   This function frees memory for MVHD atom.
 *
 * Parameters:
 *
 *   mvhd       MVHD atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMVHD(movieHeaderAtom *mvhd)
{
  if (mvhd)
  {
    if (freeAtomHeader(mvhd->atomhdr) < 0)
      return -1;

    mp4free(mvhd);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeTRAK(trackAtom *trak)
 *
 * Description:
 *
 *   This function frees memory for TRAK atom.
 *
 * Parameters:
 *
 *   trak       TRAK atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeTRAK(trackAtom *trak)
{
  if (trak)
  {
    if (freeAtomHeader(trak->atomhdr) < 0)
      return -1;
    if (freeTKHD(trak->tkhd) < 0)
      return -1;
    if (freeTREF(trak->tref) < 0)
      return -1;
    if (freeEDTS(trak->edts) < 0)
      return -1;
    if (freeMDIA(trak->mdia) < 0)
      return -1;
    if (freeUDTA(trak->udta) < 0)
      return -1;

    mp4free(trak);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeTKHD(trackHeaderAtom *tkhd)
 *
 * Description:
 *
 *   This function frees memory for TKHD atom.
 *
 * Parameters:
 *
 *   tkhd       TKHD atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeTKHD(trackHeaderAtom *tkhd)
{
  if (tkhd)
  {
    if (freeAtomHeader(tkhd->atomhdr) < 0)
      return -1;

    mp4free(tkhd);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeTREF(trackReferenceAtom *tref)
 *
 * Description:
 *
 *   This function frees memory for TREF atom.
 *
 * Parameters:
 *
 *   tref       TREF atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeTREF(trackReferenceAtom *tref)
{
  if (tref)
  {
    if (freeAtomHeader(tref->atomhdr) < 0)
      return -1;

    mp4free(tref);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeEDTS(editListContainerAtom *edts)
 *
 * Description:
 *
 *   This function frees memory for EDTS atom.
 *
 * Parameters:
 *
 *   edts       EDTS atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeEDTS(editListContainerAtom *edts)
{
  if (edts)
  {
    if (freeAtomHeader(edts->atomhdr) < 0)
      return -1;

    mp4free(edts);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeMDIA(mediaAtom *mdia)
 *
 * Description:
 *
 *   This function frees memory for MDIA atom.
 *
 * Parameters:
 *
 *   mdia       MDIA atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMDIA(mediaAtom *mdia)
{
  if (mdia)
  {
    if (freeAtomHeader(mdia->atomhdr) < 0)
      return -1;
    if (freeMDHD(mdia->mdhd) < 0)
      return -1;
    if (freeHDLR(mdia->hdlr) < 0)
      return -1;
    if (freeMINF(mdia->minf) < 0)
      return -1;

    mp4free(mdia);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeMDHD(mediaHeaderAtom *mdhd)
 *
 * Description:
 *
 *   This function frees memory for MDHD atom.
 *
 * Parameters:
 *
 *   mdhd       MDHD atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMDHD(mediaHeaderAtom *mdhd)
{
  if (mdhd)
  {
    if (freeAtomHeader(mdhd->atomhdr) < 0)
      return -1;

    mp4free(mdhd);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeHDLR(handlerAtom *hdlr)
 *
 * Description:
 *
 *   This function frees memory for HDLR atom.
 *
 * Parameters:
 *
 *   hdlr       HDLR atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeHDLR(handlerAtom *hdlr)
{
  if (hdlr)
  {
    if (freeAtomHeader(hdlr->atomhdr) < 0)
      return -1;
    if (hdlr->name)
      mp4free(hdlr->name);

    mp4free(hdlr);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeMINF(mediaInformationAtom *minf)
 *
 * Description:
 *
 *   This function frees memory for MINF atom.
 *
 * Parameters:
 *
 *   minf       MINF atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMINF(mediaInformationAtom *minf)
{
  if (minf)
  {
    if (freeAtomHeader(minf->atomhdr) < 0)
      return -1;
    if (freeVMHD(minf->vmhd) < 0)
      return -1;
    if (freeSMHD(minf->smhd) < 0)
      return -1;
    if (freeDINF(minf->dinf) < 0)
      return -1;
    if (freeSTBL(minf->stbl) < 0)
      return -1;

    mp4free(minf);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeVMHD(videoMediaHeaderAtom *vmhd)
 *
 * Description:
 *
 *   This function frees memory for VMHD atom.
 *
 * Parameters:
 *
 *   vmhd       VMHD atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeVMHD(videoMediaHeaderAtom *vmhd)
{
  if (vmhd)
  {
    if (freeAtomHeader(vmhd->atomhdr) < 0)
      return -1;

    mp4free(vmhd);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSMHD(soundMediaHeaderAtom *smhd)
 *
 * Description:
 *
 *   This function frees memory for SMHD atom.
 *
 * Parameters:
 *
 *   smhd       SMHD atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSMHD(soundMediaHeaderAtom *smhd)
{
  if (smhd)
  {
    if (freeAtomHeader(smhd->atomhdr) < 0)
      return -1;

    mp4free(smhd);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeDINF(dataInformationAtom *dinf)
 *
 * Description:
 *
 *   This function frees memory for DINF atom.
 *
 * Parameters:
 *
 *   dinf       DINF atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeDINF(dataInformationAtom *dinf)
{
  if (dinf)
  {
    if (freeAtomHeader(dinf->atomhdr) < 0)
      return -1;
    if (freeDREF(dinf->dref) < 0)
      return -1;

    mp4free(dinf);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeDREF(dataReferenceAtom *dref)
 *
 * Description:
 *
 *   This function frees memory for DREF atom.
 *
 * Parameters:
 *
 *   dref       DREF atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeDREF(dataReferenceAtom *dref)
{
  if (dref)
  {
    if (freeAtomHeader(dref->atomhdr) < 0)
      return -1;
    if (freeURL(dref->url) < 0)
      return -1;
    if (freeURN(dref->urn) < 0)
      return -1;

    mp4free(dref);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeURL(dataEntryURLAtom *url)
 *
 * Description:
 *
 *   This function frees memory for URL atom.
 *
 * Parameters:
 *
 *   url        URL atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeURL(dataEntryURLAtom *url)
{
  if (url)
  {
    if (freeAtomHeader(url->atomhdr) < 0)
      return -1;

    mp4free(url);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeURN(dataEntryURNAtom *urn)
 *
 * Description:
 *
 *   This function frees memory for URN atom.
 *
 * Parameters:
 *
 *   urn        URN atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeURN(dataEntryURNAtom *urn)
{
  if (urn)
  {
    if (freeAtomHeader(urn->atomhdr) < 0)
      return -1;

    mp4free(urn);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSTBL(sampleTableAtom *stbl)
 *
 * Description:
 *
 *   This function frees memory for STBL atom.
 *
 * Parameters:
 *
 *   stbl       STBL atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTBL(sampleTableAtom *stbl)
{
  if (stbl)
  {
    if (freeAtomHeader(stbl->atomhdr) < 0)
      return -1;
    if (freeSTTS(stbl->stts) < 0)
      return -1;
    if (freeCTTS(stbl->ctts) < 0)
      return -1;
    if (freeSTSD(stbl->stsd) < 0)
      return -1;
    if (freeSTSZ(stbl->stsz) < 0)
      return -1;
    if (freeSTSC(stbl->stsc) < 0)
      return -1;
    if (stbl->is32BitOffsets)
    {
    
      if (freeSTCO(stbl->stco) < 0)
        return -1;
    }
    else
    {
    
      if (freeSTCO64(stbl->stco64) < 0)
        return -1;
    }
    if (freeSTSS(stbl->stss) < 0)
      return -1;
    if (freeSTSH(stbl->stsh) < 0)
      return -1;
    if (freeSDTP(stbl->sdtp) < 0)
      return -1;

    mp4free(stbl);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSTTS(timeToSampleAtom *stts)
 *
 * Description:
 *
 *   This function frees memory for STTS atom.
 *
 * Parameters:
 *
 *   stts       STTS atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTTS(timeToSampleAtom *stts)
{
  if (stts)
  {
    if (freeAtomHeader(stts->atomhdr) < 0)
      return -1;
    if (stts->sampleCount)
      mp4free(stts->sampleCount);
    if (stts->sampleDelta)
      mp4free(stts->sampleDelta);

    mp4free(stts);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeCTTS(compositionTimeToSampleAtom *ctts)
 *
 * Description:
 *
 *   This function frees memory for CTTS atom.
 *
 * Parameters:
 *
 *   ctts       CTTS atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeCTTS(compositionTimeToSampleAtom *ctts)
{
  if (ctts)
  {
    if (freeAtomHeader(ctts->atomhdr) < 0)
      return -1;
    if (ctts->sampleCount)
      mp4free(ctts->sampleCount);
    if (ctts->sampleOffset)
      mp4free(ctts->sampleOffset);

    mp4free(ctts);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSTSD(sampleDescriptionAtom *stsd)
 *
 * Description:
 *
 *   This function frees memory for STSD atom.
 *
 * Parameters:
 *
 *   stsd       STSD atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTSD(sampleDescriptionAtom *stsd)
{
  mp4_u32 sampleentrycount = 0;
  mp4_u32 entryindex;
  
  if (stsd)
  {
	sampleentrycount = stsd->entryCount;

    if (freeAtomHeader(stsd->atomhdr) < 0)
      return -1;
	for (entryindex = 0; entryindex < sampleentrycount; entryindex++)
	{
		if (freeMP4V(stsd->mp4v[entryindex]) < 0)
		  return -1;
		if (freeMP4A(stsd->mp4a[entryindex]) < 0)
		  return -1;
		if (freeMP4S(stsd->mp4s[entryindex]) < 0)
		  return -1;
		if (freeS263(stsd->s263[entryindex]) < 0)
		  return -1;
		if (freeSAMR(stsd->samr[entryindex]) < 0)
		  return -1;
		if (freeSAWB(stsd->sawb[entryindex]) < 0)
		  return -1;
		if (freeAVC1(stsd->avc1[entryindex]) < 0)
			return -1;
        if (freeSQCP(stsd->sqcp[entryindex]) < 0)
          return -1;
	}
    mp4free(stsd);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeMP4V(visualSampleEntry *mp4v)
 *
 * Description:
 *
 *   This function frees memory for MP4V atom.
 *
 * Parameters:
 *
 *   mp4v       MP4V atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMP4V(visualSampleEntry *mp4v)
{
  if (mp4v)
  {
    if (freeAtomHeader(mp4v->atomhdr) < 0)
      return -1;
    if (freeESD(mp4v->esd) < 0)
      return -1;

    mp4free(mp4v);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeESD(ESDAtom *esd)
 *
 * Description:
 *
 *   This function frees memory for ESD atom.
 *
 * Parameters:
 *
 *   esd        ESD atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeESD(ESDAtom *esd)
{
  if (esd)
  {
    if (freeAtomHeader(esd->atomhdr) < 0)
      return -1;
    if (esd->URLString)
      mp4free(esd->URLString);
    if (esd->decSpecificInfo)
      mp4free(esd->decSpecificInfo);

    mp4free(esd);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeMP4A(audioSampleEntry *mp4a)
 *
 * Description:
 *
 *   This function frees memory for MP4A atom.
 *
 * Parameters:
 *
 *   mp4a       MP4A atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMP4A(audioSampleEntry *mp4a)
{
  if (mp4a)
  {
    if (freeAtomHeader(mp4a->atomhdr) < 0)
      return -1;
    if (freeESD(mp4a->esd) < 0)
      return -1;

    mp4free(mp4a);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeMP4S(mpegSampleEntry *mp4s)
 *
 * Description:
 *
 *   This function frees memory for MP4S atom.
 *
 * Parameters:
 *
 *   mp4s       MP4S atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMP4S(mpegSampleEntry *mp4s)
{
  if (mp4s)
  {
    if (freeAtomHeader(mp4s->atomhdr) < 0)
      return -1;
    if (freeESD(mp4s->esd) < 0)
      return -1;

    mp4free(mp4s);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeS263(h263SampleEntry *s263)
 *
 * Description:
 *
 *   This function frees memory for S263 atom.
 *
 * Parameters:
 *
 *   s263       S263 atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeS263(h263SampleEntry *s263)
{
  if (s263)
  {
    if (freeAtomHeader(s263->atomhdr) < 0)
      return -1;
    if (freeD263(s263->d263) < 0)
      return -1;

    mp4free(s263);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeD263(h263SpecificAtom *d263)
 *
 * Description:
 *
 *   This function frees memory for D263 atom.
 *
 * Parameters:
 *
 *   d263       D263 atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeD263(h263SpecificAtom *d263)
{
  if (d263)
  {
    if (freeAtomHeader(d263->atomhdr) < 0)
      return -1;
    if (freeBITR(d263->bitr) < 0)
      return -1;

    mp4free(d263);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeBITR(BitrateAtom *bitr)
 *
 * Description:
 *
 *   This function frees memory for BITR atom.
 *
 * Parameters:
 *
 *   bitr       BITR atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeBITR(bitrateAtom *bitr)
{
  if (bitr)
  {
    if (freeAtomHeader(bitr->atomhdr) < 0)
      return -1;

    mp4free(bitr);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSAMR(amrSampleEntry *samr)
 *
 * Description:
 *
 *   This function frees memory for SAMR atom.
 *
 * Parameters:
 *
 *   samr       SAMR atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSAMR(amrSampleEntry *samr)
{
  if (samr)
  {
    if (freeAtomHeader(samr->atomhdr) < 0)
      return -1;
    if (freeDAMR(samr->damr) < 0)
      return -1;

    mp4free(samr);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSAWB(amrSampleEntry *sawb)
 *
 * Description:
 *
 *   This function frees memory for SAWB atom.
 *
 * Parameters:
 *
 *   sawb       SAWB atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSAWB(amrSampleEntry *sawb)
{
  if (sawb)
  {
    if (freeAtomHeader(sawb->atomhdr) < 0)
      return -1;
    if (freeDAMR(sawb->damr) < 0)
      return -1;

    mp4free(sawb);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeDAMR(amrDecSpecStruc *damr)
 *
 * Description:
 *
 *   This function frees memory for DAMR atom.
 *
 * Parameters:
 *
 *   damr       DAMR atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeDAMR(amrDecSpecStruc *damr)
{
  if (damr)
  {
    if (freeAtomHeader(damr->atomhdr) < 0)
      return -1;

    mp4free(damr);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSTSZ(sampleSizeAtom *stsz)
 *
 * Description:
 *
 *   This function frees memory for STSZ atom.
 *
 * Parameters:
 *
 *   stsz       STSZ atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTSZ(sampleSizeAtom *stsz)
{
  if (stsz)
  {
    if (freeAtomHeader(stsz->atomhdr) < 0)
      return -1;
    if (stsz->entrySize)
      mp4free(stsz->entrySize);

    mp4free(stsz);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSTSC(sampleToChunkAtom *stsc)
 *
 * Description:
 *
 *   This function frees memory for STSC atom.
 *
 * Parameters:
 *
 *   stsc       STSC atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTSC(sampleToChunkAtom *stsc)
{
  if (stsc)
  {
    if (freeAtomHeader(stsc->atomhdr) < 0)
      return -1;
    if (stsc->firstChunk)
      mp4free(stsc->firstChunk);
    if (stsc->samplesPerChunk)
      mp4free(stsc->samplesPerChunk);
    if (stsc->sampleDescriptionIndex)
      mp4free(stsc->sampleDescriptionIndex);

    mp4free(stsc);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSTCO(chunkOffsetAtom *stco)
 *
 * Description:
 *
 *   This function frees memory for STCO atom.
 *
 * Parameters:
 *
 *   stco       STCO atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTCO(chunkOffsetAtom *stco)
{
  if (stco)
  {
    if (freeAtomHeader(stco->atomhdr) < 0)
      return -1;
    if (stco->chunkOffset)
      mp4free(stco->chunkOffset);

    mp4free(stco);
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 freeSTCO64(chunkOffset64Atom *stco64)
 *
 * Description:
 *
 *   This function frees memory for STCO64 atom.
 *
 * Parameters:
 *
 *   stco64       STCO64 atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTCO64(chunkOffset64Atom *stco64)
{
  if (stco64)
  {
    if (freeAtomHeader(stco64->atomhdr) < 0)
      return -1;
    if (stco64->chunkOffset)
      mp4free(stco64->chunkOffset);

    mp4free(stco64);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSTSS(syncSampleAtom *stss)
 *
 * Description:
 *
 *   This function frees memory for STSS atom.
 *
 * Parameters:
 *
 *   stss       STSS atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTSS(syncSampleAtom *stss)
{
  if (stss)
  {
    if (freeAtomHeader(stss->atomhdr) < 0)
      return -1;
    if (stss->sampleNumber)
      mp4free(stss->sampleNumber);

    mp4free(stss);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSTSH(shadowSyncSampleAtom *stsh)
 *
 * Description:
 *
 *   This function frees memory for STSH atom.
 *
 * Parameters:
 *
 *   stsh       STSH atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSTSH(shadowSyncSampleAtom *stsh)
{
  if (stsh)
  {
    if (freeAtomHeader(stsh->atomhdr) < 0)
      return -1;

    mp4free(stsh);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 freeSDTP(sampleDependencyAtom *sdtp)
 *
 * Description:
 *
 *   This function frees memory for SDTP atom.
 *
 * Parameters:
 *
 *   sdtp       SDTP atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSDTP(sampleDependencyAtom *sdtp)
	{
	if (sdtp)
		{
		if (freeAtomHeader(sdtp->atomhdr) < 0)
			{
			return -1;
			}

		if (sdtp->dep)
			{
			mp4free(sdtp->dep);
			}		
    
		mp4free(sdtp);
		}

  	return 0;
	}

/*
 * Function:
 *
 *   mp4_i32 freeIODS(objectDescriptorAtom *iods)
 *
 * Description:
 *
 *   This function frees memory for IODS atom.
 *
 * Parameters:
 *
 *   iods       IODS atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeIODS(objectDescriptorAtom *iods)
{
  if (iods)
  {
    if (freeAtomHeader(iods->atomhdr) < 0)
      return -1;

    mp4free(iods);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 readUDTA(MP4HandleImp handle,
 *                    userDataAtom *udta)
 *
 * Description:
 *
 *   This function parses one UDTA atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   udta               UDTA pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readUDTA(MP4HandleImp handle, userDataAtom *udta)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;

  if ((udta->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, udta->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (udta->atomhdr->type != ATOMTYPE_UDTA)
    return -1;

  if ( handle->file )
      {
      udta->atomcontentloc = handle->diskReadBufStart + handle->diskReadBufPos;
      }
  else
      {
      udta->atomcontentloc = handle->absPosition;
      }

  if ( udta->atomhdr->size == 1 )
      {
      udta->atomcontentsize = I64INT(udta->atomhdr->largeSize) - (TInt)totalbytesread;
      }
  else
      {
      udta->atomcontentsize = (TInt)(udta->atomhdr->size - totalbytesread);
      }

  if ( handle->file )
  {
  	if ( seekFile(handle, udta->atomhdr->size - totalbytesread) < 0 )
  	{
  	   return -1;	
  	}
  	else
  	{
  	   return udta->atomhdr->size;	
  	}
  }
  else
  {
	  bytesread = discardData(handle, udta->atomhdr->size - totalbytesread);
	  if (bytesread < 0)
	    return -1;
	  totalbytesread += bytesread;
  }
  return totalbytesread;
}

/*
 * Function:
 *
 *   mp4_i32 freeUDTA(userDataAtom *udta)
 *
 * Description:
 *
 *   This function frees memory for UDTA atom.
 *
 * Parameters:
 *
 *   udta       UDTA atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeUDTA(userDataAtom *udta)
{
  if (udta)
  {
    if (freeAtomHeader(udta->atomhdr) < 0)
      return -1;
    mp4free(udta);
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineVideoLength(MP4HandleImp handle,
 *                                mp4_u32 *videolength)
 *
 * Description:
 *
 *   This function determines the length of video in milliseconds.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   videolength  Video length is returned here
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 determineVideoLength(MP4HandleImp handle, mp4_u32 *videolength)
{
  if (!handle->moov)
    return -1;

  if (!handle->moov->mvhd)
    return -1;

  if (!handle->moov->trakVideo)
    return -1;

  if (!handle->moov->trakVideo->tkhd)
    return -1;

  /* Is timescale set? */
  if (handle->moov->mvhd->timeScale == 0)
    return -1;

  *videolength = (mp4_u32)(((mp4_double)handle->moov->trakVideo->tkhd->duration / (mp4_double)handle->moov->mvhd->timeScale) * (mp4_double)1000);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineFrameRate(MP4HandleImp handle,
 *                              mp4_double *framerate)
 *
 * Description:
 *
 *   This function determines the frame rate of video.
 *
 *   Frame rate is calculated as the average frame rate of the entire video.
 *
 * Parameters:
 *
 *   handle     MP4 library handle
 *   framerate  Frame rate is returned here
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 determineFrameRate(MP4HandleImp handle, mp4_double *framerate)
{
  mp4_double numberofframes;
  mp4_double length;


  if (!handle)
    return -1;
  if (!handle->moov)
    return -1;
  if (!handle->moov->trakVideo)
    return -1;
  if (!handle->moov->trakVideo->mdia)
    return -1;
  if (!handle->moov->trakVideo->mdia->minf)
    return -1;
  if (!handle->moov->trakVideo->mdia->minf->stbl)
    return -1;
  if (!handle->moov->trakVideo->mdia->minf->stbl->stsz)
    return -1;
  if (!handle->moov->trakVideo->mdia->mdhd)
    return -1;


  	if (handle->moov->trakVideo->mdia->mdhd->timeScale == 0)
    	return -1;
  	
  	if (handle->moov->trakVideo->mdia->mdhd->duration == 0)
  		{
  		*framerate = 0;
  		}
	else
		{
  		numberofframes = (mp4_double)handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleCount;
  		length = (mp4_double)handle->moov->trakVideo->mdia->mdhd->duration /
           		 (mp4_double)handle->moov->trakVideo->mdia->mdhd->timeScale;

  		*framerate = numberofframes / length;			
		}    

  	return 0;
	}


/*
 * Function:
 *
 *   mp4_i32 determineVideoType(MP4HandleImp handle,
 *                              mp4_u32 *videotype)
 *
 * Description:
 *
 *   This function determines the video type of the MP4.
 *
 * Parameters:
 *
 *   handle     MP4 library handle
 *   videotype  Video type is returned here
 *
 * Return value:
 *
 *   0          Success
 *   -1			Error
 *	 -2			Unknown video type
 *
 */
mp4_i32 determineVideoType(MP4HandleImp handle, mp4_u32 *videotype)
{
  *videotype = MP4_TYPE_NONE;

  if (!handle->moov)
    return -1;

  if (!handle->moov->trakVideo)
    return -1;

  if (!handle->moov->trakVideo->mdia)
    return -1;

  if (!handle->moov->trakVideo->mdia->minf)
    return -1;

  if (!handle->moov->trakVideo->mdia->minf->stbl)
    return -1;

  if (!handle->moov->trakVideo->mdia->minf->stbl->stsd)
    return -1;
  
  if (handle->moov->trakVideo->mdia->minf->stbl->stsd->entryCount == 0)
    return -1;  

/* Assume that the video type is the same for all sample entries. Just get the video type from the first one */

  if (handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[0])
  {
    if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[0]->esd)
      return -1;

    if ((handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[0]->esd->objectTypeIndication == 0x20) &&
        ((handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[0]->esd->stream >> 2) == 0x04))
      *videotype = MP4_TYPE_MPEG4_VIDEO;
  }
  else
  if (handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0])
  {
    if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0]->d263)
      return -1;

    switch (handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0]->d263->h263Profile)
    {
    case 0:
      *videotype = MP4_TYPE_H263_PROFILE_0;
      break;
    case 3:
      *videotype = MP4_TYPE_H263_PROFILE_3;
      break;
    default:
      break;
    }
  }
  else
   if (handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0])
   {
	   if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->avcc)
		   return -1;

	   if (handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->avcc->avcConfigSize == 0)
		   return -1;

	   /* AVC profile is in the second byte of the avcconfigrecord */
	   switch((mp4_u8)handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->avcc->avcConfig[1]) 
	   {
	   case 66:
		   *videotype = MP4_TYPE_AVC_PROFILE_BASELINE;
		   break;
	   case 77:
		   *videotype = MP4_TYPE_AVC_PROFILE_MAIN;
		   break;
	   case 88:
		   *videotype = MP4_TYPE_AVC_PROFILE_EXTENDED;
		   break;
       case 100:
          *videotype = MP4_TYPE_AVC_PROFILE_HIGH;
           break;
	   default:
		   {
		   mp4_u8 constraintByte = (mp4_u8)handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->avcc->avcConfig[2];
		   if ( (constraintByte & 0x80) || (constraintByte & 0x10) )
			   {
			   *videotype = MP4_TYPE_AVC_PROFILE_BASELINE;	
			   }
		   else if (constraintByte & 0x40)
			   {
			   *videotype = MP4_TYPE_AVC_PROFILE_MAIN;	
			   }
		   else if (constraintByte & 0x20)
			   {
			   *videotype = MP4_TYPE_AVC_PROFILE_EXTENDED;
			   }
		   // NOTE: Cannot reliably determine higher profiles from
		   // the constraint flags.
		   break;	   	
		   }
	   }
	}
   else
       {}
   /* Note: Read the AVC level and recreate the actual AVC profile and level in the future! */
  if (*videotype == MP4_TYPE_NONE)
    return -2;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineVideoResolution(MP4HandleImp handle,
 *                                    mp4_u32 *videowidth,
 *                                    mp4_u32 *videoheight)
 *
 * Description:
 *
 *   This function finds out the video width and height from the atom
 *   structure.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   videowidth   Video width is returned here
 *   videoheight  Video height is returned here
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 determineVideoResolution(MP4HandleImp handle, mp4_u32 *videowidth, mp4_u32 *videoheight)
{
  mp4_u32 videotype = MP4_TYPE_NONE;


  if (determineVideoType(handle, &videotype) < 0)
    return -1;

  if (videotype == MP4_TYPE_NONE)
    return -1;

  if (!handle->moov)
    return -1;

  if (!handle->moov->trakVideo)
    return -1;

  if (!handle->moov->trakVideo->mdia)
    return -1;

  if (!handle->moov->trakVideo->mdia->minf)
    return -1;

  if (!handle->moov->trakVideo->mdia->minf->stbl)
    return -1;

  if (!handle->moov->trakVideo->mdia->minf->stbl->stsd)
    return -1;

  if (handle->moov->trakVideo->mdia->minf->stbl->stsd->entryCount == 0)
    return -1;  
  
/* Assume that the video characteristics for all the video sample entries are the same. Just get them from the first one */

  if (videotype == MP4_TYPE_H263_PROFILE_0 ||
      videotype == MP4_TYPE_H263_PROFILE_3)
  {
    if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0])
      return -1;

    *videowidth  = handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0]->width;
    *videoheight = handle->moov->trakVideo->mdia->minf->stbl->stsd->s263[0]->height;
  }
  else if (videotype == MP4_TYPE_MPEG4_VIDEO)
  {
    if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[0])
      return -1;

    *videowidth  = handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[0]->width;
    *videoheight = handle->moov->trakVideo->mdia->minf->stbl->stsd->mp4v[0]->height;
  }
  else if ( isAvcVideo(videotype) )
  {
    if (!handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0])
      return -1;

    *videowidth  = handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->width;
    *videoheight = handle->moov->trakVideo->mdia->minf->stbl->stsd->avc1[0]->height;
  }
  else
  {
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineVideoTimeScale(MP4HandleImp handle,
 *                                   mp4_u32 *timescale)
 *
 * Description:
 *
 *   This function determines the timescale of video track.
 *
 * Parameters:
 *
 *   handle     MP4 library handle
 *   timescale  Timescale of video track is returned here
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 determineVideoTimeScale(MP4HandleImp handle, mp4_u32 *timescale)
{
  if (timescale == NULL)
    return 0;

  if (!handle)
    return -1;
  if (!handle->moov)
    return -1;
  if (!handle->moov->trakVideo)
    return -1;
  if (!handle->moov->trakVideo->mdia)
    return -1;
  if (!handle->moov->trakVideo->mdia->mdhd)
    return -1;
  if (handle->moov->trakVideo->mdia->mdhd->timeScale == 0)
    return -1;

  *timescale = handle->moov->trakVideo->mdia->mdhd->timeScale;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineAudioLength(MP4HandleImp handle,
 *                                mp4_u32 *audiolength)
 *
 * Description:
 *
 *   This function determines the length of audio in milliseconds.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   audiolength  Audio length is returned here
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 determineAudioLength(MP4HandleImp handle, mp4_u32 *audiolength)
{
  if (!handle->moov)
    return -1;

  if (!handle->moov->mvhd)
    return -1;

  if (!handle->moov->trakAudio)
    return -1;

  if (!handle->moov->trakAudio->tkhd)
    return -1;

  /* Is timescale set? */
  if (handle->moov->mvhd->timeScale == 0)
    return -1;

  *audiolength = (mp4_u32)(((mp4_double)handle->moov->trakAudio->tkhd->duration / (mp4_double)handle->moov->mvhd->timeScale) * (mp4_double)1000);

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineAudioType(MP4HandleImp handle,
 *                              mp4_u32 *audiotype)
 *
 * Description:
 *
 *   This function determines the audio type of the MP4.
 *
 * Parameters:
 *
 *   handle     MP4 library handle
 *   audiotype  Audio type is returned here
 *
 * Return value:
 *
 *   0          Success
 *   -1   		Error
 *   -2			Unknown audiotrack
 *
 */
mp4_i32 determineAudioType(MP4HandleImp handle, mp4_u32 *audiotype)
{
  *audiotype = MP4_TYPE_NONE;


  if (!handle->moov)
    return -1;

  if (!handle->moov->trakAudio)
    return -1;

  if (!handle->moov->trakAudio->mdia)
    return -1;

  if (!handle->moov->trakAudio->mdia->minf)
    return -1;

  if (!handle->moov->trakAudio->mdia->minf->stbl)
    return -1;

  if (!handle->moov->trakAudio->mdia->minf->stbl->stsd)
    return -1;
  
  if (handle->moov->trakAudio->mdia->minf->stbl->stsd->entryCount == 0)
    return -1;  

/* Assume that the audio type is the same for all sample entries. Just get the audio type from the first one */
  if (handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[0])
  {
    if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[0]->esd)
      return -1;

    if ((handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[0]->esd->objectTypeIndication == 0x40) &&
        ((handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[0]->esd->stream >> 2) == 0x05))
      *audiotype = MP4_TYPE_MPEG4_AUDIO;

   if ((handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[0]->esd->objectTypeIndication == 0xE1) &&
     ((handle->moov->trakAudio->mdia->minf->stbl->stsd->mp4a[0]->esd->stream >> 2) == 0x05))  
   {
      *audiotype = MP4_TYPE_QCELP_13K;
      handle->qcelpStoredAsMPEGAudio = MP4TRUE;
   }
  }
  else if (handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[0])
    *audiotype = MP4_TYPE_AMR_NB;
  else if (handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[0])
    *audiotype = MP4_TYPE_AMR_WB;
  else if (handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[0])
  {
    *audiotype = MP4_TYPE_QCELP_13K;
    handle->qcelpStoredAsMPEGAudio = MP4FALSE;
  }
  else
  {
  }

  if (*audiotype == MP4_TYPE_NONE)
    return -2;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineAudioFramesPerSample(MP4HandleImp handle,
 *                                         mp4_u8 *framespersample)
 *
 * Description:
 *
 *   This function determines the number of audio frames in each sample.
 *   The function works with AMR audio type only.
 *
 * Parameters:
 *
 *   handle           MP4 library handle
 *   framespersample  Number of frames in each sample
 *
 * Return value:
 *
 *   0                Success
 *   Negative         Error
 *
 */
mp4_i32 determineAudioFramesPerSample(MP4HandleImp handle, mp4_u8 *framespersample)
{
  mp4_i8 sampleentryindex;
  
  *framespersample = 0;


  if (!((handle->type & MP4_TYPE_AMR_NB) ||
        (handle->type & MP4_TYPE_AMR_WB) ||
        ((handle->type & MP4_TYPE_QCELP_13K) && (!handle->qcelpStoredAsMPEGAudio))))
  {
    *framespersample = 1;

    return 0;
  }


  if (!handle->moov)
    return -1;

  if (!handle->moov->trakAudio)
    return -1;

  if (!handle->moov->trakAudio->mdia)
    return -1;

  if (!handle->moov->trakAudio->mdia->minf)
    return -1;

  if (!handle->moov->trakAudio->mdia->minf->stbl)
    return -1;

  if (!handle->moov->trakAudio->mdia->minf->stbl->stsd)
    return -1;

  if (handle->type & MP4_TYPE_AMR_NB)
  {
/* Now, framespersample returns the maximum frames_per_sample listed in the AMR sample entries*/
	for (sampleentryindex = 0; (mp4_u8) sampleentryindex < handle->moov->trakAudio->mdia->minf->stbl->stsd->entryCount; sampleentryindex++)
	{
		if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[sampleentryindex])
		  return -1;

		if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[sampleentryindex]->damr)
		  return -1;

		if (*framespersample < handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[sampleentryindex]->damr->framesPerSample)
			*framespersample = handle->moov->trakAudio->mdia->minf->stbl->stsd->samr[sampleentryindex]->damr->framesPerSample;
	}
  }
  else if (handle->type & MP4_TYPE_AMR_WB)
  {
/* Now, framespersample returns the maximum frames_per_sample listed in the AMR sample entries*/
	for (sampleentryindex = 0; (mp4_u8)sampleentryindex < handle->moov->trakAudio->mdia->minf->stbl->stsd->entryCount; sampleentryindex++)
	{
		if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[sampleentryindex])
		  return -1;

		if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[sampleentryindex]->damr)
		  return -1;

		if (*framespersample < handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[sampleentryindex]->damr->framesPerSample)
 		  *framespersample = handle->moov->trakAudio->mdia->minf->stbl->stsd->sawb[sampleentryindex]->damr->framesPerSample;
	}
  }
  else if ((handle->type & MP4_TYPE_QCELP_13K) && (!handle->qcelpStoredAsMPEGAudio))
  {
/* Now, framespersample returns the maximum frames_per_sample listed in the QCELP-13K sample entries*/
	for (sampleentryindex = 0; (mp4_u8)sampleentryindex < handle->moov->trakAudio->mdia->minf->stbl->stsd->entryCount; sampleentryindex++)
	{
		if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[sampleentryindex])
		  return -1;

		if (!handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[sampleentryindex]->dqcp)
		  return -1;

		if (*framespersample < handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[sampleentryindex]->dqcp->framesPerSample)
		  *framespersample = handle->moov->trakAudio->mdia->minf->stbl->stsd->sqcp[sampleentryindex]->dqcp->framesPerSample;
	}
  }
  else
  {
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineAudioTimeScale(MP4HandleImp handle,
 *                                   mp4_u32 *timescale)
 *
 * Description:
 *
 *   This function determines the timescale of audio track.
 *
 * Parameters:
 *
 *   handle     MP4 library handle
 *   timescale  Timescale of audio track is returned here
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 determineAudioTimeScale(MP4HandleImp handle, mp4_u32 *timescale)
{
  if (timescale == NULL)
    return 0;

  if (!handle)
    return -1;
  if (!handle->moov)
    return -1;
  if (!handle->moov->trakAudio)
    return -1;
  if (!handle->moov->trakAudio->mdia)
    return -1;
  if (!handle->moov->trakAudio->mdia->mdhd)
    return -1;


  if (handle->moov->trakAudio->mdia->mdhd->timeScale == 0)
    return -1;

  *timescale = handle->moov->trakAudio->mdia->mdhd->timeScale;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineAudioAverageBitRate(MP4HandleImp handle,
 *                                        mp4_u32 *averagebitrate)
 *
 * Description:
 *
 *   This function determines the average bitrate of the audio in bits per
 *   second.
 *
 *   The average is calculated so that the audio data length is divided by
 *   the length of the  audio track.
 *
 * Parameters:
 *
 *   handle                MP4 library handle
 *   averagebitrate        Result is returned here
 *
 * Return value:
 *
 *   0           Success
 *   Negative    Error
 *
 */
mp4_i32 determineAudioAverageBitRate(MP4HandleImp handle, mp4_u32 *averagebitrate)
	{
	mp4_u32 audiosize = 0;

  	if (!handle->moov)
  		{
  		return -1;	
  		}
    	
  	if (handle->moov->trakAudio)
  		{
		if (!handle->moov->trakAudio->mdia)
			{
			return -1;		
			}
					
		if (!handle->moov->trakAudio->mdia->minf)
			{
			return -1;	
			}
		
		if (!handle->moov->trakAudio->mdia->minf->stbl)
			{
			return -1;				
			}

		if (!handle->moov->trakAudio->mdia->minf->stbl->stsz)
			{
			return -1;				
			}

		if (handle->moov->trakAudio->mdia->minf->stbl->stsz->sampleSize != 0)
			{
			audiosize += (handle->moov->trakAudio->mdia->minf->stbl->stsz->sampleCount *
	                   	 handle->moov->trakAudio->mdia->minf->stbl->stsz->sampleSize);			
			}
	    else
	    	{
	      	mp4_u32 i;
	      	for (i = 0; i < handle->moov->trakAudio->mdia->minf->stbl->stsz->sampleCount; i++)
	      		{
	        	audiosize += handle->moov->trakAudio->mdia->minf->stbl->stsz->entrySize[i];      			
	      		}
	    	}
	  	}
	else
		{
		return -1;  		
		}

  	if (!handle->moov->trakAudio->mdia->mdhd)
  		{
    	return -1;  			
  		}
  	
  	if (!handle->moov->trakAudio->mdia->mdhd->timeScale)
  		{
		return -1;  			
  		}

  	if (handle->moov->trakAudio->mdia->mdhd->duration == 0)
  		{
  		*averagebitrate = 0;
  		}
	else
		{
		*averagebitrate = (mp4_u32)((mp4_double)8 *
		                  (mp4_double)audiosize /
		                  ((mp4_double)handle->moov->trakAudio->mdia->mdhd->duration /
		                  (mp4_double)handle->moov->trakAudio->mdia->mdhd->timeScale));
		}  	

  	return 0;
	}


/*
 * Function:
 *
 *   mp4_i32 determineStreamSize(MP4HandleImp handle,
 *                              mp4_u32 *streamsize)
 *
 * Description:
 *
 *   This function determines the size of media data in MP4 file/stream.
 *
 * Parameters:
 *
 *   handle      MP4 library handle
 *   streamsize  Size of data
 *
 * Return value:
 *
 *   0           Success
 *   Negative    Error
 *
 */
mp4_i32 determineStreamSize(MP4HandleImp handle, mp4_u32 *streamsize)
{
  *streamsize = 0;

  if (!handle->moov)
    return -1;

  if ((!handle->moov->trakAudio) && (!handle->moov->trakVideo))
    return -1;

  if (handle->moov->trakAudio)
  {
    if (!handle->moov->trakAudio->mdia)
      return -1;

    if (!handle->moov->trakAudio->mdia->minf)
      return -1;

    if (!handle->moov->trakAudio->mdia->minf->stbl)
      return -1;

    if (!handle->moov->trakAudio->mdia->minf->stbl->stsz)
      return -1;

    if (handle->moov->trakAudio->mdia->minf->stbl->stsz->sampleSize != 0)
      *streamsize += handle->moov->trakAudio->mdia->minf->stbl->stsz->sampleCount *
                     handle->moov->trakAudio->mdia->minf->stbl->stsz->sampleSize;
    else
    {
      mp4_u32 i;

      for (i = 0; i < handle->moov->trakAudio->mdia->minf->stbl->stsz->sampleCount; i++)
        *streamsize += handle->moov->trakAudio->mdia->minf->stbl->stsz->entrySize[i];
    }
  }

  if (handle->moov->trakVideo)
  {
    if (!handle->moov->trakVideo->mdia)
      return -1;

    if (!handle->moov->trakVideo->mdia->minf)
      return -1;

    if (!handle->moov->trakVideo->mdia->minf->stbl)
      return -1;

    if (!handle->moov->trakVideo->mdia->minf->stbl->stsz)
      return -1;

    if (handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleSize != 0)
      *streamsize += handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleCount *
                     handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleSize;
    else
    {
      mp4_u32 i;

      for (i = 0; i < handle->moov->trakVideo->mdia->minf->stbl->stsz->sampleCount; i++)
        *streamsize += handle->moov->trakVideo->mdia->minf->stbl->stsz->entrySize[i];
    }
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 determineStreamAverageBitRate(MP4HandleImp handle,
 *                                         mp4_u32 *streamaveragebitrate,
 *                                         mp4_u32 streamsize)
 *
 * Description:
 *
 *   This function determines the average bitrate of the stream in bits per
 *   second.
 *
 *   The average is calculated so that the media data length is divided by
 *   the length of the presentation.
 *
 * Parameters:
 *
 *   handle                MP4 library handle
 *   streamaveragebitrate  Result is returned here
 *   streamsize            Size of media data in bytes
 *
 * Return value:
 *
 *   0           Success
 *   Negative    Error
 *
 */
mp4_i32 determineStreamAverageBitRate(MP4HandleImp handle, mp4_u32 *streamaveragebitrate, mp4_u32 streamsize)
{
  if (!handle->moov)
    return -1;

  if (!handle->moov->mvhd)
    return -1;

  if (!handle->moov->mvhd->timeScale)
    return -1;

  if (handle->moov->mvhd->atomhdr->version == 1) /* 64 bit */
      {
      if (!handle->moov->mvhd->duration64)
        return -1;
     
      *streamaveragebitrate = (mp4_u32)((mp4_double)8 * (mp4_double)streamsize /
                                        ((mp4_double)handle->moov->mvhd->duration64 / handle->moov->mvhd->timeScale));      
      }
  else /* 32 bit */
      {
      if (!handle->moov->mvhd->duration)
        return -1;

      *streamaveragebitrate = (mp4_u32)((mp4_double)8 * (mp4_double)streamsize /
                                        ((mp4_double)handle->moov->mvhd->duration / handle->moov->mvhd->timeScale));      
      }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 advanceVideoFrame(MP4HandleImp handle,
 *                             trackAtom *trak)
 *
 * Description:
 *
 *   This function Advances one video frame and finds the frame offset
 *   and size.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   trak      TRAK atom pointer
 *
 * Return value:
 *
 *   0         Success
 *   Negative  Error
 *
 */
mp4_i32 advanceVideoFrame(MP4HandleImp handle, trackAtom *trak)
{
  if (!trak->mdia)
    return -1;
  if (!trak->mdia->minf)
    return -1;
  if (!trak->mdia->minf->stbl)
    return -1;
  if (!trak->mdia->minf->stbl->stsz)
    return -1;

  /* Are there frames (samples) left? */

  if (trak->mdia->minf->stbl->stsz->sampleCount > handle->videoSampleNum)
    handle->videoSampleNum++;
  else
    return -2;

  /* Find the size of the frame (sample) */

  if (resolveVideoSampleSize(handle, trak->mdia->minf->stbl->stsz) < 0)
    return -1;

  /* Find the offset of the frame (sample) */

  if (resolveVideoSampleOffset(handle, trak->mdia->minf->stbl) < 0)
    return -1;


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 resolveVideoSampleOffset(MP4HandleImp handle,
 *                                    sampleTableAtom *stbl)
 *
 * Description:
 *
 *   This function finds the offset of the current video sample.
 *   The result is stored in handle->videoFrameOffset.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   stbl      STBL atom pointer
 *
 * Return value:
 *
 *   0         Success
 *   Negative  Error
 *
 */
mp4_i32 resolveVideoSampleOffset(MP4HandleImp handle, sampleTableAtom *stbl)
	{
	mp4_u32  chunk;            /* Current chunk number */
	mp4_u32  sample;           /* Number of samples before this run of chunks */
	mp4_u32  entry;            /* Current entry in sample to chunk */
	mp4_u32  chunksInThisRun;  /* Number of chunks in this run */
	mp4_u32  sampleNrInChunk = 0;	/* Sample number in the chunk */

	if (!stbl->stsc || stbl->stsc->entryCount == 0)
		{
		return -1;
		}
	
	if (!stbl->stco || stbl->stco->entryCount == 0)
		{
		return -1;
		}

	chunk = 0;
	sample = 0;
	entry = 0;

  for (;;)
		{
		/* Find how many chunks there are in this run */
		
		if (stbl->stsc->entryCount > entry + 1) /* Not last chunk run */
			{
			chunksInThisRun = stbl->stsc->firstChunk[entry + 1] - stbl->stsc->firstChunk[entry];
			}
		else
			{
			chunksInThisRun = stbl->stco->entryCount - chunk + 1;
			}

		if (handle->videoSampleNum <= chunksInThisRun * stbl->stsc->samplesPerChunk[entry] + sample)
			{
			chunk += (handle->videoSampleNum - sample + stbl->stsc->samplesPerChunk[entry] - 1) / stbl->stsc->samplesPerChunk[entry];
			sampleNrInChunk = (handle->videoSampleNum - sample + stbl->stsc->samplesPerChunk[entry] - 1) % stbl->stsc->samplesPerChunk[entry];
	  
			/* The following functions are needed for multiple sample entry support */
			handle->videoSampleEntryIndex = stbl->stsc->sampleDescriptionIndex[entry];

			break; /* We know the chunk number and sample number in chunk AND the sample entry index of the current sample*/
			}
		else
			{
			chunk += chunksInThisRun;
			sample += chunksInThisRun * stbl->stsc->samplesPerChunk[entry];
			}

	    entry++;
		}

	if (chunk > stbl->stco->entryCount)
		{
		return -1;
		}

	handle->videoFrameOffset = getChunkOffset(stbl, chunk - 1);

	if (sampleNrInChunk)
		{
		if (stbl->stsz->sampleSize)
			{
			handle->videoFrameOffset += stbl->stsz->sampleSize * sampleNrInChunk;
			}
		else
			{
			if (stbl->stsz->sampleCount == 0)
				{
				return -1;
				}
			while (sampleNrInChunk)
				{
				handle->videoFrameOffset += stbl->stsz->entrySize[handle->videoSampleNum - sampleNrInChunk - 1];
				sampleNrInChunk--;
				}
			}
		}
		
		
	//PRINT((_L("videoFrameOffset %Lu"), handle->videoFrameOffset));
	return 0;
	}


/*
 * Function:
 *
 *   mp4_i32 resolveVideoSampleSize(MP4HandleImp handle,
 *                                  sampleSizeAtom *stsz)
 *
 * Description:
 *
 *   This function finds the size of the current video sample.
 *   The result is stored in handle->videoFrameSize.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   stsz      STSZ atom pointer
 *
 * Return value:
 *
 *   0         Success
 *   Negative  Error
 *
 */
mp4_i32 resolveVideoSampleSize(MP4HandleImp handle, sampleSizeAtom *stsz)
	{
	if (stsz->sampleSize)
		{
		handle->videoFrameSize = stsz->sampleSize;
		return 0;
		}

	if (stsz->sampleCount == 0)
		{
		return -1;
		}
	handle->videoFrameSize = stsz->entrySize[handle->videoSampleNum - 1];

	return 0;
	}


/*
 * Function:
 *
 *   mp4_i32 fetchVideoFrame(MP4HandleImp handle,
 *                           trackAtom *trak,
 *                           mp4_u8 *buffer,
 *                           mp4_u32 buffersize,
 *                           mp4_u32 *framesize,
 *                           mp4_u32 *timestamp,
 *                           mp4_bool *keyframe,
 *                           mp4_u32 *timestamp2)
 *
 * Description:
 *
 *   This function fetches one video frame from a file.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   trak         TRAK atom pointer
 *   buffer       Video frame is retuned here
 *   buffersize   Size of buffer
 *   framesize    Size of returned frame in bytes
 *   timestamp    Frame time in milliseconds (from the beginning of the
 *                presentation)
 *   keyframe     True if intra frame, false otherwise
 *   timestamp2   Frame time in timescale (from the beginning of the
 *                presentation)
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 fetchVideoFrame(MP4HandleImp handle,
                        trackAtom *trak,
                        mp4_u8 *buffer,
                        mp4_u32 buffersize,
                        mp4_u32 *framesize,
                        mp4_u32 *timestamp,
                        mp4_bool *keyframe,
                        mp4_u32 *timestamp2)
{
  mp4_i32 bytesread;


  if (!trak->mdia)
    return -1;

  if (handle->file) /* Input is in a file */
  {
    if (seekFileAbs(handle, handle->videoFrameOffset) != 0)
      return -4;
  }
  else /* Input is a stream */
  {
      if (handle->videoFrameOffset + handle->videoFrameSize <= getCumulativeBufferedBytes(handle))
      {
          handle->absPosition = handle->videoFrameOffset;
      }
      else
        return -3;
  }

  if (handle->videoFrameSize > buffersize)
  {
    *framesize = handle->videoFrameSize;
    return -2;
  }

  bytesread = readData(handle, buffer, handle->videoFrameSize);
  switch (bytesread)
  {
    case -1:
      return -1;
    case -2:
      return -4;
    case -10:
      return -3;
    default:
      break;
  }

  if (handle->file)
    if (handle->videoFrameOffset + handle->videoFrameSize - 1 > handle->lastAccessedPosInFile)
      handle->lastAccessedPosInFile = handle->videoFrameOffset + handle->videoFrameSize - 1;

  *framesize = handle->videoFrameSize;

  if (convertVideoSampleToTime(handle, trak->mdia, timestamp, timestamp2) < 0)
    return -1;

  if (isVideoFrameKeyFrame(handle, trak, keyframe) < 0)
    return -1;

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 fetchVideoFrameAsync(MP4HandleImp handle,
 *                           	  trackAtom *trak,
 *                           	  mp4_u8 *buffer,
 *                           	  mp4_u32 buffersize,
 *
 * Description:
 *
 *   This function fetches one video frame from a file asyncronously.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   trak         TRAK atom pointer
 *   buffer       Video frame is retuned here
 *   buffersize   Size of buffer
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 fetchVideoFrameAsync(MP4HandleImp handle,
                        trackAtom *trak,
                        mp4_u8 *buffer,
                        mp4_u32 *buffersize)
{
  if (trak)
	{
	if (!trak->mdia)
		return -1;
	}
  else
    {
	return -1;
    }
  
  if (handle->videoFrameSize > *buffersize)
  {
    *buffersize = handle->videoFrameSize;
    return -2;
  }
  
  if ( handle->asyncReader == NULL )
  	{
	TRAPD(error, handle->asyncReader = CFileAsyncParser::NewL( handle, (RFile64&)handle->rfile ));
	if ( error != KErrNone )
		{
		if (error == KErrNoMemory )
			{
			return MP4_OUT_OF_MEMORY;    		
			}
		else
			{
			return -1;
			}
		}  	
  	}
  
  return handle->asyncReader->ReadVideoFrame( buffer, handle->videoFrameOffset, handle->videoFrameSize);
}

/*
 * Function:
 *
 *   mp4_i32 isVideoFrameKeyFrame(MP4HandleImp handle,
 *                                trackAtom *trak,
 *                                mp4_bool *keyframe)
 *
 * Description:
 *
 *   This function determines if the current frame is a keyframe (intra)
 *   or not.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   trak         TRAK atom pointer
 *   keyframe     Has a value of MP4TRUE if current frame is a keyframe
 *                (intra) or MP4FALSE otherwise
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 isVideoFrameKeyFrame(MP4HandleImp handle, trackAtom *trak, mp4_bool *keyframe)
{
  mp4_u32  i;


  *keyframe = MP4FALSE;

  if (!trak->mdia)
    return -1;

  if (!trak->mdia->minf)
    return -1;

  if (!trak->mdia->minf->stbl)
    return -1;

  if (!trak->mdia->minf->stbl->stss)
  {
    *keyframe = MP4TRUE;

    return 0;
  }

  for (i = 0; i < trak->mdia->minf->stbl->stss->entryCount; i++)
  {
    if (trak->mdia->minf->stbl->stss->sampleNumber[i] == handle->videoSampleNum)
    {
      *keyframe = MP4TRUE;
      break;
    }
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 convertVideoSampleToTime(MP4HandleImp handle,
 *                                    mediaAtom *mdia,
 *                                    mp4_u32 *timestamp,
 *                                    mp4_u32 *timestamp2)
 *
 * Description:
 *
 *   This function converts a video sample to corresponding time.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   mdia         MDIA atom pointer
 *   timestamp    Time in milliseconds is returned here
 *   timestamp2   Time in timescale is returned here
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 convertVideoSampleToTime(MP4HandleImp handle,
                                 mediaAtom *mdia,
                                 mp4_u32 *timestamp,
                                 mp4_u32 *timestamp2)
	{
	mp4_u32      tmptime;
	mp4_double   tmptime2;
	mp4_u32      sample;
	mp4_u32      entry;

	if (!mdia->mdhd)
		{
		return -1;
		}
	if (!mdia->minf)
		{
		return -1;
		}
	if (!mdia->minf->stbl)
		{
		return -1;
		}
	if (!mdia->minf->stbl->stts)
		{
		return -1;
		}
	if (mdia->minf->stbl->stts->entryCount == 0)
		{
		return -1;
		}

	tmptime = 0;
	sample = 0;
	entry = 0;

	for (;;)
		{
		if (sample + mdia->minf->stbl->stts->sampleCount[entry] < handle->videoSampleNum)
			{
			sample += mdia->minf->stbl->stts->sampleCount[entry];
			tmptime += (mdia->minf->stbl->stts->sampleCount[entry] * mdia->minf->stbl->stts->sampleDelta[entry]);
			entry++;
			if (entry == mdia->minf->stbl->stts->entryCount)
				{
				return -1;
				}
			}
		else
			{
			tmptime += ((handle->videoSampleNum - sample - 1) * mdia->minf->stbl->stts->sampleDelta[entry]);
			break;
			}
		}
	
  	if (mdia->mdhd->timeScale == 0)
  		{
  		return -1;
  		}

  	if (timestamp2)
  		{
  		*timestamp2 = tmptime;
  		}

  	tmptime2 = (mp4_double)tmptime * (mp4_double)1000 / (mp4_double)mdia->mdhd->timeScale + (mp4_double)0.5;

  	*timestamp = (mp4_u32)tmptime2;

  	return 0;
	}


/*
 * Function:
 *
 *   mp4_i32 advanceAudioSample(MP4HandleImp handle,
 *                              trackAtom *trak)
 *
 * Description:
 *
 *   This function advances one audio sample and finds the sample
 *   offset and sample size.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   trak      TRAK atom pointer
 *
 * Return value:
 *
 *   0         Success
 *   Negative  Error
 *
 */
mp4_i32 advanceAudioSample(MP4HandleImp handle,
                           trackAtom *trak)
{
  if (!trak->mdia)
    return -1;
  if (!trak->mdia->minf)
    return -1;
  if (!trak->mdia->minf->stbl)
    return -1;
  if (!trak->mdia->minf->stbl->stsz)
    return -1;


  /* Are there samples left? */

  if (trak->mdia->minf->stbl->stsz->sampleCount > handle->audioSampleNum)
    handle->audioSampleNum++;
  else
    return -2;

  /* Find the size of the sample */

  if (resolveAudioSampleSize(handle, trak->mdia->minf->stbl->stsz) < 0)
    return -1;

  /* Find the offset of the sample */

  if (resolveAudioSampleOffset(handle, trak->mdia->minf->stbl) < 0)
    return -1;


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 resolveAudioSampleOffset(MP4HandleImp handle,
 *                                    sampleTableAtom *stbl)
 *
 * Description:
 *
 *   This function finds the offset of the current audio sample.
 *   The result is stored in handle->audioSampleOffset.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   stbl      STBL atom pointer
 *
 * Return value:
 *
 *   0         Success
 *   Negative  Error
 *
 */
mp4_i32 resolveAudioSampleOffset(MP4HandleImp handle, sampleTableAtom *stbl)
{
  mp4_u32  chunk;            /* Current chunk number */
  mp4_u32  sample;            /* Number of samples before this run of chunks */
  mp4_u32  entry;            /* Current entry in sample to chunk */
  mp4_u32 chunksInThisRun;  /* Number of chunks in this run */
  mp4_u32  sampleNrInChunk;  /* Sample number in the chunk */


  if (!stbl->stsc || stbl->stsc->entryCount == 0)
    return -1;
  if (!stbl->stco || stbl->stco->entryCount == 0)
    return -1;

  chunk = 0;
  sample = 0;
  entry = 0;

  for (;;)
  {
    /* Find how many chunks there are in this run */

    if (stbl->stsc->entryCount > entry + 1) /* Not last chunk run */
    {
      chunksInThisRun = stbl->stsc->firstChunk[entry + 1] -
                        stbl->stsc->firstChunk[entry];
    }
    else
      chunksInThisRun = stbl->stco->entryCount - chunk + 1;


    if (handle->audioSampleNum <= chunksInThisRun * stbl->stsc->samplesPerChunk[entry] + sample)
    {
      chunk += (handle->audioSampleNum - sample + stbl->stsc->samplesPerChunk[entry] - 1) / stbl->stsc->samplesPerChunk[entry];
      sampleNrInChunk = (handle->audioSampleNum - sample + stbl->stsc->samplesPerChunk[entry] - 1) % stbl->stsc->samplesPerChunk[entry];

	  /* The following functions are needed for multiple sample entry support */
	  handle->audioSampleEntryIndex = stbl->stsc->sampleDescriptionIndex[entry];

      break; /* We know the chunk number and sample number in chunk AND the sample entry index of the current sample*/
    }
    else
    {
      chunk += chunksInThisRun;
      sample += chunksInThisRun * stbl->stsc->samplesPerChunk[entry];
    }

    entry++;
  }

  if (chunk > stbl->stco->entryCount)
    return -1;

  handle->audioSampleOffset = getChunkOffset(stbl, chunk - 1);

  	if (sampleNrInChunk)
  		{
  		if (stbl->stsz->sampleSize)
  			{
  			handle->audioSampleOffset += stbl->stsz->sampleSize * sampleNrInChunk;
  			}
  		else
  			{
  			if (stbl->stsz->sampleCount == 0)
  				{
  				// ensure there are entries in the entrySize array
  				return -1;
  				}
    
  			while (sampleNrInChunk)
  				{
  				handle->audioSampleOffset += stbl->stsz->entrySize[handle->audioSampleNum - sampleNrInChunk - 1];
  				sampleNrInChunk--;
  				}
  			}
  		}
  		
  //PRINT((_L("audioSampleOffset %Lu"), handle->audioSampleOffset));
  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 resolveAudioSampleSize(MP4HandleImp handle,
 *                                  sampleSizeAtom *stsz)
 *
 * Description:
 *
 *   This function finds the size of the current audio sample.
 *   The result is stored in handle->audioSampleSize.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   stsz      STSZ atom pointer
 *
 * Return value:
 *
 *   0         Success
 *   Negative  Error
 *
 */
mp4_i32 resolveAudioSampleSize(MP4HandleImp handle, sampleSizeAtom *stsz)
	{
	if (stsz->sampleSize)
		{
		handle->audioSampleSize = stsz->sampleSize;
		return 0;
		}

	if (stsz->sampleCount == 0)
		{
		return -1;
		}
	handle->audioSampleSize = stsz->entrySize[handle->audioSampleNum - 1];

	return 0;
	}


/*
 * Function:
 *
 *   mp4_i32 fetchAudioSample(MP4HandleImp handle,
 *                            trackAtom *trak,
 *                            mp4_u8 *buffer,
 *                            mp4_u32 buffersize,
 *                            mp4_u32 *framesize,
 *                            mp4_u32 *timestamp,
 *                            mp4_u32 *returnedframes,
 *                            mp4_u32 *timestamp2)
 *
 * Description:
 *
 *   This function fetches one audio sample from a file.
 *
 *   Note: returnedframes may differ from the correct value when accessing
 *         the last audio sample.
 *
 * Parameters:
 *
 *   handle           MP4 library handle
 *   trak             TRAK atom pointer
 *   buffer           Audio frame is retuned here
 *   buffersize       Size of buffer
 *   framesize        Size of returned frame in bytes
 *   timestamp        Frame time in milliseconds (from the beginning of the
 *                    presentation)
 *   returnedframes   Number of frames returned, of 0 if not known
 *   timestamp2       Frame time in timescale (from the beginning of the
 *                    presentation)
 *
 * Return value:
 *
 *   0                Success
 *   Negative         Error
 *
 */
mp4_i32 fetchAudioSample(MP4HandleImp handle,
                         trackAtom *trak,
                         mp4_u8 *buffer,
                         mp4_u32 buffersize,
                         mp4_u32 *framesize,
                         mp4_u32 *timestamp,
                         mp4_u32 *returnedframes,
                         mp4_u32 *timestamp2)
{
  mp4_i32 bytesread;
  mp4_i32 frameLength;
  mp4_u32 numOfFrames;
  mp4_u8 *framepointer;
  mp4_u32 rawAmrFrameLength[16] = {13,14,16,18,20,21,27,32,6,0,0,0,0,0,0,1};

  if (!trak->mdia)
    return -1;

  if (handle->file) /* Input is in a file */
  {
    if (seekFileAbs(handle, handle->audioSampleOffset) != 0)
      return -4;
  }
  else /* Input is a stream */
  {
      if (handle->audioSampleOffset + handle->audioSampleSize <= getCumulativeBufferedBytes(handle))
      {
          handle->absPosition = handle->audioSampleOffset;
      }
      else
        return -3;
  }

  if (handle->audioSampleSize > buffersize)
  {
    *framesize = handle->audioSampleSize;
    return -2;
  }

  bytesread = readData(handle, buffer, handle->audioSampleSize);
  switch (bytesread)
  {
    case -1:
      return -1;
    case -2:
      return -4;
    case -10:
      return -3;
    default:
      break;
  }

  if (handle->file)
    if (handle->audioSampleOffset + handle->audioSampleSize - 1 > handle->lastAccessedPosInFile)
      handle->lastAccessedPosInFile = handle->audioSampleOffset + handle->audioSampleSize - 1;

  *framesize = handle->audioSampleSize;
  if (convertAudioSampleToTime(handle, trak->mdia, timestamp, timestamp2) < 0)
    return -1;

  *returnedframes = 0;

  /* AMR */
  if (trak->mdia->minf)
    if (trak->mdia->minf->stbl)
      if (trak->mdia->minf->stbl->stsd)
        if (handle->type & MP4_TYPE_AMR_NB)
        {
            framepointer = buffer;
            numOfFrames = 0;
            while ( bytesread > 0 )
            {
                frameLength = rawAmrFrameLength[(TInt)(((*framepointer) & 0x78) >> 3)];
                if ( frameLength == 0)
                {
                    return -4;
                }
                bytesread -= frameLength;
                framepointer += frameLength;
                numOfFrames++;
            }
            *returnedframes = numOfFrames;

		  /* Return the number of sample entries listed for this particular sample entry index 
          if (trak->mdia->minf->stbl->stsd->samr[handle->audioSampleEntryIndex - 1])
            if (trak->mdia->minf->stbl->stsd->samr[handle->audioSampleEntryIndex - 1]->damr)
              *returnedframes = trak->mdia->minf->stbl->stsd->samr[handle->audioSampleEntryIndex - 1]->damr->framesPerSample;*/
        }
        else if (handle->type & MP4_TYPE_AMR_WB)
        {
		  /* Return the number of sample entries listed for this particular sample entry index */
          if (trak->mdia->minf->stbl->stsd->sawb[handle->audioSampleEntryIndex - 1])
            if (trak->mdia->minf->stbl->stsd->sawb[handle->audioSampleEntryIndex - 1]->damr)
              *returnedframes = trak->mdia->minf->stbl->stsd->sawb[handle->audioSampleEntryIndex - 1]->damr->framesPerSample;
        }
        else
        {
        }

  /* MPEG-4 audio */
  if (trak->mdia->minf)
    if (trak->mdia->minf->stbl)
      if (trak->mdia->minf->stbl->stsd)
        if (trak->mdia->minf->stbl->stsd->mp4a[handle->audioSampleEntryIndex - 1])
          *returnedframes = 1;

  /* QCELP 13K as QCELPSampleEntry*/
  if (trak->mdia->minf)
    if (trak->mdia->minf->stbl)
      if (trak->mdia->minf->stbl->stsd)
        if ((handle->type & MP4_TYPE_QCELP_13K) && (!handle->qcelpStoredAsMPEGAudio))
        {
		  /* Return the number of sample entries listed for this particular sample entry index */
          if (trak->mdia->minf->stbl->stsd->sqcp[handle->audioSampleEntryIndex - 1])
            if (trak->mdia->minf->stbl->stsd->sqcp[handle->audioSampleEntryIndex - 1]->dqcp)
              *returnedframes = trak->mdia->minf->stbl->stsd->sqcp[handle->audioSampleEntryIndex - 1]->dqcp->framesPerSample;
        }

  /* QCELP 13K as MPEG-4 audio */
  if (trak->mdia->minf)
    if (trak->mdia->minf->stbl)
      if (trak->mdia->minf->stbl->stsd)
        if (trak->mdia->minf->stbl->stsd->mp4a[handle->audioSampleEntryIndex - 1])
          *returnedframes = 1;

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 fetchAudioSampleAsync(MP4HandleImp handle,
 *                            trackAtom *trak,
 *                            mp4_u8 *buffer,
 *                            mp4_u32 buffersize,
 *
 * Description:
 *
 *   This function fetches one audio sample from a file asyncronously.
 *
 * Parameters:
 *
 *   handle           MP4 library handle
 *   trak             TRAK atom pointer
 *   buffer           Audio frame is retuned here
 *   buffersize       Size of buffer
 *
 * Return value:
 *
 *   0                Success
 *   Negative         Error
 *
 */
mp4_i32 fetchAudioSampleAsync(MP4HandleImp handle,
                         	  trackAtom *trak,
                         	  mp4_u8 *buffer,
                         	  mp4_u32 *buffersize)
{
  if (trak)
	{
	if (!trak->mdia)
		return -1;
	}
  else
    {
	return -1;
    }
  
  if (handle->audioSampleSize > *buffersize)
		{
		*buffersize = handle->audioSampleSize;
		return -2;
		}
  
  if (!handle->file) // Other input than file is not supported
	  {
	  return -1;
	  }
  
  if ( handle->asyncReader == NULL )
  	{
	TRAPD(error, handle->asyncReader = CFileAsyncParser::NewL( handle, (RFile64&)handle->rfile ));
	if ( error != KErrNone )
		{
		if (error == KErrNoMemory )
			{
			return MP4_OUT_OF_MEMORY;    		
			}
		else
			{
			return -1;
			}
		}  	
  	}
    
  return handle->asyncReader->ReadAudioFrames( buffer, handle->audioSampleOffset, handle->audioSampleSize);
}

/*
 * Function:
 *
 *   mp4_i32 convertAudioSampleToTime(MP4HandleImp handle,
 *                                    mediaAtom *mdia,
 *                                    mp4_u32 *timestamp,
 *                                    mp4_u32 *timestamp2)
 *
 * Description:
 *
 *   This function converts an audio sample to corresponding time.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   mdia         MDIA atom pointer
 *   timestamp    Time in milliseconds is returned here
 *   timestamp2   Time in timescale is returned here
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 convertAudioSampleToTime(MP4HandleImp handle,
                                 mediaAtom *mdia,
                                 mp4_u32 *timestamp,
                                 mp4_u32 *timestamp2)
	{
	mp4_u32      tmptime;
	mp4_double  tmptime2;
	mp4_u32      sample;
	mp4_u32      entry;

  	if (!mdia->mdhd)
  		{
  		return -1;
  		}
  	if (!mdia->minf)
  		{
  		return -1;
  		}
  	if (!mdia->minf->stbl)
  		{
  		return -1;
  		}
  	if (!mdia->minf->stbl->stts)
  		{
  		return -1;
  		}
  	if (mdia->minf->stbl->stts->entryCount == 0)
  		{
  		return -1;
  		}

  	tmptime = 0;
  	sample = 0;
  	entry = 0;

  	for (;;)
  		{
  		if (sample + mdia->minf->stbl->stts->sampleCount[entry] < handle->audioSampleNum)
  			{
  			sample += mdia->minf->stbl->stts->sampleCount[entry];
  			tmptime += mdia->minf->stbl->stts->sampleCount[entry] *
                 	mdia->minf->stbl->stts->sampleDelta[entry];
  			entry++;
  			
  			if (entry == mdia->minf->stbl->stts->entryCount)
  				return -1;
  			}
  		else
  			{
  			tmptime += (handle->audioSampleNum - sample - 1) * mdia->minf->stbl->stts->sampleDelta[entry];
  			break;
  			}
  		}

  	if (mdia->mdhd->timeScale == 0)
  		{
  		return -1;
  		}

  	if (timestamp2)
  		{
  		*timestamp2 = tmptime;
  		}

  	tmptime2 = (mp4_double)tmptime * (mp4_double)1000 / (mp4_double)mdia->mdhd->timeScale + (mp4_double)0.5;

  	*timestamp = (mp4_u32)tmptime2;

  	return 0;
	}


/*
 * Function:
 *
 *   mp4_i32 convertTimeToSample(MP4HandleImp handle,
 *                               trackAtom *trak,
 *                               mp4_u32 position,
 *                               mp4_u32 *sample)
 *
 * Description:
 *
 *   This function converts time to corresponding sample number.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   trak         trackAtom pointer
 *   position     Time in milliseconds
 *   sample       Sample number is returned here
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 convertTimeToSample(MP4HandleImp handle,
                            trackAtom *trak,
                            mp4_u32 position,
                            mp4_u32 *sample)
{
  mp4_u32 pos;      /* Target position in media timescale */
  mp4_u32 tmppos;   /* Temporary position counter */
  mp4_u32 i;


  if (!handle)
    return -1;
  if (!trak)
    return -1;
  if (!trak->mdia)
    return -1;
  if (!trak->mdia->mdhd)
    return -1;
  if (!trak->mdia->minf)
    return -1;
  if (!trak->mdia->minf->stbl)
    return -1;
  if (!trak->mdia->minf->stbl->stts)
    return -1;
  if (trak->mdia->minf->stbl->stts->entryCount == 0)
    return -1;

  *sample = 1;
  tmppos = 0;

  pos = (mp4_u32)((mp4_double)position / (mp4_double)1000 * (mp4_double)trak->mdia->mdhd->timeScale + (mp4_double)0.5);

  for (i = 0; i < trak->mdia->minf->stbl->stts->entryCount; i++)
  {
    if (pos >= (tmppos + trak->mdia->minf->stbl->stts->sampleCount[i] *
                         trak->mdia->minf->stbl->stts->sampleDelta[i]))
    {
      *sample += trak->mdia->minf->stbl->stts->sampleCount[i];
      tmppos += (trak->mdia->minf->stbl->stts->sampleCount[i] *
                 trak->mdia->minf->stbl->stts->sampleDelta[i]);

      if (i == trak->mdia->minf->stbl->stts->entryCount - 1) /* Last entry */
        *sample = *sample - 1;
    }
    else
    {
      if (trak->mdia->minf->stbl->stts->sampleDelta[i] == 0)
        return -1;

      *sample += ((pos - tmppos) / trak->mdia->minf->stbl->stts->sampleDelta[i]);

      break;
    }
  }

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 goToVideoSample(MP4HandleImp handle,
 *                           trackAtom *trak,
 *                           mp4_u32 sample)
 *
 * Description:
 *
 *   This function moves to video sample indicated by sample and finds the
 *   sample offset and sample size.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   trak      TRAK atom pointer
 *   sample    Sample to go to
 *
 * Return value:
 *
 *   0         Success
 *   Negative  Error
 *
 */
mp4_i32 goToVideoSample(MP4HandleImp handle,
                        trackAtom *trak,
                        mp4_u32 sample)
{
  if (!trak->mdia)
    return -1;
  if (!trak->mdia->minf)
    return -1;
  if (!trak->mdia->minf->stbl)
    return -1;
  if (!trak->mdia->minf->stbl->stsz)
    return -1;


  /* Is the sample valid? */

  if (sample <= trak->mdia->minf->stbl->stsz->sampleCount)
    handle->videoSampleNum = sample;
  else
    return -1;

  /* Find the size of the sample */

  if (resolveVideoSampleSize(handle, trak->mdia->minf->stbl->stsz) < 0)
    return -1;

  /* Find the offset of the sample */

  if (resolveVideoSampleOffset(handle, trak->mdia->minf->stbl) < 0)
    return -1;


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 goToAudioSample(MP4HandleImp handle,
 *                           trackAtom *trak,
 *                           mp4_u32 sample)
 *
 * Description:
 *
 *   This function moves to audio sample indicated by sample and finds the
 *   sample offset and sample size.
 *
 * Parameters:
 *
 *   handle    MP4 library handle
 *   trak      TRAK atom pointer
 *   sample    Sample to go to
 *
 * Return value:
 *
 *   0         Success
 *   Negative  Error
 *
 */
mp4_i32 goToAudioSample(MP4HandleImp handle,
                        trackAtom *trak,
                        mp4_u32 sample)
{
  if (!trak->mdia)
    return -1;
  if (!trak->mdia->minf)
    return -1;
  if (!trak->mdia->minf->stbl)
    return -1;
  if (!trak->mdia->minf->stbl->stsz)
    return -1;


  /* Is the sample valid? */

  if (sample <= trak->mdia->minf->stbl->stsz->sampleCount)
    handle->audioSampleNum = sample;
  else
    return -1;

  /* Find the size of the sample */

  if (resolveAudioSampleSize(handle, trak->mdia->minf->stbl->stsz) < 0)
    return -1;

  /* Find the offset of the sample */

  if (resolveAudioSampleOffset(handle, trak->mdia->minf->stbl) < 0)
    return -1;


  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 findVideoKeyFrame(MP4HandleImp handle,
 *                             trackAtom *trak,
 *                             mp4_u32 sample,
 *                             mp4_u32 *newsample)
 *
 * Description:
 *
 *   This function finds the video keyframe that is identical to or precedes
 *   the sample indicated by sample.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   trak         TRAK atom pointer
 *   sample       Sample number
 *   newsample    Sample number of found keyframe
 *
 * Return value:
 *
 *   0            Success
 *   Negative     Error
 *
 */
mp4_i32 findVideoKeyFrame(MP4HandleImp handle,
                          trackAtom *trak,
                          mp4_u32 sample,
                          mp4_u32 *newsample)
{
  mp4_i32  i;


  if (!handle)
    return -1;
  if (!trak->mdia)
    return -1;
  if (!trak->mdia->minf)
    return -1;
  if (!trak->mdia->minf->stbl)
    return -1;

  if (!trak->mdia->minf->stbl->stss) /* No sync sample atom => all samples are
                                        random access points */
  {
    *newsample = sample;

    return 0;
  }
  *newsample = 0;

  for (i = trak->mdia->minf->stbl->stss->entryCount - 1; i >= 0; i--)
  {
    if (sample >= trak->mdia->minf->stbl->stss->sampleNumber[i])
    {
      *newsample = trak->mdia->minf->stbl->stss->sampleNumber[i];
      break;
    }
  }

  if (*newsample == 0)
    return -1;

  return 0;
}


/*
 * Function:
 *
 *   mp4_i32 readAVCC(MP4HandleImp handle,
 *                    avcConfigurationAtom *avcc)
 *
 * Description:
 *
 *   This function parses one avcc atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   avcc               avcC pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readAVCC(MP4HandleImp handle, avcConfigurationAtom *avcc)
{
	mp4_i32 bytesread;
	mp4_i32 totalbytesread = 0;

	avcc->avcConfigSize = 0;

	if ((avcc->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
		return -100;

	bytesread = readAtomHeader(handle, avcc->atomhdr);
	if (bytesread < 0)
		return -1;
	totalbytesread +=bytesread;

	if (avcc->atomhdr->type != ATOMTYPE_AVCC)
		return -1;

    /* read the avcDecoderConfigurationRecord */
    if  (avcc->atomhdr->size != 1)
    	avcc->avcConfigSize = avcc->atomhdr->size - 8;
    else
    	avcc->avcConfigSize = (mp4_u32)(I64INT(avcc->atomhdr->largeSize) - 16);

   	avcc->avcConfig = (mp4_u8 *)mp4malloc(avcc->avcConfigSize);
	if (avcc->avcConfig == 0)
		return -100;
	
	bytesread = readData(handle, avcc->avcConfig, avcc->avcConfigSize );
    
	if (bytesread < 0)
		return -1;
	totalbytesread +=bytesread;
	return totalbytesread;
}


/*
 * Function:
 *
 *   mp4_i32 readBTRT(MP4HandleImp handle,
 *                    mpeg4BitrateAtom *btrt)
 *
 * Description:
 *
 *   This function parses one btrt atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   btrt					btrt pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readBTRT(MP4HandleImp handle, mpeg4BitrateAtom *btrt)
{
	mp4_i32 bytesread;
	mp4_i32 totalbytesread = 0;


	if ((btrt->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
		return -100;

	bytesread = readAtomHeader(handle, btrt->atomhdr);
	if (bytesread < 0)
		return -1;
	totalbytesread +=bytesread;

	if (btrt->atomhdr->type != ATOMTYPE_BTRT)
		return -1;

    /* read the mpeg4BitrateAtom */
   bytesread = readData(handle, handle->buf, 4);
   if (bytesread < 0)
	   return -1;
   btrt->bufferSizeDB = u32endian(*((mp4_u32 *)handle->buf));
   totalbytesread +=bytesread;

   bytesread = readData(handle, handle->buf, 4);
   if (bytesread < 0)
	   return -1;
   btrt->maxBitRate = u32endian(*((mp4_u32 *)handle->buf));
   totalbytesread +=bytesread;

   bytesread = readData(handle, handle->buf, 4);
   if (bytesread < 0)
	   return -1;
   btrt->avgBitrate = u32endian(*((mp4_u32 *)handle->buf));
   totalbytesread +=bytesread;

	return totalbytesread;
}

/*
 * Function:
 *
 *   mp4_i32 readM4DS(MP4HandleImp handle,
 *                    mpeg4ExtensionDescriptorsAtom *m4ds)
 *
 * Description:
 *
 *   This function parses one m4ds atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   m4ds               m4ds pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readM4DS(MP4HandleImp handle, mpeg4ExtensionDescriptorsAtom *m4ds)
{
	mp4_i32 bytesread;
	mp4_i32 totalbytesread = 0;
	mp4_u32 i;

	m4ds->descrSize = 0;

	if ((m4ds->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
		return -100;

	bytesread = readAtomHeader(handle, m4ds->atomhdr);
	if (bytesread < 0)
		return -1;
	totalbytesread +=bytesread;

	if (m4ds->atomhdr->type != ATOMTYPE_M4DS)
		return -1;

    /* read the avcDecoderConfigurationRecord */
	if  (m4ds->atomhdr->size != 1)
		bytesread = readData(handle, handle->buf, m4ds->atomhdr->size - 8);
	else
        bytesread = readData(handle, handle->buf, (mp4_u32)(I64INT(m4ds->atomhdr->largeSize) - 16) );

	if (bytesread < 0)
		return -1;
	m4ds->descrSize = bytesread;
	m4ds->descr = (mp4_u8 *)mp4malloc(m4ds->descrSize * sizeof(mp4_u8));
	
	/* copy the mpeg4ExtensionDescriptors from the temp. buffer */
	for(i = 0; i <  m4ds->descrSize; i++)
		m4ds->descr[i] = handle->buf[i];
	
	totalbytesread +=bytesread;

	return totalbytesread;
}

/*
 * Function:
 *
 *   mp4_i32 readAVC1(MP4HandleImp handle,
 *                    avcSampleEntry *avc1)
 *
 * Description:
 *
 *   This function parses one avc1 atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   avc1               avc1 pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readAVC1(MP4HandleImp handle, avcSampleEntry *avc1)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;

  if ((avc1->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, avc1->atomhdr);

  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (avc1->atomhdr->type != ATOMTYPE_AVC1)
    return -1;

  bytesread = discardData(handle, 6);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  avc1->dataReferenceIndex = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 16);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  avc1->width = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  avc1->height = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 50);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  /* Check for the present atoms */
  while ((mp4_u32)totalbytesread < avc1->atomhdr->size)
  {
    mp4_u32 type;

    if (peekData(handle, handle->buf, 8) < 0)
      return -1;

    type = u32endian(*((mp4_u32 *)(handle->buf+4)));

    switch (type)
    {
    case ATOMTYPE_AVCC:

		if (avc1->avcc) /* avcC has already been read, more than one is not allowed. */
			return -1;

		if ((avc1->avcc = (avcConfigurationAtom *)mp4malloc(sizeof(avcConfigurationAtom))) == NULL)
			return -100;

		bytesread = readAVCC(handle, avc1->avcc);  
		if(bytesread < 0)
			return -1;
		totalbytesread +=bytesread;
  	    break;

    case ATOMTYPE_BTRT:

		if (avc1->btrt) /* btrt has already been read, more than one is not allowed. */
			return -1;

		if ((avc1->btrt = (mpeg4BitrateAtom *)mp4malloc(sizeof(mpeg4BitrateAtom))) == NULL)
			return -100;

		bytesread = readBTRT(handle, avc1->btrt);  
		if(bytesread < 0)
			return -1;
		totalbytesread +=bytesread;
	    break;

    case ATOMTYPE_M4DS:

		if (avc1->m4ds) /* m4ds has already been read, more than one is not allowed. */
			return -1;

		if ((avc1->m4ds = (mpeg4ExtensionDescriptorsAtom *)mp4malloc(sizeof(mpeg4ExtensionDescriptorsAtom))) == NULL)
			return -100;

		bytesread = readM4DS(handle, avc1->m4ds);  
		if(bytesread < 0)
			return -1;
		totalbytesread +=bytesread;
	    break;
	 
	 default:

        bytesread = readUnknown(handle);
        if (bytesread < 0)
            return -1;
        totalbytesread += bytesread;

        break;

	}
  }

  return totalbytesread;
}

/*
 * Function:
 *
 *   mp4_i32 freeAVCC(avcConfigurationAtom *avcc)
 *
 * Description:
 *
 *   This function frees memory for avcc atom.
 *
 * Parameters:
 *
 *   avcc       avcc atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeAVCC(avcConfigurationAtom *avcc)
{
  if (avcc)
  {
    if (freeAtomHeader(avcc->atomhdr) < 0)
      return -1;
	if(avcc->avcConfig)
		mp4free(avcc->avcConfig);

    mp4free(avcc);
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 freeBTRT(mpeg4BitrateAtom *btrt)
 *
 * Description:
 *
 *   This function frees memory for btrt atom.
 *
 * Parameters:
 *
 *   btrt       btrt atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeBTRT(mpeg4BitrateAtom *btrt)
{
  if (btrt)
  {
    if (freeAtomHeader(btrt->atomhdr) < 0)
      return -1;

    mp4free(btrt);
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 freeM4DS(mpeg4ExtensionDescriptorsAtom *m4ds)
 *
 * Description:
 *
 *   This function frees memory for m4ds atom.
 *
 * Parameters:
 *
 *   m4ds       m4ds atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeM4DS(mpeg4ExtensionDescriptorsAtom *m4ds)
{
  if (m4ds)
  {
    if (freeAtomHeader(m4ds->atomhdr) < 0)
      return -1;
	if(m4ds->descr)
		mp4free(m4ds->descr);

    mp4free(m4ds);
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 freeAVC1(avcSampleEntry *avc1)
 *
 * Description:
 *
 *   This function frees memory for avc1 atom.
 *
 * Parameters:
 *
 *   avc1       avc1 atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeAVC1(avcSampleEntry *avc1)
{
  if (avc1)
  {
    if (freeAtomHeader(avc1->atomhdr) < 0)
      return -1;
    if (freeAVCC(avc1->avcc) < 0)
      return -1;
	if (freeBTRT(avc1->btrt) < 0)
		return -1;
	if (freeM4DS(avc1->m4ds) < 0)
		return -1;

    mp4free(avc1);
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 readSQCP(MP4HandleImp handle,
 *                    qcelpSampleEntry *sqcp)
 *
 * Description:
 *
 *   This function parses one SQCP atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   sqcp               SQCP pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readSQCP(MP4HandleImp handle, qcelpSampleEntry *sqcp)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((sqcp->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, sqcp->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (sqcp->atomhdr->type != ATOMTYPE_SQCP)
    return -1;


  bytesread = discardData(handle, 6);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  sqcp->dataReferenceIndex = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 16);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 2);
  if (bytesread < 0)
    return -1;
  sqcp->timeScale = u16endian(*((mp4_u16 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = discardData(handle, 2);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if ((sqcp->dqcp = (qcelpDecSpecStruc *)mp4malloc(sizeof(qcelpDecSpecStruc))) == NULL)
    return -100;

  bytesread = readDQCP(handle, sqcp->dqcp);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;
  
  if ( totalbytesread < sqcp->atomhdr->size )
  	{
    bytesread = discardData(handle, sqcp->atomhdr->size - totalbytesread );
  	if (bytesread < 0)
  		{
    	return -1;	  		
  		}
  	totalbytesread += bytesread;
  	}  

  return totalbytesread;
}

/*
 * Function:
 *
 *   mp4_i32 freeSQCP(qcelpSampleEntry *sqcp)
 *
 * Description:
 *
 *   This function frees memory for SQCP atom.
 *
 * Parameters:
 *
 *   sqcp       SQCP atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeSQCP(qcelpSampleEntry *sqcp)
{
  if (sqcp)
  {
    if (freeAtomHeader(sqcp->atomhdr) < 0)
      return -1;
    if (freeDQCP(sqcp->dqcp) < 0)
      return -1;

    mp4free(sqcp);
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 readDQCP(MP4HandleImp handle,
 *                    qcelpDecSpecStruc *dqcp)
 *
 * Description:
 *
 *   This function parses one DQCP atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   dqcp               DQCP pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readDQCP(MP4HandleImp handle, qcelpDecSpecStruc *dqcp)
{
  mp4_i32 bytesread;
  mp4_i32 totalbytesread = 0;


  if ((dqcp->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, dqcp->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (dqcp->atomhdr->type != ATOMTYPE_DQCP)
    return -1;


  bytesread = readData(handle, handle->buf, 4);
  if (bytesread < 0)
    return -1;
  dqcp->vendor = u32endian(*((mp4_u32 *)handle->buf));
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  dqcp->decoderVersion = handle->buf[0];
  totalbytesread += bytesread;

  bytesread = readData(handle, handle->buf, 1);
  if (bytesread < 0)
    return -1;
  dqcp->framesPerSample = handle->buf[0];
  totalbytesread += bytesread;

  return totalbytesread;
}

/*
 * Function:
 *
 *   mp4_i32 freeDQCP(qcelpDecSpecStruc *dqcp)
 *
 * Description:
 *
 *   This function frees memory for DQCP atom.
 *
 * Parameters:
 *
 *   damr       DQCP atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeDQCP(qcelpDecSpecStruc *dqcp)
{
  if (dqcp)
  {
    if (freeAtomHeader(dqcp->atomhdr) < 0)
      return -1;

    mp4free(dqcp);
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 readSDTP(MP4HandleImp handle, sampleDependencyAtom *sdtp, 
 * 				      mp4_i32 sample_count)
 *
 * Description:
 *
 *   This function parses one SDTP atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   sdtp               SDTP atom pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */

mp4_i32 readSDTP(MP4HandleImp handle, 
				 sampleDependencyAtom *sdtp, 
				 mp4_i32 sample_count)
{
  mp4_i32 bytesread = 0;
  mp4_i32 totalbytesread = 0;
  mp4_u32 i = 0;

  if ((sdtp->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
    return -100;

  bytesread = readAtomHeader(handle, sdtp->atomhdr);
  if (bytesread < 0)
    return -1;
  totalbytesread += bytesread;

  if (sdtp->atomhdr->type != ATOMTYPE_SDTP)
    return -1;
  
  bytesread = readData(handle, handle->buf, 1); //Version must be 0
  if (bytesread < 0 || handle->buf[0] != 0)
    return -1;
  totalbytesread += bytesread;
  bytesread = readData(handle, handle->buf, 3); // Flags must be 0
  if (bytesread < 0 || handle->buf[0] != 0 || handle->buf[1] != 0 || handle->buf[2] != 0)
    return -1;
  totalbytesread += bytesread;

  // sample_count == (size_of_atom - 12) ???  12 = size + 'sdtp' + ver + flags = 4 + 4 + 1 + 3 ???
  //
  // sample_count is taken from the sample_count in the Sample Size Box ('stsz') or
  // Compact Sample Size Box (‘stz2’).
  if (sample_count != sdtp->atomhdr->size-12 || sample_count < 0)
	  {
	  // as each byte consititue one entry in the table, the number of remaining bytes in this
	  // atom should match the sample count.  If not, the file is corrupted.
	  return -1;
	  }
  
  if ((sdtp->dep = (sampleDependency *)mp4malloc(sample_count * sizeof(sampleDependency))) == NULL)
    return -100;
  
  for(i=0;i<sample_count;i++)
  {
    bytesread = readData(handle, handle->buf, 1);
    if (bytesread < 0)
      return -1;
    
    sdtp->dep[i].sDependsOn = (handle->buf[0] >> 4) & 0x03; // value from 0 to 3
    sdtp->dep[i].sIsDependentOn = (handle->buf[0] >> 2) & 0x03; // value from 0 to 3
    sdtp->dep[i].sHasRedundancy = handle->buf[0] & 0x03; // value from 0 to 3
    totalbytesread += bytesread;
  }

  sdtp->sampleCount = sample_count;
  return totalbytesread;	
}

/*
 * Function:
 *
 *   mp4_i32 readBoxHeader(MP4HandleImp handle, mp4_i64* size, mp4_u32* type)
 * 				      
 *
 * Description:
 *
 *   This function parses the size and type information for a box. 
 *   Taking into account largesize if needed.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   size               Size of box is returned here
 *   type               Type of the box is returned here
 * Return value:
 *
 *   Negative integer   Error
 *   0               Success.
 *
 */
mp4_i32 readBoxHeader(MP4HandleImp handle, mp4_i64* size, mp4_u32* type)
{
  if (peekData(handle, handle->buf, 8) < 0)
    return -1;

  *size = u32endian(*((mp4_u32 *)handle->buf));
  *type = u32endian(*((mp4_u32 *)(handle->buf+4)));

  if (*size == 1)
  {
    if (peekData(handle, handle->buf, 16) < 0)
      return -1;
    
    *size = u64endian(*((mp4_u64 *)(handle->buf+8)));
  }

  return 0;
}

/*
 */

mp4_i64 getChunkOffset(sampleTableAtom *stbl, mp4_u32 index)
{
  if (stbl->is32BitOffsets)
  	return (mp4_i64)stbl->stco->chunkOffset[index];
  else
    return stbl->stco64->chunkOffset[index];
}

/*
 * Function:
 *
 *   mp4_i32 readMeta(MP4HandleImp handle,
 *                    metaAtom *meta)
 *
 * Description:
 *
 *   This function parses one META atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   meta               META pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readMeta(MP4HandleImp handle, metaAtom *meta)
{
    mp4_i32 bytesread;
    mp4_i32 totalbytesread = 0;


    if ((meta->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
      return -100;

    bytesread = readFullAtomHeader(handle, meta->atomhdr);
    if (bytesread < 0)
      return -1;
    totalbytesread += bytesread;

    if (meta->atomhdr->type != ATOMTYPE_META)
      return -1;


    while ((mp4_u32)totalbytesread < meta->atomhdr->size)
    {
      mp4_u32 type;


      if (peekData(handle, handle->buf, 8) < 0)
        return -1;
      
      type = u32endian(*((mp4_u32 *)(handle->buf+4)));

      switch (type)
      {
      case ATOMTYPE_HDLR:

        if (meta->hdlr) /* HDLR has already been read, more than one is not allowed */
          return -1;

        if ((meta->hdlr = (handlerAtom *)mp4malloc(sizeof(handlerAtom))) == NULL)
          return -100;

        bytesread = readHDLR(handle, meta->hdlr);
        if (bytesread < 0)
          return -1;
        totalbytesread += bytesread;

        break;
          
      case ATOMTYPE_ID32:
          
        if (meta->ID32) /* ID32 has already been read, more than one is not allowed */
          return -1;

        if ((meta->ID32 = (ID32Atom *)mp4malloc(sizeof(ID32Atom))) == NULL)
          return -100;

        bytesread = readID32(handle, meta->ID32);
        if (bytesread < 0)
          return -1;
        totalbytesread += bytesread;
          
        break;
        
      default:

        bytesread = readUnknown(handle);
        if (bytesread < 0)
          return -1;
        totalbytesread += bytesread;

        break;
      }
    }

    return totalbytesread;
}

/*
 * Function:
 *
 *   mp4_i32 readID32(MP4HandleImp handle,
 *                    ID32Atom *ID32)
 *
 * Description:
 *
 *   This function parses one ID32 atom.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   ID32               ID32 pointer
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 readID32(MP4HandleImp handle, ID32Atom *ID32)
{
   mp4_i32 bytesread;
   mp4_i32 totalbytesread = 0;


   if ((ID32->atomhdr = (atomHeader *)mp4malloc(sizeof(atomHeader))) == NULL)
     return -100;

   bytesread = readFullAtomHeader(handle, ID32->atomhdr);
   if (bytesread < 0)
     return -1;
   totalbytesread += bytesread;

   if (ID32->atomhdr->type != ATOMTYPE_ID32)
     return -1;

   // next 2 bytes: top bit is padding, remaining 15 bits is Packed ISO-639-2/T language code 
   bytesread = readData(handle, handle->buf, 2);
   if (bytesread < 0)
     return -1;
   ID32->language = u16endian(*((mp4_u16 *)handle->buf));
   totalbytesread += bytesread;
   
   if ( handle->file )
       {
       ID32->atomcontentloc = handle->diskReadBufStart + handle->diskReadBufPos;
       }
   else
       {
       ID32->atomcontentloc = handle->absPosition;
       }
   
   bytesread = discardData(handle, ID32->atomhdr->size - totalbytesread );
   if (bytesread < 0)
     return -1;
   totalbytesread += bytesread;     
   
   return totalbytesread;
}

/*
 * Function:
 *
 *   mp4_i32 freeMETA(metaAtom *meta)
 *
 * Description:
 *
 *   This function frees memory for META atom.
 *
 * Parameters:
 *
 *   meta       META atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeMETA(metaAtom *meta)
{
  if (meta)
  {
    if (freeAtomHeader(meta->atomhdr) < 0)
      return -1;
    if (freeHDLR(meta->hdlr) < 0)
       return -1;
    if (freeID32(meta->ID32) < 0)
      return -1;

    mp4free(meta);
  }

  return 0;
}

/*
 * Function:
 *
 *   mp4_i32 freeID32(ID32Atom *ID32)
 *
 * Description:
 *
 *   This function frees memory for ID32 atom.
 *
 * Parameters:
 *
 *   ID32       ID32 atom pointer
 *
 * Return value:
 *
 *   0          Success
 *   Negative   Error
 *
 */
mp4_i32 freeID32(ID32Atom *ID32)
{
  if (ID32)
  {
    if (freeAtomHeader(ID32->atomhdr) < 0)
      return -1;

    mp4free(ID32);
  }

  return 0;
}
// End of File
