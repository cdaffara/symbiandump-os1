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

/*!
 *  \file owfpool.h
 */
#ifndef OWFPOOL_H_
#define OWFPOOL_H_

#include <stdlib.h>
#include "owftypes.h"


#ifdef __cplusplus
extern "C"
{
#endif


#define EOC         (OWFuint32) 0xFFFFFFFF


typedef struct
{
    /*! linked list of free entries */
    OWFuint32* entries;
    /*! solid chunk of memory containing all the objects;
     * no book-keeping data here, plain sequentially
     * stored objects only */
    char* chunk;
    /*! index number of the first free object in the pool;
     * EOC if the pool is empty */
    OWFuint32 firstFree;
    /*! pools capacity */
    size_t capacity;
    /*! number of free (unallocated) objects in the pool */
    size_t free;
    /*! size of an object */
    size_t entrySize;
} OWF_POOL;

/*!
 *  Creates new object pool. Initially all pool's
 *  objects are unallocated.
 *
 *  \param objectSize Size of an individual pool object
 *  \param objectCount Pool capacity (number of objects)
 *
 *  \return New pool or NULL
 */
OWF_API_CALL OWF_POOL*
OWF_Pool_Create(size_t objectSize, size_t objectCount);

/*!
 *  Allocate object from pool. The pool retains ownership of
 *  the object returned by the function.
 *
 *  \param pool Pool from which the object should be allocated
 *
 *  \return Object or NULL
 */
OWF_API_CALL void*
OWF_Pool_GetObject(OWF_POOL* pool);

/*!
 *  Returns previously allocated object back to the pool.
 *
 *  \param object Object (must be valid pool object)
 */
OWF_API_CALL void
OWF_Pool_PutObject(void* object);

/*!
 *  Destroys a pool. Frees all resources allocated by
 *  the pool and invalidates all objects in the pool
 *  (both allocated and unallocated)
 *
 *  \param pool Pool to destroy
 */
OWF_API_CALL void
OWF_Pool_Destroy(OWF_POOL* pool);


#ifdef __cplusplus
}
#endif


#endif /* OWFPOOL_H_ */
