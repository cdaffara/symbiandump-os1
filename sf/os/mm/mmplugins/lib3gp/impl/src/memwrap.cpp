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
#include <stdlib.h>
#include <string.h>


/*
 * Function:
 *
 *   void *mp4malloc(size_t size)
 *
 * Description:
 *
 *   This is a wrapper for standard C library function calloc.
 *
 * Parameters:
 *
 *   size   Size of memory block to be allocated.
 *
 * Return value:
 *
 *   See calloc.
 *
 */
void *mp4malloc(size_t size)
	{
	TInt sizex = size;
  
	if ( sizex <= 0 || sizex >= KMaxTInt/2 )
		{
		return NULL;    // calloc will panic with negative values or size >= KMaxTInt / 2
		}
	else
		{
		return calloc(size, 1);
		}
	}


/*
 * Function:
 *
 *   void *mp4realloc(void *memblock,
 *                    size_t size,
 *                    size_t oldsize)
 *
 * Description:
 *
 *   This is a wrapper for standard C library function realloc.
 *
 * Parameters:
 *
 *   memblock   Allocated memory pointer
 *   size       New size
 *   oldsize    Size of memblock
 *
 * Return value:
 *
 *   See realloc.
 *
 */
void *mp4realloc(void *memblock, size_t size, size_t oldsize)
{
  void *p;

  p = realloc(memblock, size);
  if (p == NULL)
    return p;

  memset(((mp4_u8 *)p) + oldsize, 0, size - oldsize);

  return p;
}


/*
 * Function:
 *
 *   void mp4free(void *mem)
 *
 * Description:
 *
 *   This is a wrapper for standard C library function free.
 *
 * Parameters:
 *
 *   mem   Pointer to allocated memory
 *
 * Return value:
 *
 *   None
 *
 */
void mp4free(void *mem)
{
  free(mem);
}


/*
 * Function:
 *
 *   void *mp4memcpy(void *dest,
 *                   void *src,
 *                   mp4_u32 count)
 *
 * Description:
 *
 *   This is a wrapper for standard C library function memcpy.
 *
 * Parameters:
 *
 *   dest    Destination of copy
 *   src     Source of copy
 *   count   Number of nytes to copy
 *
 * Return value:
 *
 *   See memcpy.
 *
 */
void *mp4memcpy(void *dest, void *src, mp4_u32 count)
{
  memcpy(dest, src, count);

  return dest;
}

// End of File
