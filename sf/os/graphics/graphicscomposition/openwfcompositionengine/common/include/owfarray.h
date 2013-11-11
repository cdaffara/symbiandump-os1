/* Copyright (c) 2009 The Khronos Group Inc.
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
#ifndef OWFARRAY_H_
#define OWFARRAY_H_

#include "owftypes.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef void*                 OWF_ARRAY_ITEM;

typedef struct {
    OWF_ARRAY_ITEM* items;
    OWFint          capacity;
    OWFint          length;
} OWF_ARRAY;

/*!
 *  Initialize array object
 *
 *  \param array Array object
 */
OWF_API_CALL void
OWF_Array_Initialize(OWF_ARRAY* array);

/*!
 *  Reset array. Frees resources allocated by the array.
 *  Doesn't destroy the actual contents (managed
 *  by the array user). The array can be safely reused afterwards.
 *
 *  \param array Array object
 */
OWF_API_CALL void
OWF_Array_Reset(OWF_ARRAY* array);

/*!
 *  Destroy array. Free all resources allocated
 *  by the array.
 *
 *  \param array Array object
 */
OWF_API_CALL void
OWF_Array_Destroy(OWF_ARRAY* array);

/*!
 *  Append item to array
 *
 *  \param array Array object
 *  \param item Item to add
 *
 *  \return OWF_TRUE if the operation succeeded, OWF_FALSE otherwise
 */
OWF_API_CALL OWFboolean
OWF_Array_AppendItem(OWF_ARRAY* array,
                      OWF_ARRAY_ITEM item);

/*!
 *  Insert item into the array
 *
 *  \param array Array object
 *  \param position Where the item should be inserted at
 *  \param item Item to insert
 *
 *  \return OWF_TRUE if the operation succeeded, OWF_FALSE otherwise
 */
OWF_API_CALL OWFboolean
OWF_Array_InsertItem(OWF_ARRAY*    array,
                     OWFint position,
                     OWF_ARRAY_ITEM item);

/*!
 *  Remove item (by value) from the array
 *
 *  \param array Array object
 *  \param item Item to remove
 *
 *  \return Removed item or NULL if the item is invalid.
 *
 */
OWF_API_CALL OWF_ARRAY_ITEM
OWF_Array_RemoveItem(OWF_ARRAY* array,
                      OWF_ARRAY_ITEM item);

/*!
 *  Remove item (by index) from the array
 *
 *  \param array Array object
 *  \param position Index of the item to remove
 *
 *  \return Removed item or NULL if the index is out of bounds.
 */
OWF_API_CALL OWF_ARRAY_ITEM
OWF_Array_RemoveItemAt(OWF_ARRAY* array,
                          OWFint position);

/*!
 *  Get item from array
 *
 *  \param array Array object
 *  \param position Index of the item to fetch (0..array.length-1)
 *
 *  \return Item or NULL, if the position argument is out of bounds.
 *
 */
OWF_API_CALL OWF_ARRAY_ITEM
OWF_Array_GetItemAt(OWF_ARRAY* array,
                    OWFint position);


#ifdef __cplusplus
}
#endif

#endif
