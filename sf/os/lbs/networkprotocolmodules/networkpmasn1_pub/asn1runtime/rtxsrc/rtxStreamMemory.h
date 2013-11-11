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
 * @file rtxStreamMemory.h
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXSTREAMMEMORY_H_
#define _RTXSTREAMMEMORY_H_

#include "rtxsrc/rtxStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @defgroup rtxStreamMemory Memory stream functions. 
 * Memory stream functions are used for memory stream operations. @{
 */
/**
 * Opens a memory stream. A memory buffer will be created by this function. The
 * 'flags' parameter specifies the access mode for the stream - input or
 * output.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamMemoryCreate (OSCTXT* pctxt, OSUINT16 flags);

/**
 * Opens a memory stream using the specified memory buffer. The 'flags'
 * parameter specifies the access mode for the stream - input or output.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pMemBuf      The pointer to the buffer.
 * @param bufSize      The size of the buffer.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamMemoryAttach (OSCTXT* pctxt, OSOCTET* pMemBuf, 
                                     size_t bufSize, OSUINT16 flags);

/**
 * This function returns the memory buffer and its size for the given memory
 * stream.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pSize        The pointer to size_t to receive the size of buffer.
 * @return             The pointer to memory buffer. NULL, if error occurred.
 */                                   
EXTERNRT OSOCTET* rtxStreamMemoryGetBuffer (OSCTXT* pctxt, size_t* pSize);

/**
 * This function creates an input memory stream using the specified buffer.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pMemBuf      The pointer to the buffer
 * @param bufSize      The size of the buffer
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */                                   
EXTERNRT int rtxStreamMemoryCreateReader 
   (OSCTXT* pctxt, OSOCTET* pMemBuf, size_t bufSize);

/**
 * This function creates an output memory stream using the specified buffer. If
 * \c pMemBuf or \c bufSize is NULL then new buffer will be allocated.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pMemBuf      The pointer to the buffer. Can be NULL - new buffer will
 *                       be allocated in this case.
 * @param bufSize      The size of the buffer. Can be 0 - new buffer will be
 *                       allocated in this case.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */                                   
EXTERNRT int rtxStreamMemoryCreateWriter 
   (OSCTXT* pctxt, OSOCTET* pMemBuf, size_t bufSize);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _RTXSTREAMMEMORY_H_ */

