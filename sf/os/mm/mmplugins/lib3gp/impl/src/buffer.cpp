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

#include "mp4atom.h"
#include <3gplibrary/mp4config.h>
#include "mp4buffer.h"
#include "mp4memwrap.h"
#include "mp4file.h"
#include "mp4list.h"



/*
 * Function:
 *
 *   mp4_i32 addData(MP4HandleImp handle,
 *                   mp4_u8 *buffer,
 *                   mp4_u32 bytestowrite)
 *
 * Description:
 *
 *   This function allocates memory for the data and copies it from
 *   buffer to the new allocated buffer.
 *
 * Parameters:
 *
 *   handle        MP4 library handle
 *   buffer        Buffer containing data
 *   bytestowrite  Size of buffer in bytes
 *
 * Return value:
 *
 *   Negative      Error
 *   0             Success
 *
 */
mp4_i32 addData(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestowrite)
{
  void *newBuffer;

  newBuffer = mp4malloc(bytestowrite);
  if (newBuffer == NULL)
    return -1;

  mp4memcpy(newBuffer, buffer, bytestowrite);

  if (listAppend(handle->mem, newBuffer, bytestowrite)) /* Success */
    return 0;
  else
	  {
	  if (newBuffer)
		  {
		  delete newBuffer;
		  newBuffer = NULL;
		  }
	  return -1;
	  }
}


/*
 * Function:
 *
 *   mp4_u32 getBufferedBytes(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function returns the number of bytes in the library internal
 *   buffers.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0   The input is in a file and therefore no memory is used to store MP4
 *       data or no memory in buffers.
 *   >0  Number of bytes stored in the library internal buffers
 *
 */
mp4_u32 getBufferedBytes(MP4HandleImp handle)
{
  if (handle->file)
    return 0;

  return listBytesInList(handle->mem);
}


/*
 * Function:
 *
 *   mp4_u32 getCumulativeBufferedBytes(MP4HandleImp handle)
 *
 * Description:
 *
 *   This function returns the number of bytes passed through the library
 *   internal buffers.
 *
 * Parameters:
 *
 *   handle   MP4 library handle
 *
 * Return value:
 *
 *   0   The input is in a file and therefore no memory is used to store MP4
 *       data or no memory in buffers.
 *   >0  Number of bytes stored in the library internal buffers
 *
 */
mp4_u32 getCumulativeBufferedBytes(MP4HandleImp handle)
{
  if (handle->file)
    return 0;

  return listCumulativeBytesInList(handle->mem);
}


/*
 * Function:
 *
 *   mp4_i32 readData(MP4HandleImp handle,
 *                    mp4_u8 *buffer,
 *                    mp4_u32 bytestoread)
 *
 * Description:
 *
 *   This function reads bytestoread bytes from memory buffers or file
 *   to buffer.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   buffer       Caller allocated buffer for the data
 *   bytestoread  Number of bytes to read
 *
 * Return value:
 *
 *   >= 0         Success. Value tells the number of bytes read.
 *   -1           File has not been opened
 *   -2           End of file or file error
 *   -10          Not enough data in memory
 *
 */
mp4_i32 readData(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread)
{
  if (handle->file) /* Input is in a file */
  {
    switch (readFile(handle, buffer, bytestoread))
    {
    case -2: /* EOF or error */
      return -2;
    case -1: /* File not open */
      return -1;
    case 0: /* Ok */
      return bytestoread;
    default:
      break;
    }
  }
  else /* Input is in memory list */
  {
    mp4_u32 i, j;
    node_s *node;

    if (handle->mem->bytesInList - handle->absPosition < bytestoread)
      return -10;

    i = 0;
    j = handle->absPosition;

    node = handle->mem->first;

    while (i < bytestoread)
    {
      if ((mp4_i32)(node->dataSize - j) <= 0)
      {
        j -= node->dataSize;
        node = node->next;
        continue;
      }

      {
        mp4_u32 k;

        k = node->dataSize - j >= bytestoread - i ? bytestoread - i : node->dataSize - j;

        mp4memcpy(buffer + i, ((mp4_u8 *)node->data) + j, k);
        i += k;
        j += k;
      }
    }

    handle->position = j;
    handle->absPosition += bytestoread;

    node = handle->mem->first;
  }

  return bytestoread;
}

/*
 * Function:
 *
 *   mp4_i32 peekData(MP4HandleImp handle,
 *                    mp4_u8 *buffer,
 *                    mp4_u32 bytestoread)
 *
 * Description:
 *
 *   This function reads bytestoread bytes from memory buffers or file
 *   to buffer but doesn't change the internal position in the file/stream.
 *
 * Parameters:
 *
 *   handle       MP4 library handle
 *   buffer       Caller allocated buffer for the data
 *   bytestoread  Number of bytes to read
 *
 * Return value:
 *
 *   >= 0         Success. Value tells the number of bytes read.
 *   -1           File has not been opened
 *   -2           End of file or file error
 *   -3           fseek failed
 *   -10          Not enough data in memory
 *
 */
mp4_i32 peekData(MP4HandleImp handle, mp4_u8 *buffer, mp4_u32 bytestoread)
{
  if (handle->file) /* Input is in a file */
  {
    switch (peekFile(handle, buffer, bytestoread))
    {
    case -3: /* fseek failed */
      return -3;
    case -2: /* EOF or error */
      return -2;
    case -1: /* File not open */
      return -1;
    case 0: /* Ok */
      return bytestoread;
    default:
      break;
    }
  }
  else /* Input is in memory list */
  {
    mp4_u32 i, j;
    node_s *node;

    if ((mp4_i32)(handle->mem->bytesInList - handle->absPosition) < (mp4_i32)bytestoread)
      return -10;

    i = 0;
    j = handle->absPosition;

    node = handle->mem->first;

    while (i < bytestoread)
    {
      if ((mp4_i32)(node->dataSize - j) <= 0)
      {
        j -= node->dataSize;
        node = node->next;
        continue;
      }

      {
        mp4_u32 k;

        k = node->dataSize - j >= bytestoread - i ? bytestoread - i : node->dataSize - j;

        mp4memcpy(buffer + i, ((mp4_u8 *)node->data) + j, k);
        i += k;
        j += k;
      }
    }
  }

  return bytestoread;
}


/*
 * Function:
 *
 *   mp4_i32 discardData(MP4HandleImp handle,
 *                       mp4_i32 bytesToDiscard)
 *
 * Description:
 *
 *   This function reads and discards bytesToDiscard bytes from file/stream.
 *
 * Parameters:
 *
 *   handle             MP4 library handle
 *   bytesToDiscard     This many bytes are discarded
 *
 * Return value:
 *
 *   Negative integer   Error
 *   >= 0               Success. Value tells how many bytes were read.
 *
 */
mp4_i32 discardData(MP4HandleImp handle, mp4_i32 bytesToDiscard)
{
  mp4_i32 bytesread;
  mp4_i32 bytestoread;
  mp4_i32 totalbytesread = 0;

  while (totalbytesread < bytesToDiscard)
  {
    bytestoread = bytesToDiscard - totalbytesread;
    if (bytestoread > TMPBUFSIZE)
      bytestoread = TMPBUFSIZE;

    bytesread = readData(handle, handle->buf, bytestoread);
    if (bytesread < 0)
      return -1;
    totalbytesread += bytesread;
  }

  return totalbytesread;
}
// End of File
