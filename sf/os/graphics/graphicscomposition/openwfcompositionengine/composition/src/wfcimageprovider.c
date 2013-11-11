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
/*! \ingroup wfc
 *  \file wfcimageprovider.c
 *
 *  \brief SI image providers
 */

#include <stdio.h>
#include <stdlib.h>

#include "wfcimageprovider.h"
#include "wfcdevice.h"
#include "wfccontext.h"
#include "wfcstructs.h"

#include "owfimage.h"
#include "owfarray.h"
#include "owfmemory.h"
#include "owfobject.h"
#include "owfnativestream.h"
#include "owfdebug.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define FIRST_IMAGEPROVIDER_HANDLE   4000

static WFCint               nextImageProviderHandle =
                            FIRST_IMAGEPROVIDER_HANDLE;

OWF_API_CALL void
WFC_IMAGE_PROVIDER_Ctor(void* self)
{
    WFC_IMAGE_PROVIDER*     ip;

    ENTER(WFC_IMAGE_PROVIDER_Ctor);

    ip = IMAGE_PROVIDER(self);
    
    ip->lockedStream.image=NULL;
    ip->lockedStream.lockCount=0;
    
    LEAVE(WFC_IMAGE_PROVIDER_Dtor);
}

OWF_API_CALL void
WFC_IMAGE_PROVIDER_Dtor(void* self)
{

    WFC_IMAGE_PROVIDER*     ip;

    ENTER(WFC_IMAGE_PROVIDER_Dtor);

    ip = IMAGE_PROVIDER(self);

    DPRINT(("ptr=%p, handle=%d", ip, ip->handle));
    owfNativeStreamDestroy(ip->streamHandle);
    DESTROY(ip->owner);
    if (ip->lockedStream.image)
        {
        if (ip->lockedStream.lockCount)
            {   /* belts and braces: unlock the read buffer when image provider is destroyed */
            DPRINT(("Native stream buffer still locked when Image Provider destroyed ptr=%p, handle=%d", ip, ip->handle));
            owfNativeStreamReleaseReadBuffer(ip->streamHandle, ip->lockedStream.buffer);
            }
        OWF_Image_Destroy(ip->lockedStream.image);
        }
    LEAVE(WFC_IMAGE_PROVIDER_Dtor);
}

static WFC_IMAGE_PROVIDER*
WFC_ImageProvider_DoCreate(void* owner, /*WFC_CONTEXT* context,*/
                           OWFNativeStreamType stream,
                           WFC_IMAGE_PROVIDER_TYPE type)
{
    WFC_IMAGE_PROVIDER*         object;

    ENTER(WFC_ImageProvider_DoCreate);

    if (!stream)
    {
        return NULL;
    }
    object = CREATE(WFC_IMAGE_PROVIDER);

    if (!object)
    {
        return NULL;
    }

    owfNativeStreamAddReference(stream);
    object->streamHandle = stream;
    object->type            = type;
    object->contentUpdated  = WFC_FALSE;

    WFC_ImageProvider_LockForReading(object);
    if (object->lockedStream.image==NULL || object->lockedStream.image->data==NULL)
        {
        owfNativeStreamDestroy(stream);
        DESTROY(object);
        return NULL;
        }
    WFC_ImageProvider_Unlock(object);
    ADDREF(object->owner, owner);

    LEAVE(WFC_ImageProvider_DoCreate);
    return object;
}

OWF_API_CALL WFC_IMAGE_PROVIDER*
WFC_ImageProvider_Create(void* owner, /*WFC_CONTEXT* context,*/
                         OWFNativeStreamType stream,
                         WFC_IMAGE_PROVIDER_TYPE type)
{
    WFC_IMAGE_PROVIDER*               object;

    ENTER(WFC_ImageProvider_Create);

    object = WFC_ImageProvider_DoCreate(owner/*context*/, stream, type);

    if (object)
    {
        object->handle = nextImageProviderHandle++;
        DPRINT(("WFC_ImageProvider_Create: attaching image provider %d to "
                "stream %p",
               object->handle, object->streamHandle));
    }

    LEAVE(WFC_ImageProvider_Create);
    return object;
}

OWF_API_CALL void
WFC_ImageProvider_LockForReading(WFC_IMAGE_PROVIDER* provider)
{
    void*                   pixels;
    OWFint                  width, height, pixelSize = 0;
    OWF_IMAGE_FORMAT        imgf;
    OWFint                  stride;

    if (!provider) {
        DPRINT(("WFC_ImageProvider_LockForReading: provider = NULL"));
        return;
    }
    OWF_ASSERT(provider->streamHandle);

     DPRINT(("stream = %p", provider->streamHandle));

     if (!provider->lockedStream.lockCount)
     {
         DPRINT(("About to acquire & lock a read buffer"));
         /* acquire buffer */
         provider->lockedStream.buffer = owfNativeStreamAcquireReadBuffer(provider->streamHandle);
         DPRINT(("  Acquired read buffer stream=%p, buffer=%d",
                 provider->streamHandle, provider->lockedStream.buffer));

         /* Bind source image to pixel buffer */
         pixels = owfNativeStreamGetBufferPtr(provider->streamHandle, provider->lockedStream.buffer);
         if (provider->lockedStream.image)
             {
             OWF_Image_SetPixelBuffer(provider->lockedStream.image,pixels);
             }
         else
             {
             owfNativeStreamGetHeader(provider->streamHandle,
                                      &width, &height,
                                      &stride, &imgf, &pixelSize);
             provider->lockedStream.image = OWF_Image_Create(width, height, &imgf, pixels, stride);
             }
     }

     ++provider->lockedStream.lockCount;
     DPRINT(("lock count = %d", provider->lockedStream.lockCount));

}

OWF_API_CALL void
WFC_ImageProvider_Unlock(WFC_IMAGE_PROVIDER* provider)
{
    if (!provider) {
        DPRINT(("WFC_ImageProvider_Unlock: provider = NULL"));
        return;
    }

    if (provider->lockedStream.lockCount > 0)
    {
        --provider->lockedStream.lockCount;
        DPRINT(("lock count = %d", provider->lockedStream.lockCount));

        if (!provider->lockedStream.lockCount)
        {
            DPRINT(("  Releasing read buffer provider=%p, buffer=%d",
                    provider->handle, provider->lockedStream.buffer));
            owfNativeStreamReleaseReadBuffer(provider->streamHandle, provider->lockedStream.buffer);
        }
    }
}



#ifdef __cplusplus
}
#endif

