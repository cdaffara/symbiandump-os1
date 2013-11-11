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
 *  \file wfcelement.c
 *
 *  \brief SI Element handling
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <WF/wfc.h>

#include "wfcelement.h"
#include "wfccontext.h"
#include "wfcdevice.h"
#include "wfcstructs.h"
#include "wfcimageprovider.h"
#include "owfnativestream.h"
#include "owfattributes.h"
#include "owfmemory.h"
#include "owfobject.h"

#include "owfdebug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FIRST_ELEMENT_HANDLE    3000

#define FAIL_IF(c,e)        if (c) { return e; }


static const WFCbitfield    validTransparencyModes[] = {
                                WFC_TRANSPARENCY_NONE,
                                WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA,
                                WFC_TRANSPARENCY_SOURCE,
                                WFC_TRANSPARENCY_MASK,
                                WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA |
                                    WFC_TRANSPARENCY_SOURCE,
                                WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA |
                                    WFC_TRANSPARENCY_MASK
                            };

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
void
WFC_Element_Initialize(WFC_ELEMENT* element)
{
    element->dstRect[0]         = 0;
    element->dstRect[1]         = 0;
    element->dstRect[2]         = 0;
    element->dstRect[3]         = 0;
    element->srcRect[0]         = 0;
    element->srcRect[1]         = 0;
    element->srcRect[2]         = 0;
    element->srcRect[3]         = 0;

    element->source             = WFC_INVALID_HANDLE;
    element->sourceFlip         = WFC_FALSE;
    element->sourceRotation     = WFC_ROTATION_0;
    element->sourceScaleFilter  = WFC_SCALE_FILTER_NONE;
    element->transparencyTypes  = 0;
    element->globalAlpha        = OWF_ALPHA_MAX_VALUE;
    element->maskHandle         = WFC_INVALID_HANDLE;
    element->sourceHandle       = WFC_INVALID_HANDLE;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Element_Destroy(WFC_ELEMENT* element)
{
    if (element)
    {
        DPRINT(("WFC_Element_Destroy"));

        DPRINT(("  element = %p (%d)", element, element->handle));

        DESTROY(element->cachedSource);
        DESTROY(element->cachedMask);

        DPRINT(("  cachedSource = %p (%d)", element->cachedSource,
                                            element->cachedSource ?
                                                element->cachedSource->handle :
                                                0));
        DPRINT(("  cachedMask = %p (%d)", element->cachedMask,
                                          element->cachedMask ?
                                              element->cachedMask->handle :
                                              0));

        DESTROY(element->source);
        DESTROY(element->mask);

        DPRINT(("  source = %p (%d)", element->source,
                                      element->source ?
                                          element->source->handle :
                                          0));
        DPRINT(("  mask = %p (%d)", element->mask,
                                    element->mask ?
                                        element->mask->handle :
                                        0));
        DESTROY(element->context);

        OWF_Pool_PutObject(element);
    }
}

/*---------------------------------------------------------------------------
 *  Create new element into context
 *
 *  \param context Context into which to create the element
 *
 *  \return New element object or NULL
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFC_ELEMENT*
WFC_Element_Create(WFC_CONTEXT* context)
{
    static WFCint           nextElementHandle = FIRST_ELEMENT_HANDLE;
    WFC_ELEMENT*            element;
	
	OWF_ASSERT(context);

    element = (WFC_ELEMENT*)OWF_Pool_GetObject(context->elementPool);

    if (element)
    {
        WFC_Element_Initialize(element);

        element->handle = nextElementHandle++;

        ADDREF(element->context, context);
        element->device = context->device;
    }
    return element;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFC_ELEMENT*
WFC_Element_Clone(WFC_ELEMENT* element)
{
    WFC_ELEMENT*        clone;
    WFC_CONTEXT*        context;

    OWF_ASSERT(element);

    DPRINT(("WFC_Element_Clone: element = %d, context = %d", element->handle,
            element->context->handle));

    context = CONTEXT(element->context);
    clone = (WFC_ELEMENT*)OWF_Pool_GetObject(context->elementPool);

    if (clone)
    {
        WFC_Element_Initialize(clone);

        clone->handle           = element->handle;

        clone->sourceFlip       = element->sourceFlip;
        clone->sourceRotation   = element->sourceRotation;
        clone->sourceScaleFilter= element->sourceScaleFilter;
        clone->transparencyTypes= element->transparencyTypes;
        clone->globalAlpha      = element->globalAlpha;
        clone->maskHandle       = element->maskHandle;
        clone->sourceHandle     = element->sourceHandle;

        ADDREF(clone->cachedMask, element->cachedMask);
        ADDREF(clone->cachedSource, element->cachedSource);

        ADDREF(clone->mask,     element->mask);
        ADDREF(clone->source,   element->source);
        clone->device = element->device;

        ADDREF(clone->context,  element->context);

        memcpy(clone->srcRect,  element->srcRect, 4 * sizeof(WFCfloat));
        memcpy(clone->dstRect,  element->dstRect, 4 * sizeof(WFCfloat));

    }

    return clone;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static WFCboolean WFC_Element_ClampRectangle(const char* rtype,
                                             WFCfloat* rect)
{
    /*
     * int -> float conversion:
     * ------------------------
     *
     * above 2^24 we start to lose precision when performing
     * conversions between floats & ints, thus we must clamp
     * values above in order to avoid nasty sign-change effects
     * and other weird side-effects.
     *
     */

    const WFCfloat          LIMIT = 1.6777216E7f;
    WFCint                  i;
    WFCboolean              clamped = WFC_FALSE;

    /* Prevent compiler warning when DPRINT is disabled */
    (void) rtype;

    for (i = 0; i < 4; i++)
    {
        if (fabs(rect[i]) > LIMIT)
        {
#ifdef DEBUG
            static const char* coord[4] = {"x", "y", "width", "height"};
            (void)coord[0];
#endif

            DPRINT(("  Warning: Precision loss in element's %s rectangle's "
                    "%s coordinate.",
                    rtype, coord[i]));

            rect[i] = rect[i] < 0 ? -LIMIT : LIMIT;

            clamped = WFC_TRUE;
        }
    }
    return clamped;
}
/*===========================================================================
 *
 * Attribute set-time checking functions. These are used for checking
 * the attributes before saving them into elements. Attributes are validated
 * for 2nd time during commit, where e.g. destination rectangle size vs.
 * mask size dependency is checked.
 *
 *============================================================================*/

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static WFCErrorCode
WFC_Element_ValidateSourceRectangle(WFC_ELEMENT* element,
                                    WFCfloat* rect)
{
    WFCErrorCode            result = WFC_ERROR_NONE;
    
#ifndef DEBUG
    (void) element;
#endif    

    OWF_ASSERT(element);
    OWF_ASSERT(rect);

    if (rect[0] < 0.0f || rect[1] < 0.0f || rect[2] < 0.0f || rect[3] < 0.0f)
    {
        result = WFC_ERROR_ILLEGAL_ARGUMENT;
    }
    else if (WFC_Element_ClampRectangle("source", rect))
    {
        result = WFC_ERROR_NONE;
    }
    return result;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static WFCErrorCode
WFC_Element_ValidateDestinationRectangle(WFC_ELEMENT* element,
                                         WFCfloat* rect)
{
    WFCErrorCode            result = WFC_ERROR_NONE;

#ifndef DEBUG
    (void) element;
#endif    
    
    OWF_ASSERT(element);
    OWF_ASSERT(rect);

    DPRINT(("WFC_Element_ValidateDestinationRectangle(element = %d)",
            element->handle));

    if (rect[2] < 0.0f || rect[3] < 0.0f)
    {
        result = WFC_ERROR_ILLEGAL_ARGUMENT;
    }
    else if (WFC_Element_ClampRectangle("destination", rect))
    {
        /* ... return error or something here? */
        result = WFC_ERROR_NONE;
    }

    /* Hmm.. let's clamp the rectangle even more! To 16k*16k at max;
     * in OWF_Image_Create we calculate the byte size of the image buffer
     * required, and if we have 4bpp, we get overflow for pixelcounts
     * >= 65536*16384. */
    rect[2] = CLAMP(rect[2], 0, 16384);
    rect[3] = CLAMP(rect[3], 0, 16384);

    return result;
}

#define BOOLEAN_TO_ERROR(x) ((x) ? WFC_ERROR_NONE : WFC_ERROR_ILLEGAL_ARGUMENT)

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static WFCErrorCode
WFC_Element_ValidateScalarAttributei(WFC_ELEMENT* element,
                                     WFCElementAttrib attrib,
                                     WFCint value)
{
    WFCErrorCode            result = WFC_ERROR_NONE;

    OWF_ASSERT(element);

    switch (attrib)
    {
        case WFC_ELEMENT_SOURCE:
        {
            WFC_IMAGE_PROVIDER* source;

            source = WFC_Device_FindImageProvider(element->device,
                                                  value,
                                                  WFC_IMAGE_SOURCE);

            result = BOOLEAN_TO_ERROR((WFC_INVALID_HANDLE == value) ||
                                      ((WFC_INVALID_HANDLE != value) &&
                                       (NULL != source)));
            break;
        }

        case WFC_ELEMENT_MASK:
        {
            WFC_IMAGE_PROVIDER* mask;

            mask = WFC_Device_FindImageProvider(element->device,
                                                value,
                                                WFC_IMAGE_MASK);

            result = BOOLEAN_TO_ERROR((WFC_INVALID_HANDLE == value) ||
                                      ((WFC_INVALID_HANDLE != value) &&
                                       (NULL != mask)));
            break;
        }

        case WFC_ELEMENT_SOURCE_ROTATION:
        {
            WFCRotation rotation = (WFCRotation) value;

            result = BOOLEAN_TO_ERROR((WFC_ROTATION_0    == rotation ||
                                       WFC_ROTATION_90   == rotation ||
                                       WFC_ROTATION_180  == rotation ||
                                       WFC_ROTATION_270  == rotation));
            break;
        }

        case WFC_ELEMENT_SOURCE_SCALE_FILTER:
        {
            WFCScaleFilter  filter = (WFCScaleFilter) value;

            result = BOOLEAN_TO_ERROR((WFC_SCALE_FILTER_NONE     == filter ||
                                       WFC_SCALE_FILTER_FASTER   == filter ||
                                       WFC_SCALE_FILTER_BETTER   == filter));
            break;
        }

        case WFC_ELEMENT_SOURCE_FLIP:
        {
            result = WFC_ERROR_NONE;
            break;
        }

        case WFC_ELEMENT_TRANSPARENCY_TYPES:
        {
            WFCint          ii;
            WFCint          count = sizeof(validTransparencyModes) /
                                    sizeof(WFCbitfield);
            WFCbitfield     types = (WFCbitfield) value;

            result = WFC_ERROR_ILLEGAL_ARGUMENT;
            for (ii = 0; ii < count; ii++) {
                if (types == validTransparencyModes[ii])
                {
                    result = WFC_ERROR_NONE;
                    break;
                }
            }
            break;
        }

        default:
        {
            result = WFC_ERROR_BAD_ATTRIBUTE;
            break;
        }
    }
    return result;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static WFCErrorCode
WFC_Element_ValidateScalarAttributef(WFC_ELEMENT* element,
                                     WFCElementAttrib attrib,
                                     WFCfloat value)
{
    WFCErrorCode          result = WFC_ERROR_NONE;

#ifndef DEBUG
    (void) element;
#endif
    
    OWF_ASSERT(element);

    switch (attrib)
    {
        case WFC_ELEMENT_GLOBAL_ALPHA:
        {
            result = BOOLEAN_TO_ERROR(value >= OWF_ALPHA_MIN_VALUE &&
                                      value <= OWF_ALPHA_MAX_VALUE);
            break;
        }

        /* SPECIAL CASES */
        case WFC_ELEMENT_SOURCE_FLIP:
        case WFC_ELEMENT_SOURCE_ROTATION:
        case WFC_ELEMENT_SOURCE_SCALE_FILTER:
        case WFC_ELEMENT_TRANSPARENCY_TYPES:
        case WFC_ELEMENT_SOURCE:
        case WFC_ELEMENT_MASK:
        {
            /* NOTE! special early out here. */
            result = WFC_ERROR_BAD_ATTRIBUTE;
            break;
        }

        default:
        {
            result = WFC_ERROR_ILLEGAL_ARGUMENT;
            break;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static void
WFC_Element_SetElementImageProvider(WFC_ELEMENT* element,
                                    WFC_IMAGE_PROVIDER_TYPE type,
                                    WFCHandle handle)
{
    WFC_IMAGE_PROVIDER*     provider;

    OWF_ASSERT(element);

    provider = WFC_Device_FindImageProvider(element->device, handle, type);

    switch (type)
    {
        case WFC_IMAGE_SOURCE:
        {
            DESTROY(element->cachedSource);
            ADDREF(element->cachedSource, provider);
            element->sourceHandle = handle;
            break;
        }
        case WFC_IMAGE_MASK:
        {
            DESTROY(element->cachedMask);
            ADDREF(element->cachedMask, provider);
            element->maskHandle = handle;
            break;
        }
    }
}

/*---------------------------------------------------------------------------
 *  \brief Set vector integer attribute for an element
 *
 *  \param element          Element
 *  \param attrib           Attribute name
 *  \param count            Attribute size
 *  \param values           Attribute vector value
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Element_SetAttribiv(WFC_ELEMENT* element,
                        WFCElementAttrib attrib,
                        WFCint count,
                        const WFCint* values)
{
    WFCErrorCode            result = WFC_ERROR_NONE;


    FAIL_IF(NULL == values, WFC_ERROR_ILLEGAL_ARGUMENT);

    switch (attrib)
    {
        /* Vector attributes */
        case WFC_ELEMENT_SOURCE_RECTANGLE:
        case WFC_ELEMENT_DESTINATION_RECTANGLE:
        {
            WFCfloat                rect[4];

            rect[0] = values[0];
            rect[1] = values[1];
            rect[2] = values[2];
            rect[3] = values[3];

            result = WFC_Element_SetAttribfv(element, attrib, count, rect);
            break;
        }

        case WFC_ELEMENT_GLOBAL_ALPHA:
        {
            WFCfloat    fvalue = values[0] /
                                 (WFCfloat) OWF_BYTE_MAX_VALUE;

            result = WFC_Element_SetAttribfv(element,
                                             attrib,
                                             1,
                                             &fvalue);
            break;
        }

        /* Scalar attributes */
        default:
        {
            WFCint          value;

            FAIL_IF(1 != count, WFC_ERROR_ILLEGAL_ARGUMENT);

            value = values[0];

            /* Validate the value thus ensuring it is safe to change it */
            result = WFC_Element_ValidateScalarAttributei(element,
                                                          attrib,
                                                          value);
            if (WFC_ERROR_NONE != result)
            {
                break;
            }

            switch (attrib)
            {
                case WFC_ELEMENT_SOURCE:
                {
                    WFC_Element_SetElementImageProvider(element,
                                                        WFC_IMAGE_SOURCE,
                                                        value);
                    break;
                }
                case WFC_ELEMENT_MASK:
                {
                    WFC_Element_SetElementImageProvider(element,
                                                        WFC_IMAGE_MASK,
                                                        value);
                    break;
                }
                case WFC_ELEMENT_SOURCE_FLIP:
                {
                    element->sourceFlip = (WFCboolean)value;
                    break;
                }
                case WFC_ELEMENT_SOURCE_ROTATION:
                {
                    element->sourceRotation = (WFCRotation)value;
                    break;
                }
                case WFC_ELEMENT_SOURCE_SCALE_FILTER:
                {
                    element->sourceScaleFilter = (WFCScaleFilter)value;
                    break;
                }
                case WFC_ELEMENT_TRANSPARENCY_TYPES:
                {
                    element->transparencyTypes = value;
                    break;
                }
                default:
                {
                    result = WFC_ERROR_BAD_ATTRIBUTE;
                    break;
                }
            }
            break;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Element_SetAttribfv(WFC_ELEMENT* element,
                        WFCElementAttrib attrib,
                        WFCint count,
                        const WFCfloat* values)
{
    WFCErrorCode            result = WFC_ERROR_NONE;

    FAIL_IF(NULL == values, WFC_ERROR_ILLEGAL_ARGUMENT);

    switch (attrib)
    {
        /* Vector attributes */
        case WFC_ELEMENT_SOURCE_RECTANGLE:
        case WFC_ELEMENT_DESTINATION_RECTANGLE:
        {
            WFCfloat        clamped[4];

            FAIL_IF(4 != count, WFC_ERROR_ILLEGAL_ARGUMENT);

            memcpy(clamped, values, 4 * sizeof(WFCfloat));

            if (WFC_ELEMENT_SOURCE_RECTANGLE == attrib)
            {
                /* this clamps the rectangle, in case it has values that
                 * cause precision loss or other fuzzy behaviour. */
                result = WFC_Element_ValidateSourceRectangle(element, clamped);

                if (WFC_ERROR_NONE == result)
                {
                    memcpy(element->srcRect, clamped, 4 * sizeof(WFCfloat));

                    DPRINT(("  Source rectangle set to (%.2f,%.2f,%.2f,%.2f)",
                            clamped[0], clamped[1], clamped[2], clamped[3]));
                }
                else
                {
                    DPRINT(("  Source rectangle (%.2f,%.2f,%.2f,%.2f) is " \
                            "invalid",
                            values[0], values[1], values[2], values[3]));
                }
            }
            else
            {
                result = WFC_Element_ValidateDestinationRectangle(element,
                                                                  clamped);
                if (WFC_ERROR_NONE == result)
                {
                    memcpy(element->dstRect, clamped, 4 * sizeof(WFCfloat));

                    DPRINT(("  Destination rectangle set to " \
                            "(%.2f,%.2f,%.2f,%.2f)",
                            clamped[0], clamped[1], clamped[2], clamped[3]));
                }
                else
                {
                    DPRINT(("  Destination rectangle (%.2f,%.2f,%.2f,%.2f) is "
                            "invalid",
                            values[0], values[1], values[2], values[3]));
                }
            }
            break;
        }

        /* scalar attributes */
        case WFC_ELEMENT_GLOBAL_ALPHA:
        {
            /* values[0] must be [0, 1] */
            WFCfloat        value = values[0];

            /* value is [0, 1] map to [0, OWF_ALPHA_MAX] */
            value = value * OWF_ALPHA_MAX_VALUE;

            /* validate the value */
            result = WFC_Element_ValidateScalarAttributef(element,
                                                          attrib,
                                                          value);

            if (WFC_ERROR_NONE != result)
            {
                /* invalid value for attribute, out we go */
                break;
            }

            element->globalAlpha = value;
            break;
        }

        default:
        {
            result = WFC_ERROR_BAD_ATTRIBUTE;
            break;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------
 * Attribute getters
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Element_GetAttribiv(WFC_ELEMENT* element,
                        WFCElementAttrib attrib,
                        WFCint count,
                        WFCint* values)
{
    WFCErrorCode            result = WFC_ERROR_NONE;


    FAIL_IF(NULL == values, WFC_ERROR_ILLEGAL_ARGUMENT);

    switch (attrib)
    {
        /* Vector attributes */
        case WFC_ELEMENT_SOURCE_RECTANGLE:
        case WFC_ELEMENT_DESTINATION_RECTANGLE:
        {
            WFCfloat        rect[4] = {0.0, 0.0, 0.0, 0.0};

            result = WFC_Element_GetAttribfv(element, attrib, count, rect);

            if (WFC_ERROR_NONE == result)
            {
                values[0] = floor(rect[0]);
                values[1] = floor(rect[1]);
                values[2] = floor(rect[2]);
                values[3] = floor(rect[3]);
            }
            break;
        }

        /* Scalar attributes */
        default:
        {
            FAIL_IF(1 != count, WFC_ERROR_ILLEGAL_ARGUMENT);

            switch (attrib)
            {
                /* pure int attributes */
                case WFC_ELEMENT_SOURCE:
                {
                    *values = element->sourceHandle;
                    break;
                }
                case WFC_ELEMENT_MASK:
                {
                    *values = element->maskHandle;
                    break;
                }
                case WFC_ELEMENT_SOURCE_FLIP:
                {
                    *values = element->sourceFlip;
                    break;
                }
                case WFC_ELEMENT_SOURCE_ROTATION:
                {
                    *values = element->sourceRotation;
                    break;
                }
                case WFC_ELEMENT_SOURCE_SCALE_FILTER:
                {
                    *values = element->sourceScaleFilter;
                    break;
                }
                case WFC_ELEMENT_TRANSPARENCY_TYPES:
                {
                    *values = element->transparencyTypes;
                    break;
                }
                case WFC_ELEMENT_GLOBAL_ALPHA:
                {
                    WFCfloat    fvalue;

                    WFC_Element_GetAttribfv(element, attrib, 1, &fvalue);
                    *values = floor(OWF_BYTE_MAX_VALUE * fvalue /
                                    OWF_ALPHA_MAX_VALUE);
                    break;
                }
                default:
                {
                    result = WFC_ERROR_BAD_ATTRIBUTE;
                    break;
                }
            }
            break;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Element_GetAttribfv(WFC_ELEMENT* element,
                        WFCElementAttrib attrib,
                        WFCint count,
                        WFCfloat* values)
{
    WFCErrorCode            result = WFC_ERROR_NONE;

    FAIL_IF(NULL == values, WFC_ERROR_ILLEGAL_ARGUMENT);

    switch (attrib)
    {
        /* Vector attributes */
        case WFC_ELEMENT_SOURCE_RECTANGLE:
        case WFC_ELEMENT_DESTINATION_RECTANGLE:
        {
            FAIL_IF(4 != count, WFC_ERROR_ILLEGAL_ARGUMENT);

            if (WFC_ELEMENT_SOURCE_RECTANGLE == attrib)
            {
                values[0] = element->srcRect[0];
                values[1] = element->srcRect[1];
                values[2] = element->srcRect[2];
                values[3] = element->srcRect[3];
            }
            else
            {
                values[0] = element->dstRect[0];
                values[1] = element->dstRect[1];
                values[2] = element->dstRect[2];
                values[3] = element->dstRect[3];
            }
            break;
        }

        /* Scalar attributes */
        default:
        {
            switch (attrib)
            {
                case WFC_ELEMENT_GLOBAL_ALPHA:
                {
                    *values = element->globalAlpha;
                    break;
                }
                default:
                {
                    result = WFC_ERROR_BAD_ATTRIBUTE;
                    break;
                }
            }
            break;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------
 *  Attribute checkers to use during commit to check element
 *  for inconsistencies.
 *----------------------------------------------------------------------------*/
static WFCboolean
WFC_Element_CheckAttribute(WFC_ELEMENT* element,
                           WFCElementAttrib attrib)
{
#define VALIDATE_SCALAR_I(v)    \
    (WFC_Element_ValidateScalarAttributei(element, attrib, v) == \
            WFC_ERROR_NONE ? WFC_TRUE : WFC_FALSE)
#define VALIDATE_SCALAR_F(v)    \
    (WFC_Element_ValidateScalarAttributef(element, attrib, v) == \
            WFC_ERROR_NONE ? WFC_TRUE : WFC_FALSE)

    WFCboolean              result = WFC_TRUE;

    DPRINT(("WFC_Element_CheckAttribute(%08x,%0x)",
           element, attrib));

    switch (attrib)
    {
        /*
        INTEGER-ATTRIBUTES
        */
        case WFC_ELEMENT_SOURCE:
        {
            /* Validated when the attribute was modified */
            break;
        }

        case WFC_ELEMENT_MASK:
        {
            /* Validated when the attribute was modified */
            break;
        }

        case WFC_ELEMENT_SOURCE_FLIP:
        {
            result = VALIDATE_SCALAR_I(element->sourceFlip) ? WFC_TRUE: WFC_FALSE;
            if (!result)
            {
                DPRINT(("  Element source flipping is invalid (%d)",
                        element->sourceFlip));
            }
            break;
        }

        case WFC_ELEMENT_SOURCE_ROTATION:
        {
            result = VALIDATE_SCALAR_I(element->sourceRotation) ? WFC_TRUE: WFC_FALSE;

            if (!result)
            {
                DPRINT(("  Element source rotation is invalid (%d)",
                        element->sourceRotation));
            }
            break;
        }

        case WFC_ELEMENT_SOURCE_SCALE_FILTER:
        {
            result = VALIDATE_SCALAR_I(element->sourceScaleFilter) ? WFC_TRUE: WFC_FALSE;

            if (!result)
            {
                DPRINT(("  Element source scale filter is invalid (%d)",
                        element->sourceScaleFilter));
            }
            break;
        }

        case WFC_ELEMENT_TRANSPARENCY_TYPES:
        {
            result = VALIDATE_SCALAR_I(element->transparencyTypes) ? WFC_TRUE: WFC_FALSE;

            if (!result)
            {
                DPRINT(("  Element transparency type is invalid (%x)",
                        element->transparencyTypes));
            }
            break;
        }

        case WFC_ELEMENT_GLOBAL_ALPHA:
        {
            result = VALIDATE_SCALAR_F(element->globalAlpha) ? WFC_TRUE: WFC_FALSE;
            if (!result)
            {
                DPRINT(("  Element global alpha is invalid (%d)",
                        element->globalAlpha));
            }
            break;
        }

        case WFC_ELEMENT_DESTINATION_RECTANGLE:
        {
            WFC_IMAGE_PROVIDER* mask;

            /* The <0 test is repeated in SetAttribfv ValidateDestinationRectangle */
           if (element->dstRect[2] < 0 || element->dstRect[3] < 0)
            {
                DPRINT(("  Element destination rectangle has negative "
                        "width/height"));
                result = WFC_FALSE;
                break;
            }
            if (element->maskHandle!=WFC_INVALID_HANDLE)
            {
                result = WFC_Element_CheckAttribute(element, WFC_ELEMENT_MASK);

                if (result)
                {
                    mask = WFC_Device_FindMask(element->device,
                                               element->maskHandle);
                    if (!mask)
                    {
                        DPRINT(("  Mask handle is valid, but mask object now destroyed"));
                        mask=element->cachedMask;
                        if (!mask)
                        {
                            mask=element->mask;
                        }
                    }
                    if (mask)
                    {
                        WFCint  maskWidth, maskHeight;
    
                        DPRINT(("  Element has a mask"));
                        /* if the element has a mask, then width & height must match
                           the dimensions of that mask */
                        owfNativeStreamGetHeader(mask->streamHandle,&maskWidth, &maskHeight,NULL,NULL,NULL);
    
                        if (element->dstRect[2] != maskWidth ||
                            element->dstRect[3] != maskHeight)
                        {
                            DPRINT(("  Mask size (%dx%d) != element size (%d,%d)",
                                   maskWidth, maskHeight,
                                   (int)element->dstRect[2],
                                   (int)element->dstRect[3]));
                            result = WFC_FALSE;
                            break;
                        }
                    }
                    else 
                    {
                        DPRINT(("  No mask pointers could be opened! Scene not safe!"));
                        result = WFC_FALSE;
                    }
                }
            }
            break;
        }

        case WFC_ELEMENT_SOURCE_RECTANGLE:
        {
            WFC_IMAGE_PROVIDER* source;

            result = WFC_Element_CheckAttribute(element, WFC_ELEMENT_SOURCE);
            
            if (result && element->sourceHandle!=WFC_INVALID_HANDLE)
            {   /* no source is valid - the element "will not affect composition results" */
                source = WFC_Device_FindImageProvider(element->device,
                                                      element->sourceHandle,
                                                      WFC_IMAGE_SOURCE);
    
                result = WFC_TRUE;
                if (!source)
                {
                    DPRINT(("  Source handle is valid, but source object now destroyed"));
                    source=element->cachedSource;
                    if (!source)
                    {
                        source=element->source;
                    }
                }
                
                if (source)
                {
                    WFCint  sourceWidth, sourceHeight;
    
                    owfNativeStreamGetHeader(source->streamHandle,&sourceWidth, &sourceHeight,NULL,NULL,NULL);
                    /* The <0 test is repeated in SetAttribfv ValidateSourceRectangle */ 
                    if ((element->srcRect[0] < 0) ||
                        (element->srcRect[1] < 0) ||
                        (element->srcRect[2] < 0) ||
                        (element->srcRect[3] < 0) ||
                        (element->srcRect[0] + element->srcRect[2]) > sourceWidth ||
                        (element->srcRect[1] + element->srcRect[3]) > sourceHeight)
                    {
                        DPRINT(("  Source rectangle out of bounds"));
                        DPRINT(("  (%f,%f,%f,%f), source size %dx%d",
                               element->srcRect[0], element->srcRect[1],
                               element->srcRect[2], element->srcRect[3],
                               sourceWidth, sourceHeight));
                        result = WFC_FALSE;
                        break;
                    }
                }
                else
                {
                    DPRINT(("  No source pointers could be opened! Scene not safe!"));
                    result = WFC_FALSE;
                }
            }
            break;
        }

        case WFC_ELEMENT_FORCE_32BIT:
        {
            /* to keep compiler happy */
            OWF_ASSERT(0);
            break;
        }
    }

    return result;

#undef VALIDATE_SCALAR_F
#undef VALIDATE_SCALAR_I
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCboolean
WFC_Element_HasConflicts(WFC_ELEMENT* element)
{
#define CHECK(x) \
    if (!WFC_Element_CheckAttribute(element, x)) \
    {\
        DPRINT(("Element %d: Conflict in attribute %08x", element->handle, x));\
        return WFC_TRUE; \
    }

    CHECK(WFC_ELEMENT_SOURCE);
    CHECK(WFC_ELEMENT_MASK);
    CHECK(WFC_ELEMENT_SOURCE_RECTANGLE);
    CHECK(WFC_ELEMENT_DESTINATION_RECTANGLE);
    CHECK(WFC_ELEMENT_SOURCE_FLIP);
    CHECK(WFC_ELEMENT_SOURCE_ROTATION);
    CHECK(WFC_ELEMENT_SOURCE_SCALE_FILTER);
    CHECK(WFC_ELEMENT_TRANSPARENCY_TYPES);
    CHECK(WFC_ELEMENT_GLOBAL_ALPHA);

#undef CHECK

    /* all ok, no conflicts */
    return WFC_FALSE;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCboolean
WFC_Element_AffectsCompositionResults(WFC_ELEMENT* element)
{
    if (    (element->transparencyTypes&WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA)
       &&   element->globalAlpha==OWF_FULLY_TRANSPARENT )
    {
        return WFC_FALSE;
    }
    if (element->sourceHandle==WFC_INVALID_HANDLE)
    {
        return WFC_FALSE;
    }
    if (element->dstRect[2]==0.0f || element->dstRect[3]==0.0f)
    {
        return WFC_FALSE;
    }
    if (element->srcRect[2]==0.0f || element->srcRect[3]==0.0f)
    {
        return WFC_FALSE;
    }
    return WFC_TRUE;
    
}
/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Element_Commit(WFC_ELEMENT* element)
{
    OWF_ASSERT(element);

    DPRINT(("WFC_Element_Commit(element = %d)\n", element->handle));

    /* replace source/mask ONLY if it has changed. without these checks,
     * both source and mask would be overwritten whenever one of them
     * is changed.
     */

    if (element->cachedSource != element->source)
    {
        element->source = element->cachedSource;
    }

    if (element->cachedMask != element->mask)
    {
        element->mask = element->cachedMask;
    }

    /* these must be reset now that the element is committed -- the only purpose
     * of these cached ones is to have source/mask object pointers in the
     * element so that source/mask can be safely deleted from the device even
     * if that particular image provider is set as source/mask for some element
     * that is not yet committed.
     */

    DPRINT(("  Prior to destroying cached objects:"));
    DPRINT(("    R(cachedMask) = %d", REFCOUNT(element->cachedMask)));
    DPRINT(("    R(cachedSource) = %d", REFCOUNT(element->cachedSource)));

    element->cachedSource = NULL;
    element->cachedMask = NULL;

    DPRINT(("  new source   = %d\n", element->source ?
                                     element->source->handle : 0));
    DPRINT(("  new mask     = %d\n", element->mask ?
                                     element->mask->handle : 0));
}



#ifdef __cplusplus
}
#endif

