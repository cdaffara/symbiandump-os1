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
 * @file rtxBase64.h 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXBASE64_H_
#define _RTXBASE64_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode binary data into base64 string form to a dynamic buffer.
 *
 * @param pctxt        Pointer to context structure.
 * @param pSrcData     Pointer to binary data to encode.
 * @param srcDataSize  Length of the binary data in octets.
 * @param ppDstData    Pointer to pointer variable to hold address of 
 *                       dynamically allocated buffer the encoded base64
 *                       string. 
 * @return             Completion status of operation:
 *                       - number of binary bytes written
 *                       - negative return value is error.
 */
EXTERNRT long rtxBase64EncodeData (OSCTXT* pctxt, const char* pSrcData, 
                                    size_t srcDataSize, OSOCTET** ppDstData);

/**
 * Decode base64 string to binary form into a dynamic buffer.
 *
 * @param pctxt        Pointer to context structure.
 * @param pSrcData     Pointer to base64 string to decode.
 * @param srcDataSize  Length of the base64 string.
 * @param ppDstData    Pointer to pointer variable to hold address of 
 *                       dynamically allocated buffer to hold data. 
 * @return             Completion status of operation:
 *                       - number of binary bytes written
 *                       - negative return value is error.
 */
EXTERNRT long rtxBase64DecodeData 
(OSCTXT* pctxt, const char* pSrcData, size_t srcDataSize, OSOCTET** ppDstData);

/**
 * Decode base64 string to binary form into a fixed-size buffer.
 *
 * @param pctxt        Pointer to context structure.
 * @param pSrcData     Pointer to base64 string to decode.
 * @param srcDataSize  Length of the base64 string.
 * @param buf          Address of buffer to receive decoded binary data.
 * @param bufsiz       Size of output buffer.
 * @return             Completion status of operation:
 *                       - number of binary bytes written
 *                       - negative return value is error.
 */
EXTERNRT long rtxBase64DecodeDataToFSB 
(OSCTXT* pctxt, const char* pSrcData, size_t srcDataSize, 
 OSOCTET* buf, size_t bufsiz);

/**
 * Calculate number of byte required to hold a decoded base64 string
 * in binary form.
 *
 * @param pSrcData     Pointer to base64 string to decode.
 * @param srcDataSize  Length of the base64 string.
 * @return             Completion status of operation: 
 *                       If success, positive value is number of bytes, 
 *                       If failure, negative status code.
 */
EXTERNRT long rtxBase64GetBinDataLen
(const char* pSrcData, size_t srcDataSize);

#ifdef __cplusplus
}
#endif

#endif /* RTXBASE64 */

