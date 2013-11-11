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
 *  \file wfcpipeline.h
 *
 *  \brief Composition pipeline interface
 */
#ifndef WFCPIPELINE_H_
#define WFCPIPELINE_H_

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

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------ *//*!
 *  \brief Composition pipeline preparation
 *
 *  \param context          Context
 *  \param element          Element
 *
 *  \return Boolean value indicating whether preparation succeeded
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL WFC_ELEMENT_STATE* 
WFC_Pipeline_BeginComposition(WFC_CONTEXT* context, WFC_ELEMENT* element);


/*------------------------------------------------------------------------ *//*!
 *  Composition pipeline cleanup
 *
 *  \param context          Context
 *  \param element          Element
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_EndComposition(WFC_CONTEXT* context, WFC_ELEMENT* element,WFC_ELEMENT_STATE* elementState);

/*------------------------------------------------------------------------ *//*!
 *  \brief Source conversion stage
 *
 *  \param context          Context
 *  \param element          Element
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteSourceConversionStage(WFC_CONTEXT* context,WFC_ELEMENT_STATE* elementState);

/*------------------------------------------------------------------------ *//*!
 *  \brief Crop stage
 *
 *  \param context          Context
 *  \param element          Element
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteCropStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* elementState);

/*------------------------------------------------------------------------ *//*!
 *  \brief Flip stage
 *
 *  \param context          Context
 *  \param element          Element
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteFlipStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* elementState);

/*------------------------------------------------------------------------ *//*!
 *  \brief Rotation stage
 *
 *  \param context          Context
 *  \param element          Element
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteRotationStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* elementState);

/*------------------------------------------------------------------------ *//*!
 *  \brief Scaling stage
 *
 *  \param context          Context
 *  \param element          Element
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteScalingStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* elementState);

/*------------------------------------------------------------------------ *//*!
 *  \brief Blending stage
 *
 *  \param context          Context
 *  \param element          Element
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Pipeline_ExecuteBlendingStage(WFC_CONTEXT* context, WFC_ELEMENT_STATE* elementState);

/*------------------------------------------------------------------------ *//*!
 *  \brief Composition pipeline preparation per context creation
 *
 *  \param context          Context
 *
 *  \return Boolean value indicating whether preparation succeeded
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL OWFboolean WFC_Pipeline_CreateState(WFC_CONTEXT* context);

/*------------------------------------------------------------------------ *//*!
 *  \brief Composition pipeline pull-dowwn per context
 *
 *  \param context          Context
 *
 *  \return Boolean value indicating whether preparation succeeded
 *//*-------------------------------------------------------------------------*/
OWF_API_CALL void WFC_Pipeline_DestroyState(WFC_CONTEXT* context);



#ifdef __cplusplus
}
#endif
#endif /* WFCPIPELINE_H_ */
