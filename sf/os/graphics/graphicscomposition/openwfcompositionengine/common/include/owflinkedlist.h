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

#ifndef OWFLLIST_H_
#define OWFLLIST_H_

#include "owfpool.h"
#include "owftypes.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
 *  Allocates new node from the node pool
 *
 *  \param pool Node pool
 *  \param data Data to store in the node
 *
 *  \return New node containing data or NULL
 */
OWF_API_CALL OWF_NODE*
OWF_Node_Create(OWF_POOL* pool, void* data);

/*!
 *  Returns node to pool it was allocated from.
 *
 *  \param node Node to "destroy"
 */
OWF_API_CALL void
OWF_Node_Destroy(OWF_NODE* node);

/*!
 *  Returns list's tail node.
 *
 *  \param root List root
 *
 *  \return List's tail (last) node
 */
OWF_API_CALL OWF_NODE*
OWF_List_Tail(OWF_NODE* root);

/*!
 *  Append node to list.
 *
 *  \param root List root
 *
 *  \return New list root node
 */
OWF_API_CALL OWF_NODE*
OWF_List_Append(OWF_NODE* root, OWF_NODE* node);

/*!
 *  Insert node to list front. I.e. current root becomes
 *  2nd in the list and so on.
 *
 *  \param root List root
 *  \param node Node to insert
 *
 *  \return New list root (inserted node)
 */
OWF_API_CALL OWF_NODE*
OWF_List_Insert(OWF_NODE* root, OWF_NODE* node);

/*!
 *  Inserts node into list, immediately after node "pred".
 *
 *  \param pred Node after which the newcomer should be placed.
 *  \param node Node to add.
 */
OWF_API_CALL void
OWF_List_InsertAfter(OWF_NODE* pred, OWF_NODE* node);

/*!
 *  Searches the list for data ptr. Returns the node
 *  that contains pointer to data, or NULL if no such node
 *  can be found from the list.
 *
 *  \param root List root
 *  \param data Data pointer
 *
 *  \return Node containing the data ptr or NULL.
 */
OWF_API_CALL OWF_NODE*
OWF_List_Contains(OWF_NODE* root, void* data);

/*!
 *  Remove node from list. Obs! The node isn't freed,
 *  but only removed from the list. It's up to caller
 *  to take care of destroying the node i.e. returning
 *  it to pool or releasing the memory otherwise allocated
 *  to it.
 *
 *  \param root List root
 *  \param node Node to remove from list
 *
 *  \return New list root after removal
 */
OWF_API_CALL OWF_NODE*
OWF_List_Remove(OWF_NODE* root, OWF_NODE* node);

/*!
 *  Remove all nodes from the list. Equals to
 *  while (list) list = OWF_List_Remove(list, list);
 *
 *  \param root List root
 *
 *  \return NULL.
 */
OWF_API_CALL OWF_NODE*
OWF_List_Clear(OWF_NODE* root);

#ifdef __cplusplus
}
#endif


#endif
