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
 *  \file wfcscene.c
 *
 *  \brief SI Scene graph management functions.
 */

#include "WF/wfc.h"
#include "wfcstructs.h"
#include "wfcscene.h"
#include "wfcelement.h"
#include "wfcimageprovider.h"

#include "owfmemory.h"
#include "owfarray.h"
#include "owflinkedlist.h"
#include "owfobject.h"

#include "owfdebug.h"

/*!
 *  \brief Destroy an element. In case the element is marked as shared,
 *  this function does nothing. An element is marked as shared, when it is
 *  inserted into working copy scene. The marked flag is reset when the element
 *  is removed from the context (i.e. it only resides in the device's
 *  list of created elements)
 *
 *  \param element          Element to destroy
 */
static void WFC_Scene_DestroyElement(WFC_ELEMENT* element)
{
    /* elements in the working copy are "read only" because
     * they're shared between the device & the working copy
     */
    if (!element->shared)
    {
        WFC_Element_Destroy(element);
    }
}

OWF_API_CALL void
WFC_Scene_Destroy(WFC_SCENE* scene)
{
    OWF_NODE*               node;

    DPRINT(("WFC_Scene_Destroy(%p)", scene));
    if (scene)
    {
        for (node = scene->elements; NULL != node; node = node->next)
        {
            WFC_Scene_DestroyElement((WFC_ELEMENT*)node->data);
        }

        scene->elements = OWF_List_Clear(scene->elements);

        DESTROY(scene->context);

        OWF_Pool_PutObject(scene);
    }
}

/*!
 *  \brief Append element into scene
 *
 *  \param scene            Scene
 *  \param element          Element to add
 */
static void
WFC_Scene_AppendElement(WFC_SCENE* scene,
                        WFC_ELEMENT* element)
{
    OWF_NODE*               node;

    /* OBS! No duplicate check here! Use with caution*/

    node = OWF_Node_Create(CONTEXT(scene->context)->nodePool, element);
    scene->elements = OWF_List_Append(scene->elements, node);
    return ;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL WFC_SCENE*
WFC_Scene_Clone(WFC_SCENE* scene)
{
    WFC_SCENE*              cloneScene;
    OWF_NODE*               node;

    cloneScene = WFC_Scene_Create(CONTEXT(scene->context));

    for (node = scene->elements; NULL != node; node = node->next)
    {
        WFC_ELEMENT* original;
        WFC_ELEMENT* cloneElem;

        original = ELEMENT(node->data);
        if (WFC_Element_AffectsCompositionResults(original))
            {
            cloneElem = WFC_Element_Clone(original);
            WFC_Scene_AppendElement(cloneScene, cloneElem);
            }
    }

    return cloneScene;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL WFC_SCENE*
WFC_Scene_Create(WFC_CONTEXT* context)
{
    WFC_SCENE*              scene;
	
	DPRINT(("WFC_Scene_Create"));
	
	OWF_ASSERT(context);
	
    scene = SCENE(OWF_Pool_GetObject(context->scenePool));

    OWF_ASSERT(scene);

    ADDREF(scene->context, context);

    return scene;
}

 /*----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Scene_InsertElement(WFC_SCENE* scene,
                        WFC_ELEMENT* element,
                        WFCElement elementBelow)
{
    WFCErrorCode            result = WFC_ERROR_NONE;
    OWF_NODE*               iter = NULL;
    OWF_NODE*               newPos = NULL;
    OWF_NODE*               oldPos = NULL;

    if (!scene || !element)
    {
        return WFC_ERROR_ILLEGAL_ARGUMENT;
    }

    if (element->handle == (WFCHandle)element)
    {
        /* nothing to do */
        return WFC_ERROR_NONE;
    }

    newPos = NULL; /*scene->elements;*/
    for (iter = scene->elements; NULL != iter; iter = iter->next)
    {
        WFC_ELEMENT*        temp = NULL;

        temp = ELEMENT(iter->data);

        if (temp->handle == elementBelow)
        {
            /* insertion point found */
            newPos = iter;
        }
        if (temp->handle == element->handle)
        {
            /* already in the scene; relocate */
            oldPos = iter;
        }
    }

    if (newPos && newPos == oldPos) {
        /* inserting element above self is a no-op */
        return WFC_ERROR_NONE;
    }

    if (!newPos && WFC_INVALID_HANDLE != elementBelow)
    {
        /* could not find elementBelow from the scene */
        result = WFC_ERROR_ILLEGAL_ARGUMENT;
    }
    else
    {
        OWF_NODE*           newNode = NULL;

        /* first remove element from its old slot, if already
         * on the list
         */
        if (NULL != oldPos)
        {
            scene->elements = OWF_List_Remove(scene->elements, oldPos);
            OWF_Node_Destroy(oldPos);
        }

        /* allocate new node to insert */
        newNode = OWF_Node_Create(CONTEXT(scene->context)->nodePool, element);
        if (NULL != newPos)
        {
            /* insert into new position, above elementBelow */
            OWF_List_InsertAfter(newPos, newNode);
        }
        else
        {
            /* insert on bottom */
            scene->elements = OWF_List_Insert(scene->elements, newNode);
        }
    }

    DPRINT(("  Order of the elements (bottom to top): "));
    for (iter = scene->elements; NULL != iter; iter = iter->next)
    {
        DPRINT(("    %d", ELEMENT(iter->data)->handle));
    }
    return result;

}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Scene_RemoveElement(WFC_SCENE* scene,
                        WFCElement element)
{
    OWF_NODE*               node = NULL;

    for (node = scene->elements; NULL != node; node = node->next)
    {
        WFC_ELEMENT*        etemp;

        etemp = ELEMENT(node->data);
        if (etemp->handle == element)
        {
            scene->elements = OWF_List_Remove(scene->elements, node);
            OWF_Node_Destroy(node);
            WFC_Scene_DestroyElement(etemp);
            break;

        }
    }
    DPRINT(("  Order of the elements (bottom to top): "));
    for (node = scene->elements; NULL != node; node = node->next)
    {
        DPRINT(("    %d", ELEMENT(node->data)->handle));
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL WFC_ELEMENT*
WFC_Scene_FindElement(WFC_SCENE* scene,
                      WFCElement element)
{
    OWF_NODE*               node = NULL;
    WFC_ELEMENT*            result = NULL;

    for (node = scene->elements; NULL != node; node = node->next)
    {
        WFC_ELEMENT*        etemp;

        etemp = ELEMENT(node->data);
        if (etemp->handle == element)
        {
            result = etemp;
            break;
        }
    }
    return result;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Scene_LockSourcesAndMasks(WFC_SCENE* scene)
{
    OWF_NODE*               node = NULL;

    DPRINT(("WFC_Scene_LockSourcesAndMasks(scene = %p)", scene));

    for (node = scene->elements; NULL != node; node = node->next)
    {
        WFC_ELEMENT*        element;
        
        element = ELEMENT(node->data);

        DPRINT(("  Element source handle = %d", element->sourceHandle));
        DPRINT(("  Element source = %p", element->source));
        DPRINT(("  Element dest size = %.2fx%.2f",
                element->dstRect[2], element->dstRect[3]));
        DPRINT(("  Element src size = %.2fx%.2f",
                element->srcRect[2], element->srcRect[3]));

        if (WFC_Element_AffectsCompositionResults(element))
         {
            DPRINT(("  Locking element %p", element));
            WFC_ImageProvider_LockForReading(element->source);
            /* set the flag so that composition knows to include the
               element into composition */
            element->skipCompose =
                (element->source->lockedStream.image->data == NULL) ?
                        WFC_TRUE : WFC_FALSE;
        }
        else
        {
            element->skipCompose = WFC_TRUE;
        }

        if (!element->skipCompose &&
            WFC_INVALID_HANDLE != element->maskHandle &&
            NULL != element->mask)
        {
            WFC_ImageProvider_LockForReading(element->mask);
            element->maskComposed = WFC_TRUE;

            OWF_ASSERT(element->mask);
            OWF_ASSERT(element->mask->streamHandle);
            OWF_ASSERT(element->mask->lockedStream.image);

             if(element->mask->lockedStream.image->data == NULL)
             {
                    WFC_ImageProvider_Unlock(element->source);
                    element->skipCompose = WFC_TRUE;
             }
        }
        else
        {
            element->maskComposed = WFC_FALSE;
        }
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Scene_UnlockSourcesAndMasks(WFC_SCENE* scene)
{
    OWF_NODE*               node = NULL;

    DPRINT(("WFC_Scene_UnlockSourcesAndMasks(scene = %p)", scene));

    for (node = scene->elements; NULL != node; node = node->next)
    {
        WFC_ELEMENT*        element;

        element = ELEMENT(node->data);

        DPRINT(("  Unlocking element %p", element));
        if (element->source && !element->skipCompose)
        {
            WFC_ImageProvider_Unlock(element->source);
        }

        if (element->mask && !element->skipCompose)
        {
            WFC_ImageProvider_Unlock(element->mask);
        }
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL WFCElement
WFC_Scene_GetNeighbourElement(WFC_SCENE* scene, WFCElement element, WFCint n)
{
    WFCElement              result = WFC_INVALID_HANDLE;
    OWF_NODE*               node = NULL;
    OWF_NODE*               prev = NULL;

    for (node = scene->elements; NULL != node; node = node->next)
    {
        WFC_ELEMENT*        etemp;

        etemp = ELEMENT(node->data);
        if (etemp->handle == element)
        {
            if (n < 0)
            {
                result = prev ? ELEMENT(prev->data)->handle
                              : WFC_INVALID_HANDLE;
            }
            else
            {
                result = node->next ? ELEMENT(node->next->data)->handle
                                    : WFC_INVALID_HANDLE;

            }
            break;
        }
        prev = node;
    }
    return result;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL WFCboolean
WFC_Scene_HasConflicts(WFC_SCENE* scene)
{
    WFCboolean              result = WFC_FALSE;
    OWF_NODE*               node = NULL;

    for (node = scene->elements; NULL != node; node = node->next)
    {
        WFC_ELEMENT*        element;

        element = ELEMENT(node->data);

        result = result || WFC_Element_HasConflicts(element) ? WFC_TRUE : WFC_FALSE;
        if (result)
        {
            /* conflict found */
            break;
        }
    }
    return result;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Scene_Commit(WFC_SCENE* scene)
{
    OWF_NODE*               node;

    for (node = scene->elements; NULL != node; node = node->next)
    {
        WFC_ELEMENT*        element;

        element = ELEMENT(node->data);
        WFC_Element_Commit(element);
    }
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL WFCElement
WFC_Scene_LowestElement(WFC_SCENE* scene)
{
    WFCElement              element = WFC_INVALID_HANDLE;

    if (scene && scene->elements)
    {
        element = ELEMENT(scene->elements->data)->handle;
    }
    return element;
}

/*----------------------------------------------------------------------------*/
OWF_API_CALL WFC_ELEMENT*
WFC_Scene_TopMostElement(WFC_SCENE* scene)
{
    OWF_NODE*               node = NULL;

    for (node = scene->elements; node && node->next; node = node->next)
    {
        /* All work done in the for statement, so no body */
    }
    
    return node ? ELEMENT(node->data) : NULL;
}
