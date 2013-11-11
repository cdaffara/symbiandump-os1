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
 *  \file wfcpipeline.c
 *
 *  \brief SI Composition pipeline stages
 *
 *  Each pipeline stage is implemented in their respective functions
 *  that take context and element as parameter. Composition status is
 *  stored in elements state variable (struct WFC_ELEMENT_STATE.)
 *  State has no strict input/output variables, each stage reads/writes
 *  those variables it needs
 */
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "WF/wfc.h"
#include "wfccontext.h"
#include "wfcelement.h"
#include "wfcimageprovider.h"
#include "wfcstructs.h"
#include "wfcscene.h"

#include "owfobject.h"

#include "owfnativestream.h"
#include "owfmemory.h"
#include "owfimage.h"
#include "owfdebug.h"

#define EXTRA_PIXEL_BOUNDARY 2

/*!
 *  \brief Check element destination visibility
 *
 *  Check if element's destination rectangle is
 *  inside context's visible limits.
 *
 *  \param context          Context
 *  \param element          Element
 *
 *  \return Boolean value indicating whether element is visible or not
 */

static WFCboolean
WFC_Pipeline_ElementIsVisible(WFC_CONTEXT* context, WFC_ELEMENT* element)
{
    OWF_RECTANGLE bounds, rect, drect;

    if ((context->rotation == WFC_ROTATION_90) || (context->rotation == WFC_ROTATION_270))
        {
        OWF_Rect_Set(&bounds, 0, 0, context->targetHeight, context->targetWidth);
        }
    else
        {
        OWF_Rect_Set(&bounds, 0, 0, context->targetWidth, context->targetHeight);
        }
    
    OWF_Rect_Set(&rect, element->dstRect[0], element->dstRect[1],
            element->dstRect[2], element->dstRect[3]);

    /* check destination rectangle against bounds - exit if not visible */
    if (!OWF_Rect_Clip(&drect, &rect, &bounds))
    {
        return WFC_FALSE;
    }

    return WFC_TRUE;
}

static void 
WFC_Pipeline_BlendInfo(WFC_CONTEXT* context, WFC_ELEMENT_STATE* state)
{

    OWF_ASSERT(state);    
    
    /* setup blending parameters */
    state->blendInfo.destination.image     = context->state.internalTargetImage;
    state->blendInfo.destination.rectangle = &state->dstRect;
    state->blendInfo.source.image          = state->scaledSourceImage;
    state->blendInfo.source.rectangle      = &state->scaledSrcRect;
    state->blendInfo.mask                  = state->originalMaskImage ? state->maskImage : NULL;
    state->blendInfo.globalAlpha           = state->globalAlpha;
    
    DPRINT(("  globalAplha = %f", state->globalAlpha));
    /* no need to check with OWF_ALPHA_MIN_VALUE as it is zero */
    OWF_ASSERT(state->blendInfo.globalAlpha <= OWF_ALPHA_MAX_VALUE);    
}

/*! Transform the source rectangle to represent the floating point viewport
    as an offset in the final rotation stage image */
static void
WFC_Pipeline_TransformSource(WFC_ELEMENT_STATE* state)
{
    OWFfloat                width, height, totalWidth, totalHeight,
                            leftMargin, rightMargin, 
                            topMargin, bottomMargin,
                            temp;
    OWF_FLIP_DIRECTION      flipping;
    WFCRotation             rotation;
    
    
    OWF_ASSERT(state);
    
    width = state->sourceRect[2];
    totalWidth = state->sourceRect[0] + state->sourceRect[2];
    
    height = state->sourceRect[3];
    totalHeight = state->sourceRect[1] + state->sourceRect[3];
    
    /* X margins - includes 1 pixel border */
    leftMargin = (state->sourceRect[0] - ((float) floor(state->sourceRect[0]))) + 1.0f;
    rightMargin = (((float) ceil(totalWidth)) - totalWidth) + 1.0f;
    
    /* Y margins - includes 1 pixel border */
    topMargin = (state->sourceRect[1] - ((float) floor(state->sourceRect[1]))) + 1.0f;    
    bottomMargin = (((float) ceil(totalHeight)) - totalHeight) + 1.0f;
    
    /* flip stages */
    flipping = state->sourceFlip > 0.0f ? OWF_FLIP_VERTICALLY
                                      : OWF_FLIP_NONE;
    
    /* top margin needs to be the bottom margin */
    if (flipping & OWF_FLIP_VERTICALLY)
    {
        temp = topMargin;
        topMargin = bottomMargin;
        bottomMargin = temp; 
    }
    
    /* rotation stages */
    rotation = state->rotation;
    
    switch (rotation)
    {
        case WFC_ROTATION_0:
        {
            break;
        }

        case WFC_ROTATION_90:
        {
            /* switch width and height */
            temp = width;
            width = height;
            height = temp;
            
            topMargin = leftMargin;
            leftMargin = bottomMargin;
            
            break;
        }

        case WFC_ROTATION_180:
        {
            leftMargin = rightMargin;
            topMargin = bottomMargin;
        
            break;
        }

        case WFC_ROTATION_270:
        {
            /* switch width and height */
            temp = width;
            width = height;
            height = temp; 
            
            leftMargin = topMargin;
            topMargin = rightMargin;
            
            break;
        }

        default:
        {
            OWF_ASSERT(0);
        }
    }    
    
    /* X offset */
    state->transformedSourceRect[0] = leftMargin;
    /* Y offset */
    state->transformedSourceRect[1] = topMargin;
    /* width */
    state->transformedSourceRect[2] = width;
    /* height */
    state->transformedSourceRect[3] = height;    
}

/*! Calculate the oversized integer crop region */
static void
WFC_Pipeline_OversizedViewport(WFC_ELEMENT_STATE* state)
{
    OWFint width, height;

    state->oversizedCropRect.x = (int) floor(state->sourceRect[0]);
    state->oversizedCropRect.y = (int) floor(state->sourceRect[1]);
    
    width = (int) ceil(state->sourceRect[0] + state->sourceRect[2]);
    state->oversizedCropRect.width = (width - state->oversizedCropRect.x) + EXTRA_PIXEL_BOUNDARY;
    
    height = (int) ceil(state->sourceRect[1] + state->sourceRect[3]);    
    state->oversizedCropRect.height = (height - state->oversizedCropRect.y) + EXTRA_PIXEL_BOUNDARY;
}

/*-----------------------------------------------------------*
 * Initial creation of element state object created just once per context
 *-----------------------------------------------------------*/
OWF_API_CALL void WFC_Pipeline_DestroyState(WFC_CONTEXT* context)
{
    WFC_ELEMENT_STATE*      state;
    state= &context->prototypeElementState;
    OWF_Image_Destroy(state->scaledSourceImage);
    OWF_Image_Destroy(state->croppedSourceImage);
    OWF_Image_Destroy(state->convertedSourceImage);
    OWF_Image_Destroy(state->rotatedSourceIntermediateImage);
    OWF_Image_Destroy(state->flippedSourceImage);
    OWF_Image_Destroy(state->rotatedSourceImage);    
    OWF_Image_Destroy(state->maskImage);
    state->scaledSourceImage=NULL;
    state->croppedSourceImage=NULL;
    state->convertedSourceImage=NULL;
    state->rotatedSourceIntermediateImage=NULL;
    state->flippedSourceImage=NULL;
    state->rotatedSourceImage=NULL;    
    state->maskImage=NULL;
}

OWF_API_CALL OWFboolean WFC_Pipeline_CreateState(WFC_CONTEXT* context)
{
    WFC_ELEMENT_STATE*      state;
    OWF_IMAGE_FORMAT        fmt;
    
    fmt.pixelFormat    = OWF_IMAGE_ARGB_INTERNAL;
    fmt.linear         = OWF_FALSE;
    fmt.premultiplied  = OWF_FALSE;
    fmt.rowPadding     = 1;
    state= &context->prototypeElementState;
    /* All buffers are initially created the full size of the scratch buffers, whicgh records the buffer size in bytes */
    state->convertedSourceImage=OWF_Image_Create(MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT, &fmt, context->scratchBuffer[1], 0);
    state->croppedSourceImage=OWF_Image_Create(MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT, &fmt, context->scratchBuffer[2], 0);
    state->flippedSourceImage=OWF_Image_Create(MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT, &fmt, context->scratchBuffer[2], 0);    

    state->rotatedSourceIntermediateImage=OWF_Image_Create(MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT, &fmt, context->scratchBuffer[1], 0);      
    state->rotatedSourceImage=OWF_Image_Create(MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT, &fmt, context->scratchBuffer[2], 0);  
    state->scaledSourceImage=OWF_Image_Create(MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT, &fmt, context->scratchBuffer[3], 0);
    fmt.pixelFormat    = OWF_IMAGE_L32;
    state->maskImage=OWF_Image_Create(MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT, &fmt, context->scratchBuffer[4], 0);
    if (!state->convertedSourceImage||!state->croppedSourceImage||!state->flippedSourceImage
        ||!state->rotatedSourceIntermediateImage||!state->rotatedSourceImage
        ||!state->scaledSourceImage||!state->maskImage
        )
        {
        WFC_Pipeline_DestroyState(context);
        return OWF_FALSE;
        }
    return OWF_TRUE;
}


/*---------------------------------------------------------------------------
 *  Composition pipeline preparation
 *
 *  \param context          Context
 *  \param element          Element
 *
 *  \return Boolean value indicating whether preparation succeeded
 *----------------------------------------------------------------------------*/
#ifdef DEBUG
/* reset size to original extent then try to set it to the target size */
#define CREATE_WITH_LIMITS(img, imgW, imgH, fmt, maxW, maxH) \
    { \
        OWFboolean resized; \
        OWF_Image_SetSize(img, maxW, maxH); \
        OWF_Image_SetFlags(img,(fmt)->premultiplied,(fmt)->linear); \
        resized = OWF_Image_SetSize(img, imgW, imgH); \
        OWF_ASSERT(resized); \
    }
#else
#define CREATE_WITH_LIMITS(img, imgW, imgH, fmt, maxW, maxH) \
    { \
        OWF_Image_SetSize(img, maxW, maxH); \
        OWF_Image_SetFlags(img,(fmt)->premultiplied,(fmt)->linear); \
        OWF_Image_SetSize(img, imgW, imgH); \
    }
#endif
OWF_API_CALL WFC_ELEMENT_STATE*
WFC_Pipeline_BeginComposition(WFC_CONTEXT* context, WFC_ELEMENT* element)
{
    WFC_ELEMENT_STATE*      state = &context->prototypeElementState;
    OWF_IMAGE_FORMAT        imgf;
    OWFint                  sourceWidth;
    OWFint                  sourceHeight;
    OWFint                  x;
    OWFint                  tempWidth, tempHeight;


    DPRINT(("WFC_Element_BeginComposition(%x,%x)",
            context ? context->handle : 0, element ? element->handle : 0));

    if (!context || !element)
    {
        DPRINT(("  context == NULL || element == NULL"));
        return NULL;
    }

    if (!WFC_Pipeline_ElementIsVisible(context, element))
    {
        DPRINT(("  element [%x] totally outside of target - skipped",
                        element ? element->handle : 0));
        return NULL;
    }

    /* setup temporary images used in composition. since the original
       source data must not be altered, we must copy it to scratch buffer
       and work it there. another scratch buffer is needed for scaling
       the image to its final size. same applies for masks; thus a grand total
       of 4 scratch buffers are needed. */
    OWF_ASSERT(element->source);
    OWF_ASSERT(element->source->streamHandle);     
    
    state->originalSourceImage = element->source->lockedStream.image;
    state->rotation = element->sourceRotation;
    state->sourceFlip = element->sourceFlip;
    state->globalAlpha = element->globalAlpha;
    state->sourceScaleFilter = element->sourceScaleFilter;
    state->transparencyTypes = element->transparencyTypes;

    if (state->transparencyTypes & WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA)
        {
        if (state->globalAlpha == OWF_FULLY_TRANSPARENT)
            {
            /* Fully transparent element - no contribution. */
            return NULL;
            }
        if (state->globalAlpha == OWF_FULLY_OPAQUE)
            {
            /* Fully opaque global alpha - global alpha can be ignored */
            state->transparencyTypes &= ~WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA;
            }
        }
    
/* replicate the source viewport rectangle and target extent rectangle */
    for (x = 0; x < 4; x++)
    {
        state->sourceRect[x] = element->srcRect[x];
        state->destinationRect[x] = element->dstRect[x];
    }
    OWF_Rect_Set(&state->dstRect, element->dstRect[0], element->dstRect[1],
                 element->dstRect[2], element->dstRect[3]);    
    
    /* transform the source rectangle to represent the floating point viewport
       as an offset in the final rotation stage image */
    WFC_Pipeline_TransformSource(state);
    
    imgf.pixelFormat    = OWF_IMAGE_ARGB_INTERNAL;
    imgf.linear         = element->source->lockedStream.image->format.linear;
    imgf.premultiplied  = element->source->lockedStream.image->format.premultiplied;
    imgf.rowPadding     = 1;

    /* add a 1 pixel boundary so we can replicate the edges */
    sourceWidth = element->source->lockedStream.image->width + EXTRA_PIXEL_BOUNDARY;
    sourceHeight = element->source->lockedStream.image->height + EXTRA_PIXEL_BOUNDARY;
    
    CREATE_WITH_LIMITS(state->convertedSourceImage,
                       sourceWidth,
                       sourceHeight,
                       &imgf,
                       MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT);
    
    /* calculate the oversized integer crop region (inc. 1 pixel boundary)
       so edge replication can be performed */
    WFC_Pipeline_OversizedViewport(state);    
    
    /* subsequent temporary renderstage pipeline images need to use the oversized
       integer crop region */
    CREATE_WITH_LIMITS(state->croppedSourceImage,
                       state->oversizedCropRect.width, state->oversizedCropRect.height,
                       &imgf,
                       MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT);
    
    CREATE_WITH_LIMITS(state->flippedSourceImage,
                       state->oversizedCropRect.width, state->oversizedCropRect.height,
                       &imgf,
                       MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT);    
    

    if (state->rotation == WFC_ROTATION_90 || state->rotation == WFC_ROTATION_270)
    {
        tempHeight = state->oversizedCropRect.width;
        tempWidth = state->oversizedCropRect.height;
    }
    else
    {
        tempWidth = state->oversizedCropRect.width;
        tempHeight = state->oversizedCropRect.height;
    }

    CREATE_WITH_LIMITS(state->rotatedSourceIntermediateImage,
                       tempWidth, tempHeight,
                       &imgf,
                       MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT);      
    

     /* no rotation required - just use the previous stages (flip) buffer */
    CREATE_WITH_LIMITS(state->rotatedSourceImage,
                       tempWidth, tempHeight,
                       &imgf,
                       MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT);  

    /* finally, scaled image uses destination width and height */
    OWF_Rect_Set(&state->scaledSrcRect, 0, 0, element->dstRect[2], element->dstRect[3]);
    CREATE_WITH_LIMITS(state->scaledSourceImage,
                       state->scaledSrcRect.width,
                       state->scaledSrcRect.height,
                       &imgf,
                        MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT);

    if (!(state->convertedSourceImage && state->croppedSourceImage &&
          state->scaledSourceImage && state->rotatedSourceIntermediateImage &&
          state->flippedSourceImage && state->rotatedSourceImage))
    {
        DPRINT(("  Preparation of intermediate pipeline image buffers failed"
                "  (May be caused by overflow or out-of-memory situation)"));
        DPRINT(("    convertedSourceImage = %p", state->convertedSourceImage));
        DPRINT(("    croppedSourceImage = %p", state->croppedSourceImage));
        DPRINT(("    scaledSourceImage = %p", state->scaledSourceImage));
        DPRINT(("    rotatedSourceIntermediateImage = %p", state->rotatedSourceIntermediateImage));
        DPRINT(("    flippedSourceImage = %p", state->flippedSourceImage));
        DPRINT(("    rotatedSourceImage = %p", state->rotatedSourceImage));


        return (WFC_ELEMENT_STATE*)WFC_FALSE;
    }

#ifdef DEBUG
        OWF_Image_Clear(state->convertedSourceImage, 0, 0, 0, 0);
        OWF_Image_Clear(state->croppedSourceImage, 0, 0, 0, 0);
        OWF_Image_Clear(state->scaledSourceImage, 0, 0, 0, 0);
        OWF_Image_Clear(state->rotatedSourceIntermediateImage, 0, 0, 0, 0);
        OWF_Image_Clear(state->flippedSourceImage, 0, 0, 0, 0);
        OWF_Image_Clear(state->rotatedSourceImage, 0, 0, 0, 0);
#endif

    /* setup mask in case the element has one */
    if (element->maskComposed)
    {
        OWF_IMAGE*          image = NULL;
        OWFsubpixel*        pix = NULL;
        WFCint              i = 0;

        DPRINT(("Processing element mask"));
        OWF_ASSERT(&element->mask);
        OWF_ASSERT(&element->mask->streamHandle);
        image = element->mask->lockedStream.image;
        OWF_ASSERT(image);

        state->originalMaskImage = element->mask->lockedStream.image;        
        
        imgf.pixelFormat    = OWF_IMAGE_L32;
        imgf.linear         = image->format.linear;
        imgf.premultiplied  = image->format.premultiplied;

        /* mask size is always same as destination rect's */
        DPRINT(("Binding stream image to scratch buffer"));
        CREATE_WITH_LIMITS(state->maskImage,
                           state->scaledSrcRect.width,
                           state->scaledSrcRect.height,
                           &imgf,
                           MAX_SOURCE_WIDTH, MAX_SOURCE_HEIGHT);

        /* initialize mask */
        DPRINT(("Initializing mask, size = %dx%d", state->scaledSrcRect.width,
                state->scaledSrcRect.height));
        pix = (OWFsubpixel*) state->maskImage->data;
        for (i = 0; i < state->scaledSrcRect.width * state->scaledSrcRect.height; i++)
        {
            pix[i] = OWF_FULLY_OPAQUE;
        }
    }
    else
        {
        state->originalMaskImage=NULL;
        }
    
    WFC_Pipeline_BlendInfo(context, state);

    DPRINT(("  Cropped source image size is %dx%d",
           state->croppedSourceImage->width, state->croppedSourceImage->height));
    DPRINT(("  Scaled source image size is %dx%d",
            state->scaledSourceImage->width, state->scaledSourceImage->height));
    DPRINT(("  Mirrored source intermediate image size is %dx%d",
            state->rotatedSourceIntermediateImage->width, state->rotatedSourceIntermediateImage->height));
    DPRINT(("  Mirrored source image size is %dx%d",
            state->flippedSourceImage->width, state->flippedSourceImage->height));
    DPRINT(("  Rotated source image size is %dx%d",
            state->rotatedSourceImage->width, state->rotatedSourceImage->height));  
    
    return state;
}

/*---------------------------------------------------------------------------
 *  Composition pipeline cleanup
 *
 *  \param context          Context
 *  \param element          Element
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_EndComposition(WFC_CONTEXT* context, WFC_ELEMENT* element, WFC_ELEMENT_STATE* state)
{

    if (!context || !element)
    {
        DPRINT(("WFC_Element_EndComposition: context == NULL || "
                "element == NULL"));
    }


    OWF_ASSERT(state);
    state->originalSourceImage=NULL;
    state->originalMaskImage=NULL;

}

/*---------------------------------------------------------------------------
 *  \brief Source conversion stage
 *
 *  \param context          Context
 *  \param element          Element
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteSourceConversionStage(WFC_CONTEXT* context,
        WFC_ELEMENT_STATE* state)
{
    /* this stage could be embedded in cropping stage */


    if (NULL == context || NULL == state)
    {
        DPRINT(("WFC_Context_ExecuteSourceConversionStage: context = %p, "
                "state = %p",
                context, state));
        return;
    }

    OWF_ASSERT(state->originalSourceImage);

    OWF_Image_SourceFormatConversion(state->convertedSourceImage,
                                     state->originalSourceImage);

    /* convert mask from stream format to internal format */
    if (state->originalMaskImage)
    {
        if (!OWF_Image_ConvertMask(state->maskImage, state->originalMaskImage))
        {
            state->originalMaskImage=NULL;
        }
    }
}

/*---------------------------------------------------------------------------
 *  \brief Crop stage
 *
 *  \param context          Context
 *  \param element          Element
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteCropStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* state)
{
    OWF_RECTANGLE           sourceRect,
                            cropRect;

    DPRINT(("WFC_Pipeline_ExecuteCropStage"));

    if (NULL == context || NULL == state)
    {
        DPRINT(("WFC_Context_ExecuteCropStage: context = %p, state = %p",
               context, state));
    }
    else
    {
        /* Source rectangle */
        OWF_Rect_Set(&sourceRect,
                     state->oversizedCropRect.x, state->oversizedCropRect.y,
                     state->oversizedCropRect.width, state->oversizedCropRect.height);

        /* cropped source size - supports oversized integer and 1 pixel boundary */
        OWF_Rect_Set(&cropRect,
                     0, 0,
                     state->oversizedCropRect.width, state->oversizedCropRect.height);

        OWF_Image_Blit(state->croppedSourceImage, &cropRect,
                       state->convertedSourceImage, &sourceRect);
    }
}

/*---------------------------------------------------------------------------
 *  \brief Flip stage
 *
 *  \param context          Context
 *  \param element          Element
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteFlipStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* state)
{
    OWF_FLIP_DIRECTION      flipping;
	
    if (NULL == context || NULL == state)
    {
        DPRINT(("WFC_Context_ExecuteFlipStage: context = %p, state = %p",
               context, state));
    }
    else
    {
        flipping = state->sourceFlip > 0.0f ? OWF_FLIP_VERTICALLY
                                          : OWF_FLIP_NONE;
        
        OWF_Image_Flip(state->flippedSourceImage, flipping);
    }
}

/*---------------------------------------------------------------------------
 *  \brief Rotation stage
 *
 *  \param context          Context
 *  \param element          Element
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteRotationStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* state)
{
    OWF_ROTATION            rot = OWF_ROTATION_0;
    OWF_RECTANGLE           rect;    
    WFCRotation             rotation;

    if (NULL == context || NULL == state)
    {
        DPRINT(("WFC_Context_ExecuteRotationStage: context = %p, state = %p",
               context, state));
        return;
    }

    rotation = state->rotation;
    DPRINT(("  Element rotation = %d", rotation));

    switch (rotation)
    {
        case WFC_ROTATION_0:
        {
            return; /* Rotate copies back into input buffer so just skip */
        }

        case WFC_ROTATION_90:
        {
            rot = OWF_ROTATION_90;
            break;
        }

        case WFC_ROTATION_180:
        {
            rot = OWF_ROTATION_180;
            break;
        }

        case WFC_ROTATION_270:
        {
            rot = OWF_ROTATION_270;
            break;
        }

        default:
        {
            OWF_ASSERT(0);
        }
    }

    /* rotate the the image using rotatedSourceIntermediateImage */
    OWF_Image_Rotate(state->rotatedSourceIntermediateImage,
                     state->flippedSourceImage,
                     rot);

    /* blit rotated image back to original image buffer */
    rect.x      = 0;
    rect.y      = 0;
    rect.width  = state->rotatedSourceIntermediateImage->width;
    rect.height = state->rotatedSourceIntermediateImage->height;

    DPRINT(("  Source image dimensions after rotation = %dx%d",
           rect.width, rect.height));

    OWF_Image_Blit(state->rotatedSourceImage,
                   &rect,
                   state->rotatedSourceIntermediateImage,
                   &rect);
    
}

/*---------------------------------------------------------------------------
 *  \brief Scaling stage
 *
 *  \param context          Context
 *  \param element          Element
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteScalingStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* state)
{
    OWF_RECTANGLE           scaledRect,
                            cropRect;
    OWF_FILTERING           filteringMode = OWF_FILTER_POINT_SAMPLING;
    WFCScaleFilter          filter;
	
    DPRINT(("WFC_Context_ExecuteScalingStage(%p,%p)", context, state));

    if (NULL == context || NULL == state)
    {
        DPRINT(("WFC_Context_ExecuteScalingStage: context = %p, state = %p",
               context, state));
        return;
    }

    filter = state->sourceScaleFilter;

    switch (filter)
    {
        case WFC_SCALE_FILTER_NONE:
        case WFC_SCALE_FILTER_FASTER:
        {
            filteringMode = OWF_FILTER_POINT_SAMPLING;
            DPRINT(("  Using point-sampling filter"));
            break;
        }
        case WFC_SCALE_FILTER_BETTER:
        {
            filteringMode = OWF_FILTER_BILINEAR;
            DPRINT(("  Using bilinear filter"));
            break;
        }

        case WFC_SCALE_FILTER_FORCE_32BIT:
        {
            /* To shut the compiler up -- not a valid filtering mode.
             * Validity is ensured when the filter attribute value
             * is set, thus it shouldn't have this value ever. */
            OWF_ASSERT(0);
            break;
        }
    }

    OWF_Rect_Set(&cropRect, 1, 1,
                 state->rotatedSourceImage->width - EXTRA_PIXEL_BOUNDARY,
                 state->rotatedSourceImage->height - EXTRA_PIXEL_BOUNDARY);

    OWF_Rect_Set(&scaledRect, 0, 0, state->destinationRect[2], state->destinationRect[3]);
    
    if (  scaledRect.width != state->transformedSourceRect[2] 
      ||  scaledRect.height != state->transformedSourceRect[3]
      ||  state->sourceRect[0] != floor(state->sourceRect[0])
      ||  state->sourceRect[1] != floor(state->sourceRect[1])
    )
    {
        /* scale the image */
        OWF_Image_Stretch(state->scaledSourceImage, &scaledRect,
                          state->rotatedSourceImage, state->transformedSourceRect,
                          filteringMode);
    }
    else
    {
        /* 1:1 copy, no need to scale */
        OWF_Image_Blit(state->scaledSourceImage, &scaledRect,
                       state->rotatedSourceImage, &cropRect);
    }
    
}

/*---------------------------------------------------------------------------
 *  \brief Blending stage
 *
 *  \param context          Context
 *  \param element          Element
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteBlendingStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* state)
{
    OWF_TRANSPARENCY        blendMode = OWF_TRANSPARENCY_NONE;
    WFCbitfield             transparency = 0;
	
    DPRINT(("WFC_Pipeline_ExecuteBlendingStage"));

    if (NULL == context || NULL == state)
    {
        return;
    }

    DPRINT(("  context = %d, state = %d",
           context->handle, state));

    transparency = state->transparencyTypes;
    blendMode = OWF_TRANSPARENCY_NONE;

    if (transparency & WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA)
    {
        blendMode = (OWF_TRANSPARENCY)(blendMode|OWF_TRANSPARENCY_GLOBAL_ALPHA);
    }

    if (transparency & WFC_TRANSPARENCY_SOURCE)
    {
        OWF_Image_PremultiplyAlpha(state->scaledSourceImage);
        blendMode = (OWF_TRANSPARENCY)(blendMode|OWF_TRANSPARENCY_SOURCE_ALPHA);
    }

    if ((transparency & WFC_TRANSPARENCY_MASK) && state->originalMaskImage)
    {
        blendMode = (OWF_TRANSPARENCY)(blendMode|OWF_TRANSPARENCY_MASK);
    }

    OWF_Image_Blend(&state->blendInfo, blendMode);
}
