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

/*!
 * Ring buffer based Image Stream implementation for Linux.
 */



#ifdef __cplusplus
extern "C" {
#endif

#define OWF_NATIVESTREAM_HANDLE 0xAB

#include <stdlib.h>
#include <string.h>

#include "owfnativestream.h"
#include "owfscreen.h"
#include "owfmemory.h"
#include "owfdebug.h"
#include "owftypes.h"
#include "owfmutex.h"
#include "owfhstore.h"

/*needed for owfNativeStreamFromWFC function */
#include "WF/wfcplatform.h"

/* macro for fetching stream object by handle */
#define GET_STREAM(ns, s)       ns = owfNativeStreamGet(s)

/* stream null checks */
#define CHECK_STREAM(v)     if (NULL == ns) { return v;}
#define CHECK_STREAM_NR()   if (NULL == ns) { return; }

#define BUFFER_HANDLE_BASE  0x100
#define STREAM_HANDLE_BASE  10000
#define STREAM_HASHTABLE_SIZE   0x100

/* buffer handle sanity checks */
#define CHECK_BUFFER_NR(x)  if ((x) < BUFFER_HANDLE_BASE || \
                                (x) >= BUFFER_HANDLE_BASE + ns->bufferCount) \
                            { \
                                return; \
                            }

#define CHECK_BUFFER(x,v)   if ((x) < BUFFER_HANDLE_BASE || \
                                (x) >= BUFFER_HANDLE_BASE + ns->bufferCount) \
                            { \
                            return v; \
                            }

#define BUFFER(x)           ns->bufferList[(x)-BUFFER_HANDLE_BASE]

#define INDEX_TO_HANDLE(x)  ((OWFNativeStreamBuffer) ((x)+BUFFER_HANDLE_BASE))
#define HANDLE_TO_INDEX(x)  ((int) ((x)-BUFFER_HANDLE_BASE))

typedef struct {
    EGLDisplay     dpy;
    EGLSyncKHR*    sync;
} OWF_SYNC_DESC;


/*!
 * Structure for image stream.
 */
typedef struct {
    OWFNativeStreamType     handle;     /* stream handle */
    void**                  bufferList;
    OWFint*                 bufferRefs;
    OWFint                  bufferCount;
    OWFint                  lockCount;
    OWFint                  screenNumber;
    OWFint                  width,      /* frame width (pixels) */
                            height,     /* frame height (pixels) */
                            stride;     /* size of single row (bytes) */
    OWF_IMAGE_FORMAT        colorFormat;
    OWFint                  referenceCount;
    OWF_MUTEX               mutex;
    OWF_SEMAPHORE           writer;
    OWFint                  idxFront;
    OWFint                  idxNextFree;
    OWF_NODE*               observers;
    OWFboolean              sendNotifications;
    OWFboolean              protected;  /* protection flag to prevent the
                                           user from destroying the stream
                                           (for onscreen-context use) */

    OWF_SYNC_DESC*          bufferSyncs; /* sync object to be signalled when
                                            buffer is 'consumed' */
} OWF_NATIVE_STREAM;


static const OWF_IMAGE_FORMAT   owfOnScreenColorFormat =
                                {
                                    OWF_IMAGE_ARGB8888,
                                    OWF_FALSE,
                                    OWF_TRUE,
                                    4
                                };

/*============================================================================
 * PRIVATE PARTS
 *============================================================================*/

/*!---------------------------------------------------------------------------
 * Internal.
 *----------------------------------------------------------------------------*/
/*

*/
/*!---------------------------------------------------------------------------
 *  Add stream to internal stream dictionary
 *
 *  \param handle           Stream handle
 *  \param stream           Stream object associated with the handle
 *
 *  \return Boolean value indicating whether the addition was succesful
 *----------------------------------------------------------------------------*/
static OWFHandle
owfNativeStreamAddStream(OWF_NATIVE_STREAM* stream)
{
    OWFHandle handle = OWF_INVALID_HANDLE;

    handle = OWF_HStore_HandleCreate(OWF_NATIVESTREAM_HANDLE, stream);

    return handle;
}

/*----------------------------------------------------------------------------
 *  Returns the stream object associated to given handle
 *
 *  \param handle           Stream handle
 *
 *  \return Stream object or NULL if the handle is invalid
 *----------------------------------------------------------------------------*/
static OWF_NATIVE_STREAM*
owfNativeStreamGet(OWFNativeStreamType handle)
{
    return (OWF_NATIVE_STREAM*) OWF_HStore_GetObj(handle, OWF_NATIVESTREAM_HANDLE);
}

/*----------------------------------------------------------------------------
 *  Remove stream from internal stream dictionary
 *
 *  \param handle           Stream handle
 *
 *  \return Boolean value indicating success of the deletion
 *----------------------------------------------------------------------------*/
static OWFboolean
owfNativeStreamRemove(OWFNativeStreamType handle)
{
    OWF_HStore_HandleDelete(handle);
    return OWF_TRUE;
}

/*!---------------------------------------------------------------------------
 *  Notify stream's observers about an event
 *
 *  \param stream           Stream
 *  \param event            Event to notify about
 *----------------------------------------------------------------------------*/
static void owfNativeStreamNotifyObservers(OWFNativeStreamType stream,
                                           OWFNativeStreamEvent event)
{
    OWF_NODE*               iter = NULL;

    OWF_NATIVE_STREAM* ns;

    DPRINT(("owfNativeStreamNotifyObservers(%p, %x)",
             stream, event));

    GET_STREAM(ns, stream);
    CHECK_STREAM_NR();

    if (ns->sendNotifications)
    {
        OWF_Mutex_Lock(&ns->mutex);

        iter = ns->observers;
        while (iter)
        {
            OWFStreamCallbackData* cbdata = (OWFStreamCallbackData*) iter->data;

            DPRINT(("Stream callback: (%p)(%p, %x, %p)",
                    cbdata->callback, stream, event, cbdata->data));

            if (cbdata->callback)
            {
                (cbdata->callback)(stream, event, cbdata->data);
            }
            iter = iter->next;
        }
        OWF_Mutex_Unlock(&ns->mutex);
    }
}

/*----------------------------------------------------------------------------
 *  Observer equality comparison
 *----------------------------------------------------------------------------*/
static OWFint
ObserversEqual(void* arg1, void* arg2)
{
#define _CALLBACK(x)        ((OWFStreamCallbackData*)(x))


    return (OWFint)(    _CALLBACK(arg1)->callback ==_CALLBACK(arg2)->callback
                    &&  _CALLBACK(arg1)->data ==_CALLBACK(arg2)->data
                    );

#undef _CALLBACK
}

static OWFint
ObserverFuncsEqual(void* arg1, void* arg2)
{
#define _CALLBACK(x)        ((OWFStreamCallbackData*)(x))


    return (OWFint)(_CALLBACK(arg1)->callback ==_CALLBACK(arg2)->callback
                    );

#undef _CALLBACK
}

static OWFint
ObserverDatasEqual(void* arg1, void* arg2)
{
#define _CALLBACK(x)        ((OWFStreamCallbackData*)(x))


    return (OWFint)(_CALLBACK(arg1)->data ==_CALLBACK(arg2)->data
                    );

#undef _CALLBACK
}

/*----------------------------------------------------------------------------
 *  Destroy native stream (implementation)
 *
 *  \param ns               Native stream object
 *----------------------------------------------------------------------------*/
static void
owfNativeStreamDoDestroy(OWF_NATIVE_STREAM* ns)
{
    OWFint                  ii = 0;

    OWF_ASSERT(ns);

    /* bail out if the stream is protected (e.g. the user tries to
     * free on-screen context's stream) */
    OWF_Mutex_Lock(&ns->mutex);
    if (ns->protected)
    {
        OWF_Mutex_Unlock(&ns->mutex);
        return;
    }

    /* decrease reference count and check whether it is safe to
     * actually destroy the stream */
    ns->referenceCount--;

    if (ns->referenceCount > 0) {
        OWF_Mutex_Unlock(&ns->mutex);
        return;
    }

    /* remove from internal dictionary */
    if (ns->handle != OWF_INVALID_HANDLE)
    {
        owfNativeStreamRemove(ns->handle);
    }

    OWF_ASSERT(0 == ns->lockCount);

    /* release resources allocated by the stream. */
    for (ii = 0; ii < ns->bufferCount; ii++)
    {
        OWF_Image_FreeData(&ns->bufferList[ii]);
    }
    xfree(ns->bufferList);

    OWF_Semaphore_Destroy(&ns->writer);
    OWF_Mutex_Unlock(&ns->mutex);
    OWF_Mutex_Destroy(&ns->mutex);

    ns->mutex = NULL;

    while (ns->observers)
    {
        OWF_NODE* next = ns->observers->next;
        xfree(ns->observers);
        ns->observers = next;
    }

    xfree(ns->bufferSyncs);
    xfree(ns->bufferRefs);
    xfree(ns);
}

/*============================================================================
 * PUBLIC API STARTS HERE
 *============================================================================*/

/*!----------------------------------------------------------------------------
 *  Create new native stream.
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
                                 const OWF_IMAGE_FORMAT* imageFormat,
                                 OWFint nbufs)
{
    OWF_NATIVE_STREAM*      ns          = NULL;
    OWFint                  multiFail   = 0;
    void**                  bufferList  = NULL;
    OWFint*                 bufferRefs  = NULL;
    OWF_SYNC_DESC*          bufferSyncs = NULL;
    OWFint                  ii          = 0,
                            j           = 0;
    OWFint                  ok          = 0;

    OWF_ASSERT(nbufs >= 1);

    /* stream must have at least 2 buffers (front & back) */
    ns          = NEW0(OWF_NATIVE_STREAM);
    bufferList  = xalloc(sizeof(void*), nbufs);
    bufferRefs  = xalloc(sizeof(int*), nbufs);

    bufferSyncs  = xalloc(sizeof(OWF_SYNC_DESC), nbufs);


    /* initialize surface/buffer list */
    if (bufferList)
    {
        for (ii = 0; ii < nbufs; ii++)
        {
            bufferList[ii] = OWF_Image_AllocData(width, height,
                                                 imageFormat->pixelFormat);
            if (!bufferList[ii])
            {
                multiFail++;
                break;
            }
        }
    }

    if (!ns || !bufferList || multiFail || !bufferRefs || !bufferSyncs)
    {
        xfree(ns);
        if (bufferList)
        {
            for (j = 0; j < ii; j++)
            {
                OWF_Image_FreeData(&bufferList[j]);
            }
        }
        xfree(bufferList);
        xfree(bufferRefs);
        xfree(bufferSyncs);

        return OWF_INVALID_HANDLE;
    }

    ns->bufferList      = bufferList;
    ns->bufferRefs      = bufferRefs;
    ns->bufferCount     = nbufs;

    ns->width           = width;
    ns->height          = height;

    ns->idxFront        = 0;
    ns->idxNextFree     = 1;
    memcpy(&ns->colorFormat, imageFormat, sizeof(ns->colorFormat));
    ns->stride          = OWF_Image_GetStride(width, imageFormat);
    ns->referenceCount  = 1;
    ns->sendNotifications = OWF_TRUE;
    ns->protected       = OWF_FALSE;

    ok = OWF_Semaphore_Init(&ns->writer, nbufs);
    if (ok == 0)
    {
        ok = OWF_Mutex_Init(&ns->mutex);
    }

    ns->bufferSyncs = bufferSyncs;

    ns->handle = owfNativeStreamAddStream(ns);
    if (ns->handle == OWF_INVALID_HANDLE || ok != 0)
    {
        owfNativeStreamDoDestroy(ns);
        ns = NULL;
        return OWF_INVALID_HANDLE;
    }

    return ns->handle;
}
/*!---------------------------------------------------------------------------
 * Converts from external WFC native stream handle type to internal OWF native stream handle type.
 * The internal handle MUST be persistant. The external handle nmay already be persistant.
 * This method may fail, either due to memory, or due to the stream object not being supported by the compositor
 * @param publicStream The publicly defined stream handle
 * @param error			Pointer to store error code - optionally can be NULL
 * @return OWFNativeStreamType an equivalent internal stream handle
 *----------------------------------------------------------------------------**/
OWF_PUBLIC OWFNativeStreamType owfNativeStreamFromWFC(WFCNativeStreamType publicStream,OWF_STREAM_ERROR* errorReturn)
	{
	OWFNativeStreamType rv=(OWFNativeStreamType)publicStream;
	OWF_IMAGE_FORMAT format;
	owfNativeStreamGetHeader(rv,NULL,NULL,NULL,&format,NULL);
	if (format.pixelFormat==OWF_IMAGE_NOT_SUPPORTED)
		{
		if (errorReturn)
			{
			*errorReturn=OWF_STREAM_ERROR_INVALID_STREAM;
			}
		return OWF_INVALID_HANDLE; 
		}
	owfNativeStreamAddReference(rv);
	if (errorReturn)
		{
		*errorReturn=OWF_STREAM_ERROR_NONE;
		}
	return rv;
	}

/*!---------------------------------------------------------------------------
 *  Increase stream's reference count
 *
 *  \param stream           Stream handle
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
owfNativeStreamAddReference(OWFNativeStreamType stream)
{
    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);

    CHECK_STREAM_NR();

    OWF_Mutex_Lock(&ns->mutex);
    ++(ns->referenceCount);
    OWF_Mutex_Unlock(&ns->mutex);
}

/*!---------------------------------------------------------------------------
 *  Decrease stream's reference count
 *
 *  \param stream           Stream handle
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
owfNativeStreamRemoveReference(OWFNativeStreamType stream)
{
    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);

    CHECK_STREAM_NR();
    OWF_Mutex_Lock(&ns->mutex);
    --(ns->referenceCount);
    OWF_Mutex_Unlock(&ns->mutex);
}

/*!---------------------------------------------------------------------------
 *  Destroy stream. The stream isn't necessarily immediately destroyed, but
 *  only when it's reference count reaches zero.
 *
 *  \param stream           Stream handle
 *----------------------------------------------------------------------------*/
OWF_PUBLIC void
owfNativeStreamDestroy(OWFNativeStreamType stream)
{
    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM_NR();

    owfNativeStreamDoDestroy(ns);
}

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
owfNativeStreamAcquireReadBuffer(OWFNativeStreamType stream)
{
    OWF_NATIVE_STREAM* ns;
    OWFNativeStreamBuffer buffer = OWF_INVALID_HANDLE;

    GET_STREAM(ns, stream);
    CHECK_STREAM(OWF_INVALID_HANDLE);

    OWF_Mutex_Lock(&ns->mutex);

    if (ns->bufferCount == 1)
    {
        /* Single buffered stream.
         * A "Write lock" must not block reading */
        buffer = INDEX_TO_HANDLE(0);
        ++(ns->bufferRefs[0]); /* Increase buffer's reference count */
    }
    else
    {
        buffer = INDEX_TO_HANDLE(ns->idxFront);
        /* Increase reference count of front buffer */
        ++(ns->bufferRefs[ns->idxFront]);
    }
    OWF_Mutex_Unlock(&ns->mutex);

    return buffer;
}

/*!---------------------------------------------------------------------------
 *  Release read buffer.
 *
 *  \param stream           Stream handle
 *  \param buf              Buffer handle
 *----------------------------------------------------------------------------*/
OWF_PUBLIC void
owfNativeStreamReleaseReadBuffer(OWFNativeStreamType stream,
                                 OWFNativeStreamBuffer buf)
{
    OWFint           i = 0;
    OWF_SYNC_DESC*   syncDesc = NULL;

    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM_NR();
    CHECK_BUFFER_NR(buf);

    OWF_Mutex_Lock(&ns->mutex);

    i = HANDLE_TO_INDEX(buf);

    OWF_ASSERT(ns->bufferRefs[i] > 0);

    --(ns->bufferRefs[i]);

    syncDesc = &ns->bufferSyncs[i];
    if (syncDesc->sync != NULL)
    {
        DPRINT(("signalling synched buffer(%p, %x)",
                stream, syncDesc->sync));

        eglSignalSyncKHR(syncDesc->dpy, syncDesc->sync, EGL_SIGNALED_KHR);
        syncDesc->dpy = EGL_NO_DISPLAY;
        syncDesc->sync = NULL;
    }
    OWF_Mutex_Unlock(&ns->mutex);
}

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
owfNativeStreamAcquireWriteBuffer(OWFNativeStreamType stream)
{
    OWFNativeStreamBuffer   buffer = OWF_INVALID_HANDLE;

    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM(OWF_INVALID_HANDLE);

    OWF_Mutex_Lock(&ns->mutex);

    /* write always blocks */
    OWF_Semaphore_Wait(&ns->writer);

    if (ns->bufferCount == 1)
    {
        /* Single buffered stream */
        buffer = INDEX_TO_HANDLE(0);
        ++(ns->bufferRefs[0]); /* Increase buffer's reference count */
    }
    else
    {
        if (ns->idxFront == ns->idxNextFree)
        {
            buffer = OWF_INVALID_HANDLE;
        }
        else
        {
            buffer = INDEX_TO_HANDLE(ns->idxNextFree);

            ++(ns->bufferRefs[ns->idxNextFree]); /* Increase buffer's
                                                    reference count */
            ++(ns->idxNextFree); /* Move pointer to next buffer */
            if (ns->idxNextFree == ns->bufferCount)
            {
                ns->idxNextFree = 0;
            }
        }
    }

    if (buffer != OWF_INVALID_HANDLE)
    {
        /* Signal associated 'old' sync because
         * buffer gets 'dropped', never consumed
         */

        OWFint           bufferIndex;
        OWF_SYNC_DESC*   syncDesc;

        bufferIndex = HANDLE_TO_INDEX(buffer);
        syncDesc = &ns->bufferSyncs[bufferIndex];

        if (syncDesc->sync != NULL)
        {
            DPRINT(("dropping synched buffer(%p, %x)",
                    stream, syncDesc->sync));

            eglSignalSyncKHR(syncDesc->dpy, syncDesc->sync, EGL_SIGNALED_KHR);
            syncDesc->dpy = EGL_NO_DISPLAY;
            syncDesc->sync = NULL;
        }
    }

    OWF_Mutex_Unlock(&ns->mutex);

    return buffer;
}

/*!---------------------------------------------------------------------------
 *  Commit write buffer to stream.
 *
 *  \param stream           Stream handle
 *  \param buf              Buffer handle
 *  \param sync             EGLSync object which is signalled when
 *                          release buffer gets consumed or dropped
 *----------------------------------------------------------------------------*/
OWF_PUBLIC void
owfNativeStreamReleaseWriteBuffer(OWFNativeStreamType stream,
                                 OWFNativeStreamBuffer buf,
                                 EGLDisplay dpy,
                                 EGLSyncKHR sync)
{
    OWFint                  bufferIndex = 0;

    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM_NR();

    CHECK_BUFFER_NR(buf);

    OWF_Mutex_Lock(&ns->mutex);

    bufferIndex = HANDLE_TO_INDEX(buf);
    OWF_ASSERT(ns->bufferRefs[bufferIndex] > 0);

    /* Look up correct buffer (naive search) */
    --(ns->bufferRefs[bufferIndex]); /* Decrease buffer's reference count */
    ns->idxFront = bufferIndex; /* Update front buffer to point to new
                                   front buffer */

    OWF_Semaphore_Post(&ns->writer);

    /* sync object bookkeeping */
    ns->bufferSyncs[bufferIndex].dpy = dpy;
    ns->bufferSyncs[bufferIndex].sync = sync;

    OWF_Mutex_Unlock(&ns->mutex);

    DPRINT(("Stream updated %p", stream ));

    owfNativeStreamNotifyObservers(stream, OWF_STREAM_UPDATED);
}

/*!---------------------------------------------------------------------------
 *  Register stream content observer. The observer will receive buffer
 *  modification event from the stream whenever a buffer is committed.
 *
 *  \param stream           Stream handle
 *  \param observer         Stream observer
 *  \param data             Optional data to pass to observer callback
 *                          function when event is dispatched.
 *----------------------------------------------------------------------------*/
OWF_PUBLIC OWF_STREAM_ERROR
owfNativeStreamAddObserver(OWFNativeStreamType stream,
                           OWFStreamCallback observer,
                           void* data)
{
    OWF_NODE*               node = NULL;
    OWFStreamCallbackData* cbdata = NULL;

    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM(OWF_STREAM_ERROR_INVALID_STREAM);

    /* exclusive access only for you my friend. */
    OWF_Mutex_Lock(&ns->mutex);



    /* new observer. allocate a slot with extra space needed
     * to store the callback data. */
    node = xalloc(sizeof(OWF_NODE) + sizeof(OWFStreamCallbackData), 1);
    if (!node)
    {
        OWF_Mutex_Unlock(&ns->mutex);
        return OWF_STREAM_ERROR_OUT_OF_MEMORY;
    }
    
    /* callback data is directly after node in the memory*/
    cbdata = (OWFStreamCallbackData*) &node[1];
    cbdata->callback = observer;
    cbdata->data = data;
    /* for convenience. */
    node->data = (void*) cbdata;


    if (node)
    {
        /* append to callback-chain */
        ns->observers = OWF_List_Append(ns->observers, node);
    }

    OWF_Mutex_Unlock(&ns->mutex);

    return OWF_STREAM_ERROR_NONE;
}

/*!---------------------------------------------------------------------------
 *  Remove stream content observer.
 *  Observer or Data can be NULL indicating search only checks for other member.
 *  Both must not be NULL.
 *  \param stream           Stream handle
 *  \param observer         Observer to remove
 *  \param data             Identifying client data
 *
 *  \return Zero if the observer was removed successfully, otherwise non-zero
 *  (OWF_STREAM_ERROR_INVALID_STREAM if the stream is invalid;
 *   OWF_STREAM_ERROR_INVALID_OBSERVER if the observer is invalid.)
 *----------------------------------------------------------------------------*/
OWF_PUBLIC OWFint
owfNativeStreamRemoveObserver(OWFNativeStreamType stream,
                              OWFStreamCallback observer,
                              void* data)
{
    OWF_NODE*               node = NULL;
    OWFStreamCallbackData   tmp;

    NODECMPFUNC search=ObserversEqual;

    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM(OWF_STREAM_ERROR_INVALID_STREAM);

    OWF_Mutex_Lock(&ns->mutex);

    tmp.callback = observer;
    tmp.data = data;
    if (!observer)
    {
        if (!data)
        {
            return OWF_STREAM_ERROR_INVALID_OBSERVER;
        }
        else
        {
            search=ObserverDatasEqual;
        }
    }
    else
    {
        if (!data)
        {
            search=ObserverFuncsEqual;
        }
    }
    
    node = OWF_List_Find(ns->observers, search, &tmp);

    if (node)
    {
        /* taketh the observer away */
        ns->observers = OWF_List_Remove(ns->observers, node);
        /*  to death */
        xfree(node);
    }

    OWF_Mutex_Unlock(&ns->mutex);

    return node ? OWF_STREAM_ERROR_NONE :
                  OWF_STREAM_ERROR_INVALID_OBSERVER;
}

/*!---------------------------------------------------------------------------
 *  Enable/disable stream content notifications.
 *
 *  \param stream           Stream handle
 *  \param send             Boolean value indicating whether the stream should
 *                          send content notifications to its observers.
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
owfNativeStreamEnableUpdateNotifications(OWFNativeStreamType stream,
                                         OWFboolean send)
{
    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM_NR();

    OWF_Mutex_Lock(&ns->mutex);

    ns->sendNotifications = send;

    OWF_Mutex_Unlock(&ns->mutex);
}

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
                            OWFNativeStreamBuffer buffer)
{
    void*                   bufferPtr = NULL;

    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM(NULL);

    /* Check that buffer has been locked */
    OWF_ASSERT(ns->bufferRefs[HANDLE_TO_INDEX(buffer)] > 0);

    OWF_Mutex_Lock(&ns->mutex);

    bufferPtr = ns->bufferList[HANDLE_TO_INDEX(buffer)];

    OWF_Mutex_Unlock(&ns->mutex);

    return bufferPtr;
}

/*!---------------------------------------------------------------------------
 *  Set/reset stream's protection flag. This flag is used for preventing the
 *  user from deleting a stream that s/he doesn't really own or should
 *  not twiddle with too much (on-screen context's target stream, for example)
 *
 *  \param stream           Stream handle
 *  \param flag             Protection status
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
owfNativeStreamSetProtectionFlag(OWFNativeStreamType stream, OWFboolean flag)
{
    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);
    CHECK_STREAM_NR();

    OWF_Mutex_Lock(&ns->mutex);
    ns->protected = flag;
    OWF_Mutex_Unlock(&ns->mutex);
}


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
                           OWFint* pixelSize)
{
    OWF_NATIVE_STREAM* ns;

    GET_STREAM(ns, stream);

    CHECK_STREAM_NR();

    if (width)
    {
        *width  = ns->width;
    }
    if (height)
    {
        *height = ns->height;
    }
    if (stride)
    {
        *stride = ns->stride;
    }
    if (format)
    {
        memcpy(format, &ns->colorFormat, sizeof(*format));
    }

    if (pixelSize)
    {
        *pixelSize = OWF_Image_GetFormatPixelSize(ns->colorFormat.pixelFormat);
    }
}

#ifdef __cplusplus
}

#endif
