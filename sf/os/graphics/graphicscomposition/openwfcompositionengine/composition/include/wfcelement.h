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
 *  \file wfcelement.h
 *
 *  \brief Element handling interface
 */
#ifndef WFCELEMENT_H_
#define WFCELEMENT_H_

#include "WF/wfc.h"
#include "wfcstructs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 *  \brief Create new element into context
 *
 *  \param context Context into which to create the element
 *
 *  \return New element object or NULL
 */
OWF_API_CALL WFC_ELEMENT*
WFC_Element_Create(WFC_CONTEXT* context);

OWF_API_CALL WFC_ELEMENT*
WFC_Element_Clone(WFC_ELEMENT* element);

OWF_API_CALL void
WFC_Element_Destroy(WFC_ELEMENT* element);

OWF_API_CALL WFCErrorCode
WFC_Element_SetAttribiv(WFC_ELEMENT* element,
                        WFCElementAttrib attrib,
                        WFCint count,
                        const WFCint* values);

OWF_API_CALL WFCErrorCode
WFC_Element_SetAttribfv(WFC_ELEMENT* element,
                        WFCElementAttrib attrib,
                        WFCint count,
                        const WFCfloat* values);

OWF_API_CALL WFCErrorCode
WFC_Element_GetAttribiv(WFC_ELEMENT* element,
                        WFCElementAttrib attrib,
                        WFCint count,
                        WFCint* values);

OWF_API_CALL WFCErrorCode
WFC_Element_GetAttribfv(WFC_ELEMENT* element,
                        WFCElementAttrib attrib,
                        WFCint count,
                        WFCfloat* values);

OWF_API_CALL WFCboolean
WFC_Element_HasConflicts(WFC_ELEMENT* element);

/** Returns False if attribute values "will not affect composition results" in API doc **/ 
OWF_API_CALL WFCboolean
WFC_Element_AffectsCompositionResults(WFC_ELEMENT* element);

OWF_API_CALL void
WFC_Element_Commit(WFC_ELEMENT* element);

#ifdef __cplusplus
}
#endif

#endif /*WFCELEMENT_H_*/
