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
#ifndef OWFMEMORY_H_
#define OWFMEMORY_H_

#include <stdlib.h>
#include "owftypes.h"
#include "owfdebug.h"

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef DEBUG

/*
 * In debug builds we use our own memory management functions:
 */
/* multiplication yields negative if overflowed */
#define xalloc(n, c)        (((OWFint)n*c < c) ? NULL : OWF_Memory_Alloc(__FILE__, __LINE__, (n)*(c)))
#define xfree(p)            OWF_Memory_Free(p)

#else

#define xalloc(n, c)        calloc(n, c)
#define xfree(p)            free(p)

#endif

#define NEW0N(x, n)         (x*) xalloc(sizeof(x), n)
#define NEW0(x)             NEW0N(x, 1)

OWF_API_CALL void*
OWF_Memory_Alloc(const char* file, OWFint line, OWFuint32 size);

OWF_API_CALL void
OWF_Memory_Free(void* ptr);

OWF_API_CALL void
OWF_Memory_BlockDump();


#ifdef __cplusplus
}
#endif


#endif
