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

 /*
 * owfpool.c
 */


#ifdef __cplusplus
extern "C"
{
#endif


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "owfdebug.h"
#include "owftypes.h"
#include "owfpool.h"
#include "owfmemory.h"




OWF_API_CALL OWF_POOL*
OWF_Pool_Create(size_t objectSize, size_t objectCount)
{
    OWF_POOL*               pool;
    char*                   chunk;
    OWFuint32*               entries;

    /* we be needing some memories, aye */
    pool    = (OWF_POOL*)xalloc(sizeof(OWF_POOL), 1);
    entries = (OWFuint32*)xalloc(sizeof(OWFuint32), objectCount);
    chunk   = (char*)xalloc((objectSize + sizeof(OWFuint32*)), objectCount);

    if (pool && entries && chunk)
    {
        size_t              i;

        /* setup pool */
        pool->entries   = entries;
        pool->chunk     = chunk;
        pool->capacity  =
        pool->free      = objectCount;
        pool->firstFree = 0;
        pool->entrySize = objectSize;

        /* initially all objects are marked as free */
        for (i = 0; i < objectCount-1; i++) {
            entries[i] = i+1;
        }
        /* terminate chain */
        entries[objectCount-1] = EOC;
    }
    else
    {
        /* failed miserably. bail out. */
        xfree(pool);
        pool = NULL;
        xfree(entries);
        xfree(chunk);
    }

    return pool;
}

OWF_API_CALL void*
OWF_Pool_GetObject(OWF_POOL* pool)
{
    void*                   object = NULL;

    if (pool)
    {
        /* we have objects left in the pool? */
        if (pool->firstFree != EOC)
        {
            OWFuint32*       temp;
            OWFuint32        chunk;

            chunk = (OWFuint32) pool->chunk;

            /* calculate offset inside the chunk */
            temp = (OWFuint32*) (chunk + (pool->entrySize +
                    sizeof(OWFuint32*)) * pool->firstFree);
            /* remember way back to home */
            temp[0] = (OWFuint32) pool;
            object = (void*) &temp[1];
            pool->firstFree = pool->entries[pool->firstFree];
            --pool->free;

            OWF_ASSERT((OWFuint32) object > (OWFuint32) pool->chunk);
            OWF_ASSERT((int)object != 0x5eaf00d);
        }
    }
    return object;
}

OWF_API_CALL void
OWF_Pool_PutObject(void* object)
{
    OWFuint32* temp = (OWFuint32*) object;

    if (temp && temp[-1])
    {
        OWF_POOL*           pool;
        OWFuint32            addr;
        OWFuint32            chunk;
        OWFuint32            entrySize;

        addr    = (OWFuint32) object - 4;
        pool    = (OWF_POOL*) temp[-1];
        chunk   = (OWFuint32) pool->chunk;
        entrySize = pool->entrySize + sizeof(OWFuint32*);

        if (addr >= chunk &&
            addr < (chunk + pool->capacity * entrySize))
        {
            OWFuint32 index = (addr - chunk) / entrySize;

            pool->entries[index] = pool->firstFree;
            pool->firstFree = index;
            memset(object, 0x0, pool->entrySize);
            ++pool->free;
        }
        else
        {
            /* invalid object */
            OWF_ASSERT(0);
        }
    }
}

OWF_API_CALL void
OWF_Pool_Destroy(OWF_POOL* pool)
{
    if (pool)
    {
        xfree(pool->entries);
        xfree(pool->chunk);
        memset(pool, 0, sizeof(OWF_POOL));
        xfree(pool);
    }
}


#ifdef __cplusplus
}
#endif
