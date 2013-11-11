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
 * @file rtxPattern.h
 * Pattern matching functions.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXPATTERN_H_
#define _RTXPATTERN_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"
#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ccfPattern Pattern matching functions 
 * @{
 *
 * These functions handle pattern matching which is required to to process XML
 * schema pattern constraints.
 */
/**
 * This function compares the given string to the given pattern. It returns
 * true if match, false otherwise.
 *
 * @param pctxt        Pointer to context structure.
 * @param text         Text to be matched.
 * @param pattern      Regular expression.
 * @return             Boolean result.
 */
EXTERNRT OSBOOL rtxMatchPattern (OSCTXT* pctxt, 
                                  const OSUTF8CHAR* text, 
                                  const OSUTF8CHAR* pattern);

EXTERNRT OSBOOL rtxMatchPattern2 (OSCTXT* pctxt, 
                                   const OSUTF8CHAR* pattern);
/**
 * @} ccfPattern
 */

#ifdef __cplusplus
}
#endif

#endif
