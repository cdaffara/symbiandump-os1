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

#ifndef OWFNATIVESTREAM_H_
#define OWFNATIVESTREAM_H_

/*!
 * \brief Image stream implementation for Linux.
 *
 * WF native stream is an abstraction of image stream or
 * a content pipe that can be used to deliver image data from
 * place to another. A stream has a producer (source) and a consumer
 * (sink) as its users.
 *
 * Streams operate on buffers, whose count is fixed at creation
 * time (minimum is 1, but for non-blocking behavior values
 * greater than 1 should be used.) Streams are meant to be used
 * strictly on "point-to-point" basis, i.e. there should be only
 * one producer and one consumer for each stream.
 *
 */

#include "owfsemaphore.h"
#include "owflinkedlist.h"
#include "owfimage.h"
#include "owftypes.h"

#include <EGL/egl.h>
#include <WF/wfcplatform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    OWF_STREAM_ERROR_NONE               = 0,
    OWF_STREAM_ERROR_INVALID_STREAM     = -1,
    OWF_STREAM_ERROR_INVALID_OBSERVER   = -2,
    OWF_STREAM_ERROR_OUT_OF_MEMORY      = -3
} OWF_STREAM_ERROR;

typedef WFCHandle           WFCNativeStreamType;
/*!---------------------------------------------------------------------------
 * Converts from external WFC native stream handle type to internal OWF native stream handle type.
 * The internal handle MUST be persistant. The external handle may already be persistant.
 * This method may fail, either due to memory, or due to the stream object not being supported by the compositor
 * @post if successful, internal stream object is returned with increased reference (@see owfNativeStreamDestroy) 
 * @param publicStream The publicly defined stream handle
 * @param error			Pointer to store error code - optionally can be NULL
 * @return OWFNativeStreamType an equivalent internal stream handle
 *----------------------------------------------------------------------------**/
 OWF_PUBLIC OWFNativeStreamType 
owfNativeStreamFromWFC(WFCNativeStreamType publicStream,OWF_STREAM_ERROR* errorReturn);

/*!---------------------------------------------------------------------------
 *  Create new off-screen image stream.
 *
 *  \param width            Stream image buffer width
 *  \param height           Stream image buffer height
 *  \param imageFormat      Stream image buffer format
 *  \param nbufs            Number of image buffers to allocate
 *
 *  \param Handle to newly created stream or OWF_INVALID_HANDLe if no
 *  stream could be created.
 *----------------------------------------------------------------------------*/
 OWF_PUBLIC OWFNativeStreamType
owfNativeStreamCreateImageStream(OWFint width,
                                 OWFint height,
                                 const OWF_IMAGE_FORMAT* format,
                                 OWFint nbufs);

/*!---------------------------------------------------------------------------
 *  Increase stream's reference count
 *
 *  \param stream           Stream handle
 *----------------------------------------------------------------------------*/
 OWF_API_CALL void
owfNativeStreamAddReference(OWFNativeStreamType stream);

/*!---------------------------------------------------------------------------
 *  Decrease stream's reference count
 *
 *  \param stream           Stream handle
 *----------------------------------------------------------------------------*/
 OWF_API_CALL void
owfNativeStreamRemoveReference(OWFNativeStreamType stream);

/*!----------------------------------------------------------------------------
 *  Destroy stream. The stream isn't necessarily immediately destroyed, but
 *  only when it's reference count reaches zero.
 *
 *  \param stream           Stream handle
 *----------------------------------------------------------------------------*/
 OWF_PUBLIC void
owfNativeStreamDestroy(OWFNativeStreamType stream);


/*!---------------------------------------------------------------------------
 * Get stream's image header
 *
 * \param stream            Stream handle
 * \param width             Stream width
 * \param height            Stream height
 * \param stride            Stream stride
 * \param format            Stream format
 * \param pixelSize         Stream pixelSize
 *
 * All the parameters above, except stream handle, are pointers to locations
 * where the particular value should be written to. Passing in a NULL
 * pointer means that the particular values is of no interest to the caller.
 *
 * E.g. to query only width & height one would call this function with
 * parameters (stream_handle, &width, &height, NULL, NULL, NULL);
 *
 *----------------------------------------------------------------------------*/
 OWF_PUBLIC void
owfNativeStreamGetHeader(OWFNativeStreamType stream,
                           OWFint* width,
                           OWFint* height,
                           OWFint* stride,
                           OWF_IMAGE_FORMAT* format,
                           OWFint* pixelSize);

/*!---------------------------------------------------------------------------
 *  Acquire read buffer from stream
 *
 *  \param stream           Stream handle
 *
 *  \return Handle to next readable (unread since last write)
 *  buffer from the stream or OWF_INVALID_HANDLE if no unread buffers
 *  are available.
 *----------------------------------------------------------------------------*/
 OWF_PUBLIC OWFNativeStreamBuffer
owfNativeStreamAcquireReadBuffer(OWFNativeStreamType stream);

/*!---------------------------------------------------------------------------
 *  Release read buffer.
 *
 *  \param stream           Stream handle
 *  \param buf              Buffer handle
 *----------------------------------------------------------------------------*/
 OWF_PUBLIC void
owfNativeStreamReleaseReadBuffer(OWFNativeStreamType stream,
                                 OWFNativeStreamBuffer buf);

/*!---------------------------------------------------------------------------
 *  Acquires writable buffer from a stream. The caller has exclusive access
 *  to returned buffer until the buffer is commited to stream by
 *  calling ReleaseWriteBuffer.
 *
 *  \param stream           Stream handle
 *
 *  \return Handle to next writable buffer or OWF_INVALID_HANDLE if no such
 *  buffer is available.
 *----------------------------------------------------------------------------*/
 OWF_PUBLIC OWFNativeStreamBuffer
owfNativeStreamAcquireWriteBuffer(OWFNativeStreamType stream);

/*!---------------------------------------------------------------------------
 *  \brief Commit write buffer to stream.
 *
 * If sync object is specified, the handle to sync object is
 * associated with the buffer. The sync object is signalled
 * when the image/buffer has been either:
 * - composed into the target stream
 * - dropped (ignored) due to it being superceded by a newer
 *   image/buffer before the compositor had a chance to read it
 *
 * Sync object is signalled exactly once. The caller is responsible
 * of destroying the object.
 *
 *  \param stream           Stream handle
 *  \param buf              Buffer handle
 *  \param dpy              Optional EGLDisplay
 *  \param sync             Optional EGLSync object which is signaled when
 *                          the buffer is consumed or dropped.
 *----------------------------------------------------------------------------*/
 OWF_PUBLIC void
owfNativeStreamReleaseWriteBuffer(OWFNativeStreamType stream,
                                  OWFNativeStreamBuffer buf,
                                  EGLDisplay dpy,
                                  EGLSyncKHR sync);


/*!---------------------------------------------------------------------------
 *  Enable/disable stream content notifications.
 *
 *  \param stream           Stream handle
 *  \param send             Boolean value indicating whether the stream should
 *                          send content notifications to its observers.
 *----------------------------------------------------------------------------*/
 OWF_API_CALL void
owfNativeStreamEnableUpdateNotifications(OWFNativeStreamType stream,
                                         OWFboolean send);


/*!---------------------------------------------------------------------------
 *  Return pointer to stream buffer's pixel data. The buffer must be
 *  a valid read/write buffer.
 *
 *  \param stream           Stream handle
 *  \param buffer           Buffer handle
 *
 *  \return Pointer to buffers pixel data.
 *----------------------------------------------------------------------------*/
 OWF_PUBLIC void*
owfNativeStreamGetBufferPtr(OWFNativeStreamType stream,
                            OWFNativeStreamBuffer buffer);

/*!---------------------------------------------------------------------------
 *  Set/reset stream's protection flag. This flag is used for preventing the
 *  user from deleting a stream that s/he doesn't really own or should
 *  not twiddle with too much (on-screen context's target stream, for example)
 *
 *  \param stream           Stream handle
 *  \param flag             Protection status
 *----------------------------------------------------------------------------*/
 OWF_API_CALL void
owfNativeStreamSetProtectionFlag(OWFNativeStreamType stream,
                                 OWFboolean flag);
 /*!---------------------------------------------------------------------------
  *  Sets (internal) target stream flip state
  *
  *  \param stream           Stream handle
  *
  *----------------------------------------------------------------------------*/
 OWF_API_CALL void
owfSetStreamFlipState(OWFNativeStreamType stream, OWFboolean flip);
 

#ifdef __cplusplus
}
#endif

#endif
