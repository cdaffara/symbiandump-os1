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
 *  \file wfcscene.h
 *
 *  \brief Scene handling interface
 */

#ifndef WFCSCENE_H_
#define WFCSCENE_H_

#include "WF/wfc.h"
#include "wfcelement.h"

#include "owfpool.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  \brief Clone a scene
 *
 *  \param scene Pointer to scene to clone
 *  \returns cloned scene
 */
OWF_API_CALL WFC_SCENE*
WFC_Scene_Clone(WFC_SCENE* scene);

/*!
 *  \brief Create new scene in a context
 *
 *  \param context          Context to create the scene in
 *
 *  \return New scene object
 */
OWF_API_CALL WFC_SCENE*
WFC_Scene_Create(WFC_CONTEXT* context);

/*!
 *  \brief Destroy a scene.
 *
 *  Free all resources used by the scene and return the
 *  scene object into the scene pool
 *
 *  \param scene            Scene
 */

OWF_API_CALL void
WFC_Scene_Destroy(WFC_SCENE* scene);

/*!
 *  \brief Insert element to scene above given element
 *
 *  \param scene            Scene
 *  \param element          Element to add
 *  \param elementBelow     Element above which the new element should be
 *                          inserted.
 *  \return WFCErrorCode
 */

OWF_API_CALL WFCErrorCode
WFC_Scene_InsertElement(WFC_SCENE* scene,
                        WFC_ELEMENT* element,
                        WFCElement elementBelow);

/*!
 *  \brief Remove element from scene
 *
 *  \param scene            Scene
 *  \param element          Element to remove
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL void
WFC_Scene_RemoveElement(WFC_SCENE* scene,
                        WFCElement element);


/*!
 *  \brief Get element's neighbour element (i.e. element on top of it
 *  or underneath it)
 *
 *  \param scene Scene
 *  \param element Element
 *  \param n Neighbour element offset (< 0 for elements underneath,
 *  > 0 for elements on top)
 *
 *  \return Neigbour element id or WFC_INVALID_HANDLE
 */
OWF_API_CALL WFCElement
WFC_Scene_GetNeighbourElement(WFC_SCENE* scene, WFCElement element, WFCint n);

/*!
 *  \brief Search the scene for given element.
 *
 *  \param scene            Scene to search
 *  \param element          Handle of the element to find
 *
 */
OWF_API_CALL WFC_ELEMENT*
WFC_Scene_FindElement(WFC_SCENE* scene,
                      WFCElement element);
/*!
 *  \brief Unlock scene sources & masks.
 *
 *  \param scene            Scene
 */
OWF_API_CALL void
WFC_Scene_UnlockSourcesAndMasks(WFC_SCENE* scene);

/*!
 *  \brief Lock all sources' & masks' streams for reading prior to rendering.

 *  Locking is actually just acquiring the read buffer for each source/mask
 *  stream.
 *
 *  All source read buffers must be acquired before the composition starts
 *  because if multiple elements share same source, they all should get same
 *  frame from that source.
 *
 *  If each source buffer were locked/unlocked multiple times during
 *  composition (i.e. once for each element that uses it), the buffer
 *  contents might change between elements thus yielding wrong visual results.
 *
 *  \param scene            Scene
 */
OWF_API_CALL void
WFC_Scene_LockSourcesAndMasks(WFC_SCENE* scene);

/*!
 *  \brief Check scene for conflicts
 *
 *  \param scene            Scene to check
 *
 *  \return WFC_TRUE if there's a conflict (inconsistency) in the
 *  scene graph, WFC_FALSE otherwise.
 */
OWF_API_CALL WFCboolean
WFC_Scene_HasConflicts(WFC_SCENE* scene);

/*!
 *  \brief Commit changes to scene
 *
 *  \param scene            Scene
 */
OWF_API_CALL void
WFC_Scene_Commit(WFC_SCENE* scene);

/*!
 *  Return handle of the lowest element in the scene.
 *
 *  \param scene Scene
 *  \return Handle of the bottom element
 */
OWF_API_CALL WFCElement
WFC_Scene_LowestElement(WFC_SCENE* scene);

/*!
 *  \brief Search for the topmost element
 *
 *  \param scene            Scene to search
 *  \return pointer to the topmost element
 */
OWF_API_CALL WFC_ELEMENT*
WFC_Scene_TopMostElement(WFC_SCENE* scene);

#ifdef __cplusplus
}
#endif
#endif /* WFCSCENE_H_ */
