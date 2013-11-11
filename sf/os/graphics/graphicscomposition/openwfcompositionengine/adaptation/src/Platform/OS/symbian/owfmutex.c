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


#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#include "owfmemory.h"
#include "owfmutex.h"



#define MUTEX(x)        (pthread_mutex_t*)(*x)

OWF_API_CALL OWFint
OWF_Mutex_Init(OWF_MUTEX* mutex)
{
    if (!mutex)
    {
        return EINVAL;
    }

    *mutex = xalloc(1, sizeof(pthread_mutex_t));
    if (!*mutex)
    {
        return ENOMEM;
    }
    return pthread_mutex_init(MUTEX(mutex), NULL);
}

OWF_API_CALL OWFint
OWF_Mutex_Destroy(OWF_MUTEX* mutex)
{
    OWFint             err = EINVAL;

    if (!mutex)
    {
        return EINVAL;
    }

    if (*mutex)
    {
        err = pthread_mutex_destroy(MUTEX(mutex));
        xfree(*mutex);
        *mutex = NULL;
    }
    return err;
}

OWF_API_CALL OWFint
OWF_Mutex_Lock(OWF_MUTEX* mutex)
{
    if (!(mutex && *mutex))
    {
        return EINVAL;
    }

    return pthread_mutex_lock(MUTEX(mutex));
}

OWF_API_CALL OWFint
OWF_Mutex_Unlock(OWF_MUTEX* mutex)
{
    if (!(mutex && *mutex))
    {
        return EINVAL;
    }
    return pthread_mutex_unlock(MUTEX(mutex));
}

#ifdef __cplusplus
}
#endif

