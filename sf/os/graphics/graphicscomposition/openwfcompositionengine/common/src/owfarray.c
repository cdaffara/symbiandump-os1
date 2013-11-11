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


#ifdef __cplusplus
extern "C"
{
#endif


#include <string.h>
#include <stdlib.h>

#include "owfarray.h"
#include "owftypes.h"
#include "owfdebug.h"



#define MINIMUM_CAPACITY    8

/* switch debug messages off (1) or off (0) */
#if 1
#ifdef DPRINT
#undef DPRINT
#endif
#define DPRINT(x)
#endif

#define SHIFT(a,f,t,n)        memmove(&(a)->items[t], \
                                    &(a)->items[f], \
                                    (n) * sizeof(OWF_ARRAY_ITEM))


void
OWF_Array_Initialize(OWF_ARRAY* array)
{
    OWF_ASSERT(array);

    memset(array, 0, sizeof(OWF_ARRAY));
}

void
OWF_Array_Reset(OWF_ARRAY* array)
{
    OWF_ASSERT(array);

    array->length = 0;
    memset(array->items, 0, sizeof(OWF_ARRAY_ITEM) * array->capacity);

}

void
OWF_Array_Destroy(OWF_ARRAY* array)
{
    OWF_ASSERT(array);

    free(array->items);
    OWF_Array_Initialize(array);
}

static OWFboolean
OWF_Array_Enlarge(OWF_ARRAY* array)
{
    OWF_ARRAY_ITEM*         temp        = NULL;
    OWFint                  newcapacity = 0;

    OWF_ASSERT(array);

    DPRINT(("OWF_Array_Enlarge\n"));
    DPRINT(("  capacity = %d, length = %d\n", array->capacity, array->length));

    /*
    newcapacity = MAX(MINIMUM_CAPACITY,
                      array->capacity + (array->capacity >> 1));
    */
    newcapacity = MAX(MINIMUM_CAPACITY, 2 * array->capacity);

    temp = (OWF_ARRAY_ITEM*) realloc(array->items,
                                     sizeof(OWF_ARRAY_ITEM) * newcapacity);

    if (!temp)
    {
        return OWF_FALSE;
    }

    DPRINT(("  new capacity = %d\n", newcapacity));

    array->items = temp;
    array->capacity = newcapacity;

    return OWF_TRUE;
}

OWFboolean
OWF_Array_AppendItem(OWF_ARRAY* array,
                      OWF_ARRAY_ITEM item)
{
    OWF_ASSERT(array);

    DPRINT(("OWF_Array_AppendItem\n"));

    if (array->length >= array->capacity)
    {
        if (!OWF_Array_Enlarge(array))
        {
            return OWF_FALSE;
        }
    }

    DPRINT(("  item[%d] is now %p\n", array->length, item));

    array->items[array->length] = item;
    ++array->length;

    return OWF_TRUE;
}

OWFboolean
OWF_Array_InsertItem(OWF_ARRAY*    array,
                     OWFint position,
                     OWF_ARRAY_ITEM item)
{
    OWF_ASSERT(array);

    DPRINT(("bounds check\n"));

    /* check bounds */
    if (position < 0 || position > array->length)
    {
        return OWF_FALSE;
    }

    DPRINT(("enlarge\n"));

    if (array->length >= array->capacity)
    {
        if (!OWF_Array_Enlarge(array))
        {
            return OWF_FALSE;
        }
    }

    DPRINT(("new capacity = %d\n", array->capacity));

    /* shift forward (obs! memmove because src & dst overlap) */
    SHIFT(array, position, position + 1, array->length - position);

    DPRINT(("  item[%d] is now %p\n", array->length, item));

    /* put */
    array->items[position] = item;
    ++array->length;

    return OWF_TRUE;
}

OWF_ARRAY_ITEM
OWF_Array_RemoveItem(OWF_ARRAY* array,
                     OWF_ARRAY_ITEM item)
{
    OWFint                    ii = 0;
    OWF_ARRAY_ITEM            result = NULL;

    OWF_ASSERT(array);

    for (ii = 0; ii < array->length; ii++)
    {
        if (array->items[ii] == item)
        {
            result = OWF_Array_RemoveItemAt(array, ii);
            break;
        }
    }
    return result;
}

OWF_ARRAY_ITEM
OWF_Array_RemoveItemAt(OWF_ARRAY* array,
                          OWFint position)
{
    OWF_ARRAY_ITEM            result;

    OWF_ASSERT(array);

    /* check bounds */
    if (position < 0 || position >= array->length)
    {
        return NULL;
    }

    --array->length;
    result = array->items[position];
    SHIFT(array, position + 1, position, array->length - position);

    return result;
}

OWF_ARRAY_ITEM
OWF_Array_GetItemAt(OWF_ARRAY* array,
                    OWFint position)
{
    OWF_ASSERT(array);

    if (position < 0 || position >= array->length)
    {
        return NULL;
    }

    return array->items[position];
}

#ifdef __cplusplus
}
#endif

