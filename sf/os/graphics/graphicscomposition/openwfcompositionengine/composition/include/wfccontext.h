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
 *  \file wfccontext.h
 *
 *  \brief Context handling interface
 */
#ifndef WFCCONTEXT_H_
#define WFCCONTEXT_H_

#include "WF/wfc.h"
#include "wfcstructs.h"
#include "wfcdevice.h"
#include "owfscreen.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_SOURCE_WIDTH        OWF_SURFACE_WIDTH
#define MAX_SOURCE_HEIGHT       OWF_SURFACE_HEIGHT
/*!
Initialize context attributes
\return ATTR_ERROR_NONE if attributes have been properly initialised.
*/
OWF_API_CALL OWF_ATTRIBUTE_LIST_STATUS
WFC_Context_InitializeAttributes(WFC_CONTEXT* context,
                                 WFCContextType type);

/*!
 *  \brief Create new context on device
 *
 *  \param device Device on which the context should be created
 *  \param type Context type (on- or off-screen)
 *
 *  \return New context object or NULL in case of failure
*/
OWF_API_CALL WFC_CONTEXT*
WFC_Context_Create(WFC_DEVICE* device,
                   WFCNativeStreamType stream,
                   WFCContextType type,
                   WFCint screenNum);
/*!
 * \brief Releases the resources associated with the context.  
 * Some of these resources hold reference-counts on the context,
 * so this must be called BEFORE the main attempt to destroy the context.
 * 
 *  \param type Context type (on- or off-screen)
 */
OWF_API_CALL void
WFC_Context_Shutdown(WFC_CONTEXT* context);

/*!
 *  \brief Setup context rendering target
 *
 *  \param context Context
 *  \param stream Target stream to use for rendering
 */
OWF_API_CALL void
WFC_Context_SetTargetStream(WFC_CONTEXT* context,
                            OWFNativeStreamType stream);

/*!
 * \brief Checks if the given stream would be valid as an off-screen context target.
 * Checks that the format can be rendered TO.
 * Also checks that the image size is acceptable (within the scratch buffers).
 * This is called before the context is created so is effectively a "static" context method.
 * 
 *  \param stream Target stream to use for rendering
 */
OWF_API_CALL WFCboolean
WFC_Context_IsValidTargetStream(OWFNativeStreamType stream);
/*!
 * \brief Checks that the image size is acceptable (within the scratch buffers).
 * This is called before the source is created.
 * 
 *  \param stream source stream to use for rendering.
 */
OWF_API_CALL WFCboolean
WFC_Context_IsValidSourceStream(OWFNativeStreamType stream);
/*!
 * \brief Destroy all masks in context
 *
 * \param context Context
 */
OWF_API_CALL void
WFC_Context_DestroyMasks(WFC_CONTEXT* context);

/*!
 * \brief Destroy all context sources
 */
OWF_API_CALL void
WFC_Context_DestroySources(WFC_CONTEXT* context);

/*!
 *  \brief Destroy all context elements
 *
 *  \param context Context object
 */
OWF_API_CALL void
WFC_Context_DestroyElements(WFC_CONTEXT* context);

/*!
 *  \brief Find element from current scene
 *
 *  \param context Context object
 *  \param element Element to find
 *
 *  \return Element object or NULL if element hasn't been inserted
 *  into current scene.
 */
OWF_API_CALL WFC_ELEMENT*
WFC_Context_FindElement(WFC_CONTEXT* context,
                        WFCElement element);

/*!
 *  \brief Activate/deactivate auto-composition on context
 *
 *  \param context Context
 *  \param act Auto-composition enable/disable
 */
OWF_API_CALL void
WFC_Context_Activate(WFC_CONTEXT* context,
                     WFCboolean act);

/*!
 *  \brief Insert composition request to command stream
 *
 *  \param device
 *  \param context
 *  \param wait WFD_TRUE if executed synchronously
 */
OWF_API_CALL WFCboolean
WFC_Context_InvokeComposition(WFC_DEVICE* device,
                              WFC_CONTEXT* context,
                              WFCboolean wait);

/*!
 *
 *  \brief Insert commit request to command stream
 *
 *  \param device
 *  \param context
 *  \param wait WFD_TRUE if executed synchronously
 */
OWF_API_CALL WFCErrorCode
WFC_Context_InvokeCommit(WFC_DEVICE* device,
                         WFC_CONTEXT* context,
                         WFCboolean wait);

/*!
 *  \brief Insert fence token to context's command stream
 *
 *  \param dpy
 *  \param context
 *  \param sync
 */
OWF_API_CALL void
WFC_Context_InsertFence(WFC_CONTEXT* context,
                        WFCEGLDisplay dpy,
                        WFCEGLSync sync);

/*!
 *  \brief Create element
 *
 *  \param context
 */
OWF_API_CALL WFC_ELEMENT*
WFC_Context_CreateElement(WFC_CONTEXT* context);

/*! \brief Destroy element
 *  \param context
 *  \param element
 */
OWF_API_CALL WFCErrorCode
WFC_Context_DestroyElement(WFC_CONTEXT* context, WFCElement element);

/*!
 *  \brief IncreaseClientElementCount
 *
 *  \param context
 *  \return positive increased current element count; negative or zero indicating the count can't be increased
 */
OWF_API_CALL WFCint
WFC_Context_IncreaseClientElementCount(WFC_CONTEXT* context);

/*!
 *  \brief DecreaseClientElementCount
 *
 *  \param context
 *  \return positive or zero decreased current element count; negative indicating the count can't be decreased - p0robably already zero
 */
OWF_API_CALL WFCint
WFC_Context_DecreaseClientElementCount(WFC_CONTEXT* context);

/*!
 *  \brief Insert element into context's scene
 *
 *  \param context
 *  \param element
 *  \param subordinate
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL WFCErrorCode
WFC_Context_InsertElement(WFC_CONTEXT* context,
                          WFCElement element,
                          WFCElement subordinate);

/*!
 *  \brief Remove element from context's scene
 *
 *  \param context
 *  \param element
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL WFCErrorCode
WFC_Context_RemoveElement(WFC_CONTEXT* context,
                          WFCElement element);

/*!
 *  \brief Fetch the handle of the element that is above the given element.
 *
 *  \param context
 *  \param element
 *
 *  \return Error code. On return, the location pointed by result
 *  will contain handle of the element above OR WFC_INVALID_HANDLE,
 *  if no element is above given element.
 */
OWF_API_CALL WFCErrorCode
WFC_Context_GetElementAbove(WFC_CONTEXT* context,
                            WFCElement element,
                            WFCElement* result);

/*!
 *  \brief Fetch the handle of the element that is below the given element.
 *
 *  \param context
 *  \param element
 *
 *  \return Error code. On return, the location pointed by result
 *  will contain handle of the element below OR WFC_INVALID_HANDLE,
 *  if no element is below given element.
 */
OWF_API_CALL WFCErrorCode
WFC_Context_GetElementBelow(WFC_CONTEXT* context,
                            WFCElement element,
                            WFCElement* result);

/*!
 *  \brief Get context attribute value
 *
 *  \param context
 *  \param attrib
 *  \param value
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL WFCErrorCode
WFC_Context_GetAttribi(WFC_CONTEXT* context,
                       WFCContextAttrib attrib,
                       WFCint* value);

/*!
 *  \brief Set context attribute value
 *
 *  \param context
 *  \param attrib
 *  \param value
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL WFCErrorCode
WFC_Context_SetAttribi(WFC_CONTEXT* context,
                       WFCContextAttrib attrib,
                       WFCint value);

/*!
 *  \brief Get context attribute value
 *
 *  \param context
 *  \param attrib
 *  \param value
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL WFCErrorCode
WFC_Context_GetAttribfv(WFC_CONTEXT* context,
                        WFCContextAttrib attrib,
                        WFCint count,
                        WFCfloat* values);

/*!
 *  \brief Set context attribute value
 *
 *  \param context
 *  \param attrib
 *  \param value
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL WFCErrorCode
WFC_Context_SetAttribfv(WFC_CONTEXT* context,
                        WFCContextAttrib attrib,
                        WFCint count,
                        const WFCfloat* values);


/*!
 *  \brief Check if context is currently activated
 *
 *  \param context
 */

OWF_API_CALL WFCboolean
WFC_Context_Active(WFC_CONTEXT* context);

#ifdef __cplusplus
}
#endif

#endif /*WFCCONTEXT_H_*/
