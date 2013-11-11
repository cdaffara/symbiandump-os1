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
 * @file rtxDecimal.h 
 * Common runtime functions for working with xsd:decimal numbers.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXDECIMAL_H_
#define _RTXDECIMAL_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtxDecimal Decimal number utility functions
 * @{
 * Decimal utility function provide run-time functions for handling 
 * decimal number types defined within a schema.
 */
/* These need to be documented.. */

EXTERNRT const char* rtxNR3toDecimal (OSCTXT *pctxt, const char* object_p); 

/**
 * @} rtxDecimal
 */

#ifdef __cplusplus
}
#endif

#endif
