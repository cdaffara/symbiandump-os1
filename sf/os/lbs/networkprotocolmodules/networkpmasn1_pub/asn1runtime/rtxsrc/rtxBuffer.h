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
 * @file rtxBuffer.h 
 * Common runtime functions for reading from or writing to the message 
 * buffer defined within the context structure.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXBUFFER_H_
#define _RTXBUFFER_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

EXTERNRT OSINT32 rtxCheckBuffer (OSCTXT* pctxt, size_t nbytes);

EXTERNRT OSINT32 rtxCopyAsciiText (OSCTXT* pctxt, const char* text);

EXTERNRT OSINT32 rtxCopyUTF8Text (OSCTXT* pctxt, const OSUTF8CHAR* text);

EXTERNRT OSINT32 rtxCopyUnicodeText (OSCTXT* pctxt, const OSUNICHAR* text);

EXTERNRT int rtxWriteBytes 
(OSCTXT* pctxt, const OSOCTET* pdata, size_t nocts);

#ifdef __cplusplus
}
#endif

#endif
