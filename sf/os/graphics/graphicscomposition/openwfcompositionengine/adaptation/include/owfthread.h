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

#ifndef OWFTHREAD_H
#define OWFTHREAD_H

#include "owftypes.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void*               OWF_THREAD;

OWF_API_CALL void
OWF_Thread_Destroy(OWF_THREAD thread);

OWF_API_CALL OWF_THREAD
OWF_Thread_Create(void* (*threadfunc)(void*), void* data);

OWF_API_CALL OWFint
OWF_Thread_Join(OWF_THREAD thread, void** retval);

OWF_API_CALL OWFint
OWF_Thread_Cancel(OWF_THREAD thread);

OWF_API_CALL void
OWF_Thread_Exit(void* retval);

OWF_API_CALL void
OWF_Thread_MicroSleep(OWFuint32 usecs);

OWF_API_CALL void
OWF_Thread_Sleep(OWFuint32 secs);


#ifdef __cplusplus
}
#endif


#endif
