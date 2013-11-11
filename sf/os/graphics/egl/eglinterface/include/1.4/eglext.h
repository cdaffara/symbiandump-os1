
#ifndef __eglext_h_
#define __eglext_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Copyright (c) 2007-2010 The Khronos Group Inc.
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

/**
@publishedAll
@released
*/

/* differences from the actual sample implementation provided by Khronos:
- this comment
- the Doxygen comment with tag 'publishedAll', and tag 'released'
- changing platform-dependant header name from <EGL/eglplatform.h> to <EGL/egltypes.h>
- EGLImageKHR definition is consistent with other EGL objects, i.e. is defined as integer 
  instead of pointer to void.
- macro EGL_IMAGE_PRESERVED_SYMBIAN introduced in this header represents EGL_IMAGE_PRESERVED_KHR
  macro defined in Khronos spec. Note that it has been retained temporarily for compatibility reasons 
  and it is advised to use Khronos macro instead.
- resource profiler and reusable sync object extensions are added
*/

#include <EGL/egltypes.h>
#include <khronos_types.h>

/*************************************************************/

#define EGL_EGLEXT_VERSION 3

#ifndef EGL_KHR_config_attribs
#define EGL_KHR_config_attribs 1
#define EGL_CONFORMANT_KHR			0x3042	/* EGLConfig attribute */
#define EGL_VG_COLORSPACE_LINEAR_BIT_KHR	0x0020	/* EGL_SURFACE_TYPE bitfield */
#define EGL_VG_ALPHA_FORMAT_PRE_BIT_KHR		0x0040	/* EGL_SURFACE_TYPE bitfield */
#endif

#ifndef EGL_KHR_lock_surface
#define EGL_KHR_lock_surface 1
#define EGL_READ_SURFACE_BIT_KHR		0x0001	/* EGL_LOCK_USAGE_HINT_KHR bitfield */
#define EGL_WRITE_SURFACE_BIT_KHR		0x0002	/* EGL_LOCK_USAGE_HINT_KHR bitfield */
#define EGL_LOCK_SURFACE_BIT_KHR		0x0080	/* EGL_SURFACE_TYPE bitfield */
#define EGL_OPTIMAL_FORMAT_BIT_KHR		0x0100	/* EGL_SURFACE_TYPE bitfield */
#define EGL_MATCH_FORMAT_KHR			0x3043	/* EGLConfig attribute */
#define EGL_FORMAT_RGB_565_EXACT_KHR		0x30C0	/* EGL_MATCH_FORMAT_KHR value */
#define EGL_FORMAT_RGB_565_KHR			0x30C1	/* EGL_MATCH_FORMAT_KHR value */
#define EGL_FORMAT_RGBA_8888_EXACT_KHR		0x30C2	/* EGL_MATCH_FORMAT_KHR value */
#define EGL_FORMAT_RGBA_8888_KHR		0x30C3	/* EGL_MATCH_FORMAT_KHR value */
#define EGL_MAP_PRESERVE_PIXELS_KHR		0x30C4	/* eglLockSurfaceKHR attribute */
#define EGL_LOCK_USAGE_HINT_KHR			0x30C5	/* eglLockSurfaceKHR attribute */
#define EGL_BITMAP_POINTER_KHR			0x30C6	/* eglQuerySurface attribute */
#define EGL_BITMAP_PITCH_KHR			0x30C7	/* eglQuerySurface attribute */
#define EGL_BITMAP_ORIGIN_KHR			0x30C8	/* eglQuerySurface attribute */
#define EGL_BITMAP_PIXEL_RED_OFFSET_KHR		0x30C9	/* eglQuerySurface attribute */
#define EGL_BITMAP_PIXEL_GREEN_OFFSET_KHR	0x30CA	/* eglQuerySurface attribute */
#define EGL_BITMAP_PIXEL_BLUE_OFFSET_KHR	0x30CB	/* eglQuerySurface attribute */
#define EGL_BITMAP_PIXEL_ALPHA_OFFSET_KHR	0x30CC	/* eglQuerySurface attribute */
#define EGL_BITMAP_PIXEL_LUMINANCE_OFFSET_KHR	0x30CD	/* eglQuerySurface attribute */
#define EGL_LOWER_LEFT_KHR			0x30CE	/* EGL_BITMAP_ORIGIN_KHR value */
#define EGL_UPPER_LEFT_KHR			0x30CF	/* EGL_BITMAP_ORIGIN_KHR value */
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLBoolean EGLAPIENTRY eglLockSurfaceKHR (EGLDisplay display, EGLSurface surface, const EGLint *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglUnlockSurfaceKHR (EGLDisplay display, EGLSurface surface);
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLBoolean (EGLAPIENTRYP PFNEGLLOCKSURFACEKHRPROC) (EGLDisplay display, EGLSurface surface, const EGLint *attrib_list);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLUNLOCKSURFACEKHRPROC) (EGLDisplay display, EGLSurface surface);
#endif

#ifndef EGL_KHR_image
#define EGL_KHR_image 1
#define EGL_NATIVE_PIXMAP_KHR			0x30B0	/* eglCreateImageKHR target */
typedef int EGLImageKHR;
#define EGL_NO_IMAGE_KHR			((EGLImageKHR)0)
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLImageKHR EGLAPIENTRY eglCreateImageKHR (EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglDestroyImageKHR (EGLDisplay dpy, EGLImageKHR image);
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLImageKHR (EGLAPIENTRYP PFNEGLCREATEIMAGEKHRPROC) (EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLDESTROYIMAGEKHRPROC) (EGLDisplay dpy, EGLImageKHR image);
#endif

#ifndef EGL_KHR_vg_parent_image
#define EGL_KHR_vg_parent_image 1
#define EGL_VG_PARENT_IMAGE_KHR			0x30BA	/* eglCreateImageKHR target */
#endif

#ifndef EGL_KHR_gl_texture_2D_image
#define EGL_KHR_gl_texture_2D_image 1
#define EGL_GL_TEXTURE_2D_KHR			0x30B1	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_LEVEL_KHR		0x30BC	/* eglCreateImageKHR attribute */
#endif

#ifndef EGL_KHR_gl_texture_cubemap_image
#define EGL_KHR_gl_texture_cubemap_image 1
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR	0x30B3	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR	0x30B4	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR	0x30B5	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR	0x30B6	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR	0x30B7	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR	0x30B8	/* eglCreateImageKHR target */
#endif

#ifndef EGL_KHR_gl_texture_3D_image
#define EGL_KHR_gl_texture_3D_image 1
#define EGL_GL_TEXTURE_3D_KHR			0x30B2	/* eglCreateImageKHR target */
#define EGL_GL_TEXTURE_ZOFFSET_KHR		0x30BD	/* eglCreateImageKHR attribute */
#endif

#ifndef EGL_KHR_gl_renderbuffer_image
#define EGL_KHR_gl_renderbuffer_image 1
#define EGL_GL_RENDERBUFFER_KHR			0x30B9	/* eglCreateImageKHR target */
#endif

#ifndef EGL_KHR_image_base
#define EGL_KHR_image_base 1
/* Most interfaces defined by EGL_KHR_image_pixmap above */
#define EGL_IMAGE_PRESERVED_KHR			0x30D2	/* eglCreateImageKHR attribute */
#endif

#ifndef EGL_KHR_image_pixmap
#define EGL_KHR_image_pixmap 1
/* Interfaces defined by EGL_KHR_image above */
#endif

#ifndef EGL_SYMBIAN_image_preserved
#define EGL_SYMBIAN_image_preserved 1
/* The following macro was retained for compatibility reasons and will be removed soon. 
   For the new written code it is advised to use EGL_IMAGE_PRESERVED_KHR macro 
   defined in Khronos spec. */
#define EGL_IMAGE_PRESERVED_SYMBIAN		0x30D2	/* eglCreateImageKHR attribute */
#endif

#ifndef EGL_NOK_resource_profiling2
#define EGL_NOK_resource_profiling2 1
/* Accepted by the <query_bits> parameter of eglQueryProfilingDataNOK */
#define EGL_PROF_QUERY_GLOBAL_BIT_NOK               0x0001
#define EGL_PROF_QUERY_MEMORY_USAGE_BIT_NOK         0x0002
/* Returned as attribute identifiers by eglQueryProfilingDataNOK */
#define EGL_PROF_TOTAL_MEMORY_NOK                   0x3070
#define EGL_PROF_USED_MEMORY_NOK                    0x3071
#define EGL_PROF_PROCESS_ID_NOK                      0x3072
#define EGL_PROF_PROCESS_USED_PRIVATE_MEMORY_NOK     0x3073
#define EGL_PROF_PROCESS_USED_SHARED_MEMORY_NOK      0x3074
/* Accepted as the <type> parameter of eglCreateSyncKHR */
#define EGL_SYNC_RESOURCE_PROFILING_NOK             0x3078
/* Accepted as the value of the EGL_SYNC_CONDITION_KHR attribute passed in */
/* <attrib_list> for eglCreateSyncKHR, and returned in <value> when */
/* eglGetSyncAttribKHR is called with <attribute> EGL_SYNC_CONDITION_KHR */
#define EGL_PROF_TOTAL_MEMORY_USAGE_GT_NOK          0x3075
#define EGL_PROF_TOTAL_MEMORY_USAGE_LT_NOK          0x3076
/* Accepted as an attribute name in the <attrib_list> parameter of */
/* eglCreateSyncKHR and in <attribute> for eglGetSyncAttribKHR */
#define EGL_PROF_MEMORY_USAGE_THRESHOLD_NOK         0x3077
typedef TUint64 EGLNativeProcessIdTypeNOK;
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLBoolean EGLAPIENTRY eglQueryProfilingDataNOK(EGLDisplay dpy, EGLint query_bits, EGLint *data, EGLint data_size, EGLint *data_count);
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLBoolean (EGLAPIENTRYP PFNEGLQUERYPROFILINGDATANOKPROC) (EGLDisplay dpy, EGLint query_bits, EGLint *data, EGLint data_size, EGLint *data_count);
#endif

#ifndef EGL_KHR_reusable_sync
#define EGL_KHR_reusable_sync 1
/* Accepted by the <type> parameter of eglCreateSyncKHR, and returned
in <value> when eglGetSyncAttribKHR is called with <attribute>
EGL_SYNC_TYPE_KHR */
#define EGL_SYNC_REUSABLE_KHR                   0x30FA
/* Accepted by the <attribute> parameter of eglGetSyncAttribKHR */
#define EGL_SYNC_TYPE_KHR                       0x30F7
#define EGL_SYNC_STATUS_KHR                     0x30F1
/* Accepted by the <mode> parameter of eglSignalSyncKHR and returned in
<value> when eglGetSyncAttribKHR is called with <attribute>
EGL_SYNC_STATUS_KHR */
#define EGL_SIGNALED_KHR                        0x30F2
#define EGL_UNSIGNALED_KHR                      0x30F3
/* Accepted in the <flags> parameter of eglClientWaitSyncKHR */
#define EGL_SYNC_FLUSH_COMMANDS_BIT_KHR         0x0001
/* Accepted in the <timeout> parameter of eglClientWaitSyncKHR */
#define EGL_FOREVER_KHR                         0xFFFFFFFFFFFFFFFFull
/* Returned by eglClientWaitSyncKHR */
#define EGL_TIMEOUT_EXPIRED_KHR                 0x30F5
#define EGL_CONDITION_SATISFIED_KHR             0x30F6
/* EGLSyncKHR is an opaque handle to an EGL sync object */
typedef void* EGLSyncKHR;
/* EGLTimeKHR is a 64-bit unsigned integer representing intervals in nanoseconds. */
typedef khronos_utime_nanoseconds_t EGLTimeKHR;
/* Returned by eglCreateSyncKHR in the event of an error */
#define EGL_NO_SYNC_KHR                         ((EGLSyncKHR)0)
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLSyncKHR EGLAPIENTRY eglCreateSyncKHR(EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR sync);
EGLAPI EGLint EGLAPIENTRY eglClientWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);
EGLAPI EGLBoolean EGLAPIENTRY eglSignalSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode);
EGLAPI EGLBoolean EGLAPIENTRY eglGetSyncAttribKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLint attribute, EGLint *value);
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLSyncKHR (EGLAPIENTRYP PFNEGLCREATESYNCKHRPROC) (EGLDisplay dpy, EGLenum type, const EGLint *attrib_list);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLDESTROYSYNCKHRPROC) (EGLDisplay dpy, EGLSyncKHR sync);
typedef EGLint (EGLAPIENTRYP PFNEGLCLIENTWAITSYNCKHRPROC) (EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLSIGNALSYNCKHRPROC) (EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLGETSYNCATTRIBKHRPROC) (EGLDisplay dpy, EGLSyncKHR sync, EGLint attribute, EGLint *value);
#endif

#ifndef EGL_NOK_swap_region
#define EGL_NOK_swap_region 1
#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffersRegionNOK(EGLDisplay dpy, EGLSurface surface, EGLint numRects, const EGLint *rects);
#endif /* EGL_EGLEXT_PROTOTYPES */
typedef EGLBoolean (EGLAPIENTRYP PFNEGLSWAPBUFFERSREGIONNOKPROC) (EGLDisplay dpy, EGLSurface surface, EGLint numRects, const EGLint *rects);
#endif

#ifndef EGL_NOK_ENDPOINT
#define EGL_NOK_ENDPOINT 1
/* Endpoint datatype */
typedef void* EGLEndpointNOK;
typedef void* EGLTRequestStatusNOK;
typedef void* EGLEndpointSourceNOK;
/* Endpoint constant */
#define EGL_ENDPOINT_TYPE_CONSUMER_NOK   0x30D5
#define EGL_TSURFACEID_NOK               0x30D3
/* Endpoint invalid parameter */
#define EGL_NO_ENDPOINT_NOK              ((EGLEndpointNOK)0)
/* Endpoint error code */
#define EGL_BAD_ENDPOINT_NOK             0x30D4
/* Endpoint attribute keys */
#define EGL_ENDPOINT_READY_NOK           0x30D6
#define EGL_DELAY_NOK                    0x30D7

#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLEndpointNOK EGLAPIENTRY eglCreateEndpointNOK(EGLDisplay dpy, EGLenum type, EGLenum source_type, EGLEndpointSourceNOK source, const EGLint *attrib_list);
EGLAPI EGLBoolean EGLAPIENTRY eglDestroyEndpointNOK(EGLDisplay dpy, EGLEndpointNOK endpoint);
EGLAPI EGLint EGLAPIENTRY eglGetEndpointAttribNOK(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib);
EGLAPI EGLBoolean EGLAPIENTRY eglSetEndpointAttribNOK(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib, EGLint value);
EGLAPI EGLBoolean EGLAPIENTRY eglEndpointBeginStreamingNOK(EGLDisplay dpy, EGLEndpointNOK endpoint);
EGLAPI EGLBoolean EGLAPIENTRY eglEndpointEndStreamingNOK(EGLDisplay dpy, EGLEndpointNOK endpoint);
EGLAPI EGLImageKHR EGLAPIENTRY eglAcquireImageNOK(EGLDisplay dpy, EGLEndpointNOK endpoint);
EGLAPI EGLBoolean EGLAPIENTRY eglReleaseImageNOK(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLImageKHR image, EGLenum api);
EGLAPI EGLint EGLAPIENTRY eglGetEndpointDirtyAreaNOK(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint* rects, EGLint start_rect, EGLint max_rects, EGLBoolean collapse); 
EGLAPI EGLBoolean EGLAPIENTRY eglEndpointRequestNotificationNOK(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLTRequestStatusNOK sync);
EGLAPI EGLBoolean EGLAPIENTRY eglEndpointCancelNotificationNOK(EGLDisplay dpy, EGLEndpointNOK endpoint);
#endif

typedef EGLEndpointNOK (EGLAPIENTRYP PFNEGLCREATEENDPOINTNOKPROC) (EGLDisplay dpy, EGLenum type, EGLenum source_type, EGLEndpointSourceNOK source, const EGLint* attrib_list);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLDESTROYENDPOINTNOKPROC) (EGLDisplay dpy, EGLEndpointNOK endpoint);
typedef EGLint (EGLAPIENTRYP PFNEGLGETENDPOINTATTRIBNOKPROC) (EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLSETENDPOINTATTRIBNOKPROC) (EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib, EGLint value);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLENDPOINTBEGINSTREAMINGNOKPROC) (EGLDisplay dpy, EGLEndpointNOK endpoint);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLENDPOINTENDSTREAMINGNOKPROC) (EGLDisplay dpy, EGLEndpointNOK endpoint);
typedef EGLImageKHR  (EGLAPIENTRYP PFNEGLACQUIREIMAGENOKPROC)(EGLDisplay dpy, EGLEndpointNOK endpoint);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLRELEASEIMAGENOKPROC) (EGLDisplay dpy, EGLEndpointNOK endpoint, EGLImageKHR image, EGLenum api);
typedef EGLint (EGLAPIENTRYP PFNEGLGETENDPOINTDIRTYAREANOKPROC) (EGLDisplay dpy,  EGLEndpointNOK endpoint, EGLint* rects, EGLint start_rect, EGLint max_rects,  EGLBoolean collapse); 
typedef EGLBoolean (EGLAPIENTRYP PFNEGLENDPOINTREQUESTNOTIFICATIONNOKPROC) (EGLDisplay dpy, EGLEndpointNOK endpoint, EGLTRequestStatusNOK sync);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLENDPOINTCANCELNOTIFICATIONNOKPROC) (EGLDisplay dpy, EGLEndpointNOK endpoint);
#endif

#ifndef EGL_SYMBIAN_COMPOSITION
#define EGL_SYMBIAN_COMPOSITION 1
/* Extension defines a behavioural change which does not introduce functions or tokens. */
/* The presence of this extension means that semi-transparent UI content can be displayed above EGL window surfaces. */
#endif  /* EGL_SYMBIAN_COMPOSITION */

#ifndef EGL_NOK_SURFACE_SCALING
#define EGL_NOK_SURFACE_SCALING 1
#define EGL_SURFACE_SCALING_NOK           0x30DD  /* Surface Scaling attribute for EGLConfig */
#define EGL_TARGET_EXTENT_OFFSET_X_NOK    0x3079  /* attribute for eglCreateWindowSurface and eglQuerySurface */
#define EGL_TARGET_EXTENT_OFFSET_Y_NOK    0x307A  /* attribute for eglCreateWindowSurface and eglQuerySurface */
#define EGL_TARGET_EXTENT_WIDTH_NOK       0x307B  /* attribute for eglCreateWindowSurface and eglQuerySurface */
#define EGL_TARGET_EXTENT_HEIGHT_NOK      0x307C  /* attribute for eglCreateWindowSurface and eglQuerySurface */
#define EGL_BORDER_COLOR_RED_NOK          0x307D  /* attribute for eglCreateWindowSurface and eglQuerySurface */
#define EGL_BORDER_COLOR_GREEN_NOK        0x307E  /* attribute for eglCreateWindowSurface and eglQuerySurface */
#define EGL_BORDER_COLOR_BLUE_NOK         0x30D8  /* attribute for eglCreateWindowSurface and eglQuerySurface */
#define EGL_FIXED_WIDTH_NOK               0x30DB  /* attribute for eglCreateWindowSurface */
#define EGL_FIXED_HEIGHT_NOK              0x30DC  /* attribute for eglCreateWindowSurface */

/* Tokens returned by eglQuerySurfaceScalingCapabilityNOK */
#define EGL_NOT_SUPPORTED_NOK   0
#define EGL_SUPPORTED_NOK       1
#define EGL_SLOW_NOK            3

#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurfaceScalingCapabilityNOK(EGLDisplay dpy, EGLConfig config, EGLint surface_width, EGLint surface_height, EGLint target_width, EGLint target_height, EGLint *value);
EGLAPI EGLBoolean EGLAPIENTRY eglSetSurfaceScalingNOK(EGLDisplay dpy, EGLSurface surface, EGLint target_offset_x, EGLint target_offset_y, EGLint target_width, EGLint target_height);
#endif
typedef EGLBoolean (EGLAPIENTRYP PFNEGLQUERYSURFACESCALINGCAPABILITYNOKPROC)(EGLDisplay dpy, EGLConfig config, EGLint surface_width, EGLint surface_height, EGLint target_width, EGLint target_height, EGLint *value);
typedef EGLBoolean (EGLAPIENTRYP PFNEGLSETSURFACESCALINGNOKPROC)(EGLDisplay dpy, EGLSurface surface, EGLint target_offset_x, EGLint target_offset_y, EGLint target_width, EGLint target_height);
#endif /* EGL_NOK_SURFACE_SCALING */

#ifdef __cplusplus
}
#endif

#endif
