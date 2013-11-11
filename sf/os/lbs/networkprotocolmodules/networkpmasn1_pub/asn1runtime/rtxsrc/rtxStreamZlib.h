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
 * @file rtxStreamZlib.h 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXSTREAMZLIB_H_
#define _RTXSTREAMZLIB_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"
#include "rtxsrc/rtxStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function initializes a zlib compressed stream. This function should
 *  be called first before any operation with a zlib compressed stream.
 *
 * @param pctxt        Pointer to context structure variable.
 * @param flags        Specifies the access mode for the stream: 
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamZlibAttach (OSCTXT* pctxt, OSUINT16 flags);

/*
 * This function finish compressed block. Stream switched to transparent mode.
 *
 * @param pctxt        Pointer to context structure variable.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamZlibEnd (OSCTXT* pctxt);

/*
 * This function free a zlib compressed stream and restore underlying stream. 
 *
 * @param pctxt        Pointer to context structure variable.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamZlibFree (OSCTXT* pctxt);

/*
 * This function finish compressed block and start new block. 
 * Stream remained in compressed mode.
 *
 * @param pctxt        Pointer to context structure variable.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamZlibReset (OSCTXT* pctxt);

/*
 * This function start compression parameters. 
 *
 * @param pctxt        Pointer to context structure variable.
 * @param level        Set compression level 0..9.
 *                       0 - Z_NO_COMPRESSION
 *                       1 - Z_BEST_SPEED
 *                       9 - Z_BEST_COMPRESSION
 *                      -1 - Z_DEFAULT_COMPRESSION - like level 6
 * @param windowBits   Set decompression window size.
 *                       8..15 - ZLIB header (RFC 1950)
 *                       -8..-15 - header is absent (RFC 1951)
 *                       (8..15) + 16 - GZIP header (RFC 1952)
 * @param memLevel     Set memory usage level 1..9.
 *                       1 - use minimum memory
 *                       9 - use maximum memory
 * @param strategy     Set copression algorithm.
 *                       0 - Z_DEFAULT_STRATEGY - common method any data
 *                       other possible values defined in zlib.h
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamZlibSetDeflateParams 
(OSCTXT* pctxt, int level, int windowBits, int memLevel, int strategy);

/*
 * This function start decompression parameter. 
 *
 * @param pctxt        Pointer to context structure variable.
 * @param windowBits   Set decompression window size.
 *                       8..15 - ZLIB header (RFC 1950)
 *                       -8..-15 - header is absent (RFC 1951)
 *                       (8..15) + 16 - GZIP header (RFC 1952)
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamZlibSetInflateParams 
(OSCTXT* pctxt, int windowBits);

/*
 * This function start compressed block. 
 *
 * @param pctxt        Pointer to context structure variable.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamZlibStart (OSCTXT* pctxt);

#ifdef __cplusplus
}
#endif

#endif /* _RTXSTREAMZLIB_H_ */

