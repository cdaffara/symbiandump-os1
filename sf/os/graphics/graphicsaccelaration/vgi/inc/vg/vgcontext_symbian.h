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
#ifndef _OPENVG_CONTEXT_SYMBIAN_H
#define _OPENVG_CONTEXT_SYMBIAN_H

#include "vg/vgcontext.h"
#include <fbs.h>

/**
 * OpenVG is initialized with the selected size and colorspace. A single rendering context is created and
 * made active (there is no support for multiple contexts). Single rendering (back buffer) surface is created
 * by OpenVG and made active for Read and Write.
 * @param aSize Size of the rendering surface (back buffer) size in pixels.
 * @param aColorSpace Color space to be used. Must be either VGI_COLORSPACE_LINEAR or VGI_COLORSPACE_SRGB.
 * @return Returns KErrNone if initialization was successful. Returns error code KErrArgument
 *         if width or height are negative. Returns error code KErrNoMemory if there is insufficient
 *         free memory to complete the initialization (smaller size for back buffer or smaller antialiasing setting may
 *         work). Returns error code KErrAlreadyExists if the surface and context already exists in this
 *         process & thread (call VGITerminate to reset). Returns error code KErrNotSupported
 *         if the given colorspace is not supported by the implementation.
 */
IMPORT_C TInt VGISymbianInitialize( TSize aSize, VGIColorSpace aColorSpace );

/**
 * OpenVG is initialized with the selected size and colorspace. A single rendering context is created and
 * made active (there is no support for multiple contexts). Single rendering (back buffer) surface is created
 * by OpenVG and made active for Read and Write.
 * @param aSize Size of the rendering surface (back buffer) size in pixels.
 * @param aColorSpace Color space to be used. Must be either VGI_COLORSPACE_LINEAR or VGI_COLORSPACE_SRGB.
 * @return Returns KErrNone if initialization was successful. Returns error code KErrArgument
 *         if width or height are negative. Returns error code KErrNoMemory if there is insufficient
 *         free memory to complete the initialization (smaller size for back buffer or smaller antialiasing setting may
 *         work). Returns error code KErrAlreadyExists if the surface and context already exists in this
 *         process & thread (call VGITerminate to reset). Returns error code KErrNotSupported
 *         if the given colorspace is not supported by the implementation.
 */
IMPORT_C TInt VGISymbianInitializeEx( TSize aSize, VGIColorSpace aColorSpace, TBool aPremultiplied, TBool aConformant );

/**
 * Copies the current contents of the internal back buffer to the given target buffer using the given stride
 * and format. Back buffer contents may be undefined after this function as it marks the start of new frame.
 * All target formats are supported by all implementations and the format bit pattern definitions match the
 * Symbian formats (EColo64K, EColor16M, EColor16MU, EColor16MA). Target buffer memory is assumed to be generic
 * system memory (MMU mapped).
 * @param aBitmap Target buffer where the current back buffer is copied to.
 *        The implementation uses internal back buffer and this target buffer is used only
 *        during the execution of this method.
 * @param aMaskBitmap Target buffer for the 8-bits per pixel destination alpha output
 *        (only written to by the method if parameter is not NULL and format is other than EColor16MA)
 * @param aHint Specify how to process transparent pixels.
 *         Value VGI_SKIP_TRANSPARENT_PIXELS means transparent pixels are not copied to the target buffer.
 *         Value VGI_COPY_TRANSPARENT_PIXELS means every pixel transparent or not is copied to the target buffer.
 * @return Returns KErrNone if copying completed successfully. Returns error code KErrArgument
 *         if aBitmap is NULL  or if aHint is neither VGI_SKIP_TRANSPARENT_PIXELS nor VGI_COPY_TRANSPARENT_PIXELS.
 *         Returns error code KErrNoMemory if there is insufficient
 *         free memory to complete the copying (before returning this error code the implementation internally
 *         calls VGISymbianTerminate() and current context and surface are lost.
 */
IMPORT_C TInt VGISymbianCopyToBitmap( CFbsBitmap *aBitmap, CFbsBitmap *aMaskBitmap = NULL, VGICopyToTargetHint aHint = VGI_COPY_TRANSPARENT_PIXELS );

/**
 * Rendering surface and context are freed and may not be used after call to this method. OpenVG is terminated (all
 * other related resources are freed). If this method is called before VGISymbianInitialize, or VGISymbianTerminate is called after
 * already being called, this method does nothing. In other words it is safe to call VGISymbianTerminate() multiple times
 * sequentially.
 */
IMPORT_C void VGISymbianTerminate();

/**
 * Resizes the back buffer to have the given width and height . This method should always be used to resize the back buffer as in
 * many implementations the resize operation can be handled more efficiently than a plain delete old, create new method.
 * Old context is preserved and can be used after the resize operation. Contents of the new frame buffer are UNDEFINED.
 * @param aSize Size of the rendering surface (back buffer) after resize operation, size in pixels.
 * @return Returns KErrNone if resize operation was successful.
 *	  Returns error code KErrPermissionDenied if an image is currently set as the render target.
 *	  Returns error code KErrArgument if width or height are negative or error code KErrNoMemory if there is insufficient free memory
 *	  to allocate the back buffer of given size (smaller size may work).
 * @note: if KErrNoMemory is returned, the original surface is lost and VGISymbianTerminate is called internally (context & surface is lost)
 */
IMPORT_C TInt VGISymbianResize( TSize aSize );

/**
 * Redirects rendering to an image.
 * @param aImage Image to be set as the render target.
 * @return Returns KErrNone if successful.
 *    Returns error code KErrBadHandle if aImage is not a valid image handle, or is not shared with the current context.
 *    Returns error code KErrNotSupported if aImage is not of the same format as the "master surface".
 *    Returns error code KErrInUse if aImage shares storage with any other image (via use of the vgChildImage function),
 *      or is set as a paint pattern image on a paint object.
 *    Returns error code KErrNoMemory if there is insufficient free memory to perform the operation.
 * @note: if KErrNoMemory is returned, the original surface is lost and VGISymbianTerminate is called internally (context & surface is lost)
 */
IMPORT_C TInt VGISymbianBindToImage( VGImage aImage );

/**
 * Reset the "master surface" as current render target .
 * @return Returns KErrNone if successful.
 *    Returns error code KErrNoMemory if there is insufficient free memory to perform the operation.
 * @note: if KErrNoMemory is returned, the original surface is lost and VGITerminate is called internally (context & surface is lost)
 */
IMPORT_C TInt VGISymbianUnBindImage();

#endif /* _OPENVG_CONTEXT_SYMBIAN_H */
