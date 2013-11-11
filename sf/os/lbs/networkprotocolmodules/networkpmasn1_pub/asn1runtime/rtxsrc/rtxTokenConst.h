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
   const OSUTF8CHAR* pStr;
   const OSUTF8CHAR* pEndStr;
   const char* delimiters;
   size_t      delimSize;

   const OSUTF8CHAR* pLastToken;
   size_t      lastTokenSize;

   OSBOOL      bFinal;
} OSCONSTTOKENCTXT;

EXTERNRT OSBOOL 
rtxTokIsWhiteSpaceConst (OSCONSTTOKENCTXT* pTokCtxt, OSUTF8CHAR ch);

EXTERNRT const OSUTF8CHAR* 
rtxTokSkipWhiteSpacesConst (OSCONSTTOKENCTXT* pTokCtxt);

EXTERNRT const OSUTF8CHAR* 
rtxTokGetNextConst (OSCONSTTOKENCTXT* pTokCtxt, size_t* pTokenLength);

EXTERNRT const OSUTF8CHAR* 
rtxTokGetFirstConst (OSCONSTTOKENCTXT* pTokCtxt, 
                     const OSUTF8CHAR* pStr, size_t strSize, 
                     const char* delimiters, size_t delimSize, 
                     size_t* pTokenLength, OSBOOL isFinal);

#ifdef __cplusplus
}
#endif

#endif
