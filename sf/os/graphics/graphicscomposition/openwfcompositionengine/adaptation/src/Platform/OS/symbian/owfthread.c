/* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
extern "C" {
#endif

#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

#ifndef _XOPEN_SOURCE
#define LIPLAP
#define _XOPEN_SOURCE 1 /* nanosleep */
#endif

#include <time.h>

#ifdef LIPLAP
#undef _XOPEN_SOURCE
#endif

#include "owfthread.h"
#include "owfmemory.h"
#include "owftypes.h"

OWF_API_CALL void
OWF_Thread_Destroy(OWF_THREAD thread)
{
    if (thread)
    {
        OWF_Thread_Join(thread, NULL);
        xfree(thread);
    }
}

OWF_API_CALL OWF_THREAD
OWF_Thread_Create(void* (*threadfunc)(void*), void* data)
{
    pthread_t*              handle;

    handle = (pthread_t*)xalloc(sizeof(pthread_t), 1);
    if (!handle)
    {
        return NULL;
    }

    if (pthread_create(handle, NULL, threadfunc, data))
    {
        xfree(handle);
        return NULL;
    }

    return (OWF_THREAD) handle;
}

OWF_API_CALL OWFint
OWF_Thread_Join(OWF_THREAD thread, void** retval)
{
    OWFint          ret = EINVAL;

    if (!thread)
    {
        return ret;
    }
    ret = (int) pthread_join(*(pthread_t*) thread, retval);
    return ret;
}

OWF_API_CALL OWFint
OWF_Thread_Cancel(OWF_THREAD thread)
{
	(void)thread;
	return EINVAL;
}


OWF_API_CALL void
OWF_Thread_Exit(void* retval)
{
    pthread_exit(retval);
}

OWF_API_CALL void
OWF_Thread_MicroSleep(OWFuint32 usecs)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = usecs / 1000000;
    ts.tv_nsec = (usecs % 1000000) * 1000;
    nanosleep(&ts, NULL);
#else
    usleep(usecs);
#endif
}

OWF_API_CALL void
OWF_Thread_Sleep(OWFuint32 secs)
{
    sleep(secs);
}


#ifdef __cplusplus
}
#endif
