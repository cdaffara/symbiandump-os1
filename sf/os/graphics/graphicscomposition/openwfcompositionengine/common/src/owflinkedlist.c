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


#ifdef __cplusplus
extern "C"
{
#endif


#include <string.h>
#include <stdlib.h>

#include "owfmemory.h"
#include "owflinkedlist.h"
#include "owftypes.h"
#include "owfpool.h"




OWF_API_CALL OWF_NODE*
OWF_Node_Create(OWF_POOL* pool, void* data)
{
    OWF_NODE*               node;

    node = (OWF_NODE*)OWF_Pool_GetObject(pool);
    if (node)
    {
        node->data = data;
    }
    return node;
}

OWF_API_CALL void
OWF_Node_Destroy(OWF_NODE* node)
{
    OWF_Pool_PutObject(node);
}

OWF_API_CALL OWF_NODE*
OWF_List_Tail(OWF_NODE* root)
{
    if (root)
    {
        while (root->next)
        {
            root = root->next;
        }
    }
    return root;
}

OWF_API_CALL OWF_NODE*
OWF_List_Append(OWF_NODE* root, OWF_NODE* node)
{
    OWF_NODE*               tail;

    tail = OWF_List_Tail(root);
    if (tail)
    {
        tail->next = node;
    }
    else
    {
        root = node;
    }
    return root;
}

OWF_API_CALL OWF_NODE*
OWF_List_Insert(OWF_NODE* root, OWF_NODE* node)
{
    if (root)
    {
        node->next = root;
    }
    root = node;
    return root;
}

OWF_API_CALL void
OWF_List_InsertAfter(OWF_NODE* pred, OWF_NODE* succ)
{
    if (pred && succ)
    {
        succ->next = pred->next;
        pred->next = succ;
    }
}

OWF_API_CALL OWF_NODE*
OWF_List_Contains(OWF_NODE* root, void* data)
{
    while (root)
    {
        if (root->data == data)
        {
            break;
        }
        root = root->next;
    }

    return root;
}

OWF_API_CALL OWF_NODE*
OWF_List_Remove(OWF_NODE* root, OWF_NODE* node)
{
    OWF_NODE*                iter = NULL;

    if (root)
    {
        if (node != root)
        {
            iter = root;
            while (iter)
            {
                if (iter->next == node)
                {
                    iter->next = node->next;
                    break;
                }
                iter = iter->next;
            }
        }
        else
        {
            root = root->next;
        }
    }

    return root;
}

OWF_API_CALL OWF_NODE*
OWF_List_Clear(OWF_NODE* root)
{
    OWF_NODE* next = NULL;
    while (root)
    {
        next = root->next;
        OWF_Node_Destroy(root);
        root = next;
    }
    return root;
}

#ifdef __cplusplus
}
#endif
