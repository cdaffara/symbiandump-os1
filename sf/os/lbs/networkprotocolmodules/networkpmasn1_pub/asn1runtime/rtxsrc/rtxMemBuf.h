// Copyright (c) 2001-2009 Objective Systems, Inc. (http://www.obj-sys.com) 
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available 
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Objective Systems, Inc - initial contribution.
//
// Contributors:
//
// Description:
// 

/**
 * @file rtxMemBuf.h
 */
/**
 * @defgroup buffermanfun Memory Buffer Management Functions 
 * @{
 *
 * Memory buffer management functions handle the allocation, expansion, and
 * deallocation of dynamic memory buffers used by some encode/decode functions.
 * Dynamic memory buffers are buffers that can grow or shrink to hold variable
 * sized amounts of data. This group of functions allows data to be appended to
 * buffers, to be set within buffers, and to be retrieved from buffers.
 * Currently, these functions are used within the generated SAX decode routines
 * to collect data as it is parsed by an XML parser.
 */

/* memory buffer */


/**
@file
@internalTechnology
*/

#ifndef _RTXMEMBUF_H_
#define _RTXMEMBUF_H_

#include "rtxsrc/rtxContext.h"

typedef struct OSRTMEMBUF {
   OSCTXT*      pctxt;
   OSUINT32     segsize;        /* segment size                     */
   OSUINT32     startidx;       /* start index of useful info       */
   OSUINT32     usedcnt;        /* used byte count                  */
   OSUINT32     bufsize;        /* buffer size                      */
   OSUINT32     bitOffset;      /* bit offset                       */
   OSUINT32     userState;      /* user state - any value, 0 is initial */
   OSOCTET*     buffer;         /* memory buffer                    */
   OSBOOL       isDynamic;      /* is buffer allocated dynamically? */
   OSBOOL       isExpandable;   /* is buffer expandable?            */
} OSRTMEMBUF;

#define OSMBDFLTSEGSIZE 1024

#define OSMEMBUFPTR(pmb)    ((pmb)->buffer + (pmb)->startidx)
#define OSMEMBUFENDPTR(pmb) ((pmb)->buffer + (pmb)->startidx + (pmb)->usedcnt)
#define OSMEMBUFUSEDSIZE(pmb) ((size_t)(pmb)->usedcnt)

#define OSMBAPPENDSTR(pmb,str) \
rtxMemBufAppend(pmb,(OSOCTET*)str,OSCRTLSTRLEN(str))

#define OSMBAPPENDUTF8(pmb,str) \
rtxMemBufAppend(pmb,(OSOCTET*)str,rtxUTF8LenBytes(str))

#ifdef __cplusplus
extern "C" {
#endif

/* Memory buffer functions */

/**
 * This function appends the data to the end of a memory buffer. If the buffer
 * was dynamic and full then the buffer will be reallocated. If it is static
 * (the static buffer was assigned by a call to rtxMemBufInitBuffer) or it is
 * empty (no memory previously allocated) then a new buffer will be allocated.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param pdata        The pointer to the buffer to be appended. The data will
 *                       be copied at the end of the memory buffer.
 * @param nbytes       The number of bytes to be copied from pData.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufAppend 
(OSRTMEMBUF* pMemBuf, const OSOCTET* pdata, size_t nbytes);

/**
 * This function cuts off the part of memory buffer. The beginning of the
 * cutting area is specified by offset "fromOffset" and the length is specified
 * by "nbytes". All data in this part will be lost. The data from the offset
 * "fromOffset + nbytes" will be moved to "fromOffset" offset.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param fromOffset   The offset of the beginning part, being cut off.
 * @param nbytes       The number of bytes to be cut off from the memory
 *                       buffer.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufCut (OSRTMEMBUF* pMemBuf, size_t fromOffset, size_t nbytes);

/**
 * This function frees the memory buffer. If memory was allocated then it will
 * be freed. Do not use the memory buffer structure after this function is
 * called.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 */
EXTERNRT void rtxMemBufFree (OSRTMEMBUF* pMemBuf);

/** 
 * This function returns the pointer to the used part of a memory buffer.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param length       The pointer to the length of the used part of the memory
 *                       buffer.
 * @return             The pointer to the used part of the memory buffer.
 */
EXTERNRT OSOCTET* rtxMemBufGetData (OSRTMEMBUF* pMemBuf, int* length);

/** 
 * This function returns the length of the used part of a memory buffer.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @return             The length of the used part of the buffer.
 */
EXTERNRT int rtxMemBufGetDataLen (OSRTMEMBUF* pMemBuf); 

/**
 * This function initializes a memory buffer structure. It does not allocate
 * memory; it sets the fields of the structure to the proper states. This
 * function must be called before any operations with the memory buffer.
 *
 * @param pCtxt        A provides a storage area for the function to store all
 *                       working variables that must be maintained between
 *                       function calls.
 * @param pMemBuf      A pointer to the initialized memory buffer structure.
 * @param segsize      The number of bytes in which the memory buffer will be
 *                       expanded incase it is full.
 */
EXTERNRT void rtxMemBufInit (OSCTXT* pCtxt, OSRTMEMBUF* pMemBuf, 
                              size_t segsize);

/**
 * This function assigns a static buffer to the memory buffer structure. It
 * does not allocate memory; it sets the pointer to the passed buffer. If
 * additional memory is required (for example, additional data is appended to
 * the buffer using rtxMemBufAppend), a dynamic buffer will be allocated and
 * all data copied to the new buffer.
 *
 * @param pCtxt        A pointer to a context structure. This provides a
 *                       storage area for the function t store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param buf          A pointer to the buffer to be assigned.
 * @param bufsize      The size of the buffer.
 * @param segsize      The number of bytes on which the memory buffer will be
 *                       expanded in case it is full.
 */
EXTERNRT void rtxMemBufInitBuffer (OSCTXT* pCtxt, OSRTMEMBUF* pMemBuf, 
                                    OSOCTET* buf, size_t bufsize, 
                                    size_t segsize);

/**
 * This function allocates a buffer with a predetermined amount of space.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param nbytes       The number of bytes to be copied from pData.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufPreAllocate (OSRTMEMBUF* pMemBuf, size_t nbytes);

/**
 * This function resets the memory buffer structure. It does not free memory,
 * just sets the pointer to the beginning and the used length to zero.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 */
EXTERNRT void rtxMemBufReset (OSRTMEMBUF* pMemBuf);

/**
 * This function sets part of a memory buffer to a specified octet value. The
 * filling is started from the end of the memory buffer. If the buffer is
 * dynamic and full, then the buffer will be reallocated. If it is static (a
 * static buffer was assigned by a call to rtxMemBufInitBuffer) or it is empty
 * (no memory previously was allocated) then a new buffer will be allocated.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param value        The pointer to the buffer to be appended. The data will
 *                       be copied at the end of the memory buffer.
 * @param nbytes       The number of bytes to be copied from pData.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufSet (OSRTMEMBUF* pMemBuf, OSOCTET value, size_t nbytes);

/**
 * This function sets "isExpandable" flag for the memory buffer object.
 * By default, this flag is set to TRUE, thus, memory buffer could be 
 * expanded, even if it was initialized by static buffer (see 
 * \c rtMemBufInitBuffer). If flag is cleared and buffer is full the
 * rtMemBufAppend/rtMemBufPreAllocate functions will return error status.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param isExpandable TRUE, if buffer should be expandable.
 * @return             Previous state of "isExpandable" flag.
 */
EXTERNRT OSBOOL rtxMemBufSetExpandable 
(OSRTMEMBUF* pMemBuf, OSBOOL isExpandable);

/**
 * This function trims white space of the memory buffer.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufTrimW (OSRTMEMBUF* pMemBuf); 

/**
 * @} buffermanfun
 */
#ifdef __cplusplus
}
#endif

#endif
