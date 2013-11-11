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
 * @file rtxFloat.h
 */


/**
@file
@internalTechnology
*/

#ifndef _RTXFLOAT_H_
#define _RTXFLOAT_H_

#include "rtxsrc/rtxCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _NO_ULP_EQUAL

/* Fall back to normal equality test */
#define rtxFloatEqual(a, b)           (a == b)
#define rtxFloatGreater(a, b)         (a >  b)
#define rtxFloatGreaterOrEqual(a, b)  (a >= b)
#define rtxFloatSmaller(a, b)         (a <  b)
#define rtxFloatSmallerOrEqual(a, b)  (a <= b)
#ifndef __SYMBIAN32__
#define rtxDoubleEqual(a, b)          (a == b)
#define rtxDoubleGreater(a, b)        (a >  b)
#define rtxDoubleGreaterOrEqual(a, b) (a >= b)
#define rtxDoubleSmaller(a, b)        (a <  b)
#define rtxDoubleSmallerOrEqual(a, b) (a <= b)
#endif /* not building for Symbian */

#else /* _NO_ULP_EQUAL */

#define rtxFloatEqual(a, b) rtxFloatEqualImpl(a, b)
#define rtxFloatGreater(a, b)         ((a>b) && !rtxFloatEqual(a,b))
#define rtxFloatGreaterOrEqual(a, b)  ((a>b) ||  rtxFloatEqual(a,b))
#define rtxFloatSmaller(a, b)         ((a<b) && !rtxFloatEqual(a,b))
#define rtxFloatSmallerOrEqual(a, b)  ((a<b) ||  rtxFloatEqual(a,b))
#ifndef __SYMBIAN32__
#define rtxDoubleEqual(a, b) rtxDoubleEqualImpl(a, b)
#define rtxDoubleGreater(a, b)        ((a>b) && !rtxDoubleEqual(a,b))
#define rtxDoubleGreaterOrEqual(a, b) ((a>b) ||  rtxDoubleEqual(a,b))
#define rtxDoubleSmaller(a, b)        ((a<b) && !rtxDoubleEqual(a,b))
#define rtxDoubleSmallerOrEqual(a, b) ((a<b) ||  rtxDoubleEqual(a,b))
#endif /* not building for Symbian */

#endif /* _NO_ULP_EQUAL */

EXTERNRT OSBOOL rtxFloatEqualImpl(float a, float b);

#ifndef __SYMBIAN32__
EXTERNRT OSBOOL rtxDoubleEqualImpl(double a, double b);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTXFLOAT_H_ */

