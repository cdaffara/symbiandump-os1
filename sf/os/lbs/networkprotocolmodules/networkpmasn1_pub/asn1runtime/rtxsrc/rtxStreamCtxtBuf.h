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

/* 
 * @file rtxStreamCtxtBuf.h 
 * Stream functions that work directly with the context buffer. 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXSTREAMCTXTBUF_H_
#define _RTXSTREAMCTXTBUF_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"
#include "rtxsrc/rtxStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function create and initializes a context-buffered stream. This 
 * function should be called first before any operation with the stream.
 *
 * @param pctxt        Pointer to context structure variable.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamCtxtBufCreate (OSCTXT* pctxt);

/*
 * This function pre-reads a 'size' number of octets of data from the input
 * stream into the internal stream buffer. This functions blocks until all
 * necessary data is read, end of file is detected, or another error 
 * occurs. This function guarantees that the internal buffer will contain at
 * least the 'size' number of octets. Thus, it is possible to obtain direct
 * access to the memory buffer with pre-read data by using the 'buffer' field
 * of the OSCTXT context block.
 *
 * @param pctxt        Pointer to a context structure variable that has 
 *                       been initialized for stream operations.
 * @param size         Number of octets to be pre-read.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamCtxtBufPreRead (OSCTXT* pctxt, size_t size);

/*
 * This function checks if the stream is already context-buffered or not.
 * If not, the stream will be created above the existing stream.
 * If the stream cannot be opened, an error code is returned.
 *
 * @param pctxt        Pointer to a context structure variable that has 
 *                       been initialized for stream operations.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamCtxtBufCheck (OSCTXT* pctxt);

/*
 * This function frees a context-buffered stream and restores the underlying stream. 
 *
 * @param pctxt        Pointer to context structure variable.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamCtxtBufFree (OSCTXT* pctxt);

#ifdef __cplusplus
}
#endif

#endif /* _RTXSTREAMCTXTBUF_H_ */

