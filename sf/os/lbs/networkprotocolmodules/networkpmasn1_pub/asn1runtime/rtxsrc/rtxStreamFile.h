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
 * @file rtxStreamFile.h
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXSTREAMFILE_H_
#define _RTXSTREAMFILE_H_

#include <stdio.h>
#include "rtxsrc/rtxStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @defgroup rtxStreamFile File stream functions. 
 * File stream functions are used for stream operations with files. @{
 */
/** 
 * Attaches the existing file structure pointer to the stream. The file should
 * be already opened either for the reading or writing. The 'flags' parameter
 * specifies the access mode for the stream - input or output.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pFile        Pointer to FILE structure. File should be already opened
 *                       either for the writing or reading.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamFileAttach (OSCTXT* pctxt, 
                                   FILE* pFile, OSUINT16 flags);

/**
 * Opens a file stream. The 'flags' parameter specifies the access mode for the
 * stream - input or output.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pFilename    Pointer to null-terminated string that contains the name
 *                       of file.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamFileOpen (OSCTXT* pctxt, 
                                 const char* pFilename, OSUINT16 flags);

/**
 * This function creates an input file stream using the specified file name.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pFilename    Pointer to null-terminated string that contains the name
 *                       of file.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */                                   
EXTERNRT int rtxStreamFileCreateReader (OSCTXT* pctxt, const char* pFilename);

/**
 * This function creates an output file stream using the file name.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pFilename    Pointer to null-terminated string that contains the name
 *                       of file.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */                                   
EXTERNRT int rtxStreamFileCreateWriter (OSCTXT* pctxt, const char* pFilename);


#ifdef __cplusplus
}
#endif
/**
 * @} rtxStreamFile 
 */
#endif /* _RTXSTREAMFILE_H_ */

