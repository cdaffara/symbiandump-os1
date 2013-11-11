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

#include "owfobject.h"
#include "owftypes.h"
#include "owfmemory.h"


#ifdef __cplusplus
extern "C"
{
#endif



OWF_API_CALL void*
OWF_Object_Construct(size_t size,
                     const char* type,
                     CONSTRUCTOR ctor,
                     DESTRUCTOR dtor)
{
    OWF_OBJECT* obu = (OWF_OBJECT*) xalloc(sizeof(OWF_OBJECT) + size, 1);

    if (obu)
    {
        obu->type = type;
        obu->destructor = dtor;
        obu->referenceCount = 1;
        if (ctor)
        {
            ctor(&obu->payload);
        }
        DPRINT(("CREATE: created %s object (%p)\n", type, &obu->payload));
        return &obu->payload;
    }

    return NULL;
}



#ifdef __cplusplus
}
#endif
