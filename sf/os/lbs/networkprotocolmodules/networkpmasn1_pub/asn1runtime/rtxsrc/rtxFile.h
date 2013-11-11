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
 * @file rtxFile.h 
 * Common runtime functions for reading from or writing to files.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXFILE_H_
#define _RTXFILE_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function reads the entire contents of a binary file into memory. A
 * memory buffer is allocated for the file contents using the run-time
 * memory management functions.
 *
 * @param pctxt        Pointer to context block structure.
 * @param filePath     Complete file path name of file to read.
 * @param ppMsgBuf     Pointer to message buffer to receive allocated memory
 *                       pointer.
 * @param pLength      Pointer to integer to receive length of data read.
 * @return             Completion status of operation:
 *                       - 0 (ASN_OK) = success,
 *                       - RTERR_FILNOTFOU = file not found
 *                       - RTERR_FILEREAD = file read error (see errno)
 */
EXTERNRT int rtxFileReadBinary 
(OSCTXT* pctxt, const char* filePath, OSOCTET** ppMsgBuf, size_t* pLength);

/**
 * This function reads the entire contents of an ASCII text file into 
 * memory. A memory buffer is allocated for the file contents using the 
 * run-time memory management functions.  This function is identical to 
 * rtxReadFileBinary except that a) the file is opened in text mode, and 
 * b) and extra byte is allocated at the end for a null-terminator 
 * character.
 *
 * @param pctxt        Pointer to context block structure.
 * @param filePath     Complete file path name of file to read.
 * @param ppMsgBuf     Pointer to message buffer to receive allocated memory
 *                       pointer.
 * @param pLength      Pointer to integer to receive length of data read.
 * @return             Completion status of operation:
 *                       - 0 (ASN_OK) = success,
 *                       - RTERR_FILNOTFOU = file not found
 *                       - RTERR_FILEREAD = file read error (see errno)
 */
EXTERNRT int rtxFileReadText 
(OSCTXT* pctxt, const char* filePath, OSOCTET** ppMsgBuf, size_t* pLength);

/**
 * This function writes binary data from memory to the given file.
 *
 * @param filePath     Complete file path name of file to be written to.
 * @param pMsgBuf      Pointer to buffer containing data to be written.
 * @param length       Size (in bytes) of data to be written
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative status code if error
 */
EXTERNRT int rtxFileWriteBinary 
(const char* filePath, const OSOCTET* pMsgBuf, size_t length);

#ifdef __cplusplus
}
#endif

#endif
