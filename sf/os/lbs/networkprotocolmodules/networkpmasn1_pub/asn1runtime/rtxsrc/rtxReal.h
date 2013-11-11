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
 * @file rtxReal.h 
 * Common runtime functions for working with floating-point numbers.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXREAL_H_
#define _RTXREAL_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtxReal Floating-point number utility functions
 * @{
 * Floating-point utility function provide run-time functions for handling 
 * floating-point number types defined within a schema.
 */

/**
 * Returns the IEEE negative infinity value.  This is defined as
 * 0xfff0000000000000 in IEEE standard 754.  We assume the presence of the
 * IEEE double type, that is, 64-bits of precision. */
EXTERNRT OSREAL rtxGetMinusInfinity (void);

/**
 * Returns the IEEE minus zero value.  This is defined as
 * 0x8000000000000000 in IEEE standard 754.  We assume the presence of the
 * IEEE double type, that is, 64-bits of precision. */
EXTERNRT OSREAL rtxGetMinusZero (void);

/**
 * Returns the IEEE Not-A-Number (NaN) value.  This is defined as
 * 0x7ff8000000000000 in IEEE standard 754.  We assume the presence of the
 * IEEE double type, that is, 64-bits of precision. */
EXTERNRT OSREAL rtxGetNaN (void);

/**
 * Returns the IEEE posative infinity value.  This is defined as
 * 0x7ff0000000000000 in IEEE standard 754.  We assume the presence of the
 * IEEE double type, that is, 64-bits of precision. */
EXTERNRT OSREAL rtxGetPlusInfinity (void);

/**
 * A utility function that compares the given input value to the IEEE 754
 * value for negative infinity.
 *
 * @param   value    The input real value. */
EXTERNRT OSBOOL rtxIsMinusInfinity (OSREAL value);

/**
 * A utility function that compares the given input value to the IEEE 754
 * value for minus zero.
 *
 * @param   value    The input real value. */
EXTERNRT OSBOOL rtxIsMinusZero (OSREAL value);

/**
 * A utility function that compares the given input value to the IEEE 754
 * value for Not-A-Number (NaN).
 *
 * @param   value    The input real value. */
EXTERNRT OSBOOL rtxIsNaN (OSREAL value);

/**
 * A utility function that compares the given input value to the IEEE 754
 * value for positive infinity.
 *
 * @param   value    The input real value. */
EXTERNRT OSBOOL rtxIsPlusInfinity (OSREAL value);

/**
 * @} rtxReal
 */

#ifdef __cplusplus
}
#endif

#endif
