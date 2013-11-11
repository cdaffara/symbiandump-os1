/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Internal binding API for OpenVG, used for managing OpenVG rendering contexts.
*
*/
#ifndef _OPENVG_CONTEXT_H
#define _OPENVG_CONTEXT_H

#include <VG/openvg.h>

/*
Implementation note: all VGI_ERROR_xxx should be negative and well below standard Symbian error codes!
*/
enum {
	VGI_OK                              = 0,						
	VGI_ERROR_OUT_OF_MEMORY             = (-10000),
	VGI_ERROR_INVALID_ARGUMENTS         = (-10001),
	VGI_ERROR_ALREADY_EXISTS            = (-10002),
	VGI_ERROR_COLORSPACE_NOT_SUPPORTED  = (-10003),
    VGI_ERROR_NOT_SUPPORTED             = (-10004),
    VGI_ERROR_ILLEGAL_IMAGE_HANDLE      = (-10005),
    VGI_ERROR_IMAGE_IN_USE              = (-10006),
    VGI_ERROR_ILLEGAL_OPERATION         = (-10007),
};

typedef enum {
	VGI_COLORSPACE_LINEAR,
	VGI_COLORSPACE_SRGB
} VGIColorSpace;

typedef enum {
	VGI_COLOR_BUFFER_FORMAT_RGB565,
	VGI_COLOR_BUFFER_FORMAT_RGB888,
	VGI_COLOR_BUFFER_FORMAT_XRGB8888,
	VGI_COLOR_BUFFER_FORMAT_ARGB8888,
	VGI_COLOR_BUFFER_FORMAT_ARGB8888_PRE,
	VGI_COLOR_BUFFER_FORMAT_XRGB4444,
} VGIColorBufferFormat;

typedef enum {
   VGI_SKIP_TRANSPARENT_PIXELS,
   VGI_COPY_TRANSPARENT_PIXELS,
} VGICopyToTargetHint;

#ifdef GRAPHICS_LIBVGI_DLL
	#define VGI_API_CALL		__declspec(dllexport)
#else
	#define VGI_API_CALL		__declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * OpenVG is initialized with the selected size and colorspace. A single rendering context is created and
 * made active (there is no support for multiple contexts). Single rendering (back buffer) surface is created
 * by OpenVG and made active for Read and Write.
 * @param width of the rendering surface (back buffer) size in pixels.
 * @param height of the rendering surface (back buffer) size in pixels.
 * @param colorSpace space to be used. Must be either VGI_COLORSPACE_LINEAR or VGI_COLORSPACE_SRGB.
 * @return Returns VGI_OK if initialization was successful. Returns error code VGI_ERROR_INVALID_ARGUMENTS
 *         if width or height are negative. Returns error code VGI_ERROR_OUT_OF_MEMORY if there is insufficient
 *         free memory to complete the initialization (smaller size for back buffer or smaller antialiasing setting may
 *         work). Returns error code VGI_ERROR_ALREADY_EXISTS if the surface and context already exists in this
 *         process & thread (call VGITerminate to reset). Returns error code VGI_ERROR_COLORSPACE_NOT_SUPPORTED
 *         if the given colorspace is not supported by the implementation.
 */
VGI_API_CALL int VGIInitialize( int width, int height, VGIColorSpace colorSpace );

/**
 * OpenVG is initialized with the selected size and colorspace. A single rendering context is created and
 * made active (there is no support for multiple contexts). Single rendering (back buffer) surface is created
 * by OpenVG and made active for Read and Write.
 * @param width of the rendering surface (back buffer) size in pixels.
 * @param height of the rendering surface (back buffer) size in pixels.
 * @param colorSpace space to be used. Must be either VGI_COLORSPACE_LINEAR or VGI_COLORSPACE_SRGB.
 * @param premultiplied flag indicates whether to create a premultiplied or non-premultiplied rendering surface.
 * @return Returns VGI_OK if initialization was successful. Returns error code VGI_ERROR_INVALID_ARGUMENTS
 *         if width or height are negative. Returns error code VGI_ERROR_OUT_OF_MEMORY if there is insufficient
 *         free memory to complete the initialization (smaller size for back buffer or smaller antialiasing setting may
 *         work). Returns error code VGI_ERROR_ALREADY_EXISTS if the surface and context already exists in this
 *         process & thread (call VGITerminate to reset). Returns error code VGI_ERROR_COLORSPACE_NOT_SUPPORTED
 *         if the given colorspace is not supported by the implementation.
 */
VGI_API_CALL int VGIInitializeEx( int width, int height, VGIColorSpace colorSpace, int premultiplied, int conformant );

/**
 * Copies the current contents of the internal back buffer to the given target buffer using the given stride
 * and format. Back buffer contents may be undefined after this function as it marks the start of new frame.
 * All target formats are supported by all implementations and the format bit pattern definitions match the
 * Symbian formats (EColo64K, EColor16M, EColor16MU, EColor16MA). Target buffer memory is assumed to be generic
 * system memory (MMU mapped).
 * @param format Format that is to be used when writing data to the given target buffer. Must be one of the
 *        VGI_COLOR_BUFFER_FORMAT_xxx values.
 * @param bufferStride Amount of bytes from beginning of scanline to the beginning of the next.
 *        Do NOT assume that width * bpp always equals stride!
 * @param buffer Target buffer where the current back buffer is copied to.
 *        The implementation uses internal back buffer and this target buffer is used only
 *        during the execution of this method.
 * @param maskStride Amount of bytes from beginning of scanline to the beginning of the next
 *        in the target mask buffer. Do NOT assume that width * bpp always equals stride!
 * @param mask Target buffer for the 8-bits per pixel destination alpha output
 *        (only written to by the method if parameter is not NULL and format is other than VGI_BUFFER_FORMAT_ARGB8888)
 * @param hint Specify how to process transparent pixels.
 *         Value VGI_SKIP_TRANSPARENT_PIXELS means transparent pixels are not copied to the target buffer.
 *         Value VGI_COPY_TRANSPARENT_PIXELS means every pixel transparent or not is copied to the target buffer.
 * @return Returns VGI_OK if copying completed successfully. Returns error code VGI_ERROR_INVALID_ARGUMENTS
 *         if bufferStride or maskStride are negative, if target buffer is either NULL or is not properly aligned. if
 *         the format is invalid or if hint is neither VGI_SKIP_TRANSPARENT_PIXELS nor VGI_COPY_TRANSPARENT_PIXELS.
 *         Returns error code VGI_ERROR_OUT_OF_MEMORY if there is insufficient
 *         free memory to complete the copying (before returning this error code the implementation internally
 *         calls VGITerminate() and current context and surface are lost.
 */
VGI_API_CALL int VGICopyToTarget( VGIColorBufferFormat format, int bufferStride, void *buffer, int maskStride, void *mask, VGICopyToTargetHint hint );

/**
 * Rendering surface and context are freed and may not be used after call to this method. OpenVG is terminated (all
 * other related resources are freed). If this method is called before VGIInitialize, or VGITerminate is called after
 * already being called, this method does nothing. In other words it is safe to call VGITerminate() multiple times
 * sequentially.
 */
VGI_API_CALL void VGITerminate( void );

/**
 * Resizes the back buffer to have the given width and height . This method should always be used to resize the back buffer as in
 * many implementations the resize operation can be handled more efficiently than a plain delete old, create new method.
 * Old context is preserved and can be used after the resize operation. Contents of the new frame buffer are UNDEFINED.
 * @param width Width of the back buffer after resize operation.
 * @param height Height of the back buffer after resize operation.
 * @return Returns VGI_OK if resize operation was successful. Returns error code VGI_ERROR_ILLEGAL_OPERATION if an image is currently
 *    set as the render target. Returns error code VGI_ERROR_INVALID_ARGUMENTS if width
 *    or height are negative or error code VGI_ERROR_OUT_OF_MEMORY if there is insufficient free memory to allocate the back
 *    buffer of given size (smaller size may work).
 * @note: if VGI_ERROR_OUT_OF_MEMORY is returned, the original surface is lost and VGITerminate is called internally (context & surface is lost)
 */
VGI_API_CALL int VGIResize( int width, int height );

/**
 * Redirects rendering to an image.
 * @param image Image to be set as the render target.
 * @return Returns VGI_OK if successful.
 *    Returns error code VGI_ERROR_ILLEGAL_IMAGE_HANDLE if image is not a valid image handle, or is not shared with the current context.
 *    Returns error code VGI_ERROR_NOT_SUPPORTED if image is not of the same format as the "master surface".
 *    Returns error code VGI_ERROR_IMAGE_IN_USE if image shares storage with any other image (via use of the vgChildImage function),
 *      or is set as a paint pattern image on a paint object.
 *    Returns error code VGI_ERROR_OUT_OF_MEMORY if there is insufficient free memory to perform the operation.
 * @note: if VGI_ERROR_OUT_OF_MEMORY is returned, the original surface is lost and VGITerminate is called internally (context & surface is lost)
 */
VGI_API_CALL int VGIBindToImage( VGImage image );

/**
 * Reset the "master surface" as current render target .
 * @return Returns VGI_OK if successful.
 *    Returns error code VGI_ERROR_OUT_OF_MEMORY if there is insufficient free memory to perform the operation.
 * @note: if VGI_ERROR_OUT_OF_MEMORY is returned, the original surface is lost and VGITerminate is called internally (context & surface is lost)
 */
VGI_API_CALL int VGIUnBindImage( void );

#ifdef __cplusplus 
} /* extern "C" */
#endif

#endif /* _OPENVG_CONTEXT_H */
