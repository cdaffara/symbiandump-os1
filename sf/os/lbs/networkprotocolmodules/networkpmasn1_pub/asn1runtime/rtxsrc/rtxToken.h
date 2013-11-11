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
 * @file rtxToken.h 
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXTOKEN_H_
#define _RTXTOKEN_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
   OSUTF8CHAR* pStr;
   const OSUTF8CHAR* pEndStr;
   const char* delimiters;
   size_t      delimSize;
   OSUTF8CHAR* pLastToken;
   size_t      lastTokenSize;
} OSTOKENCTXT;

EXTERNRT OSBOOL rtxTokIsWhiteSpace (OSTOKENCTXT* pTokCtxt, OSUTF8CHAR ch);

EXTERNRT OSUTF8CHAR* rtxTokSkipWhiteSpaces (OSTOKENCTXT* pTokCtxt);

EXTERNRT const OSUTF8CHAR* rtxTokGetNext (OSTOKENCTXT* pTokCtxt);

EXTERNRT const OSUTF8CHAR* rtxTokGetFirst 
(OSTOKENCTXT* pTokCtxt, OSUTF8CHAR* pStr, size_t strSize, 
 const char* delimiters, size_t delimSize);

#ifdef __cplusplus
}
#endif

#endif /*_RTXTOKEN_H_*/
