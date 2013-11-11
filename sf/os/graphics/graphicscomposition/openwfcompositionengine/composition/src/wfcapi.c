/* Copyright (c) 2009-2010 The Khronos Group Inc.
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

/*! \ingroup wfc
 *  \file wfcapi.c
 *  \brief OpenWF Composition SI, API implementation.
 *
 *  For function documentations, see OpenWF Composition specification 1.0
 *
 *  The general layout of an API function is:
 *  - grab API mutex (lock API)
 *  - check parameter validity
 *  - invoke implementation function (WFD_...)
 *  - unlock API
 *  - return
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <WF/wfc.h>
#include <WF/wfcext.h>

#include "wfcstructs.h"
#include "wfccontext.h"
#include "owfnativestream.h"
#include "owfscreen.h"

#define RGB_NUM_BYTES       (sizeof(WFCuint8) * 4)

#define COND_FAIL(cond,error,retval) \
    if (!(cond)) { \
        WFC_Device_SetError(dev, error); \
        WFC_Unlock(); \
        return retval; \
    }

#define COND_FAIL_NR(cond,error) \
    if (!(cond)) { \
        WFC_Device_SetError(dev, error); \
        WFC_Unlock(); \
        return; \
    }

#define GET_DEVICE(d, h, x) \
    WFC_Lock(); \
    d = WFC_Device_FindByHandle(h); \
    if (NULL == d) { \
        WFC_Unlock(); \
        return x; \
    }

#define GET_DEVICE_NR(d, h) \
    WFC_Lock(); \
    d = WFC_Device_FindByHandle(h); \
    if (WFC_INVALID_HANDLE == d) { \
        WFC_Unlock(); \
        return; \
    }

#define GET_CONTEXT_NR(c,d,h) \
    c = WFC_Device_FindContext(d, h); \
    if (WFC_INVALID_HANDLE == c) { \
        WFC_Device_SetError((d)->handle, WFC_ERROR_BAD_HANDLE); \
        WFC_Unlock(); \
        return; \
    }

#define GET_CONTEXT(c,d,h,x) \
    c = WFC_Device_FindContext(d, h); \
    if (WFC_INVALID_HANDLE == c) { \
        WFC_Device_SetError((d)->handle, WFC_ERROR_BAD_HANDLE); \
        WFC_Unlock(); \
        return x; \
    }

#define SUCCEED(retval) \
    WFC_Device_SetError(dev, WFC_ERROR_NONE); \
    WFC_Unlock(); \
    return retval

#define SUCCEED_NR() \
    WFC_Device_SetError(dev, WFC_ERROR_NONE); \
    WFC_Unlock(); \
    return

#define FAIL(err, retval) \
    WFC_Device_SetError(dev, err); \
    WFC_Unlock(); \
    return retval

#define FAIL_NR(err) \
    WFC_Device_SetError(dev, err); \
    WFC_Unlock(); \
    return


static OWF_MUTEX           mutex;

static void WFC_Cleanup()
{
    OWF_Mutex_Destroy(&mutex);
}

static void WFC_Lock()
{
    if (!mutex)
    {
        OWF_Mutex_Init(&mutex);
        xatexit(WFC_Cleanup);   // Use adaptation version of atexit
    }
    OWF_Mutex_Lock(&mutex);
}

static void WFC_Unlock()
{
    OWF_Mutex_Unlock(&mutex);
}

/*=========================================================================*/
/*  4. DEVICE                                                              */
/*=========================================================================*/

WFC_API_CALL WFCint WFC_APIENTRY
wfcEnumerateDevices(WFCint *deviceIds,
                    WFCint deviceIdsCount,
                    const WFCint *filterList)
{
    WFCint                  n;

    /* populate list with device IDs */
    n = WFC_Devices_GetIds(deviceIds, deviceIdsCount, filterList);
    return n;
}

WFC_API_CALL WFCDevice WFC_APIENTRY
wfcCreateDevice(WFCint deviceId, const WFCint *attribList)
{
    WFCDevice               device;

    /* currently only empty attribute list or NULL is allowed */
    if (attribList && *attribList != WFC_NONE)
    {

        return WFC_INVALID_HANDLE;
    }
    WFC_Lock();
    device = WFC_Device_Create(deviceId);
    WFC_Unlock();

    return device;
}

WFC_API_CALL WFCErrorCode WFC_APIENTRY
wfcDestroyDevice(WFCDevice dev)
{
    WFC_DEVICE*             device;
    WFCErrorCode            result = WFC_ERROR_BAD_DEVICE;

    DPRINT(("wfcDestroyDevice(%d)", dev));

    WFC_Lock();
    device = WFC_Device_FindByHandle(dev);
    if (device)
    {
        WFC_Device_Destroy(device);
        result = WFC_ERROR_NONE;
    }
    WFC_Unlock();
    return result;
}

WFC_API_CALL WFCint WFC_APIENTRY
wfcGetDeviceAttribi(WFCDevice dev,
                    WFCDeviceAttrib attrib)
{
    WFC_DEVICE*             device;
    WFCint                  result = 0;
    WFCErrorCode            err;

    GET_DEVICE(device, dev, 0);
    err = WFC_Device_GetAttribi(device, attrib, &result);

    FAIL(err, result);
}

WFC_API_CALL WFCErrorCode WFC_APIENTRY
wfcGetError(WFCDevice dev)
{
    WFC_DEVICE* device;
    WFCErrorCode            err;

    GET_DEVICE(device, dev, WFC_ERROR_BAD_DEVICE);

    err = WFC_Device_GetError(device);
    WFC_Unlock();
    return err;
}

WFC_API_CALL void WFC_APIENTRY
wfcCommit(WFCDevice dev, WFCContext ctx, WFCboolean wait)
{
    WFC_DEVICE*             device;
    WFC_CONTEXT*            context;
    WFCErrorCode            error;

    DPRINT(("wfcCommit(%d,%d,%d)", dev, ctx, wait));
    GET_DEVICE_NR(device, dev);
    GET_CONTEXT_NR(context, device, ctx);

    error = WFC_Context_InvokeCommit(device, context, wait);

    FAIL_NR(error);
}

/*=========================================================================*/
/* 5. CONTEXT                                                              */
/*=========================================================================*/

WFC_API_CALL WFCContext WFC_APIENTRY
wfcCreateOnScreenContext(WFCDevice dev,
                         WFCint screenNumber,
                         const WFCint* attribList)
{
    WFC_CONTEXT*            context = NULL;
    WFC_DEVICE*             device = NULL;

    GET_DEVICE(device, dev, WFC_INVALID_HANDLE);

    /* currently only empty attribute list or NULL is allowed */
    COND_FAIL(!(attribList && *attribList != WFC_NONE),
              WFC_ERROR_BAD_ATTRIBUTE,
              WFC_INVALID_HANDLE);

    if (screenNumber == WFC_DEFAULT_SCREEN_NUMBER)
    {
        screenNumber = OWF_Screen_GetDefaultNumber();
    }
    
    /* check screen number validity */
    if (!OWF_Screen_Valid(screenNumber))
    {
        FAIL(WFC_ERROR_UNSUPPORTED, WFC_INVALID_HANDLE);
    }

    /* check that no other context currently uses this screen */
    if (WFC_Device_FindScreenNumber(screenNumber))
    {
        FAIL(WFC_ERROR_IN_USE, WFC_INVALID_HANDLE);
    }


    context = WFC_Device_CreateContext(device,
                                       WFC_INVALID_HANDLE,
                                       WFC_CONTEXT_TYPE_ON_SCREEN,
                                       screenNumber);
    if (!context)
    {
        FAIL(WFC_ERROR_OUT_OF_MEMORY, WFC_INVALID_HANDLE);
    }

    SUCCEED(context->handle);
}

WFC_API_CALL WFCContext WFC_APIENTRY
wfcCreateOffScreenContext(WFCDevice dev,
                          WFCNativeStreamType stream,
                          const WFCint *attribList)
{
    WFC_CONTEXT*            context = NULL;
    WFC_DEVICE*             device = NULL;
    OWFNativeStreamType 	internalStream=OWF_INVALID_HANDLE;
    OWF_STREAM_ERROR 		streamError;
    
    GET_DEVICE(device, dev, WFC_INVALID_HANDLE);

    /* currently only empty attribute list or NULL is allowed */
    COND_FAIL(!(attribList && *attribList != WFC_NONE),
              WFC_ERROR_BAD_ATTRIBUTE,
              WFC_INVALID_HANDLE);

    COND_FAIL(OWF_INVALID_HANDLE != (OWFNativeStreamType) stream,
              WFC_ERROR_ILLEGAL_ARGUMENT,
              WFC_INVALID_HANDLE);
    internalStream=owfNativeStreamFromWFC(stream,&streamError);
    COND_FAIL(OWF_STREAM_ERROR_INVALID_STREAM!=streamError,WFC_ERROR_UNSUPPORTED,WFC_INVALID_HANDLE);
    COND_FAIL(OWF_STREAM_ERROR_OUT_OF_MEMORY!=streamError,WFC_ERROR_OUT_OF_MEMORY,WFC_INVALID_HANDLE);
    
    if (!WFC_Context_IsValidTargetStream(internalStream))
    {
        owfNativeStreamDestroy(internalStream); /*cancel owfNativeStreamFromWFC reference increase*/
        FAIL(WFC_ERROR_UNSUPPORTED,WFC_INVALID_HANDLE);
    }
    context = WFC_Device_CreateContext(device, internalStream,
                                       WFC_CONTEXT_TYPE_OFF_SCREEN,
                                       OWF_RESERVED_BAD_SCREEN_NUMBER);
    owfNativeStreamDestroy(internalStream);	/*cancel owfNativeStreamFromWFC reference increase*/
    COND_FAIL(NULL != context, WFC_ERROR_OUT_OF_MEMORY, WFC_INVALID_HANDLE);

    SUCCEED(context->handle);
}

WFC_API_CALL void WFC_APIENTRY
wfcDestroyContext(WFCDevice dev,
                  WFCContext ctx)
{
    WFC_DEVICE*             device = NULL;
    WFCErrorCode            err;

    DPRINT(("wfcDestroyContext(%d, %d)", dev, ctx));
    GET_DEVICE_NR(device, dev);

    err = WFC_Device_DestroyContext(device, ctx);
    FAIL_NR(err);
}

WFC_API_CALL WFCint WFC_APIENTRY
wfcGetContextAttribi(WFCDevice dev,
                     WFCContext ctx,
                     WFCContextAttrib attrib)
{
    WFC_CONTEXT*            context = NULL;
    WFC_DEVICE*             device = NULL;
    WFCint                  value = 0;
    WFCErrorCode            err;

    GET_DEVICE(device, dev, 0);
    GET_CONTEXT(context, device, ctx, 0);

    err = WFC_Context_GetAttribi(context, attrib, &value);

    FAIL(err, value);
}

WFC_API_CALL void WFC_APIENTRY
wfcSetContextAttribi(WFCDevice dev,
                     WFCContext ctx,
                     WFCContextAttrib attrib,
                     WFCint value)
{
    WFC_DEVICE*             device;
    WFC_CONTEXT*            context;
    WFCErrorCode            err;

    GET_DEVICE_NR(device, dev);
    GET_CONTEXT_NR(context, device, ctx);

    err = WFC_Context_SetAttribi(context, attrib, value);
    FAIL_NR(err);
}

WFC_API_CALL void WFC_APIENTRY
wfcGetContextAttribfv(WFCDevice dev,
                      WFCContext ctx,
                      WFCContextAttrib attrib,
                      WFCint count,
                      WFCfloat *values) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFC_CONTEXT*            context;
    WFCErrorCode            err;

    GET_DEVICE_NR(device, dev);
    GET_CONTEXT_NR(context, device, ctx);

    err = WFC_Context_GetAttribfv(context, attrib, count, values);
    FAIL_NR(err);
}

WFC_API_CALL void WFC_APIENTRY
wfcSetContextAttribfv(WFCDevice dev,
                      WFCContext ctx,
                      WFCContextAttrib attrib,
                      WFCint count,
                      const WFCfloat *values) WFC_APIEXIT
{
      WFC_DEVICE*             device;
      WFC_CONTEXT*            context;
      WFCErrorCode            err;

      GET_DEVICE_NR(device, dev);
      GET_CONTEXT_NR(context, device, ctx);

      err = WFC_Context_SetAttribfv(context, attrib, count, values);
      FAIL_NR(err);
}

/*=========================================================================*/
/* 6. IMAGE PROVIDERS (SOURCES & MASKS)                                    */
/*=========================================================================*/

WFC_API_CALL WFCSource WFC_APIENTRY
wfcCreateSourceFromStream(WFCDevice dev,
                          WFCContext ctx,
                          WFCNativeStreamType stream,
                          const WFCint *attribList)
{
    WFC_DEVICE*             device;
    WFC_IMAGE_PROVIDER*     source;
    WFC_CONTEXT*            context;
    OWFNativeStreamType 	internalStream=OWF_INVALID_HANDLE;
    OWF_STREAM_ERROR 		streamError;

    GET_DEVICE(device, dev, WFC_INVALID_HANDLE);

    /* currently only empty attribute list or NULL is allowed */
    COND_FAIL(!(attribList && *attribList != WFC_NONE),
              WFC_ERROR_BAD_ATTRIBUTE,
              WFC_INVALID_HANDLE);

    GET_CONTEXT(context, device, ctx, WFC_INVALID_HANDLE);
    COND_FAIL(OWF_INVALID_HANDLE != (OWFNativeStreamType) stream,
              WFC_ERROR_ILLEGAL_ARGUMENT,
              WFC_INVALID_HANDLE);

    internalStream=owfNativeStreamFromWFC(stream,&streamError);
    COND_FAIL(OWF_STREAM_ERROR_INVALID_STREAM!=streamError,WFC_ERROR_UNSUPPORTED,WFC_INVALID_HANDLE);
    COND_FAIL(OWF_STREAM_ERROR_OUT_OF_MEMORY!=streamError,WFC_ERROR_OUT_OF_MEMORY,WFC_INVALID_HANDLE);

    if (context->stream == internalStream)
    	{
    	owfNativeStreamDestroy(internalStream);
        FAIL( WFC_ERROR_IN_USE, WFC_INVALID_HANDLE);
    	}
    
    if (!WFC_Context_IsValidSourceStream(internalStream))
        {
        owfNativeStreamDestroy(internalStream);
        FAIL( WFC_ERROR_UNSUPPORTED, WFC_INVALID_HANDLE);
        }

    source = WFC_Device_CreateSource(device, context, internalStream);
    owfNativeStreamDestroy(internalStream);	/*cancel FromWFC reference increase*/
    COND_FAIL(NULL != source, WFC_ERROR_OUT_OF_MEMORY, WFC_INVALID_HANDLE);

    SUCCEED(source->handle);
}

WFC_API_CALL void WFC_APIENTRY
wfcDestroySource(WFCDevice dev,
                 WFCSource src)
{
    WFC_DEVICE*             device;
    WFCErrorCode            err;

    GET_DEVICE_NR(device, dev);

    err = WFC_Device_DestroySource(device, src);

    FAIL_NR(err);
}

WFC_API_CALL WFCMask WFC_APIENTRY
wfcCreateMaskFromStream(WFCDevice dev,
                        WFCContext ctx,
                        WFCNativeStreamType stream,
                        const WFCint *attribList) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFC_CONTEXT*            context;
    WFC_IMAGE_PROVIDER*     mask;
    WFCboolean              inUse;
    OWFNativeStreamType 	internalStream=OWF_INVALID_HANDLE;
    OWF_STREAM_ERROR 		streamError;


    GET_DEVICE(device, dev, WFC_ERROR_BAD_DEVICE);

    /* currently only empty attribute list or NULL is allowed */
    COND_FAIL(!(attribList && *attribList != WFC_NONE),
              WFC_ERROR_BAD_ATTRIBUTE,
              WFC_INVALID_HANDLE);

    GET_CONTEXT(context, device, ctx, WFC_INVALID_HANDLE);
    COND_FAIL(OWF_INVALID_HANDLE != (OWFNativeStreamType) stream,
              WFC_ERROR_ILLEGAL_ARGUMENT,
              WFC_INVALID_HANDLE);

    internalStream=owfNativeStreamFromWFC(stream,&streamError);
    COND_FAIL(OWF_STREAM_ERROR_INVALID_STREAM!=streamError,WFC_ERROR_UNSUPPORTED,WFC_INVALID_HANDLE);
    COND_FAIL(OWF_STREAM_ERROR_OUT_OF_MEMORY!=streamError,WFC_ERROR_OUT_OF_MEMORY,WFC_INVALID_HANDLE);

    inUse = WFC_Device_StreamIsTarget(device, internalStream);
    if (inUse)
    	{
       	owfNativeStreamDestroy(internalStream);
        FAIL( WFC_ERROR_IN_USE, WFC_INVALID_HANDLE);
       	}

    mask = WFC_Device_CreateMask(device, context, internalStream);
    owfNativeStreamDestroy(internalStream);	/*cancel FromWFC reference increase*/
    COND_FAIL(NULL != mask, WFC_ERROR_OUT_OF_MEMORY, WFC_INVALID_HANDLE);

    SUCCEED(mask->handle);
}

WFC_API_CALL void WFC_APIENTRY
wfcDestroyMask(WFCDevice dev,
               WFCMask mask) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFCErrorCode            error;

    GET_DEVICE_NR(device, dev);

    error = WFC_Device_DestroyMask(device, mask);

    FAIL_NR(error);
}

/*=========================================================================*/
/* 7. COMPOSITION ELEMENTS                                                 */
/*=========================================================================*/

WFC_API_CALL WFCElement WFC_APIENTRY
wfcCreateElement(WFCDevice dev,
                 WFCContext ctx,
                 const WFCint* attribList) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFC_ELEMENT*            element;
    WFC_CONTEXT*            context;

    GET_DEVICE(device, dev, WFC_INVALID_HANDLE);

    /* currently only empty attribute list or NULL is allowed */
    COND_FAIL(!(attribList && *attribList != WFC_NONE),
              WFC_ERROR_BAD_ATTRIBUTE,
              WFC_INVALID_HANDLE);

    GET_CONTEXT(context, device, ctx, WFC_INVALID_HANDLE);

    element = WFC_Device_CreateElement(device, context);
    COND_FAIL(NULL != element, WFC_ERROR_OUT_OF_MEMORY, WFC_INVALID_HANDLE);

    SUCCEED(element->handle);
}

WFC_API_CALL void WFC_APIENTRY
wfcDestroyElement(WFCDevice dev,
                  WFCElement element) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFCErrorCode            error = WFC_ERROR_BAD_HANDLE;

    GET_DEVICE_NR(device, dev);

    error = WFC_Device_DestroyElement(device, element);

    FAIL_NR(error);
}

#define ATTR_FUNC_PROLOGUE  \
    WFCErrorCode            error; \
    WFC_DEVICE*             device; \
    \
    GET_DEVICE(device, dev, 0)

#define ATTR_FUNC_PROLOGUE_NR  \
    WFCErrorCode            error; \
    WFC_DEVICE*             device; \
    \
    GET_DEVICE_NR(device, dev)

#define ATTR_FUNC_EPILOGUE(x) \
    FAIL(error, x)

#define ATTR_FUNC_EPILOGUE_NR \
    FAIL_NR(error)

WFC_API_CALL WFCint WFC_APIENTRY
wfcGetElementAttribi(WFCDevice dev,
                     WFCElement element,
                     WFCElementAttrib attrib) WFC_APIEXIT
{
    WFCint                  value;

    ATTR_FUNC_PROLOGUE;

    error = WFC_Device_GetElementAttribiv(device, element, attrib, 1, &value);

    ATTR_FUNC_EPILOGUE(value);
}

WFC_API_CALL WFCfloat WFC_APIENTRY
wfcGetElementAttribf(WFCDevice dev,
                     WFCElement element,
                     WFCElementAttrib attrib) WFC_APIEXIT
{
    WFCfloat                value;

    ATTR_FUNC_PROLOGUE;

    COND_FAIL(WFC_ELEMENT_GLOBAL_ALPHA == attrib,
              WFC_ERROR_BAD_ATTRIBUTE,
              0.0f);

    error = WFC_Device_GetElementAttribfv(device, element, attrib, 1, &value);

    /* value is [0, OWF_ALPHA_MAX_VALUE], map to [0, 1] */
    value = value / OWF_ALPHA_MAX_VALUE;

    ATTR_FUNC_EPILOGUE(value);
}

WFC_API_CALL void WFC_APIENTRY
wfcGetElementAttribiv(WFCDevice dev,
                      WFCElement element,
                      WFCElementAttrib attrib,
                      WFCint count,
                      WFCint *values) WFC_APIEXIT
{
    ATTR_FUNC_PROLOGUE_NR;

    COND_FAIL_NR(WFC_ELEMENT_SOURCE_RECTANGLE == attrib ||
                 WFC_ELEMENT_DESTINATION_RECTANGLE == attrib,
                 WFC_ERROR_BAD_ATTRIBUTE);

    error = WFC_Device_GetElementAttribiv(device, element, attrib, count,
                                          values);
    ATTR_FUNC_EPILOGUE_NR;
}

WFC_API_CALL void WFC_APIENTRY
wfcGetElementAttribfv(WFCDevice dev,
                      WFCElement element,
                      WFCElementAttrib attrib,
                      WFCint count,
                      WFCfloat *values) WFC_APIEXIT
{
    ATTR_FUNC_PROLOGUE_NR;

    COND_FAIL_NR(WFC_ELEMENT_SOURCE_RECTANGLE == attrib ||
                 WFC_ELEMENT_DESTINATION_RECTANGLE == attrib,
                 WFC_ERROR_BAD_ATTRIBUTE);

    error = WFC_Device_GetElementAttribfv(device, element, attrib, count,
                                          values);

    ATTR_FUNC_EPILOGUE_NR;
}

WFC_API_CALL void WFC_APIENTRY
wfcSetElementAttribi(WFCDevice dev,
                     WFCElement element,
                     WFCElementAttrib attrib,
                     WFCint value) WFC_APIEXIT
{
    ATTR_FUNC_PROLOGUE_NR;

    error = WFC_Device_SetElementAttribiv(device, element, attrib, 1, &value);

    ATTR_FUNC_EPILOGUE_NR;
}

WFC_API_CALL void WFC_APIENTRY
wfcSetElementAttribf(WFCDevice dev,
                     WFCElement element,
                     WFCElementAttrib attrib,
                     WFCfloat value) WFC_APIEXIT
{
    ATTR_FUNC_PROLOGUE_NR;

    COND_FAIL_NR(WFC_ELEMENT_GLOBAL_ALPHA == attrib,
                 WFC_ERROR_BAD_ATTRIBUTE);

    error = WFC_Device_SetElementAttribfv(device, element, attrib, 1, &value);

    ATTR_FUNC_EPILOGUE_NR;
}

WFC_API_CALL void WFC_APIENTRY
wfcSetElementAttribiv(WFCDevice dev,
                      WFCElement element,
                      WFCElementAttrib attrib,
                      WFCint count,
                      const WFCint *values) WFC_APIEXIT
{
    ATTR_FUNC_PROLOGUE_NR;

    COND_FAIL_NR(WFC_ELEMENT_SOURCE_RECTANGLE == attrib ||
                 WFC_ELEMENT_DESTINATION_RECTANGLE == attrib,
                 WFC_ERROR_BAD_ATTRIBUTE);

    error = WFC_Device_SetElementAttribiv(device, element, attrib, count,
                                          values);

    ATTR_FUNC_EPILOGUE_NR;
}

WFC_API_CALL void WFC_APIENTRY
wfcSetElementAttribfv(WFCDevice dev,
                      WFCElement element,
                      WFCElementAttrib attrib,
                      WFCint count,
                      const WFCfloat *values) WFC_APIEXIT
{
    ATTR_FUNC_PROLOGUE_NR;

    COND_FAIL_NR(WFC_ELEMENT_SOURCE_RECTANGLE == attrib ||
                 WFC_ELEMENT_DESTINATION_RECTANGLE == attrib,
                 WFC_ERROR_BAD_ATTRIBUTE);

    error = WFC_Device_SetElementAttribfv(device, element, attrib, count,
                                          values);

    ATTR_FUNC_EPILOGUE_NR;
}

WFC_API_CALL void WFC_APIENTRY
wfcInsertElement(WFCDevice dev,
                 WFCElement element,
                 WFCElement subordinate) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFC_ELEMENT*            elemento;
    WFCErrorCode            error;

    GET_DEVICE_NR(device, dev);

    /*
    - element is inserted immediately above subordinate
    - if subordinate is NULL, then element will go to bottom
    - both elements must be in the same scene (context)
    */
    elemento = WFC_Device_FindElement(device, element);
    COND_FAIL_NR(WFC_INVALID_HANDLE != elemento, WFC_ERROR_BAD_HANDLE);

    error = WFC_Context_InsertElement(CONTEXT(elemento->context),
                                      element,
                                      subordinate);

    FAIL_NR(error);
}

WFC_API_CALL void WFC_APIENTRY
wfcRemoveElement(WFCDevice dev,
                 WFCElement element) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFC_ELEMENT*            elemento;
    WFCErrorCode            error;

    GET_DEVICE_NR(device, dev);

    elemento = WFC_Device_FindElement(device, element);
    COND_FAIL_NR(WFC_INVALID_HANDLE != elemento, WFC_ERROR_BAD_HANDLE);

    error = WFC_Context_RemoveElement(CONTEXT(elemento->context), element);

    FAIL_NR(error);
}

WFC_API_CALL WFCElement WFC_APIENTRY
wfcGetElementAbove(WFCDevice dev, WFCElement element) WFC_APIEXIT
{
    WFC_DEVICE*             device = NULL;
    WFC_ELEMENT*            elemento = NULL;
    WFCElement              result = WFC_INVALID_HANDLE;
    WFCErrorCode            error = WFC_ERROR_NONE;

    GET_DEVICE(device, dev, WFC_INVALID_HANDLE);

    elemento = WFC_Device_FindElement(device, element);
    COND_FAIL(WFC_INVALID_HANDLE != elemento,
              WFC_ERROR_BAD_HANDLE,
              WFC_INVALID_HANDLE);

    error = WFC_Context_GetElementAbove(CONTEXT(elemento->context),
                                        element,
                                        &result);

    FAIL(error, result);
}

WFC_API_CALL WFCElement WFC_APIENTRY
wfcGetElementBelow(WFCDevice dev, WFCElement element) WFC_APIEXIT
{
    WFC_DEVICE*             device = NULL;
    WFC_ELEMENT*            elemento = NULL;
    WFCElement              result = WFC_INVALID_HANDLE;
    WFCErrorCode            error = WFC_ERROR_NONE;

    GET_DEVICE(device, dev, WFC_INVALID_HANDLE);

    elemento = WFC_Device_FindElement(device, element);
    COND_FAIL(WFC_INVALID_HANDLE != elemento,
              WFC_ERROR_BAD_HANDLE,
              WFC_INVALID_HANDLE);

    error = WFC_Context_GetElementBelow(CONTEXT(elemento->context),
                                        element,
                                        &result);

    FAIL(error, result);
}

/*=========================================================================*/
/* 8. RENDERING                                                            */
/*=========================================================================*/

WFC_API_CALL void WFC_APIENTRY
wfcActivate(WFCDevice dev,
            WFCContext ctx) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFC_CONTEXT*            context;

    GET_DEVICE_NR(device, dev);
    GET_CONTEXT_NR(context, device, ctx);

    WFC_Context_Activate(context, WFC_TRUE);

    SUCCEED_NR();
}

WFC_API_CALL void WFC_APIENTRY
wfcDeactivate(WFCDevice dev,
              WFCContext ctx) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFC_CONTEXT*            context;

    GET_DEVICE_NR(device, dev);
    GET_CONTEXT_NR(context, device, ctx);

    WFC_Context_Activate(context, WFC_FALSE);

    SUCCEED_NR();
}

WFC_API_CALL void WFC_APIENTRY
wfcCompose(WFCDevice dev,
           WFCContext ctx,
           WFCboolean wait) WFC_APIEXIT
{
    WFC_DEVICE*             device;
    WFC_CONTEXT*            context;
    WFCboolean              able;

    GET_DEVICE_NR(device, dev);
    GET_CONTEXT_NR(context, device, ctx);

    /* context must not be active */
    COND_FAIL_NR(!WFC_Context_Active(context), WFC_ERROR_UNSUPPORTED);

    /* send composition request */
    able = WFC_Context_InvokeComposition(device, context, wait);
    COND_FAIL_NR(WFC_TRUE == able, WFC_ERROR_BUSY);

    SUCCEED_NR();
}


/*=========================================================================*/
/* 9. SYNCHRONIZATION                                                      */
/*=========================================================================*/

WFC_API_CALL void WFC_APIENTRY
wfcFence(WFCDevice dev,
         WFCContext ctx,
         WFCEGLDisplay dpy,
         WFCEGLSync sync)
{
    WFC_DEVICE*     device;
    WFC_CONTEXT*    context;

    DPRINT(("wfcFence(%08x,%08x,%08x)", dev, ctx, sync));

    GET_DEVICE_NR(device, dev);
    DPRINT(("  device = %p", device));

    GET_CONTEXT_NR(context, device, ctx);
    DPRINT(("  context = %p", context));
    
    /* unsignal sync object first */
    OWF_ASSERT(context->eglPrivateSignalSync);
    if ( !(context->eglPrivateSignalSync((EGLDisplay)dpy, sync, EGL_UNSIGNALED_KHR)) )
    {
        FAIL_NR(WFC_ERROR_ILLEGAL_ARGUMENT);
    }

    /* insert fence 'token' to command stream */
    WFC_Context_InsertFence(context, dpy, sync);
    SUCCEED_NR();
}

/*=========================================================================*/
/* 10. EXTENSION SUPPORT                                                   */
/*=========================================================================*/

WFC_API_CALL WFCint WFC_APIENTRY
wfcGetStrings(WFCDevice dev,
              WFCStringID name,
              const char **strings,
              WFCint stringsCount) WFC_APIEXIT
{
    WFC_DEVICE*             pDevice;
    const char**            tmp;
    WFCint                  retVal;

    GET_DEVICE(pDevice, dev, 0);
    COND_FAIL(stringsCount >= 0, WFC_ERROR_ILLEGAL_ARGUMENT, 0);

    switch (name)
    {
    case WFC_VENDOR:
        retVal = 1;
        tmp = &wfc_strings[WFC_VENDOR_INDEX];
        break;

    case WFC_RENDERER:
        retVal = 1;
        tmp = &wfc_strings[WFC_RENDERER_INDEX];
        break;

    case WFC_VERSION:
        retVal = 1;
        tmp = &wfc_strings[WFC_VERSION_INDEX];
        break;

    case WFC_EXTENSIONS:
        tmp = &wfc_extensions[0];
		/* we know that wfc_extensions always has NULL as the last member 
		   so this loop will never exceed the size of tmp */
		/* coverity[overrun-local] */
        for (retVal=0; tmp[retVal] != NULL; retVal++)
        {
            /* get extensions array size */
        }
        break;

    default:
        FAIL(WFC_ERROR_ILLEGAL_ARGUMENT, 0);
    }

    if (strings != NULL)
    {
        WFCint i;

        if (stringsCount < retVal)
        {
            retVal = stringsCount;
        }
        for (i=0; i<retVal; i++)
        {
            strings[i] = tmp[i];
        }
    }

    SUCCEED(retVal);
}

WFC_API_CALL WFCboolean WFC_APIENTRY
wfcIsExtensionSupported(WFCDevice dev,
                        const char *string) WFC_APIEXIT
{
    WFC_DEVICE* pDevice;
    WFCint i;
    WFCboolean retVal = WFC_FALSE;

    GET_DEVICE(pDevice, dev, retVal);
    /* Bad param does not update device error state */
    COND_FAIL(string,WFC_ERROR_NONE,retVal);
    
    for (i=0; wfc_extensions[i] != NULL; i++)
    {
       if (strcmp(string, wfc_extensions[i])==0)
       {
           retVal = WFC_TRUE;
           break;
       }
    }

    SUCCEED(retVal);
}

/*=========================================================================*/
/* 11. TEST ONLY API FOR ON SCREEN IMAGE EXPORTING                         */
/*=========================================================================*/

WFC_API_CALL WFCNativeStreamType WFC_APIENTRY
extwfcGetOnScreenStream(WFCDevice dev, WFCContext ctx) WFC_APIEXIT
{
	WFC_DEVICE*             device;
	WFC_CONTEXT*            context;

	GET_DEVICE(device, dev, WFC_INVALID_HANDLE);
	DPRINT(("  device = %p", device));

	GET_CONTEXT(context, device, ctx, WFC_INVALID_HANDLE);
	DPRINT(("  context = %p", context));

	/* Protect context's target stream from being destroyed by the user
	 * WFC_CONTEXT_Dtor will reset this flag. */
	owfNativeStreamSetProtectionFlag(context->stream, OWF_TRUE);
	
	OWF_DisplayContext_FlagInternalStreamAccessed(context->displayContext);

	SUCCEED(context->stream);
}
