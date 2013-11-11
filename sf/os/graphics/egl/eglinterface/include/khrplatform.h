#ifndef __khrplatform_h_
#define __khrplatform_h_

/* $Revision$ on $Date::   $ */

/*
** Copyright (c) 2008 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

/* Platform-specific types and definitions.
 * Last modified on 2008/12/19
 * 
 * Adopters may modify this file to suit their platform. Adopters are
 * encouraged to submit platform specific modifications to the Khronos
 * group so that they can be included in future versions of this file.
 * Please submit changes by sending them to the public Khronos Bugzilla
 * (http://khronos.org/bugzilla) by filing a bug against product
 * "Khronos (general)" component "Registry".
 *
 * A predefined template which fills in some of the bug fields can be
 * reached using http://tinyurl.com/khrplatform-h-bugreport, but you
 * must create a Bugzilla login first.
 * 
 *
 *
 * This file should be included as
 *        #include <KHR/khrplatform.h>
 * by the Khronos API header file that uses its types and defines.
 *
 * The types in this file should only be used to define API-specific types.
 * Types defined in this file:
 *    khronos_int8_t              signed   8  bit
 *    khronos_uint8_t             unsigned 8  bit
 *    khronos_int16_t             signed   16 bit
 *    khronos_uint16_t            unsigned 16 bit
 *    khronos_int32_t             signed   32 bit
 *    khronos_uint32_t            unsigned 32 bit
 *    khronos_int64_t             signed   64 bit
 *    khronos_uint64_t            unsigned 64 bit
 *    khronos_intptr_t            signed   same number of bits as a pointer
 *    khronos_uintptr_t           unsigned same number of bits as a pointer
 *    khronos_ssize_t             signed   size
 *    khronos_usize_t             unsigned size
 *    khronos_float_t             signed   32 bit floating point
 *    khronos_time_ns_t           unsigned 64 bit time in nanoseconds
 *    khronos_utime_nanoseconds_t unsigned time interval or absolute time in
 *                                         nanoseconds
 *    khronos_stime_nanoseconds_t signed time interval in nanoseconds
 *
 * KHRONOS_SUPPORT_INT64 is 1 if 64 bit integers are supported; otherwise 0.
 * KHRONOS_SUPPORT_FLOAT is 1 if floats are supported; otherwise 0.
 * 
 *
 * Macros defined in this file:
 *    KHRONOS_APICALL
 *    KHRONOS_APIENTRY
 *    KHRONOS_APIATTRIBUTES
 * These may be used in function prototypes as:
 *      KHRONOS_APICALL void KHRONOS_APIENTRY funcname(
 *                                  int arg1,
 *                                  int arg2) KHRONOS_APIATTRIBUTES;
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <e32def.h>

/*-------------------------------------------------------------------------
 * Definition of KHRONOS_APICALL and KHRONOS_APIENTRY
 *-----------------------------------------------------------------------*/

#if defined(AEE_SIMULATOR)
#define __KHR_EXPORTS
#endif

#if defined(_WIN32) || defined(__VC32__) || defined(__MWERKS__) || defined(__CW32__)         /* Win32 */
#   ifdef __KHR_EXPORTS
#       define KHRONOS_APICALL __declspec(dllexport)
#   else
#       define KHRONOS_APICALL __declspec(dllexport)
#   endif
#else
#   if defined (__ARMCC_VERSION)
#       if (__ARMCC_VERSION <= 310000) || (__ARMCC_VERSION >= 400000)
#          ifdef __KHR_EXPORTS
#            define KHRONOS_APICALL __declspec(dllexport)
#          else
#            define KHRONOS_APICALL __declspec(dllimport)
#          endif
#       else
#         define KHRONOS_APICALL __declspec(dllimport)
#       endif
#   else
#       ifdef __KHR_EXPORTS
#           define KHRONOS_APICALL
#       else
#           define KHRONOS_APICALL extern
#       endif
#   endif
#endif

#define KHRONOS_APIENTRY

#ifndef APIENTRY
#   define APIENTRY KHRONOS_APIENTRY
#endif

#ifndef KHRAPI
#	define KHRAPI KHRONOS_APICALL
#endif

#if defined(__ARMCC__)
#define KHRONOS_APIATTRIBUTES __softfp
#else
#define KHRONOS_APIATTRIBUTES
#endif

#define KHRONOS_SUPPORT_INT64   0
#define KHRONOS_SUPPORT_FLOAT   0

typedef TReal32 khronos_float;    // float
typedef TReal32 khronos_float_t;    // float
typedef TInt8   khronos_int8_t;   // signed char
typedef TUint8  khronos_uint8_t;  // unsigned char
typedef TInt16  khronos_int16_t;  // short int
typedef TUint16 khronos_uint16_t; // unsigned short int
typedef TInt32  khronos_int32_t;  // long int
typedef TUint32 khronos_uint32_t; // unsigned long int
typedef TInt64  khronos_int64_t;  // long int
typedef TUint64 khronos_uint64_t; // unsigned long int

typedef TUint64	khronos_utime_nanoseconds_t;
typedef TInt32	khronos_stime_nanoseconds_t;

#ifdef __cplusplus
}
#endif

/*
 * Platform definition of TRUE and FALSE
 */
#define KHR_BOOLEAN_TRUE    (0 == 0)
#define KHR_BOOLEAN_FALSE    (0 != 0)

#endif /* __khrplatform_h_ */
