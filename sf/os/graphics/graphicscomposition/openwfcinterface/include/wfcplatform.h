/********************************************************************
 *                                                                   *
 * Sample implementation of wfcplatform.h, version 1.0, draft 18     *
 *                                                                   *
 * Copyright (c) 2007-2009 The Khronos Group                         *
 *                                                                   *
 *********************************************************************/

#ifndef _WFCPLATFORM_H_
#define _WFCPLATFORM_H_

#include <KHR/khrplatform.h>
#include <EGL/egl.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WFC_API_CALL
#define WFC_API_CALL KHRONOS_APICALL
#endif
#ifndef WFC_APIENTRY
#define WFC_APIENTRY KHRONOS_APIENTRY
#endif
#ifndef WFC_APIEXIT
#define WFC_APIEXIT KHRONOS_APIATTRIBUTES
#endif

#ifndef WFC_DEFAULT_SCREEN_NUMBER
#define WFC_DEFAULT_SCREEN_NUMBER (0)
#endif

/* for backward compatibility of wfcGetOnScreenStream symbol use */
#define wfcGetOnScreenStream extwfcGetOnScreenStream

typedef khronos_int32_t     WFCint;
typedef khronos_float_t     WFCfloat;
typedef khronos_uint32_t    WFCbitfield;
typedef khronos_uint32_t    WFCHandle;

typedef EGLDisplay          WFCEGLDisplay;
typedef void*               WFCEGLSync;   /* An opaque handle to an EGLSyncKHR */
typedef WFCHandle           WFCNativeStreamType;

#ifdef __cplusplus
}
#endif

#endif /* _WFCPLATFORM_H_ */
