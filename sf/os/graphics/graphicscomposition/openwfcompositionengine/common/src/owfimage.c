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

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "owfobject.h"
#include "owfimage.h"
#include "owfutils.h"
#include "owfmemory.h"
#include "owfdebug.h"
#include "owfdisplaycontextgeneral.h"

#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
#define roundSubPixel(p) ((OWFuint32)((p) + 0.5f))
#else
#define roundSubPixel(p) (p)
#endif


/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_IMAGE_Ctor(void* self)
{
    DPRINT(("OWF_IMAGE_Ctor"));

    self = self;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_IMAGE_Dtor(void* self)
{
    OWF_IMAGE* image;

    DPRINT(("OWF_IMAGE_Dtor"));

    image = (OWF_IMAGE*)self;

    if (image && image->data)
    {
        if (!image->foreign)
        {
            OWF_Image_FreeData(0, &image->data);
        }
    }

    if (image)
    {
        memset(image, 0, sizeof(OWF_IMAGE));
    }
}

/*----------------------------------------------------------------------------*/
static OWFfloat
NonLinear(OWFfloat x)
{
    if (x <= 0.00304f)
    {
        return 12.92f * x;
    }
    return 1.0556f * pow(x, 1.f/2.4f) - 0.0556f;
}

/*----------------------------------------------------------------------------*/
static OWFfloat
Linear(OWFfloat x)
{
    if (x <= 0.03928)
    {
        return x / 12.92f;
    }
    return pow((x + 0.0556f) / 1.0556f, 2.4f);
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_NonLinearizeData(OWF_IMAGE* image)\
{
    OWFpixel*            ptr;
    OWFint               count;

    DPRINT(("OWF_Image_NonLinearizeData"));

    OWF_ASSERT(image != NULL && image->data != NULL);
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    if (!image->format.linear)
    {
        return;
    }

    ptr = (OWFpixel*) image->data;
    count = image->width * image->height;

    while (count > 0)
    {
        ptr->color.red   = (OWFsubpixel) NonLinear(ptr->color.red /
                               OWF_RED_MAX_VALUE) * OWF_RED_MAX_VALUE;
        ptr->color.green = (OWFsubpixel) NonLinear(ptr->color.green /
                               OWF_GREEN_MAX_VALUE) * OWF_GREEN_MAX_VALUE;
        ptr->color.blue  = (OWFsubpixel) NonLinear(ptr->color.blue /
                               OWF_BLUE_MAX_VALUE) * OWF_BLUE_MAX_VALUE;

        --count;
        ptr++;
    }

    image->format.linear = OWF_FALSE;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_LinearizeData(OWF_IMAGE* image)
{
    OWFpixel*               ptr;
    OWFuint                 count;

    DPRINT(("OWF_Image_LinearizeData"));

    OWF_ASSERT(image != NULL && image->data != NULL);
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    if (image->format.linear)
    {
        return;
    }

    ptr = (OWFpixel*) image->data;
    count = image->width * image->height;

    while (count > 0)
    {
        ptr->color.red   = (OWFsubpixel) Linear(ptr->color.red /
                               OWF_RED_MAX_VALUE) * OWF_RED_MAX_VALUE;
        ptr->color.green = (OWFsubpixel) Linear(ptr->color.green /
                               OWF_GREEN_MAX_VALUE) * OWF_GREEN_MAX_VALUE;
        ptr->color.blue  = (OWFsubpixel) Linear(ptr->color.blue /
                               OWF_BLUE_MAX_VALUE) * OWF_BLUE_MAX_VALUE;

        --count;
        ptr += image->pixelSize;
    }

    image->format.linear = OWF_TRUE;

}

/*----------------------------------------------------------------------------*/
#define GAMMA(color, max, gamma) (max * pow(color/max, gamma))

OWF_API_CALL void
OWF_Image_Gamma(OWF_IMAGE* image, OWFfloat gamma)
{
    OWFpixel*            ptr;
    OWFint               count;

    DPRINT(("OWF_Image_Gamma"));

    OWF_ASSERT(image != NULL && image->data != NULL);
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    if (gamma == 1.0f)
    {
        return;
    }

    ptr = (OWFpixel*) image->data;
    count = image->width * image->height;

    while (count > 0)
    {
        ptr->color.red   =
            (OWFsubpixel) GAMMA(ptr->color.red, OWF_RED_MAX_VALUE, gamma);
        ptr->color.green =
            (OWFsubpixel) GAMMA(ptr->color.green, OWF_GREEN_MAX_VALUE, gamma);
        ptr->color.blue  =
            (OWFsubpixel) GAMMA(ptr->color.blue, OWF_BLUE_MAX_VALUE, gamma);

        --count;
        ptr++;
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_EdgeReplication(OWF_IMAGE* image)
{
    OWFint          y;
    OWFint          copyStride;
    OWFuint8*       srcPtr      = NULL;
    OWFuint8*       dstPtr      = NULL;
    
    DPRINT(("OWF_Image_EdgeReplication"));

    OWF_ASSERT(image);
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);
    OWF_ASSERT(image->width >= 3 && image->height >= 3);
    
    copyStride = OWF_Image_GetStride(image->width, &image->format, 0);
    
    /* top side replication */
    srcPtr = (OWFuint8*) image->data;
    srcPtr += 1 * image->stride + 0 * image->pixelSize;
    dstPtr = (OWFuint8*) image->data;

    memcpy(dstPtr, srcPtr, copyStride);
    
    /* bottom side replication */
    srcPtr = (OWFuint8*) image->data;
    srcPtr += (image->height-2) * image->stride + 0 * image->pixelSize;        
    dstPtr = (OWFuint8*) image->data;
    dstPtr += (image->height-1) * image->stride + 0 * image->pixelSize;
    
    memcpy(dstPtr, srcPtr, copyStride);
    
    /* left side replication */
    for (y = 0; y < image->height; y++)
    {
        OWF_Image_SetPixel(image, 0, y, OWF_Image_GetPixelPtr(image, 1, y));
    }
    
    /* right side replication */
    for (y = 0; y < image->height; y++)
    {
        OWF_Image_SetPixel(image, image->width-1, y, OWF_Image_GetPixelPtr(image,
                                                              image->width-2,
                                                              y));
    }   
    
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_SourceFormatConversion(OWF_IMAGE* dst, OWF_IMAGE* src)
{
    OWFint      countY, widthDiff, heightDiff;
    void*       srcLinePtr;
    OWFpixel*   dstLinePtr;
    OWFboolean  replicateEdges = OWF_FALSE;
#ifndef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
    OWFint      widthBytes;
#endif

    DPRINT(("OWF_Image_SourceFormatConversion"));

    OWF_ASSERT(dst != 0 && dst->data != NULL);
    OWF_ASSERT(src != 0 && src->data != NULL);
    OWF_ASSERT(dst->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);
    OWF_ASSERT(dst->stride == OWF_Image_GetStride(dst->width, &dst->format, 0));
    
    srcLinePtr = src->data;
    dstLinePtr = (OWFpixel*) dst->data;    
    
    /* dst image must either be the same size as the src image or 2 pixels 
       bigger (enough space to perform edge replication) */
    if (dst->width != src->width || dst->height != src->height)
    {
        widthDiff = dst->width - src->width;
        heightDiff = dst->height - src->height;
        
        if (widthDiff == 2 && heightDiff == 2)
        {
            replicateEdges = OWF_TRUE;   
            /* start of the destination buffer should have a 1 pixel offset */
            dstLinePtr = (OWFpixel*) dst->data + 1 * dst->width + 1;
        }
        else
        {
            return OWF_FALSE;
        }
    }

    if (dst->format.pixelFormat != OWF_IMAGE_ARGB_INTERNAL)
    {
        return OWF_FALSE;
    }

#ifndef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
    widthBytes = OWF_Image_GetStride(src->width, &src->format, 0);
#endif

    for (countY = src->height; countY; countY--)
    {
        OWFint count = src->width;
        OWFpixel* dstPtr = dstLinePtr;
    
        switch (src->format.pixelFormat)
        {
            case OWF_IMAGE_ARGB8888:
            {
#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
                OWFuint32* srcPtr = (OWFuint32*) srcLinePtr;
    
                while (count > 0)
                {
                    dstPtr->color.alpha = (OWFsubpixel)
                        OWF_ALPHA_MAX_VALUE * ((*srcPtr & ARGB8888_ALPHA_MASK) >> ARGB8888_ALPHA_SHIFT) / OWF_BYTE_MAX_VALUE;
                    dstPtr->color.red   = (OWFsubpixel)
                        OWF_RED_MAX_VALUE * ((*srcPtr & ARGB8888_RED_MASK) >> ARGB8888_RED_SHIFT) / OWF_BYTE_MAX_VALUE;
                    dstPtr->color.green = (OWFsubpixel)
                        OWF_GREEN_MAX_VALUE * ((*srcPtr & ARGB8888_GREEN_MASK)>> ARGB8888_GREEN_SHIFT) / OWF_BYTE_MAX_VALUE;
                    dstPtr->color.blue  = (OWFsubpixel)
                        OWF_BLUE_MAX_VALUE * ((*srcPtr & ARGB8888_BLUE_MASK) >> ARGB8888_BLUE_SHIFT) / OWF_BYTE_MAX_VALUE;
                    dstPtr ++;
                    srcPtr ++;
                    count--;
                }
#else
                memcpy(dstLinePtr, srcLinePtr, widthBytes);
#endif
                break;
            }
    
            case OWF_IMAGE_XRGB8888:
            {
                OWFuint32* srcPtr = (OWFuint32*) srcLinePtr;
                
                while (count > 0)
                {
#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
                    dstPtr->color.alpha = OWF_FULLY_OPAQUE;
                    dstPtr->color.red   = (OWFsubpixel)
                        OWF_RED_MAX_VALUE * ((*srcPtr & ARGB8888_RED_MASK) >> ARGB8888_RED_SHIFT) / OWF_BYTE_MAX_VALUE;
                    dstPtr->color.green = (OWFsubpixel)
                        OWF_GREEN_MAX_VALUE * ((*srcPtr & ARGB8888_GREEN_MASK) >> ARGB8888_GREEN_SHIFT) / OWF_BYTE_MAX_VALUE;
                    dstPtr->color.blue  = (OWFsubpixel)
                        OWF_BLUE_MAX_VALUE * ((*srcPtr & ARGB8888_BLUE_MASK) >> ARGB8888_BLUE_SHIFT) / OWF_BYTE_MAX_VALUE;
#else
                    *(OWFuint32*)dstPtr = *srcPtr | ARGB8888_ALPHA_MASK;
#endif
                    dstPtr ++;
                    srcPtr ++;
                    count--;
                }
                break;
            }
    
            case OWF_IMAGE_RGB565:
            {
                OWFfloat tmp;
                OWFuint16* srcPtr = (OWFuint16*) srcLinePtr;
                
                while (count > 0)
                {
                    /*
                     * Formula for converting channel value is:
                     * Each channel is multiplied by (2^d - 1)/(2^s -1)
                     * where d is dest channel bits and s is source channel bits.
                     */
                    dstPtr->color.alpha = (OWFsubpixel) OWF_FULLY_OPAQUE;
    
                    tmp = (OWFfloat)((*srcPtr & RGB565_RED_MASK) >> RGB565_RED_SHIFT);
                    dstPtr->color.red   = (OWFsubpixel)OWF_RED_MAX_VALUE * tmp / 31.0f + OWF_SOURCE_CONVERSION_ROUNDING_VALUE;
    
                    tmp = (OWFfloat)((*srcPtr & RGB565_GREEN_MASK) >> RGB565_GREEN_SHIFT);
                    dstPtr->color.green = (OWFsubpixel)OWF_GREEN_MAX_VALUE * tmp / 63.0f + OWF_SOURCE_CONVERSION_ROUNDING_VALUE;
    
                    tmp = (OWFfloat)(*srcPtr & RGB565_BLUE_MASK);
                    dstPtr->color.blue  = (OWFsubpixel)OWF_BLUE_MAX_VALUE * tmp / 31.0f + OWF_SOURCE_CONVERSION_ROUNDING_VALUE;
    
                    dstPtr ++;
                    srcPtr ++;
                    count--;
                }
                break;
            }
    
            default:
            {
                return OWF_FALSE; /* source format not supported */
            }
        }
        
        dstLinePtr+=dst->width;
        srcLinePtr=(OWFuint8*)srcLinePtr+src->stride;
    }
    
    if (replicateEdges)
    {
        OWF_Image_EdgeReplication(dst);
    }

    return OWF_TRUE;
}

/*----------------------------------------------------------------------------*/
OWF_PUBLIC OWFint
OWF_Image_GetStride(OWFint width, const OWF_IMAGE_FORMAT* format, OWFint minimumStride)
{
    OWFint                  size;
    OWFint                  pixelSize;

    DPRINT(("OWF_Image_GetStride"));

    OWF_ASSERT(format);

    pixelSize = OWF_Image_GetFormatPixelSize(format->pixelFormat);

    if (pixelSize < 0)
    {
        /* negative pixelSize means that pixel size is a fraction
         * (1/-pixelSize) of a byte, e.g. -8 means pixel has size
         * of one bit. */

        size = (width + 1) / -pixelSize;
    }
    else
    {
        size = width * pixelSize;
    }
    if (size<minimumStride)
    {
        size=minimumStride;
    }
    if (format->rowPadding)
    {
        size += format->rowPadding - 1;
        size -= size % format->rowPadding;
    }
    return size;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_IsValidDestinationFormat(OWF_IMAGE_FORMAT* format)
{
    DPRINT(("OWF_Image_IsValidDestinationFormat"));

    switch (format->pixelFormat)
    {
        case OWF_IMAGE_ARGB8888:
        {
            return OWF_TRUE;
        }
        case OWF_IMAGE_XRGB8888:
        {
            return OWF_TRUE;
        }
#ifdef SUPPORT_565_OUTPUT
        case OWF_IMAGE_RGB565:
        {
            return OWF_TRUE;
        }
#endif
        default:
        {
            return OWF_FALSE; /* destination format not supported */
        }
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_DestinationFormatConversion(OWF_IMAGE* dst, OWF_IMAGE* src)
{
    OWFint                  countY;
    OWFuint32*              dstPtr;
    OWFpixel*               srcPtr;
#ifndef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
    OWFint                  widthBytes;
#endif

    DPRINT(("OWF_Image_DestinationFormatConversion"));

    OWF_ASSERT(dst != 0 && dst->data != NULL);
    OWF_ASSERT(src != 0 && src->data != NULL);
    OWF_ASSERT(src->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);
    /* Note: src->stride cannot be relied upon to be correct. Assume lines are contiguous. */

    if (src->format.pixelFormat != OWF_IMAGE_ARGB_INTERNAL)
    {
        return OWF_FALSE;
    }

    /* src and data must have equal amount of pixels */
    if (dst->width != src->width || dst->height != src->height)
    {
        return OWF_FALSE;
    }

    dstPtr = (OWFuint32*) dst->data;
    srcPtr = (OWFpixel*) src->data;

    if (dst->format.premultiplied && !src->format.premultiplied)
    {
        OWF_Image_PremultiplyAlpha(src);
    }
    else if (!dst->format.premultiplied && src->format.premultiplied)
    {
        OWF_Image_UnpremultiplyAlpha(src);
    }

#ifndef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
    widthBytes = OWF_Image_GetStride(src->width, &src->format, 0);
#endif

    for (countY = 0; countY < src->height; countY++)
    {   
        OWFuint8* destination = (OWFuint8*) dst->data;
        destination += countY*dst->stride;
        dstPtr = (OWFuint32*) destination;    
    
        switch (dst->format.pixelFormat)
        {
            case OWF_IMAGE_ARGB8888:
            {
#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
                OWFint countX;
                OWFuint32 dstPixel = 0;               
                
                for (countX = 0; countX < src->width; countX++)
                {
                    dstPixel  = ((OWFuint8)(roundSubPixel(OWF_BYTE_MAX_VALUE * srcPtr->color.alpha / OWF_ALPHA_MAX_VALUE)) <<
                                ARGB8888_ALPHA_SHIFT);
                    dstPixel |= ((OWFuint8)(roundSubPixel(OWF_BYTE_MAX_VALUE * srcPtr->color.red / OWF_RED_MAX_VALUE))   <<
                                ARGB8888_RED_SHIFT);
                    dstPixel |= ((OWFuint8)(roundSubPixel(OWF_BYTE_MAX_VALUE * srcPtr->color.green / OWF_GREEN_MAX_VALUE)) <<
                                ARGB8888_GREEN_SHIFT);
                    dstPixel |= ((OWFuint8)(roundSubPixel(OWF_BYTE_MAX_VALUE * srcPtr->color.blue / OWF_BLUE_MAX_VALUE))  <<
                                ARGB8888_BLUE_SHIFT);
                    *dstPtr = dstPixel;
                    dstPtr ++;
                    srcPtr ++;
                }
#else
                memcpy(dstPtr, srcPtr, widthBytes);
                srcPtr = (OWFpixel*)((OWFuint8*)srcPtr + widthBytes);
#endif
                break;
            }
    
            case OWF_IMAGE_XRGB8888:
            {
                OWFint countX;
                OWFuint32 dstPixel = 0;
                
                for (countX = 0; countX < src->width; countX++)
                {
#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
                    dstPixel  = ARGB8888_ALPHA_MASK;
                    dstPixel |= ((OWFuint8)(roundSubPixel(OWF_BYTE_MAX_VALUE * srcPtr->color.red / OWF_RED_MAX_VALUE))   <<
                                ARGB8888_RED_SHIFT);
                    dstPixel |= ((OWFuint8)(roundSubPixel(OWF_BYTE_MAX_VALUE * srcPtr->color.green / OWF_GREEN_MAX_VALUE)) <<
                                ARGB8888_GREEN_SHIFT);
                    dstPixel |= ((OWFuint8)(roundSubPixel(OWF_BYTE_MAX_VALUE * srcPtr->color.blue / OWF_BLUE_MAX_VALUE))  <<
                                ARGB8888_BLUE_SHIFT);
#else
                    dstPixel = *(OWFuint32*)srcPtr | ARGB8888_ALPHA_MASK;
#endif
                    *dstPtr = dstPixel;
                    dstPtr ++;
                    srcPtr ++;
                }
                break;
            }
    
            default:
            {
                return OWF_FALSE; /* destination format not supported */
            }
        }
    }

    return OWF_TRUE;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Init(OWF_IMAGE* image)
{
    DPRINT(("OWF_Image_Init"));

    OWF_ASSERT(NULL != image);
    memset(image, 0, sizeof(OWF_IMAGE));
}

/*----------------------------------------------------------------------------*/
OWF_PUBLIC OWF_IMAGE*
OWF_Image_Create(OWFint width,
                 OWFint height,
                 const OWF_IMAGE_FORMAT* format,
                 void* buffer,
                 OWFint minimumStride
                 )
{
    OWF_IMAGE*      image;

    DPRINT(("OWF_Image_Create"));

    image = CREATE(OWF_IMAGE);

    if (image)
    {

        image->format.pixelFormat   = format->pixelFormat;
        image->format.linear        = format->linear;
        image->format.premultiplied = format->premultiplied;
        image->format.rowPadding    = format->rowPadding;

        image->pixelSize    = OWF_Image_GetFormatPixelSize(format->pixelFormat);
        image->width        = width;
        image->height       = height;

        /* stride is row length in bytes (aligned according to image format) */
        image->stride = OWF_Image_GetStride(width, &image->format, minimumStride);
        image->foreign  = (buffer) ? OWF_TRUE : OWF_FALSE;
        image->dataMax = image->stride * height;

        DPRINT(("OWF_Image_Create:"));
        DPRINT(("  Pixel format     = %x", format->pixelFormat));
        DPRINT(("  Linear           = %d", format->linear));
        DPRINT(("  Premultiplied    = %d", format->premultiplied));
        DPRINT(("  Row alignment    = %d bytes", format->rowPadding));
        DPRINT(("  Pixel size       = %d", image->pixelSize));
        DPRINT(("  Width            = %d", image->width));
        DPRINT(("  Height           = %d", image->height));
        DPRINT(("  Stride           = %d", image->stride));
        DPRINT(("  Foreign data     = %d", image->foreign));
        DPRINT(("  Data size        = %d", image->dataMax));
        if (image->dataMax > 0)
        {
            image->data = (image->foreign) ? (buffer) :
                OWF_Image_AllocData(0, width, height, format->pixelFormat);
        }
        else
        {
            /* either overflow occured or width/height is zero */
            if (width && height)
            {
                DPRINT(("OWF_Image_Create: Overflow in image size calculation."));
            }
            else
            {
                DPRINT(("OWF_Image_Create: width or height is zero."));
            }
        }
    }

    if (!(image && image->data && (image->dataMax > 0)))
    {
        DPRINT(("OWF_Image_Create: Image creation failed (image = %p, "
                "data = %p, dataSize = %d)",
                image, image ? (void*)(image->data) : (void*)NULL, image?image->dataMax:-1));
        DESTROY(image);
        return NULL;
    }
    return image;
}

/*----------------------------------------------------------------------------*/
OWF_PUBLIC void OWF_Image_Destroy(OWF_IMAGE* image)
{
    if (image)
        {
        DESTROY(image);
        }
}


/*----------------------------------------------------------------------------*/
OWF_API_CALL OWF_IMAGE*
OWF_Image_Copy(const OWF_IMAGE* image)
{
    OWF_IMAGE* newImage = NULL;

    DPRINT(("OWF_Image_Copy"));

    OWF_ASSERT(image);

    newImage = CREATE(OWF_IMAGE);
    if (newImage)
    {
        memcpy(newImage, image, sizeof(*newImage));
        if (!image->foreign)
        {
            newImage->data = xalloc(1, image->dataMax);
            if (newImage->data)
            {
                memcpy(newImage->data,
                       image->data,
                       image->height*image->stride);
            }
        }
    }

    return newImage;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_SetSize(OWF_IMAGE* image,
                  OWFint width,
                  OWFint height)
{
    OWFint                  size;
    OWFint                  stride;

    DPRINT(("OWF_Image_SetSize"));

    OWF_ASSERT(image);
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL ||
            image->format.pixelFormat==OWF_IMAGE_L32);
    
    /** note that this setsize ignores any specialised stride **/
    stride = OWF_Image_GetStride(width, &image->format, 0);
    size = height * stride;

    /* change source size if buffer have enough space */
    if (size > 0 && size <= image->dataMax)
    {
        image->height = height;
        image->width  = width;
        image->stride = stride;
        return OWF_TRUE;
    }
    return OWF_FALSE;
}
/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_SetFlags(OWF_IMAGE* image,
                  OWFboolean premultiply,
                  OWFboolean linear)
    {
    DPRINT(("OWF_Image_SetFlags"));

    OWF_ASSERT(image);
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL ||
            image->format.pixelFormat==OWF_IMAGE_L32);
    image->format.linear=linear;
    image->format.premultiplied=premultiply;
    }

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_SetPixelBuffer(OWF_IMAGE* image,      void* buffer)
    {
    DPRINT(("OWF_Image_SetPixelBuffer"));

    OWF_ASSERT(image);
    OWF_ASSERT(buffer);
    OWF_ASSERT(image->foreign);
    if (image->foreign)
        {
        image->data=buffer;
        }
    }
/*----------------------------------------------------------------------------*/
/* NEVER USED
OWF_API_CALL OWFboolean
OWF_Image_SetPixelData(OWF_IMAGE* image,
                       OWFint width,
                       OWFint height,
                       const OWF_IMAGE_FORMAT* format,
                       void* buffer)
{
    OWFint                  size = 0,
                            stride = 0;

    OWF_ASSERT(image && format);

    stride  = OWF_Image_GetStride(width, format);
    size    = height * stride;

    if (size <= 0)
    {
        return OWF_FALSE; 
    }

    if (!image->foreign)
    {
        OWF_Image_FreeData(0, &image->data);
    }

    image->format.pixelFormat   = format->pixelFormat;
    image->format.linear        = format->linear;
    image->format.premultiplied = format->premultiplied;
    image->format.rowPadding    = format->rowPadding;

    image->pixelSize    = OWF_Image_GetFormatPixelSize(format->pixelFormat);
    image->width        = width;
    image->height       = height;
    image->stride       = stride;
    image->foreign      = OWF_TRUE;
    image->dataMax      = size;
    image->data         = buffer;

    return OWF_TRUE;
}
*/
/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_Blit(OWF_IMAGE* dst,
               OWF_RECTANGLE const* dstRect,
               OWF_IMAGE const* src,
               OWF_RECTANGLE const* srcRect)
{
    OWF_RECTANGLE           bounds, rect, drect, srect;
    OWFint                  lineCount   = 0;
    OWFint                  copyStride  = 0;
    OWFuint8*               srcPtr      = NULL;
    OWFuint8*               dstPtr      = NULL;

    DPRINT(("OWF_Image_Blit"));

    OWF_ASSERT(dst != 0 && dst->data != NULL);
    OWF_ASSERT(src != 0 && src->data != NULL);
    OWF_ASSERT(dst->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);
    OWF_ASSERT(src->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    OWF_Rect_Set(&bounds, 0, 0, dst->width, dst->height);
    OWF_Rect_Set(&rect,
                 dstRect->x, dstRect->y,
                 srcRect->width, srcRect->height);
    OWF_Rect_Set(&srect,
                 srcRect->x, srcRect->y,
                 srcRect->width, srcRect->height);

    /* clip destination rectangle against bounds */
    if (!OWF_Rect_Clip(&drect, &rect, &bounds))
    {
        return OWF_FALSE;
    }

    if (src->pixelSize != dst->pixelSize)
    {
        DPRINT(("OWF_Image_Blit(): pixels sizes differ\n"));
        return OWF_FALSE;
    }

    lineCount = srect.height;
    copyStride = srect.width * src->pixelSize;

    /* use bytepointers in copy  - generic */
    srcPtr = (OWFuint8*) src->data;
    srcPtr += srect.y * src->stride + srect.x * src->pixelSize;
    dstPtr = (OWFuint8*)dst->data;
    dstPtr += drect.y * dst->stride + drect.x * src->pixelSize;

    while (lineCount > 0)
    {
        --lineCount;

        memcpy(dstPtr, srcPtr, copyStride);

        srcPtr += src->stride;
        dstPtr += dst->stride;
    }

    return OWF_TRUE;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFpixel*
OWF_Image_GetPixelPtr(OWF_IMAGE* image,
                      OWFint x,
                      OWFint y)
{
    OWF_ASSERT(image && image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);
    if (!(image && image->data))
    {
        return 0;
    }

    x = CLAMP(x, 0, image->width-1);
    y = CLAMP(y, 0, image->height-1);

    return (OWFpixel*)image->data + y * image->width  + x ;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_GetPixel(OWF_IMAGE* image,
                   OWFint x,
                   OWFint y,
                   OWFpixel* pixel)
{
    OWFpixel*               temp = NULL;

    OWF_ASSERT(pixel);

    if (!(image && image->data))
    {
        return;
    }
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    pixel->color.alpha = 0;
    pixel->color.red = 0;
    pixel->color.green = 0;
    pixel->color.blue = 0;

    if (x < 0 || y < 0 || x >= image->width || y >= image->height)
    {
        return;
    }

    temp = (OWFpixel*)image->data + y * image->width  + x;

    pixel->color.alpha = temp->color.alpha;
    pixel->color.red = temp->color.red;
    pixel->color.green = temp->color.green;
    pixel->color.blue = temp->color.blue;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_SetPixel(OWF_IMAGE* image,
                   OWFint x,
                   OWFint y,
                   OWFpixel const* pixel)
{
    OWFpixel*               data = NULL;

    if (!(image && image->data))
    {
        return;
    }
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    if(x < 0 || y < 0 || x >= image->width || y >= image->height)
    {
        return;
    }


    data = (OWFpixel*)image->data + y * image->width + x;

    data->color.red = pixel->color.red;
    data->color.green = pixel->color.green;
    data->color.blue = pixel->color.blue;
    data->color.alpha = pixel->color.alpha;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_PointSamplingStretchBlit(OWF_IMAGE* dst,
                                   OWF_RECTANGLE* dstRect,
                                   OWF_IMAGE* src,
                                   OWFfloat* srcRect)
{
    OWFint                  ox = 0, oy = 0;
    OWFint                  x, y;

    DPRINT(("OWF_Image_PointSamplingStretchBlit"));

    /* images must be valid */
    if (!((src != NULL) && (src->data != NULL) &&
          (dst != NULL) && (dst->data != NULL)))
    {
        return OWF_FALSE;
    }
    OWF_ASSERT(src->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);
    OWF_ASSERT(dst->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    /* ditto with rectangles, too */
    if (!((dstRect != NULL) && (dstRect->width && dstRect->height) &&
          (srcRect != NULL) && (srcRect[2] && srcRect[3])))
    {
        return OWF_FALSE;
    }

    /* Note: bounds check missing */

    if (src->pixelSize != dst->pixelSize)
    {
        return OWF_FALSE;
    }

#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
    {
        OWFfloat                dx = 0.f, dy = 0.f; 
    
        /* solve scaling ratios for image */
        dx = (OWFfloat) srcRect[2] / (OWFfloat) dstRect->width;
        dy = (OWFfloat) srcRect[3] / (OWFfloat) dstRect->height;
    
        for (y = 0; y < dstRect->height; y++)
        {
            for (x = 0; x < dstRect->width; x++) 
            {
                OWFpixel*       pixel;         
                
                /* NOTE This code uses pixel center points to calculate distances
                        and factors. Results can differ slightly when pixel corner
                        coordinates are used */            
                
                /* coordinates of nearest pixel in original image */
                ox = (int) floor((((OWFfloat) x + 0.5) * dx) + srcRect[0]);
                oy = (int) floor((((OWFfloat) y + 0.5) * dy) + srcRect[1]);     
                
                pixel = OWF_Image_GetPixelPtr(src,
                                              ox,
                                              oy);
                
                OWF_Image_SetPixel(dst,
                                   dstRect->x + x,
                                   dstRect->y + y,
                                   pixel);
                
            }
        }
    }
#else
    if (srcRect[0] < 0 || (srcRect[0] + srcRect[2]) > src->width ||
            srcRect[1] < 0 || (srcRect[1] + srcRect[3]) > src->height)
        {
        /* Source rectangle out of bounds */
        return OWF_FALSE;
        }

    if (dstRect->x < 0 || (dstRect->x + dstRect->width) > dst->width ||
            dstRect->y < 0 || (dstRect->y + dstRect->height) > dst->height)
        {
        /* Destination rectangle out of bounds */
        return OWF_FALSE;
        }

    {
        OWFint dxFix, dyFix;
        OWFint xFixStart, yFix;
        OWFuint32 *dstPtr, *srcLinePtr;
    
/* Integer <-> 16.16 fixed point conversion macros */
#define INT_TO_FIXED_PT(X) ((X) << 16)
#define FIXED_PT_TO_INT(X) ((X) >> 16)
    
        /* Calculate scaling factors in fixed point (with rounding). */
        dxFix = (OWFint)((srcRect[2] * INT_TO_FIXED_PT(1) + (dstRect->width >> 1)) / dstRect->width);
        dyFix = (OWFint)((srcRect[3] * INT_TO_FIXED_PT(1) + (dstRect->height >> 1)) / dstRect->height);
    
        /* Calculate fixed point location in source, with half-pixel offset */
        xFixStart = (OWFint)(srcRect[0] * INT_TO_FIXED_PT(1) + (dxFix >> 1));
        yFix = (OWFint)(srcRect[1] * INT_TO_FIXED_PT(1) + (dyFix >> 1));
    
        /* Initial target address. */
        dstPtr = (OWFuint32*)dst->data + dstRect->y * dst->width + dstRect->x;
    
        for (y = 0; y < dstRect->height; y++)
        {
            OWFint xFix = xFixStart;
    
            oy = FIXED_PT_TO_INT(yFix);
            srcLinePtr = (OWFuint32*)src->data + oy * src->width;
    
            for (x = 0; x < dstRect->width; x++) 
            {
                ox = FIXED_PT_TO_INT(xFix);
                dstPtr[x] = srcLinePtr[ox];
                xFix += dxFix;
            }
    
            dstPtr += dst->width;
            yFix += dyFix;
        }
    }
#endif
    return OWF_TRUE;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_BilinearStretchBlit(OWF_IMAGE* dst,
                              OWF_RECTANGLE* dstRect,
                              OWF_IMAGE* src,
                              OWFfloat* srcRect)
{
    OWFint                  x = 0, y = 0;
    OWFint                  ox = 0, oy = 0;
    OWFfloat                dx = 0.f, dy = 0.f, wx = 0.f, wy = 0.f;
    OWFfloat                w[2 * 2];
    OWFpixel*               sample[4];
    OWFpixel*               pixel = NULL;

    DPRINT(("OWF_Image_BilinearStretchBlit"));

    /* images must be valid */
    if (!((src != NULL) && (src->data != NULL) &&
          (dst != NULL) && (dst->data != NULL)))
    {
        return OWF_FALSE;
    }
    OWF_ASSERT(src->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);
    OWF_ASSERT(dst->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    /* ditto with rectangles, too */
    if (!((dstRect != NULL) && (dstRect->width && dstRect->height) &&
          (srcRect != NULL) && (srcRect[2] && srcRect[3])))
    {
        return OWF_FALSE;
    }

    if (src->pixelSize != dst->pixelSize)
    {
        return OWF_FALSE;
    }

    /* solve scaling ratios for image */
    dx = (OWFfloat) srcRect[2] / (OWFfloat) dstRect->width;
    dy = (OWFfloat) srcRect[3] / (OWFfloat) dstRect->height;

    for (y = 0; y < dstRect->height; y++)
    {
        for (x = 0; x < dstRect->width; x++)
        {
            OWFfloat tempOx, tempOy;            
            
            /* NOTE This code uses pixel center points to calculate distances
                    and factors. Results can differ slightly when pixel corner
                    coordinates are used */

            /* coordinates of nearest pixel in original image */
            tempOx = (((OWFfloat) x + 0.5) * dx) + srcRect[0];
            tempOy = (((OWFfloat) y + 0.5) * dy) + srcRect[1];            
            
            ox = (int) floor((((OWFfloat) x + 0.5) * dx) + srcRect[0]);
            oy = (int) floor((((OWFfloat) y + 0.5) * dy) + srcRect[1]);    

            /* Distances to nearest pixel, eg. fractional part of coordinate */    
            wx = (OWFfloat) ox + 0.5 - tempOx;
            wy = (OWFfloat) oy + 0.5 - tempOy;

            /* If distance is positive, we should use left or upper pixel for
             * second nearest pixel. */
            if (wx > 0.0)
            {
                ox--;
                wx = 1.0 - wx;
            }
            else
            {
                wx = -wx; /* abs */
            }

            if (wy > 0.0)
            {
                oy--;
                wy = 1.0 - wy;
            }
            else
            {
                wy = -wy;
            }
            
            /* Calculate weights for samples */
            w[0] = (1.0 - wx) * (1.0 - wy);
            w[1] = wx * (1.0 - wy);
            w[2] = (1.0 - wx) * wy;
            w[3] = wx * wy;

            /* get sample */
            sample[0] = OWF_Image_GetPixelPtr(src, ox + 0, oy + 0);
            sample[1] = OWF_Image_GetPixelPtr(src, ox + 1, oy + 0);
            sample[2] = OWF_Image_GetPixelPtr(src, ox + 0, oy + 1);
            sample[3] = OWF_Image_GetPixelPtr(src, ox + 1, oy + 1);

            
            /* get result pixel */
            pixel = OWF_Image_GetPixelPtr(dst, x, y);

            /* calculate final color */
            pixel->color.red   =
                sample[0]->color.red * w[0] + sample[1]->color.red * w[1] +
                sample[2]->color.red * w[2] + sample[3]->color.red * w[3] + OWF_BILINEAR_ROUNDING_VALUE;

            pixel->color.green =
                sample[0]->color.green * w[0] + sample[1]->color.green * w[1] +
                sample[2]->color.green * w[2] + sample[3]->color.green * w[3] + OWF_BILINEAR_ROUNDING_VALUE;

            pixel->color.blue  =
                sample[0]->color.blue * w[0] + sample[1]->color.blue * w[1] +
                sample[2]->color.blue * w[2] + sample[3]->color.blue * w[3] + OWF_BILINEAR_ROUNDING_VALUE;

            pixel->color.alpha =
                sample[0]->color.alpha * w[0] + sample[1]->color.alpha * w[1] +
                sample[2]->color.alpha * w[2] + sample[3]->color.alpha * w[3] + OWF_BILINEAR_ROUNDING_VALUE;
        }
    }
    return OWF_TRUE;

}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_Stretch(OWF_IMAGE* dst,
                  OWF_RECTANGLE* dstRect,
                  OWF_IMAGE* src,
                  OWFfloat* srcRect,
                  OWF_FILTERING filter)
{
    OWFboolean          result = OWF_FALSE;

    switch (filter)
    {
        case OWF_FILTER_POINT_SAMPLING:
        {
            result = OWF_Image_PointSamplingStretchBlit(dst, dstRect, src,
                                                        srcRect);
            break;
        }
        case OWF_FILTER_BILINEAR:
        {
            result = OWF_Image_BilinearStretchBlit(dst, dstRect, src, srcRect);
            break;
        }
    }

    return result;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Clear(OWF_IMAGE* image,
                OWFsubpixel red,
                OWFsubpixel green,
                OWFsubpixel blue,
                OWFsubpixel alpha)
{
    OWFint                  i, numPixels;
    OWFpixel*                pixels;

    DPRINT(("OWF_Image_Clear"));

    OWF_ASSERT(image != 0);
    OWF_ASSERT(image->data != 0);
    OWF_ASSERT(image->format.pixelFormat == OWF_IMAGE_ARGB_INTERNAL);
    /* Note: image->stride cannot be relied upon to be correct. Assume lines are contiguous. */

    numPixels = image->width * image->height;
    pixels = (OWFpixel*) image->data;

#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
    for (i = 0; i < numPixels; i++)
    {
        pixels[i].color.red   = (OWFsubpixel) red;
        pixels[i].color.green = (OWFsubpixel) green;
        pixels[i].color.blue  = (OWFsubpixel) blue;
        pixels[i].color.alpha = (OWFsubpixel) alpha;
    }
#else
    if (alpha == red && alpha == green && alpha == blue)
        {
        /* If all four bytes are the same, just use memset */
        OWFuint32 clearBytes = numPixels * sizeof(OWFuint32);

        memset(pixels, alpha, clearBytes);
        }
    else
        {
        /* Otherwise assign each pixel the packed value */
        OWFuint32* pixelPtr = (OWFuint32*)pixels;
        OWFuint32 pixel;
    
        pixel = (alpha << ARGB8888_ALPHA_SHIFT) | (red << ARGB8888_RED_SHIFT) |
                (green << ARGB8888_GREEN_SHIFT) | (blue << ARGB8888_BLUE_SHIFT);

        for (i = 0; i < numPixels; i++)
            {
            pixelPtr[i] = pixel;
            }
        }
#endif
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_PremultiplyAlpha(OWF_IMAGE* image)
{
    OWFint       x, y;

    DPRINT(("OWF_Image_PremultiplyAlpha"));

    OWF_ASSERT(image != 0);

    if (image->format.premultiplied)
    {
        return;
    }

    /* only for internal format */
    if (image->format.pixelFormat != OWF_IMAGE_ARGB_INTERNAL)
    {
        return;
    }

    for (y = 0; y < image->height; y++)
    {
        for (x = 0; x < image->width; x++)
        {
            OWFpixel* pixel;
            OWFsubpixel alpha;

            pixel = OWF_Image_GetPixelPtr(image, x, y);

            alpha = pixel->color.alpha;

            if (0 == alpha)
            {
                pixel->color.red =
                pixel->color.green =
                pixel->color.blue = 0;
            }
            else
            {
                pixel->color.red    = (pixel->color.red * alpha + OWF_PREMUL_ROUNDING_FACTOR ) /
                                      OWF_ALPHA_MAX_VALUE;
                pixel->color.green  = (pixel->color.green * alpha + OWF_PREMUL_ROUNDING_FACTOR ) /
                                      OWF_ALPHA_MAX_VALUE;
                pixel->color.blue   = (pixel->color.blue * alpha + OWF_PREMUL_ROUNDING_FACTOR ) /
                                      OWF_ALPHA_MAX_VALUE;
            }
        }
    }

    image->format.premultiplied = OWF_TRUE;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_UnpremultiplyAlpha(OWF_IMAGE* image)
{
    OWFint       count;
    OWFpixel*    pixelPtr;

    DPRINT(("OWF_Image_UnpremultiplyAlpha"));

    OWF_ASSERT(image != 0);

    if (!image->format.premultiplied)
    {
        return;
    }

    /* only for internal format */
    if (image->format.pixelFormat != OWF_IMAGE_ARGB_INTERNAL)
    {
        return;
    }

    count = image->width * image->height;
    pixelPtr = (OWFpixel*)image->data;

    while (count > 0)
    {

        OWFsubpixel a = pixelPtr->color.alpha;

        #ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
			OWF_ASSERT(a <= OWF_ALPHA_MAX_VALUE && a >= OWF_ALPHA_MIN_VALUE);
		#endif

        if (a > OWF_ALPHA_MIN_VALUE)
        {
            OWFsubpixel r = pixelPtr->color.red   * OWF_RED_MAX_VALUE / a;
            OWFsubpixel g = pixelPtr->color.green * OWF_GREEN_MAX_VALUE / a;
            OWFsubpixel b = pixelPtr->color.blue  * OWF_BLUE_MAX_VALUE / a;

            pixelPtr->color.red   = r;
            pixelPtr->color.green = g;
            pixelPtr->color.blue  = b;

            --count;
            pixelPtr++;
        }
    }

    image->format.premultiplied = OWF_TRUE;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Rotate(OWF_IMAGE* dst,
                 OWF_IMAGE* src,
                 OWF_ROTATION rotation)
{
    OWFint                  ox = 0, oy = 0,
                            w = 0, h = 0,
                            x = 0, y = 0;
    OWFint                  xx = 0, xy = 0,
                            yx = 0, yy = 0;

    DPRINT(("OWF_Image_Rotate"));

    OWF_ASSERT(src && src->data);
    OWF_ASSERT(dst && dst->data);
    OWF_ASSERT(src->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);
    OWF_ASSERT(dst->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    w = src->width;
    h = src->height;

    switch (rotation)
    {
        case OWF_ROTATION_0:
        {
            /*
             * origin: (0, 0)
             * x-axis: (1, 0)
             * y-axis: (0, 1)
             */
            ox = 0;
            oy = 0;
            xx = 1;
            xy = 0;
            yx = 0;
            yy = 1;
            break;
        }
        case OWF_ROTATION_90:
        {
            /*
             * origin: (height-1, 0)
             * x-axis: (0, 1)
             * y-axis: (-1, 0)
             */
            ox = h - 1;
            oy = 0;
            xx = 0;
            xy = 1;
            yx = -1;
            yy = 0;
            break;
        }
        case OWF_ROTATION_180:
        {
            /*
             * origin: (width-1, height-1)
             * x-axis: (-1, 0)
             * y-axis: (0, -1)
             */
            ox = w - 1;
            oy = h - 1;
            xx = -1;
            xy = 0;
            yx = 0;
            yy = -1;
            break;
        }
        case OWF_ROTATION_270:
        {
            /*
             * origin: (0, height-1)
             * x-axis: (0, -1)
             * y-axis: (1, 0)
             */
            ox = 0;
            oy = w - 1;
            xx = 0;
            xy = -1;
            yx = 1;
            yy = 0;
            break;

        }
    }

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
        /*
         * O = [ox oy]' X = [xx xy]' Y = [yx yy]'
         *
         * p_dst(x_src,y_src) = O + x_src*X + y_src*Y
         */
        OWF_Image_SetPixel(dst,
                           ox + x * xx + y * yx,
                           oy + x * xy + y * yy,
                           OWF_Image_GetPixelPtr(src, x, y));
        }
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_Flip(OWF_IMAGE* image,
               OWF_FLIP_DIRECTION dir)
{
    OWFint                  x, y;

    DPRINT(("OWF_Image_Flip"));

    if (!image)
    {
        return;
    }
    OWF_ASSERT(image->format.pixelFormat==OWF_IMAGE_ARGB_INTERNAL);

    if (dir & OWF_FLIP_VERTICALLY)
    {
        OWFint h = image->height/2;

        for (y = 0; y < h; y++)
        {
            for (x = 0; x < image->width; x++)
            {
                OWFpixel        pix;

                OWF_Image_GetPixel(image, x, y, &pix);
                OWF_Image_SetPixel(image, x, y,
                                   OWF_Image_GetPixelPtr(image,
                                                         x,
                                                         image->height - 1 - y
                                                         ));
                OWF_Image_SetPixel(image, x, image->height - 1 - y, &pix);
            }
        }
    }

    if (dir & OWF_FLIP_HORIZONTALLY)
    {
        OWFint w = image->width/2;

        for (y = 0; y < image->height; y++)
        {
            for (x = 0; x < w; x++)
            {
                OWFpixel        pix;

                OWF_Image_GetPixel(image, x, y, &pix);
                OWF_Image_SetPixel(image, x, y,
                                   OWF_Image_GetPixelPtr(image,
                                                         image->width - 1 - x,
                                                         y
                                                         ));
                OWF_Image_SetPixel(image, image->width - 1 - x, y, &pix);
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
#define BLENDER_INNER_LOOP_BEGIN \
    OWFint rowCount = drect.height; \
    while (rowCount > 0) { \
        OWFint colCount = drect.width; \
        while (colCount > 0) {

#define BLENDER_INNER_LOOP_END \
            srcPtr ++; \
            dstPtr ++; \
            maskPtr++; \
            --colCount; \
        } \
        srcPtr += srcLineDelta; \
        dstPtr += dstLineDelta; \
        maskPtr += maskLineDelta; \
        --rowCount; \
    }
	
#define BLENDER_INNER_LOOP_END_NO_MASK \
            srcPtr ++; \
            dstPtr ++; \
            --colCount; \
        } \
        srcPtr += srcLineDelta; \
        dstPtr += dstLineDelta; \
        --rowCount; \
    }

#define SA srcPtr->color.alpha
#define SR srcPtr->color.red
#define SG srcPtr->color.green
#define SB srcPtr->color.blue

#define DA dstPtr->color.alpha
#define DR dstPtr->color.red
#define DG dstPtr->color.green
#define DB dstPtr->color.blue

#define MA *maskPtr
#define GA blend->globalAlpha

OWF_API_CALL void
OWF_Image_Blend(OWF_BLEND_INFO* blend,
                OWF_TRANSPARENCY transparency)
{
    OWF_IMAGE*              dst;
    OWF_IMAGE*              src;
    OWF_IMAGE*              mask;
    OWF_RECTANGLE*          srcRect;
    OWF_RECTANGLE*          dstRect;
    OWF_RECTANGLE           bounds, srect, drect, rect;
    OWFint                  srcLineDelta, dstLineDelta, maskLineDelta;
    OWFpixel*               srcPtr;
    OWFpixel*               dstPtr;
    OWFsubpixel*            maskPtr;
    
    
    /* preparation */
    OWF_ASSERT(blend);
    DPRINT(("OWF_Image_Blend: transparency = %d", transparency));
    /* Mask must be set if mask-transparency is used */
    OWF_ASSERT(((transparency & OWF_TRANSPARENCY_MASK) && blend->mask) ||
           !(transparency & OWF_TRANSPARENCY_MASK));

    OWF_ASSERT(blend->source.image->format.pixelFormat == OWF_IMAGE_ARGB_INTERNAL);
    OWF_ASSERT(blend->destination.image->format.pixelFormat == OWF_IMAGE_ARGB_INTERNAL);
    if (blend->mask)
    {
        OWF_ASSERT(blend->mask->format.pixelFormat == OWF_IMAGE_L32);
    }
    
    dst     = blend->destination.image;
    src     = blend->source.image;
    mask    = blend->mask;
    dstRect = blend->destination.rectangle;
    srcRect = blend->source.rectangle;    

    /* this is actually asserted above */
    if (OWF_TRANSPARENCY_MASK == (transparency & OWF_TRANSPARENCY_MASK) &&
        NULL == mask)
    {
        return;
    }

    OWF_Rect_Set(&bounds, 0, 0, dst->width, dst->height);
    /* NOTE: src and dst rects should be of same size!!! */
    OWF_Rect_Set(&rect,
                 dstRect->x, dstRect->y,
                 dstRect->width, dstRect->height);
    OWF_Rect_Set(&srect,
                 srcRect->x, srcRect->y,
                 srcRect->width, srcRect->height);

    /* clip destination rectangle against bounds */
    if (!OWF_Rect_Clip(&drect, &rect, &bounds))
    {
        return;
    }

    /* adjust source rectangle if needed */
    if (drect.x > rect.x)
    {
        OWFint dx = drect.x - rect.x;
        srect.x += dx;
        srect.width -= dx;
    }

    if (drect.y > rect.y)
    {
        OWFint dy = drect.y - rect.y;
        srect.y += dy;
        srect.height -= dy;
    }

    if (drect.width < srect.width)
    {
        srect.width = drect.width;
    }

    if (drect.height < srect.height)
    {
        srect.height = drect.height;
    }


    srcPtr  = (OWFpixel*) src->data;
    srcPtr  += srect.y * src->width + srect.x;
    dstPtr  = (OWFpixel*) dst->data;
    dstPtr  += drect.y * dst->width + drect.x;

    if (mask)
    {
        maskPtr = (OWFsubpixel*) mask->data + srect.y * mask->width + srect.x;
        maskLineDelta = mask->width - drect.width;
    }
    else
    {
        maskPtr = 0;
        maskLineDelta = 0;
    }
    srcLineDelta = src->width - srect.width;
    dstLineDelta = dst->width - drect.width;

    /* inner loops */
    switch (transparency)
    {
        case OWF_TRANSPARENCY_NONE:
        {
            /*
            rgb     = src.rgb
            alpha    = 1
            */
            BLENDER_INNER_LOOP_BEGIN;
#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
                DR = SR;
                DG = SG;
                DB = SB;
                DA = OWF_FULLY_OPAQUE;
#else
                *(OWFuint32*)dstPtr = *(OWFuint32*)srcPtr | ARGB8888_ALPHA_MASK;
#endif
            BLENDER_INNER_LOOP_END_NO_MASK;
            break;
        }

        case OWF_TRANSPARENCY_GLOBAL_ALPHA:
        {
            /*
            rgb        = src.rgb * elem.alpha + dst.rgb * (1 - elem.alpha)
            alpha   = elem.alpha + dst.alpha * (1 - elem.alpha)
            */
            BLENDER_INNER_LOOP_BEGIN;
                DR = (SR * GA + DR * (OWF_FULLY_OPAQUE - GA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DG = (SG * GA + DG * (OWF_FULLY_OPAQUE - GA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DB = (SB * GA + DB * (OWF_FULLY_OPAQUE - GA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DA = GA + (DA * (OWF_FULLY_OPAQUE - GA) + OWF_BLEND_ROUNDING_VALUE) / 
                     OWF_ALPHA_MAX_VALUE;
            BLENDER_INNER_LOOP_END_NO_MASK;
            break;
        }

        case OWF_TRANSPARENCY_SOURCE_ALPHA:
        {
#ifndef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
        OWFuint32 redBlueMask = (ARGB8888_RED_MASK | ARGB8888_BLUE_MASK);
        OWFuint32 alphaGreenMask = (ARGB8888_ALPHA_MASK | ARGB8888_GREEN_MASK);
        OWFuint32 halfRedBlue = 0x00800080;
#endif
            /*
            rgb     = src.rgb + dst.rgb * (1 - src.alpha)
            alpha    = src.alpha + dst.alpha * (1 - src.alpha)
            */
            BLENDER_INNER_LOOP_BEGIN;
#ifdef OWF_IMAGE_INTERNAL_PIXEL_IS_FLOAT
            DR = SR + (DR * (OWF_FULLY_OPAQUE - SA) + OWF_BLEND_ROUNDING_VALUE) / OWF_ALPHA_MAX_VALUE;
            DG = SG + (DG * (OWF_FULLY_OPAQUE - SA) + OWF_BLEND_ROUNDING_VALUE) / OWF_ALPHA_MAX_VALUE;
            DB = SB + (DB * (OWF_FULLY_OPAQUE - SA) + OWF_BLEND_ROUNDING_VALUE) / OWF_ALPHA_MAX_VALUE;
            DA = SA + (DA * (OWF_FULLY_OPAQUE - SA) + OWF_BLEND_ROUNDING_VALUE) / OWF_ALPHA_MAX_VALUE;
#else
            {
                OWFuint32 srcP = *(OWFuint32*)srcPtr;
                OWFuint8 srcA = srcP >> ARGB8888_ALPHA_SHIFT;

                if (srcA == OWF_ALPHA_MAX_VALUE)
                    {
                    /* Fully opaque source pixel */
                    *(OWFuint32*)dstPtr = srcP;
                    }
                else
                    {
                    if (srcA)
                        {
                        /* Not fully transparent source pixel. Algorithm after Jim Blinn */
                        OWFuint32 mask = 0xFF - srcA;
                        OWFuint32 dstP = *(OWFuint32*)dstPtr;
                        OWFuint32 blend;

                        blend = mask * (dstP & redBlueMask) + halfRedBlue;
                        srcP += ((blend + ((blend >> 8) & redBlueMask) >> 8)) & redBlueMask;
                        blend = mask * ((dstP >> 8) & redBlueMask) + halfRedBlue;
                        srcP += (blend + ((blend >> 8) & redBlueMask)) & alphaGreenMask;

                        *(OWFuint32*)dstPtr = srcP;
                        }
                    }
            }
#endif
            BLENDER_INNER_LOOP_END_NO_MASK;
            break;
        }

        case OWF_TRANSPARENCY_MASK:
        {
            /*
            rgb     = src.rgb * mask.alpha + dst.rgb * (1 - mask.alpha)
            alpha    = mask.alpha + dst.alpha * (1 - mask.alpha)
            */
            BLENDER_INNER_LOOP_BEGIN;
                DR = (SR * MA + DR * (OWF_FULLY_OPAQUE - MA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DG = (SG * MA + DG * (OWF_FULLY_OPAQUE - MA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DB = (SB * MA + DB * (OWF_FULLY_OPAQUE - MA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DA = MA + (DA * (OWF_FULLY_OPAQUE - MA) + OWF_BLEND_ROUNDING_VALUE) / 
                     OWF_ALPHA_MAX_VALUE;
            BLENDER_INNER_LOOP_END;
            break;
        }

        case OWF_TRANSPARENCY_GLOBAL_ALPHA | OWF_TRANSPARENCY_SOURCE_ALPHA:
        {
            /*
            rgb = src.rgb * elem.a + dst.rgb * (1 - src.a * elem.a)
            a = src.a * elem.a + dst.a * (1 - src.a * elem.a)
            */
            OWFsubpixel         SAEA;

            BLENDER_INNER_LOOP_BEGIN;
                SAEA = SA * GA / OWF_ALPHA_MAX_VALUE;
                DR = (SR * GA + DR * (OWF_FULLY_OPAQUE - SAEA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DG = (SG * GA + DG * (OWF_FULLY_OPAQUE - SAEA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DB = (SB * GA + DB * (OWF_FULLY_OPAQUE - SAEA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DA = SAEA + (DA * (OWF_FULLY_OPAQUE - SAEA) + OWF_BLEND_ROUNDING_VALUE) /
                         OWF_ALPHA_MAX_VALUE;
            BLENDER_INNER_LOOP_END_NO_MASK;
            break;
        }

        case OWF_TRANSPARENCY_GLOBAL_ALPHA | OWF_TRANSPARENCY_MASK:
        {
            /*
            rgb    = src.rgb * mask.a * elem.a + dst.rgb * (1 - mask.a * elem.a)
            a     = mask.a * elem.a + dest.a * (1 - mask.a * elem.a)
            */
            OWFsubpixel             MAEA;

            BLENDER_INNER_LOOP_BEGIN;
                MAEA = MA * GA / OWF_ALPHA_MAX_VALUE;
                DR = (SR * MAEA + DR * (OWF_FULLY_OPAQUE - MAEA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DG = (SG * MAEA + DG * (OWF_FULLY_OPAQUE - MAEA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DB = (SB * MAEA + DB * (OWF_FULLY_OPAQUE - MAEA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                DA = MAEA + (DA * (OWF_FULLY_OPAQUE - MAEA) + OWF_BLEND_ROUNDING_VALUE) /
                     OWF_ALPHA_MAX_VALUE;
                //No need to check with OWF_ALPHA_MIN_VALUE as it is zero
                OWF_ASSERT(GA <= OWF_ALPHA_MAX_VALUE);
            BLENDER_INNER_LOOP_END;
            break;
        }

        default:
        {
            DPRINT(("OWF_Image_Blend: whooops. invalid blending mode\n"));
            abort();
            break;
        }
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void*
OWF_Image_AllocData(OWF_DISPCTX dc, OWFint width, OWFint height, OWF_PIXEL_FORMAT pixelFormat)
{
    OWF_IMAGE_FORMAT        imgf;
    OWFint                  stride;

    DPRINT(("OWF_Image_AllocData"));

    /* kludge. GetStride need pixelFormat. */
    imgf.pixelFormat = pixelFormat;
    imgf.rowPadding = OWF_Image_GetFormatPadding(pixelFormat);
    stride = OWF_Image_GetStride(width, &imgf, 0);

    OWF_ASSERT (width > 0 && height > 0);

    if (stride == 0)
    {
        return NULL;
    }

    if (dc)
        {
        return OWF_DisplayContext_ScratchBuffer_Allocate(dc, height * stride);
        }
    
    return xalloc(1, height * stride);
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_FreeData(OWF_DISPCTX dc, void** data)
{
    DPRINT(("OWF_Image_FreeData"));

    if (*data)
    {
    if (dc)
        {
        OWF_DisplayContext_ScratchBuffer_Destroy(dc, *data);
        }
    else
        {
        xfree(*data);
        }
    }
    *data = NULL;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFint
OWF_Image_GetFormatPixelSize(OWF_PIXEL_FORMAT format)
{
    DPRINT(("OWF_Image_GetFormatPixelSize"));

    switch (format)
    {
        case OWF_IMAGE_ARGB_INTERNAL:
        {
            return sizeof(OWFpixel);
        }

        case OWF_IMAGE_ARGB8888:
        case OWF_IMAGE_XRGB8888:
        case OWF_IMAGE_L32:
        {
            return 4;
        }

        case OWF_IMAGE_RGB888:
        {
            return 3;
        }

        case OWF_IMAGE_RGB565:
        case OWF_IMAGE_L16:
        {
            return 2;
        }

        case OWF_IMAGE_L8:
        {
            return 1;
        }

        case OWF_IMAGE_L1:
        {
            /* Use negative numbers for divisor, e.g., -8 = 1/8. */
            /* L1 is 1 bit alpha, LSB->MSB, each row padded to 32-bit
             * boundary. */
            return -8;
        }
        default:
        {
            return 0;
        }
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFint
OWF_Image_GetFormatPadding(OWF_PIXEL_FORMAT format)
{
    OWFint                  padding = 1;

    DPRINT(("OWF_Image_GetFormatPadding"));

    switch (format)
    {
        case OWF_IMAGE_ARGB_INTERNAL:
        {
            padding = sizeof(OWFpixel);
            break;
        }

        case OWF_IMAGE_ARGB8888:
        case OWF_IMAGE_XRGB8888:
        case OWF_IMAGE_L32:
        {
            padding = 4;
            break;
        }

        /*
        case OWF_IMAGE_RGB888:
        {
            return 3;
        }
        */

        case OWF_IMAGE_RGB565:
        case OWF_IMAGE_L16:
        {
            padding = 2;
            break;
        }

        case OWF_IMAGE_L8:
        {
            padding = 1;
            break;
        }

        case OWF_IMAGE_L1:
        {
            /* Use negative numbers for divisor, e.g., -8 = 1/8. */
            /* L1 is 1 bit alpha, LSB->MSB, each row padded to 32-bit
             * boundary. */
            padding = 4;
            break;
        }
        default:
        {
			OWF_ASSERT(0);
            break;
        }
    }


    return padding;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
OWF_Image_SwapWidthAndHeight(OWF_IMAGE* image)
{
    DPRINT(("OWF_Image_SwapWidthAndHeight"));

    /* swap w & h. Note that original stride is not restored if swap back. */
    image->width ^= image->height;
    image->height ^= image->width;
    image->width ^= image->height;

    image->stride = OWF_Image_GetStride(image->width, &image->format, 0);
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean
OWF_Image_ConvertMask(OWF_IMAGE* output, OWF_IMAGE* input)
{
    OWFboolean              result = OWF_TRUE;
    void*                   srcLinePtr;
    OWFsubpixel*            dstLinePtr;
    OWFint                  countY;

    DPRINT(("OWF_Image_ConvertMask:"));
    DPRINT(("  Converting mask from stream format to internal 8-bit"));

    OWF_ASSERT(input);
    OWF_ASSERT(output);
    
    srcLinePtr = input->data;
    dstLinePtr = (OWFsubpixel*) output->data;    

    for (countY = input->height; countY; countY--)
    {
        OWFsubpixel* dstData = dstLinePtr;    
    
        switch (input->format.pixelFormat)
        {
            case OWF_IMAGE_L1:
            {
                OWFint          countX;
                OWFuint8*       srcData = (OWFuint8*) srcLinePtr;
    
                DPRINT(("1-bit alpha, width = %d, height = %d",
                       input->width, input->height));
                
                for (countX = 0; countX < input->width; countX++)
                {
                    /*
                     * alpha pixel ordering is LSB -> MSB
                     *
                     * byte# |----- byte 0 ----|----- byte 1-----|--
                     * bit#  | 7 6 5 4 3 2 1 0 | 7 6 5 4 3 2 1 0 |
                     * pix#  | 7 6 5 4 3 2 1 0 | f e d c b a 9 8 | ...
                     */
                    if (srcData[countX >> 3] & (1 << (countX & 7)))
                    {
                        dstData[countX] = OWF_FULLY_OPAQUE;
                    }
                    else
                    {
                        dstData[countX] = OWF_FULLY_TRANSPARENT;
                    }
                }           
                break;
            }
            case OWF_IMAGE_L8:
            {
                OWFint          countX;
                OWFuint8*       srcData = (OWFuint8*) srcLinePtr;
    
                DPRINT(("8-bit alpha, width = %d, height = %d",
                       input->width, input->height));
                
                for (countX = 0; countX < input->width; countX++)
                {
                    dstData[countX] = srcData[countX] * OWF_FULLY_OPAQUE /
                                 (OWFfloat) OWF_BYTE_MAX_VALUE;                
                }
                break;
            }
            case OWF_IMAGE_ARGB8888:
            {
                /* ARGB image - take the alpha channel and discard
                 * everything else */
                OWFint          countX;
                OWFuint32*      srcData = (OWFuint32*) srcLinePtr;
    
                DPRINT(("32-bit ARGB, width = %d, height = %d",
                       input->width, input->height));
                
                for (countX = 0; countX < input->width; countX++)
                {
                    dstData[countX] = (srcData[countX] >> 24) * OWF_FULLY_OPAQUE /
                                 (OWFfloat) OWF_BYTE_MAX_VALUE;          
                }
                break;
            }
            default:
            {
                DPRINT(("Unsupported alpha format, ignoring mask"));
    
                result = OWF_FALSE;
                break;
            }
        }
        
        dstLinePtr+=output->width;
        /* Presumes that the stride is always whole bytes - eg. a 2x2-pixel mono 
         * image takes at least 2 bytes */
        srcLinePtr=(OWFuint8*)srcLinePtr+input->stride;        
    }
    return result;
}

#ifdef __cplusplus
}
#endif
