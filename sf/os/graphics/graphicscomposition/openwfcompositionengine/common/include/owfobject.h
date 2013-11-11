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

#ifndef OWFOBJECT_H_
#define OWFOBJECT_H_

#include <stdlib.h>

#include "owftypes.h"
#include "owfdebug.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef void (*DESTRUCTOR)(void*);
typedef void (*CONSTRUCTOR)(void*);

typedef struct {
    const char*             type;
    DESTRUCTOR              destructor;
    OWFuint32               referenceCount;
    void*                   payload;
} OWF_OBJECT;

#define TYPE(x)             "" # x "_type"

/*! returns field offset within structure */
#define FOFF(x,y)           ((OWFuint32) &(((x*) 0x1000)->y) - 0x1000)

/*! returns pointer to wrapper object which holds object x */
#define _O(x)               ((OWF_OBJECT*) ((OWFuint32)(x) -\
                                           FOFF(OWF_OBJECT,payload)))

/*! create new object instance */
#define CREATE(x)           (x*) OWF_Object_Construct(sizeof(x), TYPE(x), \
                                                      & x##_Ctor, & x##_Dtor)

#define REFCOUNT(x)         ((x) ? _O(x)->referenceCount : 0xFFFFFFFF)

/*! get the type of the contained object  */
#define TYPEOF(x)           (_O(x)->type)

/*! increment object reference count  */
#define ADDREF(x,y) \
    if (y) {\
         _O(y)->referenceCount++; \
         DPRINT(("ADDREF: ref count of %s(%p) is now %d\n", TYPEOF(y), y, \
                 REFCOUNT(y)));\
    }\
    x = y;

/*! decrement object reference count, call destructor if count reaches zero  */
#define REMREF(x) \
    if (x) {\
        --_O(x)->referenceCount; \
        DPRINT(("REMREF: ref count of %s(%p) is now %d\n", TYPEOF(x), x, \
                REFCOUNT(x))); \
        if (0 == _O(x)->referenceCount) {\
            if (_O(x)->destructor) {\
                _O(x)->destructor(x);\
            }\
            xfree(_O(x));\
        }\
        x = NULL;\
    }

#define DESTROY(x) \
    REMREF(x); \
    x = NULL;

OWF_API_CALL void*
OWF_Object_Construct(size_t size,
                     const char* type,
                     CONSTRUCTOR ctor,
                     DESTRUCTOR dtor);


#ifdef __cplusplus
}
#endif


#endif /* OWFOBJECT_H_ */
