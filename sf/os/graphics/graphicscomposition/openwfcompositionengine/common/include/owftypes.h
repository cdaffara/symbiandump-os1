/* Copyright (c) 2009 The Khronos Group Inc.
 * Portions copyright (c) 2009-2010  Nokia Corporation and/or its subsidiary(-ies)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef _OWFTYPES_H_
#define _OWFTYPES_H_

#include "KHR/khrplatform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef khronos_int8_t      OWFint8;
typedef khronos_int16_t     OWFint16;
typedef khronos_int32_t     OWFint32;
typedef khronos_int32_t     OWFint;
typedef khronos_uint8_t     OWFuint8;
typedef khronos_uint16_t    OWFuint16;
typedef khronos_uint32_t    OWFuint32;
typedef khronos_uint32_t    OWFuint;
typedef khronos_float_t     OWFfloat;

typedef khronos_utime_nanoseconds_t OWFtime;

typedef OWFuint32 OWFHandle;

#define OWF_FOREVER (0xFFFFFFFFFFFFFFFFull)
#define OWF_INVALID_HANDLE ((OWFHandle)0)


/* Used for Function Prototypes */
/*! \brief OWF_API_CALL definition can be used to restrict
 *  visibility of symbols
 */

#ifndef OWF_API_CALL
#define OWF_API_CALL
#define OWF_PUBLIC
#endif
#ifndef OWF_APIENTRY
#define OWF_APIENTRY
#endif
#ifndef OWF_APIEXIT
#define OWF_APIEXIT
#endif
/* supported external image formats */
typedef enum {
    OWF_IMAGE_NOT_SUPPORTED = 0,
    OWF_IMAGE_ARGB8888      = 0x8888,
    OWF_IMAGE_XRGB8888      = 0xf888,
    OWF_IMAGE_RGB888        = 0x888,
    OWF_IMAGE_RGB565        = 0x565,
    OWF_IMAGE_L32           = 0xA32,
    OWF_IMAGE_L16           = 0xA16,
    OWF_IMAGE_L8            = 0xA8,
    OWF_IMAGE_L1            = 0xA1,
    OWF_IMAGE_ARGB_INTERNAL = 0x666 /* OWFpixel rep */
} OWF_PIXEL_FORMAT;

typedef enum {
    OWF_FALSE               = KHR_BOOLEAN_FALSE,
    OWF_TRUE                = KHR_BOOLEAN_TRUE
} OWFboolean;

typedef struct {
    OWF_PIXEL_FORMAT        pixelFormat;
    OWFboolean              linear;
    OWFboolean              premultiplied;
    OWFint                  rowPadding; /* row alignment, in bytes */
} OWF_IMAGE_FORMAT;

typedef struct {
    OWFint                  x;
    OWFint                  y;
    OWFint                  width;
    OWFint                  height;
} OWF_RECTANGLE;


#define MIN(a,b)            ((a)<(b)?(a):(b))
#define MAX(a,b)            ((a)>(b)?(a):(b))
#define CLAMP(x,a,b)        MAX(a,MIN(x,b))
#define INRANGE(x,a,b)      ((x) >= (a) && (x) <= (b))

typedef void*               OWF_MUTEX;
typedef void*               OWF_SEMAPHORE;

typedef struct OWF_NODE_
{
    void*                   data;
    struct OWF_NODE_*       next;
} OWF_NODE;

typedef OWFint (*NODECMPFUNC)(void*, void*);
typedef OWFint (*NODEITERFUNC)(void*, void*);

typedef OWFHandle           OWFNativeStreamType;
typedef OWFint              OWFNativeStreamBuffer;

/*!
 *  Events emitted by native streams.
 */

typedef struct{
    TInt32      length;
    TInt32      event;
} OWF_DEFAULT_EVENT_PARAM;

#define ALPHA_MASK          0xFF000000
#define RED_MASK            0xFF0000
#define GREEN_MASK          0xFF00
#define BLUE_MASK           0xFF

/*! Native stream callback function type */
typedef void (*OWFStreamCallback)( OWFNativeStreamType,
                                   OWFint,
                                   void*,
                                   void*);

typedef struct
{
    OWFStreamCallback       callback;
    void*                   data;
} OWFStreamCallbackData;

#define OWF_SUPPORTED_DEVICE_ID 1
#define OWF_RESERVED_BAD_SCREEN_NUMBER (-1)  

#ifdef __cplusplus
}
#endif

#endif /* _OWFTYPES_H_ */
